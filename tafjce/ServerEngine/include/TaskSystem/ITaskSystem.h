#ifndef _ITASKSYSTEM_H_
#define _ITASKSYSTEM_H_

class ITaskSystem:public IEntitySubsystem
{
public:
	virtual bool getTaskHaveFinish(int iTaskId) = 0;
	virtual bool getTaskHaveReward(int iTaskId) = 0;
	virtual int getFinishProcess(int taskId) = 0;
	virtual void setFinishState(int taskId, int finishState) = 0;
	virtual void chgFinishProcess(int taskId,int process) = 0;
	virtual bool getTaskDBUnit(ServerEngine::TaskDBUnit& unit,int taskID) = 0; 
	virtual bool fillTaskDetail(int iTaskId, GSProto::TaskDetail& detail) = 0;
	virtual void PayMonthCard(int iCardCount) = 0;
};
#endif