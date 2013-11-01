#ifndef auto_update_h__
#define auto_update_h__


#include <windows.h>
#include <string>

typedef struct updateFileInfo_
{
	std::string	update_version;
	std::string file_url;
	std::string file_description;
	std::string file_checksum;
}UPDATE_FILE_INFO;


enum AUTO_UPDATE_EVENT
{
	DOWNLOAD_FILE_START,
	CHECK_FILE_START,
	UNZIP_FILE_START,

	DOWNLOAD_FILE_DONE,
	CHECK_FILE_DONE,
	UNZIP_FILE_DONE,

	DOWNLOAD_FILE_FAILED,
	CHECK_FILE_FAILED,
	UNZIP_FILE_FAILED,

	DOWNLOAD_FILE_STOP,
};


class IAutoUpdateEvent
{
public:

	virtual void RunEvent(int event_type) = 0;
	virtual void DownLoadProgress(const double total, const double downloaded) = 0;
};

class CInitConfig;
class CFileDownLoad;

#ifndef UNIT_TEST

#ifdef _DLL_MODE
#define DLL_MODE_API __declspec(dllexport)
#else
#define DLL_MODE_API __declspec(dllimport)
#endif

#endif

#ifndef UNIT_TEST
class  DLL_MODE_API CAutoUpdate
#else
class  CAutoUpdate
#endif
{
public:

	CAutoUpdate(void);
	~CAutoUpdate(void);

	void Init(const UPDATE_FILE_INFO& updateFileInfo, const std::string& download_path, const std::string& replace_path);
	void SetAutoUpdateEvent(IAutoUpdateEvent* autoUpdateEvent) {pAutoUpdateEvent_ = autoUpdateEvent; }

	bool Run();
	void StopDownloadFile();

private:

	static DWORD WINAPI RunThread(LPVOID param);

	bool CheckMD5(const std::string& get_md5, const std::string& source_file);
	bool UnzipFile(const std::string& source_file, const std::string& dest_path);

private:

	UPDATE_FILE_INFO  updateFileInfo_;
	std::string		  download_path_;
	std::string       replace_path_;

	IAutoUpdateEvent* pAutoUpdateEvent_;
	CInitConfig*	  pInitConfig_;
	CFileDownLoad*    pFileDownLoad_;

	bool running_;
};


#endif // auto_update_h__
