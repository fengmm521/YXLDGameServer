#include "LegionPch.h"
#include "LegionFactory.h"
#include "LegionImp.h"
#include "util/tc_base64.h"
#include "LegionCampBattle.h"

#define GLOBAL_LEGION_CITY	"GLOBAL_LEGION_CITY"

extern "C" IComponent* createLegionFactory(Int32)
{
	return new LegionFactory;
}


LegionFactory::LegionFactory():m_bReady(false), /*m_iSumShopChance(0),*/ m_iCityFightStaus(GSProto::enStateNULL), m_iLastCityFightDay(0)
{
}

LegionFactory::~LegionFactory()
{
}

void GetLegionListCallback::callback_getLegionList(taf::Int32 ret,  const ServerEngine::LegionList& legionList)
{
	if(ret != ServerEngine::en_DataRet_OK)
	{
		SvrErrLog("callback_getLegionList Fail|%d", ret);
		assert(false);
		return;
	}

	m_pLegionFactory->initLegionList(legionList.legionList);
}

void GetLegionListCallback::callback_getLegionList_exception(taf::Int32 ret)
{
	SvrErrLog("getLegionList Exception|%d", ret);
	assert(false);
}
	

bool LegionFactory::initlize(const PropertySet& propSet)
{
	IJZMessageLayer* pJZMessageLayer = getComponent<IJZMessageLayer>(COMPNAME_MessageLayer, IID_IJZMessageLayer);
	assert(pJZMessageLayer);

	pJZMessageLayer->AsyncGetLegionList(new GetLegionListCallback(this) );

	loadLegionLevel();
	loadLegionContribute();
	loadLegionBless();
	loadLegionPayParam();
	//loadLegionShop();
	loadGiftCfg();

	ITimerComponent* pTimeAxis = getComponent<ITimerComponent>(COMPNAME_TimeAxis, IID_ITimerComponent);
	assert(pTimeAxis);
	
	m_hCheckHandle = pTimeAxis->setTimer(this, 1, 120000, "LegionCleanCheck");
	
	// 初始化军团领地数据
	initlizeLegionCity();
	m_hGlobalSaveHanle = pTimeAxis->setTimer(this, 2, 60000, "LegionCitySave");
	m_hCheckFightHandle = pTimeAxis->setTimer(this, 3, 2000, "LegionCityFightCheck");

	m_hSaveThreeContributeHandle = pTimeAxis->setTimer(this, 4, 60*5*1000, "LegionSaveThreeDayContribute");

	return true;
}

void LegionFactory::loadGiftCfg()
{
	ITable* pGiftTb = getCompomentObjectManager()->findTable("Gift");
	assert(pGiftTb);

	int iRecordCount = pGiftTb->getRecordCount();
	for(int i = 0; i < iRecordCount; i++)
	{
		GiftCfg tmpGift;
		tmpGift.iGiftID = pGiftTb->getInt(i, "ID");
		string strDropIDList = pGiftTb->getString(i, "奖励ID");
		tmpGift.dropIDList = TC_Common::sepstr<int>(strDropIDList, "#");

		m_mapGiftCfg[tmpGift.iGiftID] = tmpGift;
	}
}


void LegionFactory::initlizeLegionCity()
{
	// 读取远端存档数据
	try
	{
		printf("%s\n", "test--------------1");
		IJZMessageLayer* pJZMessageLayer = getComponent<IJZMessageLayer>(COMPNAME_MessageLayer, IID_IJZMessageLayer);
		assert(pJZMessageLayer);
		printf("%s\n", "test--------------2");
		// printf("%s\n", );
		// 同步读取
		string strGlobalCityData;
		int iGetDataRet = pJZMessageLayer->getGlobalData(GLOBAL_LEGION_CITY, strGlobalCityData);
		printf("%s\n", "test--------------3");
		ServerEngine::GlobalLegionCityData tmpGlobalLegionCityData;
		if(iGetDataRet == ServerEngine::en_DataRet_OK)
		{
			ServerEngine::JceToObj(strGlobalCityData, tmpGlobalLegionCityData);
		}
		else if(iGetDataRet == ServerEngine::en_DataRet_SysError)
		{
			assert(false);
		}
		printf("%s\n", "test--------------4");
		m_iLastCityFightDay = tmpGlobalLegionCityData.iLastDayNO;
		m_cityCampFightLog = tmpGlobalLegionCityData.cityCampFightLog ;
		m_curCityFightLog = tmpGlobalLegionCityData.curCityFightLog ;
		// 读取配置，初始化
		ITable* pTable = getCompomentObjectManager()->findTable("LegionCity");
		assert(pTable);
		printf("%s\n", "test--------------5");
		int iRecordCount = pTable->getRecordCount();
		for(int i = 0; i < iRecordCount; i++)
		{
			LegionCityCfg tmpCfg;
			tmpCfg.iCityID = pTable->getInt(i, "领地ID");
			tmpCfg.strCityName = pTable->getInt(i, "领地名字");
			tmpCfg.iSilver = pTable->getInt(i, "俸禄");
			tmpCfg.strNpcLegionName = pTable->getString(i, "NPC军团名");
			tmpCfg.strLeaderName = pTable->getString(i, "团长名字");
			tmpCfg.iLegionLevel = pTable->getInt(i, "军团等级");

			tmpCfg.iReportCost = pTable->getInt(i,"报名消耗");
			tmpCfg.iLegionIcon = pTable->getInt(i, "军团ICON");

			m_legionCityCfgMap[tmpCfg.iCityID] = tmpCfg;

			// 初始化领地对象
			initCityObj(tmpGlobalLegionCityData, tmpCfg);
		}
		printf("%s\n", "test--------------6");
		IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
		assert(pGlobalCfg);
	
		string strHourRange = pGlobalCfg->getString("领地争夺战秒", "12#20#22");
		vector<double> hourRangeList = TC_Common::sepstr<double>(strHourRange, "#");
		assert(hourRangeList.size() == 3);

		m_iReportBeginSecond = hourRangeList[0];// * 3600;
		m_iFightBeginSecond = hourRangeList[1]; //* 3600;
		m_iFightEndSecond = hourRangeList[2]; //* 3600;
	
	}
	catch(...)
	{
		assert(false);
	}
}

void LegionFactory::initCityObj(const ServerEngine::GlobalLegionCityData& globalCityData, const LegionCityCfg& cityCfg)
{
	ServerEngine::LegionCity curData;
	ServerEngine::LegionCity hisData;
	

	curData.iCityID = cityCfg.iCityID;
	hisData.iCityID = cityCfg.iCityID;

	if(globalCityData.lastLegionCityData.find(cityCfg.iCityID) != globalCityData.lastLegionCityData.end() )
	{
		hisData = globalCityData.lastLegionCityData.at(cityCfg.iCityID);
	}

	if(globalCityData.curLegionCityData.find(cityCfg.iCityID) != globalCityData.curLegionCityData.end() )
	{
		curData = globalCityData.curLegionCityData.at(cityCfg.iCityID);
	}
	else
	{
		makeCityInitData(cityCfg, curData);
	}
	
	LegionCampBattle* pLegionCityImp = new LegionCampBattle(this);
	assert(pLegionCityImp);

	if(!pLegionCityImp->initFromData(hisData, curData) )
	{
		assert(false);
	}

	pLegionCityImp->initCityCfgData(cityCfg);

	m_legionCampBattleMap[cityCfg.iCityID] = pLegionCityImp;
	m_ReportLegionMap = globalCityData.iRoportCampMap;
}

void LegionFactory::makeCityInitData(const LegionCityCfg& cityCfg, ServerEngine::LegionCity& newCityDaya)
{
	newCityDaya.iCityID = cityCfg.iCityID;
	newCityDaya.strOwnerLegion = cityCfg.strNpcLegionName;
	newCityDaya.bNpcHold = true;

	// 军团状态数据
	//newCityDaya.legionStateList[cityCfg.strNpcLegionName].iAliveCount = (int)cityCfg.monsterGrpList.size();
	
}

const LegionCityCfg* LegionFactory::queryCityCfg(int iCityID)
{
	map<int, LegionCityCfg>::iterator it = m_legionCityCfgMap.find(iCityID);
	if(it == m_legionCityCfgMap.end() )
	{
		return NULL;
	}

	return &(it->second);
}


void LegionFactory::onTimer(int nEventId)
{
	if(!isReady() ) return;

	if(1 == nEventId)
	{
		//checkCleanLegion();
	}
	else if(2 == nEventId)
	{
		saveGlobalCityData(false);
	}
	else if(3 == nEventId)
	{
		onCheckLegionCityFight();
	}
	else if(4 == nEventId)
	{
		onSaveThreeDayContribute();
	}
}

void LegionFactory::onSaveThreeDayContribute()
{
	IZoneTime* pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	
	if(m_mapLegion.size() == 0) return;
	map<string, ILegion*>::iterator it = m_mapLegion.begin();
	
	GSLegionImp* pLegionImp = static_cast<GSLegionImp*>(it->second);
	const ServerEngine::LegionData& legionData = pLegionImp->getLegionData();
	const ServerEngine::TimeResetValue& value = legionData.memberInfo.saveContributeVecValue;
	
	if(pZoneTime->IsInSameDay(value.dwLastChgTime, pZoneTime->GetCurSecond() ) )
	{
		return;
	}
	
	for(; it != m_mapLegion.end(); ++it)
	{
		GSLegionImp* pLegionImpTemp = static_cast<GSLegionImp*>(it->second);
		assert(pLegionImpTemp);
		pLegionImpTemp->saveThreeDayContribute();
	}
	
}


void LegionFactory::onCheckLegionCityFight()
{

	//开始报名
	int iRemaind = 0;
	
	unsigned int status = getCampBattleStatus(iRemaind);
		
	switch( m_iCityFightStaus)
	{
		case GSProto::enStateNULL:
			{
				
				if(status == GSProto::enStateInReport)
				{
					//清理数据准备开始报名

					
					IZoneTime* pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
					assert(pZoneTime);
					int iCurDayNO = pZoneTime->GetChineseDay(pZoneTime->GetCurSecond() );
					if(m_iLastCityFightDay < iCurDayNO )
					{
							cleanCityDataBeforeReport();
							
					}
					m_iCityFightStaus = GSProto::enStateInReport;
				}
				
				
			}break;

		case GSProto::enStateInReport:
			{
				if(status == GSProto::enStateInBattle)
				{
					//分配战斗数据
					IZoneTime* pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
					assert(pZoneTime);
					int iCurDayNO = pZoneTime->GetChineseDay(pZoneTime->GetCurSecond() );
					if(m_iLastCityFightDay < iCurDayNO )
					{
						pairCityBattle();
					}
					
					m_iCityFightStaus = GSProto::enStateInBattle;
				}
				
			}break;
		
		case GSProto::enStateInBattle:
		{
				if(status == GSProto::enStateAfterBattle)
				{

					//战斗结束 修改数据
					IZoneTime* pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
					assert(pZoneTime);
					int iCurDayNO = pZoneTime->GetChineseDay(pZoneTime->GetCurSecond() );
					if(m_iLastCityFightDay < iCurDayNO )
					{
						cityBattleOver();
					}
					m_iLastCityFightDay = iCurDayNO;
					
					m_cityCampFightLog.lastFightLogMap.clear();
					m_cityCampFightLog.lastFightLogDetail.clear();
					
					m_cityCampFightLog = m_curCityFightLog;
					
					m_curCityFightLog.lastFightLogMap.clear();
					m_curCityFightLog.lastFightLogDetail.clear();
					
					
					m_iCityFightStaus = GSProto::enStateAfterBattle;
				}
				
		}break;

		case GSProto::enStateAfterBattle:
		{
				if(status == GSProto::enStateBeforReport)
				{
					m_iCityFightStaus = GSProto::enStateNULL;
				}
		}break;
		
	}
	
}

void LegionFactory::cleanCityDataBeforeReport()
{
	for(map<int, LegionCampBattle*>::iterator it = m_legionCampBattleMap.begin(); it != m_legionCampBattleMap.end(); it++)
	{
		LegionCampBattle* pLegionCityImp = it->second;
		assert(pLegionCityImp);

		pLegionCityImp->cleanDataBeforeReport();
	}

	m_curCityFightLog.lastFightLogMap.clear();
	m_curCityFightLog.lastFightLogMap.clear();
	
}

void LegionFactory::pairCityBattle()
{
	for(map<int, LegionCampBattle*>::iterator it = m_legionCampBattleMap.begin(); it != m_legionCampBattleMap.end(); it++)
	{
		LegionCampBattle* pLegionCityImp = it->second;
		assert(pLegionCityImp);

		pLegionCityImp->pairBattle();
	}
}
	
void LegionFactory::cityBattleOver()
{
	for(map<int, LegionCampBattle*>::iterator it = m_legionCampBattleMap.begin(); it != m_legionCampBattleMap.end(); it++)
	{
		LegionCampBattle* pLegionCityImp = it->second;
		assert(pLegionCityImp);

		pLegionCityImp->battleOver();
	}
	m_ReportLegionMap.clear();
	
}

void LegionFactory::gmStartFight()
{
//	for(map<int, LegionCityImp*>::iterator it = m_legionCityMap.begin(); it != m_legionCityMap.end(); it++)
//	{
//		LegionCityImp* pCityImp = it->second;
//		assert(pCityImp);
//		pCityImp->startFight();
//	}
}


void LegionFactory::saveGlobalCityData(bool bSync)
{
	ServerEngine::GlobalLegionCityData legionCityData;
	legionCityData.iLastDayNO = m_iLastCityFightDay;
	legionCityData.iRoportCampMap = m_ReportLegionMap;
	legionCityData.cityCampFightLog = m_cityCampFightLog;
	legionCityData.curCityFightLog = m_curCityFightLog;
	
	for(map<int, LegionCampBattle*>::iterator it = m_legionCampBattleMap.begin(); it != m_legionCampBattleMap.end(); it++)
	{
		LegionCampBattle* pLegionCityImp = it->second;
		assert(pLegionCityImp);

		pLegionCityImp->packSaveData(legionCityData);
	}

	IJZMessageLayer* pJZMessageLayer = getComponent<IJZMessageLayer>(COMPNAME_MessageLayer, IID_IJZMessageLayer);
	assert(pJZMessageLayer);	

	string strLegionGlobalData = ServerEngine::JceToStr(legionCityData);
	if(bSync)
	{
		pJZMessageLayer->setGlobalData(GLOBAL_LEGION_CITY, strLegionGlobalData);
	}
	else
	{
		pJZMessageLayer->AsyncSetGlobalData(new GlobalLegionDataSaveCallback, GLOBAL_LEGION_CITY, strLegionGlobalData);
	}
}

void LegionFactory::checkCleanLegion()
{
	if(isLegionCityBattle() ) return;

	IZoneTime* pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);

	const Uint32 iWeekSecond = 24*3600*7;

	for(map<string, ILegion*>::iterator it = m_mapLegion.begin(); it != m_mapLegion.end();)
	{
		GSLegionImp* pLegionImp = static_cast<GSLegionImp*>(it->second);
		assert(pLegionImp);
		
		if( (pZoneTime->GetCurSecond() - pLegionImp->getLastLoginTime() ) >= iWeekSecond)
		{
			_destroy(it->second);
			m_mapLegion.erase(it++);
		}
		else
		{
			it++;
		}
	}
}


void GlobalLegionDataSaveCallback::callback_setGlobalData(taf::Int32 ret)
{
	FDLOG("Legion")<<"setGlobalData|Ret|"<<ret<<endl;
}

void GlobalLegionDataSaveCallback::callback_setGlobalData_exception(taf::Int32 ret)
{
	FDLOG("Legion")<<"setGlobalData|Exception|"<<ret<<endl;
}


void LegionFactory::removeLegion(ILegion* pLegion)
{
	assert(pLegion);
	map<string, ILegion*>::iterator it = m_mapLegion.find(pLegion->getLegionName() );
	if(it == m_mapLegion.end() )
	{
		return;
	}

	string strLegionName = pLegion->getLegionName();

	_destroy(it->second);
	m_mapLegion.erase(it);
}

void DelLegionCallback::callback_delLegion(taf::Int32 ret)
{
	FDLOG("Legion")<<"DelLegion|"<<ret<<"|"<<m_strName<<endl;
}

void DelLegionCallback::callback_delLegion_exception(taf::Int32 ret)
{
	FDLOG("Legion")<<"DelLegionException|"<<ret<<"|"<<m_strName<<endl;
}


bool LegionFactory::isLegionCityBattle()
{
	if(m_iCityFightStaus == GSProto::enStateInReport || m_iCityFightStaus==GSProto::enStateInBattle)
	{
		return true;
	}
	
	return false;
}

int LegionFactory::getFightWaitTime(bool noCheckToday)
{
 	int remaindSecond = 0;
	int iStaus = getCampBattleStatus(remaindSecond);
	return iStaus;
}

void LegionFactory::sendCityOwnerInfo(HEntity hActor)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

	string strName = pActor->getProperty(PROP_ENTITY_NAME, "");
	GSLegionImp* pLegion  = static_cast<GSLegionImp*>(getActorLegion(strName) );
	if(!pLegion) return;

	/*GSProto::CMD_QUERY_BATTLEOWNER_SC scMsg;
	for(map<int, LegionCityImp*>::iterator it = m_legionCityMap.begin(); it != m_legionCityMap.end(); it++)
	{
		LegionCityImp* pCityImp = it->second;
		pCityImp->fillScCityOwnerInfo(hActor, scMsg.mutable_szcitylist()->Add() );
	}

	pActor->sendMessage(GSProto::CMD_QUERY_BATTLEOWNER, scMsg);*/
}


void LegionFactory::sendQueryBattleMsg(HEntity hActor)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

	GSProto::CMD_QUERY_LEGION_CAMPBATTLE_SC scMsg;

	//add campList
	fillCampListInfo(hActor, scMsg);
	//status
	int iRemaindSecond = 0;
	int iCampId = getLegionReportCampId(hActor);
	
	int iStatus = m_iCityFightStaus;
	
	scMsg.set_istatus(iStatus);

	/*enStateBeforReport = 1;	enStateAfterReport = 2;	enStateInBattle = 3;	enStateAfterBattle = 4;*/
	if(GSProto::enStateInReport==iStatus || GSProto::enStateInBattle == iStatus)
	{
		getCampBattleStatus(iRemaindSecond);
		scMsg.set_iremaidsecond(iRemaindSecond);
		if(-1 != iCampId)
		{
			scMsg.set_ihavereportid(iCampId);
		}
	}

	pActor->sendMessage(GSProto::CMD_QUERY_LEGION_CAMPBATTLE, scMsg);
	
}

void LegionFactory::timeResetCampReport()
{
	
}

void LegionFactory::getCampBattleDetail(int& iStaus, int& iRemaindSecond/*=0*/)
{
	iStaus = getCampBattleStatus(iRemaindSecond);
}

int LegionFactory::getLegionReportCampId(HEntity hEntity)
{
	IEntity* pActor = getEntityFromHandle(hEntity);
	if(!pActor) return -1;

	string strName = pActor->getProperty(PROP_ENTITY_NAME, "");
	GSLegionImp* pLegion  = static_cast<GSLegionImp*>(getActorLegion(strName) );
	if(!pLegion) return -1;

	return getReportCity(pLegion->getLegionName());
}


int LegionFactory::getCampBattleStatus(int& iRemaindSecond)
{
	IZoneTime* pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);

	int iDayInWeek = pZoneTime->GetDayInWeek(pZoneTime->GetCurSecond() );

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	string strDayList = pGlobalCfg->getString("领地争夺战日期", "3#6");
	vector<int> dayList = TC_Common::sepstr<int>(strDayList, "#");
		
	int iReportBeginSecond = m_iReportBeginSecond;
	int iFightBeginSecond = m_iFightBeginSecond; //* 3600;
	int iFightEndSecond = m_iFightEndSecond; //* 3600;

	// 判断是否要计算今天
	//int iCurDayNO = pZoneTime->GetChineseDay(pZoneTime->GetCurSecond() );
	
	int iSecondInDay = (int)(pZoneTime->GetCurSecond() - pZoneTime->GetDayBeginSecond(pZoneTime->GetCurSecond() ) );
	//int iDistanceDay = 0;
	vector<int>::iterator it = find(dayList.begin(), dayList.end(), iDayInWeek);
	if(it != dayList.end())
	{
		if(iSecondInDay < iReportBeginSecond)
		{
			return GSProto::enStateBeforReport;
		}
		else if(iSecondInDay >=iReportBeginSecond &&   iSecondInDay < iFightBeginSecond)
		{
			iRemaindSecond = iFightBeginSecond - iSecondInDay;
			return GSProto::enStateInReport;
		}
		else if(iSecondInDay >= iFightBeginSecond && iSecondInDay < iFightEndSecond) 
		{
			return  GSProto::enStateInBattle;
		}
		return GSProto::
enStateAfterBattle;
	}
	else
	{
		return  GSProto::
enStateNoBattle;
	}
	
}


void LegionFactory::fillCampListInfo(HEntity hEntity, GSProto::CMD_QUERY_LEGION_CAMPBATTLE_SC& scMsg)
{

	int iLegionReportCampId = getLegionReportCampId(hEntity);
		
	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	int iReportLimit = pGlobalCfg->getInt("领地争夺战最大宣战数",3);

	IEntity* pActor = getEntityFromHandle(hEntity);
	if(!pActor) return;

	string strName = pActor->getProperty(PROP_ENTITY_NAME, "");
	GSLegionImp* pLegion  = static_cast<GSLegionImp*>(getActorLegion(strName) );
	if(!pLegion) return;
	
	string strLegionName = pLegion->getLegionName();
	for(map<int, LegionCampBattle*>::iterator it = m_legionCampBattleMap.begin(); it != m_legionCampBattleMap.end(); it++)
	{
		LegionCampBattle* pLegionCampBattle = it->second;
		int iReportSize = pLegionCampBattle->getCurReportLegionCount();
		int iCityId = pLegionCampBattle->iGetCampBattleId();
		string strOwerName = pLegionCampBattle->strOwerName();
		
		GSProto::CampUnit& unit = *(scMsg.add_szcamplist());
		unit.set_iid(iCityId);
		unit.set_strowername(strOwerName);

		
		bool bCanReport = false;
		if((iReportSize < iReportLimit) && (GSProto::enStateInReport == m_iCityFightStaus) && (-1==iLegionReportCampId) && (strLegionName != strOwerName)  )
		{
			bCanReport = true;
			unit.set_bcanreport(bCanReport);
		}
		
		
		if((GSProto::enStateInBattle== m_iCityFightStaus ) && (-1!=iLegionReportCampId)  )
		{
		 	if(iLegionReportCampId == it->first)
		 	{
		 		unit.set_bcanbattle(true);
		 	}
		}

		if((strLegionName == strOwerName) && (GSProto::enStateInBattle== m_iCityFightStaus ))
		{
			unit.set_bcanbattle(true);
		}
	}
	
}

void LegionFactory::sendQueryLegionReportInfo(HEntity hActor, int iQueryCityID)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

	string strName = pActor->getProperty(PROP_ENTITY_NAME, "");
	GSLegionImp* pLegion  = static_cast<GSLegionImp*>(getActorLegion(strName) );
	if(!pLegion) return;

	/*GSProto::CMD_QUERY_LEGIONBATTLE_REPORT_SC scMsg;
	if(iQueryCityID > 0)
	{
		map<int, LegionCityImp*>::iterator it = m_legionCityMap.find(iQueryCityID);
		if(it == m_legionCityMap.end() )
		{
			return;
		}

		LegionCityImp* pLegionCity = it->second;
		assert(pLegionCity);
		pLegionCity->fillQueryReportInfo(pLegion->getLegionName(), scMsg.mutable_szreportstatelist()->Add() );

		pActor->sendMessage(GSProto::CMD_QUERY_LEGIONBATTLE_REPORT, scMsg);
		return;
	}
	
	for(map<int, LegionCityImp*>::iterator it = m_legionCityMap.begin(); it != m_legionCityMap.end(); it++)
	{
		LegionCityImp* pLegionCity = it->second;
		assert(pLegionCity);
		
		pLegionCity->fillQueryReportInfo(pLegion->getLegionName(), scMsg.mutable_szreportstatelist()->Add() );
	}
	
	pActor->sendMessage(GSProto::CMD_QUERY_LEGIONBATTLE_REPORT, scMsg);*/
}


void LegionFactory::sendCurBattlePage(HEntity hActor)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

	string strName = pActor->getProperty(PROP_ENTITY_NAME, "");
	GSLegionImp* pLegion  = static_cast<GSLegionImp*>(getActorLegion(strName) );
	if(!pLegion) return;

	if(!isLegionCityBattle() )
	{
		pActor->sendErrorCode(ERROR_LEGIONFIGHT_NOSTART);
		return;
	}

	int iReportedCityID = getReportCity(pLegion->getLegionName() );
	if(iReportedCityID == 0)
	{
		pActor->sendErrorCode(ERROR_NO_REPORTCITY);
		return;
	}

	//LegionCityImp* pCityImp = getCityImp(iReportedCityID);
	//if(!pCityImp) return;

	//GSProto::CMD_QUERY_CURBATTLE_PAGE_SC scMsg;
	//pCityImp->fillScBattlePage(hActor, scMsg.mutable_battlepage() );

	//pActor->sendMessage(GSProto::CMD_QUERY_CURBATTLE_PAGE, scMsg);
}


void LegionFactory::sendSelfBattleRecord(HEntity hActor)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

	string strName = pActor->getProperty(PROP_ENTITY_NAME, "");
	GSLegionImp* pLegion  = static_cast<GSLegionImp*>(getActorLegion(strName) );
	if(!pLegion) return;

	if(!isLegionCityBattle() )
	{
		pActor->sendErrorCode(ERROR_LEGIONFIGHT_NOSTART);
		return;
	}

	int iReportedCityID = getReportCity(pLegion->getLegionName() );
	if(iReportedCityID == 0)
	{
		pActor->sendErrorCode(ERROR_NO_REPORTCITY);
		return;
	}

//	LegionCityImp* pCityImp = getCityImp(iReportedCityID);
//	if(!pCityImp) return;

	//GSProto::CMD_QUERY_CURBATTLE_SELFRECORD_SC scMsg;
	//pCityImp->fillSelfRecord(hActor, scMsg.mutable_szselfrecord() );

	//pActor->sendMessage(GSProto::CMD_QUERY_CURBATTLE_SELFRECORD, scMsg);
}


void LegionFactory::sendCurHonorRank(HEntity hActor)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

	string strName = pActor->getProperty(PROP_ENTITY_NAME, "");
	GSLegionImp* pLegion  = static_cast<GSLegionImp*>(getActorLegion(strName) );
	if(!pLegion) return;

	if(!isLegionCityBattle() )
	{
		pActor->sendErrorCode(ERROR_LEGIONFIGHT_NOSTART);
		return;
	}

	int iReportedCityID = getReportCity(pLegion->getLegionName() );
	if(iReportedCityID == 0)
	{
		pActor->sendErrorCode(ERROR_NO_REPORTCITY);
		return;
	}

//	LegionCityImp* pCityImp = getCityImp(iReportedCityID);
//	if(!pCityImp) return;

	//GSProto::CMD_QUERY_CURBATTLE_HONORRANK_SC scMsg;
	//pCityImp->fillHonorRank(hActor, scMsg.mutable_szhornorrank() );

	//pActor->sendMessage(GSProto::CMD_QUERY_CURBATTLE_HONORRANK, scMsg);
}

void LegionFactory::sendHisHonorRank(HEntity hActor, int iCityID)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

//	LegionCityImp* pCityImp = getCityImp(iCityID);
//	if(!pCityImp) return;

//	GSProto::CMD_QUERY_LASTBATTLE_HONORRANK_SC scMsg;
//	pCityImp->fillHisHonorRank(hActor, scMsg.mutable_szhornorrank() );

//	pActor->sendMessage(GSProto::CMD_QUERY_LASTBATTLE_HONORRANK, scMsg);
}

void LegionFactory::sendHisSelfBattleRecord(HEntity hActor, int iCityID)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;
	
	//LegionCityImp* pCityImp = getCityImp(iCityID);
	//if(!pCityImp) return;

//	GSProto::CMD_QUERY_LASTBATTLE_SELFRECORD_SC scMsg;
//	pCityImp->fillSelfHisRecord(hActor, scMsg.mutable_szselfrecord() );

//	pActor->sendMessage(GSProto::CMD_QUERY_LASTBATTLE_SELFRECORD, scMsg);
}


void LegionFactory::sendHisBattleRecord(HEntity hActor, int iCityID, int iPage)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

//	LegionCityImp* pCityImp = getCityImp(iCityID);
//	if(!pCityImp) return;

	////GSProto::CMD_QUERY_LASTBATTLE_RECORD_PAGE_SC scMsg;
	//pCityImp->fillHisBattleRecordByPage(hActor, iPage, scMsg);

	//pActor->sendMessage(GSProto::CMD_QUERY_LASTBATTLE_RECORD_PAGE, scMsg);
}


void LegionFactory::reportFightCity(HEntity hActor, int iCityID)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

	string strName = pActor->getProperty(PROP_ENTITY_NAME, "");
	GSLegionImp* pLegion  = static_cast<GSLegionImp*>(getActorLegion(strName) );
	if(!pLegion) return;

	// 权限验证
	int iOccupation = pLegion->getOccupation(strName);
	if( (iOccupation != GSProto::en_LegionOccupation_Leader) && (iOccupation != GSProto::en_LegionOccupation_ViceLeader) )
	{
		pActor->sendErrorCode(ERROR_NO_REPORTRIGHT);
		return;
	}

	// 判断时间是否正确
	if(GSProto::enStateInReport != m_iCityFightStaus)
	{
		pActor->sendErrorCode(ERROR_LEGION_NOT_INREPORT);
		return;
	}

	// 是否已经报名了
	int iReportedCity = getReportCity(pLegion->getLegionName() );
	if(iReportedCity > 0)
	{
		pActor->sendErrorCode(ERROR_HAVE_REPORT);
		return;
	}

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	int iCondLv = pGlobalCfg->getInt("军团战等级", 3);

	// 是否到3级了
	if(pLegion->getLevel() < iCondLv)
	{
		pActor->sendErrorCode(ERROR_REPORT_LEVEL);
		return;
	}

	LegionCampBattle* pCampBattle = getCampBattle(iCityID);
	if(!pCampBattle) return;

	if(!pCampBattle->reportCampBattle(hActor, pLegion->getLegionName() ) )
	{
		return;
	}
	
	m_ReportLegionMap.insert(make_pair(pLegion->getLegionName() ,iCityID));
	// 刷新客户端

	pCampBattle->sendCampBattleInfo(hActor);

}

void LegionFactory::onQueryCampBattleInfo(HEntity hActor, int iCityID)
{
	
	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;
	
	LegionCampBattle* pCampBattle = getCampBattle(iCityID);
	if(!pCampBattle) return;

	//返回消息给客户端
	pCampBattle->sendCampBattleInfo(hActor);
}

void LegionFactory::onJoinCampBattle(HEntity hActor, int iCityID)
{
	
	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

	string strName = pActor->getProperty(PROP_ENTITY_NAME, "");
	GSLegionImp* pLegion  = static_cast<GSLegionImp*>(getActorLegion(strName) );
	if(!pLegion) return;

	// 未在战斗阶段不能加入
	if( GSProto::enStateInBattle != m_iCityFightStaus)
	{
		pActor->sendErrorCode(ERROR_NOTIN_LEGIONCAMPBATTLE);
		return;
	}

	// 如果军团未参战，不能加入
	int iActorCityID = getReportCity(pLegion->getLegionName() );
	if(iCityID != iActorCityID)
	{
		pActor->sendErrorCode(ERROR_LEGION_NOTREPORT);
		return;
	}

	LegionCampBattle* pCampBattle = getCampBattle(iCityID);
	if(!pCampBattle) return;

	// 通知客户端
	GSProto::CMD_LEGION_CAMPBATTLE_JOIN_SC scMsg;

	
	pCampBattle->fillScJoinInfo(hActor, scMsg );

	pActor->sendMessage(GSProto::CMD_LEGION_CAMPBATTLE_JOIN, scMsg);
}

void LegionFactory::onEnterCampBattle(HEntity hActor, int iCampId, string strVsLegionName)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

	string strName = pActor->getProperty(PROP_ENTITY_NAME, "");
	GSLegionImp* pLegion  = static_cast<GSLegionImp*>(getActorLegion(strName) );
	if(!pLegion) return;
	string strActorLoginName = pLegion->getLegionName();

	// 1.不是防守方不应该上行这个协议
	LegionCampBattle* pCampBattle = getCampBattle(iCampId);
	if(!pCampBattle) return;
	//if(pCampBattle->strOwerName() != strActorLoginName )
	//{
	//	return;
	//}
	
	// 通知客户端
	GSProto::CMD_LEGION_CAMPBATTLE_JOIN_SC scMsg;
	pCampBattle->fillScJoinInfo(hActor, scMsg, true, strVsLegionName);

	pActor->sendMessage(GSProto::CMD_LEGION_CAMPBATTLE_JOIN, scMsg);
}

void LegionFactory::onCampMemberBattle(HEntity hActor,int iCampId, int iBattleId ,string strMemName, int iStar)
{
	IEntity *pActor = getEntityFromHandle(hActor);
	assert(pActor);
	
	if( GSProto::enStateInBattle !=  m_iCityFightStaus)
	{
		pActor->sendErrorCode(ERROR_NOTIN_LEGIONCAMPBATTLE);
		return;
	}
	
	LegionCampBattle* pCampBattle = getCampBattle(iCampId);
	if(!pCampBattle) return;

	pCampBattle->onCampMemberBattle(hActor, iBattleId, strMemName,iStar);

}


int LegionFactory::getReportCity(const string& strLegionName)
{
	map<string, int>::iterator iter = m_ReportLegionMap.find(strLegionName);
	if(iter != m_ReportLegionMap.end())
	{
		return iter->second;
	}	
	return -1;
}

int LegionFactory::getJoinCity(const string& strActrorName)
{
	GSLegionImp* pLegion = static_cast<GSLegionImp*>(getActorLegion(strActrorName) );
	if(!pLegion) return 0;

	
	//int iCityID = getReportCity(pLegion->getLegionName() );
	//LegionCityImp* pCityImp = getCityImp(iCityID);
//	if(!pCityImp) return 0;

//	if(pCityImp->isActorJoined(pLegion->getLegionName(), strActrorName) )
//	{
//		return iCityID;
//	}

	return 0;
}

LegionCampBattle* LegionFactory::getCampBattle(int iCityID)
{
	map<int, LegionCampBattle*>::iterator it = m_legionCampBattleMap.find(iCityID);
	if(it == m_legionCampBattleMap.end() )
	{
		return NULL;
	}

	return it->second;
}

struct GetActorDataCallBack
{
	GetActorDataCallBack(string strUUID, ServerEngine::MailData  mailData, int iBoradId)
		:m_strUUID(strUUID),
		m_mailData(mailData),
		m_iBoradId(iBoradId)
	{
		
	}

	virtual void operator()(int iRet, HEntity hTarget, ServerEngine::PIRole& roleInfo)
	{
		if( iRet != ServerEngine::en_RoleRet_OK)
		{
			FDLOG("ManorSystem")<<"WillLootActorCallBack|"<< iRet <<endl;
			return;
		}

		if(0 != hTarget)
		{
			IEntity *pEntity = getEntityFromHandle(hTarget);
			assert(pEntity);
			IMailSystem*pMailSys = static_cast<IMailSystem*>(pEntity->querySubsystem( IID_IMailSystem));
			assert(pMailSys);
			pMailSys->addMail(m_strUUID,m_mailData, "Legion _destroy");	
			//通知被踢玩家
			GSProto::CMD_NOTICE_LEGOIN_MEMBER_OUTLEGION_SC scMsg;
			if(m_iBoradId == 1001)
			{
				scMsg.set_inoticetype(1);
			}
			else if(m_iBoradId == 1002)
			{
				scMsg.set_inoticetype(2);
			}
			pEntity->sendMessage(GSProto::CMD_NOTICE_LEGOIN_MEMBER_OUTLEGION, scMsg);
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
		}
		
	}
	private:
		string m_strUUID;
		ServerEngine::MailData  m_mailData;
		int m_iBoradId;
};



void LegionFactory::_destroy(ILegion* pLegion)
{
	GSLegionImp* pLegionImp = static_cast<GSLegionImp*>(pLegion);
	assert(pLegionImp);

	// 处理宣战相关信息
//	int iReportCity = getReportCity(pLegion->getLegionName() );
//	LegionCityImp* pCityImp = getCityImp(iReportCity);
//	if(pCityImp)
//	{
//		pCityImp->onLegionDismiss(pLegion->getLegionName() );
//}

	vector<Uint32>::iterator it = find(m_sortLegionList.begin(), m_sortLegionList.end(), pLegion->getHandle() );
	assert(it != m_sortLegionList.end() );
	m_sortLegionList.erase(it);

	// 删除所有名字映射
	
	
	const map<std::string, ServerEngine::LegionMemberItem>& refMemMap = pLegionImp->m_legionData.memberInfo.memberList;
	for(map<std::string, ServerEngine::LegionMemberItem>::const_iterator memit = refMemMap.begin(); memit != refMemMap.end(); memit++)
	{
		//发送通知邮件
		sendMail(memit->second, 1002);
		m_mapActorName2Legion.erase(memit->first);
	}

	IJZMessageLayer* pJZMessageLayer = getComponent<IJZMessageLayer>(COMPNAME_MessageLayer, IID_IJZMessageLayer);
	assert(pJZMessageLayer);

	pJZMessageLayer->AsyncDelLegionData(new DelLegionCallback(pLegion->getLegionName() ), pLegion->getLegionName() );

	delete pLegionImp;
}

void LegionFactory::sendMail(const ServerEngine::LegionMemberItem& item, int iBroadId)
{
	IModifyDelegate* pModifyDelegate = getComponent<IModifyDelegate>(COMPNAME_ModifyDelegate, IID_IModifyDelegate);
	assert(pModifyDelegate);
	string strUUID;
	ServerEngine::MailData mailData;
	fillMail(strUUID, mailData, item.beWorshipGetContribute, iBroadId);
	pModifyDelegate->submitDelegateTask(item.roleKey, GetActorDataCallBack(strUUID, mailData,iBroadId));
}


void LegionFactory::fillMail(string& struuid, ServerEngine::MailData & mailData, int iCanContribute, int iBroadId)
{
	// 检测策划配置的公告
	ITable* pMailTb = getCompomentObjectManager()->findTable("Mail");
	assert(pMailTb);
	
	int iMailRecord = pMailTb->findRecord(iBroadId);
	assert(iMailRecord >= 0);
	
	uuid_t itemuuid;
	uuid_generate(itemuuid);
	char szUUIDString[1024] = {0};
	uuid_unparse_upper(itemuuid, szUUIDString);
	mailData.strUUID = szUUIDString;

	mailData.strTitle = pMailTb->getString(iMailRecord, "标题");
	mailData.strSender = pMailTb->getString(iMailRecord, "发送者");
	string strContent =  pMailTb->getString(iMailRecord, "内容");
	string strTmpContent = TC_Common::replace(strContent, "%rank", TC_Common::tostr(iCanContribute));
	
	mailData.strContent = strTmpContent;
	mailData.iState = GSProto::en_MailState_UnRead;
	mailData.dwSenderTime = time(0);
	mailData.iKeepDay = pMailTb->getInt(iMailRecord, "保留天数");
	mailData.iMailType = GSProto::enMailType_Com;

	
	GSProto::FightAwardResult awardResult;
	GSProto::PropItem &item = *(awardResult.add_szawardproplist());
	item.set_ilifeattid(GSProto::en_LifeAtt_Contribute);
	item.set_ivalue(iCanContribute);
		
	if(awardResult.IsInitialized() )
	{
		awardResult.SerializeToString(&mailData.strAwardInfo);
	}

	struuid = mailData.strUUID;

	//PLAYERLOG(pEntity)<<"AddMail| Legion _destroy|"<<struuid<<"|"<<awardResult.DebugString()<<endl;
}


ILegion* LegionFactory::findLegion(const string& strLegionName)
{
	map<string, ILegion*>::iterator it = m_mapLegion.find(strLegionName);
	if(it != m_mapLegion.end() )
	{
		return it->second;
	}

	return NULL;
}

ILegion* LegionFactory::getActorLegion(const string& strActorName)
{
	map<string, Uint32>::iterator it = m_mapActorName2Legion.find(strActorName);
	if(it != m_mapActorName2Legion.end() )
	{
		HLegion hTmpHandle = it->second;
		ILegion* pResult = getLegionFromHandle(hTmpHandle);
		return pResult;
	}

	return NULL;
}

void NewLegionCallback::callback_newLegion(taf::Int32 ret)
{
	if(ret == ServerEngine::en_DataRet_OK)
	{
		// 创建军团
		LegionFactory* pLegionFactory = static_cast<LegionFactory*>(getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory) );
		assert(pLegionFactory);

		pLegionFactory->createLegion(m_legionData);
		pLegionFactory->resortRank();
	}

	m_cb(ret);
}

void GetLegionDataCallback::callback_getLegion(taf::Int32 ret,  const std::string& strValue)
{
	if(ret == ServerEngine::en_DataRet_OK)
	{
		ServerEngine::LegionData tmpData;
		ServerEngine::JceToObj(strValue, tmpData);
		m_pLegionFactory->createLegion(tmpData);
	}
	else
	{
		FDLOG("Legion")<<"InvalidLegion|"<<m_strLegionName<<endl;
	}

	if(m_remainList.size() > 0)
	{
		IJZMessageLayer* pJZMessageLayer = getComponent<IJZMessageLayer>(COMPNAME_MessageLayer, IID_IJZMessageLayer);
		assert(pJZMessageLayer);
	
		string strKey = m_remainList.back();
		m_remainList.pop_back();
		pJZMessageLayer->AsyncGetLegionData(new GetLegionDataCallback(m_pLegionFactory, strKey, m_remainList), strKey);
	}
	else
	{
		m_pLegionFactory->setReady();
	}
}

void GetLegionDataCallback::callback_getLegion_exception(taf::Int32 ret)
{
	assert(false);
}

void LegionFactory::initLegionList(const vector<string>& legionList)
{
	IJZMessageLayer* pJZMessageLayer = getComponent<IJZMessageLayer>(COMPNAME_MessageLayer, IID_IJZMessageLayer);
	assert(pJZMessageLayer);

	vector<string> allList = legionList;

	if(allList.size() > 0)
	{
		string strKey = allList.back();
		allList.pop_back();
		pJZMessageLayer->AsyncGetLegionData(new GetLegionDataCallback(this, strKey, allList), strKey);
	}
	else
	{
		setReady();
	}
}


void LegionFactory::setReady()
{
	m_bReady = true;
	SvrRunLog("LegionFactory Ready!!");
	resortRank();
}

int LegionFactory::GMOnlineGetLegionMemberNameList(string strLegionName, string &strJson)
{
	ILegion* pLegion = findLegion(strLegionName);
	if(!pLegion) 
	{
		return -1;
	}

	pLegion->GMOnlineGetActorInfo(strJson);
	if(strJson.size() >0)
		return 1;
	return -1;
}

void LegionFactory::createLegion(const ServerEngine::LegionData& data)
{
	GSLegionImp* pLegion = new GSLegionImp;
	if(!pLegion->initFromDB(data) )
	{
		delete pLegion;
		assert(false);
		return;
	}

	// 添加军团名字索引
	m_mapLegion[pLegion->getLegionName()] = pLegion;

	HLegion hHandle = pLegion->getHandle();

	// 添加成员索引
	const map<std::string, ServerEngine::LegionMemberItem>& memList = data.memberInfo.memberList;
	for( map<std::string, ServerEngine::LegionMemberItem>::const_iterator it = memList.begin(); it != memList.end(); it++)
	{
		string strMemName = it->first;
		//assert(m_mapActorName2Legion.find(strMemName) ==m_mapActorName2Legion.end() )
		if(m_mapActorName2Legion.find(strMemName) !=m_mapActorName2Legion.end())
		{
			ILegion* pOldLegion = getLegionFromHandle(m_mapActorName2Legion[strMemName]);
			if(pOldLegion)
			{
				FDLOG("LegionFactory")<<strMemName<<"|"<<pLegion->getLegionName()<< "|" <<pOldLegion->getLegionName()<<endl;
			}
			else
			{
				FDLOG("LegionFactory")<<strMemName<<"|"<<pLegion->getLegionName()<<endl;
			}
		}
		m_mapActorName2Legion[strMemName] = hHandle;
	}

	// 添加到队列，这里不排序
	m_sortLegionList.push_back(hHandle);
}

void NewLegionCallback::callback_newLegion_exception(taf::Int32 ret)
{
	m_cb(ServerEngine::en_DataRet_SysError);
}

void LegionFactory::fillNewLegionData(ServerEngine::LegionData& data, HEntity hCreater, const string& strLegionName, int iLegionIconId)
{
	IEntity* pCreater = getEntityFromHandle(hCreater);
	assert(pCreater);

	data.baseInfo.strLegionName = strLegionName;
	data.baseInfo.strLeaderName = pCreater->getProperty(PROP_ENTITY_NAME, "");
	data.baseInfo.iLegionLevel = 1;
	data.baseInfo.dwLastLoginTime= time(0);
	data.baseInfo.dwLevelUpTime = time(0);
	data.baseInfo.iLegionIconId = iLegionIconId;

	// 填充成员信息
	ServerEngine::LegionMemberItem& leaderMemInfo = data.memberInfo.memberList[data.baseInfo.strLeaderName];
	leaderMemInfo.roleKey.strAccount = pCreater->getProperty(PROP_ACTOR_ACCOUNT, "");
	leaderMemInfo.roleKey.rolePos = 0;
	leaderMemInfo.roleKey.worldID = pCreater->getProperty(PROP_ACTOR_WORLD, 0);
	leaderMemInfo.contributeVec.resize(3,0);
	leaderMemInfo.iOccupation = GSProto::en_LegionOccupation_Leader;
	
	leaderMemInfo.strName = data.baseInfo.strLeaderName;
	leaderMemInfo.iLevel = pCreater->getProperty(PROP_ENTITY_LEVEL, 0);

	leaderMemInfo.bIsOutLine = false;
	
}

bool LegionFactory::isReady()
{
	return m_bReady;
}


// 刷新工会登录时间、玩家在工会信息
void LegionFactory::updateActorInfo(HEntity hActor)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

	int iClassID = pActor->getProperty(PROP_ENTITY_CLASS, 0);
	if(GSProto::en_class_Actor != iClassID)
	{
		return;
	}

	string strActorName = pActor->getProperty(PROP_ENTITY_NAME, "");
	GSLegionImp* pLegion = static_cast<GSLegionImp*>(getActorLegion(strActorName) );
	
	if(!pLegion) return;
	pLegion->updateActorInfo(hActor);
}

void LegionFactory::actorLogionOut(HEntity hActor,bool bIsOut)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

	int iClassID = pActor->getProperty(PROP_ENTITY_CLASS, 0);
	if(GSProto::en_class_Actor != iClassID)
	{
		return;
	}

	string strActorName = pActor->getProperty(PROP_ENTITY_NAME, "");
	GSLegionImp* pLegion = static_cast<GSLegionImp*>(getActorLegion(strActorName) );
	
	if(!pLegion) return;
	pLegion->actorLoginOut(hActor, bIsOut);
}

void LegionFactory::updateActorLevelInfo(HEntity hActor)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

	int iClassID = pActor->getProperty(PROP_ENTITY_CLASS, 0);
	if(GSProto::en_class_Actor != iClassID)
	{
		return;
	}

	string strActorName = pActor->getProperty(PROP_ENTITY_NAME, "");
	GSLegionImp* pLegion = static_cast<GSLegionImp*>(getActorLegion(strActorName) );

	if(!pLegion) return;
	pLegion->updateActorLevelInfo(hActor);
}


 bool LegionFactory::newLegion(HEntity hCreater, const string& strLegionName, DelegateCreateLegion cb, int iLegionIconId)
{
	// 先验证创建者是否合法
	IEntity* pCreater = getEntityFromHandle(hCreater);
	if(!pCreater) return false;

	// 是否已经有帮派了?
	if(getActorLegion(pCreater->getProperty(PROP_ENTITY_NAME, "") ) )
	{
		return false;
	}

	// 本地先验证下名字重名问题
	if(findLegion(strLegionName) )
	{
		pCreater->sendErrorCode(ERROR_LEGIONNAME_EXIST);
		return false;
	}

	if(strLegionName.size() == 0)
	{
		return false;
	}

	// 验证名字合法
	IReserveWordCheck* pReserveWordCheck = getComponent<IReserveWordCheck>("ReserveWordCheck", IID_IReserveWordCheck);
	assert(pReserveWordCheck);

	int iWordCount = pReserveWordCheck->getWordLength(strLegionName);
	if( (iWordCount > GSProto::MAX_ROLENAME_LEN) || (iWordCount < 0) )
	{
		pCreater->sendErrorCode(ERROR_LEGION_NAMETOOLONG);
		return false;
	}

	// 保留字验证
	if(pReserveWordCheck->hasReserveWord(strLegionName) )
	{
		pCreater->sendErrorCode(ERROR_LEGIONAME_INVALID);
		return false;
	}
	
	// 发到远端验证
	IJZMessageLayer* pJZMessageLayer = getComponent<IJZMessageLayer>(COMPNAME_MessageLayer, IID_IJZMessageLayer);
	assert(pJZMessageLayer);

	ServerEngine::LegionData newLegionData;
	fillNewLegionData(newLegionData, hCreater, strLegionName, iLegionIconId);

	string strTmpData = ServerEngine::JceToStr(newLegionData);
	pJZMessageLayer->AsyncNewLegion(new NewLegionCallback(cb, newLegionData), strLegionName, strTmpData);

	return true;
}


void LegionFactory::sendLegionPage(HEntity hActor, int iPageIndex)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	if(!pActor) return;

	int iSize = (int)m_sortLegionList.size();
	int iTotalPage = (iSize + GSProto::LEGION_LIST_PAGESIZE - 1)/GSProto::LEGION_LIST_PAGESIZE;

	if(iPageIndex >= (iTotalPage - 1) ) iPageIndex = iTotalPage - 1;
	if(iPageIndex < 0) iPageIndex = 0;

	int iBeginIndex = iPageIndex * GSProto::LEGION_LIST_PAGESIZE;
	int iEndIndex = std::min(iBeginIndex + GSProto::LEGION_LIST_PAGESIZE, iSize);

	GSProto::CMD_QUERY_LEGIONLIST_SC scMsg;
	scMsg.set_itotalpages(iTotalPage);
	scMsg.set_ipageindex(iPageIndex);

	string strActorName = pActor->getProperty(PROP_ENTITY_NAME, "");
	for(int i = iBeginIndex; i < iEndIndex; i++)
	{
		GSProto::LegionListItem* pNewItem = scMsg.mutable_szlegionlist()->Add();
		fillLegionListItem(strActorName, m_sortLegionList[i], pNewItem);
	}

	pActor->sendMessage(GSProto::CMD_QUERY_LEGIONLIST, scMsg);
}

void LegionFactory::fillLegionListItem(const string& strActorName, Uint32 dwLegionHandle, GSProto::LegionListItem* pListItem)
{
	GSLegionImp* pLegion = static_cast<GSLegionImp*>(getLegionFromHandle(dwLegionHandle));
	assert(pLegion);

	pListItem->set_irank(pLegion->getRank() );
	pListItem->set_strlegionname(pLegion->getLegionName() );
	pListItem->set_strleadername(pLegion->getLeaderName() );
	pListItem->set_dwlegionobjectid(dwLegionHandle);
	pListItem->set_ilevel(pLegion->getLevel() );
	pListItem->set_imemcount(pLegion->getMemberCount() );
	pListItem->set_imemlimit(pLegion->getMemberLimit() );
	pListItem->set_bapplyed(pLegion->isApplyed(strActorName) );
	pListItem->set_ilegioniconid(pLegion->getLegionIconId());
}

/*
void LegionFactory::loadLegionShop()
{
	ITable* pTable = getCompomentObjectManager()->findTable("LegionShop");
	assert(pTable);

	int iRecordCount = pTable->getRecordCount();
	m_iSumShopChance = 0;
	for(int i = 0; i < iRecordCount; i++)
	{
		LegionShopCfg tmpShopItem;
		tmpShopItem.iID = pTable->getInt(i, "ID");
		string strTypeString = pTable->getString(i, "类型Id");
		vector<int> typeIDList =  TC_Common::sepstr<int>(strTypeString, "#");
		if(typeIDList.size() == 1)
		{
			tmpShopItem.awardItem.iDropTypeID = typeIDList[0];
			if(typeIDList[0] < GSProto::en_LifeAtt_End)
			{
				tmpShopItem.awardItem.iType = GSProto::en_ShopType_Prop;
			}
			else
			{
				tmpShopItem.awardItem.iType = GSProto::en_ShopType_Item;
			}
		}
		else if(typeIDList.size() == 2)
		{
			tmpShopItem.awardItem.iType = GSProto::en_ShopType_Hero;
			tmpShopItem.awardItem.iDropTypeID = typeIDList[0];
			tmpShopItem.awardItem.iHeroLevelStep = typeIDList[1];
		}
		else
		{
			assert(false);
		}
		
		tmpShopItem.awardItem.iCount = pTable->getInt(i, "数量");
		tmpShopItem.awardItem.iChance = pTable->getInt(i, "权重");
		m_iSumShopChance += tmpShopItem.awardItem.iChance;
		
		m_shopItemMap[tmpShopItem.iID] = tmpShopItem;
	}
}

*/

struct LegionCmp
{
	bool operator ()(Uint32 dwFirst, Uint32 dwSecond) const
	{
		GSLegionImp* pFirst = static_cast<GSLegionImp*>(getLegionFromHandle(dwFirst));
		GSLegionImp* pSecond = static_cast<GSLegionImp*>(getLegionFromHandle(dwSecond));

		assert(pFirst && pSecond);

		if(pFirst->getLevel() != pSecond->getLevel() )
		{
			return pFirst->getLevel() > pSecond->getLevel();
		}

		// 按照时间
		if(pFirst->getLevelUpTime() != pSecond->getLevelUpTime() )
		{
			return pFirst->getLevelUpTime() < pSecond->getLevelUpTime();
		}

		return pFirst->getLegionName() > pSecond->getLegionName();
	}
};

void LegionFactory::resortRank()
{
	std::sort(m_sortLegionList.begin(), m_sortLegionList.end(), LegionCmp() );

	// 设置到Legion
	for(size_t i = 0; i < m_sortLegionList.size(); i++)
	{
		GSLegionImp* pLegion = static_cast<GSLegionImp*>(getLegionFromHandle(m_sortLegionList[i]));
		assert(pLegion);

		pLegion->setRank((int)i+1);
	}
}

const LegionLevelInfo* LegionFactory::queryLevelInfo(int iLevel)
{
	map<int, LegionLevelInfo>::iterator it = m_legionLevelMap.find(iLevel);
	if(it == m_legionLevelMap.end() )
	{
		return NULL;
	}

	return &(it->second);
}


void LegionFactory::loadLegionLevel()
{
	ITable* pLegionLvTb = getCompomentObjectManager()->findTable("LegionLevelExp");
	assert(pLegionLvTb);

	int iRecordCount = pLegionLvTb->getRecordCount();
	for(int i = 0; i < iRecordCount; i++)
	{
		LegionLevelInfo tmpLevelInfo;
		tmpLevelInfo.iLevel = pLegionLvTb->getInt(i, "军团等级");
		tmpLevelInfo.iNeedExp = pLegionLvTb->getInt(i, "经验");
		tmpLevelInfo.iLeaderLimit = pLegionLvTb->getInt(i, "团长数");
		tmpLevelInfo.iViceLeaderLimitt = pLegionLvTb->getInt(i, "副团长数");
		tmpLevelInfo.iMemLimit = pLegionLvTb->getInt(i, "成员上限");

		assert(m_legionLevelMap.find(tmpLevelInfo.iLevel) == m_legionLevelMap.end() );
		m_legionLevelMap[tmpLevelInfo.iLevel] = tmpLevelInfo;
	}
}

void LegionFactory::loadLegionContribute()
{
	ITable* pContributeTb = getCompomentObjectManager()->findTable("LegionContribute");
	assert(pContributeTb);

	int iRecordCount = pContributeTb->getRecordCount();
	for(int i = 0; i < iRecordCount; i++)
	{
		LegionContributeCfg tmpCfg;
		tmpCfg.iID = pContributeTb->getInt(i, "ID");
		tmpCfg.iCostSilver = pContributeTb->getInt(i, "银币");
		tmpCfg.iCostGold = pContributeTb->getInt(i, "金币");
		tmpCfg.iGetContribute = pContributeTb->getInt(i, "贡献");
		
		m_legionContributeMap[tmpCfg.iID] = tmpCfg;
	}
}

void LegionFactory::loadLegionBless()
{
	ITable* pBlessTb = getCompomentObjectManager()->findTable("LegionBless");
	assert(pBlessTb);

	int iRecordCount = pBlessTb->getRecordCount();
	for(int i =0; i < iRecordCount; i++)
	{
		LegionBlessCfg tmpCfg;
		tmpCfg.iID = pBlessTb->getInt(i, "ID");
		tmpCfg.iCostSilver = pBlessTb->getInt(i, "银币");
		tmpCfg.iCostGold = pBlessTb->getInt(i, "金币");
		tmpCfg.iGetContribute = pBlessTb->getInt(i, "贡献");
		tmpCfg.iGetPhystrength = pBlessTb->getInt(i, "获得体力");

		m_legionBlessMap[tmpCfg.iID] = tmpCfg;
	}
}

void LegionFactory::loadLegionPayParam()
{
	ITable* pTable = getCompomentObjectManager()->findTable("LegionPay");
	assert(pTable);

	int iRecordCount = pTable->getRecordCount();
	for(int i = 0; i < iRecordCount; i++)
	{
		LegionPayParam tmpParam;
		string strRankRange = pTable->getString(i, "贡献区间");

		vector<int> rangeList = TC_Common::sepstr<int>(strRankRange, "#");
		assert(rangeList.size() == 2);
		
		tmpParam.iMinRank = rangeList[0];
		tmpParam.iMaxRank = rangeList[1];
		tmpParam.iParam = pTable->getInt(i, "加成系数");

		m_legionPayParamList.push_back(tmpParam);
	}
}

const LegionContributeCfg* LegionFactory::queryContributeCfg(int ContriID)
{
	map<int, LegionContributeCfg>::iterator it = m_legionContributeMap.find(ContriID);
	if(it == m_legionContributeMap.end() )
	{
		return NULL;
	}

	return &(it->second);
}

const LegionBlessCfg* LegionFactory::queryBlessCfg(int iBlessID)
{
	map<int, LegionBlessCfg>::iterator it = m_legionBlessMap.find(iBlessID);
	if(it == m_legionBlessMap.end() )
	{
		return NULL;
	}

	return &(it->second);
}

int LegionFactory::getPayParam(int iRank)
{
	for(size_t i = 0; i < m_legionPayParamList.size(); i++)
	{
		if( (iRank >= m_legionPayParamList[i].iMinRank) && (iRank <= m_legionPayParamList[i].iMaxRank) )
		{
			return m_legionPayParamList[i].iParam;
		}
	}

	return 0;
}

/*
const LegionShopCfg* LegionFactory::queryShopCfg(int iShopID)
{
	map<int, LegionShopCfg>::iterator it = m_shopItemMap.find(iShopID);
	if(it == m_shopItemMap.end() )
	{
		return NULL;
	}

	return &(it->second);
}
*/
GSProto::FightAwardHero* LegionFactory::fillToAward(const LegionShopCfg* pAward, GSProto::FightAwardResult& awardResult)
{
	switch(pAward->awardItem.iType)
	{
		case GSProto::en_ShopType_Prop:
			{
				GSProto::PropItem* pPropItem = awardResult.mutable_szawardproplist()->Add();
				pPropItem->set_ilifeattid(pAward->awardItem.iDropTypeID);
				pPropItem->set_ivalue(pAward->awardItem.iCount);
			}
			break;

		case GSProto::en_ShopType_Item:
			{
				GSProto::FightAwardItem* pItemAward = awardResult.mutable_szawarditemlist()->Add();
				pItemAward->set_iitemid(pAward->awardItem.iDropTypeID);
				pItemAward->set_icount(pAward->awardItem.iCount);
			}
			break;

		case GSProto::en_ShopType_Hero:
			{
				GSProto::FightAwardHero* pHeroAward = awardResult.mutable_szawardherolist()->Add();
				pHeroAward->set_iheroid(pAward->awardItem.iDropTypeID);
				pHeroAward->set_icount(pAward->awardItem.iCount);
				pHeroAward->set_ilevelstep(pAward->awardItem.iHeroLevelStep);
				return pHeroAward;
			}
			break;
	}

	return NULL;
}

/*void LegionFactory::randLegionShop(vector<int>& outShopItemList, int  iSize)
{
	IRandom* pRandom = getComponent<IRandom>("Random", IID_IMiniAprRandom);
	assert(pRandom);

	outShopItemList.clear();
	while(outShopItemList.size() < (size_t)iSize)
	{
		assert(m_iSumShopChance > 0);
		int iRandV = pRandom->random()% m_iSumShopChance;

		int iTmpSum = 0;
		for(map<int, LegionShopCfg>::iterator it = m_shopItemMap.begin(); it != m_shopItemMap.end(); it++)
		{
			const LegionShopCfg& refShopCfg = it->second;
			iTmpSum += refShopCfg.awardItem.iChance;
			if(iRandV < iTmpSum)
			{
				outShopItemList.push_back(refShopCfg.iID);
				break;
			}
		}
	}
}*/

void LegionFactory::cleanupSave()
{
	for(map<string, ILegion*>::iterator it = m_mapLegion.begin(); it != m_mapLegion.end(); it++)
	{
		ILegion* pLegion = it->second;
		assert(pLegion);

		pLegion->saveData(true);
	}

	saveGlobalCityData(true);
}

int LegionFactory::getOwnCityCount(const string& strLegionName)
{
	int iRet = 0;
	for(map<int, LegionCampBattle*>::iterator it = m_legionCampBattleMap.begin(); it != m_legionCampBattleMap.end(); it++)
	{
		LegionCampBattle* pCityImp = it->second;
		assert(pCityImp);

		if(pCityImp->getOwnerLegion() == strLegionName)
		{
			iRet++;
		}
	}

	return iRet;
}

int LegionFactory::getDomainDayPay(const string& strLegionName)
{
	int iResult = 0;

	/*for(map<int, LegionCityImp*>::iterator it = m_legionCityMap.begin(); it != m_legionCityMap.end(); it++)
	{
		LegionCityImp* pCityImp = it->second;
		assert(pCityImp);

		if(pCityImp->getOwnerLegion() == strLegionName)
		{
			const LegionCityCfg* pCityCfg = queryCityCfg(it->first);
			assert(pCityCfg);
		
			iResult += pCityCfg->iSilver;
		}
	}*/

	return iResult;
}

void LegionFactory::addActorNameMap(const string& strName, Uint32 dwLegionHandle)
{
	m_mapActorName2Legion[strName] = dwLegionHandle;
}

void LegionFactory::removeActorNameMap(const string& strName)
{
	m_mapActorName2Legion.erase(strName);
}


const GiftCfg* LegionFactory::queryGiftCfg(int iGiftID)
{
	map<int, GiftCfg>::iterator it = m_mapGiftCfg.find(iGiftID);
	if(it == m_mapGiftCfg.end() )
	{
		return NULL;
	}

	return &(it->second);
}

void LegionFactory::addLegionContribute(HEntity hActor, int iValue)
{
	IEntity *pActor = getEntityFromHandle(hActor);
	assert(pActor);
	
	string strActorName = pActor->getProperty(PROP_ENTITY_NAME, "");
	
	GSLegionImp* pLegionImp = static_cast<GSLegionImp*>(getActorLegion(strActorName) );

	if(!pLegionImp) return;

	pLegionImp->addMemberContribyte(hActor, iValue);
}

int LegionFactory::iGetLegionMemCount(string strLegionName)
{
	GSLegionImp* pLegionImp = static_cast<GSLegionImp*>(findLegion(strLegionName));
	if(!pLegionImp) return 0;

	return  pLegionImp->getMemberCount();
}

void  LegionFactory::updateActorFightValue(HEntity hEntity)
{
	IEntity *pActor = getEntityFromHandle(hEntity);
	assert(pActor);
	
	string strActorName = pActor->getProperty(PROP_ENTITY_NAME, "");
	
	GSLegionImp* pLegionImp = static_cast<GSLegionImp*>(getActorLegion(strActorName) );

	if(!pLegionImp) return;

	pLegionImp->updateActorFightValue(hEntity);
}

void LegionFactory::onQueryLastFightLog(HEntity hActor, string strActorLegionName)
{
	IEntity *pActor = getEntityFromHandle(hActor);
	if(!pActor) return;
	
	GSProto::CMD_QUERY_CAMPBATTLE_LASTFIGHT_LOG_SC scMsg;

	map<std::string, map<taf::Int32, std::string> >::const_iterator  iter  =  m_cityCampFightLog.lastFightLogMap.find(strActorLegionName);
	if(iter != m_cityCampFightLog.lastFightLogMap.end())
	{
		const map<taf::Int32, std::string>& actorLegionLog = iter->second;

		map<taf::Int32, std::string>::const_iterator rollIter = actorLegionLog.begin();
		for(; rollIter != actorLegionLog.end(); ++rollIter)
		{
			string strFightLog = rollIter->second;

			GSProto::LastFightLogItem& lastFightLogItem = *(scMsg.add_szfightloglist());
			bool bResoult = lastFightLogItem.ParseFromString(strFightLog);
			assert(bResoult);
		}
	}
	pActor->sendMessage(GSProto::CMD_QUERY_CAMPBATTLE_LASTFIGHT_LOG, scMsg);
}

void LegionFactory::onQueryLastFightSimpleLog(HEntity hActor, string strActorLegionName, int iCampId)
{
	IEntity *pActor = getEntityFromHandle(hActor);
	if(!pActor) return;
	
	GSProto::CMD_QUERY_CAMPBATTLE_LASTFIGHT_SIMPLELOG_SC scMsg;
	
	map<taf::Int32, ServerEngine::LegionFightDetail >::const_iterator iter = m_cityCampFightLog.lastFightLogDetail.find(iCampId);
	if(iter != m_cityCampFightLog.lastFightLogDetail.end() )
	{
		 map<std::string, ServerEngine::LegionFightRecord>::const_iterator fightDetailIter = iter->second.m_fightDetail.begin();
		 scMsg.set_strlegionname(strActorLegionName);
		 GSProto::VSLegionBase &baseMsg = *(scMsg.mutable_vslegionmsg());
		 for(;fightDetailIter != iter->second.m_fightDetail.end(); ++fightDetailIter)
		 {
		 	const ServerEngine::LegionFightRecord& detail = fightDetailIter->second;
			
	 		GSProto::VSLegion& vsLegion = *(baseMsg.add_szvslegionlist());
			bool res = vsLegion.ParseFromString(detail.strVsLegion);
			assert(res);
		 }
		
		pActor->sendMessage(GSProto::CMD_QUERY_CAMPBATTLE_LASTFIGHT_SIMPLELOG,	scMsg);
	}

	 
}

void LegionFactory::onQueryLastFightDetailLog(HEntity hActor, string strAttLegionName,int iCampId, string strActorLegionname)
{
	IEntity *pActor = getEntityFromHandle(hActor);
	if(!pActor) return;
	
	GSProto::CMD_QUERY_CAMPBATTLE_LASTFIGHT_DETAIL_LOG_SC scMsg;
	
	map<taf::Int32, ServerEngine::LegionFightDetail >::const_iterator iter = m_cityCampFightLog.lastFightLogDetail.find(iCampId);
	if(iter != m_cityCampFightLog.lastFightLogDetail.end() )
	{
		const ServerEngine::LegionFightDetail& detail = iter->second;

		bool bActorIsGuard = false;
		if(detail.strGuardName == strActorLegionname )
		{
			bActorIsGuard = true;
		}
		
		map<std::string, ServerEngine::LegionFightRecord>::const_iterator findIter = detail.m_fightDetail.find(strAttLegionName);
		if(findIter != detail.m_fightDetail.end())
		{
			const ServerEngine::LegionFightRecord& record = findIter->second;
			GSProto::VSLegion &vsLegion = *(scMsg.mutable_vslegion());
			bool res = vsLegion.ParseFromString(record.strVsLegion);
			assert(res);

			for(size_t i = 0; i<record.guardFightLog.size(); ++i )
			{
				if(bActorIsGuard)
				{
					GSProto::FightRecord& csFightRecord = *(scMsg.add_selffightrecord());
					bool res = csFightRecord.ParseFromString(record.guardFightLog[i]);
					assert(res);
				}
				else
				{
					GSProto::FightRecord& csFightRecord = *(scMsg.add_enemyfightrecod());
					bool res = csFightRecord.ParseFromString(record.guardFightLog[i]);
					assert(res);
				}
			}
			
			for(size_t i = 0; i<record.attFightLog.size(); ++i )
			{
				if(bActorIsGuard)
				{
					GSProto::FightRecord& csFightRecord = *(scMsg.add_enemyfightrecod());
					bool res = csFightRecord.ParseFromString(record.attFightLog[i]);
					assert(res);
				}
				else
				{
					GSProto::FightRecord& csFightRecord = *(scMsg.add_selffightrecord());
					bool res = csFightRecord.ParseFromString(record.attFightLog[i]);
					assert(res);
				}
			}
			pActor->sendMessage(GSProto::CMD_QUERY_CAMPBATTLE_LASTFIGHT_DETAIL_LOG,  scMsg);
		}
	}
}

void LegionFactory::onQueryCurFightCondition(HEntity hActor, int iCampId)
{
	IEntity *pActor = getEntityFromHandle(hActor);
	assert(pActor);
	
	if( GSProto::enStateInBattle !=  m_iCityFightStaus)
	{
		pActor->sendErrorCode(ERROR_NOTIN_LEGIONCAMPBATTLE);
		return;
	}
	
	LegionCampBattle* pCampBattle = getCampBattle(iCampId);
	if(!pCampBattle) return;

	pCampBattle->onQueryCurFightCondition(hActor);
}

void  LegionFactory::onQuerycurFightDetailLog(HEntity hActor, string strAttLegionName,int iCampId, string strActorLegionname)
{
	
	IEntity *pActor = getEntityFromHandle(hActor);
	if(!pActor) return;
	
	GSProto::CMD_QUERY_CAMPBATTLE_LASTFIGHT_DETAIL_LOG_SC scMsg;
	
	map<taf::Int32, ServerEngine::LegionFightDetail >::const_iterator iter = m_curCityFightLog.lastFightLogDetail.find(iCampId); //名字错误了，就错了吧
	if(iter != m_curCityFightLog.lastFightLogDetail.end() )
	{
		const ServerEngine::LegionFightDetail& detail = iter->second;

		bool bActorIsGuard = false;
		if(detail.strGuardName == strActorLegionname )
		{
			bActorIsGuard = true;
		}
		
		map<std::string, ServerEngine::LegionFightRecord>::const_iterator findIter = detail.m_fightDetail.find(strAttLegionName);
		if(findIter != detail.m_fightDetail.end())
		{
			const ServerEngine::LegionFightRecord& record = findIter->second;
			GSProto::VSLegion &vsLegion = *(scMsg.mutable_vslegion());
			bool res = vsLegion.ParseFromString(detail.battleBaseLog);
			assert(res);

			for(size_t i = 0; i<record.guardFightLog.size(); ++i )
			{
				if(bActorIsGuard)
				{
					GSProto::FightRecord& csFightRecord = *(scMsg.add_selffightrecord());
					bool res = csFightRecord.ParseFromString(record.guardFightLog[i]);
					assert(res);
				}
				else
				{
					GSProto::FightRecord& csFightRecord = *(scMsg.add_enemyfightrecod());
					bool res = csFightRecord.ParseFromString(record.guardFightLog[i]);
					assert(res);
				}
			}
			
			for(size_t i = 0; i<record.attFightLog.size(); ++i )
			{
				if(bActorIsGuard)
				{
					GSProto::FightRecord& csFightRecord = *(scMsg.add_enemyfightrecod());
					bool res = csFightRecord.ParseFromString(record.attFightLog[i]);
					assert(res);
				}
				else
				{
					GSProto::FightRecord& csFightRecord = *(scMsg.add_selffightrecord());
					bool res = csFightRecord.ParseFromString(record.attFightLog[i]);
					assert(res);
				}
			}
			pActor->sendMessage(GSProto::CMD_QUERY_CAMPBATTLE_LASTFIGHT_DETAIL_LOG,  scMsg);
		}
	}
}

void LegionFactory::addFightLogInBattle(int iCampId, string attLegionName, string strGuardLog, string strAttLog, string fightBaseLog,string strGuardName, bool addGuardLog)
{

	map<taf::Int32, ServerEngine::LegionFightDetail>& detailMap = m_curCityFightLog.lastFightLogDetail;
	map<taf::Int32, ServerEngine::LegionFightDetail>::iterator iter = detailMap.find(iCampId);
	if(iter != detailMap.end())
	{
		ServerEngine::LegionFightDetail&  detail = iter->second;
		detail.battleBaseLog = fightBaseLog;
		map<std::string, ServerEngine::LegionFightRecord>::iterator recordIter = detail.m_fightDetail.find(attLegionName);
		if(recordIter != detail.m_fightDetail.end())
		{
			ServerEngine::LegionFightRecord& record = recordIter->second;
			if(addGuardLog)
			{
				record.guardFightLog.push_back( strGuardLog);
			}
			else
			{
				record.attFightLog.push_back( strAttLog);
			}
			
			
			record.strVsLegion = fightBaseLog;
		}
		else
		{
	
			ServerEngine::LegionFightRecord record;
			if(addGuardLog)
			{
				record.guardFightLog.push_back( strGuardLog);
			}
			else
			{
				record.attFightLog.push_back( strAttLog);
			}
			
			
			record.strVsLegion = fightBaseLog;

			detail.m_fightDetail.insert(make_pair(attLegionName, record));
		}
	}
	else
	{
		ServerEngine::LegionFightRecord record;
		if(addGuardLog)
		{
			record.guardFightLog.push_back( strGuardLog);
		}
		else
		{
			record.attFightLog.push_back( strAttLog);
		}
		
		record.strVsLegion = fightBaseLog;

		ServerEngine::LegionFightDetail  fightDetail;
		fightDetail.strGuardName = strGuardName;
		fightDetail.battleBaseLog = fightBaseLog;
		
		fightDetail.m_fightDetail.insert(make_pair(attLegionName, record));

		detailMap.insert(make_pair(iCampId, fightDetail));
	}
	
}

void LegionFactory::addFightLogInBattleOver(string& strLegionName, int iCampId, string& strLog)
{
	map<std::string, map<taf::Int32, std::string> >::iterator iter = m_curCityFightLog.lastFightLogMap.find(strLegionName);
	if(iter != m_curCityFightLog.lastFightLogMap.end())
	{
		map<taf::Int32, std::string>::iterator findCampIdIter =  iter->second.find( iCampId);
		assert( findCampIdIter==iter->second.end());
		iter->second.insert(make_pair( iCampId,strLog));
	}
	else
	{
		
map<taf::Int32, std::string> tempMap;
		tempMap.insert( make_pair( iCampId,strLog));
		m_curCityFightLog.lastFightLogMap.insert( make_pair( strLegionName, tempMap));
	}
}

void LegionFactory::setLegionGuard(int iCampId, string strLegionName)
{
	 
	if(m_iCityFightStaus == GSProto::enStateInReport ||m_iCityFightStaus == GSProto::enStateInBattle )
 return;
	
	LegionCampBattle *pCampBattle = getCampBattle(iCampId);
	if(!pCampBattle) return;

	pCampBattle->setLegionGuard(strLegionName);
}

void LegionFactory::openCampBattle(int iReportBegin,int iBattleBegin, int iBattleOver)
{
	
	m_iReportBeginSecond = iReportBegin;
	m_iFightBeginSecond = iBattleBegin;
	m_iFightEndSecond  = iBattleOver;

	m_iLastCityFightDay -= 1;
	m_ReportLegionMap.clear();
	m_curCityFightLog.lastFightLogMap.clear();
	m_curCityFightLog.lastFightLogMap.clear();
}

bool LegionFactory::bLegionInCampBattle(string strLegionName)
{
	if(!isLegionCityBattle())
	{
		return false;
	}
	else
	{
		map<string, int>::iterator iter = m_ReportLegionMap.find(strLegionName);
		if( iter != m_ReportLegionMap.end())
		{
			return true;
		}

		for(map<int, LegionCampBattle*>::iterator it = m_legionCampBattleMap.begin(); it != m_legionCampBattleMap.end(); it++)
		{
			LegionCampBattle* pCityImp = it->second;
			assert(pCityImp);
	
			if(pCityImp->getOwnerLegion() == strLegionName)
			{
				return true;
			}
		}
		return false;
	}
}

