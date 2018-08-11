#include "CoreImpPch.h"
#include "../GameEngine/Entity.h"
#include "Actor.h"
#include "IActorSaveSystem.h"
#include "LogHelp.h"
#include "IDungeonSystem.h"
#include "IFormationSystem.h"
#include "IVIPFactory.h"
#include "IHeroSystem.h"
#include "IItemFactory.h"
#include "IItemSystem.h"
#include "ILegionFactory.h"

static int s_szSaveProp[] = {PROP_ACTOR_ACCOUNT, PROP_ACTOR_ROLEPOS, PROP_ENTITY_NAME, PROP_ENTITY_LEVEL, PROP_ENTITY_EXP,
	PROP_ENTITY_UUID, PROP_ENTITY_PHYSTRENGTH, PROP_ACTOR_FSCHIPCOUNT, PROP_ENTITY_HEROEXP, PROP_ACTOR_SILVER, PROP_ACTOR_GOLD,
	PROP_ENTITY_FUNCTIONMASK, PROP_ACTOR_VIPLEVEL, PROP_ACTOR_WORLD, PROP_ENTITY_HONOR, PROP_LAST_PVETIME,PROP_ENTITY_LASTPHYSTRENGTHTIME,
	PROP_ACTOR_RECHARGED,PROP_ACTOR_VIPEXP, PROP_ENTITY_SAODANGPOINT, PROP_ENTITY_LASTSAODANG_RESUMETIME,PROP_ACTER_MAX_FIGHTVALUE,
	PROP_ENTITY_ACTOR_HEAD,PROP_ENTITY_ACTOR_HEADTYPE,PROP_ENTITY_ACTOR_VIGOR, PROP_ACTOR_LEGION_CONTRIBUTE, PROP_ACTOR_SHOWFIRSTPAY, PROP_ACTOR_ACC_PAYMENT };


extern "C" IObject* createActor()
{
	return new Actor(GSProto::en_class_Actor); 
}


extern "C" IObject* createGhost()
{
	return new Actor(GSProto::en_class_Ghost);
}

extern "C" IObject* createMachine()
{
	return new Actor(GSProto::en_class_Ghost);
}


extern void HelpMakeScMsg(GSProto::SCMessage& outPkg, int iCmd, const ::google::protobuf::Message& msgBody);

static int s_logLifeAtt[PROP_ENTITY_MAX] = {0};
static bool s_bInitLogLifeAtt = false;
static map<int, int> s_functionFilterMap;

extern int Prop2LifeAtt(int iPropID);

Actor::Actor(int iClassID)
{
	setProperty(PROP_ENTITY_CLASS, iClassID);
	if(GSProto::en_class_Actor == iClassID)
	{
		getEventServer()->subscribeEvent(EVENT_ENTITY_RELOGIN, this, &Actor::onEventReLogin);

		getEventServer()->subscribeEvent(EVENT_ENTITY_FIGHTVALUECHG,this,&Actor::onEventFightValueChg);
	}

	if(!s_bInitLogLifeAtt)
	{
		s_bInitLogLifeAtt = true;
		s_logLifeAtt[PROP_ACTOR_SILVER] = 1;
		s_logLifeAtt[PROP_ACTOR_GOLD] = 1;
		s_logLifeAtt[PROP_ENTITY_PHYSTRENGTH] = 1;
		s_logLifeAtt[PROP_ACTOR_FSCHIPCOUNT] = 1;
		s_logLifeAtt[PROP_ENTITY_HEROEXP] = 1;
		s_logLifeAtt[PROP_ENTITY_HONOR] = 1;
		s_logLifeAtt[PROP_ENTITY_ACTOR_VIGOR] = 1;
		// 功能过滤
		//s_functionFilterMap[IID_IDomainSystem] = GSProto::en_Function_Domain;
		//s_functionFilterMap[IID_ILegionSystem] = GSProto::en_Function_Legion;
		
		s_functionFilterMap[IID_IGodAnimalSystem] = GSProto::en_Function_GodAnimal;
		s_functionFilterMap[IID_IArenaSystem] = GSProto::en_Function_Arena;
		//s_functionFilterMap[IID_IFriendSystem] = GSProto::en_Function_Friend;
		//s_functionFilterMap[IID_I] = GSProto::en_Function_WorldBoss;
		s_functionFilterMap[IID_IFightSoulSystem] = GSProto::en_Function_FightSoul;
		//s_functionFilterMap[IID_ICampBattleSystem] = GSProto::en_Function_CampFight;
		s_functionFilterMap[IID_IClimbTowerSystem] = GSProto::en_Function_ClimTower;
		s_functionFilterMap[IID_IDreamLandSystem] = GSProto::en_Function_DreamLand;
		s_functionFilterMap[IID_IManorSystem] = GSProto::en_Function_Manor;
	}
	//设置玩家VIP等级
	//setProperty(PROP_ACTOR_VIPLEVEL, 6);
}

Actor::~Actor()
{
	int iClassID = getProperty(PROP_ENTITY_CLASS, 0);
	if(GSProto::en_class_Actor == iClassID)
	{
		getEventServer()->unsubscribeEvent(EVENT_ENTITY_RELOGIN, this, &Actor::onEventReLogin);
		getEventServer()->unsubscribeEvent(EVENT_ENTITY_FIGHTVALUECHG,this,&Actor::onEventFightValueChg);
	}
}



bool Actor::createEntity(const std::string& strData)
{
	ServerEngine::ActorCreateContext createContext;
	ServerEngine::JceToObj(strData, createContext);

	// 设置RS信息
	Entity::setProperty(PROP_ACTOR_RS, createContext.strRsAddress.c_str() );
	setInt64Property(PROP_ACTOR_CON, createContext.ddCon);
	
	// 初始化基础属性
	intitBaseProp(createContext.data.basePropData);

	// 初始化子系统数据
	initSubsystem(createContext.data);
	
	// 执行子系统createComplete 函数
	completeSubsystem();
	
	setProperty(PROP_ACTOR_CREATEFINISH, 1);
	sendToClientInfo();
	
	return true;
}

void Actor::changeProperty(PropertySet::PropertyKey iKey, int iChgValue, int iReason)
{
	if(1 == s_logLifeAtt[iKey])
	{
		int iLifeAtt = Prop2LifeAtt(iKey);
		PLAYERLOG(this)<<"ChgProperty|"<<iLifeAtt<<"|"<<iChgValue<<"|"<<iReason<<endl;

		int iClassID = getProperty(PROP_ENTITY_CLASS, 0);
		if(GSProto::en_class_Actor == iClassID)
	 	{
			if(iLifeAtt == GSProto::en_LifeAtt_Gold || iLifeAtt == GSProto::en_LifeAtt_Silver)
			{
				int iSrcWorldID = getProperty(PROP_ACTOR_WORLD, 0);
				STAT_GAMECOIN<<iLifeAtt<<"|"<<iChgValue<<"|"<<iReason<<"|"<<iSrcWorldID<<endl;
			}
			
			if((getProperty(PROP_ACTOR_FIRSTCONSUME, 0)==0) && (iLifeAtt == GSProto::en_LifeAtt_Gold)&&(iChgValue<0))
			{
				setProperty( PROP_ACTOR_FIRSTCONSUME,1);
				STAT_FIRSTCONSUME<<iReason<<"|"<<iChgValue<<endl;
			}
     	}
	}

	Entity::changeProperty(iKey, iChgValue, iReason);
}


void Actor::setProperty(PropertySet::PropertyKey key, Int32 nValue)
{
	// 金钱上下限设置下
	if( (PROP_ACTOR_SILVER == key) ||(PROP_ACTOR_GOLD == key) || (PROP_ENTITY_HEROEXP == key) )
	{
		int iBkValue = nValue;
		int iOldValue = getProperty(key, 0);
		if(nValue < 0)
		{
			nValue = 0;
			PLAYERLOG(this)<<"invalidMoney|"<<key<<"|"<<iOldValue<<"|"<<iBkValue<<"|"<<nValue<<endl;
		}
		
		if(nValue > GSProto::MAX_MONEY_VALUE)
		{
			nValue = GSProto::MAX_MONEY_VALUE;
			PLAYERLOG(this)<<"invalidMoney|"<<key<<"|"<<iOldValue<<"|"<<iBkValue<<"|"<<nValue<<endl;
		}
	}

	Entity::setProperty(key, nValue);	
}

void Actor::onEventReLogin(EventArgs& args) 
{
	sendToClientInfo();
}

//记录最大战斗力
void Actor::onEventFightValueChg(EventArgs& args)
{
	int iOldFightValue = getProperty(PROP_ACTER_MAX_FIGHTVALUE,0);

	int iNewFightValue = getProperty(PROP_ENTITY_FIGHTVALUE,0);

	if(iOldFightValue <  iNewFightValue)
	{
		setProperty( PROP_ACTER_MAX_FIGHTVALUE,iNewFightValue);
	}
}

void Actor::sendErrorCode(int iErrorCode)
{
	// actor 才能发送
	int iClassID = getProperty(PROP_ENTITY_CLASS, 0);
	if(GSProto::en_class_Actor != iClassID)
	{
		return;
	}

	if(iErrorCode == ERROR_NEED_SILVER)
	{
		sendMessage(GSProto::CMD_NEED_SILVER);
		return;
	}

	if(iErrorCode == ERROR_NO_HEROEXP)
	{
		sendMessage(GSProto::CMD_NEED_HEROEXP);
		return;
	}

	IMessageLayer* pMsgLayer = getComponent<IMessageLayer>(COMPNAME_MessageLayer, IID_IMessageLayer);
	assert(pMsgLayer);

	string strRsAddress = getProperty(PROP_ACTOR_RS, "");
	Int64 ddConn = getInt64Property(PROP_ACTOR_CON, 0);
	pMsgLayer->sendErrorMessage2Connection(strRsAddress, ddConn, iErrorCode);
}

void Actor::sendToClientInfo()
{
	// 发送GetRole下行
	{
		GSProto::Cmd_Sc_GetRole getRoleMsg;
		getRoleMsg.set_iresult(GSProto::en_GetRole_OK);
		GSProto::RoleBase* pRoleBaseData = getRoleMsg.mutable_rolebasedata();
		assert(pRoleBaseData);

		int iLevel = getProperty(PROP_ENTITY_LEVEL, 0);
		int iExp = getProperty(PROP_ENTITY_EXP, 0);
		string strName = getProperty(PROP_ENTITY_NAME, "");
		int iPhyStrength = getProperty(PROP_ENTITY_PHYSTRENGTH, 0);
		int iSilver = getProperty(PROP_ACTOR_SILVER, 0);
		int iGold = getProperty(PROP_ACTOR_GOLD, 0);
		int iHeroCvtCount = getProperty(PROP_ENTITY_HEROCVTCOUNT, 0);

		int iVipLevel = getProperty( PROP_ACTOR_VIPLEVEL,0);
		int iVipLevelExp = getProperty( PROP_ACTOR_VIPEXP,0);
	
		int iShowFirstPay = getProperty( PROP_ACTOR_SHOWFIRSTPAY,0);
		bool bShowFirstBtn = (iShowFirstPay != 2);
		
		IVIPFactory* pVipFactory = getComponent<IVIPFactory>(COMPNAME_VIPFactory,IID_IVIPFactory);
		assert(pVipFactory);
		
       	int iVipLevelUpExp = 0;
		if(pVipFactory->getVipTopLevel() >= iVipLevel+1 )
		{
			iVipLevelUpExp =  pVipFactory->getVipPropByVIPLevel( iVipLevel,VIP_PROP_VIPEXP);
		}
		
		int iHeadId = getProperty(PROP_ENTITY_ACTOR_HEAD, 0);
		GSProto::enHeadType iheadType = (GSProto::enHeadType)getProperty(PROP_ENTITY_ACTOR_HEADTYPE, 0);
		
		pRoleBaseData->set_dwobjectid(m_hHandle);
		pRoleBaseData->set_ilevel(iLevel);
		pRoleBaseData->set_iexp(iExp);
		pRoleBaseData->set_strname(strName);
		pRoleBaseData->set_iphystrength(iPhyStrength);
		pRoleBaseData->set_isilver(iSilver);
		pRoleBaseData->set_igold(iGold);
		pRoleBaseData->set_iheroconvertcount(iHeroCvtCount);
		pRoleBaseData->set_iviplevel(iVipLevel);
		pRoleBaseData->set_ivipexp(iVipLevelExp);
		pRoleBaseData->set_iviplevelupexp(iVipLevelUpExp);
		pRoleBaseData->set_iheadid(iHeadId);
		pRoleBaseData->set_iheadtype(iheadType);
		pRoleBaseData->set_bshowfirstpaybtn( bShowFirstBtn);

		//军团归属
		ILegionFactory* pLegionFactory = getComponent<ILegionFactory>(COMPNAME_LegionFactory,IID_ILegionFactory);
		assert(pLegionFactory);
		
		ILegion* pLengion = pLegionFactory->getActorLegion(strName);
		string belongName = "";
		if(pLengion)
		{
			belongName = pLengion->getLegionName();
			pRoleBaseData->set_strlegionname(belongName);
		}
		
		ITable* pExpTable = getCompomentObjectManager()->findTable(TABLENAME_Exp);
		assert(pExpTable);

		int iRecord = pExpTable->findRecord(iLevel);
		assert(iRecord >= 0);
		int iLevelUpExp = pExpTable->getInt(iRecord, "经验");
		pRoleBaseData->set_ilevelupexp(iLevelUpExp);

		static int s_norifyLifeAtt[] = {PROP_ENTITY_HEROEXP, PROP_ENTITY_PHYSTRENGTHLIMIT, PROP_ENTITY_FUNCTIONMASK, PROP_ENTITY_HONOR,
									PROP_ENTITY_FIGHTVALUE,PROP_ACTOR_NOTICE,PROP_ENTITY_ACTOR_VIGOR,
									PROP_ENTITY_STRENGTHBUYTIMES};
		for(size_t i = 0; i < sizeof(s_norifyLifeAtt)/sizeof(s_norifyLifeAtt[0]); i++)
		{
			GSProto::PropItem* pNewItem = pRoleBaseData->mutable_szproplist()->Add();
			int iTmpLifeAtt = Prop2LifeAtt(s_norifyLifeAtt[i]);
			pNewItem->set_ilifeattid(iTmpLifeAtt);
			int iTmpV = getProperty(s_norifyLifeAtt[i], 0);
			pNewItem->set_ivalue(iTmpV);
		}

		GSProto::SCMessage pkg;
		HelpMakeScMsg(pkg, GSProto::CMD_GETROLE, getRoleMsg);
		sendMessage(pkg);
	}

	// 触发完全创建Actor事件(用于客户端通知)
	int iClassID = getProperty(PROP_ENTITY_CLASS, 0);
	if(GSProto::en_class_Actor == iClassID)
	{
		EventArgs args;
		args.context.setInt("entity", m_hHandle);
		getEventServer()->setEvent(EVENT_ENTITY_SENDACTOR_TOCLIENT, args);
		getEventServer()->setEvent(EVENT_ENTITY_SENDACTOR_TOCLIENT_POST, args);
	}

	// 发送fin
	{
		GSProto::SCMessage pkg;
		pkg.set_icmd(GSProto::CMD_ROLE_FIN);
		sendMessage(pkg);
	}
}

void Actor::packSaveData(ServerEngine::RoleSaveData& data)
{
	packBaseProp(data, s_szSaveProp, (int)(sizeof(s_szSaveProp)/sizeof(s_szSaveProp[0])) );

	// 子系统存储
	packSubsystem(data);
}


void Actor::save()
{
	int iClassID = getProperty(PROP_ENTITY_CLASS, 0);
	if(GSProto::en_class_Actor != iClassID)
	{
		return;
	}
	
	IActorSaveSystem* pActorSaveSys = static_cast<IActorSaveSystem*>(querySubsystem(IID_IActorSaveSystem) );
	assert(pActorSaveSys);

	pActorSaveSys->doSave(false);
}

void Actor::sendMessage(const GSProto::SCMessage& scMessage)
{
	int iClassID = getProperty(PROP_ENTITY_CLASS, 0);
	if(GSProto::en_class_Actor != iClassID)
	{
		return;
	}

	IMessageLayer* pMsgLayer = getComponent<IMessageLayer>(COMPNAME_MessageLayer, IID_IMessageLayer);
	assert(pMsgLayer);

	string strRSAddress = getProperty(PROP_ACTOR_RS, "");
	Int64 ddCon = getInt64Property(PROP_ACTOR_CON, 0);

	pMsgLayer->sendMessage2Connection(strRSAddress, ddCon, scMessage);
}

void Actor::sendMessage(int iCmd, const ::google::protobuf::Message& msgBody)
{
	GSProto::SCMessage pkg;
	HelpMakeScMsg(pkg, iCmd, msgBody);

	sendMessage(pkg);
}

void Actor::sendMessage(int iCmd)
{
	GSProto::SCMessage pkg;
	pkg.set_icmd(iCmd);

	sendMessage(pkg);
}


void Actor::onMessage(QxMessage* pMessage)
{
	// 按照功能开启,这里统一过滤下
	SubsystemMsgMap::iterator it = m_subsystemMsgMap.find(pMessage->dwMsgID);
	if(it == m_subsystemMsgMap.end() )
	{
		return;
	}

	IEntitySubsystem* pSubsystem = it->second;
	assert(pSubsystem);

	static IJZEntityFactory* pJZEntityFactory = NULL;
	if(!pJZEntityFactory)
	{
		pJZEntityFactory = getComponent<IJZEntityFactory>(COMPNAME_EntityFactory, IID_IJZEntityFactory);
		assert(pJZEntityFactory);
	}
	
	if(s_functionFilterMap.find(pSubsystem->getSubsystemID() ) != s_functionFilterMap.end() )
	{
		int iFunction = s_functionFilterMap.at(pSubsystem->getSubsystemID() );
		const FunctionOpenDesc* pDescFunction = pJZEntityFactory->getFunctionData(iFunction);
		if(!isFunctionOpen(iFunction) && pDescFunction)
		{
			sendErrorCode(pDescFunction->iErrorCode);
			return;
		}
	}

	pSubsystem->onMessage(pMessage);
}

void Actor::addExp(int iExp)
{
	//关闭上浮
	HEntity hEntity = this->getHandle();
	CloseAttCommUP close(hEntity);
	
	ITable* pExpTable = getCompomentObjectManager()->findTable(TABLENAME_Exp);
	assert(pExpTable);

	int iTmpExp = iExp;
	int iLevel = getProperty(PROP_ENTITY_LEVEL, 0);
	int iCurExp = getProperty(PROP_ENTITY_EXP, 0);
	int iOldLv = iLevel;
	while(true)
	{	
		int iRecord = pExpTable->findRecord(iLevel);
		assert(iRecord >= 0);

		int iNeedExp = pExpTable->getInt(iRecord, "经验");		
		int iLeftExp = iNeedExp - iCurExp;

		if(iTmpExp < iLeftExp)
		{
			iCurExp += iTmpExp;
			break;
		}

		// 是否已经达到最大等级了?
		if(pExpTable->findRecord(iLevel + 1) < 0)
		{
			iCurExp = iNeedExp-1;
			break;
		}

		iLevel++;
		iTmpExp -= iLeftExp;
		iCurExp = 0;
	}

	IFormationSystem* pFormationSys = static_cast<IFormationSystem*>(querySubsystem(IID_IFormationSystem));
	assert(pFormationSys);

	int iPreLimitHeroCount = pFormationSys->getFormationHeroLimit();

	setProperty(PROP_ENTITY_LEVEL, iLevel);
	setProperty(PROP_ENTITY_EXP, iCurExp);
	
	if(iOldLv != iLevel)
	{
		EventArgs args;
		args.context.setInt("entity", getHandle() );
		args.context.setInt("oldlv", iOldLv);
		args.context.setInt("newlv", iLevel);
		getEventServer()->setEvent(EVENT_ENTITY_LEVELUP, args);
	}

	int iNewLvRecrod = pExpTable->findRecord(iLevel);
	assert(iNewLvRecrod);

	int iLevelUpExp = pExpTable->getInt(iNewLvRecrod, "经验");
	int iPrePhyStrength = getProperty(PROP_ENTITY_PHYSTRENGTH, 0);

	// 升级奖励体力
	//if(iOldLv != iLevel)
	for(int i = iOldLv+1; i<= iLevel; i++)
	{
		int iTmpRecord = pExpTable->findRecord(i);
		assert(iTmpRecord >= 0);
	
		int iAwardPhyStrength = pExpTable->getInt(iTmpRecord, "体力");
		changeProperty(PROP_ENTITY_PHYSTRENGTH, iAwardPhyStrength, GSProto::en_Reason_LevelUpCreate);

		// 奖励修为
		int iAddHeroExp = pExpTable->getInt(iTmpRecord, "修为");
		changeProperty(PROP_ENTITY_HEROEXP, iAddHeroExp, GSProto::en_Reason_LevelUpCreate);

		// 奖励英雄
		string strHeroList = pExpTable->getString(iTmpRecord, "英雄");
		if(strHeroList == "formation")
		{
			addAllFormationHero();
		}
		else
		{
			vector<int> heroIDList = TC_Common::sepstr<int>(strHeroList, "#");
			assert(heroIDList.size() % 2 == 0);
			addHeroList(heroIDList);
		}

		// 添加道具
		IItemSystem* pItemSys = static_cast<IItemSystem*>(this->querySubsystem(IID_IItemSystem) );
		assert(pItemSys);
		
		string strItemList = pExpTable->getString(iTmpRecord, "道具");
		vector<int> itemList = TC_Common::sepstr<int>(strItemList, "#");
		for(size_t i = 0; i < itemList.size(); i++)
		{
			pItemSys->addItem(itemList[i], 1, GSProto::en_Reason_LevelUpCreate);
		}
	}

	int iCurPhyStrength = getProperty(PROP_ENTITY_PHYSTRENGTH, 0);
	int iCurLimitHeroCount = pFormationSys->getFormationHeroLimit();

	// 通知客户端
	GSProto::Cmd_Sc_ActorLevelUp scMsg;
	scMsg.set_ilevel(iLevel);
	scMsg.set_iexp(iCurExp);
	scMsg.set_ilevelupexp(iLevelUpExp);
	scMsg.set_iprelevel(iOldLv);
	scMsg.set_iprephystrength(iPrePhyStrength);
	scMsg.set_icurphystrength(iCurPhyStrength);
	scMsg.set_ipreformationherolimit(iPreLimitHeroCount);
	scMsg.set_icurformationherolimit(iCurLimitHeroCount);
	

	GSProto::SCMessage pkg;
	HelpMakeScMsg(pkg, GSProto::CMD_ACTOR_LEVELUP, scMsg);
	sendMessage(pkg);

	// 处理功能开启
	if(iOldLv != iLevel)
	{
		EventArgs args;
		args.context.setInt("entity", getHandle() );
		args.context.setInt("oldlv", iOldLv);
		args.context.setInt("newlv", iLevel);
		getEventServer()->setEvent(EVENT_ENTITY_POSTLEVELUP, args);
	}

	PLAYERLOG(this)<<"AddExp|"<<iExp<<endl;
}


void Actor::addAllFormationHero()
{
	IFormationSystem* pFormationSys = static_cast<IFormationSystem*>(querySubsystem(IID_IFormationSystem) );
	assert(pFormationSys);
	
	vector<HEntity> formationHeroList;
	pFormationSys->getEntityList(formationHeroList);

	IHeroSystem* pHeroSys = static_cast<IHeroSystem*>(querySubsystem(IID_IHeroSystem) );
	assert(pHeroSys);

	for(size_t i = 0; i < formationHeroList.size(); i++)
	{
		IEntity* pTmpHero = getEntityFromHandle(formationHeroList[i]);
		if(!pTmpHero) continue;

		int iHeroID = pTmpHero->getProperty(PROP_ENTITY_BASEID, 0);
		pHeroSys->addHero(iHeroID, false, GSProto::en_Reason_LevelUpCreate);
	}
}

void Actor::addHeroList(const vector<int>& heroList)
{
	IHeroSystem* pHeroSys = static_cast<IHeroSystem*>(querySubsystem(IID_IHeroSystem) );
	assert(pHeroSys);

	assert(heroList.size() % 2 == 0);

	for(size_t i = 0; i < heroList.size()/2; i++)
	{
		int iHeroID = heroList[i*2];
		int iLevelStep = heroList[i*2+1];
		pHeroSys->addHeroWithLevelStep(iHeroID, iLevelStep, false, GSProto::en_Reason_LevelUpCreate);
	}
}


bool Actor::isFunctionOpen(int iFunctionID)
{
	int iMask = getProperty(PROP_ENTITY_FUNCTIONMASK, 0);
	
	return (iMask & (1 << iFunctionID) )!=0;
}


void Actor::enableFunction(int iFunctionID)
{
	int iMask = getProperty(PROP_ENTITY_FUNCTIONMASK, 0);
	iMask |= (1 << iFunctionID);

	setProperty(PROP_ENTITY_FUNCTIONMASK, iMask);

	EventArgs args;
	args.context.setInt("entity", m_hHandle);
	args.context.setInt("function", iFunctionID);

	getEventServer()->setEvent(EVENT_ENTITY_OPENFUNCTION, args);
}

void Actor::addVipExp(int iExp)
{
	IVIPFactory* pVipFactory = getComponent<IVIPFactory>(COMPNAME_VIPFactory,IID_IVIPFactory);
	assert(pVipFactory);
	
	
	int iLevel = getProperty(PROP_ACTOR_VIPLEVEL, 0);
	int iCurExp = getProperty(PROP_ACTOR_VIPEXP, 0);
	int iTmpExp = iCurExp + iExp;
	int iLevelTemp = iLevel;
	while(true)
	{
		if(pVipFactory->getVipTopLevel() == iLevelTemp) break;
		int iNeedExp = pVipFactory->getVipPropByVIPLevel(iLevelTemp,VIP_PROP_VIPEXP);
		if(iTmpExp >= iNeedExp)
		{
			iLevelTemp ++;
			if(iLevelTemp >= pVipFactory->getVipTopLevel())
			{
				iTmpExp = iNeedExp;
				break;
			}
		}
		else
		{
			break;
		}
	}
	
	setProperty(PROP_ACTOR_VIPLEVEL, iLevelTemp);
	setProperty(PROP_ACTOR_VIPEXP, iTmpExp);

	int iNextLevel = std::min(pVipFactory->getVipTopLevel(),iLevelTemp+1);
	int iLevelUpExp =  pVipFactory->getVipPropByVIPLevel(iNextLevel-1,VIP_PROP_VIPEXP);
	

	// 通知客户端
	GSProto::CMD_ACTOR_VIPLEVELUP_SC  s2cMsg;
	s2cMsg.set_iviplevel(iLevelTemp);
	s2cMsg.set_ivipexp(iTmpExp);
	s2cMsg.set_iviplevelupexp(iLevelUpExp);
	s2cMsg.set_ivipprelevel(iLevel);

	sendMessage(GSProto::CMD_ACTOR_VIPLEVELUP,s2cMsg);

	PLAYERLOG(this)<<"AddVIPExp|"<<iExp<<endl;
}

void Actor::chgNotice(int iNoticeId,bool haveNotice)
{
	int iMask = getProperty(PROP_ACTOR_NOTICE, 0);
	int oldMask = iMask;
	if(haveNotice)
	{
		iMask |= (1 << iNoticeId);
	}
	else
	{
		iMask &= ~(1 << iNoticeId);
	}
	if(oldMask == iMask) return;
	setProperty(PROP_ACTOR_NOTICE, iMask);
}


