#include "FightServerPch.h"
#include "OutPrxImp.h"
#include "FightFactory.h"


extern int Prop2LifeAtt(int iPropID);

void FightRecordQueryCb::callback_getFightData(taf::Int32 ret,  const std::string& strBattleData)
{
	SvrRunLog("call back getFightData Ret|%d", ret);
	ServerEngine::BattleData emptyData;
	if(ret == ServerEngine::en_FightDataRet_OK)
	{
		ServerEngine::BattleData data;
		ServerEngine::JceToObj(strBattleData, data);
		m_callBack(en_QueryBattle_OK, data);
	}
	else if(ret == ServerEngine::en_FightDataRet_NotExist)
	{
		m_callBack(en_QueryBattle_NotExist, emptyData);
	}
	else
	{
		m_callBack(en_QueryBattle_SysError, emptyData);
	}
}

void FightRecordQueryCb::callback_getFightData_exception(taf::Int32 ret)
{
	SvrErrLog("call back getFightData Exception|%d", ret);

	ServerEngine::BattleData emptyData;
	m_callBack(en_QueryBattle_SysError, emptyData);
}

void FightRecordSaveCb::callback_saveFightData(taf::Int32 ret)
{
	SvrRunLog("callback_saveFightData Ret|%d", ret);
}

void FightRecordSaveCb::callback_saveFightData_exception(taf::Int32 ret)
{
	SvrRunLog("callback_saveFightData Exception|%d", ret);
}


DelBuffActionListener::DelBuffActionListener(GSProto::FightDataAll& wholeData, const vector<HEntity>& memberList, const vector<HEntity>& externList):
	m_wholeData(wholeData), m_memberList(memberList), m_externList(externList)
{
	for(size_t i = 0; i < m_memberList.size(); i++)
	{
		IEntity* pTmp = getEntityFromHandle(m_memberList[i]);
		if(!pTmp) continue;

		pTmp->getEventServer()->subscribeEvent(EVENT_ENTITY_RELEASEBUFF, this, &DelBuffActionListener::onEventDelBuff);
	}

	for(size_t i = 0; i< m_externList.size(); i++)
	{
		IEntity* pTmp = getEntityFromHandle(m_externList[i]);
		if(!pTmp) continue;

		pTmp->getEventServer()->subscribeEvent(EVENT_ENTITY_RELEASEBUFF, this, &DelBuffActionListener::onEventDelBuff);
	}
}

DelBuffActionListener::~DelBuffActionListener()
{
	for(size_t i = 0; i < m_memberList.size(); i++)
	{
		IEntity* pTmp = getEntityFromHandle(m_memberList[i]);
		if(!pTmp) continue;

		pTmp->getEventServer()->unsubscribeEvent(EVENT_ENTITY_RELEASEBUFF, this, &DelBuffActionListener::onEventDelBuff);
	}

	for(size_t i = 0; i< m_externList.size(); i++)
	{
		IEntity* pTmp = getEntityFromHandle(m_externList[i]);
		if(!pTmp) continue;

		pTmp->getEventServer()->unsubscribeEvent(EVENT_ENTITY_RELEASEBUFF, this, &DelBuffActionListener::onEventDelBuff);
	}
}

void DelBuffActionListener::onEventDelBuff(EventArgs& args)
{
	HEntity hEntity = args.context.getInt("entity", 0);
	int iBuffID = args.context.getInt("buffid", 0);

	IBuffFactory* pBuffFactory = getComponent<IBuffFactory>(COMPNAME_BuffFactory, IID_IBuffFactory);
	assert(pBuffFactory);

	const IEntityBuff* pBuffProto = pBuffFactory->getBuffPrototype(iBuffID);
	if(!pBuffProto) return;

	if(!pBuffProto->isShowClient() ) return;

	IEntity* pTarget = getEntityFromHandle(hEntity);
	assert(pTarget);
	
	int iTargetPos = pTarget->getProperty(PROP_ENTITY_BATTLEPOS, 0);

	GSProto::FightAction* pAction = m_wholeData.add_szfightaction();
	pAction->set_iactiontype(GSProto::en_FightAction_DelBuff);

	GSProto::FightDelBuffAction* pDelBuffAction = pAction->mutable_delbuffaction();
	pDelBuffAction->set_itargetpos(iTargetPos);
	pDelBuffAction->set_ibuffid(iBuffID);

	FightFactory* pFightFactory = static_cast<FightFactory*>(getComponent<IFightFactory>(COMPNAME_FightFactory, IID_IFightFactory));
	assert(pFightFactory);

	pFightFactory->triggerBeginResultSet();
}


PropChgResultListener::PropChgResultListener(HEntity hEntity)
{
	m_hEntity = hEntity;

	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_DOFIGHTPROPCHG, this, &PropChgResultListener::onEventDoPropChg);
}

PropChgResultListener::~PropChgResultListener()
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_DOFIGHTPROPCHG, this, &PropChgResultListener::onEventDoPropChg);
}

void PropChgResultListener::onEventDoPropChg(EventArgs& args)
{
	EventArgs_FightPropChg& fightPropChgArgs = (EventArgs_FightPropChg&)args;

	FightFactory* pFightFactory = static_cast<FightFactory*>(getComponent<IFightFactory>(COMPNAME_FightFactory, IID_IFightFactory));
	assert(pFightFactory);

	GSProto::FightAction* pCurAction = &(*pFightFactory->m_pContinueDataAll->mutable_szfightaction()->rbegin());
	
	pFightFactory->fillPropChgToAction(pCurAction, fightPropChgArgs);
}


BuffAddListener::BuffAddListener(const vector<HEntity>& memberList):m_memberList(memberList)
{
	for(size_t i = 0; i < memberList.size(); i++)
	{
		IEntity* pMember = getEntityFromHandle(memberList[i]);
		if(!pMember) continue;

		pMember->getEventServer()->subscribeEvent(EVENT_ENTITY_NEWBUFF, this, &BuffAddListener::onEventBuffAdd);
	}
}


BuffAddListener::~BuffAddListener()
{
	for(size_t i = 0; i < m_memberList.size(); i++)
	{
		IEntity* pMember = getEntityFromHandle(m_memberList[i]);
		if(!pMember) continue;

		pMember->getEventServer()->unsubscribeEvent(EVENT_ENTITY_NEWBUFF, this, &BuffAddListener::onEventBuffAdd);
	}
}


void BuffAddListener::onEventBuffAdd(EventArgs& args)
{
	HEntity hEntity = args.context.getInt("entity", 0);
	//int iBuffID = args.context.getInt("buffid", 0);

	FightFactory* pFightFactory = static_cast<FightFactory*>(getComponent<IFightFactory>(COMPNAME_FightFactory, IID_IFightFactory));
	assert(pFightFactory);

	GSProto::FightAction* pCurAction = &(*pFightFactory->m_pContinueDataAll->mutable_szfightaction()->rbegin());
	
	pFightFactory->fillAddBuffToAction(pCurAction, hEntity, args);
}



SkillActionListener::SkillActionListener(GSProto::FightDataAll& wholeData, HEntity hAttacker, const vector<HEntity>& memberList, const vector<HEntity>& externList):
	 m_whleData(wholeData), m_hAttacker(hAttacker), m_memberList(memberList), m_externList(externList), 
		m_bBatterSkill(false), m_iActionIndex(0)
{
	IEntity* pAttacker = getEntityFromHandle(hAttacker);
	assert(pAttacker);

	pAttacker->getEventServer()->subscribeEvent(EVENT_ENTITY_USESKILL, this, &SkillActionListener::onEventUseSkill);
	pAttacker->getEventServer()->subscribeEvent(EVENT_ENTITY_USESKILL_FINISH, this, &SkillActionListener::onEventSkillUseFinish);
	pAttacker->getEventServer()->subscribeEvent(EVENT_ENTITY_DODAMAGE, this, &SkillActionListener::onEventDoDamage);
	pAttacker->getEventServer()->subscribeEvent(EVENT_ENTITY_BEDAMAGE, this, &SkillActionListener::onEventBeDamage);
	pAttacker->getEventServer()->subscribeEvent(EVENT_ENTITY_CONTINUESKILL, this, &SkillActionListener::onEventContinueSkill);

	for(size_t i = 0; i < m_memberList.size(); i++)
	{
		IEntity* pTmp = getEntityFromHandle(m_memberList[i]);
		if(!pTmp) continue;
		
		pTmp->getEventServer()->subscribeEvent(EVENT_ENTITY_DOFIGHTPROPCHG, this, &SkillActionListener::onEventDoFightPropChg);
		pTmp->getEventServer()->subscribeEvent(EVENT_ENTITY_NEWBUFF, this, &SkillActionListener::onEventAddBuff);
		pTmp->getEventServer()->subscribeEvent(EVENT_ENTITY_RELEASEBUFF, this, &SkillActionListener::onEventDelBuff);
		pTmp->getEventServer()->subscribeEvent(EVENT_ENTITY_EFFECTDOKILL, this, &SkillActionListener::onEventKill);

		// 这里目前仅仅处理反击，在执行效果前添加ResultSet
		pTmp->getEventServer()->subscribeEvent(EVENT_ENTITY_USESKILL, this, &SkillActionListener::onEventOtherUseSkill);
		pTmp->getEventServer()->subscribeEvent(EVENT_ENTITY_PREEXE_EFFECT, this, &SkillActionListener::onEventPreExeEffect);

		pTmp->getEventServer()->subscribeEvent(EVENT_ENTITY_ZIBAO, this, &SkillActionListener::onEventZiBao);

		pTmp->getEventServer()->subscribeEvent(EVENT_ENTITY_DODAMAGE, this, &SkillActionListener::onEventDoDamage);
		pTmp->getEventServer()->subscribeEvent(EVENT_ENTITY_BEDAMAGE, this, &SkillActionListener::onEventBeDamage);
		
	}

	for(size_t i = 0; i < m_externList.size(); i++)
	{
		IEntity* pTmp = getEntityFromHandle(m_externList[i]);
		if(!pTmp) continue;
		
		pTmp->getEventServer()->subscribeEvent(EVENT_ENTITY_DOFIGHTPROPCHG, this, &SkillActionListener::onEventDoFightPropChg);
		pTmp->getEventServer()->subscribeEvent(EVENT_ENTITY_NEWBUFF, this, &SkillActionListener::onEventAddBuff);
		pTmp->getEventServer()->subscribeEvent(EVENT_ENTITY_RELEASEBUFF, this, &SkillActionListener::onEventDelBuff);
		pTmp->getEventServer()->subscribeEvent(EVENT_ENTITY_EFFECTDOKILL, this, &SkillActionListener::onEventKill);
		pTmp->getEventServer()->subscribeEvent(EVENT_ENTITY_PREEXE_EFFECT, this, &SkillActionListener::onEventPreExeEffect);
	}

	//m_fightAction.set_iactiontype(GSProto::en_FightAction_Skill);
}

SkillActionListener::~SkillActionListener()
{
	IEntity* pAttacker = getEntityFromHandle(m_hAttacker);
	assert(pAttacker);

	pAttacker->getEventServer()->unsubscribeEvent(EVENT_ENTITY_USESKILL, this, &SkillActionListener::onEventUseSkill);
	pAttacker->getEventServer()->unsubscribeEvent(EVENT_ENTITY_USESKILL_FINISH, this, &SkillActionListener::onEventSkillUseFinish);
	pAttacker->getEventServer()->unsubscribeEvent(EVENT_ENTITY_DODAMAGE, this, &SkillActionListener::onEventDoDamage);
	pAttacker->getEventServer()->unsubscribeEvent(EVENT_ENTITY_BEDAMAGE, this, &SkillActionListener::onEventBeDamage);
	pAttacker->getEventServer()->unsubscribeEvent(EVENT_ENTITY_CONTINUESKILL, this, &SkillActionListener::onEventContinueSkill);

	for(size_t i = 0; i < m_memberList.size(); i++)
	{
		IEntity* pTmp = getEntityFromHandle(m_memberList[i]);
		if(!pTmp) continue;
		pTmp->getEventServer()->unsubscribeEvent(EVENT_ENTITY_DOFIGHTPROPCHG, this, &SkillActionListener::onEventDoFightPropChg);
		pTmp->getEventServer()->unsubscribeEvent(EVENT_ENTITY_NEWBUFF, this, &SkillActionListener::onEventAddBuff);
		pTmp->getEventServer()->unsubscribeEvent(EVENT_ENTITY_RELEASEBUFF, this, &SkillActionListener::onEventDelBuff);
		pTmp->getEventServer()->unsubscribeEvent(EVENT_ENTITY_EFFECTDOKILL, this, &SkillActionListener::onEventKill);

		
		pTmp->getEventServer()->unsubscribeEvent(EVENT_ENTITY_USESKILL, this, &SkillActionListener::onEventOtherUseSkill);
		pTmp->getEventServer()->unsubscribeEvent(EVENT_ENTITY_PREEXE_EFFECT, this, &SkillActionListener::onEventPreExeEffect);

		pTmp->getEventServer()->unsubscribeEvent(EVENT_ENTITY_ZIBAO, this, &SkillActionListener::onEventZiBao);

		pTmp->getEventServer()->unsubscribeEvent(EVENT_ENTITY_DODAMAGE, this, &SkillActionListener::onEventDoDamage);
		pTmp->getEventServer()->unsubscribeEvent(EVENT_ENTITY_BEDAMAGE, this, &SkillActionListener::onEventBeDamage);
		
	}

	for(size_t i = 0; i < m_externList.size(); i++)
	{
		IEntity* pTmp = getEntityFromHandle(m_externList[i]);
		if(!pTmp) continue;
		
		pTmp->getEventServer()->unsubscribeEvent(EVENT_ENTITY_DOFIGHTPROPCHG, this, &SkillActionListener::onEventDoFightPropChg);
		pTmp->getEventServer()->unsubscribeEvent(EVENT_ENTITY_NEWBUFF, this, &SkillActionListener::onEventAddBuff);
		pTmp->getEventServer()->unsubscribeEvent(EVENT_ENTITY_RELEASEBUFF, this, &SkillActionListener::onEventDelBuff);
		pTmp->getEventServer()->unsubscribeEvent(EVENT_ENTITY_EFFECTDOKILL, this, &SkillActionListener::onEventKill);
		pTmp->getEventServer()->unsubscribeEvent(EVENT_ENTITY_PREEXE_EFFECT, this, &SkillActionListener::onEventPreExeEffect);
	}
}


void SkillActionListener::onEventZiBao(EventArgs& args)
{
	HEntity hEntity = args.context.getInt("entity");
	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	GSProto::FightResulSet* pResultSet = &(*getCurrentAction()->mutable_szresultset()->rbegin());
	assert(pResultSet);

	GSProto::FightResult* pFightResult = pResultSet->add_szresult();
	pFightResult->set_iresulttype(GSProto::en_FightResult_ZiBao);

	GSProto::FightZiBaoResult* pZiBaoResult = pFightResult->mutable_zibaoresult();

	int iBattlePos = pEntity->getProperty(PROP_ENTITY_BATTLEPOS, 0);
	pZiBaoResult->set_ipos(iBattlePos);
}


void SkillActionListener::onEventPreExeEffect(EventArgs& args)
{
	if(!m_bBatterSkill)
	{
		return;
	}

	// 有可能已经存在了一个新的ResultSet
	google::protobuf::RepeatedPtrField<GSProto::FightResulSet>* pszResultSet = getCurrentAction()->mutable_szresultset();
	assert(pszResultSet->size() > 0);

	if(pszResultSet->rbegin()->szresult_size() == 0)
	{
		return;
	}

	pszResultSet->Add();
	triggerResultSet();
}


void SkillActionListener::onEventKill(EventArgs& args)
{
	FightFactory* pFightFactory = static_cast<FightFactory*>(getComponent<IFightFactory>(COMPNAME_FightFactory, IID_IFightFactory));
	assert(pFightFactory);

	ServerEngine::FightContext* pFightCtx = pFightFactory->m_pCurPVEFightCtx;
	if(!pFightCtx) return;

	// 如果不能吃药的战斗，不掉落药物(但是首场战斗可以吃药)
	if(!pFightFactory->canUseFightUseActorSkill(pFightCtx->iFightType) )
	{
		return;
	}

	HEntity hGiver = args.context.getInt("entity");
	IEntity* pGiver = getEntityFromHandle(hGiver);
	if(!pGiver) return;

	int iAttackerPos = pGiver->getProperty(PROP_ENTITY_BATTLEPOS, 0);
	if(pFightFactory->isTargetPos(iAttackerPos) ) return;

	// 判断目标是不是怪物方
	HEntity hTarget = args.context.getInt("target");
	IEntity* pTarget = getEntityFromHandle(hTarget);
	if(!pTarget) return;
	/*if(GSProto::en_class_Monster != pTarget->getProperty(PROP_ENTITY_CLASS, 0) )
	{
		return;
	}*/

	int iTargetPos = pTarget->getProperty(PROP_ENTITY_BATTLEPOS, 0);
	if(!pFightFactory->isTargetPos(iTargetPos) ) return;

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);
	
	int iItemLimit = pGlobalCfg->getInt("掉落血瓶上限", 12);
	
	// 如果达到上限，不掉落了
	if(m_whleData.icuritemcount() >= iItemLimit)
	{
		return;
	}

	int iDropItemCount = calcDropItemCount();
	if(0 == iDropItemCount) return;

	// 产出药物
	int iItemCount = m_whleData.icuritemcount() + iDropItemCount;	
	m_whleData.set_icuritemcount(std::min(iItemCount, iItemLimit) );

	google::protobuf::RepeatedPtrField<GSProto::FightResulSet>* pszResultSet = getCurrentAction()->mutable_szresultset();
	assert(pszResultSet->size() > 0);

	GSProto::FightResulSet* pCurResultSet = &(*pszResultSet->rbegin() );
	GSProto::FightResult* pNewResult = pCurResultSet->add_szresult();
	pNewResult->set_iresulttype(GSProto::en_FightResult_CreateItem);
	
	GSProto::FightCreateItemResult* pCreateItemResult = pNewResult->mutable_createitemresult();
	pCreateItemResult->set_iresultitemcount(m_whleData.icuritemcount() );
	pCreateItemResult->set_itargetpos(iTargetPos);
	pCreateItemResult->set_icreateitemcount(iDropItemCount);
}


int SkillActionListener::calcDropItemCount()
{
	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	IRandom* pRandom = getComponent<IRandom>("Random", IID_IMiniAprRandom);
	assert(pRandom);

	string strItemChanceList = pGlobalCfg->getString("击杀怪物掉落药品概率", "8000#1#1000#2");
	vector<int> chanceList = TC_Common::sepstr<int>(strItemChanceList, "#");
	assert(chanceList.size() % 2 == 0);

	int iRandV = (int)(pRandom->random()%10000);
	int iSumChance = 0;
	for(size_t i = 0; i < chanceList.size()/2; i++)
	{
		iSumChance += chanceList[i*2];
		if(iRandV < iSumChance)
		{
			return chanceList[i*2 + 1];
		}
	}

	return 0;
}

void SkillActionListener::triggerResultSet()
{
	FightFactory* pFightFactory = static_cast<FightFactory*>(getComponent<IFightFactory>(COMPNAME_FightFactory, IID_IFightFactory));
	assert(pFightFactory);

	pFightFactory->triggerBeginResultSet();
}


void SkillActionListener::onEventAddBuff(EventArgs& args)
{
	HEntity hEntity = args.context.getInt("entity", 0);
	//int iBuffID = args.context.getInt("buffid", 0);

	FightFactory* pFightFactory = static_cast<FightFactory*>(getComponent<IFightFactory>(COMPNAME_FightFactory, IID_IFightFactory));
	assert(pFightFactory);

	pFightFactory->fillAddBuffToAction(getCurrentAction(), hEntity, args);

	/*IBuffFactory* pBuffFactory = getComponent<IBuffFactory>(COMPNAME_BuffFactory, IID_IBuffFactory);
	assert(pBuffFactory);

	const IEntityBuff* pBuffProto = pBuffFactory->getBuffPrototype(iBuffID);
	if(!pBuffProto) return;

	if(!pBuffProto->isShowClient() ) return;

	IEntity* pTarget = getEntityFromHandle(hEntity);
	assert(pTarget);
	
	int iTargetPos = pTarget->getProperty(PROP_ENTITY_BATTLEPOS, 0);

	google::protobuf::RepeatedPtrField<GSProto::FightResulSet>* pszResultSet = getCurrentAction()->mutable_szresultset();
	assert(pszResultSet->size() > 0);

	GSProto::FightResulSet* pCurResultSet = &(*pszResultSet->rbegin() );

	// 新增一个Result
	GSProto::FightResult* pNewResult = pCurResultSet->mutable_szresult()->Add();
	assert(pNewResult);

	pNewResult->set_iresulttype(GSProto::en_FightResult_AddBuff);
	GSProto::FightAddBuffResult* pNewAddBuffResult = pNewResult->mutable_addbuffresult();
	pNewAddBuffResult->set_itargetpos(iTargetPos);
	pNewAddBuffResult->set_ibuffid(iBuffID);*/
}

void SkillActionListener::onEventDelBuff(EventArgs& args)
{
	HEntity hEntity = args.context.getInt("entity", 0);
	int iBuffID = args.context.getInt("buffid", 0);

	IBuffFactory* pBuffFactory = getComponent<IBuffFactory>(COMPNAME_BuffFactory, IID_IBuffFactory);
	assert(pBuffFactory);

	const IEntityBuff* pBuffProto = pBuffFactory->getBuffPrototype(iBuffID);
	if(!pBuffProto) return;

	if(!pBuffProto->isShowClient() ) return;

	IEntity* pTarget = getEntityFromHandle(hEntity);
	assert(pTarget);
	
	int iTargetPos = pTarget->getProperty(PROP_ENTITY_BATTLEPOS, 0);

	google::protobuf::RepeatedPtrField<GSProto::FightResulSet>* pszResultSet = getCurrentAction()->mutable_szresultset();
	assert(pszResultSet->size() > 0);


	GSProto::FightResulSet* pCurResultSet = &(*pszResultSet->rbegin() );

	// 新增一个Result
	GSProto::FightResult* pNewResult = pCurResultSet->mutable_szresult()->Add();
	assert(pNewResult);

	pNewResult->set_iresulttype(GSProto::en_FightResult_DelBuff);
	
	GSProto::FightDelBuffResult* pDelBuffResult = pNewResult->mutable_delbuffresult();
	pDelBuffResult->set_itargetpos(iTargetPos);
	pDelBuffResult->set_ibuffid(iBuffID);
}

void SkillActionListener::onEventContinueSkill(EventArgs& args)
{
	bool bContinueKill = args.context.getInt("continuekill");
	getCurrentAction()->mutable_szresultset()->Add();

	// 设置连击标记
	getCurrentAction()->mutable_szresultset()->rbegin()->set_bcontinueskill(true);
	getCurrentAction()->mutable_szresultset()->rbegin()->set_bcontinuekill(bContinueKill);
	triggerResultSet();
}

void SkillActionListener::onEventDoFightPropChg(EventArgs& args)
{
	EventArgs_FightPropChg& fightPropChgArgs = (EventArgs_FightPropChg&)args;

	FightFactory* pFightFactory = static_cast<FightFactory*>(getComponent<IFightFactory>(COMPNAME_FightFactory, IID_IFightFactory));
	assert(pFightFactory);
	
	pFightFactory->fillPropChgToAction(getCurrentAction(), fightPropChgArgs);
}


GSProto::FightAction* SkillActionListener::getCurrentAction()
{
	return &(*m_whleData.mutable_szfightaction()->rbegin() );
}


void SkillActionListener::onEventOtherUseSkill(EventArgs& args)
{
	bool bIsBackAttack = args.context.getInt("isBackAttack") == 1;
	if(!bIsBackAttack) return;

	getCurrentAction()->mutable_szresultset()->Add();
	triggerResultSet();
}


void SkillActionListener::onEventUseSkill(EventArgs& args)
{
	int iSkillID = args.context.getInt("SkillID", 0);

	IEntity* pAttacker = getEntityFromHandle(m_hAttacker);
	assert(pAttacker);
	
	int iBattlePos = pAttacker->getProperty(PROP_ENTITY_BATTLEPOS, 0);

	m_whleData.mutable_szfightaction()->Add();

	m_iActionIndex = m_whleData.szfightaction_size() - 1;
	
	getCurrentAction()->set_iactiontype(GSProto::en_FightAction_Skill);

	GSProto::FightSkillAction* pSkillAction = getCurrentAction()->mutable_skillaction();
	assert(pSkillAction);
	pSkillAction->set_iattackerpos(iBattlePos);
	pSkillAction->set_iskillid(iSkillID);

	ISkillFactory* pSkillFactory = getComponent<ISkillFactory>(COMPNAME_SkillFactory, IID_ISkillFactory);
	assert(pSkillFactory);

	const ISkill* pSkillPrototype = pSkillFactory->getSkillPrototype(iSkillID);
	assert(pSkillPrototype);

	pSkillAction->set_bisstageskill(pSkillPrototype->getSkillType() == en_SkillType_Stage);

	m_bBatterSkill = pSkillPrototype->isBattleSkill();

	// 添加一个ResultSet吧
	getCurrentAction()->mutable_szresultset()->Add();

	// 技能开始触发事件
	triggerResultSet();
}

void SkillActionListener::onEventSkillUseFinish(EventArgs& args)
{
	// 需要过滤反击
	bool bIsBackAttack = args.context.getInt("isBackAttack") == 1;
	if(bIsBackAttack) return;

	IEntity* pAttacker = getEntityFromHandle(m_hAttacker);
	assert(pAttacker);

	int iAnger = pAttacker->getProperty(PROP_ENTITY_ANGER, 0);
	GSProto::FightSkillAction* pSkillAction = getCurrentAction()->mutable_skillaction();
	assert(pSkillAction);
	pSkillAction->set_iangerafterskill(iAnger);

	// 填充神兽怒气
	for(size_t i = 0; i < m_externList.size(); i++)
	{
		HEntity hTmp = m_externList[i];
		IEntity* pTmp = getEntityFromHandle(hTmp);
		if(!pTmp) continue;

		int iPos = pTmp->getProperty(PROP_ENTITY_BATTLEPOS, 0);
		int iTmpAnger = pTmp->getProperty(PROP_ENTITY_ANGER, 0);

		if(GSProto::ATT_GOLDANIMAL_POS == iPos)
		{
			pSkillAction->set_iagodanimalanger(iTmpAnger);
		}
		else if(GSProto::TARGET_GOLDANIMAL_POS == iPos)
		{
			pSkillAction->set_itgodanimalanger(iTmpAnger);
		}
	}

	// 连杀可能导致最后一个ResultSet为空，过滤
	if(getCurrentAction()->mutable_szresultset()->size() > 0)
	{
		if(getCurrentAction()->mutable_szresultset()->rbegin()->mutable_szresult()->size() == 0)
		{
			getCurrentAction()->mutable_szresultset()->RemoveLast();
		}
	}
}

void SkillActionListener::onEventDoDamage(EventArgs& args)
{
	EventArgsDamageCtx& damageCtx = (EventArgsDamageCtx&)args;

	// 反击放在被伤害方处理
	bool bIsBackAtt = damageCtx.effectCtx->getInt(PROP_EFFECT_ISBACKATTACK) == 1;
	if(bIsBackAtt)
	{
		return;
	}

	// BUff伤害跳过
	bool bIsBuff = damageCtx.effectCtx->getInt(PROP_EFFECT_ISBUFFEFFECT) != 0;
	if(bIsBuff) return;

	processNormalDamage(damageCtx);
}

void SkillActionListener::onEventBeDamage(EventArgs& args)
{
	// 只处理反击
	EventArgsDamageCtx& damageCtx = (EventArgsDamageCtx&)args;
	bool bIsBackAtt = damageCtx.effectCtx->getInt(PROP_EFFECT_ISBACKATTACK) == 1;
	if(!bIsBackAtt) return;

	processBackDamage(damageCtx);
}


void SkillActionListener::processNormalDamage(EventArgsDamageCtx& damageCtx)
{
	google::protobuf::RepeatedPtrField<GSProto::FightResulSet>* pszResultSet = getCurrentAction()->mutable_szresultset();

	// 连击需要拆一个新的Set(这效果开始前触发)
	assert(pszResultSet->size() > 0);

	GSProto::FightResulSet* pCurResultSet = &(*pszResultSet->rbegin() );

	// 添加独立的result
	GSProto::FightResult* pNewFightResult = pCurResultSet->mutable_szresult()->Add();
	pNewFightResult->set_iresulttype(GSProto::en_FightResult_SkillDamage);
	
	GSProto::FightSkillDamageResult* pFightDamageResult = pNewFightResult->mutable_damageresult();

	IEntity* pTarget = getEntityFromHandle(damageCtx.hEntity);
	assert(pTarget);

	int iTargetPos = pTarget->getProperty(PROP_ENTITY_BATTLEPOS, 0);
	int iTargetHP = pTarget->getProperty(PROP_ENTITY_HP, 0);
	int iTargetAnger = pTarget->getProperty(PROP_ENTITY_ANGER, 0);
	
	pFightDamageResult->set_itargetpos(iTargetPos);
	pFightDamageResult->set_ihitresult(damageCtx.iHitResult);
	pFightDamageResult->set_idamagehp(damageCtx.iResultDamage);
	pFightDamageResult->set_iresulthp(iTargetHP);
	pFightDamageResult->set_iresultanger(iTargetAnger);
}

void SkillActionListener::processBackDamage(EventArgsDamageCtx& damageCtx)
{
	int iBackSkillID = damageCtx.effectCtx->getInt(PROP_EFFECT_USESKILL_ID);

	google::protobuf::RepeatedPtrField<GSProto::FightResulSet>* pszResultSet = getCurrentAction()->mutable_szresultset();
	assert(pszResultSet->size() > 0);
	
	GSProto::FightResulSet* pNewResultSet = &(*pszResultSet->rbegin() );
	
	GSProto::FightResult* pNewFightResult = pNewResultSet->mutable_szresult()->Add();
	pNewFightResult->set_iresulttype(GSProto::en_FightResult_BackDamage);
	
	GSProto::FightBackDamageResult* pBackDamageResult = pNewFightResult->mutable_backresult();

	IEntity* pGiver = getEntityFromHandle(damageCtx.hGiver);
	assert(pGiver);

	IEntity* pEntity = getEntityFromHandle(damageCtx.hEntity);
	assert(pEntity);

	int iResultHP = pEntity->getProperty(PROP_ENTITY_HP, 0);

	int iAttPos = pGiver->getProperty(PROP_ENTITY_BATTLEPOS, 0);
	pBackDamageResult->set_iattackpos(iAttPos);
	pBackDamageResult->set_idamagehp(damageCtx.iResultDamage);
	pBackDamageResult->set_itargetresulthp(iResultHP);
	pBackDamageResult->set_ibackskillid(iBackSkillID);
}





