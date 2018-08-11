#ifndef DREAMLANDSYSTEM_H_
#define DREAMLANDSYSTEM_H_

enum  enDreamLandSceneState
{
	en_State_BeforeOpen = GSProto::en_State_NOOPEN,
	en_State_OPen = GSProto::en_State_OPEN,
	en_State_Finish = GSProto::en_State_FINISH,
};

class DreamLandSystem:public ObjectBase<IDreamLandSystem>,public Detail::EventHandle
{
public:
	DreamLandSystem();
	virtual ~DreamLandSystem();
public:

	//interface 
	virtual Uint32 getSubsystemID() const;

	virtual Uint32 getMasterHandle();
	
	virtual bool create(IEntity* pEntity, const std::string& strData);
	
	virtual bool createComplete();

	virtual const std::vector<Uint32>& getSupportMessage();

	virtual void onMessage(QxMessage* pMessage);

	virtual void packSaveData(string& data);
public:
	virtual void GMRequsetFight(int iSection, int iSceneID);
	virtual void GMRequsetFormation(int iSection, int iSceneID);
	virtual void CMClearData();

protected:
	
	void onReqOpenReqSectionInfo(const GSProto::CSMessage& msg);
	
	void onReqResetDreamLand(const GSProto::CSMessage& msg);
	
	void onQuerySectionSceneInfo(const GSProto::CSMessage& msg);
	
	void onQuerySceneFormation(const GSProto::CSMessage& msg);
	
	void onRequstOPenBox(const GSProto::CSMessage& msg);
	
	void onGetActorFormation(const GSProto::CSMessage& msg);
	
	void onChgActorFormation(const GSProto::CSMessage& msg);
	
	void onRequstFight(const GSProto::CSMessage& msg);

	void onQueryBox(const GSProto::CSMessage& msg);

	void onRequestHaveWinSceneInfo(const GSProto::CSMessage& msg);

	void loadHeroFormation();

	//void takeHeroUpFormation(HEntity htargetHero,int iPos);
	
	void sendFormationToClient();
	
	int getHeroPosFromBaseID(int iBaseID);
	
//	void chagePos(HEntity hMoveEntity, int iSrcPos, int iTargetPos);

	void packHeroListData(string& data);
	void packHeroFormationData(string& data);

	virtual void GMAddDreamLandResetTimes();
public:
	void processFightResult(int iSectionId, int iSceneId,const ServerEngine::BattleData& battleInfo);
	void beginDreamLandSysBattle(EventArgs& args);
	bool getActorCreateContext(ServerEngine::PKRole& pkRole, IEntity* pEntity);
	
	void fillBattleUnitContext(ServerEngine::FightContext& fightContext,ServerEngine::DreamLandSceneRecord& recordCtx, int iSceneId);

	bool getUUIDByiPos(int iPos, string& sUUID);
	
	void sendSectionInfo();

	void checkReset();

	void sendSceneInfo(int iSecetionID);

	void saveSceneActorData(HEntity hEntity,int iSectionID, int iSceneID, ServerEngine::PIRole &roleData, ServerEngine::PKRole roleKey);

	void sendSceneActorInfo(HEntity hEntity,  int index);
	
protected:
	
	int getIposByUUDID(string UUID);
	
	bool getArenaSavePlayer(int iSectionId);
	
	int getSceneRecord(int iSceneId);

	void openNewScene(int iSectionId, int iSceneId);


	void sendSceneGuardFormation(int iSectionId, int iSceneId);

	void clearActorFormation();
	
private:

	HEntity m_hEntity;

	vector<HEntity> m_FormationList;

	vector<ServerEngine::ArenaSavePlayer> m_guardNamesVec;
	
	ServerEngine::DreamLandData m_DreamLandData;

	//玩家在战斗不能改变阵型

	bool m_FightFlag;
};



#endif