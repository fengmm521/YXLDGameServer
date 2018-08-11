#include "RobotServerPch.h"
#include "ClientPlayer.h"
#include "ClientState_Auth.h"

ClientPlayer::ClientPlayer(IClientSocket* pClientSocket, const string& strAccount, const string& strRoleName, int iWorldID, const string& strAICmd):
m_pClientSocket(pClientSocket), m_strAccount(strAccount), m_strRoleName(strRoleName), m_iWorldID(iWorldID), m_pCurrentState(NULL),
	m_strAICmd(strAICmd)
{
	gotoState(new ClientState_Auth(this) );

	ITimerComponent* pTimeAxis = getComponent<ITimerComponent>(COMPNAME_TimeAxis, IID_ITimerComponent);
	assert(pTimeAxis);
	
	m_hHandle = pTimeAxis->setTimer(this, 1, 1000, "ClientUpdate");
}

ClientPlayer::~ClientPlayer()
{
	ITimerComponent* pTimeAxis = getComponent<ITimerComponent>(COMPNAME_TimeAxis, IID_ITimerComponent);
	assert(pTimeAxis);

	pTimeAxis->killTimer(m_hHandle);
}


void ClientPlayer::OnConnect(int nErrorCode)
{	
	if(en_Connect_Fail == nErrorCode)
	{
		delete this;
		return;
	}

	SvrRunLog("Client|connected|%s", m_strAccount.c_str() );

	EventArgs args;
	getEventServer()->setEvent(EVENT_PLAYER_CONNECTED, args);
}


class LuaStackHelperRobot
{
public:
	
	LuaStackHelperRobot(lua_State* L)
	{
		m_luaState = L;
		m_nTop = lua_gettop(L);
	}

	~LuaStackHelperRobot()
	{
		lua_settop(m_luaState, m_nTop);
	}

private:
	lua_State*	m_luaState;
	int			m_nTop;
};


void pushMessage(lua_State* L, int iCmd, const string& strAccount, const char* pszData, int nLen)
{
	LuaStackHelperRobot keeper(L);

	lua_getglobal(L, "processMessage");
	if (!lua_isfunction(L, -1) )
	{
		FDLOG("Script")<<"[ScriptEngine::runFunction] processMessage is not a function"<<endl;
		return;
	}

	lua_pushnumber(L, iCmd);
	lua_pushstring(L, strAccount.c_str() );
	lua_pushlstring(L, pszData, nLen);
	int error = lua_pcall(L, 3, 0, 0);
	if (error)
	{
		std::string errMsg = lua_tostring(L, -1);
		FDLOG("Script")<<"run lua function:[processMessage]error:["<<errMsg.c_str()<<"]"<<endl;
		return;
	}
};

void ClientPlayer::OnRecv(const char* pszData, int nLen)
{
	GSProto::SCMessage scMessage;
	if(!scMessage.ParseFromArray(pszData + 2, nLen - 2) )
	{
		return;
	}

	if(m_pCurrentState)
	{
		m_pCurrentState->onMessage(scMessage);
	}

	// 消息传递到lua层，并且执行一次AI
	IScriptEngine* pScriptEngine = getComponent<IScriptEngine>(COMPNAME_ScriptEngine, IID_IScriptEngine);
	assert(pScriptEngine);
	
	pushMessage(pScriptEngine->getLuaState(), scMessage.icmd(), m_strAccount, pszData+2, nLen-2);
	if(m_pCurrentState)
	{
		m_pCurrentState->onUpdate();
	}
}

void ClientPlayer::OnClosed()
{
	delete this;
}

void ClientPlayer::onTimer(int nEventId)
{
	if(m_pCurrentState)
	{
		m_pCurrentState->onUpdate();
	}
}

void ClientPlayer::gotoState(ClientPlayerState_Base* pNewState)
{
	if(m_pCurrentState)
	{
		m_pCurrentState->leaveState();
		delete m_pCurrentState;
	}

	m_pCurrentState = pNewState;
	m_pCurrentState->enterState();
}

void ClientPlayer::sendBuff(const char* pData, int iLen)
{
	int iPackLen = iLen + 2;
	char szData[8192] = {0};
	Uint16 wLen = htons(iPackLen);

	char* p = &szData[0];
	*(Uint16*)p = wLen;

	p+=2;

	memcpy(p, pData, iLen);

	m_pClientSocket->SendData(&szData[0], iLen+2);
}

void ClientPlayer::sendMessage(int iMsgID)
{
	GSProto::CSMessage csMsg;
	csMsg.set_icmd(iMsgID);

	string strData;
	csMsg.SerializeToString(&strData);

	this->sendBuff(strData.c_str(), strData.size() );
}

void ClientPlayer::sendMessage(int iMsgID, const ::google::protobuf::Message& msgBody)
{
	GSProto::CSMessage csMsg;
	csMsg.set_icmd(iMsgID);

	msgBody.SerializeToString(csMsg.mutable_strmsgbody() );

	string strData;
	csMsg.SerializeToString(&strData);

	this->sendBuff(strData.c_str(), strData.size() );
}

void ClientPlayer::sendGmMsg(const string& strCmd)
{
	GSProto::Cmd_Cs_GmMsg gmMsgBody;
	gmMsgBody.set_strcmd(strCmd);

	sendMessage(GSProto::CMD_GM_MSG, gmMsgBody);
}





