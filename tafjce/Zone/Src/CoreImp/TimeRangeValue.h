#ifndef __TIMERANGE_VALUE_H__
#define __TIMERANGE_VALUE_H__

struct TimeRangeValueHold
{
	Detail::EventHandle::Proxy prx;
	string strName;
};

class TimeRangeValue;
class TimeRangeTimer:public ITimerCallback, public Detail::EventHandle
{
public:

	TimeRangeTimer(ITimeRangeTask* pTask, const string& strName);
	~TimeRangeTimer();

	void setTimeHandle(ITimerComponent::TimerHandle handle){m_timeHandle = handle;}
	virtual void onTimer(int nEventId);

private:

	ITimeRangeTask* m_pJustKey;

	Detail::EventHandle::Proxy m_prx;
	string m_strName;
	ITimerComponent::TimerHandle m_timeHandle;

	PROFILE_OBJ_COUNTER(TimeRangeTimer);
};

class TimeRangeValue:public ComponentBase<ITimeRangeValue, IID_ITimeRangeValue>
{
public:

	friend class TimeRangeTimer;

	TimeRangeValue();
	~TimeRangeValue();

	// IComponent Interface
	virtual bool initlize(const PropertySet& propSet);

	// ITimeRangeValue Interface
	virtual void addRangeValue(ITimeRangeTask* pTask, const string& strName);
	virtual void removeRangeValue(ITimeRangeTask* pTask);

private:

	map<ITimeRangeTask*, Detail::EventHandle::Proxy> m_timeRangeDelegateList;
};


#endif
