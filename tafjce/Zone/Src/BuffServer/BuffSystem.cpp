#include "BuffServerPch.h"
#include "BuffSystem.h"
#include "EntityBuff.h"

extern "C" IObject* createBuffSystem()
{
	return new BuffSystem;
}


BuffSystem::BuffSystem()
{
}

BuffSystem::~BuffSystem()
{
	for(BuffList::iterator it = m_buffList.begin(); it != m_buffList.end(); it++)
	{
		IEntityBuff* pBuff = *it;
		
		pBuff->setSysReleaseFlag();
		delete pBuff;
	}

	m_buffList.clear();
}

Uint32 BuffSystem::getSubsystemID() const
{
	return IID_IBuffSystem;
}


Uint32 BuffSystem::getMasterHandle()
{
	return m_hEntity;
}

bool BuffSystem::create(IEntity* pEntity, const std::string& strData)
{
	PROFILE_MONITOR("BuffSystem::create");
	m_hEntity = pEntity->getHandle();

	if(strData.size() == 0)
	{
		return true;
	}

	m_strCacheData = strData;

	return true;
}

bool BuffSystem::createComplete()
{
	PROFILE_MONITOR("BuffSystem::createComplete");
	/*IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	int iClassID = pEntity->getProperty(PROP_ENTITY_CLASS, (Int32)0);

	if( (m_strCacheData.size() > 0) && (iClassID == en_class_Actor) )
	{
		Aegis::BuffSystemSaveData saveData;
		Aegis::JceToObj(m_strCacheData, saveData);
		initBuffList(saveData);	
	}*/

    return true;
}

void BuffSystem::packSaveData(string& data)
{
}

bool BuffSystem::existBuff(int iBuffID)
{
	return getBuffByID(iBuffID) != NULL;
}


IEntityBuff* BuffSystem::addBuff(HEntity hGiver, int iBuffID, const EffectContext& preContext)
{
	PROFILE_MONITOR("BuffSystem::addBuff");
	IBuffFactory* pBuffFactory = getComponent<IBuffFactory>("BuffFactory", IID_IBuffFactory);
	assert(pBuffFactory);

	const IEntityBuff* pProtoBuff = pBuffFactory->getBuffPrototype(iBuffID);
	if(!pProtoBuff)
	{
		return NULL;
	}

	// 如果存在相同ID的Buff,刷新生效时间即可
	IEntityBuff* pSameIDBuff = getBuffByID(iBuffID);
	if(pSameIDBuff)
	{
		int iOverlapCount = pSameIDBuff->getOverlapCount();
		int iMaxOverlapCount = pSameIDBuff->getMaxOverlapCount();
		iOverlapCount = std::min(iOverlapCount+1, iMaxOverlapCount);

		pSameIDBuff->setOverlapCount(iOverlapCount);
	
		static_cast<EntityBuff*>(pSameIDBuff)->resetEffectRound();
		return pSameIDBuff;
	}

	// 删除同ID的
	//delBuff(iBuffID);

	// 删除所有同系列的状态
	int iTmpGroupID = pProtoBuff->getBuffGroupID();
	delBuffByGroup(iTmpGroupID);

	// 加个新的状态吧
	return doAddNewBuff(hGiver, iBuffID, 1, preContext);
}

void BuffSystem::delBuffByGroup(int iGroupID)
{
	if(0 == iGroupID)
	{
		return;
	}

	vector<string> delList;
	for(BuffList::iterator it = m_buffList.begin(); it != m_buffList.end(); it++)
	{
		IEntityBuff* pBuff = *it;
		if( (iGroupID > 0) && (pBuff->getBuffGroupID() == iGroupID) )
		{
			delList.push_back(pBuff->getBuffUUID() );
		}
	}

	for(size_t i = 0; i < delList.size(); i++)
	{
		delBuffByUUID(delList[i]);
	}
}

IEntityBuff* BuffSystem::doAddNewBuff(HEntity hGiver, int iBuffID, int iInitOverlap, const EffectContext& preContext)
{
	IBuffFactory* pBuffFactory = getComponent<IBuffFactory>("BuffFactory", IID_IBuffFactory);
	assert(pBuffFactory);

	const IEntityBuff* pBuffPrototype = pBuffFactory->getBuffPrototype(iBuffID);
	assert(pBuffPrototype);

	EntityBuff* pNewBuff = static_cast<EntityBuff*>(pBuffPrototype->clone() );
	assert(pNewBuff);

	pNewBuff->initlize(hGiver, m_hEntity, iInitOverlap, preContext);
	m_buffList.push_back(pNewBuff);

	MINIAPR_TRACE("添加新状态:%d", pNewBuff->getBuffID() );
	return pNewBuff;
}

bool BuffSystem::isImMinityBuff(int iBuffID)
{
	for(BuffList::iterator it = m_buffList.begin(); it != m_buffList.end(); it++)
	{
		EntityBuff* pEntityBuff = static_cast<EntityBuff*>(*it);
		assert(pEntityBuff);

		if(pEntityBuff->isImMunityBuff(iBuffID) )
		{
			MINIAPR_TRACE("免疫状态:%d", iBuffID);
			return true;
		}
	}
	
	return false;
}

IEntityBuff* BuffSystem::getBuff(const std::string& strUUID)
{
	for(BuffList::iterator it = m_buffList.begin(); it != m_buffList.end(); it++)
	{
		IEntityBuff* pBuff = *it;
		if(strUUID == pBuff->getBuffUUID() )
		{
			return pBuff;
		}
	}

	return NULL;
}

bool BuffSystem::randomDelBuffByType(int iBuffType)
{
	vector<string> delBuffList;

	for(BuffList::iterator it = m_buffList.begin(); it != m_buffList.end(); it++)
	{
		IEntityBuff* pBuff = *it;
		if(iBuffType == pBuff->getBuffType() )
		{
			delBuffList.push_back(pBuff->getBuffUUID() );
		}
	}

	if(0 == delBuffList.size() )
	{
		return false;
	}

	IRandom* pRandom = getComponent<IRandom>("Random", IID_IMiniAprRandom);
	assert(pRandom);

	int iRandV = pRandom->random() % (int)delBuffList.size();
	string strDelUUID = delBuffList[iRandV];

	delBuffByUUID(strDelUUID);

	return true;
}


IEntityBuff* BuffSystem::getBuffByID(int iBuffID)
{
	for(BuffList::iterator it = m_buffList.begin(); it != m_buffList.end(); it++)
	{
		IEntityBuff* pBuff = *it;
		if(pBuff->getBuffID() == iBuffID)
		{
			return pBuff;
		}
	}

	return NULL;
}


void BuffSystem::delBuff(int iBaseID)
{
	for(BuffList::iterator it = m_buffList.begin(); it != m_buffList.end();)
	{
		IEntityBuff* pBuff = *it;
		if(pBuff->getBuffID() == iBaseID)
		{
			it = m_buffList.erase(it);
			MINIAPR_TRACE("删除状态:%d", pBuff->getBuffID() );
			delete pBuff;
		}
		else
		{
			it++;
		}
	}
}

void BuffSystem::delBuffByUUID(const std::string& strUUID)
{
	for(BuffList::iterator it = m_buffList.begin(); it != m_buffList.end(); it++)
	{
		IEntityBuff* pBuff = *it;
		if(strUUID != pBuff->getBuffUUID() )
		{
			continue;
		}
		m_buffList.erase(it);
		MINIAPR_TRACE("删除状态:%d", pBuff->getBuffID() );
		delete pBuff;
		return;
	}
}

void BuffSystem::delBuffByType(int iBuffType)
{
	for(BuffList::iterator it = m_buffList.begin(); it != m_buffList.end();)
	{
		IEntityBuff* pBuff = *it;
		if(pBuff->getBuffType() == iBuffType)
		{
			it = m_buffList.erase(it);
			MINIAPR_TRACE("删除状态:%d", pBuff->getBuffID() );
			delete pBuff;
		}
		else
		{
			it++;
		}
	}
}



