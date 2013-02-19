/********************************************************************
 *
 *  description:		数据库连接池
 *
 *	author:				wei yao
 *
 *********************************************************************/
#ifndef CONN_POOL_H
#define CONN_POOL_H

#include <mysql_connection.h>
#include <mysql_driver.h>
#include <cppconn/connection.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <boost/thread.hpp>
#include <deque>
#include <map>

using namespace std;

#define SAFE_CLOSE(x) \
{ \
	if (x != 0){ \
	try{x->close();}catch(...){}; \
	delete x; \
	x = 0; \
	} \
}

#define SAFE_CLOSE2(x,y) \
{ \
	if (x != 0){ \
	try{x->close();}catch(...){}; \
	delete x; \
	x = 0; \
	} \
	if (y != 0){ \
	try{y->close();}catch(...){}; \
	delete y; \
	y = 0; \
	} \
}

class ConnPool
{
public:
	ConnPool();
	~ConnPool();

	std::string 	GetDBName();
	void 			SetDBName(const char* DBName);
	sql::Connection*GetConnection();
	void 			ReleaseConnection(sql::Connection * conn);

	void 			Init();
	void 			Destroy();

private:
	deque<sql::Connection *> conns_;
	int 				maxSize_;
	boost::mutex 		mutex_;
	sql::Driver* 		driver_;
	const char*   		DBName_;

	static ConnPool* 	connPool;
	static boost::mutex instance_mutex_;

	sql::Connection * 	CreateConnection();
	void 			  	TerminateConnection(sql::Connection * conn);
	bool 				IsValidConnection(sql::Connection *conn); //判断数据库连接是否有效（为了解决8小时连接失效问题)
};

#endif
