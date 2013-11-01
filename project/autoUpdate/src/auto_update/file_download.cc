#include "file_download.h"
#include <sys/stat.h>
#include "../common/utils.h"
#include "../common/defines.h"

CURL * CFileDownLoad::curl_ = NULL;
bool CFileDownLoad::stop_flag = false;
curl_off_t CFileDownLoad::local_file_len_ = -1;

CFileDownLoad::CFileDownLoad(void)
{
	curl_global_init(CURL_GLOBAL_ALL);
	curl_ = curl_easy_init();
}

CFileDownLoad::~CFileDownLoad(void)
{
	if (curl_ != NULL)
	{
		curl_easy_cleanup(curl_);
	}
	curl_global_cleanup();
}

int CFileDownLoad::StartDownLoadFile( const std::string& fileurl, const std::string& local_path,  IAutoUpdateEvent* pAutoUpdateEvent, std::string& local_full_path)
{	
	if (curl_ == NULL)
	{
		LOG4CXX_ERROR(g_logger, "CFileDownLoad::StartDownLoadFile curl_ == NULL.");
		pAutoUpdateEvent->RunEvent(DOWNLOAD_FILE_FAILED);
		return DOWNLOAD_FAILED;
	}

	char* version = curl_version();
	//LOG4CXX_TRACE(g_logger, "The version is " << version);

	long filesize =0 ;
	int use_resume = 0;
	struct stat file_info;
	CURLcode res;

	std::string filename = fileurl.substr(fileurl.find_last_of("/") + 1, fileurl.length());
	local_full_path = local_path + filename;

	/* 得到本地文件大小 */
	if(stat(local_full_path.c_str(), &file_info) == 0) 
	{
		local_file_len_ =  file_info.st_size;
		use_resume  = 1;
	}

	FILE *outfile_;
	if ((outfile_ = fopen(local_full_path.c_str(), "ab+")) == NULL)
	{
		LOG4CXX_ERROR(g_logger, "CFileDownLoad::StartDownLoadFile fopen failed : " << local_full_path);
		return DOWNLOAD_FAILED;
	}

	curl_easy_setopt(curl_, CURLOPT_URL, fileurl.c_str());

	const long timeout = 60; 
	curl_easy_setopt(curl_,  CURLOPT_CONNECTTIMEOUT, timeout);
	// 设置文件续传的位置给libcurl
	curl_easy_setopt(curl_, CURLOPT_RESUME_FROM_LARGE, use_resume?local_file_len_:0);

	curl_easy_setopt(curl_, CURLOPT_WRITEDATA, outfile_);
	curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, &CFileDownLoad::my_write_func);
	curl_easy_setopt(curl_, CURLOPT_NOPROGRESS, false);
	curl_easy_setopt(curl_, CURLOPT_PROGRESSFUNCTION, &CFileDownLoad::my_progress_func);
	curl_easy_setopt(curl_, CURLOPT_PROGRESSDATA, pAutoUpdateEvent);

	res = curl_easy_perform(curl_);
	int ret = 0;
	if (CURLE_OK == res || CURLE_RANGE_ERROR == res)
	{
		ret = DOWNLOAD_SUCCESS;		
	}
	else if (CURLE_ABORTED_BY_CALLBACK == res)
	{
		ret = DOWNLOAD_STOP;
	}
	else
	{
		LOG4CXX_ERROR(g_logger, "CFileDownLoad::StartDownLoadFile error : " << curl_easy_strerror(res));
		ret = DOWNLOAD_FAILED;
	}

	if(0 !=fclose(outfile_))
	{
		LOG4CXX_ERROR(g_logger, "CFileDownLoad::StartDownLoadFile:fclose error : " << ferror(outfile_));
		ret = DOWNLOAD_FAILED;
	}

	return ret;
}

void CFileDownLoad::StopDownLoadFile()
{
	stop_flag = true;
}

size_t CFileDownLoad::my_write_func(void *buffer, size_t size, size_t count, void *stream)
{
	int writed =  fwrite(buffer, size, count, (FILE *)stream);
	return writed;
}

int CFileDownLoad::my_progress_func(IAutoUpdateEvent* pAutoUpdateEvent, double total, double received, double ultotal, double ulnow )
{
	if (total != 0 || received != 0)
	{
		pAutoUpdateEvent->DownLoadProgress(local_file_len_ + total, local_file_len_ + received);
	}
	if (stop_flag)
	{
		return CURL_WRITEFUNC_PAUSE;
	}
	return 0;
}
