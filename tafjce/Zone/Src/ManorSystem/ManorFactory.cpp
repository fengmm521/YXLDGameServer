#include "ManorSystemPch.h"
#include "ManorFactory.h"

extern "C" IComponent* createManorFactory(Int32)
{
	return new ManorFactory;
}

ManorFactory::ManorFactory():
	dwChgTime(0)
{
}

ManorFactory::~ManorFactory()
{
	ITimerComponent* timeComponent = getComponent<ITimerComponent>(COMPNAME_TimeAxis,IID_ITimerComponent);
	assert(timeComponent);
	timeComponent->killTimer( m_ResetMachineTimerHandle);

	for(size_t i =0; i < m_RefreshCostList.size(); ++i)
	{
		delete m_RefreshCostList[i];
	}
	m_RefreshCostList.clear();
}

bool ManorFactory::initlize(const PropertySet& propSet)
{
	ITable* pResTable = getCompomentObjectManager()->findTable(TABLENAME_ManorRes);
	assert(pResTable);
	ITable* pResLevelTable = getCompomentObjectManager()->findTable(TABLENAME_ManorResLevel);
	assert(pResLevelTable);
	ITable* pWuHunDianTable = getCompomentObjectManager()->findTable(TABLENAME_ManorWuHunDian);
	assert(pWuHunDianTable);
	ITable* pTieJiangPuTable = getCompomentObjectManager()->findTable(TABLENAME_ManorTieJiangPu);
	assert(pTieJiangPuTable);

	int iResTableCount = pResTable->getRecordCount();
	for(int i = 0; i < iResTableCount; ++i)
	{
		int iResid = pResTable->getInt(i,"领地矿ID");
		string strResType = pResTable->getString(i,"类型");
		int iResLevel = pResTable->getInt(i,"开启等级");
		ManorRes res;
		res.iOpenLevel = iResLevel;
		res.iResId = iResid;

		if(strResType == "铜矿")
		{
			res.iResType = GSProto::en_ManorResType_Silver;
			m_ManorSilverRes.push_back(res);
		}
		else if( strResType == "修为矿")
		{
			res.iResType = GSProto::en_ManorResType_HEROEXP;
			m_ManorHeroExpRes.push_back(res);
		}
		else
		{
			assert(0);
		}
		
	}

	int iResLevelTableCount = pResLevelTable->getRecordCount();
	for(int i= 0; i < iResLevelTableCount; ++i)
	{
		int iResId = pResLevelTable->getInt(i, "领地矿ID");
		string strType = pResLevelTable->getString(i, "类型");
		int iResLevel = pResLevelTable->getInt(i, "等级");
		int iProductRate = pResLevelTable->getInt(i, "产量");
		int iLevelUpCost = pResLevelTable->getInt(i,"升级消耗");
		int iLevelUpSecond = pResLevelTable->getInt(i, "升级时间");
		int iTotalProduct = pResLevelTable->getInt(i, "最大储量");
		string ResName = pResLevelTable->getString(i, "名称");
		ManorResUnit unit;
		unit.iResId = iResId;
		unit.iLevel = iResLevel;
		unit.iProductRate = iProductRate;
		unit.iLevelUpCost = iLevelUpCost;
		unit.iLevelUpSecond = iLevelUpSecond;
		unit.iResTotalProduct = iTotalProduct;
		unit.strName = ResName;
		if( strType == "铜矿")
		{
			unit.resType = GSProto::en_ManorResType_Silver;
			ResDetailMap::iterator iter = m_SilverResMap.find(unit.iResId);
			if( iter !=  m_SilverResMap.end())
			{
				(iter->second).insert(make_pair(unit.iLevel, unit));
			}
			else
			{
				map<int, ManorResUnit> unitMap;
				unitMap.insert(make_pair(unit.iLevel, unit));
				m_SilverResMap.insert(make_pair(unit.iResId, unitMap));
			}
			
		}
		else if(strType == "修为矿")
		{
			unit.resType = GSProto::en_ManorResType_HEROEXP;
			ResDetailMap::iterator iter = m_HeroExpResMap.find(unit.iResId);
			if( iter !=  m_HeroExpResMap.end())
			{
				(iter->second).insert(make_pair(unit.iLevel, unit));
			}
			else
			{
				map<int, ManorResUnit> unitMap;
				unitMap.insert(make_pair(unit.iLevel, unit));
				m_HeroExpResMap.insert(make_pair(unit.iResId, unitMap));
			}
		}
		else
		{
			assert(0);
		}	
	}

	int iResWuHunDianTableCount = pWuHunDianTable->getRecordCount();
	for(int i= 0; i < iResWuHunDianTableCount; ++i)
	{
		int iLevel = pWuHunDianTable->getInt(i, "等级");
		int iLevelUpCost = pWuHunDianTable->getInt(i, "升级花费铜币");
		int iLevelUpSecond = pWuHunDianTable->getInt(i, "升级时间");
		int iProductSecond = pWuHunDianTable->getInt(i, "祭炼时间");
		string strProduct = pWuHunDianTable->getString(i, "产物");
		int iOpenGeZiCount = pWuHunDianTable->getInt(i, "开启格子数");
		
		HeroSoulOrItem manorHeroSoul;
		manorHeroSoul.iLevel = iLevel;
		manorHeroSoul.iLevelUpCost = iLevelUpCost;
		manorHeroSoul.iLevelUpSecond = iLevelUpSecond;
		manorHeroSoul.iProductSecond = iProductSecond;
		manorHeroSoul.iOpenGeZiCount = iOpenGeZiCount;
		
		vector<int> productVec = TC_Common::sepstr<int>(strProduct,"#");
		assert(productVec.size()%3 == 0);
		for(size_t i = 0; i < productVec.size(); i = i + 3)
		{
			ManorItem item;
			item.iId = productVec[i];
			item.iCount = productVec[i+1];
			item.iChance = productVec[i+2];
			manorHeroSoul.iTotalChance += item.iChance;
			manorHeroSoul.manorItemList.push_back(item);
		}
		m_WuHunDianMap.insert(make_pair(manorHeroSoul.iLevel, manorHeroSoul));
		
	}

	int iResTieJiangPuTableCount = pTieJiangPuTable->getRecordCount();
	for(int i= 0; i < iResTieJiangPuTableCount; ++i)
	{
		int iLevel = pTieJiangPuTable->getInt(i, "等级");
		int iLevelUpCost = pTieJiangPuTable->getInt(i, "升级花费铜币");
		int iLevelUpSecond = pTieJiangPuTable->getInt(i, "升级时间");
		int iProductSecond = pTieJiangPuTable->getInt(i, "祭炼时间");
		string strProduct = pTieJiangPuTable->getString(i, "产物");
		int iOpenGeZiCount = pWuHunDianTable->getInt(i, "开启格子数");
		
		HeroSoulOrItem manorTieJiangPu;
		manorTieJiangPu.iLevel = iLevel;
		manorTieJiangPu.iLevelUpCost = iLevelUpCost;
		manorTieJiangPu.iLevelUpSecond = iLevelUpSecond;
		manorTieJiangPu.iProductSecond = iProductSecond;
		manorTieJiangPu.iOpenGeZiCount = iOpenGeZiCount;
		
		vector<int> productVec = TC_Common::sepstr<int>(strProduct,"#");
		assert(productVec.size()%3 == 0);
		for(size_t i = 0; i < productVec.size(); i = i + 3)
		{
			ManorItem item;
			item.iId = productVec[i];
			item.iCount = productVec[i+1];
			item.iChance = productVec[i+2];
			manorTieJiangPu.iTotalChance += item.iChance;
			manorTieJiangPu.manorItemList.push_back(item);
		}
		m_TieJaingPuMap.insert(make_pair(manorTieJiangPu.iLevel, manorTieJiangPu));
		
	}
	
	//
	ITimerComponent* timeComponent = getComponent<ITimerComponent>(COMPNAME_TimeAxis,IID_ITimerComponent);
	assert(timeComponent);
	m_LoadMachineTimerHandle = timeComponent->setTimer(this,1,1,"ManorFactory");


	//刷新消耗
	ITable *pRefreshCostTable = getCompomentObjectManager()->findTable(TABLENAME_ManorRefreshCost);
	assert(pRefreshCostTable);

	int iRefreshCostRecord = pRefreshCostTable->getRecordCount();
	for(int i = 0; i < iRefreshCostRecord; ++i)
	{
		string strRefreshTimes = pRefreshCostTable->getString(i, "刷新次数");
		int iRefreshCost = pRefreshCostTable->getInt(i , "消耗铜币");
		
		vector<int> refreshTimesVec = TC_Common::sepstr<int>(strRefreshTimes,"#");
		assert(refreshTimesVec.size() == 2);

		CRefreshCost *unit = new CRefreshCost();
		unit->iCost = iRefreshCost;
		unit->iLow = refreshTimesVec[0];
		unit->iHight = refreshTimesVec[1];

		m_RefreshCostList.push_back(unit);
		
	}
	
	loadManorProtect();
	
	return true;
}

void ManorFactory::loadManorProtect()
{
	ITable *pManorProtectTable = getCompomentObjectManager()->findTable(TABLENAME_ManorProtect);
	assert(pManorProtectTable);

	int iRecord = pManorProtectTable->getRecordCount();
	for(int i = 0; i < iRecord; ++i)
	{

		int iId = pManorProtectTable->getInt(i, "盾ID");
		int iPrice = pManorProtectTable->getInt(i, "消耗元宝");
		int iBuycdSecond = pManorProtectTable->getInt(i, "购买CD");
		int iProtectSecond = pManorProtectTable->getInt(i, "保护持续时间");

		ManorProtect protect;

		protect.iProtectId = iId;
		protect.iPrice = iPrice;
		protect.iBuycdSecond = iBuycdSecond;
		protect.iProtectSecond = iProtectSecond;
		
		m_ManorProtect.insert(make_pair(iId, protect));
	}
	
	
}

bool ManorFactory::getManorProtectById(int id, ManorProtect& protect)
{
	map<int, ManorProtect>::iterator iter = m_ManorProtect.find(id);
	if(iter == m_ManorProtect.end())
	{
		return false;
	}
	protect = iter->second;
	return true;
}


int  ManorFactory::getRefreshTieJiangPuOrWuHunDianCost(int iNowRefreshTimes)
{
	for(size_t i =0; i < m_RefreshCostList.size(); ++i)
	{
		const CRefreshCost & unit = *(m_RefreshCostList[i]);
		if(unit.iLow <= iNowRefreshTimes && iNowRefreshTimes <= unit.iHight)
		{
			return unit.iCost;
		}
	}
	return -1;
}




bool ManorFactory::getActorOpenSilverResList(vector<ManorRes>& openResVec,HEntity hEntity)
{
	IEntity *pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);
	int iActorLevel = pEntity->getProperty(PROP_ENTITY_LEVEL,0);
	assert(iActorLevel);

	size_t silverResSize = m_ManorSilverRes.size();
	for(size_t i = 0; i < silverResSize; ++i)
	{
		ManorRes res = m_ManorSilverRes[i];
		if(res.iOpenLevel <= iActorLevel)
		{
			openResVec.push_back(res);
		}
	}
	return true;
}

bool ManorFactory::getActorOPenHeroExpResList(vector<ManorRes>& openResVec,HEntity hEntity)
{
	IEntity *pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);
	int iActorLevel = pEntity->getProperty(PROP_ENTITY_LEVEL,0);
	assert(iActorLevel);

	size_t heroExpResSize = m_ManorHeroExpRes.size();
	for(size_t i = 0; i < heroExpResSize; ++i)
	{
		ManorRes res = m_ManorHeroExpRes[i];
		if(res.iOpenLevel <= iActorLevel)
		{
			openResVec.push_back(res);
		}
	}
	return true;	
}

bool ManorFactory::getActorSilverResByIdAndLevel(ManorResUnit& unit, int ResId, int iResLevel)
{
	ResDetailMap::iterator iter = m_SilverResMap.find(ResId);
	if(iter == m_SilverResMap.end())
	{
		return false;
	}

	map<int , ManorResUnit>::iterator iter2 = (iter->second).find(iResLevel);
	if(iter2 == (iter->second).end())
	{
		return false;
	}

	unit = iter2->second;
	return true;
	
}

bool ManorFactory::getActorHeroExpResByIdAndLevel(ManorResUnit& unit, int ResId, int iResLevel)
{
	ResDetailMap::iterator iter = m_HeroExpResMap.find(ResId);
	if(iter == m_HeroExpResMap.end())
	{
		return false;
	}

	map<int , ManorResUnit>::iterator iter2 = (iter->second).find(iResLevel);
	if(iter2 == (iter->second).end())
	{
		return false;
	}

	unit = iter2->second;
	return true;
}

bool ManorFactory::getActorWuHunDianByLevel(HeroSoulOrItem& unit, int iLevel)
{
	map<int, HeroSoulOrItem>::iterator  iter =  m_WuHunDianMap.find(iLevel);
	if(iter == m_WuHunDianMap.end())
	{
		return false;
	}
	unit = iter->second;
	return true;
}

bool ManorFactory::getActorTieJiangPuByLevel(HeroSoulOrItem& unit, int iLevel)
{
	map<int, HeroSoulOrItem>::iterator  iter =  m_TieJaingPuMap.find(iLevel);
	if(iter == m_TieJaingPuMap.end())
	{
		return false;
	}
	unit = iter->second;
	return true;
}

void ManorFactory::getWuHunByLevel(vector<ManorItem>& wuHunList, int iCount, int level)
{
	HeroSoulOrItem unit;
	bool res = getActorWuHunDianByLevel(unit,level);
	assert(res);

	unit.randManorItem(wuHunList,iCount);
}

void ManorFactory::getTieJiangPuByLevel(vector<ManorItem>& tieJiangPuList, int iCount, int level)
{
	HeroSoulOrItem unit;
	bool res = getActorTieJiangPuByLevel(unit,level);
	assert(res);

	unit.randManorItem(tieJiangPuList,iCount);
}



void ManorFactory::onTimer(int nEventId)
{
	if(1 == nEventId)
	{
		loadMachine();
	}
	else if (2 == nEventId)
	{
		resetMachine();
	}
}

void ManorFactory::resetMachine()
{
	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	if( pZoneTime->IsInSameDay(dwChgTime, pZoneTime->GetCurSecond()))
	{
		return;
	}
	for(size_t i = 0; i < m_machinePlayer.size(); i++)
	{
		IEntity* pTmp = getEntityFromHandle(m_machinePlayer[i]);
		assert(pTmp);
		IManorSystem *pSystem = static_cast<IManorSystem*>(pTmp->querySubsystem(IID_IManorSystem));
		assert(pSystem);
		pSystem->resetMachineData();
	}
	dwChgTime =  pZoneTime->GetCurSecond();
}

void ManorFactory::loadMachine()
{
	ITable* pTable = getCompomentObjectManager()->findTable("Machine");
	assert(pTable);

	IJZEntityFactory* pJZEntityFactory = getComponent<IJZEntityFactory>(COMPNAME_EntityFactory, IID_IJZEntityFactory);
	assert(pJZEntityFactory);

	IRandom* pRandom = getComponent<IRandom>(COMPNAME_Random, IID_IMiniAprRandom);
	assert(pRandom);

	set<string> machineAccountList;
	set<string> machineNameList;
	for(int i = 0; i < pTable->getRecordCount(); i ++)
	{
		string strAccount = pTable->getString(i, "帐号");
		string strName = pTable->getString(i, "名字");
		int iBaseSilver = pTable->getInt(i, "当前铜币初始储量");
		int iBaseHeroExo = pTable->getInt(i, "当前修为初始储量");
		int iSilverLevel = pTable->getInt(i,"金矿等级");
		int iHeroExpLevel = pTable->getInt(i, "修为矿等级");
		string strHeroIDList = pTable->getString(i, "初始英雄");
		int iLevel = pTable->getInt(i, "等级");
		if(iLevel == 0) iLevel = 1;

		assert(machineAccountList.find(strAccount) == machineAccountList.end() );
		assert(machineNameList.find(strName) == machineNameList.end() );

		machineAccountList.insert(strAccount);
		machineNameList.insert(strName);

		vector<int> heroIDList = TC_Common::sepstr<int>(strHeroIDList, "#");
		assert(heroIDList.size() % 4 == 0);
		int iFormationPos = 0;
		vector<CreateHeroDesc> createHeroList;
		for(size_t ii = 0; ii < heroIDList.size(); ii = ii +4)
		{
			// 这几个位置被屏蔽了
			if(iFormationPos % 3 == 2) 
			{
				iFormationPos++;
			}

			if(iFormationPos >= GSProto::MAX_BATTLE_MEMBER_SIZE/2)
			{
				break;
			}
		
			CreateHeroDesc tmpHeroDesc;
			tmpHeroDesc.iHeroID = heroIDList[ii];
			tmpHeroDesc.iFormationPos = iFormationPos++;
			tmpHeroDesc.iLevel = heroIDList[ ii + 1];
			tmpHeroDesc.iLevelStep =  heroIDList[ ii + 2];

			// 后续给策划配置
			tmpHeroDesc.iQuality = heroIDList[ ii + 3];;
			createHeroList.push_back(tmpHeroDesc);
		}
		
		IEntity* pMachine = pJZEntityFactory->createMachine(strAccount, strName, createHeroList);
		//pMachine->setProperty(PROP_ACTOR_SILVER, iBaseSilver);
		pMachine->setProperty(PROP_ENTITY_ISMACHINE, 1);
		pMachine->setProperty(PROP_ENTITY_LEVEL, iLevel);

		int iRandV = pRandom->random() % createHeroList.size();
		pMachine->setProperty(PROP_ENTITY_ACTOR_HEADTYPE, GSProto::en_HeadType_Hero);
		pMachine->setProperty(PROP_ENTITY_ACTOR_HEAD, createHeroList[iRandV].iHeroID);

		stringstream ss;
		ss<<iBaseSilver<<"#"<<iBaseHeroExo<<"#"<<iSilverLevel<<"#"<<iHeroExpLevel;
	
		
		IManorSystem* pManorSys = static_cast<IManorSystem*>(pMachine->querySubsystem(IID_IManorSystem));
		assert(pManorSys);
		pManorSys->initMachineData(ss.str());
	
		m_machinePlayer.push_back(pMachine->getHandle() );
		
	}

	ITimerComponent* timeComponent = getComponent<ITimerComponent>(COMPNAME_TimeAxis,IID_ITimerComponent);
	assert(timeComponent);
	timeComponent->killTimer( m_LoadMachineTimerHandle);
	
	// 这里才开始
	m_ResetMachineTimerHandle = timeComponent->setTimer(this,2,300*1000,"ManorFactory");
	
	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime,IID_IZoneTime);
	assert(pZoneTime);
	dwChgTime = pZoneTime->GetCurSecond();
	
}



