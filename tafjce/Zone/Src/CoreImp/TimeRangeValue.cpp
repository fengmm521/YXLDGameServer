#include "CoreImpPch.h"
#include "TimeRangeValue.h"


extern "C" IComponent* createTimeRangeValueFactory(Int32)
{
	return new TimeRangeValue;
}

ITimeRangeTask::~ITimeRangeTask()
{
	ITimeRangeValue* pTimeRangeValue = getComponent<ITimeRangeValue>(COMPNAME_TimeRangeValue, IID_ITimeRangeValue);
	assert(pTimeRangeValue);

	pTimeRangeValue->removeRangeValue(this);
}



struct KeeperDayChgCheck
{
	KeeperDayChgCheck(TimeRangeKeeper& ref):m_refKeeper(ref){}
	bool operator() (Uint32 dwCurTime) const
	{
		IZoneTime* pZomeTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
		assert(pZomeTime);

		if(!pZomeTime->IsInSameDay(m_refKeeper.getTime(), dwCurTime) )
		{
			return true;
		}

		return false;
	}

private:
	TimeRangeKeeper& m_refKeeper;
};


struct KeeperWeekChgCheck
{
	KeeperWeekChgCheck(TimeRangeKeeper& ref):m_refKeeper(ref){}
	bool operator() (Uint32 dwCurTime) const
	{
		IZoneTime* pZomeTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
		assert(pZomeTime);

		if(!pZomeTime->IsInSameChineseWeek(m_refKeeper.getTime(), dwCurTime) )
		{
			return true;
		}

		return false;
	}

private:
	TimeRangeKeeper& m_refKeeper;
};


struct KeeperChgReset
{
	KeeperChgReset(TimeRangeKeeper& ref):m_refKeeper(ref){}

	void operator() (Uint32 dwCurTime) const
	{
		m_refKeeper._setValue(0);
	}

private:
	TimeRangeKeeper& m_refKeeper;
};


void TimeRangeKeeper::setCheckMode(int iMode)
{
	m_iCheckMode = iMode;
}

TimeRangeKeeper::TimeRangeKeeper():m_iBindPropID(0), m_bWeekCheck(false), m_iCheckMode(en_TimeRange_Day), m_bCustomExe(false)
{
}
void TimeRangeKeeper::initData(const ServerEngine::TimeResetValue& data, HEntity hEntity, int iPropID)
{
	m_hBindEntity = hEntity;
	m_iBindPropID = iPropID;
	
	m_keepData = data;

	IZoneTime* pZoneTime = getComponent<IZoneTime>("ZoneTime", IID_IZoneTime);
	assert(pZoneTime);
	
	if(getCheckDelegate()(pZoneTime->GetCurSecond() ) )
	{
		getExecuteDelegate()(pZoneTime->GetCurSecond() );
	}

	IEntity* pEntity = getEntityFromHandle(m_hBindEntity);
	if(pEntity && m_iBindPropID) pEntity->setProperty(m_iBindPropID, data.iValue);
}
	
int TimeRangeKeeper::getValue()
{
	IZoneTime* pZoneTime = getComponent<IZoneTime>("ZoneTime", IID_IZoneTime);
	assert(pZoneTime);

	if(getCheckDelegate()(pZoneTime->GetCurSecond() ) )
	{
		getExecuteDelegate()(pZoneTime->GetCurSecond() );
	}

	return m_keepData.iValue;
}

void TimeRangeKeeper::_setValue(int iValue)
{
	IZoneTime* pZoneTime = getComponent<IZoneTime>("ZoneTime", IID_IZoneTime);
	assert(pZoneTime);

	m_keepData.iValue = iValue;
	m_keepData.dwLastChgTime = pZoneTime->GetCurSecond();

	IEntity* pEntity = getEntityFromHandle(m_hBindEntity);
	if(pEntity && m_iBindPropID) pEntity->setProperty(m_iBindPropID, iValue);
}

	
void TimeRangeKeeper::setValue(int iValue)
{
	IZoneTime* pZoneTime = getComponent<IZoneTime>("ZoneTime", IID_IZoneTime);
	assert(pZoneTime);

	if(getCheckDelegate()(pZoneTime->GetCurSecond() ) )
	{
		getExecuteDelegate()(pZoneTime->GetCurSecond() );
	}

	_setValue(iValue);
}

Uint32 TimeRangeKeeper::getTime()
{
	return m_keepData.dwLastChgTime;
}

void TimeRangeKeeper::fillData(ServerEngine::TimeResetValue& data)
{
	IZoneTime* pZoneTime = getComponent<IZoneTime>("ZoneTime", IID_IZoneTime);
	assert(pZoneTime);

	if(getCheckDelegate()(pZoneTime->GetCurSecond() ) )
	{
		getExecuteDelegate()(pZoneTime->GetCurSecond() );
	}

	data = m_keepData;
}

TimeRangeExecute TimeRangeKeeper::getExecuteDelegate()
{
	if(m_bCustomExe)
	{
		return m_execute;
	}

	return KeeperChgReset(*this);
}

void TimeRangeKeeper::setCustomExecute(TimeRangeExecute execute)
{
	m_bCustomExe = true;
	m_execute = execute;
}


TimeRangeCheck TimeRangeKeeper::getCheckDelegate()
{
	if(m_iCheckMode == en_TimeRange_Day)
	{
		return KeeperDayChgCheck(*this);	
	}
	else if(m_iCheckMode == en_TimeRange_Week)
	{
		return KeeperWeekChgCheck(*this);
	}

	assert(false);
	return KeeperDayChgCheck(*this);
}

ServerEngine::TimeResetValue& getDayResetValue(ServerEngine::TimeResetValue& value)
{
	IZoneTime* pZomeTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZomeTime);

	if(!pZomeTime->IsInSameDay(value.dwLastChgTime, pZomeTime->GetCurSecond() ) )
	{
		value.iValue = 0;
		value.dwLastChgTime = pZomeTime->GetCurSecond();
	}

	return value;
}

ServerEngine::TimeResetValue& getWeekResetValue(ServerEngine::TimeResetValue& value)
{
	IZoneTime* pZomeTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZomeTime);

	if(!pZomeTime->IsInSameChineseWeek(value.dwLastChgTime, pZomeTime->GetCurSecond() ) )
	{
		value.iValue = 0;
		value.dwLastChgTime = pZomeTime->GetCurSecond();
	}

	return value;
}

TimeRangeTimer::TimeRangeTimer(ITimeRangeTask* pTask, const string& strName):m_pJustKey(pTask)
{
	assert(pTask);

	m_prx = pTask->getEventHandle();
	m_strName = strName;
	
}

TimeRangeTimer::~TimeRangeTimer()
{
	ITimerComponent* pTimeAxis = getComponent<ITimerComponent>(COMPNAME_TimeAxis, IID_ITimerComponent);
	assert(pTimeAxis);

	pTimeAxis->killTimer(m_timeHandle);
}


void TimeRangeTimer::onTimer(int nEventId)
{
	//PROFILE_MONITOR("TimeRangeTimer::onTimer");
	ITimerComponent* pTimeAxis = getComponent<ITimerComponent>(COMPNAME_TimeAxis, IID_ITimerComponent);
	assert(pTimeAxis);

	IZoneTime* pZomeTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZomeTime);

	if(!m_prx.get() )
	{	
		FDLOG("TimeRangeValue")<<"invalid TimeRangeValue|"<<m_strName<<endl;
		pTimeAxis->killTimer(m_timeHandle);

		ITimeRangeValue* pTimeRangeValue = getComponent<ITimeRangeValue>(COMPNAME_TimeRangeValue, IID_ITimeRangeValue);
		assert(pTimeRangeValue);

		pTimeRangeValue->removeRangeValue(m_pJustKey);
		return;
	}

	ITimeRangeTask* pTask = static_cast<ITimeRangeTask*>(m_prx.get() );
	assert(pTask);

	TimeRangeCheck checkDelegate = pTask->getCheckDelegate();
	Uint32 dwCurSecond = pZomeTime->GetCurSecond();

	if(checkDelegate(dwCurSecond) )
	{
		TimeRangeExecute exeDelegate = pTask->getExecuteDelegate();
		exeDelegate(dwCurSecond);
	}
}


TimeRangeValue::TimeRangeValue()
{
}

TimeRangeValue::~TimeRangeValue()
{
}

bool TimeRangeValue::initlize(const PropertySet& propSet)
{
	return true;
}

void TimeRangeValue::addRangeValue(ITimeRangeTask* pTask, const string& strName)
{
	if(m_timeRangeDelegateList.find(pTask) != m_timeRangeDelegateList.end() )
	{
		return;
	}

	IZoneTime* pZomeTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZomeTime);

	ITimerComponent* pTimeAxis = getComponent<ITimerComponent>(COMPNAME_TimeAxis, IID_ITimerComponent);
	assert(pTimeAxis);

	TimeRangeCheck checkDelegate = pTask->getCheckDelegate();
	Uint32 dwCurSecond = pZomeTime->GetCurSecond();

	if(checkDelegate(dwCurSecond) )
	{
		TimeRangeExecute exeDelegate = pTask->getExecuteDelegate();
		exeDelegate(dwCurSecond);
	}

	TimeRangeTimer* pTmpTimer = new TimeRangeTimer(pTask, strName);
	assert(pTmpTimer);
	
	ITimerComponent::TimerHandle timeHandle = pTimeAxis->setTimer(pTmpTimer, 1, pTask->checkInteval(), "TimeRangeValue");
	pTmpTimer->setTimeHandle(timeHandle);
	
	m_timeRangeDelegateList[pTask] = pTmpTimer->getEventHandle();	
}

void TimeRangeValue::removeRangeValue(ITimeRangeTask* pTask)
{
	if(m_timeRangeDelegateList.find(pTask) == m_timeRangeDelegateList.end() )
	{
		return;
	}

	Detail::EventHandle::Proxy prx = m_timeRangeDelegateList[pTask];
	if(prx.get() )
	{
		TimeRangeTimer* pRangeTimer = static_cast<TimeRangeTimer*>(prx.get() );
		assert(pRangeTimer);

		delete pRangeTimer;
	}

	m_timeRangeDelegateList.erase(pTask);
}





