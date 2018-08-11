#ifndef __BOSS_IMP_H__
#define __BOSS_IMP_H__


struct BossFightOp
{
	HEntity hActor;
	ServerEngine::AttackBossCtx attCtx;
	DelegateBossFight cb;
};


class BossImp:public ObjectBase<IBoss>
{
public:


	BossImp(int iMonsterGrpID, const ServerEngine::CreateBossCtx& bossCtx);
	~BossImp();

	// IBoss Interface
	virtual bool AsynFightBoss(Uint32 dwActor, const ServerEngine::AttackBossCtx& attackCtx, DelegateBossFight cb);
	virtual int getBossHP();
	virtual int getBossMaxHP();
	virtual void getDamageRankList(int iLimitSize, vector<BossDamageRecord>& rankList);
	virtual bool getKiller(ServerEngine::PKRole& roleKey, string& strName);
	virtual int getDamage(const string& strName);
	virtual Uint32 getCreateTime();
	virtual int getVisibleMonsterID();

	void processBattleResult(HEntity hActor, int iRet, const ServerEngine::BattleData& data, DelegateBossFight cb);

private:

	void popNextFight();
	void addDamageRecord(IEntity* pActor, int iDamage);

private:

	int m_iMonsterGrpID;
	ServerEngine::CreateBossCtx m_createCtx;
	vector<BossDamageRecord> m_rankList;
	map<string, int> m_damageMap;
	list<BossFightOp> m_fightOpList;
	
	bool m_bBusy;
	Uint32 m_dwLastFightTime;

	ServerEngine::PKRole m_killerKey;
	string m_strRoleName;
	Uint32 m_dwCreateTime;
};



#endif
