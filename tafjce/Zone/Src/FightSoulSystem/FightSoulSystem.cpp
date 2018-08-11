#include "FightSoulSystemPch.h"
#include "FightSoul.h"
#include "FightSoulSystem.h"
#include "FightSoulFactory.h"
#include "IVIPFactory.h"

extern "C" IObject* createFightSoulSystem()
{
	return new FightSoulSystem;
}


FightSoulSystem::FightSoulSystem():m_bUserPracticeFightSoul(false)
{
	m_fightSoulBag.resize(GSProto::MAX_FIGHTSOUL_BAGSIZE, 0);
}

FightSoulSystem::~FightSoulSystem()
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	if(pEntity)
	{
		pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_SENDACTOR_TOCLIENT, this, &FightSoulSystem::onEventSend2Client);
	}

	for(size_t i = 0; i < m_fightSoulBag.size(); i++)
	{
		IFightSoul* pFightSoul = getFightSoulFromHandle(m_fightSoulBag[i]);
		if(!pFightSoul) continue;

		delete pFightSoul;
	}

	/*ITimeRangeValue* pTimeRange = getComponent<ITimeRangeValue>(COMPNAME_TimeRangeValue, IID_ITimeRangeValue);
	assert(pTimeRange);

	pTimeRange->removeRangeValue(&m_callAdvanceCount);*/
}

Uint32 FightSoulSystem::getSubsystemID() const
{
	return IID_IFightSoulSystem;
}

Uint32 FightSoulSystem::getMasterHandle()
{
	return m_hEntity;
}

bool FightSoulSystem::create(IEntity* pEntity, const std::string& strData)
{
	m_hEntity = pEntity->getHandle();

	if(strData.size() > 0)
	{
		ServerEngine::FightSoulSystemData tmpData;
		ServerEngine::JceToObj(strData, tmpData);
		initFightSoulData(tmpData);
	}
	else
	{
		m_practiceStatus.iBaseGrade = GSProto::en_practice_Grade1;
		m_practiceStatus.bMode = GSProto::en_practice_BaseMode;
	}

	pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_SENDACTOR_TOCLIENT, this, &FightSoulSystem::onEventSend2Client);

	return true;
}

void FightSoulSystem::initFightSoulData(const ServerEngine::FightSoulSystemData& saveData)
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	IFightSoulFactory* pFightSoulFactory = getComponent<IFightSoulFactory>("FightSoulFactory", IID_IFightSoulFactory);
	assert(pFightSoulFactory);

	for(map<taf::Int32, ServerEngine::FightSoulItem>::const_iterator it = saveData.fightSoulMap.begin(); it != saveData.fightSoulMap.end(); it++)
	{
		int iPos = it->first;
		if(iPos >= (int)m_fightSoulBag.size() )
		{
			PLAYERLOG(pEntity)<<"FightSoul|InvalidSize|"<<iPos<<"|"<<m_fightSoulBag.size()<<endl;
			continue;
		}
		//assert(iPos < (int)m_fightSoulBag.size() );
		
		const ServerEngine::FightSoulItem& tmpSoulItem = it->second;
		IFightSoul* pTmpFightSoul = pFightSoulFactory->createFightSoulFromDB(tmpSoulItem);
		assert(pTmpFightSoul);

		m_fightSoulBag[iPos] = pTmpFightSoul->getHandle();
	}

	m_practiceStatus = saveData.practiceStatus;
	m_callAdvanceCount.initData(saveData.callAdvanceCount, 0, 0);
	m_bUserPracticeFightSoul = saveData.bUsedPracticeFightSoul != 0;

}

bool FightSoulSystem::createComplete()
{
	return true;
}

void FightSoulSystem::onEventSend2Client(EventArgs& args)
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	GSProto::Cmd_Sc_FightSoulBag scBag;

	for(size_t i = 0; i < m_fightSoulBag.size(); i++)
	{
		FightSoul* pFightSoul = static_cast<FightSoul*>(getFightSoulFromHandle(m_fightSoulBag[i]) );
		if(!pFightSoul) continue;

		GSProto::FightSoulItem* pNewScItem = scBag.mutable_szfightsoullist()->Add();
		pFightSoul->fillScData((int)i, *pNewScItem);
	}

	int iChipCount = pEntity->getProperty(PROP_ACTOR_FSCHIPCOUNT, 0);
	scBag.set_ichipcount(iChipCount);
	GSProto::FightSoulPracticeStatus* pStatus = scBag.mutable_status();
	pStatus->set_igrade(getCurrentGrade() );
	pStatus->set_imode(m_practiceStatus.bMode);

	GSProto::SCMessage pkg;
	HelpMakeScMsg(pkg, GSProto::CMD_FIGHTSOUL_BAG, scBag);

	pEntity->sendMessage(pkg);
}

const std::vector<Uint32>& FightSoulSystem::getSupportMessage()
{
	static std::vector<Uint32> resultList;

	if(resultList.size() == 0)
	{
		resultList.push_back(GSProto::CMD_MOVE_FIGHTSOUL);
		resultList.push_back(GSProto::CMD_FIGHTSOUL_PRACTICE);
		resultList.push_back(GSProto::CMD_FIGHTSOUL_LOCK);
		resultList.push_back(GSProto::CMD_QUERY_HEROFIGHTSOUL);
		resultList.push_back(GSProto::CMD_FIGHTSOUL_AUTOCOMBINE);
		resultList.push_back(GSProto::CMD_FIGHTSOUL_ADVANCEMODE);
		resultList.push_back(GSProto::CMD_FIGHTSOUL_EXCHANGE);
		resultList.push_back(GSProto::CMD_QUERY_FIGHTSOUL);
		resultList.push_back(GSProto::CMD_FIGHTSOUL_AUTOPRACTICE);
	}
	
	return resultList;
}

void FightSoulSystem::onMessage(QxMessage* pMessage)
{
	assert(pMessage->dwMsgLen == sizeof(GSProto::CSMessage) );
	const GSProto::CSMessage& msg = *(const GSProto::CSMessage*)(pMessage->pMsgDataBuff);

	IEntity* pActor = getEntityFromHandle(m_hEntity);
	assert(pActor);

	switch(msg.icmd() )
	{
		case GSProto::CMD_MOVE_FIGHTSOUL:
			onMoveFightSoulMsg(msg);
			break;

		case GSProto::CMD_FIGHTSOUL_PRACTICE:
			practice(true);
			break;

		case GSProto::CMD_FIGHTSOUL_AUTOCOMBINE:
			onReqAutoCombine(msg);
			break;

		case GSProto::CMD_FIGHTSOUL_LOCK:
			onReqLock(msg);
			break;

		case GSProto::CMD_QUERY_HEROFIGHTSOUL:
			onReqQueryHeroFightSoul(msg);
			break;

		case GSProto::CMD_FIGHTSOUL_ADVANCEMODE:
			onReqEnableAdvance(msg);
			break;

		case GSProto::CMD_FIGHTSOUL_EXCHANGE:
			onReqExchange(msg);
			break;

		case GSProto::CMD_QUERY_FIGHTSOUL:
			pActor->sendMessage(GSProto::CMD_QUERY_FIGHTSOUL);
			break;

		case GSProto::CMD_FIGHTSOUL_AUTOPRACTICE:
			autoPractice();
			break;
	}
}


void FightSoulSystem::autoPractice()
{
	bool bResult = true;
	
	while(bResult)
	{
		bResult = practice(true);
	}
}


int FightSoulSystem::getCallAnvanceLeftCount()
{
	IVIPFactory* pVipFactory = getComponent<IVIPFactory>(COMPNAME_VIPFactory, IID_IVIPFactory);
	assert(pVipFactory);

	int iLimitCount = pVipFactory->getVipPropByHEntity(m_hEntity, VIP_PROP_ADVANCED_PRACTICE);

	if(iLimitCount <= m_callAdvanceCount.getValue() )
	{
		return 0;
	}

	return iLimitCount - m_callAdvanceCount.getValue();
}


void FightSoulSystem::onReqEnableAdvance(const GSProto::CSMessage& req)
{
	// 提示
	IScriptEngine* pScriptEngine = getComponent<IScriptEngine>(COMPNAME_ScriptEngine, IID_IScriptEngine);
	assert(pScriptEngine);

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	// 验证次数
	int iLeftCount = getCallAnvanceLeftCount();
	if(iLeftCount <= 0)
	{
		pEntity->sendErrorCode(ERROR_FIGHTSOUL_ADVANCELIMIT);
		return;
	}
	

	int iEnableCostGold = pGlobalCfg->getInt("武魂修炼高级模式消费", 200);
	
	EventArgs args;
	args.context.setInt("entity", m_hEntity);
	args.context.setInt("costGold", iEnableCostGold);
	args.context.setInt("leftCount", iLeftCount);

	pScriptEngine->runFunction("NotifyFSEnableAdvance", &args, "EventArgs");
}

void FightSoulSystem::onReqQueryHeroFightSoul(const GSProto::CSMessage& req)
{
	GSProto::Cmd_Cs_QueryHeroFightSoul queryMsg;
	if(!queryMsg.ParseFromString(req.strmsgbody() ) )
	{
		return;
	}

	HEntity hQueryHero = queryMsg.dwheroobjectid();
	IEntity* pHero = getEntityFromHandle(hQueryHero);
	if(!pHero) return;

	if(m_hEntity != (HEntity)pHero->getProperty(PROP_ENTITY_MASTER, 0))
	{
		return;
	}

	IFightSoulContainer* pFSContainer = getContainer(GSProto::en_FSConType_Wear, hQueryHero);
	if(!pFSContainer) return;

	GSProto::Cmd_Sc_QueryHeroFightSoul scMsg;
	scMsg.set_dwheroobjectid(hQueryHero);

	for(int i = 0; i < pFSContainer->getSize(); i++)
	{
		HFightSoul hTmpFightSoul = pFSContainer->getFightSoul(i);
		FightSoul* pTmpFightSoul = (FightSoul*)getFightSoulFromHandle(hTmpFightSoul);
		if(!pTmpFightSoul) continue;
	
		GSProto::FightSoulItem* pNewAdd = scMsg.mutable_szfightsoullist()->Add();
		pTmpFightSoul->fillScData(i, *pNewAdd);
	}

	GSProto::SCMessage pkg;
	HelpMakeScMsg(pkg, GSProto::CMD_QUERY_HEROFIGHTSOUL, scMsg);

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	pEntity->sendMessage(pkg);
}


int FightSoulSystem::getCurrentGrade()
{
	if(m_practiceStatus.bMode == GSProto::en_practice_AdvanceMode)
	{
		return m_practiceStatus.iAdvanceGrade;
	}
	else if(m_practiceStatus.bMode == GSProto::en_practice_BaseMode)
	{
		return m_practiceStatus.iBaseGrade;
	}
	else
	{
		assert(false);
	}

	return m_practiceStatus.iBaseGrade;
}


void FightSoulSystem::onReqLock(const GSProto::CSMessage& req)
{
	GSProto::Cmd_Cs_FightSoulLock lockMsg;
	if(!lockMsg.ParseFromString(req.strmsgbody() ) )
	{
		return;
	}

	bool bLocked = lockMsg.block();	
	int iConType = lockMsg.icontype();
	HEntity hHero = lockMsg.dwheroobjectid();
	int iPos = lockMsg.ipos();

	IFightSoulContainer* pContainer = getContainer(iConType, hHero);
	if(!pContainer) return;

	
	HFightSoul hFightSoul = pContainer->getFightSoul(iPos);
	if(0 == hFightSoul) return;

	FightSoul* pFightSoul = (FightSoul*)getFightSoulFromHandle(hFightSoul);
	if(!pFightSoul) return;

	bool bCurrentLocked = pFightSoul->isLocked();
	if(bCurrentLocked == bLocked)
	{
		return;
	}

	pFightSoul->setLock(bLocked);

	// 通知客户端
	vector<int> tmpPosList;
	tmpPosList.push_back(iPos);
	pContainer->sendContainerChg(tmpPosList);
}


void FightSoulSystem::onReqAutoCombine(const GSProto::CSMessage& req)
{
	// 确认
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	vector<int> posList;
	int iHostPos = -1;
	if(!findAutoCombHost(posList, iHostPos) )
	{
		pEntity->sendErrorCode(ERROR_NOVALID_FIGHTSOL);
		return;
	}

	IFightSoul* pHostFightSoul = getFightSoulFromHandle(getFightSoul(iHostPos) );
	assert(pHostFightSoul);

	int iGetExp = 0;
	while(posList.size() > 0)
	{
		if(pHostFightSoul->isTochMax() ) break;
	
		int iTmpPos = posList.back();
		posList.pop_back();

		if(iTmpPos == iHostPos) continue;

		IFightSoul* pTmpFightSoul = getFightSoulFromHandle(getFightSoul(iTmpPos) );
		assert(pTmpFightSoul);

		iGetExp += pTmpFightSoul->getCombineExp();
	}

	string strName = pHostFightSoul->getName();

	FightSoulFactory* pFightSoulFactory = static_cast<FightSoulFactory*>(getComponent<IFightSoulFactory>(COMPNAME_FightSoulFactory, IID_IFightSoulFactory) );
	assert(pFightSoulFactory);

	EventArgs args;
	args.context.setInt("entity", m_hEntity);
	args.context.setString("hostName", strName.c_str() );
	args.context.setInt("exp", iGetExp);
	args.context.setInt("level", pHostFightSoul->getLevel() );

	int iNewLevel = pFightSoulFactory->calcLevel(pHostFightSoul->getID(), pHostFightSoul->getExp() + iGetExp);
	args.context.setInt("newlevel", iNewLevel);

	IScriptEngine* pScriptEngine = getComponent<IScriptEngine>(COMPNAME_ScriptEngine, IID_IScriptEngine);
	assert(pScriptEngine);

	pScriptEngine->runFunction("NotifyAutoCombine", &args, "EventArgs");
}

void FightSoulSystem::autoCombine()
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	vector<int> posList;
	int iHostPos = -1;
	if(!findAutoCombHost(posList, iHostPos) )
	{
		pEntity->sendErrorCode(ERROR_NOVALID_FIGHTSOL);
		return;
	}

	IFightSoul* pHostFightSoul = getFightSoulFromHandle(getFightSoul(iHostPos) );
	assert(pHostFightSoul);

	int iPreHostLv = pHostFightSoul->getLevel();
	vector<int> chgPosList;
	while(posList.size() > 0)
	{
		if(pHostFightSoul->isTochMax() ) break;
	
		int iTmpPos = posList.back();
		posList.pop_back();

		if(iTmpPos == iHostPos) continue;

		chgPosList.push_back(iTmpPos);
		
		IFightSoul* pTmpFightSoul = getFightSoulFromHandle(getFightSoul(iTmpPos) );
		assert(pTmpFightSoul);

		pHostFightSoul->addExp(pTmpFightSoul->getCombineExp());
		setFightSoul(iTmpPos, 0, NULL);
		delete pTmpFightSoul;
	}

	chgPosList.push_back(iHostPos);
	sendContainerChg(chgPosList);

	if(iPreHostLv != pHostFightSoul->getLevel() )
	{
		EventArgs args;
		args.context.setInt("soulLevel", pHostFightSoul->getLevel());
		args.context.setInt("entity",m_hEntity);
		pEntity->getEventServer()->setEvent(EVENT_ENTITY_TASKFINISH_SOUL, args);
	}
}

bool FightSoulSystem::findAutoCombHost(vector<int>& posList, int& iTargetPos)
{
	int iHostTmpPos = -1;
	IFightSoul* pHostFightSoul = NULL;
	for(int i = 0; i < getSize(); i++)
	{
		IFightSoul* pFightSoul = getFightSoulFromHandle(getFightSoul(i) );
		if(!pFightSoul) continue;

		if(pFightSoul->isLocked() || pFightSoul->isTochMax() ) continue;
		
		posList.push_back(i);

		// 选择最好的
		if(iHostTmpPos < 0)
		{
			iHostTmpPos = i;
			pHostFightSoul = pFightSoul;
			continue;
		}

		// 优先品质
		if(pFightSoul->getQuality() < pHostFightSoul->getQuality() )
		{
			continue;
		}

		if((pFightSoul->getQuality() > pHostFightSoul->getQuality()) || (pFightSoul->getExp() > pHostFightSoul->getExp() ) )
		{
			iHostTmpPos = i;
			pHostFightSoul = pFightSoul;
			continue;
		}
	}

	// 如果只有一个，也没啥好合并的。。
	if( (posList.size() > 1) && (iHostTmpPos >= 0) )
	{
		iTargetPos = iHostTmpPos;
		return true;
	}

	return false;
}


void FightSoulSystem::onMoveFightSoulMsg(const GSProto::CSMessage& req)
{
	GSProto::Cmd_Cs_MoveFightSoul movReq;
	if(!movReq.ParseFromString(req.strmsgbody() ) )
	{
		return;
	}

	int iSrcConType = movReq.isrccontype();
	int iSrcPos = movReq.isrcpos();

	int iDstConType = movReq.idstcontype();
	int iDstPos = movReq.idstpos();

	HEntity hTargetHero = movReq.dwheroobjectid();
	_doFSMove(iSrcConType, iSrcPos, iDstConType, iDstPos, hTargetHero, false);
}

void FightSoulSystem::_doFSMove(int iSrcConType, int iSrcPos, int iDstConType, int iDstPos, HEntity hTargetHero, bool bConfirm)
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	FightSoulFactory* pFightSoulFactory = static_cast<FightSoulFactory*>(getComponent<IFightSoulFactory>(COMPNAME_FightSoulFactory, IID_IFightSoulFactory) );
	assert(pFightSoulFactory);

	// 验证容器
	IFightSoulContainer* pSrcContainer = getContainer(iSrcConType, hTargetHero);
	IFightSoulContainer* pDstContainer = getContainer(iDstConType, hTargetHero);

	if(!checkMoveValid(pSrcContainer, iSrcPos, pDstContainer, iDstPos) )
	{
		return;
	}

	HFightSoul hSrcFightSoul = pSrcContainer->getFightSoul(iSrcPos);
	IFightSoul* pSrcFightSoul = getFightSoulFromHandle(hSrcFightSoul);
	assert(pSrcFightSoul);
	
	// 如果目标有，吃掉Src武魂
	HFightSoul hDstFightSoul = pDstContainer->getFightSoul(iDstPos); 
	IFightSoul* pDstFightSoul = getFightSoulFromHandle(hDstFightSoul);
	if(pDstFightSoul)
	{
		// 发送确认框
		if(!bConfirm)
		{
			EventArgs args;
			args.context.setInt("entity", m_hEntity);
			args.context.setInt("srcConType", iSrcConType);
			args.context.setInt("srcPos", iSrcPos);
			args.context.setInt("dstConType", iDstConType);
			args.context.setInt("dstPos", iDstPos);
			args.context.setInt("heroObjectID", hTargetHero);

			string strSrcName = pSrcFightSoul->getName().c_str();
			string strDstName = pDstFightSoul->getName();
			args.context.setString("scrName", strSrcName.c_str() );
			args.context.setString("dstName", strDstName.c_str() );
			args.context.setInt("exp", pSrcFightSoul->getCombineExp());

			int iDstNewLv = pFightSoulFactory->calcLevel(pDstFightSoul->getID(), pDstFightSoul->getExp() + pSrcFightSoul->getCombineExp() );
			args.context.setInt("level", pDstFightSoul->getLevel() );
			args.context.setInt("newlevel", iDstNewLv);
			

			IScriptEngine* pScriptEngine = getComponent<IScriptEngine>(COMPNAME_ScriptEngine, IID_IScriptEngine);
			assert(pScriptEngine);
			pScriptEngine->runFunction("sendFightSoulCmbConfirm", &args, "EventArgs");
			return;
		}

		pSrcContainer->setFightSoul(iSrcPos, 0, NULL);	
		pDstContainer->setFightSoul(iDstPos, 0, NULL);

		int iDstPreLevel = pDstFightSoul->getLevel();
		pDstFightSoul->addExp(pSrcFightSoul->getCombineExp() );

		// 如果武魂升级了，触发任务事件
		if(iDstPreLevel != pDstFightSoul->getLevel() )
		{
			EventArgs args;
			args.context.setInt("soulLevel", pDstFightSoul->getLevel());
			args.context.setInt("entity",m_hEntity);
			pEntity->getEventServer()->setEvent(EVENT_ENTITY_TASKFINISH_SOUL, args);
		}
		
		// 删掉原武魂
		delete pSrcFightSoul;
		pDstContainer->setFightSoul(iDstPos, hDstFightSoul, NULL);
	}
	else
	{
		pSrcContainer->setFightSoul(iSrcPos, 0, NULL);	
		pDstContainer->setFightSoul(iDstPos, hSrcFightSoul, NULL);
	}

	// 发送
	vector<int> sendPosList;
	if(pSrcContainer == pDstContainer)
	{
		sendPosList.push_back(iSrcPos);
		sendPosList.push_back(iDstPos);
		pDstContainer->sendContainerChg(sendPosList);
	}
	else
	{
		sendPosList.push_back(iSrcPos);
		pSrcContainer->sendContainerChg(sendPosList);
		sendPosList.clear();
		sendPosList.push_back(iDstPos);
		pDstContainer->sendContainerChg(sendPosList);
	}
}

void FightSoulSystem::combine(int iSrcConType, int iSrcPos, int iDstConType, int iDstPos, Uint32 dwHeroObjectID)
{
	_doFSMove(iSrcConType, iSrcPos, iDstConType, iDstPos, dwHeroObjectID, true);
}

bool FightSoulSystem::checkMoveValid(IFightSoulContainer* pSrcContainer, int iSrcPos, IFightSoulContainer* pDstContainer, int iDstPos)
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	if(!pSrcContainer || !pDstContainer)
	{
		return false;
	}

	// 验证位置合法
	if( (iSrcPos < 0) || (iDstPos < 0) || (iSrcPos >= pSrcContainer->getSize() ) || (iDstPos >= pDstContainer->getSize() ) )
	{
		return false;
	}
	
	// Src 必须有对象
	HFightSoul hSrcFightSoul = pSrcContainer->getFightSoul(iSrcPos);
	IFightSoul* pSrcFightSoul = getFightSoulFromHandle(hSrcFightSoul);
	if(!pSrcFightSoul) return false;

	// 验证是否可以添加到目标
	if(!pDstContainer->canSetFightSoul(iDstPos, hSrcFightSoul, true) )
	{
		return false;
	}

	HFightSoul hDstFightSoul = pDstContainer->getFightSoul(iDstPos); 
	// 锁定了,不能被吃掉
	if( (pSrcFightSoul->isLocked() ) && (0 != hDstFightSoul) )
	{
		pEntity->sendErrorCode(ERROR_FIGHTSOUL_LOCKED);
		return false;
	}

	// 目标是否已经满了
	IFightSoul* pDstFightSoul = getFightSoulFromHandle(hDstFightSoul);
	if(pDstFightSoul && pDstFightSoul->isTochMax() )
	{
		pEntity->sendErrorCode(ERROR_FIGHTSOUL_TOUCHMAX);
		return false;
	}

	return true;
}


void FightSoulSystem::sendContainerChg(const vector<int>& posList)
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	GSProto::Cmd_Sc_ChgFsContainer scMsg;
	scMsg.set_iobjecttype(GSProto::en_class_Actor);
	scMsg.set_dwobjectid(m_hEntity);
	scMsg.set_icontainertype(GSProto::en_FSConType_Bag);

	for(size_t i = 0; i < posList.size(); i++)
	{
		int iPos = posList[i];
		assert( (iPos >=0) && (iPos < (int)getSize() ) );
		FightSoul* pTmpFightSoul = (FightSoul*)getFightSoulFromHandle(getFightSoul(iPos) );
		GSProto::FightSoulChgItem* pChgItem = scMsg.mutable_szchglist()->Add();

		pChgItem->set_ipos(iPos);
		if(!pTmpFightSoul)
		{
			pChgItem->set_bdel(true);
		}
		else
		{
			pChgItem->set_bdel(false);
			pTmpFightSoul->fillScData(iPos, *pChgItem->mutable_fightsouldata() );
		}
	}

	GSProto::SCMessage pkg;
	HelpMakeScMsg(pkg, GSProto::CMD_CHG_FSCONTAINER, scMsg);
	pEntity->sendMessage(pkg);
}


IFightSoulContainer* FightSoulSystem::getContainer(int iType, HEntity hHero)
{
	IFightSoulContainer* pResultContainer = NULL;
	if(GSProto::en_FSConType_Bag == iType)
	{
		pResultContainer = (IFightSoulContainer*)this;
	}
	else if(GSProto::en_FSConType_Wear == iType)
	{
		if(0 == hHero) return NULL;

		IEntity* pTargetHero = getEntityFromHandle(hHero);
		if(!pTargetHero) return NULL;

		// 验证hero合法性
		HEntity hMaster = pTargetHero->getProperty(PROP_ENTITY_MASTER, 0);
		if(hMaster != m_hEntity) return NULL;

		pResultContainer = (IFightSoulContainer*)pTargetHero->querySubsystem(IID_IHeroEqupSystem)->queryInterface(IID_IFightSoulContainer);
	}

	return pResultContainer;
}


void FightSoulSystem::packSaveData(string& data)
{
	ServerEngine::FightSoulSystemData tmpSaveData;
	for(size_t i = 0; i < m_fightSoulBag.size(); i++)
	{
		IFightSoul* pTmpFightSoul = getFightSoulFromHandle(m_fightSoulBag[i]);
		if(!pTmpFightSoul) continue;

		ServerEngine::FightSoulItem tmpItem;
		tmpItem.iBaseID = pTmpFightSoul->getID();
		tmpItem.iExp = pTmpFightSoul->getExp();
		tmpItem.bLocked = pTmpFightSoul->isLocked();
		tmpSaveData.fightSoulMap[i] = tmpItem;
	}

	tmpSaveData.practiceStatus = m_practiceStatus;
	m_callAdvanceCount.fillData(tmpSaveData.callAdvanceCount);
	tmpSaveData.bUsedPracticeFightSoul = m_bUserPracticeFightSoul;
	
	data = ServerEngine::JceToStr(tmpSaveData);
}

void FightSoulSystem::enableAdvanceMode()
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	// 先验证次数
	int iLeftCount = getCallAnvanceLeftCount();
	if(iLeftCount <= 0)
	{
		pEntity->sendErrorCode(ERROR_FIGHTSOUL_ADVANCELIMIT);
		return;
	}

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	int iEnableCostGold = pGlobalCfg->getInt("武魂修炼高级模式消费", 200);

	int iTkGold = pEntity->getProperty(PROP_ACTOR_GOLD, 0);
	if(iTkGold < iEnableCostGold)
	{
		pEntity->sendErrorCode(ERROR_NEED_GOLD);
		return;
	}

	pEntity->changeProperty(PROP_ACTOR_GOLD, 0- iEnableCostGold, GSProto::en_Reason_FightSoulAdvanceMode);
	
	m_practiceStatus.bMode = GSProto::en_practice_AdvanceMode;
	m_practiceStatus.iAdvanceGrade = GSProto::en_practice_Grade4;

	// 通知客户端消息
	m_callAdvanceCount.setValue(m_callAdvanceCount.getValue() + 1);
	
	GSProto::Cmd_Sc_FightSoulPractice scMsg;
	scMsg.mutable_status()->set_igrade(getCurrentGrade() );
	scMsg.mutable_status()->set_imode(m_practiceStatus.bMode);

	GSProto::SCMessage pkg;
	HelpMakeScMsg(pkg, GSProto::CMD_FIGHTSOUL_PRACTICE, scMsg);
	pEntity->sendMessage(pkg);
}

bool FightSoulSystem::practice(bool bNotofyError)
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	FightSoulFactory* pFightSoulFactory = (FightSoulFactory*)(getComponent<IFightSoulFactory>("FightSoulFactory", IID_IFightSoulFactory) );
	assert(pFightSoulFactory);

	GSProto::SCMessage failPkg;
	failPkg.set_icmd(GSProto::CMD_FIGHTSOUL_PRACTICEFAIL);

	// 检测银两是否OK
	int iCurrentGrade = getCurrentGrade();
	int iNeedSilver = pFightSoulFactory->calcPracticeSilver(iCurrentGrade);
	if(!m_bUserPracticeFightSoul)
	{
		m_bUserPracticeFightSoul = true;
		iNeedSilver = 0;
	}
	
	int iTkSilver = pEntity->getProperty(PROP_ACTOR_SILVER, 0);
	if(iTkSilver < iNeedSilver)
	{
		if(bNotofyError) pEntity->sendErrorCode(ERROR_NEED_SILVER);
		pEntity->sendMessage(failPkg);
		return false;
	}

	// 满了，不允许
	if(isFull() )
	{
		if(bNotofyError) pEntity->sendErrorCode(ERROR_FIGHTSOUL_BAGFULL);
		pEntity->sendMessage(failPkg);
		return false;
	}

	// 扣除银币
	pEntity->changeProperty(PROP_ACTOR_SILVER, 0-iNeedSilver, GSProto::en_Reason_FightSoulPractice);

	// 在当前等级随机个
	const FightPractice* pPracticeCfg = pFightSoulFactory->querFightPracticeCfg(iCurrentGrade, m_practiceStatus.bMode);
	assert(pPracticeCfg);

	// 计算获取的奖励
	IRandom* pRandom = getComponent<IRandom>("Random", IID_IMiniAprRandom);
	assert(pRandom);
	
	int iAwardRandV = pRandom->random()%10000;
	int iSumChance = 0;
	int iAwardFightSoulID = 0;
	for(size_t i = 0; i < pPracticeCfg->chanceList.size(); i++)
	{
		iSumChance += pPracticeCfg->chanceList[i].iChance;
		if(iAwardRandV < iSumChance)
		{
			iAwardFightSoulID = pPracticeCfg->chanceList[i].iFightSoulID;
			break;
		}
	}

	if(m_practiceStatus.bMode == GSProto::en_practice_AdvanceMode)
	{
		pEntity->changeProperty(PROP_ACTOR_FSCHIPCOUNT, 1, GSProto::en_Reason_FightSoulPractice);
	}

	// 判断下一等级和模式
	int iNextGradeChance = pFightSoulFactory->getNextGradeChance(iCurrentGrade, m_practiceStatus.bMode);
	int iRandV = pRandom->random() % 10000;
	if(iRandV < iNextGradeChance)
	{
		if(GSProto::en_practice_BaseMode == m_practiceStatus.bMode)
		{
			m_practiceStatus.iBaseGrade++;
		}
		else if(GSProto::en_practice_AdvanceMode == m_practiceStatus.bMode)
		{
			m_practiceStatus.iAdvanceGrade++;
		}
	}
	else
	{
		if(GSProto::en_practice_BaseMode == m_practiceStatus.bMode)
		{
			m_practiceStatus.iBaseGrade = GSProto::en_practice_Grade1;
			m_practiceStatus.bMode = GSProto::en_practice_BaseMode;
		}
		else if(GSProto::en_practice_AdvanceMode == m_practiceStatus.bMode)
		{
			m_practiceStatus.bMode = GSProto::en_practice_BaseMode;
		}
	}

	// 通知客户端消息
	GSProto::Cmd_Sc_FightSoulPractice scMsg;
	scMsg.mutable_status()->set_igrade(getCurrentGrade() );
	scMsg.mutable_status()->set_imode(m_practiceStatus.bMode);

	GSProto::SCMessage pkg;
	HelpMakeScMsg(pkg, GSProto::CMD_FIGHTSOUL_PRACTICE, scMsg);
	pEntity->sendMessage(pkg);

	// 发奖励(应客户端要求，发放放到这里)
	if(iAwardFightSoulID > 0)
	{
		HFightSoul hNewFightSoul = addFightSoul(iAwardFightSoulID, false);
		FightSoul* pNewFightSoul = static_cast<FightSoul*>(getFightSoulFromHandle(hNewFightSoul) );
		assert(pNewFightSoul);
		if(pNewFightSoul->getQuality() >= GSProto::en_Quality_Gold)
		{
			notifyGoldFightSoul(iAwardFightSoulID);
		}
	}

	//任务系统
	{
		EventArgs args;
		args.context.setInt("times", 1);
		args.context.setInt("entity",m_hEntity);
		pEntity->getEventServer()->setEvent(EVENT_ENTITY_TASKFINISH_FIGHTSOULTRAIN, args);
	}

	return true;
}

bool FightSoulSystem::isFull()
{
	vector<HFightSoul>::iterator it = find(m_fightSoulBag.begin(), m_fightSoulBag.end(), HEntity() );
	if(it == m_fightSoulBag.end() )
	{
		return true;
		
	}
	
	return false;
}


Uint32 FightSoulSystem::addFightSoul(int iFightSoulID, bool bNotifyErr)
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	if(isFull() )
	{
		if(bNotifyErr) pEntity->sendErrorCode(ERROR_FIGHTSOUL_BAGFULL);
		return 0;
	}

	IFightSoulFactory* pFightSoulFactory = getComponent<IFightSoulFactory>("FightSoulFactory", IID_IFightSoulFactory);
	assert(pFightSoulFactory);

	IFightSoul* pFightSoul = pFightSoulFactory->createFightSoul(iFightSoulID);
	assert(pFightSoul);

	vector<HFightSoul>::iterator it = find(m_fightSoulBag.begin(), m_fightSoulBag.end(), HEntity() );
	assert(it != m_fightSoulBag.end() );

	int iPos = (int)std::distance(m_fightSoulBag.begin(), it);
	setFightSoul(iPos, pFightSoul->getHandle(), NULL);

	vector<int> tmpList;
	tmpList.push_back(iPos);
	sendContainerChg(tmpList);

	PLAYERLOG(pEntity)<<"AddFightSoul|"<<iFightSoulID<<endl;

	{
		EventArgs args;
		args.context.setInt("soulquality",pFightSoul->getQuality());
		args.context.setInt("entity",m_hEntity);
		pEntity->getEventServer()->setEvent(EVENT_ENTITY_TAKS_SOULQUALITY,args);
	}
	
	return pFightSoul->getHandle();
}


void FightSoulSystem::notifyGoldFightSoul(int iFightSoulID)
{
	GSProto::CMD_MARQUEE_SC scMsg;
	scMsg.set_marqueeid(GSProto::en_marQueue_FightSoul);

	IEntity* pActor = getEntityFromHandle(m_hEntity);
	assert(pActor);

	string strActorName = pActor->getProperty(PROP_ENTITY_NAME, "");

	scMsg.add_szstrmessage(strActorName);
	scMsg.add_szstrmessage(TC_Common::tostr(iFightSoulID) );

	IJZMessageLayer* pMsgLayer = getComponent<IJZMessageLayer>(COMPNAME_MessageLayer, IID_IJZMessageLayer);
	assert(pMsgLayer);

	GSProto::SCMessage pkg;
	HelpMakeScMsg(pkg, GSProto::CMD_MARQUEE, scMsg);
	
	pMsgLayer->broadcastMsg(pkg);
}


Uint32 FightSoulSystem::getHFightSoulFromPos(int iPos)
{
	if( (iPos < 0) || (iPos >= (int)m_fightSoulBag.size() ) )
	{
		return 0;
	}

	return m_fightSoulBag[iPos];
}

void* FightSoulSystem::queryInterface(int iInterfaceID)
{
	if(IID_IFightSoulContainer == iInterfaceID)
	{
		return (IFightSoulContainer*)this;
	}

	return NULL;
}

Int32 FightSoulSystem::getSpace()
{
	return (Int32)m_fightSoulBag.size();
}

Int32 FightSoulSystem::getSize()
{
	return (Int32)m_fightSoulBag.size();
}

Uint32 FightSoulSystem::getFightSoul(int iPos)
{
	//assert( (iPos >= 0) && (iPos < (int)m_fightSoulBag.size() ) );
	if( (iPos < 0) || (iPos >= (int)m_fightSoulBag.size() ) )
	{
		return 0;
	}

	return m_fightSoulBag[iPos];
}

void FightSoulSystem::onReqExchange(const GSProto::CSMessage& req)
{
	GSProto::Cmd_Cs_FightSoulExchange scMsg;
	if(!scMsg.ParseFromString(req.strmsgbody() ) )
	{
		return;
	}

	int iExchangeID = scMsg.iexchangeid();
	exchangeFightSoul(iExchangeID);
}


void FightSoulSystem::exchangeFightSoul(int iExchangeID)
{
	FightSoulFactory* pFightSoulFactory = (FightSoulFactory*)(getComponent<IFightSoulFactory>("FightSoulFactory", IID_IFightSoulFactory) );
	assert(pFightSoulFactory);

	const FightSoulExchange* pExchangeCfg = pFightSoulFactory->queryExchange(iExchangeID);
	if(!pExchangeCfg) return;

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	int iTkChipCount = pEntity->getProperty(PROP_ACTOR_FSCHIPCOUNT, 0);
	if(iTkChipCount < pExchangeCfg->iNeedChipCount)
	{
		pEntity->sendErrorCode(ERROR_NEED_CHIP);
		return;
	}

	// 查看背包是否足够
	if(isFull() )
	{
		pEntity->sendErrorCode(ERROR_FIGHTSOUL_BAGFULL);
		return;
	}

	// 执行兑换
	pEntity->changeProperty(PROP_ACTOR_FSCHIPCOUNT, 0-pExchangeCfg->iNeedChipCount, GSProto::en_Reason_FightSoulExchange);
	addFightSoul(pExchangeCfg->iTargetBaseID, true);
}

bool FightSoulSystem::setFightSoul(int iPos, Uint32 hFightSoulHandle, GSProto::FightSoulChgItem* pOutItem)
{
	assert( (iPos >= 0) && (iPos < (int)m_fightSoulBag.size() ) );

	m_fightSoulBag[iPos] = hFightSoulHandle;

	if( (0 == hFightSoulHandle) && pOutItem)
	{	
		pOutItem->set_ipos(iPos);
		pOutItem->set_bdel(true);
		return true;
	}

	if(pOutItem)
	{
		pOutItem->set_ipos(iPos);
		pOutItem->set_ipos(iPos);
		pOutItem->set_bdel(false);

		FightSoul* pFightSoul = static_cast<FightSoul*>(getFightSoulFromHandle(hFightSoulHandle) );
		assert(pFightSoul);

		pFightSoul->fillScData(iPos, *pOutItem->mutable_fightsouldata() );
	}
	
	return true;
}


HEntity FightSoulSystem::getOwner()
{
	return m_hEntity;
}

bool FightSoulSystem::canSetFightSoul(int iPos, Uint32 hFightSoulHandle, bool bNoitifyErr)
{
	return true;
}





