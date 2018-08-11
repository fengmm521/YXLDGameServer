#ifndef __ENTITY_H__
#define __ENTITY_H__

class Entity:public ObjectBase<IEntity>, public Detail::EventHandle 
{
public:

	// Constructor/Destructor
	Entity();
	~Entity();

	// IObject Interface
	virtual bool initlize(const PropertySet& propSet);
	virtual bool inject(const std::string& strClassName, IObject* pObject);
	virtual void setProperty(PropertySet::PropertyKey key, Int32 nValue);
	virtual void setProperty(PropertySet::PropertyKey key, const char* strValue);
	virtual void setProperty(PropertySet::PropertyKey key, float fValue);
	virtual void setInt64Property(PropertySet::PropertyKey key, Int64 value);

	// IEntity Interface
	virtual IEntitySubsystem* querySubsystem(Uint32 dwSubsystemID);
	virtual std::vector<Uint32> getSubsystemList();
	virtual bool createEntity(const std::string& strData);
	virtual HEntity getHandle();
	virtual EventServer* getEventServer();
	virtual void onMessage(QxMessage* pMessage);
	virtual void packSaveData(ServerEngine::RoleSaveData& data);
	virtual void save();
	virtual void changeProperty(PropertySet::PropertyKey iKey, int iChgValue, int iReason);
	virtual void sendMessage(const GSProto::SCMessage& scMessage);
	virtual void sendErrorCode(int iErrorCode){}
	virtual void addExp(int iExp) {}
	virtual void sendMessage(int iCmd, const ::google::protobuf::Message& msgBody){}
	virtual void sendMessage(int iCmd){}
	virtual bool isFunctionOpen(int iFunctionID){return false;}
	virtual void enableFunction(int iFunctionID) {}
	virtual void addVipExp(int iExp){}
	virtual void chgNotice(int iNoticeId,bool haveNotice){}

	void intitBaseProp(const ServerEngine::RoleBase& roleBaseData);
	void initSubsystem(const ServerEngine::RoleSaveData& roleSaveData);
	void completeSubsystem();
	
	void packBaseProp(ServerEngine::RoleSaveData& data, int* pszData, int iSize);
	void packSubsystem(ServerEngine::RoleSaveData& data);
	void rebindHandle(HEntity hNewHandle);
	
protected:

	PROFILE_OBJ_COUNTER(Entity);
	typedef std::map<Uint32, IEntitySubsystem*>	SubsystemMap;
	SubsystemMap	m_subsystemMap;

	//typedef std::multimap<Uint32, IEntitySubsystem*>	SubsystemMsgMap;
	typedef std::map<Uint32, IEntitySubsystem*>	SubsystemMsgMap;
	SubsystemMsgMap	m_subsystemMsgMap;

	HEntity		m_hHandle;
	EventServer	m_eventServer;
};


#endif
