#include "RouterComm.h"
#include "TimeoutThread.h"
#include "RouterImp.h"


TimeoutThread::TimeoutThread():m_bTerminate(false)
{
}


void TimeoutThread::terminate()
{
	TC_ThreadLock::Lock lock(*this);
	m_bTerminate = true;
	notify();
}


void TimeoutThread::run()
{
	while(!m_bTerminate)
	{
		TC_ThreadLock::Lock lock(*this);

        timedWait(2000);

		if(m_bTerminate) return;

		ConnectionManager::getInstance()->timeout();
	}
}


