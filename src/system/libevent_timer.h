//============================================================================
// Author      : yaocoder
// Version     :
// Copyright   : Your copyright notice
// Description : libevent封装的定时器
//============================================================================
#include <time.h>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <event2/util.h>
#include <event2/event.h>
#include <event.h>

typedef void (*Func)(void* param);

class TimerImplByLibevent
{

public:
	TimerImplByLibevent(Func f, void* param, long seconds) :param_(param),heartBeatSeconds_(seconds), func_(f)
	{
		b_stop_ = false;
		base_ = event_init();
		tv_.tv_sec = heartBeatSeconds_;
		tv_.tv_usec = 0;
		evtimer_set(&ev_, time_cb, (void*)this);
		event_add(&ev_, &tv_);
	}

	void Run()
	{
		event_base_dispatch(base_);
	}

	void Stop()
	{
		b_stop_ = true;
//		struct timeval tv;
//		tv.tv_sec = 2;
//		tv.tv_usec = 0;
		event_base_loopbreak(base_);
	}

private:
	void* 	param_;
	long	heartBeatSeconds_;
	Func	func_;

	struct event_base* base_;
	struct event ev_;
	struct timeval tv_;

	bool b_stop_;

	static void time_cb(int fd, short event, void *argc)
	{
		TimerImplByLibevent* pThis = static_cast<TimerImplByLibevent*>(argc);

		if(pThis->b_stop_)
		{
			event_del(&pThis->ev_);
			return;
		}
		event_add(&pThis->ev_, &pThis->tv_); // reschedule timer
		pThis->func_(pThis->param_);
	}

};

class CImplTimer
{
public:

	void HeartBeatImpl(Func func, void* param, long seconds);

	void StopHeartBeatImpl();

private:

	TimerImplByLibevent *timer_;
	static void HeartBeatImplThread(void* arg);
};

void CImplTimer::HeartBeatImpl( Func func, void* param, long seconds)
{
	timer_ = new TimerImplByLibevent(func, param, seconds);

	boost::thread thread_obj(boost::bind(&HeartBeatImplThread, (void*)this));
}

void CImplTimer::HeartBeatImplThread( void* arg )
{
	CImplTimer *pThis = static_cast<CImplTimer*>(arg);

	pThis->timer_->Run();

}

void CImplTimer::StopHeartBeatImpl()
{
	timer_->Stop();
}
