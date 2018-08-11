#ifndef __FIGHT_FACTORY_H__
#define __FIGHT_FACTORY_H__


struct BattleRandomRcvCb
{
	BattleRandomRcvCb(vector<taf::Int64>& valueList):m_randomValueList(valueList){}

	void operator()(long int iValue);

	vector<taf::Int64>& m_randomValueList;
};


struct BattleRandomCache
{
	BattleRandomCache(ServerEngine::BattleData& data);
	~BattleRandomCache();
	
	ServerEngine::BattleData& m_battleData;
};

struct BattleRandSetGuard
{
	BattleRandSetGuard(const vector<taf::Int64>& valueList);
	~BattleRandSetGuard();
};

struct BattleFightCtxGuard
{
	BattleFightCtxGuard(ServerEngine::FightContext& ctx);
	~BattleFightCtxGuard();
};

struct BattleWholeDataGuard
{
	BattleWholeDataGuard(GSProto::FightDataAll* pFightDataAll);
	~BattleWholeDataGuard();
};

struct BattleFightLogHelper
{
	BattleFightLogHelper(const string& strName):m_strName(strName)
	{
		FDLOG("Fight")<<"BeginFight---"<<strName<<endl;
	}

	~BattleFightLogHelper()
	{
		FDLOG("Fight")<<"EndFight---"<<m_strName<<endl;
	}

	string m_strName;
};

struct ActorSkillCfg
{
	ActorSkillCfg():iActorSkillID(0), iCost(0), iCondLevel(0){}

	int iActorSkillID;
	int iCost;
	vector<int> effectIDList;
	int iCondLevel;
};

class FightFactory:public ComponentBase<IFightFactory, IID_IFightFactory>
{
public:

	friend class SkillActionListener;
	friend class BattleFightCtxGuard;
	friend class BattleWholeDataGuard;
	friend class PropChgResultListener;
	friend class BuffAddListener;

	FightFactory();
	~FightFactory();

	virtual bool initlize(const PropertySet& propSet);

	// IFightFactory Interface
	virtual void queryBattleRecord(const ServerEngine::PKFight& fightKey, DelegateQueryFight cb);
	virtual void saveBattleRecord(const ServerEngine::BattleData& data);
	virtual string makeFightStringKey(const ServerEngine::PKFight& key);
	virtual bool decodeFightStringKey(const std::string& strKey, ServerEngine::PKFight& key);
	virtual void generateFightKey(int iWorldID, ServerEngine::BattleData& data);
	virtual void AsyncPVEFight(DelegatePVEFight cb, const ServerEngine::ActorCreateContext& attacker, int iMonsterGrpID, const ServerEngine::FightContext& ctx);
	virtual void AsyncPVPFight(DelegatePVPFight cb, const ServerEngine::ActorCreateContext& attacker, const ServerEngine::ActorCreateContext& target, const ServerEngine::FightContext& ctx);
	virtual int doPVEFight(ServerEngine::BattleData& battleInfo, const ServerEngine::ActorCreateContext& roleInfo, int iMonsterGrpID, const ServerEngine::FightContext& ctx);
	virtual int doPVPFight(ServerEngine::BattleData& battleInfo, const ServerEngine::ActorCreateContext& attRoleInfo, const ServerEngine::ActorCreateContext& targetRileInfo, const ServerEngine::FightContext& ctx);	
	virtual void AsyncMultiPVEFight(DelegatePVEFight cb, const ServerEngine::ActorCreateContext& roleInfo, vector<int> monsterGrpList, const ServerEngine::FightContext& ctx);
	virtual int doMultiPVEFight(ServerEngine::BattleData& battleInfo, const ServerEngine::ActorCreateContext& roleInfo, vector<int> monsterGrpList, const ServerEngine::FightContext& ctx);
	virtual int continuePVEFight(ServerEngine::BattleData& battleInfo, const ServerEngine::ActorCreateContext& roleInfo, int iMonsterGrpID, const ServerEngine::FightContext& ctx);
	virtual int continueMultiPVEFight(ServerEngine::BattleData& battleInfo, const ServerEngine::ActorCreateContext& roleInfo, vector<int> monsterGrpList, const ServerEngine::FightContext& ctx);
	virtual int continuePVPFight(ServerEngine::BattleData& battleInfo, const ServerEngine::ActorCreateContext& roleInfo, const ServerEngine::ActorCreateContext& targetRileInfo, const ServerEngine::FightContext& ctx);
	virtual void AsyncContinuePVEFight(DelegatePVEFight cb, const ServerEngine::ActorCreateContext& roleInfo, int iMonsterGrpID, const ServerEngine::FightContext& ctx);
	virtual void AsyncContinueMultiPVEFight(DelegatePVEFight cb, const ServerEngine::ActorCreateContext& roleInfo, vector<int> monsterGrpList, const ServerEngine::FightContext& ctx);
	virtual void AsyncContinuePVPFight(DelegatePVPFight cb, const ServerEngine::ActorCreateContext& roleInfo, const ServerEngine::ActorCreateContext& targetRileInfo, const ServerEngine::FightContext& ctx);
	virtual EventServer* getEventServer();
	virtual const vector<HEntity>* getMemberList();
	virtual int getCurrentRound();


	// internal function
	void fillGhost(vector<HEntity>& memberList, HEntity hGhost, bool bAttacker);
	void fillMonsterGrp(vector<HEntity>& memberList, const vector<HEntity>& monsterList);
	int _doCommonFight(const vector<HEntity>& memberList, ServerEngine::BattleData& data, const ServerEngine::FightContext& ctx, const vector<HEntity>& externList, int iWorldID, int iLoopCount, GSProto::FightDataAll& wholeData);
	bool isAttackerSideFirstMove(const ServerEngine::FightContext& ctx, const vector<HEntity>& memberList);
	bool isAttackerFirstByFightValue(const vector<HEntity>& memberList);
	
	void doOnStageSkill(const vector<HEntity>& memberList, bool bAttackerFirst, const vector<HEntity>& externList, GSProto::FightDataAll& data);
	bool checkFinish(const vector<HEntity>& memberList, ServerEngine::BattleData& data, int iRound, const ServerEngine::FightContext& ctx);
	void sortAttackList(const vector<HEntity>& memberList, bool bAttackerFirst, vector<HEntity>& firsts, vector<HEntity>& second);
	bool procGoldAnimalFight(int iRound, const vector<HEntity>& memberList, bool bAttackerFirstMove, vector<HEntity>& externList, const vector<HEntity>& orgExternList, GSProto::FightDataAll& data);

	void _triggerBeginFight(const vector<HEntity>& memberList, const ServerEngine::FightContext& ctx, const vector<HEntity>& externList, int iLoopCount);
	void _triggerEndFight(const vector<HEntity>& memberList, const ServerEngine::FightContext& ctx, const vector<HEntity>& externList);
	void _triggerBeginRound(const vector<HEntity>& memberList, int iRound, const vector<HEntity>& externList);
	void _triggerEndRound(const vector<HEntity>& memberList, int iRound, const vector<HEntity>& externList);
	void _triggerPreAttack(HEntity hEntity, int iRound);
	void _triggerPostAttack(HEntity hEntity, int iRound);
	void useStateSkill(HEntity hEntity, const vector<HEntity>& memberList);
	void cleanFightMemList(const vector<HEntity>& memberList);
	void fillBattleMember(GSProto::FightDataAll& data, const vector<HEntity>& memberList, const vector<HEntity>& externList, const ServerEngine::FightContext& ctx);
	void fillSingleFightObj(HEntity hEntity, GSProto::FightObj& fightObj);
	void fillMemberFinStatus(const vector<HEntity>& memberList, GSProto::FightDataAll& data);
	int calcFightStar(GSProto::FightDataAll& data);
	void fillExternList(HEntity hGost, bool bAttacker, vector<HEntity>& externList);
	bool isTargetPos(int iPos);
	void triggerBeginResultSet();
	void fillReplaceAction(GSProto::FightDataAll& data, const vector<HEntity>& memberList, bool bLastFight);
	bool useFightItem(const ServerEngine::FightUseItem& useItem);
	bool useActorSkill(const ServerEngine::FightUseItem& useItem);
	void fillPropChgToAction(GSProto::FightAction* pAction, EventArgs_FightPropChg& args);
	void fillAddBuffToAction(GSProto::FightAction* pAction, HEntity hTarget, EventArgs& args);
	bool canUseFightUseActorSkill(int iFightType);
	bool isActionResultHit(int iActionIndex, int iResultsetIndex, const ServerEngine::FightUseItem& useItemCtx);
	void _useItemImp(const ServerEngine::FightUseItem& useItemCtx, IEntity* pTarget, int iAddHP);
	void fixResultSetHP(GSProto::FightResulSet& refResultSet);
	void appendDlgList(GSProto::FightDataAll& fightAllData, const vector<int>& dlgList);
	const ActorSkillCfg* getActorSkillCfg(int iActorSkillID);

	void useActorEffect(IEntity* pGiver, const vector<int>& effectList);
	int getLeftActorLevel(const vector<HEntity>& memberList);

private:

	EventServer m_eventServer;
	ServerEngine::FightContext* m_pCurPVEFightCtx;  // PVE战斗非COntinue也需要
	bool m_bContinue;
	GSProto::FightDataAll* m_pContinueDataAll;
	vector<HEntity> m_memberList;
	ServerEngine::BattleData* m_pContinueBattleData;
	map<int, ActorSkillCfg> m_actorSkillMap;

	int m_iRound;
};

#endif


