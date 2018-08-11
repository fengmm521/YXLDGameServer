#include "GuideSystemPch.h"
#include "GuideFactory.h"


extern "C" IComponent* createGuideFactory(Int32)
{
	return new GuideFactory;
}


bool GuideCondition_Event::checkCondition(HEntity hEntity, int iEventID, EventArgs& args) const
{
	if(iEventID != iEventID)
	{
		return false;
	}

	return true;
}

int GuideCondition_Event::getEventID() const
{
	return m_iEventID;
}


bool GuideCondition_PassScene::checkCondition(HEntity hEntity, int iEventID, EventArgs& args) const
{
	if(iEventID != getEventID() )
	{
		return false;
	}

	// 判断场景
	int iSceneID = args.context.getInt("sceneid");

	return iSceneID == m_iSceneID;
}

int GuideCondition_PassScene::getEventID() const 
{
	return EVENT_ENTITY_PASSSCENE;
}


bool GuideCondition_FinishGuide::checkCondition(HEntity hEntity, int iEventID, EventArgs& args) const
{
	if(iEventID != getEventID() )
	{
		return false;
	}

	int iGuideID = args.context.getInt("guideid");
	if(iGuideID != m_iFinishGuideID)
	{
		return false;
	}

	return true;
}

int GuideCondition_FinishGuide::getEventID() const
{
	return EVENT_ENTITY_FINISGUIDE;
}

bool GuideCondition_FunctionOpen::checkCondition(HEntity hEntity, int iEventID, EventArgs& args) const
{	
	if(iEventID != getEventID() )
	{
		return false;
	}

	int iFunctionID = args.context.getInt("function");
	if(iFunctionID != m_iFunctionID)
	{
		return false;
	}

	return true;
}

int GuideCondition_FunctionOpen::getEventID() const
{
	return EVENT_ENTITY_OPENFUNCTION;
}


bool GuideCondition_PassSection::checkCondition(HEntity hEntity, int iEventID, EventArgs& args) const
{
	if(iEventID != getEventID() )
	{
		return false;
	}

	int iSectionID = args.context.getInt("sectionid");
	if(iSectionID != m_iSectionID)
	{
		return false;
	}

	bool bFirstPerfectPass = args.context.getInt("firstperfectpass") != 0;
	if(bFirstPerfectPass != m_bFirstPerfectPass)
	{
		return false;
	}

	return true;
}

int GuideCondition_PassSection::getEventID() const
{
	return EVENT_ENTITY_PERFECT_PASSSECTION;
}

bool GuideCondition_FinishTask::checkCondition(HEntity hEntity, int iEventID, EventArgs& args) const
{
	if(iEventID != getEventID() )
	{
		return false;
	}

	int iTaskID = args.context.getInt("taskid", 0);
	
	return iTaskID == m_iTaskID;
}

int GuideCondition_FinishTask::getEventID() const
{
	return EVENT_ENTITY_TASK_HAVEREWARD;
}


bool GuideCondition_Level::checkCondition(HEntity hEntity, int iEventID, EventArgs& args) const
{
	if(iEventID != getEventID() )
	{
		return false;
	}

	int iLevel = args.context.getInt("newlv");
	int iOldLevel = args.context.getInt("oldlv");

	return (iOldLevel < m_iLevel) && (iLevel >= m_iLevel);
}

int GuideCondition_Level::getEventID() const
{
	return EVENT_ENTITY_LEVELUP;
}



Guide::Guide():m_iGuideID(0), m_iDependGuideID(0), m_bMustDo(0), m_iMinLv(0), m_iMaxLv(0), m_bRepeat(false), m_bAutoFinish(false)
{
}

Guide::~Guide()
{
	for(size_t i = 0; i < m_conditionList.size(); i++)
	{
		delete m_conditionList[i];
	}
	m_conditionList.clear();
}

bool Guide::checkOpen(HEntity hEntity, int iEventID, EventArgs& args) const
{
	// 这里不判断完成和Open状态，状态在Factory里判断了
	IEntity* pEntity = getEntityFromHandle(hEntity);
	if(!pEntity) return false;

	IGuideSystem* pGuideSys = static_cast<IGuideSystem*>(pEntity->querySubsystem(IID_IGuideSystem));
	assert(pGuideSys);

	int iLevel = pEntity->getProperty(PROP_ENTITY_LEVEL, 0);

	// 如果等级有限制
	if( (m_iMinLv != 0) && (iLevel < m_iMinLv) )
	{
		return false;
	}

	if( (m_iMaxLv != 0) && (iLevel > m_iMaxLv) )
	{
		return false;
	}
	
	// 如果有	前置引导,未完成，返回
	if( (m_iDependGuideID != 0) && !pGuideSys->isGuideFinished(m_iDependGuideID) )
	{
		return false;
	}
	
	for(size_t i = 0; i < m_conditionList.size(); i++)
	{
		if(!m_conditionList[i]->checkCondition(hEntity, iEventID, args) )
		{
			return false;
		}
	}

	return true;
}


GuideFactory::GuideFactory()
{
}

GuideFactory::~GuideFactory()
{
}

bool GuideFactory::initlize(const PropertySet& propSet)
{
	loadGuide();

	return true;
}

void GuideFactory::loadGuide()
{
	ITable* pTable = getCompomentObjectManager()->findTable("Guide");
	assert(pTable);

	int iRecordCount = pTable->getRecordCount();
	for(int i = 0; i < iRecordCount; i++)
	{
		Guide* pGuide = new Guide;
		
		pGuide->m_iGuideID = pTable->getInt(i, "引导ID");
		pGuide->m_iDependGuideID = pTable->getInt(i, "前置引导");
		pGuide->m_bMustDo = pTable->getInt(i, "是否强制引导") == 1;
		pGuide->m_iMinLv = pTable->getInt(i, "等级下限");
		pGuide->m_iMaxLv = pTable->getInt(i, "等级上限");

		string strCondition = pTable->getString(i, "触发事件");
		GuideConditionBase* pCondition = parseGuideCondition(strCondition);
		if(pCondition) pGuide->m_conditionList.push_back(pCondition);

		pGuide->m_bRepeat = pTable->getInt(i, "是否重复触发") == 1;
		pGuide->m_bAutoFinish = pTable->getInt(i, "自动完成") == 1;

		m_guideList[pGuide->m_iGuideID] = pGuide;

		for(vector<GuideConditionBase*>::iterator it = pGuide->m_conditionList.begin(); it != pGuide->m_conditionList.end(); it++)
		{
			int iTmpEventID = (*it)->getEventID();
			m_guideEventRelationMap[iTmpEventID].insert(pGuide->m_iGuideID);
			m_eventList.insert(iTmpEventID);
		}
	}
}


bool GuideFactory::isAutoFinish(int iGuideID)
{
	map<int, Guide*>::const_iterator it = m_guideList.find(iGuideID);
	if(it == m_guideList.end() )
	{
		return false;
	}

	return it->second->m_bAutoFinish;
}


set<int> GuideFactory::getNewOpenGuideList(HEntity hEntity, int iEventID, EventArgs& args)
{
	set<int> resultList;

	if(m_guideEventRelationMap.find(iEventID) == m_guideEventRelationMap.end() )
	{
		return resultList;
	}

	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	IGuideSystem* pGuideSys = static_cast<IGuideSystem*>(pEntity->querySubsystem(IID_IGuideSystem));
	assert(pGuideSys);

	const set<int>& guideIDList = m_guideEventRelationMap[iEventID];

	for(set<int>::const_iterator it = guideIDList.begin(); it != guideIDList.end(); it++)
	{
		int iTmpGuideID = *it;

		Guide* pTmpGuide = getGuide(iTmpGuideID);
		assert(pTmpGuide);
		
		if( (pGuideSys->isGuideFinished(iTmpGuideID) && !(pTmpGuide->m_bRepeat) ) || pGuideSys->isGuideOpened(iTmpGuideID) )
		{
			continue;
		}

		if(pTmpGuide->checkOpen(hEntity, iEventID,args) )
		{
			resultList.insert(iTmpGuideID);
		}
	}

	return resultList;
}

const set<int>& GuideFactory::getRelationEventList()
{
	return m_eventList;
}


Guide* GuideFactory::getGuide(int iGuideID)
{
	map<int, Guide*>::iterator it = m_guideList.find(iGuideID);
	if(it != m_guideList.end() )
	{
		return it->second;
	}

	return NULL;
}

GuideConditionBase* GuideFactory::parseGuideCondition(const string& strCondition)
{
	if(strCondition.size() == 0) return NULL;

	vector<string> paramList = TC_Common::sepstr<string>(strCondition, "#");
	if(paramList.size() == 0)
	{
		return NULL;
	}

	if(paramList[0] == "新角色首次进入")
	{
		return new GuideCondition_Event(EVENT_ENTITY_SENDACTOR_TOCLIENT_POST);
	}
	else if( (paramList[0] == "通关副本") && (paramList.size() == 2) )
	{
		int iSceneID = AdvanceAtoi(paramList[1]);
		return new GuideCondition_PassScene(iSceneID);
	}
	else if( (paramList[0] == "完成引导") && (paramList.size() == 2) )
	{
		int iGuideID = AdvanceAtoi(paramList[1]);
		return new GuideCondition_FinishGuide(iGuideID);
	}
	else if( (paramList[0] == "开启功能") && (paramList.size() == 2) )
	{
		int iFunctionID = AdvanceAtoi(paramList[1]);
		return new GuideCondition_FunctionOpen(iFunctionID);
	}
	else if( (paramList[0] == "首次完美通关章节") && (paramList.size() == 2) )
	{
		int iSectionID = AdvanceAtoi(paramList[1]);
		return new GuideCondition_PassSection(iSectionID, true);
	}
	else if( (paramList[0] == "任务完成") && (paramList.size() == 2) )
	{
		int iTaskID = AdvanceAtoi(paramList[1]);
		return new GuideCondition_FinishTask(iTaskID);
	}
	else if( (paramList[0] == "达到等级") && (paramList.size() == 2) )
	{
		int iLevel = AdvanceAtoi(paramList[1]);
		return new GuideCondition_Level(iLevel);
	}
	/*
	else if(paramList[0] == "程序实现")
	{
		return new GuideCondition_None();
	}
	*/
	else
	{
		assert(false);
	}

	return NULL;
}



