#include "FriendSystemPch.h"
#include "VIPFactory.h"

extern "C" IComponent* createVIPFactory(Int32)
{
	return new VIPFactory;
}

VIPFactory::VIPFactory()
{
}

VIPFactory::~VIPFactory()
{
	m_VipDataMap.clear();
}

bool VIPFactory::initlize(const PropertySet& propSet)
{
	ITable* table = getCompomentObjectManager()->findTable(TABLENAME_Vip);
	assert(table);

	int recordCount = table->getRecordCount();
	for(int i = 0; i < recordCount; ++i)
	{
		vector<int> vec;
		vec.resize(VIP_PROP_COUNT,0);
		vec[VIP_PROP_SPEEDPRODUCT_TIMES] = table->getInt(i,"加速生产次数");
		vec[VIP_PROP_PHYSTRENGTH_UPLIMIT] = table->getInt(i,"体力上限");
		vec[VIP_PROP_BUY_PHYSTRENGTH_TIMES] = table->getInt(i,"购买体力次数");
		vec[VIP_PROP_BUY_CHALLENGE_TIMES] = table->getInt(i,"购买挑战次数");
		vec[VIP_PROP_ADVANCED_PRACTICE] = table->getInt(i,"购买挑战次数");
		vec[VIP_PROP_GIVEFRIEND_PHYSTRENGTH_TIEMS] = table->getInt(i,"赠送好友体力次数");
		vec[VIP_PROP_GETFRIEND_PHYSTRENGTH_TIMES] = table->getInt(i,"接受好友体力次数");
		vec[VIP_PROP_FRIENDCOUNT_LIMIT] = table->getInt(i,"好友上限");
		vec[VIP_PROP_CLIMBTOWER_TIMES] = table->getInt(i,"通天塔次数");
		vec[VIP_PROP_VIPEXP] = table->getInt(i,"经验");
		vec[VIP_PROP_SAODANG_TIMES] = table->getInt(i, "扫荡次数");
		vec[VIP_PROP_DREAMLAND_RESET] = table->getInt(i,  "九天幻境重置次数");
		vec[VIP_PROP_MOBAI_TIMES] = table->getInt(i,  "膜拜次数");
		vec[VIP_PROP_MANOR_HARVEST_TIMES] = table->getInt(i,  "领地收取次数");
		vec[VIP_PROP_FB_ADDEXP] = table->getInt(i, "FB经验加成");

		int vipLevel = table->getInt(i,"VIP等级");
		
		assert(m_VipDataMap.find(vipLevel) == m_VipDataMap.end() );
		m_VipDataMap[vipLevel] = vec;
	}
	return true;
}

int VIPFactory::getVipPropByHEntity(HEntity hEntity,unsigned int propKey)
{
	IEntity* pEntity = getEntityFromHandle(hEntity);
	int vipLevel = pEntity->getProperty(PROP_ACTOR_VIPLEVEL, 0);

	std::map<int, std::vector<int> >::iterator iter = m_VipDataMap.find(vipLevel);
	
	assert( iter != m_VipDataMap.end());
	assert(propKey < VIP_PROP_COUNT);
	
	return iter->second[propKey];
}

int VIPFactory::getVipPropByVIPLevel(int vipLevel, unsigned int propKey)
{

	std::map<int, std::vector<int> >::iterator iter = m_VipDataMap.find(vipLevel);
	assert( iter != m_VipDataMap.end());
	assert(propKey < VIP_PROP_COUNT);
	
	return iter->second[propKey];
}

int VIPFactory::getVipTopLevel()
{
	return m_VipDataMap.size()-1;
}



