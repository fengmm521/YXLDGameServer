#ifndef __IFIGHT_SYSTEM_H__
#define __IFIGHT_SYSTEM_H__

#include "IFightFactory.h"


class IFightSystem:public IEntitySubsystem
{
public:

	virtual void AsyncPVEFight(DelegatePVEFight cb, int iMonsterGrpID, const ServerEngine::FightContext& ctx) = 0;

	virtual void AsyncPVPFight(DelegatePVPFight cb, const ServerEngine::PKRole& target, const ServerEngine::FightContext& ctx) = 0;
	
	virtual void AsyncPVPFight(DelegatePVPFight cb, const ServerEngine::ActorCreateContext& targetCtx, const ServerEngine::FightContext& ctx) = 0;

	virtual void AsyncMultPVEFight(DelegatePVEFight cb, vector<int> monsterList, const ServerEngine::FightContext& ctx) = 0;

	virtual void sendBattleMemMsg(const GSProto::FightDataAll& allData) = 0;
	
	virtual void sendBattleAction(const GSProto::FightDataAll& allData) = 0;
	
	virtual void sendBattleFin() = 0;

	virtual void sendAllBattleMsg(const ServerEngine::BattleData& battleData) = 0;

	virtual void sendAllBattleMsg(const ServerEngine::BattleData& battleData, GSProto::SCMessage& resultMsg) = 0;
};

struct GhostSaveGuard
{
	GhostSaveGuard(HEntity hMaster):m_hMaster(hMaster)
	{
		IEntity* pMaster = getEntityFromHandle(hMaster);
		assert(pMaster);

		pMaster->setProperty(PROP_ENTITY_PACKFORGHOST, 1);
	}

	~GhostSaveGuard()
	{
		IEntity* pMaster = getEntityFromHandle(m_hMaster);
		assert(pMaster);

		pMaster->setProperty(PROP_ENTITY_PACKFORGHOST, 0);
	}

	HEntity m_hMaster;
};


#endif
