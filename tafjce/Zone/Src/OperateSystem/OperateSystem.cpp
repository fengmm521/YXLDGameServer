#include "OperateSystemPch.h"
#include "OperateSystem.h"
#include "OperateFactory.h"
#include "util/tc_http.h"
#include "util/tc_http_async.h"

extern TC_HttpAsync g_httpAsync;

extern "C" IObject* createOperateSystem()
{
	return new OperateSystem;
}

OperateSystem::OperateSystem()
	:m_hEntity(0),
	m_bHaveGetTelphoneSteam(false)
{
}

OperateSystem::~OperateSystem()
{
	ITimerComponent* timeComponent = getComponent<ITimerComponent>(COMPNAME_TimeAxis,IID_ITimerComponent);
	assert(timeComponent);
	timeComponent->killTimer( m_CheckNoticeTimerHandle);
}

Uint32 OperateSystem::getSubsystemID() const
{
	return IID_IOperateSystem;
}

Uint32 OperateSystem::getMasterHandle()
{
	return m_hEntity;
}

bool OperateSystem::create(IEntity* pEntity, const std::string& strData)
{
	m_hEntity = pEntity->getHandle();

	if(strData.length()>0)
	{
		initData(strData);
	}
	else
	{
		m_growUpdata.bHavebuyGrowUp = false;
		m_bHaveGetTelphoneSteam = false;
		m_actorLoginData.dwLoginSecond = time(0);
		m_actorLoginData.iLoginDays +=1;
	}
	

	ITimerComponent* timeComponent = getComponent<ITimerComponent>(COMPNAME_TimeAxis,IID_ITimerComponent);
	assert(timeComponent);
	m_CheckNoticeTimerHandle = timeComponent->setTimer(this,1,5*1000,"OperateSystem");
	checkNotice();
	return true;
}

void OperateSystem::onTimer(int nEventId)
{
	if(1 == nEventId)
	{
		checkNotice();
	}
}

void OperateSystem::checkNotice()
{
	bool bNeedNotice = true;
	int iHaveCheckInCount = m_CheckInData.checkInList.size();
	
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	if(m_CheckInData.checkInList.size() > 0)
	{
		const ServerEngine::TimeResetValue&  lastCheckIn = m_CheckInData.checkInList.back();
			
		IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
		assert(pZoneTime);
		if(pZoneTime->IsInSameDay(lastCheckIn.dwLastChgTime, pZoneTime->GetCurSecond()))
		{
			IOperateFactory *pOperateFac = getComponent<IOperateFactory>(COMPNAME_OperateFactory, IID_IOperateFactory);
			assert(pOperateFac);
			int iMonth = pZoneTime->GetMonthInYear(pZoneTime->GetCurSecond());
					
			CheckInUnit  unit;
			bool res = pOperateFac->checkInGetReward(iMonth, iHaveCheckInCount - 1, unit);
			assert(res);

			if(unit.bCanGetTwice)
			{
				int iActorVipLevel = pEntity->getProperty(PROP_ACTOR_VIPLEVEL, 0);
				if(iActorVipLevel > unit.iVipNeed)
				{
					if(lastCheckIn.iValue > 1)
					{
						bNeedNotice = false;
					}
				}
				else
				{
					bNeedNotice = false;
				}
					
			}
			else
			{
				if(lastCheckIn.iValue == 1)
				{
					bNeedNotice = false;
				}
			}
			
		}
	}
	
	pEntity->chgNotice( GSProto::en_NoticeGuid_CheckIn, bNeedNotice);


	//check grow Up
	checkGrowUpNotice();
	checkAccPayMentNotice();
	//
}

void OperateSystem::checkGrowUpNotice()
{
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	bool bNoticeGrowUp = false;
	if(m_growUpdata.bHavebuyGrowUp)
	{

		IEntity *pEntity = getEntityFromHandle(m_hEntity);
		assert(pEntity);
		
		int iActorLevel = pEntity->getProperty( PROP_ENTITY_LEVEL,1);
		
		OperateFactory* pOperateFactory = static_cast<OperateFactory*>(getComponent<IOperateFactory>(COMPNAME_OperateFactory, IID_IOperateFactory));
		assert(pOperateFactory);
		vector<GrowUpPlan> growUpPlanList =  pOperateFactory->getGrowUpPlanList();
		
		
			
		for(size_t i = 0; i < growUpPlanList.size(); ++i)
		{
			const GrowUpPlan& planUnit = growUpPlanList[i];
			map<int,int>::iterator iter = m_growUpdata.haveGetReward.find(planUnit.iId);
			if(iter != m_growUpdata.haveGetReward.end())
			{
				continue;
			}

			if(iActorLevel >= planUnit.iGoal)
			{
				bNoticeGrowUp = true;
				break;
			}
		}
		
	}

	pEntity->chgNotice( GSProto::en_NoticeGuid_GrowUpHaveReward, bNoticeGrowUp);
}

void OperateSystem::checkAccPayMentNotice()
{
	IEntity *pActor = getEntityFromHandle(m_hEntity);
	assert(pActor);
	
	OperateFactory* pOperateFactory = static_cast<OperateFactory*>(getComponent<IOperateFactory>(COMPNAME_OperateFactory, IID_IOperateFactory));
	assert(pOperateFactory);
	
	int accPayCount = pActor->getProperty(PROP_ACTOR_ACC_PAYMENT, 0);
	bool bNotice = false;
		
	vector<AccumulatePayment> accpayMentList =  pOperateFactory->getAccumulatePayMentList();
	for(size_t i =0 ; i < accpayMentList.size(); ++i)
	{
		const AccumulatePayment& accPayMentUnit = accpayMentList[i];
		map<int,int>::iterator iter = m_accPayMentData.paymentGetRewardMap.find(accPayMentUnit.iId);
		if(iter != m_accPayMentData.paymentGetRewardMap.end())
		{
			continue;
		}

		
		if(accPayMentUnit.iGoal <= accPayCount)
		{
			bNotice	 = true;
			break;
		}
		
	}
	pActor->chgNotice( GSProto::en_NoticeGuid_ACCPaymentHaveReward, bNotice);
}

bool OperateSystem::initData(const std::string& strData)
{
	ServerEngine::OperateSystemData operateData;

	ServerEngine::JceToObj(strData, operateData);

	m_CheckInData = operateData.checkInData;

	m_growUpdata = operateData.growUpdata;
	m_accPayMentData = operateData.accPayMentData;
	m_bHaveGetTelphoneSteam = operateData.bHaveGetTelphoneSteam;
	m_actorLoginData =  operateData.actorLoginData;
	this->m_dayPaymentData	= operateData.dayPaymentData;

	IZoneTime *pZone = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZone);
	if(!pZone->IsInSameDay(m_actorLoginData.dwLoginSecond ,pZone->GetCurSecond()))
	{
		m_actorLoginData.dwLoginSecond = pZone->GetCurSecond();
		m_actorLoginData.iLoginDays +=1;
	}
		
	return true;
}

void OperateSystem::gmLogin()
{	
	IZoneTime *pZone = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
		assert(pZone);

	m_actorLoginData.dwLoginSecond = pZone->GetCurSecond();
	m_actorLoginData.iLoginDays +=1;
}


void OperateSystem::IncDayPaymentPaied(int _dt)
{
	this->m_dayPaymentData.paied += _dt;
}


bool OperateSystem::createComplete()
{
	
	return true;
}


const std::vector<Uint32>& OperateSystem::getSupportMessage()
{
	static vector<Uint32> msgVec;
	if(msgVec.size() == 0)
	{
		msgVec.push_back(GSProto::CMD_OPERATESYSTEM_QUERY_CHECKIN);
		msgVec.push_back(GSProto::CMD_OPERATESYSTEM_CHECKIN);	
		
		msgVec.push_back(GSProto::CMD_QUERY_GROWUP_DETAIL);
		msgVec.push_back(GSProto::CMD_BUY_GROWUP_PLAN);
		msgVec.push_back(GSProto::CMD_GET_GROWUP_REWARD);
		
		msgVec.push_back(GSProto::CMD_QUERY_ACCOUNT_PAYMENT_DETAIL);
		msgVec.push_back(GSProto::CMD_GET_ACCOUNT_PAYMENT_REWARD);
		msgVec.push_back(GSProto::CMD_QUERY_OPERATE_ACTIVE);
		msgVec.push_back(GSProto::CMD_OPERATE_TELPHONE_STREAM);
		msgVec.push_back(GSProto::CMD_QUERY_ACCOUNT_LOGIN_IN);
		msgVec.push_back(GSProto::CMD_GET_ACCOUNT_REWARD);
		msgVec.push_back(GSProto::CMD_QUERY_DAY_PAYMENT_DETAIL);
		msgVec.push_back(GSProto::CMD_GET_DAY_PAYMENT_REWARD);
	}

	
	return msgVec;
}

void OperateSystem::onMessage(QxMessage* pMessage)
{
	assert(pMessage->dwMsgLen== sizeof(GSProto::CSMessage));
	const GSProto::CSMessage& msg = *(const GSProto::CSMessage*)(pMessage->pMsgDataBuff);

	switch(msg.icmd())
	{
		case GSProto::CMD_OPERATESYSTEM_QUERY_CHECKIN:
		{
			checkIncheckReset();
			onQueryCheckInInfo(msg);
		}break;
		
		case GSProto::CMD_OPERATESYSTEM_CHECKIN:
		{
			checkIncheckReset();
			onExeCheckIn(msg);
		}break;

		case GSProto::CMD_QUERY_GROWUP_DETAIL:
		{
			onQueryGrowUpPlanInfo(msg);
		}break;

		case GSProto::CMD_BUY_GROWUP_PLAN:
		{
			onBuyGrowUpPlan(msg);
		}break;

		case GSProto::CMD_GET_GROWUP_REWARD:
		{
			onGetGrowUpPlanReward(msg);
		}break;
		
		case GSProto::CMD_QUERY_ACCOUNT_PAYMENT_DETAIL:
		{
			onQueryAccPaymentInfo(msg);
		}break;
		
		case GSProto::CMD_GET_ACCOUNT_PAYMENT_REWARD:
		{
			onGetAccPaymentReward(msg);
		}break;

		case GSProto::CMD_QUERY_OPERATE_ACTIVE:
		{
			onQueryOpenaActive(msg);
		}break;

		case GSProto::CMD_OPERATE_TELPHONE_STREAM:
		{
			onGetPhoneStream( msg);
		}break;

		case GSProto::CMD_QUERY_ACCOUNT_LOGIN_IN:
		{
				onQueryAccLogin(msg);
		}break;

		case GSProto::CMD_GET_ACCOUNT_REWARD:
		{
			onGetAccLoginReward(msg);
		}break;

		case GSProto::CMD_QUERY_DAY_PAYMENT_DETAIL:
			onQueryDayPaymentDetail(msg);
			break;

		case GSProto::CMD_GET_DAY_PAYMENT_REWARD:
			onGetDayPaymentReward(msg);
			break;
		
	}
}

void OperateSystem::onQueryAccLogin(const GSProto::CSMessage& msg)
{
	GSProto::CMD_QUERY_ACCOUNT_LOGIN_IN_SC scMsg;
	scMsg.set_iacclogincount(m_actorLoginData.iLoginDays);
	
 	OperateFactory *pOperate = static_cast<OperateFactory*>(getComponent<IOperateFactory>(COMPNAME_OperateFactory,IID_IOperateFactory));
	assert(pOperate);
	
	vector<AccumlateLoginIn>& acclatLoginInVec = pOperate->getAccumulateLoginList();
	assert(acclatLoginInVec.size() > 0);

	for(size_t i = 0 ; i < acclatLoginInVec.size(); ++i)
	{
		const AccumlateLoginIn& unit = acclatLoginInVec[i];
		int dayCount = unit.iId;
		map<taf::Int32, taf::Int32>::iterator iter = m_actorLoginData.haveRewardDays.find(dayCount);
		if(iter != m_actorLoginData.haveRewardDays.end()) continue;

		scMsg.add_szaccount(dayCount);
	}

	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	pEntity->sendMessage(GSProto::CMD_QUERY_ACCOUNT_LOGIN_IN, scMsg);
}

void OperateSystem::onGetAccLoginReward(const GSProto::CSMessage& msg)
{
	GSProto::CMD_GET_ACCOUNT_REWARD_CS csMsg;
	if(!csMsg.ParseFromString(msg.strmsgbody()))
	{
		return;
	}

	int iAccountTimes = csMsg.iaccounttimes();

	//1.领了的不能再领
	map<taf::Int32, taf::Int32>::iterator iter = m_actorLoginData.haveRewardDays.find(iAccountTimes);
	if(iter != m_actorLoginData.haveRewardDays.end()) return;

	//2.没有到这个天数不能ling
	if(iAccountTimes > m_actorLoginData.iLoginDays) return;

	//3.去领取
	OperateFactory *pOperate = static_cast<OperateFactory*>(getComponent<IOperateFactory>(COMPNAME_OperateFactory,IID_IOperateFactory));
	assert(pOperate);
	
	vector<AccumlateLoginIn>& acclatLoginInVec = pOperate->getAccumulateLoginList();
	assert(acclatLoginInVec.size() > 0);
		
	int iDropId = -1;
	for(size_t i = 0 ; i < acclatLoginInVec.size(); ++i)
	{
			const AccumlateLoginIn& unit = acclatLoginInVec[i];
			int dayCount = unit.iId;
			if(iAccountTimes == dayCount)
			{
				iDropId = unit.iDropId;
			}
	}
	if(iDropId== -1 )return;
	m_actorLoginData.haveRewardDays.insert(make_pair(iAccountTimes,1));

	IDropFactory *pDropFactory = getComponent<IDropFactory>(COMPNAME_DropFactory, IID_IDropFactory);
	assert(pDropFactory);
	
	GSProto::CMD_GET_ACCOUNT_REWARD_SC scMsg;
	GSProto::FightAwardResult& reward = *(scMsg.mutable_reward());
	
	bool res1 = pDropFactory->calcDrop(iDropId, reward);
	assert(res1);
	pDropFactory->excuteDrop(m_hEntity, reward,GSProto::en_Reason_Get_Account_Login_Reward);
	
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	pEntity->sendMessage(GSProto::CMD_GET_ACCOUNT_REWARD, scMsg);
	
}

class GetPhoneStreamCallback : public ServerEngine::LJSDKPrxCallback
{
	public:
		GetPhoneStreamCallback(const string strAccount):m_strAccount(strAccount){}
		
		virtual void callback_getPhoneStream()
		{ 
		}
		virtual void callback_getPhoneStream_exception(taf::Int32 ret)
		{

		}

	private:
		string m_strAccount;
};


void OperateSystem::gmGetPhoneSteam(string strPhoneNum)
{
	IGlobalCfg *pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);
	OperateFactory *pFactory = static_cast<OperateFactory*>(getComponent<IOperateFactory>(COMPNAME_OperateFactory, IID_IOperateFactory));
	assert(pFactory);

	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	vector<string> strVec = TC_Common::sepstr<string>(strPhoneNum,"_");
	if(strVec.size() != 2)
	{
		return;
	}

	//
	string telPhoneNum = strVec[0] + strVec[1];
	

	//送流量
	string streamURL = pGlobal->getString("送流量url","http://sdk.gmall.cn/FlowService/FlowService.asmx/SendFlow?");

	string strProductCode ;
	bool bRes = pFactory->getStreamCode(strVec[0],strProductCode);
	if(!bRes) 
	{	
		FDLOG("PhoneStream")<<telPhoneNum<<"|"<<pEntity->getProperty(PROP_ACTOR_ACCOUNT,"")<<"|"<<pEntity->getProperty(PROP_ENTITY_NAME,"")<<endl;
		return;
	}

	/*
		?LoginName=jiansheng&Password=fe7740bf0a065227&SmsKind=820&SendSim=13585857669&ProCode=QG1008610010	
	 */
	string strUrl = streamURL+"LoginName=jiansheng&Password=fe7740bf0a065227&SmsKind=820&SendSim="+telPhoneNum+"&ProCode="+strProductCode;
	cout<<"strUrl = " <<strUrl.c_str()<<endl;
	
	IJZMessageLayer* pJZMessageLayer = getComponent<IJZMessageLayer>(COMPNAME_MessageLayer, IID_IJZMessageLayer);
	assert(pJZMessageLayer);

	pJZMessageLayer->AsyncGetPhoneStream(new GetPhoneStreamCallback(pEntity->getProperty(PROP_ACTOR_ACCOUNT,"") ), strUrl);

	PLAYERLOG(pEntity)<<telPhoneNum<<endl;
}

void OperateSystem::onGetPhoneStream(const GSProto::CSMessage& msg)
{
	
	//是否已经领取
	if(m_bHaveGetTelphoneSteam)
	{
		return;
	}

	IGlobalCfg *pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);
	
	int iSecond = pGlobal->getInt("送流量开服有效时间",432000);
	OperateFactory *pFactory = static_cast<OperateFactory*>(getComponent<IOperateFactory>(COMPNAME_OperateFactory, IID_IOperateFactory));
	assert(pFactory);
	if(time(0) > pFactory->getOpenServerSecond() + iSecond)
	{
		return;
	}
	
	//去领取
	GSProto::CMD_OPERATE_TELPHONE_STREAM_CS csMsg;
	if(!csMsg.ParseFromString(msg.strmsgbody()))
	{
		return;
	}
	vector<string> strVec = TC_Common::sepstr<string>(csMsg.strtelphonenum(),"_");
	
	if(strVec.size() != 2)
	{
		return;
	}

	//
	string telPhoneNum = strVec[0] + strVec[1];
	
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	int iAcotorLevel = pEntity->getProperty(PROP_ENTITY_LEVEL,1);
	int iNeedLevel = pGlobal->getInt("送流量等级",35);

	if(iAcotorLevel < iNeedLevel)
	{
		return;
	}

	//送流量
	string streamURL = pGlobal->getString("送流量url","http://sdk.gmall.cn/FlowService/FlowService.asmx/SendFlow?");

	string strProductCode ;
	bool bRes = pFactory->getStreamCode(strVec[0],strProductCode);
	if(!bRes)
	{
		FDLOG("PhoneStream")<<telPhoneNum<<"|"<<pEntity->getProperty(PROP_ACTOR_ACCOUNT,"")<<"|"<<pEntity->getProperty(PROP_ENTITY_NAME,"")<<endl;
		return;
	}
		

	/*
		?LoginName=jiansheng&Password=fe7740bf0a065227&SmsKind=820&SendSim=13585857669&ProCode=QG1008610010	
	 */
	string strUrl = streamURL+"LoginName=jiansheng&Password=fe7740bf0a065227&SmsKind=820&SendSim="+telPhoneNum+"&ProCode="+strProductCode;
	
	//cout<<"strUrl = " <<strUrl.c_str()<<endl;
	
	IJZMessageLayer* pJZMessageLayer = getComponent<IJZMessageLayer>(COMPNAME_MessageLayer, IID_IJZMessageLayer);
	assert(pJZMessageLayer);

	pJZMessageLayer->AsyncGetPhoneStream(new GetPhoneStreamCallback(pEntity->getProperty(PROP_ACTOR_ACCOUNT,"")), strUrl);
	
	m_bHaveGetTelphoneSteam = true;
	pEntity->sendMessage(GSProto::CMD_OPERATE_TELPHONE_STREAM);
	
}

void OperateSystem::onQueryOpenaActive(const GSProto::CSMessage& msg)
{
	GSProto::CMD_QUERY_OPERATE_ACTIVE_SC scMsg;
	//加入成长计划

	OperateFactory* pOperateFactory = static_cast<OperateFactory*>(getComponent<IOperateFactory>(COMPNAME_OperateFactory, IID_IOperateFactory));
	assert(pOperateFactory);
		
	if(!m_growUpdata.bHavebuyGrowUp)
	{
		//是否过期
		if(time(0) < pOperateFactory->getOpenServerSecond() + ONE_WEEK_SECONDS)
		{
			scMsg.add_szopenactive( GSProto::enSpecialActiveId_GrowUpID);
		}
	}
	else
	{
		vector<GrowUpPlan> growUpPlanList =  pOperateFactory->getGrowUpPlanList();
		int iSize = growUpPlanList.size();
		int iRewardSize = m_growUpdata.haveGetReward.size();
		if(iSize > iRewardSize)
		{
			scMsg.add_szopenactive( GSProto::enSpecialActiveId_GrowUpID);
		}
	}
	
	//加入累积充值
	vector<AccumulatePayment> accpayMentList =  pOperateFactory->getAccumulatePayMentList();
	int iSize = accpayMentList.size();
	int iGetRewardSize = m_accPayMentData.paymentGetRewardMap.size();
	if(iSize > iGetRewardSize)
	{
		scMsg.add_szopenactive( GSProto::enSpecialActiveId_AccPayMent);
	}

	//添加手机号
	IGlobalCfg *pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);
	int iOpenPhoneString = pGlobal->getInt("送流量开关",0);
	if(iOpenPhoneString == 1)
	{
		if(!m_bHaveGetTelphoneSteam)
		{
			int iSecond = pGlobal->getInt("送流量开服有效时间",432000);
			if(time(0) < pOperateFactory->getOpenServerSecond() + iSecond)
			{
			  scMsg.add_szopenactive( GSProto::enSpecialActiveId_PhoneStream);
			}
			
		}
	}


	//七天登陆
	if(m_actorLoginData.haveRewardDays.size() < 7)
	{
		scMsg.add_szopenactive( GSProto::enSpecialActiveId_AccLogin);
	}

	// 日累计充值活动
	DayPayment* activeDayPayment = pOperateFactory->getActiveDayPayment();
	if(activeDayPayment != NULL)
	{
		scMsg.add_szopenactive( GSProto::enSpecialActiveId_DayPayment);
	}
	

	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	pEntity->sendMessage(GSProto::CMD_QUERY_OPERATE_ACTIVE, scMsg);
}

void OperateSystem::onQueryAccPaymentInfo(const GSProto::CSMessage& msg)
{
	PROFILE_MONITOR("onQueryAccPaymentInfo");
	
	OperateFactory *pOperateFactory = static_cast<OperateFactory*>(getComponent<IOperateFactory>(COMPNAME_OperateFactory, IID_IOperateFactory));
	assert(pOperateFactory);

	//IDropFactory *pDropFactory = getComponent<IDropFactory>(COMPNAME_DropFactory, IID_IDropFactory);
	//(pDropFactory);

	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	int accPayCount = pEntity->getProperty(PROP_ACTOR_ACC_PAYMENT, 0);

	GSProto::CMD_QUERY_ACCOUNT_PAYMENT_DETAIL_SC scMsg;
	scMsg.set_iactoraccpayment(accPayCount);
	
	vector<AccumulatePayment> accpayMentList =  pOperateFactory->getAccumulatePayMentList();
	for(size_t i = 0; i < accpayMentList.size(); ++i)
	{
		const AccumulatePayment& accPayMentUnit = accpayMentList[i];
		
		map<int,int>::iterator iter = m_accPayMentData.paymentGetRewardMap.find(accPayMentUnit.iId);
		if(iter != m_accPayMentData.paymentGetRewardMap.end())
		{
			continue;
		}

		GSProto::AccountUnit&  unit = *(scMsg.add_szaccountlist());
		
		//bool res = pDropFactory->calcDrop(accPayMentUnit.iDropId, *(unit.mutable_award()));
		//assert(res);
		
		unit.set_igoal(accPayMentUnit.iGoal);
		unit.set_iid(accPayMentUnit.iId);
	}
	pEntity->sendMessage(GSProto::CMD_QUERY_ACCOUNT_PAYMENT_DETAIL, scMsg);
}

void OperateSystem::onGetAccPaymentReward(const GSProto::CSMessage& msg)
{
	GSProto::CMD_GET_ACCOUNT_PAYMENT_REWARD_CS csMsg;
	if(!csMsg.ParseFromString(msg.strmsgbody()))
	{
		return;
	}

	int iId = csMsg.iid();

	map<int,int>::iterator iter = m_accPayMentData.paymentGetRewardMap.find(iId);
	if(iter != m_accPayMentData.paymentGetRewardMap.end())
	{
		return;
	}

	IEntity *pActor = getEntityFromHandle(m_hEntity);
	assert(pActor);
	
	OperateFactory* pOperateFactory = static_cast<OperateFactory*>(getComponent<IOperateFactory>(COMPNAME_OperateFactory, IID_IOperateFactory));
	assert(pOperateFactory);
	
	vector<AccumulatePayment> accpayMentList =  pOperateFactory->getAccumulatePayMentList();

	int iCanGetId  = -1;
	int iDropId = -1;
	
	for(size_t i =0 ; i < accpayMentList.size(); ++i)
	{
		const AccumulatePayment& accPayMentUnit = accpayMentList[i];
		
		map<int,int>::iterator iter = m_accPayMentData.paymentGetRewardMap.find(accPayMentUnit.iId);
		if(iter != m_accPayMentData.paymentGetRewardMap.end())
		{
			continue;
		}
		
		if(accPayMentUnit.iId ==iId)
		{
			int accPayCount = pActor->getProperty(PROP_ACTOR_ACC_PAYMENT, 0);
			if(accPayCount < accPayMentUnit.iGoal)
			{
				return;
			}
			
			iCanGetId = accPayMentUnit.iId;
			iDropId = accPayMentUnit.iDropId;
			break;
		}
		
	}

	if(iCanGetId != -1)
	{
		//发奖
		IDropFactory *pDropFactory = getComponent<IDropFactory>(COMPNAME_DropFactory, IID_IDropFactory);
		assert(pDropFactory);
		
		GSProto::CMD_GET_ACCOUNT_PAYMENT_REWARD_SC scMsg;
		GSProto::FightAwardResult& reward = *(scMsg.mutable_reward());
		
		bool res1 = pDropFactory->calcDrop(iDropId, reward);
		assert(res1);
		pDropFactory->excuteDrop(m_hEntity, reward,GSProto::en_Reason_ACC_PayMent_GET);
		
		m_accPayMentData.paymentGetRewardMap.insert(make_pair(iCanGetId,1));

		pActor->sendMessage(GSProto::CMD_GET_ACCOUNT_PAYMENT_REWARD, scMsg);
	}
		
}

void OperateSystem::sendGrowUpPlanInfo()
{
	OperateFactory* pOperateFactory = static_cast<OperateFactory*>(getComponent<IOperateFactory>(COMPNAME_OperateFactory, IID_IOperateFactory));
	assert(pOperateFactory);
	
	vector<GrowUpPlan> growUpPlanList =  pOperateFactory->getGrowUpPlanList();
	
	GSProto::CMD_QUERY_GROWUP_DETAIL_SC scMsg;
	scMsg.set_bisbuy(m_growUpdata.bHavebuyGrowUp);
	
	for(size_t i = 0; i < growUpPlanList.size(); ++i)
	{
		const GrowUpPlan& planUnit = growUpPlanList[i];
		
		map<int,int>::iterator iter = m_growUpdata.haveGetReward.find(planUnit.iId);
		if(iter != m_growUpdata.haveGetReward.end())
		{
			continue;
		}

		GSProto::GrowupUnitDetail& growUpDetail = *(scMsg.add_szgrowupdetail());
		growUpDetail.set_iid(planUnit.iId);
		growUpDetail.set_igoallevel(planUnit.iGoal);
		growUpDetail.set_iaward(planUnit.iCanGetGold);
	}
	
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	pEntity->sendMessage(GSProto::CMD_QUERY_GROWUP_DETAIL, scMsg);
}

void OperateSystem::onQueryGrowUpPlanInfo(const GSProto::CSMessage& msg)
{
	PROFILE_MONITOR("onQueryGrowUpPlanInfo");
	
	OperateFactory* pOperateFactory = static_cast<OperateFactory*>(getComponent<IOperateFactory>(COMPNAME_OperateFactory, IID_IOperateFactory));
	assert(pOperateFactory);
		
	if(!m_growUpdata.bHavebuyGrowUp)
	{
		//是否过期
		if(time(0) > pOperateFactory->getOpenServerSecond() + ONE_WEEK_SECONDS)
		{
			return;
		}
	}
	
	sendGrowUpPlanInfo();
}

void OperateSystem::onBuyGrowUpPlan(const GSProto::CSMessage& msg)
{
	OperateFactory* pOperateFactory = static_cast<OperateFactory*>(getComponent<IOperateFactory>(COMPNAME_OperateFactory, IID_IOperateFactory));
	assert(pOperateFactory);
		
	if(m_growUpdata.bHavebuyGrowUp)
	{
		return;
	}

	//是否过期
	if(time(0) > pOperateFactory->getOpenServerSecond() + ONE_WEEK_SECONDS)
	{
		return;
	}

	//等级 VIP 限制

	IGlobalCfg *pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);
	
	int iVIPLevel = pGlobal->getInt("购买成长计划VIP等级", 3);

	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	int iActorVip = pEntity->getProperty(PROP_ACTOR_VIPLEVEL, 0);
	if(iActorVip < iVIPLevel )
	{
		//pEntity->sendMessage(GSProto::CMD_NOTICE_CLIENT_PUSHVIP_SCENE_COMMON);
		return;
	}

	//元宝足不足
	int iActorHave = pEntity->getProperty(PROP_ACTOR_GOLD, 0);
	int iNeedGold = pGlobal->getInt("购买成长计划元宝消耗", 200);

	if( iActorHave < iNeedGold)
	{
		pEntity->sendErrorCode(ERROR_NEED_GOLD);
		return;
	}

	//购买扣钱
	pEntity->changeProperty( PROP_ACTOR_GOLD, 0 - iNeedGold,GSProto::en_Reason_BUY_GROWUP_PLAN);
	
	m_growUpdata.bHavebuyGrowUp = true;
	
	sendGrowUpPlanInfo();
	
}

void OperateSystem::onGetGrowUpPlanReward(const GSProto::CSMessage& msg)
{
	GSProto::CMD_GET_GROWUP_REWARD_CS csMsg;
	if(!csMsg.ParseFromString(msg.strmsgbody()))
	{
		return;
	}

	int iId = csMsg.iid();
	
	map<int,int>::iterator iter = m_growUpdata.haveGetReward.find(iId);
	if(iter != m_growUpdata.haveGetReward.end())
	{
		return;
	}
		
	IEntity *pActor = getEntityFromHandle(m_hEntity);
	assert(pActor);
	
	OperateFactory* pOperateFactory = static_cast<OperateFactory*>(getComponent<IOperateFactory>(COMPNAME_OperateFactory, IID_IOperateFactory));
	assert(pOperateFactory);
	
	vector<GrowUpPlan> growUpPlanList =  pOperateFactory->getGrowUpPlanList();

	int iCanGetId  = -1;
	int iCanGetGold = -1;
	
	for(size_t i =0 ; i < growUpPlanList.size(); ++i)
	{
		const GrowUpPlan& planUnit = growUpPlanList[i];
		
		if(planUnit.iId ==iId)
		{
			int iLevel = pActor->getProperty(PROP_ENTITY_LEVEL, 1);
			if(iLevel < planUnit.iGoal)
			{
				return;
			}
			
			iCanGetId = planUnit.iId;
			iCanGetGold = planUnit.iCanGetGold;
			break;
		}
		
	}

	if(iCanGetId != -1)
	{
		//发奖
		pActor->changeProperty( PROP_ACTOR_GOLD,iCanGetGold,GSProto::en_Reason_GROWUPPLAN_GET);	
		//
		m_growUpdata.haveGetReward.insert(make_pair(iCanGetId,1));
		sendGrowUpPlanInfo();
	}
	
}


/*

void OperateSystem::onExeBuyActive (const GSProto::CSMessage& msg)
{
	GSProto::CMD_OPERATE_ACTIVE_BUY_CS csMsg;
	if(!csMsg.ParseFromString(msg.strmsgbody()))
	{
		return;
	}

	int iActiveId = csMsg.iactiveid();
	
	//买了没有
	bool bActiveBuy = activeHaveBuy(iActiveId);
	if(bActiveBuy )
	{
		return;
	}

	//能不能买
	
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	OperateFactory* pOperatorFactory = static_cast<OperateFactory*>(getComponent<IOperateFactory>(COMPNAME_OperateFactory, IID_IOperateFactory));
	assert(pOperatorFactory);

	OperateActive active;
	bool res = pOperatorFactory->getActiveById(iActiveId,active);
	if(!res) return;

	if(active.m_iType != GSProto::enActiveType_GrowUp)
	{
		return;
	}

	//买买买

	int iActorHave = pEntity->getProperty(PROP_ACTOR_GOLD, 0);
	int iNeedGold = active.iPrice;
	if(iNeedGold > iActorHave)
	{
		pEntity->sendErrorCode( ERROR_NEED_GOLD);
		return;
	}

	//扣钱
	pEntity->changeProperty( PROP_ACTOR_GOLD, 0-iNeedGold, GSProto::en_Reason_BUY_GROWUP_PLAN);
	m_ActiveData.activeIdList.insert(make_pair(iActiveId,true));

	//将任务打开存放进入任务列表
	*/
	
	
//}

void OperateSystem::checkIncheckReset()
{
	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	
	if(!pZoneTime->IsInSameMonth(m_CheckInData.dwLastSecond,pZoneTime->GetCurSecond()))
	{
		m_CheckInData.checkInList.clear();
		m_CheckInData.dwLastSecond = pZoneTime->GetCurSecond();
	}	
}

void OperateSystem::onQueryCheckInInfo(const GSProto::CSMessage& msg)
{
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	GSProto::CMD_OPERATESYSTEM_QUERY_CHECKIN_SC scMsg;
	
	int iHaveCheckInCount = m_CheckInData.checkInList.size();

	
		
	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	if(iHaveCheckInCount  == 0)
	{
		scMsg.set_ihavecheckincount(1);
		scMsg.set_blastcanclick(true);
		scMsg.set_ihaveclicktimes(0);
		pEntity->sendMessage(GSProto::CMD_OPERATESYSTEM_QUERY_CHECKIN,  scMsg);
		return;
	}
	const ServerEngine::TimeResetValue&  lastCheckIn = m_CheckInData.checkInList.back();
	if(!pZoneTime->IsInSameDay(lastCheckIn.dwLastChgTime, pZoneTime->GetCurSecond()))
	{
		scMsg.set_ihavecheckincount(iHaveCheckInCount+1);
		scMsg.set_blastcanclick(true);
		scMsg.set_ihaveclicktimes(0);
	}
	else
	{
		IOperateFactory *pOperateFac = getComponent<IOperateFactory>(COMPNAME_OperateFactory, IID_IOperateFactory);
		assert(pOperateFac);
		int iMonth = pZoneTime->GetMonthInYear(pZoneTime->GetCurSecond());
				
		CheckInUnit  unit;
		bool res = pOperateFac->checkInGetReward(iMonth, iHaveCheckInCount - 1, unit);
		assert(res);

		if(unit.bCanGetTwice)
		{
			if(lastCheckIn.iValue == 1)
			{
				scMsg.set_ihavecheckincount(iHaveCheckInCount);
				scMsg.set_blastcanclick(true);
				scMsg.set_ihaveclicktimes(1);
			}
			else
			{
				scMsg.set_ihavecheckincount(iHaveCheckInCount);
				scMsg.set_blastcanclick(false);
				scMsg.set_ihaveclicktimes(2);
			}
			
		}
		else
		{
			if(lastCheckIn.iValue == 1)
			{
				scMsg.set_ihavecheckincount(iHaveCheckInCount);
				scMsg.set_blastcanclick(false);
				scMsg.set_ihaveclicktimes(1);
			}
		}
		
	}
	
	pEntity->sendMessage(GSProto::CMD_OPERATESYSTEM_QUERY_CHECKIN,  scMsg);
	
}

void OperateSystem::onExeCheckIn(const GSProto::CSMessage& msg)
{
	GSProto::CMD_OPERATESYSTEM_CHECKIN_CS csMsg;
	if( !csMsg.ParseFromString(msg.strmsgbody()))
	{
		return;
	}

	int iCount = csMsg.iwillcheckinid();
	int iHaveCheckIn = m_CheckInData.checkInList.size();
	if(iCount < iHaveCheckIn)
	{
		return;
	}

	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	
	IOperateFactory *pOperateFac = getComponent<IOperateFactory>(COMPNAME_OperateFactory, IID_IOperateFactory);
	assert(pOperateFac);

	int iMonth = pZoneTime->GetMonthInYear(pZoneTime->GetCurSecond());
	
	int iActorVip = pEntity->getProperty( PROP_ACTOR_VIPLEVEL, 0);

	if(iCount == iHaveCheckIn)
	{
		ServerEngine::TimeResetValue&  lastCheckIn = m_CheckInData.checkInList.back();
		if( 2 <= lastCheckIn.iValue)
		{
			return;
		}

		if(!pZoneTime->IsInSameDay(lastCheckIn.dwLastChgTime, pZoneTime->GetCurSecond()))
		{
			return;
		}
			
		CheckInUnit  unit;
		bool res = pOperateFac->checkInGetReward(iMonth, iCount - 1, unit);
		assert(res);

		if(unit.bCanGetTwice)
		{
			if(iActorVip < unit.iVipNeed)
			{
				pEntity->sendMessage(GSProto::CMD_NOTICE_CLIENT_TOLEVELUP_VIP);
				return;
			}
			else
			{
				bool res = unit.reward(m_hEntity, 1);
				assert(res);
				lastCheckIn.iValue +=1;
				assert(lastCheckIn.iValue == 2);
				lastCheckIn.dwLastChgTime = time(0);
			}	
		}
		
		
	}
	else if(iCount == (iHaveCheckIn+1))
	{
		if(iHaveCheckIn > 0)
		{
			ServerEngine::TimeResetValue&  lastCheckIn = m_CheckInData.checkInList.back();
			if(pZoneTime->IsInSameDay(lastCheckIn.dwLastChgTime, pZoneTime->GetCurSecond()))
			{
				return;
			}
		}
		CheckInUnit  unit;
		bool res1 = pOperateFac->checkInGetReward(iMonth, iCount-1, unit);
		assert(res1);

		if(unit.bCanGetTwice)
		{
			if(iActorVip >= unit.iVipNeed)
			{
				bool res = unit.reward(m_hEntity, 2);
				assert(res);
				ServerEngine::TimeResetValue newCheckIn;
				newCheckIn.iValue = 2;
				newCheckIn.dwLastChgTime = time(0);
				m_CheckInData.checkInList.push_back(newCheckIn);
				checkNotice();
				return;
			}	
		}
		bool res = unit.reward(m_hEntity, 1);
		assert(res);

		ServerEngine::TimeResetValue newCheckIn;
		newCheckIn.iValue = 1;
		newCheckIn.dwLastChgTime = time(0);
		
		m_CheckInData.checkInList.push_back(newCheckIn);
		
	}

	checkNotice();
}


void OperateSystem::__resetDayPaymentActorData()
{
	IEntity*			playerE 		= getEntityFromHandle(m_hEntity);
	OperateFactory *	operateFactory	= static_cast<OperateFactory*>(getComponent<IOperateFactory>(COMPNAME_OperateFactory, IID_IOperateFactory));
	assert(playerE);
	assert(operateFactory);
	DayPayment* activeDayPayment = operateFactory->getActiveDayPayment();
	if(this->m_dayPaymentData.operateId != activeDayPayment->m_operateId)
	{
		this->m_dayPaymentData.operateId	= activeDayPayment->m_operateId;
		this->m_dayPaymentData.paied		= 0;
		this->m_dayPaymentData.fetched.clear();
	}
}


void OperateSystem::onQueryDayPaymentDetail(const GSProto::CSMessage& _msg)
{
	// 获取角色实体，工厂等
	IEntity* 			playerE 		= getEntityFromHandle(m_hEntity);
	OperateFactory * 	operateFactory	= static_cast<OperateFactory*>(getComponent<IOperateFactory>(COMPNAME_OperateFactory, IID_IOperateFactory));
	assert(playerE);
	assert(operateFactory);

	// 获取当前激活日充值活动(客户端正常逻辑不会发生为NULL的情况)
	DayPayment* activeDayPayment = operateFactory->getActiveDayPayment();
	if(activeDayPayment == NULL)
	{
		return;
	}

	// 重置&获取玩家累计充值量
	this->__resetDayPaymentActorData();
	UInt32 actorPaied = this->m_dayPaymentData.paied;

	// 返回数据到客户端
	GSProto::CMD_QUERY_DAY_PAYMENT_DETAIL_SC response;
	response.set_paied(actorPaied);
	for(size_t i = 0; i < activeDayPayment->m_rewardList.size(); i ++)
	{
		DayPaymentReward* reward 	= activeDayPayment->m_rewardList[i];
		GSProto::AccountUnit&  unit = *(response.add_rewardlist());
		unit.set_igoal(reward->m_goal);
		unit.set_iid(reward->m_rewardId);
	}
	map<taf::Int32, taf::Bool>::iterator it = this->m_dayPaymentData.fetched.begin();
	while(it != this->m_dayPaymentData.fetched.end())
	{
		int rewardId = (int)it->first;
		response.add_fetched(rewardId);
		it ++;
	}
	playerE->sendMessage(GSProto::CMD_QUERY_DAY_PAYMENT_DETAIL, response);
}


void OperateSystem::onGetDayPaymentReward(const GSProto::CSMessage& _msg)
{
	// 获取参数
	GSProto::CMD_GET_DAY_PAYMENT_REWARD_CS request;
	if(!request.ParseFromString(_msg.strmsgbody()))
	{
		return;
	}
	int rewardId = request.rewardid();

	// 获取角色实体
	IEntity* playerE = getEntityFromHandle(m_hEntity);
	OperateFactory * operateFactory	= static_cast<OperateFactory*>(getComponent<IOperateFactory>(COMPNAME_OperateFactory, IID_IOperateFactory));
	assert(playerE);
	assert(operateFactory);

	// 获取当前日累计充值活动
	DayPayment* activeDayPayment = operateFactory->getActiveDayPayment();
	if(activeDayPayment == NULL)
	{
		return;
	}

	// 获取对应的奖励数据
	DayPaymentReward* dayPaymentReward = activeDayPayment->getDayPaymentRewardByRewardId(rewardId);
	if(dayPaymentReward == NULL)
	{
		DEBUG_LOG(playerE)<<"can't find daypaymentreward data with reward id:"<<rewardId<<endl;
		return;
	}

	// 检查是否已领取
	map<taf::Int32, taf::Bool>::iterator it = this->m_dayPaymentData.fetched.find(taf::Int32(rewardId));
	if(it != this->m_dayPaymentData.fetched.end())
	{
		DEBUG_LOG(playerE)<<"reward:"<<rewardId<<" already fetched."<<endl;
		return;
	}

	// 检查累计充值数
	if(this->m_dayPaymentData.paied < dayPaymentReward->m_goal)
	{
		DEBUG_LOG(playerE)<<"not enough payment"<<endl;
	}

	// 兑换奖励
	GSProto::CMD_GET_DAY_PAYMENT_REWARD_SC response;
	GSProto::FightAwardResult& reward = *(response.mutable_reward());
	IDropFactory* dropFactory = getComponent<IDropFactory>(COMPNAME_DropFactory, IID_IDropFactory);
	assert(dropFactory);
	bool ret = dropFactory->calcDrop(dayPaymentReward->m_dropId, reward);
	assert(ret);
	dropFactory->excuteDrop(this->m_hEntity, reward, GSProto::en_Reason_Operate_DayPayment);

	// 生成领取记录
	this->m_dayPaymentData.fetched.insert(make_pair(rewardId, true));

	// 下行数据通知客户端
	playerE->sendMessage(GSProto::CMD_GET_DAY_PAYMENT_REWARD, response);
	
}

void OperateSystem::packSaveData(string& data)
{
	ServerEngine::OperateSystemData operateData;
	operateData.checkInData = m_CheckInData ;
	operateData.growUpdata = m_growUpdata;
	operateData.accPayMentData = m_accPayMentData;
	operateData.bHaveGetTelphoneSteam = m_bHaveGetTelphoneSteam;
	operateData.actorLoginData = m_actorLoginData;
	operateData.dayPaymentData = this->m_dayPaymentData;
	
	data = ServerEngine::JceToStr( operateData);
	
}





