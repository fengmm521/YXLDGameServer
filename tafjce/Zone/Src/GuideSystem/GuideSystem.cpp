#include "GuideSystemPch.h"
#include "GuideSystem.h"
#include "GuideFactory.h"

extern "C" IObject* createGuideSystem()
{
	return new GuideSystem;
}


GuideSystem::GuideSystem()
{
}

GuideSystem::~GuideSystem()
{
	for(size_t i = 0; i < m_eventHelperList.size(); i++)
	{
		delete m_eventHelperList[i];
	}
}

Uint32 GuideSystem::getSubsystemID() const
{
	return IID_IGuideSystem;
}

Uint32 GuideSystem::getMasterHandle(){return m_hEntity;}

bool GuideSystem::create(IEntity* pEntity, const std::string& strData)
{
	m_hEntity = pEntity->getHandle();

	if(strData.size() > 0)
	{
		ServerEngine::GuideSystemData sysData;
		ServerEngine::JceToObj(strData, sysData);

		std::copy(sysData.finishedList.begin(), sysData.finishedList.end(), std::inserter(m_finishedList, m_finishedList.end() ) );
		std::copy(sysData.openedList.begin(), sysData.openedList.end(), std::inserter(m_opendList, m_opendList.end() ) );
	}

	// 注册通知事件
	pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_SENDACTOR_TOCLIENT, this, &GuideSystem::onEventSend2Client);

	registerGuideEvent();

	return true;
}


void GuideEventHelper::onEventArgs(EventArgs& args)
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	if(!pEntity) return;

	GuideSystem* pGudeSystem = static_cast<GuideSystem*>(pEntity->querySubsystem(IID_IGuideSystem));
	if(!pGudeSystem) return;

	pGudeSystem->onGuideRelationEvent(m_iEventID, args);
}

void GuideSystem::registerGuideEvent()
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	// 注册事件
	GuideFactory* pGuideFactory = static_cast<GuideFactory*>(getComponent<IGuideFactory>("GuideFactory", IID_IGuideFactory));
	assert(pGuideFactory);

	const set<int>& evenetList = pGuideFactory->getRelationEventList();
	for(set<int>::const_iterator it = evenetList.begin(); it != evenetList.end(); it++)
	{
		GuideEventHelper* pTmpHelper = new GuideEventHelper(m_hEntity, *it);
		m_eventHelperList.push_back(pTmpHelper);

		pEntity->getEventServer()->subscribeEvent(*it, pTmpHelper, &GuideEventHelper::onEventArgs);
	}
}


void GuideSystem::onEventSend2Client(EventArgs& args)
{
	// 通知当前正在进行的引导
	for(set<int>::iterator it = m_opendList.begin(); it != m_opendList.end(); it++)
	{
		notifyGuide(*it);
	}
}


void GuideSystem::notifyGuide(int iGuideID)
{
	GSProto::CMD_NOTIFY_GUIDE_SC scMsg;
	scMsg.set_iguideid(iGuideID);

	IEntity* pActor = getEntityFromHandle(m_hEntity);
	assert(pActor);

	pActor->sendMessage(GSProto::CMD_NOTIFY_GUIDE, scMsg);
}


void GuideSystem::checkGuideOpen(int iEventID, EventArgs& args, set<int>& newOpenList)
{
	GuideFactory* pGuideFactory = static_cast<GuideFactory*>(getComponent<IGuideFactory>("GuideFactory", IID_IGuideFactory));
	assert(pGuideFactory);

	newOpenList = pGuideFactory->getNewOpenGuideList(m_hEntity, iEventID, args);

	/*
	// xh 添加, 针对1101和1102的特殊处理
	if(newOpenList.empty())
	{
		IEntity* playerE = getEntityFromHandle(m_hEntity);
		int level = playerE->getProperty(PROP_ENTITY_LEVEL, 0);
		if(level < 6 )
		{
			//if(!this->isGuideFinished(1101))
			//{
				newOpenList.insert(1101);
			//}
		}
		else if(level >=6 && level < 10)
		{
			//if(!this->isGuideFinished(1102))
			//{
				newOpenList.insert(1102);
			//}
		}
	}
	// xh end
	*/

	// 保存到开启队列
	std::copy(newOpenList.begin(), newOpenList.end(), std::inserter(m_opendList, m_opendList.end() ) );

	// 通知新开启引导
	for(set<int>::iterator it = newOpenList.begin(); it != newOpenList.end(); it++)
	{
		notifyGuide(*it);

		// 如果存在自动完成的，需要立即完成
		if(pGuideFactory->isAutoFinish(*it) )
		{
			finishGuide(*it);
		}
	}
}


bool GuideSystem::createComplete()
{
	return true;
}

const std::vector<Uint32>& GuideSystem::getSupportMessage()
{
	static vector<Uint32> resultList;
	if(resultList.size() == 0)
	{
		resultList.push_back(GSProto::CMD_GUIDE_FINISH);
	}

	return resultList;
}


void GuideSystem::onGuideRelationEvent(int iEventID, EventArgs& args)
{
	// 看看有无新的引导需要通知
	set<int> newGuideList;
	checkGuideOpen(iEventID, args, newGuideList);
}


void GuideSystem::onMessage(QxMessage* pMessage)
{
	assert(pMessage->dwMsgLen == sizeof(GSProto::CSMessage) );
	const GSProto::CSMessage& msg = *(const GSProto::CSMessage*)(pMessage->pMsgDataBuff);

	switch(msg.icmd() )
	{
		case GSProto::CMD_GUIDE_FINISH:
			onReqGuideFinish(msg);
			break;
	}
}


void GuideSystem::onReqGuideFinish(const GSProto::CSMessage& msg)
{
	GSProto::CMD_GUIDE_FINISH_CS req;
	if(!req.ParseFromString(msg.strmsgbody() ) )
	{
		return;
	}

	int iGuideID = req.iguideid();
	finishGuide(iGuideID);
}


bool GuideSystem::finishGuide(int iGuideID)
{
	// 验证是否是开启的
	if(!isGuideOpened(iGuideID) )
	{
		return false;
	}
	
	m_finishedList.insert(iGuideID);
	m_opendList.erase(iGuideID);

	// 触发完成引导事件
	EventArgs args;
	args.context.setInt("entity", m_hEntity);
	args.context.setInt("guideid", iGuideID);

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	pEntity->getEventServer()->setEvent(EVENT_ENTITY_FINISGUIDE, args);

	return true;
}



void GuideSystem::packSaveData(string& data)
{
	ServerEngine::GuideSystemData guideSysData;
	std::copy(m_finishedList.begin(), m_finishedList.end(), std::inserter(guideSysData.finishedList, guideSysData.finishedList.end() ) );
	std::copy(m_opendList.begin(), m_opendList.end(), std::inserter(guideSysData.openedList, guideSysData.openedList.end() ) );

	data = ServerEngine::JceToStr(guideSysData);
}

bool GuideSystem::isGuideOpened(int iGuideID)
{
	return m_opendList.find(iGuideID) != m_opendList.end();
}

bool GuideSystem::isGuideFinished(int iGuideID)
{
	return m_finishedList.find(iGuideID) != m_finishedList.end();
}


