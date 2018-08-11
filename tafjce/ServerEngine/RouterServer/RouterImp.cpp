#include "RouterImp.h"
#include "RouterComm.h"
#include "TimeoutThread.h"
#include "RouterServer.pb.h"
#include "ServerManager.h"
#include <google/protobuf/stubs/common.h>
#include <sys/time.h>
#include <sys/resource.h>


using namespace google::protobuf;


extern void sendSCErrorMsg(taf::JceCurrentPtr current, const string& strError);

string g_strLocalPushObj;

ConnectionManager::ConnectionManager()
{
	init();
}

ConnectionManager::~ConnectionManager()
{
}

void ConnectionManager::init()
{
	string strConfigFile = ServerConfig::ServerName + ".conf";
	TC_Config tmpCfg;
	tmpCfg.parseFile(strConfigFile);

	int iTimeOut = TC_Common::strto<int>(tmpCfg.get("/RouterServer/Property<timeout>", "300"));
	setTimeout(iTimeOut * 1000);
}


void ConnectionManager::update(int64_t ddConnID, ConnectionInfoPtr conInfoPtr)
{
	{
		TC_LockT<TC_ThreadMutex> lock(*this);
		typename data_type::iterator it = _data.find(ddConnID);
	    if(it != _data.end())
	    {
		   	NodeInfo& ndInfo = *(it->second.timeIter);
			
		   	struct timeval tv;
    	   	TC_TimeProvider::getInstance()->getNow(&tv);
			ndInfo.createTime = tv.tv_sec * (int64_t)1000 + tv.tv_usec/1000;

			_time.erase(it->second.timeIter);			
			it->second.timeIter = _time.insert(_time.end(), ndInfo);

	       	return;
	    }
	}

	push(conInfoPtr, ddConnID);
}


ConnectionInfoPtr ConnectionManager::get(int64_t ddConnID)
{
	return taf::TC_TimeoutQueue<ConnectionInfoPtr>::get(ddConnID, false);
}


void ConnectionManager::destroyConn(ConnectionInfoPtr& conInfo)
{
	FDLOG("connection")<<"timeoutClose|"<<conInfo->strAccount<<"|"<<conInfo->current->getIp()<<"|"<<conInfo->current->getPort()<<endl;
	// Í¨ÖªGS¶ÏÏß
	conInfo->current->close();

	if(conInfo->iState == en_ConnState_Connected)
	{
		conInfo->gamePrx->async_doNotifyLoginOff(NULL, conInfo->strAccount, ServerEngine::LOGINOFF_CODE_TIMEOUT, g_strLocalPushObj, conInfo->ddConnID);
	}
}

int64_t makeConKey(const string& strIp, int iPort)
{
	in_addr ipAddr;
	inet_aton(strIp.c_str(), &ipAddr);

	int64_t ret = ((int64_t)ipAddr.s_addr<<32)|iPort;

	return ret;
}

RouterImp::RouterImp()
{
}

RouterImp::~RouterImp()
{
}

void JZServerLogHandler(LogLevel level, const char* filename, int line, const string& message)
{
	 static const char* level_names[] = { "INFO", "WARNING", "ERROR", "FATAL" };
	 FDLOG("Protobuf")<<level_names[level]<<"|"<<message<<"|"<<line<<"|"<<filename<<endl;
}


class AccountRoleSnapshotCb:public ServerEngine::SnapshotPrxCallback
{
public:

	AccountRoleSnapshotCb(const string& strAccount, int64_t ddcon):m_strAccount(strAccount), m_ddConnection(ddcon){}

	virtual void callback_getRoleWorldDesc(taf::Int32 ret,  const ServerEngine::RoleSnapshot& snapDesc)
	{
		ConnectionInfoPtr conPtr = ConnectionManager::getInstance()->get(m_ddConnection);
		if(!conPtr || (ret == ServerEngine::en_SnapshotRet_Fail) )
		{
			FDLOG("Error")<<"getRoleWorldDesc error connect invalid|"<<ret<<"|"<<m_strAccount<<"|"<<m_ddConnection<<endl;
			return;
		}
	
		ServerManager::getInstance()->sendWorldList(conPtr->current, snapDesc);
	}

    virtual void callback_getRoleWorldDesc_exception(taf::Int32 ret)
    {
    	FDLOG("Error")<<"getRoleWorldDesc exception|"<<ret<< "|"<<m_strAccount<<"|"<<m_ddConnection<<endl;
    	ConnectionInfoPtr conPtr = ConnectionManager::getInstance()->get(m_ddConnection);
		if(!conPtr)
		{
			FDLOG("Error")<<"getRoleWorldDesc exception|connection error|"<<ret<< "|"<<m_strAccount<<"|"<<m_ddConnection<<endl;
			return;
		}
	
		ServerManager::getInstance()->sendWorldList(conPtr->current, ServerEngine::RoleSnapshot() );
    }

private:

	string m_strAccount;
	int64_t m_ddConnection;
};

void RouterImp::initialize()
{
	// ÉèÖÃ protobuf log
	SetLogHandler(&JZServerLogHandler);

	TC_Config tmpCfg;
	string strConfigFile = ServerConfig::ConfigFile;
	tmpCfg.parseFile(strConfigFile);

	// ÉèÖÃ×î´óÁ¬½ÓÊýÏÞÖÆ
	struct rlimit tmpLimit;
	tmpLimit.rlim_cur = 30000;
	tmpLimit.rlim_max = 30000;
	if(0 != setrlimit(RLIMIT_NOFILE, &tmpLimit) )
	{
		int iErrNO = errno;
		FDLOG("Error")<<"set socket rlimit fail|"<<iErrNO<<endl;
		assert(false);
		return;
	}

	string strEndPoint = tmpCfg.get("/taf/application/server/Push<endpoint>");
	m_strLocalPushObject = ServerConfig::Application + "." + "RouterServer" + ".PushObj@ " + strEndPoint;
	g_strLocalPushObj = m_strLocalPushObject;

	loadSnapshotServer();	
}


void RouterImp::loadSnapshotServer()
{	
	string strSnapshotObject = ServerManager::getInstance()->getSnapshotServerObj();
	assert(strSnapshotObject.size() > 0);

	Application::getCommunicator()->stringToProxy(strSnapshotObject, m_snapshotPrx);
}


class TransData2GameCb:public ServerEngine::GamePrxCallback
{
public:

	TransData2GameCb(const string& strGameObj):m_strGameObj(strGameObj){}
	virtual void callback_doRequest_exception(taf::Int32 ret)
	{
		TRLOG<<"doRequest exception|"<<m_strGameObj<<" ret|"<<ret<<endl;
	}

private:

	string m_strGameObj;
};

int RouterImp::doRequest(taf::JceCurrentPtr current, vector<char>& response)
{
	current->setResponse(false);

	const vector<char>& requestBuffer = current->getRequestBuffer();
	if(requestBuffer.size() < 2)
	{
		current->close();
		return 0;
	}

	int64_t ddConID = makeConKey(current->getIp(), current->getPort() );
	ConnectionInfoPtr conPtr = ConnectionManager::getInstance()->get(ddConID);
	if(!conPtr)
	{
		processNewConnection(current);
	}
	else
	{
		if(conPtr->iState == en_ConnState_Connected)
		{
			transToGameServer(current);
		}
		else if(conPtr->iState == en_ConnState_Half)
		{
			processHalfRequest(current);
		}
		else
		{
			FDLOG("Error")<<"invalid msg|"<<conPtr->iState<<"|"<<conPtr->strAccount<<"|"<<current->getIp()<<endl;
		}
	}

	return -1;
}

void RouterImp::processHalfRequest(taf::JceCurrentPtr current)
{
	int64_t ddConID = makeConKey(current->getIp(), current->getPort() );
	ConnectionInfoPtr conPtr = ConnectionManager::getInstance()->get(ddConID);
	assert(conPtr);

	const vector<char>& requestBuffer = current->getRequestBuffer();

	ServerEngine::CSMessage tmpCsMsg;
	bool bResult = tmpCsMsg.ParseFromArray(&requestBuffer[2], requestBuffer.size() - 2);
	if(!bResult)
	{
		current->close();
		return;
	}

	if(tmpCsMsg.icmd() == ServerEngine::RS_ROUTER_WORLDLIST)
	{
		FDLOG("connection")<<"queryWorldList|"<<conPtr->strAccount<<"|"<<current->getIp()<<"|"<<current->getPort()<<endl;
		ConnectionManager::getInstance()->update(ddConID, conPtr);

		// ÏÈÀ­È¡×´Ì¬, ÔÙÏÂ·¢
		m_snapshotPrx->async_getRoleWorldDesc(new AccountRoleSnapshotCb(conPtr->strAccount, ddConID),conPtr->strAccount);
		//ServerManager::getInstance()->sendWorldList(current);
	}
	else if(tmpCsMsg.icmd() == ServerEngine::RS_ROUTER_ALOGIN)
	{
		ServerEngine::CS_RS_ALogin tmpAlogin;
		if(!tmpAlogin.ParseFromString(tmpCsMsg.strmsgbody() ) )
		{
			current->close();
			return;
		}

		int iWorldIndex = tmpAlogin.iworldid();
		ServerStatus serverStatus;
		if(!ServerManager::getInstance()->getGameServer(iWorldIndex, serverStatus) )
		{
			current->close();
			return;
		}

		if(!serverStatus.bAlive)
		{
			sendSCErrorMsg(current, "服务器维护中，请耐心等待");
			return;
		}

		ConnectionManager::getInstance()->update(ddConID, conPtr);

		conPtr->strGameObject = serverStatus.strObject;
		conPtr->gamePrx = serverStatus.gamePrx;
		conPtr->iState = en_ConnState_Connected;
		conPtr->iWorldID = serverStatus.iWorldID;
		
		ServerEngine::GamePrxCallbackPtr tmpCb = new TransData2GameCb(conPtr->strGameObject);
		conPtr->gamePrx->async_doRequest(tmpCb, ddConID, conPtr->strAccount, conPtr->iWorldID, tmpAlogin.strtransmsg(), m_strLocalPushObject, map<std::string, std::string>() );		
		FDLOG("connection")<<"ALogin|"<<conPtr->strAccount<<"|"<<current->getIp()<<"|"<<current->getPort()<<endl;
	}
}
	
void RouterImp::transToGameServer(taf::JceCurrentPtr current)
{
	int64_t ddConID = makeConKey(current->getIp(), current->getPort() );
	ConnectionInfoPtr conPtr = ConnectionManager::getInstance()->get(ddConID);

	assert(conPtr);

	const vector<char>& requestBuffer = current->getRequestBuffer();

	ConnectionManager::getInstance()->update(ddConID, conPtr);
	
	ServerEngine::GamePrxCallbackPtr tmpCb = new TransData2GameCb(conPtr->strGameObject);
	conPtr->gamePrx->async_doRequest(tmpCb, ddConID, conPtr->strAccount, conPtr->iWorldID, string(&requestBuffer[2], requestBuffer.size() - 2), m_strLocalPushObject, map<std::string, std::string>() );

	FDLOG("connection")<<"trans msg|"<<conPtr->strAccount<<"|"<<conPtr->strGameObject<<endl;
}

void sendScMessage(taf::JceCurrentPtr current, const ServerEngine::SCMessage& scMsg)
{
	char szTmpBuff[scMsg.ByteSize() + 2];
	scMsg.SerializeToArray(&szTmpBuff[2], scMsg.ByteSize() );
	*(unsigned short*)szTmpBuff = htons(scMsg.ByteSize() + 2);
	current->sendResponse(szTmpBuff, scMsg.ByteSize() + 2);
}

void sendSCErrorMsg(taf::JceCurrentPtr current, const string& strError)
{
	ServerEngine::SCMessage scMsg;
	scMsg.set_icmd(ServerEngine::RS_ROUTER_ERROR);

	ServerEngine::SC_RS_Error errMsg;
	errMsg.set_strerrormsg(strError);

	errMsg.SerializeToString(scMsg.mutable_strmsgbody() );
	sendScMessage(current, scMsg);
}

void RouterImp::processNewConnection(taf::JceCurrentPtr current)
{
	const vector<char>& requestBuffer = current->getRequestBuffer();
	ServerEngine::CSMessage tmpCsMsg;
	bool bResult = tmpCsMsg.ParseFromArray(&requestBuffer[2], requestBuffer.size() - 2);
	if(!bResult)
	{
		current->close();
		return;
	}

	// ÓÐ¿ÉÄÜÊÇ²éÑ¯°æ±¾µÄÇëÇó
	if(tmpCsMsg.icmd() == ServerEngine::RS_ROUTER_VERSION)
	{
		//ServerEngine::SCMessage tmpScMsg;
		//tmpScMsg.set_icmd(ServerEngine::RS_ROUTER_VERSION);
		
		//ServerEngine::RS_ROUTER_VERSION_SC tmpVerMsg;
		//ServerManager::getInstance()->getResVersion(*tmpVerMsg.mutable_strurldir(), *tmpVerMsg.mutable_strversion(), *tmpVerMsg.mutable_strminversion() );
		
		//bool bSerializeResult = tmpVerMsg.SerializeToString(tmpScMsg.mutable_strmsgbody());
		//assert(bSerializeResult);

		//sendScMessage(current, tmpScMsg);

		// ÅÐ¶¨ÊÇ·ñ´øÓÐclientCode²ÎÊý
		bool hasCodeVersion = false;
			{
				bool hasMsgBody = tmpCsMsg.has_strmsgbody();
				if(hasMsgBody)
				{
					std::string msgBodyString = tmpCsMsg.strmsgbody();
					size_t msgBodyLength = msgBodyString.length();
					if(msgBodyLength != 0)
					{
						hasCodeVersion = true;
					}
				}
			}

		std::string codeVersion;
		bool noResUpdate = false;
		if(!hasCodeVersion)
		{
			// ÀÏ°æ±¾¿Í»§¶Ë
		}
		else
		{
			// ÐÂ°æ±¾¿Í»§¶Ë
			// È¡³öcodeVersion
			{
				ServerEngine::CS_RS_Version versionPkg;
				if(!versionPkg.ParseFromString(tmpCsMsg.strmsgbody()))
				{
					current->close();
					return;
				}
				codeVersion = versionPkg.codeversion();
			}

			// ¼ì²écodeVersionÊÇ·ñ½ûÖ¹¸üÐÂ
			noResUpdate = ServerManager::getInstance()->codeVersionNoResUpdate(codeVersion);
		}
		//printf("codeVersion:%s noResUpdate:%u\n", codeVersion.c_str(), noResUpdate);

		// ·µ»ØÏûÏ¢¸ø¿Í»§¶Ë
		ServerEngine::SCMessage tmpScMsg;
		tmpScMsg.set_icmd(ServerEngine::RS_ROUTER_VERSION);
	
		ServerEngine::RS_ROUTER_VERSION_SC scVersionPkg;
		ServerManager::getInstance()->getResVersion(*scVersionPkg.mutable_strurldir(), *scVersionPkg.mutable_strversion(), *scVersionPkg.mutable_strminversion() );
		scVersionPkg.set_noresupdate(noResUpdate);
		bool bSerializeResult = scVersionPkg.SerializeToString(tmpScMsg.mutable_strmsgbody());
		assert(bSerializeResult);
		sendScMessage(current, tmpScMsg);
		return;
	}

	ServerEngine::CS_RS_Auth tmpAuth;
	if(!tmpAuth.ParseFromString(tmpCsMsg.strmsgbody() ))
	{
		current->close();
		return;
	}

	int64_t ddConID = makeConKey(current->getIp(), current->getPort() );

	// Ìí¼ÓÐÂµÄÁ¬½Ó
	ConnectionInfoPtr conPtr = new ConnectionInfo;
	conPtr->strAccount = tmpAuth.straccount();
	conPtr->current = current;
	conPtr->ddConnID = ddConID;
	conPtr->iState = en_ConnState_Half;

	ConnectionManager::getInstance()->update(ddConID, conPtr);

	// ²âÊÔ»·¾³,ÑéÖ¤Ö±½Ó³É¹¦,Í¨Öª¿Í»§¶Ë
	{
		ServerEngine::SCMessage tmpScMsg;
		tmpScMsg.set_icmd(ServerEngine::RS_ROUTER_AUTH);

		ServerEngine::SC_RS_Auth tmpScAuth;
		tmpScAuth.set_straccount(tmpAuth.straccount() );
		tmpScAuth.set_iretcode(ServerEngine::RS_AUTU_OK);
		bool bSerializeResult = tmpScAuth.SerializeToString(tmpScMsg.mutable_strmsgbody());
		assert(bSerializeResult);

		sendScMessage(current, tmpScMsg);
	}

	FDLOG("connection")<<"new connection|"<<current->getIp()<<"|"<<current->getPort()<<"|"<<conPtr->strAccount<<endl;
}


int RouterImp::doResponse(ReqMessagePtr resp)
{
	return -1;
}


int RouterImp::doResponseException(ReqMessagePtr resp)
{
	return -1;
}


int RouterImp::doResponseNoRequest(ReqMessagePtr resp)
{
	return -1;
}


void RouterImp::destroy()
{
}



