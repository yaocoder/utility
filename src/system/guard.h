
/*****************************************************************************
written by
   Yunhong Gu, last updated 08/01/2009
*****************************************************************************/


#ifndef guard_h__
#define guard_h__

#ifndef WIN32
   #include <sys/time.h>
   #include <sys/uio.h>
   #include <pthread.h>
#else
   #include <windows.h>
#endif
#include <cstdlib>


#ifdef WIN32
   // Windows compability
   typedef HANDLE pthread_t;
   typedef HANDLE pthread_mutex_t;
   typedef HANDLE pthread_cond_t;
   typedef DWORD pthread_key_t;
#endif


class CGuard
{
public:
   CGuard(pthread_mutex_t& lock);
   ~CGuard();

public:
   static void enterCS(pthread_mutex_t& lock);
   static void leaveCS(pthread_mutex_t& lock);

   static void createMutex(pthread_mutex_t& lock);
   static void releaseMutex(pthread_mutex_t& lock);

   static void createCond(pthread_cond_t& cond);
   static void releaseCond(pthread_cond_t& cond);

private:
   pthread_mutex_t& m_Mutex;            // Alias name of the mutex to be protected
   int m_iLocked;                       // Locking status

   CGuard& operator=(const CGuard&);
};

#endif // guard_h__


