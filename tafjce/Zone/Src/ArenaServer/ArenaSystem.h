#ifndef __ARENA_SYSTEM_H__
#define __ARENA_SYSTEM_H__

class ArenaSystem:public ObjectBase<IArenaSystem>, public Detail::EventHandle
{
public:

	ArenaSystem();
	~ArenaSystem();

	// IArenaSystem Interface
	virtual bool addArenaCount();
	virtual void addArenaLog(const GSProto::ArenaLogItem& logItem, bool bActorIsAtt);
	virtual void confirmArenaGlodResetCD() ;

	// IEntitySubSystem Interface
	virtual Uint32 getSubsystemID() const {return IID_IArenaSystem;}
	virtual Uint32 getMasterHandle(){return m_hEntity;}
	virtual bool create(IEntity* pEntity, const std::string& strData);
	virtual bool createComplete();
	virtual const std::vector<Uint32>& getSupportMessage();
	virtual void onMessage(QxMessage* pMessage);
	virtual void packSaveData(string& data);

	void onEventSend2Clinet(EventArgs& args);
	void onEventFightValueChg(EventArgs& args);

	void onReqOpenArena(const GSProto::CSMessage& msg);
	void onReqQueryArenaAward(const GSProto::CSMessage& msg);
	void onReqGetArenaAward(const GSProto::CSMessage& msg);
	void onReqQueryArenaRank(const GSProto::CSMessage& msg);
	void onReqQueryArenaLog(const GSProto::CSMessage& msg);
	void onReqAddChallengeCount(const GSProto::CSMessage& msg);
	void onReqArenaFight(const GSProto::CSMessage& msg);

	void onChangeChallengeList(const GSProto::CSMessage& msg);
	void onResetRemaind(const GSProto::CSMessage& msg);

	int getLeftChallengeCount();
	void processArenaFight(int iRet, const ServerEngine::BattleData& battleData);
	int getArenBuyCountLimit();
	void checkNotice();
	void fillArenaAppendData(GSProto::ArenaAppendInfo& info);
	void checkReset();
	void pushArenaInfo(); 

private:

	HEntity m_hEntity;
	ServerEngine::ArenaSystemSaveData m_arenaSaveData;
	vector<GSProto::ArenaPlayer> m_challengeList;

	Uint32 m_dwLastArenaFightTime;
	//bool m_bBusy;
};

#endif
