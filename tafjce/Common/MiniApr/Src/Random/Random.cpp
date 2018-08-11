#include "MiniAprPch.h"
#include "Random/IRandom.h"
#include "Random.h"

using namespace MINIAPR;
using namespace std;


extern "C"  IComponent* createMiniAprRandom(Int32 /*dwVersion*/)
{
	return new MiniAprRandom;
}


MiniAprRandom::MiniAprRandom():m_bUseRcv(false)
{
}

bool MiniAprRandom::initlize(const PropertySet& propSet)
{
	srandom(time(0) );

	return true;
}

long int MiniAprRandom::random()
{
	long int lRet = 0;
	if(m_valueList.size() > 0)
	{
		lRet = m_valueList.front();
		m_valueList.pop_front();
	}
	else
	{
		lRet = ::random();
	}

	if(m_bUseRcv)
	{
		m_randRcv(lRet);
	}

	return lRet;
}


void MiniAprRandom::setRandomValue(const list<long int>& valueList)
{
	m_valueList = valueList;
}

void MiniAprRandom::setRandRcv(DelegateRandRcv rcvCb)
{
	m_bUseRcv = true;
	m_randRcv = rcvCb;
}

void MiniAprRandom::clearRandRcv()
{
	m_bUseRcv = false;
}


void MiniAprRandom::clearValueList()
{
	m_valueList.clear();
}



