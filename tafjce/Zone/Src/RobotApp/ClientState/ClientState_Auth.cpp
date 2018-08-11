#include "RobotServerPch.h"
#include "ClientPlayer.h"
#include "RouterServer.pb.h"
#include "ClientState_Auth.h"
#include "ClientState_Login.h"


ClientState_Auth::ClientState_Auth(ClientPlayer* pPlayer):m_pClientPlayer(pPlayer)
{
}

void ClientState_Auth::enterState()
{
	m_pClientPlayer->getEventServer()->subscribeEvent(EVENT_PLAYER_CONNECTED, this, &ClientState_Auth::onEventConnected);
}

void ClientState_Auth::onEventConnected(EventArgs& args)
{	
	ServerEngine::CS_RS_Auth authBody;
	authBody.set_straccount(m_pClientPlayer->m_strAccount);
	authBody.set_strmd5passwd("");

	m_pClientPlayer->sendMessage(ServerEngine::RS_ROUTER_AUTH, authBody);
}


void ClientState_Auth::leaveState()
{
	m_pClientPlayer->getEventServer()->subscribeEvent(EVENT_PLAYER_CONNECTED, this, &ClientState_Auth::onEventConnected);
}

void ClientState_Auth::onMessage(const GSProto::SCMessage& message)
{
	int iCmd = message.icmd();

	switch(iCmd)
	{
		case ServerEngine::RS_ROUTER_AUTH:
			onAuthMessage(message);
			break;

		case ServerEngine::RS_ROUTER_WORLDLIST:
			onWorldListMessage(message);
			break;
	}
}

void ClientState_Auth::onAuthMessage(const GSProto::SCMessage& message)
{
	ServerEngine::SC_RS_Auth authBody;
	if(!authBody.ParseFromString(message.strmsgbody() ) )
	{
		return;
	}

	if(authBody.iretcode() == ServerEngine::RS_AUTU_OK)
	{
		m_pClientPlayer->sendMessage(ServerEngine::RS_ROUTER_WORLDLIST);
	}
}

void ClientState_Auth::onWorldListMessage(const GSProto::SCMessage& message)
{
	ServerEngine::SC_RS_WorldList scWorldListBody;
	if(!scWorldListBody.ParseFromString(message.strmsgbody() ) )
	{
		return;
	}

	int iNeedWorldID = m_pClientPlayer->m_iWorldID;
	bool bServerAlive = false;
	for(int i = 0; i < scWorldListBody.worldlist_size(); i++)
	{
		const ServerEngine::RSWorld& worldItem = scWorldListBody.worldlist(i);
		if( (worldItem.iworldid() == iNeedWorldID) && (worldItem.ionlinestatus() == 1) )
		{
			bServerAlive = true;
			break;
		}
	}

	if(!bServerAlive)
	{
		FDLOG("Robot")<<"word|"<<iNeedWorldID<<"|not alive"<<endl;
		return;
	}

	ServerEngine::CS_RS_ALogin rsAloginBody;
	rsAloginBody.set_iworldid(iNeedWorldID);

	GSProto::CSMessage csMessage;
	csMessage.set_icmd(GSProto::CMD_ALOGIN);
	
	GSProto::Cmd_Cs_Alogin gsAloginBody;
	gsAloginBody.set_iisversioncheck(1);
	gsAloginBody.set_iversiontype(1);
	gsAloginBody.set_iversionmain(1);
	gsAloginBody.set_iversionfeature(1);
	gsAloginBody.set_iversionbuild(1);

	gsAloginBody.SerializeToString(csMessage.mutable_strmsgbody() );
	csMessage.SerializeToString(rsAloginBody.mutable_strtransmsg() );

	m_pClientPlayer->sendMessage(ServerEngine::RS_ROUTER_ALOGIN, rsAloginBody);

	// ½øÈëLoginState
	m_pClientPlayer->gotoState(new ClientState_Login(m_pClientPlayer) );
}


void ClientState_Auth::onUpdate()
{
}


