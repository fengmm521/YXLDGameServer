#include "CoreImpPch.h"
#include "IJZMessageLayer.h"
#include "LoginManager.h"
#include "IActorSaveSystem.h"
#include "LogHelp.h"
#include "IVIPFactory.h"
#include "IFightFactory.h"
#include "ILegionFactory.h"
#include "IShopSystem.h"
#include "IMailSystem.h"


extern "C" IComponent* createLoginManager(Int32 iVersion)
{
	return new LoginManager;
}

LoginManager::LoginManager():m_bGetedFirstBattle(false)
{
}

LoginManager::~LoginManager()
{
	ITimerComponent* pTimeAxis = getComponent<ITimerComponent>(COMPNAME_TimeAxis, IID_ITimerComponent);
	if(pTimeAxis)
	{
		pTimeAxis->killTimer(m_hTimerHandle);
		pTimeAxis->killTimer(m_initHandler);
	}
}

bool LoginManager::initlize(const PropertySet& propSet)
{
	ITimerComponent* pTimeAxis = getComponent<ITimerComponent>(COMPNAME_TimeAxis, IID_ITimerComponent);
	assert(pTimeAxis);

	// 每分钟检测一次(5分钟无应答，直接踢掉)
	m_hTimerHandle = pTimeAxis->setTimer(this, 1, 60000, "KickTimer");
	m_initHandler = pTimeAxis->setTimer(this, 2, 1, "initTimer");

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>("GlobalCfg", IID_IGlobalCfg);
	assert(pGlobalCfg);

	string strSnapshotServer = pGlobalCfg->getGlobalCfg().get("/OuterObj<SnapshotServer>", "");	
	Application::getCommunicator()->stringToProxy(strSnapshotServer, m_snapshotPrx);

	m_bIsActive = false; //(pGlobalCfg->getGlobalCfg().get("/Zone<bIsActive>", "0")=="1");
	return true;
}

void* LoginManager::queryInterface(IID interfaceId)
{
	if(interfaceId == IID_IMessageListener)
	{
		return static_cast<IMessageListener*>(this);
	}
	else if(interfaceId == IID_IComponent)
	{
		return static_cast<IComponent*>(this);
	}
	else if(interfaceId == IID_IUserStateManager)
	{
		return static_cast<IUserStateManager*>(this);
	}

	return NULL;
}


void LoginManager::onMessage(Uint32 dwMsgID, const char* pMsgBuff, Int32 nBuffLen)
{
	assert(nBuffLen == sizeof(QxMessage) );
	QxMessage* pTmpQxMsg = (QxMessage*)pMsgBuff;

	assert(pTmpQxMsg->dwMsgLen == sizeof(GSProto::CSMessage) );
	const GSProto::CSMessage& msg = *(const GSProto::CSMessage*)(pTmpQxMsg->pMsgDataBuff);

	switch(dwMsgID)
	{
		case GSProto::CMD_ALOGIN:
			onReqALoginMsg(pTmpQxMsg, msg);
			break;

		case GSProto::CMD_GETROLE:
			onReqGetRole(pTmpQxMsg, msg);
			break;

		case GSProto::CMD_NEW_ROLE:
			onReqNewRole(pTmpQxMsg, msg);
			break;

		case GSProto::CMD_CHECK_ACTORNAME:
			OnReqcheckNameCanUse(pTmpQxMsg, msg);
			break;

		case GSProto::CMD_QUERY_FIRSTFIGHT:
			onReqQueryFirstFight(pTmpQxMsg, msg);
			break;

		case GSProto::CMD_REQUEST_REFRESHGOLD:
			onReqRefreshGold(pTmpQxMsg, msg);
			break;
	}
}


void LoginManager::sendBattleInfo(const UserInfo* pUserInfo)
{
	GSProto::FightDataAll fightDataAll;
	if(!fightDataAll.ParseFromString(m_firstBattleData.strBattleBody) )
	{
		return;
	}

	sendBattleMemMsg(pUserInfo, fightDataAll);
	sendBattleAction(pUserInfo, fightDataAll);	
	sendBattleFin(pUserInfo);
}


void LoginManager::sendBattleMemMsg(const UserInfo* pUserInfo, const GSProto::FightDataAll& allData)
{
	GSProto::Cmd_Sc_FightMember scMember;
	
	*scMember.mutable_szfightobject() = allData.szfightobject();
	scMember.set_ifighttype(allData.ifighttype() );
	scMember.set_isceneid(allData.isceneid() );
	scMember.set_iisattackerwin(allData.iisattackerwin() );
	scMember.set_iitemcount(allData.iinititemcount() );
	scMember.set_bcanuseactorskill(true);


	GSProto::SCMessage pkg;
	HelpMakeScMsg(pkg, GSProto::CMD_FIGHT_MEMBER, scMember);
	

	IMessageLayer* pMsgLayer = getComponent<IMessageLayer>(COMPNAME_MessageLayer, IID_IMessageLayer);
	assert(pMsgLayer);

	pMsgLayer->sendPlayerMessage(pUserInfo, pkg);
}

void LoginManager::sendBattleAction(const UserInfo* pUserInfo, const GSProto::FightDataAll& allData)
{
	IMessageLayer* pMsgLayer = getComponent<IMessageLayer>(COMPNAME_MessageLayer, IID_IMessageLayer);
	assert(pMsgLayer);

	for(int i = 0; i < allData.szfightaction().size(); i++)
	{
		GSProto::Cmd_Sc_FightAction scAction;
		*scAction.mutable_singleaction() = allData.szfightaction().Get(i);

		GSProto::SCMessage pkg;
		HelpMakeScMsg(pkg, GSProto::CMD_FIGHT_ACTION, scAction);
		
		pMsgLayer->sendPlayerMessage(pUserInfo, pkg);
	}
}

void LoginManager::sendBattleFin(const UserInfo* pUserInfo)
{
	GSProto::SCMessage pkg;
	pkg.set_icmd(GSProto::CMD_FIGHT_FIN);

	IMessageLayer* pMsgLayer = getComponent<IMessageLayer>(COMPNAME_MessageLayer, IID_IMessageLayer);
	assert(pMsgLayer);

	pMsgLayer->sendPlayerMessage(pUserInfo, pkg);
}


void LoginManager::onReqQueryFirstFight(QxMessage* pQxMsg, const GSProto::CSMessage& msg)
{
	const UserInfo* pUserInfo = getUserInfo(pQxMsg->strAccount);
	if(!pUserInfo) return;

	if( (pQxMsg->strRsAddress != pUserInfo->strRsAddress) || (pQxMsg->ddCon != pUserInfo->ddConnectID) || (pUserInfo->nState == en_UserState_Running) )
	{
		return;
	}

	sendBattleInfo(pUserInfo);
}

void LoginManager::generateFirstFight()
{
	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);
	
	// 创建机器人
	string strHeroList = pGlobalCfg->getString("首场战斗机器人", "1006#0#1#0;1014#1#1#0;1031#2#1#0");

	vector<CreateHeroDesc> createHeroList;
	vector<string> heroStringList = TC_Common::sepstr<string>(strHeroList, ";");
	for(size_t i = 0; i < heroStringList.size(); i++)
	{
		vector<int> heroPropList = TC_Common::sepstr<int>(heroStringList[i], "#");
		assert(heroPropList.size() == 5);

		CreateHeroDesc tmpDesc;
		tmpDesc.iHeroID = heroPropList[0];
		tmpDesc.iFormationPos = heroPropList[1];
		tmpDesc.iLevel= heroPropList[2];
		tmpDesc.iLevelStep= heroPropList[3];
		tmpDesc.iQuality = heroPropList[4];
		createHeroList.push_back(tmpDesc);
	}
	
	// 怪物组
	string strMonsterGrpList = pGlobalCfg->getString("首场战斗怪物", "1#2#3");
	vector<int> monsterGrpList = TC_Common::sepstr<int>(strMonsterGrpList, "#");

	IJZEntityFactory* pJZEntityFactory = getComponent<IJZEntityFactory>(COMPNAME_EntityFactory, IID_IJZEntityFactory);
	assert(pJZEntityFactory);

	IEntity* pMachine = pJZEntityFactory->createMachine("", "", createHeroList);
	assert(pMachine);

	IFightFactory* pFightFactory = getComponent<IFightFactory>(COMPNAME_FightFactory, IID_IFightFactory);
	assert(pFightFactory);
		
	ServerEngine::ActorCreateContext roleInfo;
	pMachine->packSaveData(roleInfo.data);

	ServerEngine::FightContext ctx;
	ctx.iFightType = GSProto::en_FightType_FirstFight;
	ctx.iSceneID = pGlobalCfg->getInt("首场战斗场景", 1);
	int iRet = pFightFactory->doMultiPVEFight(m_firstBattleData, roleInfo, monsterGrpList, ctx);

	// 吃药吧,按照肖总的配置
	ctx.randValueList = m_firstBattleData.randValueList;
	
	ServerEngine::FightUseItem useItemCtx;
	useItemCtx.iActionIndex = pGlobalCfg->getInt("首场战斗吃药Action索引", 5);
	useItemCtx.iResultsetIndex = 0;
	useItemCtx.iPost = 0;
	useItemCtx.iActorSkillID = 1;
	//useItemCtx.iTargetPos = pGlobalCfg->getInt("首场战斗吃药位置", 1);
	
	ctx.useItemActionList.push_back(useItemCtx);

	// 再次战斗吧
	m_firstBattleData = ServerEngine::BattleData();
	iRet = pFightFactory->continueMultiPVEFight(m_firstBattleData, roleInfo, monsterGrpList, ctx);

	assert(iRet == en_FightResult_OK);

	// 从保留机器人里面删除
	removeReserveGhost(pMachine->getHandle() );
	delete pMachine;

	// 处理下，增加首场战斗对话
	GSProto::FightDataAll fightDataAll;
	if(!fightDataAll.ParseFromString(m_firstBattleData.strBattleBody) )
	{
		assert(false);
		return;
	}

	fixFirstFightDlg(fightDataAll);

	// 写回去
	m_firstBattleData.strBattleBody.clear();
	fightDataAll.SerializeToString(&m_firstBattleData.strBattleBody);
	
	FDLOG("FirstFight")<<fightDataAll.DebugString()<<endl;
	
	m_bGetedFirstBattle = true;
}


void LoginManager::fixFirstFightDlg(GSProto::FightDataAll& fightDataAll)
{
	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	string strBeginDlg = pGlobalCfg->getString("首场战斗开始对话ID", "1#2#3");
	vector<int> beginDlgList = TC_Common::sepstr<int>(strBeginDlg, "#");
	
	string strEndDlg = pGlobalCfg->getString("首场战斗结束对话ID", "2#3#4");
	vector<int> endDlgList = TC_Common::sepstr<int>(strEndDlg, "#");
	
	string strDlgList = pGlobalCfg->getString("首场战斗换人战斗ID列表", "3;4;5#5;6;7#7;8;9");

	vector<string> dlgStrIDList = TC_Common::sepstr<string>(strDlgList, "#");

	GSProto::FightDataAll newFightData = fightDataAll;
	newFightData.mutable_szfightaction()->Clear();
	
	if(beginDlgList.size() > 0)
	{
		for(size_t i = 0; i < beginDlgList.size(); i++)
		{
			appendDlgAction(newFightData, beginDlgList[i]);
		}
	}
	
	for(int i = 0; i < fightDataAll.szfightaction_size(); i++)
	{
		const GSProto::FightAction& tmpAction = fightDataAll.szfightaction(i);
		if( (tmpAction.iactiontype() == GSProto::en_FightAction_ReplaceMember) && (dlgStrIDList.size() > 0) )
		{
			// 添加原Action(对话放到之后)
			GSProto::FightAction* pNewAction = newFightData.mutable_szfightaction()->Add();
			*pNewAction = tmpAction;
			pNewAction->set_iactionindex(newFightData.szfightaction_size() - 1);
		
			string strTmpDlgInfo = dlgStrIDList.front();
			dlgStrIDList.erase(dlgStrIDList.begin() );

			vector<int> tmpDlgIDList = TC_Common::sepstr<int>(strTmpDlgInfo, ";");
			for(vector<int>::iterator it = tmpDlgIDList.begin(); it != tmpDlgIDList.end(); it++)
			{
				appendDlgAction(newFightData, *it);
			}
			continue;
		}
		
		// 吃药前引导
		if( (tmpAction.szresultset_size() > 0) && (tmpAction.szresultset(0).szresult_size() > 0))
		{
			int iResultSize = tmpAction.szresultset(0).szresult_size();
			for(int iIndex = 0; iIndex < iResultSize; iIndex++)
			{
				const ::GSProto::FightResult& tmpFightResult = tmpAction.szresultset(0).szresult(iIndex);
				if(tmpFightResult.iresulttype() == GSProto::en_FightResult_UseActorSkill)
				{
					GSProto::FightAction* pNewGuideAction = newFightData.mutable_szfightaction()->Add();
					pNewGuideAction->set_iactiontype(GSProto::en_FightAction_GuideUseItem);
					pNewGuideAction->set_iactionindex(newFightData.szfightaction_size() - 1);
					break;
				}	
			}
		}
		
		// 添加原Action
		GSProto::FightAction* pNewAction = newFightData.mutable_szfightaction()->Add();
		*pNewAction = tmpAction;
		pNewAction->set_iactionindex(newFightData.szfightaction_size() - 1);
	}

	if(endDlgList.size() > 0)
	{
		for(size_t i = 0; i < endDlgList.size(); i++)
		{
			appendDlgAction(newFightData, endDlgList[i]);
		}
	}

	fightDataAll = newFightData;
}


void LoginManager::appendDlgAction(GSProto::FightDataAll& fightDataAll, int iDlgID)
{
	GSProto::FightAction* pNewAction = fightDataAll.mutable_szfightaction()->Add();
	assert(pNewAction);

	pNewAction->set_iactiontype(GSProto::en_FightAction_Dlg);
	pNewAction->set_iactionindex(fightDataAll.szfightaction_size() - 1);

	GSProto::DialogAction* pDlgAction = pNewAction->mutable_dlgaction();
	pDlgAction->set_idlgid(iDlgID);
}


void CheckNameCallback::callback_getNameDesc(taf::Int32 ret,  const ServerEngine::NameDesc& descInfo)
{
	SvrRunLog("callback_getNameDesc ret|%s|%s|%d", m_strAccount.c_str(), m_strName.c_str(), ret);
	
	LoginManager* pLoginManager = static_cast<LoginManager*>(getComponent<IUserStateManager>(COMPNAME_LoginManager, IID_IUserStateManager) );
	assert(pLoginManager);

	pLoginManager->processCheckNameRet(m_strAccount, ret);
}

void CheckNameCallback::callback_getNameDesc_exception(taf::Int32 ret)
{
	SvrRunLog("callback_getNameDesc exception|%s|%s|%d", m_strAccount.c_str(), m_strName.c_str(), ret);

	LoginManager* pLoginManager = static_cast<LoginManager*>(getComponent<IUserStateManager>(COMPNAME_LoginManager, IID_IUserStateManager) );
	assert(pLoginManager);

	pLoginManager->processCheckNameException(m_strAccount, ret);
}

void LoginManager::processCheckNameRet(const string& strAccount, int iRet)
{
	const UserInfo* pUserInfo = getUserInfo(strAccount);
	if(!pUserInfo) return;

	if(en_UserState_NewRole != pUserInfo->nState)
	{
		return;
	}

	IMessageLayer* pMsgLayer = getComponent<IMessageLayer>(COMPNAME_MessageLayer, IID_IMessageLayer);
	assert(pMsgLayer);

	GSProto::SCMessage pkg;
	GSProto::Cmd_Sc_CheckActorName scInfo;
	if(iRet == ServerEngine::en_NameRet_NameNotExist)
	{
		scInfo.set_iisok(1);
	}
	else if(iRet == ServerEngine::en_NameRet_OK)
	{
		scInfo.set_iisok(0);
		pMsgLayer->sendPlayerErrorMessage(pUserInfo, ERROR_DUNPLICATENAME);
	}
	else
	{
		scInfo.set_iisok(0);
		pMsgLayer->sendPlayerErrorMessage(pUserInfo, ERROR_SYSERROR);
	}

	HelpMakeScMsg(pkg, GSProto::CMD_CHECK_ACTORNAME, scInfo);
	pMsgLayer->sendMessage2Connection(pUserInfo->strRsAddress, pUserInfo->ddConnectID, pkg);
}

void LoginManager::processCheckNameException(const string& strAccount, int iRet)
{
	const UserInfo* pUserInfo = getUserInfo(strAccount);
	if(!pUserInfo) return;

	if(en_UserState_NewRole != pUserInfo->nState)
	{
		return;
	}

	IMessageLayer* pMsgLayer = getComponent<IMessageLayer>(COMPNAME_MessageLayer, IID_IMessageLayer);
	assert(pMsgLayer);

	pMsgLayer->sendPlayerErrorMessage(pUserInfo, ERROR_SYSERROR);

	GSProto::SCMessage pkg;
	GSProto::Cmd_Sc_CheckActorName scInfo;

	scInfo.set_iisok(0);

	HelpMakeScMsg(pkg, GSProto::CMD_CHECK_ACTORNAME, scInfo);
	pMsgLayer->sendMessage2Connection(pUserInfo->strRsAddress, pUserInfo->ddConnectID, pkg);
}

void LoginManager::OnReqcheckNameCanUse(QxMessage* pQxMsg, const GSProto::CSMessage& msg)
{
	const UserInfo* pUserInfo = getUserInfo(pQxMsg->strAccount);
	if(!pUserInfo) return;

	if(pUserInfo->nState != en_UserState_NewRole)
	{
		return;
	}

	GSProto::Cmd_Cs_CheckActorName tmpCheckNameMsg;
	if(!tmpCheckNameMsg.ParseFromString(msg.strmsgbody() ) )
	{
		SvrErrLog("CheckNameCanUse Invalid MsgData|%s", pQxMsg->strAccount.c_str() );
		return;
	}

	if(!checkNameValid(pQxMsg->strAccount, tmpCheckNameMsg.stracorname() ) )
	{
		return;
	}
	
	IMessageLayer* pMsgLayer = getComponent<IMessageLayer>(COMPNAME_MessageLayer, IID_IMessageLayer);
	assert(pMsgLayer);

	ServerEngine::NamePrxCallbackPtr handle = new CheckNameCallback(pQxMsg->strAccount, tmpCheckNameMsg.stracorname() );
	pMsgLayer->AsyncGetNameDesc(tmpCheckNameMsg.stracorname(), handle);
}


bool LoginManager::checkClientVersion(const string& strRsAddress, Int64 ddConn, const GSProto::Cmd_Cs_Alogin& csReq)
{
	IMessageLayer* pMsgLayer = getComponent<IMessageLayer>(COMPNAME_MessageLayer, IID_IMessageLayer);
	assert(pMsgLayer);

	int iVersionType = csReq.iversiontype();
	Uint32 dwClientVer = makeClientVer(csReq.iversionmain(), csReq.iversionfeature(), csReq.iversionbuild() );

	ITable* pTable = getCompomentObjectManager()->findTable(TABLENAME_ClientVersion);
	assert(pTable);

	int iRecord = pTable->findRecord(iVersionType);
	if(iRecord < 0) return false;

	Uint32 dwMinCltVer = pTable->getInt(iRecord, "最低适合版本");
	Uint32 dwMaxCltVer = pTable->getInt(iRecord, "最高适合版本");
	string strDonwloadUrl = pTable->getString(iRecord, "下载链接");

	if( (dwClientVer < dwMinCltVer) || (dwClientVer > dwMaxCltVer) )
	{
		// 必须升级
		{
			GSProto::Cmd_Sc_Alogin scLogin;
			scLogin.set_iresult(GSProto::en_LoginResult_MustUpdata);
			scLogin.set_strmd5(""); // 暂无
			scLogin.set_strurl(strDonwloadUrl);

			GSProto::SCMessage pkg;
			HelpMakeScMsg(pkg, GSProto::CMD_ALOGIN, scLogin);
			pMsgLayer->sendMessage2Connection(strRsAddress, ddConn, pkg);
		}

		// 暂时无提示，搞个错误吗下发
		{
			IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
			assert(pGlobalCfg);

			string strErrorMsg = pGlobalCfg->getString("版本错误提示", "version error");
			GSProto::Cmd_Sc_Error scError;
			scError.set_strerrormsg(strErrorMsg);

			GSProto::SCMessage pkg;
			HelpMakeScMsg(pkg, GSProto::CMD_ERROR, scError);
			pMsgLayer->sendMessage2Connection(strRsAddress, ddConn, pkg);
		}
		
		return false;
	}

	// 暂时不处理建议升级
	
	return true;
}


void LoginManager::updateChannelInfo(UserInfo* pUserInfo, const GSProto::Cmd_Cs_Alogin csAlogin)
{
	pUserInfo->strChannelFlag = csAlogin.strchannelflag();
	
	if(pUserInfo->strChannelFlag =="LJ")
	{
		pUserInfo->strChannelId = csAlogin.strchannelid();
		pUserInfo->strUserId = csAlogin.struserid();
		pUserInfo->strToken = csAlogin.strtoken();
		pUserInfo->strProductCode = csAlogin.strproductcode();
		pUserInfo->strChannelLabel = csAlogin.strchannellabel();
	}
	else if(pUserInfo->strChannelFlag =="vivo")
	{
		pUserInfo->strChannelId = "vivo";
		pUserInfo->strUserId = csAlogin.struserid();
		pUserInfo->strToken = csAlogin.strtoken();
	}
	else
	{
		pUserInfo->strChannel = csAlogin.strchannel();
		pUserInfo->strAccessToken = csAlogin.straccesskey();
		pUserInfo->strPayToken = csAlogin.strpaytoken();
		pUserInfo->strPf = csAlogin.strpf();
		pUserInfo->strPfKey = csAlogin.strpfkey();
	}
	
	
}


void LoginManager::onReqALoginMsg(QxMessage* pQxMsg, const GSProto::CSMessage& msg)
{
	IMessageLayer* pMsgLayer = getComponent<IMessageLayer>(COMPNAME_MessageLayer, IID_IMessageLayer);
	assert(pMsgLayer);

	GSProto::Cmd_Cs_Alogin csAlogin;
	if(!csAlogin.ParseFromString(msg.strmsgbody() ) )
	{
		SvrErrLog("invalid alogin Msg|%s", pQxMsg->strAccount.c_str() );
		return;
	}

	// 保留帐号检查
	if(m_reserveGhostMap.find(pQxMsg->strAccount) != m_reserveGhostMap.end() )
	{
		SvrErrLog("Alogin Fail Reserve|%s", pQxMsg->strAccount.c_str() );
		return;
	}
	
	// todo 版本检查
	/*if(!checkClientVersion(pQxMsg->strRsAddress, pQxMsg->ddCon, csAlogin) )
	{
		int iTmpClientType = csAlogin.iversiontype();
		Uint32 dwTmpClientVersion = makeClientVer(csAlogin.iversionmain(), csAlogin.iversionfeature(), csAlogin.iversionbuild() );
		FDLOG("ClientVersion")<<"ClientVersion|"<<iTmpClientType<<"|"<<dwTmpClientVersion<<endl;
		return;
	}*/

	// 先通知原客户端被踢
	UserInfo* pUserInfo = (UserInfo*)getUserInfo(pQxMsg->strAccount);
	if(pUserInfo)
	{
		pMsgLayer->sendPlayerErrorMessage(pUserInfo, ERROR_ACCOUNT_OTHERLOGIN);
		
		GSProto::SCMessage pkg;
		GSProto::Cmd_Sc_Kickoff tmpKickOff;
		tmpKickOff.set_ireason(GSProto::en_KickOff_DuplicateAccount);
		HelpMakeScMsg(pkg, GSProto::CMD_KICKOFF_ACCOUNT, tmpKickOff);
		pMsgLayer->sendMessage2Connection(pUserInfo->strRsAddress, pUserInfo->ddConnectID, pkg);
	}
	
	if(pUserInfo && (pUserInfo->nState == en_UserState_Running) )
	{
		pUserInfo->strRsAddress = pQxMsg->strRsAddress;
		pUserInfo->ddConnectID = pQxMsg->ddCon;

		updateChannelInfo(pUserInfo, csAlogin);

		IEntity* pActor = getEntityFromHandle(pUserInfo->hEntity);
		assert(pActor);

		pActor->setProperty(PROP_ACTOR_RS, pQxMsg->strRsAddress.c_str() );
		pActor->setInt64Property(PROP_ACTOR_CON, pQxMsg->ddCon);
		
		sendLoginResult(pQxMsg->strRsAddress, pQxMsg->ddCon, GSProto::en_LoginResult_RoleOnLine);	
		return;
	}

	pUserInfo = &(m_userStateMap[pQxMsg->strAccount]);
	*pUserInfo = UserInfo();

	pUserInfo->nState = en_UserState_GetRole;
	pUserInfo->dwLoginTime = time(0);
	pUserInfo->strRsAddress = pQxMsg->strRsAddress;
	pUserInfo->ddConnectID = pQxMsg->ddCon;
	pUserInfo->dwLastPingTime = time(0);
	pUserInfo->dwCltType = csAlogin.iversiontype();
	pUserInfo->dwCltVer = makeClientVer(csAlogin.iversionmain(), csAlogin.iversionfeature(), csAlogin.iversionbuild() );
	updateChannelInfo(pUserInfo, csAlogin);

	sendLoginResult(pQxMsg->strRsAddress, pQxMsg->ddCon, GSProto::en_LoginResult_OK);
}

Uint32 LoginManager::makeClientVer(int iVerMain, int iVerFeature, int iVerBuild)
{
	Uint32 dwVer = (Uint32)((iVerMain<<16)|(iVerFeature<<8)|iVerBuild);

	return dwVer;
}


void LoginManager::sendLoginResult(const string& strRsAddress, Int64 ddConn, int iResult)
{
	GSProto::SCMessage pkg;
	GSProto::Cmd_Sc_Alogin scLoginMsg;
	scLoginMsg.set_iresult(GSProto::en_LoginResult_RoleOnLine);
	scLoginMsg.set_strmd5("");
	HelpMakeScMsg(pkg, GSProto::CMD_ALOGIN, scLoginMsg);

	IMessageLayer* pMsgLayer = getComponent<IMessageLayer>(COMPNAME_MessageLayer, IID_IMessageLayer);
	assert(pMsgLayer);
	pMsgLayer->sendMessage2Connection(strRsAddress, ddConn, pkg);
}


void GetRoleCallback::callback_getRole(taf::Int32 ret,  const ServerEngine::PIRole& roleInfo)
{
	SvrRunLog("GetRole ret|%s|%d", m_strAccount.c_str(), ret);

	LoginManager* pLoginManager = static_cast<LoginManager*>(getComponent<IUserStateManager>(COMPNAME_LoginManager, IID_IUserStateManager) );
	assert(pLoginManager);
	if(ret == ServerEngine::en_RoleRet_OK)
	{
		pLoginManager->processGetRole(roleInfo, m_verifyKey);
		return;
	}
	else if(ret == ServerEngine::en_RoleRet_NotExist)
	{
		pLoginManager->processNoRole(m_strAccount);
	}
	else
	{
		sendErrorCode(ERROR_GETROLE_ERROR);
		pLoginManager->processGetRoleError(m_strAccount);
	}
}

void GetRoleCallback::callback_getRole_exception(taf::Int32 ret)
{
	SvrErrLog("GetRole Exception|%s|%d", m_strAccount.c_str(), ret);

	// 发送错误码
	sendErrorCode(ERROR_GETROLE_ERROR);

	LoginManager* pLoginManager = static_cast<LoginManager*>(getComponent<IUserStateManager>(COMPNAME_LoginManager, IID_IUserStateManager) );
	assert(pLoginManager);

	pLoginManager->processGetRoleError(m_strAccount);
}

void GetRoleCallback::sendErrorCode(int iErrorCode)
{
	LoginManager* pLoginManager = static_cast<LoginManager*>(getComponent<IUserStateManager>(COMPNAME_LoginManager, IID_IUserStateManager) );
	assert(pLoginManager);

	const UserInfo* pUserInfo = pLoginManager->getUserInfo(m_strAccount);
	if(!pUserInfo) return;

	IMessageLayer* pMsgLayer = getComponent<IMessageLayer>(COMPNAME_MessageLayer, IID_IMessageLayer);
	assert(pMsgLayer);

	pMsgLayer->sendErrorMessage2Connection(pUserInfo->strRsAddress, pUserInfo->ddConnectID, iErrorCode);
}


void NewRoleCallback::sendErrorCode(int iErrorCode)
{
	LoginManager* pLoginManager = static_cast<LoginManager*>(getComponent<IUserStateManager>(COMPNAME_LoginManager, IID_IUserStateManager) );
	assert(pLoginManager);

	const UserInfo* pUserInfo = pLoginManager->getUserInfo(m_strAccount);
	if(!pUserInfo) return;

	IMessageLayer* pMsgLayer = getComponent<IMessageLayer>(COMPNAME_MessageLayer, IID_IMessageLayer);
	assert(pMsgLayer);

	pMsgLayer->sendErrorMessage2Connection(pUserInfo->strRsAddress, pUserInfo->ddConnectID, iErrorCode);
}


void NewRoleCallback::callback_newRole(taf::Int32 ret)
{
	LoginManager* pLoginManager = static_cast<LoginManager*>(getComponent<IUserStateManager>(COMPNAME_LoginManager, IID_IUserStateManager) );
	assert(pLoginManager);

	SvrRunLog("new Role ret|%s|%s|%d", m_strAccount.c_str(), m_strRoleName.c_str(), ret);
	if(ret == ServerEngine::en_RoleRet_OK)
	{
		pLoginManager->processNewRoleResult(m_strAccount, GSProto::en_NewRole_OK);
		STAT_NEWROLE<<m_strRoleName<<"|"<<m_strAccount<<"|"<<m_iWorldID<<"|"<<m_strChannelLabel<<endl;

		// 通知SnapshotServer增加World
		pLoginManager->addAccountSnapshot(m_strAccount, m_iWorldID);
	}
	else if(ret == ServerEngine::en_RoleRet_NameDunplicated)
	{
		sendErrorCode(ERROR_DUNPLICATENAME);
		pLoginManager->processNewRoleResult(m_strAccount, GSProto::en_NewRole_DunplicateName);
	}
	else
	{
		sendErrorCode(ERROR_NEWROLE_SYSERROR);
		pLoginManager->processNewRoleResult(m_strAccount, GSProto::en_NewRole_Fail);
	}
}

void NewRoleCallback::callback_newRole_exception(taf::Int32 ret)
{
	SvrErrLog("new Role Exception|%s|%d", m_strAccount.c_str(), ret);
	sendErrorCode(ERROR_NEWROLE_SYSERROR);
}


class SnapshotAddCb:public ServerEngine::SnapshotPrxCallback
{
public:

	SnapshotAddCb(const string& strAccount, int iWorldID):m_strAccount(strAccount), m_iWorldID(iWorldID){}

	virtual void callback_addWorld(taf::Int32 ret)
	{
		// 在异步线程，只记录日志，不能做其他的事情
		SvrRunLog("addSnapshotRet|%s|%d|%d\n", m_strAccount.c_str(), m_iWorldID, ret);
	}
	
    virtual void callback_addWorld_exception(taf::Int32 ret)
    {
    	// 在异步线程，只记录日志，不能做其他的事情
    	SvrRunLog("addSnapshotException|%s|%d|%d\n", m_strAccount.c_str(), m_iWorldID, ret);
    }

private:

	string m_strAccount;
	int m_iWorldID;
};

class CheckAccountPayReturnCallback:public ServerEngine::PayReturnPrxCallback
{
public:

	CheckAccountPayReturnCallback(const string& strAccount, const string& strName):m_strAccount(strAccount), m_strName(strName){}
	void callback_payReturnMoneyAddVIP(taf::Int32 ret, taf::Int32 state, taf::Int32 iMoney, taf::Int32 VipLevel)
	{
		if(ret == ServerEngine::en_PayReturnState_OK)
		{
			int iVIPLevel = VipLevel;
			IUserStateManager* pUserStateMgr = getComponent<IUserStateManager>(COMPNAME_LoginManager, IID_IUserStateManager);
			assert(pUserStateMgr);

			HEntity hTarget = pUserStateMgr->getRichEntityByAccount(m_strAccount);

			IEntity *pEntity = getEntityFromHandle(hTarget);
			if(!pEntity)
			{
				FDLOG("CheckAccountPayReturnCallback")<<m_strAccount<<"|"<<m_strName<<"|"<<iMoney<<"|"<<iVIPLevel<<endl;
				return;
			}

			//发送返还邮件
			//
			
			IMailSystem *pMailSys = static_cast<IMailSystem*>(pEntity->querySubsystem(IID_IMailSystem));
			assert(pMailSys);
			if(iMoney != 0)
			{
				pMailSys->sendPayReturnMail(iMoney);
			}
			
			IVIPFactory *pVipFact = getComponent<IVIPFactory>(COMPNAME_VIPFactory, IID_IVIPFactory);
			assert(pVipFact);

			if(iVIPLevel == 0) return;

			int iVIPLEVELEXP = pVipFact->getVipPropByVIPLevel(iVIPLevel-1,VIP_PROP_VIPEXP);
			pEntity->addVipExp(iVIPLEVELEXP);

			if( iVIPLevel != 15)
			{
				//邮件通知额外获得的元宝
				pMailSys->sendLifeLongVIP(iVIPLevel);
			}
			
		}
		
	}
	void callback_payReturnMoneyAddVIP_exception(taf::Int32 ret)
	{
		SvrErrLog("callback_payReturnMoneyAddVIP_exception|%s|%d", m_strAccount.c_str(), ret);
	}
private:

	string m_strAccount;
	string m_strName;
};






void LoginManager::addAccountSnapshot(const std::string& strAccount, int iWorldID)
{
	// 这里无需强制同步到同一个线程
	m_snapshotPrx->async_addWorld(new SnapshotAddCb(strAccount, iWorldID), strAccount, iWorldID);
}


void LoginManager::processNewRoleResult(const string& strAccount,int iResult)
{
	UserInfo* pUserInfo = (UserInfo*)getUserInfo(strAccount);
	if(!pUserInfo) return;

	IMessageLayer* pMsgLayer = getComponent<IMessageLayer>(COMPNAME_MessageLayer, IID_IMessageLayer);
	assert(pMsgLayer);

	GSProto::SCMessage pkg;
	GSProto::Cmd_Sc_NewRole newRoleMsg;
	newRoleMsg.set_iresult(iResult);

	HelpMakeScMsg(pkg, GSProto::CMD_NEW_ROLE, newRoleMsg);
	pMsgLayer->sendMessage2Connection(pUserInfo->strRsAddress, pUserInfo->ddConnectID, pkg);

	

	if(iResult == GSProto::en_NewRole_OK)
	{
		pUserInfo->nState = en_UserState_GetRole;
	}

	//拉取玩家充值返还
	
}


void LoginManager::processNoRole(const string& strAccount)
{
	UserInfo* pUserInfo = (UserInfo*)getUserInfo(strAccount);
	if(!pUserInfo) return;

	pUserInfo->nState = en_UserState_NewRole;

	GSProto::SCMessage pkg;
	GSProto::Cmd_Sc_GetRole tmpGetRole;
	tmpGetRole.set_iresult(GSProto::en_GetRole_NoRole);

	HelpMakeScMsg(pkg, GSProto::CMD_GETROLE, tmpGetRole);

	IMessageLayer* pMsgLayer = getComponent<IMessageLayer>(COMPNAME_MessageLayer, IID_IMessageLayer);
	assert(pMsgLayer);

	pMsgLayer->sendMessage2Connection(pUserInfo->strRsAddress, pUserInfo->ddConnectID, pkg);
}

void LoginManager::processGetRoleError(const string& strAccount)
{
	UserInfo* pUserInfo = (UserInfo*)getUserInfo(strAccount);
	if(!pUserInfo) return;

	pUserInfo->nState = en_UserState_GetRole;

	GSProto::SCMessage pkg;
	GSProto::Cmd_Sc_GetRole tmpGetRole;
	tmpGetRole.set_iresult(GSProto::en_GetRole_Error);

	HelpMakeScMsg(pkg, GSProto::CMD_GETROLE, tmpGetRole);

	IMessageLayer* pMsgLayer = getComponent<IMessageLayer>(COMPNAME_MessageLayer, IID_IMessageLayer);
	assert(pMsgLayer);

	pMsgLayer->sendMessage2Connection(pUserInfo->strRsAddress, pUserInfo->ddConnectID, pkg);
}

class PayQQYYBYuanBaoCb:public ServerEngine::QQYYBPayPrxCallback
{
public:

	PayQQYYBYuanBaoCb(const string& strAccount, int iWorldID, const ServerEngine::QQYYBPayQueryKey& key, int iCostNum, int iSendNum, bool bFirstSave, int iSumCoin):m_strAccount(strAccount),
		m_iWorldID(iWorldID), m_queryKey(key), m_iCostNum(iCostNum), m_iSendNum(iSendNum), m_bFirstSave(bFirstSave), m_iSumNum(iSumCoin){}

	virtual void callback_payYuanBao(taf::Int32 ret)
	{
		FDLOG("Pay")<<"PayRet|"<<m_strAccount<<"|"<<m_iWorldID<<"|"<<ret<<"|"<<m_iCostNum<<endl;
	
		if(ret == ServerEngine::en_QQYYBPayRet_OK)
		{
			IUserStateManager* pUserStateManager = getComponent<IUserStateManager>(COMPNAME_LoginManager, IID_IUserStateManager);
			assert(pUserStateManager);

			const UserInfo* pUserInfo = pUserStateManager->getUserInfo(m_strAccount);
			if(!pUserInfo)
			{
				FDLOG("Pay")<<"PayAccountNotOnline|"<<m_strAccount<<"|"<<m_iWorldID<<"|"<<ret<<"|"<<m_iCostNum<<endl;
				return;
			}

			IEntity* pEntity = getEntityFromHandle(pUserInfo->hEntity);
			if(!pEntity)
			{
				FDLOG("Pay")<<"PayEntityNotOnline|"<<m_strAccount<<"|"<<m_iWorldID<<"|"<<ret<<"|"<<m_iCostNum<<endl;
				return;
			}

			int iWorldID = pEntity->getProperty(PROP_ACTOR_WORLD, 0);
			if(iWorldID != m_iWorldID)
			{
				FDLOG("Pay")<<"PayEntityWorldInvalid|"<<m_strAccount<<"|"<<m_iWorldID<<"|"<<ret<<"|"<<m_iCostNum<<endl;
				return;
			}

		//	pEntity->addVipExp(m_iCostNum);
		//	pEntity->changeProperty(PROP_ACTOR_GOLD, m_iCostNum, GSProto::en_Reason_Gold_Pay);

			IShopSystem *pShopSystem = static_cast<IShopSystem*>(pEntity->querySubsystem(IID_IShopSystem));
			assert(pShopSystem);

			bool res = pShopSystem->addGoldForPayment(m_iCostNum, "tecent");
			if(!res)
			{
				return ;
			}

			EventArgs args;
			args.context.setInt("entity", pUserInfo->hEntity);
			args.context.setInt("curCoin", m_iCostNum);
			args.context.setInt("bFirstsave", m_bFirstSave);
			args.context.setInt("sumMoney", m_iSumNum);
			pEntity->getEventServer()->setEvent(EVENT_ENTITY_SAVEMONEY, args);
		}
	}
	
   	virtual void callback_payYuanBao_exception(taf::Int32 ret)
   	{
   		FDLOG("Pay")<<"PayException|"<<ret<<endl;	
   	}

private:

	string m_strAccount;
	int m_iWorldID;
	ServerEngine::QQYYBPayQueryKey m_queryKey;
	int m_iCostNum;
	int m_iSendNum;
	bool m_bFirstSave;
	int m_iSumNum;
};


class GetQQYYBYuanBaoCb:public ServerEngine::QQYYBPayPrxCallback
{
public:

	GetQQYYBYuanBaoCb(const string& strAccount, int iWorldID, const ServerEngine::QQYYBPayQueryKey& key):m_strAccount(strAccount),
		m_iWorldID(iWorldID), m_queryKey(key){}

	virtual void callback_queryYuanBao(taf::Int32 ret, taf::Int32 iCurCoin, taf::Int32 iSendCoin, taf::Bool bFirstSave, taf::Int32 iSumCoin)
	{
		FDLOG("Pay")<<"QueryRet|"<<m_strAccount<<"|"<<m_iWorldID<<"|"<<ret<<"|"<<iCurCoin<<"|"<<iSendCoin<<"|"<<(int)bFirstSave<<"|"<<iSumCoin<<endl;
		if(ret == ServerEngine::en_QQYYBPayRet_OK)
		{
			// 没钱了，不去调用扣款接口了
			if(0 == iCurCoin) return;
		
			// 判断玩家是否在线
			IUserStateManager* pUserStateManager = getComponent<IUserStateManager>(COMPNAME_LoginManager, IID_IUserStateManager);
			assert(pUserStateManager);


			const UserInfo* pUserInfo = pUserStateManager->getUserInfo(m_strAccount);
			if(!pUserInfo) return;

			IEntity* pEntity = getEntityFromHandle(pUserInfo->hEntity);
			if(!pEntity) return;

			int iWorldID = pEntity->getProperty(PROP_ACTOR_WORLD, 0);
			if(iWorldID != m_iWorldID) return;
		
			IJZMessageLayer* pJZMessageLayer = getComponent<IJZMessageLayer>(COMPNAME_MessageLayer, IID_IJZMessageLayer);
			assert(pJZMessageLayer);

			pJZMessageLayer->AsyncPayQQYYBYuanBao(new PayQQYYBYuanBaoCb(m_strAccount, m_iWorldID, m_queryKey, iCurCoin, iSendCoin, bFirstSave, iSumCoin), m_queryKey,  iCurCoin);
		}
	}
    
    virtual void callback_queryYuanBao_exception(taf::Int32 ret)
    {
		FDLOG("Pay")<<"QueryException|"<<ret<<endl;	
    }

private:

	string m_strAccount;
	int m_iWorldID;
	ServerEngine::QQYYBPayQueryKey m_queryKey;
};

class VerifyUserLoginCallback : public ServerEngine::LJSDKPrxCallback
{
	public:
		VerifyUserLoginCallback(const string strAccount):m_strAccount(strAccount){}
		virtual void callback_verifyUserLogin(taf::Int32 iState)
        { 
        	if(iState == ServerEngine::en_INVALIDLOGIN)
        	{
        		//认证失败踢掉玩家
        		FDLOG("VerifyUserLogin")<<"en_INVALIDLOGIN|"<<m_strAccount<<endl;	
				
				IUserStateManager* pUserStateManager = getComponent<IUserStateManager>(COMPNAME_LoginManager, IID_IUserStateManager);
				assert(pUserStateManager);
				
				const UserInfo* pUserInfo = pUserStateManager->getUserInfo(m_strAccount);
				if(!pUserInfo) return;

				pUserStateManager->kickUser(m_strAccount);
        	}
        }
        virtual void callback_verifyUserLogin_exception(taf::Int32 ret)
        { 
        	FDLOG("VerifyUserLogin")<<"callback_verifyUserLogin_exception|"<<ret<<endl;	
        }
	
	private:
		string m_strAccount;
};


void LoginManager::processGetRole(const ServerEngine::PIRole& roleInfo, ServerEngine::VerifyKey& verifyKey)
{
	UserInfo* pUserInfo = (UserInfo*)getUserInfo(roleInfo.strAccount);
	if(!pUserInfo) return;

	pUserInfo->nState = en_UserState_Running;

	// 创建玩家对象(同时下发GetRole下行)
	IJZEntityFactory* pEntityFactory = getComponent<IJZEntityFactory>(COMPNAME_EntityFactory, IID_IJZEntityFactory);
	assert(pEntityFactory);

	// 填充内容RS等
	ServerEngine::ActorCreateContext createContext;
	createContext.strAccount = roleInfo.strAccount;
	createContext.strRsAddress = pUserInfo->strRsAddress;
	createContext.ddCon = pUserInfo->ddConnectID;
	ServerEngine::JceToObj(roleInfo.roleData, createContext.data);

	string strCreateString = ServerEngine::JceToStr(createContext);

	IEntity* pEntity = pEntityFactory->getEntityFactory()->createEntity("Actor", strCreateString);
	assert(pEntity);

	pUserInfo->hEntity = pEntity->getHandle();
	pUserInfo->dwEnterTime = time(0);
	pUserInfo->dwLastPingTime = time(0);

	m_entityNameMap[pEntity->getProperty(PROP_ENTITY_NAME, "")] = pEntity->getHandle();

	string strActorName = pEntity->getProperty(PROP_ENTITY_NAME, "");
	int iSrcWorldID = pEntity->getProperty(PROP_ACTOR_WORLD, 0);
	STAT_LOG_LOGIN<<strActorName<<"|"<<roleInfo.strAccount<<"|"<<iSrcWorldID<<"|"<<verifyKey.strChannelLabel<<endl;

	// 统计最高在线
	if(getDayResetValue(m_maxOnline).iValue < (int)m_entityNameMap.size() )
	{
		getDayResetValue(m_maxOnline).iValue = (int)m_entityNameMap.size();
	}

	// 请求查询元宝数目
	IJZMessageLayer* pJZMessageLayer = getComponent<IJZMessageLayer>(COMPNAME_MessageLayer, IID_IJZMessageLayer);
	assert(pJZMessageLayer);

	if(pUserInfo->strChannelFlag == "QQ" )
	{

		ServerEngine::QQYYBPayQueryKey key;

		parseOpenIDChannel(roleInfo.strAccount, key.strOpenID, key.strChannel);
	
		//key.strOpenID = roleInfo.strAccount.substr(sPos+1);
		key.strOpenKey = pUserInfo->strAccessToken;
		key.strPayToken = pUserInfo->strPayToken;
		key.strPf = pUserInfo->strPf;
		key.strPfKey = pUserInfo->strPfKey;
		key.iZoneID = roleInfo.worldID+1;
		//key.strChannel = roleInfo.strAccount.substr(0, sPos);
		
		pJZMessageLayer->AsyncQueryQQYYBYuanBao(new GetQQYYBYuanBaoCb(roleInfo.strAccount, roleInfo.worldID, key) , key);
	}
	else if(pUserInfo->strChannelFlag == "LJ" )
	{
		//账号验证
		pJZMessageLayer->AsyncVrifyUserLogin(new VerifyUserLoginCallback(roleInfo.strAccount) , verifyKey);
	}
	else if(pUserInfo->strChannelFlag == "TBT" )
	{
	}
	else if(pUserInfo->strChannelFlag == "PC")
	{
	}
	else if(pUserInfo->strChannelFlag == "UC")
	{
	}
	else if(pUserInfo->strChannelFlag == "2144")
	{
		
	}
	else if(pUserInfo->strChannelFlag == "IOS51")
	{
	}
	else if (pUserInfo->strChannelFlag == "vivo")
	{
		//去vivo验证
		pJZMessageLayer->AsyncVrifyUserLogin(new VerifyUserLoginCallback(roleInfo.strAccount) , verifyKey);
	}
	else
	{
		IUserStateManager* pUserStateManager = getComponent<IUserStateManager>(COMPNAME_LoginManager, IID_IUserStateManager);
		assert(pUserStateManager);
				
		const UserInfo* pUserInfo = pUserStateManager->getUserInfo(roleInfo.strAccount);
		if(!pUserInfo) return;

		pUserStateManager->kickUser(roleInfo.strAccount);
	}

	//查询返还元宝
	pJZMessageLayer->AsyncPayReturnMoneyAndVip(new CheckAccountPayReturnCallback(roleInfo.strAccount, strActorName ),roleInfo.strAccount);
}


void LoginManager::onReqRefreshGold(QxMessage* pQxMsg, const GSProto::CSMessage& msg)
{
	// 请求查询元宝数目
	IJZMessageLayer* pJZMessageLayer = getComponent<IJZMessageLayer>(COMPNAME_MessageLayer, IID_IJZMessageLayer);
	assert(pJZMessageLayer);

	
	GSProto::CMD_REQUEST_REFRESHGOLD_CS reqBody;
	if(!reqBody.ParseFromString(msg.strmsgbody() ) )
	{
		return;
	}

	UserInfo* pUserInfo = (UserInfo*)getUserInfo(pQxMsg->strAccount);
	if(!pUserInfo) return;
	
	IEntity* pEntity = getEntityFromHandle(pUserInfo->hEntity);
	if(!pEntity) return;

	ServerEngine::QQYYBPayQueryKey key;

	parseOpenIDChannel(pQxMsg->strAccount, key.strOpenID, key.strChannel);
	
	//key.strOpenID = pQxMsg->strAccount.substr(sPos+1);
	key.strOpenKey = reqBody.straccesskey();
	key.strPayToken = reqBody.strpaytoken();
	key.strPf = reqBody.strpf();
	key.strPfKey = reqBody.strpfkey();
	key.iZoneID = pEntity->getProperty(PROP_ACTOR_WORLD, 0) + 1;
	//key.strChannel = pQxMsg->strAccount.substr(0, sPos);
	
	pJZMessageLayer->AsyncQueryQQYYBYuanBao(new GetQQYYBYuanBaoCb(pQxMsg->strAccount, key.iZoneID-1, key) , key);
}


bool LoginManager::parseOpenIDChannel(const string& strAccount, string& strOpenID, string& strChannel)
{
	string strTmpAccount = strAccount;
	size_t sPos = strTmpAccount.find("_");
	if(sPos == string::npos)
	{
		return false;
	}

	strChannel = strTmpAccount.substr(0, sPos);
	strTmpAccount = strTmpAccount.substr(sPos+1);

	sPos = strTmpAccount.find("_");
	if(sPos == string::npos)
	{
		return false;
	}

	strChannel += string("_") + strTmpAccount.substr(0, sPos);
	strOpenID = strTmpAccount.substr(sPos+1);

	return true;
}


void LoginManager::processOnlineRoleGet(const UserInfo* pUserInfo)
{
	IEntity* pEntity = getEntityFromHandle(pUserInfo->hEntity);
	assert(pEntity);

	EventArgs args;
	args.context.setInt("entity", pUserInfo->hEntity);
	pEntity->getEventServer()->setEvent(EVENT_ENTITY_RELOGIN, args);
}

void LoginManager::onReqGetRole(QxMessage* pQxMsg, const GSProto::CSMessage& msg)
{
	const UserInfo* pUserInfo = getUserInfo(pQxMsg->strAccount);
	if(!pUserInfo) return;

	if(en_UserState_Running == pUserInfo->nState)
	{
		processOnlineRoleGet(pUserInfo);
		return;
	}

	if(en_UserState_GetRole != pUserInfo->nState)
	{
		return;
	}

	IMessageLayer* pMsgLayer = getComponent<IMessageLayer>(COMPNAME_MessageLayer, IID_IMessageLayer);
	assert(pMsgLayer);

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	ServerEngine::PKRole roleKey;
	roleKey.strAccount = pQxMsg->strAccount;
	roleKey.rolePos = 0;
	roleKey.worldID = pQxMsg->iWorldID;


	//GSProto::CMD_GETROLE_CS csMsg;
	//if(!csMsg.ParseFromString(msg.strmsgbody() ) )
	//{
	//	return;
	//}

	ServerEngine::VerifyKey verifyKey;
	verifyKey.strUserId = pUserInfo->strUserId;
	verifyKey.strChannelId = pUserInfo->strChannelId;
	verifyKey.strToken = pUserInfo->strToken;
	verifyKey.strProductCode = pUserInfo->strProductCode;
	verifyKey.strChannelLabel = pUserInfo->strChannelLabel;
	try
	{
		ServerEngine::RolePrxCallbackPtr handle = new GetRoleCallback(pQxMsg->strAccount,verifyKey);
		pMsgLayer->AsyncGetRole(roleKey, handle);
	}
	catch(...)
	{
		return;
	}

	((UserInfo*)pUserInfo)->nState = en_UserState_GetingRole;
}

bool LoginManager::checkNameValid(const std::string& strAccount, const string& strRoleName)
{
	const UserInfo* pUserInfo = getUserInfo(strAccount);
	if(!pUserInfo) return false;

	IMessageLayer* pMsgLayer = getComponent<IMessageLayer>(COMPNAME_MessageLayer, IID_IMessageLayer);
	assert(pMsgLayer);

	if(strRoleName.size() == 0)
	{
		pMsgLayer->sendPlayerErrorMessage(pUserInfo, ERROR_ROLENAME_EMPTY);
		return false;
	}

	IReserveWordCheck* pReserveWordCheck = getComponent<IReserveWordCheck>("ReserveWordCheck", IID_IReserveWordCheck);
	assert(pReserveWordCheck);

	int iWordCount = pReserveWordCheck->getWordLength(strRoleName);
	if(iWordCount < 0)
	{
		pMsgLayer->sendPlayerErrorMessage(pUserInfo, ERROR_ROLENAME_INVALID);
		return false;
	}
	
	if(iWordCount > GSProto::MAX_ROLENAME_LEN)
	{
		pMsgLayer->sendPlayerErrorMessage(pUserInfo, ERROR_ROLENAME_TOOLONG);
		return false;
	}

	// todo 敏感字检测
	if(pReserveWordCheck->hasReserveWord(strRoleName) )
	{
		pMsgLayer->sendPlayerErrorMessage(pUserInfo, ERROR_NAME_BANSTRING);
		return false;
	}

	return true;
}

void LoginManager::onReqNewRole(QxMessage* pQxMsg, const GSProto::CSMessage& msg)
{
	const UserInfo* pUserInfo = getUserInfo(pQxMsg->strAccount);
	if(!pUserInfo) return;

	if(en_UserState_NewRole != pUserInfo->nState)
	{
		return;
	}

	GSProto::Cmd_Cs_NewRole tmpNewRole;
	if(!tmpNewRole.ParseFromString(msg.strmsgbody() ) )
	{
		SvrErrLog("new Role Msg Error|%s", pQxMsg->strAccount.c_str() );
		return;
	}

	if(!checkNameValid(pQxMsg->strAccount, tmpNewRole.strrolename() ) )
	{
		return;
	}

	// 填充新角色数据
	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);
	
	ServerEngine::PIRole roleData;
	roleData.strAccount = pQxMsg->strAccount;
	roleData.rolePos = 0;
	roleData.worldID = pQxMsg->iWorldID;

	ServerEngine::RoleSaveData tmpSaveData;
	fillNewActorData(tmpSaveData, pQxMsg->strAccount, pQxMsg->iWorldID, tmpNewRole);

	roleData.roleData = ServerEngine::JceToStr(tmpSaveData);
	
	// 发送创建请求
	IMessageLayer* pMsgLayer = getComponent<IMessageLayer>(COMPNAME_MessageLayer, IID_IMessageLayer);
	assert(pMsgLayer);

	ServerEngine::RolePrxCallbackPtr handle = new NewRoleCallback(pQxMsg->strAccount, tmpNewRole.strrolename() , pQxMsg->iWorldID, pUserInfo->strChannelLabel);

	pMsgLayer->AsyncNewRole(pQxMsg->strAccount, pQxMsg->iWorldID, 0 , tmpNewRole.strrolename(), roleData, handle);
}

bool LoginManager::checkNewHeroValid(int iHeroID)
{
	ITable* pTable = getCompomentObjectManager()->findTable(TABLENAME_HeroCreate);
	assert(pTable);

	int iRecord = pTable->findRecord(iHeroID);

	return iRecord >= 0;
}

void LoginManager::fillNewActorData(ServerEngine::RoleSaveData& actorSaveData, const std::string& strAccount, int iWorldID, const GSProto::Cmd_Cs_NewRole& reqInfo)
{
	string strName = reqInfo.strrolename();

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	string strHeroList = pGlobalCfg->getString("初始英雄ID", "1018#1006");
	
	//vector<int> heroIDList;
	//heroIDList.push_back(iInitHeroID);

	vector<int> heroIdList = TC_Common::sepstr<int>(strHeroList,"#");
	assert(heroIdList.size() == 2);
	vector<CreateHeroDesc> heroIDList;
	CreateHeroDesc tmpHeroDesc1;
	
	int hero1Id = heroIdList[0];
	tmpHeroDesc1.iHeroID = hero1Id;
	tmpHeroDesc1.iFormationPos = 4;
	tmpHeroDesc1.iLevel = 1;
	

	ITable* pHeroTable = getCompomentObjectManager()->findTable(TABLENAME_Hero);
	assert(pHeroTable);

	int iRecord = pHeroTable->findRecord(hero1Id);
	assert(iRecord >= 0);
	tmpHeroDesc1.iLevelStep = pHeroTable->getInt(iRecord, "初始星级");
	tmpHeroDesc1.iQuality = pHeroTable->getInt(iRecord, "初始品质");

	CreateHeroDesc tmpHeroDesc2;
	int hero2Id = heroIdList[1];
	tmpHeroDesc2.iHeroID = hero2Id;
	tmpHeroDesc2.iFormationPos = 3;
	tmpHeroDesc2.iLevel = 1;
	
	int iRecord2 = pHeroTable->findRecord(hero2Id);
	assert(iRecord2 >= 0);
	tmpHeroDesc2.iLevelStep = pHeroTable->getInt(iRecord2, "初始星级");
	tmpHeroDesc2.iQuality = pHeroTable->getInt(iRecord2, "初始品质");

	heroIDList.push_back(tmpHeroDesc1);
	heroIDList.push_back(tmpHeroDesc2);
	
	fillNewActorData(actorSaveData, strAccount, iWorldID, strName, heroIDList);
}

void LoginManager::fillNewActorData(ServerEngine::RoleSaveData& actorSaveData, const std::string& strAccount, int iWorldID, const string& strName, const vector<CreateHeroDesc>& heroIDList)
{
	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	// 填充新角色数据
	uuid_t itemuuid;
	uuid_generate(itemuuid);
	char szUUIDString[1024] = {0};
	uuid_unparse_upper(itemuuid, szUUIDString);

	actorSaveData.basePropData.roleStringPropset[PROP_ENTITY_UUID] = szUUIDString;
	actorSaveData.basePropData.roleStringPropset[PROP_ACTOR_ACCOUNT] = strAccount;
	actorSaveData.basePropData.roleIntPropset[PROP_ACTOR_ROLEPOS] = 0;
	actorSaveData.basePropData.roleIntPropset[PROP_ENTITY_LEVEL] = 1;

	IVIPFactory* pVipFactory = getComponent<IVIPFactory>(COMPNAME_VIPFactory, IID_IVIPFactory);
	assert(pVipFactory);
	
	actorSaveData.basePropData.roleIntPropset[PROP_ENTITY_PHYSTRENGTH] = pVipFactory->getVipPropByVIPLevel(0, VIP_PROP_PHYSTRENGTH_UPLIMIT);
	
	actorSaveData.basePropData.roleIntPropset[PROP_ACTOR_WORLD] = iWorldID;
	actorSaveData.basePropData.roleIntPropset[PROP_ENTITY_LASTPHYSTRENGTHTIME] = time(0);
	
	 //add by hyf
	//设置玩家VIP等级
	if(m_bIsActive)
	{
		actorSaveData.basePropData.roleIntPropset[PROP_ACTOR_VIPLEVEL] = 6;
		actorSaveData.basePropData.roleIntPropset[PROP_ACTOR_VIPEXP] = pVipFactory->getVipPropByVIPLevel(6,VIP_PROP_VIPEXP);
	 
	}
	
	 
	actorSaveData.basePropData.roleStringPropset[PROP_ENTITY_NAME] = strName;

	// 填充英雄信息
	fillHeroFormationSystemData(actorSaveData,  heroIDList);
}

	
void LoginManager::fillHeroFormationSystemData(ServerEngine::RoleSaveData& actorSaveData, const vector<CreateHeroDesc>& heroIDList)
{
	// 英雄数据
	ITable* pHeroTb = getCompomentObjectManager()->findTable(TABLENAME_Hero);
	assert(pHeroTb);

	ServerEngine::FormationSystemData formationSysData;
	ServerEngine::HeroSystemData heroSysSaveData;
	
	for(size_t i = 0; i < heroIDList.size(); i++)
	{
		const CreateHeroDesc& tmpHeroDesc = heroIDList[i];
		int iRecord = pHeroTb->findRecord(tmpHeroDesc.iHeroID);
		assert(iRecord >= 0);
		
		ServerEngine::RoleSaveData heroSaveData;
		heroSaveData.basePropData.roleIntPropset[PROP_ENTITY_BASEID] = tmpHeroDesc.iHeroID;
		heroSaveData.basePropData.roleIntPropset[PROP_ENTITY_LEVEL] = tmpHeroDesc.iLevel;
		heroSaveData.basePropData.roleIntPropset[PROP_ENTITY_LEVELSTEP] = tmpHeroDesc.iLevelStep;
		heroSaveData.basePropData.roleIntPropset[PROP_ENTITY_QUALITY] = tmpHeroDesc.iQuality;

		// 设置UUID吧
		uuid_t itemuuid;
		uuid_generate(itemuuid);
		char szUUIDString[1024] = {0};
		uuid_unparse_upper(itemuuid, szUUIDString);
		heroSaveData.basePropData.roleStringPropset[PROP_ENTITY_UUID] = szUUIDString;

		// 英雄 技能数据
		/*ServerEngine::SkillSystemSaveData skillSysData;
		string strSkillList = pHeroTb->getString(iRecord, "技能列表");
		skillSysData.skillList = TC_Common::sepstr<int>(strSkillList, "#");
		heroSaveData.subsystemData[IID_ISkillSystem] = ServerEngine::JceToStr(skillSysData);*/
		
		heroSysSaveData.heroList.push_back(heroSaveData);
		

		//--------------------------
		//Actor 阵形数据
		
		ServerEngine::FormationItem tmpItem;
		tmpItem.strUUID = szUUIDString;

		if(i < GSProto::MAX_BATTLE_MEMBER_SIZE/2)
		{
			formationSysData.formationList[tmpHeroDesc.iFormationPos] = tmpItem;
		}
	}
	
	// 填充子系统数据
	actorSaveData.subsystemData[IID_IHeroSystem] = ServerEngine::JceToStr(heroSysSaveData);
	actorSaveData.subsystemData[IID_IFormationSystem] = ServerEngine::JceToStr(formationSysData);
}

std::vector<Uint32> LoginManager::getSupportMsgList()
{
	std::vector<Uint32> result;

	result.push_back(GSProto::CMD_ALOGIN);
	result.push_back(GSProto::CMD_GETROLE);
	result.push_back(GSProto::CMD_NEW_ROLE);
	result.push_back(GSProto::CMD_CHECK_ACTORNAME);
	result.push_back(GSProto::CMD_QUERY_FIRSTFIGHT);

	result.push_back(GSProto::CMD_REQUEST_REFRESHGOLD);
	
	return result;
}

Int32 LoginManager::getUserState(const std::string& strAccount)
{
	UserStateMap::iterator it = m_userStateMap.find(strAccount);
	if(it != m_userStateMap.end() )
	{
		return it->second.nState;
	}

	return 0;
}

const UserInfo* LoginManager::getUserInfo(const std::string& strAccount)
{
	UserStateMap::iterator it = m_userStateMap.find(strAccount);
	if(it != m_userStateMap.end() )
	{
		return &(it->second);
	}

	return NULL;
}

size_t LoginManager::getUserSize()
{
	return m_userStateMap.size();
}

void LoginManager::getAllUserQQ(std::vector<std::string>& qqList)
{
	qqList.clear();
	for(UserStateMap::iterator it = m_userStateMap.begin(); it != m_userStateMap.end(); it++)
	{
		qqList.push_back(it->first);
	}
}

const map<string, UserInfo>& LoginManager::getUserMap()
{
	return m_userStateMap;
}



void LoginManager::kickUser(const std::string& strAccount, int iReason, bool bSynSave)
{
	IMessageLayer* pMsgLayer = getComponent<IMessageLayer>("MessageLayer", IID_IMessageLayer);
	assert(pMsgLayer);

	const UserInfo* pUserInfo = getUserInfo(strAccount);
	if(!pUserInfo) return;

	FDLOG("KickOff")<<strAccount<<"|"<<iReason<<endl;

	HEntity hEntity = pUserInfo->hEntity;
	IEntity* pEntity = getEntityFromHandle(hEntity);
	if(pEntity)
	{
		//添加最近一次下线时间
		
		ILegionFactory* pLegionFactory = getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory);
		if(pLegionFactory)
		{
			pLegionFactory->actorLogionOut(hEntity,true);
		}
	
		IActorSaveSystem* pActorSaveSystem = static_cast<IActorSaveSystem*>(pEntity->querySubsystem(IID_IActorSaveSystem));
		assert(pActorSaveSystem);
		pActorSaveSystem->doSave(bSynSave);
	}

	// 通知客户端
	GSProto::SCMessage pkg;
	GSProto::Cmd_Sc_Kickoff tmpKickOff;
	tmpKickOff.set_ireason(iReason);
	HelpMakeScMsg(pkg, GSProto::CMD_KICKOFF_ACCOUNT, tmpKickOff);
	pMsgLayer->sendMessage2Connection(pUserInfo->strRsAddress, pUserInfo->ddConnectID, pkg);
	
	// 删除
	removeUser(strAccount);
}

void LoginManager::removeUser(const std::string& strAccount)
{
	UserStateMap::iterator it = m_userStateMap.find(strAccount);
	if(it == m_userStateMap.end() )
	{
		return;
	}
	
	HEntity hEntity = it->second.hEntity;
	IEntity* pEntity = getEntityFromHandle(hEntity);
	if(pEntity)
	{


		string strActorName = pEntity->getProperty(PROP_ENTITY_NAME, "");
		int iSrcWorldID = pEntity->getProperty(PROP_ACTOR_WORLD, 0);
		int iActorLevel = pEntity->getProperty(PROP_ENTITY_LEVEL,0);
		STAT_LOG_LOGOUT<<strActorName<<"|"<<strAccount<<"|"<<iSrcWorldID<<"|"<<iActorLevel<<endl;
		
		m_entityNameMap.erase(strActorName);
		pEntity->Release();
	}
	
	m_userStateMap.erase(it);
}


Uint32 LoginManager::getActorByName(const std::string& strName)
{
	EntityNameMap::iterator it = m_entityNameMap.find(strName);
	if(it != m_entityNameMap.end() )
	{
		return it->second;
	}

	return 0;
}

void LoginManager::onTimer(int nEventId)
{
	if(2 == nEventId)
	{
		ITimerComponent* pTimeAxis = getComponent<ITimerComponent>(COMPNAME_TimeAxis, IID_ITimerComponent);
		assert(pTimeAxis);
		pTimeAxis->killTimer(m_initHandler);
		if(!m_bGetedFirstBattle) generateFirstFight();
		return;
	}
	else if(1 == nEventId)
	{
		IGlobalCfg* pGlobal = getComponent<IGlobalCfg>("GlobalCfg", IID_IGlobalCfg);
		assert(pGlobal);

		int iTimeoutSecond = pGlobal->getInt("超时踢人时间", 300);

		Uint32 dwCurSecond = time(0);
		vector<string> kickList;
		for(UserStateMap::iterator it = m_userStateMap.begin(); it != m_userStateMap.end(); it++)
		{
			const UserInfo& userInfo = it->second;
			if(dwCurSecond > (userInfo.dwLastPingTime + iTimeoutSecond) )
			{
				kickList.push_back(it->first);
			}
		}

		// 踢人吧
		for(size_t i = 0; i < kickList.size(); i++)
		{
			kickUser(kickList[i], GSProto::en_KickOff_Timeout);
		}

		// 记录在线和最高在线
		STAT_ONLINE<<m_entityNameMap.size()<<endl;
		STAT_MAXONLINE<<getDayResetValue(m_maxOnline).iValue<<endl;
	}	
}


void LoginManager::removeReserveGhost(HEntity hGhost)
{
	IEntity* pEntity = getEntityFromHandle(hGhost);
	if(!pEntity) return;

	string strAccount = pEntity->getProperty(PROP_ACTOR_ACCOUNT, "");
	string strActorName = pEntity->getProperty(PROP_ENTITY_NAME, "");

	m_reserveGhostMap.erase(strAccount);
	m_reserveActorNameMap.erase(strActorName);
}


void LoginManager::addReserveGhost(HEntity hEntity)
{
	IEntity* pEntity = getEntityFromHandle(hEntity);
	if(!pEntity) return;

	string strAccount = pEntity->getProperty(PROP_ACTOR_ACCOUNT, "");
	string strActorName = pEntity->getProperty(PROP_ENTITY_NAME, "");

	m_reserveGhostMap[strAccount] = hEntity;
	m_reserveActorNameMap[strActorName] = hEntity;
}

HEntity LoginManager::getRichEntityByName(const string& strName)
{
	if(m_reserveActorNameMap.find(strName) != m_reserveActorNameMap.end() )
	{
		return m_reserveActorNameMap[strName];
	}

	return getActorByName(strName);
}


HEntity LoginManager::getRichEntityByAccount(const std::string& strAccount)
{
	// 先从正常渠道获得
	const UserInfo* pUserInfo = getUserInfo(strAccount);
	if(pUserInfo && (0 != pUserInfo->hEntity) )
	{
		return pUserInfo->hEntity;
	}

	// 验证下保留渠道
	map<string, HEntity>::iterator it = m_reserveGhostMap.find(strAccount);
	if(it != m_reserveGhostMap.end() )
	{
		return it->second;
	}

	return 0;
}

void LoginManager::getRecommendList(vector<HEntity>& vecHEntity)
{
	int size = m_entityNameMap.size();
	if(size > 100)
	{
		size = 100;
	}
	
	EntityNameMap::iterator iter = m_entityNameMap.begin();
	int i = 0;
	while(i < size)
	{
		if(iter != m_entityNameMap.end())
		{
			vecHEntity.push_back(iter->second);
		}
		else
		{
			break;
		}
		
		++i;
		++iter;
	}
}



