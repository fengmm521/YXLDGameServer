

#include "cocos2d.h"
#include "RefCountAutoPtr.h"
#include "ClientConect.h"
#include "RouterServer.pb.h"
#include "ClientSinkCallbackMgr.h"
#include "Script/FmScriptSys.h"
#include "Common.h"

USING_NS_CC;

NS_FM_BEGIN

static ISocketSystem* s_pSocketSystem = NULL;
static IClientSocket* s_pClientSocket = NULL;


void HelpMakeScMsg(ServerEngine::CSMessage& outPkg, int iCmd, const ::google::protobuf::Message& msgBody)
{
	outPkg.set_icmd(iCmd);
	msgBody.SerializeToString(outPkg.mutable_strmsgbody() );
}

void sendPkg(ServerEngine::CSMessage& outPkg)
{
	char szBuff[4096]= {0};
	unsigned short wLen = outPkg.ByteSize() + 2;
	wLen = htons(wLen);

	char* p = &szBuff[0];
	*(unsigned short*)p = wLen;
	p += 2;

	outPkg.SerializeWithCachedSizesToArray((unsigned char*)p);

	s_pClientSocket->sendData(&szBuff[0], outPkg.ByteSize() + 2);
}


bool parseRSMsg(ServerEngine::SCMessage& pkg, const char* pBuff, int iLen)
{
	bool bResult = pkg.ParseFromArray(pBuff+2, iLen-2);

	return bResult;
}

bool parseMsg(GSProto::SCMessage& pkg, const char* pBuff, int iLen)
{
	bool bResult = pkg.ParseFromArray(pBuff+2, iLen-2);

	return bResult;
}

void ClientSink::onConnect( int nErrorCode )
{
	CCLOG("connected|%d\n", nErrorCode);
	if(0 == nErrorCode)
	{
		//std::string strMsg = "GET\r\n";
		//s_pClientSocket->sendData(strMsg.c_str(), strMsg.size() );

		// 发送 RS_ROUTER_AUTH
		/*ServerEngine::CSMessage pkg;
		ServerEngine::CS_RS_Auth authMsg;
		authMsg.set_straccount("feiwuX");
		authMsg.set_strmd5passwd("11");

		HelpMakeScMsg(pkg, ServerEngine::RS_ROUTER_AUTH, authMsg);
		sendPkg(pkg);*/

#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS || defined(XH_DEBUG_IOS_LOGIN_UNDER_WIN32)
		// XH: 太J2恶心了。。
		ScriptSys::GetInstance().Execute("IOSLogin_ConnectSuc");
#else
		ScriptSys::GetInstance().Execute("Login_ConnectSuc");
#endif
	}
}

void ClientSink::onRecv( const char* pszData, int nLen )
{
	if (m_isConnectGameServer)
	{
		GSProto::SCMessage pkg;
		if(!parseMsg(pkg, pszData, nLen) )
		{
			return;
		}

		int iCmd = pkg.icmd();

		bool isProcess = ClientSinkCallbackMgr::GetInstance().ProcessPacket(iCmd, pkg);

		//if (!isProcess)
		{
			ScriptSys::GetInstance().DispatchPacket(iCmd, pkg.strmsgbody());
		}
	}
	else
	{
		ServerEngine::SCMessage pkg;
		if(!parseRSMsg(pkg, pszData, nLen) )
		{
			return;
		}

		int iCmd = pkg.icmd();
		
		bool isProcess = ClientSinkCallbackMgr::GetInstance().ProcessRSPacket(iCmd, pkg);

		//if (!isProcess)
		{
			ScriptSys::GetInstance().DispatchPacket(iCmd, pkg.strmsgbody());
		}
	}
	
	
	// auth 处理
	/*if(ServerEngine::RS_ROUTER_AUTH == iCmd)
	{
		ServerEngine::SC_RS_Auth autuRet;
		if(autuRet.ParseFromString(pkg.strmsgbody() ) )
		{
			autuRet.PrintDebugString();

			// 发送拉取服务器列表消息
			ServerEngine::CSMessage worldPkg;
			worldPkg.set_icmd(ServerEngine::RS_ROUTER_WORLDLIST);
			sendPkg(worldPkg);
		}
	}
	else if(ServerEngine::RS_ROUTER_WORLDLIST == iCmd)
	{
		ServerEngine::SC_RS_WorldList worldList;
		if(worldList.ParseFromString(pkg.strmsgbody()) )
		{
			worldList.PrintDebugString();

			// 发送登录消息
			ServerEngine::CSMessage aloginPkg;
			ServerEngine::CS_RS_ALogin loginBody;
			loginBody.set_iworldid(0);
			loginBody.set_strtransmsg("Hello");
			HelpMakeScMsg(aloginPkg, ServerEngine::RS_ROUTER_ALOGIN, loginBody);
			sendPkg(aloginPkg);
		}
	}

	*/
	//CCLOG("rcvdata|%s\n", pszData);
}

void ClientSink::onClosed()
{
	CCLOG("\nsocket closed!!");
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS || defined(XH_DEBUG_IOS_LOGIN_UNDER_WIN32)
	ScriptSys::GetInstance().Execute("IOSLogin_OnSocketClose");
#else
	ScriptSys::GetInstance().Execute("Socket_OnClose");
#endif
	m_isConnectGameServer = false;
}

ClientSink::ClientSink()
{
	m_isConnectGameServer = false;
	//ClientSinkCallbackMgr::GetInstance().Register(ServerEngine::RS_ROUTER_AUTH, OnAuth);
}

void ClientSink::OnAuth( int iCmd, ServerEngine::SCMessage& pkg )
{
	ServerEngine::SC_RS_Auth autuRet;
	if(autuRet.ParseFromString(pkg.strmsgbody() ) )
	{
		autuRet.PrintDebugString();

		// 发送拉取服务器列表消息
		ServerEngine::CSMessage worldPkg;
		worldPkg.set_icmd(ServerEngine::RS_ROUTER_WORLDLIST);
		sendPkg(worldPkg);
	}
}

void ClientSink::sendStringPacket( const char* pStr, int nLen )
{
	assert(nLen < 4094);
	char szBuff[4096]= {0};
	unsigned short wLen = nLen + 2;
	wLen = htons(wLen);

	char* p = &szBuff[0];
	*(unsigned short*)p = wLen;
	p += 2;

	memcpy(szBuff+2, pStr, nLen);

	s_pClientSocket->sendData(&szBuff[0], nLen + 2);
	
}

void ClientSink::Connect( string address, int port )
{
	if(s_pClientSocket)
	{
		delete s_pClientSocket;
		s_pClientSocket = s_pSocketSystem->createClientSocket();
		s_pClientSocket->setClientSink(&(ClientSink::GetInstance()));
	}

	GetClientSocket()->connect(address, port);
}

bool ClientSink::IsConnect()
{
	return GetClientSocket()->isConnect();
}

void ClientSink::SendPkg( ServerEngine::CSMessage& outPkg )
{
	sendPkg(outPkg);
}


ISocketSystem* GetSocketSystem()
{
	return s_pSocketSystem;
}

IClientSocket* GetClientSocket()
{
	return s_pClientSocket;
}

void InitSocket()
{
	if (NULL == s_pSocketSystem)
	{
		StreamParse2* pParse = new StreamParse2;
		s_pSocketSystem = createSocketSystem();
		s_pSocketSystem->setPacketParse(pParse);
		if(!s_pSocketSystem->initlize() )
		{
			assert(false);
		}
	}
	
	if (NULL == s_pClientSocket)
	{
		s_pClientSocket = s_pSocketSystem->createClientSocket();
		assert(s_pClientSocket);

		
		s_pClientSocket->setClientSink(&(ClientSink::GetInstance()));
	}
}

NS_FM_END