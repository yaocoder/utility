/*
 * init_log4cxx.h
 *
 *  Created on: 2012-9-12
 *      Author: yaowei
 */

#ifndef INIT_LOG4CXX_H_
#define INIT_LOG4CXX_H_

#include <string>


class CInitConfig {
public:
	CInitConfig();
	virtual ~CInitConfig();

public:

	void InitLog4cxx(std::string project_name);

	bool LoadConfiguration(std::string& file_path );

    std::string getCurrentPath() const;

private:

	std::string current_path_;
};

#endif /* INIT_LOG4CXX_H_ */
