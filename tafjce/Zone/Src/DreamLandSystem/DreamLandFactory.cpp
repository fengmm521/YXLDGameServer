#include "DreamLandSystemPch.h"
#include "DreamLandFactory.h"


extern "C" IComponent* createDreamLandFactory(Int32 )
{
	return new DreamLandFactory;
}


DreamLandFactory::DreamLandFactory()
{

}

DreamLandFactory::~DreamLandFactory()
{
 	m_MapBoxInfo.clear();
	m_MapSceneInfo.clear();
	m_MapSectionInfo.clear();
}
	
bool DreamLandFactory::initlize(const PropertySet& propSet)
{
	loadData();
	
	return true;
}

void DreamLandFactory::loadData()
{
	ITable* pSectionTable = getCompomentObjectManager()->findTable( TABLENAME_DreamLandSection);
	assert( pSectionTable);

	ITable* pSceneTable = getCompomentObjectManager()->findTable( TABLENAME_DreamLandScene);
	assert( pSceneTable);

	ITable* pBoxTable = getCompomentObjectManager()->findTable( TABLENAME_DreamLandBox);
	assert( pBoxTable);

	for(int i = 0; i < pSectionTable->getRecordCount(); ++i)
	{
		DreamLandSection section;
		section.iSectionId = pSectionTable->getInt(i,"章节ID");

		string strScene = pSectionTable->getString(i,"关卡列表");
		assert(strScene.length()!=0);
		
		std::vector<int> vecScene = TC_Common::sepstr<int>(strScene,Effect_MagicSep);
		assert(vecScene.size() != 0);

		section.sceneIdVec = vecScene;

		m_MapSectionInfo.insert( make_pair(section.iSectionId, section));
		m_VecSectionIdVec.push_back( section.iSectionId);
	}


	for(int i =0; i < pSceneTable->getRecordCount(); ++i)
	{
		DreamLandScene scene;

		scene.iSceneId = pSceneTable->getInt(i, "关卡ID");
		scene.iSceneType = (enSceneType)pSceneTable->getInt(i,"关卡类型");

		string strFight = pSceneTable->getString(i,"战力范围");
		assert(strFight.length() !=0 );

		std::vector< int > vecFight = TC_Common::sepstr< int >(strFight, Effect_MagicSep);

		scene.iFightMinWeight = vecFight[0];
		scene.iFightMaxWeight = vecFight[1];

		string strAddFight = pSceneTable->getString(i,"怪物加成");
		std::vector< string > vecStr = TC_Common::sepstr<string>(strAddFight, Effect_MagicSep);

		for(size_t j =0; j < vecStr.size(); j = j + 2)
		{
			int key = 0;
			int value = 0;
			
			if(vecStr[j] == "生命")
			{
				key = GSProto::en_LifeAtt_HP;
			}
			if(vecStr[j] == "攻击")
			{
				key = GSProto::en_LifeAtt_Att;
			}
			
			value = TC_S2I(vecStr[j+1]);
			assert(key != 0);
			assert(value != 0);
			scene.addFightmap.insert( make_pair( key, value));
		}

		scene.iDreamLandBoxId = pSceneTable->getInt(i,"宝箱ID");
		scene.iMonsterGrupId = pSceneTable->getInt(i,"怪物组ID");
		printf("%d,%d,%d\n",i,scene.iDreamLandBoxId,scene.iMonsterGrupId );
		if(scene.iMonsterGrupId == 0)
		{
			scene.iSceneType = eSeceneType_Normal;
		}
		else
		{
			scene.iSceneType = eSeceneType_Special;	
		}

		m_MapSceneInfo.insert(make_pair(scene.iSceneId,scene));
	} 

	for(int i = 0; i < pBoxTable->getRecordCount(); ++i)
	{
		DreamLandBox box;

		box.iDreamLandBoxId = pBoxTable->getInt(i , "宝箱ID");

		string rewadStr = pBoxTable->getString(i,"固定奖励ID");
		printf("%d,%d,%d,%s\n",i,pBoxTable->getRecordCount(),box.iDreamLandBoxId,rewadStr.c_str() );
		assert(rewadStr.length() != 0);
		box.m_RewardVec = TC_Common::sepstr<int>(rewadStr,Effect_MagicSep);
		
		string str = pBoxTable->getString(i, "神秘奖励");
		box.m_magicRewardVec = TC_Common::sepstr<int>(str,Effect_MagicSep);
			
		m_MapBoxInfo.insert(make_pair(box.iDreamLandBoxId, box));
	}

	
}

bool DreamLandFactory::querySectionInfo(int iDreamLandId,DreamLandSection& section)
{
	std::map<int,DreamLandSection>::iterator iter = m_MapSectionInfo.find(iDreamLandId);
	if(iter == m_MapSectionInfo.end())
	{
		return false;
	}
	
	section = iter->second;

	return true;;
}

bool  DreamLandFactory::getFristSection(DreamLandSection& section)
{
	assert(m_VecSectionIdVec.size() != 0);
	int iFirstSecionID = m_VecSectionIdVec[0];

	return querySectionInfo(iFirstSecionID,section);
}

bool DreamLandFactory::querySceneInfo(int iSceneID,DreamLandScene& scene)
{
	std::map<int, DreamLandScene>::iterator iter = m_MapSceneInfo.find(iSceneID);

	if(iter == m_MapSceneInfo.end())
	{
		return false;
	}
	scene = iter->second;
	return true;
}

bool DreamLandFactory::queryDreamLandBoxInfo(int iDreamLandBoxId, DreamLandBox& box)
{
	std::map<int, DreamLandBox>::iterator iter = m_MapBoxInfo.find(iDreamLandBoxId);
	if(iter == m_MapBoxInfo.end())
	{
		return false;
	}

	box =  iter->second;
	return true;
}

int DreamLandFactory::queryNextSectionId(int iNowSectionId)
{
	std::vector<int>::iterator iter = find(m_VecSectionIdVec.begin(), m_VecSectionIdVec.end(), iNowSectionId);
	
	if( iter == m_VecSectionIdVec.end())
	{
		return 0;
	}

	size_t itempId = std::distance(m_VecSectionIdVec.begin(), iter);

	if( itempId < m_VecSectionIdVec.size()-1)
	{
		return m_VecSectionIdVec[itempId + 1];
	}

	return 0;
}

int DreamLandFactory::queryNextSceneinfo(int iSectionId, int iSceneId,DreamLandScene& scene)
{
	std::map<int,DreamLandSection>::iterator iter = m_MapSectionInfo.find(iSectionId);
	assert(iter != m_MapSectionInfo.end());

	vector<int>::iterator iter2 = std::find( iter->second.sceneIdVec.begin(), iter->second.sceneIdVec.end(),iSceneId);

	if(iter2 == iter->second.sceneIdVec.end())
	{
		return -1;
	}

	int sceneId = *(++iter2);

	bool res = querySceneInfo(sceneId,scene);
	if(!res) 
	{
		return -1;
	}
	return sceneId;
}




