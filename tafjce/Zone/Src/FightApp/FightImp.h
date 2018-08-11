#ifndef __FIGHT_IMP__
#define __FIGHT_IMP__

#include "MiniApr.h"
using namespace MINIAPR;


#include "servant/Application.h"
#include "Fight.h"

class FightImp:public ServerEngine::Fight
{
public:

	FightImp();
	~FightImp();

	// Servant Interface
	virtual void initialize();
    virtual void destroy();
	virtual int doCustomMessage(bool bExpectIdle = false);

	// Game Interface
	virtual taf::Int32 doPVEFight(ServerEngine::BattleData &battleInfo,const ServerEngine::ActorCreateContext & roleInfo,taf::Int32 iMonsterGrpID,const ServerEngine::FightContext & ctx,taf::JceCurrentPtr current);
	virtual taf::Int32 doPVPFight(ServerEngine::BattleData &battleInfo,const ServerEngine::ActorCreateContext & attRoleInfo,const ServerEngine::ActorCreateContext & targetRileInfo,const ServerEngine::FightContext & ctx,taf::JceCurrentPtr current);
	virtual taf::Int32 doMultiPVEFight(ServerEngine::BattleData &battleInfo,const ServerEngine::ActorCreateContext & roleInfo,const vector<taf::Int32> & monsterGrpList,const ServerEngine::FightContext & ctx,taf::JceCurrentPtr current);
	virtual taf::Int32 continuePVEFight(ServerEngine::BattleData &battleInfo,const ServerEngine::ActorCreateContext & roleInfo,taf::Int32 iMonsterGrpID,const ServerEngine::FightContext & ctx, taf::JceCurrentPtr current);
    virtual taf::Int32 continueMultiPVEFight(ServerEngine::BattleData &battleInfo,const ServerEngine::ActorCreateContext & roleInfo,const vector<taf::Int32> & monsterGrpList,const ServerEngine::FightContext & ctx,taf::JceCurrentPtr current);
	virtual taf::Int32 continuePVPFight(ServerEngine::BattleData &battleInfo,const ServerEngine::ActorCreateContext & attRoleInfo,const ServerEngine::ActorCreateContext & targetRileInfo,const ServerEngine::FightContext & ctx,taf::JceCurrentPtr current);
};

#endif

