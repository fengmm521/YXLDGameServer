#include "RobotServerPch.h"
#include "ClientPlayer.h"
#include "RouterServer.pb.h"
#include "ClientState_Running.h"


ClientState_Running::ClientState_Running(ClientPlayer* pPlayer):m_pClientPlayer(pPlayer), m_dwLastPing(0)
{
}

void ClientState_Running::enterState()
{
	FDLOG("Robot")<<"Enter Game|"<<m_pClientPlayer->m_strAccount<<endl;
	//m_pClientPlayer->sendGmMsg("PVEFight#1");
	//m_pClientPlayer->sendGmMsg("AddItem#1#13");
	//m_pClientPlayer->sendGmMsg("RemoveItem#1#3");
	//m_pClientPlayer->sendGmMsg("RemoveItem#1#5");

	// 查询管卡
	/*GSProto::Cmd_Cs_QueryDSectionDetail queryPkg;
	queryPkg.set_isectionid(10000);
	m_pClientPlayer->sendMessage(GSProto::CMD_QUERY_DSECTIONDETAIL, queryPkg);*/


	// 战斗管卡
	//GSProto::Cmd_Cs_DungeonFight fightPkg;
	//fightPkg.set_isceneid(1);
	//m_pClientPlayer->sendMessage(GSProto::CMD_DUNGEON_FIGHT, fightPkg);

	// 领取奖励
	/*GSProto::Cmd_Cs_GetSectionAward getAwardPkg;
	getAwardPkg.set_isectionid(10000);
	m_pClientPlayer->sendMessage(GSProto::CMD_GET_SECTION_AWARD, getAwardPkg);*/

	// 添加经验
	//m_pClientPlayer->sendGmMsg("AddExp#10000");
	//m_pClientPlayer->sendGmMsg("AddHero#2");

	GSProto::Cmd_Cs_GoldAnimalActive reqMsg;
	reqMsg.set_dwobjectid(0);

	m_pClientPlayer->sendMessage(GSProto::CMD_GODANIMAL_ACTIVE, reqMsg);
}

void ClientState_Running::leaveState()
{
}

void ClientState_Running::onMessage(const GSProto::SCMessage& message)
{
	FDLOG("Robot")<<"Cmd|"<<message.icmd()<<endl;
	switch(message.icmd() )
	{
		/*case GSProto::CMD_FIGHT_MEMBER:
			parseAndShowMsg<GSProto::Cmd_Sc_FightMember>(message);
			break;

		case GSProto::CMD_FIGHT_ACTION:
			parseAndShowMsg<GSProto::Cmd_Sc_FightAction>(message);
			break;

		case GSProto::CMD_FIGHT_FIN:
			message.PrintDebugString();
			break;

		case GSProto::CMD_BAG_DATA:
			parseAndShowMsg<GSProto::Cmd_Sc_BagData>(message);
			break;

		case GSProto::CMD_BAG_CHG:
			parseAndShowMsg<GSProto::Cmd_Sc_BagChg>(message);
			break;

		case GSProto::CMD_COMM_FIGHTRESULT:
			parseAndShowMsg<GSProto::Cmd_Sc_CommFightResult>(message);
			break;

		case GSProto::CMD_SECTION_UPDATE:
			parseAndShowMsg<GSProto::Cmd_Sc_SectionUpdate>(message);
			break;

		case GSProto::CMD_QUERY_DSECTIONDETAIL:
			parseAndShowMsg<GSProto::Cmd_Sc_QueryDSectionDetail>(message);
			break;

		case GSProto::CMD_GET_SECTION_AWARD:
			parseAndShowMsg<GSProto::Cmd_Sc_GetSectionAward>(message);
			break;

		case GSProto::CMD_ACTOR_LEVELUP:
			parseAndShowMsg<GSProto::Cmd_Sc_ActorLevelUp>(message);
			break;

		case GSProto::CMD_FORMATION_DATA:
			parseAndShowMsg<GSProto::Cmd_Sc_FormationData>(message);
			break;

		case GSProto::CMD_HERO_UPDATE:
			parseAndShowMsg<GSProto::Cmd_Sc_HeroUpdate>(message);
			break;

		case GSProto::CMD_HERO_DEL:
			parseAndShowMsg<GSProto::Cmd_Sc_HeroDel>(message);
			break;*/

		case GSProto::CMD_GET_HERODESC:
			parseAndShowMsg<GSProto::Cmd_Sc_GetHeroDesc>(message);
			break;
	}
}

void ClientState_Running::onUpdate()
{
	// 发送ping
	if(time(0) - m_dwLastPing > 30)
	{
		m_pClientPlayer->sendMessage(GSProto::CMD_PING);
		m_dwLastPing = time(0);
	}
	
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


