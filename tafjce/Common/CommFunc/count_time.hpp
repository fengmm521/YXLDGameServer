#ifndef _COUNT_TIME_H
#define _COUNT_TIME_H

#include <sys/time.h>

class TCountTimeExt
{
public:
	TCountTimeExt():m_tEnd() {vBegin();}
	~TCountTimeExt(){}
public:
	void vBegin()
	{
	    gettimeofday(&m_tBegin,0);
	}
	void vEnd()
	{
	    gettimeofday(&m_tEnd,0);
	}
	int iCountUsec()
	{
        vEnd ();
	    return (m_tEnd.tv_sec - m_tBegin.tv_sec) * 1000000 + (m_tEnd.tv_usec - m_tBegin.tv_usec);
	}
public:
	timeval m_tBegin;
	timeval m_tEnd;
};


class TCountTime
{
public:
	TCountTime(){}
	~TCountTime(){}
public:
	void vBegin()
	{
		gettimeofday(&m_tBegin,0);
	}
	void vEnd()
	{
		gettimeofday(&m_tEnd,0);
	}
	int iCountUsec()
	{
		return (m_tEnd.tv_sec - m_tBegin.tv_sec)*1000000+(m_tEnd.tv_usec -m_tBegin.tv_usec);
	}
public:
	timeval m_tBegin;
	timeval m_tEnd;
};


#endif



