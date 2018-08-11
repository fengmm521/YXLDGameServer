#include "LegionPch.h"
#include "LegionImp.h"
#include "LegionFactory.h"
#include "IJZMessageLayer.h"
#include "IModifyDelegate.h"
#include "BossFactory.h"
#include "LegionSystem.h"
#include "IMailSystem.h"

static HandleManager<ILegion*> g_legionHandleMgr;

ILegion* getLegionFromHandle(HLegion hHandle)
{
	ILegion* pLegion = g_legionHandleMgr.getDataPoint(hHandle);
	
	return pLegion;
}

#define DAYCONDITION_OP(time, timeOp, valueOp)\
	do\
	{\
		IZoneTime* pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);\
		assert(pZoneTime);\
		if(!pZoneTime->IsInSameDay(pZoneTime->GetCurSecond(), time) )\
		{\
			timeOp;\
			valueOp;\
		}\
	}while(0);


GSLegionImp::GSLegionImp():m_iRank(0), m_pLegionBoss(NULL)
{
	ILegion** pPLegion = g_legionHandleMgr.Aquire(m_hHandle);
	*pPLegion = this;
}

GSLegionImp::~GSLegionImp()
{
	g_legionHandleMgr.Release(m_hHandle);

	ITimerComponent* pTimeAxis = getComponent<ITimerComponent>(COMPNAME_TimeAxis, IID_ITimerComponent);
	assert(pTimeAxis);

	pTimeAxis->killTimer(m_saveHandle);
	pTimeAxis->killTimer(m_bossTimerHandle);
	pTimeAxis->killTimer(m_bossUpdateHandle);
}

bool GSLegionImp::initFromDB(const ServerEngine::LegionData& data)
{
	m_legionData = data;

	for(map<std::string, ServerEngine::LegionMemberItem>::const_iterator it = data.memberInfo.memberList.begin(); it != data.memberInfo.memberList.end(); it++)
	{
		m_sortMemberList.push_back(it->first);
	}

	sortMemberRank();

	ITimerComponent* pTimeAxis = getComponent<ITimerComponent>(COMPNAME_TimeAxis, IID_ITimerComponent);
	assert(pTimeAxis);

	IRandom* pRandom = getComponent<IRandom>("Random", IID_IMiniAprRandom);
	assert(pRandom);
	
	m_saveHandle = pTimeAxis->setTimer(this, 1, 300*1000, "LegionSave", (pRandom->random() % 30) * 1000);

	return true;
}

class LegionSaveCallback:public ServerEngine::LegionPrxCallback
{
public:

	LegionSaveCallback(const string& strLegionName):m_strLegionName(strLegionName){}

	virtual void callback_updateLegion(taf::Int32 ret)
	{
		FDLOG("Legion")<<"saver|et|"<<m_strLegionName<<"|"<<ret<<endl;
	}
 
    virtual void callback_updateLegion_exception(taf::Int32 ret)
    {
    	FDLOG("Legion")<<"save|exception|"<<m_strLegionName<<"|"<<ret<<endl;
    }

private:

	string m_strLegionName;
};

void GSLegionImp::saveData(bool bSync)
{
	IJZMessageLayer* pMsgLayer = getComponent<IJZMessageLayer>(COMPNAME_MessageLayer, IID_IJZMessageLayer);
	assert(pMsgLayer);

	string strLegionData = ServerEngine::JceToStr(m_legionData);
	if(!bSync)
	{
		pMsgLayer->AsyncSetLegionData(new LegionSaveCallback(getLegionName() ), getLegionName(), strLegionData);
	}
	else
	{
		pMsgLayer->setLegionData(getLegionName(), strLegionData);
	}
}

void GSLegionImp::onTimer(int nEventId)
{
	if(1 == nEventId)
	{
		saveData(false);
	}
	else if(2 == nEventId)
	{
		procBossTimeout();
	}
	else if(3 == nEventId)
	{
		notifyBossState();
	}
}

void GSLegionImp::notifyBossState()
{
	if(!m_pLegionBoss) return;

	GSProto::CMD_UPDATE_LEGIONBOSS_SC scMsg;
	GSProto::LegionBossUpdate* pBossUpdate = scMsg.mutable_updateinfo();

	vector<BossDamageRecord> rankList;
	m_pLegionBoss->getDamageRankList(10, rankList);
	for(size_t i = 0; i < rankList.size(); i++)
	{
		GSProto::LegionBossRankItem* pNewRankItem = pBossUpdate->mutable_szranklist()->Add();
		pNewRankItem->set_strname(rankList[i].strName);
		pNewRankItem->set_idamagevalue(rankList[i].iDamageValue);
	}
		
	pBossUpdate->set_ibossmaxhp(m_pLegionBoss->getBossMaxHP() );
	pBossUpdate->set_ibosshp(m_pLegionBoss->getBossHP() );

	GSProto::SCMessage pkg;
	HelpMakeScMsg(pkg, GSProto::CMD_UPDATE_LEGIONBOSS, pkg);

	broadcastMessage(pkg);
}


void GSLegionImp::procBossTimeout()
{
	broadcastBossEnd();

	// 清除所有统计信息
	clearBossRecord(false);
}

void GSLegionImp::doBossRankAward()
{
	vector<BossDamageRecord> rankList;
	m_pLegionBoss->getDamageRankList(10, rankList);

	ITable* pBossRankAward = getCompomentObjectManager()->findTable("LegionBossRankAward");
	assert(pBossRankAward);

	IModifyDelegate* pModifyDelegate = getComponent<IModifyDelegate>(COMPNAME_ModifyDelegate, IID_IModifyDelegate);
	assert(pModifyDelegate);

	for(size_t i = 0; i < rankList.size(); i++)
	{
		int iRecord = pBossRankAward->findRecord( (int)i + 1);
		assert(iRecord >= 0);

		int iGiftID = pBossRankAward->getInt(iRecord, "礼包ID");
		pModifyDelegate->submitDelegateTask(rankList[i].roleKey, AddBossGift(rankList[i].roleKey, rankList[i].strName, iGiftID, i+1, true) );
	}
}

void GSLegionImp::doBossKillAward()
{
	IEntity* pKiller = getEntityFromHandle(m_hKiller);
	if(!pKiller) return;

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	int iKillerGiftID = pGlobalCfg->getInt("军团BOSS击杀礼包", 1);

	IMailSystem* pMailSys = static_cast<IMailSystem*>(pKiller->querySubsystem(IID_IMailSystem));
	assert(pMailSys);

	IDropFactory* pDropFactory = getComponent<IDropFactory>(COMPNAME_DropFactory, IID_IDropFactory);
	assert(pDropFactory);

	GSProto::FightAwardResult awardResult;
	pDropFactory->calcDrop(iKillerGiftID, awardResult);

	string strSender = pGlobalCfg->getString("军团奖励发放人", "LegionManager");
	string strTitle = pGlobalCfg->getString("军团BOSS击杀奖励标题", "");
	string strContent = pGlobalCfg->getString("军团BOSS击杀奖励正文", "LegionBoss Kill");

	pMailSys->addMail(strSender, strTitle, strContent,awardResult, "LegionBossKill");

	// BOSS成长处理
	Uint32 dwKillCostTime = time(0) - m_legionData.bossInfo.dwLastBossTime;
	Uint32 dwCostMin = dwKillCostTime/60;

	string strLegonBossGrp = pGlobalCfg->getString("军团BOSS成长规则", "3#2#5#1");
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

	FDLOG("Legion")<<"BossGrow|"<<dwCostMin<<"|"<<iGrowLv<<endl;
	m_legionData.bossInfo.iBossLevel += iGrowLv;
}

void GSLegionImp::clearBossRecord(bool bKilled)
{
	// 发放奖励
	if(bKilled)
	{
		doBossRankAward();
		doBossKillAward();
	}
	
	IBossFactory* pBossFactory = getComponent<IBossFactory>("BossFactory", IID_IBossFactory);
	assert(pBossFactory);

	pBossFactory->delBoss(m_pLegionBoss);
	m_pLegionBoss = NULL;

	m_mapBossState.clear();
	
	// 关闭BOSS定时器
	ITimerComponent* pTimeAxis = getComponent<ITimerComponent>(COMPNAME_TimeAxis, IID_ITimerComponent);
	assert(pTimeAxis);

	pTimeAxis->killTimer(m_bossTimerHandle);
	pTimeAxis->killTimer(m_bossUpdateHandle);

	// 清除击杀者
	m_hKiller = 0;
}

struct LegionMemCmp
{
	LegionMemCmp(GSLegionImp* pLegion):m_pLegion(pLegion){}

	bool operator () (const string& strFirstName, const string& strSecondName) const
	{
		const ServerEngine::LegionMemberItem* pFirstInfo = m_pLegion->getMemberInfo(strFirstName);
		const ServerEngine::LegionMemberItem* pSecondInfo = m_pLegion->getMemberInfo(strSecondName);

		assert(pFirstInfo && pSecondInfo);
		//增加 排序

		/*if(pFirstInfo->iOccupation < pSecondInfo->iOccupation)
		{
			return true;
		}
		
		if(pFirstInfo->iOccupation > pSecondInfo->iOccupation)
		{
			return false;
		}*/

		//相同比较其他

		if(pFirstInfo->iSumContribute != pSecondInfo->iSumContribute)
		{
			return pFirstInfo->iSumContribute > pSecondInfo->iSumContribute;
		}

		return strFirstName > strSecondName;
	}
private:

	GSLegionImp* m_pLegion;
};

void GSLegionImp::sortMemberRank()
{
	m_sortMemberList.clear();
	
	const map<std::string, ServerEngine::LegionMemberItem>& memMap = m_legionData.memberInfo.memberList;
	for(map<std::string, ServerEngine::LegionMemberItem>::const_iterator it = memMap.begin(); it != memMap.end(); it++)
	{
		m_sortMemberList.push_back(it->first);
	}

	// 剩余排序
	std::sort(m_sortMemberList.begin(), m_sortMemberList.end(), LegionMemCmp(this) );
}

const ServerEngine::LegionMemberItem* GSLegionImp::getMemberInfo(const string& strMemName)
{
	const map<std::string, ServerEngine::LegionMemberItem>& memMap = m_legionData.memberInfo.memberList;
	map<std::string, ServerEngine::LegionMemberItem>::const_iterator it = memMap.find(strMemName);
	if(it != memMap.end() )
	{
		return &(it->second);
	}

	return NULL;
}

int GSLegionImp::getMemberCount()
{
	return (int)m_legionData.memberInfo.memberList.size();
}


bool GSLegionImp::isApplyed(const string& strActorName)
{
	return m_legionData.appInfo.applyList.find(strActorName) != m_legionData.appInfo.applyList.end();
}

int GSLegionImp::getLegionIconId()
{
	return m_legionData.baseInfo.iLegionIconId;
}


Uint32 GSLegionImp::getHandle()
{
	return m_hHandle;
}

string GSLegionImp::getLegionName()
{
	return m_legionData.baseInfo.strLegionName;
}

void GSLegionImp::updateActorInfo(HEntity hActor)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

	string strActorName = pActor->getProperty(PROP_ENTITY_NAME, "");

	ServerEngine::LegionMemberItem* pMemInfo = (ServerEngine::LegionMemberItem*)getMemberInfo(strActorName);
	if(!pMemInfo) return;
	pMemInfo->iLevel = pActor->getProperty(PROP_ENTITY_LEVEL, 0);

	pMemInfo->iHeadIcon = pActor->getProperty(PROP_ENTITY_ACTOR_HEAD, 0);

	pMemInfo->iFightValue =  pActor->getProperty(PROP_ENTITY_FIGHTVALUE, 0);
}

void GSLegionImp::actorLoginOut(HEntity hActor,bool bIsOut)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

	string strActorName = pActor->getProperty(PROP_ENTITY_NAME, "");

	ServerEngine::LegionMemberItem* pMemInfo = (ServerEngine::LegionMemberItem*)getMemberInfo(strActorName);
	if(!pMemInfo) return;
	if(bIsOut)
	{
		pMemInfo->bIsOutLine = true;
		pMemInfo->dwLogionOutTime = time(0);
	}
	else
	{
		pMemInfo->bIsOutLine = false;
		pMemInfo->dwLogionOutTime = time(0);
	}
}

void GSLegionImp::updateActorLevelInfo(HEntity hActor)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

	string strActorName = pActor->getProperty(PROP_ENTITY_NAME, "");

	ServerEngine::LegionMemberItem* pMemInfo = (ServerEngine::LegionMemberItem*)getMemberInfo(strActorName);
	if(!pMemInfo) return;

	pMemInfo->iLevel = pActor->getProperty(PROP_ENTITY_LEVEL, 0);
}


void GSLegionImp::onMessage(HEntity hActor, const GSProto::CSMessage& msg)
{
	switch(msg.icmd() )
	{
		case GSProto::CMD_QUERY_LEGION_BASE:
			onReqLegionBase(hActor, msg);
			break;
			
		case GSProto::CMD_QUERY_LEGION_MEMBERLIST:
			onReqLegionMemList(hActor, msg);
			break;
			
		case GSProto::CMD_QUERY_LEGION_APP:
			onReqQueryAppList(hActor, msg);
			break;
			
		case GSProto::CMD_MODIFY_LEGION_SETTING:
			onReqModifySetting(hActor, msg);
			break;
			
		case GSProto::CMD_DEAL_LEGIONAPP:
			onReqDealLegionApp(hActor, msg);
			break;
			
		case GSProto::CMD_DISMISS_LEGION:
			onReqDisssmissLegion(hActor, msg);
			break;
			
		case GSProto::CMD_KICK_LEGIONMEMBER:
			onReqKickMember(hActor, msg);
			break;
			
		case GSProto::CMD_TRANSFER_LEGIONLEADER:
			onReqTransLeader(hActor, msg);
			break;
		
		case GSProto::CMD_QUERY_LELGION_CONTRIBUTE:
			onReqQueryContribute(hActor, msg);
			break;
			
		case GSProto::CMD_LEGION_CONTRIBUTE:
			onReqContribute(hActor, msg);
			break;
			
		case GSProto::CMD_LEGION_BLESS:
			onReqBless(hActor, msg);
			break;
			
		case GSProto::CMD_QUERY_LEGIONPAY:
			onReqQueryPay(hActor, msg);
			break;
			
		case GSProto::CMD_GET_LEGIONPAY:
			onReqGetPay(hActor, msg);
			break;

		case GSProto::CMD_QUERY_LEGIONBOSS:
			onReqQueryBoss(hActor, msg);
			break;
			
		case GSProto::CMD_CALL_LEGIONBOSS:
			onReqCallBoss(hActor, msg);
			break;
			
		case GSProto::CMD_CHEER_LEGIONBOSS:
			onReqCheerLegionBoss(hActor, msg);
			break;
			
		case GSProto::CMD_FIGHT_LEGIONBOSS:
			onReqFightLegionBoss(hActor, msg);
			break;
			
		case GSProto::CMD_LEGIONBOSS_RELIVE:
			onReqRelive(hActor, msg);
			break;

		case GSProto::CMD_DEAL_LEGIONAPP_ONEKEY:
			onReqDealLegionAppOneKey(hActor, msg);
			break;

		case GSProto::CMD_LEGION_APPOINT:
			onLegionAppoint(hActor, msg);
			break;

		case GSProto::CMD_LEGION_QUERY_WORSHIP:
			onQueryWorship(hActor, msg);
			break;
			
		case GSProto::CMD_LEGION_WORSHIP_MEMBER:
			onWorshipMember(hActor, msg);
			break;
			
		case GSProto::CMD_LEGION_GET_WORSHIP_REWARD:
			onGetWorshipReward(hActor, msg);
			break;

		//新军团领地战开始
		case GSProto::CMD_QUERY_LEGION_CAMPBATTLE:
			onReqQueryBattle(hActor, msg);
			break;
			
		case GSProto::CMD_LEGION_CAMPBATTLE_REPORT:
			onReqReportLegionBattle(hActor, msg);
			break;
			
		case GSProto::CMD_LEGION_CAMPBATTLE_JOIN:
			onRepJoinLegionBattle(hActor, msg);
			break;
			
		case GSProto::CMD_LEGION_CAMPBATTLE_ENTER:
			onReqEnterLegionBattle(hActor, msg);
			break;
			
		case GSProto::CMD_LEGION_CAMPBATTLE_BATTLE:
			 onReqCampMemberBattle(hActor, msg);
			break;
		
		case GSProto::CMD_QUERY_CAMPBATTLE_LASTFIGHT_SIMPLELOG:
			 onQueryLastFightSimpleLog(hActor, msg);
			break;
			
		case GSProto::CMD_QUERY_CAMPBATTLE_LASTFIGHT_DETAIL_LOG:
			onQueryLastFightDetailLog(hActor, msg);
			break;
			
		case GSProto::CMD_QUERY_CUR_FIGHT_LEGION_LOG:
			 onQueryCurFightLog(hActor, msg);
			break;
			
		case GSProto::CMD_QUERY_CUR_FIGHT_CONDITION:
			onQueryCurFightCondition(hActor, msg);
			break;
			
		case GSProto::CMD_QUERY_CAMPBATTLE_LASTFIGHT_LOG:
			onQueryLastFightLog(hActor, msg);
			break;
			
		case GSProto::CMD_QUERY_CAMPBATTLE_INFO:
			onQueryCampBattleInfo(hActor, msg);
			break;
	}
}

void GSLegionImp::onQueryLastFightDetailLog(HEntity hActor, const GSProto::CSMessage& msg)
{
	if(!isLegionBattleEnable() )
	{
		return;
	}
	GSProto::CMD_QUERY_CAMPBATTLE_LASTFIGHT_DETAIL_LOG_CS csMsg;
	if(!csMsg.ParseFromString(msg.strmsgbody()))
	{
		return;
	}
	
	LegionFactory* pLegionFactory = static_cast<LegionFactory*>(getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory) );
	assert(pLegionFactory);
	
	string strAttLegionName = csMsg.strattlegionname();
	string actorLegionName = getLegionName();
	
	pLegionFactory->onQueryLastFightDetailLog(hActor,strAttLegionName, csMsg.icampid(), actorLegionName);
}

void GSLegionImp::onQueryCurFightLog(HEntity hActor, const GSProto::CSMessage& msg)
{
	if(!isLegionBattleEnable() )
	{
		return;
	}
	GSProto::CMD_QUERY_CUR_FIGHT_LEGION_LOG_CS csMsg;
	if(!csMsg.ParseFromString(msg.strmsgbody()))
	{
		return;
	}
	
	LegionFactory* pLegionFactory = static_cast<LegionFactory*>(getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory) );
	assert(pLegionFactory);
	
	string strAttLegionName = csMsg.strattlegionname();
	string actorLegionName = getLegionName();
	
	pLegionFactory->onQuerycurFightDetailLog(hActor,strAttLegionName, csMsg.icampid(), actorLegionName);
	
}

void GSLegionImp::onQueryCurFightCondition(HEntity hActor, const GSProto::CSMessage& msg)	
{
	if(!isLegionBattleEnable() )
	{
		return;
	}
	GSProto::CMD_QUERY_CUR_FIGHT_CONDITION_CS csMsg;
	if(!csMsg.ParseFromString(msg.strmsgbody()))
	{
		return;
	}
	
	LegionFactory* pLegionFactory = static_cast<LegionFactory*>(getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory) );
	assert(pLegionFactory);
	
	pLegionFactory->onQueryCurFightCondition(hActor,csMsg.icampid());
}

void GSLegionImp::onQueryLastFightSimpleLog(HEntity hActor, const GSProto::CSMessage& msg)
{
	if(!isLegionBattleEnable() )
	{
		return;
	}
	GSProto::CMD_QUERY_CAMPBATTLE_LASTFIGHT_SIMPLELOG_CS csMsg;
	if(!csMsg.ParseFromString(msg.strmsgbody()))
	{
		return;
	}
	
	LegionFactory* pLegionFactory = static_cast<LegionFactory*>(getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory) );
	assert(pLegionFactory);

	pLegionFactory->onQueryLastFightSimpleLog(hActor, getLegionName(),csMsg.icampid());
	
}

void GSLegionImp::onQueryLastFightLog(HEntity hActor, const GSProto::CSMessage& msg)
{
	if(!isLegionBattleEnable() )
	{
		return;
	}

	LegionFactory* pLegionFactory = static_cast<LegionFactory*>(getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory) );
	assert(pLegionFactory);

	pLegionFactory->onQueryLastFightLog(hActor, getLegionName());
	
}


void GSLegionImp::onQueryWorship(HEntity hActor, const GSProto::CSMessage& msg)
{
	pushWorshipInfo(hActor);
}

void GSLegionImp::pushWorshipInfo(HEntity hActor)
{
	IEntity *pActor = getEntityFromHandle(hActor);
	if(!pActor) return;
	ServerEngine::LegionMemberItem* pLegionItem = getMemberInfoByName(pActor->getProperty( PROP_ENTITY_NAME, ""));
	if(!pLegionItem) return;

	IVIPFactory *pVIPFact = getComponent<IVIPFactory>(COMPNAME_VIPFactory, IID_IVIPFactory);
	assert(pVIPFact);
	
	int worshipTimesLimit = pVIPFact->getVipPropByHEntity(hActor, VIP_PROP_MOBAI_TIMES);

	LegionSystem *pLegionSys = static_cast<LegionSystem*>(pActor->querySubsystem(IID_ILegionSystem));
	assert(pLegionSys);

	int iActorHaveUse = pLegionSys->getWorshipTimes().iValue;
	
	int iRemaindWorshipTimes = worshipTimesLimit - iActorHaveUse;

	int iCurContribute = pActor->getProperty(PROP_ACTOR_LEGION_CONTRIBUTE, 0);
	
	GSProto::CMD_LEGION_QUERY_WORSHIP_SC scMsg;

	scMsg.set_iremainworshiptimes(iRemaindWorshipTimes);
	scMsg.set_itotalworshiptimes(worshipTimesLimit);
	scMsg.set_icangetworshipreward(pLegionItem->beWorshipGetContribute);
	scMsg.set_iactorcontribute( iCurContribute );
	
	pActor->sendMessage(GSProto::CMD_LEGION_QUERY_WORSHIP,  scMsg);
}


int GSLegionImp::getActorRemaindWorshipTimes(HEntity hActor)
{
	IEntity *pActor = getEntityFromHandle(hActor);
	if(!pActor) return -1 ;
	
	ServerEngine::LegionMemberItem* pLegionItem = getMemberInfoByName(pActor->getProperty( PROP_ENTITY_NAME, ""));
	if(!pLegionItem) return -1 ;
	
	IVIPFactory *pVIPFact = getComponent<IVIPFactory>(COMPNAME_VIPFactory, IID_IVIPFactory);
	assert(pVIPFact);
	int worshipTimesLimit = pVIPFact->getVipPropByHEntity(hActor, VIP_PROP_MOBAI_TIMES);

	LegionSystem *pLegionSys = static_cast<LegionSystem*>(pActor->querySubsystem(IID_ILegionSystem));
	assert(pLegionSys);

	int iActorHaveUse = pLegionSys->getWorshipTimes().iValue;
	
	int iRemaindWorshipTimes = worshipTimesLimit - iActorHaveUse;
	
	return iRemaindWorshipTimes;
}

void GSLegionImp::onWorshipMember(HEntity hActor, const GSProto::CSMessage& msg)
{
	IGlobalCfg *pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);

	GSProto::CMD_LEGION_WORSHIP_MEMBER_CS csMsg;
	if(! csMsg.ParseFromString(msg.strmsgbody()))
	{
		return;
	}

	//膜拜次数不足
	IEntity *pActor = getEntityFromHandle( hActor);
	if( !pActor ) return;
	
	if(getActorRemaindWorshipTimes(hActor) <= 0)
	{
		pActor->sendErrorCode(ERROR_LEGION_WORSHIP_TIMES);
		return;
	}
	
	ServerEngine::LegionMemberItem *pActorItem = getMemberInfoByName(pActor->getProperty( PROP_ENTITY_NAME,""));
	if(!pActorItem) return;
	

	string strWorshipMemName = csMsg.strmembername();
	unsigned int iworshipType = csMsg.worshiptype();

	if( ! bMemberCanWorship(hActor,strWorshipMemName))
	{
		return;
	}

	
	LegionSystem *pLegionSys = static_cast<LegionSystem*>(pActor->querySubsystem(IID_ILegionSystem));
	assert(pLegionSys);


	ServerEngine::LegionMemberItem *pMemItem = getMemberInfoByName(strWorshipMemName);
	if(! pMemItem) return;

	if(iworshipType == GSProto::enWorshipType_Free)
	{
		string strConfig = pGlobal->getString("免费膜拜", "0#3#10");
		vector<int> vecConfig = TC_Common::sepstr<int>(strConfig, "#");
		assert(vecConfig.size() == 3);
		assert(vecConfig[1] >= 0);
		
		int iGetPhyStrength = vecConfig[1];
		int iGetContribute = vecConfig[2];
		assert(iGetContribute >0);

		//getDayResetValue(pActorItem->useWorshipTimes).iValue += 1;

		pLegionSys->getWorshipTimes().iValue ++;
		map<string, unsigned int>& haveWorshipList = pLegionSys->getActorHaveWorshipList();
		
		haveWorshipList.insert(make_pair(strWorshipMemName, time(0)));
		
		pActor->changeProperty(PROP_ENTITY_PHYSTRENGTH, iGetPhyStrength, GSProto::en_Reason_Legin_Worship_GetPhyStrength);
		PLAYERLOG(pActor)<<"Worship Get PhyStrength|"<< iGetPhyStrength<<"|"<<GSProto::en_Reason_Legin_Worship_GetPhyStrength<<endl;

		pMemItem->beWorshipGetContribute += iGetContribute;

		
	}
	else if(iworshipType == GSProto::enWorshipType_UseSilver)
	{
		string strConfig = pGlobal->getString("铜币膜拜", "30000#6#100");
		vector<int> vecConfig = TC_Common::sepstr<int>(strConfig, "#");
		assert(vecConfig.size() == 3);
		int iNeedSilver = vecConfig[0];
		int iGetPhyStrength = vecConfig[1];
		int iGetContribute = vecConfig[2];
		
		assert(iNeedSilver > 0);
		assert(iGetPhyStrength > 0);
		assert(iGetContribute >0);

		int iActorHaveSilver = pActor->getProperty(PROP_ACTOR_SILVER,0);
		if(iActorHaveSilver < iNeedSilver)
		{
			pActor->sendErrorCode(ERROR_NEED_SILVER);
			return;
		}

		//getDayResetValue(pActorItem->useWorshipTimes).iValue += 1;
		pLegionSys->getWorshipTimes().iValue ++;
		
		map<string, unsigned int>& haveWorshipList = pLegionSys->getActorHaveWorshipList();
		
		haveWorshipList.insert(make_pair(strWorshipMemName, time(0)));
		
		
		pActor->changeProperty( PROP_ACTOR_SILVER, 0- iNeedSilver, GSProto::en_Reason_Legin_Worship);
		
		pActor->changeProperty(PROP_ENTITY_PHYSTRENGTH, iGetPhyStrength, GSProto::en_Reason_Legin_Worship_GetPhyStrength);
		PLAYERLOG(pActor)<<"Worship Get PhyStrength|"<< iGetPhyStrength<<"|"<<GSProto::en_Reason_Legin_Worship_GetPhyStrength<<endl;

		pMemItem->beWorshipGetContribute += iGetContribute;
	}
	else if(iworshipType == GSProto::enWorshipType_UseGold)
	{	
		string strConfig = pGlobal->getString("元宝膜拜", "100#12#1000");
		vector<int> vecConfig = TC_Common::sepstr<int>(strConfig, "#");
		assert(vecConfig.size() == 3);
		
		int iNeedGold = vecConfig[0];
		int iGetPhyStrength = vecConfig[1];
		int iGetContribute = vecConfig[2];
		
		assert(iNeedGold > 0);
		assert(iGetPhyStrength > 0);
		assert(iGetContribute >0);

		int iActorHaveGold= pActor->getProperty(PROP_ACTOR_GOLD,0);
		if(iActorHaveGold < iNeedGold)
		{
			pActor->sendErrorCode(ERROR_NEED_GOLD);
			return;
		}

		//getDayResetValue((*pActorItem).useWorshipTimes).iValue += 1;
		pLegionSys->getWorshipTimes().iValue ++;
		
		map<string, unsigned int>& haveWorshipList = pLegionSys->getActorHaveWorshipList();
		haveWorshipList.insert(make_pair(strWorshipMemName, time(0)));

		
		
		pActor->changeProperty( PROP_ACTOR_GOLD, 0- iNeedGold, GSProto::en_Reason_Legin_Worship);
		
		pActor->changeProperty(PROP_ENTITY_PHYSTRENGTH, iGetPhyStrength, GSProto::en_Reason_Legin_Worship_GetPhyStrength);
		PLAYERLOG(pActor)<<"Worship Get PhyStrength|"<< iGetPhyStrength<<"|"<<GSProto::en_Reason_Legin_Worship_GetPhyStrength<<endl;

		pMemItem->beWorshipGetContribute += iGetContribute;
		
	}

	pActor->sendMessage(GSProto::CMD_LEGION_WORSHIP_MEMBER);
	
}

bool GSLegionImp::bMemberCanWorship(HEntity hActor, const string& strMemberName)
{
	IEntity *pActor = getEntityFromHandle(hActor);
	if(!pActor) return false;

	int iActorLevel = pActor->getProperty( PROP_ENTITY_LEVEL, 1);
	string strActorName = pActor->getProperty( PROP_ENTITY_NAME,"");
	if(strActorName == strMemberName)
	{
		return false;
	}
	
	ServerEngine::LegionMemberItem* pLegionItem = getMemberInfoByName(strMemberName);
	if(!pLegionItem) return false;

	if(pLegionItem->iLevel >= iActorLevel) 
	{
		LegionSystem *pLegionSys = static_cast<LegionSystem*>(pActor->querySubsystem(IID_ILegionSystem));
		assert(pLegionSys);
		map<string, unsigned int> haveWorshipList = pLegionSys->getActorHaveWorshipList();

		map<string, unsigned int>::iterator iter = haveWorshipList.find(strMemberName);
		if(iter == haveWorshipList.end())
		{
			return true;
		}
	}
		

	return false;
	
}


void GSLegionImp::onGetWorshipReward(HEntity hActor, const GSProto::CSMessage& msg)
{
	IEntity *pActor = getEntityFromHandle(hActor);
	if(!pActor) return;
	
	ServerEngine::LegionMemberItem *pActorItem = getMemberInfoByName(pActor->getProperty( PROP_ENTITY_NAME,""));
	if(!pActorItem) return;

	if(pActorItem->beWorshipGetContribute == 0) return;

	addMemberContribyte(hActor,pActorItem->beWorshipGetContribute);
	pActorItem->beWorshipGetContribute = 0;
	
	pushWorshipInfo(hActor);
}

void GSLegionImp::onLegionAppoint(HEntity hActor, const GSProto::CSMessage& msg)
{	
	GSProto::CMD_LEGION_APPOINT_CS csMsg;
	if(!csMsg.ParseFromString(msg.strmsgbody()))
	{
		return;
	}

	IEntity *pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

	//只有军团长有权利
	int iOccupation = getOccupation(pActor->getProperty(PROP_ENTITY_NAME, "") );
	if(iOccupation != GSProto::en_LegionOccupation_Leader) return;

	string strMemberName = csMsg.strlegionmemname();
	bool bAppoint = csMsg.bisappoint();
	
	//ServerEngine::LegionMemberItem* pLegionMemItem = getMemberInfo(strMemberName);

	
	map<std::string, ServerEngine::LegionMemberItem>::iterator it = m_legionData.memberInfo.memberList.find(strMemberName);
	if(it == m_legionData.memberInfo.memberList.end() )
	{
		return;
	}
	
	ServerEngine::LegionMemberItem& pLegionMemItem = it->second;

	if(bAppoint)
	{
		if(pLegionMemItem.iOccupation == GSProto::en_LegionOccupation_Member)
		{
			
			const LegionLevelInfo* pLevelInfo = getCurLevelInfo();
			assert(pLevelInfo);

			int iCount = getViceCount();
			int iLimitCount = getViceMemberLimit();
			if(iCount >= iLimitCount)
			{
				pActor->sendErrorCode(ERROR_LEGION_VICE_LIMIT);
				return;
			}
			
			pLegionMemItem.iOccupation = GSProto::en_LegionOccupation_ViceLeader;

			pActor->sendMessage(GSProto::CMD_LEGION_APPOINT);

			PLAYERLOG(pActor)<<"Appoint|"<<strMemberName<<endl;
		}
		
	}
	else
	{
		if(pLegionMemItem.iOccupation == GSProto::en_LegionOccupation_ViceLeader)
		{
			pLegionMemItem.iOccupation = GSProto::en_LegionOccupation_Member;

			pActor->sendMessage(GSProto::CMD_LEGION_APPOINT);
			
			PLAYERLOG(pActor)<<"Dis Appoint|"<<strMemberName<<endl;
		}
	}
}

void GSLegionImp::onReqDealLegionAppOneKey(HEntity hActor, const GSProto::CSMessage& msg)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

	int iOccupation = getOccupation(pActor->getProperty(PROP_ENTITY_NAME, "") );
	if(iOccupation > GSProto::en_LegionOccupation_ViceLeader)
	{
		pActor->sendErrorCode(ERROR_NO_RIGHT);
		return;
	}
	
	m_legionData.appInfo.applyList.clear();
	pActor->sendMessage(GSProto::CMD_DEAL_LEGIONAPP_ONEKEY);
}


void GSLegionImp::onReqLastBattleSelfBattleRecord(HEntity hActor, const GSProto::CSMessage& msg)
{
	GSProto::CMD_QUERY_LASTBATTLE_SELFRECORD_CS req;
	if(!req.ParseFromString(msg.strmsgbody() ) )
	{
		return;
	}

	LegionFactory* pLegionFactory = static_cast<LegionFactory*>(getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory) );
	assert(pLegionFactory);

	pLegionFactory->sendHisSelfBattleRecord(hActor, req.icityid() );
}


void GSLegionImp::onReqLastBattleRecordPage(HEntity hActor, const GSProto::CSMessage& msg)
{
	GSProto::CMD_QUERY_LASTBATTLE_RECORD_PAGE_CS req;
	if(!req.ParseFromString(msg.strmsgbody() ) )
	{
		return;
	}

	LegionFactory* pLegionFactory = static_cast<LegionFactory*>(getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory) );
	assert(pLegionFactory);

	pLegionFactory->sendHisBattleRecord(hActor, req.icityid(), req.ipage() );
}

void GSLegionImp::onReqLastBattleHonorRank(HEntity hActor, const GSProto::CSMessage& msg)
{
	GSProto::CMD_QUERY_LASTBATTLE_HONORRANK_CS req;
	if(!req.ParseFromString(msg.strmsgbody() ) )
	{
		return;
	}

	LegionFactory* pLegionFactory = static_cast<LegionFactory*>(getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory) );
	assert(pLegionFactory);

	pLegionFactory->sendHisHonorRank(hActor, req.icityid() );
}


void GSLegionImp::onReqCloseCurBattlePage(HEntity hActor, const GSProto::CSMessage& msg)
{
	// nothing
}


void GSLegionImp::onReqQueryCurHonorRank(HEntity hActor, const GSProto::CSMessage& msg)
{
	LegionFactory* pLegionFactory = static_cast<LegionFactory*>(getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory) );
	assert(pLegionFactory);

	if(!isLegionBattleEnable() )
	{
		return;
	}

	pLegionFactory->sendCurHonorRank(hActor);
}


void GSLegionImp::onReqQueryCurBattleSelf(HEntity hActor, const GSProto::CSMessage& msg)
{
	LegionFactory* pLegionFactory = static_cast<LegionFactory*>(getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory) );
	assert(pLegionFactory);

	if(!isLegionBattleEnable() )
	{
		return;
	}

	pLegionFactory->sendSelfBattleRecord(hActor);
}


void GSLegionImp::onReqQueryCurBattlePage(HEntity hActor, const GSProto::CSMessage& msg)
{
	LegionFactory* pLegionFactory = static_cast<LegionFactory*>(getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory) );
	assert(pLegionFactory);

	if(!isLegionBattleEnable() )
	{
		return;
	}

	pLegionFactory->sendCurBattlePage(hActor);
}


void GSLegionImp::onReqQueryLegionReport(HEntity hActor, const GSProto::CSMessage& msg)
{
	if(!isLegionBattleEnable() )
	{
		return;
	}

	GSProto::CMD_QUERY_LEGIONBATTLE_REPORT_CS reqMsg;
	if(!reqMsg.ParseFromString(msg.strmsgbody() ) )
	{
		return;
	}

	int iCityID = reqMsg.iquerycityid();

	LegionFactory* pLegionFactory = static_cast<LegionFactory*>(getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory) );
	assert(pLegionFactory);

	pLegionFactory->sendQueryLegionReportInfo(hActor, iCityID);
}


void GSLegionImp::onReqReportLegionBattle(HEntity hActor, const GSProto::CSMessage& msg)
{
	if(!isLegionBattleEnable() )
	{
		return;
	}

	GSProto::CMD_LEGION_CAMPBATTLE_REPORT_CS req;
	if(!req.ParseFromString(msg.strmsgbody() ) )
	{
		return;
	}

	LegionFactory* pLegionFactory = static_cast<LegionFactory*>(getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory) );
	assert(pLegionFactory);

	pLegionFactory->reportFightCity(hActor, req.iid() );
}

void GSLegionImp::onQueryCampBattleInfo(HEntity hActor, const GSProto::CSMessage& msg)
{
	if(!isLegionBattleEnable() )
	{
		return;
	}

	GSProto::CMD_LEGION_CAMPBATTLE_REPORT_CS req;
	if(!req.ParseFromString(msg.strmsgbody() ) )
	{
		return;
	}

	LegionFactory* pLegionFactory = static_cast<LegionFactory*>(getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory) );
	assert(pLegionFactory);

	pLegionFactory->onQueryCampBattleInfo(hActor, req.iid() );
}




void GSLegionImp::onRepJoinLegionBattle(HEntity hActor, const GSProto::CSMessage& msg)
{
	if(!isLegionBattleEnable() )
	{
		return;
	}

	LegionFactory* pLegionFactory = static_cast<LegionFactory*>(getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory) );
	assert(pLegionFactory);

	GSProto::CMD_LEGION_CAMPBATTLE_JOIN_CS req;
	if(!req.ParseFromString(msg.strmsgbody() ) )
	{
		return;
	}

	pLegionFactory->onJoinCampBattle(hActor,req.iid());
}

void GSLegionImp::onReqEnterLegionBattle(HEntity hActor, const GSProto::CSMessage& msg)
{
	if(!isLegionBattleEnable() )
	{
		return;
	}

	LegionFactory* pLegionFactory = static_cast<LegionFactory*>(getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory) );
	assert(pLegionFactory);

	GSProto::CMD_LEGION_CAMPBATTLE_ENTER_CS req;
	if(!req.ParseFromString(msg.strmsgbody() ) )
	{
		return;
	}

	pLegionFactory->onEnterCampBattle(hActor,req.icampid(),req.strvslegionname());
}

void GSLegionImp::onReqCampMemberBattle(HEntity hActor, const GSProto::CSMessage& msg)
{
	if(!isLegionBattleEnable() )
	{
		return;
	}
	
	LegionFactory* pLegionFactory = static_cast<LegionFactory*>(getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory) );
	assert(pLegionFactory);

	GSProto::CMD_LEGION_CAMPBATTLE_BATTLE_CS req;
	if(!req.ParseFromString(msg.strmsgbody() ) )
	{
		return;
	}
	pLegionFactory->onCampMemberBattle(hActor,req.icampid(),req.ibattleid(),req.strmemname(),req.istar());
}



void GSLegionImp::onReqQuerySingleLastBattle(HEntity hActor, const GSProto::CSMessage& msg)
{

}


void GSLegionImp::onReqQueryLastBattleList(HEntity hActor, const GSProto::CSMessage& msg)
{
}


bool GSLegionImp::isLegionBattleEnable()
{
	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	int iCondLevel = pGlobalCfg->getInt("领地争夺战开启等级", 3);

	return getLevel() >= iCondLevel;
}



void GSLegionImp::onReqQueryBattle(HEntity hActor, const GSProto::CSMessage& msg)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

	if(!isLegionBattleEnable() )
	{
		pActor->sendErrorCode(ERROR_LEGIONFIGHT_LEVEL);
		return;
	}

	LegionFactory* pLegionFactory = static_cast<LegionFactory*>(getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory) );
	assert(pLegionFactory);

	pLegionFactory->sendQueryBattleMsg(hActor);
}

int GSLegionImp::getCallBossCD()
{
	IZoneTime* pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	string strFightDays = pGlobalCfg->getString("军团BOSS召唤日", "1#3#5#7");
	vector<int> dayList =  TC_Common::sepstr<int>(strFightDays, "#");
	assert(dayList.size() > 0);

	int iDayInWeek = pZoneTime->GetDayInWeek(pZoneTime->GetCurSecond() );
	Uint32 distanceDay = 0;
	
	// 如果今天就是可以召唤的天，看看今天是否召唤了,否则找到下一个可召唤的天
	vector<int>::iterator it = find(dayList.begin(), dayList.end(), iDayInWeek);
	if(it != dayList.end() )
	{
		if(!pZoneTime->IsInSameDay(m_legionData.bossInfo.dwLastBossTime, pZoneTime->GetCurSecond() ) )
		{
			return 0;
		}
		
		if(*it != dayList.back() )
		{
			distanceDay = *(it+1) - iDayInWeek;
		}
		else
		{
			distanceDay = 7 + dayList.front() - iDayInWeek;
		}
	}
	else
	{
		it = std::upper_bound(dayList.begin(), dayList.end(), iDayInWeek);
		if(it == dayList.end() )
		{
			distanceDay = 7 + dayList.front() - iDayInWeek;
		}
		else
		{
			distanceDay = *it - iDayInWeek;
		}
	}

	Uint32 dwFirstSecondToday = pZoneTime->GetFirstSecondsOnSameDay(pZoneTime->GetCurSecond() );
	int iLeftCDSecond = (Uint32) ((double)distanceDay * 24*3600 - (pZoneTime->GetCurSecond() - dwFirstSecondToday));

	return iLeftCDSecond;
}

int GSLegionImp::getBossMaxHP()
{
	if(m_legionData.bossInfo.iBossLevel == 0)
	{
		m_legionData.bossInfo.iBossLevel = 1;
	}

	int iBossLevel = m_legionData.bossInfo.iBossLevel;

	ITable* pBossLvTb = getCompomentObjectManager()->findTable("LegionBoss");
	assert(pBossLvTb);

	int iRecord = pBossLvTb->findRecord(iBossLevel);
	assert(iRecord >= 0);

	int iMaxHP = pBossLvTb->getInt(iRecord, "MAXHP");

	return iMaxHP;
}

int GSLegionImp::getLeftKillBossTime()
{
	if(!m_pLegionBoss) return 0;

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	int iBossTime = pGlobalCfg->getInt("军团BOSS持续时间", 600);
	if(time(0) > (m_legionData.bossInfo.dwLastBossTime + (Uint32)iBossTime) )
	{
		return 0;
	}

	return m_legionData.bossInfo.dwLastBossTime + (Uint32)iBossTime - time(0);
}

int GSLegionImp::getActorCheerCount(HEntity hActor)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return 0;

	string strActorName = pActor->getProperty(PROP_ENTITY_NAME, "");
	map<string, MemberLegionBossState>::iterator it = m_mapBossState.find(strActorName);
	if(it == m_mapBossState.end() )
	{
		return 0;
	}

	return it->second.iCheerCount;
}


MemberLegionBossState* GSLegionImp::getMemberBossState(const string& strName)
{
	map<string, MemberLegionBossState>::iterator it = m_mapBossState.find(strName);
	if(it == m_mapBossState.end() )
	{
		return &m_mapBossState[strName];
	}

	return &(it->second);
}

int GSLegionImp::getBossMemReliveCD(const string& strName)
{
	const MemberLegionBossState* pMemState = getMemberBossState(strName);
	if(!pMemState) return 0;

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	int iReliveTime = pGlobalCfg->getInt("军团BOSS复活时间", 120);

	if( time(0) >= (pMemState->dwLastDeadTime + (Uint32)iReliveTime) )
	{
		return 0;
	}

	return (pMemState->dwLastDeadTime + (Uint32)iReliveTime) - time(0);
}



void GSLegionImp::fillScBossInfo(HEntity hActor, GSProto::LegionBossFightInfo* pBossFightInfo)
{
	if(!m_pLegionBoss) return;

	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;
	
	pBossFightInfo->set_ileftkillsecond(getLeftKillBossTime() );
	
	vector<BossDamageRecord> rankList;
	m_pLegionBoss->getDamageRankList(10, rankList);
	for(size_t i = 0; i < rankList.size(); i++)
	{
		GSProto::LegionBossRankItem* pNewRankItem = pBossFightInfo->mutable_szranklist()->Add();
		pNewRankItem->set_strname(rankList[i].strName);
		pNewRankItem->set_idamagevalue(rankList[i].iDamageValue);
	}

	string strActorName = pActor->getProperty(PROP_ENTITY_NAME, "");
	pBossFightInfo->set_icheeredcount(getActorCheerCount(hActor) );
	pBossFightInfo->set_iselfdamage(m_pLegionBoss->getDamage(strActorName) );
	
	const MemberLegionBossState* pMemberBossState = getMemberBossState(strActorName);
	if(pMemberBossState)
	{
		pBossFightInfo->set_isumsilver(pMemberBossState->iSumSilver);
		pBossFightInfo->set_isumhonor(pMemberBossState->iSumHonor);
		
	}
	else
	{
		pBossFightInfo->set_isumsilver(0);
		pBossFightInfo->set_isumhonor(0);
	}

	pBossFightInfo->set_irelivecd(getBossMemReliveCD(strActorName) );
	pBossFightInfo->set_ibossmaxhp(m_pLegionBoss->getBossMaxHP() );
	pBossFightInfo->set_ibosshp(m_pLegionBoss->getBossHP() );
}


void GSLegionImp::onReqQueryBoss(HEntity hActor, const GSProto::CSMessage& msg)
{
	GSProto::CMD_QUERY_LEGIONBOSS_SC scMsg;
	if(!m_pLegionBoss)
	{
		scMsg.set_bcalled(false);
		GSProto::LegionBossPreCall* pPreCallInfo = scMsg.mutable_precallinfo();
		assert(pPreCallInfo);

		pPreCallInfo->set_icallcdsecond(getCallBossCD() );
		pPreCallInfo->set_ibossmaxhp(getBossMaxHP() );
		pPreCallInfo->set_ibosshp(getBossMaxHP() );
	}
	else
	{
		scMsg.set_bcalled(true);
		GSProto::LegionBossFightInfo* pScBossFightInfo = scMsg.mutable_fightinfo();
		fillScBossInfo(hActor, pScBossFightInfo);
	}

	IEntity* pActor = getEntityFromHandle(hActor);
	assert(pActor);

	pActor->sendMessage(GSProto::CMD_QUERY_LEGIONBOSS, scMsg);
}

void GSLegionImp::onReqCallBoss(HEntity hActor, const GSProto::CSMessage& msg)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

	string strActorName = pActor->getProperty(PROP_ENTITY_NAME, "");
	int iOccupation = getOccupation(strActorName);
	if(iOccupation != GSProto::en_LegionOccupation_Leader)
	{
		pActor->sendErrorCode(ERROR_ONLY_LEADER);
		return;
	}

	if(m_pLegionBoss)
	{
		pActor->sendErrorCode(ERROR_BOSS_ISRUNNING);
		return;
	}

	// 是否可召唤?
	if(getCallBossCD() > 0)
	{
		pActor->sendErrorCode(ERROR_CALLBOSS_CD);
		return;
	}

	// 召唤吧
	IBossFactory* pBossFactory = getComponent<IBossFactory>("BossFactory", IID_IBossFactory);
	assert(pBossFactory);

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	int iBossMonsterGrpID = pGlobalCfg->getInt("军团BOSS怪物组ID", 2);
	ServerEngine::CreateBossCtx bossCtx;

	bossCtx.iFixMaxHP = getBossMaxHP();
	bossCtx.iHP = bossCtx.iFixMaxHP;
	bossCtx.iFixLevel = m_legionData.bossInfo.iBossLevel;
	
	m_pLegionBoss = pBossFactory->createBoss(iBossMonsterGrpID, bossCtx);
	assert(m_pLegionBoss);

	// 记录时间
	m_legionData.bossInfo.dwLastBossTime = time(0);

	// 设置定时器
	ITimerComponent* pTimeAxis = getComponent<ITimerComponent>(COMPNAME_TimeAxis, IID_ITimerComponent);
	assert(pTimeAxis);

	int iBossTime = pGlobalCfg->getInt("军团BOSS持续时间", 600);
	m_bossTimerHandle = pTimeAxis->setTimer(this, 2, iBossTime*1000, "LegionBoss");
	m_bossUpdateHandle = pTimeAxis->setTimer(this, 3, 5*1000, "LegionBoss");

	// 通知客户端
	GSProto::CMD_CALL_LEGIONBOSS_SC scMsg;
	fillScBossInfo(hActor, scMsg.mutable_fightinfo() );

	pActor->sendMessage(GSProto::CMD_CALL_LEGIONBOSS, scMsg);

	PLAYERLOG(pActor)<<"CallLegionBoss|"<<bossCtx.iFixMaxHP<<"|"<<bossCtx.iFixLevel<<endl;
}

void GSLegionImp::onReqCheerLegionBoss(HEntity hActor, const GSProto::CSMessage& msg)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

	if(!m_pLegionBoss)
	{
		pActor->sendErrorCode(ERROR_CUR_NOLEGIONBOSS);
		return;
	}

	// 验证是否达到上限了
	string strActorName = pActor->getProperty(PROP_ENTITY_NAME, "");
	MemberLegionBossState* pState = getMemberBossState(strActorName);
	if(!pState)
	{
		pState = &m_mapBossState[strActorName];
	}
	assert(pState);

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);
	
	int iCheerCountLimit = pGlobalCfg->getInt("鼓舞次数限制", 10);
	if(pState->iCheerCount >= iCheerCountLimit)
	{
		pActor->sendErrorCode(ERROR_LEGIONBOSS_TOUCHMAX);
		return;
	}

	// 验证元宝是否足够
	int iNeedGold = pGlobalCfg->getInt("军团BOSS鼓舞消耗", 100);
	int iTkGold = pActor->getProperty(PROP_ACTOR_GOLD, 0);
	if(iTkGold < iNeedGold)
	{
		pActor->sendErrorCode(ERROR_NEED_GOLD);
		return;
	}

	// 扣除
	pActor->changeProperty(PROP_ACTOR_GOLD, 0-iNeedGold, GSProto::en_Reason_LegionBoss_Consume);
	pState->iCheerCount++;

	GSProto::CMD_CHEER_LEGIONBOSS_SC scMsg;
	scMsg.set_icheercount(pState->iCheerCount);
	pActor->sendMessage(GSProto::CMD_CHEER_LEGIONBOSS, scMsg);

	PLAYERLOG(pActor)<<"CheerLegionBoss|"<<iNeedGold<<endl;
}

struct LegionBossPVE
{
	LegionBossPVE(HEntity hActor, Detail::EventHandle::Proxy prx):m_hActor(hActor), m_legionPrx(prx){}
	void operator()(taf::Int32 iRet, const ServerEngine::BattleData& data, int iDamage)
	{
		if(en_FightResult_OK != iRet)
		{
			return;
		}

		if(!m_legionPrx.get() )
		{
			return;
		}
		
		IEntity* pActor = getEntityFromHandle(m_hActor);
		if(!pActor) return;

		GSLegionImp* pLegion = static_cast<GSLegionImp*>(m_legionPrx.get() );
		assert(pLegion);
		
		pLegion->processBossFightResult(m_hActor, data, iDamage);
	}

private:

	HEntity m_hActor;
	Detail::EventHandle::Proxy m_legionPrx;
};


void GSLegionImp::processBossFightResult(HEntity hActor, const ServerEngine::BattleData& data, int iDamage)
{
	if(!m_pLegionBoss) return;

	// 先计算奖励
	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	IDropFactory* pDropFactory = getComponent<IDropFactory>(COMPNAME_DropFactory, IID_IDropFactory);
	assert(pDropFactory);

	int iSilverParam = pGlobalCfg->getInt("军团BOSS伤害银币参数", 1000);
	int iHonorParam = pGlobalCfg->getInt("军团BOSS伤害荣誉参数", 100000);
	
	int iGetSilver = std::max(iDamage/iSilverParam, 1);
	int iGetHonor = std::max(iDamage/iHonorParam, 1);

	IEntity* pActor = getEntityFromHandle(hActor);
	assert(pActor);

	//关闭上浮
	CloseAttCommUP close(hActor);


	pActor->changeProperty(PROP_ACTOR_SILVER, iGetSilver, GSProto::en_Reason_LegionBoss_Create);
	pActor->changeProperty(PROP_ENTITY_HONOR, iGetHonor, GSProto::en_Reason_LegionBoss_Create);

	string strActorName = pActor->getProperty(PROP_ENTITY_NAME, "");
	m_mapBossState[strActorName].iSumSilver += iGetSilver;
	m_mapBossState[strActorName].iSumHonor += iGetHonor;

	// 发送战报
	GSProto::Cmd_Sc_CommFightResult scCommFightResult;
	int iScene = pGlobalCfg->getInt("军团BOSS战场景", 1);
	scCommFightResult.set_isceneid(iScene);
	scCommFightResult.set_iissuccess(data.bAttackerWin);
	scCommFightResult.set_istar(data.iStar);

	GSProto::FightAwardResult* pScAward = scCommFightResult.mutable_awardresult();
	pDropFactory->addPropToResult(*pScAward, GSProto::en_LifeAtt_Silver, iGetSilver);
	pDropFactory->addPropToResult(*pScAward, GSProto::en_LifeAtt_Honor, iGetHonor);

	PLAYERLOG(pActor)<<"FightLegionBoss|"<<iGetSilver<<"|"<<iGetHonor<<"|"<<(int)data.bAttackerWin<<endl;

	IFightSystem* pFightSys = static_cast<IFightSystem*>(pActor->querySubsystem(IID_IFightSystem));
	assert(pFightSys);
	GSProto::SCMessage awardPkg;
	HelpMakeScMsg(awardPkg, GSProto::CMD_COMM_FIGHTRESULT, scCommFightResult);
	pFightSys->sendAllBattleMsg(data, awardPkg);
	
	// 通知更新自身状态
	/*GSProto::CMD_FIGHT_LEGIONBOSS_SC scMsg;
	fillScBossInfo(hActor, scMsg.mutable_fightinfo() );
	pActor->sendMessage(GSProto::CMD_FIGHT_LEGIONBOSS, scMsg);*/

	if(!data.bAttackerWin)
	{
		m_mapBossState[strActorName].dwLastDeadTime = time(0);	
	}
	else
	{
		m_hKiller = hActor;
		broadcastBossEnd();
		
		clearBossRecord(true);
	}
}

void GSLegionImp::broadcastBossEnd()
{
	IUserStateManager* pUserStateManager = getComponent<IUserStateManager>(COMPNAME_LoginManager, IID_IUserStateManager);
	assert(pUserStateManager);

	vector<BossDamageRecord> rankList;
	m_pLegionBoss->getDamageRankList(10, rankList);

	map<string, int> rankMap;
	for(size_t i = 0; i < rankList.size(); i++)
	{
		rankMap[rankList[i].strName] = (int)(i+1);
	}

	for(map<string, MemberLegionBossState>::iterator it = m_mapBossState.begin(); it != m_mapBossState.end(); it++)
	{
		string strActorName = it->first;
		const MemberLegionBossState& memBossState = it->second;
		HEntity hActor = pUserStateManager->getActorByName(strActorName);
		IEntity* pActor = getEntityFromHandle(hActor);

		if(!pActor) continue;
		
		GSProto::CMD_LEGIONBOSS_END_SC scMsg;
		scMsg.set_bkill(0 != m_hKiller);
		if(rankMap.find(strActorName) != rankMap.end() )
		{
			scMsg.set_irank(rankMap[strActorName]);
		}
		else
		{
			scMsg.set_irank(0);
		}
		scMsg.set_idamage(m_pLegionBoss->getDamage(strActorName) );
		scMsg.set_iawardsilver(memBossState.iSumSilver);
		scMsg.set_iawardhonor(memBossState.iSumHonor);

		pActor->sendMessage(GSProto::CMD_LEGIONBOSS_END, scMsg);
	}
}

void GSLegionImp::onReqFightLegionBoss(HEntity hActor, const GSProto::CSMessage& msg)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

	if(!m_pLegionBoss)
	{
		pActor->sendErrorCode(ERROR_CUR_NOLEGIONBOSS);
		return;
	}

	// 判断是否CD期间
	string strActorName = pActor->getProperty(PROP_ENTITY_NAME, "");
	if(getBossMemReliveCD(strActorName) > 0)
	{
		pActor->sendErrorCode(ERROR_LEGIONBOSS_CD);
		return;
	}

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	int iCheerAttAddPercent = pGlobalCfg->getInt("鼓舞攻击增加万份比", 2000);

	ServerEngine::AttackBossCtx attackerCtx;
	int iCheerCount = getActorCheerCount(hActor);
	attackerCtx.iAttAddPercent = iCheerAttAddPercent * iCheerCount;
	
	m_pLegionBoss->AsynFightBoss(hActor, attackerCtx, LegionBossPVE(hActor, getEventHandle() ) );
}

void GSLegionImp::onReqRelive(HEntity hActor, const GSProto::CSMessage& msg)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

	if(!m_pLegionBoss)
	{
		pActor->sendErrorCode(ERROR_CUR_NOLEGIONBOSS);
		return;
	}

	string strActorName = pActor->getProperty(PROP_ENTITY_NAME, "");
	if(getBossMemReliveCD(strActorName) == 0)
	{
		pActor->sendErrorCode(ERROR_CUR_ALIVE);
		return;
	}

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	int iReliveCost = pGlobalCfg->getInt("军团BOSS战复活消耗", 10);
	int iTkGold = pActor->getProperty(PROP_ACTOR_GOLD, 0);
	if(iTkGold < iReliveCost)
	{
		pActor->sendErrorCode(ERROR_NEED_GOLD);
		return;
	}

	pActor->changeProperty(PROP_ACTOR_GOLD, 0 - iReliveCost, GSProto::en_Reason_LegionBossReliveConsume);

	MemberLegionBossState* pMemState = getMemberBossState(strActorName);
	assert(pMemState);

	pMemState->dwLastDeadTime = 0;

	pActor->sendMessage(GSProto::CMD_LEGIONBOSS_RELIVE);

	PLAYERLOG(pActor)<<"Relive|"<<iReliveCost<<endl;
}


void GSLegionImp::onReqGetPay(HEntity hActor, const GSProto::CSMessage& msg)
{
	LegionFactory* pLegionFactory = static_cast<LegionFactory*>(getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory) );
	assert(pLegionFactory);

	GSProto::CMD_GET_LEGIONPAY_CS req;
	if(!req.ParseFromString(msg.strmsgbody() ) )
	{
		return;
	}

	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

	int iGetType = req.ipaytype();
	if( (iGetType != GSProto::en_LegionPay_Day) && (iGetType != GSProto::en_LegionPay_Week) )
	{
		return;
	}

	string strActorName = pActor->getProperty(PROP_ENTITY_NAME, "");
	if(checkPayGeted(hActor, iGetType) )
	{
		pActor->sendErrorCode(ERROR_LEGION_PAYGETED);
		return;
	}

	LegionSystem* pLegionSys = static_cast<LegionSystem*>(pActor->querySubsystem(IID_ILegionSystem));
	assert(pLegionSys);

	int iContributeRank = getContributeRank(strActorName);
	int iParam = pLegionFactory->getPayParam(iContributeRank);

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	if(iGetType == GSProto::en_LegionPay_Day)
	{
		int iTotalSilver = (int)(getDayPayTotalSilver() * (double)iParam/100);
		pActor->changeProperty(PROP_ACTOR_SILVER, iTotalSilver, GSProto::en_Reason_LegionPay);
		PLAYERLOG(pActor)<<"GetPay|"<<iGetType<<"|"<<iTotalSilver<<"|"<<endl;
	}
	else
	{
		int iWeekParam = pGlobalCfg->getInt("军团周俸禄倍数", 5);
		int iWeekBaseGold = pGlobalCfg->getInt("军团基础元宝", 100);
		int iTotalSilver = getDayPayTotalSilver();

		int iDomainCount = getOwnDomainCount();
		int iWeekGetSilver = (int)(iTotalSilver * iWeekParam * (double)iParam/100);
		int iWeekGetGold = (int)(iWeekBaseGold * (1 + iDomainCount) * (double)iParam/100);
		pActor->changeProperty(PROP_ACTOR_SILVER, iWeekGetSilver, GSProto::en_Reason_LegionPay);
		pActor->changeProperty(PROP_ACTOR_GOLD, iWeekGetGold, GSProto::en_Reason_LegionPay);
		PLAYERLOG(pActor)<<"GetPay|"<<iGetType<<"|"<<iWeekGetSilver<<"|"<<iWeekGetGold<<endl;
	}	

	// 设置标记
	if(iGetType == GSProto::en_LegionPay_Day)
	{
		getDayResetValue(pLegionSys->getDayAwardData() ).iValue = 1;
	}
	else
	{
		getWeekResetValue(pLegionSys->getWeekAwardData() ).iValue = 1;
	}

	// 通知客户端
	int iCurContribute = pActor->getProperty(PROP_ACTOR_LEGION_CONTRIBUTE, 0);
	
	GSProto::CMD_GET_LEGIONPAY_SC scMsg;
	scMsg.set_ipaytype(iGetType);
	scMsg.set_bgeted(true);
	scMsg.set_iactorcontribute( iCurContribute );


	pActor->sendMessage(GSProto::CMD_GET_LEGIONPAY, scMsg);
}


bool GSLegionImp::checkPayGeted(HEntity hActor, int iPayType)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	assert(pActor);

	LegionSystem* pLegionSys = static_cast<LegionSystem*>(pActor->querySubsystem(IID_ILegionSystem) );
	assert(pLegionSys);
	
	if(GSProto::en_LegionPay_Day == iPayType)
	{
		int iDayFlag = getDayResetValue(pLegionSys->getDayAwardData() ).iValue;
		return iDayFlag;
	}
	else if(GSProto::en_LegionPay_Week == iPayType)
	{
		int iWeekFlag = getWeekResetValue(pLegionSys->getWeekAwardData() ).iValue;
		return iWeekFlag;
	}

	return false;
}


void GSLegionImp::onReqQueryPay(HEntity hActor, const GSProto::CSMessage& msg)
{
	LegionFactory* pLegionFactory = static_cast<LegionFactory*>(getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory) );
	assert(pLegionFactory);

	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

	string strActorName = pActor->getProperty(PROP_ENTITY_NAME, "");
	int iContributeRank = getContributeRank(strActorName);

	int iParam = pLegionFactory->getPayParam(iContributeRank);

	
	GSProto::CMD_QUERY_LEGIONPAY_SC scMsg;
	
	fillSCPayInfo(hActor, GSProto::en_LegionPay_Day, iContributeRank, iParam, scMsg.mutable_szpaylist()->Add() );
	fillSCPayInfo(hActor, GSProto::en_LegionPay_Week, iContributeRank, iParam, scMsg.mutable_szpaylist()->Add() );

	//ServerEngine::LegionMemberItem* pMemInfo = (ServerEngine::LegionMemberItem*)getMemberInfo(strActorName);
	//assert(pMemInfo);
	int iCurContribute = pActor->getProperty(PROP_ACTOR_LEGION_CONTRIBUTE, 0);
	scMsg.set_iactorcontribute(iCurContribute);

	pActor->sendMessage(GSProto::CMD_QUERY_LEGIONPAY, scMsg);
}

int GSLegionImp::getDayPayBaseSilver()
{
	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	int iBaseSilver = pGlobalCfg->getInt("军团每日基础俸禄", 30000);
	int iResultSilver = (int)( (double)iBaseSilver * (1.0 + (double)getLevel()/5) );

	return iResultSilver;
}

int GSLegionImp::getDayPayTotalSilver()
{
	LegionFactory* pLegionFactory = static_cast<LegionFactory*>(getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory) );
	assert(pLegionFactory);

	int iBaseSilver = getDayPayBaseSilver();
	int iDomainAdd = pLegionFactory->getDomainDayPay(getLegionName() );

	int iResult = iBaseSilver + iDomainAdd;

	return iResult;
}


void GSLegionImp::fillSCPayInfo(HEntity hActor, int iPayType, int iContributeRank, int iParam, GSProto::LegionPay* pScPay)
{
	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	int iDomainCount = getOwnDomainCount();
	
	pScPay->set_ipaytype(iPayType);
	pScPay->set_ilegionlv(m_legionData.baseInfo.iLegionLevel);
	pScPay->set_idomaincount(iDomainCount);
	pScPay->set_icontributerank(iContributeRank);

	int iTotalDaySilver = getDayPayTotalSilver();
	if(GSProto::en_LegionPay_Day == iPayType)
	{
		pScPay->set_ibasesilver(iTotalDaySilver);
		pScPay->set_ibasegold(0);
		pScPay->set_itotalsilver( (int)(iTotalDaySilver * (double)iParam/100) );
		pScPay->set_itotalgold(0);
	}
	else
	{
		int iWeekParam = pGlobalCfg->getInt("军团周俸禄倍数", 5);
		int iWeekGold = pGlobalCfg->getInt("军团基础元宝", 100) * (iDomainCount+1);

		int iBaseWeekSilver = iTotalDaySilver * iWeekParam;
		
		pScPay->set_ibasesilver(iBaseWeekSilver);
		pScPay->set_ibasegold(iWeekGold);

		
		pScPay->set_itotalsilver( (int)(iBaseWeekSilver * (double)iParam/100) );
		pScPay->set_itotalgold( (int)(iWeekGold * (double)iParam/100) );
	}

	IEntity* pActor = getEntityFromHandle(hActor);
	assert(pActor);

	string strActorName = pActor->getProperty(PROP_ENTITY_NAME, "");
	bool bGeted = checkPayGeted(hActor, iPayType);
	pScPay->set_bgeted(bGeted);
}

int GSLegionImp::getOwnDomainCount()
{
	LegionFactory* pLegionFactory = static_cast<LegionFactory*>(getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory) );
	assert(pLegionFactory);

	return pLegionFactory->getOwnCityCount(getLegionName() );
}


void GSLegionImp::onReqBless(HEntity hActor, const GSProto::CSMessage& msg)
{
	GSProto::CMD_LEGION_BLESS_CS req;
	if(!req.ParseFromString(msg.strmsgbody() ) )
	{
		return;
	}

	int iBlessID = req.iblesstype();

	LegionFactory* pLegionFactory = static_cast<LegionFactory*>(getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory) );
	assert(pLegionFactory);

	const LegionBlessCfg* pBlessCfg = pLegionFactory->queryBlessCfg(iBlessID);
	if(!pBlessCfg) return;

	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

	LegionSystem* pLegionSys = static_cast<LegionSystem*>(pActor->querySubsystem(IID_ILegionSystem));
	assert(pLegionSys);

	string strActorName = pActor->getProperty(PROP_ENTITY_NAME, "");
	ServerEngine::LegionMemberItem* pMemInfo = (ServerEngine::LegionMemberItem*)getMemberInfo(strActorName);
	assert(pMemInfo);

	// 验证是否已经祝福过了
	if(getDayResetValue(pLegionSys->getBlessData() ).iValue != 0)
	{
		pActor->sendErrorCode(ERROR_LEGION_BLESSED);
		return;
	}

	// 验证消耗
	int iTkSilver = pActor->getProperty(PROP_ACTOR_SILVER, 0);
	int iTkGold = pActor->getProperty(PROP_ACTOR_GOLD, 0);

	if(iTkSilver < pBlessCfg->iCostSilver)
	{
		pActor->sendErrorCode(ERROR_NEED_SILVER);
		return;
	}

	if(iTkGold < pBlessCfg->iCostGold)
	{
		pActor->sendErrorCode(ERROR_NEED_GOLD);
		return;
	}

	// 扣除消耗
	if(pBlessCfg->iCostSilver > 0)
	{
		pActor->changeProperty(PROP_ACTOR_SILVER, 0- pBlessCfg->iCostSilver, GSProto::en_Reason_BlessCost);
	}

	if(pBlessCfg->iCostGold > 0)
	{
		pActor->changeProperty(PROP_ACTOR_GOLD, 0- pBlessCfg->iCostGold, GSProto::en_Reason_BlessCost);
	}

	// 记录
	getDayResetValue(pLegionSys->getBlessData() ).iValue = 1;

	// 发送奖励
	addMemberContribyte(hActor, pBlessCfg->iGetContribute);
	pActor->changeProperty(PROP_ENTITY_PHYSTRENGTH, pBlessCfg->iGetPhystrength, GSProto::en_Reason_BlessCreate);

	// 通知客户端成功
	GSProto::CMD_LEGION_BLESS_SC scMsg;
	
	int iCurContribute = pActor->getProperty(PROP_ACTOR_LEGION_CONTRIBUTE, 0);
	scMsg.set_icontribute(iCurContribute);
	scMsg.set_igetcontribute(pBlessCfg->iGetContribute);
	pActor->sendMessage(GSProto::CMD_LEGION_BLESS, scMsg);
	
	PLAYERLOG(pActor)<<"Bless|"<<iBlessID<<"|"<<pBlessCfg->iGetContribute<<endl;
}


void GSLegionImp::onReqContribute(HEntity hActor, const GSProto::CSMessage& msg)
{
	GSProto::CMD_LEGION_CONTRIBUTE_CS req;
	if(!req.ParseFromString(msg.strmsgbody() ) )
	{
		return;
	}

	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;
	
	IGlobalCfg *pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);
	int iContriButeTimes = pGlobal->getInt("每日军团建设次数", 1);

	LegionSystem *pLegion = static_cast<LegionSystem*>( pActor->querySubsystem(IID_ILegionSystem));
	assert(pLegion);
	
	int iHaveContributeTimes = pLegion->getContributeTimes().iValue;
	
	if(iContriButeTimes <= iHaveContributeTimes)
	{
		pActor->sendErrorCode(ERROR_LEGION_BUILD_TIMES);
		return;
	}

	int iContributeID = req.icontributetype();

	LegionFactory* pLegionFactory = static_cast<LegionFactory*>(getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory) );
	assert(pLegionFactory);

	const LegionContributeCfg* pContributeCfg = pLegionFactory->queryContributeCfg(iContributeID);
	if(!pContributeCfg) return;

	

	// 验证消耗
	int iTkSilver = pActor->getProperty(PROP_ACTOR_SILVER, 0);
	int iTkGold = pActor->getProperty(PROP_ACTOR_GOLD, 0);

	if(iTkSilver < pContributeCfg->iCostSilver)
	{
		pActor->sendErrorCode(ERROR_NEED_SILVER);
		return;
	}

	if(iTkGold < pContributeCfg->iCostGold)
	{
		pActor->sendErrorCode(ERROR_NEED_GOLD);
		return;
	}

	// 扣除消耗
	if(pContributeCfg->iCostSilver > 0)
	{
		pActor->changeProperty(PROP_ACTOR_SILVER, 0- pContributeCfg->iCostSilver, GSProto::en_Reason_ContributeCost);
	}

	if(pContributeCfg->iCostGold > 0)
	{
		pActor->changeProperty(PROP_ACTOR_GOLD, 0- pContributeCfg->iCostGold, GSProto::en_Reason_ContributeCost);
	}

	// 发放奖励
	addMemberContribyte(hActor, pContributeCfg->iGetContribute);
	pLegion->getContributeTimes().iValue++;

	GSProto::CMD_LEGION_CONTRIBUTE_SC scMsg;
	//string strActorName = pActor->getProperty(PROP_ENTITY_NAME, "");
	//const ServerEngine::LegionMemberItem* pLegionMemInfo = getMemberInfo(strActorName);
	//assert(pLegionMemInfo);
	int iCurContribute = pActor->getProperty(PROP_ACTOR_LEGION_CONTRIBUTE, 0);
	scMsg.set_icontribute(iCurContribute);
	scMsg.set_ilegionexp(m_legionData.baseInfo.iLegionExp);
	scMsg.set_ilegionuplevelexp(getLevelUpExp() );
	scMsg.set_ilevel(getLevel() );
	scMsg.set_igetcontribute(pContributeCfg->iGetContribute);

	pActor->sendMessage(GSProto::CMD_LEGION_CONTRIBUTE, scMsg);

	PLAYERLOG(pActor)<<"Contribute|"<<iContributeID<<"|"<<pContributeCfg->iGetContribute<<endl;
}

void GSLegionImp::onReqQueryContribute(HEntity hActor, const GSProto::CSMessage& msg)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

	//string strActorName = pActor->getProperty(PROP_ENTITY_NAME, "");
	//const ServerEngine::LegionMemberItem* pMemInfo = getMemberInfo(strActorName);
	//assert(pMemInfo);

	GSProto::CMD_QUERY_LELGION_CONTRIBUTE_SC scMsg;
	
	int iCurContribute = pActor->getProperty(PROP_ACTOR_LEGION_CONTRIBUTE, 0);
	
	scMsg.set_icontribute(iCurContribute);
	scMsg.set_ilegionexp(m_legionData.baseInfo.iLegionExp);
	scMsg.set_ilegionuplevelexp(getLevelUpExp() );
	scMsg.set_ilevel(m_legionData.baseInfo.iLegionLevel);

	pActor->sendMessage(GSProto::CMD_QUERY_LELGION_CONTRIBUTE, scMsg);
}


void GSLegionImp::onReqTransLeader(HEntity hActor, const GSProto::CSMessage& msg)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

	string strActorName = pActor->getProperty(PROP_ENTITY_NAME, "");
	int iOccupation = getOccupation(strActorName);
	if(iOccupation != GSProto::en_LegionOccupation_Leader)
	{
		pActor->sendErrorCode(ERROR_NO_RIGHT);
		return;
	}

	GSProto::CMD_TRANSFER_LEGIONLEADER_CS req;
	if(!req.ParseFromString(msg.strmsgbody() ) )
	{
		return;
	}

	string strTargetName = req.strtargetmemname();

	// 是否成员
	ServerEngine::LegionMemberItem* pMemItem =  getMemberInfoByName(strTargetName);
	if(!pMemItem )
	{
		pActor->sendErrorCode(ERROR_TARGET_NOT_MEMBER);
		return;
	}

	if(strTargetName == strActorName)
	{
		return;
	}

	// 转让吧
	m_legionData.baseInfo.strLeaderName = strTargetName;
	pMemItem->iOccupation = GSProto::en_LegionOccupation_Leader;
	
	//把自己变成成员
	ServerEngine::LegionMemberItem* pMemSelf =  getMemberInfoByName(strActorName);
	pMemSelf->iOccupation = GSProto::en_LegionOccupation_Member;
	
	// 排序
	sortMemberRank();

	// 通知客户端
	GSProto::CMD_TRANSFER_LEGIONLEADER_SC scMsg;
	scMsg.set_strnewleader(strTargetName);

	pActor->sendMessage(GSProto::CMD_TRANSFER_LEGIONLEADER, scMsg);

	PLAYERLOG(pActor)<<"TansLeader|"<<strTargetName<<"|"<<getLegionName()<<endl;
}

ServerEngine::LegionMemberItem* GSLegionImp::getMemberInfoByName(const string& strMemName)
{
	map<std::string, ServerEngine::LegionMemberItem>::iterator it = m_legionData.memberInfo.memberList.find(strMemName);
	if(it != m_legionData.memberInfo.memberList.end() )
	{
		return &(it->second);
	}

	return NULL;
}

void GSLegionImp::onReqKickMember(HEntity hActor, const GSProto::CSMessage& msg)
{
	GSProto::CMD_KICK_LEGIONMEMBER_CS req;
	if(!req.ParseFromString(msg.strmsgbody() ) )
	{
		return;
	}

	// 权限验证
	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

	string strActorName = pActor->getProperty(PROP_ENTITY_NAME, "");
	int iOccupation = getOccupation(strActorName);

	if(iOccupation == GSProto::en_LegionOccupation_Member)
	{
		pActor->sendErrorCode(ERROR_NO_RIGHT);
		return;
	}

	string strMemName = req.strlegionmemname();
	map<std::string, ServerEngine::LegionMemberItem>::iterator iter = m_legionData.memberInfo.memberList.find(strMemName); 
	if( iter == m_legionData.memberInfo.memberList.end() )
	{
		pActor->sendErrorCode(ERROR_TARGET_NOT_MEMBER);
		return;
	}
	
	if(strMemName == strActorName)
	{
		pActor->sendErrorCode(ERROR_CANNOT_KICKSELF);
		return;
	}

	if( iOccupation == GSProto::en_LegionOccupation_ViceLeader)
	{
		if( getOccupation(strMemName) <=  GSProto::en_LegionOccupation_ViceLeader)
		{
			pActor->sendErrorCode(ERROR_NO_RIGHT);
			return;
		}
	}


	LegionFactory* pLegionFactory = static_cast<LegionFactory*>(getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory) );
	assert(pLegionFactory);

	if(pLegionFactory->bLegionInCampBattle(getLegionName()) )
	{
		pActor->sendErrorCode(ERROR_LEGION_KICKMEMBER_INBATTLE);
		return;
	}
	
	
	vector<string>::iterator it = find(m_sortMemberList.begin(), m_sortMemberList.end(), strMemName);
	if(it != m_sortMemberList.end() )
	{
		
		m_sortMemberList.erase(it);
	}

	
	//发送邮箱

	pLegionFactory->sendMail( iter->second, 1001);

	// 同步名字Cache
	m_legionData.memberInfo.memberList.erase(strMemName);
	pLegionFactory->removeActorNameMap(strMemName);
	
	// 通知客户端
	GSProto::CMD_KICK_LEGIONMEMBER_SC scMsg;
	scMsg.set_strlegionmemname(strMemName);

	pActor->sendMessage(GSProto::CMD_KICK_LEGIONMEMBER, scMsg);

	PLAYERLOG(pActor)<<"KickMember|"<<strMemName<<endl;
}


void GSLegionImp::onReqDisssmissLegion(HEntity hActor, const GSProto::CSMessage& msg)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

	int iOccupation = getOccupation(pActor->getProperty(PROP_ENTITY_NAME, "") );
	if(iOccupation != GSProto::en_LegionOccupation_Leader)
	{
		pActor->sendErrorCode(ERROR_NO_RIGHT);
		return;
	}

	LegionFactory* pLegionFactory = static_cast<LegionFactory*>(getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory) );
	assert(pLegionFactory);

	if(pLegionFactory->bLegionInCampBattle(getLegionName()) )
	{
		pActor->sendErrorCode(ERROR_LEGION_INFIGHTING);
		return;
	}

	//GSProto::SCMessage pkg;
	//pkg.set_icmd(GSProto::CMD_DISMISS_LEGION);

	//broadcastMessage(pkg);

	PLAYERLOG(pActor)<<"DismissLegion|"<<getLegionName()<<endl;
	pLegionFactory->removeLegion(this);

	// 不要在这后面写代码了
}


void GSLegionImp::gmSetLevel(int iLevel)
{
	LegionFactory* pLegionFactory = static_cast<LegionFactory*>(getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory) );
	assert(pLegionFactory);

	const LegionLevelInfo* pLegionLevelInfo = pLegionFactory->queryLevelInfo(iLevel);
	if(!pLegionLevelInfo) return;

	m_legionData.baseInfo.iLegionLevel = iLevel;
}


void GSLegionImp::broadcastMessage(const GSProto::SCMessage& msg)
{
	IUserStateManager* pUserstateMgr = getComponent<IUserStateManager>(COMPNAME_LoginManager, IID_IUserStateManager);
	assert(pUserstateMgr);

	for(size_t i =0; i < m_sortMemberList.size(); i++)
	{
		HEntity hTmp = pUserstateMgr->getActorByName(m_sortMemberList[i]);
		IEntity* pTmp = getEntityFromHandle(hTmp);
		if(!pTmp) continue;

		pTmp->sendMessage( (GSProto::SCMessage&)msg);
	}
}

void GSLegionImp::onReqRefuseAllApp(HEntity hActor, const GSProto::CSMessage& msg)
{
	/*IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

	int iOccupation = getOccupation(pActor->getProperty(PROP_ENTITY_NAME, "") );
	if(iOccupation > GSProto::en_LegionOccupation_ViceLeader)
	{
		pActor->sendErrorCode(ERROR_NO_RIGHT);
		return;
	}

	m_legionData.appInfo.applyList.clear();

	GSProto::SCMessage pkg;
	pkg.set_icmd(GSProto::CMD_REFUSE_ALLAPP);

	pActor->sendMessage(pkg);

	PLAYERLOG(pActor)<<"RefuseAll|"<<getLegionName()<<endl;*/
}

struct JoinGetActorDataCallBack
{
	JoinGetActorDataCallBack()
	{
		
	}

	virtual void operator()(int iRet, HEntity hTarget, ServerEngine::PIRole& roleInfo)
	{
		if( iRet != ServerEngine::en_RoleRet_OK)
		{
			FDLOG("ManorSystem")<<"WillLootActorCallBack|"<< iRet <<endl;
			return;
		}

		if(0 != hTarget)
		{
			IEntity *pEntity = getEntityFromHandle(hTarget);
			assert(pEntity);
			LegionSystem *pLegion = static_cast<LegionSystem*>(pEntity->querySubsystem( IID_ILegionSystem));
			assert(pLegion);
			pLegion->addJionLegionTimes(1);
		}
		else
		{
			ServerEngine::RoleSaveData roleSaveData;
			ServerEngine::JceToObj(roleInfo.roleData, roleSaveData);
			map<taf::Int32, std::string>::iterator iter = roleSaveData.subsystemData.find(IID_ILegionSystem);
			assert(iter != roleSaveData.subsystemData.end());
			ServerEngine::LegionSystemData systemData;
			ServerEngine::JceToObj(iter->second, systemData);
			//修改数据
			getWeekResetValue( systemData.actorWeekAddLegionTimes).iValue++;
			//回写
			iter->second = ServerEngine::JceToStr(systemData);
			roleInfo.roleData = ServerEngine::JceToStr(roleSaveData);
		}
		
	}
};

int GSLegionImp::iGetRequestListSize()
{
	return m_legionData.appInfo.applyList.size();
}


void GSLegionImp::onReqDealLegionApp(HEntity hActor, const GSProto::CSMessage& msg)
{
	GSProto::CMD_DEAL_LEGIONAPP_CS req;
	if(!req.ParseFromString(msg.strmsgbody() ) )
	{
		return;
	}

	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

	int iOccupation = getOccupation(pActor->getProperty(PROP_ENTITY_NAME, "") );
	if(iOccupation > GSProto::en_LegionOccupation_ViceLeader)
	{
		pActor->sendErrorCode(ERROR_NO_RIGHT);
		return;
	}

	LegionFactory* pLegionFactory = static_cast<LegionFactory*>(getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory) );
	assert(pLegionFactory);

	if(pLegionFactory->bLegionInCampBattle(getLegionName()) )
	{
		pActor->sendErrorCode(ERROR_LEGION_APP_INBATTLE);
		return;
	}
	

	string strAppName = req.strappname();
	bool bAccept = req.baccept();

	if(!bAccept)
	{
		m_legionData.appInfo.applyList.erase(strAppName);
		GSProto::CMD_DEAL_LEGIONAPP_SC scMsg;
		scMsg.set_strappname(strAppName);
		scMsg.set_baccept(bAccept);
		pActor->sendMessage(GSProto::CMD_DEAL_LEGIONAPP, scMsg);

		PLAYERLOG(pActor)<<"DealLegionApp|Refuse|"<<strAppName<<"|"<<getLegionName()<<endl;
		return;
	}

	if(m_legionData.appInfo.applyList.find(strAppName) == m_legionData.appInfo.applyList.end() )
	{
		return;
	}

	// 添加名字索引
	
	if(pLegionFactory->getActorLegion(strAppName))
	{
		pActor->sendErrorCode(ERROR_HAS_LEGION);
		//删除请求
		ServerEngine::LegionAppItem tmpAppItem = m_legionData.appInfo.applyList[strAppName];
		m_legionData.appInfo.applyList.erase(strAppName);
		
		GSProto::CMD_DEAL_LEGIONAPP_SC scMsg;
		scMsg.set_strappname(strAppName);
		scMsg.set_baccept(bAccept);
		pActor->sendMessage(GSProto::CMD_DEAL_LEGIONAPP, scMsg);
		return;
	}

	//人数上限
	if( (int)m_legionData.memberInfo.memberList.size() >= getMemberLimit())
	{
		pActor->sendErrorCode(ERROR_LEGION_MEMBER_LIMIT);
		return;
	}
	
	// 添加到成员
	ServerEngine::LegionAppItem tmpAppItem = m_legionData.appInfo.applyList[strAppName];
	m_legionData.appInfo.applyList.erase(strAppName);

	
	pLegionFactory->addActorNameMap(strAppName, m_hHandle);
	
	ServerEngine::LegionMemberItem& memItem = m_legionData.memberInfo.memberList[strAppName];
	memItem.roleKey = tmpAppItem.roleKey;
	memItem.strName = strAppName;
	memItem.iLevel = tmpAppItem.iLevel;
	//memItem.iArenaRank = tmpAppItem.iArenaRank;
	memItem.contributeVec.resize(3,0);
	memItem.iOccupation = GSProto::en_LegionOccupation_Member;

	IUserStateManager* pUserManager = getComponent<IUserStateManager>(COMPNAME_LoginManager,IID_IUserStateManager);
	assert(pUserManager);	
	HEntity hEntity = pUserManager->getActorByName(strAppName);
	
	memItem.bIsOutLine = (0 == hEntity);
	memItem.dwLogionOutTime = time(0) - 1;
	memItem.iHeadIcon = tmpAppItem.iActorHeadIcon;
	// 添加到sortList
	m_sortMemberList.push_back(strAppName);

	GSProto::CMD_DEAL_LEGIONAPP_SC scMsg;
	scMsg.set_strappname(strAppName);
	scMsg.set_baccept(bAccept);

	pActor->sendMessage(GSProto::CMD_DEAL_LEGIONAPP, scMsg);


	IModifyDelegate* pModifyDelegate = getComponent<IModifyDelegate>(COMPNAME_ModifyDelegate, IID_IModifyDelegate);
	assert(pModifyDelegate);
	pModifyDelegate->submitDelegateTask(tmpAppItem.roleKey, JoinGetActorDataCallBack());
	

	PLAYERLOG(pActor)<<"DealLegionApp|Accept|"<<strAppName<<"|"<<getLegionName()<<endl;
}


void GSLegionImp::onReqModifySetting(HEntity hActor, const GSProto::CSMessage& msg)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

	
	int iOccupation = getOccupation(pActor->getProperty(PROP_ENTITY_NAME, "") );
	if(iOccupation != GSProto::en_LegionOccupation_Leader)
	{
		pActor->sendErrorCode(ERROR_NO_RIGHT);
		return;
	}

	GSProto::CMD_MODIFY_LEGION_SETTING_CS req;
	if(!req.ParseFromString(msg.strmsgbody() ) )
	{
		return;
	}

	m_legionData.baseInfo.strAnnouce = req.strannouce();
	m_legionData.baseInfo.strBroadcast = req.strbroadcast();
	m_legionData.baseInfo.iLegionIconId = req.ilegioniconid();

	pActor->sendMessage(GSProto::CMD_MODIFY_LEGION_SETTING);
}

void GSLegionImp::onReqQueryAppList(HEntity hActor, const GSProto::CSMessage& msg)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

	LegionFactory* pLegionFactory = static_cast<LegionFactory*>(getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory) );
	assert(pLegionFactory);
		
	GSProto::CMD_QUERY_LEGION_APP_SC scMsg;
	for(map<std::string, ServerEngine::LegionAppItem>::iterator it = m_legionData.appInfo.applyList.begin(); it != m_legionData.appInfo.applyList.end();)
	{
		//删除已经有军团的申请
		
		if(pLegionFactory->getActorLegion(it->first))
		{
			m_legionData.appInfo.applyList.erase(it++);
			continue;
		}
		
		GSProto::LegionAppItem* pNewAppItem = scMsg.mutable_szapplist()->Add();
		fillLegionAppItem(it->second, pNewAppItem);
		
	 	it++;
	}

	pActor->sendMessage(GSProto::CMD_QUERY_LEGION_APP, scMsg);
}

void GSLegionImp::fillLegionAppItem(const ServerEngine::LegionAppItem& refItem, GSProto::LegionAppItem* pNewItem)
{
	pNewItem->set_strappname(refItem.strName);
	pNewItem->set_ilevel(refItem.iLevel);
	pNewItem->set_iarenarank(getArenaRank(refItem.strName));
	pNewItem->set_iactorheadicon( refItem.iActorHeadIcon);
}

void GSLegionImp::onReqLegionMemList(HEntity hActor, const GSProto::CSMessage& msg)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

	GSProto::CMD_QUERY_LEGION_MEMBERLIST_SC scMsg;
	for(size_t i = 0; i < m_sortMemberList.size(); i++)
	{
		fillLegionMember(m_sortMemberList[i], scMsg.mutable_szmemberlist()->Add(), hActor );
	}

	pActor->sendMessage(GSProto::CMD_QUERY_LEGION_MEMBERLIST, scMsg);
}


void GSLegionImp::fillLegionMember(const string& strMemName, GSProto::LegionMember* pScMember, HEntity hActor)
{
	assert(m_legionData.memberInfo.memberList.find(strMemName) != m_legionData.memberInfo.memberList.end() );

	ServerEngine::LegionMemberItem& reflegionMemIte = m_legionData.memberInfo.memberList[strMemName];

	pScMember->set_strmemname(strMemName);
	pScMember->set_ilevel(reflegionMemIte.iLevel);
	pScMember->set_ioccupation(getOccupation(strMemName) );
	pScMember->set_iarenarank(getArenaRank(strMemName));
	pScMember->set_idaycontribute(getDayResetValue(reflegionMemIte.dayContribute).iValue);
	pScMember->set_itotalcontribute(reflegionMemIte.iSumContribute);
	pScMember->set_icontributerank(getContributeRank(strMemName) );
	pScMember->set_bactorisonline( reflegionMemIte.bIsOutLine);
	pScMember->set_iheadicon(reflegionMemIte.iHeadIcon);
		
	if( reflegionMemIte.bIsOutLine)
	{
		IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
		assert(pZoneTime);

		int iSecond = pZoneTime->GetCurSecond() - reflegionMemIte.dwLogionOutTime;

		pScMember->set_outlinesecond(iSecond);
	}
	
	pScMember->set_biscanworship(bMemberCanWorship(hActor, strMemName));
	pScMember->set_ithreedaycontribute(getThreeDayContribute(reflegionMemIte));
	
}

int GSLegionImp::getContributeRank(const string& strMemName)
{
	vector<string>::iterator it = find(m_sortMemberList.begin(), m_sortMemberList.end(), strMemName);

	int iContributeRank = std::distance(m_sortMemberList.begin(), it);

	return iContributeRank + 1;
}


void GSLegionImp::onReqLegionBase(HEntity hActor, const GSProto::CSMessage& msg)
{
	sendBaseInfo(hActor);
}


int GSLegionImp::getOccupation(const string& strMemName)
{
	const ServerEngine::LegionMemberItem* pMemInfo = getMemberInfo(strMemName);
	if(!pMemInfo) return 0;

	return pMemInfo->iOccupation;

	/*
	if(strMemName == m_legionData.baseInfo.strLeaderName)
	{
		return GSProto::en_LegionOccupation_Leader;
	}

	// 如果贡献不到2000, 则为见习
	IGlobalCfg* pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);

	int iMemberNeedContr = pGlobal->getInt("团员所需贡献", 2000);

	const ServerEngine::LegionMemberItem* pMemInfo = getMemberInfo(strMemName);
	assert(pMemInfo);

	if(pMemInfo->iSumContribute < iMemberNeedContr)
	{
		return GSProto::en_LegionOccupation_Newer;
	}

	const LegionLevelInfo* pLevelInfo = getCurLevelInfo();
	assert(pLevelInfo);

	for(int i = 0; (i < pLevelInfo->iViceLeaderLimitt) && (i < (int)m_sortMemberList.size() ); i++)
	{
		if(strMemName == m_sortMemberList[i])
		{
			return GSProto::en_LegionOccupation_ViceLeader;
		}
	}

	// 剩下的都是群众	
	return GSProto::en_LegionOccupation_Member;*/
}


string GSLegionImp::getLeaderName()
{
	return m_legionData.baseInfo.strLeaderName;
}


void GSLegionImp::sendBaseInfo(HEntity hActor)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	assert(pActor);

	LegionSystem* pLegionSys = static_cast<LegionSystem*>(pActor->querySubsystem(IID_ILegionSystem));
	assert(pLegionSys);

	GSProto::CMD_QUERY_LEGION_BASE_SC scMsg;
	GSProto::LegionBaseInfo* pScBaseInfo = scMsg.mutable_baseinfo();

	pScBaseInfo->set_dwlegionobjectid(m_hHandle);
	pScBaseInfo->set_strlegionname(getLegionName() );
	pScBaseInfo->set_strleadername(getLeaderName() );
	pScBaseInfo->set_irank(getRank() );
	pScBaseInfo->set_imemcount((int)m_legionData.memberInfo.memberList.size() );
	pScBaseInfo->set_imemlimit(getMemberLimit() );
	pScBaseInfo->set_ilevel(m_legionData.baseInfo.iLegionLevel);
	pScBaseInfo->set_iexp(m_legionData.baseInfo.iLegionExp);
	pScBaseInfo->set_ilevelupexp(getLevelUpExp() );
	pScBaseInfo->set_strbroadcat(m_legionData.baseInfo.strBroadcast);
	pScBaseInfo->set_ilegioniconid(m_legionData.baseInfo.iLegionIconId);
	pScBaseInfo->set_ilegionmoney(m_legionData.baseInfo.iLegionMoney);
	pScBaseInfo->set_strannouce(m_legionData.baseInfo.strAnnouce);

	string strActorName = pActor->getProperty(PROP_ENTITY_NAME, "");
	pScBaseInfo->set_ioccupation(getOccupation(strActorName));

//	ServerEngine::LegionMemberItem* pMemInfo = (ServerEngine::LegionMemberItem*)getMemberInfo(strActorName);
//	assert(pMemInfo);
	int iCurContribute = pActor->getProperty(PROP_ACTOR_LEGION_CONTRIBUTE, 0);
	pScBaseInfo->set_icontribute( iCurContribute);
	pScBaseInfo->set_iblessed(getDayResetValue(pLegionSys->getBlessData() ).iValue);

	pActor->sendMessage(GSProto::CMD_QUERY_LEGION_BASE, scMsg);
}

void GSLegionImp::sendSimpleInfo(HEntity hActor)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	assert(pActor);

	GSProto::CMD_QUERY_OTHERLEGION_SC scMsg;
	GSProto::OtherLegionInfo* pOtherInfo = scMsg.mutable_otherlegioninfo();

	pOtherInfo->set_dwlegionobjectid(m_hHandle);
	pOtherInfo->set_strlegionname(getLegionName() );
	pOtherInfo->set_ilevel(getLevel() );
	pOtherInfo->set_strleadername(getLeaderName() );
	pOtherInfo->set_irank(getRank() );
	pOtherInfo->set_imemcount(getMemberCount() );
	pOtherInfo->set_imemlimit(getMemberLimit() );
	pOtherInfo->set_strannouce(m_legionData.baseInfo.strAnnouce);

	pActor->sendMessage(GSProto::CMD_QUERY_OTHERLEGION, scMsg);
}


void GSLegionImp::cancelApp(HEntity hActor)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

	string strActorName = pActor->getProperty(PROP_ENTITY_NAME, "");

	// 是否已经申请了
	if(m_legionData.appInfo.applyList.find(strActorName) == m_legionData.appInfo.applyList.end() )
	{
		pActor->sendErrorCode(ERROR_NO_APPLYED);
		return;
	}

	m_legionData.appInfo.applyList.erase(strActorName);

	GSProto::CMD_CANCEL_LEGIONAPP_SC scMsg;
	scMsg.set_dwlegionobjectid( (Uint32)m_hHandle);

	pActor->sendMessage(GSProto::CMD_CANCEL_LEGIONAPP, scMsg);

	PLAYERLOG(pActor)<<"cancelApp|"<<getLegionName()<<endl;
}


void GSLegionImp::applyJoin(HEntity hActor)
{
	LegionFactory* pLegionFactory = static_cast<LegionFactory*>(getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory) );
	assert(pLegionFactory);

	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

	string strActorName = pActor->getProperty(PROP_ENTITY_NAME, "");

	// 判断玩家是否已经有帮派
	if(pLegionFactory->getActorLegion(strActorName) )
	{
		pActor->sendErrorCode(ERROR_HAS_LEGION);
		return;
	}

	// 是否已经申请了
	if(m_legionData.appInfo.applyList.find(strActorName) != m_legionData.appInfo.applyList.end() )
	{
		pActor->sendErrorCode(ERROR_HAS_APPLYED);
		return;
	}

	// 是否达到申请上限
	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	int iLimitCount = pGlobalCfg->getInt("军团申请上限", 30);
	if(m_legionData.appInfo.applyList.size() >= (size_t)iLimitCount)
	{
		pActor->sendErrorCode(ERROR_APPLY_FULL);
		return;
	}

	// 添加申请
	ServerEngine::LegionAppItem& refAppItem = m_legionData.appInfo.applyList[strActorName];
	refAppItem.strName = strActorName;
	refAppItem.iLevel = pActor->getProperty(PROP_ENTITY_LEVEL, 0);
	refAppItem.iActorHeadIcon = pActor->getProperty( PROP_ENTITY_ACTOR_HEAD, 0);

	refAppItem.roleKey.strAccount = pActor->getProperty(PROP_ACTOR_ACCOUNT, "");
	refAppItem.roleKey.rolePos = 0;
	refAppItem.roleKey.worldID = pActor->getProperty(PROP_ACTOR_WORLD, 0);



	// 通知客户端
	GSProto::CMD_APPJOIN_LEGION_SC scMsg;
	scMsg.set_dwlegionobjectid( (Uint32)m_hHandle);

	pActor->sendMessage(GSProto::CMD_APPJOIN_LEGION, scMsg);

	PLAYERLOG(pActor)<<"AppLegion|"<<getLegionName()<<endl;
}

int GSLegionImp::getArenaRank(const string& strActorName)
{
	IArenaFactory *pArenaFactory = getComponent<IArenaFactory>(COMPNAME_ArenaFactory, IID_IArenaFactory);
	assert(pArenaFactory);

	return pArenaFactory->getArenaRank(strActorName);
}


const LegionLevelInfo* GSLegionImp::getCurLevelInfo()
{
	LegionFactory* pLegionFactory = static_cast<LegionFactory*>(getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory) );
	assert(pLegionFactory);

	const LegionLevelInfo* pLegionLevelInfo = pLegionFactory->queryLevelInfo(getLevel() );
	assert(pLegionLevelInfo);

	return pLegionLevelInfo;
}

int GSLegionImp::getMemberLimit()
{
	return getCurLevelInfo()->iMemLimit;
}

int GSLegionImp::getViceCount()
{
	int iCount = 0;
	map<std::string, ServerEngine::LegionMemberItem>::iterator it = m_legionData.memberInfo.memberList.begin();
	for(;it != m_legionData.memberInfo.memberList.end(); ++it )
	{
		const ServerEngine::LegionMemberItem & item = it->second;
		if(item.iOccupation == GSProto::en_LegionOccupation_ViceLeader)
		{
			iCount += 1;
		}
	}

	return iCount;
}

int GSLegionImp::getViceMemberLimit()
{
	return getCurLevelInfo()->iViceLeaderLimitt;
}

int GSLegionImp::getRank()
{
	return m_iRank;
}

int GSLegionImp::getLevelUpExp()
{
	return getCurLevelInfo()->iNeedExp;
}


void GSLegionImp::addMemberContribyte(const string& strMemName, int iContribute, HEntity hActor)
{
	map<std::string, ServerEngine::LegionMemberItem>::iterator it = m_legionData.memberInfo.memberList.find(strMemName);
	if(it == m_legionData.memberInfo.memberList.end() )
	{
		return;
	}

	ServerEngine::LegionMemberItem& refMemberItem = it->second;

	// 添加之前的职位
	//int iPreOccupation = getOccupation(strMemName);

	getDayResetValue(refMemberItem.dayContribute).iValue += iContribute;
	refMemberItem.iSumContribute += iContribute;
	//refMemberItem.iCurContribute += iContribute;

	IEntity *pEntity = getEntityFromHandle(hActor);
	assert(pEntity);
	
	pEntity->changeProperty( PROP_ACTOR_LEGION_CONTRIBUTE, iContribute,  GSProto::en_Reason_Add_Contribute);
	
	//军团成员每获得一点贡献，可以为军团增加1000点资金
	IGlobalCfg *pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	int iAddMoneyMulNum = pGlobalCfg->getInt("成员获得一点贡献军团增加资金", 1000);
	assert(iAddMoneyMulNum >= 0);
	
	m_legionData.baseInfo.iLegionMoney += iContribute * iAddMoneyMulNum;
	
	if(refMemberItem.contributeVec.size() < 3)
	{
		refMemberItem.contributeVec.resize(3,0);
	}
	
	refMemberItem.contributeVec[2] += iContribute;

	sortMemberRank();

	/*if(iPreOccupation == GSProto::en_LegionOccupation_Newer)
	{
		int iCurOccupation = getOccupation(strMemName);
		if(iCurOccupation != GSProto::en_LegionOccupation_Newer)
		{
			// 添加到军团
			addExp(refMemberItem.iSumContribute);
		}
	}
	else
	{
		addExp(iContribute);
	}*/

	addExp(iContribute);
}
	
void GSLegionImp::addMemberContribyte(HEntity hActor, int iContribute)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

	string strActorName = pActor->getProperty(PROP_ENTITY_NAME, "");
	addMemberContribyte(strActorName, iContribute, hActor);

	PLAYERLOG(pActor)<<"MemContribute|"<<iContribute<<endl;
}

void GSLegionImp::addExp(int iContribute)
{
	LegionFactory* pLegionFactory = static_cast<LegionFactory*>(getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory) );
	assert(pLegionFactory);

	int iLeftContribute = iContribute + m_legionData.baseInfo.iLegionExp;
	int iCurLevel = getLevel();
	int iCurExp = m_legionData.baseInfo.iLegionExp;
	
	while(true)
	{
		int iNeedExp = pLegionFactory->queryLevelInfo(iCurLevel)->iNeedExp;
		if(!pLegionFactory->queryLevelInfo(iCurLevel + 1) )
		{
			break;
		}

		if(iLeftContribute >= iNeedExp)
		{
			iCurLevel++;
			iLeftContribute -= iNeedExp;
			iCurExp = 0;
		}
		else
		{
			iCurExp = iLeftContribute;
			break;
		}
	}

	m_legionData.baseInfo.iLegionExp = iCurExp;
	m_legionData.baseInfo.iLegionLevel = iCurLevel;

	// 帮派重新排名
	pLegionFactory->resortRank();
}

void GSLegionImp::saveThreeDayContribute()
{
	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
 	assert(pZoneTime);
	m_legionData.memberInfo.saveContributeVecValue.iValue = 0;
	m_legionData.memberInfo.saveContributeVecValue.dwLastChgTime = pZoneTime->GetCurSecond();
	map<std::string, ServerEngine::LegionMemberItem>::iterator it = m_legionData.memberInfo.memberList.begin();
	for( ; it != m_legionData.memberInfo.memberList.end(); ++it)
	{
		 ServerEngine::LegionMemberItem& memberItem = it->second;
		 memberItem.contributeVec.resize(3,0);
		 memberItem.contributeVec.erase(memberItem.contributeVec.begin());
		 memberItem.contributeVec.resize(3,0);
		 //需要存数据哪里修改
	}
}

int GSLegionImp::getThreeDayContribute(const ServerEngine::LegionMemberItem& memberItem)
{
	/*map<std::string, ServerEngine::LegionMemberItem>::iterator it = m_legionData.memberInfo.memberList.find(strActorName);
	if(m_legionData.memberInfo.memberList.end() == it)
	{
		return 0;
	}
	*/
	// const ServerEngine::LegionMemberItem& memberItem = it->second;
	int iThreeDayContribute = 0;
	assert(memberItem.contributeVec.size() == 3);
	for(size_t i =0; i < memberItem.contributeVec.size(); ++i )
	{
		iThreeDayContribute += memberItem.contributeVec[i];
	}

	return iThreeDayContribute;
	
}


void GSLegionImp::removeLegionMember(string strActorName)
{
	m_legionData.memberInfo.memberList.erase(strActorName);
}

void  GSLegionImp::removeAcotrFromSortList(string strActorName)
{
	vector<string>::iterator it = find(m_sortMemberList.begin(), m_sortMemberList.end(), strActorName);
	if(it != m_sortMemberList.end() )
	{
		m_sortMemberList.erase(it);
	}
}

void GSLegionImp::GMOnlineGetActorInfo(string& strJson)
{
	//{"cmd":"legionmember", "data":["",""]} //玩家名字

	int flag = 0;
	strJson = "{\"cmd\":\"legionmember\",\"data\":[\"";
	map<std::string, ServerEngine::LegionMemberItem>::iterator iter = m_legionData.memberInfo.memberList.begin();
	for(; iter != m_legionData.memberInfo.memberList.end(); ++iter)
	{
		if(flag != 0)
		{
		 	strJson +=",\"";
		}		
		flag = 1;
		string actorName = iter->first;
		strJson =  strJson + actorName + "\"";		
	 }
	strJson += "]}";
	cout<<strJson<<endl;
}

void GSLegionImp::getMemeberNameList(vector<string>& memberNamelist)
{
	map<std::string, ServerEngine::LegionMemberItem>::iterator iter = m_legionData.memberInfo.memberList.begin();
	for(; iter != m_legionData.memberInfo.memberList.end(); ++iter)
	{
		string actorName = iter->first;
		memberNamelist.push_back(actorName);
	 }
}


void GSLegionImp::getMemberVSLegionMemDetail(map<std::string, ServerEngine::LegionMemberItem>& memberList)
{
	memberList =  m_legionData.memberInfo.memberList;
   
}

void GSLegionImp::updateActorFightValue(HEntity hEntity)
{
	
	IEntity *pActor = getEntityFromHandle(hEntity);
	assert(pActor);
	
	string strActorName = pActor->getProperty(PROP_ENTITY_NAME, "");
	int iFightValue =  pActor->getProperty(PROP_ENTITY_FIGHTVALUE, 0);

	map<std::string, ServerEngine::LegionMemberItem>::iterator iter = m_legionData.memberInfo.memberList.find(strActorName);
	if( iter == m_legionData.memberInfo.memberList.end()) return;
	
	 ServerEngine::LegionMemberItem& actorInLegionData = iter->second;

	 actorInLegionData.iFightValue = iFightValue;
	 
}

/*

int GSLegionImp::iGetActorHeadIcon(string strActorname)
{

	map<std::string, ServerEngine::LegionMemberItem>::iterator iter = m_legionData.memberInfo.memberList.find(strActorname);
	if( iter == m_legionData.memberInfo.memberList.end()) return;
	
	 ServerEngine::LegionMemberItem& actorInLegionData = iter->second;

	 return actorInLegionData.iHeadIcon;
}*/

/*int GSLegionImp::iGetActorFightValue(string strActorname)
{
	map<std::string, ServerEngine::LegionMemberItem>::iterator iter = m_legionData.memberInfo.memberList.find(strActorname);
	if( iter == m_legionData.memberInfo.memberList.end()) return;
	
	 ServerEngine::LegionMemberItem& actorInLegionData = iter->second;

	 return actorInLegionData.iFightValue;
}*/



