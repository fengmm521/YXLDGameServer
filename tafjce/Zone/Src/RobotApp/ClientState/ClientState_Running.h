#ifndef __CLIENTSTATE_RUNNING_H__
#define __CLIENTSTATE_RUNNING_H__

class ClientPlayer;


class ClientState_Running:public ClientPlayerState_Base
{
public:

	ClientState_Running(ClientPlayer* pPlayer);

	virtual void enterState();
	virtual void leaveState();
	virtual void onMessage(const GSProto::SCMessage& message);
	virtual void onUpdate();
	
private:

	ClientPlayer* m_pClientPlayer;
	Uint32 m_dwLastPing;
};



#endif

