#ifndef __PROFILER_STRATEGY_AVERAGE_H__
#define __PROFILER_STRATEGY_AVERAGE_H__

#include "TimerManager/ITimerComponent.h"

BEGIN_MINIAPR_NAMESPACE

class ProfilerStrategyAverage:public ComponentBase<IProfilerStrategy, IID_IProfilerStrategy>, public ITimerCallback
{
public:
	
	struct ProfileAverageData
	{
		Uint32	dwMinMicroseconds;
		Uint32	dwMaxMicroseconds;
		Uint32	dwTotalMicroseconds;
		Int32	nCount;
		Uint64	ddBeginTime;
	};

	ProfilerStrategyAverage();
	~ProfilerStrategyAverage();

	virtual bool initlize(const PropertySet& propSet);
	virtual bool inject(const std::string& componentName, IComponent* pComponent);
	virtual void onBeginProfiler(const std::string& strProfilerName, Uint64 ddTimer);
	virtual void onEndProfiler(const std::string& strProfilerName, Uint64 ddTimer);
	virtual void onObjCountInc(const std::string& strClassName, int iSingleSize);
	virtual void onObjCountDec(const std::string& strClassName);

	virtual void onTimer(int nEventId);

private:
	
	typedef std::map<std::string, ProfileAverageData>	MapProfileData;
	
	//FILE*			m_pFile;
	//std::string		m_strLogFileName;
	Int32			m_nTimeInterval;
	MapProfileData	m_profileDataMap;
	ITimerComponent*		m_pTimerManager;
	std::map<std::string, int> m_mapObjCount;
	std::map<std::string, int> m_mapObjSize;
};

END_MINIAPR_NAMESPACE

#endif
