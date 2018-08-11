#ifndef __ARENA_FACTORY_H__
#define __ARENA_FACTORY_H__


struct ArenaAwardCfg
{
	ArenaAwardCfg():iMinRank(0), iMaxRank(0), iSilver(0), iHonor(0), iGold(0){}

	int iMinRank;
	int iMaxRank;
	int iSilver;
	int iHonor;
	int iGold;
};

struct RandRuleUnit
{
	RandRuleUnit():iLowRankNum(0),iHightRankNum(0)
	{
		rankSet1.clear();
		rankSet2.clear();
	}
	int iLowRankNum;
	int iHightRankNum;
	vector<int> rankSet1;
	vector<int> rankSet2;
};
	
class ArenaFactory:public ComponentBase<IArenaFactory, IID_IArenaFactory>, public ITimerCallback
{
public:

	ArenaFactory();
	~ArenaFactory();

	// IComponent Interface
	virtual bool initlize(const PropertySet& propSet);

	// IArenaFactory Interface
	virtual int getArenaRank(const string& strName);
	virtual void saveArenaData(bool bSync);
	virtual void getDomainList(HEntity hActor, vector<ServerEngine::ArenaSavePlayer>& playerList);
	virtual void moveArenaAward(const string& strActorName, vector<ServerEngine::MailData>& mailDataList);
	virtual bool isInitFinish();
	
	void loadArenaAward();

	// ITimerCallback Interface
	virtual void onTimer(int nEventId);

	// 功能: 加载竞技场数据
	void loadArenaData();

	// 功能: 和怪物战斗
	bool fightArenaRank(HEntity hActor, int iRank, DelegatePVEFight cb);

	// 功能: 初始化竞技场
	void initArenaWithMonster(bool bFillRank);

	void randActorChallenge(HEntity hActor, vector<GSProto::ArenaPlayer>& challengeList);

	void addArenaPlayer(vector<GSProto::ArenaPlayer>& challengeList, int iIndex);

	void generatePlayerList(vector<GSProto::ArenaPlayer>& challengeList, const RandRuleUnit& unit, int iActorRank);

	void fillActorAward(HEntity hActor, google::protobuf::RepeatedPtrField<GSProto::ArenaAwardItem>* pszAwardItem);

	bool getRankAward(int iRank, int& iHonor, int& iSilver, int& iGold);

	int getNextAwardCD();

	bool doGetArenaAward(HEntity hActor, const string& strUUID);

	void sendTopRankList(HEntity hActor);

	void processArenaFight(HEntity hActor, int iRet, int iSelfRank, int iTargetRank, const ServerEngine::BattleData& battleData, DelegatePVEFight cb);

	void fillActorSaveInfo(HEntity hActor, ServerEngine::ArenaSavePlayer& arenaPlayer);

	void updateActorInfo(HEntity hActor);

	void checkArenaAward();

	void broadcastTopOne(HEntity hActor, const string& strTargetName);

	bool hasAwardCanGet(HEntity hActor);

	void rankFightValue();
	virtual bool getDreamLandList(HEntity hEntity, ServerEngine::ArenaSavePlayer& player, int imin, int iMax);
	//virtual void getDomainRandomActorList(HEntity hActor, vector<ServerEngine::ArenaSavePlayer>&playerList, int ivecSize);
	virtual bool getManorWillLootActor(HEntity hActor, ServerEngine::ArenaSavePlayer& player);
	const ArenaAwardCfg* queryArenaAwardCfg(int iRank);
	virtual void loadRandRuleList();
	void getPlayerList(vector<GSProto::ArenaPlayer>& challengeList,const vector<int>& randSet, int iActorRank);

	virtual void updateActorBeManorLootInfo(HEntity hActor);

	virtual bool getActorForCityBattle(vector<ServerEngine::ArenaSavePlayer>&playerList, int iCount);

private:

	ServerEngine::GlobalArenaData m_globalArenaData;
	map<string, int> m_arenaRankMap;
	set<int> m_busyTarget;

	vector<ServerEngine::ArenaSavePlayer> m_fightValueList;
	vector<ArenaAwardCfg> m_arenaAwardCfgList;
	vector<RandRuleUnit> m_RandRuleList;
	
	ITimerComponent::TimerHandle m_hHandle;
	ITimerComponent::TimerHandle m_checkAwardHandle;
	ITimerComponent::TimerHandle m_hInitHandle;
	
	ITimerComponent::TimerHandle m_hRankFightValue;

	vector<int> m_ManorRandChanceVec;
	int m_iManorRandTotalChance;

	bool m_bInitFinish;

	vector<string> m_GuardMachineNameList;
};


#endif
