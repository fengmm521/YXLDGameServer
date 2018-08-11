#include "GameServer.h"
#include "GameServantImp.h"
#include "ICleanupManager.h"
#include "MessageWrapper.h"
#include "IUserStateManager.h"
#include "IMessageLayer.h"
#include "WatchThread.h"
#include "PacketSendThread.h"
#include "GameServer.pb.h"
#include "GameServerExt.pb.h"
#include "IMessageDispatch.h"
#include "ComponentConstDef.h"
#include "IMessageLayer.h"
#include <google/protobuf/stubs/common.h>
#include "IZoneTime.h"
#include "ComponentIdDef.h"
#include "Push.h"
#include "EntityEventDef.h"
#include "IShopSystem.h"
#include "IModifyDelegate.h"
#include "IFriendSystem.h"
#include "IEntityFactory.h"
#include "IJZEntityFactory.h"
#include "IItemFactory.h"

#include "IItemSystem.h"
#include "IHeroSystem.h"
#include "IDropFactory.h"
#include "IMailSystem.h"
#include "uuid.h"
#include "IItemFactory.h"
#include "ILegionFactory.h"
#include "IJZMessageLayer.h"




using namespace google::protobuf;

using namespace MINIAPR;

taf::Servant* g_pMainServant = NULL;

extern WatchThread g_watchThread;
extern PacketSendThread g_packerSendThread;
static bool s_bInitlizeFinish = false;

bool isInitlizeFinish(){return s_bInitlizeFinish;}
bool isGameServer(){return true;}


GameServantImp::GameServantImp()
{
}

GameServantImp::~GameServantImp()
{
}

void exitCleanUp()
{
	std::cout<<"do cleanUp"<<endl;

	ICleanUpManager* pCleanUpManager = getComponent<ICleanUpManager>("CleanUpManager", IID_ICleanUpManager);
	if(pCleanUpManager)
	{
		pCleanUpManager->doCleanUp();
	}
	else
	{
		SvrErrLog("CleanUp Fail Find CleanUpManager Fail");
	}
}

void catchSignalCleanUp(int nSignal)
{
	signal(SIGSEGV, SIG_DFL);
	signal(SIGINT, SIG_DFL);
	signal(SIGABRT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	signal(SIGTERM, SIG_DFL);
	signal(SIGFPE, SIG_DFL);
	
	exitCleanUp();

	
	raise(nSignal);
}


void GameServantImp::onTimer(int nEventId)
{
	
}

void JZServerLogHandler(LogLevel level, const char* filename, int line, const string& message)
{
	 static const char* level_names[] = { "INFO", "WARNING", "ERROR", "FATAL" };
	 FDLOG("Protobuf")<<level_names[level]<<"|"<<message<<"|"<<line<<"|"<<filename<<endl;
}

extern int luaopen_JZServerExport(lua_State * tolua_S);

void GameServantImp::initialize()
{
	atexit(exitCleanUp);
	
	signal(SIGSEGV, &catchSignalCleanUp);
	signal(SIGINT, &catchSignalCleanUp);
	signal(SIGABRT, &catchSignalCleanUp);
	signal(SIGQUIT, &catchSignalCleanUp);
	signal(SIGTERM, &catchSignalCleanUp);
	signal(SIGFPE, &catchSignalCleanUp);

	//忽略SIGPIPE
	TC_Common::ignorePipe();

	// 设置ProtobufLogger
	SetLogHandler(&JZServerLogHandler);

	// 设置异步毁掉Servant
	g_pMainServant= this;
	printf("%s\n", "initialize----------------0");
	bool bResult = getCompomentObjectManager()->loadScheme("GameServer.xml");
	if(!bResult)
	{
		assert(0);
		return;
	}
	printf("%s\n", "initialize----------------1");
	ITimerComponent* pTimerManager = getComponent<ITimerComponent>( "TimeAxis", IID_ITimerComponent);
	assert(pTimerManager);

	pTimerManager->setTimer(this, 1, 10000, "keepAlive");
	pTimerManager->setTimer(this, 2, 60*1000, "checkGeniusRole");

	IScriptEngine* pScriptEngine = getComponent<IScriptEngine>("ScriptEngine", IID_IScriptEngine);
	assert(pScriptEngine);
	printf("%s\n", "initialize----------------2");
	luaopen_JZServerExport(pScriptEngine->getLuaState() );

	string strFilePath = taf::ServerConfig::DataPath + "/script/main.lua";
	pScriptEngine->runFile(strFilePath.c_str() );

	printf("%s\n", "initialize----------------3");

	IMessageLayer* pMsgLayer = getComponent<IMessageLayer>("MessageLayer", IID_IMessageLayer);
	assert(pMsgLayer);
	g_packerSendThread.setMessageQueue(pMsgLayer->getCsMessageQueue() );
	g_packerSendThread.start();
	g_watchThread.start();

	printf("%s\n", "initialize----------------4");
	s_bInitlizeFinish = true;
}

void GameServantImp::destroy()
{
}

taf::Int32 GameServantImp::doRequest(taf::Int64 iConnId,const std::string & strAccount, int iWorldID, const std::string & sMsgPack,const std::string & sRsObjAddr,const map<std::string, std::string> & mClientParam,taf::JceCurrentPtr current)
{
	try
	{
		cout<<"recv request|"<<iConnId<<"|"<<sRsObjAddr<<endl;
		
		// 取消自动响应, 采用异步通知
		current->setResponse(false);	
		GameServantImp::async_response_doRequest(current, 0);
		IMessageDispatch* pMessageDispatch = getComponent<IMessageDispatch>("MessageDispatch", IID_IMessageDispatch);
		assert(pMessageDispatch);

		GSProto::CSMessage tmpCsMsg;
		if(!tmpCsMsg.ParseFromString(sMsgPack) )
		{
			SvrErrLog("rcv invalid msg|%s", strAccount.c_str() );
			
			return -1;
		}

		QxMessage tmpQxMsg;
		tmpQxMsg.strRsAddress = sRsObjAddr;
		tmpQxMsg.ddCon = iConnId;
		tmpQxMsg.strAccount = strAccount;
		tmpQxMsg.dwMsgID = tmpCsMsg.icmd();
		tmpQxMsg.pMsgDataBuff = (char*)&tmpCsMsg;
		tmpQxMsg.dwMsgLen = (Uint32)sizeof(tmpCsMsg);
		tmpQxMsg.iWorldID = iWorldID;

		stringstream ss;
		ss<<"doRequest_"<<tmpQxMsg.dwMsgID;
		string strProfName = ss.str();
		PROFILE_MONITOR(strProfName.c_str() );

		cout<<"dispatch|"<<strAccount<<"|"<<tmpCsMsg.icmd()<<endl;
		pMessageDispatch->dispatchMessage(tmpCsMsg.icmd(), (const char*)&tmpQxMsg, sizeof(tmpQxMsg) );

		IUserStateManager* pUserStateManager = getComponent<IUserStateManager>(COMPNAME_LoginManager, IID_IUserStateManager);
		assert(pUserStateManager);

		const UserInfo* pUserInfo = pUserStateManager->getUserInfo(strAccount);
		if(pUserInfo && (pUserInfo->nState == en_UserState_Running) && (pUserInfo->ddConnectID == iConnId) && (pUserInfo->strRsAddress == sRsObjAddr) )
		{
			IEntity* pEntity = getEntityFromHandle(pUserInfo->hEntity);
			assert(pEntity);
			pEntity->onMessage(&tmpQxMsg);

			// 更新最后PING时间
			((UserInfo*)pUserInfo)->dwLastPingTime = time(NULL);
		}
		else if(!pUserInfo || (pUserInfo->ddConnectID != iConnId) || (pUserInfo->strRsAddress != sRsObjAddr) )
		{
			ServerEngine::PushPrx prx = NULL;
			Application::getCommunicator()->stringToProxy(sRsObjAddr, prx);
			prx->doNotifyLoginOff(iConnId, "");
		}
		
		return 1;
	}
	__COMMON_EXCEPTION_CATCH_EXT__("doRequest")
	
	return 1;
}

taf::Int32 GameServantImp::doNotifyLoginOff(const std::string & strAccount,taf::Short nLoginOffCode,const std::string & sRsObjAddr,taf::Int64 iConnId,taf::JceCurrentPtr current)
{
	FDLOG("ConnClosed")<<strAccount<<endl;;

	IUserStateManager* pUserStateMgr = getComponent<IUserStateManager>(COMPNAME_LoginManager, IID_IUserStateManager);
	assert(pUserStateMgr);

	const UserInfo* pUserInfo = pUserStateMgr->getUserInfo(strAccount);
	if(!pUserInfo) return 0;

	if( (pUserInfo->strRsAddress == sRsObjAddr) && (pUserInfo->ddConnectID == iConnId) )
	{
		pUserStateMgr->kickUser(strAccount);	
	}

	return 1;
}

int GameServantImp::doResponse(ReqMessagePtr resp)
{
	PROFILE_MONITOR("GameServantImp::doResponse");
	try
	{
		ServantProxyCallbackPtr callbackHandle = resp->callback;
		callbackHandle->onDispatch(resp);
		return -1;
	}
	__COMMON_EXCEPTION_CATCH_EXT__("doResponse")

	return -1;
}

int GameServantImp::doResponseException(ReqMessagePtr resp)
{
	PROFILE_MONITOR("GameServantImp::doResponseException");
	try
	{
		ServantProxyCallbackPtr callbackHandle = resp->callback;
		callbackHandle->onDispatch(resp);
		return -1;
	}
	__COMMON_EXCEPTION_CATCH_EXT__("doResponseException")
		
	return -1;
}

void GameServantImp::getGameStatus(ServerEngine::GameQueryStatus &gameStatus,taf::JceCurrentPtr current)
{
	IUserStateManager* pUserstateManager = getComponent<IUserStateManager>(COMPNAME_LoginManager, IID_IUserStateManager);
	if(!pUserstateManager) return;

	gameStatus.iMemberCount = (int)pUserstateManager->getUserSize();
}

taf::Int32 GameServantImp::TbT_AddMoney(const std::string & strAccount,taf::Int32 iWorldID,taf::Int32 iCount,taf::JceCurrentPtr current)
{
	IUserStateManager* pUserStateManager = getComponent<IUserStateManager>(COMPNAME_LoginManager, IID_IUserStateManager);
	assert(pUserStateManager);
	const UserInfo* pUserInfo = pUserStateManager->getUserInfo(strAccount);
	if(!pUserInfo ||(0 == pUserInfo->hEntity))
	{
		FDLOG("Pay")<<"PayAccountNotOnline|"<<strAccount<<"|"<<iWorldID<<"|"<<iCount<<endl;
		return -1;
	}
	
	IEntity *pEntity = getEntityFromHandle(pUserInfo->hEntity);
	if(!pEntity)
	{
		FDLOG("Pay")<<"PayEntityNotOnline|"<<strAccount<<"|"<<iWorldID<<"|"<<iCount<<endl;
		return -1;
	}
	
	int iActorWorldID = pEntity->getProperty(PROP_ACTOR_WORLD, 0);
	if(iActorWorldID != iWorldID)
	{
		FDLOG("Pay")<<"PayEntityWorldInvalid|"<<strAccount<<"|"<<iWorldID<<"|"<<iCount<<endl;
		return -1;
	}

	IShopSystem *pShopSystem = static_cast<IShopSystem*>(pEntity->querySubsystem(IID_IShopSystem));
	assert(pShopSystem);
	
	bool res = pShopSystem->addGoldForPayment(iCount, "tongbutui");
	if(!res)
	{
		return -1;
	}
	


	return 1;
	
}

taf::Int32 GameServantImp::Comm_Pay_AddMoney(const std::string & strAccount,taf::Int32 iWorldID,taf::Int32 iCount,const std::string & strChannel,taf::JceCurrentPtr current)
{
	IUserStateManager* pUserStateManager = getComponent<IUserStateManager>(COMPNAME_LoginManager, IID_IUserStateManager);
	assert(pUserStateManager);
	const UserInfo* pUserInfo = pUserStateManager->getUserInfo(strAccount);
	if(!pUserInfo ||(0 == pUserInfo->hEntity))
	{
		FDLOG("Pay")<<"PayAccountNotOnline|"<<strAccount<<"|"<<iWorldID<<"|"<<iCount<<endl;
		return -1;
	}
	
	IEntity *pEntity = getEntityFromHandle(pUserInfo->hEntity);
	if(!pEntity)
	{
		FDLOG("Pay")<<"PayEntityNotOnline|"<<strAccount<<"|"<<iWorldID<<"|"<<iCount<<endl;
		return -1;
	}
	
	int iActorWorldID = pEntity->getProperty(PROP_ACTOR_WORLD, 0);
	if(iActorWorldID != iWorldID)
	{
		FDLOG("Pay")<<"PayEntityWorldInvalid|"<<strAccount<<"|"<<iWorldID<<"|"<<iCount<<endl;
		return -1;
	}

	IShopSystem *pShopSystem = static_cast<IShopSystem*>(pEntity->querySubsystem(IID_IShopSystem));
	assert(pShopSystem);
	
	bool res = pShopSystem->addGoldForPayment(iCount, strChannel);
	if(!res)
	{
		return -1;
	}

	return 1;
}

taf::Int32 GameServantImp::LJSDK_Pay_AddMoney(const std::string & strAccount,taf::Int32 iWorldID,taf::Int32 iCount,const std::string & strChannel,const std::string & strOrderId,taf::JceCurrentPtr current)
{
	IUserStateManager* pUserStateManager = getComponent<IUserStateManager>(COMPNAME_LoginManager, IID_IUserStateManager);
	assert(pUserStateManager);
	const UserInfo* pUserInfo = pUserStateManager->getUserInfo(strAccount);
	if(!pUserInfo ||(0 == pUserInfo->hEntity))
	{
		FDLOG("Pay")<<"PayAccountNotOnline|"<<strAccount<<"|"<<iWorldID<<"|"<<iCount<<endl;
		return -1;
	}
	
	IEntity *pEntity = getEntityFromHandle(pUserInfo->hEntity);
	if(!pEntity)
	{
		FDLOG("Pay")<<"PayEntityNotOnline|"<<strAccount<<"|"<<iWorldID<<"|"<<iCount<<endl;
		return -1;
	}
	
	int iActorWorldID = pEntity->getProperty(PROP_ACTOR_WORLD, 0);
	if(iActorWorldID != iWorldID)
	{
		FDLOG("Pay")<<"PayEntityWorldInvalid|"<<strAccount<<"|"<<iWorldID<<"|"<<iCount<<endl;
		return -1;
	}

	IShopSystem *pShopSystem = static_cast<IShopSystem*>(pEntity->querySubsystem(IID_IShopSystem));
	assert(pShopSystem);
	
	bool res = pShopSystem->addGoldForPayment(iCount, strChannel,strOrderId);
	if(!res)
	{
		return -1;
	}

	return 1;
}



int GameServantImp::doCustomMessage(bool bExpectIdle)
{
	try
	{
		PROFILE_MONITOR("doCustomMessage");
		IZoneTime* pZoneTime = getComponent<IZoneTime>("ZoneTime", IID_IZoneTime);
		assert(pZoneTime);

		pZoneTime->UpdateTime();

		ITimerComponent* pTimeAxis = getComponent<ITimerComponent>(COMPNAME_TimeAxis, IID_ITimerComponent);
		if(!pTimeAxis) return -1;

		pTimeAxis->runTimer();
		return 0;
	}
	__COMMON_EXCEPTION_CATCH_EXT__("doCustomMessage")

	return 0;
}

struct GetRoleBaseDataDB
{
	GetRoleBaseDataDB(taf::JceCurrentPtr current, ServerEngine::PKRole  pkRole,string strCallBackFlag)
		:m_current(current),
		m_pkRole(pkRole),
		m_strcallbackFlag(strCallBackFlag)
		{
		}
	virtual void operator()(int iRet, HEntity hEntity, ServerEngine::PIRole& roleInfo)
		{
			if( iRet != ServerEngine::en_RoleRet_OK)
			{
				return;
			}

			if( 0 != hEntity)
			{
				IEntity* pEntity = getEntityFromHandle(hEntity);
				assert(pEntity);

				if(m_strcallbackFlag == "getRoleBaseData" )
				{
					IFriendSystem* pFriendSys = static_cast<IFriendSystem*>(pEntity->querySubsystem(IID_IFriendSystem));
					assert(pFriendSys);
					string strJson;
					pFriendSys->GMOnlineGetActorBaseInfo(strJson);
					ServerEngine::Game::async_response_getRoleBaseData(m_current,1,strJson);
				}
				else if(m_strcallbackFlag == "getRoleBagData" )
				{
					IItemSystem *pItemSystem = static_cast<IItemSystem*>(pEntity->querySubsystem(IID_IItemSystem));
					assert(pItemSystem);
					string strJson;
					pItemSystem->GMOnlineGetBagData(strJson);
					ServerEngine::Game::async_response_getRoleBagData(m_current, 1 , strJson);
					
				}
				else if(m_strcallbackFlag == "getRoleHeroData")
				{
					IHeroSystem *pSystem = static_cast<IHeroSystem*>(pEntity->querySubsystem(IID_IHeroSystem));
					assert(pSystem);
					string strJson;
					pSystem->GMOnlineGetRoleHeroData(strJson);
					ServerEngine::Game::async_response_getRoleHeroData(m_current, 1 , strJson);
				}
			
			}
			else
			{

				//离线创建 Gost
				IJZEntityFactory* pEntityFactory = getComponent<IJZEntityFactory>(COMPNAME_EntityFactory, IID_IJZEntityFactory);
				assert(pEntityFactory);

				ServerEngine::ActorCreateContext actorCtx;
				actorCtx.strAccount = roleInfo.strAccount;
				ServerEngine::JceToObj(roleInfo.roleData, actorCtx.data);

				string strGhostCreateInfo = ServerEngine::JceToStr(actorCtx);
				IEntity* pGhost = pEntityFactory->getEntityFactory()->createEntity("Ghost", strGhostCreateInfo);
				if(!pGhost) return ;
				
				if(m_strcallbackFlag == "getRoleBaseData" )
				{
					IFriendSystem* pFriendSys = static_cast<IFriendSystem*>(pGhost->querySubsystem(IID_IFriendSystem));
					assert(pFriendSys);
					string strJson;
					pFriendSys->GMOnlineGetActorBaseInfo(strJson);
					ServerEngine::Game::async_response_getRoleBaseData(m_current,1,strJson);
				}
				else if(m_strcallbackFlag == "getRoleBagData" )
				{
					IItemSystem *pItemSystem = static_cast<IItemSystem*>(pGhost->querySubsystem(IID_IItemSystem));
					assert(pItemSystem);
					string strJson;
					pItemSystem->GMOnlineGetBagData(strJson);
					ServerEngine::Game::async_response_getRoleBagData(m_current, 1 , strJson);
					
				}
				else if(m_strcallbackFlag == "getRoleHeroData")
				{
					IHeroSystem *pSystem = static_cast<IHeroSystem*>(pGhost->querySubsystem(IID_IHeroSystem));
					assert(pSystem);
					string strJson;
					pSystem->GMOnlineGetRoleHeroData(strJson);
					ServerEngine::Game::async_response_getRoleHeroData(m_current, 1 , strJson);
				}
				
				delete pGhost;
			
			}
		}
	

private:
	taf::JceCurrentPtr m_current;
	ServerEngine::PKRole m_pkRole;
	string m_strcallbackFlag;

};


taf::Int32 GameServantImp::getRoleBaseData(const ServerEngine::PKRole & pkRole,std::string &strJson,taf::JceCurrentPtr current)
{
	current->setResponse(false);
	//拿玩家数据
	IModifyDelegate* pModifyDelegate = getComponent<IModifyDelegate>(COMPNAME_ModifyDelegate, IID_IModifyDelegate);
	assert(pModifyDelegate);
	pModifyDelegate->submitDelegateTask(pkRole,GetRoleBaseDataDB(current,pkRole,"getRoleBaseData"));
	return 1;
}

taf::Int32 GameServantImp::getRoleBagData(const ServerEngine::PKRole & pkRole,std::string &strJson,taf::JceCurrentPtr current)	
{

	current->setResponse(false);
	//拿玩家数据
	IModifyDelegate* pModifyDelegate = getComponent<IModifyDelegate>(COMPNAME_ModifyDelegate, IID_IModifyDelegate);
	assert(pModifyDelegate);
	pModifyDelegate->submitDelegateTask(pkRole,GetRoleBaseDataDB(current,pkRole,"getRoleBagData"));
	
	return 1;
}

taf::Int32 GameServantImp::getRoleHeroData(const ServerEngine::PKRole & pkRole,std::string &strJson,taf::JceCurrentPtr current)
{
	current->setResponse(false);
	//拿玩家数据
	IModifyDelegate* pModifyDelegate = getComponent<IModifyDelegate>(COMPNAME_ModifyDelegate, IID_IModifyDelegate);
	assert(pModifyDelegate);
	pModifyDelegate->submitDelegateTask(pkRole,GetRoleBaseDataDB(current,pkRole,"getRoleHeroData"));
	
	return 1;

}

taf::Int32 GameServantImp::getLegionMember(const std::string & strLegionName,std::string &strJson,taf::JceCurrentPtr current)
{
	current->setResponse(false);
	ILegionFactory *pLegionFac = getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory);
	assert(pLegionFac);
	int iRes = pLegionFac->GMOnlineGetLegionMemberNameList(strLegionName, strJson);
	if(-1 == iRes)
	{
		ServerEngine::Game::async_response_getLegionMember(current,-1,strJson);
	}
	else
	{
		ServerEngine::Game::async_response_getLegionMember(current,1,strJson);
	}
	return 1;
}

taf::Int32 GameServantImp::GMOnlneRollMessage(const std::string & strMsg,taf::JceCurrentPtr current)
{
	current->setResponse(false);
	GSProto::CMD_MARQUEE_SC scMsg;
	scMsg.set_marqueeid(GSProto::en_GMOnline_Send);

	scMsg.add_szstrmessage(strMsg);

	IJZMessageLayer* pMsgLayer = getComponent<IJZMessageLayer>(COMPNAME_MessageLayer, IID_IJZMessageLayer);
	assert(pMsgLayer);

	GSProto::SCMessage pkg;
	HelpMakeScMsg(pkg, GSProto::CMD_MARQUEE, scMsg);
	
	pMsgLayer->broadcastMsg(pkg);
	
	ServerEngine::Game::async_response_GMOnlneRollMessage(current,1);
	
	return 1;
}


struct GMOnlineSendActorMailCallBack
{

	GMOnlineSendActorMailCallBack(ServerEngine::MailData&  mailData, taf::JceCurrentPtr current)
		:m_mailData(mailData),
		m_current(current)
	{
		
	}

	virtual void operator()(int iRet, HEntity hTarget, ServerEngine::PIRole& roleInfo)
	{
		if( iRet != ServerEngine::en_RoleRet_OK)
		{
			FDLOG("GMOnlineSendActorMailCallBack")<<iRet <<endl;
			return;
		}

		if(0 != hTarget)
		{
			IEntity *pEntity = getEntityFromHandle(hTarget);
			assert(pEntity);
			IMailSystem*pMailSys = static_cast<IMailSystem*>(pEntity->querySubsystem( IID_IMailSystem));
			assert(pMailSys);
			pMailSys->addMail(m_mailData.strUUID,m_mailData, "GMOnlineAdd");
			ServerEngine::Game::async_response_sendRoleMail(m_current,1);
		}
		else
		{
			ServerEngine::RoleSaveData roleSaveData;
			ServerEngine::JceToObj(roleInfo.roleData, roleSaveData);
			map<taf::Int32, std::string>::iterator iter = roleSaveData.subsystemData.find(IID_IMailSystem);
			assert(iter != roleSaveData.subsystemData.end());
			ServerEngine::MailSystemData systemData;
			ServerEngine::JceToObj(iter->second, systemData);
			//修改数据
			systemData.mailDataList.push_back( m_mailData );
			//回写
			iter->second = ServerEngine::JceToStr(systemData);
			roleInfo.roleData = ServerEngine::JceToStr(roleSaveData);
			ServerEngine::Game::async_response_sendRoleMail(m_current,1);
		}
		
	}
private:
		ServerEngine::MailData	m_mailData;
		taf::JceCurrentPtr m_current;
};


taf::Int32 GameServantImp::sendRoleMail(const ServerEngine::PKRole & pkRole,const map<std::string, std::string> & mailMap,taf::JceCurrentPtr current)
{
	current->setResponse(false);
	//if(mailMap.size() != 5)
	if(mailMap.size() < 5)
	{
		ServerEngine::Game::async_response_sendRoleMail(current,-1);
		return -1;
	}
	
	IModifyDelegate* pModifyDelegate = getComponent<IModifyDelegate>(COMPNAME_ModifyDelegate, IID_IModifyDelegate);
	assert(pModifyDelegate);
	
	ServerEngine::MailData mailData;
	
	uuid_t itemuuid;
	uuid_generate(itemuuid);
	char szUUIDString[1024] = {0};
	uuid_unparse_upper(itemuuid, szUUIDString);

	map<std::string, std::string> tempMap = mailMap;
	mailData.strUUID = szUUIDString;
	mailData.strTitle = tempMap["title"];
	mailData.strSender =  tempMap["sender"];
	mailData.strContent =  tempMap["context"];
	mailData.iState = GSProto::en_MailState_UnRead;
	mailData.dwSenderTime = time(0);
	mailData.iKeepDay = 15;
	mailData.iMailType = GSProto::enMailType_Com;

	GSProto::FightAwardResult awardResult;
	if(tempMap["lifeatt"]!="" )
	{
		vector<int> dropIDList = TC_Common::sepstr<int>(tempMap["lifeatt"], "#");
		if(dropIDList.size() % 2 != 0)
		{
			ServerEngine::Game::async_response_sendRoleMail(current,-2);
			return-1;
		}
		
		for(size_t i =0; i < dropIDList.size(); i = i + 2)
		{
			int ilifeatt = dropIDList[i];
			if(ilifeatt == 1 || ilifeatt == 2 || ilifeatt == 4|| ilifeatt == 17  )
			{
				GSProto::PropItem &item = *(awardResult.add_szawardproplist());
				item.set_ilifeattid(ilifeatt);
				item.set_ivalue(dropIDList[i+1]);
			}
			else
			{
				ServerEngine::Game::async_response_sendRoleMail(current,-2);
				return -1;
			}
		}
	}

	if(tempMap["item"]!="")
	{
		vector<int> dropIDList = TC_Common::sepstr<int>(tempMap["item"], "#");
		if(dropIDList.size() % 2 != 0)
		{
			ServerEngine::Game::async_response_sendRoleMail(current,-2);
			return -1;
		}
		
		IItemFactory* pItemFactory = getComponent<IItemFactory>(COMPNAME_ItemFactory, IID_IItemFactory);
		assert(pItemFactory);
		for(size_t i =0; i < dropIDList.size(); i = i + 2)
		{
			int iItemID = dropIDList[i];
			const PropertySet* pItemProp = pItemFactory->getItemPropset(iItemID);
			if(!pItemProp) 
			{
				ServerEngine::Game::async_response_sendRoleMail(current,-2);
				return -1;
			}
			
			GSProto::FightAwardItem &item = *(awardResult.add_szawarditemlist());
			item.set_iitemid( iItemID);
			item.set_icount( dropIDList[i+1]);
		}
	}


	// pay
	if(tempMap["pay"]!="" )
	{
		vector<int> dropIDList = TC_Common::sepstr<int>(tempMap["pay"], "#");
		if(dropIDList.size() % 2 != 0)
		{
			ServerEngine::Game::async_response_sendRoleMail(current,-2);
			return-1;
		}
		
		for(size_t i =0; i < dropIDList.size(); i = i + 2)
		{
			int money = dropIDList[i];
			int count = dropIDList[i+1];
			GSProto::AwardPay& pay = *(awardResult.add_szawardpaylist());
			pay.set_money(money);
			pay.set_count(count);
		}
	}
	
	if(awardResult.IsInitialized() )
	{
		awardResult.SerializeToString(&mailData.strAwardInfo);
	}
	
	pModifyDelegate->submitDelegateTask(pkRole, GMOnlineSendActorMailCallBack(mailData,current));
	
	return 1;
}




