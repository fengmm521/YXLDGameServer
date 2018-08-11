#include "CoreImpPch.h"
#include "GmSystem.h"
#include "IFightFactory.h"
#include "IFightSystem.h"
#include "IItemFactory.h"
#include "IItemSystem.h"
#include "IHeroSystem.h"
#include "IFightSoulFactory.h"
#include "IFightSoulSystem.h"
#include "IGodAnimalSystem.h"
//#include "IDomainSystem.h"
#include "ILegionFactory.h"
#include "ILegionSystem.h"
#include "util/tc_encoder.h"
#include "ICampBattleFactory.h"
#include "LogHelp.h"
#include "IEquipBagSystem.h"
#include "IDreamLandSystem.h"
#include "IHeroTallentSystem.h"
#include "IManorSystem.h"
#include "IShopSystem.h"
#include "IOperateSystem.h"

static Uint32 s_startTime = time(0);

extern "C" IObject* createGMSystem()
{
	return new GMSystem;
}


bool GMSystem::create(IEntity* pEntity, const std::string& strData)
{
	m_hEntity = pEntity->getHandle();
	
	return true;
}

const std::vector<Uint32>& GMSystem::getSupportMessage()
{
	static std::vector<Uint32> resultList;

	if(resultList.size() == 0)
	{
		resultList.push_back(GSProto::CMD_GM_MSG);
	}

	return resultList;
}

void GMSystem::onMessage(QxMessage* pMessage)
{
	assert(pMessage->dwMsgLen == sizeof(GSProto::CSMessage) );
	const GSProto::CSMessage& msg = *(const GSProto::CSMessage*)(pMessage->pMsgDataBuff);

	GSProto::Cmd_Cs_GmMsg GmMsgBody;
	if(!GmMsgBody.ParseFromString(msg.strmsgbody() ) )
	{
		return;
	}

	string strGmCmd;
	ServerEngine::StringToLower(GmMsgBody.strcmd(), strGmCmd);

	vector<string> cmdList = TC_Common::sepstr<string>(strGmCmd, "#");
	if(cmdList.size() == 0)
	{
		return;
	}

	// 暂时所有的都是GM
	if( (cmdList[0] == "pvefight") && (cmdList.size() == 2) )
	{
		int iMonsterGrpID = AdvanceAtoi(cmdList[1]);
		doPVEFight(iMonsterGrpID);
	}
	else if( (cmdList[0] == "additem") && (cmdList.size() == 3) )
	{
		int iItemID = AdvanceAtoi(cmdList[1]);
		int iItemCount = AdvanceAtoi(cmdList[2]);

		addItem(iItemID, iItemCount);
	}
	else if( (cmdList[0] == "removeitem") && (cmdList.size() == 3) )
	{
		int iItemID = AdvanceAtoi(cmdList[1]);
		int iItemCount = AdvanceAtoi(cmdList[2]);

		removeItem(iItemID, iItemCount);
	}
	else if( (cmdList[0] == "addexp") && (cmdList.size() == 2) )
	{
		int iAddExp = AdvanceAtoi(cmdList[1]);
		IEntity* pActor = getEntityFromHandle(m_hEntity);
		assert(pActor);
		pActor->addExp(iAddExp);
	}
	else if( (cmdList[0] =="addhero") && (cmdList.size() >= 2) )
	{
		int iHeroID = AdvanceAtoi(cmdList[1]);
		int iCount = 1;
		if(cmdList.size() == 3)
		{
			iCount = AdvanceAtoi(cmdList[2]);
		}
		addHero(iHeroID, iCount);
	}
	else if(cmdList[0] == "practice")
	{
		fightSoulPractice();
	}
	else if(cmdList[0] == "enableadvance")
	{
		enableFightSoulAdvance();
	}
	else if(cmdList[0] == "autocombine")
	{
		autoCombineFightSoul();
	}
	else if( (cmdList[0] == "setlifeatt") && (cmdList.size() == 3) )
	{
		int iLifeAttID = AdvanceAtoi(cmdList[1]);
		int iValue = AdvanceAtoi(cmdList[2]);
		if( 17 == iLifeAttID) return;
		setLifeAtt(iLifeAttID, iValue);
	}
	else if( (cmdList.size() == 2) && (cmdList[0] == "fightsoulexchange") )
	{
		int iExchangeID = AdvanceAtoi(cmdList[1]);
		fightSoulExchange(iExchangeID);
	}
	else if( (cmdList[0] == "addgodanimal") && (cmdList.size() == 2))
	{
		int iGodAnimalID = AdvanceAtoi(cmdList[1]);
		addGodAnimal(iGodAnimalID);
	}
	else if(cmdList[0] == "autogodanimal")
	{
		autoGodAnimal();
	}
	else if( (cmdList[0] == "setherolevelstep") && (cmdList.size() == 2) )
	{
		int iLevelStep = AdvanceAtoi(cmdList[1]);
		setHeroLevelStep(iLevelStep);
	}
	else if( (cmdList[0] == "setherolevel") && (cmdList.size() == 2) )
	{
		int iLevel = AdvanceAtoi(cmdList[1]);
		setHeroLevel(iLevel);
	}
	else if( (cmdList[0] == "enabledomain"))
	{
		enableDomain();
	}
	else if( (cmdList[0] == "setlevel") && (cmdList.size() == 2) )
	{
		int iLevel = AdvanceAtoi(cmdList[1]);
		setLevel(iLevel);
	}
	else if( (cmdList[0] == "setlegionlevel") && (cmdList.size() == 2) )
	{
		int iLevel = AdvanceAtoi(cmdList[1]);
		setLegionLevel(iLevel);
	}
	else if(cmdList[0] == "startlegionfight")
	{
		startLegionFight();
	}
	else if(cmdList[0] == "sysinfo")
	{
		sysInfo();
	}
	else if( (cmdList[0] == "sendsysmsg") && (cmdList.size() == 2) )
	{
		sendSysMsg(cmdList[1]);
	}
	else if( (cmdList[0]== "scb")&&(cmdList.size() == 4))
	{
		startCampBattle(cmdList[1],cmdList[2],cmdList[3]);
	}
	else if( (cmdList[0] == "addgift") && (cmdList.size() == 2) )
	{
		int iGiftID = AdvanceAtoi(cmdList[1]);
		addNormalGift(iGiftID);
	}
	else if( (cmdList[0] == "addcustomgift") && (cmdList.size() == 2))
	{
		int iItemID = AdvanceAtoi(cmdList[1]);
		addCustomGift(iItemID);
	}
	else if(cmdList[0] == "testrecharge")
	{
		testRechareLog();
	}
	else if(cmdList[0] == "marquee")
	{
		postMarquee(cmdList);
	}
	else if( (cmdList[0] == "addfightsoul") && (cmdList.size() == 3))
	{
		int iFightSoulID = AdvanceAtoi(cmdList[1]);
		int iCount = AdvanceAtoi(cmdList[2]);
		for(int i = 0; i < iCount; i++)
		{
			addFightSoul(iFightSoulID);
		}
	}
	else if( (cmdList[0] == "multifight") && (cmdList.size() >= 2) )
	{
		vector<int> monsterGrpList;
		for(size_t i = 1; i < cmdList.size(); i++) 
		{
			monsterGrpList.push_back(AdvanceAtoi(cmdList[i]));
		}
		multiFight(monsterGrpList);
	}
	else if( (cmdList[0] == "replay") && (cmdList.size() == 4) )
	{
		int iActionIndex = AdvanceAtoi(cmdList[1]);
		int iResultSetIndex = AdvanceAtoi(cmdList[2]);
		int iTargetPos = AdvanceAtoi(cmdList[3]);
		replay(iActionIndex, iResultSetIndex, iTargetPos);
	}
	else if ((cmdList[0] == "addnum") && (cmdList.size() == 2)  )
	{
		int num = AdvanceAtoi(cmdList[1]);
		
		IEntity *pEntity = getEntityFromHandle(m_hEntity);
		pEntity->addVipExp(num);
	}
	else if( (cmdList[0] == "addequip") && (cmdList.size() == 2) )
	{
		int iEquipID = AdvanceAtoi(cmdList[1]);
		addEquip(iEquipID);
	}
	else if( (cmdList[0] == "dreamfight") && (cmdList.size() == 3))
	{
		int iSectionID = AdvanceAtoi(cmdList[1]);
		int iSceneId =  AdvanceAtoi(cmdList[2]);

		GMTestDreamLand(iSectionID,iSceneId);
			
	}
	else if( (cmdList[0] == "dreamreq") && (cmdList.size() == 3))
	{
		int iSectionID = AdvanceAtoi(cmdList[1]);
		int iSceneId =	AdvanceAtoi(cmdList[2]);

		GMRequsetFormation(iSectionID,iSceneId);
			
	}
	else if( (cmdList[0] == "cleardream") && (cmdList.size() == 1))
	{
		GMClearDreamLandData();
	}
	else if(  (cmdList[0] == "dreamaddreset") && (cmdList.size() == 1))
	{
		GMAddDreamLandResetTimes();
	}
	else if( (cmdList[0] =="addherowithstep") && (cmdList.size() >= 2) )
	{
		int iHeroID = AdvanceAtoi(cmdList[1]);
		int iStep = AdvanceAtoi(cmdList[2]);
		addHeroWithStep(iHeroID, iStep);
	}
	else if( (cmdList[0] == "callhero") && (cmdList.size() == 2) )
	{
		int iHeroID = AdvanceAtoi(cmdList[1]);
		callHero(iHeroID);
	}
	else if( (cmdList[0] == "setheroquality") && (cmdList.size() == 2) )
	{
		int iQuality = AdvanceAtoi(cmdList[1]);
		setAllHeroQuality(iQuality);
	}
	else if((cmdList[0] == "pvpfight") && (cmdList.size() == 4) )
	{
		string strAccount = cmdList[1];
		int iPos = AdvanceAtoi(cmdList[2]);
		int iWorldID = AdvanceAtoi(cmdList[3]);
		pvpFight(strAccount, iPos, iWorldID);
	}
	else if(cmdList[0] == "manorreset")
	{
		IEntity *pEntity = getEntityFromHandle(m_hEntity);
		assert(pEntity);
		IManorSystem *pManor = static_cast<IManorSystem*>(pEntity->querySubsystem(IID_IManorSystem));
		assert(pManor);
		pManor->resetHarvest();
	}
	else if((cmdList[0] == "loot") && (cmdList.size() == 4))
	{
		
		
		string strAccount = cmdList[1];
		int iPos = AdvanceAtoi(cmdList[2]);
		int iWorldID = AdvanceAtoi(cmdList[3]);
		lootFight(strAccount, iPos, iWorldID);
		
		
	}
	else if((cmdList[0] == "addlegionexp") && (cmdList.size() == 2))
	{
		IEntity *pEntity = getEntityFromHandle(m_hEntity);
		assert(pEntity);
		ILegionSystem* pLegionSys = static_cast<ILegionSystem*>(pEntity->querySubsystem(IID_ILegionSystem));
		assert(pLegionSys);

		pLegionSys->gmAddLegionExp(AdvanceAtoi(cmdList[1]));
		
	}
	else if((cmdList[0] == "pay") && (cmdList.size() == 2))
	{
		IEntity *pEntity = getEntityFromHandle(m_hEntity);
		assert(pEntity);
		IShopSystem *pShopSystem = static_cast<IShopSystem*>(pEntity->querySubsystem(IID_IShopSystem));
		assert(pShopSystem);
		
		 pShopSystem->addGoldForPayment(AdvanceAtoi(cmdList[1]), "GM");
	}
	else if((cmdList[0] == "phonenum") && (cmdList.size() == 2))
	{
		IEntity *pEntity = getEntityFromHandle(m_hEntity);
		assert(pEntity);
		IOperateSystem* pOperateSystem = static_cast<IOperateSystem*>(pEntity->querySubsystem(IID_IOperateSystem));
		assert(pOperateSystem);
		pOperateSystem->gmGetPhoneSteam(cmdList[1]);
	}
	else if((cmdList[0] == "login") && (cmdList.size() == 1))
	{
		IEntity *pEntity = getEntityFromHandle(m_hEntity);
		assert(pEntity);
		IOperateSystem* pOperateSystem = static_cast<IOperateSystem*>(pEntity->querySubsystem(IID_IOperateSystem));
		assert(pOperateSystem);
		pOperateSystem->gmLogin();		
	}
	else if((cmdList[0]=="setguard")&&(cmdList.size() == 3))
	{
		IEntity *pEntity = getEntityFromHandle(m_hEntity);
		assert(pEntity);
		ILegionSystem* pLegionSystem = static_cast<ILegionSystem*>(pEntity->querySubsystem(IID_ILegionSystem));
		assert(pLegionSystem);
		pLegionSystem->setLegionGuard(AdvanceAtoi(cmdList[1]),cmdList[2]);	
	}
	else if( (cmdList[0]=="setopen")&&(cmdList.size() == 4))
	{
		IEntity *pEntity = getEntityFromHandle(m_hEntity);
		assert(pEntity);
		ILegionSystem* pLegionSystem = static_cast<ILegionSystem*>(pEntity->querySubsystem(IID_ILegionSystem));
		assert(pLegionSystem);
		pLegionSystem->openCampBattle(AdvanceAtoi(cmdList[1]), AdvanceAtoi(cmdList[2]), AdvanceAtoi(cmdList[3]));	
	}
}



void GMSystem::lootFight(const string& strAccount, int iPos, int iWorldID)
{
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	ServerEngine::PKRole targetKey;
	targetKey.strAccount = strAccount;
	targetKey.rolePos = iPos;
	targetKey.worldID = iWorldID;
	
	IManorSystem *pManor = static_cast<IManorSystem*>(pEntity->querySubsystem(IID_IManorSystem));
	assert(pManor);
	pManor->GMLootFight(targetKey);
}



struct GMPVPFight
{
	GMPVPFight(HEntity hEntity):m_hMaster(hEntity){}

	void operator()(taf::Int32 iret, ServerEngine::BattleData& data)
	{
		if(iret != en_FightResult_OK)
		{
			return;
		}

		IEntity* pMaster = getEntityFromHandle(m_hMaster);
		if(!pMaster) return;

		IFightSystem* pMasterFightSys = static_cast<IFightSystem*>(pMaster->querySubsystem(IID_IFightSystem));
		assert(pMasterFightSys);

		pMasterFightSys->sendAllBattleMsg(data);
	}

	HEntity m_hMaster;
};


void GMSystem::pvpFight(const string& strAccount, int iPos, int iWorldID)
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	IFightSystem* pFightSys = static_cast<IFightSystem*>(pEntity->querySubsystem(IID_IFightSystem));
	assert(pFightSys);

	ServerEngine::PKRole targetKey;
	targetKey.strAccount = strAccount;
	targetKey.rolePos = iPos;
	targetKey.worldID = iWorldID;
	
	ServerEngine::FightContext ctx;
	ctx.iFightType = GSProto::en_FightType_Arena;
	ctx.iSceneID = 1;
	
	pFightSys->AsyncPVPFight(GMPVPFight(m_hEntity), targetKey, ctx);
}


void GMSystem::setAllHeroQuality(int iQuality)
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	IHeroSystem* pHeroSys = static_cast<IHeroSystem*>(pEntity->querySubsystem(IID_IHeroSystem));
	assert(pHeroSys);

	vector<HEntity> heroList = pHeroSys->getHeroList();
	for(size_t i = 0; i < heroList.size(); i++)
	{
		IEntity* pHero = getEntityFromHandle(heroList[i]);
		assert(pHero);

		pHero->setProperty(PROP_ENTITY_QUALITY, iQuality);

		IHeroTallentSystem* pHeroTallentSys = static_cast<IHeroTallentSystem*>(pHero->querySubsystem(IID_IHeroTallentSystem));
		assert(pHeroTallentSys);

		pHeroTallentSys->checkAutoUpgrade();
	}
}

void GMSystem::callHero(int iHeroID)
{
	GSProto::CMD_HERO_CALLHERO_CS req;
	req.set_iheorid(iHeroID);

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	IHeroSystem* pHeroSys = static_cast<IHeroSystem*>(pEntity->querySubsystem(IID_IHeroSystem));
	assert(pHeroSys);

	pHeroSys->callHero(iHeroID);
}


void GMSystem::addEquip(int iItemID)
{
	IEntity* pActor = getEntityFromHandle(m_hEntity);
	assert(pActor);

	IEquipBagSystem* pEquipBagSys = static_cast<IEquipBagSystem*>(pActor->querySubsystem(IID_IEquipBagSystem) );
	assert(pEquipBagSys);

	pEquipBagSys->addEquip(iItemID, GSProto::en_Reason_GM, false);
}

void GMSystem::addFightSoul(int iFightSoulID)
{
	IEntity* pActor = getEntityFromHandle(m_hEntity);
	assert(pActor);

	IFightSoulSystem* pFightSoulSys = static_cast<IFightSoulSystem*>(pActor->querySubsystem(IID_IFightSoulSystem));
	assert(pFightSoulSys);

	pFightSoulSys->addFightSoul(iFightSoulID, true);
}


void GMSystem::postMarquee(vector<string>& cmdList)
{
	if(cmdList.size() < 2)
	{
		return;
	}
		
	GSProto::CMD_MARQUEE_SC s2cMsg;
	int id = atoi(cmdList[1].c_str());
	s2cMsg.set_marqueeid(id);
	for(size_t i = 2; i < cmdList.size();++i)
	{
		// 替换；为#
		string strTmp = TC_Common::replace(cmdList[i], ";", "#");
		s2cMsg.add_szstrmessage(strTmp);
	}
	
	IEntity* pActor = getEntityFromHandle(m_hEntity);
	assert(pActor);

	pActor->sendMessage(GSProto::CMD_MARQUEE,s2cMsg);
}

void GMSystem::testRechareLog()
{
	IEntity* pActor = getEntityFromHandle(m_hEntity);
	assert(pActor);

	string strName = pActor->getProperty(PROP_ENTITY_NAME, "");
	string strAccount = pActor->getProperty(PROP_ACTOR_ACCOUNT, "");
	int iWorldID = pActor->getProperty(PROP_ACTOR_WORLD, 0);
	bool bFirstRecharge = pActor->getProperty(PROP_ACTOR_RECHARGED, 0) == 0;
	pActor->setProperty(PROP_ACTOR_RECHARGED, 1);

	STAT_RECHARGE<<10000<<"|"<<(int)bFirstRecharge<<"|"<<strName<<"|"<<strAccount<<"|"<<iWorldID<<endl;
}



void GMSystem::addNormalGift(int iGiftID)
{
	IEntity* pActor = getEntityFromHandle(m_hEntity);
	assert(pActor);

	ILegionSystem* pLegionSys = static_cast<ILegionSystem*>(pActor->querySubsystem(IID_ILegionSystem));
	assert(pLegionSys);

	pLegionSys->addGift(iGiftID);
}


void GMSystem::addCustomGift(int iItemID)
{
	IEntity* pActor = getEntityFromHandle(m_hEntity);
	assert(pActor);

	ILegionSystem* pLegionSys = static_cast<ILegionSystem*>(pActor->querySubsystem(IID_ILegionSystem));
	assert(pLegionSys);

	ServerEngine::CustomGiftData customGift;
	customGift.strTitle = "Test";
	customGift.propAward[PROP_ACTOR_GOLD] = 100;
	customGift.itemAward[iItemID] = 1;
	customGift.iAwardedContibute = 200;
	pLegionSys->addGift(customGift);
}


void GMSystem::sysInfo()
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	IUserStateManager* pUserStateMgr = getComponent<IUserStateManager>(COMPNAME_LoginManager, IID_IUserStateManager);
	assert(pUserStateMgr);

	int iAccountSize = pUserStateMgr->getUserSize();
	int iPlayerCount = pUserStateMgr->getActorSize();
	
	EventArgs args;
	args.context.setInt("accountsize", iAccountSize);
	args.context.setInt("playersize", iPlayerCount);
	args.context.setInt("starttime", s_startTime);

	IScriptEngine* pScriptEngine = getComponent<IScriptEngine>(COMPNAME_ScriptEngine, IID_IScriptEngine);
	assert(pScriptEngine);

	string strMsg = pScriptEngine->runFunction_String("MakeSysInfo", &args, "EventArgs");

	GSProto::Cmd_Sc_Error scMsg;
	scMsg.set_strerrormsg(strMsg);
	pEntity->sendMessage(GSProto::CMD_ERROR, scMsg);
}


void GMSystem::startLegionFight()
{
	ILegionFactory* pLegionFactory = getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory);
	assert(pLegionFactory);

	pLegionFactory->gmStartFight();
}


void GMSystem::setLegionLevel(int iLevel)
{
	ILegionFactory* pLegionFactory = getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory);
	assert(pLegionFactory);

	IEntity* pActor = getEntityFromHandle(m_hEntity);
	assert(pActor);

	string strActorName = pActor->getProperty(PROP_ENTITY_NAME, "");
	ILegion* pLegion = pLegionFactory->getActorLegion(strActorName);
	if(!pLegion) return;

	pLegion->gmSetLevel(iLevel);
}

void GMSystem::setLevel(int iLevel)
{
	IEntity* pActor = getEntityFromHandle(m_hEntity);
	assert(pActor);	

	int iCurLevel = pActor->getProperty(PROP_ENTITY_LEVEL, 0);

	if(iCurLevel != iLevel)
	{
		pActor->setProperty(PROP_ENTITY_LEVEL, iLevel);
		pActor->setProperty(PROP_ENTITY_EXP, 0);
	
		EventArgs args;
		args.context.setInt("entity", m_hEntity);
		args.context.setInt("oldlv", iCurLevel);
		args.context.setInt("newlv", iLevel);
		pActor->getEventServer()->setEvent(EVENT_ENTITY_LEVELUP, args);
	}

	// 通知客户端
	GSProto::Cmd_Sc_ActorLevelUp scMsg;
	scMsg.set_ilevel(iLevel);
	scMsg.set_iexp(0);

	GSProto::SCMessage pkg;
	HelpMakeScMsg(pkg, GSProto::CMD_ACTOR_LEVELUP, scMsg);
	pActor->sendMessage(pkg);

	if(iCurLevel != iLevel)
	{
		EventArgs args;
		args.context.setInt("entity", m_hEntity);
		args.context.setInt("oldlv", iCurLevel);
		args.context.setInt("newlv", iLevel);
		pActor->getEventServer()->setEvent(EVENT_ENTITY_POSTLEVELUP, args);
	}
}


void GMSystem::enableDomain()
{
	IEntity* pActor = getEntityFromHandle(m_hEntity);
	assert(pActor);	

//	IDomainSystem* pDomainSys = static_cast<IDomainSystem*>(pActor->querySubsystem(IID_IDomainSystem));
//	assert(pDomainSys);
	//pDomainSys->enableDomain();
}


void GMSystem::autoGodAnimal()
{
	IEntity* pActor = getEntityFromHandle(m_hEntity);
	assert(pActor);	

	IGodAnimalSystem* pGodAnimalSys = static_cast<IGodAnimalSystem*>(pActor->querySubsystem(IID_IGodAnimalSystem));
	assert(pGodAnimalSys);

	pGodAnimalSys->autoActiveGodAnimal();
}

void GMSystem::addItem(int iItemID, int iItemCount)
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	IItemSystem* pItemSys = static_cast<IItemSystem*>(pEntity->querySubsystem(IID_IItemSystem));
	assert(pItemSys);

	IItemFactory* pItemFactory = getComponent<IItemFactory>(COMPNAME_ItemFactory, IID_IItemFactory);
	assert(pItemFactory);

	if(!pItemFactory->getItemPropset(iItemID) )
	{
		return;
	}

	pItemSys->addItem(iItemID, iItemCount, GSProto::en_Reason_GM);
}

void GMSystem::removeItem(int iItemID, int iItemCount)
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	IItemSystem* pItemSys = static_cast<IItemSystem*>(pEntity->querySubsystem(IID_IItemSystem));
	assert(pItemSys);

	pItemSys->removeItem(iItemID, iItemCount, GSProto::en_Reason_GM);
}


struct NullContionueCb
{
	NullContionueCb(HEntity hEntity):m_hEntity(hEntity){}

	void operator()(taf::Int32 iRet, const ServerEngine::BattleData& data)
	{
		if(iRet != en_FightResult_OK)
		{
			SvrErrLog("Fight Fail|%d", iRet);
			return;
		}
	
		IEntity* pEntity = getEntityFromHandle(m_hEntity);
		if(!pEntity) return;

		IFightSystem* pFightSys = static_cast<IFightSystem*>(pEntity->querySubsystem(IID_IFightSystem));
		assert(pFightSys);

		pFightSys->sendAllBattleMsg(data);
	}


	HEntity m_hEntity;
};



void NullPVECb::operator()(taf::Int32 iRet, const ServerEngine::BattleData& data)
{
	if(iRet != en_FightResult_OK)
	{
		SvrErrLog("Fight Fail|%d", iRet);
		return;
	}

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	if(!pEntity) return;

	IFightSystem* pFightSys = static_cast<IFightSystem*>(pEntity->querySubsystem(IID_IFightSystem));
	assert(pFightSys);

	pFightSys->sendAllBattleMsg(data);
	
	GMSystem* pGMSys = static_cast<GMSystem*>(pEntity->querySubsystem(IID_IGMSystem));
	assert(pGMSys);

	pGMSys->m_randValueList = data.randValueList;
}


void GMSystem::replay(int iActionIndex, int iResultSetIndex, int iTargetPos)
{
	IFightFactory* pFightFactory = getComponent<IFightFactory>(COMPNAME_FightFactory, IID_IFightFactory);
	assert(pFightFactory);

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	ServerEngine::ActorCreateContext roleCreateCtx;
	roleCreateCtx.strAccount = pEntity->getProperty(PROP_ACTOR_ACCOUNT, "");
	pEntity->packSaveData(roleCreateCtx.data);

	m_lastPVEFight.m_fightCtx.useItemActionList.clear();
	
	ServerEngine::FightUseItem tmpUseItem;
	tmpUseItem.iActionIndex = iActionIndex;
	tmpUseItem.iResultsetIndex = iResultSetIndex;
	//tmpUseItem.iTargetPos = iTargetPos;

	if(m_lastPVEFight.m_iMonsterGrpID > 0)
	{
		m_lastPVEFight.m_fightCtx.useItemActionList.push_back(tmpUseItem);
		m_lastPVEFight.m_fightCtx.randValueList = m_randValueList;
		pFightFactory->AsyncContinuePVEFight(NullContionueCb(m_hEntity), roleCreateCtx, m_lastPVEFight.m_iMonsterGrpID, m_lastPVEFight.m_fightCtx);
	}
	else if(m_lastPVEFight.m_monsterGrpList.size() > 0)
	{
		m_lastPVEFight.m_fightCtx.useItemActionList.push_back(tmpUseItem);
		m_lastPVEFight.m_fightCtx.randValueList = m_randValueList;
		pFightFactory->AsyncContinueMultiPVEFight(NullContionueCb(m_hEntity), roleCreateCtx, m_lastPVEFight.m_monsterGrpList, m_lastPVEFight.m_fightCtx);
	}
}

void GMSystem::doPVEFight(int iMonsterGrpID)
{
	IEntity* pActor = getEntityFromHandle(m_hEntity);
	assert(pActor);

	IFightSystem* pFightSys = static_cast<IFightSystem*>(pActor->querySubsystem(IID_IFightSystem));
	assert(pFightSys);

	ServerEngine::FightContext ctx;
	ctx.iFightType = GSProto::en_FightType_Scene;
	vector<int> emptyList;

	m_lastPVEFight = NullPVECb(m_hEntity, iMonsterGrpID, emptyList, ctx);
	
	pFightSys->AsyncPVEFight(m_lastPVEFight,iMonsterGrpID, ctx);
}

void GMSystem::multiFight(const vector<int>& monsterGrpList)
{
	IEntity* pActor = getEntityFromHandle(m_hEntity);
	assert(pActor);

	IFightSystem* pFightSys = static_cast<IFightSystem*>(pActor->querySubsystem(IID_IFightSystem));
	assert(pFightSys);

	ServerEngine::FightContext ctx;
	ctx.iFightType = GSProto::en_FightType_Scene;
	ctx.iSceneID = 1;

	m_lastPVEFight = NullPVECb(m_hEntity, 0, monsterGrpList, ctx);
	
	pFightSys->AsyncMultPVEFight(m_lastPVEFight, monsterGrpList, ctx);
}


void GMSystem::addHero(int iHeroID, int iCount)
{
	IEntity* pActor = getEntityFromHandle(m_hEntity);
	assert(pActor);

	IHeroSystem* pHeroSys = static_cast<IHeroSystem*>(pActor->querySubsystem(IID_IHeroSystem));
	assert(pHeroSys);

	ITable* pHeroTb = getCompomentObjectManager()->findTable(TABLENAME_Hero);
	assert(pHeroTb);

	if(pHeroTb->findRecord(iHeroID) < 0)
	{
		return;
	}

	for(int i = 0; i < iCount; i++)
	{
		pHeroSys->addHero(iHeroID, true, 0);
	}
}

void GMSystem::addHeroWithStep(int iHeroID, int istep)
{
	IEntity* pActor = getEntityFromHandle(m_hEntity);
	assert(pActor);

	IHeroSystem* pHeroSys = static_cast<IHeroSystem*>(pActor->querySubsystem(IID_IHeroSystem));
	assert(pHeroSys);

	ITable* pHeroTb = getCompomentObjectManager()->findTable(TABLENAME_Hero);
	assert(pHeroTb);

	if(pHeroTb->findRecord(iHeroID) < 0)
	{
		return;
	}

	pHeroSys->addHeroWithLevelStep(iHeroID,istep , true, 0);
}

void GMSystem::fightSoulPractice()
{
	IEntity* pActor = getEntityFromHandle(m_hEntity);
	assert(pActor);

	IFightSoulSystem* pFightSoulSys = static_cast<IFightSoulSystem*>(pActor->querySubsystem(IID_IFightSoulSystem));
	assert(pFightSoulSys);

	pFightSoulSys->practice(true);
}

void GMSystem::enableFightSoulAdvance()
{
	IEntity* pActor = getEntityFromHandle(m_hEntity);
	assert(pActor);

	IFightSoulSystem* pFightSoulSys = static_cast<IFightSoulSystem*>(pActor->querySubsystem(IID_IFightSoulSystem));
	assert(pFightSoulSys);

	pFightSoulSys->enableAdvanceMode();
}

void GMSystem::autoCombineFightSoul()
{
	IEntity* pActor = getEntityFromHandle(m_hEntity);
	assert(pActor);

	IFightSoulSystem* pFightSoulSys = static_cast<IFightSoulSystem*>(pActor->querySubsystem(IID_IFightSoulSystem));
	assert(pFightSoulSys);

	pFightSoulSys->autoCombine();
}

void GMSystem::fightSoulExchange(int iID)
{
	IEntity* pActor = getEntityFromHandle(m_hEntity);
	assert(pActor);

	IFightSoulSystem* pFightSoulSys = static_cast<IFightSoulSystem*>(pActor->querySubsystem(IID_IFightSoulSystem));
	assert(pFightSoulSys);

	pFightSoulSys->exchangeFightSoul(iID);
}


extern int LifeAtt2Prop(int iLifeAtt);

void GMSystem::setLifeAtt(int iID, int iValue)
{
	IEntity* pActor = getEntityFromHandle(m_hEntity);
	assert(pActor);	

	int iPropID = LifeAtt2Prop(iID);
	if(iPropID < 0) return;

	pActor->setProperty(iPropID, iValue);
}

void GMSystem::addGodAnimal(int iGodAnimalID)
{
	IEntity* pActor = getEntityFromHandle(m_hEntity);
	assert(pActor);	

	IGodAnimalSystem* pGodAnimalSys = static_cast<IGodAnimalSystem*>(pActor->querySubsystem(IID_IGodAnimalSystem));
	assert(pGodAnimalSys);

	ITable* pGoldAnimal = getCompomentObjectManager()->findTable(TABLENAME_GodAnimal);
	assert(pGoldAnimal);

	if(pGoldAnimal->findRecord(iGodAnimalID) < 0)
	{
		return;
	}

	pGodAnimalSys->addGodAnimal(iGodAnimalID, true);
}


void GMSystem::setHeroLevel(int iLevel)
{
	IEntity* pActor = getEntityFromHandle(m_hEntity);
	assert(pActor);	

	IHeroSystem* pHeroSys = static_cast<IHeroSystem*>(pActor->querySubsystem(IID_IHeroSystem));
	assert(pHeroSys);

	pHeroSys->gmSetAllHeroLevel(iLevel);
}


void GMSystem::setHeroLevelStep(int iLevelStep)
{
	IEntity* pActor = getEntityFromHandle(m_hEntity);
	assert(pActor);	

	IHeroSystem* pHeroSys = static_cast<IHeroSystem*>(pActor->querySubsystem(IID_IHeroSystem));
	assert(pHeroSys);

	pHeroSys->gmSetAllHeroLevelStep(iLevelStep);
}

void GMSystem::sendSysMsg(const string& strMsg)
{
	IEntity* pActor = getEntityFromHandle(m_hEntity);
	assert(pActor);	

	GSProto::CMD_CHANNEL_CHAT_SC scMsg;
	scMsg.set_strsender("");
	scMsg.mutable_senderkey();
	scMsg.set_strmsg(strMsg);
	scMsg.set_ichannel(GSProto::en_ChatChannel_System);

	pActor->sendMessage(GSProto::CMD_CHANNEL_CHAT, scMsg);
}

void GMSystem::startCampBattle(string startTime,string preTime, string endTime)
{
	IEntity* pActor = getEntityFromHandle(m_hEntity);
	assert(pActor);	

	ICampBattleFactory* factory = getComponent<ICampBattleFactory>(COMPNAME_CampBattleFactory,IID_ICampBattleFactory);
	assert(factory);

	factory->startCampBattle(startTime,preTime,endTime);
}

void GMSystem::GMTestDreamLand(int iSectionID, int iSceneId)
{
	IEntity* pActor = getEntityFromHandle(m_hEntity);
	assert(pActor);

	IDreamLandSystem* pDreamLand = static_cast<IDreamLandSystem*>(pActor->querySubsystem(IID_IDreamLandSystem));
	assert(pDreamLand);

	pDreamLand->GMRequsetFight(iSectionID,iSceneId);
}

void GMSystem::GMRequsetFormation(int iSectionID, int iSceneId)
{
	IEntity* pActor = getEntityFromHandle(m_hEntity);
	assert(pActor);

	IDreamLandSystem* pDreamLand = static_cast<IDreamLandSystem*>(pActor->querySubsystem(IID_IDreamLandSystem));
	assert(pDreamLand);

	pDreamLand->GMRequsetFormation(iSectionID,iSceneId);
}

void GMSystem::GMClearDreamLandData()
{
	IEntity* pActor = getEntityFromHandle(m_hEntity);
	assert(pActor);

	IDreamLandSystem* pDreamLand = static_cast<IDreamLandSystem*>(pActor->querySubsystem(IID_IDreamLandSystem));
	assert(pDreamLand);

	pDreamLand->CMClearData();
}

void GMSystem::GMAddDreamLandResetTimes()
{
	IEntity* pActor = getEntityFromHandle(m_hEntity);
	assert(pActor);

	IDreamLandSystem* pDreamLand = static_cast<IDreamLandSystem*>(pActor->querySubsystem(IID_IDreamLandSystem));
	assert(pDreamLand);

	pDreamLand->GMAddDreamLandResetTimes();
}



