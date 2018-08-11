#ifndef CAMPBATTLESYSTEM_H_
#define CMAPBATTLESYSTEM_H_

class CampBattleSystem: public ObjectBase<ICampBattleSystem>,public Detail::EventHandle
{
public:
	CampBattleSystem();
	virtual ~CampBattleSystem();
public:
	virtual Uint32 getSubsystemID() const;
	virtual Uint32 getMasterHandle();
	virtual bool create(IEntity* pEntity, const std::string& strData);
	virtual bool createComplete();
	virtual const std::vector<Uint32>& getSupportMessage();
	virtual void onMessage(QxMessage* pMessage);
	virtual void packSaveData(string& data);
protected:
	void onOpenCampBattle(const GSProto::CSMessage& msg);
	void onCloseCampBattle(const GSProto::CSMessage& msg);
	void onJoinCampBattle(const GSProto::CSMessage& msg);
	void onJoinCancel(const GSProto::CSMessage& msg);

	void fillActorInfo(	GSProto::CampBattlePlayerDetail& detail);
	void fillTimeInfo( GSProto::RemaindTime& remainTime);
	
	void saveCampBattle(EventArgs& args);
	void beginCampBattle(EventArgs& args);

	bool checkOpen();
	
private:
	HEntity m_hEntity;
};

#endif
