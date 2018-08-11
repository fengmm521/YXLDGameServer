
#include "FightSoulSystemPch.h"
#include "FightSoulFactory.h"
#include "HeroEquipSystem.h"
#include "FightSoul.h"

extern "C" IObject* createHeroEquipSystem()
{
	return new HeroEquipSystem;
}


HeroEquipSystem::HeroEquipSystem()
{
	m_fightSoulWear.resize(GSProto::MAX_FIGHTSOUL_WEARSIZE, 0);
}

HeroEquipSystem::~HeroEquipSystem()
{
	for(size_t i = 0; i < m_fightSoulWear.size(); i++)
	{
		IFightSoul* pFightSoul = getFightSoulFromHandle(m_fightSoulWear[i]);
		if(!pFightSoul) continue;

		delete pFightSoul;
	}
}

Uint32 HeroEquipSystem::getSubsystemID() const
{
	return IID_IHeroEqupSystem;
}

Uint32 HeroEquipSystem::getMasterHandle()
{
	return m_hEntity;
}

bool HeroEquipSystem::create(IEntity* pEntity, const std::string& strData)
{
	m_hEntity = pEntity->getHandle();

	if(strData.size() > 0)
	{
		ServerEngine::HeroEquipSystemData tmpData;
		ServerEngine::JceToObj(strData, tmpData);
		
		initHeroEquipData(tmpData);
	}

	return true;
}

void HeroEquipSystem::initHeroEquipData(const ServerEngine::HeroEquipSystemData& data)
{
	FightSoulFactory* pFightSoulFactory = (FightSoulFactory*)(getComponent<IFightSoulFactory>("FightSoulFactory", IID_IFightSoulFactory) );
	assert(pFightSoulFactory);

	int iLimitSize = getSize();

	for(map<taf::Int32, ServerEngine::FightSoulItem>::const_iterator it = data.equipList.begin(); it != data.equipList.end(); it++)
	{
		int iPos = it->first;
		assert( (iPos >= 0) && (iPos < iLimitSize) );
		
		const ServerEngine::FightSoulItem& refSoulItem = it->second;

		IFightSoul* pTmpFightSoul = pFightSoulFactory->createFightSoulFromDB(refSoulItem);
		assert(pTmpFightSoul);

		m_fightSoulWear[iPos] = pTmpFightSoul->getHandle();

		pTmpFightSoul->changeProperty(m_hEntity, true);
	}

	IEntity* pHero = getEntityFromHandle(m_hEntity);
	assert(pHero);
	
	if(data.equipList.size() > 0)
	{
		pHero->setProperty(PROP_HERO_HASFIGHTSOUL, 1);
	}
}

bool HeroEquipSystem::createComplete()
{
	return true;
}


const std::vector<Uint32>& HeroEquipSystem::getSupportMessage()
{
	static std::vector<Uint32> nullList;

	return nullList;
}


void HeroEquipSystem::packSaveData(string& data)
{
	ServerEngine::HeroEquipSystemData tmpData;
	for(int i = 0; i < (int)m_fightSoulWear.size(); i++)
	{
		IFightSoul* pFightSoul = getFightSoulFromHandle(m_fightSoulWear[i]);
		if(!pFightSoul) continue;

		ServerEngine::FightSoulItem tmpItem;
		tmpItem.iBaseID = pFightSoul->getID();
		tmpItem.iExp = pFightSoul->getExp();

		tmpData.equipList[i] = tmpItem;
	}

	data = ServerEngine::JceToStr(tmpData);
}

Int32 HeroEquipSystem::getSpace()
{
	return (int)m_fightSoulWear.size();
}

Int32 HeroEquipSystem::getSize()
{
	IEntity* pHero = getEntityFromHandle(m_hEntity);
	assert(pHero);

	FightSoulFactory* pFightSoulFactory = (FightSoulFactory*)(getComponent<IFightSoulFactory>("FightSoulFactory", IID_IFightSoulFactory) );
	assert(pFightSoulFactory);

	int iLevel = pHero->getProperty(PROP_ENTITY_LEVEL, 0);

	return pFightSoulFactory->calcEquipLimitCount(iLevel);
}

Uint32 HeroEquipSystem::getFightSoul(int iPos)
{
	if( (iPos < 0) || (iPos >= (int)m_fightSoulWear.size() ) )
	{
		return 0;
	}

	//assert( (iPos >= 0) && (iPos < (int)m_fightSoulWear.size() ) );

	return m_fightSoulWear[iPos];
}

bool HeroEquipSystem::setFightSoul(int iPos, Uint32 hFightSoulHandle, GSProto::FightSoulChgItem* pOutItem)
{
	assert( (iPos >= 0) && (iPos < (int)m_fightSoulWear.size() ) );

	if(hFightSoulHandle == m_fightSoulWear[iPos])
	{
		return true;
	}

	// 验证位置是否开启
	if(iPos >= getSize() )
	{
		return false;
	}

	IEntity* pHero = getEntityFromHandle(m_hEntity);
	assert(pHero);

	HFightSoul hOldFightSoul = m_fightSoulWear[iPos];

	// 去掉原武魂数值
	IFightSoul* pOldFightSoul = getFightSoulFromHandle(hOldFightSoul);
	if(pOldFightSoul)pOldFightSoul->changeProperty(m_hEntity, false);

	// 添加新武魂数值
	FightSoul* pNewFightSoul = static_cast<FightSoul*>(getFightSoulFromHandle(hFightSoulHandle) );
	if(pNewFightSoul) pNewFightSoul->changeProperty(m_hEntity, true);
	
	m_fightSoulWear[iPos] = hFightSoulHandle;

	pHero->setProperty(PROP_HERO_HASFIGHTSOUL, (int)hasFightSoul() );
	
	if( (0 == hFightSoulHandle) && pOutItem)
	{	
		pOutItem->set_ipos(iPos);
		pOutItem->set_bdel(true);
		return true;
	}
	else
	{
		if(pOutItem)
		{
			pOutItem->set_ipos(iPos);
			pOutItem->set_bdel(false);
			assert(pNewFightSoul);
			pNewFightSoul->fillScData(iPos, *pOutItem->mutable_fightsouldata() );
		}
	}

	return true;
}

bool HeroEquipSystem::canSetFightSoul(int iPos, Uint32 hFightSoulHandle, bool bNoitifyErr)
{
	// 卸载，OK(这里不考虑卸下的目标容器)
	if(0 == hFightSoulHandle)
	{
		return true;
	}

	if( (iPos < 0) || (iPos >= getSize() ) )
	{
		return false;
	}

	// 原位置有武魂，OK(被吞噬)
	if(0 != m_fightSoulWear[iPos])
	{
		return true;
	}

	// 同类型的武魂不能共存
	FightSoul* pFightSoul = static_cast<FightSoul*>(getFightSoulFromHandle(hFightSoulHandle));
	assert(pFightSoul);

	//int iBaseID = pFightSoul->getID();
	for(int i = 0; i < getSize(); i++)
	{
		HFightSoul hTmp = m_fightSoulWear[i];
		FightSoul* pTmp = static_cast<FightSoul*>(getFightSoulFromHandle(hTmp) );
		if(!pTmp) continue;

		if(pFightSoul->getType()== pTmp->getType() )
		{
			if(bNoitifyErr)
			{
				IEntity* pEntity = getEntityFromHandle(m_hEntity);
				assert(pEntity);

				HEntity hMaster = pEntity->getProperty(PROP_ENTITY_MASTER, 0);
				IEntity* pMaster = getEntityFromHandle(hMaster);
				assert(pMaster);

				pMaster->sendErrorCode(ERROR_EXIST_SAMEID_FIGHTSOUL);
			}
			return false;
		}
	}
	
	return true;
}

bool HeroEquipSystem::hasFightSoul()
{
	for(size_t i = 0; i < m_fightSoulWear.size(); i++)
	{
		if(0 != m_fightSoulWear[i])
		{
			return true;
		}
	}

	return false;
}

void HeroEquipSystem::sendContainerChg(const vector<int>& posList)
{
	GSProto::Cmd_Sc_ChgFsContainer scMsg;
	scMsg.set_iobjecttype(GSProto::en_class_Hero);
	scMsg.set_dwobjectid(m_hEntity);
	scMsg.set_icontainertype(GSProto::en_FSConType_Wear);

	for(size_t i = 0; i < posList.size(); i++)
	{
		int iPos = posList[i];
		assert( (iPos >=0) && (iPos < (int)getSize() ) );
		FightSoul* pTmpFightSoul = (FightSoul*)getFightSoulFromHandle(getFightSoul(iPos) );
		GSProto::FightSoulChgItem* pChgItem = scMsg.mutable_szchglist()->Add();

		pChgItem->set_ipos(iPos);
		if(!pTmpFightSoul)
		{
			pChgItem->set_bdel(true);
		}
		else
		{
			pChgItem->set_bdel(false);
			pTmpFightSoul->fillScData(iPos, *pChgItem->mutable_fightsouldata() );
		}
	}

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	HEntity hMaster = pEntity->getProperty(PROP_ENTITY_MASTER, 0);
	IEntity* pMaster = getEntityFromHandle(hMaster);
	assert(pMaster);

	GSProto::SCMessage pkg;
	HelpMakeScMsg(pkg, GSProto::CMD_CHG_FSCONTAINER, scMsg);
	pMaster->sendMessage(pkg);
}


void* HeroEquipSystem::queryInterface(int iInterfaceID)
{
	if(IID_IFightSoulContainer == iInterfaceID)
	{
		return (IFightSoulContainer*)this;
	}

	return NULL;
}

HEntity HeroEquipSystem::getOwner()
{
	return m_hEntity;
}





