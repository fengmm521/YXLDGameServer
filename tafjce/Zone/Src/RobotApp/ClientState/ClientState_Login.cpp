#include "RobotServerPch.h"
#include "ClientPlayer.h"
#include "RouterServer.pb.h"
#include "ClientState_Login.h"
#include "ClientState_Running.h"


ClientState_Login::ClientState_Login(ClientPlayer* pPlayer):m_pClientPlayer(pPlayer)
{
	
}

void ClientState_Login::enterState()
{
}

void ClientState_Login::leaveState()
{
}

void ClientState_Login::onMessage(const GSProto::SCMessage& message)
{
	int iCmd = message.icmd();

	switch(iCmd)
	{
		case GSProto::CMD_ALOGIN:
			onMessageAlogin(message);
			break;

		case GSProto::CMD_GETROLE:
			onMessageGetRole(message);
			break;

		case GSProto::CMD_NEW_ROLE:
			onMessageNewRole(message);
			break;

		case GSProto::CMD_ROLE_FIN:
			onMessageRoleFin();
			break;

		case GSProto::CMD_BAG_DATA:
			parseAndShowMsg<GSProto::Cmd_Sc_BagData>(message);
			break;

		case GSProto::CMD_DUNGEON_SECTIONLIST:
			parseAndShowMsg<GSProto::Cmd_Sc_DungeonSectionList>(message);
			break;

		case GSProto::CMD_FORMATION_DATA:
			parseAndShowMsg<GSProto::Cmd_Sc_FormationData>(message);
			break;

		case GSProto::CMD_HERO_LIST:
			parseAndShowMsg<GSProto::Cmd_Sc_HeroList>(message);
			break;
	}
}

void ClientState_Login::onMessageRoleFin()
{
	m_pClientPlayer->gotoState(new ClientState_Running(m_pClientPlayer) );
}


void ClientState_Login::onMessageAlogin(const GSProto::SCMessage& message)
{
	m_pClientPlayer->sendMessage(GSProto::CMD_GETROLE);
}

void ClientState_Login::onMessageGetRole(const GSProto::SCMessage& message)
{
	GSProto::Cmd_Sc_GetRole getRoleBody;
	if(!getRoleBody.ParseFromString(message.strmsgbody() ) )
	{
		return;
	}

	// 角色存在，等待RoleFin即可,否则验证名字
	if(getRoleBody.iresult() == GSProto::en_GetRole_NoRole)
	{
		/*GSProto::Cmd_Cs_CheckActorName checkRoleNameBody;
		checkRoleNameBody.set_stracorname(m_pClientPlayer->m_strRoleName);
		m_pClientPlayer->sendMessage(GSProto::CMD_CHECK_ACTORNAME, checkRoleNameBody);
		FDLOG("Robot")<<"check Role Name request|"<<m_pClientPlayer->m_strAccount<<"|"<<m_pClientPlayer->m_strRoleName<<endl;*/

		// 直接创建, 不走checkName流程了,本来也不用走这个流程
		GSProto::Cmd_Cs_NewRole newRoleBody;
		newRoleBody.set_strrolename(m_pClientPlayer->m_strRoleName);
		m_pClientPlayer->sendMessage(GSProto::CMD_NEW_ROLE, newRoleBody);
	}
	else if(getRoleBody.iresult() == GSProto::en_GetRole_OK)
	{
		FDLOG("Robot")<<"get Role|"<<m_pClientPlayer->m_strAccount<<"|OK"<<endl;
	}
}

void ClientState_Login::onMessageNewRole(const GSProto::SCMessage& message)
{
	GSProto::Cmd_Sc_NewRole newRoleBody;
	if(!newRoleBody.ParseFromString(message.strmsgbody() ) )
	{
		return;
	}

	if(newRoleBody.iresult() == GSProto::en_NewRole_OK)
	{
		FDLOG("Robot")<<"New Role OK|"<<m_pClientPlayer->m_strAccount<<"|"<<m_pClientPlayer->m_strRoleName<<endl;
		m_pClientPlayer->sendMessage(GSProto::CMD_GETROLE);
	}
}

void ClientState_Login::onUpdate()
{
	if(m_pClientPlayer->m_strAICmd.size() == 0)
	{
		return;
	}

	vector<string> paramList = TC_Common::sepstr<string>(m_pClientPlayer->m_strAICmd, "#");
	if(paramList.size() == 0) return;

	IScriptEngine* pScriptEngine = getComponent<IScriptEngine>(COMPNAME_ScriptEngine, IID_IScriptEngine);
	assert(pScriptEngine);

	EventArgs args;
	args.context.setString("cmd", m_pClientPlayer->m_strAICmd.c_str() );
	pScriptEngine->runFunction(paramList[0].c_str(), m_pClientPlayer, "ClientPlayer", &args, "EventArgs");
}


