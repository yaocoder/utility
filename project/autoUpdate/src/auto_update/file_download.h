#ifndef file_download_h__
#define file_download_h__

#include <string>
#include <stdio.h>
#include <curl/curl.h>
#include "auto_update.h"

enum DOWNLOAD_RESULT
{
	DOWNLOAD_SUCCESS,
	DOWNLOAD_STOP,
	DOWNLOAD_FAILED,
};

class CFileDownLoad
{
public:
	CFileDownLoad(void);
	~CFileDownLoad(void);

	int  StartDownLoadFile(const std::string& fileurl, const std::string& local_path, IAutoUpdateEvent* pAutoUpdateEvent, std::string& local_full_path);

	void StopDownLoadFile();

private:

	static size_t my_write_func(void *buffer, size_t size, size_t count, void *stream);

	static int my_progress_func(IAutoUpdateEvent* autoUpdateEvent,double total, double reciced, double ultotal, double ulnow);

	static CURL *curl_;

	static bool stop_flag;

	static curl_off_t local_file_len_;

};
#endif // file_download_h__
