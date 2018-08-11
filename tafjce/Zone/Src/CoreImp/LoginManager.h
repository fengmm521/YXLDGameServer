#ifndef __LOGINMANAGER_H__
#define __LOGINMANAGER_H__

#include "Snapshot.h"
#include "LJSDK.h"


class GetRoleCallback:public ServerEngine::RolePrxCallback
{
public:

	GetRoleCallback(const string& strAccount, ServerEngine::VerifyKey verifyKey):m_strAccount(strAccount),m_verifyKey(verifyKey){}

	virtual void callback_getRole(taf::Int32 ret,  const ServerEngine::PIRole& roleInfo);
	virtual void callback_getRole_exception(taf::Int32 ret);

	void sendErrorCode(int iErrorCode);

private:

	string m_strAccount;
	ServerEngine::VerifyKey m_verifyKey;
};

class NewRoleCallback:public ServerEngine::RolePrxCallback
{
public:

	NewRoleCallback(const string& strAccount, const string& strRoleName, int iWorldID,string channelLabel)
		:m_strAccount(strAccount), 
		m_strRoleName(strRoleName),
		m_iWorldID(iWorldID),
		m_strChannelLabel(channelLabel)
		{}

	virtual void callback_newRole(taf::Int32 ret);
	virtual void callback_newRole_exception(taf::Int32 ret);
	void sendErrorCode(int iErrorCode);

private:

	string m_strAccount;
	string m_strRoleName;
	int m_iWorldID;
	string m_strChannelLabel;
};

class CheckNameCallback:public ServerEngine::NamePrxCallback
{
public:

	CheckNameCallback(const string& strAccount, const string& strName):m_strAccount(strAccount), m_strName(strName){}
	virtual void callback_getNameDesc(taf::Int32 ret,  const ServerEngine::NameDesc& descInfo);
    virtual void callback_getNameDesc_exception(taf::Int32 ret);

private:

	string m_strAccount;
	string m_strName;
};

class LoginManager:public ComponentBase<IMessageListener, IID_IMessageListener>, public IUserStateManager, public ITimerCallback
{
public:

	LoginManager();
	~LoginManager();

	// IComponent Interface
	virtual bool initlize(const PropertySet& propSet);
	virtual void* queryInterface(IID interfaceId);

	// IMessageListener Interface
	virtual void onMessage(Uint32 dwMsgID, const char* pMsgBuff, Int32 nBuffLen);
	virtual std::vector<Uint32> getSupportMsgList();

	// IUserStateManager Interface
	virtual Int32 getUserState(const std::string& strAccount);
	virtual const UserInfo* getUserInfo(const std::string& strAccount);
	virtual size_t getUserSize();
	virtual void getAllUserQQ(std::vector<std::string>& qqList);
	virtual void kickUser(const std::string& strAccount, int iReason = 0, bool bSynSave = false);
	virtual Uint32 getActorByName(const std::string& strName);
	virtual EventServer* getEventServer(){return &m_eventServer;}
	virtual void addReserveGhost(HEntity hEntity);
	virtual HEntity getRichEntityByAccount(const std::string& strAccount);
	virtual HEntity getRichEntityByName(const string& strName);
	virtual int getActorSize(){return (int)m_entityNameMap.size(); }
	virtual void getRecommendList(vector<HEntity>& vecHEntity);
	virtual const map<string, UserInfo>& getUserMap();

	void removeReserveGhost(HEntity hGhost);

	// ITimerCallback Interface
	virtual void onTimer(int nEventId);

	void onReqALoginMsg(QxMessage* pQxMsg, const GSProto::CSMessage& msg);
	void onReqGetRole(QxMessage* pQxMsg, const GSProto::CSMessage& msg);
	void onReqNewRole(QxMessage* pQxMsg, const GSProto::CSMessage& msg);
	void onReqQueryFirstFight(QxMessage* pQxMsg, const GSProto::CSMessage& msg);
	void OnReqcheckNameCanUse(QxMessage* pQxMsg, const GSProto::CSMessage& msg);
	void onReqRefreshGold(QxMessage* pQxMsg, const GSProto::CSMessage& msg);
	
	void sendLoginResult(const string& strRsAddress, Int64 ddConn, int iResult);
	Uint32 makeClientVer(int iVerMain, int iVerFeature, int iVerBuild);
	void processGetRole(const ServerEngine::PIRole& roleInfo, ServerEngine::VerifyKey& verifyKey);
	void processNoRole(const string& strAccount);
	void processGetRoleError(const string& strAccount);

	
	void processOnlineRoleGet(const UserInfo* pUserInfo);
	bool checkNewHeroValid(int iHeroID);
	void fillNewActorData(ServerEngine::RoleSaveData& actorSaveData, const std::string& strAccount,  int iWorldID, const GSProto::Cmd_Cs_NewRole& reqInfo);
	void fillNewActorData(ServerEngine::RoleSaveData& actorSaveData, const std::string& strAccount, int iWorldID, const string& strName, const vector<CreateHeroDesc>& heroIDList);

	void fillHeroFormationSystemData(ServerEngine::RoleSaveData& actorSaveData, const vector<CreateHeroDesc>& heroIDList);
	void processNewRoleResult(const string& strAccount, int iResult);
	bool checkNameValid(const std::string& strAccount, const string& strRoleName);
	void processCheckNameRet(const string& strAccount, int iRet);
	void processCheckNameException(const string& strAccount, int iRet);
	void sendBattleInfo(const UserInfo* pUserInfo);
	void generateFirstFight();
	void sendBattleMemMsg(const UserInfo* pUserInfo, const GSProto::FightDataAll& fightDataAll);
	void sendBattleAction(const UserInfo* pUserInfo,  const GSProto::FightDataAll& fightDataAll);	
	void sendBattleFin(const UserInfo* pUserInfo);
	void fixFirstFightDlg(GSProto::FightDataAll& fightDataAll);
	void appendDlgAction(GSProto::FightDataAll& fightDataAll, int iDlgID);
	void addAccountSnapshot(const std::string& strAccount, int iWorldID);
	bool checkClientVersion(const string& strRsAddress, Int64 ddConn, const GSProto::Cmd_Cs_Alogin& csReq);

	void updateChannelInfo(UserInfo* pUserInfo, const GSProto::Cmd_Cs_Alogin csAlogin);

	bool parseOpenIDChannel(const string& strAccount, string& strOpenID, string& strChannel);

private:

	void removeUser(const std::string& strAccount);

private:

	typedef std::map<string, UserInfo> UserStateMap;
	UserStateMap m_userStateMap;

	typedef std::map<string, HEntity> EntityNameMap;
	EntityNameMap m_entityNameMap;

	EventServer m_eventServer;

	map<string, HEntity> m_reserveGhostMap;
	map<string, HEntity> m_reserveActorNameMap;

	ITimerComponent::TimerHandle m_hTimerHandle;
	ServerEngine::TimeResetValue m_maxOnline;

	ITimerComponent::TimerHandle m_initHandler;

	ServerEngine::BattleData m_firstBattleData;
	bool m_bGetedFirstBattle;

	ServerEngine::SnapshotPrx m_snapshotPrx;

	bool m_bIsActive;
};

#endif
