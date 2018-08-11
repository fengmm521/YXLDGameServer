#ifndef __CLIENT_PLAYER_H__
#define __CLIENT_PLAYER_H__

#include "ClientPlayerState_Base.h"

enum
{
	EVENT_PLAYER_CONNECTED,
};


class ClientPlayer:public ObjectBase<IObject>, public IClientSink, public ITimerCallback
{
public:

	friend class ClientState_Auth;
	friend class ClientState_Login;
	friend class ClientState_Running;

	ClientPlayer(IClientSocket* pClientSocket, const string& strAccount, const string& strRoleName, int iWorldID, const string& strAICmd);
	~ClientPlayer();


	// IClientSink Interface
	virtual void OnConnect(int nErrorCode);
	virtual void OnRecv(const char* pszData, int nLen);
	virtual void OnClosed();

	// ITimerCallback
	virtual void onTimer(int nEventId);

	EventServer* getEventServer(){return &m_eventServer;}
	void gotoState(ClientPlayerState_Base* pNewState);

	void sendBuff(const char* pData, int iLen);
	void sendMessage(int iMsgID);
	void sendMessage(int iMsgID, const ::google::protobuf::Message& msgBody);
	void sendGmMsg(const string& strCmd);
	string getAccount(){return m_strAccount;}

private:

	EventServer m_eventServer;

	IClientSocket* m_pClientSocket;
	//int m_iIndex;
	string m_strAccount;
	string m_strRoleName;
	int m_iWorldID;

	ClientPlayerState_Base* m_pCurrentState;
	ITimerComponent::TimerHandle m_hHandle;
	string m_strAICmd;
};


extern string getDataPath();
int getRunSecond();
void logMsg(const string& strMgs);




#endif
