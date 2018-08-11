#include "FightDbAccessImp.h"
#include "DbPool.h"
#include "DalFight.h"
#include <time.h>

using namespace ServerEngine;

unsigned long getMicroSecond()
{
	struct timespec tmpSpec;
	clock_gettime(CLOCK_MONOTONIC, &tmpSpec);	
	unsigned long ddV = tmpSpec.tv_sec*1000000 + tmpSpec.tv_nsec/1000;

	return ddV;
}

struct PerformanceHelp
{
	PerformanceHelp(const std::string& strName):m_strName(strName), m_ddBeginTime(0)
	{
		m_ddBeginTime = getMicroSecond();
	}

	~PerformanceHelp()
	{
		unsigned long ddCost = getMicroSecond() - m_ddBeginTime;
		m_ddCount++;
		m_ddSumTime += ddCost;

		if( (m_ddCount % 100) == 0)
		{
			FDLOG("Perforance")<<m_strName<<"|"<<(m_ddSumTime/m_ddCount)<<endl;
		}
	}

	string m_strName;
	unsigned long m_ddBeginTime;
	static unsigned long m_ddCount;
	static unsigned long m_ddSumTime;
};

unsigned long PerformanceHelp::m_ddCount;
unsigned long PerformanceHelp::m_ddSumTime;


FightDbAccessImp::FightDbAccessImp()
{
}

FightDbAccessImp::~FightDbAccessImp()
{
}

void FightDbAccessImp::initialize()
{
	// nothing
}

void FightDbAccessImp::destroy()
{
	// nothing 
}


taf::Int32 FightDbAccessImp::setString(const std::string & strKey,const std::string & strValue,taf::JceCurrentPtr current)
{
	PerformanceHelp tmpGuard("setString");
	RLOG<<"FightDbAccessImp setString:"<<strKey<<"|"<<endl;
    pool_object<DefaultFightDb> aDbConn(DbPool<DefaultFightDb>::getInstance());
    
    return aDbConn->Set(strKey, strValue);
}

taf::Int32 FightDbAccessImp::delString(const std::string & strKey,taf::JceCurrentPtr current)
{
	pool_object<DefaultFightDb> aDbConn(DbPool<DefaultFightDb>::getInstance());
    return aDbConn->Del(strKey);
}

taf::Int32 FightDbAccessImp::getString(const std::string & strKey,std::string &strValue,taf::JceCurrentPtr current)
{
	RLOG<<"FightDbAccessImp getString:"<<strKey<<"|"<<endl;
	//从连接池获取一个连接
    pool_object<DefaultFightDb> aDbConn(DbPool<DefaultFightDb>::getInstance());

    return aDbConn->Get(strKey, strValue);
}

