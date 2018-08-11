#include "cocos2d.h"

#include "Thread.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
using namespace cocos2d;

#ifdef WIN32
#include <process.h>

ThreadControl::ThreadControl():m_hThreadHandle(0)
{
}

ThreadControl::ThreadControl(HANDLE hHandle):m_hThreadHandle(hHandle)
{
}

void ThreadControl::join()
{
	if(0 == m_hThreadHandle)
	{
		throw GenericException("invalid handle");
	}

	DWORD rc = WaitForSingleObjectEx(m_hThreadHandle, INFINITE, true);
	if(rc != WAIT_OBJECT_0)
	{
		DWORD dwLastError = GetLastError();
		throw SysCallException(__FILE__, __LINE__, dwLastError);
	}

	detach();
}

void ThreadControl::detach()
{
	if(m_hThreadHandle == 0)
	{
		throw GenericException("detach fail invalid handle");
	}

	if(CloseHandle(m_hThreadHandle) == 0)
	{
		DWORD dwLastError = GetLastError();
		throw SysCallException(__FILE__, __LINE__, dwLastError);
	}
}


void ThreadControl::sleep(int iMicroSecond)
{
	Sleep(iMicroSecond);
}


static unsigned int WINAPI startHook(void* arg)
{
	Thread* pTmpThread = NULL;
	try
	{
		Thread* pThread = static_cast<Thread*>(arg);
		assert(pThread);
		pTmpThread = pThread;
		pThread->run();
	}
	catch(std::exception& e)
	{
		CCLOG("startHook:%s", e.what() );
	}
	catch(...)
	{
		CCLOG("startHook unknow exception");
	}

	pTmpThread->_done();

	return 0;
}


Thread::Thread():m_handle(0), m_id(0), m_bStart(false), m_bAlive(false)
{
}

Thread::Thread(const std::string& strName):m_handle(0), m_id(0), m_bStart(false), m_bAlive(false), m_strThreadName(strName)
{

}

Thread::~Thread()
{
}

ThreadControl Thread::start()
{
	if(m_bStart)
    {
        throw GenericException("thread is started");
    }

	Thread::Lock guardLock(*this);
	unsigned int id = 0;
    m_handle = reinterpret_cast<HANDLE>(
            _beginthreadex(0, 
                            static_cast<unsigned int>(0), 
                            startHook, this, 
                            0, 
                            &id));
    m_id = id;
    assert(m_handle != (HANDLE)-1L);
    if(m_handle == 0)
    {
    	DWORD dwLastError = GetLastError();
		throw SysCallException(__FILE__, __LINE__, dwLastError);
    }

    m_bStart = true;
    m_bAlive = true;
    
    return ThreadControl(m_handle);
}

const std::string& Thread::getName()
{
	return m_strThreadName;
}

ThreadControl Thread::getThreadControl()
{
	if(0 == m_handle)
	{
		throw GenericException("thread not start");
	}

	return ThreadControl(m_handle);
}

void Thread::_done()
{
	Thread::Lock guardLock(*this);
	m_bAlive = false;
}

#else


extern "C" 
{
static void*
startHook(void* arg)
{
   	Thread* pTmpThread = NULL;
    try
    {
        Thread* rawThread = static_cast<Thread*>(arg);
		pTmpThread = rawThread;
		assert(pTmpThread);
		
        pTmpThread->run();
    }
    catch(std::exception& e)
    {
    	CCLOG("startHook exception:%s", e.what() );
    }
	catch(...)
	{
		CCLOG("startHook unknow exception");
	}

    pTmpThread->_done();
    
    return 0;
}
}


ThreadControl::ThreadControl():m_thread(pthread_self())
{
}

ThreadControl::ThreadControl(pthread_t thread):m_thread(thread)
{
}

void ThreadControl::join()
{
	if(pthread_self() == m_thread)
    {
        throw GenericException(std::string("invalid join operation"));
    }

    void* ignore = 0;
    int rc = pthread_join(m_thread, &ignore);
    if(rc != 0)
    {
		throw SysCallException(__FILE__, __LINE__, rc);
    }
}

void ThreadControl::detach()
{
	if(pthread_self() == m_thread)
    {
        throw GenericException("invalid detach operation");
    }

    int rc = pthread_detach(m_thread);
    if(rc != 0)
    {
		throw SysCallException(__FILE__, __LINE__, rc);
    }
}

void ThreadControl::sleep(int iMicroSecond)
{
	usleep(iMicroSecond);
}


Thread::Thread():m_bStart(false), m_bAlive(false)
{
}

Thread::Thread(const std::string& strName):m_bStart(false), m_bAlive(false), m_strThreadName(strName)
{
}

Thread::~Thread(){}
	
ThreadControl Thread::start()
{
    if(m_bStart)
    {
        throw GenericException("thread is started");
    }

	Thread::Lock guardLock(*this);

    pthread_attr_t attr;
    int rc = pthread_attr_init(&attr);
    if(rc != 0)
    {
        pthread_attr_destroy(&attr);
		throw SysCallException(__FILE__, __LINE__, rc);
    }

    rc = pthread_create(&m_thread, &attr, startHook, this);
    pthread_attr_destroy(&attr);
    if(rc != 0)
    {
       	std::stringstream ss;
		throw SysCallException(__FILE__, __LINE__, rc);
    }

    m_bStart = true;
    m_bAlive = true;
    return ThreadControl(m_thread);
}

const std::string& Thread::getName()
{
	return m_strThreadName;
}

ThreadControl Thread::getThreadControl()
{
	return ThreadControl(m_thread);
}

void Thread::_done()
{
	Thread::Lock guardLock(*this);
	m_bAlive = false;
}

#endif

