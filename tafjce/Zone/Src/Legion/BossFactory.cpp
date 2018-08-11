#include "LegionPch.h"
#include "BossFactory.h"
#include "BossImp.h"
#include "IModifyDelegate.h"
#include "IJZEntityFactory.h"
#include "IMailSystem.h"

#define GLOBAL_WORLDBOSS_DATA		"GLOBAL_WORLDBOSS_DATA"

extern "C" IComponent* createBossFactory(Int32)
{
	return new BossFactory;
}

BossFactory::BossFactory():
	m_bInitFinish(false)
{
}

BossFactory::~BossFactory()
{
	ITimerComponent* pTimeAxis = getComponent<ITimerComponent>(COMPNAME_TimeAxis, IID_ITimerComponent);
	assert(pTimeAxis);

	pTimeAxis->killTimer(m_hCheckBossHandle);
}

bool BossFactory::initlize(const PropertySet& propSet)
{
	ITimerComponent* pTimeAxis = getComponent<ITimerComponent>(COMPNAME_TimeAxis, IID_ITimerComponent);
	assert(pTimeAxis);

	m_hCheckBossHandle = pTimeAxis->setTimer(this, 1, 5000, "WorldBossCheck");
	m_hSaveHandle = pTimeAxis->setTimer(this, 2, 60*1000, "WorldBossSave");

	loadWorldBossCfg();

	return true;
}

bool BossFactory::isInitFinish()
{
	return m_bInitFinish;
}

void BossFactory::loadWorldBossCfg()
{
	IJZMessageLayer* pMsgLayer = getComponent<IJZMessageLayer>(COMPNAME_MessageLayer, IID_IJZMessageLayer);
	assert(pMsgLayer);
	// try
	// {
	// 	string strValue;
	// 	int iRet = pMsgLayer->getGlobalData(GLOBAL_WORLDBOSS_DATA, strValue);
	// 	if(iRet == ServerEngine::en_DataRet_OK)
	// 	{
	// 		if(strValue.size() > 0) ServerEngine::JceToObj(strValue, m_bossLevelInfo);
			
	// 	}
	// 	else if(iRet == ServerEngine::en_DataRet_NotExist)
	// 	{
	// 		// nothing
	// 	}
	// 	else
	// 	{
	// 		assert(false);
	// 	}
	// }
	// catch(...)
	// {
	// 	assert(false);
	// }

	//Load配置
	ITable* pTable = getCompomentObjectManager()->findTable("WorldBoss");
	assert(pTable);

	int iRecordCount = pTable->getRecordCount();
	for(int i = 0; i < iRecordCount; i++)
	{
		WorldBossCfg tmpCfg;
		tmpCfg.iWorldBossID = pTable->getInt(i, "ID");
		tmpCfg.iMonsterGrpID = pTable->getInt(i, "怪物组ID");
		string strTime = pTable->getString(i, "开启时间");
		vector<int> timeList = TC_Common::sepstr<int>(strTime, "#");
		assert(timeList.size() == 2);
		
		tmpCfg.iStartTime = 3600 * timeList[0] + timeList[1] * 60;
		tmpCfg.iKillGift = pTable->getInt(i, "击杀礼包");
		tmpCfg.iKeepSecond = pTable->getInt(i, "持续时间");
		tmpCfg.iScene = pTable->getInt(i, "战斗场景");

		m_bossCfgMap[tmpCfg.iWorldBossID] = tmpCfg;

		if(m_bossLevelInfo.m_worldBossLevelMap.find(tmpCfg.iWorldBossID) == m_bossLevelInfo.m_worldBossLevelMap.end() )
		{
			m_bossLevelInfo.m_worldBossLevelMap[tmpCfg.iWorldBossID] = 1;
		}
	}
	m_bInitFinish = true;
}


void BossFactory::onTimer(int nEventId)
{
	if(1 == nEventId)
	{
		checkBossStart();
		checkBossTimeout();
		broadcastUpdate();
	}
	else if(2 == nEventId)
	{
		saveWorldBossData(false);
	}
}

class SaveWorldBossCb:public ServerEngine::LegionPrxCallback
{
public:

	virtual void callback_setGlobalData(taf::Int32 ret)
	{
		FDLOG("WorldBoss")<<"setGlobalDataRet|"<<ret<<endl;
	}

	virtual void callback_setGlobalData_exception(taf::Int32 ret)
	{
		FDLOG("WorldBoss")<<"setGlobalDataException|"<<ret<<endl;
	}
};


void BossFactory::saveWorldBossData(bool bSync)
{
	IJZMessageLayer* pMsgLayer = getComponent<IJZMessageLayer>(COMPNAME_MessageLayer, IID_IJZMessageLayer);
	assert(pMsgLayer);

	try
	{
		string strData = ServerEngine::JceToStr(m_bossLevelInfo);
		if(bSync)
		{
			pMsgLayer->setGlobalData(GLOBAL_WORLDBOSS_DATA, strData);
		}
		else
		{
			pMsgLayer->AsyncSetGlobalData(new SaveWorldBossCb, GLOBAL_WORLDBOSS_DATA, strData);
		}
	}
	catch(...)
	{
		SvrErrLog("saveWorldBossData Fail");
	}
}


void BossFactory::checkBossTimeout()
{
	Uint32 dwCurSecond = time(0);
	for(map<int, IBoss*>::iterator it = m_enabledBossList.begin(); it != m_enabledBossList.end();)
	{
		IBoss* pTmpBoss = it->second;
		assert(pTmpBoss);

		assert(m_bossCfgMap.find(it->first) != m_bossCfgMap.end() );
		const WorldBossCfg& refWorldBossCfg = m_bossCfgMap[it->first];

		if(dwCurSecond >= (pTmpBoss->getCreateTime() + (Uint32)refWorldBossCfg.iKeepSecond) )
		{
			int iBossID = it->first;
			it++;
			
			onWorldBossEnd(iBossID, pTmpBoss, 0);
		}
		else
		{
			it++;
		}
	}
}

void BossFactory::onWorldBossEnd(int iBossID, IBoss* pBoss, HEntity hKiller)
{
	FDLOG("WorldBoss")<<"BossEnd|"<<iBossID<<endl;

	// 计算BOSS等级变化
	if(0 != hKiller)
	{
		doKillAward(iBossID, hKiller);
		doRankAward(pBoss);

		// 算完奖励后升级吧
		Uint32 dwMin = (time(0) - pBoss->getCreateTime() )/60;
		calcWorldBossGrow(iBossID, dwMin);
	}

	// 通知客户端
	vector<BossDamageRecord> rankList;
	pBoss->getDamageRankList(10,  rankList);
	map<string, int> rankMap;
	for(size_t i = 0; i < rankList.size(); i++)
	{
		rankMap[rankList[i].strName] = (int)i+1;
	}

	IUserStateManager* pUserStateMgr = getComponent<IUserStateManager>(COMPNAME_LoginManager, IID_IUserStateManager);
	assert(pUserStateMgr);
	
	const map<string, WorldBossDamageState>& refDamageMap = m_bossFightStateMap[iBossID];
	for(map<string, WorldBossDamageState>::const_iterator it = refDamageMap.begin(); it != refDamageMap.end(); it++)
	{
		HEntity hTmp = pUserStateMgr->getActorByName(it->first);
		IEntity* pTmp = getEntityFromHandle(hTmp);
		if(!pTmp) continue;

		const WorldBossDamageState& refDamageState = it->second;

		GSProto::CMD_WORLDBOSS_END_SC scMsg;
		scMsg.set_bkill(0 != hKiller);

		if(rankMap.find(it->first) != rankMap.end() )
		{
			scMsg.set_irank(rankMap[it->first]);
		}
		else
		{
			scMsg.set_irank(0);
		}

		scMsg.set_idamage(pBoss->getDamage(it->first));
		scMsg.set_iawardsilver(refDamageState.iSumSilver);
		scMsg.set_iawardhonor(refDamageState.iSumHonor);
		
		pTmp->sendMessage(GSProto::CMD_WORLDBOSS_END, scMsg);
	}

	// 清理数据
	m_enabledBossList.erase(iBossID);
	m_bossFightStateMap.erase(iBossID);
	delete pBoss;
}

void BossFactory::doKillAward(int iBossID, HEntity hKiller)
{
	assert(m_bossCfgMap.find(iBossID) != m_bossCfgMap.end() );

	IEntity* pKiller = getEntityFromHandle(hKiller);
	if(!pKiller) return;

	//ILegionSystem* pLegionSys = static_cast<ILegionSystem*>(pKiller->querySubsystem(IID_ILegionSystem));
	//assert(pLegionSys);

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	//int iBossLevel = m_bossLevelInfo.m_worldBossLevelMap[iBossID];
	
	string strTitle = pGlobalCfg->getString("世界BOSS击杀礼包标题", "WorldBossKillAward");

	string strContent = pGlobalCfg->getString("世界BOSS击杀正文", "Kill Boss");
	string strSender = pGlobalCfg->getString("世界BOSS奖励发送者", "System");

	IMailSystem* pMailSys = static_cast<IMailSystem*>(pKiller->querySubsystem(IID_IMailSystem) );
	assert(pMailSys);
	
	IDropFactory* pDropFactory = getComponent<IDropFactory>(COMPNAME_DropFactory, IID_IDropFactory);
	assert(pDropFactory);
	
	int iDropId = pGlobalCfg->getInt("世界BOSS击杀奖励ID", 2000);
	GSProto::FightAwardResult awardResult;
	pDropFactory->calcDrop(iDropId, awardResult);
	
	pMailSys->addMail(strSender, strTitle, strContent, awardResult, "WorldBossKill");
	
	//PLAYERLOG(pKiller)<<"KillWorldBoss|"<<iBossID<<"|"<<dSilver<<endl;
	
	broadcastKill(hKiller, iBossID, awardResult);
}

void BossFactory::broadcastKill(HEntity hKiller, int iBossID, const GSProto::FightAwardResult& resoult)
{
	GSProto::CMD_MARQUEE_SC scMsg;
	scMsg.set_marqueeid(GSProto::en_marQueue_WorldBossKill);

	IEntity* pKiller = getEntityFromHandle(hKiller);
	assert(pKiller);

	string strKillerName = pKiller->getProperty(PROP_ENTITY_NAME, "");
	scMsg.add_szstrmessage(strKillerName);

	assert(m_enabledBossList.find(iBossID) != m_enabledBossList.end() );
	IBoss* pBoss = m_enabledBossList[iBossID];
	assert(pBoss);

	int iGold = 0;
	int iSilver = 0;
	for(int i = 0;  i < resoult.szawardproplist_size(); ++i)
	{
		const GSProto::PropItem&  propItem = resoult.szawardproplist(i);
		if(propItem.ilifeattid() == GSProto::en_LifeAtt_Gold)
		{
			iGold	 = propItem.ivalue();
		}
		else if(propItem.ilifeattid() == GSProto::en_LifeAtt_Silver)
		{
			iSilver = propItem.ivalue();
		}
	}

	int iMonsterID = pBoss->getVisibleMonsterID();
	
	scMsg.add_szstrmessage(TC_Common::tostr(iMonsterID) );
	scMsg.add_szstrmessage(TC_Common::tostr(iGold) );
	scMsg.add_szstrmessage(TC_Common::tostr(iSilver) );

	IJZMessageLayer* pMsgLayer = getComponent<IJZMessageLayer>(COMPNAME_MessageLayer, IID_IJZMessageLayer);
	assert(pMsgLayer);

	GSProto::SCMessage pkg;
	HelpMakeScMsg(pkg, GSProto::CMD_MARQUEE, scMsg);
	
	pMsgLayer->broadcastMsg(pkg);
}


struct AddBossGift;
void BossFactory::doRankAward(IBoss* pBoss)
{
	vector<BossDamageRecord> rankList;
	pBoss->getDamageRankList(10, rankList);
	
	IModifyDelegate* pModifyDelegate = getComponent<IModifyDelegate>(COMPNAME_ModifyDelegate, IID_IModifyDelegate);
	assert(pModifyDelegate);

	ITable* pRankAwardTb = getCompomentObjectManager()->findTable("WorldBossRankAward");
	assert(pRankAwardTb);
	
	for(size_t i = 0; i < rankList.size(); i++)
	{
		int iRank = (int)i+1;
		int iRecord = pRankAwardTb->findRecord(iRank);
		assert(iRecord >= 0);
		
		int iDropId = pRankAwardTb->getInt(iRecord, "掉落ID");
		pModifyDelegate->submitDelegateTask(rankList[i].roleKey, AddBossGift(rankList[i].roleKey, rankList[i].strName, iDropId, iRank, false) );
	}
}

void BossFactory::calcWorldBossGrow(int iBossID, Uint32 dwCostMin)
{
	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);
	
	string strLegonBossGrp = pGlobalCfg->getString("世界BOSS成长规则", "5#2#10#1");
	vector<int> legionBossKillGrp = TC_Common::sepstr<int>(strLegonBossGrp, "#");
	assert( (legionBossKillGrp.size() % 2) == 0);

	int iGrowLv = 0;
	for(size_t  i =0; i < legionBossKillGrp.size()/2; i++)
	{
		if((int)dwCostMin < legionBossKillGrp[i*2])
		{
			iGrowLv = legionBossKillGrp[i*2 + 1];
			break;
		}
	}

	m_bossLevelInfo.m_worldBossLevelMap[iBossID] += iGrowLv;
	FDLOG("WorldBoss")<<"BossLevelGrow|"<<iBossID<<"|"<<iGrowLv<<"|"<<m_bossLevelInfo.m_worldBossLevelMap[iBossID]<<endl;
}


void BossFactory::cheerFightBoss(HEntity hActor, int iBossID)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	assert(pActor);

	WorldBossDamageState* pBossFightState = getWorldBossState(hActor, iBossID);
	if(!pBossFightState) return;

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	int iCheerLimit = pGlobalCfg->getInt("世界BOSS鼓舞次数限制", 10);
	if(pBossFightState->iCheerCount >= iCheerLimit)
	{
		pActor->sendErrorCode(ERROR_WORLDBOSS_CHEERLIMIT);
		return;
	}

	// 验证消耗
	int iWorldBossCheerCost = pGlobalCfg->getInt("世界BOSS鼓舞消耗", 20);
	int iTkGold = pActor->getProperty(PROP_ACTOR_GOLD, 0);
	if(iTkGold < iWorldBossCheerCost)
	{
		pActor->sendErrorCode(ERROR_NEED_GOLD);
		return;
	}

	// 消耗
	pActor->changeProperty(PROP_ACTOR_GOLD, 0-iWorldBossCheerCost, GSProto::en_Reason_WorldBossCheerComsume);
	pBossFightState->iCheerCount++;

	// 通知客户端
	GSProto::CMD_CHEER_WORLDBOSS_SC scMsg;
	scMsg.set_icheercount(pBossFightState->iCheerCount);

	pActor->sendMessage(GSProto::CMD_CHEER_WORLDBOSS, scMsg);
}


void BossFactory::onReqCheerWorldBoss(HEntity hActor)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

	if(m_enabledBossList.size() == 0) return;

	int iBossID = m_enabledBossList.begin()->first;

	cheerFightBoss(hActor, iBossID);
}



struct WorldBossFightCb
{
	WorldBossFightCb(HEntity hActor, int iBossID):m_hActor(hActor), m_iBossID(iBossID){}

	void operator()(taf::Int32 iRet, const ServerEngine::BattleData& battleData, int iDamage)
	{
		BossFactory* pBossFactory = static_cast<BossFactory*>(getComponent<IBossFactory>(COMPNAME_BossFactory, IID_IBossFactory) );
		if(!pBossFactory) return;

		if(iRet != en_FightResult_OK)
		{
			return;
		}

		pBossFactory->processBossFight(m_hActor, m_iBossID, battleData, iDamage);
	}

private:

	HEntity m_hActor;
	int m_iBossID;
};


void BossFactory::processBossFight(HEntity hActor, int iBossID, const ServerEngine::BattleData& battleData, int iDamage)
{	
	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

	//关闭上浮
	CloseAttCommUP close(hActor);

	WorldBossDamageState* pBossFightState = getWorldBossState(hActor, iBossID);
	if(!pBossFightState) return;

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	int iSilverParam = pGlobalCfg->getInt("世界BOSS伤害银两比", 1000);
	int iHonorParam = pGlobalCfg->getInt("世界BOSS伤害荣誉比", 100000);

	int iHonorLimit = pGlobalCfg->getInt("世界Boos单次获取荣誉上限", 100);
	int iSilverLimitParam = pGlobalCfg->getInt("世界Boss单次获取的银两上限系数",1000);
	int iActorLevel = pActor->getProperty(PROP_ENTITY_LEVEL,1);
	int iSilverLimit = iActorLevel * iSilverLimitParam;
	
	int iGetSilver = std::max(1, iDamage/iSilverParam);
	int iGetHonor = std::max(1, iDamage/iHonorParam);

	iGetSilver = std::min(iSilverLimit, iGetSilver );
	iGetHonor = std::min(iHonorLimit, iGetHonor);

	pActor->changeProperty(PROP_ACTOR_SILVER, iGetSilver, GSProto::en_Reason_WorldBossDamageCreate);
	pActor->changeProperty(PROP_ENTITY_HONOR, iGetHonor, GSProto::en_Reason_WorldBossDamageCreate);

	pBossFightState->iSumHonor += iGetHonor;
	pBossFightState->iSumSilver += iGetSilver;

	PLAYERLOG(pActor)<<"WorldBossDamage|"<<iDamage<<"|"<<iGetSilver<<"|"<<iGetHonor<<endl;

	// 发送战报
	IFightSystem* pFightSys = static_cast<IFightSystem*>(pActor->querySubsystem(IID_IFightSystem));
	assert(pFightSys);

	// 发送战报
	IDropFactory* pDropFactory = getComponent<IDropFactory>(COMPNAME_DropFactory, IID_IDropFactory);
	assert(pDropFactory);
	
	GSProto::Cmd_Sc_CommFightResult scCommFightResult;

	int iScenID = m_bossCfgMap[iBossID].iScene;
	scCommFightResult.set_isceneid(iScenID);
	scCommFightResult.set_iissuccess(battleData.bAttackerWin);
	scCommFightResult.set_istar(battleData.iStar);

	GSProto::FightAwardResult* pScAward = scCommFightResult.mutable_awardresult();
	pDropFactory->addPropToResult(*pScAward, GSProto::en_LifeAtt_Silver, iGetSilver);
	pDropFactory->addPropToResult(*pScAward, GSProto::en_LifeAtt_Honor, iGetHonor);

	PLAYERLOG(pActor)<<"FightWorldBoss|"<<iBossID<<"|"<<iGetSilver<<"|"<<iGetHonor<<"|"<<(int)battleData.bAttackerWin<<endl;
	GSProto::SCMessage resultMsg;
	HelpMakeScMsg(resultMsg, GSProto::CMD_COMM_FIGHTRESULT, scCommFightResult);
	pFightSys->sendAllBattleMsg(battleData, resultMsg);

	// 如果已经结束
	if(battleData.bAttackerWin)
	{
		IBoss* pBoss = m_enabledBossList[iBossID];
		onWorldBossEnd(iBossID, pBoss, hActor);
		return;
	}
	else
	{
		pBossFightState->dwLastDeadTime = time(0);
	}
	//任务系统
	{
		EventArgs args;
		args.context.setInt("times",1);
		args.context.setInt("entity",hActor);
		pActor->getEventServer()->setEvent(EVENT_ENTITY_TASKFINISH_WORLDBOSS, args);
	}
}

void BossFactory::relivePlayer(HEntity hActor, int iBossID)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

	WorldBossDamageState* pWorldBossDamageState = getWorldBossState(hActor, iBossID);
	if(!pWorldBossDamageState) return;

	if(getReliveCD(hActor, iBossID) == 0)
	{
		pActor->sendErrorCode(ERROR_WORLDBOSS_ALIVE);
		return;
	}

	// 判断世界BOSS复活消耗
	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	int iWorldBossReliveCost = pGlobalCfg->getInt("世界BOSS复活元宝", 10);
	int iTkGold = pActor->getProperty(PROP_ACTOR_GOLD, 0);
	if(iTkGold < iWorldBossReliveCost)
	{
		pActor->sendErrorCode(ERROR_NEED_GOLD);
		return;
	}

	// 扣除
	pActor->changeProperty(PROP_ACTOR_GOLD,0-iWorldBossReliveCost, GSProto::en_Reason_WorldBossReliveConsume);

	pWorldBossDamageState->dwLastDeadTime = 0;

	// 通知客户端
	pActor->sendMessage(GSProto::CMD_WORLDBOSS_RELIVE);
}

void BossFactory::onReqRelive(HEntity hActor)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

	if(m_enabledBossList.size() == 0) return;

	int iBossID = m_enabledBossList.begin()->first;
	relivePlayer(hActor, iBossID);
}


int BossFactory::getReliveCD(HEntity hActor, int iBossID)
{
	WorldBossDamageState* pWorldBossDamageState = getWorldBossState(hActor, iBossID);
	if(!pWorldBossDamageState) return 0;

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	int iCD = pGlobalCfg->getInt("世界BOSS复活CD", 60);
	if(pWorldBossDamageState->dwLastDeadTime + iCD > time(0) )
	{
		return pWorldBossDamageState->dwLastDeadTime + (Uint32)iCD - time(0);
	}

	return 0;
}


void BossFactory::fightWorldBoss(HEntity hActor, int iBossID)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

	if(m_enabledBossList.find(iBossID) == m_enabledBossList.end() )
	{
		return;
	}

	if(getReliveCD(hActor, iBossID) > 0)
	{
		pActor->sendErrorCode(ERROR_WORLDBOSS_CD);
		return;
	}

	IBoss* pBoss = m_enabledBossList[iBossID];
	if(!pBoss) return;

	WorldBossDamageState* pBossFightState = getWorldBossState(hActor, iBossID);
	if(!pBossFightState) return;

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	int iAddAttPercent = pGlobalCfg->getInt("世界BOSS鼓舞增加攻击万份比", 2000);
	
	ServerEngine::AttackBossCtx attackerCtx;
	attackerCtx.iAttAddPercent = pBossFightState->iCheerCount * iAddAttPercent;

	pBoss->AsynFightBoss(hActor, attackerCtx, WorldBossFightCb(hActor, iBossID) );
}

void BossFactory::onReqFightWorldBoss(HEntity hActor)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

	if(m_enabledBossList.size() == 0) return;

	int iBossID = m_enabledBossList.begin()->first;
	fightWorldBoss(hActor, iBossID);
}


WorldBossDamageState* BossFactory::getWorldBossState(HEntity hActor, int iBossID)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return NULL;

	if(m_bossFightStateMap.find(iBossID) == m_bossFightStateMap.end() )
	{
		return NULL;
	}

	string strtActorName = pActor->getProperty(PROP_ENTITY_NAME, "");
	WorldBossDamageState& bossState = m_bossFightStateMap[iBossID][strtActorName];

	bossState.roleKey.strAccount = pActor->getProperty(PROP_ACTOR_ACCOUNT, "");
	bossState.roleKey.rolePos = 0;
	bossState.roleKey.worldID = pActor->getProperty(PROP_ACTOR_WORLD, 0);

	return &bossState;
}

void BossFactory::checkBossStart()
{
	IZoneTime* pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);

	Uint32 dwBeginSecond = pZoneTime->GetDayBeginSecond(pZoneTime->GetCurSecond() );
	Uint32 dwSecondInDay = pZoneTime->GetCurSecond() - dwBeginSecond;
	Uint32 dwCurDayNO = pZoneTime->GetChineseDay(pZoneTime->GetCurSecond() );

	for(map<int, WorldBossCfg>::iterator it = m_bossCfgMap.begin(); it != m_bossCfgMap.end(); it++)
	{
		if(m_enabledBossList.find(it->first) != m_enabledBossList.end() )
		{
			continue;
		}

		if(m_bossLevelInfo.m_bossLastFightDayMap.find(it->first) != m_bossLevelInfo.m_bossLastFightDayMap.end() )
		{
			Uint32 dwLastChineseDay = m_bossLevelInfo.m_bossLastFightDayMap[it->first];
			if(dwCurDayNO == dwLastChineseDay)
			{
				continue;
			}
		}
	
		const WorldBossCfg& tmpWorldBossCfg = it->second;
		if( (dwSecondInDay >= (Uint32)tmpWorldBossCfg.iStartTime) && (dwSecondInDay < ((Uint32)tmpWorldBossCfg.iStartTime + tmpWorldBossCfg.iKeepSecond) ) )
		{
			IBoss* pTmpBoss = createWorldBoss(tmpWorldBossCfg);
			assert(pTmpBoss);

			m_enabledBossList[it->first] = pTmpBoss;
			m_bossLevelInfo.m_bossLastFightDayMap[it->first] = dwCurDayNO;
			m_bossFightStateMap[it->first] = map<string, WorldBossDamageState>();
		}
	}
}


IBoss* BossFactory::createWorldBoss(const WorldBossCfg& worldBossCfg)
{
	int iLevel = m_bossLevelInfo.m_worldBossLevelMap[worldBossCfg.iWorldBossID];

	ITable* pWorldBossHPTb = getCompomentObjectManager()->findTable("WorldBossHP");
	assert(pWorldBossHPTb);

	int iRecord = pWorldBossHPTb->findRecord(iLevel);
	assert(iRecord >= 0);

	int iMaxHP = pWorldBossHPTb->getInt(iRecord, "MAXHP");
	int iDef = pWorldBossHPTb->getInt(iRecord, "防御");
	
	ServerEngine::CreateBossCtx bossCtx;
	bossCtx.iFixMaxHP = iMaxHP;
	bossCtx.iHP = bossCtx.iFixMaxHP;
	bossCtx.iDef = iDef;
	bossCtx.iFixLevel = iLevel;

	FDLOG("WorldBoss")<<"CreateBoss|"<<worldBossCfg.iWorldBossID<<"|"<<iLevel<<"|"<<iMaxHP<<endl;

	return createBoss(worldBossCfg.iMonsterGrpID, bossCtx);
}


IBoss* BossFactory::createBoss(int iMonsterGrp, const ServerEngine::CreateBossCtx& bossCtx)
{
	IBoss* pBoss = new BossImp(iMonsterGrp, bossCtx);
	m_bossList.insert(pBoss);

	return pBoss;
}

void BossFactory::delBoss(IBoss* pBoss)
{
	m_bossList.erase(pBoss);
	delete pBoss;
}


void AddBossGift::operator()(int iRet, HEntity hEntity, ServerEngine::PIRole& roleInfo)
{
	if(ServerEngine::en_RoleRet_OK != iRet)
	{
		SvrErrLog("AddGiftError|%s|%d|%s", m_roleKey.strAccount.c_str(), m_iGiftID, m_strName.c_str() );
		return;
	}

	IDropFactory* pDropFactory = getComponent<IDropFactory>(COMPNAME_DropFactory, IID_IDropFactory);
	assert(pDropFactory);

	GSProto::FightAwardResult awardResult;
	pDropFactory->calcDrop(m_iGiftID, awardResult);

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	string strTitle = pGlobalCfg->getString("世界BOSS排名奖励标题", "WorldBossRank");
	string strSender = pGlobalCfg->getString("世界BOSS奖励发送者", "System");
	string strContent = pGlobalCfg->getString("世界BOSS奖励正文", "%rank");

	if(m_bLegionBoss)
	{
		strTitle = pGlobalCfg->getString("军团BOSS排名奖励标题", "WorldBossRank");
		strSender = pGlobalCfg->getString("军团奖励发放人", "System");
		strContent = pGlobalCfg->getString("军团BOSS奖励正文", "%rank");
	}
	
	strContent = TC_Common::replace(strContent, "%rank", TC_Common::tostr(m_iRank) );

	if(0 != hEntity)
	{
		IEntity* pEntity = getEntityFromHandle(hEntity);
		assert(pEntity);

		IMailSystem* pMailSys = static_cast<IMailSystem*>(pEntity->querySubsystem(IID_IMailSystem));
		assert(pMailSys);

		pMailSys->addMail(strSender, strTitle, strContent, awardResult, "WorldBossRank");
	}
	else
	{
		ServerEngine::RoleSaveData tmpData;
		ServerEngine::JceToObj(roleInfo.roleData, tmpData);

		ServerEngine::MailSystemData mailSysData;
		if(tmpData.subsystemData[IID_IMailSystem].size() > 0)
		{
			ServerEngine::JceToObj(tmpData.subsystemData[IID_IMailSystem], mailSysData);
		}

		ServerEngine::MailData tmpMailData;
		uuid_t itemuuid;
		uuid_generate(itemuuid);
		char szUUIDString[1024] = {0};
		uuid_unparse_upper(itemuuid, szUUIDString);
		tmpMailData.strUUID = szUUIDString;

		tmpMailData.strTitle = strTitle;
		tmpMailData.strSender = strSender;
		tmpMailData.strContent = strContent;
		tmpMailData.iState = GSProto::en_MailState_UnRead;
		tmpMailData.dwSenderTime = time(0);
		tmpMailData.iKeepDay = pGlobalCfg->getInt("邮件保留天数", 15);
		tmpMailData.iMailType = GSProto::enMailType_Com;
		awardResult.SerializeToString(&tmpMailData.strAwardInfo);
		
		mailSysData.mailDataList.push_back(tmpMailData);
		
		tmpData.subsystemData[IID_IMailSystem] = ServerEngine::JceToStr(mailSysData);
		
		roleInfo.roleData = ServerEngine::JceToStr(tmpData);
	}
}


void BossFactory::sendBossInfo(HEntity hActor)
{
	GSProto::CMD_QUERY_WORLDBOSS_SC scMsg;

	int iBossMonsterID = 0;
	int iBossLevel = 0;

	// 由于世界BOSS是定时开启的，存在5S误差，如果这个时候客户端请求，容易导致计算到第二天
	// 所以，这里强制检测下
	if(m_enabledBossList.size() == 0)
	{
		checkBossStart();
	}
	
	if(m_enabledBossList.size() > 0)
	{
		scMsg.set_bstarted(true);
		GSProto::WorldBossFightInfo* pBossFightInfo = scMsg.mutable_fightinfo();
		fillBossFightInfo(hActor, pBossFightInfo, iBossMonsterID, iBossLevel);
	}
	else
	{
		scMsg.set_bstarted(false);
		GSProto::WorldBossPreStart* pPreBossFightInfo = scMsg.mutable_prestartinfo();
		fillPreBossInfo(pPreBossFightInfo, iBossMonsterID, iBossLevel);
	}
	scMsg.set_ishowmonsterid(iBossMonsterID);
	scMsg.set_ibosslevel(iBossLevel);

	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

	pActor->sendMessage(GSProto::CMD_QUERY_WORLDBOSS, scMsg);
}


void BossFactory::fillPreBossInfo(GSProto::WorldBossPreStart* pScPreStartInfo, int& iBossMonsterID, int& iOutLevel)
{
	if(m_enabledBossList.size() > 0)
	{
		return;
	}

	// 找到下一场
	IZoneTime* pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);

	Uint32 dwSecondInDay = pZoneTime->GetCurSecond() - pZoneTime->GetDayBeginSecond(pZoneTime->GetCurSecond() );
	const WorldBossCfg* pNextWorldBoss = NULL;
	for(map<int, WorldBossCfg>::iterator it = m_bossCfgMap.begin(); it != m_bossCfgMap.end(); it++)
	{
		const WorldBossCfg& tmpWorldBossCfg = it->second;
		if(dwSecondInDay < (Uint32)tmpWorldBossCfg.iStartTime)
		{
			pNextWorldBoss = &tmpWorldBossCfg;
			break;
		}
	}

	// 第二天的那场
	if(!pNextWorldBoss)
	{
		pNextWorldBoss = &(m_bossCfgMap.begin()->second);
		pScPreStartInfo->set_iwaitsecond(pNextWorldBoss->iStartTime - (int)dwSecondInDay + 3600*24);
	}
	else
	{
		pScPreStartInfo->set_iwaitsecond(pNextWorldBoss->iStartTime - (int)dwSecondInDay);
	}

	if(!pNextWorldBoss) return;

	// 填充信息
	

	int iLevel = m_bossLevelInfo.m_worldBossLevelMap[pNextWorldBoss->iWorldBossID];
	ITable* pWorldBossHPTb = getCompomentObjectManager()->findTable("WorldBossHP");
	assert(pWorldBossHPTb);

	int iRecord = pWorldBossHPTb->findRecord(iLevel);
	assert(iRecord >= 0);

	int iMaxHP = pWorldBossHPTb->getInt(iRecord, "MAXHP");
	pScPreStartInfo->set_ibossmaxhp(iMaxHP);
	pScPreStartInfo->set_ibosshp(iMaxHP);

	IJZEntityFactory* pJZEntityFactory = getComponent<IJZEntityFactory>(COMPNAME_EntityFactory, IID_IJZEntityFactory);
	assert(pJZEntityFactory);

	iBossMonsterID = pJZEntityFactory->getVisibleMonsterID(pNextWorldBoss->iMonsterGrpID);
	iOutLevel = iLevel;
}

void BossFactory::fillBossFightInfo(HEntity hActor, GSProto::WorldBossFightInfo* pScFightInfo, int& iBossMonsterID, int& iOutLevel)
{
	if(m_enabledBossList.size() == 0)
	{
		return;
	}

	int iBossID = m_enabledBossList.begin()->first;
	IBoss* pBoss = m_enabledBossList.begin()->second;
	assert(pBoss);

	int iUsedTime = (int)time(0) - (int)pBoss->getCreateTime();
	assert(m_bossCfgMap.find(iBossID) != m_bossCfgMap.end() );
	const WorldBossCfg& tmpBossCfg = m_bossCfgMap[iBossID];

	WorldBossDamageState* pBossFightState = getWorldBossState(hActor, iBossID);
	assert(pBossFightState);

	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

	string strActorName = pActor->getProperty(PROP_ENTITY_NAME, "");
	
	pScFightInfo->set_ileftkillsecond(std::max(0, tmpBossCfg.iKeepSecond - iUsedTime) );
	pScFightInfo->set_icheeredcount(pBossFightState->iCheerCount);
	pScFightInfo->set_iselfdamage(pBoss->getDamage(strActorName) );
	pScFightInfo->set_isumsilver(pBossFightState->iSumSilver);
	pScFightInfo->set_isumhonor(pBossFightState->iSumHonor);
	pScFightInfo->set_irelivecd(getReliveCD(hActor, iBossID));
	pScFightInfo->set_ibossmaxhp(pBoss->getBossMaxHP() );
	pScFightInfo->set_ibosshp(pBoss->getBossHP() );

	vector<BossDamageRecord> rankList;
	pBoss->getDamageRankList(10, rankList);
	for(size_t i = 0; i < rankList.size(); i++)
	{
		GSProto::WorldBossRankItem* pNewRankItem = pScFightInfo->mutable_szranklist()->Add();
		pNewRankItem->set_strname(rankList[i].strName);
		pNewRankItem->set_idamagevalue(rankList[i].iDamageValue);
	}

	IJZEntityFactory* pJZEntityFactory = getComponent<IJZEntityFactory>(COMPNAME_EntityFactory, IID_IJZEntityFactory);
	assert(pJZEntityFactory);

	iBossMonsterID = pBoss->getVisibleMonsterID();
	iOutLevel = m_bossLevelInfo.m_worldBossLevelMap[iBossID];
}


void BossFactory::broadcastUpdate()
{
	if(m_enabledBossList.size() == 0)
	{
		return;
	}

	int iBossID = m_enabledBossList.begin()->first;
	IBoss* pBoss = m_enabledBossList.begin()->second;
	assert(pBoss);

	GSProto::CMD_UPDATE_WORLDBOSS_SC scMsg;
	GSProto::WorldBossUpdate* pScUpdate = scMsg.mutable_updateinfo();

	pScUpdate->set_ibossmaxhp(pBoss->getBossMaxHP() );
	pScUpdate->set_ibosshp(pBoss->getBossHP() );

	vector<BossDamageRecord> rankList;
	pBoss->getDamageRankList(10, rankList);

	for(size_t i = 0; i < rankList.size(); i++)
	{
		GSProto::WorldBossRankItem* pNewItem = pScUpdate->mutable_szranklist()->Add();
		pNewItem->set_strname(rankList[i].strName);
		pNewItem->set_idamagevalue(rankList[i].iDamageValue);
	}

	IUserStateManager* pUserStateMgr = getComponent<IUserStateManager>(COMPNAME_LoginManager, IID_IUserStateManager);
	assert(pUserStateMgr);
	
	const map<string, WorldBossDamageState>& refDamageMap = m_bossFightStateMap[iBossID];
	for(map<string, WorldBossDamageState>::const_iterator it = refDamageMap.begin(); it != refDamageMap.end(); it++)
	{
		HEntity hTmp = pUserStateMgr->getActorByName(it->first);
		IEntity* pTmp = getEntityFromHandle(hTmp);
		if(!pTmp) continue;

		pTmp->sendMessage(GSProto::CMD_UPDATE_WORLDBOSS, scMsg);
	}	
}




