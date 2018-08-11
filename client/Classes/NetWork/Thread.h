#ifndef __THREAD_H__
#define __THREAD_H__

#include <iostream>
#include <sstream>
#include <assert.h>
#include <list>
#include "Lock.h"
// #include <unistd.h>
#include <string.h>
#include <pthread.h>

//#define CCLOG	printf
//#define CCLOGERROR printf

namespace cocos2d
{

class GenericException:public std::exception
{
public:

	GenericException(const char* pszErrMsg):m_strExceptionMsg(pszErrMsg){}
	GenericException(const std::string& strErrMsg):m_strExceptionMsg(strErrMsg){}
	~GenericException() throw(){}

	virtual const char* what() const throw()
	{
		return m_strExceptionMsg.c_str();
	}

private:
	std::string m_strExceptionMsg;	
};

class SysCallException:public std::exception
{
public:

	#ifdef WIN32
	SysCallException(const char* szFile, int iLine, DWORD dwLastError)
	{
		std::stringstream ss;
		ss<<"syscall fail|"<<szFile<<"|"<<iLine<<"|"<<dwLastError;
		m_strExceptionMsg = ss.str();
	}
	#else
	SysCallException(const char* szFile, int iLine, int iRc)
	{
		std::stringstream ss;
		ss<<"syscall fail|"<<szFile<<"|"<<iLine<<"|"<<iRc;
		m_strExceptionMsg = ss.str();
	}
	#endif

	~SysCallException() throw(){}

	virtual const char* what() const throw()
	{
		return m_strExceptionMsg.c_str();
	}

private:
	std::string m_strExceptionMsg;
};

class ThreadControl
{
public:

	// constructor
	ThreadControl();
#ifdef WIN32
	ThreadControl(HANDLE hHandle);
#else
	ThreadControl(pthread_t);
#endif

	// operator
	
	// wait thread finish
	void join();

	// detach a thread
	void detach();

	// sleep time
	void sleep(int iMicroSecond);


#ifdef WIN32
	HANDLE m_hThreadHandle;	
#else
	pthread_t m_thread;
#endif
};

class Thread:public Mutext
{
public:

	Thread();
	Thread(const std::string& strName);
	virtual ~Thread();
	
	// start the thread
	ThreadControl start();

	// thread work function
	virtual void run() = 0;

	// get thread Name
	const std::string& getName();

	// get thread control
	ThreadControl getThreadControl();

	// internal help function
	void _done();

protected:
#ifdef WIN32
	HANDLE m_handle;
	DWORD  m_id;
#else
	pthread_t m_thread;
#endif

	bool m_bStart;
	bool m_bAlive;
	std::string m_strThreadName;
};

};

#endif
