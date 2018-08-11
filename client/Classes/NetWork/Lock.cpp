#include "cocos2d.h"

#include "Lock.h"

using namespace cocos2d;

Mutext::Mutext()
{
#ifdef WIN32
    InitializeCriticalSection(&_mutex);
#else
	
	int rc;
    pthread_mutexattr_t attr;
    rc = pthread_mutexattr_init(&attr);
    assert(rc == 0);

    rc = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
    assert(rc == 0);

    rc = pthread_mutex_init(&_mutex, &attr);
    assert(rc == 0);

    rc = pthread_mutexattr_destroy(&attr);
    assert(rc == 0);

    if(rc != 0)
    {
        assert(false);
    }
	
#endif
}

Mutext::~Mutext()
{
#ifdef WIN32
	DeleteCriticalSection(&_mutex);
#else
	int rc = 0;
    rc = pthread_mutex_destroy(&_mutex);
    assert(rc == 0);
#endif
}

void Mutext::lock() const
{
#ifdef WIN32
	EnterCriticalSection(&_mutex);
    assert(_mutex.RecursionCount == 1);
#else
	int rc = pthread_mutex_lock(&_mutex);
   	assert(rc == 0);
#endif
}

void Mutext::unlock() const
{
#ifdef WIN32
	assert(_mutex.RecursionCount == 1);
    LeaveCriticalSection(&_mutex);
#else
	int rc = pthread_mutex_unlock(&_mutex);
   	assert(rc == 0);
#endif
}


