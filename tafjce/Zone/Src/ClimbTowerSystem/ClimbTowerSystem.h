#ifndef _CLIMBTOWERSYSTEM_H_
#define _CLIMBTOWERSYSTEM_H_

class ClimbTowerSystem:public ObjectBase<IClimbTowerSystem>,public Detail::EventHandle
{
public:
	ClimbTowerSystem();
	virtual ~ClimbTowerSystem();
	
public:
	// IEntitySubSystem Interface
	virtual Uint32 getSubsystemID() const;
	virtual Uint32 getMasterHandle();
	virtual bool create(IEntity* pEntity, const std::string& strData);
	virtual bool createComplete();
	virtual const std::vector<Uint32>& getSupportMessage();
	virtual void onMessage(QxMessage* pMessage);
	virtual void packSaveData(string& data);
	void processClimbTowerChallengeRes(int iScenID, int iRet, const ServerEngine::BattleData& data);
	
	virtual void addTowerResetCount(bool ensuer);
	virtual bool canSkip();
	virtual int getTowerTop();
protected:
	void onQueryClimbTowerDetail(const GSProto::CSMessage& message);
	void onResetClimbTower(const GSProto::CSMessage& message);
	void onDirectChallengeTowerLayer(const GSProto::CSMessage& message);
	void onFastChallengeTowerLayer(const GSProto::CSMessage& message);
	void onFillClimbTowerDetail(GSProto::ClimbTowerDatail & detail );
	void onAddTowerResetCount(const GSProto::CSMessage& message );
	void processChallengeAward(int iScenID, GSProto::FightAwardResult& awardResult);
	//玩家第二天重置Rest Count 
	void checkNeedRest(); 
private:
	HEntity m_hEntity;
	ServerEngine::ClimbTowerSystemData m_climbTowerSystemDBData;
	int m_towerLayerCount; //塔的总层数
	ServerEngine::TimeResetValue m_resetValue;
	int m_resetBaseCount;
};



#endif









































