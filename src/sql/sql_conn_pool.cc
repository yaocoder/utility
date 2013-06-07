/********************************************************************
 *
 *	created:	   		2011/11/08
 *
 *	filename:
 *
 *  description:		数据库连接池
 *
 *	author:				wei yao
 *
 *  modify by
 *
 *  modify description
 *
 *********************************************************************/
#include "sql_conn_pool.h"
#include <stdexcept>
#include <cppconn/config.h>
#include <cppconn/connection.h>
#include "defines.h"
#include "../public/config_file.h"
#include "../public/utils.h"

using namespace std;
using namespace sql;

CSqlConnPool::CSqlConnPool()
{
	driver_ = NULL;
	DBName_ = NULL;
}

CSqlConnPool::~CSqlConnPool()
{
	Destroy();
}

void CSqlConnPool::Init(const SqlConnInfo& sqlConnInfo)
{
	sqlConnInfo_ = sqlConnInfo;

	try
	{
		this->driver_ = get_driver_instance();
	}
	catch (sql::SQLException &e)
	{
		LOG4CXX_ERROR(g_logger, "CSqlConnPool::Init:SQLException" << e.what());
	}
	catch (std::runtime_error &e)
	{
		LOG4CXX_ERROR(g_logger, "CSqlConnPool::Init:runtime_error" << e.what());
	}

	for (int i = 0; i < sqlConnInfo.max_conn_num; ++i)
	{
		Connection *conn = this->CreateConnection();
		if (conn)
		{
			queue_conn_.push_back(conn);
		}
		else
		{
			LOG4CXX_ERROR(g_logger, "CSqlConnPool::Init:connection failed! index = " << i);
		}
	}
}

void CSqlConnPool::SetDBName(const char *DBName)
{
	DBName_ = DBName;
}

void CSqlConnPool::Destroy()
{
	for (deque<sql::Connection *>::iterator pos = queue_conn_.begin(); pos != queue_conn_.end(); ++pos)
	{
		this->TerminateConnection(*pos);
	}
	queue_conn_.clear();
}

sql::Connection * CSqlConnPool::CreateConnection()
{
	sql::Connection *conn = NULL;
	try
	{
		std::string url = string("tcp://") + sqlConnInfo_.ip + string(":") + sqlConnInfo_.port;
		conn = driver_->connect(url.c_str(), sqlConnInfo_.user_name.c_str(), sqlConnInfo_.password.c_str());

		LOG4CXX_TRACE(g_logger, "CSqlConnPool::CreateConnection:Create one new connection.");
	}
	catch (sql::SQLException &e)
	{
		LOG4CXX_ERROR(g_logger, "CSqlConnPool::CreateConnection:SQLException=" << e.what());
	}
	catch (std::runtime_error &e)
	{
		LOG4CXX_ERROR(g_logger, "CSqlConnPool::CreateConnection:runtime_error=" << e.what());
	}
	return conn;
}

void CSqlConnPool::TerminateConnection(sql::Connection * conn)
{
	if (conn)
	{
		try
		{
			conn->close();
		}
		catch (...)
		{
			LOG4CXX_ERROR(g_logger, "CSqlConnPool::TerminateConnection failed: unknown reason.");
		}

		delete conn;
		conn = NULL;
	}
}

sql::Connection * CSqlConnPool::GetConnection()
{
	sql::Connection * conn = NULL;
	boost::unique_lock<boost::mutex> lock(mutex_);

	if (!queue_conn_.empty())
	{
		conn = queue_conn_.front();
		queue_conn_.pop_front();
	}

	/* 如果连接不是空指针，先判断是否已经失效（8小时未用失效问题)*/
	if (conn)
	{
		if (!this->IsValidConnection(conn))
		{
			this->TerminateConnection(conn);
		}
	}

	if (conn == NULL)
	{
		conn = this->CreateConnection();
		if(conn != NULL)
		{
			conn->setSchema(DBName_);
		}
	}


	return conn;
}

void CSqlConnPool::ReleaseConnection(sql::Connection * conn)
{
	if (conn)
	{
		boost::unique_lock<boost::mutex> lock(mutex_);
		bool found = false;
		for (deque<sql::Connection *>::iterator pos = queue_conn_.begin(); pos != queue_conn_.end(); ++pos)
		{
			if (*pos == conn)
			{
				found = true;
				break;
			}
		}
		if (!found)
		{
			queue_conn_.push_back(conn);
		}
	}
}

/* 判断连接是否有效（为了解决8小时连接失效问题) */
bool CSqlConnPool::IsValidConnection(sql::Connection *conn)
{
	bool ret = true;
	try
	{
		conn->setSchema(DBName_);
	}
	catch (...)
	{
		ret = false;
	}
	return ret;
}

