#include "GameEnginePch.h"
#include "MessageLayer.h"
#include "Push.h"
//#include "AegisGsInterface.h"

extern taf::Servant* g_pMainServant;

extern "C" IComponent* createMessageLayer(Int32)
{
	return new MessageLayer;
}

void HelpMakeScMsg(GSProto::SCMessage& outPkg, int iCmd, const ::google::protobuf::Message& msgBody)
{
	outPkg.set_icmd(iCmd);
	msgBody.SerializeToString(outPkg.mutable_strmsgbody() );
}


MessageLayer::MessageLayer()
{
}

MessageLayer::~MessageLayer()
{
}

bool MessageLayer::initlize(const PropertySet& /*propSet*/)
{
	initProxy();
    PROFILE_MONITOR("NewRole");

	// 默认为20M
	m_csMsgQueue.Create(1024*1024*50);
	//m_csMsgLayer = new CCSMsgLayer;
    
	return true;
}

void MessageLayer::sendPlayerMessage( const UserInfo *pUserInfo, const GSProto::SCMessage& mMsgPack)
{
	sendMessage2Connection( pUserInfo->strRsAddress, pUserInfo->ddConnectID, mMsgPack);
}

void MessageLayer::sendMessage2Connection(const std::string& strRsAddress, Int64 ddCon, const GSProto::SCMessage& msgPack)
{
	PROFILE_MONITOR("MessageLayer::sendMessage2Connection");

	char szMessage[64*1024] = {0};
	char* p = szMessage;

	*(Uint16*)p = (Uint16)strRsAddress.size();
	p+=2;

	memcpy(p, strRsAddress.c_str(), strRsAddress.size() );
	p += strRsAddress.size();
	
	*(Uint64*)p = (Uint64)ddCon;
	p += sizeof(Uint64);

	Uint32 dwBodySize = (Uint32)msgPack.ByteSize();
	*(Uint16*)p = (Uint16)dwBodySize+2;
	p += sizeof(Uint16);

	*(Uint16*)p = htons(dwBodySize + 2);
	p += sizeof(Uint16);
	Uint8* pEnd = msgPack.SerializeWithCachedSizesToArray((Uint8*)p);
	m_csMsgQueue.PushMsg(szMessage, (char*)pEnd - (char*)(&szMessage[0]) );
}


void MessageLayer::sendPlayerErrorMessage( const UserInfo *pUserInfo, int iErrorCode)
{
    sendErrorMessage2Connection( pUserInfo->strRsAddress, pUserInfo->ddConnectID, iErrorCode);
}

void MessageLayer::sendErrorMessage2Connection( const std::string& strRsAddress, Int64 ddCon, int iErrorCode)
{
	ITable* pTable = getCompomentObjectManager()->findTable("ErrorCode");
	assert(pTable);

	string strErrorMsg;
	int iRecord = pTable->findRecord(iErrorCode);
	int iType = GSProto::en_ErrorCodeType_Normal;
	if(iRecord < 0)
	{
		stringstream ss;
		ss<<"unknow error|"<<iErrorCode;
		strErrorMsg = ss.str();
	}
	else
	{
		strErrorMsg = pTable->getString(iRecord, "错误描述");
		iType = pTable->getInt(iRecord, "类型");
	}

	GSProto::SCMessage pkg;
	GSProto::Cmd_Sc_Error errMsg;
	errMsg.set_strerrormsg(strErrorMsg);
	errMsg.set_itype(iType);
	HelpMakeScMsg(pkg, GSProto::CMD_ERROR, errMsg);
	
	sendMessage2Connection(strRsAddress, ddCon, pkg);
}

int MessageLayer::NewRole(const std::string& strAccount, int iWorldID, int nRolePos, const std::string& strName, const ServerEngine::PIRole& data)
{
	PROFILE_MONITOR("NewRole");
	return getRolePrx(iWorldID)->newRole(strAccount, nRolePos, iWorldID, strName, data);
}

int MessageLayer::SaveRoleData(const ServerEngine::PKRole& key, const ServerEngine::PIRole& data)
{
	PROFILE_MONITOR("SaveRoleData");
	int nRet = getRolePrx(key.worldID)->updateRole(key, data);
	return nRet;
}

int MessageLayer::DelRoleData(const ServerEngine::PKRole& key)
{
	PROFILE_MONITOR("DelRoleData");
	int nRet = getRolePrx(key.worldID)->delRole(key);
	return nRet;
}


int MessageLayer::GetRoleData(const ServerEngine::PKRole& key, ServerEngine::PIRole& data)
{
	PROFILE_MONITOR("GetRoleData");
	int nRet = getRolePrx(key.worldID)->getRole(key, data);
	return nRet;
}

void MessageLayer::AsyncNewRole(const std::string& strAccount, int iWorldID, int nRolePos, const std::string& strName, const ServerEngine::PIRole& data, ServerEngine::RolePrxCallbackPtr callback)
{
	PROFILE_MONITOR("AsyncNewRole");
	PrxCallbackHelper<ServerEngine::RolePrxCallback>* pReplaceCallback = new PrxCallbackHelper<ServerEngine::RolePrxCallback>(g_pMainServant, callback);
	
	ServerEngine::RolePrxCallbackPtr replaceHandle = pReplaceCallback;
	getRolePrx(iWorldID)->async_newRole(replaceHandle, strAccount, nRolePos, iWorldID, strName, data);
}

void MessageLayer::AsyncUpdateRole(const ServerEngine::PKRole& key, const ServerEngine::PIRole& data, ServerEngine::RolePrxCallbackPtr callback)
{
	PROFILE_MONITOR("AsyncUpdateRole");


	PrxCallbackHelper<ServerEngine::RolePrxCallback>* pReplaceCallback = new PrxCallbackHelper<ServerEngine::RolePrxCallback>(g_pMainServant, callback);
	
	ServerEngine::RolePrxCallbackPtr replaceHandle = pReplaceCallback;
	getRolePrx(key.worldID)->async_updateRole(replaceHandle, key, data);
}

void MessageLayer::AsyncDelRole(const ServerEngine::PKRole& key,const string &sRoleName, ServerEngine::RolePrxCallbackPtr callback)
{
	PROFILE_MONITOR("AsyncDelRole");

	PrxCallbackHelper<ServerEngine::RolePrxCallback>* pReplaceCallback = new PrxCallbackHelper<ServerEngine::RolePrxCallback>(g_pMainServant, callback);
	
	ServerEngine::RolePrxCallbackPtr replaceHandle = pReplaceCallback;
	getRolePrx(key.worldID)->async_delRole(replaceHandle, key);
}


void MessageLayer::AsyncGetRole(const ServerEngine::PKRole& key, ServerEngine::RolePrxCallbackPtr callback)
{
	PROFILE_MONITOR("AsyncGetRole");
	PrxCallbackHelper<ServerEngine::RolePrxCallback>* pReplaceCallback = new PrxCallbackHelper<ServerEngine::RolePrxCallback>(g_pMainServant, callback);
	
	ServerEngine::RolePrxCallbackPtr replaceHandle = pReplaceCallback;
	getRolePrx(key.worldID)->async_getRole(replaceHandle, key);
}

void MessageLayer::AsyncGetNameDesc(const string& strName, ServerEngine::NamePrxCallbackPtr callback)
{
	PROFILE_MONITOR("checkNameCanUse");

	PrxCallbackHelper<ServerEngine::NamePrxCallback>* pReplaceCallback = new PrxCallbackHelper<ServerEngine::NamePrxCallback>(g_pMainServant, callback);
	ServerEngine::NamePrxCallbackPtr replaceHandle = pReplaceCallback;

	m_namePrx->async_getNameDesc(replaceHandle, strName);
}


void MessageLayer::AsyncGetRoleList(const std::string& strAccount, ServerEngine::RolePrxCallbackPtr callback)
{	

}

ServerEngine::RolePrx MessageLayer::getRolePrx(int iWorldID)
{
	map<int, ServerEngine::RolePrx>::iterator it = m_rolePrxMap.find(iWorldID);
	if(it == m_rolePrxMap.end() )
	{
		assert(false);
	}

	return it->second;
}

ServerEngine::FightDataPrx MessageLayer::getFightPrx(int iWorldID)
{
	map<int, ServerEngine::FightDataPrx>::iterator it = m_fightPrxMap.find(iWorldID);
	if(it == m_fightPrxMap.end() )
	{
		assert(false);
	}

	return it->second;
}

void MessageLayer::initProxy()
{
	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>("GlobalCfg", IID_IGlobalCfg);
	assert(pGlobalCfg);

	vector<string> roleServerList;
    if (pGlobalCfg->getGlobalCfg().getDomainVector("/OuterObj/RoleServer", roleServerList) )
    {
    	for(size_t i = 0; i < roleServerList.size(); i++)
    	{
    		int iTmpWorldID = TC_Common::strto<int>(pGlobalCfg->getGlobalCfg().get("/OuterObj/RoleServer/" + roleServerList[i] + "<world>") );
			string strTmpRoleAddress = pGlobalCfg->getGlobalCfg().get("/OuterObj/RoleServer/" + roleServerList[i] + "<obj>");
			Application::getCommunicator()->stringToProxy(strTmpRoleAddress, m_rolePrxMap[iTmpWorldID]);
    	}
    }

	

	string sNamePrxAddr = pGlobalCfg->getGlobalCfg().get("/OuterObj<NameServer>","Aegis.NameServer.NameObj");
	Application::getCommunicator()->stringToProxy(sNamePrxAddr, m_namePrx);

	vector<string> fightServerList;
	if (pGlobalCfg->getGlobalCfg().getDomainVector("/OuterObj/FightDataServer", fightServerList) )
	{
		for(size_t i = 0; i < fightServerList.size(); i++)
		{
			int iTmpWorldID = TC_Common::strto<int>(pGlobalCfg->getGlobalCfg().get("/OuterObj/FightDataServer/" + fightServerList[i] + "<world>") );
			string strTmpFightAddress = pGlobalCfg->getGlobalCfg().get("/OuterObj/FightDataServer/" + fightServerList[i] + "<obj>");
			Application::getCommunicator()->stringToProxy(strTmpFightAddress, m_fightPrxMap[iTmpWorldID]);
		}
	}

	string strCovertCodeServer = pGlobalCfg->getGlobalCfg().get("/OuterObj<ConvertCodeServer>", ""); 
	Application::getCommunicator()->stringToProxy(strCovertCodeServer, m_ConvertCodePrx);
		
}

void MessageLayer::AsyncSaveFightRecord(const ServerEngine::BattleData& fightData, ServerEngine::FightDataPrxCallbackPtr callback)
{
	PrxCallbackHelper<ServerEngine::FightDataPrxCallback>* pReplaceCallback = new PrxCallbackHelper<ServerEngine::FightDataPrxCallback>(g_pMainServant, callback);
	ServerEngine::FightDataPrxCallbackPtr replaceHandle = pReplaceCallback;

	string strSaveData = ServerEngine::JceToStr(fightData);
	getFightPrx(fightData.FightKey.iWorldID)->async_saveFightData(replaceHandle, fightData.FightKey, strSaveData);
}

void MessageLayer::AsyncGetFightRecord(const ServerEngine::PKFight& fightKey, ServerEngine::FightDataPrxCallbackPtr callback)
{
	PrxCallbackHelper<ServerEngine::FightDataPrxCallback>* pReplaceCallback = new PrxCallbackHelper<ServerEngine::FightDataPrxCallback>(g_pMainServant, callback);
	ServerEngine::FightDataPrxCallbackPtr replaceHandle = pReplaceCallback;

	getFightPrx(fightKey.iWorldID)->async_getFightData(replaceHandle, fightKey);
}

void MessageLayer::AsyncDelFightRecord(const ServerEngine::PKFight& fightKey, ServerEngine::FightDataPrxCallbackPtr callback)
{
	PrxCallbackHelper<ServerEngine::FightDataPrxCallback>* pReplaceCallback = new PrxCallbackHelper<ServerEngine::FightDataPrxCallback>(g_pMainServant, callback);
	ServerEngine::FightDataPrxCallbackPtr replaceHandle = pReplaceCallback;

	getFightPrx(fightKey.iWorldID)->async_delFightData(replaceHandle, fightKey);
}

CMsgCircleQueue* MessageLayer::getCsMessageQueue()
{
	return &m_csMsgQueue;
}

void MessageLayer::AsyncGetConvertCode(const std::string& strName, ServerEngine::ConvertCodePrxCallbackPtr callback)
{
	PrxCallbackHelper<ServerEngine::ConvertCodePrxCallback>* pReplaceCallback = new PrxCallbackHelper<ServerEngine::ConvertCodePrxCallback>(g_pMainServant, callback);
	ServerEngine::ConvertCodePrxCallbackPtr replaceHandle = pReplaceCallback;

	m_ConvertCodePrx->async_getConvertCode(replaceHandle, strName);
}

void MessageLayer::AsyncUpdateConvertCode(const std::string& strName, int state ,ServerEngine::ConvertCodePrxCallbackPtr callback)
{
	PrxCallbackHelper<ServerEngine::ConvertCodePrxCallback>* pReplaceCallback = new PrxCallbackHelper<ServerEngine::ConvertCodePrxCallback>(g_pMainServant, callback);
	ServerEngine::ConvertCodePrxCallbackPtr replaceHandle = pReplaceCallback;
	m_ConvertCodePrx->async_updateConvertCode(replaceHandle, strName, state);
}



