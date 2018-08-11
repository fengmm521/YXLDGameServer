#ifndef _IOPERATEFACTORY_H_
#define _IOPERATEFACTORY_H_

class DayPayment;
class CheckInUnit;
class IOperateFactory
	:public IComponent
{
public:
	virtual bool checkInGetReward( int iMonth, int haveCheckInCount,  CheckInUnit& unit) = 0;

	/**
	 * 跟据时间，更新当前生效的日累计充值活动
	 */
	virtual bool updateActiveDayPayment(	DayPayment** _outOldDayPayment = NULL,
													DayPayment** _outNewDayPayment = NULL) = 0;


	/**
	 * 获取当前生效的日累计充值活动
	 */
	virtual DayPayment*	getActiveDayPayment() = 0;


	
};

#endif
