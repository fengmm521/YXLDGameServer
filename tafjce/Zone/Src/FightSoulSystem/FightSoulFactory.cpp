#include "FightSoulSystemPch.h"
#include "FightSoulFactory.h"
#include "FightSoul.h"

extern "C" IComponent* createFightSoulFactory(Int32)
{
	return new FightSoulFactory;
}


FightSoulFactory::FightSoulFactory()
{
}

FightSoulFactory::~FightSoulFactory()
{
}

bool FightSoulFactory::initlize(const PropertySet& propSet)
{
	loadFightSoul();
	loadQualityExp();
	loadPractice();
	loadPracticeSilverCost();
	loadFightSoulExchange();
	loadHeroEquipLimieMap();

	return true;
}

const FightSoulData* FightSoulFactory::queryFightSoulData(int iFightSoulID)
{
	MapFightSoulData::iterator it = m_mapFightSoulData.find(iFightSoulID);
	if(it==m_mapFightSoulData.end() )
	{
		return NULL;
	}

	return &(it->second);
}


IFightSoul* FightSoulFactory::createFightSoul(int iFightSoulID)
{
	const FightSoulData* pCfgData = queryFightSoulData(iFightSoulID) ;
	if(!pCfgData)
	{
		return NULL;
	}

	FightSoul* pFightSoul = new FightSoul;
	pFightSoul->m_iFightSoulID = iFightSoulID;

	// 经验不在这显示
	//pFightSoul->m_iExp = pCfgData->iInitExp;

	return pFightSoul;
}


IFightSoul* FightSoulFactory::createFightSoulFromDB(const ServerEngine::FightSoulItem& fightSoulData)
{
	if(!queryFightSoulData(fightSoulData.iBaseID) )
	{
		return NULL;
	}

	FightSoul* pFightSoul = new FightSoul;
	pFightSoul->m_iFightSoulID = fightSoulData.iBaseID;
	pFightSoul->m_iExp = fightSoulData.iExp;
	pFightSoul->m_bLocked = fightSoulData.bLocked == 1;

	return pFightSoul;
}

const map<int, int>* FightSoulFactory::getEffectMap(int iFightSoulID, int iLevel)
{
	MapFightSoulData::iterator it = m_mapFightSoulData.find(iFightSoulID);
	if(it == m_mapFightSoulData.end() )
	{
		return NULL;
	}

	const FightSoulData& refFightSoulData = it->second;

	int iIndex = iLevel - 1;
	if( (iIndex < 0) || (iIndex >= (int)refFightSoulData.levelPropList.size() ) )
	{
		return NULL;
	}

	return &(refFightSoulData.levelPropList[iIndex]);
}

int FightSoulFactory::calcLevel(int iFightSoulID, int iExp)
{
	MapFightSoulData::iterator it = m_mapFightSoulData.find(iFightSoulID);
	assert(it != m_mapFightSoulData.end() );

	const FightSoulData& refFightSoulData = it->second;
	int iQuality = refFightSoulData.iQuality;

	MapQualityExpTable::iterator expIt = m_tbQualityExp.find(iQuality);
	assert(expIt != m_tbQualityExp.end() );

	const vector<int>& expTable = expIt->second;
	vector<int>::const_iterator findIt = upper_bound(expTable.begin(), expTable.end(), iExp);

	int iLevel = (int)(findIt - expTable.begin()) + 1;
	
	return iLevel;
}

int FightSoulFactory::getLevelNeedExp(int iFightSoulID, int iLevel)
{
	MapFightSoulData::iterator it = m_mapFightSoulData.find(iFightSoulID);
	assert(it != m_mapFightSoulData.end() );

	const FightSoulData& refFightSoulData = it->second;
	int iQuality = refFightSoulData.iQuality;

	MapQualityExpTable::iterator expIt = m_tbQualityExp.find(iQuality);
	assert(expIt != m_tbQualityExp.end() );

	const vector<int>& expTable = expIt->second;
	assert(expTable.size() > 0);

	if(iLevel <2) return 0;

	if(iLevel >= (int)expTable.size() + 1)
	{
		return expTable.back();
	}

	return expTable[iLevel-2];
}


int FightSoulFactory::getMaxExpByQuality(int iQuality)
{
	MapQualityExpTable::iterator expIt = m_tbQualityExp.find(iQuality);	
	assert(expIt != m_tbQualityExp.end() );

	const vector<int>& expTable = expIt->second;
	assert(expTable.size() );

	return expTable.back();
}


void FightSoulFactory::loadFightSoul()
{
	ITable* pFightSoulTb = getCompomentObjectManager()->findTable("FightSoul");
	assert(pFightSoulTb);

	int iRecordCount = pFightSoulTb->getRecordCount();
	for(int i = 0; i < iRecordCount; i++)
	{
		FightSoulData tmpData;
		
		tmpData.iFightSoulID = pFightSoulTb->getInt(i, "ID");
		tmpData.iQuality = pFightSoulTb->getInt(i, "品质");
		tmpData.iTypeID = pFightSoulTb->getInt(i, "类型ID");
		tmpData.iInitExp = pFightSoulTb->getInt(i, "初始经验");
		tmpData.strName = pFightSoulTb->getString(i, "名称");

		for(int iLevel = 0; iLevel < 20; iLevel++)
		{
			stringstream ss;
			ss<<(iLevel+1)<<"级属性";
			string strKey = ss.str();
			string strValueList = pFightSoulTb->getString(i, strKey);

			map<int, int> tmpPropMap;
			bool bResult = parsePropMap(strValueList, tmpPropMap);
			assert(bResult);

			tmpData.levelPropList.push_back(tmpPropMap);
		}

		m_mapFightSoulData[tmpData.iFightSoulID] = tmpData;
	}
}

void FightSoulFactory::loadFightSoulExchange()
{
	ITable* pExchangeTb = getCompomentObjectManager()->findTable("FightSoulExchange");
	assert(pExchangeTb);

	int iRecordCount = pExchangeTb->getRecordCount();

	for(int i = 0; i < iRecordCount; i++)
	{
		FightSoulExchange tmpData;
		tmpData.iExChangeID = pExchangeTb->getInt(i, "ID");
		tmpData.iTargetBaseID = pExchangeTb->getInt(i, "武魂ID");
		tmpData.iNeedChipCount = pExchangeTb->getInt(i, "需求碎片");

		assert(m_exchangeMap.find(tmpData.iExChangeID) == m_exchangeMap.end() );
		m_exchangeMap[tmpData.iExChangeID] = tmpData;
	}
}


const FightSoulExchange* FightSoulFactory::queryExchange(int iExchangeID)
{
	map<int, FightSoulExchange>::iterator it = m_exchangeMap.find(iExchangeID);
	if(it == m_exchangeMap.end() )
	{
		return NULL;
	}

	return &(it->second);
}


bool FightSoulFactory::parsePropMap(const string& strValue, map<int, int>& propMap)
{
	vector<string> paramList = TC_Common::sepstr<string>(strValue, "#");
	assert(paramList.size() % 2 == 0);

	IPropDefine* pFightSoulPropDef = getPropDefine("FightSoulPropDef");
	assert(pFightSoulPropDef);
	
	for(size_t i = 0; i < paramList.size()/2; i++)
	{
		string strPropName = paramList[i*2];
		int iValue = AdvanceAtoi(paramList[i*2+1]);

		int iTmpPropID = pFightSoulPropDef->getPropKey(strPropName);
		assert(iTmpPropID >= 0);

		assert(propMap.find(iTmpPropID) == propMap.end() );
		propMap[iTmpPropID] = iValue;
	}

	return true;
}

void FightSoulFactory::loadQualityExp()
{
	ITable* pQualityExpTb = getCompomentObjectManager()->findTable("FightSoulExp");
	assert(pQualityExpTb);

	int iRecordCount = pQualityExpTb->getRecordCount();
	for(int i = 0; i < iRecordCount; i++)
	{
		//int iLevel = pQualityExpTb->getInt(i, "等级");
		int iGrenExp = pQualityExpTb->getInt(i, "绿色经验");
		int iBlueExp = pQualityExpTb->getInt(i, "蓝色经验");
		int iPureExp = pQualityExpTb->getInt(i, "紫色经验");
		int iGoldExp = pQualityExpTb->getInt(i, "金色经验");

		m_tbQualityExp[GSProto::en_Quality_Green].push_back(iGrenExp);
		m_tbQualityExp[GSProto::en_Quality_Blue].push_back(iBlueExp);
		m_tbQualityExp[GSProto::en_Quality_Pure].push_back(iPureExp);
		m_tbQualityExp[GSProto::en_Quality_Gold].push_back(iGoldExp);
	}
}

void FightSoulFactory::loadPractice()
{
	ITable* pPracticeTb = getCompomentObjectManager()->findTable("FightSoulPractice");
	assert(pPracticeTb);

	int iRecordCount = pPracticeTb->getRecordCount();
	for(int i = 0; i < iRecordCount; i++)
	{
		FightPractice tmpPractice;
		tmpPractice.iPracticeGrade = pPracticeTb->getInt(i, "修炼境界");
		tmpPractice.iType = pPracticeTb->getInt(i, "修炼类型");
		tmpPractice.iNextGradeChance = pPracticeTb->getInt(i, "境界提升几率");

		string strChance = pPracticeTb->getString(i, "武魂获取");
		bool bResult = parseChaceList(strChance, tmpPractice.chanceList);
		assert(bResult);

		Int64 ddKey = (Int64)(((Int64)tmpPractice.iPracticeGrade<<32)|tmpPractice.iType);
		m_mapPractice[ddKey] = tmpPractice;
	}
}

bool FightSoulFactory::parseChaceList(const string& strChanceList, vector<FightSoulChance>& chanceList)
{
	vector<string> paramList = TC_Common::sepstr<string>(strChanceList, "#");
	assert(paramList.size() % 2 == 0);

	for(size_t i = 0; i < paramList.size()/2; i++)
	{
		FightSoulChance tmpChance;
		tmpChance.iFightSoulID = AdvanceAtoi(paramList[i*2]);
		tmpChance.iChance = AdvanceAtoi(paramList[i*2 + 1]);

		chanceList.push_back(tmpChance);
	}

	return true;
}

int FightSoulFactory::getNextGradeChance(int iGrade, int iType)
{
	// 如果已经到达最高阶段，打回
	if(GSProto::en_practice_Grade5 == iGrade)
	{
		return 0;
	}

	// 常规概率，查找
	Int64 ddKey = ((Int64)iGrade<<32)|iType;
	MapPractice::iterator it = m_mapPractice.find(ddKey);
	if(it == m_mapPractice.end() )
	{
		return 0;
	}

	const FightPractice& tmpPractice = it->second;
	
	return tmpPractice.iNextGradeChance;
}

const FightPractice* FightSoulFactory::querFightPracticeCfg(int iGrade, int iType)
{
	Int64 ddKey = ((Int64)iGrade<<32)|iType;
	MapPractice::iterator it = m_mapPractice.find(ddKey);
	if(it == m_mapPractice.end() )
	{
		return NULL;
	}

	return &(it->second);
}

int FightSoulFactory::calcPracticeSilver(int iGrade)
{
	map<int, int>::iterator it = m_silverCostMap.find(iGrade);
	assert(it != m_silverCostMap.end() );

	return it->second;
}

void FightSoulFactory::loadPracticeSilverCost()
{
	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	string strPracticeCost = pGlobalCfg->getString("武魂修炼消耗", "10#20#30#40#50");
	vector<int> costList = TC_Common::sepstr<int>(strPracticeCost, "#");

	int iBeginGrade = GSProto::en_practice_Grade1;
	for(int i = 0; i < (int)costList.size(); i++)
	{
		m_silverCostMap[iBeginGrade++] = costList[i];
	}
}

void FightSoulFactory::loadHeroEquipLimieMap()
{
	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	string strTmpData = pGlobalCfg->getString("武魂装备栏开启数目", "1#10#2#20#3#40#4#60#5#80#6#100#7#120#8#140");
	vector<int> tmpList = TC_Common::sepstr<int>(strTmpData, "#");
	assert(tmpList.size() % 2 == 0);

	for(size_t i = 0; i < tmpList.size()/2; i++)
	{
		int iValue = tmpList[i*2];
		int iLevel = tmpList[i*2+1];
		m_heroEquipLimitMap[iLevel] = iValue;
	}
}


int FightSoulFactory::calcEquipLimitCount(int iLevel)
{
	for(map<int, int>::iterator it = m_heroEquipLimitMap.begin(); it != m_heroEquipLimitMap.end(); it++)
	{
		int iTmpLevel = it->first;
		int iValue = it->second;

		if(iLevel <=  iTmpLevel)
		{
			return iValue;
		}
	}

	return 0;
}




