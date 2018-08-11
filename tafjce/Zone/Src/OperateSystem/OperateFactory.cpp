#include "OperateSystemPch.h"
#include "OperateFactory.h"

extern "C" IComponent* createOperateFactory(Int32)
{
	return new OperateFactory;
}

extern int LifeAtt2Prop(int iLifeAtt);

#define GLOBAL_SERVER_OPENSECOND		"GLOBAL_SERVER_OPENSECOND"


//////////////////////////////////
OperateFactory::OperateFactory():
m_activeDayPayment(NULL)
{

}

OperateFactory::~OperateFactory()
{

}

bool OperateFactory::initlize(const PropertySet& propSet)
{

	loadCheckInData();
	loadGrowUpPlanData();
	loadAccumulatePayMentData();
	loadPhoneMap();
	loadAccumulateLoginInData();
	loadDayPaymentData();

	ITimerComponent* pTimeAxis = getComponent<ITimerComponent>(COMPNAME_TimeAxis, IID_ITimerComponent);
	assert(pTimeAxis);
	
	m_hInitTimeHandle = pTimeAxis->setTimer(this, 1 ,1000,"loadTimeData");
	
	return true;
}

void OperateFactory::loadAccumulateLoginInData()
{
	ITable *pTable = getCompomentObjectManager()->findTable(TABLENAME_AccumlateLogin);
	assert(pTable);
	int iRecord = pTable->getRecordCount();
	for(int i = 0; i < iRecord; ++i)
	{
		int iId = pTable->getInt(i, "累积登录天数");
		int iDropId = pTable->getInt(i, "奖励DropId");

		AccumlateLoginIn loginInUnit;
		loginInUnit.iId = iId;
		loginInUnit.iDropId = iDropId;
		
		m_AccLoginInList.push_back( loginInUnit );
	}
}

void OperateFactory::loadPhoneMap()
{
	ITable *pTable = getCompomentObjectManager()->findTable(TABLENAME_PhoneStream);
	assert(pTable);
	int iRecord = pTable->getRecordCount();
	for(int i = 0; i < iRecord; ++i)
	{
		string strPhoneNum =  pTable->getString(i, "号码段");
		string strCode = pTable->getString(i, "产品编码");
		m_phoneMap.insert(make_pair(strPhoneNum, strCode));
	}
}

bool OperateFactory::getStreamCode(string strPhone, string& productCode)
{
	map<string,string>::iterator iter = m_phoneMap.find(strPhone);
	if(iter != m_phoneMap.end())
	{
		productCode = iter->second;
		return true;
	}
	return false;
}


void OperateFactory::loadGrowUpPlanData()
{
	ITable *pTable = getCompomentObjectManager()->findTable(TABLENAME_GrowUpPlan);
	assert(pTable);
	int iRecord = pTable->getRecordCount();
	for(int i = 0; i < iRecord; ++i)
	{
		int iId = pTable->getInt(i, "计划ID");
		int iFinishLevel = pTable->getInt(i, "完成等级");
		int iCanGetGold = pTable->getInt(i, "奖励");

		GrowUpPlan plan;
		plan.iId = iId;
		plan.iGoal = iFinishLevel;
		plan.iCanGetGold = iCanGetGold;

		m_GrowUpPlanList.push_back( plan );
	}
}

void OperateFactory::loadAccumulatePayMentData()
{
	ITable *pTable = getCompomentObjectManager()->findTable(TABLENAME_AccumulatePayment);
	assert(pTable);
	int iRecord = pTable->getRecordCount();
	for(int i = 0; i < iRecord; ++i)
	{
		int iId = pTable->getInt(i, "累积充值ID");
		int ipayCount = pTable->getInt(i, "充值目标");
		int iDropId = pTable->getInt(i, "奖励DropId");


		AccumulatePayment payment;
		payment.iId = iId;
		payment.iGoal = ipayCount;
		payment.iDropId = iDropId;

		m_AccPayMentList.push_back( payment );
	}
}


void OperateFactory::loadDayPaymentData()
{
	ITable*	dayPaymentTable			= getCompomentObjectManager()->findTable(TABLENAME_DayPayment);
	ITable* dayPaymentRewardTable	= getCompomentObjectManager()->findTable(TABLENAME_DayPaymentReward);
	IZoneTime *zoneTime 			= getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(dayPaymentTable);
	assert(dayPaymentRewardTable);
	assert(zoneTime);

	// 加载DayPayment
	int recordCount = dayPaymentTable->getRecordCount();
	for(int i = 0; i < recordCount; i ++)
	{
		int 	operateId 		= dayPaymentTable->getInt(i, 	"活动id");
		string 	strStartTime	= dayPaymentTable->getString(i, "开始时间");
		string 	strEndTime		= dayPaymentTable->getString(i, "结束时间");
		Uint32	startTime		= zoneTime->StrTime2Seconds(strStartTime.c_str(),	"%Y/%m/%d");
		Uint32	endTime			= zoneTime->StrTime2Seconds(strEndTime.c_str(),		"%Y/%m/%d");

		DayPayment	dayPayment;
		dayPayment.m_operateId	= operateId;
		dayPayment.m_startTime	= startTime;
		dayPayment.m_endTime	= endTime;
		this->m_dayPaymentList[operateId] = dayPayment;
	}

	// 加载DayPaymentReward
	recordCount = dayPaymentRewardTable->getRecordCount();
	for(int i = 0; i < recordCount; i ++)
	{
		int 	operateId 		= dayPaymentRewardTable->getInt(i, 	"活动id");
		int		rewardId		= dayPaymentRewardTable->getInt(i,	"奖励id");
		int		goal			= dayPaymentRewardTable->getInt(i,	"充值目标");
		int		dropId			= dayPaymentRewardTable->getInt(i,	"奖励DropId");

		DayPaymentReward dayPaymentReward;
		dayPaymentReward.m_operateId	= operateId;
		dayPaymentReward.m_rewardId		= rewardId;
		dayPaymentReward.m_goal			= goal;
		dayPaymentReward.m_dropId		= dropId;
		this->m_dayPaymentRewardList.push_back(dayPaymentReward);
	}

	// 关联DayPaymentReward 与 DayPayment
	// 必须单独处理，否则m_dayPaymentRewardList内存不稳定，会出问题
	for(size_t i = 0; i < this->m_dayPaymentRewardList.size(); i ++)
	{
		DayPaymentReward* dayPaymentRewardPTR = & this->m_dayPaymentRewardList[i];
		map<int, DayPayment>::iterator it = this->m_dayPaymentList.find(dayPaymentRewardPTR->m_operateId);
		if(it != this->m_dayPaymentList.end())
		{
			DayPayment& dayPayment = it->second;
			dayPayment.m_rewardList.push_back(dayPaymentRewardPTR);
		}
	}

	// 计算当前生效日充值活动
	this->updateActiveDayPayment();

	// debug
	/*
	map<int, DayPayment>::iterator it = this->m_dayPaymentList.begin();
	while(it != this->m_dayPaymentList.end())
	{
		const DayPayment& dayPayment = it->second;
		printf(	"operate id:%i start time:%u end time:%u \n",
				dayPayment.m_operateId,
				dayPayment.m_startTime,
				dayPayment.m_endTime);
		printf(	"----------------------------------------------------\n");
		for(size_t i = 0; i < dayPayment.m_rewardList.size(); i ++)
		{
			DayPaymentReward* reward = dayPayment.m_rewardList[i];
			printf(	"[%i] operate id:%i reward id:%i goal:%i drop id:%i\n",
					i,
					reward->m_operateId,
					reward->m_rewardId,
					reward->m_goal,
					reward->m_dropId);
		}
		printf(	"----------------------------------------------------\n");
		it ++;	
	}
	*/
	// debug end
}


bool OperateFactory::updateActiveDayPayment(	DayPayment** _outOldDayPayment,
														DayPayment** _outNewDayPayment)
{
	
	Uint32 current = time(0);
	DayPayment*	oldDayPayment	= this->m_activeDayPayment;
	DayPayment*	newDayPayment	= this->m_activeDayPayment;

	// TODO: goto 让人又爱又恨,可以集中统一逻辑，但需要前置定义，到底用不用呢。
	//int fromId = -1;
	//int toId = -1;
	map<int, DayPayment>::iterator it = this->m_dayPaymentList.end();
	
	if(this->m_activeDayPayment != NULL)
	{
		// 初步检查,快速通过
		if(	current > this->m_activeDayPayment->m_startTime && current < this->m_activeDayPayment->m_endTime)
		{
			goto __END;
		}
	}

	// 失效，进行更新
	newDayPayment = NULL;
	it = this->m_dayPaymentList.begin();
	while(it != this->m_dayPaymentList.end())
	{
		DayPayment* dayPayment = & it->second;
		if(current > dayPayment->m_startTime && current < dayPayment->m_endTime)
		{
			newDayPayment = dayPayment;
			break;
		}
		it ++;
	}
	
__END:
	this->m_activeDayPayment = newDayPayment;
	if(_outOldDayPayment != NULL)	* _outOldDayPayment = oldDayPayment;
	if(_outNewDayPayment != NULL)	* _outNewDayPayment = newDayPayment;

	// debug
	/*
	if(oldDayPayment != NULL)	fromId 	= oldDayPayment->m_operateId;
	if(newDayPayment != NULL)	toId	= newDayPayment->m_operateId;
	printf(	"switch daypayment from %i to %i \n",
			fromId,
			toId);
	*/
	// debug end
	
	return (oldDayPayment != newDayPayment);
	
}


DayPayment* OperateFactory::getActiveDayPayment()
{
	return this->m_activeDayPayment;
}


/*
void OperateFactory::loadOperateActiveData()
{
	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	
	ITable *pTable = getCompomentObjectManager()->findTable(TABLENAME_OperateActive);
	assert(pTable);
	int iRecord = pTable->getRecordCount();
	for(int i = 0; i < iRecord; ++i)
	{
		int id = pTable->getInt(i, "活动ID");
		int iType = pTable->getInt(i,"活动类型");
		string strActiveOpenSecond = pTable->getString(i, "开始时间");
		string strActiveCloseSecond = pTable->getString(i, "结束时间");
		string strActiveCheckCondition = pTable->getString(i, "活动开放条件1");
		
		string strTaskIdList = pTable->getString(i, "任务ID");
		vector<int> taskVec = TC_Common::sepstr<int>(strTaskIdList, "#");
		
		int iResetFlag = pTable->getInt(i, "重置标记");
		
		string strOpenFunc = pTable->getString(i,"开启功能ID");
		vector<int> openFuncVec = TC_Common::sepstr<int>(strOpenFunc, "#");

		int iBuyFlag = pTable->getInt(i, "购买标记");
		int iPrice = pTable->getInt(i, "购买价格");
		
		OperateActive active;

		active.m_id = id;
		active.m_iType = iType;
		active.m_iBeginSecond = pZoneTime->StrTime2Seconds(strActiveOpenSecond.c_str(), "%Y/%m/%d");
		active.m_iEndSecond = pZoneTime->StrTime2Seconds(strActiveCloseSecond.c_str(), "%Y/%m/%d");
		active.m_bNeedReset = (iResetFlag ==1);
		active.m_TaskVec = taskVec;
		active.m_OpenFuncVec = openFuncVec;
		active.bBuyFlag =  (iBuyFlag == 1);
		active.iPrice = iPrice;

		
		active.m_OpenCheckPoint = parseActiveCondition(strActiveCheckCondition);
		assert(active.m_OpenCheckPoint);
		
		m_OperateActiveList.push_back(active);
	}
}



ActiveOpenConditionBase* OperateFactory::parseActiveCondition(const string& strCondition)
{
	vector<string> conditionList = TC_Common::sepstr<string>(strCondition, "#");
	assert(conditionList.size() == 2);

	if( conditionList[0] == "level")
	{
		return new ActiveOpenWithLevel( atoi(conditionList[1].c_str()));
	}
	else if(conditionList[0] == "vip" )
	{
		return new ActiveOpenWithVip( atoi(conditionList[1].c_str()) );
	}
	
	return NULL;
}

bool  ActiveOpenWithLevel::checkOpenCondition(HEntity hEntity) 
{
	IEntity *pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	int iActorLevel = pEntity->getProperty(PROP_ENTITY_LEVEL, 1);
	if(iActorLevel >= m_iNeedCondition)
	{
		return true;
	}
	
	return false;
}

bool  ActiveOpenWithVip::checkOpenCondition(HEntity hEntity) 
{
	IEntity *pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	int iActorVipLevel = pEntity->getProperty(PROP_ACTOR_VIPLEVEL, 0);

	if(iActorVipLevel >= m_iNeedCondition)
	{
		return true;
	}
	
	return false;
}*/


void  OperateFactory::loadCheckInData()
{
	ITable *pTable = getCompomentObjectManager()->findTable(TABLENAME_CheckIn);
	assert(pTable);
	int iRecord = pTable->getRecordCount();
	for(int i = 0; i < iRecord; ++i)
	{
		int iMouth = pTable->getInt(i, "月份");
		int iCount = pTable->getInt(i, "累积次数");
		int iTwiceFlag = pTable->getInt(i, "是否可以领双倍");
		int iVipLevel = pTable->getInt(i, "领双倍Vip等级");
		string strReward = pTable->getString(i,"奖励");

		CheckInUnit unit;

		unit.iMonth = iMouth;
		unit.iTotalTimes = iCount;
		unit.bCanGetTwice = (iTwiceFlag == 1);
		unit.iVipNeed = iVipLevel;
		
		unit.rewardBase = ParseReward(strReward);
		assert(unit.rewardBase);

		map<int, map<int, CheckInUnit> >::iterator iter = m_CheckInData.find(iMouth);

		if(iter == m_CheckInData.end())
		{
			map<int,CheckInUnit> tempMap;
			tempMap.insert(make_pair(iCount, unit));
			
			m_CheckInData.insert(make_pair(iMouth, tempMap));
			continue;
		}

		map<int, CheckInUnit>& mouthMap = iter->second;
		mouthMap.insert(make_pair(iCount, unit));
	}
	
}


CheckInReward_Base* OperateFactory::ParseReward(const string rewardStr)
{
	if(rewardStr.length()==0)
	return NULL;
	
	vector<string> paramList = TC_Common::sepstr<string>(rewardStr, Effect_MagicSep);
	
	if(paramList[0] == "LifeAtt")
	{
		int iLifeatt = atoi(paramList[1].c_str());
		int iCount = atoi(paramList[2].c_str());
		return new CheckInReward_LifeAtt(iLifeatt,iCount);
	}
	else if(paramList[0] == "Item")
	{
		int iItemId = atoi(paramList[1].c_str());
		int iCount = atoi(paramList[2].c_str());
		return new CheckInReward_Item(iItemId,iCount);
	}
	else if(paramList[0] == "Hero")
	{
		int iHeroId = atoi(paramList[1].c_str());
		int iLevelStep = atoi(paramList[2].c_str());
		return new CheckInReward_Hero(iHeroId,iLevelStep);
	}
	else
	{
		assert(0);
	}
	return NULL;
}

bool OperateFactory::checkInGetReward( int iMonth, int haveCheckInCount,  CheckInUnit& unit)
{
	map<int, map<int, CheckInUnit> >::iterator iter = m_CheckInData.find(iMonth);
	if(iter == m_CheckInData.end())
	{
		return false;
	}

	map<int, CheckInUnit>& monthMap = iter->second;
	map<int, CheckInUnit>::iterator monthMapIter = monthMap.find(haveCheckInCount+1);
	if(monthMapIter == monthMap.end())
	{
		return false;
	}

	unit = monthMapIter->second;
	
	return true;	
}


bool CheckInReward_LifeAtt::checkInReward(HEntity hEntity, int iDouble)
{
	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	GSProto::CMD_OPERATESYSTEM_CHECKIN_SC scMsg;
	
 	if( m_ilifeattID == GSProto::en_LifeAtt_Exp)
	{
		pEntity->addExp(m_iCount*iDouble);
	}
	else
	{
		int iPropID = LifeAtt2Prop(m_ilifeattID);
		assert(iPropID >= 0);
		pEntity->changeProperty(iPropID,m_iCount*iDouble,GSProto::en_Reason_CheckInReward);
	
	}

	GSProto::PropItem& prop =  *(scMsg.add_szawardproplist());
	prop.set_ilifeattid(m_ilifeattID);
	prop.set_ivalue(m_iCount *iDouble);


	pEntity->sendMessage(GSProto::CMD_OPERATESYSTEM_CHECKIN,  scMsg);
		
	return true;
}

bool CheckInReward_Item::checkInReward(HEntity hEntity, int iDouble)
{
	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	IItemSystem* pItemSystem = static_cast<IItemSystem*>(pEntity->querySubsystem(IID_IItemSystem));
	assert(pItemSystem);
	
	PLAYERLOG(pEntity)<<"addItem|"<<"|"<<GSProto::en_Reason_CheckInReward<<endl;
	bool res=pItemSystem->addItem(m_iItemId,m_iCount*iDouble,GSProto::en_Reason_CheckInReward);
	assert(res);
	GSProto::CMD_OPERATESYSTEM_CHECKIN_SC scMsg;
	GSProto::FightAwardItem& awardItem = *( scMsg.add_szawarditemlist());
	awardItem.set_iitemid(m_iItemId);
	awardItem.set_icount(m_iCount *iDouble);
	pEntity->sendMessage(GSProto::CMD_OPERATESYSTEM_CHECKIN,  scMsg);

	return true;
}

bool CheckInReward_Hero::checkInReward(HEntity hEntity, int iDouble)
{
	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	IHeroSystem* pHeroSystem = static_cast<IHeroSystem*>(pEntity->querySubsystem(IID_IHeroSystem));
	assert(pHeroSystem);

	GSProto::CMD_OPERATESYSTEM_CHECKIN_SC scMsg;

	int iCount = 0;
	int iHeroId = 0;
	int iHeroSoulId = 0;
	for(int i = 0; i < iDouble; ++i)
	{
		
		AddHeroReturn heroReturn(hEntity);
		bool resoult = pHeroSystem->addHeroWithLevelStep(m_iHeroId,m_iStepLevel,true,GSProto::en_Reason_CheckInReward);
		if(!resoult)
		{
			PLAYERLOG(pEntity)<<"[ERROR]"<<"|addHERO|"<<"|"<<GSProto::en_Reason_CheckInReward<<endl;
		}
		iCount = heroReturn.iSoulCount ;
		iHeroId = heroReturn.iHeroId;
		iHeroSoulId = heroReturn.iSoulId;
	}
	
	GSProto::DreamRewardHero& hero = *(scMsg.add_szawardherolist());
	hero.set_icount(iCount*iDouble);
	hero.set_iheroid( iHeroId );
	hero.set_iherosoulid(iHeroSoulId);
	pEntity->sendMessage(GSProto::CMD_OPERATESYSTEM_CHECKIN,  scMsg);
	
	return true;
}

void OperateFactory::onTimer(int nEventId)
{
	if(1 == nEventId)
	{
		loadTimeData();

		// 检测日充值活动的更新
		this->updateActiveDayPayment();
	}
}

unsigned int OperateFactory::getOpenServerSecond()
{
	IGlobalCfg *pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal );

	int iOpenServerAdmen = pGlobal->getInt("开服天数修正",0);
	int iAdmenSecond = ONE_DAY_SECONDS * iOpenServerAdmen;
	
	return m_openServerSecond.dwOpenServerSecond + iAdmenSecond;
}


void OperateFactory::loadTimeData()
{
	// xh modify : 	日充值活动需要改定时器长期运行，所以修改该代码，不再关闭定时器
	//				当然我也可以启一个新的定时器，这不是懒吗
	/*
	try
	{
		IJZMessageLayer* pMsgLayer = getComponent<IJZMessageLayer>(COMPNAME_MessageLayer, IID_IJZMessageLayer);
		assert(pMsgLayer);

		string strGlobalOpenServerData;
		int iRet = pMsgLayer->getGlobalData(GLOBAL_SERVER_OPENSECOND, strGlobalOpenServerData);
		if(ServerEngine::en_DataRet_OK == iRet)
		{
			ServerEngine::JceToObj(strGlobalOpenServerData, m_openServerSecond);
		}
		else if(ServerEngine::en_DataRet_NotExist == iRet)
		{
			m_openServerSecond.dwOpenServerSecond = time(0);
			strGlobalOpenServerData = ServerEngine::JceToStr(m_openServerSecond);
			pMsgLayer->setGlobalData(GLOBAL_SERVER_OPENSECOND, strGlobalOpenServerData);
		}
		else
		{
			assert(false);
		}

	
	}
	catch(...)
	{
		assert(false);
	}

	ITimerComponent* pTimeAxis = getComponent<ITimerComponent>(COMPNAME_TimeAxis, IID_ITimerComponent);
	assert(pTimeAxis);
	pTimeAxis->killTimer(m_hInitTimeHandle);
	*/

	static bool isInvoked = false;
	if(!isInvoked)
	{
		try
		{
			IJZMessageLayer* pMsgLayer = getComponent<IJZMessageLayer>(COMPNAME_MessageLayer, IID_IJZMessageLayer);
			assert(pMsgLayer);

			string strGlobalOpenServerData;
			int iRet = pMsgLayer->getGlobalData(GLOBAL_SERVER_OPENSECOND, strGlobalOpenServerData);
			if(ServerEngine::en_DataRet_OK == iRet)
			{
				ServerEngine::JceToObj(strGlobalOpenServerData, m_openServerSecond);
			}
			else if(ServerEngine::en_DataRet_NotExist == iRet)
			{
				m_openServerSecond.dwOpenServerSecond = time(0);
				strGlobalOpenServerData = ServerEngine::JceToStr(m_openServerSecond);
				pMsgLayer->setGlobalData(GLOBAL_SERVER_OPENSECOND, strGlobalOpenServerData);
			}
			else
			{
				assert(false);
			}

		
		}
		catch(...)
		{
			assert(false);
		}
		
		isInvoked = true;
	}
	// xh modify end

}


