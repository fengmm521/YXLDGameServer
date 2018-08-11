#ifndef _OPERATEFACTORY_H_
#define _OPERATEFACTORY_H_

////////////////
class CheckInReward_Base
{
public:
	virtual ~CheckInReward_Base(){}
	virtual bool checkInReward(HEntity hEntity, int iDouble) = 0;

	virtual CheckInReward_Base* clone() = 0;
};

class CheckInReward_LifeAtt : public CheckInReward_Base
{
public:
	CheckInReward_LifeAtt(int lifeattId,int count)
		:m_ilifeattID(lifeattId),
		m_iCount(count)
		{
		}
	virtual bool checkInReward(HEntity hEntity, int iDouble);

	virtual CheckInReward_Base* clone()
	{
		return new CheckInReward_LifeAtt(m_ilifeattID,m_iCount);
	}
private:
	int m_ilifeattID;
	int m_iCount;
};

class CheckInReward_Item : public CheckInReward_Base
{
public:
	CheckInReward_Item(int itemId,int count)
	:m_iItemId(itemId),
	m_iCount(count)
	{
		
	}
	virtual bool checkInReward(HEntity hEntity, int iDouble);

	virtual CheckInReward_Base* clone()
	{
		return new CheckInReward_Item(m_iItemId,m_iCount);
	}
	
private:
	int m_iItemId;
	int m_iCount;
};

class CheckInReward_Hero : public CheckInReward_Base
{
public:
	CheckInReward_Hero(int iHeroId,int iStepLevel)
		:m_iHeroId(iHeroId),
		m_iStepLevel(iStepLevel)
	{
	}
	virtual bool checkInReward(HEntity hEntity, int iDouble);

	virtual CheckInReward_Base* clone()
	{
		return new CheckInReward_Hero(m_iHeroId,m_iStepLevel);
	}
	
private:
	int m_iHeroId;
	int m_iStepLevel;
};

class OperateFactory;
class CheckInUnit
{
	public:
		CheckInUnit()
			:
			iMonth(0),
			iTotalTimes(0),
			rewardBase(NULL)
		{
			iVipNeed = 0;
			bCanGetTwice = false;
		}

		friend OperateFactory;
		
		virtual ~CheckInUnit()
			{
				delete rewardBase;
			}

		CheckInUnit(const CheckInUnit& unit)
		{
		 	iMonth = unit.iMonth;
			iTotalTimes = unit.iTotalTimes;
			bCanGetTwice = unit.bCanGetTwice;
			iVipNeed = unit.iVipNeed;
			rewardBase = unit.rewardBase->clone();
		}

		virtual CheckInUnit& operator=(const CheckInUnit& unit)
		{
			iMonth = unit.iMonth;
			iTotalTimes = unit.iTotalTimes;
			bCanGetTwice = unit.bCanGetTwice;
			iVipNeed = unit.iVipNeed;
			rewardBase = unit.rewardBase->clone();
			return *this;
		}

		bool reward(HEntity hEntity, int iDouble)
		{
			return rewardBase->checkInReward(hEntity, iDouble);
		}
public:
		int iVipNeed;
		bool bCanGetTwice;
private:
	int iMonth;
	int iTotalTimes;
	CheckInReward_Base *rewardBase;
};

//运营活动

class ActiveOpenConditionBase
{
	public:	
		ActiveOpenConditionBase(int iNeed):m_iNeedCondition(iNeed)
		{
		
		}
		virtual ~ActiveOpenConditionBase(){};
		virtual bool  checkOpenCondition(HEntity hEntity) = 0;
		virtual ActiveOpenConditionBase* clone() = 0;
	public:
		int m_iNeedCondition;
};

class ActiveOpenWithVip : public ActiveOpenConditionBase
{
public:
	ActiveOpenWithVip(int iNeed):ActiveOpenConditionBase(iNeed)
	{
		
	}
public:
	virtual bool  checkOpenCondition(HEntity hEntity);
	virtual ActiveOpenConditionBase* clone()
		{
			return	new ActiveOpenWithVip(m_iNeedCondition);
		}

};

class ActiveOpenWithLevel: public ActiveOpenConditionBase
{
public:

	ActiveOpenWithLevel(int iNeed):ActiveOpenConditionBase(iNeed)
	{
		
	}
public:
	virtual bool  checkOpenCondition(HEntity hEntity);
	virtual ActiveOpenConditionBase* clone()
	{
		return new ActiveOpenWithLevel(m_iNeedCondition);
	}
	
};

/*
class OperateActive
{
public:
	int m_id;
	int m_iType;
	int m_iBeginSecond;
	int m_iEndSecond;
	ActiveOpenConditionBase* m_OpenCheckPoint;
	bool m_bNeedReset;
	vector<int> m_TaskVec;
	vector<int> m_OpenFuncVec;
	bool bBuyFlag;
	bool iPrice;
public:
	friend OperateFactory;
	OperateActive():m_id(0),
		m_iType(0),
		m_iBeginSecond(0),
		m_iEndSecond(0),
		m_OpenCheckPoint(NULL),
		m_bNeedReset(false),
		bBuyFlag(false),
		iPrice(0)
	{
		
	}

	virtual ~OperateActive()
		{
			delete m_OpenCheckPoint;
			m_OpenCheckPoint = NULL;
		}

	OperateActive(const OperateActive& unit)
	{
		m_id = unit.m_id;
	    m_iType = unit.m_iType;
		m_iBeginSecond = unit.m_iBeginSecond;
		m_iEndSecond = unit.m_iEndSecond;
		
		if( unit.m_OpenCheckPoint) 
			m_OpenCheckPoint = unit.m_OpenCheckPoint->clone();
		
	 	m_bNeedReset = unit.m_bNeedReset;
		m_TaskVec= unit.m_TaskVec;
		m_OpenFuncVec = unit.m_OpenFuncVec;
		bBuyFlag = unit.bBuyFlag;
		iPrice = unit.iPrice;
	}
	
	virtual OperateActive& operator=(const OperateActive& unit)
	{
		m_id = unit.m_id;
	    m_iType = unit.m_iType;
		m_iBeginSecond = unit.m_iBeginSecond;
		m_iEndSecond = unit.m_iEndSecond;
		
		if( unit.m_OpenCheckPoint) 
			m_OpenCheckPoint = unit.m_OpenCheckPoint->clone();
		
	 	m_bNeedReset = unit.m_bNeedReset;
		m_TaskVec= unit.m_TaskVec;
		m_OpenFuncVec = unit.m_OpenFuncVec;
		bBuyFlag = unit.bBuyFlag;
		iPrice = unit.iPrice;
		
		return *this;
	}

	

};
*/

class GrowUpPlan
{
public:
	int iId;
	int iGoal;
	int iCanGetGold;

	GrowUpPlan():iId(0),iGoal(0),iCanGetGold(0)
	{
			
	}

	GrowUpPlan(const GrowUpPlan& unit)
	{
		iId = unit.iId;
		iGoal = unit.iGoal;
		iCanGetGold = unit.iCanGetGold;
	}

	virtual GrowUpPlan& operator=(const GrowUpPlan& unit)
	{
		iId = unit.iId;
		iGoal = unit.iGoal;
		iCanGetGold = unit.iCanGetGold;
		return *this;
	}
	
};


class AccumulatePayment
{
public:
	int iId;
	int iGoal;
	int iDropId;

	AccumulatePayment():iId(0),iGoal(0),iDropId(0)
	{
			
	}

	AccumulatePayment(const AccumulatePayment& unit)
	{
		iId = unit.iId;
		iGoal = unit.iGoal;
		iDropId = unit.iDropId;
	}

	virtual AccumulatePayment& operator=(const AccumulatePayment& unit)
	{
		iId = unit.iId;
		iGoal = unit.iGoal;
		iDropId = unit.iDropId;
		
		return *this;
	}
	
};


class AccumlateLoginIn :public AccumulatePayment
{
	public:
		AccumlateLoginIn()
		{
		
		}

	AccumlateLoginIn(const AccumulatePayment& unit)
	{
		iId = unit.iId;
		iGoal = unit.iGoal;
		iDropId = unit.iDropId;
	}

	virtual AccumlateLoginIn& operator=(const AccumlateLoginIn& unit)
	{
		iId = unit.iId;
		iGoal = unit.iGoal;
		iDropId = unit.iDropId;
		
		return *this;
	}
		
};



// DayPayment 相关配置对象
class DayPaymentReward
{
public:
	int m_operateId;	// 活动id
	int m_rewardId;		// 奖励id
	int m_goal;			// 充值目标
	int m_dropId;		// 掉落id

	DayPaymentReward():
		m_operateId(0),
		m_rewardId(0),
		m_goal(0),
		m_dropId(0)
	{
	};

	DayPaymentReward(const DayPaymentReward& _reward)
	{
		this->m_operateId 	= _reward.m_operateId;
		this->m_rewardId 	= _reward.m_rewardId;
		this->m_goal 		= _reward.m_goal;
		this->m_dropId		= _reward.m_dropId;
	};

	virtual DayPaymentReward& operator = (const DayPaymentReward& _reward)
	{
		this->m_operateId 	= _reward.m_operateId;
		this->m_rewardId 	= _reward.m_rewardId;
		this->m_goal 		= _reward.m_goal;
		this->m_dropId		= _reward.m_dropId;
		return * this;
	}
};


class DayPayment
{
public:
	int 							m_operateId;	// 活动id
	Uint32 							m_startTime;	// 开始时间
	Uint32 							m_endTime;		// 结束时间
	vector<DayPaymentReward*>		m_rewardList;	// 奖励列表

	DayPayment():
	m_operateId(0),
	m_startTime(0),
	m_endTime(0),
	m_rewardList()
	{
	};

	DayPayment(const DayPayment& _dayPayment)
	{
		this->m_operateId 	= _dayPayment.m_operateId;
		this->m_startTime	= _dayPayment.m_startTime;
		this->m_endTime		= _dayPayment.m_endTime;
		this->m_rewardList	= _dayPayment.m_rewardList;
	};

	virtual DayPayment& operator = ( const DayPayment& _dayPayment)
	{
		this->m_operateId 	= _dayPayment.m_operateId;
		this->m_startTime	= _dayPayment.m_startTime;
		this->m_endTime		= _dayPayment.m_endTime;
		this->m_rewardList	= _dayPayment.m_rewardList;
		return * this;
	};

	/**
	 * @brief 通过reward id 查询对应的DayPaymentReward结构
	 * @param	_rewardId	[in] : reward id
	 * @return	对应配置, 未查询到，则返回NULL
	 */
	DayPaymentReward* getDayPaymentRewardByRewardId(int _rewardId)
	{
		for(size_t i = 0; i < this->m_rewardList.size(); i ++)
		{
			DayPaymentReward* reward = this->m_rewardList[i];
			if(reward->m_rewardId == _rewardId)
			{
				return reward;
			}
		}
		return NULL;
	}
};
// DayPayment 相关配置对象end


class OperateFactory:public ComponentBase<IOperateFactory, IID_IOperateFactory>, public ITimerCallback
{
public:
	OperateFactory();
	virtual ~OperateFactory();
	
public: 
	virtual bool initlize(const PropertySet& propSet);

	virtual bool checkInGetReward( int iMonth, int haveCheckInCount,  CheckInUnit& unit);

	virtual void onTimer(int nEventId);

	virtual unsigned int getOpenServerSecond();

	virtual vector<GrowUpPlan>& getGrowUpPlanList(){ return m_GrowUpPlanList;}
	virtual vector<AccumulatePayment>& getAccumulatePayMentList(){ return m_AccPayMentList;}
	virtual	bool getStreamCode(string strPhone, string& productCode);

	virtual vector<AccumlateLoginIn>& getAccumulateLoginList(){ return m_AccLoginInList;}

	virtual bool updateActiveDayPayment(	DayPayment** _outOldDayPayment = NULL,
													DayPayment** _outNewDayPayment = NULL);

	virtual DayPayment*	getActiveDayPayment();
	
protected:
	CheckInReward_Base* ParseReward(const string rewardStr);

	void loadCheckInData();

	void loadTimeData();

	void loadGrowUpPlanData();

	void loadAccumulatePayMentData();

	void loadPhoneMap();

	void loadAccumulateLoginInData();

	/**
	 * 加载日充值活动的配置数据
	 */
	void loadDayPaymentData();

	
private:

	map<int, map<int, CheckInUnit> > m_CheckInData;

	vector<GrowUpPlan> m_GrowUpPlanList;

    ITimerComponent::TimerHandle m_hInitTimeHandle;	

	ServerEngine::OpenServerSecond m_openServerSecond;

	vector<AccumulatePayment> m_AccPayMentList;

	map<string,string> m_phoneMap;

	vector<AccumlateLoginIn> m_AccLoginInList;

	vector<DayPaymentReward>	m_dayPaymentRewardList;
	map<int, DayPayment>		m_dayPaymentList;
	DayPayment*					m_activeDayPayment;
	
	
};
#endif
