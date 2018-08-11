#include "FormationSystemPch.h"
#include "FormationSystem.h"
#include "ErrorCodeDef.h"
#include "IGodAnimalSystem.h"

extern "C" IObject* createFormationSystem()
{
	return new FormationSystem;
}


FormationSystem::FormationSystem()
	:m_bClose(false)
{
	m_formationList.resize(GSProto::MAX_BATTLE_MEMBER_SIZE/2, 0);
}

FormationSystem::~FormationSystem()
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	if(pEntity)
	{
		pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_SENDACTOR_TOCLIENT, this, &FormationSystem::onSendClientFormation);
		pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_DELHERO, this, &FormationSystem::onEventDestroyHero);
		pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_LEVELUP, this, &FormationSystem::onEventLevelUp);
		pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_ACTIVEGODANIMAL_CHG, this, &FormationSystem::onEventActiveGodAnimalChg);
	}
}

Uint32 FormationSystem::getSubsystemID() const
{
	return IID_IFormationSystem;
}

Uint32 FormationSystem::getMasterHandle()
{
	return m_hEntity;
}

bool FormationSystem::create(IEntity* pEntity, const std::string& strData)
{
	assert(pEntity);
	m_hEntity = pEntity->getHandle();

	m_strCacheData = strData; 
	
	return true;
}

bool FormationSystem::createComplete()
{
	initFormation(m_strCacheData);

	sumFightValue();

	// 注册事件
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_SENDACTOR_TOCLIENT, this, &FormationSystem::onSendClientFormation);
	pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_DELHERO, this, &FormationSystem::onEventDestroyHero);
	pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_LEVELUP, this, &FormationSystem::onEventLevelUp);
	pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_ACTIVEGODANIMAL_CHG, this, &FormationSystem::onEventActiveGodAnimalChg);


	registerHeroFightValueChg();

	IGodAnimalSystem* pGodAnimalSys = static_cast<IGodAnimalSystem*>(pEntity->querySubsystem(IID_IGodAnimalSystem));
	assert(pGodAnimalSys);
	registerGodAnimalEvent(0, pGodAnimalSys->getActiveGodAnimal() );

	int iHeadId = pEntity->getProperty(PROP_ENTITY_ACTOR_HEAD, 0);
	GSProto::enHeadType iheadType = (GSProto::enHeadType)getProperty(PROP_ENTITY_ACTOR_HEADTYPE, 0);
	if(iHeadId == 0)
	{
		iheadType = GSProto::en_HeadType_Hero;
		IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
		assert(pGlobalCfg);
		int iInitHeroID = pGlobalCfg->getInt("初始英雄ID", 1014);
		iHeadId = iInitHeroID;

		pEntity->setProperty(PROP_ENTITY_ACTOR_HEADTYPE, iheadType);
		pEntity->setProperty( PROP_ENTITY_ACTOR_HEAD, iHeadId);
	}

	return true;
}

void FormationSystem::onEventHeroFightChg(EventArgs& args)
{
	sumFightValue();
}

void FormationSystem::onEventActiveGodAnimalChg(EventArgs& args)
{
	// 重新计算，重新注册事件
	HEntity hOld = args.context.getInt("old");
	HEntity hCur = args.context.getInt("cur");

	registerGodAnimalEvent(hOld, hCur);
	sumFightValue();
}

void FormationSystem::registerGodAnimalEvent(HEntity hOld, HEntity hCur)
{
	IEntity* pOldGod = getEntityFromHandle(hOld);
	if(pOldGod)
	{
		pOldGod->getEventServer()->unsubscribeEvent(EVENT_ENTITY_FIGHTVALUECHG, this, &FormationSystem::onEventGodAnimaFightValueChg);
	}

	// 注册新的
	IEntity* pCurGod = getEntityFromHandle(hCur);
	if(pCurGod)
	{
		pCurGod->getEventServer()->subscribeEvent(EVENT_ENTITY_FIGHTVALUECHG, this, &FormationSystem::onEventGodAnimaFightValueChg);
	}
}

void FormationSystem::onEventGodAnimaFightValueChg(EventArgs& args)
{
	sumFightValue();
}


void FormationSystem::sumFightValue()
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	int iSumFightValue = 0;
	for(size_t i = 0; i < m_formationList.size(); i++)
	{		
		HEntity hTmpEntity = m_formationList[i];
		IEntity* pTmpHero = getEntityFromHandle(hTmpEntity);
		if(!pTmpHero) continue;

		iSumFightValue += pTmpHero->getProperty(PROP_ENTITY_FIGHTVALUE, 0);
	}

	// 计算下神兽
	IGodAnimalSystem* pGodAnimalSys = static_cast<IGodAnimalSystem*>(pEntity->querySubsystem(IID_IGodAnimalSystem));
	assert(pGodAnimalSys);

	IEntity* pGodAnimal = getEntityFromHandle(pGodAnimalSys->getActiveGodAnimal() );
	if(pGodAnimal)
	{
		iSumFightValue += pGodAnimal->getProperty(PROP_ENTITY_FIGHTVALUE, 0);
	}

	pEntity->setProperty(PROP_ENTITY_FIGHTVALUE, iSumFightValue);

	EventArgs tmpArgs;
	tmpArgs.context.setInt("entity", m_hEntity);
	pEntity->getEventServer()->setEvent(EVENT_ENTITY_FIGHTVALUECHG, tmpArgs);
}

void FormationSystem::registerHeroFightValueChg()
{	
	for(size_t i = 0; i < m_formationList.size(); i++)
	{		
		HEntity hTmpEntity = m_formationList[i];
		IEntity* pTmpHero = getEntityFromHandle(hTmpEntity);
		if(!pTmpHero) continue;

		pTmpHero->getEventServer()->subscribeEvent(EVENT_ENTITY_FIGHTVALUECHG, this, &FormationSystem::onEventHeroFightChg);
	}
}

void FormationSystem::unRegisterHeroFightValueChg()
{
	for(size_t i = 0; i < m_formationList.size(); i++)
	{		
		HEntity hTmpEntity = m_formationList[i];
		IEntity* pTmpHero = getEntityFromHandle(hTmpEntity);
		if(!pTmpHero) continue;

		pTmpHero->getEventServer()->unsubscribeEvent(EVENT_ENTITY_FIGHTVALUECHG, this, &FormationSystem::onEventHeroFightChg);
	}
}


const std::vector<Uint32>& FormationSystem::getSupportMessage()
{
	static vector<Uint32> resultList;

	if(resultList.size() == 0)
	{
		resultList.push_back(GSProto::CMD_FORMATION_CHG);
	}

	return resultList;
}

void FormationSystem::setChgFormationSwitchState(bool bClose)
{
	m_bClose = bClose;
}

void FormationSystem::onMessage(QxMessage* pMessage)
{
	//如果关闭了队形切换，那么客户端消息不合法 
	if(m_bClose)
	{
		return;
	}
	
	assert(pMessage->dwMsgLen == sizeof(GSProto::CSMessage) );
	const GSProto::CSMessage& msg = *(const GSProto::CSMessage*)(pMessage->pMsgDataBuff);

	switch(msg.icmd() )
	{
		case GSProto::CMD_FORMATION_CHG:

			// 简单粗暴点
			unRegisterHeroFightValueChg();
			onFormationChgMsg(msg);
			registerHeroFightValueChg();

			// 发送消息给客户端的时候会重新计算
			//sumFightValue();

			break;


	}
}

int FormationSystem::getVisibleHeroID()
{
	//这么写 有点~~~~
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	return pEntity->getProperty( PROP_ENTITY_ACTOR_HEAD,0);
}

void FormationSystem::onFormationChgMsg(const GSProto::CSMessage& message)
{
	GSProto::Cmd_Cs_FormationChg chgReq;
	if(!chgReq.ParseFromString(message.strmsgbody() ) )
	{
		return;
	}

	// 验证英雄是否合法
	vector<HEntity> csFormationList;
	csFormationList.resize(GSProto::MAX_BATTLE_MEMBER_SIZE/2,0);
	
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	int iLimitCount = getFormationHeroLimit();
	if(chgReq.szformationlist_size()  > iLimitCount)
	{
		pEntity->sendErrorCode(ERROR_FORMATION_LIMIT);
		return;
	}
	
	for(int i = 0; i < chgReq.szformationlist_size(); ++i)
	{
		const GSProto::FormationItemData& data = chgReq.szformationlist( i);
		IEntity* pHeroEntity = getEntityFromHandle(data.dwobjectid());
		if( !pHeroEntity)return;
		HEntity hMaster = pHeroEntity->getProperty(PROP_ENTITY_MASTER, 0);
		if(m_hEntity != hMaster) return;
		int ipos = data.ipos();
		
		if( -1 > ipos && ipos > GSProto::MAX_BATTLE_MEMBER_SIZE/2 )
		{
			return;
		}
		
		if(ipos%3 == 2) return;
		unsigned int dwObjcetId = data.dwobjectid();
		//验证唯一性
		for(size_t j = 0; j < csFormationList.size(); ++j)
		{
			if( dwObjcetId == csFormationList[ j ] )
			{
				return;
			}
		}
		csFormationList[ipos] = data.dwobjectid();
	}
	int iNotZeroCount = (int)std::count_if(csFormationList.begin(), csFormationList.end(), bind2nd(not_equal_to<Uint32>(), 0));
	if(iNotZeroCount == 0)
	{
		pEntity->sendErrorCode(ERROR_FORMATION_CANNOTEMPTY);
		return;
	}
	
	int iLasttCount = (int)std::count_if(m_formationList.begin(), m_formationList.end(), bind2nd(not_equal_to<Uint32>(), 0));
	m_formationList = csFormationList;
	int iCurrentCount = (int)std::count_if(m_formationList.begin(), m_formationList.end(), bind2nd(not_equal_to<Uint32>(), 0));
	if(iLasttCount < iCurrentCount)
	{
		EventArgs args;
		args.context.setInt("formationCount",iCurrentCount);
		pEntity->getEventServer()->setEvent( EVENT_ENTITY_TASK_FORMATION,args);
	}

	sendFormationData();
}

int FormationSystem::getHeroPosFromBaseID(int iBaseID)
{
	for(size_t i = 0; i < m_formationList.size(); i++)
	{
		IEntity* pTmpEntity = getEntityFromHandle(m_formationList[i]);
		if(!pTmpEntity) continue;

		int iTmpHeroID = pTmpEntity->getProperty(PROP_ENTITY_BASEID, 0);
		if(iTmpHeroID == iBaseID)
		{
			return (int)i;
		}
	}

	return -1;
}

/*void FormationSystem::putOn(HEntity hMoveEntity, int iPos)
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	IEntity* pMoveEntity = getEntityFromHandle(hMoveEntity);
	if(!pMoveEntity) return;

	int iHeroID = pMoveEntity->getProperty(PROP_ENTITY_BASEID, 0);
	int iSameIDPos = getHeroPosFromBaseID(iHeroID);

	// 如果已经有相同的英雄，并且不会被替换下，则提示错误
	if( (iSameIDPos >=0) && (iSameIDPos != iPos) )
	{
		pEntity->sendErrorCode(ERROR_FORMATION_SAMEID);
		return;
	}

	// 直接替换
	if(0 != m_formationList[iPos])
	{
		m_formationList[iPos] = hMoveEntity;
		sendFormationData();
		return;
	}

	// 新增，验证上阵人数限制
	int iLimitCount = getFormationHeroLimit();
	int iCurrentCount = (int)std::count_if(m_formationList.begin(), m_formationList.end(), bind2nd(not_equal_to<Uint32>(), 0));

	if(iCurrentCount >= iLimitCount)
	{
		pEntity->sendErrorCode(ERROR_FORMATION_LIMIT);
		return;
	}

	m_formationList[iPos] = hMoveEntity;
	sendFormationData();
	
}

void FormationSystem::chagePos(HEntity hMoveEntity, int iSrcPos, int iTargetPos)
{
	HEntity hTmp = m_formationList[iTargetPos];

	m_formationList[iTargetPos] = hMoveEntity;
	m_formationList[iSrcPos] = hTmp;

	sendFormationData();
}

void FormationSystem::putDown(HEntity hMoveEntity)
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	// 先判断是否在阵形上
	vector<HEntity>::iterator it = std::find(m_formationList.begin(), m_formationList.end(), hMoveEntity);
	if(it == m_formationList.end() )
	{
		pEntity->sendErrorCode(ERROR_NOT_ONFORMATION);
		return;
	}
	
	// 不能下阵所有人
	int iCurrentCount = (int)std::count_if(m_formationList.begin(), m_formationList.end(), bind2nd(not_equal_to<Uint32>(), 0));
	if(iCurrentCount == 1)
	{
		pEntity->sendErrorCode(ERROR_FORMATION_CANNOTEMPTY);
		return;
	}

	*it = 0;
	sendFormationData();
}
*/

void FormationSystem::packSaveData(string& data)
{
	ServerEngine::FormationSystemData saveData;

	for(size_t i = 0; i < m_formationList.size(); i++)
	{
		HEntity hTmpEntity = m_formationList[i];
		IEntity* pTmpHero = getEntityFromHandle(hTmpEntity);
		if(!pTmpHero) continue;

		int iClassID = pTmpHero->getProperty(PROP_ENTITY_CLASS, 0);
		assert(GSProto::en_class_Hero == iClassID);

		string strUUID = pTmpHero->getProperty(PROP_ENTITY_UUID, "");

		ServerEngine::FormationItem tmpItem;
		tmpItem.strUUID = strUUID;
		saveData.formationList[i] = tmpItem;
	}
	saveData.iVisibleHeroId = getVisibleHeroID();
	data = ServerEngine::JceToStr(saveData);
}

void FormationSystem::getEntityList(vector<HEntity>& entityList)
{
	for(size_t i = 0; i < m_formationList.size() ;i++)
	{
		if(0 != m_formationList[i])
		{
			entityList.push_back(m_formationList[i]);
		}
	}
}

HEntity FormationSystem::getEntityFromPos(int iPos)
{
	if( (iPos < 0) || (iPos >= GSProto::MAX_BATTLE_MEMBER_SIZE/2) )
	{
		return 0;
	}

	return m_formationList[iPos];
}


void FormationSystem::initFormation(const string& strFormationData)
{	
	if(m_strCacheData.size() == 0)
	{
		return;
	}

	// 从英雄系统初始化英雄数据到阵形
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	IHeroSystem* pHeroSystem = static_cast<IHeroSystem*>(pEntity->querySubsystem(IID_IHeroSystem));
	assert(pHeroSystem);

	ServerEngine::FormationSystemData saveData;
	ServerEngine::JceToObj(strFormationData, saveData);

	for(map<taf::Int32, ServerEngine::FormationItem>::const_iterator it = saveData.formationList.begin(); it != saveData.formationList.end(); it++)
	{
		int iPos = it->first;
		const ServerEngine::FormationItem& tmpItem = it->second;
		
		HEntity hTmpHero = pHeroSystem->getHeroFromUUID(tmpItem.strUUID);
		assert(0 != hTmpHero);
		assert(iPos < GSProto::MAX_BATTLE_MEMBER_SIZE/2);

		m_formationList[iPos] = hTmpHero;
	}
}

void FormationSystem::onSendClientFormation(EventArgs& args)
{
	// 通知客户端
	sendFormationData();
}

void FormationSystem::sendFormationData()
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	sumFightValue();

	GSProto::Cmd_Sc_FormationData scData;	

	//add by hyf
	int iOldNum = 0;
	
	for(int i = 0; i < GSProto::MAX_BATTLE_MEMBER_SIZE/2; i++)
	{
		if(0 == m_formationList[i]) continue;

		GSProto::FormationItemData* pItemData = scData.add_szformationlist();
		assert(pItemData);

		pItemData->set_dwobjectid(m_formationList[i]);
		pItemData->set_ipos(i);
		iOldNum++;
	}
	

	int iFormationLimit = getFormationHeroLimit();
	scData.set_imemberlimit(iFormationLimit);

	int iFightValue = pEntity->getProperty(PROP_ENTITY_FIGHTVALUE, 0);
	scData.set_ifightvalue(iFightValue);

	pEntity->sendMessage(GSProto::CMD_FORMATION_DATA, scData);
	//
	IHeroSystem *pHeroSys = static_cast<IHeroSystem*>(pEntity->querySubsystem(IID_IHeroSystem));
	assert(pHeroSys);
	
	int iActorHeroCount = pHeroSys->getHeroCount();
	iFormationLimit = std::min(iActorHeroCount, iFormationLimit);
	if(iFormationLimit != iOldNum )
	{
		pEntity->chgNotice(GSProto::en_NoticeGuid_Formation,true);
	}
	else
	{
		pEntity->chgNotice(GSProto::en_NoticeGuid_Formation,false);
	}
}

void FormationSystem::onEventDestroyHero(EventArgs& args)
{
	HEntity hHero = args.context.getInt("hero");

	std::vector<HEntity>::iterator it = find(m_formationList.begin(), m_formationList.end(), hHero);
	if(it != m_formationList.end() )
	{
		*it = 0;
	}

	// 通知客户端变化
	sendFormationData();
}

int FormationSystem::getFormationHeroLimit()
{
	IEntity* pActor = getEntityFromHandle(m_hEntity);
	assert(pActor);
	
	int iLevel = pActor->getProperty(PROP_ENTITY_LEVEL, 0);

	return calcFormationLimit(iLevel);
}

int FormationSystem::calcFormationLimit(int iLevel)
{
	static vector<int> s_formationLimitList;

	if(s_formationLimitList.size() == 0)
	{
		IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
		assert(pGlobalCfg);

		string strFormationLimit = pGlobalCfg->getString("上阵人数限制", "3#9#4#19#5#100");
		s_formationLimitList = TC_Common::sepstr<int>(strFormationLimit, "#");
		assert( (s_formationLimitList.size() % 2) == 0);
	}

	for(size_t i =0; i < s_formationLimitList.size()/2; i++)
	{
		int iTmpLimit = s_formationLimitList[i*2];
		int iTmpLevel = s_formationLimitList[i*2 + 1];

		if(iLevel <= iTmpLevel)
		{
			return iTmpLimit;
		}
	}

	assert(false);

	return 3;
}

int FormationSystem::getEntityPos(HEntity hEntity)
{
	int iResult = -1;
	std::vector<HEntity>::iterator it = std::find(m_formationList.begin(), m_formationList.end(), hEntity);
	if(it == m_formationList.end() )
	{
		return iResult;
	}

	return (int)std::distance(m_formationList.begin(), it);
}

void FormationSystem::onEventLevelUp(EventArgs& args)
{
	int iOldLv = args.context.getInt("oldlv");
	int iNewLv = args.context.getInt("newlv");

	int iOldLimit = calcFormationLimit(iOldLv);
	int iNewLimit = calcFormationLimit(iNewLv);

	if(iOldLimit != iNewLimit)
	{
		sendFormationData();
	}
}

int FormationSystem::getFormationHeroCount()
{
	int iCurrentCount = (int)std::count_if(m_formationList.begin(), m_formationList.end(), bind2nd(not_equal_to<Uint32>(), 0));
	return iCurrentCount;
}






