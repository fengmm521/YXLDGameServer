#ifndef __IBOSS_FACTORY_H__
#define __IBOSS_FACTORY_H__

#include "IFightSystem.h"

typedef TC_Functor<void, TL::TLMaker<taf::Int32, const ServerEngine::BattleData&, int>::Result> DelegateBossFight;

struct BossDamageRecord
{
	BossDamageRecord():iDamageValue(0){}

	ServerEngine::PKRole roleKey;
	string strName;
	int iDamageValue;
};


class IBoss:public IObject, public Detail::EventHandle
{
public:

	virtual bool AsynFightBoss(Uint32 dwActor, const ServerEngine::AttackBossCtx& attackCtx, DelegateBossFight cb) = 0;

	virtual int getBossHP() = 0;

	virtual int getBossMaxHP() = 0;

	virtual void getDamageRankList(int iLimitSize, vector<BossDamageRecord>& rankList) = 0;

	virtual bool getKiller(ServerEngine::PKRole& roleKey, string& strName) = 0;

	virtual int getDamage(const string& strName) = 0;

	virtual Uint32 getCreateTime() = 0;

	virtual int getVisibleMonsterID() = 0;
};


class IBossFactory:public IComponent
{
public:

	// 功能: 创建BOSS
	// 参数: [iMonsterGrp] 怪物组
	// 参数: [pFixCtx] 创建现场
	virtual IBoss* createBoss(int iMonsterGrp, const ServerEngine::CreateBossCtx& bossCtx) = 0;

	// 功能: 删除BOSS
	// 参数: [pBoss] BOSS对象
	virtual void delBoss(IBoss* pBoss) = 0;

	virtual void saveWorldBossData(bool bSync) = 0;

	virtual bool isInitFinish() = 0;
};


#endif
