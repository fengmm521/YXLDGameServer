#include "ClimbTowerSystemPch.h"
#include "ClimbTowerSystem.h"
#include "ClimbTowerFactory.h"
#include "ErrorCodeDef.h"

extern "C" IObject* createClimbTowerSystem()
{
	return new ClimbTowerSystem;
}
ClimbTowerSystem::ClimbTowerSystem()
{
}
ClimbTowerSystem:: ~ClimbTowerSystem()
{
}
Uint32 ClimbTowerSystem::getSubsystemID() const
{
	return IID_IClimbTowerSystem;
}
Uint32 ClimbTowerSystem::getMasterHandle()
{
	return m_hEntity;
}
bool ClimbTowerSystem::create(IEntity* pEntity, const std::string& strData){
	m_hEntity = pEntity->getHandle();//通天塔总层数
	
	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);
	m_resetBaseCount =  pGlobalCfg->getInt("通天塔基础次数",1);
	
	if(strData.size()>0)
	{
		ServerEngine::JceToObj(strData,m_climbTowerSystemDBData);
		checkNeedRest();
	}
	
	m_towerLayerCount = pGlobalCfg->getInt("通天塔总层数",100);
	
	return true;	
}

bool ClimbTowerSystem::createComplete(){
	return true;
}
const std::vector<Uint32>& ClimbTowerSystem::getSupportMessage()
{
	static vector<Uint32> messageList;
	if( messageList.size() == 0 )
	{
		messageList.push_back(GSProto::CMD_QUERY_CLIMBTOWERDETAIL);
		messageList.push_back(GSProto::CMD_RESET_CLIMBTOWER);
		messageList.push_back(GSProto::CMD_CHALLENGE_TOWER);
		messageList.push_back(GSProto::CMD_FAST_CHALLENGE_TOWER);
		messageList.push_back(GSProto::CMD_TOWER_ADD_RESETCOUNT);
	}
	return messageList;
}
void ClimbTowerSystem::onMessage(QxMessage* pMessage)
{
	checkNeedRest();
	assert(pMessage->dwMsgLen == sizeof(GSProto::CSMessage) );
	const GSProto::CSMessage& msg = *(const GSProto::CSMessage*)(pMessage->pMsgDataBuff);
	switch(msg.icmd() )
	{
		case GSProto::CMD_QUERY_CLIMBTOWERDETAIL:
			onQueryClimbTowerDetail(msg);
			break;

		case GSProto::CMD_RESET_CLIMBTOWER:
			onResetClimbTower(msg);
			break;

		case GSProto::CMD_CHALLENGE_TOWER:
			onDirectChallengeTowerLayer(msg);
			break;
		case GSProto::CMD_FAST_CHALLENGE_TOWER:
			onFastChallengeTowerLayer(msg);
			break;
		case GSProto::CMD_TOWER_ADD_RESETCOUNT:
			onAddTowerResetCount(msg);
			break;
				
	}
}
void ClimbTowerSystem::onAddTowerResetCount(const GSProto::CSMessage&  detail )
{
	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);
	
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	IVIPFactory* factory = getComponent<IVIPFactory>(COMPNAME_VIPFactory, IID_IVIPFactory);
	assert(factory);
	
	if( m_climbTowerSystemDBData.iCurHaveBuyTimes >= factory->getVipPropByHEntity(m_hEntity,VIP_PROP_CLIMBTOWER_TIMES))
	{
		pEntity->sendErrorCode(ERROR_CLIMBTOWER_NOBUYTIMES);
		return;
	}

	//发送确认信息
	IScriptEngine* pScriptEngine = getComponent<IScriptEngine>(COMPNAME_ScriptEngine, IID_IScriptEngine);
	assert(pScriptEngine);

	
	int iEnableCostGold = pGlobalCfg->getInt("通天塔增加重置次数消耗", 100);
	
	EventArgs args;
	args.context.setInt("entity", m_hEntity);
	args.context.setInt("costGold", iEnableCostGold);

	pScriptEngine->runFunction("AddTowerRestCount", &args, "EventArgs");
}

void ClimbTowerSystem::addTowerResetCount(bool ensuer)
{
	if(!ensuer)
	{
		return;
	}
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	//扣钱 存日志
	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);	
	assert(pGlobalCfg);	
	int costGold = pGlobalCfg->getInt("通天塔增加重置次数消耗", 100);
	if(pEntity->getProperty(PROP_ACTOR_GOLD,0) < costGold )
	{
		pEntity->sendErrorCode(ERROR_NEED_GOLD);
		return;
	}
	IVIPFactory* factory = getComponent<IVIPFactory>(COMPNAME_VIPFactory, IID_IVIPFactory);
	assert(factory);
	
	if(m_climbTowerSystemDBData.iCurHaveBuyTimes >= factory->getVipPropByHEntity(m_hEntity,VIP_PROP_CLIMBTOWER_TIMES) )
	{
		return;
	}
	
	
	// 扣除金币,记录日志
	PLAYERLOG(pEntity)<<"CostGold|"<<costGold<<"|"<<"ClimbTowerSystem"<<endl;
	pEntity->changeProperty(PROP_ACTOR_GOLD, 0-costGold, GSProto::en_Reason_addTowerRestCount);
	
	//重置次数加一次
	m_climbTowerSystemDBData.iCurHaveBuyTimes += 1;
	m_climbTowerSystemDBData.iTotalBuyTimes += 1;
	
	//更新客户端
	GSProto::CMD_QUERY_CLIMBTOWERDETAIL_SC scMsg;
	onFillClimbTowerDetail(*scMsg.mutable_detail());
	//装包发送
	GSProto::SCMessage pkg;
	HelpMakeScMsg(pkg,GSProto::CMD_QUERY_CLIMBTOWERDETAIL,scMsg);
	
	
	pEntity->sendMessage(pkg);
}

//查询爬塔信息
void ClimbTowerSystem::onQueryClimbTowerDetail(const GSProto::CSMessage& message)
{
	GSProto::CMD_QUERY_CLIMBTOWERDETAIL_SC scMsg;
	
	onFillClimbTowerDetail(*scMsg.mutable_detail());
	//装包发送
	GSProto::SCMessage pkg;
	HelpMakeScMsg(pkg,GSProto::CMD_QUERY_CLIMBTOWERDETAIL,scMsg);
	
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	pEntity->sendMessage(pkg);
	
}

void ClimbTowerSystem::onFillClimbTowerDetail(GSProto::ClimbTowerDatail & detail )
{
	//获取下层塔能得到的修为
	IClimbTowerFactory *factory = getComponent<IClimbTowerFactory>("ClimbTowerFactory",IID_IClimbTowerFactory);
	assert(factory);
	int nextLayerCultivation = factory->getNextLayerCanGetExp(m_climbTowerSystemDBData.iCurrentLayer);
	
	detail.set_inextlayercultivation(nextLayerCultivation);
	detail.set_iclimbresetcount( m_climbTowerSystemDBData.iTotalBuyTimes + m_resetBaseCount - m_climbTowerSystemDBData.iClimbResetCount);
	detail.set_icurrentlayer(m_climbTowerSystemDBData.iCurrentLayer);
	detail.set_ihightestlayer(m_climbTowerSystemDBData.iHightestLayer);
	detail.set_itotalcultivation(m_climbTowerSystemDBData.iTotalCultivation);
	detail.set_itotallayers(m_towerLayerCount);
}

// 重置
void ClimbTowerSystem::onResetClimbTower(const GSProto::CSMessage& message)
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	//0层不能重置
	if( 0 == m_climbTowerSystemDBData.iCurrentLayer)
	{
		pEntity->sendErrorCode(ERROR_IN_BOTTOM);
		return;
	}
		
 	//重置次数 不够判读元宝够不够，够提示扣钱
 	if( m_resetBaseCount + m_climbTowerSystemDBData.iTotalBuyTimes <=  m_climbTowerSystemDBData.iClimbResetCount)
 	{
 		pEntity->sendErrorCode(ERROR_LACK_RESETCOUNT);
 		return;
 	}

	
	if( m_climbTowerSystemDBData.iClimbResetCount >= m_resetBaseCount)
	{
		m_climbTowerSystemDBData.iTotalBuyTimes--;
	}
	else
	{
		m_climbTowerSystemDBData.iClimbResetCount ++; 
	}
	
	m_climbTowerSystemDBData.iCurrentLayer = 0;
	m_climbTowerSystemDBData.iTotalCultivation = 0;

	//处理完成发送给客户端
	GSProto::CMD_QUERY_CLIMBTOWERDETAIL_SC scMsg;
	
	onFillClimbTowerDetail(*scMsg.mutable_detail());
	//装包发送
	GSProto::SCMessage pkg;
	HelpMakeScMsg(pkg,GSProto::CMD_QUERY_CLIMBTOWERDETAIL,scMsg);
	
	pEntity->sendMessage(pkg);
	
}

struct ClimbTowerFightCb
{
	ClimbTowerFightCb(HEntity hEntity, int iSceneID):m_hEntity(hEntity), m_iSceneID(iSceneID){}

	void operator() (int iRet, const ServerEngine::BattleData& data)
	{
		IEntity* pEntity = getEntityFromHandle(m_hEntity);
		if(!pEntity) return;
		
		ClimbTowerSystem* pClimbTowerSys = static_cast<ClimbTowerSystem*>(pEntity->querySubsystem(IID_IClimbTowerSystem));
		if( !pClimbTowerSys ) return;
		
		
		//战斗处理
		pClimbTowerSys->processClimbTowerChallengeRes(m_iSceneID,iRet,data);
	}

	HEntity m_hEntity;
	int m_iSceneID;
};

//直接挑战
void ClimbTowerSystem::onDirectChallengeTowerLayer(const GSProto::CSMessage& message)
{
	GSProto::CMD_CHALLENGE_TOWER_CS reqMsg;
	if( !reqMsg.ParseFromString( message.strmsgbody()) )
	{
		return;
	}
	int clientWillChallengeLayerId = reqMsg.itowerlayerid();

	//客户端数据不合法
	if(clientWillChallengeLayerId<0 || clientWillChallengeLayerId > m_towerLayerCount)
	{
		return;
	}
	
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	//当前层在最高层不能挑战
	if( m_climbTowerSystemDBData.iCurrentLayer == m_towerLayerCount)
	{
		pEntity->sendErrorCode(ERROR_IN_TOWERTOP);
		return ;
	}
	
	// 验证 挑战 合法性
	if( clientWillChallengeLayerId != m_climbTowerSystemDBData.iCurrentLayer + 1)
	{
		return;
	}
	
	// 进入战斗
	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);	
	assert(pGlobalCfg);	
	int iScene = pGlobalCfg->getInt("通天塔场景", 1);
	ServerEngine::FightContext ctx;
	ctx.iFightType = GSProto::en_FightType_ClimbTower;
	ctx.iSceneID = iScene;
		
    IFightSystem* pFightSys = static_cast<IFightSystem*>(pEntity->querySubsystem(IID_IFightSystem));
	assert(pFightSys);
	
	IClimbTowerFactory *factory = getComponent<IClimbTowerFactory>("ClimbTowerFactory",IID_IClimbTowerFactory);
	assert(factory);
	pFightSys->AsyncMultPVEFight(ClimbTowerFightCb(m_hEntity, iScene), factory->getLayerMonsterGropIdList(clientWillChallengeLayerId), ctx);
	
}

//快速挑战
void ClimbTowerSystem::onFastChallengeTowerLayer(const GSProto::CSMessage& message)
{
	GSProto::CMD_FAST_CHALLENGE_TOWER_CS reqMsg;
	if( !reqMsg.ParseFromString( message.strmsgbody()) )
	{
		return;
	}
	int clientWillChallengeLayerId = reqMsg.itowerlayerid();

	//客户端数据不合法
	if(clientWillChallengeLayerId<0 || clientWillChallengeLayerId > m_towerLayerCount)
	{
		return;
	}
	
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	//当前层在最高层不能挑战
	if( m_climbTowerSystemDBData.iCurrentLayer == m_towerLayerCount)
	{
		pEntity->sendErrorCode(ERROR_IN_TOWERTOP);

		//todo 
		return ;
	}
	
	// 验证 挑战 合法性
	if( clientWillChallengeLayerId > m_climbTowerSystemDBData.iHightestLayer)
	{
		pEntity->sendErrorCode(ERROR_CLiMTOWER_HAVENOT_PAST);
		
		GSProto::CMD_TOWER_FAST_CHALLENGE_RESOULT_SC fastChallengeRes;
		fastChallengeRes.set_biswin(0);
		fastChallengeRes.set_ichallengelayerid(0);
		
		pEntity->sendMessage(GSProto::CMD_TOWER_FAST_CHALLENGE_RESOULT ,fastChallengeRes);
		return;
	}

	//关闭上浮
	CloseAttCommUP close(m_hEntity);
	
	// 胜利了，计算奖励
	//胜利爬升一层
	IClimbTowerFactory *factory = getComponent<IClimbTowerFactory>("ClimbTowerFactory",IID_IClimbTowerFactory);
	assert(factory);

	int haveExp = factory->getNextLayerCanGetExp(m_climbTowerSystemDBData.iCurrentLayer);
	m_climbTowerSystemDBData.iTotalCultivation += haveExp;
	m_climbTowerSystemDBData.iCurrentLayer += 1;
	if(m_climbTowerSystemDBData.iCurrentLayer > m_climbTowerSystemDBData.iHightestLayer)
	{
		m_climbTowerSystemDBData.iHightestLayer = m_climbTowerSystemDBData.iCurrentLayer;
	}

	//pEntity->changeProperty(PROP_ENTITY_HEROEXP,haveExp,GSProto::en_Reason_ClimTowerChallengeAward);
	
	
	GSProto::CMD_TOWER_FAST_CHALLENGE_RESOULT_SC fastChallengeRes;
	fastChallengeRes.set_biswin(1);
	fastChallengeRes.set_ichallengelayerid(m_climbTowerSystemDBData.iCurrentLayer);

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);	
	int iScene = pGlobalCfg->getInt("通天塔场景", 1);
	processChallengeAward(iScene, *fastChallengeRes.mutable_awrdres());
	
	// 通知客户端战斗结果和奖励结果
	GSProto::SCMessage pkg;
	HelpMakeScMsg(pkg, GSProto::CMD_TOWER_FAST_CHALLENGE_RESOULT, fastChallengeRes);

	//装包发送
	pEntity->sendMessage(pkg);	
}

void ClimbTowerSystem::packSaveData(string& data)
{
	data = ServerEngine::JceToStr(m_climbTowerSystemDBData);
}


void ClimbTowerSystem::processClimbTowerChallengeRes(int iScenID, int iRet, const ServerEngine::BattleData& data)
{

	if(iRet == en_FightResult_SysError)
	{
		SvrErrLog("ClimbTower Fight Error|%d", iScenID);
		return;
	}

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	// 回一个quit吧
	if(data.bQuit != 0)
	{
		pEntity->sendMessage(GSProto::CMD_FIGHT_QUIT);
		((ServerEngine::BattleData&)data).bQuitSuccess = 1;
	}

	IFightSystem* pFightSys = static_cast<IFightSystem*>(pEntity->querySubsystem(IID_IFightSystem));
	assert(pFightSys);

	//挑战失败
	if(!data.bAttackerWin)
	{
		GSProto::Cmd_Sc_CommFightResult scFightResult;
		scFightResult.set_isceneid(iScenID);
		scFightResult.set_iissuccess(0);

		pEntity->sendMessage(GSProto::CMD_COMM_FIGHTRESULT, scFightResult);
		pFightSys->sendBattleFin();
		return;
	}

	//关闭上浮
	CloseAttCommUP close(m_hEntity);
	
	// 胜利了，计算奖励
	//胜利爬升一层
	IClimbTowerFactory *factory = getComponent<IClimbTowerFactory>("ClimbTowerFactory",IID_IClimbTowerFactory);
	assert(factory);
	int haveExp = factory->getNextLayerCanGetExp(m_climbTowerSystemDBData.iCurrentLayer);
	m_climbTowerSystemDBData.iTotalCultivation += haveExp;
	m_climbTowerSystemDBData.iCurrentLayer += 1;
	if(m_climbTowerSystemDBData.iCurrentLayer > m_climbTowerSystemDBData.iHightestLayer)
	{
		m_climbTowerSystemDBData.iHightestLayer = m_climbTowerSystemDBData.iCurrentLayer;
	}


	//pEntity->changeProperty(PROP_ENTITY_HEROEXP,haveExp,GSProto::en_Reason_ClimTowerChallengeAward);
	
	GSProto::Cmd_Sc_CommFightResult scCommFightResult;
	scCommFightResult.set_isceneid(iScenID);
	scCommFightResult.set_iissuccess(1);
	scCommFightResult.set_istar(data.iStar);

	processChallengeAward(iScenID, *scCommFightResult.mutable_awardresult() );
	
	// 通知客户端战斗结果和奖励结果
	pEntity->sendMessage(GSProto::CMD_COMM_FIGHTRESULT, scCommFightResult);
	pFightSys->sendBattleFin();

	//任务系统
	{
		EventArgs args;
		args.context.setInt("toplayer",m_climbTowerSystemDBData.iHightestLayer);
		args.context.setInt("entity",m_hEntity);
		pEntity->getEventServer()->setEvent(EVENT_ENTITY_TASKFINISH_CLIMBTOWER, args);
	}
}

bool ClimbTowerSystem::canSkip()
{
	return m_climbTowerSystemDBData.iHightestLayer > m_climbTowerSystemDBData.iCurrentLayer;
}

void ClimbTowerSystem::processChallengeAward(int iScenID, GSProto::FightAwardResult& awardResult)
{
	IClimbTowerFactory *factory = getComponent<IClimbTowerFactory>("ClimbTowerFactory",IID_IClimbTowerFactory);
	assert(factory);

	IDropFactory* pDropFactory = getComponent<IDropFactory>(COMPNAME_DropFactory, IID_IDropFactory);
	assert(pDropFactory);

   //掉落奖励
   vector<int> dropIdVec = factory->getLayerDropIdVec(m_climbTowerSystemDBData.iCurrentLayer + 1);
   for(size_t i = 0; i < dropIdVec.size(); ++i)
   	{
   		pDropFactory->calcDrop(dropIdVec[i], awardResult);
   	}
	
	// 经验奖励
	pDropFactory->addPropToResult(awardResult, GSProto::en_LifeAtt_HeroExp, factory->getNextLayerCanGetExp(m_climbTowerSystemDBData.iCurrentLayer-1));

	// 执行奖励
	pDropFactory->excuteDrop(m_hEntity, awardResult, GSProto::en_Reason_ClimTowerChallengeAward);
}

void  ClimbTowerSystem::checkNeedRest()
{
	IZoneTime* pZomeTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZomeTime);

	if(!pZomeTime->IsInSameDay(m_climbTowerSystemDBData.dayRestFlag.dwLastChgTime, pZomeTime->GetCurSecond() ) )
	 {
	   //通天塔重置次数
	   IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
		assert(pGlobalCfg);
		
	   m_climbTowerSystemDBData.dayRestFlag.iValue = true;
	   m_climbTowerSystemDBData.iClimbResetCount= 0;
	   m_climbTowerSystemDBData.iCurHaveBuyTimes = 0;
	   m_climbTowerSystemDBData.dayRestFlag.dwLastChgTime = pZomeTime->GetCurSecond();
	}
}

int ClimbTowerSystem::getTowerTop()
{
	return m_climbTowerSystemDBData.iHightestLayer;
}


