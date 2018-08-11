#include "ArenaServerPch.h"
#include "ArenaFactory.h"
#include "IFormationSystem.h"
#include "IModifyDelegate.h"
#include "IJZEntityFactory.h"

#define GLOBAL_ARENA_DATA		"GLOBAL_ARENA_DATA"
#define MAX_ARENA_SIZE		30000
#define ARENA_PAGE_COUNT 4
#define MAX_SHOW_RANKSIZE 50

extern "C" IComponent* createArenaFactory(Int32)
{
	return new ArenaFactory;
}


ArenaFactory::ArenaFactory():
	m_bInitFinish(false)
{
}

ArenaFactory::~ArenaFactory()
{
	ITimerComponent* pTimeAxis = getComponent<ITimerComponent>(COMPNAME_TimeAxis, IID_ITimerComponent);
	assert(pTimeAxis);

	pTimeAxis->killTimer(m_hHandle);
	pTimeAxis->killTimer(m_checkAwardHandle);
	pTimeAxis->killTimer(m_hInitHandle);
	
	pTimeAxis->killTimer(m_hRankFightValue);
}


bool ArenaFactory::initlize(const PropertySet& propSet)
{
	loadArenaAward();

	ITimerComponent* pTimeAxis = getComponent<ITimerComponent>(COMPNAME_TimeAxis, IID_ITimerComponent);
	assert(pTimeAxis);

	m_hHandle = pTimeAxis->setTimer(this, 1, 60*1000, "ArenaGlobalSave");
	m_checkAwardHandle = pTimeAxis->setTimer(this, 2, 10 * 1000, "ArenaCheckAward");
	m_hInitHandle = pTimeAxis->setTimer(this, 3, 1000, "ArenaInit");

	m_hRankFightValue = pTimeAxis->setTimer(this, 4, 300*1000, "RankFightValue");	

	loadRandRuleList();
	
	IGlobalCfg *pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg,IID_IGlobalCfg);
	assert(pGlobal);
	int iInterval1Chance = pGlobal->getInt("战力排名小于玩家100概率",10);
	int iInterval2Chance = pGlobal->getInt("玩家战力排名差小于100概率",80);
	int iInterval3Chance = pGlobal->getInt("战力排名大于100概率",10);
	m_ManorRandChanceVec.push_back(iInterval1Chance);
	m_ManorRandChanceVec.push_back(iInterval2Chance);
	m_ManorRandChanceVec.push_back(iInterval3Chance);
	m_iManorRandTotalChance = iInterval1Chance + iInterval2Chance + iInterval3Chance;
	return true;
}

void ArenaFactory::loadRandRuleList()
{
	ITable *pTable = getCompomentObjectManager()->findTable(TABLENAME_ArenaRefresh);
	assert( pTable);

	int iRecord = pTable->getRecordCount();
	for(int i = 0; i < iRecord; ++i)
	{
		string strActorRankSet = pTable->getString( i, "玩家名次区间");
		string strSet1 = pTable->getString( i, "第一目标区间及人数");
		string strSet2 = pTable->getString( i, "第二目标区间及人数");
		vector<int> actorRankSet = TC_Common::sepstr<int>(strActorRankSet, "#");
		vector<int> rankSet1 = TC_Common::sepstr<int>(strSet1, "#");
		vector<int> rankSet2 = TC_Common::sepstr<int>(strSet2, "#");
		assert(actorRankSet.size() == 2);
		assert(rankSet1.size() == 3);
		assert(rankSet2.size() == 3);
		RandRuleUnit unit;
		unit.iLowRankNum = actorRankSet[0];
		unit.iHightRankNum= actorRankSet[1];
		unit.rankSet1 = rankSet1;
		unit.rankSet2 = rankSet2;
		m_RandRuleList.push_back(unit);
	} 
	
}


int ArenaFactory::getArenaRank(const string& strName)
{
	map<string, int>::iterator it = m_arenaRankMap.find(strName);
	if(it != m_arenaRankMap.end() )
	{
		return it->second;
	}

	return 0;
}

void ArenaFactory::getDomainList(HEntity hActor, vector<ServerEngine::ArenaSavePlayer>& playerList)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

	string strActorName = pActor->getProperty(PROP_ENTITY_NAME, "");
	int iRank = getArenaRank(strActorName);

	if(0 == iRank) iRank = (int)m_globalArenaData.rankList.size();

	int iMinRank = std::max(iRank - 300, 1);
	int iMaxRank = std::min(iRank + 100, (int)m_globalArenaData.rankList.size() );

	int iRange = iMaxRank - iMinRank + 1;
	int iLimitRandCount = 5 * 20;
	set<int> userdList;
	userdList.insert(iRank);

	IRandom* pRandom = getComponent<IRandom>("Random", IID_IMiniAprRandom);
	assert(pRandom);
	
	while( (playerList.size() < 5) && (--iLimitRandCount > 0) )
	{
		int iRandV = pRandom->random() % iRange;
		int iTmpRank = iMinRank + iRandV;

		if(userdList.find(iTmpRank) != userdList.end() )
		{
			continue;
		}

		if(m_globalArenaData.rankList[iTmpRank - 1].bMonster)
		{
			continue;
		}

		userdList.insert(iTmpRank);
		playerList.push_back(m_globalArenaData.rankList[iTmpRank - 1]);
	}
}


bool ArenaFactory::isInitFinish()
{
	return m_bInitFinish;
}

void ArenaFactory::loadArenaData()
{
	try
	{
		IJZMessageLayer* pMsgLayer = getComponent<IJZMessageLayer>(COMPNAME_MessageLayer, IID_IJZMessageLayer);
		assert(pMsgLayer);

		string strGlobalData;
		int iRet = pMsgLayer->getGlobalData(GLOBAL_ARENA_DATA, strGlobalData);
		if(ServerEngine::en_DataRet_OK == iRet)
		{
			ServerEngine::JceToObj(strGlobalData, m_globalArenaData);
				// 建立索引
			if(m_globalArenaData.rankList.size() == 0)
			{
				initArenaWithMonster(true);
				// 建立索引
				for(size_t i = 0; i < m_globalArenaData.rankList.size(); i++)
				{
					m_arenaRankMap[m_globalArenaData.rankList[i].strName] = (int)i+1;
				}
			}
			else
			{
				for(size_t i = 0; i < m_globalArenaData.rankList.size(); i++)
				{
					m_arenaRankMap[m_globalArenaData.rankList[i].strName] = (int)i+1;
				}
				initArenaWithMonster(false);
			}
			
			
		}
		else if(ServerEngine::en_DataRet_NotExist == iRet)
		{
			initArenaWithMonster(true);
				// 建立索引
			for(size_t i = 0; i < m_globalArenaData.rankList.size(); i++)
			{
				m_arenaRankMap[m_globalArenaData.rankList[i].strName] = (int)i+1;
			}
		}
		else
		{
			assert(false);
		}

	
	}
	catch(...)
	{
		assert(false);
	}

	ITimerComponent* pTimeAxis = getComponent<ITimerComponent>(COMPNAME_TimeAxis, IID_ITimerComponent);
	assert(pTimeAxis);
	
	pTimeAxis->killTimer(m_hInitHandle);

	m_bInitFinish = true;
}

struct ArenaPVECallback
{
	ArenaPVECallback(ArenaFactory* pArenaFactory, HEntity hEntity, int iSelfRank, int iTargetRank, DelegatePVEFight cb):
		m_pArenaFactory(pArenaFactory), m_hEntity(hEntity), m_iSelfRank(iSelfRank), m_iTargetRank(iTargetRank), m_cb(cb){}

	void operator()(taf::Int32 iRet, const ServerEngine::BattleData& battleData)
	{
		m_pArenaFactory->processArenaFight(m_hEntity, iRet, m_iSelfRank, m_iTargetRank, battleData, m_cb);
	}

private:

	ArenaFactory* m_pArenaFactory;
	HEntity m_hEntity;
	int m_iSelfRank;
	int m_iTargetRank;
	DelegatePVEFight m_cb;
};

struct ArenaPVPCallback
{
	ArenaPVPCallback(ArenaFactory* pArenaFactory, HEntity hEntity, int iSelfRank, int iTargetRank, DelegatePVEFight cb):
		m_pArenaFactory(pArenaFactory), m_hEntity(hEntity), m_iSelfRank(iSelfRank), m_iTargetRank(iTargetRank), m_cb(cb){}

	void operator()(taf::Int32 iRet, ServerEngine::BattleData& battleData)
	{
		m_pArenaFactory->processArenaFight(m_hEntity, iRet, m_iSelfRank, m_iTargetRank, battleData, m_cb);
	}

private:
	ArenaFactory* m_pArenaFactory;
	HEntity m_hEntity;
	int m_iSelfRank;
	int m_iTargetRank;
	DelegatePVEFight m_cb;
};


class ArenaSaveFightCb:public ServerEngine::FightDataPrxCallback
{
public:

	virtual void callback_saveFightData(taf::Int32 ret)
	{
		FDLOG("ArenaFightSave")<<"Ret|"<<ret<<endl;
	}
    virtual void callback_saveFightData_exception(taf::Int32 ret)
    {
    	FDLOG("ArenaFightSave")<<"exception|"<<ret<<endl;
	}
};

struct ArenaLogAddCb
{
	ArenaLogAddCb(const ServerEngine::PKRole& roleKey, const GSProto::ArenaLogItem& log):m_roleKey(roleKey), m_arenaLog(log){}

	void operator()(int iRet, HEntity hEntity, ServerEngine::PIRole& roleInfo)
	{
		if(iRet != ServerEngine::en_RoleRet_OK)
		{
			SvrErrLog("ArenaLogAddCb Fail|%s|%d", m_roleKey.strAccount.c_str(), m_roleKey.rolePos);
			return;
		}

		if(0 != hEntity)
		{
			IEntity* pEntity = getEntityFromHandle(hEntity);
			if(!pEntity)
			{
				SvrErrLog("ArenaLogAddCb Fail|Entity offLine|%s|%d", m_roleKey.strAccount.c_str(), m_roleKey.rolePos);
				return;
			}

			IArenaSystem* pArenaSys = static_cast<IArenaSystem*>(pEntity->querySubsystem(IID_IArenaSystem));
			assert(pArenaSys);

			pArenaSys->addArenaLog(m_arenaLog,false);
		}
		else
		{
			ServerEngine::RoleSaveData tmpData;
			ServerEngine::JceToObj(roleInfo.roleData, tmpData);
			ServerEngine::ArenaSystemSaveData arenaSaveData;

			string strArenaData = tmpData.subsystemData[IID_IArenaSystem];
			if(strArenaData.size() > 0)
			{
				ServerEngine::JceToObj(strArenaData, arenaSaveData);
			}

			IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
			assert(pGlobalCfg);

			int iLogCount = pGlobalCfg->getInt("竞技场日志条数", 20);
			string strLog;
			m_arenaLog.SerializeToString(&strLog);
			arenaSaveData.m_arenaLogList.insert(arenaSaveData.m_arenaLogList.begin(), strLog);
			
			if(arenaSaveData.m_arenaLogList.size() > (size_t)iLogCount)
			{
				arenaSaveData.m_arenaLogList.resize((size_t)iLogCount);
			}
			arenaSaveData.bHaveNewLog = true;
			tmpData.subsystemData[IID_IArenaSystem] = ServerEngine::JceToStr(arenaSaveData);
			roleInfo.roleData = ServerEngine::JceToStr(tmpData);
		}
	}

private:

	ServerEngine::PKRole m_roleKey;
	GSProto::ArenaLogItem m_arenaLog;
};

void ArenaFactory::processArenaFight(HEntity hActor, int iRet, int iSelfRank, int iTargetRank, const ServerEngine::BattleData& battleData, DelegatePVEFight cb)
{
	m_busyTarget.erase(iSelfRank);
	m_busyTarget.erase(iTargetRank);

	if(iRet != en_FightResult_OK)
	{	
		cb(iRet, (ServerEngine::BattleData&)battleData);
		return;	
	}

	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor)
	{
		SvrErrLog("processArenaFight|Actor Not Exist");
		return;
	}

	// 记录战报
	IMessageLayer* pMsgLayer = getComponent<IMessageLayer>(COMPNAME_MessageLayer, IID_IMessageLayer);
	assert(pMsgLayer);

	pMsgLayer->AsyncSaveFightRecord(battleData, new ArenaSaveFightCb);

	IFightFactory* pFightFactory = getComponent<IFightFactory>(COMPNAME_FightFactory, IID_IFightFactory);
	assert(pFightFactory);
	
	string strUUID = pFightFactory->makeFightStringKey(battleData.FightKey);

	GSProto::ArenaLogItem newAttLog;
	newAttLog.set_strattname(pActor->getProperty(PROP_ENTITY_NAME, "") );
	newAttLog.set_strtarget(m_globalArenaData.rankList[iTargetRank - 1].strName);
	newAttLog.set_battackerwin(battleData.bAttackerWin);
	newAttLog.set_struuid(strUUID);

	GSProto::ArenaLogItem newTargetLog = newAttLog;
	ServerEngine::PKRole targetKey = m_globalArenaData.rankList[iTargetRank - 1].roleKey;
	
	// 如果胜利，调整排名
	if(battleData.bAttackerWin)
	{
		if(iSelfRank > 0)
		{
			// 前4的玩家，只有挑战名次好的猜调整
			if(iTargetRank < iSelfRank)
			{
				std::swap(m_globalArenaData.rankList[iSelfRank - 1], m_globalArenaData.rankList[iTargetRank - 1]);
				// 更新排名索引
				m_arenaRankMap[newAttLog.strattname()] = iTargetRank;
				m_arenaRankMap[newAttLog.strtarget()] = iSelfRank;

				//排名变化日志
				newAttLog.set_irankchgtype(GSProto::en_RankChgType_Up);
				newAttLog.set_irank(iTargetRank);

				newTargetLog.set_irankchgtype(GSProto::en_RankChgType_Down);
				newTargetLog.set_irank(iSelfRank);
			}
			else
			{
				newAttLog.set_irankchgtype(GSProto::en_RankChgType_Keep);
				newTargetLog.set_irankchgtype(GSProto::en_RankChgType_Keep);
			}
		}
		else
		{
			ServerEngine::ArenaSavePlayer selfPlayerInfo;
			fillActorSaveInfo(hActor, selfPlayerInfo);
			m_globalArenaData.rankList[iTargetRank - 1] = selfPlayerInfo;

			m_arenaRankMap[selfPlayerInfo.strName] = iTargetRank;
			m_arenaRankMap.erase(newAttLog.strtarget());

			//排名变化日志
			newAttLog.set_irankchgtype(GSProto::en_RankChgType_Up);
			newAttLog.set_irank(iTargetRank);

			newTargetLog.set_irankchgtype(GSProto::en_RankChgType_Down);
			newTargetLog.set_irank(iSelfRank);
		}

		// 通知爬到第一，广播
		if(1== iTargetRank)
		{
			broadcastTopOne(hActor, newAttLog.strtarget() );	
		}
	}
	else
	{
		newAttLog.set_irankchgtype(GSProto::en_RankChgType_Keep);
		newTargetLog.set_irankchgtype(GSProto::en_RankChgType_Keep);
	}

	// 记录日志
	{
		IArenaSystem* pArenaSys = static_cast<IArenaSystem*>(pActor->querySubsystem(IID_IArenaSystem));
		assert(pArenaSys);
		pArenaSys->addArenaLog(newAttLog,true);

		IModifyDelegate* pModifyDelegate = getComponent<IModifyDelegate>(COMPNAME_ModifyDelegate, IID_IModifyDelegate);
		assert(pModifyDelegate);

		pModifyDelegate->submitDelegateTask(targetKey, ArenaLogAddCb(targetKey, newTargetLog) );
	}

	// 通知(奖励在这个里面处理)
	cb(iRet, (ServerEngine::BattleData&)battleData);
}


void ArenaFactory::broadcastTopOne(HEntity hActor, const string& strTargetName)
{
	GSProto::CMD_MARQUEE_SC scMsg;
	scMsg.set_marqueeid(GSProto::en_marQueue_ArenaTopOne);

	IEntity* pActor = getEntityFromHandle(hActor);
	assert(pActor);

	string strActorName = pActor->getProperty(PROP_ENTITY_NAME, "");

	scMsg.add_szstrmessage(strActorName);
	scMsg.add_szstrmessage(strTargetName);

	IJZMessageLayer* pMsgLayer = getComponent<IJZMessageLayer>(COMPNAME_MessageLayer, IID_IJZMessageLayer);
	assert(pMsgLayer);

	GSProto::SCMessage pkg;
	HelpMakeScMsg(pkg, GSProto::CMD_MARQUEE, scMsg);
	
	pMsgLayer->broadcastMsg(pkg);
}


bool ArenaFactory::fightArenaRank(HEntity hActor, int iRank, DelegatePVEFight cb)
{
	// 注意，公共部分需要处理战报保存、日志记录、排名切换
	if( (iRank <= 0) || (iRank > (int)m_globalArenaData.rankList.size() ) )
	{
		return false;
	}

	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return false;

	string strActorName = pActor->getProperty(PROP_ENTITY_NAME, "");
	int iSelfRank = getArenaRank(strActorName);

	// 判断自身是否忙
	if( (iSelfRank != 0) && (m_busyTarget.find(iSelfRank) != m_busyTarget.end() ) )
	{
		pActor->sendErrorCode(ERROR_ARENA_BUSY);
		return false;
	}

	// 判断对方是否忙
	if(m_busyTarget.find(iRank) != m_busyTarget.end() )
	{
		pActor->sendErrorCode(ERROR_ARENA_BUSY);
		return false;
	}

	// 是否是自己
	if(iRank == iSelfRank)
	{
		return false;
	}

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	IFightSystem* pFightSys = static_cast<IFightSystem*>(pActor->querySubsystem(IID_IFightSystem));
	if(!pFightSys) return false;

	m_busyTarget.insert(iRank);
	if(iSelfRank != 0) m_busyTarget.insert(iSelfRank);
	
	const ServerEngine::ArenaSavePlayer& targetPlayer = m_globalArenaData.rankList[iRank-1];
	if(targetPlayer.bMonster)
	{
		ServerEngine::FightContext ctx;
		ctx.iSceneID = pGlobalCfg->getInt("竞技场场景", 1);
		ctx.iFightType = GSProto::en_FightType_Arena;
		pFightSys->AsyncPVEFight(ArenaPVECallback(this, hActor, iSelfRank, iRank, cb), targetPlayer.iMonsterGrpID, ctx);
	}
	else
	{
		ServerEngine::FightContext ctx;
		ctx.iSceneID = pGlobalCfg->getInt("竞技场场景", 1);
		ctx.iFightType = GSProto::en_FightType_Arena;
		pFightSys->AsyncPVPFight(ArenaPVPCallback(this, hActor, iSelfRank, iRank, cb), targetPlayer.roleKey, ctx);
	}

	return true;
}

void ArenaFactory::initArenaWithMonster(bool bFillRank)
{
	// 直接采用领地怪物
	ITable* pTable = getCompomentObjectManager()->findTable("Machine");//getCompomentObjectManager()->findTable("ArenaMonster");
	assert(pTable);

	IUserStateManager* pUserStateManager = getComponent<IUserStateManager>(COMPNAME_LoginManager, IID_IUserStateManager);
	assert(pUserStateManager);

	set<string> machineAccountList;
	set<string> machineNameList;
	
	int iRecordCount = pTable->getRecordCount();
	for(int i = 0; i < iRecordCount; i++)
	{
		ServerEngine::ArenaSavePlayer tmpArenaPlayer;
		
		string strName = pTable->getString(i, "名字");


		int iGuardFlag = pTable->getInt(i,"初始竞技场");

		HEntity hMachine = pUserStateManager->getRichEntityByName(strName);
		IEntity* pMachine = getEntityFromHandle(hMachine);
		assert(pMachine);

		if(1 == iGuardFlag)
		{	
			m_GuardMachineNameList.push_back( strName  );
		}
		
		if(!bFillRank)
		{
			updateActorInfo(hMachine);
			continue;
		}

	
		IFormationSystem* pFormationSys = static_cast<IFormationSystem*>(pMachine->querySubsystem(IID_IFormationSystem));
		assert(pFormationSys);

		tmpArenaPlayer.bMonster = false;
		tmpArenaPlayer.iShowHeroID = pFormationSys->getVisibleHeroID();
		tmpArenaPlayer.roleKey.strAccount = pMachine->getProperty(PROP_ACTOR_ACCOUNT, "");
		tmpArenaPlayer.roleKey.rolePos = 0;
		tmpArenaPlayer.roleKey.worldID = 0;
		tmpArenaPlayer.strName = strName;
		tmpArenaPlayer.iLevel = pMachine->getProperty(PROP_ENTITY_LEVEL, 0);
		tmpArenaPlayer.iFightValue = pMachine->getProperty(PROP_ENTITY_FIGHTVALUE, 0);

		IHeroSystem* pHeroSys = static_cast<IHeroSystem*>(pMachine->querySubsystem(IID_IHeroSystem) );
		assert(pHeroSys);

		pHeroSys->fillFemaleHeroList(tmpArenaPlayer.femaleHeroList);

		if(bFillRank)
		{
			m_globalArenaData.rankList.push_back(tmpArenaPlayer);

			
		}

		
	
	}

	// debug
	/*
	vector<ServerEngine::ArenaSavePlayer>::iterator it = this->m_globalArenaData.rankList.begin();
	while(it != this->m_globalArenaData.rankList.end()){
		const ServerEngine::ArenaSavePlayer& arenaSavePlayer = * it;
		cout<<"name:"<<arenaSavePlayer.strName<<" fightValue:"<<arenaSavePlayer.iFightValue<<endl;
		it ++;
	}
	*/
	// debug end

	assert(m_GuardMachineNameList.size() == 4);
	
}

void ArenaFactory::fillActorSaveInfo(HEntity hActor, ServerEngine::ArenaSavePlayer& arenaPlayer)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

	IFormationSystem* pFormationSys = static_cast<IFormationSystem*>(pActor->querySubsystem(IID_IFormationSystem));
	assert(pFormationSys);

	arenaPlayer.bMonster = false;
	arenaPlayer.iShowHeroID = pFormationSys->getVisibleHeroID();
	
	arenaPlayer.roleKey.strAccount = pActor->getProperty(PROP_ACTOR_ACCOUNT, "");
	arenaPlayer.roleKey.rolePos = 0;
	arenaPlayer.roleKey.worldID = pActor->getProperty(PROP_ACTOR_WORLD, 0);
	arenaPlayer.strName = pActor->getProperty(PROP_ENTITY_NAME, "");
	arenaPlayer.iLevel = pActor->getProperty(PROP_ENTITY_LEVEL, 0);
	arenaPlayer.iFightValue = pActor->getProperty(PROP_ENTITY_FIGHTVALUE, 0);

	IHeroSystem* pHeroSys = static_cast<IHeroSystem*>(pActor->querySubsystem(IID_IHeroSystem));
	assert(pHeroSys);

	arenaPlayer.femaleHeroList.clear();
	pHeroSys->fillFemaleHeroList(arenaPlayer.femaleHeroList);
}


void ArenaFactory::randActorChallenge(HEntity hActor, vector<GSProto::ArenaPlayer>& challengeList)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

	string strActorName = pActor->getProperty(PROP_ENTITY_NAME, "");

	IFormationSystem* pFormationSys = static_cast<IFormationSystem*>(pActor->querySubsystem(IID_IFormationSystem));
	assert(pFormationSys);
	
	int iArenaRank = getArenaRank(strActorName);
	if( (0 == iArenaRank) && (m_globalArenaData.rankList.size() < MAX_ARENA_SIZE) )
	{
		ServerEngine::ArenaSavePlayer tmpArenaPlayer;
		fillActorSaveInfo(hActor, tmpArenaPlayer);

		m_globalArenaData.rankList.push_back(tmpArenaPlayer);
		
		m_arenaRankMap[strActorName] = (int)m_globalArenaData.rankList.size();
		iArenaRank = (int)m_globalArenaData.rankList.size();

		//第一次找机器人对战
		for(size_t i = 0; i < m_GuardMachineNameList.size(); ++i)
		{
			string strMachineName = m_GuardMachineNameList[i];
			int iRankIndex = getArenaRank(strMachineName);
			addArenaPlayer( challengeList , iRankIndex-1 );
		}
		
		return;
	}

	// 随机挑战目标
	/*if(iArenaRank == 0)
	{
		generatePlayerList(challengeList, 10000, 100);
	}
	else if(iArenaRank <= ARENA_PAGE_COUNT)
	{
		int iMax = std::min(ARENA_PAGE_COUNT, (int)m_globalArenaData.rankList.size() );
		for(int i = 0; i < iMax; i++) addArenaPlayer(challengeList, i);
	}
	else if(iArenaRank <= 200)
	{
		for(int i = iArenaRank - ARENA_PAGE_COUNT - 1; i < iArenaRank - 1; i++) addArenaPlayer(challengeList, i);
	}
	else if(iArenaRank <= 2000)
	{
		generatePlayerList(challengeList, iArenaRank, 20);
	}
	else if(iArenaRank <= 10000)
	{
		generatePlayerList(challengeList, iArenaRank, 100);
	}
	else
	{
		generatePlayerList(challengeList, 10000, 100);
	}*/

	size_t randRuleListSize = m_RandRuleList.size();
	if(iArenaRank == 0)
	{
		RandRuleUnit &unit = m_RandRuleList[randRuleListSize-1];
		generatePlayerList(challengeList, unit, iArenaRank);
	}
	else
	{
		for(size_t i = 0; i < randRuleListSize; ++i)
		{
			const RandRuleUnit &unit = m_RandRuleList[ i ];
			if(unit.iLowRankNum <= iArenaRank && iArenaRank <= unit.iHightRankNum)
			{
				generatePlayerList(challengeList, unit, iArenaRank);
				break;
			}
		}
	}
	
}

void ArenaFactory::addArenaPlayer(vector<GSProto::ArenaPlayer>& challengeList, int iIndex)
{
	const ServerEngine::ArenaSavePlayer& refArenaSavePlayer = m_globalArenaData.rankList[iIndex];
	GSProto::ArenaPlayer tmpPlayer;

	tmpPlayer.set_bmonster(refArenaSavePlayer.bMonster);
	tmpPlayer.set_ishowid(refArenaSavePlayer.iShowHeroID);
	tmpPlayer.set_strname(refArenaSavePlayer.strName);
	tmpPlayer.set_ilevel(refArenaSavePlayer.iLevel);
	tmpPlayer.set_ifightvalue(refArenaSavePlayer.iFightValue);
	tmpPlayer.set_irank(iIndex+1);

	//FDLOG("ArenaSystem::pushArenaInfo")<<refArenaSavePlayer.strName<<" | "<<refArenaSavePlayer.iFightValue<<endl;
	challengeList.push_back(tmpPlayer);
}

void ArenaFactory::generatePlayerList(vector<GSProto::ArenaPlayer>& challengeList, const RandRuleUnit& unit, int iActorRank)
{
	if(unit.rankSet1[2] != 0)
	{
		getPlayerList(challengeList,unit.rankSet1,iActorRank);
	}

	if(unit.rankSet2[2] != 0)
	{
		getPlayerList(challengeList,unit.rankSet2,iActorRank);
	}
}

void ArenaFactory::getPlayerList(vector<GSProto::ArenaPlayer>& challengeList,const vector<int>& randSet, int iActorRank)
{
	assert(randSet.size() == 3);
	int iLimitRandCount = ARENA_PAGE_COUNT*20;
	set<int> usedList;
	int iRank = std::min(iActorRank, randSet[1]);
	
	int iRange = iRank - randSet[0] ;
	int iBeginIndex = randSet[0] -1;

	if(( iRank-randSet[0] +1) == randSet[2])
	{
		for(int i = iBeginIndex; i <  iRank; i++)
		{
			addArenaPlayer(challengeList, i);
		}
	}
	else
	{
		IRandom* pRandom = getComponent<IRandom>("Random", IID_IMiniAprRandom);
		assert(pRandom);
		
		while( (usedList.size() < (size_t)randSet[2]) && (--iLimitRandCount > 0) )
		{
			int iRandV = pRandom->random() % iRange;
			if(usedList.find(iRandV) == usedList.end() )
			{
				usedList.insert(iRandV);
			}
		}

		for(set<int>::iterator it = usedList.begin(); it != usedList.end(); it++)
		{
			addArenaPlayer(challengeList, *it + iBeginIndex);
		}
	}
}


bool ArenaFactory::hasAwardCanGet(HEntity hActor)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return false;

	string strActorName = pActor->getProperty(PROP_ENTITY_NAME, "");
	map<std::string, vector<ServerEngine::ArenaSaveAwardItem> >::iterator it = m_globalArenaData.awardMap.find(strActorName);
	if(it == m_globalArenaData.awardMap.end() )
	{
		return false;
	}

	const vector<ServerEngine::ArenaSaveAwardItem>& awardList = it->second;

	return awardList.size() > 0;
}


void ArenaFactory::fillActorAward(HEntity hActor, google::protobuf::RepeatedPtrField<GSProto::ArenaAwardItem>* pszAwardItem)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

	string strActorName = pActor->getProperty(PROP_ENTITY_NAME, "");

	// 先填充现有奖励
	map<std::string, vector<ServerEngine::ArenaSaveAwardItem> >::iterator it = m_globalArenaData.awardMap.find(strActorName);
	if(it != m_globalArenaData.awardMap.end() )
	{
		vector<ServerEngine::ArenaSaveAwardItem>& awardList = it->second;

		// 如果超过2个，则去掉开始的几个
		if(awardList.size() > 7)
		{
			awardList.erase(awardList.begin(), awardList.begin() + (awardList.size() - 7) );
		}
		
		for(vector<ServerEngine::ArenaSaveAwardItem>::reverse_iterator it = awardList.rbegin(); it != awardList.rend(); it++)
		{
			ServerEngine::ArenaSaveAwardItem& arenaSaveAwardItem = *it;
			if(arenaSaveAwardItem.strUUID.size() == 0)
			{
				uuid_t itemuuid;
				uuid_generate(itemuuid);
				char szUUIDString[1024] = {0};
				uuid_unparse_upper(itemuuid, szUUIDString);
				arenaSaveAwardItem.strUUID = szUUIDString;
			}

			GSProto::ArenaAwardItem* pNewAwardItem = pszAwardItem->Add();
			pNewAwardItem->set_dwtime(arenaSaveAwardItem.dwTime);
			pNewAwardItem->set_irank(arenaSaveAwardItem.iRank);
			pNewAwardItem->set_struuid(arenaSaveAwardItem.strUUID);
			pNewAwardItem->set_bcangetnow(true);

			int iHonor = 0;
			int iSilver = 0;
			int iGold = 0;
			getRankAward(arenaSaveAwardItem.iRank, iHonor, iSilver, iGold);
			pNewAwardItem->set_ihonoraward(iHonor);
			pNewAwardItem->set_isilveraward(iSilver);
			pNewAwardItem->set_igold(iGold);
		}
	}

	// 再填充即将获取的英雄
	int iCurRank = getArenaRank(strActorName);
	int iCurHonor = 0;
	int iCurSilver = 0;
	int iGold = 0;
	if(getRankAward(iCurRank, iCurHonor, iCurSilver, iGold) )
	{
		GSProto::ArenaAwardItem* pNewAwardItem = pszAwardItem->Add();
		pNewAwardItem->set_dwtime(time(0) );
		pNewAwardItem->set_irank(iCurRank);
		pNewAwardItem->set_ihonoraward(iCurHonor);
		pNewAwardItem->set_isilveraward(iCurSilver);
		pNewAwardItem->set_igold(iGold);

		pNewAwardItem->set_bcangetnow(false);
		pNewAwardItem->set_ileftsecond(getNextAwardCD() );
		pNewAwardItem->set_struuid("");
	}
}


int ArenaFactory::getNextAwardCD()
{
	IZoneTime* pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	int iAwardHour = pGlobalCfg->getInt("竞技场领奖时", 19);
	int iHorInDay = pZoneTime->GetHourInDay(pZoneTime->GetCurSecond() );

	int iDayBeginSecond = (int)(pZoneTime->GetDayBeginSecond(pZoneTime->GetCurSecond() ) );
	int iSecondInDay = (int)pZoneTime->GetCurSecond() - iDayBeginSecond;
	if(iHorInDay < iAwardHour)
	{
		return iAwardHour*3600 - iSecondInDay;
	}
	else
	{
		return 24*3600 - iSecondInDay + iAwardHour*3600;
	}
}

const ArenaAwardCfg* ArenaFactory::queryArenaAwardCfg(int iRank)
{
	for(size_t i = 0; i < m_arenaAwardCfgList.size(); i++)
	{
		if( (iRank>= m_arenaAwardCfgList[i].iMinRank) && (iRank<=m_arenaAwardCfgList[i].iMaxRank) )
		{
			return &(m_arenaAwardCfgList[i]);
		}
	}

	return NULL;
}

bool ArenaFactory::getRankAward(int iRank, int& iHonor, int& iSilver, int& iGold)
{
	if(iRank == 0)
	{
		return false;
	}

	const ArenaAwardCfg* pCfg = queryArenaAwardCfg(iRank);
	if(!pCfg) return false;

	iHonor = pCfg->iHonor;
	iSilver = pCfg->iSilver;
	iGold = pCfg->iGold;

	return true;
}

void ArenaFactory::loadArenaAward()
{
	ITable* pTable = getCompomentObjectManager()->findTable("ArenaAward");
	assert(pTable);

	int iRecordCount = pTable->getRecordCount();

	for(int i = 0; i < iRecordCount; i++)
	{
		ArenaAwardCfg tmpCfg;
		string strRankInfo = pTable->getString(i, "名次");
		vector<int> szRankList = TC_Common::sepstr<int>(strRankInfo, "#");
		if( (szRankList.size() != 1) && (szRankList.size() != 2) )
		{
			assert(false);
			continue;
		}

		tmpCfg.iMinRank = szRankList[0];
		if(szRankList.size() == 1)
		{
			tmpCfg.iMaxRank = tmpCfg.iMinRank;
		}
		else
		{
			tmpCfg.iMaxRank = szRankList[1];
		}

		tmpCfg.iSilver = pTable->getInt(i, "铜钱");
		tmpCfg.iHonor = pTable->getInt(i, "荣誉");
		tmpCfg.iGold = pTable->getInt(i, "元宝");

		m_arenaAwardCfgList.push_back(tmpCfg);
	}
}


void ArenaFactory::moveArenaAward(const string& strActorName, vector<ServerEngine::MailData>& mailDataList)
{
	map<std::string, vector<ServerEngine::ArenaSaveAwardItem> >::iterator it = m_globalArenaData.awardMap.find(strActorName);
	if(it == m_globalArenaData.awardMap.end() )
	{
		return;
	}

	IDropFactory* pDropFactory = getComponent<IDropFactory>(COMPNAME_DropFactory, IID_IDropFactory);
	assert(pDropFactory);

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);
	
	int iMailKeepDay = pGlobalCfg->getInt("邮件保留天数", 15);
	string strArenaAwardTitle = pGlobalCfg->getString("竞技场奖励邮件标题", "Arena Award");
	string strContent = pGlobalCfg->getString("竞技场奖励邮件正文", "Arena Award %rank");
	string strSender = pGlobalCfg->getString("竞技场奖励发送者", "ArenaManager");

	vector<ServerEngine::ArenaSaveAwardItem>& awardList = it->second;
	for(size_t i = 0; i < awardList.size(); i++)
	{
		string strTmpContent = TC_Common::replace(strContent, "%rank", TC_Common::tostr(awardList[i].iRank) );

		int iHonor = 0;
		int iSilver = 0;
		int iGold = 0;
		if(!getRankAward(awardList[i].iRank, iHonor, iSilver, iGold) )
		{
			continue;
		}

		if( awardList[i].strUUID.size() == 0)
		{
			uuid_t itemuuid;
			uuid_generate(itemuuid);
			char szUUIDString[1024] = {0};
			uuid_unparse_upper(itemuuid, szUUIDString);
			awardList[i].strUUID = szUUIDString;
		}
		
		GSProto::FightAwardResult awardResult;
		pDropFactory->addPropToResult(awardResult, GSProto::en_LifeAtt_Honor, iHonor);
		pDropFactory->addPropToResult(awardResult, GSProto::en_LifeAtt_Silver, iSilver);
		pDropFactory->addPropToResult(awardResult, GSProto::en_LifeAtt_Gold, iGold);
	
		ServerEngine::MailData tmpMailData;
		tmpMailData.strUUID = awardList[i].strUUID;
		tmpMailData.strTitle = strArenaAwardTitle;
		tmpMailData.strSender = strSender;
		tmpMailData.strContent = strTmpContent;
		
		
		awardResult.SerializeToString(&tmpMailData.strAwardInfo);

		tmpMailData.iState = GSProto::en_MailState_UnRead;
		tmpMailData.dwSenderTime = awardList[i].dwTime;
		tmpMailData.iKeepDay = iMailKeepDay;
		tmpMailData.iMailType = GSProto::enMailType_Com;

		mailDataList.push_back(tmpMailData);
	}

	m_globalArenaData.awardMap.erase(it);
}


bool ArenaFactory::doGetArenaAward(HEntity hActor, const string& strUUID)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return false;

	string strActorName = pActor->getProperty(PROP_ENTITY_NAME, "");

	map<std::string, vector<ServerEngine::ArenaSaveAwardItem> >::iterator it = m_globalArenaData.awardMap.find(strActorName);
	if(it == m_globalArenaData.awardMap.end() )
	{
		return false;
	}

	vector<ServerEngine::ArenaSaveAwardItem>& awardList = it->second;
	for(size_t i = 0; i < awardList.size(); i++)
	{
		if(awardList[i].strUUID == strUUID)
		{
			ServerEngine::ArenaSaveAwardItem tmpAwardItem = awardList[i];
			awardList.erase(awardList.begin() + i);

			int iHonor = 0;
			int iSilver = 0;
			int iGold = 0;
			if(!getRankAward(tmpAwardItem.iRank, iHonor, iSilver, iGold) )
			{
				return false;
			}

			pActor->changeProperty(PROP_ENTITY_HONOR, iHonor, GSProto::en_Reason_ArenaRankGet);
			pActor->changeProperty(PROP_ACTOR_SILVER, iSilver, GSProto::en_Reason_ArenaRankGet);
			pActor->changeProperty(PROP_ACTOR_GOLD, iGold, GSProto::en_Reason_ArenaRankGet);
			return true;
		}
	}	

	return false;
}


void ArenaFactory::sendTopRankList(HEntity hActor)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

	GSProto::CMD_QUERY_ARENA_RANK_SC scMsg;

	size_t iLimit = std::min( (size_t)MAX_SHOW_RANKSIZE, m_globalArenaData.rankList.size() );

	for(size_t i = 0; i < iLimit; i++)
	{
		const ServerEngine::ArenaSavePlayer& refSavePlayer = m_globalArenaData.rankList[i];
		GSProto::ArenaRankItem* pNewRankItem = scMsg.mutable_szranklist()->Add();

		pNewRankItem->set_strname(refSavePlayer.strName);
		pNewRankItem->set_ilevel(refSavePlayer.iLevel);
		pNewRankItem->set_ifightvalue(refSavePlayer.iFightValue);
	}

	pActor->sendMessage(GSProto::CMD_QUERY_ARENA_RANK, scMsg);
}


void ArenaFactory::onTimer(int nEventId)
{
	if(1 == nEventId)
	{
		saveArenaData(false);
	}
	else if(2 == nEventId)
	{
		checkArenaAward();
	}
	else if(3 == nEventId)
	{
		loadArenaData();
	}
	else if(4 == nEventId)
	{
		rankFightValue();
	}
}


void ArenaFactory::checkArenaAward()
{
	PROFILE_MONITOR("checkArenaAward")
	IZoneTime* pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);

	// 当天已经发过了
	if(pZoneTime->IsInSameDay(pZoneTime->GetCurSecond(), m_globalArenaData.dwLastAwardTime) )
	{
		return;
	}

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	int iAwardHour = pGlobalCfg->getInt("竞技场领奖时", 19);
	int iHourInDay = pZoneTime->GetHourInDay(pZoneTime->GetCurSecond() );
	if(iHourInDay < iAwardHour)
	{
		return;
	}

	Uint32 dwCurSecond = pZoneTime->GetCurSecond();

	// 开始发奖
	size_t iLimit = std::min( (size_t)5000, m_globalArenaData.rankList.size() );
	for(size_t i = 0; i < iLimit; i++)
	{
		const ServerEngine::ArenaSavePlayer& refArenaRankItem = m_globalArenaData.rankList[i];

		// 时间关系，这里不生成UUID, 也不做剔除多余记录,留到玩家查询处理
		ServerEngine::ArenaSaveAwardItem tmpAwardItem;
		tmpAwardItem.dwTime = dwCurSecond;
		tmpAwardItem.iRank = (int)i+1;
		m_globalArenaData.awardMap[refArenaRankItem.strName].push_back(tmpAwardItem);
		FDLOG("ArenaAward")<<refArenaRankItem.strName<<"|"<<tmpAwardItem.iRank<<"|"<<tmpAwardItem.dwTime<<endl;
	}
	m_globalArenaData.dwLastAwardTime = dwCurSecond;
}

class ArenaSaveCb:public ServerEngine::LegionPrxCallback
{
public:

	virtual void callback_setGlobalData(taf::Int32 ret)
	{
		FDLOG("ArenaSave")<<"ret|"<<ret<<endl;
	}

	virtual void callback_setGlobalData_exception(taf::Int32 ret)
	{
		FDLOG("ArenaSave")<<"exception|"<<ret<<endl;
	}
};

void ArenaFactory::saveArenaData(bool bSync)
{
	IJZMessageLayer* pMsgLayer = getComponent<IJZMessageLayer>(COMPNAME_MessageLayer, IID_IJZMessageLayer);
	assert(pMsgLayer);

	string strGlobalData = ServerEngine::JceToStr(m_globalArenaData);

	try
	{
		if(bSync)
		{
			pMsgLayer->setGlobalData(GLOBAL_ARENA_DATA, strGlobalData);
		}
		else
		{
			pMsgLayer->AsyncSetGlobalData(new ArenaSaveCb, GLOBAL_ARENA_DATA, strGlobalData);
		}
	}
	catch(...)
	{
		SvrErrLog("ArenaFactory::saveArenaData Exception");
	}
}


void ArenaFactory::updateActorInfo(HEntity hActor)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

	string strActorName = pActor->getProperty(PROP_ENTITY_NAME, "");
	int iRank = getArenaRank(strActorName);

	if(iRank == 0)
	{
		return;
	}

	IFormationSystem* pFormationSys = static_cast<IFormationSystem*>(pActor->querySubsystem(IID_IFormationSystem));
	if(!pFormationSys) return;

	ServerEngine::ArenaSavePlayer& refSavePlayer = m_globalArenaData.rankList[iRank - 1];
	refSavePlayer.iShowHeroID = pFormationSys->getVisibleHeroID();
	refSavePlayer.iLevel = pActor->getProperty(PROP_ENTITY_LEVEL, 0);
	refSavePlayer.iFightValue = pActor->getProperty(PROP_ENTITY_FIGHTVALUE, 0);
	
	//FDLOG("ArenaSystem::pushArenaInfo")<<refSavePlayer.strName<<" | "<<refSavePlayer.iFightValue<<endl;

	IHeroSystem* pHeroSys = static_cast<IHeroSystem*>(pActor->querySubsystem(IID_IHeroSystem));
	assert(pHeroSys);
	
	refSavePlayer.femaleHeroList.clear();
	pHeroSys->fillFemaleHeroList(refSavePlayer.femaleHeroList);
}


void ArenaFactory::updateActorBeManorLootInfo(HEntity hActor)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

	string strActorName = pActor->getProperty(PROP_ENTITY_NAME, "");
	int iRank = getArenaRank(strActorName);

	if(iRank == 0)
	{
		return;
	}
	
	IFormationSystem* pFormationSys = static_cast<IFormationSystem*>(pActor->querySubsystem(IID_IFormationSystem));
	if(!pFormationSys) return;

	ServerEngine::ArenaSavePlayer& refSavePlayer = m_globalArenaData.rankList[iRank - 1];
	refSavePlayer.iShowHeroID = pFormationSys->getVisibleHeroID();
	refSavePlayer.iLevel = pActor->getProperty(PROP_ENTITY_LEVEL, 0);
	refSavePlayer.iFightValue = pActor->getProperty(PROP_ENTITY_FIGHTVALUE, 0);
	
	///FDLOG("ArenaSystem::pushArenaInfo")<<refSavePlayer.strName<<" | "<<refSavePlayer.iFightValue<<endl;

	IHeroSystem* pHeroSys = static_cast<IHeroSystem*>(pActor->querySubsystem(IID_IHeroSystem));
	assert(pHeroSys);
	
	refSavePlayer.femaleHeroList.clear();
	pHeroSys->fillFemaleHeroList(refSavePlayer.femaleHeroList);

	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	refSavePlayer.iLastBeLootSecond = pZoneTime->GetCurSecond();

}

struct FightValueSort
{
	bool operator()(const ServerEngine::ArenaSavePlayer& actorA , const ServerEngine::ArenaSavePlayer& actorB) const
	{
		if(actorA.iFightValue != actorB.iFightValue)
		{
			return actorA.iFightValue < actorB.iFightValue;
		}
		return actorA.strName > actorB.strName;
	}
};


void ArenaFactory::rankFightValue()
{
	m_fightValueList = m_globalArenaData.rankList;
	std::sort(m_fightValueList.begin(),m_fightValueList.end(), FightValueSort());
} 

bool  ArenaFactory::getDreamLandList(HEntity hEntity, ServerEngine::ArenaSavePlayer& player, int imin, int iMax)
{
	if(m_fightValueList.size() == 0)
	{
		// cout<<"caculate fight value list...."<<endl;
		rankFightValue();
	}

	// debug
//	vector<ServerEngine::ArenaSavePlayer>::iterator it = this->m_fightValueList.begin();
	//while(it != this->m_fightValueList.end()){
	//	const ServerEngine::ArenaSavePlayer& arenaSavePlayer = * it;
		// cout<<"=== name:"<<arenaSavePlayer.strName<<" fightValue:"<<arenaSavePlayer.iFightValue<<endl;
	//	it ++;
	//}
	// debug end
	
	
	IEntity *pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);
	
	string strActorName = pEntity->getProperty( PROP_ENTITY_NAME, "");
	assert(strActorName.length() > 0);

	int iHistoryFightValue = pEntity->getProperty( PROP_ACTER_MAX_FIGHTVALUE, 0);
	int iActorRank  = 0;

	ServerEngine::ArenaSavePlayer actor;
	actor.strName = strActorName;
	actor.iFightValue = iHistoryFightValue;
	

	// TODO:以下逻辑存在问题
	vector<ServerEngine::ArenaSavePlayer>::iterator iter = std::lower_bound(m_fightValueList.begin(),m_fightValueList.end(),actor,FightValueSort());
	iActorRank = std::distance(m_fightValueList.begin(),iter);
	// cout<<"name:"<<strActorName<<" fight value:"<<iHistoryFightValue<<" iActorRank:"<<iActorRank<<endl;
	// iActorRank = 1
	// imin = -201
	// iMax = -180
	// m_fightValueList.size() = 316


	int iLowerIndex = iActorRank+imin;
	iLowerIndex = std::max( iLowerIndex, 0);
	
	int hightIndex = iActorRank+iMax;
	hightIndex = std::min(hightIndex, (int)m_fightValueList.size()-1);

	if( hightIndex -iLowerIndex <= 0)
	{
		return false;
	}

	IRandom* pRandom = getComponent<IRandom>("Random", IID_IMiniAprRandom);
	assert(pRandom);
	int randNum = hightIndex - iLowerIndex + 1;
	int desIndex = ( pRandom->random() % randNum) + iLowerIndex;
	int cout = 0;
	while( m_fightValueList[desIndex].strName == strActorName)
	{
		desIndex = ( pRandom->random() % randNum) + iLowerIndex;
		cout++;
		if(cout == 1000)
		{
			return false;
		}
	}
	
	player = m_fightValueList[desIndex];
	return true;
}

bool ArenaFactory::getManorWillLootActor(HEntity hActor, ServerEngine::ArenaSavePlayer& player)
{
	if(m_fightValueList.size() == 0)
	{
		rankFightValue();
	}
	IEntity *pEntity = getEntityFromHandle(hActor);
	assert(pEntity);

	IRandom *pRandom = getComponent<IRandom>( COMPNAME_Random,  IID_IMiniAprRandom);
	assert(pRandom);
	
	string strActorName = pEntity->getProperty( PROP_ENTITY_NAME, "");
	int iHistoryFightValue = pEntity->getProperty( PROP_ACTER_MAX_FIGHTVALUE, 0);
	ServerEngine::ArenaSavePlayer actor;
	actor.strName = strActorName;
	actor.iFightValue = iHistoryFightValue;
	vector<ServerEngine::ArenaSavePlayer>::iterator iter = std::lower_bound(m_fightValueList.begin(),m_fightValueList.end(),actor,FightValueSort());
	int iRankIndex  = std::distance(m_fightValueList.begin(),iter) - 1;
	//根据排名 分段
	//100< | 100 > iRank < 100 | >100

	const int iInterval = 100;
	int iLowRank = iRankIndex - iInterval;
	iLowRank = std::max(iLowRank, 0);
	
	int iHighRank = iRankIndex + iInterval;
	int iSize = m_fightValueList.size();
	iHighRank = std::min(iHighRank, iSize);
	assert(iLowRank != iHighRank );
	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	IGlobalCfg *pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg,IID_IGlobalCfg);
	assert(pGlobal);
	int iSecond = pGlobal->getInt("领地被掠夺了的保护时间",7200);
	int iCurSecond = pZoneTime->GetCurSecond();
	
	vector<ServerEngine::ArenaSavePlayer> canLootActorVec;
	for(int i = iLowRank; i < iHighRank; ++i)
	{
		if( i != iRankIndex)
		{
			const ServerEngine::ArenaSavePlayer& arenaPlayer = m_fightValueList[i];
			if( ( iCurSecond - arenaPlayer.iLastBeLootSecond >= iSecond ) && (arenaPlayer.strName != strActorName) )
			{
				canLootActorVec.push_back( arenaPlayer);
			}
		}
	}

	int iAreanCount = iHighRank-iLowRank;
	int iRandPercent = pRandom->random()% iAreanCount;
	int iCanLootSize = canLootActorVec.size();
	if( iCanLootSize > iRandPercent)
	{
		int iRandIndex = pRandom->random()%canLootActorVec.size();
		player = canLootActorVec[iRandIndex];
		return true;
	}
	else
	{
		assert(m_fightValueList.size() > 0);
		int iCount = 0;
		int iTotalRand = pGlobal->getInt("领地掠夺查找次数",20);
		while(1)
		{
			int iRandIndex = pRandom->random()%m_fightValueList.size();
			const ServerEngine::ArenaSavePlayer& arenaPlayer = m_fightValueList[iRandIndex];
			if( ( iCurSecond - arenaPlayer.iLastBeLootSecond >= iSecond ) && (arenaPlayer.strName != strActorName) )
			{
				player = arenaPlayer;
				return true;
			}
		 	iCount ++; 
			if(iCount >= iTotalRand )
			{
			//pntity->sendErrorCode( ERROR_NO_CANLOOTACTOR);
				IScriptEngine *pScript = getComponent<IScriptEngine>(COMPNAME_ScriptEngine, IID_IScriptEngine);
				assert(pScript);

				EventArgs args;
				args.context.setInt("entity", hActor);
				pScript->runFunction("manorNoCanLootActor",&args, "EventArgs");
				
				return false;
			}
		}
	}
	return false;
}

bool ArenaFactory::getActorForCityBattle(vector<ServerEngine::ArenaSavePlayer>&playerList, int iCount)
{
	int iSize = m_globalArenaData.rankList.size();
	assert(iSize > 200);

	
	IRandom *pRandom = getComponent<IRandom>( COMPNAME_Random,  IID_IMiniAprRandom);
	assert(pRandom);
	
	for(int i =0; i < iCount; ++i)
	{
		int iRandIndex = pRandom->random()%200;
		
		assert(iRandIndex < iSize );

		playerList.push_back(m_globalArenaData.rankList[iRandIndex]);
		
	}
	
	return true;
}


