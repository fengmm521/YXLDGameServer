#include "ClimbTowerSystemPch.h"
#include "ClimbTowerFactory.h"

extern "C" IComponent* createClimbTowerFactory(Int32)
{
	return new ClimbTowerFactory;
}

		
ClimbTowerFactory::ClimbTowerFactory():m_iTowerCount(0)
{
}

ClimbTowerFactory::~ClimbTowerFactory()
{
	m_TowerStaticPropMap.clear();
}

bool ClimbTowerFactory::initlize(const PropertySet& propSet)
{
	loadAllTower();
	return true;
}

void ClimbTowerFactory::loadAllTower()
{
	ITable* pClimbTowerTb = getCompomentObjectManager()->findTable(TABLENAME_ClimbTower);
	assert(pClimbTowerTb);

	//IPropDefine* pPropDefine = getPropDefine("ItemPropDef");
	//assert(pPropDefine);

	m_iTowerCount = pClimbTowerTb->getRecordCount();
	for(int i = 0; i < m_iTowerCount; i++)
	{
		SingleTowerLayerInfo lLayerInfo;
		lLayerInfo.iLayerId = pClimbTowerTb->getInt(i, "层数");
		assert(lLayerInfo.iLayerId > 0);
		//lLayerInfo.iMonsterGroupId= pClimbTowerTb->getInt(i, "怪物组ID");

		string strMonsterGrpInfo = pClimbTowerTb->getString(i, "怪物组ID");
		lLayerInfo.monsterGrpList = TC_Common::sepstr<int>(strMonsterGrpInfo, "#");
		lLayerInfo.iCanGetExp= pClimbTowerTb->getInt(i, "修为奖励");
		std::string dropIDListStr = pClimbTowerTb->getString(i, "掉落ID");
		lLayerInfo.dropIdVec = TC_Common::sepstr<int>(dropIDListStr, Effect_MagicSep);
		assert(m_TowerStaticPropMap.find( lLayerInfo.iLayerId ) == m_TowerStaticPropMap.end() );

		m_TowerStaticPropMap[lLayerInfo.iLayerId] = lLayerInfo;
	}
}


int ClimbTowerFactory::getNextLayerCanGetExp(int currentLayerId)
{
	//if(currentLayerId > m_iTowerCount || currentLayerId < 0)
	//	return 0;
	return m_iTowerCount == currentLayerId ? 0:m_TowerStaticPropMap[currentLayerId+1].iCanGetExp;
}
vector<int> ClimbTowerFactory:: getLayerMonsterGropIdList(int currentLayerId)
{
	return m_TowerStaticPropMap[currentLayerId].monsterGrpList;
}
std::vector<int>& ClimbTowerFactory::getLayerDropIdVec(int currentLayerId)
{
	return m_TowerStaticPropMap[currentLayerId].dropIdVec;
}

int ClimbTowerFactory::getLayerCanGetHeroExp(int currentLayerId)
{
	return m_TowerStaticPropMap[currentLayerId].iCanGetExp;
}






























