#ifndef __WATCH_THREAD_H__
#define  __WATCH_THREAD_H__

class WatchThread:public taf::TC_Thread
{
public:

	// Constructor/Destructor
	WatchThread();
	~WatchThread();

	// 功能: 停止线程
	void StopSelf();
	
	// TC_Thread Interface
	virtual void run();

private:
	bool	m_bStopThread;
};

#endif
