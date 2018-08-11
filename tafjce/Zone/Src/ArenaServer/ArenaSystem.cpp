#include "ArenaServerPch.h"
#include "ArenaSystem.h"
#include "ArenaFactory.h"
#include "IVIPFactory.h"


extern "C" IObject* createArenaSystem()
{
	return new ArenaSystem;
}


ArenaSystem::ArenaSystem():m_dwLastArenaFightTime(0)
{
}

ArenaSystem::~ArenaSystem()
{
}

bool ArenaSystem::create(IEntity* pEntity, const std::string& strData)
{
	assert(pEntity);

	m_hEntity = pEntity->getHandle();

	if(strData.size() > 0)
	{
		ServerEngine::JceToObj(strData, m_arenaSaveData);
	}
	else
	{
		m_arenaSaveData.bHaveNewLog = false;
	}

	pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_SENDACTOR_TOCLIENT, this, &ArenaSystem::onEventSend2Clinet);
	checkNotice();
	return true;
}

void ArenaSystem::checkNotice()
{
	bool bNotice = m_arenaSaveData.bHaveNewLog;
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	pEntity->chgNotice(GSProto::en_NoticeGuid_ArenaHaveNewLog, bNotice);
}

bool ArenaSystem::createComplete()
{

	return true;
}

void ArenaSystem::onEventSend2Clinet(EventArgs& args)
{
	ArenaFactory* pArenaFactory = static_cast<ArenaFactory*>(getComponent<IArenaFactory>("ArenaFactory", IID_IArenaFactory) );
	assert(pArenaFactory);

	pArenaFactory->updateActorInfo(m_hEntity);

	// 注册战力变化事件
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_FIGHTVALUECHG, this, &ArenaSystem::onEventFightValueChg);
}

void ArenaSystem::onEventFightValueChg(EventArgs& args)
{
	ArenaFactory* pArenaFactory = static_cast<ArenaFactory*>(getComponent<IArenaFactory>("ArenaFactory", IID_IArenaFactory) );
	assert(pArenaFactory);

	pArenaFactory->updateActorInfo(m_hEntity);
}



const std::vector<Uint32>& ArenaSystem::getSupportMessage()
{
	static std::vector<Uint32> resultList;
	if(resultList.size() == 0)
	{
		resultList.push_back(GSProto::CMD_OPEN_ARENA);
		resultList.push_back(GSProto::CMD_QUERY_ARENA_AWARD);
		resultList.push_back(GSProto::CMD_GET_ARENA_AWARD);
		resultList.push_back(GSProto::CMD_QUERY_ARENA_RANK);
		resultList.push_back(GSProto::CMD_QUERY_ARENA_LOG);
		resultList.push_back(GSProto::CMD_ADD_ARENACHALLENGE);
		resultList.push_back(GSProto::CMD_ARENA_FIGHT);
		resultList.push_back(GSProto::CMD_ARENA_CHANGE_CHALLENGELIST);
		resultList.push_back(GSProto::CMD_ARENA_RESET);
	}

	return resultList;
}

void ArenaSystem::onMessage(QxMessage* pMessage)
{
	assert(pMessage->dwMsgLen == sizeof(GSProto::CSMessage) );
	const GSProto::CSMessage& msg = *(const GSProto::CSMessage*)(pMessage->pMsgDataBuff);
	checkReset();

	switch(msg.icmd() )
	{
		case GSProto::CMD_OPEN_ARENA:
			onReqOpenArena(msg);
			break;

		case GSProto::CMD_QUERY_ARENA_AWARD:
			onReqQueryArenaAward(msg);
			break;

		case GSProto::CMD_GET_ARENA_AWARD:
			onReqGetArenaAward(msg);
			break;

		case GSProto::CMD_QUERY_ARENA_RANK:
			onReqQueryArenaRank(msg);
			break;

		case GSProto::CMD_QUERY_ARENA_LOG:
			onReqQueryArenaLog(msg);
			break;

		case GSProto::CMD_ADD_ARENACHALLENGE:
			onReqAddChallengeCount(msg);
			break;

		case GSProto::CMD_ARENA_FIGHT:
			onReqArenaFight(msg);
			break;
			
		case GSProto::CMD_ARENA_CHANGE_CHALLENGELIST:
			onChangeChallengeList(msg);
			break;
				
		case GSProto::CMD_ARENA_RESET:
			onResetRemaind(msg);
			break;
			
	}
}

void ArenaSystem::packSaveData(string& data)
{
	data = ServerEngine::JceToStr(m_arenaSaveData);
}

void ArenaSystem::onChangeChallengeList(const GSProto::CSMessage& msg)
{
	pushArenaInfo();
}

void ArenaSystem::onResetRemaind(const GSProto::CSMessage& msg)
{
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	const ServerEngine::ArenaAppendData& data = m_arenaSaveData.appendData;
	//1.有没有倒计时
	IGlobalCfg *pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);
	int iCountDown = pGlobal->getInt("竞技场挑战CD", 600);

	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	int iOverSecond =  pZoneTime->GetCurSecond() - data.iBeginCDSecond;
	if( iOverSecond> iCountDown )
	{
		pEntity->sendErrorCode(ERROR_ARENA_RESET_ERROR_CD);
		return;
	}
	
	int iResetCost = pGlobal->getInt("竞技场重置战斗CD消耗元宝", 50);
	EventArgs args;
	args.context.setInt("entity", m_hEntity);
	args.context.setInt("cost", iResetCost);
	
	IScriptEngine *pScrip = getComponent<IScriptEngine>(COMPNAME_ScriptEngine, IID_IScriptEngine);
	assert(pScrip);

	pScrip->runFunction("arenaGlodResetCD",&args, "EventArgs");
	
}

void ArenaSystem::confirmArenaGlodResetCD()
{
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	ServerEngine::ArenaAppendData& data = m_arenaSaveData.appendData;
	//1.有没有倒计时
	IGlobalCfg *pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);
	int iCountDown = pGlobal->getInt("竞技场挑战CD", 600);

	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	int iOverSecond =  pZoneTime->GetCurSecond() - data.iBeginCDSecond;
	if( iOverSecond> iCountDown )
	{
		pEntity->sendErrorCode(ERROR_ARENA_RESET_ERROR_CD);
		return;
	}
	int iResetCost = pGlobal->getInt("竞技场重置战斗CD消耗元宝", 50);
	int iActorHave = pEntity->getProperty( PROP_ACTOR_GOLD, 0);
	if(iActorHave < iResetCost)
	{
		pEntity->sendErrorCode(ERROR_NEED_GOLD);
		return;
	}
	pEntity->changeProperty(PROP_ACTOR_GOLD, 0 -iResetCost , GSProto::en_Reason_Arena_ResetCost );
	//
	data.iBeginCDSecond = pZoneTime->GetCurSecond() - iCountDown;

	//通知客户端清除CD
	pEntity->sendMessage(GSProto::CMD_ARENA_RESET);
	
}

void ArenaSystem::pushArenaInfo()
{
	ArenaFactory* pArenaFactory = static_cast<ArenaFactory*>(getComponent<IArenaFactory>("ArenaFactory", IID_IArenaFactory) );
	assert(pArenaFactory);

	IEntity* pActor = getEntityFromHandle(m_hEntity);
	assert(pActor);

	// 刷新挑战列表吧
	m_challengeList.clear();
	pArenaFactory->randActorChallenge(m_hEntity, m_challengeList);

	string strActorName = pActor->getProperty(PROP_ENTITY_NAME, "");
	int iRank = pArenaFactory->getArenaRank(strActorName);
	int iFightValue = pActor->getProperty(PROP_ENTITY_FIGHTVALUE, 0);

	GSProto::CMD_OPEN_ARENA_SC scMsg;
	scMsg.set_iselfrank(iRank);
	scMsg.set_ifightvalue(iFightValue);
	//scMsg.set_ileftchallengecount(getLeftChallengeCount() );
	
	GSProto::ArenaChallengeInfo* pArenaChallengeInfo = scMsg.mutable_challengeinfo();
	std::copy(m_challengeList.begin(), m_challengeList.end(), google::protobuf::RepeatedFieldBackInserter(pArenaChallengeInfo->mutable_szchallengelist() ) );
	scMsg.set_bcangetaward(pArenaFactory->hasAwardCanGet(m_hEntity) );

	fillArenaAppendData(*scMsg.mutable_appendinfo());
		
	pActor->sendMessage(GSProto::CMD_OPEN_ARENA, scMsg);
}

void ArenaSystem::onReqOpenArena(const GSProto::CSMessage& msg)
{
	pushArenaInfo();
}

void ArenaSystem::fillArenaAppendData(GSProto::ArenaAppendInfo& info)
{ 
	ServerEngine::ArenaAppendData& data = m_arenaSaveData.appendData;
	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);
	int iBaseLimit = pGlobalCfg->getInt("竞技场挑战免费次数", 5);
	IZoneTime* pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	int iActorChallenge = data.iNowChallengeCount;
	int iRemaindTimes = iBaseLimit - iActorChallenge;
	
	if( iRemaindTimes > 0 )
	{
		int iSeconds = pZoneTime->GetCurSecond() - data.iBeginCDSecond;
		int iBattleCd = pGlobalCfg->getInt("竞技场挑战CD", 600);
		int iRemaindSecond =  iBattleCd - iSeconds ;
		info.set_icurremaindtimes(iRemaindTimes);
		info.set_itotaltimes(iBaseLimit);
		if(iRemaindSecond > 0 )
		{
			info.set_state(GSProto::en_ArenaAppendState_Reset);
			info.set_iremaindsecond(iRemaindSecond);
			int iResetCost =  pGlobalCfg->getInt("竞技场重置花费元宝", 50);
			info.set_icost(iResetCost);
		}
		else
		{
			info.set_state(GSProto::en_ArenaAppendState_Change);
		}
	}
	else
	{
		int iBuyOneCost =  pGlobalCfg->getInt("竞技场挑战次数购买花费元宝", 50);
		info.set_icost(iBuyOneCost);
		info.set_state(GSProto::en_ArenaAppendState_BuyOnce);
	}
}

void ArenaSystem::checkReset()
{
	IZoneTime* pZomeTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZomeTime);
	ServerEngine::ArenaAppendData& data = m_arenaSaveData.appendData;
	if(!pZomeTime->IsInSameDay(data.iLastChgTime, pZomeTime->GetCurSecond() ) )
	{
		data.iBuyChallengeCount  = 0;
		data.iNowChallengeCount = 0;
		IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
		assert(pGlobalCfg);
		int iCountDown = pGlobalCfg->getInt("竞技场挑战CD", 600);
		data.iBeginCDSecond = pZomeTime->GetCurSecond() - iCountDown;
		data.iLastChgTime = pZomeTime->GetCurSecond();
	}
}
int ArenaSystem::getLeftChallengeCount()
{
	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	int iBaseLimit = pGlobalCfg->getInt("竞技场基础限次", 15);
	int iRealLimit = iBaseLimit + getDayResetValue(m_arenaSaveData.m_addChallengeCount).iValue;
	if(getDayResetValue(m_arenaSaveData.m_challengeCount).iValue >= iRealLimit)
	{
		return 0;
	}

	return iRealLimit - getDayResetValue(m_arenaSaveData.m_challengeCount).iValue;
}

void ArenaSystem::onReqQueryArenaAward(const GSProto::CSMessage& msg)
{
	ArenaFactory* pArenaFactory = static_cast<ArenaFactory*>(getComponent<IArenaFactory>("ArenaFactory", IID_IArenaFactory) );
	assert(pArenaFactory);

	GSProto::CMD_QUERY_ARENA_AWARD_SC scMsg;
	google::protobuf::RepeatedPtrField<GSProto::ArenaAwardItem>* pszAwardItem = scMsg.mutable_szarenaawardlist();

	pArenaFactory->fillActorAward(m_hEntity, pszAwardItem);

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	pEntity->sendMessage(GSProto::CMD_QUERY_ARENA_AWARD, scMsg);
}

void ArenaSystem::onReqGetArenaAward(const GSProto::CSMessage& msg)
{
	GSProto::CMD_GET_ARENA_AWARD_CS req;
	if(!req.ParseFromString(msg.strmsgbody() ) )
	{
		return;
	}

	ArenaFactory* pArenaFactory = static_cast<ArenaFactory*>(getComponent<IArenaFactory>("ArenaFactory", IID_IArenaFactory) );
	assert(pArenaFactory);

	if(!pArenaFactory->doGetArenaAward(m_hEntity, req.struuid() ) )
	{
		return;
	}

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	GSProto::CMD_GET_ARENA_AWARD_SC scMsg;
	scMsg.set_struuid(req.struuid() );
	scMsg.set_bcangetaward(pArenaFactory->hasAwardCanGet(m_hEntity) );

	pEntity->sendMessage(GSProto::CMD_GET_ARENA_AWARD, scMsg);
}

void ArenaSystem::onReqQueryArenaRank(const GSProto::CSMessage& msg)
{
	ArenaFactory* pArenaFactory = static_cast<ArenaFactory*>(getComponent<IArenaFactory>("ArenaFactory", IID_IArenaFactory) );
	assert(pArenaFactory);

	pArenaFactory->sendTopRankList(m_hEntity);
}

void ArenaSystem::onReqQueryArenaLog(const GSProto::CSMessage& msg)
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	GSProto::CMD_QUERY_ARENA_LOG_SC scMsg;

	for(size_t i = 0; i < m_arenaSaveData.m_arenaLogList.size(); i++)
	{
		GSProto::ArenaLogItem* pNewArenaLog = scMsg.mutable_szlogitemlist()->Add();
		pNewArenaLog->ParseFromString(m_arenaSaveData.m_arenaLogList[i]);
	}

	pEntity->sendMessage(GSProto::CMD_QUERY_ARENA_LOG, scMsg);
	m_arenaSaveData.bHaveNewLog = false;
	checkNotice();
}


int ArenaSystem::getArenBuyCountLimit()
{
	IVIPFactory* pVipFactory = getComponent<IVIPFactory>(COMPNAME_VIPFactory, IID_IVIPFactory);
	assert(pVipFactory);

	int iValue = pVipFactory->getVipPropByHEntity(m_hEntity, VIP_PROP_BUY_CHALLENGE_TIMES);

	return iValue;
}
void ArenaSystem::onReqAddChallengeCount(const GSProto::CSMessage& msg)
{
	// 判断可购买次数是否达到上限了
	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);
	
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity); 
	
	int iAddLimit = getArenBuyCountLimit();
	ServerEngine::ArenaAppendData& data = m_arenaSaveData.appendData;
	if( data.iBuyChallengeCount >= iAddLimit)
	{
		pEntity->sendErrorCode(ERROR_ARENA_BUYLIMIT);
		return ;
	}

	int iNeedGold = pGlobalCfg->getInt("竞技场购买挑战次数消耗元宝", 50);

	IScriptEngine* pScriptEngine = getComponent<IScriptEngine>(COMPNAME_ScriptEngine, IID_IScriptEngine);
	assert(pScriptEngine);

	EventArgs args;
	args.context.setInt("entity", m_hEntity);
	args.context.setInt("needgold", iNeedGold);
	pScriptEngine->runFunction("sendBuyChallengeConfirm", &args, "EventArgs");
}


bool ArenaSystem::addArenaCount()
{
	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	int iAddLimit = getArenBuyCountLimit();
	int iNeedGold = pGlobalCfg->getInt("竞技场购买挑战次数消耗元宝", 50);
	int iFreeCount =  pGlobalCfg->getInt("竞技场挑战免费次数", 5);
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	// 判断限次  
	ServerEngine::ArenaAppendData& data = m_arenaSaveData.appendData;
	if(data.iNowChallengeCount < iFreeCount)
	{
		return false;
	}
		
	int iTkGold = pEntity->getProperty(PROP_ACTOR_GOLD, 0);
	if(iTkGold < iNeedGold)
	{
		pEntity->sendErrorCode(ERROR_NEED_GOLD);
		return false;
	}
	
	
	if( data.iBuyChallengeCount >= iAddLimit)
	{
		pEntity->sendErrorCode(ERROR_ARENA_BUYLIMIT);
		return false;
	}

	pEntity->changeProperty(PROP_ACTOR_GOLD, 0-iNeedGold, GSProto::en_Reason_ArenaBuyCountCost);
	data.iBuyChallengeCount++;
	data.iNowChallengeCount = 0;
	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	
	int iCountDown = pGlobalCfg->getInt("竞技场挑战CD", 600);
	data.iBeginCDSecond = pZoneTime->GetCurSecond() - iCountDown ;
	
	data.iLastChgTime = pZoneTime->GetCurSecond();
	
	pushArenaInfo();

	return true;
}


struct ArenaFIghtCb
{
	ArenaFIghtCb(HEntity hEntity):m_hEntity(hEntity){}

	void operator()(taf::Int32 iRet, const ServerEngine::BattleData& battleData)
	{	
		IEntity* pEntity = getEntityFromHandle(m_hEntity);
		if(!pEntity) return;

		ArenaSystem* pArenaSys = static_cast<ArenaSystem*>(pEntity->querySubsystem(IID_IArenaSystem));
		if(!pArenaSys) return;

		pArenaSys->processArenaFight(iRet, battleData);
	}

private:

	HEntity m_hEntity;
};


void ArenaSystem::processArenaFight(int iRet, const ServerEngine::BattleData& battleData)
{
	//关闭上浮
	CloseAttCommUP close(m_hEntity);
	
	m_dwLastArenaFightTime = 0;
	if(iRet != en_QueryBattle_OK)
	{
		SvrErrLog("ArenaFight Fail");
		return;
	}
	
	ServerEngine::ArenaAppendData& data = m_arenaSaveData.appendData;
	data.iNowChallengeCount ++;
	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	data.iLastChgTime = pZoneTime->GetCurSecond();
	data.iBeginCDSecond = pZoneTime->GetCurSecond();
		
	

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	int iGetSilver = 0;
	int iGetHonor = 0;
	if(battleData.bAttackerWin)
	{
		iGetSilver = pGlobalCfg->getInt("竞技场胜利银币奖励", 5000);
		iGetHonor = pGlobalCfg->getInt("竞技场胜利荣誉奖励", 10);
	}
	else
	{
		iGetSilver = pGlobalCfg->getInt("竞技场失败银币奖励", 2000);
		iGetHonor = pGlobalCfg->getInt("竞技场失败荣誉奖励", 5);
	}

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	pEntity->changeProperty(PROP_ACTOR_SILVER, iGetSilver, GSProto::en_Reason_ArenaChallengeCreate);
	pEntity->changeProperty(PROP_ENTITY_HONOR, iGetHonor, GSProto::en_Reason_ArenaChallengeCreate);

	int iSceneID = pGlobalCfg->getInt("竞技场场景", 1);
	GSProto::Cmd_Sc_CommFightResult scCommFightResult;
	scCommFightResult.set_isceneid(iSceneID);
	scCommFightResult.set_iissuccess(battleData.bAttackerWin);
	scCommFightResult.set_istar(battleData.iStar);

	GSProto::FightAwardResult* pAwardResult = scCommFightResult.mutable_awardresult();
	{
		GSProto::PropItem* pNewPropItem = pAwardResult->mutable_szawardproplist()->Add();
		pNewPropItem->set_ilifeattid(GSProto::en_LifeAtt_Silver);
		pNewPropItem->set_ivalue(iGetSilver);
	}
	{
		GSProto::PropItem* pNewPropItem = pAwardResult->mutable_szawardproplist()->Add();
		pNewPropItem->set_ilifeattid(GSProto::en_LifeAtt_Honor);
		pNewPropItem->set_ivalue(iGetHonor);
	}

	GSProto::SCMessage pkg;
	HelpMakeScMsg(pkg, GSProto::CMD_COMM_FIGHTRESULT, scCommFightResult);

	IFightSystem* pFightSys = static_cast<IFightSystem*>(pEntity->querySubsystem(IID_IFightSystem));
	assert(pFightSys);

	pFightSys->sendAllBattleMsg(battleData, pkg);
}


void ArenaSystem::onReqArenaFight(const GSProto::CSMessage& msg)
{
	ArenaFactory* pArenaFactory = static_cast<ArenaFactory*>(getComponent<IArenaFactory>("ArenaFactory", IID_IArenaFactory) );
	assert(pArenaFactory);

	GSProto::CMD_ARENA_FIGHT_CS req;
	if(!req.ParseFromString(msg.strmsgbody() ) )
	{
		return;
	}

	if(time(0) < (m_dwLastArenaFightTime + 5) ) return;

	// 判断排名是否合法
	bool bFind = false;
	for(size_t i = 0; i < m_challengeList.size(); i++)
	{
		const GSProto::ArenaPlayer& refPlayer = m_challengeList[i];
		if(refPlayer.irank() == req.irank() )
		{
			bFind = true;
			break;
		}
	}

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	// 验证次数
	/*if(getLeftChallengeCount() == 0)
	{
		pEntity->sendErrorCode(ERROR_ARENA_NEEDCOUNT);
		return;
	}
	*/
	//验证次数是否
	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);
	int iFreeCount =  pGlobalCfg->getInt("竞技场挑战免费次数", 5);
	const ServerEngine::ArenaAppendData& data = m_arenaSaveData.appendData;
	if(data.iNowChallengeCount == iFreeCount)
	{
		pEntity->sendErrorCode(ERROR_MANOR_TIEMS_LIMIT);
		return;
	}
	//验证CD
	IGlobalCfg *pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);
	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	int iCountDown = pGlobal->getInt("竞技场挑战CD", 600);
	int iOverSecond =   pZoneTime->GetCurSecond() - data.iBeginCDSecond;
	if((iOverSecond < iCountDown) && (iOverSecond > 0))
	{	
		pEntity->sendErrorCode(ERROR_ARENA_IN_CD);
		return;
	}
	
	if(!bFind) return;

	m_dwLastArenaFightTime = time(0);
	if(pArenaFactory->fightArenaRank(m_hEntity, req.irank(), ArenaFIghtCb(m_hEntity) ))
	{
		//任务
		{
			EventArgs args;
			args.context.setInt("entity",m_hEntity);
			args.context.setInt("times",1);
			pEntity->getEventServer()->setEvent(EVENT_ENTITY_TASK_ARENA_BATTLE, args);
		}
		
	}
	else
	{
		m_dwLastArenaFightTime = 0;
	}
}

void ArenaSystem::addArenaLog(const GSProto::ArenaLogItem& logItem, bool bActorIsAtt)
{
	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	int iLogCount = pGlobalCfg->getInt("竞技场日志条数", 20);

	string strLog;
	logItem.SerializeToString(&strLog);
	m_arenaSaveData.m_arenaLogList.insert(m_arenaSaveData.m_arenaLogList.begin(), strLog);
	if(m_arenaSaveData.m_arenaLogList.size() > (size_t)iLogCount)
	{
		m_arenaSaveData.m_arenaLogList.resize((size_t)iLogCount);
	}
	if(!bActorIsAtt )
	{
		m_arenaSaveData.bHaveNewLog = true;
	}
	
	checkNotice();
}


