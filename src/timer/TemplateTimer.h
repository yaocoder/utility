/*出处：http://www.codeproject.com/Articles/146617/Simple-C-Timer-Wrapper*/

#pragma once
#include <atlbase.h>

namespace MyTimer
{

static void CALLBACK TimerProc(void*, BOOLEAN);

///////////////////////////////////////////////////////////////////////////////
//
// class CTimer
//
class CTimer
{
public:
    CTimer()
    {
        m_hTimer = NULL;
        m_mutexCount = 0;
    }

    virtual ~CTimer()
    {
        Stop();
    }

    bool Start(unsigned int interval,   // interval in ms
               bool immediately = false,// true to call first event immediately
               bool once = false)       // true to call timed event only once
    {
        if( m_hTimer )
        {
            return false;
        }

        SetCount(0);

        BOOL success = CreateTimerQueueTimer( &m_hTimer,
                                              NULL,
                                              TimerProc,
                                              this,
                                              immediately ? 0 : interval,
                                              once ? 0 : interval,
                                              WT_EXECUTEINTIMERTHREAD);

        return( success != 0 );
    }

    void Stop()
    {
        DeleteTimerQueueTimer( NULL, m_hTimer, NULL );
        m_hTimer = NULL ;
    }

    virtual void OnTimedEvent()
    {
        // Override in derived class
    }

    void SetCount(int value)
    {
        InterlockedExchange( &m_mutexCount, value );
    }

    int GetCount()
    {
        return InterlockedExchangeAdd( &m_mutexCount, 0 );
    }

private:
    HANDLE m_hTimer;
    long m_mutexCount;
};

///////////////////////////////////////////////////////////////////////////////
//
// TimerProc
//
void CALLBACK TimerProc(void* param, BOOLEAN timerCalled)
{
    CTimer* timer = static_cast<CTimer*>(param);
    timer->SetCount( timer->GetCount()+1 );
    timer->OnTimedEvent();
};

///////////////////////////////////////////////////////////////////////////////
//
// template class TTimer
//
template <class T> class TTimer : public CTimer
{
public:
    typedef private void (T::*TimedFunction)(void);

    TTimer()
    {
        m_pTimedFunction = NULL;
        m_pClass = NULL;
    }

    void SetTimedEvent(T *pClass, TimedFunction pFunc)
    {
        m_pClass         = pClass;
        m_pTimedFunction = pFunc;
    }

protected:
    void OnTimedEvent()  
    {
        if (m_pTimedFunction && m_pClass)
        {
            (m_pClass->*m_pTimedFunction)();
        }
    }

private:
    T *m_pClass;
    TimedFunction m_pTimedFunction;
};

/*
例子：
TimerTest.h
 Collapse | Copy Code
#pragma once
#include "TemplateTimer.h"

class CTimerTest
{
public:
    void RunTest();

private:
    void OnTimedEvent1();
    void OnTimedEvent2();

    TTimer<CTimerTest> timer1 ;
    TTimer<CTimerTest> timer2 ;
};

TimerTest.cpp
 Collapse | Copy Code
#include "stdafx.h"
#include "TimerTest.h"

void CTimerTest::OnTimedEvent1()
{
    printf("\r\nTimer 1  Called (count=%i)", timer1.GetCount());
}

void CTimerTest::OnTimedEvent2()
{
    printf("\r\nTimer  2 Called (count=%i)", timer2.GetCount());
}

void CTimerTest::RunTest()
{
    printf("Hit return to start and stop timers");
    getchar();

    timer1.SetTimedEvent(this, &CTimerTest::OnTimedEvent1);
    timer1.Start(1000); // Start timer 1 every 1s

    timer2.SetTimedEvent(this, &CTimerTest::OnTimedEvent2);
    timer2.Start(2000); // Start timer 2 every 2s

    // Do something, in this case just wait for user to hit return   
    getchar();          // Wait for return (stop)

    timer1.Stop();      // Stop timer 1
    timer2.Stop();      // Stop timer 2

    printf("\r\nTimers stopped (hit return to exit)");
    getchar();
}
*/

}
