#ifndef __LOCK__H__
#define __LOCK__H__

#include <pthread.h>
#include <assert.h>

namespace cocos2d
{

template <class T>
class LockT
{
public:

	LockT(const T& mutex):m_mutex(mutex)
	{
		m_mutex.lock();
	}

	~LockT()
	{
		m_mutex.unlock();
	};

private:

	const T& m_mutex;
};


class Mutext
{
public:

	typedef LockT<Mutext> Lock;

	Mutext();
	~Mutext();


	void lock() const;
	void unlock() const;

private:

	#ifdef WIN32
		mutable CRITICAL_SECTION _mutex;
	#else
		mutable pthread_mutex_t _mutex;
	#endif
};

};

#endif
