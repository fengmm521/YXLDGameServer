#include "TaskSystemPch.h"
#include "TaskFactory.h"

extern "C" IComponent* createTaskFactory(Int32)
{
	return new TaskFactory;
}

extern int LifeAtt2Prop(int iLifeAtt);

bool TaskOpenCondition_Auto::checkCondition(HEntity hEntity) const
{
	return true;
}

bool TaskOpenCondition_FinishLastTask::checkCondition(HEntity hEntity) const
{
	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	ITaskSystem* pTaskSystem = static_cast<ITaskSystem*>(pEntity->querySubsystem(IID_ITaskSystem));
	assert(pTaskSystem);

	bool lastTaskFinish = pTaskSystem->getTaskHaveReward(m_iLastTaskID);
	if(lastTaskFinish) 
	{
		return true;
	}
	
	return false;
}

bool TaskOpenCondition_AchieveLevel::checkCondition(HEntity hEntity) const
{
	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	int iLevel = pEntity->getProperty(PROP_ENTITY_LEVEL,0);
	if(iLevel >= m_iLevel)
	{
		return true;
	}
	return false;
}

bool TaskOpenCondition_InTimes::checkCondition(HEntity hEntity) const
{
	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	
	int iLeftSecond = pZoneTime->GetCurDayLeftSeconds();
	int iCurSecond = ONE_DAY_SECONDS - iLeftSecond;
	
	if(iCurSecond >= m_iMinSecond && iCurSecond <= m_iMaxSecond)
	{
		return true;
	}
	
	return false;
}

bool TaskOpenCondition_DayAuto::checkCondition(HEntity hEntity) const
{
	
	return true;
}

////////////////////////////
bool TaskFinishTimesCondition::checkFinishCondition(HEntity hEntity, int iEventID, EventArgs& args) 
{
	if(iEventID != getEventID())
	{
		return false;
	}
	
	int times =  args.context.getInt("times",0);
	if(!times) return false;
	//检查任务是否已经打开
	IEntity *pEntity = getEntityFromHandle(hEntity);
	ITaskSystem* pTaskSys = static_cast<ITaskSystem*>(pEntity->querySubsystem(IID_ITaskSystem));
	ServerEngine::TaskDBUnit unit;
	bool res = pTaskSys->getTaskDBUnit(unit,m_iTaskId);
	if(!res)
	{
		return false;
	}
	else if( unit.enTaskState == GSProto::en_Task_Finish ||  unit.enTaskState == GSProto::en_Task_HaveReward)
	{
		return true;
	}
	else if( unit.enTaskState == GSProto::en_Task_OPen)
	{

		int iProcess = unit.iTaskProcess + times;
		if(iProcess > m_igoal)
		{
			iProcess = m_igoal;
		}
		
		pTaskSys->chgFinishProcess(m_iTaskId,iProcess);
		
		if(iProcess == m_igoal)
		{
			pTaskSys->setFinishState(m_iTaskId,GSProto::en_Task_Finish);
			return true;
		}
		
		
		return false;
	}
	
	return false;
}

int TaskFinishTimesCondition::getEventID()const
{
	return m_iEventID;
}

//完成副本为1 ，
bool TaskFinishDungeonCondition::checkFinishCondition(HEntity hEntity, int iEventID, EventArgs& args) 
{
	if(iEventID != getEventID())
	{
		return false;
	}
	
	int dungeonId = args.context.getInt("dungeonId",0);
	if(m_iDungonID != dungeonId) return false;
	
	//检查任务是否已经打开
	IEntity *pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);
	ITaskSystem* pTaskSys = static_cast<ITaskSystem*>(pEntity->querySubsystem(IID_ITaskSystem));
	ServerEngine::TaskDBUnit unit;
	bool res = pTaskSys->getTaskDBUnit(unit,m_iTaskId);
	if(!res)
	{
		return false;
	}
	else if( unit.enTaskState == GSProto::en_Task_Finish ||  unit.enTaskState == GSProto::en_Task_HaveReward)
	{
		return true;
	}
	else if( unit.enTaskState == GSProto::en_Task_OPen)
	{
		int iProcess = unit.iTaskProcess + 1;
		if(iProcess > m_igoal)
		{
			iProcess = m_igoal;
		}
		pTaskSys->chgFinishProcess(m_iTaskId,iProcess);
		if(iProcess >= m_igoal)
		{
			pTaskSys->setFinishState(m_iTaskId,GSProto::en_Task_Finish);
			return true;
		}
		return false;
	}
	
	return false;
}

int TaskFinishDungeonCondition::getEventID() const
{
	return m_iEventID;
}

bool TaskFinishDungeonCondition::checkFinish(HEntity hEntity)
{
	//检查任务是否已经打开
	IEntity *pEntity = getEntityFromHandle(hEntity);
	ITaskSystem* pTaskSys = static_cast<ITaskSystem*>(pEntity->querySubsystem(IID_ITaskSystem));
	ServerEngine::TaskDBUnit unit;
	bool res = pTaskSys->getTaskDBUnit(unit,m_iTaskId);
	if(!res)
	{
		return false;
	}
	else if( unit.enTaskState == GSProto::en_Task_Finish ||  unit.enTaskState == GSProto::en_Task_HaveReward)
	{
		return true;
	}
	else if( unit.enTaskState == GSProto::en_Task_OPen)
	{	
		IDungeonSystem* pDungeonSys = static_cast<IDungeonSystem*>( pEntity->querySubsystem(IID_IDungeonSystem));
		assert(pDungeonSys);

		bool bHavePass = pDungeonSys->getSectionHavePass(m_iDungonID);
		if(!bHavePass) return false;
		int iProcess = unit.iTaskProcess + 1;
		if(iProcess > m_igoal)
		{
			iProcess = m_igoal;
		}
		
		pTaskSys->chgFinishProcess(m_iTaskId,iProcess);
		if(iProcess >= m_igoal)
		{
			pTaskSys->setFinishState(m_iTaskId,GSProto::en_Task_Finish);
			return true;
		}
		
		return false;
	}
	
	return false;
}

///
bool TaskFinishDungeonSceneCondition::checkFinishCondition(HEntity hEntity, int iEventID, EventArgs& args) 
{
	if(iEventID != getEventID())
	{
		return false;
	}
	
	int dungeonId = args.context.getInt("sceneid",0);
	if(m_iDungonID != dungeonId) return false;
	
	//检查任务是否已经打开
	IEntity *pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);
	ITaskSystem* pTaskSys = static_cast<ITaskSystem*>(pEntity->querySubsystem(IID_ITaskSystem));
	ServerEngine::TaskDBUnit unit;
	bool res = pTaskSys->getTaskDBUnit(unit,m_iTaskId);
	if(!res)
	{
		return false;
	}
	else if( unit.enTaskState == GSProto::en_Task_Finish ||  unit.enTaskState == GSProto::en_Task_HaveReward)
	{
		return true;
	}
	else if( unit.enTaskState == GSProto::en_Task_OPen)
	{
		int iProcess = unit.iTaskProcess + 1;
		if(iProcess > m_igoal)
		{
			iProcess = m_igoal;
		}
		pTaskSys->chgFinishProcess(m_iTaskId,iProcess);
		if(iProcess >= m_igoal)
		{
			pTaskSys->setFinishState(m_iTaskId,GSProto::en_Task_Finish);
			return true;
		}
		return false;
	}
	
	return false;
}

int TaskFinishDungeonSceneCondition::getEventID() const
{
	return m_iEventID;
}

bool TaskFinishDungeonSceneCondition::checkFinish(HEntity hEntity)
{
	//检查任务是否已经打开
	IEntity *pEntity = getEntityFromHandle(hEntity);
	ITaskSystem* pTaskSys = static_cast<ITaskSystem*>(pEntity->querySubsystem(IID_ITaskSystem));
	ServerEngine::TaskDBUnit unit;
	bool res = pTaskSys->getTaskDBUnit(unit,m_iTaskId);
	if(!res)
	{
		return false;
	}
	else if( unit.enTaskState == GSProto::en_Task_Finish ||  unit.enTaskState == GSProto::en_Task_HaveReward)
	{
		return true;
	}
	else if( unit.enTaskState == GSProto::en_Task_OPen)
	{	
		IDungeonSystem* pDungeonSys = static_cast<IDungeonSystem*>( pEntity->querySubsystem(IID_IDungeonSystem));
		assert(pDungeonSys);

		bool bHavePass = pDungeonSys->getSeceneHavePass(m_iDungonID);
		if(!bHavePass) return false;
		
		int iProcess = unit.iTaskProcess + 1;
		if(iProcess > m_igoal)
		{
			iProcess = m_igoal;
		}
		pTaskSys->chgFinishProcess(m_iTaskId,iProcess);
		if(iProcess >= m_igoal)
		{
			pTaskSys->setFinishState(m_iTaskId,GSProto::en_Task_Finish);
			return true;
		}
		return false;
	}
	
	return false;
}


bool TaskFinishHeroLevelCondition::checkFinishCondition(HEntity hEntity, int iEventID, EventArgs& args)
{
	if(iEventID != getEventID())
	{
		return false;
	}
	return checkFinish(hEntity);
}

bool TaskFinishHeroLevelCondition::checkFinish(HEntity hEntity)
{
	//检查任务是否已经打开
	IEntity *pEntity = getEntityFromHandle(hEntity);
	ITaskSystem* pTaskSys = static_cast<ITaskSystem*>(pEntity->querySubsystem(IID_ITaskSystem));
	ServerEngine::TaskDBUnit unit;
	bool res = pTaskSys->getTaskDBUnit(unit,m_iTaskId);
	if(!res)
	{
		return false;
	}
	else if( unit.enTaskState == GSProto::en_Task_Finish ||  unit.enTaskState == GSProto::en_Task_HaveReward)
	{
		return true;
	}
	else if( unit.enTaskState == GSProto::en_Task_OPen)
	{	

		IHeroSystem* pHeroSys = static_cast<IHeroSystem*>(pEntity->querySubsystem(IID_IHeroSystem));
		assert(pHeroSys);

		vector<HEntity> hEntityVec = pHeroSys->getHeroList();
		int maxLevel = 0;
		for(size_t i = 0; i < hEntityVec.size(); ++i)
		{
			IEntity *pHeroEntity = getEntityFromHandle(hEntityVec[i]);
			int iLevel = pHeroEntity->getProperty(PROP_ENTITY_LEVEL, 0);
			maxLevel = std::max(maxLevel,iLevel);
		}
		
		int iProcess = maxLevel;
	   if(iProcess > m_igoal)
		{
			iProcess = m_igoal;
		}
		pTaskSys->chgFinishProcess(m_iTaskId,iProcess);
		if(iProcess >= m_igoal)
		{
			pTaskSys->setFinishState(m_iTaskId,GSProto::en_Task_Finish);
			return true;
		}
		return false;
	}
	return false;

}

int TaskFinishHeroLevelCondition::getEventID() const
{
	return m_iEventID;
}


bool TaskFinishPlayerLevelCondition::checkFinishCondition(HEntity hEntity, int iEventID, EventArgs& args) 
{
	if(iEventID != getEventID())
	{
		return false;
	}

	int playerLevel = args.context.getInt("newlv",0);
	if(!playerLevel) return false;
	
	//检查任务是否已经打开
	IEntity *pEntity = getEntityFromHandle(hEntity);
	ITaskSystem* pTaskSys = static_cast<ITaskSystem*>(pEntity->querySubsystem(IID_ITaskSystem));
	ServerEngine::TaskDBUnit unit;
	bool res = pTaskSys->getTaskDBUnit(unit,m_iTaskId);
	if(!res)
	{
		return false;
	}
	else if( unit.enTaskState == GSProto::en_Task_Finish ||  unit.enTaskState == GSProto::en_Task_HaveReward)
	{
		return true;
	}
	else if( unit.enTaskState == GSProto::en_Task_OPen)
	{	

		
		int iProcess =  playerLevel;
		if(iProcess > m_igoal)
		{
			iProcess = m_igoal;
		}
		pTaskSys->chgFinishProcess(m_iTaskId,iProcess);
		if(iProcess >= m_igoal)
		{
			pTaskSys->setFinishState(m_iTaskId,GSProto::en_Task_Finish);
			return true;
		}
		return false;
	}
	
	return false;
}

bool TaskFinishPlayerLevelCondition::checkFinish(HEntity hEntity)
{
	//检查任务是否已经打开
	IEntity *pEntity = getEntityFromHandle(hEntity);
	ITaskSystem* pTaskSys = static_cast<ITaskSystem*>(pEntity->querySubsystem(IID_ITaskSystem));
	ServerEngine::TaskDBUnit unit;
	bool res = pTaskSys->getTaskDBUnit(unit,m_iTaskId);
	if(!res)
	{
		return false;
	}
	else if( unit.enTaskState == GSProto::en_Task_Finish ||  unit.enTaskState == GSProto::en_Task_HaveReward)
	{
		return true;
	}
	else if( unit.enTaskState == GSProto::en_Task_OPen)
	{	
		int iProcess =  pEntity->getProperty(PROP_ENTITY_LEVEL,0);
		if(iProcess > m_igoal)
		{
			iProcess = m_igoal;
		}
		pTaskSys->chgFinishProcess(m_iTaskId,iProcess);
		if(iProcess >= m_igoal)
		{
			pTaskSys->setFinishState(m_iTaskId,GSProto::en_Task_Finish);
			return true;
		}
		return false;
	}
	return false;
}


int TaskFinishPlayerLevelCondition::getEventID() const
{
	return m_iEventID;
}


bool TaskFinishGodAnimalLevelCondition::checkFinishCondition(HEntity hEntity, int iEventID, EventArgs& args) 
{

	if(iEventID != getEventID())
	{
		return false;
	}

	return checkFinish(hEntity);
}

bool TaskFinishGodAnimalLevelCondition::checkFinish(HEntity hEntity)
{
	//检查任务是否已经打开
	IEntity *pEntity = getEntityFromHandle(hEntity);
	ITaskSystem* pTaskSys = static_cast<ITaskSystem*>(pEntity->querySubsystem(IID_ITaskSystem));
	ServerEngine::TaskDBUnit unit;
	bool res = pTaskSys->getTaskDBUnit(unit,m_iTaskId);
	if(!res)
	{
		return false;
	}
	else if( unit.enTaskState == GSProto::en_Task_Finish ||  unit.enTaskState == GSProto::en_Task_HaveReward)
	{
		return true;
	}
	else if( unit.enTaskState == GSProto::en_Task_OPen)
	{	

		IGodAnimalSystem* pGodAnimalSys = static_cast<IGodAnimalSystem*>(pEntity->querySubsystem(IID_IGodAnimalSystem));
		assert(pGodAnimalSys);

		vector<HEntity> hEntityVec = pGodAnimalSys->getGodAnimalList();
		int maxLevel = 0;
		for(size_t i = 0; i < hEntityVec.size(); ++i)
		{
			IEntity *pHeroEntity = getEntityFromHandle(hEntityVec[i]);
			int iLevel = pHeroEntity->getProperty(PROP_ENTITY_LEVEL, 0);
			maxLevel = std::max(maxLevel,iLevel);
		}

		int iProcess = maxLevel;
		if(iProcess > m_igoal)
		{
			iProcess = m_igoal;
		}
		pTaskSys->chgFinishProcess(m_iTaskId,iProcess);
		if(iProcess >= m_igoal)
		{
			pTaskSys->setFinishState(m_iTaskId,GSProto::en_Task_Finish);
			return true;
		}
		return false;
	}
	
	return false;
}


int TaskFinishGodAnimalLevelCondition::getEventID() const
{
	return m_iEventID;
}

bool TaskFinishClimbTowerCondition::checkFinishCondition(HEntity hEntity, int iEventID, EventArgs& args) 
{

	if(iEventID != getEventID())
	{
		return false;
	}

	return checkFinish(hEntity);
}

bool TaskFinishClimbTowerCondition::checkFinish(HEntity hEntity)
{
	//检查任务是否已经打开
	IEntity *pEntity = getEntityFromHandle(hEntity);
	ITaskSystem* pTaskSys = static_cast<ITaskSystem*>(pEntity->querySubsystem(IID_ITaskSystem));
	ServerEngine::TaskDBUnit unit;
	bool res = pTaskSys->getTaskDBUnit(unit,m_iTaskId);
	if(!res)
	{
		return false;
	}
	else if( unit.enTaskState == GSProto::en_Task_Finish ||  unit.enTaskState == GSProto::en_Task_HaveReward)
	{
		return true;
	}
	else if( unit.enTaskState == GSProto::en_Task_OPen)
	{	
		IClimbTowerSystem* pClimbTower = static_cast<IClimbTowerSystem*>(pEntity->querySubsystem(IID_IClimbTowerSystem));
		assert(pClimbTower);
		
		int iProcess = pClimbTower->getTowerTop();
		if(iProcess > m_igoal)
		{
			iProcess = m_igoal;
		}
		pTaskSys->chgFinishProcess(m_iTaskId,iProcess);
		if(iProcess >= m_igoal)
		{
			pTaskSys->setFinishState(m_iTaskId,GSProto::en_Task_Finish);
			return true;
		}
		return false;
	}
	
	return false;
}

int TaskFinishClimbTowerCondition::getEventID() const
{
	return m_iEventID;
}


bool TaskFinishSilverResLevelCondition:: checkFinishCondition(HEntity hEntity,int iEventID,EventArgs & args)
{
	if( iEventID != getEventID())
	{
		return false;
	}

	return checkFinish(hEntity);
}

int TaskFinishSilverResLevelCondition::getEventID()const
{
	return m_iEventID;
}

bool TaskFinishSilverResLevelCondition::checkFinish(HEntity hEntity)
{
	IEntity *pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);
	ITaskSystem* pTaskSys = static_cast<ITaskSystem*>(pEntity->querySubsystem(IID_ITaskSystem));
	ServerEngine::TaskDBUnit unit;
	bool res = pTaskSys->getTaskDBUnit(unit,m_iTaskId);
	if(!res)
	{
		return false;
	}
	else if( unit.enTaskState == GSProto::en_Task_Finish ||  unit.enTaskState == GSProto::en_Task_HaveReward)
	{
		return true;
	}
	else if( unit.enTaskState == GSProto::en_Task_OPen)
	{	
		IManorSystem* pManorSystem = static_cast<IManorSystem*>(pEntity->querySubsystem(IID_IManorSystem));
		assert(pManorSystem);
		vector<ServerEngine::ManorResData> silverResVec;
		pManorSystem->getActorSilverRes(silverResVec);
		int iLevelCount = 0;
		 for(size_t i = 0; i< silverResVec.size(); ++i)
		 {
		 	const ServerEngine::ManorResData& data = silverResVec[ i ];
			if(data.iResLevel >= m_iResLevel)
			{
				iLevelCount ++;
			}
		 }
			 
		int iProcess = iLevelCount;
		iProcess = std::min(iProcess, m_igoal);
		
		if( unit.iTaskProcess < iProcess)
		{
			pTaskSys->chgFinishProcess(m_iTaskId,iProcess);
		}
		if(iProcess >= m_igoal)
		{
			pTaskSys->setFinishState(m_iTaskId,GSProto::en_Task_Finish);
			return true;
		}
		return false;
	}
	
	return false;
}

bool TaskFinishHeroExpResLevelCondition:: checkFinishCondition(HEntity hEntity,int iEventID,EventArgs & args)
{
	if( iEventID != getEventID())
	{
		return false;
	}

	return checkFinish(hEntity);
}

int TaskFinishHeroExpResLevelCondition::getEventID()const
{
	return m_iEventID;
}

bool TaskFinishHeroExpResLevelCondition::checkFinish(HEntity hEntity)
{
	IEntity *pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);
	ITaskSystem* pTaskSys = static_cast<ITaskSystem*>(pEntity->querySubsystem(IID_ITaskSystem));
	ServerEngine::TaskDBUnit unit;
	bool res = pTaskSys->getTaskDBUnit(unit,m_iTaskId);
	if(!res)
	{
		return false;
	}
	else if( unit.enTaskState == GSProto::en_Task_Finish ||  unit.enTaskState == GSProto::en_Task_HaveReward)
	{
		return true;
	}
	else if( unit.enTaskState == GSProto::en_Task_OPen)
	{	
		IManorSystem* pManorSystem = static_cast<IManorSystem*>(pEntity->querySubsystem(IID_IManorSystem));
		assert(pManorSystem);
		
		vector<ServerEngine::ManorResData> heroExpResVec;
		pManorSystem->getActorHeroExpRes(heroExpResVec);

		int iLevelCount = 0;
		 for(size_t i = 0; i< heroExpResVec.size(); ++i)
		 {
		 	const ServerEngine::ManorResData& data = heroExpResVec[ i ];
			if(data.iResLevel >= m_iResLevel)
			{
				iLevelCount ++;
			}
		 }


		int iProcess = iLevelCount;
		iProcess = std::min(iProcess, m_igoal);
		
		if( unit.iTaskProcess < iProcess)
		{
			pTaskSys->chgFinishProcess(m_iTaskId,iProcess);
		}
		if(iProcess >= m_igoal)
		{
			pTaskSys->setFinishState(m_iTaskId,GSProto::en_Task_Finish);
			return true;
		}
		return false;
	}
	
	return false;
}

bool TaskFinishTieJiangPuLevelCondition::checkFinishCondition(HEntity hEntity,int iEventID,EventArgs & args)
{
	if( iEventID != getEventID())
	{
		return false;
	}

	return checkFinish(hEntity);
}

int TaskFinishTieJiangPuLevelCondition::getEventID()const
{
	return EVENT_ENTITY_TASK_TIEJIANGPU_LEVEL;
}
bool TaskFinishTieJiangPuLevelCondition::checkFinish(HEntity hEntity )
{
	IEntity *pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);
	ITaskSystem* pTaskSys = static_cast<ITaskSystem*>(pEntity->querySubsystem(IID_ITaskSystem));
	ServerEngine::TaskDBUnit unit;
	bool res = pTaskSys->getTaskDBUnit(unit,m_iTaskId);
	if(!res)
	{
		return false;
	}
	else if( unit.enTaskState == GSProto::en_Task_Finish ||  unit.enTaskState == GSProto::en_Task_HaveReward)
	{
		return true;
	}
	else if( unit.enTaskState == GSProto::en_Task_OPen)
	{	
		
		IManorSystem* pManorSys = static_cast<IManorSystem*>(pEntity->querySubsystem(IID_IManorSystem));
		assert(pManorSys);
		int iProcess = pManorSys->getTieJiangPuLevel();
		iProcess = std::min(m_igoal,iProcess);
		pTaskSys->chgFinishProcess(m_iTaskId,iProcess);
		if(iProcess == m_igoal)
		{
			pTaskSys->setFinishState(m_iTaskId,GSProto::en_Task_Finish);
			return true;
		}
		return false;
	}
	
	return false;
}
	
bool TaskFinishHeroSoulLevelCondition::checkFinishCondition(HEntity hEntity,int iEventID,EventArgs & args)
{
	if( iEventID != getEventID())
	{
		return false;
	}

	return checkFinish(hEntity );
}

int TaskFinishHeroSoulLevelCondition::getEventID()const
{
	return EVENT_ENTITY_TASK_HEROSOUL_LEVEL;
}
bool TaskFinishHeroSoulLevelCondition::checkFinish(HEntity hEntity )
{
	IEntity *pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);
	ITaskSystem* pTaskSys = static_cast<ITaskSystem*>(pEntity->querySubsystem(IID_ITaskSystem));
	ServerEngine::TaskDBUnit unit;
	bool res = pTaskSys->getTaskDBUnit(unit,m_iTaskId);
	if(!res)
	{
		return false;
	}
	else if( unit.enTaskState == GSProto::en_Task_Finish ||  unit.enTaskState == GSProto::en_Task_HaveReward)
	{
		return true;
	}
	else if( unit.enTaskState == GSProto::en_Task_OPen)
	{	
			
		IManorSystem* pManorSys = static_cast<IManorSystem*>(pEntity->querySubsystem(IID_IManorSystem));
		assert(pManorSys);
		int iProcess = pManorSys->getWuHunDianLevel();
		iProcess = std::min(m_igoal,iProcess);
		pTaskSys->chgFinishProcess(m_iTaskId,iProcess);
		if(iProcess >= m_igoal)
		{
			pTaskSys->setFinishState(m_iTaskId,GSProto::en_Task_Finish);
			return true;
		}
		return false;
	}
	
	return false;
}
	
bool TaskFinishHeroQualityCondition::checkFinishCondition(HEntity hEntity,int iEventID,EventArgs & args)
{
	if( iEventID != getEventID())
	{
		return false;
	}

	return checkFinish(hEntity);
}

int TaskFinishHeroQualityCondition::getEventID()const
{
	return EVENT_ENTITY_TASK_HEROQUALITY;
}

bool TaskFinishHeroQualityCondition::checkFinish(HEntity hEntity)
{
	IEntity *pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);
	ITaskSystem* pTaskSys = static_cast<ITaskSystem*>(pEntity->querySubsystem(IID_ITaskSystem));
	ServerEngine::TaskDBUnit unit;
	bool res = pTaskSys->getTaskDBUnit(unit,m_iTaskId);
	if(!res)
	{
		return false;
	}
	else if( unit.enTaskState == GSProto::en_Task_Finish ||  unit.enTaskState == GSProto::en_Task_HaveReward)
	{
		return true;
	}
	else if( unit.enTaskState == GSProto::en_Task_OPen)
	{	
		IHeroSystem *pSys = static_cast<IHeroSystem*>(pEntity->querySubsystem(IID_IHeroSystem));
		assert(pSys);
		int iCount = pSys->getHeroCountByQuality(m_iQuality);
		
		int iProcess = std::min(m_igoal, iCount );
		pTaskSys->chgFinishProcess(m_iTaskId,iProcess);
		if(iProcess >= m_igoal)
		{
			pTaskSys->setFinishState(m_iTaskId,GSProto::en_Task_Finish);
			return true;
		}
		return false;
	}
	return false;
}

bool TaskFinishFormationCondition::checkFinishCondition(HEntity hEntity,int iEventID,EventArgs & args)
{
	if( iEventID != getEventID())
	{
		return false;
	}

	return checkFinish(hEntity);
}

int TaskFinishFormationCondition::getEventID()const
{
	return EVENT_ENTITY_TASK_FORMATION;
}

bool TaskFinishFormationCondition::checkFinish(HEntity hEntity)
{
	IEntity *pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);
	ITaskSystem* pTaskSys = static_cast<ITaskSystem*>(pEntity->querySubsystem(IID_ITaskSystem));
	ServerEngine::TaskDBUnit unit;
	bool res = pTaskSys->getTaskDBUnit(unit,m_iTaskId);
	if(!res)
	{
		return false;
	}
	else if( unit.enTaskState == GSProto::en_Task_Finish ||  unit.enTaskState == GSProto::en_Task_HaveReward)
	{
		return true;
	}
	else if( unit.enTaskState == GSProto::en_Task_OPen)
	{	
		///int iCount = args.context.getInt("formationCount");
		IFormationSystem* pFormation = static_cast<IFormationSystem*>(pEntity->querySubsystem(IID_IFormationSystem));
		assert(pFormation);
		int iCount  = pFormation->getFormationHeroCount();
		int iProcess = std::min(m_igoal, iCount );
		pTaskSys->chgFinishProcess(m_iTaskId,iProcess);
		if(iProcess >= m_igoal)
		{
			pTaskSys->setFinishState(m_iTaskId,GSProto::en_Task_Finish);
			return true;
		}
		return false;
	}
	return false;
}



///////

bool TaskUnit::checkTaskFinish(HEntity hEntity,int iEventID, EventArgs& args) 
{
	
	for(size_t i = 0; i < m_FinishConditionVec.size(); ++i)
	{
		bool res = m_FinishConditionVec[i]->checkFinishCondition(hEntity,iEventID,args);
		if(res)
		{
			return true;
		}
	}
	return false;
}

bool TaskUnit::checkTaskOpen(HEntity hEntity) const
{
	size_t size = m_OpenConditionVec.size();
	bool bIsOpen = true;
	for(size_t i = 0; i < size; ++i)
	{
	  bIsOpen = m_OpenConditionVec[i]->checkCondition(hEntity);
	  if(!bIsOpen)
	  {
	  	return bIsOpen;
	  }
	}
	
	return bIsOpen;
}

bool TaskUnit::checkTaskFinish(HEntity hEntity)
{
	bool bIsOpen = true;
	for(size_t i = 0; i < m_FinishConditionVec.size(); ++i)
	{
		bIsOpen = m_FinishConditionVec[i]->checkFinish(hEntity);
		if(!bIsOpen)return bIsOpen;
	}
	return bIsOpen;
}

bool TaskUnit::checkTaskReward(HEntity hEntity)
{
	for(size_t i = 0; i < m_RewardVec.size(); ++i)
	{
	  m_RewardVec[i]->taskReward(hEntity);
	}
	return true;
}

////////////////
bool TaskReward_LifeAtt::taskReward(HEntity hEntity)
{
	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);
	
	if( m_ilifeattID == GSProto::en_LifeAtt_Exp)
	{
		pEntity->addExp(m_iCount);
	}
	else
	{
		int iPropID = LifeAtt2Prop(m_ilifeattID);
		assert(iPropID >= 0);
		pEntity->changeProperty(iPropID,m_iCount,GSProto::en_Reason_TaskFinishReward);
	}
	return true;
}

bool TaskReward_Item::taskReward(HEntity hEntity)
{
	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	IItemSystem* pItemSystem = static_cast<IItemSystem*>(pEntity->querySubsystem(IID_IItemSystem));
	assert(pItemSystem);
	
	PLAYERLOG(pEntity)<<"addItem|"<<"|"<<GSProto::en_Reason_TaskFinishReward<<endl;
	return pItemSystem->addItem(m_iItemId,m_iCount,GSProto::en_Reason_TaskFinishReward);
	
}

bool TaskReward_Hero::taskReward(HEntity hEntity)
{
	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	IHeroSystem* pHeroSystem = static_cast<IHeroSystem*>(pEntity->querySubsystem(IID_IHeroSystem));
	assert(pHeroSystem);
	bool resoult = pHeroSystem->addHeroWithLevelStep(m_iHeroId,m_iStepLevel,true,GSProto::en_Reason_TaskFinishReward);
	if(!resoult)
	{
		PLAYERLOG(pEntity)<<"[ERROR]"<<"|addHERO|"<<"|"<<GSProto::en_Reason_TaskFinishReward<<endl;
	}
	return true;
}



//////////////////////////////////
TaskFactory::TaskFactory()
{
}

TaskFactory::~TaskFactory()
{
	m_TaskMap.clear();

}

bool TaskFactory::initlize(const PropertySet& propSet)
{
	
	ITable* table = getCompomentObjectManager()->findTable(TABLENAME_Task);
	assert(table);

	int count = table->getRecordCount();
	for(int i = 0; i < count; ++i)
	{
		TaskUnit unit;

		unit.iTaskId = table->getInt(i,"任务ID");
		unit.iTaskTypeId = table->getInt(i,"任务类型");

		string str1 = table->getString(i,"接取条件1");
		if(str1.length()>0)
		{
			TaskOpenCondition_Base* task = ParseOpenCondition(str1 );
			if(task != NULL)
			{
				unit.m_OpenConditionVec.push_back(task);
			}
			
		}
		
		string str2 = table->getString(i,"接取条件2");
		if(str2.length()>0)
		{
			TaskOpenCondition_Base* task = ParseOpenCondition(str2 );
			if(task != NULL)
			{
				unit.m_OpenConditionVec.push_back(task);
			}
		}
		
		string str3 = table->getString(i,"接取条件3");
		if(str3.length()>0)
		{
			TaskOpenCondition_Base* task = ParseOpenCondition(str3 );
			if(task != NULL)
			{
				unit.m_OpenConditionVec.push_back(task);
			}
		}
		
		string str4 = table->getString(i,"接取条件4");
		if(str4.length()>0)
		{
			TaskOpenCondition_Base* task = ParseOpenCondition(str4 );
			if(task != NULL)
			{
				unit.m_OpenConditionVec.push_back(task);
			}
		}
		
		string strFinish = table->getString(i,"完成条件");
		if(strFinish.length()>0)
		{
			TaskFinishCondition_Base* base = ParseFinishCondition(strFinish,unit.iTaskId);
			assert(base);
			unit.m_FinishConditionVec.push_back(base);
			unit.iGoal = base->m_igoal;

			FDLOG("TaskFactory")<<"string ===|"<<strFinish.c_str()<<"|Goal==" << unit.iGoal <<endl;
		}

		string strRestStr = table->getString(i,"任务重置周期");
		unit.taskResetType = getTaskResetType(strRestStr);
		unit.icanFinishTimes = table->getInt(i,"可完成次数");

		string strReward1 = table->getString(i,"奖励类型1");
		if(strReward1.length()>0)
		{
			TaskReward_Base* reward = ParseReward(strReward1);
			if(reward != NULL)
			{
				unit.m_RewardVec.push_back(reward);
			}
		}
		
		string strReward2 = table->getString(i,"奖励类型2");
		if(strReward2.length()>0)
		{
			TaskReward_Base* reward = ParseReward(strReward2);
			if(reward != NULL)
			{
				unit.m_RewardVec.push_back(reward);
			}
		}
		
		string strReward3 = table->getString(i,"奖励类型3");
		if(strReward3.length()>0)
		{
			TaskReward_Base* reward = ParseReward(strReward3);
			if(reward != NULL)
			{
				unit.m_RewardVec.push_back(reward);
			}
		}

		m_TaskMap[unit.iTaskId] = unit;
	}
	
	return true;
}

TaskFinishCondition_Base* TaskFactory::ParseFinishCondition(const string conditionKey ,int taskId)
{
	if( conditionKey.length() == 0) return NULL;
	vector<string> paramList = TC_Common::sepstr<string>(conditionKey, Effect_MagicSep);

	if(paramList[0] == "通关副本章节")
	{
		assert(paramList.size() == 3);
		int dungeonId = atoi(paramList[1].c_str());
		int iGoal =  atoi(paramList[2].c_str());
		m_FinishEventWithTaskMap[EVENT_ENTITY_FINISHDUNGEONCONDITION].push_back(taskId);
		m_FinishEventSet.insert(EVENT_ENTITY_FINISHDUNGEONCONDITION);
		return new TaskFinishDungeonCondition( EVENT_ENTITY_FINISHDUNGEONCONDITION,taskId,dungeonId,iGoal);
	}
	
	else if(paramList[0] == "参加阵营战")
	{
		assert(paramList.size() == 2);
			int iGoal = TC_S2I(paramList[1]);
		assert(iGoal > 0);
		return addFinishTimesTaskEvent(taskId,EVENT_ENTITY_TASKFINISH_JOINCAMPBATTLE,iGoal);
		
	}
	
	else if(paramList[0] == "参加世界BOSS")
	{
		assert(paramList.size() == 2);
		int iGoal = TC_S2I(paramList[1]);
		assert(iGoal > 0);
		return addFinishTimesTaskEvent(taskId,EVENT_ENTITY_TASKFINISH_WORLDBOSS,iGoal);
	}
	
	else if(paramList[0] == "招募英雄次数")
	{
		assert(paramList.size() == 2);
		int iGoal = TC_S2I(paramList[1]);
		assert(iGoal > 0);
		return addFinishTimesTaskEvent(taskId,EVENT_ENTITY_TASKFINISH_GETHERO,iGoal);
	}
	else if(paramList[0] == "英雄升级次数")
	{
		assert(paramList.size() == 2);
		int iGoal = TC_S2I(paramList[1]);
		assert(iGoal > 0);
		return addFinishTimesTaskEvent(taskId,EVENT_ENTITY_TASK_HEROLEVELUP,iGoal);
	}
	else if(paramList[0] == "神兽培养次数")
	{
		assert(paramList.size() == 2);
		int iGoal = TC_S2I(paramList[1]);
		assert(iGoal > 0);
		return addFinishTimesTaskEvent(taskId,EVENT_ENTITY_TASKFINISH_GODANIMALTRAIN,iGoal);
	}
	/*else if(paramList[0] == "挑战任意关副本关卡次数")
	{
		assert(paramList.size() == 2);
		int iGoal = TC_S2I(paramList[1]);
		assert(iGoal > 0);
		return addFinishTimesTaskEvent(taskId,EVENT_ENTITY_TASKFINISH_BATTLEANYDUNGEON,iGoal);
	}*/
	else if(paramList[0] == "掠夺次数")
	{
		assert(paramList.size() == 2);
		int iGoal = TC_S2I(paramList[1]);
		assert(iGoal > 0);
		return addFinishTimesTaskEvent(taskId,EVENT_ENTITY_TASKFINISH_LOOT,iGoal);
	}
	else if(paramList[0] == "英雄等级")
	{
		assert(paramList.size() == 2);
		int iGoal = atoi(paramList[1].c_str());

		m_FinishEventWithTaskMap[EVENT_ENTITY_TASKFINISH_HEROLEVEL].push_back(taskId);
		m_FinishEventSet.insert(EVENT_ENTITY_TASKFINISH_HEROLEVEL);
		return new TaskFinishHeroLevelCondition(EVENT_ENTITY_TASKFINISH_HEROLEVEL,iGoal,taskId);
	}
	else if(paramList[0] == "神兽等级")
	{
		assert(paramList.size() == 2);
		int iGoal = atoi(paramList[1].c_str());

		m_FinishEventWithTaskMap[EVENT_ENTITY_TASKFINISH_GODANIMLLEVEL].push_back(taskId);
		m_FinishEventSet.insert(EVENT_ENTITY_TASKFINISH_GODANIMLLEVEL);

		return new TaskFinishGodAnimalLevelCondition(EVENT_ENTITY_TASKFINISH_GODANIMLLEVEL,iGoal,taskId);
	}
	else if(paramList[0] == "到达通天塔层数")
	{
		assert(paramList.size() == 2);
		int iGoal = atoi(paramList[1].c_str());

		m_FinishEventWithTaskMap[EVENT_ENTITY_TASKFINISH_CLIMBTOWER].push_back(taskId);
		m_FinishEventSet.insert(EVENT_ENTITY_TASKFINISH_CLIMBTOWER);

		return new TaskFinishClimbTowerCondition(EVENT_ENTITY_TASKFINISH_CLIMBTOWER,iGoal,taskId);
	}
	else if(paramList[0] == "通关关卡")
	{
		assert(paramList.size() == 3);
		int secenID = atoi(paramList[1].c_str());
		int iGoal = atoi(paramList[2].c_str());

		m_FinishEventWithTaskMap[EVENT_ENTITY_PASSSCENE].push_back(taskId);
		m_FinishEventSet.insert(EVENT_ENTITY_PASSSCENE);

		return new TaskFinishDungeonSceneCondition(EVENT_ENTITY_PASSSCENE,taskId,secenID,iGoal);
	}
	else if(paramList[0] == "等级")
	{
		assert(paramList.size() == 2);
		int iGoal = atoi(paramList[1].c_str());
		//FDLOG("TaskFactory")<<"string dfsff===|"<<paramList[0].c_str()<<"|Goal==" << iGoal <<endl;

		m_FinishEventWithTaskMap[EVENT_ENTITY_POSTLEVELUP].push_back(taskId);
		m_FinishEventSet.insert(EVENT_ENTITY_POSTLEVELUP);

		return new TaskFinishPlayerLevelCondition(EVENT_ENTITY_POSTLEVELUP,iGoal,taskId);
	}
	else if( paramList[0] == "技能升级")
	{
		//assert(0);
		assert(paramList.size() == 2);
		int iGoal = TC_S2I(paramList[1]);
		assert(iGoal > 0);
		return addFinishTimesTaskEvent(taskId,EVENT_ENTITY_TASK_HEROSKILL_LEVELUP,iGoal);
	}
	else if( paramList[0] == "通过九天幻境关卡数")
	{
		//assert(0);
		assert(paramList.size() == 2);
		int iGoal = TC_S2I(paramList[1]);
		assert(iGoal > 0);
		return addFinishTimesTaskEvent(taskId,EVENT_ENTITY_TASK_CRUSH_DREAMLAND_SCENE,iGoal);
	}
	else  if( paramList[0] == "修为矿等级")
	{
		assert(paramList.size() == 3);
		int iLevel = TC_S2I(paramList[1]);
		int iGoal = TC_S2I(paramList[2]);
		assert(iGoal > 0);
		m_FinishEventWithTaskMap[EVENT_ENTITY_TASK_SILVER_RES_LEVELUP].push_back(taskId);
		m_FinishEventSet.insert(EVENT_ENTITY_TASK_SILVER_RES_LEVELUP);
		return new TaskFinishHeroExpResLevelCondition(EVENT_ENTITY_TASK_SILVER_RES_LEVELUP,iLevel , iGoal,taskId);
	}
	else  if( paramList[0] == "铜矿等级")
	{
		assert(paramList.size() == 3);
		int iLevel = TC_S2I(paramList[1]);
		int iGoal = TC_S2I(paramList[2]);
		assert(iGoal > 0);

		m_FinishEventWithTaskMap[EVENT_ENTITY_TASK_HEROEXP_RES_LEVELUP].push_back(taskId);
		m_FinishEventSet.insert(EVENT_ENTITY_TASK_HEROEXP_RES_LEVELUP);
		return new TaskFinishSilverResLevelCondition(EVENT_ENTITY_TASK_HEROEXP_RES_LEVELUP, iLevel, iGoal,taskId);
	}
	else if( paramList[0] == "通关副本关卡数")
	{
		//assert(0);
		assert(paramList.size() == 2);
		int iGoal = TC_S2I(paramList[1]);
		assert(iGoal > 0);
		return addFinishTimesTaskEvent(taskId,EVENT_ENTITY_PASSSCENE_COUNT, iGoal);
	}
	
	else if( paramList[0] == "摇钱树使用次数")
	{
		assert(paramList.size() == 2);
		int iGoal = TC_S2I(paramList[1]);
		assert(iGoal > 0);
		return addFinishTimesTaskEvent(taskId,EVENT_ENTITY_YAOQIANSHU_USETIMES,iGoal);
	}
	else if( paramList[0] == "领地收获次数")
	{
		assert(paramList.size() == 2);
		int iGoal = TC_S2I(paramList[1]);
		assert(iGoal > 0);
		return addFinishTimesTaskEvent(taskId,EVENT_ENTITY_TASK_MANOR_HARVERST,iGoal);
	}
	
	else if( paramList[0] == "武魂祭炼次数")
	{
		assert(paramList.size() == 2);
		int iGoal = TC_S2I(paramList[1]);
		assert(iGoal > 0);
		return addFinishTimesTaskEvent(taskId,EVENT_ENTITY_TASK_MANOR_WuHunJiLian,iGoal);
	}
	
	else if( paramList[0] == "物品打造次数")
	{
		assert(paramList.size() == 2);
		int iGoal = TC_S2I(paramList[1]);
		assert(iGoal > 0);
		return addFinishTimesTaskEvent(taskId,EVENT_ENTITY_TASK_MANOR_ITEMDaZao,iGoal);
	}
	
	else if( paramList[0] == "赠送体力次数")
	{
		assert(paramList.size() == 2);
		int iGoal = TC_S2I(paramList[1]);
		assert(iGoal > 0);
		return addFinishTimesTaskEvent(taskId,EVENT_ENTITY_TASK_GIVE_Strength,iGoal);
	}
	else if( paramList[0] == "竞技场挑战次数")
	{
		assert(paramList.size() == 2);
		int iGoal = TC_S2I(paramList[1]);
		assert(iGoal > 0);
		return  addFinishTimesTaskEvent(taskId,EVENT_ENTITY_TASK_ARENA_BATTLE, iGoal);
	}
	
	else if( paramList[0] == "武魂殿等级")
	{
		assert(paramList.size() == 2);
		int iGoal = TC_S2I(paramList[1]);
		assert(iGoal > 0);
		m_FinishEventWithTaskMap[EVENT_ENTITY_TASK_HEROSOUL_LEVEL].push_back(taskId);
		m_FinishEventSet.insert(EVENT_ENTITY_TASK_HEROSOUL_LEVEL);
		return new TaskFinishHeroSoulLevelCondition(EVENT_ENTITY_TASK_HEROSOUL_LEVEL, iGoal, taskId );
	}
	
	else if( paramList[0] == "铁匠铺等级")
	{
		assert(paramList.size() == 2);
		int iGoal = TC_S2I(paramList[1]);
		assert(iGoal > 0);
		m_FinishEventWithTaskMap[EVENT_ENTITY_TASK_TIEJIANGPU_LEVEL].push_back(taskId);
		m_FinishEventSet.insert(EVENT_ENTITY_TASK_TIEJIANGPU_LEVEL);
		return new TaskFinishTieJiangPuLevelCondition(EVENT_ENTITY_TASK_HEROSOUL_LEVEL, iGoal , taskId);
	}
	
	else if( paramList[0] == "英雄品质")
	{
		assert(paramList.size() == 3);
		int iQuary = TC_S2I(paramList[1]);
		int iGoal = TC_S2I(paramList[2]);
		assert(iGoal > 0);
		m_FinishEventWithTaskMap[EVENT_ENTITY_TASK_HEROQUALITY].push_back(taskId);
		m_FinishEventSet.insert(EVENT_ENTITY_TASK_HEROQUALITY);
		return new TaskFinishHeroQualityCondition(EVENT_ENTITY_TASK_HEROQUALITY,iGoal , iQuary, taskId);
	}
	
	else if( paramList[0] == "装备喜好品数量")
	{
		assert(paramList.size() == 2);
		int iGoal = TC_S2I(paramList[1]);
		assert(iGoal > 0);
		return addFinishTimesTaskEvent(taskId,EVENT_ENTITY_TASK_EQUIPLOVEQUIP,iGoal);
	}
	else if( paramList[0] == "上阵英雄数")
	{
		assert(paramList.size() == 2);
		int iGoal = TC_S2I(paramList[1]);
		assert(iGoal > 0);
		m_FinishEventWithTaskMap[EVENT_ENTITY_TASK_FORMATION].push_back(taskId);
		m_FinishEventSet.insert(EVENT_ENTITY_TASK_FORMATION);
		return new TaskFinishFormationCondition(EVENT_ENTITY_TASK_FORMATION,iGoal,taskId);
	}
	else
	{
		assert(0);
	}
	return NULL;

	
}

TaskFinishCondition_Base* TaskFactory::addFinishTimesTaskEvent(int iTaskId, int iEventId, int iGoal)
{
	m_FinishEventWithTaskMap[iEventId].push_back(iTaskId);
	m_FinishEventSet.insert(iEventId);

	return new TaskFinishTimesCondition(iGoal,iEventId,iTaskId);
}


TaskOpenCondition_Base* TaskFactory::ParseOpenCondition(const string conditonKey )
{
	if( conditonKey.length() == 0) return NULL;
	vector<string> paramList = TC_Common::sepstr<string>(conditonKey, Effect_MagicSep);

	if( paramList[0] == "自动接取")
	{
		return new TaskOpenCondition_Auto;
	}
	
	else if( paramList[0] == "前置")
	{
		if(paramList.size() == 2)
		{
			int iLastTaskID = atoi( paramList[1].c_str());
			return new TaskOpenCondition_FinishLastTask(iLastTaskID);
		}
		assert(0);
	}
	else if( paramList[0] == "等级")
	{
		if(paramList.size() == 2)
		{
			int iLevel = atoi( paramList[1].c_str());
			return new TaskOpenCondition_AchieveLevel(iLevel);
		}
		assert(0);
	}
	else if( paramList[0] == "时间")
	{
		if(paramList.size() == 3)
		{
		   IZoneTime* pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime,IID_IZoneTime);
			assert(pZoneTime);
			Uint32 min = pZoneTime->StrTime2Seconds(paramList[1].c_str(), "%Y:%m:%d:%H:%M:%S");
			Uint32 max = pZoneTime->StrTime2Seconds(paramList[2].c_str(), "%Y:%m:%d:%H:%M:%S");
			
			return new TaskOpenCondition_InTimes(min,max);
		}
		assert(0);
	}
	else if( paramList[0] == "每日自动接取")
	{
		return new TaskOpenCondition_DayAuto();
	}
	else
	{
		assert(0);
	}
} 

TaskReward_Base* TaskFactory::ParseReward(const string rewardStr)
{
	if(rewardStr.length()==0)
	return NULL;
	
	vector<string> paramList = TC_Common::sepstr<string>(rewardStr, Effect_MagicSep);
	
	if(paramList[0] == "LifeAtt")
	{
		int iLifeatt = atoi(paramList[1].c_str());
		int iCount = atoi(paramList[2].c_str());
		return new TaskReward_LifeAtt(iLifeatt,iCount);
	}
	else if(paramList[0] == "Item")
	{
		int iItemId = atoi(paramList[1].c_str());
		int iCount = atoi(paramList[2].c_str());
		return new TaskReward_Item(iItemId,iCount);
	}
	else if(paramList[0] == "Hero")
	{
		int iHeroId = atoi(paramList[1].c_str());
		int iLevelStep = atoi(paramList[2].c_str());
		return new TaskReward_Hero(iHeroId,iLevelStep);
	}
	else
	{
		assert(0);
	}
	return NULL;
}


void TaskFactory::checkFinish(HEntity hEntity)
{
	map<int, TaskUnit>::iterator iter = m_TaskMap.begin();
	for(;iter != m_TaskMap.end(); ++iter)
	{
		iter->second.checkTaskFinish(hEntity);
	}
}


void TaskFactory::getNewOpenTaskList(HEntity hEntity,vector<TaskUnit>& taskUnitVec)
{
	map<int, TaskUnit>::iterator iter = m_TaskMap.begin();
	for(;iter != m_TaskMap.end(); ++iter)
	{
		bool res = iter->second.checkTaskOpen(hEntity);
		if( res)
		{
			taskUnitVec.push_back(iter->second);
		}
	}
}

bool  TaskFactory::getTaskReward(HEntity hEntity,int iTaskId)
{
	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	ITaskSystem* pTaskSystem = static_cast<ITaskSystem*>(pEntity->querySubsystem(IID_ITaskSystem));
	assert(pTaskSystem);

	bool bTaskFinish = pTaskSystem->getTaskHaveFinish(iTaskId);
	if(bTaskFinish)
	{
		map<int, TaskUnit>::iterator iter = m_TaskMap.find(iTaskId);
		assert(iter != m_TaskMap.end());
		iter->second.checkTaskReward(hEntity);
	}
	return true;
}

TaskResetType TaskFactory::getTaskResetType(const string strRest)
{
	assert(strRest.length() > 0);
	
	vector<string> paramList = TC_Common::sepstr<string>(strRest, Effect_MagicSep);

	if(paramList[0] == "永不重置")
	{
		return en_TaskResetType_NeverReset;
	}
	else if( paramList[0] == "每日重置")
	{
		return en_TaskResetType_DayReset;
	}
	else if(paramList[0] == "周重置")
	{
		return en_TaskResetType_WeekReset;
	}
	else if(paramList[1] == "月重置")
	{
		return en_TaskResetType_MonthReset;
	}
	return en_TaskResetType_NeverReset;
}



void TaskFactory::getFinishEventVec(vector<int>& eventVec)
{	
	eventVec.clear();
	std::copy(m_FinishEventSet.begin(),m_FinishEventSet.end(),std::inserter(eventVec,eventVec.end()));
}

bool TaskFactory::checkFinish(HEntity hEntity,int eventID,EventArgs& args)
{
	 map<int ,vector<int> >::iterator iter = m_FinishEventWithTaskMap.find(eventID);
	 assert(iter != m_FinishEventWithTaskMap.end());
	 int size = iter->second.size();
	 for(int i = 0; i< size; ++i)
	 {
	 	map<int, TaskUnit>::iterator iter2 = m_TaskMap.find(iter->second[i]);
		assert(iter2 != m_TaskMap.end());
		iter2->second.checkTaskFinish(hEntity,eventID,args);
	 }
	 return true;
}


TaskUnit TaskFactory::getTaskUnitByTaskId(int iTaskId)
{
	map<int, TaskUnit>::iterator iter = m_TaskMap.find(iTaskId);
	assert(iter != m_TaskMap.end());

	return iter->second;
}


