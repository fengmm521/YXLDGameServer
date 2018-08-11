#ifndef __LEGION_IMP_H__
#define __LEGION_IMP_H__


struct LegionLevelInfo;

struct MemberLegionBossState
{
	MemberLegionBossState():dwLastDeadTime(0), iCheerCount(0), iSumSilver(0), iSumHonor(0){}

	Uint32 dwLastDeadTime;
	int iCheerCount;
	int iSumSilver;
	int iSumHonor;
};

class GSLegionImp:public ObjectBase<ILegion>, public ITimerCallback, public Detail::EventHandle
{
public:

	friend class LegionFactory;
	//friend class LegionCityImp;

	GSLegionImp();
	~GSLegionImp();

	// ILegion Interface
	virtual bool initFromDB(const ServerEngine::LegionData& data);
	virtual Uint32 getHandle();
	virtual string getLegionName();
	virtual void onMessage(HEntity hActor, const GSProto::CSMessage& msg);
	virtual void broadcastMessage(const GSProto::SCMessage& msg);
	virtual void saveData(bool bSync);
	virtual void gmSetLevel(int iLevel);
	virtual int iGetRequestListSize();
	// ITimerCallback Interface
	virtual void onTimer(int nEventId);

	//GMOnline
	virtual void GMOnlineGetActorInfo(string& strJson);

	void sendSimpleInfo(HEntity hActor);
	void sendBaseInfo(HEntity hActor);
	void applyJoin(HEntity hActor);
	void cancelApp(HEntity hActor);
	string getLeaderName();
	int getRank();
	int getMemberLimit();
	int getViceCount();
	int getViceMemberLimit();
	void updateActorInfo(HEntity hActor);
	void actorLoginOut(HEntity hActor,bool bIsOut);
	Uint32 getLastLoginTime(){return m_legionData.baseInfo.dwLastLoginTime;}
	int getLevelUpExp();
	void addMemberContribyte(HEntity hActor, int iContribute);
	void addMemberContribyte(const string& strMemName, int iContribute, HEntity hActor);
	void addExp(int iContribute);
	int getLevel(){return m_legionData.baseInfo.iLegionLevel;}
	unsigned int getLevelUpTime(){return m_legionData.baseInfo.dwLevelUpTime;}
	void setRank(int iRank){m_iRank = iRank;}
	const LegionLevelInfo* getCurLevelInfo();
	int getArenaRank(const string& roleKey);
	void sortMemberRank();
	int getMemberCount();
	bool isApplyed(const string& strActorName);
	int getOccupation(const string& strMemName);
	int getContributeRank(const string& strMemName);
	void fillLegionAppItem(const ServerEngine::LegionAppItem& refItem, GSProto::LegionAppItem* pNewItem);

	void onReqLegionBase(HEntity hActor, const GSProto::CSMessage& msg);
	void onReqLegionMemList(HEntity hActor, const GSProto::CSMessage& msg);
	void fillLegionMember(const string& strMemName, GSProto::LegionMember* pMember, HEntity hActor);
	void onReqQueryAppList(HEntity hActor, const GSProto::CSMessage& msg);
	void onReqModifySetting(HEntity hActor, const GSProto::CSMessage& msg);
	void onReqDealLegionApp(HEntity hActor, const GSProto::CSMessage& msg);
	void onReqDisssmissLegion(HEntity hActor, const GSProto::CSMessage& msg);
	void onReqKickMember(HEntity hActor, const GSProto::CSMessage& msg);
	void onReqTransLeader(HEntity hActor, const GSProto::CSMessage& msg);
	void onReqQueryContribute(HEntity hActor, const GSProto::CSMessage& msg);
	void onReqContribute(HEntity hActor, const GSProto::CSMessage& msg);
	void onReqBless(HEntity hActor, const GSProto::CSMessage& msg);
	void onReqQueryPay(HEntity hActor, const GSProto::CSMessage& msg);
	void onReqGetPay(HEntity hActor, const GSProto::CSMessage& msg);
	
	void onReqRefuseAllApp(HEntity hActor, const GSProto::CSMessage& msg);

	void onReqQueryBoss(HEntity hActor, const GSProto::CSMessage& msg);
	void onReqCallBoss(HEntity hActor, const GSProto::CSMessage& msg);
	void onReqCheerLegionBoss(HEntity hActor, const GSProto::CSMessage& msg);
	void onReqFightLegionBoss(HEntity hActor, const GSProto::CSMessage& msg);
	void onReqRelive(HEntity hActor, const GSProto::CSMessage& msg);
	void onReqQueryBattle(HEntity hActor, const GSProto::CSMessage& msg);
	void onReqQueryLastBattleList(HEntity hActor, const GSProto::CSMessage& msg);
	void onReqQuerySingleLastBattle(HEntity hActor, const GSProto::CSMessage& msg);
	void onReqReportLegionBattle(HEntity hActor, const GSProto::CSMessage& msg);
	void onRepJoinLegionBattle(HEntity hActor, const GSProto::CSMessage& msg);
	void onReqQueryLegionReport(HEntity hActor, const GSProto::CSMessage& msg);
	void onReqQueryCurBattlePage(HEntity hActor, const GSProto::CSMessage& msg);
	void onReqQueryCurBattleSelf(HEntity hActor, const GSProto::CSMessage& msg);
	void onReqQueryCurHonorRank(HEntity hActor, const GSProto::CSMessage& msg);
	void onReqCloseCurBattlePage(HEntity hActor, const GSProto::CSMessage& msg);
	void onReqLastBattleHonorRank(HEntity hActor, const GSProto::CSMessage& msg);
	void onReqLastBattleRecordPage(HEntity hActor, const GSProto::CSMessage& msg);
	void onReqLastBattleSelfBattleRecord(HEntity hActor, const GSProto::CSMessage& msg);
	void onReqDealLegionAppOneKey(HEntity hActor, const GSProto::CSMessage& msg);

	void onLegionAppoint(HEntity hActor, const GSProto::CSMessage& msg);
	void onQueryWorship(HEntity hActor, const GSProto::CSMessage& msg);
	void onWorshipMember(HEntity hActor, const GSProto::CSMessage& msg);
	void onGetWorshipReward(HEntity hActor, const GSProto::CSMessage& msg);
	
	int getCallBossCD();
	int getBossMaxHP();
	bool isLegionBattleEnable();
	int getOwnDomainCount();
	bool checkPayGeted(HEntity hActor, int iPayType);
	const ServerEngine::LegionMemberItem* getMemberInfo(const string& strMemName);
	void fillScBossInfo(HEntity hActor, GSProto::LegionBossFightInfo* pBossFightInfo);
	int getLeftKillBossTime();
	int getActorCheerCount(HEntity hActor);
	MemberLegionBossState* getMemberBossState(const string& strName);
	int getBossMemReliveCD(const string& strName);
	void procBossTimeout();
	void clearBossRecord(bool bKilled);
	void processBossFightResult(HEntity hActor, const ServerEngine::BattleData& data, int iDamage);
	void doBossRankAward();
	void doBossKillAward();
	void notifyBossState();
	ServerEngine::LegionData& getLegionData(){return m_legionData;}
	int getDayPayBaseSilver();
	int getDayPayTotalSilver();
	void fillSCPayInfo(HEntity hActor, int iPayType, int iContributeRank, int iParam, GSProto::LegionPay* pScPay);
	void broadcastBossEnd();
	
public:

	int getLegionIconId();
	void saveThreeDayContribute();
	int getThreeDayContribute(const ServerEngine::LegionMemberItem& memberItem);	
	ServerEngine::LegionMemberItem* getMemberInfoByName(const string& strMemName);
	int getActorRemaindWorshipTimes(HEntity hActor);
	bool bMemberCanWorship(HEntity hActor, const string& strMemberName);
	void updateActorLevelInfo(HEntity hActor);
	void pushWorshipInfo(HEntity hActor);
	void removeLegionMember(string strActorName);
	void removeAcotrFromSortList(string strActorName);
public:
	//void onQueryLegionCampBattleInfo(HEntity hActor, const GSProto::CSMessage& msg);

	void getMemeberNameList(vector<string>& memberNamelist);

	void onQueryCampBattleInfo(HEntity hActor, const GSProto::CSMessage& msg);
	void onReqEnterLegionBattle(HEntity hActor, const GSProto::CSMessage& msg);
	void onReqCampMemberBattle(HEntity hActor, const GSProto::CSMessage& msg);

	void getMemberVSLegionMemDetail(map<std::string, ServerEngine::LegionMemberItem>& memberList);

	void updateActorFightValue(HEntity hEntity);

	int iGetLegionMoney() {return m_legionData.baseInfo.iLegionMoney;}

	void onQueryLastFightLog(HEntity hActor, const GSProto::CSMessage& msg);

	void onQueryLastFightDetailLog(HEntity hActor, const GSProto::CSMessage& msg);
	void onQueryCurFightLog(HEntity hActor, const GSProto::CSMessage& msg);
	void onQueryCurFightCondition(HEntity hActor, const GSProto::CSMessage& msg);
	void onQueryLastFightSimpleLog(HEntity hActor, const GSProto::CSMessage& msg);

	//int iGetActorHeadIcon(string strActorname);
	//int iGetActorFightValue(string strActorname);
private:

	PROFILE_OBJ_COUNTER(GSLegionImp);

	HLegion m_hHandle;
	ServerEngine::LegionData m_legionData;
	vector<string> m_sortMemberList;
	int m_iRank;

	ITimerComponent::TimerHandle m_saveHandle;

	// ¾üÍÅBOSS
	IBoss* m_pLegionBoss;
	map<string, MemberLegionBossState> m_mapBossState;
	ITimerComponent::TimerHandle m_bossTimerHandle;
	ITimerComponent::TimerHandle m_bossUpdateHandle;
	HEntity m_hKiller;
};

#endif
