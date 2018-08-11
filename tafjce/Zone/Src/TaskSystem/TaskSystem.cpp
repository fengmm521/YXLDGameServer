#include "TaskSystemPch.h"
#include "TaskSystem.h"

extern "C" IObject* createTaskSystem()
{
	return new TaskSystem;
}

TaskSystem::TaskSystem()
	:m_hEntity(0)
{
}

TaskSystem::~TaskSystem()
{
	for(size_t i =0; i < m_taskEventHelperVec.size(); ++i )
	{
		delete m_taskEventHelperVec[i];
		m_taskEventHelperVec[i] = NULL;
	}
	m_taskEventHelperVec.clear();

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	if(pEntity)
	{
		pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_LEVELUP,this,&TaskSystem::checkTask);
	}
}

Uint32 TaskSystem::getSubsystemID() const
{
	return IID_ITaskSystem;
}

Uint32 TaskSystem::getMasterHandle()
{
	return m_hEntity;
}

bool TaskSystem::create(IEntity* pEntity, const std::string& strData)
{
	m_hEntity = pEntity->getHandle();

	if(strData.length()>0)
	{
		ServerEngine::JceToObj(strData,m_TaskSystemDbData); 
	}
	m_MonthCardData = m_TaskSystemDbData.monthCardData;
	
	RegisterTaskFinishEvent();

    checkSignIn();
	pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_LEVELUP,this,&TaskSystem::checkTask);
	
	return true;
}

bool TaskSystem::createComplete()
{
	
	return true;
}

void TaskEventHelper::onEventArgs(EventArgs& args)
{
	TaskFactory* pTaskFactory = static_cast<TaskFactory*>(getComponent<ITaskFactory>(COMPNAME_TaskFactory,IID_ITaskFactory));
	assert(pTaskFactory);

	pTaskFactory->checkFinish(m_hEntity,m_iEventID,args);
}

bool TaskSystem::getTaskDBUnit(ServerEngine::TaskDBUnit& unit,int taskID)
{
	map<taf::Int32, ServerEngine::TaskDBUnit>::iterator iter = m_TaskSystemDbData.dbTaskFinishMap.find(taskID);
	if(iter != m_TaskSystemDbData.dbTaskFinishMap.end())
	{
		unit = iter->second;
		return true;
	}
	return false;
}

void TaskSystem::RegisterTaskFinishEvent()
{
	TaskFactory* pTaskFactory = static_cast<TaskFactory*>(getComponent<ITaskFactory>(COMPNAME_TaskFactory,IID_ITaskFactory));
	assert(pTaskFactory);
	pTaskFactory->getFinishEventVec(m_eventVec);

	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	for(size_t i = 0; i < m_eventVec.size(); ++i )
	{
		TaskEventHelper *tempHelper = new TaskEventHelper(m_hEntity,m_eventVec[i]);
		m_taskEventHelperVec.push_back(tempHelper);
		pEntity->getEventServer()->subscribeEvent(m_eventVec[i], tempHelper, &TaskEventHelper::onEventArgs);
	}
}
void TaskSystem::chgFinishProcess(int taskId,int process)
{
	map<taf::Int32, ServerEngine::TaskDBUnit>::iterator iter = m_TaskSystemDbData.dbTaskFinishMap.find(taskId);
	assert(iter != m_TaskSystemDbData.dbTaskFinishMap.end());
	
	iter->second.iTaskProcess = process;
}

int TaskSystem::getFinishProcess(int taskId)
{
	map<taf::Int32, ServerEngine::TaskDBUnit>::iterator iter = m_TaskSystemDbData.dbTaskFinishMap.find(taskId);
	assert(iter != m_TaskSystemDbData.dbTaskFinishMap.end());
	return iter->second.iTaskProcess;
}

void TaskSystem::setFinishState(int taskId, int finishState)
{
	map<taf::Int32, ServerEngine::TaskDBUnit>::iterator iter = m_TaskSystemDbData.dbTaskFinishMap.find(taskId);
	assert(iter != m_TaskSystemDbData.dbTaskFinishMap.end());
	
	iter->second.enTaskState = finishState;
	if( iter->second.enTaskState == GSProto::en_Task_Finish)
	{
		iter->second.iHaveFinishedTimes += 1;

		IEntity *pEntity = getEntityFromHandle(m_hEntity);
		assert(pEntity);

		pEntity->sendMessage(GSProto::CMD_TASKSYSTEM_NEWTASKFINISH);

		pEntity->chgNotice(GSProto::en_NoticeGuid_TASK,true);

		EventArgs args;
		checkTask(args);
	}
}


const std::vector<Uint32>& TaskSystem::getSupportMessage()
{
	static vector<Uint32> msgVec;
	if(msgVec.size() == 0)
	{
		msgVec.push_back( GSProto::CMD_TASK_QUERY_TASKINFO);
		msgVec.push_back( GSProto::CMD_TASK_GET_REWARD);
		msgVec.push_back( GSProto::CMD_TASK_GET_VIGOR);
		msgVec.push_back( GSProto::CMD_GET_MONTHCARD_REWARD);
		msgVec.push_back( GSProto::CMD_CANBUY_MONTHCARD);
	}
	
	return msgVec;
	
}

void TaskSystem::onMessage(QxMessage* pMessage)
{
	assert(pMessage->dwMsgLen== sizeof(GSProto::CSMessage));
	const GSProto::CSMessage& msg = *(const GSProto::CSMessage*)(pMessage->pMsgDataBuff);

	switch(msg.icmd())
	{
		case GSProto::CMD_TASK_QUERY_TASKINFO:
		{
			onQueryTaskList(msg);
		}break;
		
		case GSProto::CMD_TASK_GET_REWARD:
		{
			onGetReward(msg);
		}break;

		case GSProto::CMD_TASK_GET_VIGOR:
		{
			getVigor(msg);
		}break;

		case GSProto::CMD_GET_MONTHCARD_REWARD:
		{
			onGetMonthReward(msg);
		}break;

		case GSProto::CMD_CANBUY_MONTHCARD:
		{
			onReqCanBuyMonthCard(msg);
		}break;
	}
}

void TaskSystem::onReqCanBuyMonthCard(const GSProto::CSMessage& msg)
{
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	IGlobalCfg* pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);
	
	int iMonthCardLimit = pGlobal->getInt("月卡剩余次数", 30);
	if(m_MonthCardData.iCanGetTimes >= iMonthCardLimit)
	{
		pEntity->sendErrorCode(ERROR_MONTH_CARD_REMAIND);
		return ;
	}

	pEntity->sendMessage(GSProto::CMD_CANBUY_MONTHCARD);
}

void TaskSystem::onGetMonthReward(const GSProto::CSMessage& msg)
{
	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	
	//今天是否已经领取
	if(pZoneTime->IsInSameDay(pZoneTime->GetCurSecond(),m_MonthCardData.lastChgTime))
	{
		return;
	}

	//还有么有剩余次数
	if(m_MonthCardData.iCanGetTimes <= 0)
	{
		return;
	}
	
	IGlobalCfg* pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);

	int iGetGold = pGlobal->getInt("月卡每日领取元宝", 120);
	
	m_MonthCardData.iCanGetTimes--;
	m_MonthCardData.lastChgTime = time(0);

	IEntity *pActor = getEntityFromHandle(m_hEntity);
	assert(pActor);

	pActor->changeProperty( PROP_ACTOR_GOLD,iGetGold,GSProto::en_Reason_MonthCard_GET);
	
	
	GSProto::CMD_TASK_QUERY_TASKINFO_SC s2cMsg;
	EventArgs args;
	checkTask(args);
		
	fillTaskListMsg(s2cMsg,  GSProto::enTaskType_DayHave);
	s2cMsg.set_bvigorhaveget(bVigorHaveGet());

	GSProto::MonthCardInfo & monthCardInfo = *(s2cMsg.mutable_monthcardinfo());
	monthCardInfo.set_bmonthcardhaveover(m_MonthCardData.iCanGetTimes <= 0);
	monthCardInfo.set_btodayhaveget(true);
	monthCardInfo.set_iremaindtimes(m_MonthCardData.iCanGetTimes);
	pActor->sendMessage(GSProto::CMD_TASK_QUERY_TASKINFO,s2cMsg);

}

void TaskSystem::getVigor(const GSProto::CSMessage& msg)
{
	
	IGlobalCfg* pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);

	string strTime = pGlobal->getString("领取精力时刻","43200#50400#64800#72000");

	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	
	vector<int> vec = TC_Common::sepstr<int>(strTime,"#");
	assert(vec.size()==4);
	assert(vec[0]<vec[1]);
	assert(vec[1]<vec[2]);
	assert(vec[2]<vec[3]);

	unsigned int dwNowSecond = pZoneTime->GetCurSecond();
	unsigned int dwDayBeginSecond = pZoneTime->GetDayBeginSecond(dwNowSecond );

	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	
	if( (dwDayBeginSecond + vec[0] <= dwNowSecond ) && (dwNowSecond <= dwDayBeginSecond + vec[1]) )
	{
		if( (dwDayBeginSecond + vec[0] <= m_TaskSystemDbData.vigorValue.dwLastChgTime) && ( m_TaskSystemDbData.vigorValue.dwLastChgTime <= dwDayBeginSecond + vec[1] ))
		{	
			return ;
		}
		m_TaskSystemDbData.vigorValue.dwLastChgTime = time(0);
		//加体力
		int iStrength = pGlobal->getInt("豪华午餐晚餐领取精力", 10);
		pEntity->changeProperty(PROP_ENTITY_PHYSTRENGTH,iStrength , GSProto::en_Reason_LUNCH_GETVIGOR);
		
	}
	else if( (dwDayBeginSecond + vec[2] <= dwNowSecond ) && (dwNowSecond <= dwDayBeginSecond + vec[3]))
	{
		if( (dwDayBeginSecond + vec[2]<= m_TaskSystemDbData.vigorValue.dwLastChgTime) && ( m_TaskSystemDbData.vigorValue.dwLastChgTime <= dwDayBeginSecond + vec[3] ))
		{
			return ;
		}
		m_TaskSystemDbData.vigorValue.dwLastChgTime = time(0);
		//加体力
		int iStrength = pGlobal->getInt("豪华午餐晚餐领取精力", 10);
		pEntity->changeProperty(PROP_ENTITY_PHYSTRENGTH,iStrength , GSProto::en_Reason_DINNER_GETVIGOR);
		
	}

	
	
	GSProto::CMD_TASK_QUERY_TASKINFO_SC s2cMsg;
	EventArgs args;
	checkTask(args);
	
	fillTaskListMsg(s2cMsg,  GSProto::enTaskType_DayHave);
	s2cMsg.set_bvigorhaveget(bVigorHaveGet());

	fillMonthCardInfo( *(s2cMsg.mutable_monthcardinfo()));
	
	pEntity->sendMessage(GSProto::CMD_TASK_QUERY_TASKINFO,s2cMsg);
	
}

bool TaskSystem::bVigorHaveGet()
{
	IGlobalCfg* pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);

	string strTime = pGlobal->getString("领取精力时刻","43200#50400#64800#72000");

	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	
	vector<int> vec = TC_Common::sepstr<int>(strTime,"#");
	assert(vec.size()==4);
	assert(vec[0]<vec[1]);
	assert(vec[1]<vec[2]);
	assert(vec[2]<vec[3]);

	unsigned int dwNowSecond = pZoneTime->GetCurSecond();
	unsigned int dwDayBeginSecond = pZoneTime->GetDayBeginSecond(dwNowSecond );

	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	if( (dwDayBeginSecond + vec[0] <= dwNowSecond ) && (dwNowSecond <= dwDayBeginSecond + vec[1]) )
	{
		if( (dwDayBeginSecond + vec[0]<= m_TaskSystemDbData.vigorValue.dwLastChgTime) && ( m_TaskSystemDbData.vigorValue.dwLastChgTime <= dwDayBeginSecond + vec[1] ))
		{
			return  true;
		}
	}
	else if( (dwDayBeginSecond + vec[2] <= dwNowSecond ) && (dwNowSecond <= dwDayBeginSecond + vec[3]))
	{
		if( (dwDayBeginSecond + vec[2]<= m_TaskSystemDbData.vigorValue.dwLastChgTime) && ( m_TaskSystemDbData.vigorValue.dwLastChgTime <= dwDayBeginSecond + vec[3] ))
		{
			return true;
		}
		
	}

	return false;
	
}


void TaskSystem::onQueryTaskList(const GSProto::CSMessage& msg)
{
	
	PROFILE_MONITOR("TaskSystem::onQueryTaskList");
	GSProto::CMD_TASK_QUERY_TASKINFO_CS csMsg;
	if(!csMsg.ParseFromString(msg.strmsgbody()))
	{
		return;
	}
	
	GSProto::enTaskType type = csMsg.type();
	
	GSProto::CMD_TASK_QUERY_TASKINFO_SC s2cMsg;
	EventArgs args;
	checkTask(args);
	
	fillTaskListMsg(s2cMsg,  type);
	s2cMsg.set_bvigorhaveget(bVigorHaveGet());
	fillMonthCardInfo( *(s2cMsg.mutable_monthcardinfo()));
	
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
    
	pEntity->sendMessage(GSProto::CMD_TASK_QUERY_TASKINFO,s2cMsg);
	
}

void TaskSystem::checkSignIn()
{
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
		
	TaskFactory* pTaskFactory = static_cast<TaskFactory*>(getComponent<ITaskFactory>(COMPNAME_TaskFactory,IID_ITaskFactory));
	assert(pTaskFactory);
	
	IZoneTime* pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime,IID_IZoneTime);
	assert(pZoneTime);
		
	vector<TaskUnit> openTaskVec;
	pTaskFactory->getNewOpenTaskList(m_hEntity,openTaskVec);
	
	for(size_t i = 0; i < openTaskVec.size(); ++i)
	{
		//已经领奖的任务不下发
		map<taf::Int32, ServerEngine::TaskDBUnit>::iterator iter = m_TaskSystemDbData.dbTaskFinishMap.find(openTaskVec[i].iTaskId);
		if(iter != m_TaskSystemDbData.dbTaskFinishMap.end())
		{
			if(checkResetTask(iter->second))
			{
				//需要重置的任务 不领奖 ，重置掉
				iter->second.iTaskProcess = 0;
				iter->second.enTaskState = GSProto::en_Task_OPen;
				iter->second.iHaveFinishedTimes = 0;
				iter->second.chgTime = pZoneTime->GetCurSecond();
				continue;
			}
			
			if(iter->second.enTaskState == GSProto::en_Task_HaveReward)
			{
				//处理多次完成的任务
				if(openTaskVec[i].icanFinishTimes > iter->second.iHaveFinishedTimes)
				{
					iter->second.enTaskState  = GSProto::en_Task_OPen;
					iter->second.iTaskProcess = 0;
					iter->second.chgTime = pZoneTime->GetCurSecond();
					
					continue;
				}		
				else
				{
					//已完成的不处理
					continue;
				}
			}
		}
		else
		{
			//新开的任务也是需要放入数据库的
			ServerEngine::TaskDBUnit unit;
			unit.iTaskId = openTaskVec[i].iTaskId;
			unit.iTaskProcess = 0;
			unit.enTaskState = GSProto::en_Task_OPen;
			unit.chgTime = pZoneTime->GetCurSecond();
			unit.iTaskType = openTaskVec[i].iTaskTypeId;
			unit.iHaveFinishedTimes = 0;
			
			m_TaskSystemDbData.dbTaskFinishMap.insert(make_pair(unit.iTaskId,unit));
		}
	}
	
	pTaskFactory->checkFinish(m_hEntity);

	//提示性引导
	checkTaskNotice();
	
}



void TaskSystem::checkTask(EventArgs& args)
{

	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
		
	TaskFactory* pTaskFactory = static_cast<TaskFactory*>(getComponent<ITaskFactory>(COMPNAME_TaskFactory,IID_ITaskFactory));
	assert(pTaskFactory);
	
	IZoneTime* pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime,IID_IZoneTime);
	assert(pZoneTime);
		
	vector<TaskUnit> openTaskVec;
	pTaskFactory->getNewOpenTaskList(m_hEntity,openTaskVec);
	bool bHaveNewTask = false;
	for(size_t i = 0; i < openTaskVec.size(); ++i)
	{
		//已经领奖的任务不下发
		map<taf::Int32, ServerEngine::TaskDBUnit>::iterator iter = m_TaskSystemDbData.dbTaskFinishMap.find(openTaskVec[i].iTaskId);
		if(iter != m_TaskSystemDbData.dbTaskFinishMap.end())
		{
			if(checkResetTask(iter->second))
			{
				//需要重置的任务 不领奖 ，重置掉
				iter->second.iTaskProcess = 0;
				iter->second.enTaskState = GSProto::en_Task_OPen;
				iter->second.iHaveFinishedTimes = 0;
				iter->second.chgTime = pZoneTime->GetCurSecond();
				continue;
			}
			
			if(iter->second.enTaskState == GSProto::en_Task_HaveReward)
			{
				//处理多次完成的任务
				if(openTaskVec[i].icanFinishTimes > iter->second.iHaveFinishedTimes)
				{
					iter->second.enTaskState  = GSProto::en_Task_OPen;
					iter->second.iTaskProcess = 0;
					iter->second.chgTime = pZoneTime->GetCurSecond();
					
					continue;
				}		
				else
				{
					//已完成的不处理
					continue;
				}
			}
		}
		else
		{
			//新开的任务也是需要放入数据库的
			ServerEngine::TaskDBUnit unit;
			unit.iTaskId = openTaskVec[i].iTaskId;
			unit.iTaskProcess = 0;
			unit.enTaskState = GSProto::en_Task_OPen;
			unit.chgTime = pZoneTime->GetCurSecond();
			unit.iTaskType = openTaskVec[i].iTaskTypeId;
			unit.iHaveFinishedTimes = 0;
			
			m_TaskSystemDbData.dbTaskFinishMap.insert(make_pair(unit.iTaskId,unit));
			
			bHaveNewTask = true;
			
		}
	}
	
	if(bHaveNewTask)
	{
		if(m_TaskSystemDbData.dbTaskFinishMap.size() > 1)
		{
			pEntity->sendMessage(GSProto::CMD_TASKSYSTEM_NEWTASKOPEN);
		}
		
	}
	
	pTaskFactory->checkFinish(m_hEntity);

	checkTaskNotice();

	
}

bool TaskSystem::vigorNeedNotice()
{
	IGlobalCfg* pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);

	string strTime = pGlobal->getString("领取精力时刻","43200#50400#64800#72000");

	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	
	vector<int> vec = TC_Common::sepstr<int>(strTime,"#");
	assert(vec.size()==4);
	assert(vec[0]<vec[1]);
	assert(vec[1]<vec[2]);
	assert(vec[2]<vec[3]);

	unsigned int dwNowSecond = pZoneTime->GetCurSecond();
	unsigned int dwDayBeginSecond = pZoneTime->GetDayBeginSecond(dwNowSecond );

	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	if( (dwDayBeginSecond + vec[0] <= dwNowSecond ) && (dwNowSecond <= dwDayBeginSecond + vec[1]) )
	{
		if( (dwDayBeginSecond + vec[0]<= m_TaskSystemDbData.vigorValue.dwLastChgTime) && ( m_TaskSystemDbData.vigorValue.dwLastChgTime <= dwDayBeginSecond + vec[1] ))
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	else if( (dwDayBeginSecond + vec[2] <= dwNowSecond ) && (dwNowSecond <= dwDayBeginSecond + vec[3]))
	{
		if( (dwDayBeginSecond + vec[2]<= m_TaskSystemDbData.vigorValue.dwLastChgTime) && ( m_TaskSystemDbData.vigorValue.dwLastChgTime <= dwDayBeginSecond + vec[3] ))
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	return false;
}



void TaskSystem::checkTaskNotice()
{
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	bool bHaveNotice = false;
	bool bNoticeLineTask = false;
	map<taf::Int32, ServerEngine::TaskDBUnit>::iterator iter = m_TaskSystemDbData.dbTaskFinishMap.begin();
	for(;iter != m_TaskSystemDbData.dbTaskFinishMap.end(); ++iter)
	{
		if(iter->second.enTaskState == GSProto::en_Task_Finish)
		{
			if(iter->second.iTaskType == GSProto::enTaskType_BranchLine ||iter->second.iTaskType == GSProto::enTaskType_MainLine)
			{
				//cout<<"task ID = |"<<iter->second.iTaskId<< " | bNoticeLineTask = true;"<<endl;
				bNoticeLineTask = true;
			}
			else
			{	
				//cout<<"task ID = |"<<iter->second.iTaskId<< "| bHaveNotice = true;"<<endl;
				bHaveNotice = true;
			}
		}	
	}

	if(vigorNeedNotice())
	{
		//cout<<"task ID = |"<<"vigorNeedNotice=true"<< "| bHaveNotice = true;"<<endl;
		bHaveNotice = true;
	}
	
	pEntity->chgNotice(GSProto::en_NoticeGuid_TASK,bHaveNotice);
	pEntity->chgNotice(GSProto::en_NoticeGuid_TASK_Line,bNoticeLineTask);
}


void TaskSystem::fillTaskListMsg(GSProto::CMD_TASK_QUERY_TASKINFO_SC& msg, GSProto::enTaskType type)
{
	TaskFactory* pTaskFactory = static_cast<TaskFactory*>(getComponent<ITaskFactory>(COMPNAME_TaskFactory,IID_ITaskFactory));
	assert(pTaskFactory);
	
	IZoneTime* pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime,IID_IZoneTime);
	assert(pZoneTime);
		
	vector<TaskUnit> openTaskVec;
	pTaskFactory->getNewOpenTaskList(m_hEntity,openTaskVec);
	msg.set_type(type);
	for(size_t i = 0; i < openTaskVec.size(); ++i)
	{
		const TaskUnit& taskUnit = openTaskVec[i];
  
		if(type == GSProto::enTaskType_MainLineAndBranchLine)
		{
			if(taskUnit.iTaskTypeId == GSProto::enTaskType_DayHave)
			{
				continue;
			}
		}
		else
		{
			if( ( taskUnit.iTaskTypeId == GSProto::enTaskType_MainLine) ||(taskUnit.iTaskTypeId == GSProto::enTaskType_BranchLine) )
			{
				continue;
			}
		}
		
	
		int iOver = 0;
		int iState = 0;
		
		//已经领奖的任务不下发
		map<taf::Int32, ServerEngine::TaskDBUnit>::iterator iter = m_TaskSystemDbData.dbTaskFinishMap.find(taskUnit.iTaskId);
		if(iter != m_TaskSystemDbData.dbTaskFinishMap.end())
		{
			if(checkResetTask(iter->second))
			{
			    //需要重置的任务 不领奖 ，重置掉
				iter->second.iTaskProcess = 0;
				iter->second.enTaskState = GSProto::en_Task_OPen;
				iter->second.iHaveFinishedTimes = 0;
				iter->second.chgTime = pZoneTime->GetCurSecond();

				//当然重置过后的 任务也是要下发的
				iOver =  iter->second.iTaskProcess;
				iState =  iter->second.enTaskState;
				continue;
			}
			
			if(iter->second.enTaskState == GSProto::en_Task_HaveReward)
			{
				//处理多次完成的任务
				if(taskUnit.icanFinishTimes > iter->second.iHaveFinishedTimes)
				{
					iter->second.enTaskState  = GSProto::en_Task_OPen;
					iter->second.iTaskProcess = 0;
					iter->second.chgTime = pZoneTime->GetCurSecond();

					iOver =  iter->second.iTaskProcess;
					iState =  iter->second.enTaskState;
					continue;
				}
				
				else
				{
					//已完成的不处理
					continue;
				}
			}
			else if((iter->second.enTaskState == GSProto::en_Task_OPen )|| (iter->second.enTaskState == GSProto::en_Task_Finish) ) 
			{
				//没有领奖的任务也是要下发的
				iOver =  iter->second.iTaskProcess;
				iState =  iter->second.enTaskState;
			}
		}
		else
		{
			//新开放的任务也需要下发的

			iOver =  0;
			iState = GSProto::en_Task_OPen;
			//新开的任务也是需要放入数据库的
			ServerEngine::TaskDBUnit unit;
			unit.iTaskId = taskUnit.iTaskId;
			unit.iTaskProcess = 0;
			unit.enTaskState = GSProto::en_Task_OPen;
			unit.chgTime = pZoneTime->GetCurSecond();
			unit.iTaskType = openTaskVec[i].iTaskTypeId;
			unit.iHaveFinishedTimes = 0;
			m_TaskSystemDbData.dbTaskFinishMap.insert(make_pair(unit.iTaskId,unit)); 
		}
		
		
		GSProto::TaskDetail& detail = *msg.add_sztaskdetail();
		detail.set_itaskid( taskUnit.iTaskId);
		detail.set_itasktype( taskUnit.iTaskTypeId);
		detail.set_itaskover( iOver);
		detail.set_itasktotal( taskUnit.iGoal);
		detail.set_itaskstate(iState );
		
	}

}

//活动使用
bool TaskSystem::fillTaskDetail(int iTaskId, GSProto::TaskDetail& detail)
{
	map<taf::Int32, ServerEngine::TaskDBUnit>::iterator iter = m_TaskSystemDbData.dbTaskFinishMap.find(iTaskId);
	if(iter != m_TaskSystemDbData.dbTaskFinishMap.end())
	{

		const ServerEngine::TaskDBUnit& dbUnit = iter->second; 
		
		TaskFactory *pTaskFactory = static_cast<TaskFactory*>( getComponent<ITaskFactory>(COMPNAME_TaskFactory,IID_ITaskFactory));
		assert(pTaskFactory);

		TaskUnit unit = pTaskFactory->getTaskUnitByTaskId( unit.iTaskId);
		detail.set_itaskid( dbUnit.iTaskId);
		detail.set_itasktype( dbUnit.iTaskType);
		detail.set_itaskover( dbUnit.iTaskProcess);
		detail.set_itasktotal( unit.iGoal);
		detail.set_itaskstate(dbUnit.enTaskState);
		return true;
	}
	return false;
}

void TaskSystem::onGetReward(const GSProto::CSMessage& msg)
{
	PROFILE_MONITOR("TaskSystem::onGetReward");
	GSProto::CMD_TASK_GET_REWARD_CS c2sMsg;
	if(	!c2sMsg.ParseFromString( msg.strmsgbody() ) )
	{
		return;
	}

	int iTaskId = c2sMsg.itaskid();
	//没有打开的任务不能领奖
	map<taf::Int32, ServerEngine::TaskDBUnit>::iterator iter = m_TaskSystemDbData.dbTaskFinishMap.find(iTaskId);
	if(iter == m_TaskSystemDbData.dbTaskFinishMap.end())
	{
		return;
	}

	//没有完成或者已经领奖了不能领奖
	if(iter->second.enTaskState != GSProto::en_Task_Finish)
	{
		return;
	}

	TaskFactory* pTaskFactory = static_cast<TaskFactory*>( getComponent<ITaskFactory>( COMPNAME_TaskFactory,IID_ITaskFactory));
	assert(pTaskFactory);

	//领奖失败
	bool res = pTaskFactory->getTaskReward(m_hEntity,iTaskId);
	if(!res)
	{
	 	return;
	}

	FDLOG("TaskSystem")<<"Have Reward|" << iTaskId<<endl;
	
	iter->second.enTaskState = GSProto::en_Task_HaveReward;
	
	EventArgs args;
	checkTask(args);

   
	
	//领奖完成下行客户端

	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	GSProto::CMD_TASK_QUERY_TASKINFO_SC s2cMsg;
	GSProto::enTaskType type = c2sMsg.type();
	fillTaskListMsg(s2cMsg,type);
	s2cMsg.set_bvigorhaveget(bVigorHaveGet());
	fillMonthCardInfo( *(s2cMsg.mutable_monthcardinfo()));
	
	pEntity->sendMessage(GSProto::CMD_TASK_QUERY_TASKINFO,s2cMsg);
	
	{
		EventArgs  args;
		args.context.setInt("entity",m_hEntity);
		args.context.setInt("taskid",iTaskId);

		pEntity->getEventServer()->setEvent(EVENT_ENTITY_TASK_HAVEREWARD, args);
	}
	
}

void TaskSystem::packSaveData(string& data)
{
	m_TaskSystemDbData.monthCardData = m_MonthCardData;
	data = ServerEngine::JceToStr(m_TaskSystemDbData);
}

bool TaskSystem::getTaskHaveFinish(int iTaskId)
{
 	map<taf::Int32, ServerEngine::TaskDBUnit>::iterator iter = m_TaskSystemDbData.dbTaskFinishMap.find(iTaskId);
	if(iter == m_TaskSystemDbData.dbTaskFinishMap.end())
	{
		return false;
	}
	
	if(iter->second.enTaskState == GSProto::en_Task_Finish) 
	{
		return true;
	}
	return false;
}

bool TaskSystem::needResetTask(TaskUnit unit)
{

	if( unit.taskResetType == en_TaskResetType_NeverReset)
	{
		return false;
	}
	if( unit.taskResetType == en_TaskResetType_DayReset)
	{
		return true;
	}
	if( unit.taskResetType == en_TaskResetType_WeekReset)
	{
		return true;
	}
	if( unit.taskResetType == en_TaskResetType_MonthReset)
	{
		return true;
	}
	return false;
}

bool TaskSystem::checkResetTask(ServerEngine::TaskDBUnit& dbUnit)
{

	TaskFactory *pTaskFactory = static_cast<TaskFactory*>( getComponent<ITaskFactory>(COMPNAME_TaskFactory,IID_ITaskFactory));
	assert(pTaskFactory);

	TaskUnit unit = pTaskFactory->getTaskUnitByTaskId(dbUnit.iTaskId);

	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime,IID_IZoneTime);
	assert(pZoneTime);

	if( unit.taskResetType == en_TaskResetType_NeverReset)
	{
		return false;
	}
	if( unit.taskResetType == en_TaskResetType_DayReset)
	{
		if(!pZoneTime->IsInSameDay(dbUnit.chgTime,pZoneTime->GetCurSecond()))
		{
			return true;
		}
	}
	if( unit.taskResetType == en_TaskResetType_WeekReset)
	{
		if(!pZoneTime->IsInSameChineseWeek(dbUnit.chgTime,pZoneTime->GetCurSecond()))
		{
			return true;
		}
	}
	if( unit.taskResetType == en_TaskResetType_MonthReset)
	{
		if(!pZoneTime->IsInSameMonth(dbUnit.chgTime,pZoneTime->GetCurSecond()))
		{
			return true;
		}
		
	}
	return false;
}

bool  TaskSystem::getTaskHaveReward(int iTaskId)
{
	map<taf::Int32, ServerEngine::TaskDBUnit>::iterator iter = m_TaskSystemDbData.dbTaskFinishMap.find(iTaskId);
	if(iter == m_TaskSystemDbData.dbTaskFinishMap.end())
	{
		return false;
	}
	
	if(iter->second.enTaskState == GSProto::en_Task_HaveReward ) 
	{
		return true;
	}
	return false;
}


void TaskSystem::PayMonthCard(int iCardCount)
{
	int iAddDay = iCardCount*30;
	m_MonthCardData.iCanGetTimes += iAddDay;
}


void TaskSystem::fillMonthCardInfo(GSProto::MonthCardInfo& monthCardInfo)
{
	monthCardInfo.set_bmonthcardhaveover(m_MonthCardData.iCanGetTimes<=0);
	
	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	if(pZoneTime->IsInSameDay(pZoneTime->GetCurSecond(),m_MonthCardData.lastChgTime))
	{
		monthCardInfo.set_btodayhaveget( true);
	}
	else
	{
		monthCardInfo.set_btodayhaveget( false);
	}

	monthCardInfo.set_iremaindtimes(m_MonthCardData.iCanGetTimes);
	
	
}



