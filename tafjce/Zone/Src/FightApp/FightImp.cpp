#include "FightImp.h"
#include "MiniApr.h"
#include "define.h"
#include "comm_func.h"
#include "CommLogic.h"
#include "svr_log.h"

#include "GameServer.pb.h"
#include "GameServerExt.pb.h"
#include "MessageWrapper.h"
#include "IUserStateManager.h"
#include "IEntityFactory.h"
#include "IFightFactory.h"
#include "ComponentConstDef.h"
#include "ComponentIdDef.h"
#include "ErrorHandle.h"


taf::Servant* g_pMainServant = NULL;
Uint32 g_lastUpdateTime = 0;
static bool s_bInitlizeFinish = false;

bool isGameServer(){return false;}
bool isInitlizeFinish(){return s_bInitlizeFinish;}



FightImp::FightImp()
{
}

FightImp::~FightImp()
{
}

void FightImp::initialize()
{
	g_pMainServant = this;

	//忽略SIGPIPE
	TC_Common::ignorePipe();

	// 禁止部分组件在FightServer上运行
	getCompomentObjectManager()->disableComponent(COMPNAME_LegionFactory);
	getCompomentObjectManager()->disableComponent(COMPNAME_ArenaFactory);
	getCompomentObjectManager()->disableComponent(COMPNAME_MessageLayer);
	getCompomentObjectManager()->disableComponent(COMPNAME_BossFactory);
	getCompomentObjectManager()->disableComponent(COMPNAME_OperateFactory);
	
	bool bResult = getCompomentObjectManager()->loadScheme("FightServer.xml");
	if(!bResult)
	{
		assert(0);
		return;
	}

	s_bInitlizeFinish = true;
}

void FightImp::destroy()
{
}

taf::Int32 FightImp::doPVEFight(ServerEngine::BattleData &battleInfo,const ServerEngine::ActorCreateContext & roleInfo,taf::Int32 iMonsterGrpID,const ServerEngine::FightContext & ctx,taf::JceCurrentPtr current)
{
	PROFILE_MONITOR("doPVEFight");
	try
	{
		IFightFactory* pFightFactory = getComponent<IFightFactory>(COMPNAME_FightFactory, IID_IFightFactory);
		assert(pFightFactory);

		int iRet = pFightFactory->doPVEFight(battleInfo, roleInfo, iMonsterGrpID, ctx);

		return iRet;
	}
	__COMMON_EXCEPTION_CATCH_EXT__("doPVEFight");

	return ServerEngine::en_FightRet_SysError;
}

taf::Int32 FightImp::doPVPFight(ServerEngine::BattleData &battleInfo,const ServerEngine::ActorCreateContext & attRoleInfo,const ServerEngine::ActorCreateContext & targetRileInfo,const ServerEngine::FightContext & ctx,taf::JceCurrentPtr current)
{
	PROFILE_MONITOR("doPVPFight");
	try
	{
		IFightFactory* pFightFactory = getComponent<IFightFactory>(COMPNAME_FightFactory, IID_IFightFactory);
		assert(pFightFactory);

		int iRet = pFightFactory->doPVPFight(battleInfo, attRoleInfo, targetRileInfo, ctx);

		return iRet;
	}
	__COMMON_EXCEPTION_CATCH_EXT__("doPVPFight");

	return ServerEngine::en_FightRet_SysError;
}

taf::Int32 FightImp::doMultiPVEFight(ServerEngine::BattleData &battleInfo,const ServerEngine::ActorCreateContext & roleInfo, const vector<taf::Int32> & monsterGrpList,const ServerEngine::FightContext & ctx,taf::JceCurrentPtr current)
{
	PROFILE_MONITOR("doMultiPVEFight");
	try
	{
		IFightFactory* pFightFactory = getComponent<IFightFactory>(COMPNAME_FightFactory, IID_IFightFactory);
		assert(pFightFactory);

		int iRet = pFightFactory->doMultiPVEFight(battleInfo, roleInfo, monsterGrpList, ctx);

		return iRet;
	
	}
	__COMMON_EXCEPTION_CATCH_EXT__("doMultiPVEFight");

	return ServerEngine::en_FightRet_SysError;
}


taf::Int32 FightImp::continuePVEFight(ServerEngine::BattleData &battleInfo,const ServerEngine::ActorCreateContext & roleInfo,taf::Int32 iMonsterGrpID,const ServerEngine::FightContext & ctx, taf::JceCurrentPtr current)
{
	PROFILE_MONITOR("continuePVEFight");
	try
	{
		IFightFactory* pFightFactory = getComponent<IFightFactory>(COMPNAME_FightFactory, IID_IFightFactory);
		assert(pFightFactory);

		int iRet = pFightFactory->continuePVEFight(battleInfo, roleInfo, iMonsterGrpID, ctx);

		return iRet;
	}
	__COMMON_EXCEPTION_CATCH_EXT__("continuePVEFight");

	return ServerEngine::en_FightRet_SysError;
}

taf::Int32 FightImp::continuePVPFight(ServerEngine::BattleData &battleInfo,const ServerEngine::ActorCreateContext & attRoleInfo,const ServerEngine::ActorCreateContext & targetRileInfo,const ServerEngine::FightContext & ctx,taf::JceCurrentPtr current)
{
	PROFILE_MONITOR("continuePVPFight");

	try
	{
		IFightFactory* pFightFactory = getComponent<IFightFactory>(COMPNAME_FightFactory, IID_IFightFactory);
		assert(pFightFactory);

		int iRet = pFightFactory->continuePVPFight(battleInfo, attRoleInfo, targetRileInfo, ctx);

		return iRet;
	}
	__COMMON_EXCEPTION_CATCH_EXT__("continuePVPFight");

	return ServerEngine::en_FightRet_SysError;
}


taf::Int32 FightImp::continueMultiPVEFight(ServerEngine::BattleData &battleInfo,const ServerEngine::ActorCreateContext & roleInfo,const vector<taf::Int32> & monsterGrpList,const ServerEngine::FightContext & ctx, taf::JceCurrentPtr current)
{
	PROFILE_MONITOR("continueMultiPVEFight");
	try
	{
		IFightFactory* pFightFactory = getComponent<IFightFactory>(COMPNAME_FightFactory, IID_IFightFactory);
		assert(pFightFactory);

		int iRet = pFightFactory->continueMultiPVEFight(battleInfo, roleInfo, monsterGrpList, ctx);

		return iRet;
	}
	__COMMON_EXCEPTION_CATCH_EXT__("continueMultiPVEFight");

	return ServerEngine::en_FightRet_SysError;
}


int FightImp::doCustomMessage(bool bExpectIdle)
{
	ITimerComponent* pTimeAxis = getComponent<ITimerComponent>(COMPNAME_TimeAxis, IID_ITimerComponent);
	assert(pTimeAxis);

	pTimeAxis->runTimer();

	return -1;
}



