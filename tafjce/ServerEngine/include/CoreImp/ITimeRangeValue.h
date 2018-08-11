#ifndef __ITIMERANGEVALUE_H__
#define __ITIMERANGEVALUE_H__

#include "RoleData.h"


typedef TC_Functor<bool, TL::TLMaker<Uint32>::Result> TimeRangeCheck;
typedef TC_Functor<void, TL::TLMaker<Uint32>::Result> TimeRangeExecute;


class ITimeRangeTask:public Detail::EventHandle
{
public:

	virtual ~ITimeRangeTask();

	virtual TimeRangeExecute getExecuteDelegate() = 0;

	virtual TimeRangeCheck getCheckDelegate() = 0;

	virtual int checkInteval() = 0;
};

enum
{
	en_TimeRange_Day = 0,
	en_TimeRange_Week = 1,
};

class TimeRangeKeeper//:public ITimeRangeTask
{
public:

	friend class KeeperChgReset;

	TimeRangeKeeper();
	void initData(const ServerEngine::TimeResetValue& data, HEntity hEntity, int iPropID);
	int getValue();
	void setValue(int iValue);
	Uint32 getTime();
	void setCheckMode(int iMode);
	void setCustomExecute(TimeRangeExecute execute);
	void fillData(ServerEngine::TimeResetValue& data);
	virtual TimeRangeExecute getExecuteDelegate();
	virtual TimeRangeCheck getCheckDelegate();
	void _setValue(int iValue);
	
private:
	
	ServerEngine::TimeResetValue m_keepData;
	HEntity m_hBindEntity;
	int m_iBindPropID;
	bool m_bWeekCheck;
	int m_iCheckMode;
	bool m_bCustomExe;
	TimeRangeExecute m_execute;
};


ServerEngine::TimeResetValue& getDayResetValue(ServerEngine::TimeResetValue& value);
ServerEngine::TimeResetValue& getWeekResetValue(ServerEngine::TimeResetValue& value);


class ITimeRangeValue:public IComponent
{
public:
	
	virtual void addRangeValue(ITimeRangeTask* pTask, const string& strName) = 0;

	virtual void removeRangeValue(ITimeRangeTask* pTask) = 0;
};


#endif
