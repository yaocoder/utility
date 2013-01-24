#ifndef time_utils_h__
#define time_utils_h__

#ifndef WIN32
#include <sys/time.h>
#else
#include<ctime> 
#endif


namespace time_utils
{

#ifndef WIN32

	inline std::string NowtimeString()
	{
		time_t   time_;
		struct   tm   *ptm;
		time_    =   time(NULL);
		ptm      =   localtime(&time_);
		char temp[100] = {0};
		snprintf(temp,sizeof(temp), "%Y-%m-%d %d:%d:%d",ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
		std::string str;
		str.assign(temp);
		return str;
	}

#else
	inline std::string NowtimeString()
	{
		std::time_t time_now = std::time(NULL);
		tm* tm_now = localtime(&time_now);
		char time_str[sizeof("yyyy-mm-dd hh:mm:ss")] = {0};
		std::strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_now);
		return time_str;
	}

	inline std::string FormatTimeString(unsigned __int64 time_stamp)
	{
		std::time_t time_then = time_stamp;
		tm* tm_now = localtime(&time_then);
		char time_str[sizeof("yyyy-mm-dd hh:mm:ss")] = {0};
		std::strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_now);
		return time_str;
	}

#endif

	
}

#endif // time_utils_h__