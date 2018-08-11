#ifndef __ACTOR_H__
#define __ACTOR_H__

class Actor:public Entity
{
public:

	Actor(int iClassID);
	~Actor();

	virtual bool createEntity(const std::string& strData);
	virtual void packSaveData(ServerEngine::RoleSaveData& data);
	virtual void save();
	virtual void sendMessage(const GSProto::SCMessage& scMessage);
	virtual void sendErrorCode(int iErrorCode);
	virtual void addExp(int iExp);
	virtual void sendMessage(int iCmd, const ::google::protobuf::Message& msgBody);
	virtual void sendMessage(int iCmd);
	virtual void changeProperty(PropertySet::PropertyKey iKey, int iChgValue, int iReason);
	virtual void setProperty(PropertySet::PropertyKey key, Int32 nValue);
		
	virtual bool isFunctionOpen(int iFunctionID);
	virtual void enableFunction(int iFunctionID);
	virtual void onMessage(QxMessage* pMessage);

	virtual void addVipExp(int iExp);
	void sendToClientInfo();
	void onEventReLogin(EventArgs& args);
	void onEventFightValueChg(EventArgs& args);

	virtual void chgNotice(int iNoticeId,bool haveNotice);

	void addAllFormationHero();
	void addHeroList(const vector<int>& heroList);

	PROFILE_OBJ_COUNTER(Actor);
};


#endif
