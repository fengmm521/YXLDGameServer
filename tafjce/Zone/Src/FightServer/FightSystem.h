#ifndef __FIGHT_SYSTEM_H__
#define __FIGHT_SYSTEM_H__


class PVPGetRoleCb:public ServerEngine::RolePrxCallback
{
public:

	PVPGetRoleCb(HEntity hAttacker, const ServerEngine::PKRole targetKey, const ServerEngine::FightContext& ctx, DelegatePVPFight cb):m_hAttacker(hAttacker), 
		m_targetKey(targetKey), m_fightCtx(ctx), m_cb(cb){}

	virtual void callback_getRole(taf::Int32 ret,  const ServerEngine::PIRole& roleInfo);
	virtual void callback_getRole_exception(taf::Int32 ret);

private:

	HEntity m_hAttacker;
	ServerEngine::PKRole m_targetKey;
	ServerEngine::FightContext m_fightCtx;
	DelegatePVPFight m_cb;
};

template<class T>
class PVEAndPVPFightCache
{
public:
	
	PVEAndPVPFightCache(HEntity hEntity, T cb, bool bUseItem):m_hEntity(hEntity), m_cb(cb), m_bUseItem(bUseItem){}

	void operator()(taf::Int32 iRet, const ServerEngine::BattleData& data);
	void operator()(taf::Int32 iRet, ServerEngine::BattleData& data);

	bool canSkip(int iFightType);

private:

	HEntity m_hEntity;
	T m_cb;
	bool m_bUseItem;
};

class FightSystem:public ObjectBase<IFightSystem>
{
public:

	friend class PVPGetRoleCb;
	friend class PVEAndPVPFightCache<DelegatePVEFight>;
	friend class PVEAndPVPFightCache<DelegatePVPFight>;
	

	FightSystem();
	~FightSystem();

	// IEntitySubSystem Interface
	virtual Uint32 getSubsystemID() const;
	virtual Uint32 getMasterHandle();
	virtual bool create(IEntity* pEntity, const std::string& strData);
	virtual bool createComplete();
	virtual const std::vector<Uint32>& getSupportMessage();
	virtual void onMessage(QxMessage* pMessage);
	virtual void packSaveData(string& data){}

	// IFightSystem Interface
	virtual void AsyncPVEFight(DelegatePVEFight cb, int iMonsterGrpID, const ServerEngine::FightContext& ctx);
	virtual void AsyncPVPFight(DelegatePVPFight cb, const ServerEngine::PKRole& target, const ServerEngine::FightContext& ctx);
	virtual void AsyncPVPFight(DelegatePVPFight cb, const ServerEngine::ActorCreateContext& targetCtx, const ServerEngine::FightContext& ctx);
	virtual void AsyncMultPVEFight(DelegatePVEFight cb, vector<int> monsterList, const ServerEngine::FightContext& ctx);
	virtual void sendBattleMemMsg(const GSProto::FightDataAll& allData);
	virtual void sendBattleAction(const GSProto::FightDataAll& allData);
	virtual void sendBattleFin();
	virtual void sendAllBattleMsg(const ServerEngine::BattleData& battleData);
	virtual void sendAllBattleMsg(const ServerEngine::BattleData& battleData, GSProto::SCMessage& resultMsg);

	void onReqUseActorSkill(const GSProto::CSMessage& msg);
	void onReqFightPlayFinish(const GSProto::CSMessage& msg);
	void onReqFightQuit(const GSProto::CSMessage& msg);
	void sendAllBattleMsgFromIndex(const ServerEngine::BattleData& battleData, int iIndex);
	void sendBattleMemMsgFixSkip(const GSProto::FightDataAll& allData, bool bCanSkip);
	void sendAllBattleMsgWithSkipFlag(const ServerEngine::BattleData& battleData, bool bSkipFlag);

private:

	void _AsyncPVPFight(DelegatePVPFight cb, const ServerEngine::ActorCreateContext targeterCtx, const ServerEngine::FightContext& ctx);

private:

	HEntity m_hEntity;

	// 上次PVE战斗现场数据
	int m_iMonsterGrpID;
	vector<int> m_monsterGrpList;
	DelegatePVEFight m_cb;
	
	// 战斗中治疗现场公共数据
	ServerEngine::FightContext m_fightCtx;
	ServerEngine::ActorCreateContext m_fightRoleInfo;
	ServerEngine::BattleData m_battleData;
	Uint32 m_dwLastFightRetTime;
	bool m_bPVEFight;

	// 上次PVP战斗现场数据
	ServerEngine::ActorCreateContext m_pvpTargetRoleInfo;
	DelegatePVPFight m_pvpCb;
};


#endif

