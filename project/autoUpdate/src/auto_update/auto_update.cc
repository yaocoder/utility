#include "auto_update.h"
#include "file_download.h"
#include <LiteUnzip/LiteUnzip.h>
#include <tchar.h>

#include "../common/init_configure.h"
#include "../common/utils.h"
#include "../common/defines.h"
#include "../common/EncodingConverter.h"
#include "../common/mini_dump.h"
#include "../common/md5_expand.h"

CAutoUpdate::CAutoUpdate( void )
{
	running_	= false;
	pInitConfig_= new CInitConfig;
}

CAutoUpdate::~CAutoUpdate( void )
{
	utils::SafeDelete(pInitConfig_);
}

void CAutoUpdate::Init(const UPDATE_FILE_INFO& updateFileInfo, const std::string& download_path, const std::string& replace_path)
{
	MiniDumper::Instance().Install();
	pInitConfig_->InitLog4cxx("autoUpdate");
	updateFileInfo_ = updateFileInfo;
	download_path_	= download_path;
	replace_path_	= replace_path;
}

bool CAutoUpdate::Run()
{
	if (!running_)
	{
		DWORD threadId;
		HANDLE hThrd =  ::CreateThread(NULL,  0 , RunThread , (LPVOID)this,  0 ,  &threadId);
		if (INVALID_HANDLE_VALUE == hThrd)
		{
			LOG4CXX_ERROR(g_logger, "CAutoUpdate::Run:RunThread error : " << GetLastError());
			return false;
		}
		::CloseHandle(hThrd);
		running_ = true;
	}
	
	return true;
}

DWORD WINAPI CAutoUpdate::RunThread( LPVOID param )
{
	CAutoUpdate* pThis = static_cast<CAutoUpdate*>(param);

	/* 1.下载文件 */
	pThis->pFileDownLoad_ = new CFileDownLoad;
	std::string local_full_path;
	pThis->pAutoUpdateEvent_->RunEvent(DOWNLOAD_FILE_START);
	int ret = pThis->pFileDownLoad_->StartDownLoadFile(pThis->updateFileInfo_.file_url, pThis->download_path_, pThis->pAutoUpdateEvent_, local_full_path);
	utils::SafeDelete(pThis->pFileDownLoad_);
	switch(ret)
	{
	case DOWNLOAD_SUCCESS:
		pThis->pAutoUpdateEvent_->RunEvent(DOWNLOAD_FILE_DONE);
		break;
	case DOWNLOAD_STOP:
		pThis->pAutoUpdateEvent_->RunEvent(DOWNLOAD_FILE_STOP);
		return 0;
	case DOWNLOAD_FAILED:
		pThis->pAutoUpdateEvent_->RunEvent(DOWNLOAD_FILE_FAILED);
		return 0;
	}

	/* 2.文件完整性校验 */
	pThis->pAutoUpdateEvent_->RunEvent(CHECK_FILE_START);

	if(pThis->CheckMD5(pThis->updateFileInfo_.file_checksum, local_full_path))
	{
		pThis->pAutoUpdateEvent_->RunEvent(CHECK_FILE_DONE);
	}
	else
	{
		pThis->pAutoUpdateEvent_->RunEvent(CHECK_FILE_FAILED);
		return 0;
	}

	/* 3.解压文件执行文件替换 */
	pThis->pAutoUpdateEvent_->RunEvent(UNZIP_FILE_START);

	if(pThis->UnzipFile(local_full_path, pThis->replace_path_))
	{
		pThis->pAutoUpdateEvent_->RunEvent(UNZIP_FILE_DONE);
	}
	else
	{
		pThis->pAutoUpdateEvent_->RunEvent(UNZIP_FILE_FAILED);
	}

	return 0;
}

bool CAutoUpdate::CheckMD5( const std::string& get_md5, const std::string& source_file)
{
	MD5 md5;
	std::ifstream fin(source_file.c_str(), std::ios::in|std::ios::binary);
	if (!fin.is_open())
	{
		LOG4CXX_ERROR(g_logger, "CAutoUpdate::CheckMD5 open file failed = " << source_file);
		return false;
	}

	md5.update(fin);	 
	std::string generate_md5 = md5.toString();

	fin.close();

	if (get_md5.compare(generate_md5) != 0)
	{
		return false;
	}

	return true;
}

bool CAutoUpdate::UnzipFile( const std::string& source_file, const std::string& dest_path )
{
	// prepare source_filew
	std::wstring source_filew = EncodingConverter::ToWideString(source_file.c_str());

	// Where we store the pointers to LiteUnzip.dll's functions that we call
	UnzipOpenFilePtr		*lpUnzipOpenFile;
	UnzipGetItemPtr			*lpUnzipGetItem;
	UnzipItemToFilePtr		*lpUnzipItemToFile;
	UnzipClosePtr			*lpUnzipClose;
	//UnzipFormatMessagePtr	*lpUnzipFormatMessage;
	typedef DWORD WINAPI UnzipFormatMessageAPtr(DWORD, char *, DWORD);
	UnzipFormatMessageAPtr	*lpUnzipFormatMessageA;
	typedef DWORD WINAPI UnzipSetBaseDirAPtr(HUNZIP, const char *);
	UnzipSetBaseDirAPtr		*lpUnzipSetBaseDirA;

	HMODULE		unzipDll;
	HUNZIP		huz;
	DWORD		result;

	unzipDll = LoadLibrary(_T("liteunzip.dll"));
	if(!unzipDll)
	{
		LOG4CXX_ERROR(g_logger, "LoadLibrary(LiteUnzip) failed. error=" << GetLastError());
		return false;
	}

	// Get the addresses of functions in LiteUnzip.dll
	lpUnzipOpenFile = (UnzipOpenFilePtr *)GetProcAddress(unzipDll, UNZIPOPENFILENAME);
	lpUnzipGetItem = (UnzipGetItemPtr *)GetProcAddress(unzipDll, UNZIPGETITEMNAME);
	lpUnzipItemToFile = (UnzipItemToFilePtr *)GetProcAddress(unzipDll, UNZIPITEMTOFILENAME);
	lpUnzipClose = (UnzipClosePtr *)GetProcAddress(unzipDll, UNZIPCLOSENAME);
	//lpUnzipFormatMessage = (UnzipFormatMessagePtr *)GetProcAddress(unzipDll, UNZIPFORMATMESSAGENAME);
	lpUnzipFormatMessageA = (UnzipFormatMessageAPtr *)GetProcAddress(unzipDll, "UnzipFormatMessageA");
	lpUnzipSetBaseDirA = (UnzipSetBaseDirAPtr *)GetProcAddress(unzipDll, "UnzipSetBaseDirA");
	if(!lpUnzipOpenFile || !lpUnzipGetItem || !lpUnzipItemToFile || !lpUnzipClose || !lpUnzipFormatMessageA || !lpUnzipSetBaseDirA)
	{
		LOG4CXX_ERROR(g_logger, "Get the addresses of functions in LiteUnzip.dll failed.");
		return false;
	}

	if (!(result = lpUnzipOpenFile(&huz, reinterpret_cast<LPCTSTR>(source_filew.c_str()), 0)))
	{
		lpUnzipSetBaseDirA(huz, dest_path.c_str());

		ZIPENTRY	ze;
		DWORD		numitems;

		// Find out how many items are in the archive.
		ze.Index = (DWORD)-1;
		if ((result = lpUnzipGetItem(huz, &ze))) goto bad2;
		numitems = ze.Index;

		// Unzip each item, using the name stored (in the zip) for that item.
		for (ze.Index = 0; ze.Index < numitems; ze.Index++)
		{
			if ((result = lpUnzipGetItem(huz, &ze)) || (result = lpUnzipItemToFile(huz, ze.Name, &ze)))
			{
bad2:				lpUnzipClose(huz);
				goto bad;
			}
		}

		// Done unzipping files, so close the ZIP archive.
		lpUnzipClose(huz);
	}
	else
	{
		char msg[100];

bad:	LOG4CXX_ERROR(g_logger, "UnzipFile failed.");	
		lpUnzipFormatMessageA(result, msg, sizeof(msg));
		LOG4CXX_TRACE(g_logger, &msg[0]);

		FreeLibrary(unzipDll);

		return false;
	}

	FreeLibrary(unzipDll);

	return true;
}

void CAutoUpdate::StopDownloadFile()
{
	if (pFileDownLoad_ != NULL)
	{
		pFileDownLoad_->StopDownLoadFile();
	}
}



