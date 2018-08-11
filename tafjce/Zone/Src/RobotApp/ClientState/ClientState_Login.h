#ifndef __CLIENTSTATE_LOGIN_H__
#define __CLIENTSTATE_LOGIN_H__

class ClientPlayer;


class ClientState_Login:public ClientPlayerState_Base
{
public:

	ClientState_Login(ClientPlayer* pPlayer);

	virtual void enterState();
	virtual void leaveState();
	virtual void onMessage(const GSProto::SCMessage& message);
	virtual void onUpdate();

	void onMessageAlogin(const GSProto::SCMessage& message);
	void onMessageGetRole(const GSProto::SCMessage& message);
	void onMessageNewRole(const GSProto::SCMessage& message);
	void onMessageRoleFin();

private:

	ClientPlayer* m_pClientPlayer;
};



#endif

