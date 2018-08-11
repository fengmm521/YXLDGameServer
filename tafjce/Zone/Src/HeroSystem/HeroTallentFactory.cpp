#include "HeroSystemPch.h"
#include "HeroTallentFactory.h"
#include "IItemFactory.h"
#include "ItemProperty.h"

extern "C" IComponent* createHeroTallentFactory(Int32)
{
	return new HeroTallentFactory;
}


bool HeroTallentFactory::initlize(const PropertySet& propSet)
{
	loadHeroConvert();
	loadHeroTallent();

	loadQualityPropParam();
	loadQualityConsume();
	loadFavoriteCombine();

	return true;
}

void HeroTallentFactory::loadHeroConvert()
{
	ITable* pCvtTable = getCompomentObjectManager()->findTable(TABLENAME_HeroConvert);
	assert(pCvtTable);

	for(int i = 0; i < pCvtTable->getRecordCount(); i++)
	{
		HeroConvertCfg tmpCvtCfg;
		tmpCvtCfg.iSrcHeroID = pCvtTable->getInt(i, "ID");

		string strDesc = pCvtTable->getString(i, "转换列表");
		vector<int> idList = TC_Common::sepstr<int>(strDesc, "#");

		assert(idList.size() % 2 == 0);

		for(size_t iIndex = 0; iIndex < idList.size()/2; iIndex++)
		{
			tmpCvtCfg.targetChanceList[idList[iIndex*2]] = idList[iIndex*2+1];
		}
		
		m_mapConvertCfg[tmpCvtCfg.iSrcHeroID] = tmpCvtCfg;
	}
}

void HeroTallentFactory::loadHeroTallent()
{
	ITable* pTallentTb = getCompomentObjectManager()->findTable("HeroTallent");
	assert(pTallentTb);

	for(int i = 0; i < pTallentTb->getRecordCount(); i++)
	{
		HeroTallentCfg tmpTallent;
		tmpTallent.iTallentID = pTallentTb->getInt(i, "ID");
		tmpTallent.iLevel = pTallentTb->getInt(i, "等级");
		tmpTallent.iQuality = pTallentTb->getInt(i, "品质");

		string strUpgradeMaterial = pTallentTb->getString(i, "升级材料");
		vector<int> upgradeMaterialList = TC_Common::sepstr<int>(strUpgradeMaterial, "#");
		assert(upgradeMaterialList.size() % 2 == 0);
		
		for(size_t iIndex = 0; iIndex < upgradeMaterialList.size()/2; iIndex++)
		{
			tmpTallent.upgradeMaterialMap[upgradeMaterialList[iIndex*2] ] = upgradeMaterialList[iIndex*2 + 1];
		}
		
		tmpTallent.iUpgradeSilver = pTallentTb->getInt(i, "升级铜币");
		tmpTallent.iUpgradeQuality = pTallentTb->getInt(i, "升级等阶");

		string strEffectIDList = pTallentTb->getString(i, "技能效果");
		tmpTallent.effectList = TC_Common::sepstr<int>(strEffectIDList, "#");
		tmpTallent.iMasterLv = pTallentTb->getInt(i, "主公等级要求");

		assert(m_mapTallentCfg[tmpTallent.iTallentID].find(tmpTallent.iLevel) == m_mapTallentCfg[tmpTallent.iTallentID].end() );
		m_mapTallentCfg[tmpTallent.iTallentID][tmpTallent.iLevel] = tmpTallent;
	}
}

const HeroConvertCfg* HeroTallentFactory::queryConvertCfg(int iHeroID)
{
	map<int, HeroConvertCfg>::iterator it = m_mapConvertCfg.find(iHeroID);
	if(it != m_mapConvertCfg.end() )
	{
		return &(it->second);
	}

	return NULL;
}


const HeroTallentCfg* HeroTallentFactory::queryTallentCfg(int iTallentID, int iLevel)
{
	map<int, map<int, HeroTallentCfg> >::iterator it = m_mapTallentCfg.find(iTallentID);
	if(it == m_mapTallentCfg.end() )
	{
		return NULL;
	}

	const map<int, HeroTallentCfg>& refLevelMap = it->second;
	map<int, HeroTallentCfg>::const_iterator levelIt = refLevelMap.find(iLevel);
	if(levelIt == refLevelMap.end() )
	{
		return NULL;
	}

	return &(levelIt->second);
}

const FavoriteConsume* HeroTallentFactory::queryQualityConsume(int iHeroID, int iQuality)
{
	map<int, map<int, FavoriteConsume> >::iterator it = m_mapQualityUpgradeCfg.find(iHeroID);
	if(it == m_mapQualityUpgradeCfg.end() )
	{
		return NULL;
	}

	map<int, FavoriteConsume>& refFavoriteConsume = it->second;
	map<int, FavoriteConsume>::iterator favIt = refFavoriteConsume.find(iQuality);

	if(favIt != refFavoriteConsume.end() )
	{
		return &(favIt->second);
	}

	return NULL;
}

const QualityProp* HeroTallentFactory::queryQualityPropMap(int iHeroID, int iQuality)
{
	map<int, map<int, QualityProp> >::iterator it = m_mapQualityPropCfg.find(iHeroID);
	if(it == m_mapQualityPropCfg.end() )
	{
		return NULL;
	}

	map<int, QualityProp>& refQualityPropCfg = it->second;

	map<int, QualityProp>::iterator favIt = refQualityPropCfg.find(iQuality);
	if(favIt != refQualityPropCfg.end() )
	{
		return &(favIt->second);
	}

	return NULL;
}

int HeroTallentFactory::queryQualityPropParam(int iQuality)
{
	map<int, int>::iterator it = m_mapPropParamCfg.find(iQuality);
	if(it != m_mapPropParamCfg.end() )
	{
		return it->second;
	}

	return 0;
}

const FavoriteCombine* HeroTallentFactory::queryFavoriteCombineInfo(int iItemID)
{
	map<int, FavoriteCombine>::iterator it = m_mapFavoriteCombineCfg.find(iItemID);
	if(it != m_mapFavoriteCombineCfg.end() )
	{
		return &(it->second);
	}

	return NULL;
}


void HeroTallentFactory::loadQualityPropParam()
{
	ITable* pTable = getCompomentObjectManager()->findTable("QualityProp");
	assert(pTable);

	for(int i = 0; i < pTable->getRecordCount(); i++)
	{
		int iQuality = pTable->getInt(i, "品质ID");
		int iPropParam = pTable->getInt(i, "品质ID");
	
		m_mapPropParamCfg[iQuality] = iPropParam;
	}
}

void HeroTallentFactory::loadQualityConsume()
{
	ITable* pTable = getCompomentObjectManager()->findTable("HeroQuality");
	assert(pTable);

	for(int i = 0; i < pTable->getRecordCount(); i++)
	{
		int iHeroID = pTable->getInt(i, "英雄ID");
		for(int iIndex = 0; iIndex < 20; iIndex++)
		{
			stringstream ss;
			ss<<"品质"<<(iIndex+1);

			string strKey = ss.str();

			string strConsumeItemList = pTable->getString(i, strKey);

			if(strConsumeItemList.size() == 0) continue;


			vector<int> tmpItemList = TC_Common::sepstr<int>(strConsumeItemList, "#");

			for(size_t iFPos = 0; iFPos < tmpItemList.size(); iFPos++)
			{
				m_mapQualityUpgradeCfg[iHeroID][iIndex+1].favoriteItemList[iFPos] = tmpItemList[iFPos];
			}
			

			// 计算属性,赋值
			parseItemPropMap(tmpItemList, m_mapQualityPropCfg[iHeroID][iIndex+1].addPropMap);

			if(iIndex > 0)
			{
				const map<int, int>& refPrePropMap = m_mapQualityPropCfg[iHeroID][iIndex].addPropMap;
				map<int, int>& curPropMap = m_mapQualityPropCfg[iHeroID][iIndex+1].addPropMap;

				for(map<int, int>::const_iterator it = refPrePropMap.begin(); it != refPrePropMap.end(); it++)
				{
					curPropMap[it->first] += it->second;
				}
			}
		}
	}
}


void HeroTallentFactory::parseItemPropMap(const vector<int>& itemList, map<int, int>& addPropMap)
{
	IItemFactory* pItemFactory = getComponent<IItemFactory>(COMPNAME_ItemFactory, IID_IItemFactory);
	assert(pItemFactory);

	
	for(size_t i = 0; i < itemList.size(); i++)
	{
		int iItemID = itemList[i];

		const map<int, int>* pItemPropMap = pItemFactory->queryFavoriteProp(iItemID);
		assert(pItemPropMap);

		for(map<int, int>::const_iterator it = pItemPropMap->begin(); it != pItemPropMap->end(); it++)
		{
			addPropMap[it->first] += it->second;
		}
	}
}


void HeroTallentFactory::loadFavoriteCombine()
{
	ITable* pTable = getCompomentObjectManager()->findTable("FavoriteCombine");
	assert(pTable);

	for(int i = 0; i < pTable->getRecordCount(); i++)
	{
		FavoriteCombine tmpCombineInfo;
		tmpCombineInfo.iItemID = pTable->getInt(i, "喜好品ID");
		tmpCombineInfo.iNeedSilver = pTable->getInt(i, "消耗铜币");
		
		for(int iIndex = 0; iIndex < 4; iIndex++)
		{
			stringstream ss;
			ss<<"消耗材料"<<(iIndex+1);
			string strKey = ss.str();

			int iItemID = pTable->getInt(i, strKey);

			if(0 == iItemID) continue;
			
			ss.str("");
			ss.clear();

			ss<<"消耗材料"<<(iIndex+1)<<"数量";
			strKey = ss.str();

			int iCount = pTable->getInt(i, strKey);
			if(0 == iCount) continue;
			
			tmpCombineInfo.needItemMap[iItemID] += iCount;
		}

		m_mapFavoriteCombineCfg[tmpCombineInfo.iItemID] = tmpCombineInfo;
	}
}




