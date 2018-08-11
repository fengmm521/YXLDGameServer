#include "MailSystemPch.h"
#include "MailSystem.h"
#include "IArenaFactory.h"
//#include "TableImp.h"

//extern ComponentManagerImp	g_ComponentManagerImp;
using namespace MINIAPR;

extern "C" IObject* createMailSystem()
{
	return new MailSystem;
}


MailSystem::MailSystem()
{
}

MailSystem::~MailSystem()
{
	ITimerComponent* pTimer = getComponent<ITimerComponent>(COMPNAME_TimeAxis,IID_ITimerComponent);
	assert(pTimer);
	pTimer->killTimer( m_CheckDayResetMail);
	
//	IEntity *pEntity = getEntityFromHandle(m_hEntity);
//	assert(pEntity);
	//pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_LEVELUP, this, &MailSystem::ActorLevelUp);
}

Uint32 MailSystem::getSubsystemID() const
{
	return IID_IMailSystem;
}

Uint32 MailSystem::getMasterHandle()
{
	return m_hEntity;
}

bool MailSystem::create(IEntity* pEntity, const std::string& strData)
{
	m_hEntity = pEntity->getHandle();

	if(strData.size() > 0)
	{
		initData(strData);
	}

	fillBroadcastMail();
	fillAranaMail();
	DayCheck();

	//红点检查
	checkMailNotic();
	
	ITimerComponent* pTimer = getComponent<ITimerComponent>(COMPNAME_TimeAxis,IID_ITimerComponent);
	assert(pTimer);
	m_CheckDayResetMail = pTimer->setTimer(this, 1, 5*60*1000, "MailSystem");



	//pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_LEVELUP, this, &MailSystem::ActorLevelUp);
	return true;
}

void MailSystem::ActorLevelUp(EventArgs& args )
{
	int iNewLv = args.context.getInt("newlv");
	int iOldLv = args.context.getInt("oldlv");
	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);
	int iNeedLevel = pGlobalCfg->getInt("登陆奖励开启等级",10);
	if((iNeedLevel >iOldLv) && (iNeedLevel <=  iNewLv))
	{
		fillLoginAward();
	}
}


void MailSystem::checkMailNotic()
{
	bool bNotice = false;
	for(map<string, ServerEngine::MailData>::iterator it = m_mailDataList.begin(); it != m_mailDataList.end(); it++)
	{
		ServerEngine::MailData &data = it->second;
		if(data.iState == GSProto::en_MailState_UnRead)
		{
			bNotice = true;
			break;
		}
	}
	
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	pEntity->chgNotice( GSProto::en_NoticeGuid_HaveMail,bNotice);
}


void MailSystem::initData(const std::string& strData)
{
	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	
	ServerEngine::MailSystemData tmpSysData;
	ServerEngine::JceToObj(strData, tmpSysData);

	Uint32 dwCurSecond = time(0);
	for(size_t i = 0; i < tmpSysData.mailDataList.size(); i++)
	{
		// 过期的邮件，删除
		const ServerEngine::MailData &mailData = tmpSysData.mailDataList[i];
		Uint32 dwSeonderTime = mailData.dwSenderTime;
		Uint32 dwKeepDay = mailData.iKeepDay;
		if(dwCurSecond >= (dwSeonderTime + dwKeepDay * 3600 * 24) )
		{
			continue;
		}
	
		m_mailDataList[tmpSysData.mailDataList[i].strUUID] = tmpSysData.mailDataList[i];
	}
	std::copy(tmpSysData.usedBroadcastIDList.begin(), tmpSysData.usedBroadcastIDList.end(), std::inserter(m_usedBroadcastList, m_usedBroadcastList.end() ) );
	m_dwChgSecond = tmpSysData.dwChgSecond;
}


void MailSystem::onTimer(int nEventId)
{
	if(1 == nEventId)
	{
		//DayCheck();
		checkMailNotic();
		fillBroadcastMail();
	}
}

void  MailSystem::DayCheck()
{
	Uint32 dwCurSecond = time(0);
	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	if(!pZoneTime->IsInSameDay(m_dwChgSecond, dwCurSecond))
	{
		//fillLoginAward();
		m_dwChgSecond = dwCurSecond;
	}
}

void MailSystem::fillLoginAward()
{
	/*IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	int iHeroLevel = pEntity->getProperty(PROP_ENTITY_LEVEL,0);

	int iNeedLevel = pGlobalCfg->getInt("登陆奖励开启等级",10);
	if(iNeedLevel > iHeroLevel)
	{
		return;
	}
	
	IDropFactory* pDropFactory = getComponent<IDropFactory>(COMPNAME_DropFactory, IID_IDropFactory);
	assert(pDropFactory);

	int iMailKeepDay =  pGlobalCfg->getInt("邮件保留天数", 15);
	string strLoginAwardTitle = pGlobalCfg->getString("登陆奖励邮件标题", "Login Award");
	string strContent = pGlobalCfg->getString("登陆奖励邮件正文", "Login Award Info");
	string strSender = pGlobalCfg->getString("登陆奖励发送者", "System");

	int iDropId = pGlobalCfg->getInt("登陆奖励掉落ID", 2000);
	GSProto::FightAwardResult awardInfo;
	bool res =  pDropFactory->calcDrop(iDropId, awardInfo) ;
	assert(res);
	

	ServerEngine::MailData newMailData;

	uuid_t itemuuid;
	uuid_generate(itemuuid);
	char szUUIDString[1024] = {0};
	uuid_unparse_upper(itemuuid, szUUIDString);
	newMailData.strUUID = szUUIDString;
	newMailData.strTitle = strLoginAwardTitle;
	newMailData.strSender = strSender;
	newMailData.strContent = strContent;
	newMailData.iState = GSProto::en_MailState_UnRead;
	newMailData.dwSenderTime = time(0);
	newMailData.iKeepDay = iMailKeepDay;
	newMailData.iMailType = GSProto::enMailType_DaySend;
	if(awardInfo.IsInitialized() )
	{
		awardInfo.SerializeToString(&newMailData.strAwardInfo);
	}
	
	m_mailDataList[newMailData.strUUID] = newMailData;
	m_dwChgSecond = time(0);
	checkMailNotic();*/
}


void MailSystem::fillAranaMail()
{
	IArenaFactory* pArenaFactory = getComponent<IArenaFactory>(COMPNAME_ArenaFactory, IID_IArenaFactory);
	assert(pArenaFactory);

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	vector<ServerEngine::MailData> mailDataList;
		
	string strActorName = pEntity->getProperty(PROP_ENTITY_NAME, "");
	pArenaFactory->moveArenaAward(strActorName, mailDataList);

	for(size_t i = 0; i < mailDataList.size(); i++)
	{
		m_mailDataList[mailDataList[i].strUUID] = mailDataList[i];

		if(mailDataList[i].strAwardInfo.size() > 0)
		{
			GSProto::FightAwardResult tmpAwardResult;
			tmpAwardResult.ParseFromString(mailDataList[i].strAwardInfo);

			PLAYERLOG(pEntity)<<"AddMail|"<<"ArenaAward"<<"|"<<mailDataList[i].strUUID<<"|"<<tmpAwardResult.DebugString()<<endl;
		}
	}
}



void MailSystem::fillBroadcastMail()
{
	getCompomentObjectManager()->reloadTable("Mail");
	//if(!res) return;
	// 检测策划配置的公告
	ITable* pMailTb = getCompomentObjectManager()->findTable("Mail");
	assert(pMailTb);

	// 当前配置已经没有了的ID，删除，防止数据无限制膨胀
	for(set<int>::iterator it = m_usedBroadcastList.begin(); it != m_usedBroadcastList.end();)
	{
		if(pMailTb->findRecord(*it) < 0)
		{
			m_usedBroadcastList.erase(it++);
			continue;
		}
		else
		{
			it++;
		}
	}

	IZoneTime* pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	IDropFactory* pDropFactory = getComponent<IDropFactory>(COMPNAME_DropFactory, IID_IDropFactory);
	assert(pDropFactory);

	Uint32 dwCurTime = pZoneTime->GetCurSecond();
	int iLevel = pEntity->getProperty(PROP_ENTITY_LEVEL, 0);

	// 初始化，策划配置的新公告和奖励
	int iRecordCount = pMailTb->getRecordCount();
	for(int i = 0; i < iRecordCount; i++)
	{
		int iBroadcastID = pMailTb->getInt(i, "公告ID");

		// 已经产出过了，不处理
		if(m_usedBroadcastList.find(iBroadcastID) != m_usedBroadcastList.end() )
		{
			continue;
		}

		// 判断时间区间
		string strBeginTime = pMailTb->getString(i, "开始时间");
		string strEndTime = pMailTb->getString(i, "截至时间");

		Uint32 dwBeginTime = pZoneTime->StrTime2Seconds(strBeginTime.c_str(), "%Y/%m/%d");
		Uint32 dwEndTime = pZoneTime->StrTime2Seconds(strEndTime.c_str(), "%Y/%m/%d");
	
		if( (dwCurTime < dwBeginTime) || (dwCurTime > dwEndTime) )
		{
			continue;
		}
		

		// 判断等级区间
		int iMinLv = pMailTb->getInt(i, "最小等级");
		int iMaxLv = pMailTb->getInt(i, "最高等级");
		if( (iLevel < iMinLv) || (iLevel > iMaxLv) )
		{
			continue;
		}

		ServerEngine::MailData newMailData;

		uuid_t itemuuid;
		uuid_generate(itemuuid);
		char szUUIDString[1024] = {0};
		uuid_unparse_upper(itemuuid, szUUIDString);
		newMailData.strUUID = szUUIDString;

		newMailData.strTitle = pMailTb->getString(i, "标题");
		newMailData.strSender = pMailTb->getString(i, "发送者");
		newMailData.strContent = pMailTb->getString(i, "内容");
		newMailData.iState = GSProto::en_MailState_UnRead;
		newMailData.dwSenderTime = time(0);
		newMailData.iKeepDay = pMailTb->getInt(i, "保留天数");
		newMailData.iMailType = GSProto::enMailType_Com;
		

		// 制造奖励吧
		string strAwardDesc = pMailTb->getString(i, "奖励");
		vector<int> dropIDList = TC_Common::sepstr<int>(strAwardDesc, "#");

		GSProto::FightAwardResult awardResult;
		for(vector<int>::iterator it = dropIDList.begin(); it != dropIDList.end(); it++)
		{
			pDropFactory->calcDrop(*it, awardResult);
		}

		if(awardResult.IsInitialized() )
		{
			awardResult.SerializeToString(&newMailData.strAwardInfo);
		}

		m_mailDataList[newMailData.strUUID] = newMailData;

		m_usedBroadcastList.insert(iBroadcastID);

		PLAYERLOG(pEntity)<<"AddMail|Broadcast|"<<newMailData.strUUID<<"|"<<awardResult.DebugString()<<endl;
	}
}


bool MailSystem::createComplete()
{
	return true;
}

const std::vector<Uint32>& MailSystem::getSupportMessage()
{
	static std::vector<Uint32> resultList;

	if(resultList.size() == 0)
	{
		resultList.push_back(GSProto::CMD_QUERY_MAILLIST);
		resultList.push_back(GSProto::CMD_QUERY_MAILDESC);
		resultList.push_back(GSProto::CMD_GET_MAILAWARD);
	}

	return resultList;
}


void MailSystem::onMessage(QxMessage* pMessage)
{
	assert(pMessage->dwMsgLen == sizeof(GSProto::CSMessage) );
	const GSProto::CSMessage& msg = *(const GSProto::CSMessage*)(pMessage->pMsgDataBuff);

	switch(msg.icmd() )
	{
		case GSProto::CMD_QUERY_MAILLIST:
			onReqQueryMailList(msg);
			break;

		case GSProto::CMD_QUERY_MAILDESC:
			onReqQueryMailDesc(msg);
			break;

		case GSProto::CMD_GET_MAILAWARD:
			onReqGetMailAward(msg);
			break;
	}
}

template<class T>
struct MailSortFunctor
{
	bool operator()(const T& left, const T& right) const
	{
		if(left.istate() != right.istate() )
		{
			return left.istate() < right.istate();
		}
		
		if(left.dwsendtime() != right.dwsendtime() )
		{
			return left.dwsendtime() > right.dwsendtime();
		}

		return left.struuid() > right.struuid();		
	}
};

void MailSystem::onReqQueryMailList(const GSProto::CSMessage& msg)
{
	GSProto::CMD_QUERY_MAILLIST_SC scMsg;
	for(map<string, ServerEngine::MailData>::iterator it = m_mailDataList.begin(); it != m_mailDataList.end(); it++)
	{
		const ServerEngine::MailData& refMailData = it->second;
		GSProto::MailShotDesc* pNewShortDesc = scMsg.add_szmailindexlist();

		pNewShortDesc->set_struuid(refMailData.strUUID);
		pNewShortDesc->set_strmailtitle(refMailData.strTitle);
		pNewShortDesc->set_strmailsender(refMailData.strSender);
		pNewShortDesc->set_dwsendtime(refMailData.dwSenderTime);
		pNewShortDesc->set_istate(refMailData.iState);
	}

	std::sort(scMsg.mutable_szmailindexlist()->begin(), scMsg.mutable_szmailindexlist()->end(), MailSortFunctor<GSProto::MailShotDesc>() );

	IEntity* pMaster = getEntityFromHandle(m_hEntity);
	assert(pMaster);

	pMaster->sendMessage(GSProto::CMD_QUERY_MAILLIST, scMsg);
}

void MailSystem::onReqQueryMailDesc(const GSProto::CSMessage& msg)
{
	GSProto::CMD_QUERY_MAILDESC_CS req;
	if(!req.ParseFromString(msg.strmsgbody() ) )
	{
		return;
	}

	IEntity* pMaster = getEntityFromHandle(m_hEntity);
	assert(pMaster);

	string strUUID = req.struuid();

	map<string, ServerEngine::MailData>::iterator it = m_mailDataList.find(strUUID);
	if(it == m_mailDataList.end() )
	{
		pMaster->sendErrorCode(ERROR_MAIL_NOTEXIST);
		return;
	}

	ServerEngine::MailData& refMailData = it->second;

	// 纯文本邮件才标记状态
	if( (refMailData.iState == GSProto::en_MailState_UnRead) && (refMailData.strAwardInfo.size() == 0) )
	{
		refMailData.iState = GSProto::en_MailState_Readed;
	}

	GSProto::CMD_QUERY_MAILDESC_SC scMsg;
	GSProto::MailDetail* pMailDetail = scMsg.mutable_maildata();

	pMailDetail->set_struuid(refMailData.strUUID);
	pMailDetail->set_strmailtitle(refMailData.strTitle);
	pMailDetail->set_strmailsender(refMailData.strSender);
	pMailDetail->set_dwsendtime(refMailData.dwSenderTime);
	pMailDetail->set_strcontent(refMailData.strContent);
	pMailDetail->set_istate(refMailData.iState);

	if(refMailData.strAwardInfo.size() > 0)
	{
		bool bParseResult = pMailDetail->mutable_awardinfo()->ParseFromString(refMailData.strAwardInfo);
		assert(bParseResult);
	}

	pMaster->sendMessage(GSProto::CMD_QUERY_MAILDESC, scMsg);

	checkMailNotic();
}

void MailSystem::onReqGetMailAward(const GSProto::CSMessage& msg)
{
	GSProto::CMD_GET_MAILAWARD_CS req;
	if(!req.ParseFromString(msg.strmsgbody() ) )
	{
		return;
	}

	IEntity* pMaster = getEntityFromHandle(m_hEntity);
	assert(pMaster);

	string strUUID = req.struuid();

	map<string, ServerEngine::MailData>::iterator it = m_mailDataList.find(strUUID);
	if(it == m_mailDataList.end() )
	{
		pMaster->sendErrorCode(ERROR_MAIL_NOTEXIST);
		return;
	}

	ServerEngine::MailData& refMailData = it->second;
	if(refMailData.strAwardInfo.size() == 0)
	{
		pMaster->sendErrorCode(ERROR_MAIL_NOAWARD);
		return;
	}
	
	GSProto::FightAwardResult awardResult;
	if(!awardResult.ParseFromString(refMailData.strAwardInfo) )
	{
		pMaster->sendErrorCode(ERROR_MAIL_AWARDERROR);
		return;
	}

	refMailData.strAwardInfo = "";

	IDropFactory* pDropFactory = getComponent<IDropFactory>(COMPNAME_DropFactory, IID_IDropFactory);
	assert(pDropFactory);

	PLAYERLOG(pMaster)<<"GetMail|"<<strUUID<<"|"<<awardResult.DebugString()<<endl;

	pDropFactory->excuteDropWithNotify(m_hEntity, awardResult, GSProto::en_Reason_GetMailAward);

	GSProto::CMD_GET_MAILAWARD_SC scMsg;
	scMsg.set_struuid(strUUID);

	// 删除邮件
	m_mailDataList.erase(it);

	pMaster->sendMessage(GSProto::CMD_GET_MAILAWARD, scMsg);

	//add by hyf 
	checkMailNotic();
}


void MailSystem::packSaveData(string& data)
{
	ServerEngine::MailSystemData saveData;

	// 保存邮件数据
	for(map<string, ServerEngine::MailData>::iterator it = m_mailDataList.begin(); it != m_mailDataList.end(); it++)
	{
		saveData.mailDataList.push_back(it->second);
	}
	std::copy(m_usedBroadcastList.begin(), m_usedBroadcastList.end(), std::inserter(saveData.usedBroadcastIDList, saveData.usedBroadcastIDList.end() ) );
	saveData.dwChgSecond = m_dwChgSecond;
	data = ServerEngine::JceToStr(saveData);
}

void MailSystem::addMail(const std::string& strSender, const std::string& strTitle, const std::string& strContent, const GSProto::FightAwardResult& awardInfo, const string& strReason)
{
	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);
	
	int iMailKeepDay = pGlobalCfg->getInt("邮件保留天数", 15);

	ServerEngine::MailData newMailData;

	uuid_t itemuuid;
	uuid_generate(itemuuid);
	char szUUIDString[1024] = {0};
	uuid_unparse_upper(itemuuid, szUUIDString);
	newMailData.strUUID = szUUIDString;
	newMailData.strTitle = strTitle;
	newMailData.strSender = strSender;
	newMailData.strContent = strContent;
	newMailData.iState = GSProto::en_MailState_UnRead;
	newMailData.dwSenderTime = time(0);
	newMailData.iKeepDay = iMailKeepDay;
	newMailData.iMailType = GSProto::enMailType_Com;
	
	if(awardInfo.IsInitialized() )
	{
		awardInfo.SerializeToString(&newMailData.strAwardInfo);
	}

	m_mailDataList[newMailData.strUUID] = newMailData;

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	PLAYERLOG(pEntity)<<"AddMail|"<<strReason<<"|"<<newMailData.strUUID<<"|"<<awardInfo.DebugString()<<endl;
	//add by hyf 
	checkMailNotic();
}

void MailSystem::addMail(const std::string& strUUID, const ServerEngine::MailData& mailData, const string& strReason) 
{
	m_mailDataList[strUUID] = mailData;

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	PLAYERLOG(pEntity)<<"AddMail|"<<strReason<<"|"<<mailData.strUUID<<endl;
	//add by hyf 
	checkMailNotic();
}


void MailSystem::sendPayReturnMail(int iMoney)
{
	ServerEngine::MailData mailData;
	int iActorCanGetCoin = iMoney*15;
	ITable* pMailTb = getCompomentObjectManager()->findTable("Mail");
	assert(pMailTb);
	
	int iMailRecord = pMailTb->findRecord(1003);
	assert(iMailRecord >= 0);
	
	uuid_t itemuuid;
	uuid_generate(itemuuid);
	char szUUIDString[1024] = {0};
	uuid_unparse_upper(itemuuid, szUUIDString);
	mailData.strUUID = szUUIDString;

	mailData.strTitle = pMailTb->getString(iMailRecord, "标题");
	mailData.strSender = pMailTb->getString(iMailRecord, "发送者");
	string strContent =  pMailTb->getString(iMailRecord, "内容");
	string strTmpContent1 = TC_Common::replace(strContent, "%rank1", TC_Common::tostr(iMoney));
	string strTmpContent2 = TC_Common::replace(strTmpContent1, "%rank2", TC_Common::tostr(iActorCanGetCoin));
	
	mailData.strContent = strTmpContent2;
	mailData.iState = GSProto::en_MailState_UnRead;
	mailData.dwSenderTime = time(0);
	mailData.iKeepDay = pMailTb->getInt(iMailRecord, "保留天数");
	mailData.iMailType = GSProto::enMailType_Com;

	
	GSProto::FightAwardResult awardResult;
	GSProto::PropItem &item = *(awardResult.add_szawardproplist());
	item.set_ilifeattid(GSProto::en_LifeAtt_Gold);
	item.set_ivalue(iActorCanGetCoin);
		
	if(awardResult.IsInitialized() )
	{
		awardResult.SerializeToString(&mailData.strAwardInfo);
	}

	addMail(mailData.strUUID,mailData, "GSProto::en_Reason_PayReturn");
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	PLAYERLOG(pEntity)<<"AddMail| PayReturn|"<<mailData.strUUID<<"|"<<awardResult.DebugString()<<endl;
}


void MailSystem::sendLifeLongVIP(int iVIPLevel)
{

	int iVIPMailId = 0;
	if(iVIPLevel == 7)
	{
		iVIPMailId = 1005;
	}
	else if(iVIPLevel == 10)
	{
		iVIPMailId = 1004;
	}
	else
	{
		return;
	}

	ServerEngine::MailData mailData;
	
	ITable* pMailTb = getCompomentObjectManager()->findTable("Mail");
	assert(pMailTb);
	int iMailRecord = pMailTb->findRecord(iVIPMailId);
	assert(iMailRecord >= 0);
	
	uuid_t itemuuid;
	uuid_generate(itemuuid);
	char szUUIDString[1024] = {0};
	uuid_unparse_upper(itemuuid, szUUIDString);
	mailData.strUUID = szUUIDString;

	mailData.strTitle = pMailTb->getString(iMailRecord, "标题");
	mailData.strSender = pMailTb->getString(iMailRecord, "发送者");
	string strContent =  pMailTb->getString(iMailRecord, "内容");
	
	mailData.strContent = strContent;
	mailData.iState = GSProto::en_MailState_UnRead;
	mailData.dwSenderTime = time(0);
	mailData.iKeepDay = pMailTb->getInt(iMailRecord, "保留天数");
	mailData.iMailType = GSProto::enMailType_Com;

	string strAwardDesc = pMailTb->getString(iMailRecord, "奖励");
	vector<int> dropIDList = TC_Common::sepstr<int>(strAwardDesc, "#");

	
	IDropFactory* pDropFactory = getComponent<IDropFactory>(COMPNAME_DropFactory, IID_IDropFactory);
	assert(pDropFactory);

	GSProto::FightAwardResult awardResult;
	for(vector<int>::iterator it = dropIDList.begin(); it != dropIDList.end(); it++)
	{
		pDropFactory->calcDrop(*it, awardResult);
	}

	if(awardResult.IsInitialized() )
	{
		awardResult.SerializeToString(&mailData.strAwardInfo);
	}
		

	addMail(mailData.strUUID,mailData, "GSProto::en_Reason_PayReturn");
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	PLAYERLOG(pEntity)<<"AddMail| PayReturn|"<<mailData.strUUID<<"|"<<awardResult.DebugString()<<endl;
}



