/*
 * Init_log4cxx.cc
 *
 *  Created on: 2012-9-12
 *      Author: yaowei
 */

#include "init_configure.h"
#include "config_file.h"
#include "defines.h"
#include "path_utils.h"
#include "utils.h"
#include "EncodingConverter.h"

#ifndef AUTO_UPDATE
#define LOG_CON_FILE        L"\\config\\log.conf"
#else
#define LOG_CON_FILE        L"\\config\\AutoUpdateLog.conf"
#endif 


#define CONF_FILE        	"\\config\\config.ini"

LoggerPtr g_logger;

CInitConfig::CInitConfig() {
	// TODO Auto-generated constructor stub

}

CInitConfig::~CInitConfig() {
	// TODO Auto-generated destructor stub
}

void CInitConfig::InitLog4cxx(std::string project_name) {
	/** 读取配置文件 */
	current_path_ = path_utils::GetModuleDirectory(NULL);

	std::wstring ws_current_path = EncodingConverter::ToWideString(current_path_.c_str());
#ifdef UNIT_TEST
	PropertyConfigurator::configure(ws_current_path + LOG_CON_FILE);
#endif
	g_logger = Logger::getLogger(project_name);
	LOG4CXX_INFO(g_logger, "Run.");
}

bool CInitConfig::LoadConfiguration(std::string& file_path )
{
	std::locale old_locale = std::locale::global(std::locale(""));
	file_path = current_path_ + CONF_FILE;
	std::ifstream conf_file(file_path.c_str());
	std::locale::global(old_locale);
	if(!conf_file)
	{
	    LOG4CXX_ERROR(g_logger, "CInitConfig::LoadConfiguration failed.");
		return false;
	}
	conf_file >> utils::G<ConfigFile>();
	return true;
}

std::string CInitConfig::getCurrentPath() const
{
    return current_path_;
}



