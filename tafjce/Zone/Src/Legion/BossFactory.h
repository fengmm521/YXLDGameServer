#ifndef __BOSS_FACTORY_H__
#define __BOSS_FACTORY_H__

struct WorldBossCfg
{
	WorldBossCfg():iWorldBossID(0), iMonsterGrpID(0), iStartTime(0), iKillGift(0), iKeepSecond(0), iScene(0){}

	int iWorldBossID;
	int iMonsterGrpID;
	int iStartTime;
	int iKillGift;
	int iKeepSecond;
	int iScene;
};


struct WorldBossDamageState
{
	WorldBossDamageState():dwLastDeadTime(0), iCheerCount(0), iSumSilver(0), iSumHonor(0){}

	ServerEngine::PKRole roleKey;
	Uint32 dwLastDeadTime;
	int iCheerCount;
	int iSumSilver;
	int iSumHonor;
};


struct AddBossGift
{
	AddBossGift(const ServerEngine::PKRole& roleKey, const string& strName, int iGiftID, int iRank, bool bLegionBoss):m_roleKey(roleKey), m_strName(strName), 
		m_iGiftID(iGiftID), m_iRank(iRank), m_bLegionBoss(bLegionBoss){}

	void operator()(int iRet, HEntity hEntity, ServerEngine::PIRole& roleInfo);
	
private:

	ServerEngine::PKRole m_roleKey;
	string m_strName;
	int m_iGiftID;
	int m_iRank;
	bool m_bLegionBoss;
};


class BossFactory:public ComponentBase<IBossFactory, IID_IBossFactory>, public ITimerCallback
{
public:

	BossFactory();
	~BossFactory();

	// IComponent Interface
	virtual bool initlize(const PropertySet& propSet);

	// ITimerCallback
	virtual void onTimer(int nEventId);

	// IBossFactory Interface
	virtual IBoss* createBoss(int iMonsterGrp, const ServerEngine::CreateBossCtx& bossCtx);
	virtual void delBoss(IBoss* pBoss);
	virtual void saveWorldBossData(bool bSync);
	virtual bool isInitFinish();

	void checkBossStart();
	void checkBossTimeout();
	void loadWorldBossCfg();
	IBoss* createWorldBoss(const WorldBossCfg& worldBossCfg);
	void onWorldBossEnd(int iBossID, IBoss* pBoss, HEntity hKiller);
	void cheerFightBoss(HEntity hActor, int iBossID);
	void fightWorldBoss(HEntity hActor, int iBossID);
	WorldBossDamageState* getWorldBossState(HEntity hActor, int iBossID);
	void processBossFight(HEntity hActort, int iBossID, const ServerEngine::BattleData& battleData, int iDamage);
	void relivePlayer(HEntity hActor, int iBossID);
	int getReliveCD(HEntity hActor, int iBossID);
	void calcWorldBossGrow(int iBossID, Uint32 dwCostMin);
	void doKillAward(int iBossID, HEntity hKiller);
	void doRankAward(IBoss* pBoss);

	void sendBossInfo(HEntity hActor);
	void fillPreBossInfo(GSProto::WorldBossPreStart* pScPreStartInfo, int& iBossMonsterID, int& iLevel);
	void fillBossFightInfo(HEntity hActor, GSProto::WorldBossFightInfo* pScFightInfo, int& iBossMonsterID, int& iLevel);
	void onReqCheerWorldBoss(HEntity hActor);
	void onReqFightWorldBoss(HEntity hActor);
	void onReqRelive(HEntity hActor);
	void broadcastUpdate();
	void broadcastKill(HEntity hKiller, int iBossID,  const GSProto::FightAwardResult& resoult);

private:

	set<IBoss*> m_bossList;
	
	ITimerComponent::TimerHandle m_hCheckBossHandle;
	ITimerComponent::TimerHandle  m_hSaveHandle;
	
	map<int, WorldBossCfg> m_bossCfgMap;
	ServerEngine::GlobalWorldBossData m_bossLevelInfo;
	map<int, IBoss*> m_enabledBossList;
	map<int, map<string, WorldBossDamageState> > m_bossFightStateMap;

	bool m_bInitFinish;
};


#endif
