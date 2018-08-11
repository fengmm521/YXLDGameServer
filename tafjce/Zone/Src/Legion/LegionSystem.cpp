#include "LegionPch.h"
#include "LegionSystem.h"
#include "LegionImp.h"
#include "LegionFactory.h"
#include "IItemFactory.h"
#include "IItemSystem.h"
#include "BossFactory.h"
#include "IJZEntityFactory.h"

extern "C" IObject* createLegionSystem()
{
	return new LegionSystem;
}

extern int Prop2LifeAtt(int iPropID);


LegionSystem::LegionSystem()
{
}

LegionSystem::~LegionSystem()
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	if(pEntity)
	{
		pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_BEGIN_FIGHT, this, &LegionSystem::onEventBeginFight);
		pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_PROPCHANGE, this, &LegionSystem::onEventPropertyChg);
		pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_LEVELUP, this, &LegionSystem::updateActorLevelInfo);
	}

	ITimerComponent* pTimeAxis = getComponent<ITimerComponent>(COMPNAME_TimeAxis, IID_ITimerComponent);
	assert(pTimeAxis);

	pTimeAxis->killTimer(m_hCheckHandle);
	pTimeAxis->killTimer(m_hCheckNoticeHandle);
	
}

Uint32 LegionSystem::getSubsystemID() const
{
	return IID_ILegionSystem;
}

Uint32 LegionSystem::getMasterHandle()
{
	return m_hEntity;
}

void LegionSystem::onEventBeginFight(EventArgs& args)
{
	const ServerEngine::FightContext* pFightCtx = (const ServerEngine::FightContext*)args.context.getInt64("clientParam", 0);
	assert(pFightCtx);

	if(pFightCtx->iFightType == GSProto::en_FightType_Boss)
	{
		onEventBossFightBegin(args);
	}
	else if(pFightCtx->iFightType == GSProto::en_FightType_LegionCity)
	{
		onEventCityFightBegin(args);
	}
}

void LegionSystem::onEventCityFightBegin(EventArgs& args)
{
	const ServerEngine::FightContext* pFightCtx = (const ServerEngine::FightContext*)args.context.getInt64("clientParam", 0);
	assert(pFightCtx);

	ServerEngine::LegionCityFightCtx cityFightCtx;
	assert(pFightCtx->context.find("LegionCityFightCtx") != pFightCtx->context.end() );
	ServerEngine::JceToObj(pFightCtx->context.at("LegionCityFightCtx"), cityFightCtx);

	const vector<HEntity>* pMemberList = (const vector<HEntity>*)args.context.getInt64("memberlist", 0);
	assert(pMemberList);

	for(size_t i = 0; i < pMemberList->size(); i++)
	{
		HEntity hTmp = (*pMemberList)[i];
		IEntity* pTmp = getEntityFromHandle(hTmp);
		if(!pTmp) continue;

		if(i < GSProto::MAX_BATTLE_MEMBER_SIZE/2)
		{
			pTmp->changeProperty(PROP_ENTITY_MAXHPAPERCENT, cityFightCtx.attCtx.iAddPercent, 0);
			pTmp->changeProperty(PROP_ENTITY_ATTAPERCENT, cityFightCtx.attCtx.iAddPercent, 0);
		}
		else
		{
			pTmp->changeProperty(PROP_ENTITY_MAXHPAPERCENT, cityFightCtx.targetCtx.iAddPercent, 0);
			pTmp->changeProperty(PROP_ENTITY_ATTAPERCENT, cityFightCtx.targetCtx.iAddPercent, 0);
		}
	}
}

void LegionSystem::onEventBossFightBegin(EventArgs& args)
{
	const ServerEngine::FightContext* pFightCtx = (const ServerEngine::FightContext*)args.context.getInt64("clientParam", 0);
	assert(pFightCtx);

	ServerEngine::CreateBossCtx bossCtx;
	assert(pFightCtx->context.find("BossContext") != pFightCtx->context.end() );
	ServerEngine::JceToObj(pFightCtx->context.at("BossContext"), bossCtx);

	// 通用BOSSHP处理，
	const vector<HEntity>* pMemberList = (const vector<HEntity>*)args.context.getInt64("memberlist", 0);
	assert(pMemberList);

	// 攻击者数据修正
	assert(pFightCtx->context.find("AttackerContext") != pFightCtx->context.end() );
	ServerEngine::AttackBossCtx tmpAttCtx;
	ServerEngine::JceToObj(pFightCtx->context.at("AttackerContext"), tmpAttCtx);

	// 填充BOSSHP和等级, 修正攻击者攻击数据
	for(size_t i = 0; i < pMemberList->size(); i++)
	{
		HEntity hTmp = (*pMemberList)[i];
		IEntity* pTmp = getEntityFromHandle(hTmp);
		if(!pTmp) continue;

		int iClassID = pTmp->getProperty(PROP_ENTITY_CLASS, 0);
		if(GSProto::en_class_Monster == iClassID)
		{
			pTmp->setProperty(PROP_ENTITY_LEVEL, bossCtx.iFixLevel);
			int iCurMaxHP = pTmp->getProperty(PROP_ENTITY_MAXHP, 0);
			pTmp->changeProperty(PROP_ENTITY_MAXHPAVALUE, bossCtx.iFixMaxHP - iCurMaxHP, 0);
			pTmp->setProperty(PROP_ENTITY_HP, bossCtx.iHP);

			if(bossCtx.iDef > 0)
			{
				int iCurDef = pTmp->getProperty(PROP_ENTITY_DEF, 0);
				int iAddDef = bossCtx.iDef - iCurDef;
				pTmp->setProperty(PROP_ENTITY_DEFAVALUE, iAddDef);
			}
		}
		else
		{
			int iCurAtt = pTmp->getProperty(PROP_ENTITY_ATT, 0);
			int iDeltaAtt = (int)(iCurAtt*(double)tmpAttCtx.iAttAddPercent/10000);
			pTmp->changeProperty(PROP_ENTITY_ATTAVALUE, iDeltaAtt, 0);
		}
	}
}


bool LegionSystem::create(IEntity* pEntity, const std::string& strData)
{
	m_hEntity = pEntity->getHandle();

	if(strData.size() > 0)
	{
		ServerEngine::LegionSystemData tmpData;
		ServerEngine::JceToObj(strData, tmpData);
		initData(tmpData);
	}

	pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_BEGIN_FIGHT, this, &LegionSystem::onEventBeginFight);

	ITimerComponent* pTimeAxis = getComponent<ITimerComponent>(COMPNAME_TimeAxis, IID_ITimerComponent);
	assert(pTimeAxis);
	
	m_hCheckHandle = pTimeAxis->setTimer(this, 1, 300*1000, "GiftTimeOut");

	checkGiftNotice();
	
	m_hCheckNoticeHandle = pTimeAxis->setTimer(this, 2, 5*1000, "checkNotice");

	pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_PROPCHANGE, this, &LegionSystem::onEventPropertyChg);
	
	pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_LEVELUP, this, &LegionSystem::updateActorLevelInfo);

	
	
	return true;
}

void  LegionSystem::onEventPropertyChg(const EventArgs& args)
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	if(!pEntity->isFunctionOpen(GSProto::en_Function_Legion) )
	{
		return ;
	}

	EventArgs_PropChange& propChg = (EventArgs_PropChange&)args;
	
	if(propChg.iPropID != PROP_ENTITY_PHYSTRENGTH)
	{
		return;
	}

	int iChgCount = propChg.iOldValue - propChg.iValue ;

	if(iChgCount <= 0)
	{
		return;
	}

	//使用了精力增加军团贡献
	IGlobalCfg *pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	int iContributeMul = pGlobalCfg->getInt("使用1点精力军团贡献增加倍数",2);
	int iAddContribute = iContributeMul * iChgCount;

	LegionFactory* pLegionFactory = static_cast<LegionFactory*>(getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory) );
	assert(pLegionFactory);

	pLegionFactory->addLegionContribute(m_hEntity, iAddContribute);
}

void LegionSystem::gmAddLegionExp(int iExp)
{
	
	LegionFactory* pLegionFactory = static_cast<LegionFactory*>(getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory) );
	assert(pLegionFactory);

	pLegionFactory->addLegionContribute(m_hEntity, iExp);
}


void LegionSystem::onTimer(int nEventId)
{
	if(1 == nEventId)
	{	
		checkGiftTimeout();
	}
	else if(2== nEventId)
	{
		checkNotice();
	}
	
}

bool LegionSystem::initData(const ServerEngine::LegionSystemData& data)
{
	m_shopList = data.shopItemList;
	m_giftList = data.giftList;

	m_blessData = data.blessData;
	m_dayAwardFlag = data.dayAwardFlag;
	m_weekAwardFlag = data.weekAwardFlag;
	
	m_dwLastRefreshSecond = data.dwLastRefreshSecond;
	m_contributeShopRefreshTimes = data.contributeShopRefreshTimes;
	m_actorHaveAddLegionList = data.actorHaveAddLegionList;
	m_actorWeekAddLegionTimes = data.actorWeekAddLegionTimes;

	m_useWorshipTimes = data.useWorshipTimes;
	m_contributeTimes = data.contributeTimes;
    m_actorHaveWorship = data.actorHaveWorship;
	
	checkGiftTimeout();

	return true;
}

string LegionSystem::addGift(int iGiftID)
{
	LegionFactory* pLegionFactory = static_cast<LegionFactory*>(getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory) );
	assert(pLegionFactory);

	const GiftCfg* pGiftCfg = pLegionFactory->queryGiftCfg(iGiftID);
	if(!pGiftCfg) return "";

	ServerEngine::GiftData tmpData;
	tmpData.bCustomGift = 0;
	tmpData.iGiftID = iGiftID;
	tmpData.dwGetTime = time(0);

	uuid_t itemuuid;
	uuid_generate(itemuuid);
	char szUUIDString[1024] = {0};
	uuid_unparse_upper(itemuuid, szUUIDString);

	m_giftList[szUUIDString] = tmpData;

	IEntity* pActor = getEntityFromHandle(m_hEntity);
	assert(pActor);

	PLAYERLOG(pActor)<<"AddGift|"<<iGiftID<<endl;

	if(m_giftList.size() == 1)
	{
		checkGiftNotice();
	}

	return szUUIDString;
}

string LegionSystem::addGift(const ServerEngine::CustomGiftData& customGift)
{
	ServerEngine::GiftData tmpData;
	tmpData.bCustomGift = 1;
	tmpData.dwGetTime = time(0);
	tmpData.customGiftData = customGift;

	uuid_t itemuuid;
	uuid_generate(itemuuid);
	char szUUIDString[1024] = {0};
	uuid_unparse_upper(itemuuid, szUUIDString);

	m_giftList[szUUIDString] = tmpData;

	IEntity* pActor = getEntityFromHandle(m_hEntity);
	assert(pActor);

	PLAYERLOG(pActor)<<"AddCustomGift|"<<szUUIDString<<endl;

	if(m_giftList.size() == 1)
	{
		checkGiftNotice();
	}
	
	return szUUIDString;
}

void LegionSystem::checkGiftTimeout()
{
	IZoneTime* pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);

	Uint32 dwCurSecond = pZoneTime->GetCurSecond();

	for(map<string, ServerEngine::GiftData>::iterator it = m_giftList.begin(); it != m_giftList.end(); )
	{
		ServerEngine::GiftData& tmpData = it->second;
		if(dwCurSecond > (tmpData.dwGetTime + 7*24*3600) )
		{
			m_giftList.erase(it++);
		}
		else
		{
			it++;
		}
	}

	checkGiftNotice();
}

bool LegionSystem::createComplete()
{
	// 通知军团刷新信息
	LegionFactory* pLegionFactory = static_cast<LegionFactory*>(getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory) );
	if(pLegionFactory)
	{
		pLegionFactory->updateActorInfo(m_hEntity);
		pLegionFactory->actorLogionOut(m_hEntity,false);
	}

	// 注册战力变化事件
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_FIGHTVALUECHG, this, &LegionSystem::onEventFightValueChg);
	

	return true;
}
void LegionSystem::onEventFightValueChg(const EventArgs& args)
{
	LegionFactory* pLegionFactory = static_cast<LegionFactory*>(getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory) );
	assert(pLegionFactory);

	pLegionFactory->updateActorFightValue(m_hEntity);
}
void LegionSystem::updateActorLevelInfo(const EventArgs& args)
{
	//int iOldLv = args.context.getInt("oldlv");
	//int iNewLv = args.context.getInt("newlv");
	LegionFactory* pLegionFactory = static_cast<LegionFactory*>(getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory) );
	if(pLegionFactory)
	{
		pLegionFactory->updateActorInfo(m_hEntity);
	}
}

void LegionSystem::checkNotice()
{
	
	ILegionFactory* pLegionFac = getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory);
	assert(pLegionFac);

	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	string strActorName = pEntity->getProperty( PROP_ENTITY_NAME,"");
	
	bool haveNotice = false;
	bool bHaveSalary = false;
	bool bHaveRequest = false;
	GSLegionImp* pLegion = static_cast<GSLegionImp*>( pLegionFac->getActorLegion(strActorName));
	if(pLegion)
	{
		ServerEngine::LegionMemberItem *pActorItem = pLegion->getMemberInfoByName(strActorName);
		if(pActorItem) 
		{
			//检查膜拜奖励
			if( pActorItem->beWorshipGetContribute > 0)
			{
				haveNotice = true;
			}
			
			//检查军团俸禄
			if(!pLegion->checkPayGeted(m_hEntity, GSProto::en_LegionPay_Day) )
			{
				bHaveSalary = true;
			}

			if(!pLegion->checkPayGeted(m_hEntity, GSProto::en_LegionPay_Week))
			{
				bHaveSalary = true;
			}

			//检查请求

			if(pActorItem->iOccupation < GSProto::en_LegionOccupation_Member)
			{
				if(pLegion->iGetRequestListSize() > 0)
				{
					bHaveRequest = true;
				}
			}
			
		}
	}

	pEntity->chgNotice(GSProto::en_NoticeGuid_Legion_Worship,haveNotice);
	pEntity->chgNotice(GSProto::en_NoticeGuid_Legion_Salary,bHaveSalary);
	pEntity->chgNotice(GSProto::en_NoticeGuid_Legion_HaveRequest,bHaveRequest);	
	
}


const std::vector<Uint32>& LegionSystem::getSupportMessage()
{
	static std::vector<Uint32> resultList;
	
	if(resultList.size() == 0)
	{
		resultList.push_back(GSProto::CMD_CLICK_LEGION);
		resultList.push_back(GSProto::CMD_QUERY_LEGIONLIST);
		resultList.push_back(GSProto::CMD_APPJOIN_LEGION);
		resultList.push_back(GSProto::CMD_CREATE_LEGION);
		resultList.push_back(GSProto::CMD_QUERY_LEGION_BASE);
		resultList.push_back(GSProto::CMD_QUERY_LEGION_MEMBERLIST);
		resultList.push_back(GSProto::CMD_QUERY_LEGION_APP);
		resultList.push_back(GSProto::CMD_MODIFY_LEGION_SETTING);
		resultList.push_back(GSProto::CMD_DEAL_LEGIONAPP);
		resultList.push_back(GSProto::CMD_DISMISS_LEGION);
		resultList.push_back(GSProto::CMD_KICK_LEGIONMEMBER);
		resultList.push_back(GSProto::CMD_TRANSFER_LEGIONLEADER);
		resultList.push_back(GSProto::CMD_QUERY_LELGION_CONTRIBUTE);
		resultList.push_back(GSProto::CMD_LEGION_CONTRIBUTE);
		resultList.push_back(GSProto::CMD_LEGION_BLESS);
		resultList.push_back(GSProto::CMD_QUERY_LEGIONPAY);
		resultList.push_back(GSProto::CMD_GET_LEGIONPAY);
		resultList.push_back(GSProto::CMD_OPEN_LEGIONSHOP);
		resultList.push_back(GSProto::CMD_REFRESH_LEGIONSHOP);
		resultList.push_back(GSProto::CMD_SELECT_LEGIONSHOP);
		//resultList.push_back(GSProto::CMD_GIVEUP_LEGIONSHOP);
		resultList.push_back(GSProto::CMD_LEAVE_LEGION);
		resultList.push_back(GSProto::CMD_CANCEL_LEGIONAPP);
		resultList.push_back(GSProto::CMD_QUERY_OTHERLEGION);
	//k	resultList.push_back(GSProto::CMD_REFUSE_ALLAPP);

		// 军团BOSS
		resultList.push_back(GSProto::CMD_QUERY_LEGIONBOSS);
		resultList.push_back(GSProto::CMD_CALL_LEGIONBOSS);
		resultList.push_back(GSProto::CMD_CHEER_LEGIONBOSS);
		resultList.push_back(GSProto::CMD_FIGHT_LEGIONBOSS);
		resultList.push_back(GSProto::CMD_LEGIONBOSS_RELIVE);

			
		// 福利系统
		resultList.push_back(GSProto::CMD_QUERY_GIFT);
		resultList.push_back(GSProto::CMD_GET_GIFT);

		// 世界BOSS
		resultList.push_back(GSProto::CMD_QUERY_WORLDBOSS);
		resultList.push_back(GSProto::CMD_CHEER_WORLDBOSS);
		resultList.push_back(GSProto::CMD_FIGHT_WORLDBOSS);
		resultList.push_back(GSProto::CMD_WORLDBOSS_RELIVE);

		resultList.push_back(GSProto::CMD_DEAL_LEGIONAPP_ONEKEY);
		resultList.push_back(GSProto::CMD_LEGION_APPOINT);

		resultList.push_back(GSProto::CMD_LEGION_QUERY_WORSHIP);
		resultList.push_back(GSProto::CMD_LEGION_WORSHIP_MEMBER);
		resultList.push_back(GSProto::CMD_LEGION_GET_WORSHIP_REWARD);

		//新军团领地
		resultList.push_back(GSProto::CMD_QUERY_LEGION_CAMPBATTLE);
		resultList.push_back(GSProto::CMD_LEGION_CAMPBATTLE_REPORT);
		resultList.push_back(GSProto::CMD_LEGION_CAMPBATTLE_JOIN);
		resultList.push_back(GSProto::CMD_LEGION_CAMPBATTLE_ENTER);
		resultList.push_back(GSProto::CMD_LEGION_CAMPBATTLE_BATTLE);

		resultList.push_back(GSProto::CMD_QUERY_CAMPBATTLE_LASTFIGHT_SIMPLELOG);
		resultList.push_back(GSProto::CMD_QUERY_CAMPBATTLE_LASTFIGHT_DETAIL_LOG);
		resultList.push_back(GSProto::CMD_QUERY_CUR_FIGHT_LEGION_LOG);
		resultList.push_back(GSProto::CMD_QUERY_CUR_FIGHT_CONDITION);

		resultList.push_back(GSProto::CMD_QUERY_CAMPBATTLE_INFO);
		resultList.push_back(GSProto::CMD_QUERY_CAMPBATTLE_LASTFIGHT_LOG);
		
	}

	return resultList;
}

void LegionSystem::onMessage(QxMessage* pMessage)
{
	assert(pMessage->dwMsgLen == sizeof(GSProto::CSMessage) );
	const GSProto::CSMessage& msg = *(const GSProto::CSMessage*)(pMessage->pMsgDataBuff);

    ILegionFactory* pLegionFactory = getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory);
	assert(pLegionFactory);
	
	if(!pLegionFactory->isReady() )
	{
		IEntity* pEntity = getEntityFromHandle(m_hEntity);
		assert(pEntity);

		pEntity->sendErrorCode(ERROR_LEGION_NOREADY);
		return;
	}

	switch(msg.icmd() )
	{
		case GSProto::CMD_CLICK_LEGION:
			onReqClickLegion(msg);
			break;

		case GSProto::CMD_QUERY_LEGIONLIST:
			onReqQueryLegionList(msg);
			break;

		case GSProto::CMD_APPJOIN_LEGION:
			onReqAppLegion(msg);
			break;

		case GSProto::CMD_CANCEL_LEGIONAPP:
			onReqCancelApp(msg);
			break;

		case GSProto::CMD_CREATE_LEGION:
			onReqCreateLegion(msg);
			break;

		case GSProto::CMD_QUERY_OTHERLEGION:
			onReqQueryOtherLegion(msg);
			break;

		case GSProto::CMD_OPEN_LEGIONSHOP:
			onReqOpenShop(msg);
			break;

		case GSProto::CMD_REFRESH_LEGIONSHOP:
			onReqRefreshShop(msg);
			break;
			
	//	case GSProto::CMD_GIVEUP_LEGIONSHOP:
	//		onReqGiveupShp(msg);
	//		break;
			
		case GSProto::CMD_SELECT_LEGIONSHOP:
			onReqSelectShop(msg);
			break;

		/*case GSProto::CMD_COMM_QUERYBATTLE:
			onReqQueryCommBattle(msg);
			break;
		*/
		case GSProto::CMD_QUERY_GIFT:
			onReqQueryGift(msg);
			break;

		case GSProto::CMD_GET_GIFT:
			onReqGetGift(msg);
			break;

		case GSProto::CMD_QUERY_WORLDBOSS:
			onReqQueryWorldBoss(msg);
			break;

		case GSProto::CMD_CHEER_WORLDBOSS:
			onReqCheerWorldBoss(msg);
			break;

		case GSProto::CMD_FIGHT_WORLDBOSS:
			onReqFightWorldBoss(msg);
			break;

		case GSProto::CMD_WORLDBOSS_RELIVE:
			onReqWBFightRelive(msg);
			break;
			
		case GSProto::CMD_LEAVE_LEGION:
			onReqLeave(msg);
			break;
			
		default:
			dispatchLegion(msg);
			break;

	}
}


void LegionSystem::onReqWBFightRelive(const GSProto::CSMessage& msg)
{
	if(!checlWorldBossLevelCond() )
	{
		return;
	}

	BossFactory* pBossFactory = static_cast<BossFactory*>(getComponent<IBossFactory>(COMPNAME_BossFactory, IID_IBossFactory) );
	assert(pBossFactory);

	pBossFactory->onReqRelive(m_hEntity);
}


void LegionSystem::onReqFightWorldBoss(const GSProto::CSMessage& msg)
{
	if(!checlWorldBossLevelCond() )
	{
		return;
	}

	BossFactory* pBossFactory = static_cast<BossFactory*>(getComponent<IBossFactory>(COMPNAME_BossFactory, IID_IBossFactory) );
	assert(pBossFactory);

	pBossFactory->onReqFightWorldBoss(m_hEntity);
}

void LegionSystem::onReqCheerWorldBoss(const GSProto::CSMessage& msg)
{
	if(!checlWorldBossLevelCond() )
	{
		return;
	}

	BossFactory* pBossFactory = static_cast<BossFactory*>(getComponent<IBossFactory>(COMPNAME_BossFactory, IID_IBossFactory) );
	assert(pBossFactory);

	pBossFactory->onReqCheerWorldBoss(m_hEntity);
}

void LegionSystem::onReqQueryWorldBoss(const GSProto::CSMessage& msg)
{
	if(!checlWorldBossLevelCond() )
	{
		return;
	}
	
	BossFactory* pBossFactory = static_cast<BossFactory*>(getComponent<IBossFactory>(COMPNAME_BossFactory, IID_IBossFactory) );
	assert(pBossFactory);

	pBossFactory->sendBossInfo(m_hEntity);
}


bool LegionSystem::checkLegionCond()
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	if(!pEntity->isFunctionOpen(GSProto::en_Function_Legion) )
	{
		pEntity->sendErrorCode(ERROR_LEGION_NOOPEN);
		return false;
	}

	return true;
}


bool LegionSystem::checlWorldBossLevelCond()
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	int iNeedLevel = pGlobalCfg->getInt("世界BOSS开发等级", 18);
	int iLevel = pEntity->getProperty(PROP_ENTITY_LEVEL, 0);
	if(iLevel < iNeedLevel)
	{
		pEntity->sendErrorCode(ERROR_WORLDBOSS_NOOPEN);
		return false;
	}

	return true;
};

void LegionSystem::checkGiftNotice()
{
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	int iSize = m_giftList.size();
	bool bNotice = (iSize>=1);
	pEntity->chgNotice(GSProto::en_NoticeGuid_HaveGift,bNotice);
}

void LegionSystem::onReqQueryGift(const GSProto::CSMessage& msg)
{
	GSProto::CMD_QUERY_GIFT_SC scMsg;
	for(map<string, ServerEngine::GiftData>::iterator it = m_giftList.begin(); it != m_giftList.end(); it++)
	{
		const ServerEngine::GiftData& refGiftData = it->second;
		GSProto::GiftItem* pNewGiftItem = scMsg.mutable_szgiftitem()->Add();

		pNewGiftItem->set_struuid(it->first);
		pNewGiftItem->set_bcustomgift(refGiftData.bCustomGift);
		
		if(refGiftData.bCustomGift)
		{
			GSProto::GiftCustomBody* pNewCustomBody = pNewGiftItem->mutable_cutombody();
			fillCustomBody(refGiftData.customGiftData, pNewCustomBody);
		}
		else
		{
			pNewGiftItem->set_igiftid(refGiftData.iGiftID);
		}
	}

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	pEntity->sendMessage(GSProto::CMD_QUERY_GIFT, scMsg);
}

void LegionSystem::fillCustomBody(const ServerEngine::CustomGiftData& customData, GSProto::GiftCustomBody* pScCustom)
{
	pScCustom->set_strtitle(customData.strTitle);

	for(map<taf::Int32, taf::Int32>::const_iterator it = customData.propAward.begin(); it != customData.propAward.end(); it++)
	{
		GSProto::PropItem* pNewPropItem = pScCustom->mutable_szawardproplist()->Add();
		int iPropID = it->first;

		int iLifeAtt = Prop2LifeAtt(iPropID);
		if(iLifeAtt < 0) continue;
		
		pNewPropItem->set_ilifeattid(iLifeAtt);
		pNewPropItem->set_ivalue(it->second);
	}

	for(map<taf::Int32, taf::Int32>::const_iterator it = customData.itemAward.begin(); it != customData.itemAward.end(); it++)
	{
		GSProto::FightAwardItem* pNewItemAward = pScCustom->mutable_szawarditemlist()->Add();
		pNewItemAward->set_iitemid(it->first);
		pNewItemAward->set_icount(it->second);
	}

	pScCustom->set_iawardedcontribute(customData.iAwardedContibute);
}

void LegionSystem::onReqGetGift(const GSProto::CSMessage& msg)
{
	GSProto::CMD_GET_GIFT_CS req;
	if(!req.ParseFromString(msg.strmsgbody() ) )
	{
		return;
	}

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	map<string, ServerEngine::GiftData>::iterator it = m_giftList.find(req.strgiftuuid() );
	if(it == m_giftList.end() )
	{
		pEntity->sendErrorCode(ERROR_NO_GIFT);
		return;
	}

	ServerEngine::GiftData tmpData = it->second;
	m_giftList.erase(it);

	if(!tmpData.bCustomGift)
	{
		doGift(tmpData.iGiftID);
	}
	else
	{
		doCustomGift(tmpData.customGiftData);
	}

	GSProto::CMD_GET_GIFT_SC scMsg;
	scMsg.set_strgiftuuid(req.strgiftuuid() );

	pEntity->sendMessage(GSProto::CMD_GET_GIFT, scMsg);

	PLAYERLOG(pEntity)<<"GetGift|"<<tmpData.iGiftID<<"|"<<req.strgiftuuid()<<endl;
	
	checkGiftNotice();
}


void LegionSystem::doGift(int iGiftID)
{
	LegionFactory* pLegionFactory = static_cast<LegionFactory*>(getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory) );
	assert(pLegionFactory);

	const GiftCfg* pGiftCfg = pLegionFactory->queryGiftCfg(iGiftID);
	assert(pGiftCfg);

	IDropFactory* pDorpFactory = getComponent<IDropFactory>(COMPNAME_DropFactory, IID_IDropFactory);
	assert(pDorpFactory);

	GSProto::FightAwardResult awardResult;
	for(size_t i = 0; i < pGiftCfg->dropIDList.size(); i++)
	{
		pDorpFactory->calcDrop(pGiftCfg->dropIDList[i], awardResult);
	}

	pDorpFactory->excuteDrop(m_hEntity, awardResult, GSProto::en_Reason_GiftCreate);
}

void LegionSystem::doCustomGift(const ServerEngine::CustomGiftData& customData)
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	IItemSystem* pItemSys = static_cast<IItemSystem*>(pEntity->querySubsystem(IID_IItemSystem));
	assert(pItemSys);

	for(map<taf::Int32, taf::Int32>::const_iterator it = customData.propAward.begin(); it != customData.propAward.end(); it++)
	{
		int iPropID = it->first;
		int iValue = it->second;
		pEntity->changeProperty(iPropID, iValue, GSProto::en_Reason_GiftCreate);
	}

	for(map<taf::Int32, taf::Int32>::const_iterator it = customData.itemAward.begin(); it != customData.itemAward.end(); it++)
	{
		int iItemID = it->first;
		int iCount = it->second;

		pItemSys->addItem(iItemID, iCount, GSProto::en_Reason_GiftCreate);
	}
}


class GetCommFightRecord:public ServerEngine::FightDataPrxCallback
{
public:

	GetCommFightRecord(HEntity hEntity):m_hEntity(hEntity){}

	 virtual void callback_getFightData(taf::Int32 ret,  const std::string& strBattleData)
	 {
	 	if(ret != ServerEngine::en_FightDataRet_OK)
	 	{
	 		return;
	 	}

		IEntity* pEntity = getEntityFromHandle(m_hEntity);
		if(!pEntity) return;

		IFightSystem* pFightSys = static_cast<IFightSystem*>(pEntity->querySubsystem(IID_IFightSystem));
		assert(pFightSys);

		ServerEngine::BattleData battleData;
		ServerEngine::JceToObj(strBattleData, battleData);
		pFightSys->sendAllBattleMsg(battleData);
	 }

	 virtual void callback_getFightData_exception(taf::Int32 ret)
	 {
	 	SvrErrLog("GetCommFightRecord|%d", ret);
	 }

private:

	HEntity m_hEntity;
};

void LegionSystem::onReqQueryCommBattle(const GSProto::CSMessage& msg)
{
	GSProto::CMD_COMM_QUERYBATTLE_CS req;
	if(!req.ParseFromString(msg.strmsgbody() ) )
	{
		return;
	}

	IMessageLayer* pMsgLayer = getComponent<IMessageLayer>(COMPNAME_MessageLayer, IID_IMessageLayer);
	assert(pMsgLayer);

	IFightFactory* pFightFactory = getComponent<IFightFactory>(COMPNAME_FightFactory, IID_IFightFactory);
	assert(pFightFactory);

	ServerEngine::PKFight fightKey;
	pFightFactory->decodeFightStringKey(req.strfightkey(), fightKey);

	pMsgLayer->AsyncGetFightRecord(fightKey, new GetCommFightRecord(m_hEntity) );
}


void LegionSystem::onReqQueryOtherLegion(const GSProto::CSMessage& msg)
{	
	if(!checkLegionCond() ) return;

	GSProto::CMD_QUERY_OTHERLEGION_CS req;
	if(!req.ParseFromString(msg.strmsgbody() ) )
	{
		return;
	}
	GSLegionImp* pLegion = static_cast<GSLegionImp*>(getLegionFromHandle(req.dwlegionobjectid() ) );
	if(!pLegion) return;

	pLegion->sendSimpleInfo(m_hEntity);
}


void LegionSystem::onReqCancelApp(const GSProto::CSMessage& msg)
{
	if(!checkLegionCond() ) return;

	GSProto::CMD_CANCEL_LEGIONAPP_CS req;
	if(!req.ParseFromString(msg.strmsgbody() ) )
	{
		return;
	}

	GSLegionImp* pLegion = static_cast<GSLegionImp*>(getLegionFromHandle(req.dwlegionobjectid() ) );
	if(!pLegion) return;

	pLegion->cancelApp(m_hEntity);
}

void LegionSystem::onReqRefreshShop(const GSProto::CSMessage& msg)
{
	if(!checkLegionCond() ) return;

	IScriptEngine *pScript = getComponent<IScriptEngine>(COMPNAME_ScriptEngine, IID_IScriptEngine);
	assert(pScript);

	IShopFactory *pShopFactory = getComponent<IShopFactory>(COMPNAME_ShopFactory, IID_IShopFactory);
	assert(pShopFactory);
	
	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	int iNowRefreshTimes = getDayResetValue( m_contributeShopRefreshTimes).iValue;
	int iCost = pShopFactory->getRefreshCost(iNowRefreshTimes+1);
	assert(iCost != -1);

	EventArgs args;
	args.context.setInt("entity", m_hEntity);
	args.context.setInt("cost", iCost);
	
	pScript->runFunction("contributeShopRefreshCost",&args, "EventArgs");
	
}

void LegionSystem::contributeShopRefreshConfirm()
{
	
	//扣钱
	IShopFactory *pShopFactory = getComponent<IShopFactory>(COMPNAME_ShopFactory, IID_IShopFactory);
	assert(pShopFactory);
	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	int iNowRefreshTimes = getDayResetValue( m_contributeShopRefreshTimes).iValue;
	int iCost = pShopFactory->getRefreshCost(iNowRefreshTimes+1);
	assert(iCost != -1);
	
	//贡献是否足够
	int iActorHave= pEntity->getProperty(PROP_ACTOR_LEGION_CONTRIBUTE, 0);
	
	if(iActorHave < iCost) 
	{
		pEntity->sendErrorCode(ERROR_NO_CONTRIBUTE);
		return;
	}
	
	//扣贡献
	pEntity->changeProperty( PROP_ACTOR_LEGION_CONTRIBUTE, 0 - iCost, GSProto::en_Reason_Legion_Shop_Refresh);
	
	PLAYERLOG(pEntity)<<"refresh ContributeShop Cost |" << iCost <<endl;
	
	////刷新
	m_shopList.clear();
	refreshShop();
	m_dwLastRefreshSecond = time(0);
	getDayResetValue( m_contributeShopRefreshTimes).iValue++;
	
	sendContributeInfo(true);
}

void LegionSystem::refreshShop()
{
	IShopFactory *pShopFactory = getComponent<IShopFactory>(COMPNAME_ShopFactory, IID_IShopFactory);
	assert(pShopFactory);
	
	vector<ShopGoodUnit> shopGoodUnitVec; 
	bool res = pShopFactory->getActorContributeShop(m_hEntity, shopGoodUnitVec);
	assert(res);
	
	m_shopList.clear();
	for(size_t i = 0; i < shopGoodUnitVec.size(); i++)
	{
		const ShopGoodUnit& unit = shopGoodUnitVec[i];
		
		ServerEngine::LegionShopItem tmpItem;
		tmpItem.iIndex = (int)i;
		tmpItem.iShopID = unit.iGoodId;
		tmpItem.bBuy = 0;
		m_shopList.push_back(tmpItem);
	}
}

/*
int LegionSystem::getBuySize()
{
	int iCount = 0;
	for(size_t i = 0; i < m_shopList.size(); i++)
	{
		if(m_shopList[i].bBuy)
		{
			iCount++;
		}
	}

	return iCount;
}*/

/*
void LegionSystem::onReqGiveupShp(const GSProto::CSMessage& msg)
{
	if(!checkLegionCond() ) return;

	LegionFactory* pLegionFactory = static_cast<LegionFactory*>(getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory) );
	assert(pLegionFactory);

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	string strActorName = pEntity->getProperty(PROP_ENTITY_NAME, "");
	GSLegionImp* pLegion = static_cast<GSLegionImp*>(pLegionFactory->getActorLegion(strActorName) );
	if(!pLegion) return;

	ServerEngine::LegionMemberItem* pMemInfo = (ServerEngine::LegionMemberItem*)pLegion->getMemberInfo(strActorName);
	if(!pMemInfo) return;

	// 无商店
	if(m_shopList.size() == 0)
	{
		return;
	}

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);
	
	// 如果没有购买，需要消耗刷新费用
	if(getBuySize() == 0)
	{
		int iRefreshCost = pGlobalCfg->getInt("军团商店刷新消耗", 50);
		if(pMemInfo->iCurContribute < iRefreshCost)
		{
			pEntity->sendErrorCode(ERROR_NO_CONTRIBUTE);
			return;
		}

		// 扣除贡献
		pMemInfo->iCurContribute -= iRefreshCost;
		PLAYERLOG(pEntity)<<"LegionShop|GiveUp|"<<iRefreshCost<<endl;
	}

	refreshShop();

	GSProto::CMD_GIVEUP_LEGIONSHOP_SC scMsg;
	fillShopPage(scMsg.mutable_page() );

	pEntity->sendMessage(GSProto::CMD_GIVEUP_LEGIONSHOP, scMsg);
}
*/

void LegionSystem::onReqSelectShop(const GSProto::CSMessage& msg)
{
	if(!checkLegionCond() ) return;

	GSProto::CMD_SELECT_LEGIONSHOP_CS csMsg;
	if( !csMsg.ParseFromString(msg.strmsgbody()))
	{
		return;
	}
	int iIndex = csMsg.iindex();
	if(( iIndex >= (int)m_shopList.size() ) ||( iIndex < 0) ) 
	{
		return;
	}

	ServerEngine::LegionShopItem &item = m_shopList[iIndex];

	if(item.bBuy != 0)
	{
		return;
	}

	IShopFactory *pShopFactory = getComponent<IShopFactory>(COMPNAME_ShopFactory, IID_IShopFactory);
	assert(pShopFactory);

	ShopGoodBaseInfo goodInfo;
	bool res = pShopFactory->getGoodById( item.iShopID, goodInfo);
	assert(res);

	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	string strActorName = pEntity->getProperty(PROP_ENTITY_NAME, "");
	LegionFactory* pLegionFactory = static_cast<LegionFactory*>(getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory) );
	assert(pLegionFactory);
	GSLegionImp* pLegion = static_cast<GSLegionImp*>(pLegionFactory->getActorLegion(strActorName) );
	if(!pLegion) return;
	int iLegionLevel = pLegion->getLevel();
	if(goodInfo.iLegionLevel > iLegionLevel)
	{
		return;
	}
		
	//扣钱
	int iActorHave= pEntity->getProperty(PROP_ACTOR_LEGION_CONTRIBUTE, 0);
	
	if(iActorHave < goodInfo.iGoodPrice) 
	{
		pEntity->sendErrorCode(ERROR_NO_CONTRIBUTE);
		return;
	}
	//扣贡献
	
	pEntity->changeProperty( PROP_ACTOR_LEGION_CONTRIBUTE, 0 - goodInfo.iGoodPrice, GSProto::en_Reason_Legion_Shop_Buy);
	
	PLAYERLOG(pEntity)<<"Buy ContributeGoods Cost |" << goodInfo.iGoodPrice <<endl;
	
	//发东西
	if(goodInfo.iType == enGoodType_Equip)
	{
		//to do
	}
	else if(goodInfo.iType == enGoodType_HeroSoul || goodInfo.iType == enGoodType_FavorConsume)
	{
		IItemSystem* pITemSys = static_cast<IItemSystem*>( pEntity->querySubsystem(IID_IItemSystem));
		pITemSys->addItem(goodInfo.iGoodsId, goodInfo.iGoodCount, GSProto::en_Reason_LegionShop_BUY);
	}
	
	item.bBuy = 1;
	sendContributeInfo();
	
}

/*
void LegionSystem::notiifyShopHero(GSProto::FightAwardHero* pHeroAward)
{
	GSProto::CMD_MARQUEE_SC scMsg;
	scMsg.set_marqueeid(GSProto::en_marQueue_LegionShop);

	IEntity* pActor = getEntityFromHandle(m_hEntity);
	assert(pActor);

	string strActorName = pActor->getProperty(PROP_ENTITY_NAME, "");

	scMsg.add_szstrmessage(strActorName);

	stringstream ss;
	ss<<pHeroAward->iheroid()<<"#"<<pHeroAward->ilevelstep()<<"#"<<pHeroAward->icount();
	scMsg.add_szstrmessage(ss.str() );

	IJZMessageLayer* pMsgLayer = getComponent<IJZMessageLayer>(COMPNAME_MessageLayer, IID_IJZMessageLayer);
	assert(pMsgLayer);

	GSProto::SCMessage pkg;
	HelpMakeScMsg(pkg, GSProto::CMD_MARQUEE, scMsg);
	
	pMsgLayer->broadcastMsg(pkg);
}


void LegionSystem::randShopSort(int iReqPos, int iAwardPos)
{
	assert(m_shopList.size() > 0);
	std::swap(m_shopList[iReqPos], m_shopList[iAwardPos]);

	IRandom* pRandom = getComponent<IRandom>("Random", IID_IMiniAprRandom);
	assert(pRandom);
	
	vector<int> tmpPosList;
	for(size_t i = 0; i < m_shopList.size(); i++)
	{
		if(0 == m_shopList[i].bBuy)
		{
			tmpPosList.push_back((int)i);
		}
	}

	// 打乱顺序
	vector<ServerEngine::LegionShopItem> tmpNewShopList = m_shopList;
	for(size_t i = 0; i < tmpNewShopList.size(); i++)
	{
		if(tmpNewShopList[i].bBuy)
		{
			continue;
		}
	
		assert(tmpPosList.size() > 0);
		int iRandV = pRandom->random() % tmpPosList.size();
		int iTmpPos = tmpPosList[iRandV];
		
		tmpPosList.erase(tmpPosList.begin() + iRandV);
		tmpNewShopList[i] = m_shopList[iTmpPos];
	}

	// 刷新顺序
	m_shopList = tmpNewShopList;
}

int LegionSystem::randShopPos()
{
	assert(m_shopList.size() > 0);

	vector<int> tmpList;
	for(size_t i = 0; i < m_shopList.size(); i++)
	{
		if(0 == m_shopList[i].bBuy)
		{
			tmpList.push_back((int)i);
		}
	}

	assert(tmpList.size() > 0);

	IRandom* pRandom = getComponent<IRandom>("Random", IID_IMiniAprRandom);
	assert(pRandom);

	int iRandV = pRandom->random() % tmpList.size();

	return tmpList[iRandV];
}


void LegionSystem::fillShopPage(GSProto::ShopPage* pPage)
{	
	LegionFactory* pLegionFactory = static_cast<LegionFactory*>(getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory) );
	assert(pLegionFactory);

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);
	
	int iBuyCost = pGlobalCfg->getInt("军团商店购买消耗", 200);
	int iRefreshCost = pGlobalCfg->getInt("军团商店刷新消耗", 50);

	string strActorName = pEntity->getProperty(PROP_ENTITY_NAME, "");
	GSLegionImp* pLegion = static_cast<GSLegionImp*>(pLegionFactory->getActorLegion(strActorName) );
	if(!pLegion) return;

	const ServerEngine::LegionMemberItem* pMemInfo = pLegion->getMemberInfo(strActorName);
	if(!pMemInfo) return;

	pPage->set_icontribute(pMemInfo->iCurContribute);
	pPage->set_irefreshcost(iRefreshCost);
	pPage->set_ibuycost(iBuyCost);

	for(size_t i = 0; i < m_shopList.size(); i++)
	{
		GSProto::LegionShopItem* pNewShopItem = pPage->mutable_szshoplist()->Add();
		const LegionShopCfg* pShopCfg = pLegionFactory->queryShopCfg(m_shopList[i].iShopID);
		assert(pShopCfg);

		fillScShopItem(m_shopList[i], pShopCfg, pNewShopItem);
	}
}
*/

void LegionSystem::onReqOpenShop(const GSProto::CSMessage& msg)
{
	if(!checkLegionCond() ) return;
	
	sendContributeInfo();
}

void  LegionSystem::sendContributeInfo(bool bHaveRefresh/* = false*/)
{
	//检查军团等级 军团商城二级开放
	IGlobalCfg *pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);
	
	LegionFactory* pLegionFactory = static_cast<LegionFactory*>(getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory) );
	assert(pLegionFactory);

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	string strActorName = pEntity->getProperty(PROP_ENTITY_NAME, "");
	GSLegionImp* pLegion = static_cast<GSLegionImp*>(pLegionFactory->getActorLegion(strActorName) );
	if(!pLegion) return;

	int iOpenLevel = pGlobal->getInt("军团商店开启等级",2);
	int iLegionLevel = pLegion->getLevel();
	if(iOpenLevel > iLegionLevel)
	{
		pEntity->sendErrorCode( ERROR_LEGION_SHOP_OPENLEVEL);
		return;
	}

	//是否需要刷新
	string strRefreshSecond = pGlobal->getString("军团商店刷新时间", "43200#75600");
	vector<int> refreshVec = TC_Common::sepstr<int>( strRefreshSecond, "#");
	assert( refreshVec.size());
	
	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);

	int iNowSecond = pZoneTime->GetCurSecond();
	int iDayBeginSecond = pZoneTime->GetDayBeginSecond(iNowSecond);
	
	int iLastDayRefreshSecond2 = iDayBeginSecond - (ONE_DAY_SECONDS - refreshVec[1]); 
	int iTodayRefreshSecond1 = iDayBeginSecond + refreshVec[0];
	int iTodayRefreshSecond2 = iDayBeginSecond + refreshVec[1];

	bool bNeedRefresh = true;
	//assert(iNowSecond > (int)m_dwLastRefreshSecond);
	bool bIsToday = false;
	int iRefreshSecond = 0;
	if((iLastDayRefreshSecond2<= iNowSecond) && (iNowSecond < iTodayRefreshSecond1 ))
	{
		if(((int)m_dwLastRefreshSecond >= iLastDayRefreshSecond2 ) )
		{
			bNeedRefresh = false;
		}
		
		//刷新时间
		bIsToday = true;
		iRefreshSecond =  refreshVec[0];
	}
	else if((iTodayRefreshSecond1<= iNowSecond) && (iNowSecond < iTodayRefreshSecond2 ))
	{
		if((int)m_dwLastRefreshSecond >= iTodayRefreshSecond1  )
		{
			bNeedRefresh = false;
		}
		
		bIsToday = true;
		iRefreshSecond =  refreshVec[1] ;
	}
	else if(iTodayRefreshSecond2 <= iNowSecond)
	{
		if((int)m_dwLastRefreshSecond >= iTodayRefreshSecond2  )
		{
			bNeedRefresh = false;
		}
		
		bIsToday = false;
		iRefreshSecond = refreshVec[0] + ONE_DAY_SECONDS ;
	}

	if( bNeedRefresh && !bHaveRefresh)
	{
		refreshShop();
		m_dwLastRefreshSecond = iNowSecond;
	}
	
	GSProto::CMD_OPEN_LEGIONSHOP_SC scMsg;
	scMsg.set_bistoday(bIsToday);
	scMsg.set_iupdatemoment(iRefreshSecond);
	
	int iCurContribute= pEntity->getProperty(PROP_ACTOR_LEGION_CONTRIBUTE, 0);

	scMsg.set_icontribute( iCurContribute);
	fillContributeShopDetail( iLegionLevel, scMsg.mutable_szgoodlist());

	pEntity->sendMessage(GSProto::CMD_OPEN_LEGIONSHOP, scMsg);
}


void LegionSystem::fillContributeShopDetail(int iLegionLevel , google::protobuf::RepeatedPtrField< ::GSProto::GoodDetail >* pszGoodDetail)
{
	IShopFactory *pShopFactory = getComponent<IShopFactory>(COMPNAME_ShopFactory, IID_IShopFactory);
	assert(pShopFactory);
	
	for(size_t i = 0; i < m_shopList.size(); ++i)
	{
		const ServerEngine::LegionShopItem& item = m_shopList[i];

		GSProto::GoodDetail* detail = pszGoodDetail->Add();
		
		detail->set_igoodid(item.iShopID);
		detail->set_iindex(item.iIndex);
		detail->set_bissale(item.bBuy==1);

		ShopGoodBaseInfo info;
		bool res = pShopFactory->getGoodById( item.iShopID, info);
		assert(res);
		bool bCanBuy = true;
		if(iLegionLevel < info.iLegionLevel)
		{
			detail->set_icanbuylevel( info.iLegionLevel);
			bCanBuy = false;
		}
		detail->set_bcanbuy( bCanBuy );
		
	}
}

/*
void LegionSystem::fillScShopItem(const ServerEngine::LegionShopItem& shopItemData,const LegionShopCfg* pShopCfg, GSProto::LegionShopItem* pScShopItem)
{
	pScShopItem->set_itype(pShopCfg->awardItem.iType);
	pScShopItem->set_iindex(shopItemData.iIndex);
	pScShopItem->set_bbuyed(shopItemData.bBuy);

	if(pShopCfg->awardItem.iType == GSProto::en_ShopType_Prop)
	{
		GSProto::PropItem* pNewLifeAtt = pScShopItem->mutable_lifeatt();
		pNewLifeAtt->set_ilifeattid(pShopCfg->awardItem.iDropTypeID);
		pNewLifeAtt->set_ivalue(pShopCfg->awardItem.iCount);
	}
	else if(pShopCfg->awardItem.iType == GSProto::en_ShopType_Item)
	{
		GSProto::FightAwardItem* pNewItem = pScShopItem->mutable_item();
		pNewItem->set_iitemid(pShopCfg->awardItem.iDropTypeID);
		pNewItem->set_icount(pShopCfg->awardItem.iCount);
	}
	else if(pShopCfg->awardItem.iType == GSProto::en_ShopType_Hero)
	{
		GSProto::FightAwardHero* pNewHero = pScShopItem->mutable_hero();
		pNewHero->set_iheroid(pShopCfg->awardItem.iDropTypeID);
		pNewHero->set_icount(pShopCfg->awardItem.iCount);
		pNewHero->set_ilevelstep(pShopCfg->awardItem.iHeroLevelStep);
	}
}
*/

struct LegionCreateCb
{
	LegionCreateCb(HEntity hEntity, const string& strLegionName):m_hEntity(hEntity), m_strLegionName(strLegionName){}

	void operator()(int iRet)
	{
		IEntity* pEntity = getEntityFromHandle(m_hEntity);
		assert(pEntity);

		IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
		assert(pGlobalCfg);

		//int iNeedSilver = pGlobalCfg->getInt("创建军团消耗", 1000000);
		int iNeedGold = pGlobalCfg->getInt("创建军团消耗", 500 );
		if(iRet == ServerEngine::en_DataRet_OK)
		{
			// 发送基础信息
			ILegionFactory* pLegionFactory = getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory);
			assert(pLegionFactory);

			GSLegionImp* pLegionImp = static_cast<GSLegionImp*>(pLegionFactory->findLegion(m_strLegionName));
			assert(pLegionImp);

			pLegionImp->sendBaseInfo(m_hEntity);

			PLAYERLOG(pEntity)<<"CreateLegion|"<<m_strLegionName<<"|"<<iNeedGold<<endl;
		}
		else if(iRet == ServerEngine::en_DataRet_Dunplicated)
		{
			pEntity->sendErrorCode(ERROR_LEGIONNAME_EXIST);
			pEntity->changeProperty(PROP_ACTOR_GOLD, iNeedGold, GSProto::en_Reason_CreateLegionFail);
		}
		else
		{
			pEntity->sendErrorCode(ERROR_SYSERROR);
			pEntity->changeProperty(PROP_ACTOR_GOLD, iNeedGold, GSProto::en_Reason_CreateLegionFail);
		}
	}

private:

	HEntity m_hEntity;
	string m_strLegionName;
};

void LegionSystem::onReqCreateLegion(const GSProto::CSMessage& msg)
{
	if(!checkLegionCond() ) return;

	GSProto::CMD_CREATE_LEGION_CS req;

	if(!req.ParseFromString(msg.strmsgbody() ) )
	{
		return;
	}

	
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	string strLegionName = req.strlegionname();

	if(strLegionName=="")
	{
		pEntity->sendErrorCode(ERROR_LEGIONAME_INVALID);
		return;
	}
	
	int iLeginIconId = req.ilegioniconid();
	// 验证金钱够不够
	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	//int iNeedSilver = pGlobalCfg->getInt("创建军团消耗", 1000000);

	int iNeedGold = pGlobalCfg->getInt("创建军团消耗", 500 );
	
	int iTkGold = pEntity->getProperty(PROP_ACTOR_GOLD, 0);
	if(iTkGold < iNeedGold)
	{
		pEntity->sendErrorCode(ERROR_NEED_GOLD);
		return;
	}

	ILegionFactory* pLegionFactory = getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory);
	assert(pLegionFactory);

	if(pLegionFactory->newLegion(m_hEntity, strLegionName, LegionCreateCb(m_hEntity, strLegionName), iLeginIconId) )
	{
		pEntity->changeProperty(PROP_ACTOR_GOLD, 0-iNeedGold, GSProto::en_Reason_CreateLegionConsume);
	}
}


void LegionSystem::onReqClickLegion(const GSProto::CSMessage& msg)
{
	if(!checkLegionCond() ) return;

	LegionFactory* pLegionFactory = static_cast<LegionFactory*>(getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory) );
	assert(pLegionFactory);

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	string strActorName = pEntity->getProperty(PROP_ENTITY_NAME, "");
	GSLegionImp* pLegion = static_cast<GSLegionImp*>(pLegionFactory->getActorLegion(strActorName) );

	if(!pLegion)
	{
		pLegionFactory->sendLegionPage(m_hEntity, 0);
	}
	else
	{
		pLegion->sendBaseInfo(m_hEntity);
	}
}

 void LegionSystem::onReqQueryLegionList(const GSProto::CSMessage& msg)
{
	if(!checkLegionCond() ) return;

	GSProto::CMD_QUERY_LEGIONLIST_CS req;
	if(!req.ParseFromString(msg.strmsgbody() ) )
	{
		return;
	}

	int iPageIndex = req.ipageindex();

	LegionFactory* pLegionFactory = static_cast<LegionFactory*>(getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory) );
	assert(pLegionFactory);
	pLegionFactory->sendLegionPage(m_hEntity, iPageIndex);
}

void LegionSystem::onReqAppLegion(const GSProto::CSMessage& msg)
{
	if(!checkLegionCond() ) return;

	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	GSProto::CMD_APPJOIN_LEGION_CS req;
	if(!req.ParseFromString(msg.strmsgbody() ) )
	{
		return;
	}

	//可申请次数
	IGlobalCfg *pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);

	int iTotolTimeOneWeek = pGlobal->getInt("每周加入可加入军团次数" , 3);
	
	int iWeekAddLegion = getWeekResetValue(m_actorWeekAddLegionTimes).iValue;
	int iRemainTimes = iTotolTimeOneWeek - iWeekAddLegion;
	if(iRemainTimes == 0)
	{
		pEntity->sendErrorCode(ERROR_LEGION_WEEK_APP_TIMES);
		return;
	}
	
	GSLegionImp* pLegion = static_cast<GSLegionImp*>(getLegionFromHandle(req.dwlegionobjectid() ) );
	if(!pLegion) return;

	string strLegionName = pLegion->getLegionName();
	map<string, unsigned int>::iterator iter = m_actorHaveAddLegionList.find(strLegionName);
	if( iter != m_actorHaveAddLegionList.end())
	{
		IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
		assert(pZoneTime);
		
		int iLeaveSeconds = pZoneTime->GetCurSecond() - iter->second;
		
		if( iLeaveSeconds < ONE_DAY_SECONDS*2)
		{
			pEntity->sendErrorCode( ERROR_LEGION_LEAVE_SECOND);
			return;
		}
	}
	
	pLegion->applyJoin(m_hEntity);
}

void LegionSystem::dispatchLegion(const GSProto::CSMessage& msg)
{
	if(!checkLegionCond() ) return;

	ILegionFactory* pLegionFactory = getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory);
	assert(pLegionFactory);

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	string strActorName = pEntity->getProperty(PROP_ENTITY_NAME, "");
	ILegion* pLegion = pLegionFactory->getActorLegion(strActorName);

	if(!pLegion) return;

	pLegion->onMessage(m_hEntity, msg);
}

void LegionSystem::onReqLeave(const GSProto::CSMessage& msg)
{
	IEntity* pActor = getEntityFromHandle(m_hEntity);
	if(!pActor) return;
	
	ILegionFactory* pLegionFactory = getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory);
	assert(pLegionFactory);
	
	string strActorName = pActor->getProperty(PROP_ENTITY_NAME, "");
	GSLegionImp* pLegion = static_cast<GSLegionImp*>(pLegionFactory->getActorLegion(strActorName));
	if(!pLegion) return;
	
	int iOccupation = pLegion->getOccupation(strActorName);

	if(iOccupation == GSProto::en_LegionOccupation_Leader)
	{
		pActor->sendErrorCode(ERROR_LEADER_CONNOTLEAVE);
		return;
	}

	if(! pLegion->getMemberInfo(strActorName) )
	{
		return;
	}
	//退出军团确认

	IScriptEngine *pScrip = getComponent<IScriptEngine>(COMPNAME_ScriptEngine, IID_IScriptEngine);
	assert(pScrip);

	IGlobalCfg *pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);

	int iTotolTimeOneWeek = pGlobal->getInt("每周加入可加入军团次数" , 3);
	
	int iWeekAddLegion = getWeekResetValue(m_actorWeekAddLegionTimes).iValue;
	int iRemainTimes = iTotolTimeOneWeek - iWeekAddLegion;
	
	EventArgs args;
	args.context.setInt("entity", m_hEntity);
	args.context.setInt("remaindTimes",iRemainTimes);
	
	pScrip->runFunction("leaveLegionNotice",&args, "EventArgs");
}

void LegionSystem::confirmLeaveLegion()
{
	IEntity* pActor = getEntityFromHandle(m_hEntity);
	if(!pActor) return;

	LegionFactory* pLegionFactory = static_cast<LegionFactory*>(getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory) );
	assert(pLegionFactory);
	
	string strActorName = pActor->getProperty(PROP_ENTITY_NAME, "");
	GSLegionImp* pLegion = static_cast<GSLegionImp*>(pLegionFactory->getActorLegion(strActorName));
	if(!pLegion) return;
		
	if(pLegionFactory->bLegionInCampBattle(pLegion->getLegionName()) )
	{
		pActor->sendErrorCode(ERROR_LEGION_INFIGHTING);
		return;
	}
	
	
	int iOccupation = pLegion->getOccupation(strActorName);

	if(iOccupation == GSProto::en_LegionOccupation_Leader)
	{
		pActor->sendErrorCode(ERROR_LEADER_CONNOTLEAVE);
		return;
	}

	if(! pLegion->getMemberInfo(strActorName) )
	{
		return;
	}
	
	// 从成员列表删除removeLegionMember

	ServerEngine::LegionMemberItem *pItem =  pLegion->getMemberInfoByName(strActorName);
	assert(pItem);
	int iCangetContribute = pItem->beWorshipGetContribute;

	m_actorHaveAddLegionList[pLegion->getLegionName()] =  time(0);
		
	pLegion->removeLegionMember(strActorName);

	// 从排名列表删除	
	pLegion->removeAcotrFromSortList(strActorName);

	// 同步名字Cache
	pLegionFactory->removeActorNameMap(strActorName);

	GSProto::CMD_LEAVE_LEGION_SC scMsg;

	scMsg.set_igetcontribute(iCangetContribute);

	pActor->sendMessage(GSProto::CMD_LEAVE_LEGION, scMsg);


	PLAYERLOG(pActor)<<"LeaveLegion|"<<pLegion->getLegionName()<<endl;
	
}

void  LegionSystem::addJionLegionTimes(int iTimes)
{
	getWeekResetValue(m_actorWeekAddLegionTimes).iValue += iTimes;
}

ServerEngine::TimeResetValue& LegionSystem::getWorshipTimes()
{
	return getDayResetValue( m_useWorshipTimes);
}
map<string,unsigned int >& LegionSystem::getActorHaveWorshipList()
{
	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);

	unsigned int iCurMoument = pZoneTime->GetCurSecond();
		
	map<string,unsigned int >::iterator iter = m_actorHaveWorship.begin();
	for(; iter != m_actorHaveWorship.end(); )
	{
		if( ! pZoneTime->IsInSameDay(iCurMoument, iter->second))
		{
			m_actorHaveWorship.erase(iter++);
			continue;
		}

		++iter;
	}

	return m_actorHaveWorship;
}


		

void LegionSystem::packSaveData(string& data)
{
	// 更新下军团中信息
	LegionFactory* pLegionFactory = static_cast<LegionFactory*>(getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory) );
	assert(pLegionFactory);

	pLegionFactory->updateActorInfo(m_hEntity);
		
	// 存储数据
	ServerEngine::LegionSystemData saveData;
	saveData.shopItemList = m_shopList;
	
	saveData.giftList = m_giftList;

	saveData.blessData = m_blessData;
	saveData.dayAwardFlag = m_dayAwardFlag;
	saveData.weekAwardFlag = m_weekAwardFlag;

	saveData.dwLastRefreshSecond = m_dwLastRefreshSecond;
	saveData.contributeShopRefreshTimes = m_contributeShopRefreshTimes;
	saveData.actorHaveAddLegionList = m_actorHaveAddLegionList;
	saveData.actorWeekAddLegionTimes = m_actorWeekAddLegionTimes;

	saveData.useWorshipTimes = m_useWorshipTimes;
	saveData.contributeTimes = m_contributeTimes;
    saveData.actorHaveWorship = m_actorHaveWorship;
	
	data = ServerEngine::JceToStr(saveData);
}

void LegionSystem::setLegionGuard(int iCampId, string strLegionName)
{	
	LegionFactory* pLegionFactory = static_cast<LegionFactory*>(getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory) );
	assert(pLegionFactory);

	pLegionFactory->setLegionGuard(iCampId,strLegionName);
}

void LegionSystem::openCampBattle(int iReportBegin,int iBattleBegin, int iBattleOver)
{
	LegionFactory* pLegionFactory = static_cast<LegionFactory*>(getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory) );
	assert(pLegionFactory);

	pLegionFactory->openCampBattle(iReportBegin, iBattleBegin, iBattleOver);
}



