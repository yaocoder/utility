/********************************************************************
 *
 *	created:	   		2011/11/08
 *
 *	filename:			sql_conn_pool.h
 *
 *  description:		数据库连接池
 *
 *	author:				wei yao
 *
 *  modify by：
 *
 *  modify description：
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

typedef struct sqlConnInfo_
{
	int max_conn_num;
	std::string ip;
	std::string port;
	std::string user_name;
	std::string password;
} SqlConnInfo;

class CSqlConnPool
{
public:
	CSqlConnPool();
	~CSqlConnPool();

	void Init(const SqlConnInfo& sqlConnInfo);
	void Destroy();

	void SetDBName(const char* DBName);

	sql::Connection*GetConnection();
	void ReleaseConnection(sql::Connection * conn);

private:

	SqlConnInfo sqlConnInfo_;

	boost::mutex mutex_;
	sql::Driver* driver_;
	const char*  DBName_;
	deque<sql::Connection *> queue_conn_;

	sql::Connection * CreateConnection();
	void TerminateConnection(sql::Connection * conn);
	bool IsValidConnection(sql::Connection *conn); //判断数据库连接是否有效（为了解决8小时连接失效问题)
};

#endif
