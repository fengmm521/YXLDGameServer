#ifndef FRIENDSYSTEM_H_
#define FRIENDSYSTEM_H_


class NameDescCallback : public ServerEngine::NamePrxCallback
{
public:
	NameDescCallback(DelegateName cb):m_cb(cb) {}
	virtual void callback_getNameDesc(taf::Int32 ret,  const ServerEngine::NameDesc& descInfo)
        {
         ServerEngine::NameDesc namedesc = descInfo;
           m_cb(ret,namedesc);
        }
	
     virtual void callback_getNameDesc_exception(taf::Int32 ret)
        { 
        	ServerEngine::NameDesc nullDesc;
        	m_cb(ret,nullDesc);
        }
private:
	DelegateName m_cb;
};


class FriendSystem: public ObjectBase<IFriendSystem>,public Detail::EventHandle,public ITimerCallback 
{
public:
	FriendSystem();
	virtual ~FriendSystem();
public:
	
	virtual Uint32 getSubsystemID() const;
	virtual Uint32 getMasterHandle();
	virtual bool create(IEntity* pEntity, const std::string& strData);
	virtual bool createComplete();
	virtual const std::vector<Uint32>& getSupportMessage();
	virtual void onMessage(QxMessage* pMessage);
	virtual void packSaveData(string& data);

	virtual bool deleteFriend(string name);
	virtual bool addRequstToRequestMap(string name,ServerEngine::PKRole role);
	virtual bool addFriendToMap(string name, ServerEngine::FriendNode node);
	virtual bool GetOrGivePhyStrength(string name);
	virtual bool deleteRequest(string strName);

	virtual void onTimer(int nEventId);
	virtual void checkNotice();
	virtual void GMOnlineGetActorBaseInfo(string& strJson);
public:
	void onQueryFriendSystemInfo(const GSProto::CSMessage& msg);	//查询系统信息
	void onDeleteFriend(const GSProto::CSMessage& msg);			//删除好友
	void onSearchFriend(const GSProto::CSMessage& msg);  			//查询好友
	void onAgreeOrDeleteRequest(const GSProto::CSMessage& msg);	//同意或者拒绝好友请求
	void onQueryFriendList(const GSProto::CSMessage& msg);		//查询好友列表
	void onQueryRequestList(const GSProto::CSMessage& msg);		//查询请求列表
	void onGivePhyStrength(const GSProto::CSMessage& msg);  		//赠送体力值
	void onGetPhyStrength(const GSProto::CSMessage& msg);			//领取体力值
	void onRequestFriend(const GSProto::CSMessage& msg);				//请求好友
	
	void onUseNameFindActorInfo(const GSProto::CSMessage& msg);			//通过名字查看好友详情
	void onUsePBPkRoleFindActorInfo(const GSProto::CSMessage& msg);		//通过PBPkRole查看好友详情
	void onOneKeyGetStrength(const GSProto::CSMessage& msg);
	
	void fillFriendNodeInfo(HEntity hEntity, ServerEngine::PIRole& roleInfo,int size,bool bIsFriend,ServerEngine::FriendNode node);
	void fillFriendListMsg(HEntity hEntity, ServerEngine::PIRole& roleInfo,int size,ServerEngine::FriendNode node);
	void fillRequestListMsg(HEntity hEntity, ServerEngine::PIRole& roleInfo,int size,ServerEngine::FriendNode node);
	
	void fillFriendPhyStrengthInfo(GSProto::FriendPhyStrengthDetail& detail);
	void deleteFriend(int iRet,HEntity hEntity, ServerEngine::PIRole& roleInfo,string strName);
	void addRequestToList(HEntity hEntity, ServerEngine::PIRole& roleInfo);
	void getRequestNameDesc(int iRet,const ServerEngine::NameDesc& nameInfo);
	void onGetOrGivePhyStrength(HEntity hEntity, ServerEngine::PIRole& roleInf,string name);

	void onUseNameFindActorInfo();
	void onUsePBPkRoleFindActorInfo();
	
protected:
	void ResetData(ServerEngine::FriendSystemData& data);
	void resetFriendNode(ServerEngine::FriendNode& node);
	bool canComein();
	
private:
	HEntity m_hEntity;
	ServerEngine::FriendSystemData m_FriendData;
	GSProto::CMD_FRIENDSYSTEM_QUERY_SC m_msgFriendSystemQueryBody;
	GSProto::CMD_FRIENDSYSTEM_QUERY_FRIENDLIST_SC m_msgQueryFriendListBody;
	GSProto::CMD_FRIENDSYSTEM_QUERY_REQUESTLIST_SC m_msgQueryRequestBody;
	ITimerComponent::TimerHandle m_CheckNoticeTimerHandle;
};
#endif
