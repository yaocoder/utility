#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/system/system_error.hpp> 
#include <boost/thread.hpp>

typedef void (*Func)();

void print()
{
	std::cout << "Hello, world!\n";
}

void print1()
{
	std::cout << "hahahhahh\n";
}

class TimerImplByAsio
{

public:
	TimerImplByAsio(Func f) : func_(f), timer_(io_, boost::posix_time::seconds(0))
	{
		try
		{
			timer_.async_wait(boost::bind(&TimerImplByAsio::update, this));
		}
		catch(boost::system::system_error& ex )
		{
			boost::system::error_code ec = ex.code(); 
			std::cerr << ec.value() << std::endl; 
			std::cerr << ec.category().name() << std::endl; 
		}

	}

	void Run()
	{
		try
		{
			io_.run();
		}
		catch(boost::system::system_error& ex )
		{
			boost::system::error_code ec = ex.code(); 
			std::cerr << ec.value() << std::endl; 
			std::cerr << ec.category().name() << std::endl; 
		}
	}
	
	void Stop()
	{
		try
		{
			io_.stop();
		}
		catch( boost::system::system_error& ex )
		{
			boost::system::error_code ec = ex.code(); 
			std::cerr << ec.value() << std::endl; 
			std::cerr << ec.category().name() << std::endl; 
		}
	}

private:
	Func	func_;
	boost::asio::io_service io_;
	boost::asio::deadline_timer timer_;
	void update()
	{
		try
		{
			func_();
			timer_.expires_at(timer_.expires_at() + boost::posix_time::seconds(2));
			timer_.async_wait(boost::bind(&TimerImplByAsio::update, this));
		}
		catch(boost::system::system_error& ex )
		{
			boost::system::error_code ec = ex.code(); 
			std::cerr << ec.value() << std::endl; 
			std::cerr << ec.category().name() << std::endl; 
		}
	}
};

class CImpl
{
public:

	void HeartBeatImpl(Func func);

	void StopHeartBeatImpl();

private:

	Func func_;
	TimerImplByAsio *timer_;
	static void HeartBeatImplThread(void* arg);
};

void CImpl::HeartBeatImpl( Func func )
{
	func_ = func;

	timer_ = new TimerImplByAsio(func_);

	boost::thread thread_obj(boost::bind(&HeartBeatImplThread, (void*)this));
}

void CImpl::HeartBeatImplThread( void* arg )
{
	CImpl *pThis = static_cast<CImpl*>(arg);

	pThis->timer_->Run();

}

void CImpl::StopHeartBeatImpl()
{
	timer_->Stop();
	if (timer_ != NULL)
	{
		delete timer_;
	}
}

int main()
{
	CImpl *impl = new CImpl;
	impl->HeartBeatImpl(print);

	getchar();

	impl->StopHeartBeatImpl();

	return 0;
}
