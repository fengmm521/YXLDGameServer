#ifndef TASKSYSTEM_H_
#define TASKSYSTEM_H_

#include "TaskFactory.h"


struct TaskEventHelper:public Detail::EventHandle
{
	TaskEventHelper(HEntity hEntity, int iEventID):m_hEntity(hEntity), m_iEventID(iEventID){}
	void onEventArgs(EventArgs& args);

	HEntity m_hEntity;
	int m_iEventID;
};

class TaskSystem:public ObjectBase<ITaskSystem>, public Detail::EventHandle
{
public:

	TaskSystem();
	virtual ~TaskSystem();

	// IEntitySubsystem interface
	virtual Uint32 getSubsystemID() const;
	virtual Uint32 getMasterHandle();
	virtual bool create(IEntity* pEntity, const std::string& strData);
	virtual bool createComplete();
	virtual const std::vector<Uint32>& getSupportMessage();
	virtual void onMessage(QxMessage* pMessage);
	virtual void packSaveData(string& data);
	virtual bool fillTaskDetail(int iTaskId, GSProto::TaskDetail& detail);

	virtual void PayMonthCard(int iCardCount) ;
public:
	void onQueryTaskList(const GSProto::CSMessage& msg);
	void onGetReward(const GSProto::CSMessage& msg);
	virtual bool getTaskHaveFinish(int iTaskId);
	virtual bool getTaskDBUnit(ServerEngine::TaskDBUnit& unit,int taskID); 
	virtual int getFinishProcess(int taskId);
	virtual void setFinishState(int taskId, int finishState);
	virtual void chgFinishProcess(int taskId,int process);
	virtual bool getTaskHaveReward(int iTaskId);
	void checkTask(EventArgs& args);
	void checkSignIn();
protected:
	void fillTaskListMsg(GSProto::CMD_TASK_QUERY_TASKINFO_SC& msg, GSProto::enTaskType type);
    void fillTaskDetail(GSProto::TaskDetail& detail);
	bool needResetTask(TaskUnit unit);
	void RegisterTaskFinishEvent();

	bool checkResetTask(ServerEngine::TaskDBUnit& unit);

	void getVigor(const GSProto::CSMessage& msg);

	bool bVigorHaveGet();
	void checkTaskNotice();
	bool vigorNeedNotice();

	void fillMonthCardInfo(GSProto::MonthCardInfo& monthCardInfo);

	void checkMonthCardReset();

	void onGetMonthReward(const GSProto::CSMessage& msg);

	void onReqCanBuyMonthCard(const GSProto::CSMessage& msg);
	
private:
	HEntity m_hEntity;
	ServerEngine::TaskSystemData m_TaskSystemDbData;
	vector<int> m_eventVec;
	vector<TaskEventHelper*> m_taskEventHelperVec;

	ServerEngine::MonthCardData m_MonthCardData;
};
#endif