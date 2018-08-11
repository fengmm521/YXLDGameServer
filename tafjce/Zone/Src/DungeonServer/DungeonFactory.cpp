#include "DungeonServerPch.h"
#include "DungeonFactory.h"

extern "C" IComponent* createDungeonFactory(Int32)
{
	return new DungeonFactory;
}


DungeonFactory::DungeonFactory()
{
}

DungeonFactory::~DungeonFactory()
{
}

bool DungeonFactory::initlize(const PropertySet& propSet)
{
	loadSection();
	loadScene();
	loadPerfectAward();

	return true;
}

const DungeonSection* DungeonFactory::querySection(int iSectionID)
{
	MapSection::iterator it = m_sectionMap.find(iSectionID);
	if(it == m_sectionMap.end() )
	{
		return NULL;
	}

	return &(it->second);
}

const DungeonScene* DungeonFactory::queryScene(int iSceneID)
{
	MapScene::iterator it = m_sceneMap.find(iSceneID);
	if(it == m_sceneMap.end() )
	{
		return NULL;
	}

	return &(it->second);
}

int DungeonFactory::getSceneOwnScetion(int iSceneID)
{
	const DungeonScene* pDungeonScene = queryScene(iSceneID);
	if(!pDungeonScene) return 0;

	return pDungeonScene->iOwnSectionID;
}


int DungeonFactory::getFirstSection()
{
	if(m_sectionIDList.size() == 0)
	{
		return 0;
	}

	return m_sectionIDList.front();
}

int DungeonFactory::getFirstScene(int iSectionID)
{
	MapSection::iterator it = m_sectionMap.find(iSectionID);
	if(it == m_sectionMap.end() )
	{
		return 0;
	}

	const DungeonSection& section = it->second;

	if(0 == section.sceneIDList.size() )
	{
		return 0;
	}

	return section.sceneIDList.front();
}

int DungeonFactory::getNextSection(int iScetionID)
{
	const DungeonSection* pSectionCfg = querySection(iScetionID);
	if(!pSectionCfg) return 0;

	if(pSectionCfg->iSectionType != GSProto::en_SectionType_Normal)
	{
		return 0;
	}

	vector<int>::iterator it = find(m_sectionIDList.begin(), m_sectionIDList.end(), iScetionID);
	assert(it != m_sectionIDList.end() );

	while(++it != m_sectionIDList.end() )
	{
		int iTmpSectionID = *it;
		const DungeonSection* pTmpDungeonSectionCfg = querySection(iTmpSectionID);
		assert(pTmpDungeonSectionCfg);

		if(pTmpDungeonSectionCfg->iSectionType == GSProto::en_SectionType_Normal)
		{
			return iTmpSectionID;
		}
	}

	return 0;
}

int DungeonFactory::getNextScene(int iSceneID)
{
	const DungeonScene* pDungeonScene = queryScene(iSceneID);
	assert(pDungeonScene);

	const DungeonSection* pSection = querySection(pDungeonScene->iOwnSectionID);
	assert(pSection);

	vector<int>::const_iterator it = std::find(pSection->sceneIDList.begin(), pSection->sceneIDList.end(), iSceneID);
	assert(it != pSection->sceneIDList.end() );

	size_t iTmpDis = std::distance(pSection->sceneIDList.begin(), it);

	if(iTmpDis < (pSection->sceneIDList.size() - 1))
	{
		int iNewSceneID = pSection->sceneIDList[iTmpDis+1];
		return iNewSceneID;
	}

	return 0;
}


const PerfectAwardCfg* DungeonFactory::queryPerfectAward(int iID)
{
	map<int, PerfectAwardCfg>::iterator it = m_perfectAwardMap.find(iID);
	if(it != m_perfectAwardMap.end() )
	{
		return &(it->second);
	}

	return NULL;
}


void DungeonFactory::loadPerfectAward()
{
	ITable* pTable = getCompomentObjectManager()->findTable("PerfectPassAward");
	assert(pTable);

	int iRecordCount = pTable->getRecordCount();
	for(int i = 0; i < iRecordCount; i++)
	{
		PerfectAwardCfg tmpAward;
		tmpAward.iID = pTable->getInt(i, "完美通关奖励ID");
		tmpAward.iType = pTable->getInt(i, "奖励类型");
		tmpAward.iItemID = pTable->getInt(i, "奖励ID");
		tmpAward.iCount = pTable->getInt(i, "数量");
		tmpAward.iLevelStep = pTable->getInt(i, "等阶");

		m_perfectAwardMap[tmpAward.iID] = tmpAward;
	}
}


void DungeonFactory::loadSection()
{
	ITable* pSectionTb = getCompomentObjectManager()->findTable(TABLENAME_DungeonSection);
	assert(pSectionTb);

	int iRecordCount = pSectionTb->getRecordCount();

	int iDependSection = 0;
	for(int i = 0; i < iRecordCount; i++)
	{
		DungeonSection tmpSection;
		tmpSection.iSectionID = pSectionTb->getInt(i, "章节ID");
		tmpSection.iDependSecionID = iDependSection;//pSectionTb->getInt(i, "前置章节");
		tmpSection.iSectionType = pSectionTb->getInt(i, "类型");
		tmpSection.iLevelCondition = pSectionTb->getInt(i, "开启等级");

		string strSceneIDList = pSectionTb->getString(i, "关卡列表");
		tmpSection.sceneIDList = TC_Common::sepstr<int>(strSceneIDList, Effect_MagicSep);
		string strPerfectAward = pSectionTb->getString(i, "完美通关奖励", "");

		tmpSection.perfectIDList = TC_Common::sepstr<int>(strPerfectAward, Effect_MagicSep);

		assert(m_sectionMap.find(tmpSection.iSectionID) == m_sectionMap.end() );

		m_sectionMap[tmpSection.iSectionID] = tmpSection;

		for(vector<int>::iterator it = tmpSection.sceneIDList.begin(); it != tmpSection.sceneIDList.end(); it++)
		{
			m_sceneOwnMap[*it] = tmpSection.iSectionID;
		}

		m_sectionIDList.push_back(tmpSection.iSectionID);
		iDependSection = tmpSection.iSectionID;

		m_sectionIDListMap[tmpSection.iSectionType].push_back(tmpSection.iSectionID);
	}
}

void DungeonFactory::loadScene()
{
	ITable* pSceneTb = getCompomentObjectManager()->findTable(TABLENAME_DungeonScene);
	assert(pSceneTb);

	int iRecordCount = pSceneTb->getRecordCount();

	int iDependSceenID = 0;
	for(int i = 0; i < iRecordCount; i++)
	{
		DungeonScene tmpScene;
		tmpScene.iSceneID = pSceneTb->getInt(i, "ID");
		tmpScene.iPreSceneID = iDependSceenID;
		
		//tmpScene.iMonsterGrpID = pSceneTb->getInt(i, "关卡怪物");
		string strMonsterGrpList = pSceneTb->getString(i, "关卡怪物");
		tmpScene.monsterGrpList = TC_Common::sepstr<int>(strMonsterGrpList, "#");

		tmpScene.iExp = pSceneTb->getInt(i, "通关经验");
		tmpScene.iHeroGetExp = pSceneTb->getInt(i, "英雄修为");
		tmpScene.iSilver = pSceneTb->getInt(i, "通关银两");
		tmpScene.bConditionAddHero = pSceneTb->getInt(i, "英雄背包未满") == 1;

		string strFirstDropList = pSceneTb->getString(i, "首次掉落");
		tmpScene.firstDropIDList = TC_Common::sepstr<int>(strFirstDropList, Effect_MagicSep);

		string strDropList = pSceneTb->getString(i, "关卡掉落");
		tmpScene.dropIDList = TC_Common::sepstr<int>(strDropList, Effect_MagicSep);

		//firstPerfectIDList
		string strFirstPerfectList = pSceneTb->getString(i, "首次三星通关掉落");
		tmpScene.firstPerfectIDList = TC_Common::sepstr<int>(strFirstPerfectList, Effect_MagicSep);

		string strBeginDlgList = pSceneTb->getString(i, "开场对话");
		tmpScene.dlgList = TC_Common::sepstr<int>(strBeginDlgList, Effect_MagicSep);

		tmpScene.bBeginDlg = pSceneTb->getInt(i, "是否开场播放") != 0;

		tmpScene.iLimitPassCount = pSceneTb->getInt(i, "限制通关次数");

		string strFpChanceList = pSceneTb->getString(i, "翻卡概率");
		if(strFpChanceList.size() > 0)
		{
			parseFpChanceMap(strFpChanceList, tmpScene.fpChanceList);
		}

		string strEliteEquipChance = pSceneTb->getString(i, "翻卡精英装备");
		if(strEliteEquipChance.size() > 0)
		{
			parseFpEquipChance(strEliteEquipChance, tmpScene.eliteEquipList);
		}

		string strNormalEquipChance = pSceneTb->getString(i, "翻卡普通装备");
		if(strNormalEquipChance.size() > 0)
		{
			parseFpEquipChance(strNormalEquipChance, tmpScene.normalEquipList);
		}

		if(m_sceneOwnMap.find(tmpScene.iSceneID) == m_sceneOwnMap.end() )
		{
			continue;
		}

		tmpScene.iOwnSectionID = m_sceneOwnMap[tmpScene.iSceneID];

		assert(m_sceneMap.find(tmpScene.iSceneID) == m_sceneMap.end() );

		m_sceneMap[tmpScene.iSceneID] = tmpScene;

		iDependSceenID = tmpScene.iSceneID;
	}
}


void DungeonFactory::parseFpChanceMap(const std::string& strFpChance, map<int, int>& chanceMap)
{
	vector<int> tmpChanceList = TC_Common::sepstr<int>(strFpChance, Effect_MagicSep);
	assert(tmpChanceList.size() % 2 == 0);

	for(size_t i = 0; i < tmpChanceList.size()/2; i++)
	{
		chanceMap[tmpChanceList[i*2]] = tmpChanceList[i*2+1];
	}
}


vector<int> DungeonFactory::getSectionList(int iSectionType)
{
	map<int, vector<int> >::iterator it = m_sectionIDListMap.find(iSectionType);
	if(it == m_sectionIDListMap.end() )
	{
		vector<int> resultList;
		return resultList;
	}


	return it->second;
}


void DungeonFactory::parseFpEquipChance(const std::string& strChance, vector<ChanceCfg>& equipChanceList)
{
	vector<int> tmpChanceList = TC_Common::sepstr<int>(strChance, Effect_MagicSep);
	assert(tmpChanceList.size() % 2 == 0);

	for(size_t i = 0; i <tmpChanceList.size()/2; i++)
	{
		ChanceCfg chanceCfg;
		chanceCfg.iID = tmpChanceList[i*2];
		chanceCfg.iChance = tmpChanceList[i*2+1];
		equipChanceList.push_back(chanceCfg);
	}
}



