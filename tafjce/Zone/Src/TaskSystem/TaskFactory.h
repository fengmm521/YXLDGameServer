#ifndef _TASKFACTORY_H_
#define _TASKFACTORY_H_


class TaskOpenCondition_Base
{
public:
	virtual ~TaskOpenCondition_Base(){}
	virtual bool checkCondition(HEntity hEntity) const = 0;

	virtual TaskOpenCondition_Base* clone() = 0;
};

//自动接取
class TaskOpenCondition_Auto:public TaskOpenCondition_Base
{
public:
	virtual ~TaskOpenCondition_Auto(){}
	virtual bool checkCondition(HEntity hEntity) const;
	virtual TaskOpenCondition_Base* clone()
	{
		return new TaskOpenCondition_Auto;
	}
};

//完成前置任务
class TaskOpenCondition_FinishLastTask:public TaskOpenCondition_Base
{
public:
	TaskOpenCondition_FinishLastTask(int iTaskID):m_iLastTaskID(iTaskID){}
	virtual ~TaskOpenCondition_FinishLastTask(){}
	virtual bool checkCondition(HEntity hEntity) const;
	virtual TaskOpenCondition_Base* clone()
	{
		return new TaskOpenCondition_FinishLastTask(m_iLastTaskID);
	}
private:
	int m_iLastTaskID;
};

//达到一定等级
class TaskOpenCondition_AchieveLevel: public TaskOpenCondition_Base
{
public:
	TaskOpenCondition_AchieveLevel(int iLevel):m_iLevel(iLevel){}
	virtual ~TaskOpenCondition_AchieveLevel(){}
	virtual bool checkCondition(HEntity hEntity) const;
	
	virtual TaskOpenCondition_Base* clone()
	{
		return new TaskOpenCondition_AchieveLevel(m_iLevel);
	}
	
private:
	int m_iLevel;
};

//在指定时间内
class TaskOpenCondition_InTimes:public TaskOpenCondition_Base
{
public:
	TaskOpenCondition_InTimes(int min, int max): m_iMinSecond(min),m_iMaxSecond(max){}
	virtual ~TaskOpenCondition_InTimes(){}
	virtual bool checkCondition(HEntity hEntity) const;
	
	virtual TaskOpenCondition_Base* clone()
	{
		return new TaskOpenCondition_InTimes(m_iMinSecond,m_iMaxSecond);
	}
private: 
	int m_iMinSecond;         //时间段的下限
	int m_iMaxSecond;			//时间段的上限
};

//每日自动接取
class TaskOpenCondition_DayAuto:public TaskOpenCondition_Base
{
public:
	virtual ~TaskOpenCondition_DayAuto(){}
	virtual bool checkCondition(HEntity hEntity) const;
	
	virtual TaskOpenCondition_Base* clone()
	{
		return new TaskOpenCondition_DayAuto;
	}
};

//////////////////////////////////////////////////////////

class TaskFinishCondition_Base
{
public:
	TaskFinishCondition_Base(int igoal):m_igoal(igoal)
	{}
	virtual ~TaskFinishCondition_Base(){}
	virtual bool checkFinishCondition(HEntity hEntity, int iEventID, EventArgs& args)=0;
	virtual int getEventID() const = 0;

	virtual TaskFinishCondition_Base* clone() = 0;
	virtual bool checkFinish(HEntity hEntity) {return false;}
public:
	int m_igoal;
};

//1.完成达到次数次数
class TaskFinishTimesCondition : public TaskFinishCondition_Base
{
public:
	TaskFinishTimesCondition(int goal,int eventID,int iTaskId):
		TaskFinishCondition_Base(goal),
		m_iEventID(eventID),
		m_iTaskId(iTaskId)
		{}
	virtual ~TaskFinishTimesCondition(){}
	virtual bool checkFinishCondition(HEntity hEntity, int iEventID, EventArgs& args);
	virtual int getEventID() const;

	virtual TaskFinishCondition_Base* clone()
	{
		return new TaskFinishTimesCondition(m_igoal,m_iEventID,m_iTaskId);
	}
private:
	int m_iEventID;
	int m_iTaskId;
};

//2、完成副本章节
class TaskFinishDungeonCondition : public TaskFinishCondition_Base
{
public:
	TaskFinishDungeonCondition(int eventID,int iTaskId,int iDungonID, int goal):
		TaskFinishCondition_Base(goal),
		m_iEventID(eventID),
		m_iTaskId(iTaskId),
		m_iDungonID(iDungonID)
		{}
	virtual ~TaskFinishDungeonCondition(){}
	virtual bool checkFinishCondition(HEntity hEntity, int iEventID, EventArgs& args);
	virtual int getEventID() const;

	virtual TaskFinishCondition_Base* clone()
	{
		return new TaskFinishDungeonCondition(m_iEventID,m_iTaskId,m_iDungonID,m_igoal);
	}
	
	virtual bool checkFinish(HEntity hEntity);
private:
	int m_iEventID;
	int m_iTaskId;
	int m_iDungonID;
};

//3、完成副本关卡
class TaskFinishDungeonSceneCondition : public TaskFinishCondition_Base
{
public:
	TaskFinishDungeonSceneCondition(int eventID,int iTaskId,int iDungonID, int goal):
		TaskFinishCondition_Base(goal),
		m_iEventID(eventID),
		m_iTaskId(iTaskId),
		m_iDungonID(iDungonID)
		{}
	virtual ~TaskFinishDungeonSceneCondition(){}
	virtual bool checkFinishCondition(HEntity hEntity, int iEventID, EventArgs& args);
	virtual int getEventID() const;

	virtual TaskFinishCondition_Base* clone()
	{
		return new TaskFinishDungeonSceneCondition(m_iEventID,m_iTaskId,m_iDungonID,m_igoal);
	}

	virtual bool checkFinish(HEntity hEntity);
	
private:
	int m_iEventID;
	int m_iTaskId;
	int m_iDungonID;
};

//英雄等级
class TaskFinishHeroLevelCondition : public TaskFinishCondition_Base
{
public:
	TaskFinishHeroLevelCondition(int eventID,int goal,int iTaskId)
		:TaskFinishCondition_Base(goal),
		m_iEventID(eventID),
		m_iTaskId(iTaskId)
		{}
	virtual ~TaskFinishHeroLevelCondition(){}
	virtual  bool checkFinishCondition(HEntity hEntity,int iEventID,EventArgs & args);
	virtual int getEventID()const;

	virtual TaskFinishCondition_Base* clone()
		{
			return new TaskFinishHeroLevelCondition(m_iEventID,m_igoal,m_iTaskId);
		}
	virtual bool checkFinish(HEntity hEntity);
private:
	int m_iEventID;
	int m_iTaskId;
};

//玩家等级
class TaskFinishPlayerLevelCondition : public TaskFinishCondition_Base
{
public:
	TaskFinishPlayerLevelCondition(int eventID,int goal,int iTaskId)
		:TaskFinishCondition_Base(goal),
		m_iEventID(eventID),
		m_iTaskId(iTaskId)
		{}
	virtual ~TaskFinishPlayerLevelCondition(){}
	virtual  bool checkFinishCondition(HEntity hEntity,int iEventID,EventArgs & args);
	virtual int getEventID()const;

	virtual TaskFinishCondition_Base* clone()
		{
			return new TaskFinishPlayerLevelCondition(m_iEventID,m_igoal,m_iTaskId);
		}
	virtual bool checkFinish(HEntity hEntity);;
private:
	int m_iEventID;
	int m_iTaskId;
};

//神兽等级
class TaskFinishGodAnimalLevelCondition : public TaskFinishCondition_Base
{
public:
	TaskFinishGodAnimalLevelCondition(int eventID,int goal,int iTaskId)
		:TaskFinishCondition_Base(goal),
		m_iEventID(eventID),
		m_iTaskId(iTaskId)
		{}
	virtual ~TaskFinishGodAnimalLevelCondition(){}
	virtual bool  checkFinishCondition(HEntity hEntity,int iEventID,EventArgs & args);
	virtual int getEventID()const;

	virtual TaskFinishCondition_Base* clone()
		{
			return new TaskFinishGodAnimalLevelCondition(m_iEventID,m_igoal,m_iTaskId);
		}
	virtual bool checkFinish(HEntity hEntity);;
private:
	int m_iEventID;
	int m_iTaskId;
};

class TaskFinishClimbTowerCondition : public TaskFinishCondition_Base
{
public:
	TaskFinishClimbTowerCondition(int eventID,int goal,int iTaskId)
		:TaskFinishCondition_Base(goal),
		m_iEventID(eventID),
		m_iTaskId(iTaskId)
		{}
	virtual ~TaskFinishClimbTowerCondition(){}
	virtual  bool checkFinishCondition(HEntity hEntity,int iEventID,EventArgs & args);
	virtual int getEventID()const;

	virtual TaskFinishCondition_Base* clone()
		{
			return new TaskFinishClimbTowerCondition(m_iEventID,m_igoal,m_iTaskId);
		}
	virtual bool checkFinish(HEntity hEntity);
private:
	int m_iEventID;
	int m_iTaskId;
};

//铜矿修为矿等级
class TaskFinishSilverResLevelCondition : public TaskFinishCondition_Base
{
public:
	TaskFinishSilverResLevelCondition(int eventID, int iLevel ,int goal,int iTaskId)
		:TaskFinishCondition_Base(goal),
		m_iEventID(eventID),
		m_iTaskId(iTaskId),
		m_iResLevel(iLevel)
		{}
	virtual ~TaskFinishSilverResLevelCondition(){}
	virtual  bool checkFinishCondition(HEntity hEntity,int iEventID,EventArgs & args);
	virtual int getEventID()const;

	virtual TaskFinishCondition_Base* clone()
		{
			return new TaskFinishSilverResLevelCondition(m_iEventID,m_iResLevel,m_igoal,m_iTaskId);
		}
	virtual bool checkFinish(HEntity hEntity);
private:
	int m_iEventID;
	int m_iTaskId;
	int m_iResLevel;
};

class TaskFinishHeroExpResLevelCondition : public TaskFinishCondition_Base
{
public:
	TaskFinishHeroExpResLevelCondition(int eventID,int iLevel,int goal,int iTaskId)
		:TaskFinishCondition_Base(goal),
		m_iEventID(eventID),
		m_iTaskId(iTaskId),
		 m_iResLevel(iLevel)
		{}
	virtual ~TaskFinishHeroExpResLevelCondition(){}
	virtual  bool checkFinishCondition(HEntity hEntity,int iEventID,EventArgs & args);
	virtual int getEventID()const;

	virtual TaskFinishCondition_Base* clone()
		{
			return new TaskFinishHeroExpResLevelCondition(m_iEventID,m_iResLevel , m_igoal, m_iTaskId);
		}
	virtual bool checkFinish(HEntity hEntity);
private:
	int m_iEventID;
	int m_iTaskId;
	int m_iResLevel;
};

//武魂殿铁匠铺任务

class TaskFinishHeroSoulLevelCondition : public TaskFinishCondition_Base
{
public:
	TaskFinishHeroSoulLevelCondition(int eventID,int goal,int iTaskId)
		:TaskFinishCondition_Base(goal),
		m_iEventID(eventID),
		m_iTaskId(iTaskId)
		{}
	virtual ~TaskFinishHeroSoulLevelCondition(){}
	virtual  bool checkFinishCondition(HEntity hEntity,int iEventID,EventArgs & args);
	virtual int getEventID()const;

	virtual TaskFinishCondition_Base* clone()
		{
			return new TaskFinishHeroSoulLevelCondition(m_iEventID,m_igoal,m_iTaskId);
		}
	virtual bool checkFinish(HEntity hEntity );
private:
	int m_iEventID;
	int m_iTaskId;
};

class TaskFinishTieJiangPuLevelCondition : public TaskFinishCondition_Base
{
public:
	TaskFinishTieJiangPuLevelCondition(int eventID,int goal,int iTaskId)
		:TaskFinishCondition_Base(goal),
		m_iEventID(eventID),
		m_iTaskId(iTaskId)
		{}
	virtual ~TaskFinishTieJiangPuLevelCondition(){}
	virtual  bool checkFinishCondition(HEntity hEntity,int iEventID,EventArgs & args);
	virtual int getEventID()const;

	virtual TaskFinishCondition_Base* clone()
		{
			return new TaskFinishTieJiangPuLevelCondition(m_iEventID,m_igoal,m_iTaskId);
		}
	virtual bool checkFinish(HEntity hEntity);
private:
	int m_iEventID;
	int m_iTaskId;
};

class TaskFinishHeroQualityCondition : public TaskFinishCondition_Base
{
public:
	TaskFinishHeroQualityCondition(int eventID,int goal, int iQuality, int iTaskId)
		:TaskFinishCondition_Base(goal),
		m_iEventID(eventID),
		m_iTaskId(iTaskId),
		m_iQuality(iQuality)
		{}
	virtual ~TaskFinishHeroQualityCondition(){}
	virtual  bool checkFinishCondition(HEntity hEntity,int iEventID,EventArgs & args);
	virtual int getEventID()const;

	virtual TaskFinishCondition_Base* clone()
		{
			return new TaskFinishHeroQualityCondition(m_iEventID, m_igoal, m_iQuality, m_iTaskId);
		}
	virtual bool checkFinish(HEntity hEntity );
private:
	int m_iEventID;
	int m_iTaskId;
	int m_iQuality;
};

class TaskFinishFormationCondition : public TaskFinishCondition_Base
{
public:
	TaskFinishFormationCondition(int eventID,int goal, int iTaskId)
		:TaskFinishCondition_Base(goal),
		m_iEventID(eventID),
		m_iTaskId(iTaskId)
		{}
	virtual ~TaskFinishFormationCondition(){}
	virtual  bool checkFinishCondition(HEntity hEntity,int iEventID,EventArgs & args);
	virtual int getEventID()const;

	virtual TaskFinishCondition_Base* clone()
		{
			return new TaskFinishFormationCondition(m_iEventID, m_igoal, m_iTaskId);
		}
	virtual bool checkFinish(HEntity hEntity );
private:
	int m_iEventID;
	int m_iTaskId;
};




////////////////
class TaskReward_Base
{
public:
	virtual ~TaskReward_Base(){}
	virtual bool taskReward(HEntity hEntity) = 0;

	virtual TaskReward_Base* clone() = 0;
};

class TaskReward_LifeAtt : public TaskReward_Base
{
public:
	TaskReward_LifeAtt(int lifeattId,int count)
		:m_ilifeattID(lifeattId),
		m_iCount(count)
		{
		}
	virtual bool taskReward(HEntity hEntity);

	virtual TaskReward_Base* clone()
	{
		return new TaskReward_LifeAtt(m_ilifeattID,m_iCount);
	}
private:
	int m_ilifeattID;
	int m_iCount;
};

class TaskReward_Item : public TaskReward_Base
{
public:
	TaskReward_Item(int itemId,int count)
	:m_iItemId(itemId),
	m_iCount(count)
	{
		
	}
	virtual bool taskReward(HEntity hEntity);

	virtual TaskReward_Base* clone()
	{
		return new TaskReward_Item(m_iItemId,m_iCount);
	}
	
private:
	int m_iItemId;
	int m_iCount;
};

class TaskReward_Hero : public TaskReward_Base
{
public:
	TaskReward_Hero(int iHeroId,int iStepLevel)
		:m_iHeroId(iHeroId),
		m_iStepLevel(iStepLevel)
	{
	}
	virtual bool taskReward(HEntity hEntity);

	virtual TaskReward_Base* clone()
	{
		return new TaskReward_Hero(m_iHeroId,m_iStepLevel);
	}
	
private:
	int m_iHeroId;
	int m_iStepLevel;
};

enum TaskResetType
{
	en_TaskResetType_None = 0,
	en_TaskResetType_NeverReset = 1,
	en_TaskResetType_DayReset = 2,
	en_TaskResetType_WeekReset = 3,
	en_TaskResetType_MonthReset = 4,
};


class TaskUnit
{
public:
	TaskUnit()
	{
	 	iTaskId = 0;
		iTaskTypeId = 0;
	    taskResetType = en_TaskResetType_None;
		icanFinishTimes = 0;
		m_OpenConditionVec.clear();
	 	m_FinishConditionVec.clear();
		m_RewardVec.clear();
		iGoal = 0;
	}

	TaskUnit(const TaskUnit& unit)
    {
    	iTaskId = unit.iTaskId;
		iTaskTypeId =  unit.iTaskTypeId;
	    taskResetType = unit.taskResetType;
		icanFinishTimes = unit.icanFinishTimes;
		iGoal = unit.iGoal;

		for(size_t i = 0; i < unit.m_OpenConditionVec.size();++i)
		{
			m_OpenConditionVec.push_back( unit.m_OpenConditionVec[i]->clone() );
		}

		for(size_t i = 0; i < unit.m_FinishConditionVec.size();++i)
		{
			m_FinishConditionVec.push_back( unit.m_FinishConditionVec[i]->clone() );
		}
		
		for(size_t i = 0; i < unit.m_RewardVec.size();++i)
		{
			m_RewardVec.push_back( unit.m_RewardVec[i]->clone() );
		}
    }
	 
	virtual ~TaskUnit()
	{
		for(size_t i = 0; i < m_OpenConditionVec.size();++i)
		{
			delete m_OpenConditionVec[i];
		}
		m_OpenConditionVec.clear();
		
		for(size_t i = 0; i < m_FinishConditionVec.size();++i)
		{
			delete m_FinishConditionVec[i];
		}
		 m_FinishConditionVec.clear();
		
		for(size_t i = 0; i < m_RewardVec.size();++i)
		{
			delete m_RewardVec[i];
		}
		m_RewardVec.clear();
	}
	
	bool checkTaskOpen(HEntity hEntity) const;
	bool checkTaskFinish(HEntity hEntity,int iEventID, EventArgs& args) ;
	bool checkTaskReward(HEntity hEntity);
	bool checkTaskFinish(HEntity hEntity);

	inline bool operator==(const TaskUnit& unit)
	{
		return unit.iTaskId == iTaskId;
	}
	
	inline TaskUnit& operator=(const TaskUnit& unit)
	{
		iTaskId = unit.iTaskId;
		iTaskTypeId =  unit.iTaskTypeId;
	    taskResetType = unit.taskResetType;
		icanFinishTimes = unit.icanFinishTimes;
		iGoal = unit.iGoal;

		for(size_t i = 0; i < unit.m_OpenConditionVec.size();++i)
		{
			m_OpenConditionVec.push_back( unit.m_OpenConditionVec[i]->clone() );
		}

		for(size_t i = 0; i < unit.m_FinishConditionVec.size();++i)
		{
			m_FinishConditionVec.push_back( unit.m_FinishConditionVec[i]->clone() );
		}

		for(size_t i = 0; i < unit.m_RewardVec.size();++i)
		{
			m_RewardVec.push_back( unit.m_RewardVec[i]->clone() );
		}
		
		return *this;
	}
	
public:
	int iTaskId;
	int iTaskTypeId;
	TaskResetType taskResetType;
	int icanFinishTimes;
	int iGoal;
	TaskFinishCondition_Base*  m_FinishCondition;
	vector<TaskOpenCondition_Base*> m_OpenConditionVec;
	vector<TaskFinishCondition_Base*> m_FinishConditionVec;
	//发奖
	vector<TaskReward_Base*> m_RewardVec;
};


class TaskFactory:public ComponentBase<ITaskFactory, IID_ITaskFactory>
{
public:
	TaskFactory();
	virtual ~TaskFactory();
public: 
	virtual bool initlize(const PropertySet& propSet);
	virtual void getNewOpenTaskList(HEntity hEntity,vector<TaskUnit>& taskUnitVec);
	virtual bool getTaskReward(HEntity hEntity,int iTaskId);
	void getFinishEventVec(vector<int>& eventVec);
	virtual bool checkFinish(HEntity hEntity,int eventID,EventArgs& args);
	TaskUnit getTaskUnitByTaskId(int iTaskId);
	void checkFinish(HEntity hEntity);

protected:
	TaskOpenCondition_Base*  ParseOpenCondition(const string conditonKey );
	TaskReward_Base* ParseReward(const string rewardStr);
	TaskFinishCondition_Base* ParseFinishCondition(const string conditionKey ,int taskId);
	TaskResetType getTaskResetType(const string strRest);

	TaskFinishCondition_Base* addFinishTimesTaskEvent(int iTaskId, int eventId, int iGoal);
	
private:
	map<int, TaskUnit> m_TaskMap;
	map<int,vector<int> > m_FinishEventWithTaskMap;    //事件 ==>任务列表 
	set<int>   m_FinishEventSet;
};
#endif
