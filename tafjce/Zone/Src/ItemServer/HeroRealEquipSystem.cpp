#include "ItemServerPch.h"
#include "ItemFactory.h"
#include "HeroRealEquipSystem.h"
#include "ErrorCodeDef.h"

extern "C" IObject* createHeroRealEquipSys()
{
	return new HeroRealEquipSystem;
}


HeroRealEquipSystem::HeroRealEquipSystem()
{
}

HeroRealEquipSystem::~HeroRealEquipSystem()
{
	for(size_t i = 0; i < m_equipList.size(); i++)
	{
		IItem* pItem = getItemFromHandle(m_equipList[i]);
		delete pItem;
	}

	m_equipList.clear();
}


Uint32 HeroRealEquipSystem::getSubsystemID() const
{
	return IID_IHeroRealEquupSystem;
}


Uint32 HeroRealEquipSystem::getMasterHandle()
{
	return m_hEntity;
}

bool HeroRealEquipSystem::create(IEntity* pEntity, const std::string& strData)
{
	m_hEntity = pEntity->getHandle();
	m_equipList.resize(getSize(), 0);

	if(strData.size() > 0)
	{
		ServerEngine::HeroRealEquipSystemData saveData;
		ServerEngine::JceToObj(strData, saveData);
		initHeroEquip(saveData);
	}

	return true;
}


void HeroRealEquipSystem::initHeroEquip(const ServerEngine::HeroRealEquipSystemData& saveData)
{
	IItemFactory* pItemFactory = getComponent<IItemFactory>(COMPNAME_ItemFactory, IID_IItemFactory);
	assert(pItemFactory);

	for(map<taf::Int32, ServerEngine::ItemData>::const_iterator it = saveData.equipList.begin(); it != saveData.equipList.end(); it++)
	{
		const ServerEngine::ItemData& refItemData = it->second;
		IItem* pNewItem = pItemFactory->createItemFromDB(refItemData);
		assert(pNewItem);

		pNewItem->changeProperty(m_hEntity, true);

		int iPos = it->first;
		m_equipList[iPos] = pNewItem->getHandle();
	}

	if(saveData.equipList.size() > 0)
	{
		IEntity* pHero = getEntityFromHandle(m_hEntity);
		assert(pHero);

		pHero->setProperty(PROP_HERO_HASEQUIP, 1);
	}

	// 计算套装属性
	calcSuitProp();
}


void HeroRealEquipSystem::useSuitProp(bool bPuton)
{
	ItemFactory* pItemFactory = static_cast<ItemFactory*>(getComponent<IItemFactory>(COMPNAME_ItemFactory, IID_IItemFactory) );
	assert(pItemFactory);

	IEntity* pHero = getEntityFromHandle(m_hEntity);
	assert(pHero);

	for(map<int, int>::iterator it = m_suitStateMap.begin(); it != m_suitStateMap.end(); it++)
	{
		int iSuitID = it->first;
		int iCount = it->second;

		assert(iCount >= 2);

		const EquipSuit* pSuitCfg = pItemFactory->querySuitCfg(iSuitID);
		assert(pSuitCfg);

		// 大于等于2
		for(int i = 0; i < iCount-2; i++)
		{
			const map<int, int>& refPropMap = pSuitCfg->suitPropList[i];
			
			for(map<int, int>::const_iterator propIt = refPropMap.begin(); propIt != refPropMap.end(); propIt++)
			{
				int iPropID = propIt->first;
				int iValue = propIt->second;

				if(!bPuton) iValue = 0 - iValue;
				pHero->changeProperty(iPropID, iValue, 0);
			}
		}
	}
}


bool HeroRealEquipSystem::calcSuitProp()
{
	ItemFactory* pItemFactory = static_cast<ItemFactory*>(getComponent<IItemFactory>(COMPNAME_ItemFactory, IID_IItemFactory) );
	assert(pItemFactory);

	map<int, int> tmpNewSuitMap;
	for(int i = 0; i < GSProto::en_EquipPos_Reserve; i++)
	{
		HItem hEquip = getEquip(i);
		IItem* pEquip = getItemFromHandle(hEquip);
		if(!pEquip) continue;

		int iItemID = pEquip->getProperty(PROP_ITEM_BASEID, 0);
		int iSuitID = pItemFactory->querySuitIDFromEquip(iItemID);
		if(0 == iSuitID) continue;

		tmpNewSuitMap[iSuitID]++;
	}

	// 去掉未到2的
	for(map<int, int>::iterator it = tmpNewSuitMap.begin(); it != tmpNewSuitMap.end();)
	{
		if(it->second < 2)
		{
			tmpNewSuitMap.erase(it++);
		}
		else
		{
			it++;
		}
	}

	if(m_suitStateMap != tmpNewSuitMap)
	{
		useSuitProp(false);
		m_suitStateMap = tmpNewSuitMap;
		useSuitProp(true);

		return true;
	}

	return false;
}


void HeroRealEquipSystem::fillSuitState(google::protobuf::RepeatedPtrField<GSProto::EquipSuitState>* pMutableStateList)
{
	vector<GSProto::EquipSuitState> chgList;
	for(map<int, int>::iterator it = m_suitStateMap.begin(); it != m_suitStateMap.end(); it++)
	{
		int iSuitID = it->first;
		int iCount = it->second;

		assert(iCount >= 2);

		GSProto::EquipSuitState* pNewSuitState = pMutableStateList->Add();
		pNewSuitState->set_isuitid(iSuitID);
		pNewSuitState->set_iactivecount(iCount-1);
	}
}


bool HeroRealEquipSystem::createComplete()
{
	return true;
}

const std::vector<Uint32>& HeroRealEquipSystem::getSupportMessage()
{
	static std::vector<Uint32> resultList;

	return resultList;
}


void HeroRealEquipSystem::onMessage(QxMessage* pMessage)
{
	
}


void HeroRealEquipSystem::packSaveData(string& data)
{
	ServerEngine::HeroRealEquipSystemData saveData;

	for(size_t i = 0; i < m_equipList.size(); i++)
	{
		IItem* pItem = getItemFromHandle(m_equipList[i]);
		if(!pItem) continue;

		pItem->packJce(saveData.equipList[i]);
	}

	data = ServerEngine::JceToStr(saveData);
}


int HeroRealEquipSystem::getSize()
{
	return 6;
}

Uint32 HeroRealEquipSystem::getEquip(int iPos)
{
	if( (iPos < 0) || (iPos >= getSize() ) )
	{
		return 0;
	}

	return m_equipList[iPos];
}

bool HeroRealEquipSystem::setEquip(int iPos, Uint32 hEquipHandle, GSProto::EquipChgItem* pOutItem)
{
	if(!canSetEquip(iPos, hEquipHandle, false) )
	{
		return false;
	}

	// 去掉原装备属性
	IItem* pOrgItem = getItemFromHandle(getEquip(iPos) );
	if(pOrgItem) pOrgItem->changeProperty(m_hEntity, false);

	m_equipList[iPos] = hEquipHandle;

	IItem* pNewItem = getItemFromHandle(hEquipHandle);
	if(pNewItem) pNewItem->changeProperty(m_hEntity, true);

	IEntity* pHero = getEntityFromHandle(m_hEntity);
	assert(pHero);
	pHero->setProperty(PROP_HERO_HASEQUIP, (int)hasEquip() );

	if(pOutItem)
	{
		if(!pNewItem)
		{
			pOutItem->set_ipos(iPos);
			pOutItem->set_bdel(true);
		}
		else
		{
			pOutItem->set_ipos(iPos);
			pOutItem->set_bdel(false);

			pNewItem->packScEquipInfo(*pOutItem->mutable_equipdata(), iPos);
		}
	}

	if(calcSuitProp() )
	{
		GSProto::CMD_EQUIP_SUITSTATE_UPDATE_SC scMsg;
		scMsg.set_dwobjectid(m_hEntity);
		fillSuitState(scMsg.mutable_szsuitstate() );

		IEntity* pEntity = getEntityFromHandle(m_hEntity);
		assert(pEntity);

		
		HEntity hMaster = pEntity->getProperty(PROP_ENTITY_MASTER, 0);
		IEntity* pMaster = getEntityFromHandle(hMaster);
		assert(pMaster);
		
		pMaster->sendMessage(GSProto::CMD_EQUIP_SUITSTATE_UPDATE, scMsg);
	}

	return true;
}

HEntity HeroRealEquipSystem::getOwner()
{
	return m_hEntity;
}

void HeroRealEquipSystem::sendContainerChg(const vector<int>& posList)
{
	GSProto::CMD_EQUIPCONTAINER_CHG_SC scMsg;
	scMsg.set_dwobjectid(m_hEntity);

	for(size_t i = 0; i < posList.size(); i++)
	{
		int iPos = posList[i];
		
		GSProto::EquipChgItem* pNewEquipChgItem = scMsg.add_szchglist();
		assert(pNewEquipChgItem);

		IItem* pTmpItem = getItemFromHandle(getEquip(iPos));
		if(pTmpItem)
		{
			pNewEquipChgItem->set_ipos(iPos);
			pNewEquipChgItem->set_bdel(false);
			pTmpItem->packScEquipInfo(*pNewEquipChgItem->mutable_equipdata(), iPos);
		}
		else
		{
			pNewEquipChgItem->set_ipos(iPos);
			pNewEquipChgItem->set_bdel(true);
		}
	}

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	HEntity hMaster = pEntity->getProperty(PROP_ENTITY_MASTER, 0);
	IEntity* pMaster = getEntityFromHandle(hMaster);
	assert(pMaster);

	pMaster->sendMessage(GSProto::CMD_EQUIPCONTAINER_CHG, scMsg);
}


bool HeroRealEquipSystem::canSetEquip(int iPos, Uint32 hEquipHandle, bool bNotifyError)
{
	if( (iPos < 0) || (iPos >= getSize() ) )
	{
		return false;
	}

	
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	HEntity hMaster = pEntity->getProperty(PROP_ENTITY_MASTER, 0);
	IEntity* pMaster = getEntityFromHandle(hMaster);
	assert(pMaster);

	int iLevel = pMaster->getProperty(PROP_ENTITY_LEVEL, 0);

	IItem* pEquip = getItemFromHandle(hEquipHandle);
	if(pEquip)
	{
		int iItemType = pEquip->getProperty(PROP_ITEM_TYPE, 0);
		if(iItemType != GSProto::en_ItemType_Equip)
		{
			if(bNotifyError) pMaster->sendErrorCode(ERROR_INVALID_EQUIP);
			return false;
		}

		int iEquipPos = pEquip->getProperty(PROP_EQUIP_POS, 0);
		if(iEquipPos != iPos)
		{
			if(bNotifyError) pMaster->sendErrorCode(ERROR_EQUIPPOS_INVALID);
			return false;
		}

		// 验证装备等级
		int iEquipLv = pEquip->getProperty(PROP_ITEM_LVLLIMIT, 0);
		if(iLevel < iEquipLv)
		{
			if(bNotifyError) pMaster->sendErrorCode(ERROR_EQUIP_LEVEL);
			return false;
		}

		// 验证职业
		int iJobLimit = pEquip->getProperty(PROP_EQUIP_JOBLIMIT, 0);
		if(iJobLimit > 0)
		{
			int iHeroJob = pEntity->getProperty(PROP_ENTITY_JOB, 0);
			int iJobMask = 1<<(iHeroJob-1);
			if((iJobMask & iJobLimit) == 0)
			{
				if(bNotifyError) pMaster->sendErrorCode(ERROR_EQUIP_JOBINVALID);
				return false;
			}
		}
	}

	return true;
}


bool HeroRealEquipSystem::hasEquip()
{
	for(size_t i = 0; i < m_equipList.size(); i++)
	{
		if(0 != m_equipList[i])
		{
			return true;
		}
	}

	return false;
}


void* HeroRealEquipSystem::queryInterface(int iInterfaceID)
{
	if(IID_IEquipContainer == iInterfaceID)
	{
		return (IEquipContainer*)this;
	}

	return NULL;
}




