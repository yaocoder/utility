/********************************************************************
 *
 *  description:		数据库连接池
 *
 *	author:				wei yao
 *
 *********************************************************************/
#include "connpool.h"
#include <string>
#include <stdexcept>
#include <cppconn/config.h>
#include <cppconn/connection.h>
#include "../utils/utils.h"
#include "../utils/config_file.h"
#include "../defines/defines.h"
using namespace std;
using namespace sql;

extern LoggerPtr g_logger;

boost::mutex ConnPool::instance_mutex_;
ConnPool * ConnPool::connPool = NULL;

ConnPool::ConnPool()
{

}

ConnPool::~ConnPool()
{

}

std::string ConnPool::GetDBName()
{
	return utils::G<ConfigFile>().read<std::string> ("sqldb.dbname", "spmanage");
}

void ConnPool::SetDBName(const char *DBName)
{
	DBName_ = DBName;
}



void ConnPool::Init()
{
	this->maxSize_ = utils::G<ConfigFile>().read<int> ("sqldb.connpool.max", 4);;
	try
	{
		this->driver_ = get_driver_instance();
	}
	catch (sql::SQLException &e)
	{
		LOG4CXX_ERROR(g_logger, "ConnPool::ConnPool:SQLException" << e.what());
	}
	catch (std::runtime_error &e)
	{
		LOG4CXX_ERROR(g_logger, "ConnPool::ConnPool:runtime_error" << e.what());
	}


	boost::unique_lock<boost::mutex> lock(mutex_);

	for (int i = 0; i < 2 && i < this->maxSize_; ++i)
	{
		Connection *conn = this->CreateConnection();
		if (conn)
		{
			conns_.push_back(conn);
		}
		else
		{
			LOG4CXX_ERROR(g_logger, "ConnPool::Init:One connection failed!");
		}
	}

}

void ConnPool::Destroy()
{
	boost::unique_lock<boost::mutex> lock(mutex_);
	for (deque<sql::Connection *>::iterator pos = conns_.begin(); pos != conns_.end(); ++pos)
	{
		this->TerminateConnection(*pos);
	}
	conns_.clear();
	delete this;
}

sql::Connection * ConnPool::CreateConnection()
{
	sql::Connection *conn = NULL;
	try
	{
		std::string ip = utils::G<ConfigFile>().read<std::string> ("sqldb.ip","127.0.0.1");
		std::string port = utils::G<ConfigFile>().read<std::string> ("sqldb.port", "3306");
		std::string url = string("tcp://") + ip + string(":") + port;
		std::string username = utils::G<ConfigFile>().read<std::string> ("sqldb.username", "root");
		std::string password = utils::G<ConfigFile>().read<std::string> ("sqldb.password", "root");

		conn = driver_->connect(url.c_str(), username.c_str(), password.c_str());

		LOG4CXX_INFO(g_logger, "ConnPool::CreateConnection:Create one new connection.");
	}
	catch (sql::SQLException &e)
	{
		LOG4CXX_ERROR(g_logger, "ConnPool::CreateConnection:SQLException=" << e.what());
	}
	catch (std::runtime_error &e)
	{
		LOG4CXX_ERROR(g_logger, "ConnPool::CreateConnection:runtime_error=" << e.what());
	}
	return conn;
}

void ConnPool::TerminateConnection(sql::Connection * conn)
{
	if (conn)
	{
		try
		{
			conn->close();
		}
		catch (...)
		{
		}
		try
		{
			delete conn;
		}
		catch (...)
		{
			LOG4CXX_ERROR(g_logger, "ConnPool::TerminateConnection failed: unknown reason.");
		}
	}
}

sql::Connection * ConnPool::GetConnection()
{
	sql::Connection * conn = NULL;
	boost::unique_lock<boost::mutex> lock(mutex_);

	if (!conns_.empty())
	{
		conn = conns_.front();
		conns_.pop_front();
		if (!this->IsValidConnection(conn))
		{
			this->TerminateConnection(conn);
			conn = NULL;
			LOG4CXX_WARN(g_logger, "ConnPool::GetConnection:one connection has invalid.");
		}
		else
			return conn;
	}

	if (conn == NULL)
	{
		conn = this->CreateConnection();
	}

	if (conn)
	{
		if (!this->IsValidConnection(conn))
		{
			this->TerminateConnection(conn);
			conn = NULL;
		}
	}

	return conn;
}

void ConnPool::ReleaseConnection(sql::Connection * conn)
{
	if (conn)
	{
		boost::unique_lock<boost::mutex> lock(mutex_);
		bool found = false;
		for (deque<sql::Connection *>::iterator pos = conns_.begin(); pos != conns_.end(); ++pos)
		{
			if (*pos == conn)
			{
				found = true;
				break;
			}
		}
		if (!found)
		{
			conns_.push_back(conn);
		}
	}
}

// 判断连接是否有效（为了解决8小时连接失效问题)
bool ConnPool::IsValidConnection(sql::Connection *conn)
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


