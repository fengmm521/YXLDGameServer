#include "ItemServerPch.h"
#include "ItemFactory.h"
#include "Item.h"

static HandleManager<IItem*> g_itemHandleMgr;


static map<int, int> s_prop2ShowLifeAttMap;

IItem* getItemFromHandle(HItem hItem)
{
	IItem* pItem = g_itemHandleMgr.getDataPoint(hItem);

	return pItem;
}


Item::Item():m_pEquipProp(NULL)
{
	IItem** pPItem = g_itemHandleMgr.Aquire(m_hHandle);
	*pPItem = this;
}


Item::~Item()
{
	g_itemHandleMgr.Release(m_hHandle);
}

bool Item::useItem(HEntity hTarget, const PropertySet& ctx)
{
	// 验证效果是否存在(等级验证在子系统处理,这里只执行使用效果)
	/*int iEffectID = getProperty(PROP_ITEM_USEEFFECT, 0);
	if(iEffectID == 0)
	{
		return false;
	}

	IEntity* pTarget = getEntityFromHandle(hTarget);
	assert(pTarget);
	
	IEffectSystem* pEffectSys = static_cast<IEffectSystem*>(pTarget->querySubsystem(IID_IEffectSystem));
	assert(pEffectSys);

	PropertySet tmpCtx = ctx;
	tmpCtx.setInt(PROP_EFFECT_USEITEM_HANDLE, m_hHandle);

	Detail::EventHandle::Proxy tmpPrx = getEventHandle();
	bool bResult = pEffectSys->addEffect(iEffectID, tmpCtx);
	if(!tmpPrx.get() )
	{
		return true;
	}

	
	
*/
	return false;
}


void Item::packJce(ServerEngine::ItemData& itemSaveData)
{
	//FDLOG("TEST")<<"item pack jce."<<endl;
	int iItemBaseID = getProperty(PROP_ITEM_BASEID, 0);
	int iStackCount = getProperty(PROP_ITEM_STACKCOUNT, 0);

	assert( (iItemBaseID > 0) && (iStackCount > 0) );

	itemSaveData.iItemID = iItemBaseID;
	itemSaveData.iStackCount = iStackCount;

	int iItemType = getProperty(PROP_ITEM_TYPE, 0);
	if(GSProto::en_ItemType_Equip == iItemType)
	{
		ServerEngine::EquipData tmpEquipData;
		tmpEquipData.iLevel = getProperty(PROP_EQUIP_LEVEL, 0);
		tmpEquipData.iEnhanceMoney = getProperty(PROP_EQUIP_ENHANCEMONEY, 0);

		tmpEquipData.iRandomPropID = getProperty(PROP_EQUIP_RANDOMPROP, 0);
		tmpEquipData.iRandomValue = getProperty(PROP_EQUIP_RANDOMVALUE, 0);

		// 打包洗练数据
		ServerEngine::XilianPropItem tmpItem;
		for(size_t i = 0; i < this->xilianProp.size(); i ++){
			const XilianPropItem& propItem = this->xilianProp[i];
			int propId = propItem.val1;
			int value = propItem.val2;
			tmpItem.propId = propId;
			tmpItem.value = value;
			tmpEquipData.xilianPropList.push_back(tmpItem);
		}

		itemSaveData.strEquipData = ServerEngine::JceToStr(tmpEquipData);
	}
}

HItem Item::getHandle()
{
	return m_hHandle;
}

void Item::packScItemInfo(GSProto::ItemInfo& scItemInfo)
{
	int iBaseItemID = getProperty(PROP_ITEM_BASEID, 0);
	int iStackCount = getProperty(PROP_ITEM_STACKCOUNT, 0);

	int iItemType = getProperty(PROP_ITEM_TYPE, 0);
	assert( (iItemType == GSProto::en_ItemType_Item) || (iItemType == GSProto::en_ItemType_Material) || (iItemType == GSProto::en_ItemType_Favorite) );

	scItemInfo.set_dwobjectid(m_hHandle);
	scItemInfo.set_iitemid(iBaseItemID);
	scItemInfo.set_iitemcount(iStackCount);

	if(iItemType == GSProto::en_ItemType_Material)
	{
		// 合成数量
		ItemFactory* pItemFactory = static_cast<ItemFactory*>(getComponent<IItemFactory>(COMPNAME_ItemFactory, IID_IItemFactory) );
		assert(pItemFactory);

		const EquipChipCombine* pEquipChipCombineCfg = pItemFactory->queryEquipChipCombine(iBaseItemID);
		assert(pEquipChipCombineCfg);
		
		scItemInfo.set_icombineneedcount(pEquipChipCombineCfg->iNeedCount);
	}
}

extern int Prop2LifeAtt(int iPropID);


int Item::convertAddPropIdToLifeAttri(int _addPropId)
{
	// 修正propID
	int adjustedPropId = _addPropId;
	switch(_addPropId)
		{
			case PROP_ENTITY_MAXHPAVALUE:
				adjustedPropId = PROP_ENTITY_MAXHP;
				break;
			case PROP_ENTITY_ATTAVALUE:
				adjustedPropId = PROP_ENTITY_ATT;
				break;
			case PROP_ENTITY_HITAVALUE:
				adjustedPropId = PROP_ENTITY_HIT;
				break;
			case PROP_ENTITY_DOGEAVALUE:
				adjustedPropId = PROP_ENTITY_DOGE;
				break;
			case PROP_ENTITY_KNOCKAVALUE:
				adjustedPropId = PROP_ENTITY_KNOCK;
				break;
			case PROP_ENTITY_ANTIKNOCKAVALUE:
				adjustedPropId = PROP_ENTITY_ANTIKNOCK;
				break;
			case PROP_ENTITY_BLOCKAVALUE:
				adjustedPropId = PROP_ENTITY_BLOCK;
				break;
			case PROP_ENTITY_WRECKAVALUE:
				adjustedPropId = PROP_ENTITY_WRECK;
				break;
			case PROP_ENTITY_ARMORAVALUE:
				adjustedPropId = PROP_ENTITY_ARMOR;
				break;
			case PROP_ENTITY_SUNDERAVALUE:
				adjustedPropId = PROP_ENTITY_SUNDER;
				break;
			case PROP_ENTITY_INITANGERAVALUE:
				adjustedPropId = PROP_ENTITY_INITANGER;
				break;
			case PROP_ENTITY_SKILLDAMAGE:
				adjustedPropId = PROP_ENTITY_SKILLDAMAGE;
				break;
			case PROP_ENTITY_SKILLDEF:
				adjustedPropId = PROP_ENTITY_SKILLDEF;
				break;
		}
		return Prop2LifeAtt(adjustedPropId);
}



void Item::packScEquipInfo(GSProto::EquipInfo& scEquipInfo, int iPos)
{
	int iBaseItemID = getProperty(PROP_ITEM_BASEID, 0);

	int iItemType = getProperty(PROP_ITEM_TYPE, 0);
	assert(iItemType == GSProto::en_ItemType_Equip);

	scEquipInfo.set_dwobjectid(m_hHandle);
	scEquipInfo.set_iitemid(iBaseItemID);
	scEquipInfo.set_ipos(iPos);

	int iEquipLevel = getProperty(PROP_EQUIP_LEVEL, 0);
	scEquipInfo.set_iequiplevel(iEquipLevel);

	if(m_pEquipProp)
	{
		// 从修改属性到显示属性之间，需要有转换
		if(s_prop2ShowLifeAttMap.size() == 0)
		{
			s_prop2ShowLifeAttMap[PROP_ENTITY_MAXHPAVALUE] = PROP_ENTITY_MAXHP;
			s_prop2ShowLifeAttMap[PROP_ENTITY_ATTAVALUE] = PROP_ENTITY_ATT;
			
			s_prop2ShowLifeAttMap[PROP_ENTITY_HITAVALUE] = PROP_ENTITY_HIT;
			s_prop2ShowLifeAttMap[PROP_ENTITY_DOGEAVALUE] = PROP_ENTITY_DOGE;
			s_prop2ShowLifeAttMap[PROP_ENTITY_KNOCKAVALUE] = PROP_ENTITY_KNOCK;

			s_prop2ShowLifeAttMap[PROP_ENTITY_ANTIKNOCKAVALUE] = PROP_ENTITY_ANTIKNOCK;
			s_prop2ShowLifeAttMap[PROP_ENTITY_BLOCKAVALUE] = PROP_ENTITY_BLOCK;

			s_prop2ShowLifeAttMap[PROP_ENTITY_WRECKAVALUE] = PROP_ENTITY_WRECK;
			s_prop2ShowLifeAttMap[PROP_ENTITY_ARMORAVALUE] = PROP_ENTITY_ARMOR;

			s_prop2ShowLifeAttMap[PROP_ENTITY_SUNDERAVALUE] = PROP_ENTITY_SUNDER;
			s_prop2ShowLifeAttMap[PROP_ENTITY_INITANGERAVALUE] = PROP_ENTITY_INITANGER;
			s_prop2ShowLifeAttMap[PROP_ENTITY_SKILLDAMAGE] = PROP_ENTITY_SKILLDAMAGE;
			s_prop2ShowLifeAttMap[PROP_ENTITY_SKILLDEF] = PROP_ENTITY_SKILLDEF;
		}

		int iAddPercent = getEquipPropAddPercent();
	
		for(map<int, int>::const_iterator it = m_pEquipProp->baseProp.begin(); it != m_pEquipProp->baseProp.end(); it++)
		{
			int iPropID = it->first;
			int iLifeAttID = -1;
			if(s_prop2ShowLifeAttMap.find(iPropID) != s_prop2ShowLifeAttMap.end() )
			{
				iLifeAttID = Prop2LifeAtt(s_prop2ShowLifeAttMap[iPropID]);
			}

			if(iLifeAttID < 0) continue;

			int iValue = it->second;
			iValue = (double)iValue * (1.0 + (double)iAddPercent/10000);
			
			GSProto::PropItem* pPropItem = scEquipInfo.add_szbaseproplist();
			pPropItem->set_ilifeattid(iLifeAttID);
			pPropItem->set_ivalue(iValue);
		}

		for(map<int, int>::const_iterator it = m_pEquipProp->specialProp.begin(); it != m_pEquipProp->specialProp.end(); it++)
		{
			int iPropID = it->first;
			int iLifeAttID = -1;
			if(s_prop2ShowLifeAttMap.find(iPropID) != s_prop2ShowLifeAttMap.end() )
			{
				iLifeAttID = Prop2LifeAtt(s_prop2ShowLifeAttMap[iPropID]);
			}

			if(iLifeAttID < 0) continue;
			
			GSProto::PropItem* pPropItem = scEquipInfo.add_szspecialproplist();
			pPropItem->set_ilifeattid(iLifeAttID);
			pPropItem->set_ivalue(it->second);
		}

		for(size_t i = 0; i < this->xilianProp.size(); i ++){
			const XilianPropItem& xilianPropItem = this->xilianProp[i];
			int propId = xilianPropItem.val1;
			int value = xilianPropItem.val2;
			int lifeAttId = -1;
			if(s_prop2ShowLifeAttMap.find(propId) != s_prop2ShowLifeAttMap.end() )
			{
				lifeAttId = Prop2LifeAtt(s_prop2ShowLifeAttMap[propId]);
			}
			if(lifeAttId < 0) continue;
			GSProto::PropItem* propItem = scEquipInfo.add_xilianproplist();
			propItem->set_ilifeattid(lifeAttId);
			propItem->set_ivalue(value);
		}

		// 填充套装ID

		ItemFactory* pItemFactory = static_cast<ItemFactory*>(getComponent<IItemFactory>(COMPNAME_ItemFactory, IID_IItemFactory) );
		assert(pItemFactory);
		int iSuitID = pItemFactory->querySuitIDFromEquip(iBaseItemID);
		scEquipInfo.set_isuitid(iSuitID);

		int iSellValue = getProperty(PROP_EQUIP_SELLPRICE, 0);
		int iEnhanceSilver = getProperty(PROP_EQUIP_ENHANCEMONEY, 0);
		scEquipInfo.set_isellmoney(iSellValue + iEnhanceSilver);

	
		int iRandomPropID = getProperty(PROP_EQUIP_RANDOMPROP, 0);
		int iRandValue = getProperty(PROP_EQUIP_RANDOMVALUE, 0);

		if(iRandomPropID > 0)
		{
			GSProto::PropItem * pPropItem = scEquipInfo.mutable_equiprandomprop();

			int iLifeAtt = pItemFactory->getShowLifeAtt(iRandomPropID);
			
			pPropItem->set_ilifeattid(iLifeAtt);
			pPropItem->set_ivalue(iRandValue);
		}
	}
}


int Item::getEquipPropAddPercent()
{
	int iEquipLevel = getProperty(PROP_EQUIP_LEVEL, 0);
	
	ITable* pEquipEnhanceTb = getCompomentObjectManager()->findTable("EquipEnhance");
	assert(pEquipEnhanceTb);

	int iRecord = pEquipEnhanceTb->findRecord(iEquipLevel);
	assert(iRecord >= 0);

	int iAddPercent = pEquipEnhanceTb->getInt(iRecord, "属性加成比例");

	return iAddPercent;
}


void Item::randomEquipProp()
{
	if(!m_pEquipProp) return;

	int iRandomGroupID = getProperty(PROP_EQUIP_RANDOMPROP_GROUPID, 0);

	ItemFactory* pItemFactory = static_cast<ItemFactory*>(getComponent<IItemFactory>(COMPNAME_ItemFactory, IID_IItemFactory) );
	assert(pItemFactory);

	int iPropID = 0;
	int iValue = 0;
	if(pItemFactory->randomEquipProp(iRandomGroupID, iPropID, iValue) )
	{
		setProperty(PROP_EQUIP_RANDOMPROP, iPropID);
		setProperty(PROP_EQUIP_RANDOMVALUE, iValue);
	}
}


void Item::changeProperty(HEntity hEntity, bool bOn)
{
	if(!m_pEquipProp) return;

	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	int iAddPercent = getEquipPropAddPercent();

	for(map<int, int>::const_iterator it = m_pEquipProp->baseProp.begin(); it != m_pEquipProp->baseProp.end(); it++)
	{
		int iPropID = it->first;
		int iValue = it->second;

		iValue = (double)iValue * (1.0 + (double)iAddPercent/10000);		
		if(bOn)
		{
			pEntity->changeProperty(iPropID, iValue, 0);
		}
		else
		{
			pEntity->changeProperty(iPropID, -iValue, 0);
		}
	}

	for(map<int, int>::const_iterator it = m_pEquipProp->specialProp.begin(); it != m_pEquipProp->specialProp.end(); it++)
	{
		int iPropID = it->first;
		int iValue = it->second;
		if(bOn)
		{
			pEntity->changeProperty(iPropID, iValue, 0);
		}
		else
		{
			pEntity->changeProperty(iPropID, -iValue, 0);
		}
	}

	for(size_t i = 0; i < this->xilianProp.size(); i ++){
		const XilianPropItem& item = this->xilianProp[i];
		int propId = item.val1;
		int value = item.val2;
		if(bOn){
			pEntity->changeProperty(propId, value, 0);
		}else{
			pEntity->changeProperty(propId, -value, 0);
		}
	}

		
	int iRandomPropID = getProperty(PROP_EQUIP_RANDOMPROP, 0);
	int iRandValue = getProperty(PROP_EQUIP_RANDOMVALUE, 0);

	if(iRandomPropID > 0)
	{
		if(bOn)
		{
			pEntity->changeProperty(iRandomPropID, iRandValue, 0);
		}
		else
		{
			pEntity->changeProperty(iRandomPropID, -iRandValue, 0);
		}
	}
}

XilianPropVec& Item::getXilianProp()
{
	return this->xilianProp;
}


