#include "CoreImpPch.h"
#include "JZMessageLayer.h"

extern taf::Servant* g_pMainServant;


extern "C" IComponent* createJZMessageLayer(Int32)
{
	return new JZMessageLayer;
}


JZMessageLayer::JZMessageLayer()
{
}

JZMessageLayer::~JZMessageLayer()
{
}

bool JZMessageLayer::initlize(const PropertySet& propSet)
{
	MessageLayer::initlize(propSet);

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>("GlobalCfg", IID_IGlobalCfg);
	assert(pGlobalCfg);

	string strLegionObj = pGlobalCfg->getGlobalCfg().get("/OuterObj<LegionServer>", "");
	Application::getCommunicator()->stringToProxy(strLegionObj, m_legionPrx);

	string strFightObj = pGlobalCfg->getGlobalCfg().get("/OuterObj<FightServer>", "");
	Application::getCommunicator()->stringToProxy(strFightObj, m_fightPrx);

	string strNameObj = pGlobalCfg->getGlobalCfg().get("/OuterObj<NameServer>", "");
	Application::getCommunicator()->stringToProxy(strNameObj, m_namePrx);

	string strQQYYBPayObj = pGlobalCfg->getGlobalCfg().get("/OuterObj<QQYYBPayServer>", "");
	Application::getCommunicator()->stringToProxy(strQQYYBPayObj, m_qqyybPayPrx);

	string strPayReturnObj = pGlobalCfg->getGlobalCfg().get("/OuterObj<PayReturnServer>", "");
	Application::getCommunicator()->stringToProxy(strPayReturnObj, m_PayReturnPrx);

	string strLJSDKObj = pGlobalCfg->getGlobalCfg().get("/OuterObj<LJSDKServer>", "");
	Application::getCommunicator()->stringToProxy(strLJSDKObj, m_LJSDKPrx);

	
	return true;
}

void* JZMessageLayer::queryInterface(IID interfaceId)
{
	if(IID_IMessageLayer == interfaceId)
	{
		return static_cast<IMessageLayer*>(this);
	}
	else if(IID_IJZMessageLayer == interfaceId)
	{
		return static_cast<IJZMessageLayer*>(this);
	}

	return NULL;
}

IMessageLayer* JZMessageLayer::getMessageLayer()
{
	return static_cast<IMessageLayer*>(this);
}

void JZMessageLayer::AsyncGetLegionList(ServerEngine::LegionPrxCallbackPtr cb)
{
	PrxCallbackHelper<ServerEngine::LegionPrxCallback>* pReplaceCallback = new PrxCallbackHelper<ServerEngine::LegionPrxCallback>(g_pMainServant, cb);
	ServerEngine::LegionPrxCallbackPtr replaceHandle = pReplaceCallback;

	m_legionPrx->async_getLegionList(replaceHandle);
}

void JZMessageLayer::AsyncNewLegion(ServerEngine::LegionPrxCallbackPtr cb, const string& strLegionName, const string& strData)
{
	PrxCallbackHelper<ServerEngine::LegionPrxCallback>* pReplaceCallback = new PrxCallbackHelper<ServerEngine::LegionPrxCallback>(g_pMainServant, cb);
	ServerEngine::LegionPrxCallbackPtr replaceHandle = pReplaceCallback;

	m_legionPrx->async_newLegion(replaceHandle, strLegionName, strData);
}

void JZMessageLayer::AsyncGetLegionData(ServerEngine::LegionPrxCallbackPtr cb, const string& strKey)
{
	PrxCallbackHelper<ServerEngine::LegionPrxCallback>* pReplaceCallback = new PrxCallbackHelper<ServerEngine::LegionPrxCallback>(g_pMainServant, cb);
	ServerEngine::LegionPrxCallbackPtr replaceHandle = pReplaceCallback;

	m_legionPrx->async_getLegion(cb, strKey);
}

void JZMessageLayer::AsyncSetLegionData(ServerEngine::LegionPrxCallbackPtr cb, const string& strKey, const string& strValue)
{
	PrxCallbackHelper<ServerEngine::LegionPrxCallback>* pReplaceCallback = new PrxCallbackHelper<ServerEngine::LegionPrxCallback>(g_pMainServant, cb);
	ServerEngine::LegionPrxCallbackPtr replaceHandle = pReplaceCallback;

	m_legionPrx->async_updateLegion(replaceHandle, strKey, strValue);
}

int JZMessageLayer::setLegionData(const string& strKey, const string& strValue)
{
	return m_legionPrx->updateLegion(strKey, strValue);
}

int JZMessageLayer::getLegionData(const string& strKey, string& strValue)
{
	return m_legionPrx->getLegion(strKey, strValue);
}

void JZMessageLayer::AsyncDelLegionData(ServerEngine::LegionPrxCallbackPtr cb, const string& strKey)
{
	PrxCallbackHelper<ServerEngine::LegionPrxCallback>* pReplaceCallback = new PrxCallbackHelper<ServerEngine::LegionPrxCallback>(g_pMainServant, cb);
	ServerEngine::LegionPrxCallbackPtr replaceHandle = pReplaceCallback;

	m_legionPrx->async_delLegion(cb, strKey);
}


void JZMessageLayer::broadcastMsg(const GSProto::SCMessage& msg)
{
	IUserStateManager* pUserStateMgr = getComponent<IUserStateManager>(COMPNAME_LoginManager, IID_IUserStateManager);
	assert(pUserStateMgr);

	char szBody[64*1024] = {0};
	int iBodySize = 0;
	{
		char* p = &szBody[0];

		Uint32 dwBodySize = (Uint32)msg.ByteSize();
		*(Uint16*)p = (Uint16)dwBodySize+2;
		p += sizeof(Uint16);

		*(Uint16*)p = htons(dwBodySize + 2);
		p += sizeof(Uint16);
		Uint8* pEnd = msg.SerializeWithCachedSizesToArray((Uint8*)p);
		iBodySize = (int)( (char*)pEnd - &szBody[0]);
	}
	

	const map<string, UserInfo>& refUserMap = pUserStateMgr->getUserMap();
	for(map<string, UserInfo>::const_iterator it = refUserMap.begin(); it != refUserMap.end(); it++)
	{
		if(0 == it->second.hEntity) continue;

		char szFullMsg[64*1024];
		char* p = &szFullMsg[0];
		*(Uint16*)p = (Uint16)it->second.strRsAddress.size();
		p+=2;
		
		memcpy(p, it->second.strRsAddress.c_str(), it->second.strRsAddress.size() );
		p += it->second.strRsAddress.size();
			
		*(Uint64*)p = (Uint64)it->second.ddConnectID;
		p += sizeof(Uint64);

		// copy body
		memcpy(p, &szBody[0], iBodySize);
		p+= iBodySize;
		
		getCsMessageQueue()->PushMsg(&szFullMsg[0], p - &szFullMsg[0]);
	}
}

int JZMessageLayer::getGlobalData(const string& strKey, string& strValue)
{
	return m_legionPrx->getGlobalData(strKey, strValue);
}

int JZMessageLayer::setGlobalData(const string& strKey, const string& strValue)
{
	return m_legionPrx->setGlobalData(strKey, strValue);
}

int JZMessageLayer::delGlobalData(const string& strKey)
{
	return m_legionPrx->delGlobalData(strKey);
}

void JZMessageLayer::AsyncGetGlobalData(ServerEngine::LegionPrxCallbackPtr cb, const string& strKey)
{
	PrxCallbackHelper<ServerEngine::LegionPrxCallback>* pReplaceCallback = new PrxCallbackHelper<ServerEngine::LegionPrxCallback>(g_pMainServant, cb);
	ServerEngine::LegionPrxCallbackPtr replaceHandle = pReplaceCallback;

	m_legionPrx->async_getGlobalData(replaceHandle, strKey);
}

void JZMessageLayer::AsyncSetGlobalData(ServerEngine::LegionPrxCallbackPtr cb, const string& strKey, const string& strValue)
{
	PrxCallbackHelper<ServerEngine::LegionPrxCallback>* pReplaceCallback = new PrxCallbackHelper<ServerEngine::LegionPrxCallback>(g_pMainServant, cb);
	ServerEngine::LegionPrxCallbackPtr replaceHandle = pReplaceCallback;

	m_legionPrx->async_setGlobalData(replaceHandle, strKey, strValue);
}

void JZMessageLayer::AsyncDelGlobalData(ServerEngine::LegionPrxCallbackPtr cb, const string& strKey)
{
	PrxCallbackHelper<ServerEngine::LegionPrxCallback>* pReplaceCallback = new PrxCallbackHelper<ServerEngine::LegionPrxCallback>(g_pMainServant, cb);
	ServerEngine::LegionPrxCallbackPtr replaceHandle = pReplaceCallback;

	m_legionPrx->async_delGlobalData(replaceHandle, strKey);
}


void JZMessageLayer::AsyncPVEFight(ServerEngine::FightPrxCallbackPtr cb, const ServerEngine::ActorCreateContext & roleInfo, taf::Int32 iMonsterGrpID,const ServerEngine::FightContext & ctx)
{
	PrxCallbackHelper<ServerEngine::FightPrxCallback>* pReplaceCallback = new PrxCallbackHelper<ServerEngine::FightPrxCallback>(g_pMainServant, cb);
	ServerEngine::FightPrxCallbackPtr replaceHandle = pReplaceCallback;

	m_fightPrx->async_doPVEFight(replaceHandle, roleInfo, iMonsterGrpID, ctx);
}

void JZMessageLayer::AsyncMultiPVEFight(ServerEngine::FightPrxCallbackPtr cb, const ServerEngine::ActorCreateContext & roleInfo, vector<taf::Int32> monsterGrpList,const ServerEngine::FightContext & ctx)
{
	PrxCallbackHelper<ServerEngine::FightPrxCallback>* pReplaceCallback = new PrxCallbackHelper<ServerEngine::FightPrxCallback>(g_pMainServant, cb);
	ServerEngine::FightPrxCallbackPtr replaceHandle = pReplaceCallback;

	m_fightPrx->async_doMultiPVEFight(replaceHandle, roleInfo, monsterGrpList, ctx);
}


void JZMessageLayer::AsyncPVPFight(ServerEngine::FightPrxCallbackPtr cb, const ServerEngine::ActorCreateContext & attRoleInfo, const ServerEngine::ActorCreateContext & targetRileInfo, const ServerEngine::FightContext & ctx)
{
	PrxCallbackHelper<ServerEngine::FightPrxCallback>* pReplaceCallback = new PrxCallbackHelper<ServerEngine::FightPrxCallback>(g_pMainServant, cb);
	ServerEngine::FightPrxCallbackPtr replaceHandle = pReplaceCallback;

	m_fightPrx->async_doPVPFight(replaceHandle, attRoleInfo, targetRileInfo, ctx);
}

void JZMessageLayer::AsyncGetNameDesc(ServerEngine::NamePrxCallbackPtr cb,const string& strKey )
{
	PrxCallbackHelper<ServerEngine::NamePrxCallback>* pReplaceCallback = new PrxCallbackHelper<ServerEngine::NamePrxCallback>(g_pMainServant,cb);
	ServerEngine::NamePrxCallbackPtr replaceHandle = pReplaceCallback;

	m_namePrx->async_getNameDesc(replaceHandle,strKey);
}


void JZMessageLayer::AsyncContinuePVEFight(ServerEngine::FightPrxCallbackPtr cb, const ServerEngine::ActorCreateContext & roleInfo, taf::Int32 iMonsterGrpID,const ServerEngine::FightContext & ctx)
{
	PrxCallbackHelper<ServerEngine::FightPrxCallback>* pReplaceCallback = new PrxCallbackHelper<ServerEngine::FightPrxCallback>(g_pMainServant, cb);
	ServerEngine::FightPrxCallbackPtr replaceHandle = pReplaceCallback;

	m_fightPrx->async_continuePVEFight(replaceHandle, roleInfo, iMonsterGrpID, ctx);
}

void JZMessageLayer::AsyncContinuePVPFight(ServerEngine::FightPrxCallbackPtr cb, const ServerEngine::ActorCreateContext& attRoleInfo, const ServerEngine::ActorCreateContext & targetRileInfo, const ServerEngine::FightContext & ctx)
{
	PrxCallbackHelper<ServerEngine::FightPrxCallback>* pReplaceCallback = new PrxCallbackHelper<ServerEngine::FightPrxCallback>(g_pMainServant, cb);
	ServerEngine::FightPrxCallbackPtr replaceHandle = pReplaceCallback;

	m_fightPrx->async_continuePVPFight(replaceHandle, attRoleInfo, targetRileInfo, ctx);
}


void JZMessageLayer::AsyncContinueMultiPVEFight(ServerEngine::FightPrxCallbackPtr cb, const ServerEngine::ActorCreateContext & roleInfo, vector<taf::Int32> monsterGrpList,const ServerEngine::FightContext & ctx)
{
	PrxCallbackHelper<ServerEngine::FightPrxCallback>* pReplaceCallback = new PrxCallbackHelper<ServerEngine::FightPrxCallback>(g_pMainServant, cb);
	ServerEngine::FightPrxCallbackPtr replaceHandle = pReplaceCallback;

	m_fightPrx->async_continueMultiPVEFight(replaceHandle, roleInfo, monsterGrpList, ctx);
}


void JZMessageLayer::AsyncQueryQQYYBYuanBao(ServerEngine::QQYYBPayPrxCallbackPtr cb, const ServerEngine::QQYYBPayQueryKey& key)
{
	PrxCallbackHelper<ServerEngine::QQYYBPayPrxCallback>* pReplaceCallback = new PrxCallbackHelper<ServerEngine::QQYYBPayPrxCallback>(g_pMainServant, cb);
	ServerEngine::QQYYBPayPrxCallbackPtr replaceHandle = pReplaceCallback;

	m_qqyybPayPrx->async_queryYuanBao(replaceHandle, key);
}

void JZMessageLayer::AsyncPayQQYYBYuanBao(ServerEngine::QQYYBPayPrxCallbackPtr cb, const ServerEngine::QQYYBPayQueryKey& key, int iCostNum)
{
	PrxCallbackHelper<ServerEngine::QQYYBPayPrxCallback>* pReplaceCallback = new PrxCallbackHelper<ServerEngine::QQYYBPayPrxCallback>(g_pMainServant, cb);
	ServerEngine::QQYYBPayPrxCallbackPtr replaceHandle = pReplaceCallback;

	m_qqyybPayPrx->async_payYuanBao(replaceHandle, key, iCostNum);
}

void JZMessageLayer::AsyncPayReturnMoneyAndVip(ServerEngine::PayReturnPrxCallbackPtr cb, const string& account)
{
	PrxCallbackHelper<ServerEngine::PayReturnPrxCallback>* pReplaceCallback = new PrxCallbackHelper<ServerEngine::PayReturnPrxCallback>(g_pMainServant, cb);
	ServerEngine::PayReturnPrxCallbackPtr replaceHandle = pReplaceCallback;
	m_PayReturnPrx->async_payReturnMoneyAddVIP(replaceHandle, account);
}

void JZMessageLayer::AsyncVrifyUserLogin(ServerEngine::LJSDKPrxCallbackPtr cb, const ServerEngine::VerifyKey verifyKey)
{
	PrxCallbackHelper<ServerEngine::LJSDKPrxCallback>* pReplaceCallback = new PrxCallbackHelper<ServerEngine::LJSDKPrxCallback>(g_pMainServant, cb);
	ServerEngine::LJSDKPrxCallbackPtr replaceHandle = pReplaceCallback;
	m_LJSDKPrx->async_verifyUserLogin(replaceHandle, verifyKey);
}

void JZMessageLayer::AsyncGetPhoneStream(ServerEngine::LJSDKPrxCallbackPtr cb,const string& strUrl)
{
	PrxCallbackHelper<ServerEngine::LJSDKPrxCallback>* pReplaceCallback = new PrxCallbackHelper<ServerEngine::LJSDKPrxCallback>(g_pMainServant, cb);
	ServerEngine::LJSDKPrxCallbackPtr replaceHandle = pReplaceCallback;
	m_LJSDKPrx->async_getPhoneStream(replaceHandle, strUrl);
}

void JZMessageLayer::AsyncGetPrePayInfo(ServerEngine::LJSDKPrxCallbackPtr cb, const string& strPostContext)
{
	PrxCallbackHelper<ServerEngine::LJSDKPrxCallback>* pReplaceCallback = new PrxCallbackHelper<ServerEngine::LJSDKPrxCallback>(g_pMainServant, cb);
	ServerEngine::LJSDKPrxCallbackPtr replaceHandle = pReplaceCallback;
	m_LJSDKPrx->async_VIVOPostPerPay(replaceHandle, strPostContext);
}


void JZMessageLayer::AsyncIosPay(	ServerEngine::LJSDKPrxCallbackPtr _cb, 
										int _worldID, 
										const string& _account, 
										int _cPrice, 
										const string& _receipt)
{
	PrxCallbackHelper<ServerEngine::LJSDKPrxCallback>* pReplaceCallback = new PrxCallbackHelper<ServerEngine::LJSDKPrxCallback>(g_pMainServant, _cb);
	ServerEngine::LJSDKPrxCallbackPtr replaceHandle = pReplaceCallback;	// set to auto ptr
	this->m_LJSDKPrx->async_iosPay(	replaceHandle,
									_worldID,
									_account,
									_cPrice,
									_receipt);
}





