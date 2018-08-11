#ifndef __CLIENTSTATE_AUTH_H__
#define __CLIENTSTATE_AUTH_H__

class ClientPlayer;


class ClientState_Auth:public ClientPlayerState_Base
{
public:

	ClientState_Auth(ClientPlayer* pPlayer);

	virtual void enterState();
	virtual void leaveState();
	virtual void onMessage(const GSProto::SCMessage& message);
	virtual void onUpdate();

	
	void onAuthMessage(const GSProto::SCMessage& message);
	void onWorldListMessage(const GSProto::SCMessage& message);
	void onEventConnected(EventArgs& args);

private:

	ClientPlayer* m_pClientPlayer;
};


#endif
