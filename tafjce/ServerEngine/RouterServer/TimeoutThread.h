#ifndef __TIMEOUT_THREAD_H__
#define __TIMEOUT_THREAD_H__

class TimeoutThread:public TC_Thread, public TC_ThreadLock, public TC_HandleBase, public TC_Singleton<TimeoutThread>
{
public:

	TimeoutThread();

	 virtual void run();

	 void terminate();

private:

	bool m_bTerminate;
};


#endif
