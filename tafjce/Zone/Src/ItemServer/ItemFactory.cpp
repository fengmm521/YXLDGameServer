#include "ItemServerPch.h"
#include "ItemFactory.h"
#include "Item.h"


extern "C" IComponent* createItemFactory(Int32)
{
	return new ItemFactory;
}


ItemFactory::ItemFactory():
m_xilianLvLimit(0)
{
}

ItemFactory::~ItemFactory()
{
}

bool ItemFactory::initlize(const PropertySet& propSet)
{
	loadGlobal();
	loadItem();
	loadEquip();
	loadEquipRandom();
	loadEquipCombine();
	loadEquipSuit();
	loadConvertCode();
	loadRandomPropShow();
	
	return true;
}

extern int Prop2LifeAtt(int iPropID);

void ItemFactory::loadRandomPropShow()
{
	ITable* pTable = getCompomentObjectManager()->findTable("RandomPropShow");
	assert(pTable);

	IEntityFactory* pEntityFactory = getComponent<IEntityFactory>("EntityFactory", IID_IEntityFactory);
	assert(pEntityFactory);


	IPropDefine* pPropDefine = pEntityFactory->getPropDefine("EntityPropDef");
	assert(pPropDefine);

	for(int i = 0; i < pTable->getRecordCount(); i++)
	{
		string strKey = pTable->getString(i, "属性名");
		string strShowName = pTable->getString(i, "显示LifeAtt名称");

		int iPropID = pPropDefine->getPropKey(strKey);
		assert(iPropID >= 0);

		int iShowPropID = pPropDefine->getPropKey(strShowName);
		assert(iShowPropID >= 0);

		//int iShowLifeAtt = Prop2LifeAtt(iShowPropID);
		//assert(iShowLifeAtt >= 0);

		m_randomPropLifeAttMap[iPropID] = iShowPropID;
	}
}


int ItemFactory::getShowLifeAtt(int iPropID)
{
	map<int, int>::iterator it = m_randomPropLifeAttMap.find(iPropID);
	if(it == m_randomPropLifeAttMap.end() ) return -1;

	int iShowPropID = it->second;

	int iRet = Prop2LifeAtt(iShowPropID);
	assert(iRet >= 0);
	
	return iRet;
}



void ItemFactory::loadEquipRandom()
{
	ITable* pEqupRandomPropTb = getCompomentObjectManager()->findTable("EquipRandomProp");
	assert(pEqupRandomPropTb);

	int iRecordCount = pEqupRandomPropTb->getRecordCount();
	for(int i = 0; i < iRecordCount; i++)
	{
		int iGroupID = pEqupRandomPropTb->getInt(i, "属性组ID");
		string strPropInfo = pEqupRandomPropTb->getString(i, "随机属性");

		vector<string> propCfgList = TC_Common::sepstr<string>(strPropInfo, Effect_MagicSep);

		assert(propCfgList.size() % 4 == 0);

		fillEquipRandPropCfg(iGroupID, m_equipRandomPropCfgMap[iGroupID], propCfgList);
	}
}


bool ItemFactory::randomEquipProp(int iGroupID, int& iPropID, int& iValue)
{
	map<int, vector<EquipRandomPropItem> >::iterator it = m_equipRandomPropCfgMap.find(iGroupID);
	if(it == m_equipRandomPropCfgMap.end() )
	{
		return false;
	}

	const vector<EquipRandomPropItem>& propCfgList = it->second;

	int iSumChance = 0;
	for(size_t i = 0; i < propCfgList.size(); i++)
	{
		iSumChance += propCfgList[i].iChance;
	}

	if(0 == iSumChance) return false;

	IRandom* pRandom = getComponent<IRandom>("Random", IID_IMiniAprRandom);
	assert(pRandom);

	int iRandV = pRandom->random() % iSumChance;

	int iSum = 0;
	for(size_t i = 0 ; i < propCfgList.size() ; i++)
	{
		iSum += propCfgList[i].iChance;
		if(iRandV < iSum)
		{
			iPropID = propCfgList[i].iPropID;
			int iMinV = propCfgList[i].iMinValue;
			int iMaxV = propCfgList[i].iMaxValue;
			iValue = pRandom->random()%(iMaxV - iMinV + 1) + iMinV;
			return true;
		}
	}
	
	return false;
}


void ItemFactory::fillEquipRandPropCfg(int iGroupID, vector<EquipRandomPropItem>& randomPropList, const vector<string>& configList)
{
	IEntityFactory* pEntityFactory = getComponent<IEntityFactory>("EntityFactory", IID_IEntityFactory);
	assert(pEntityFactory);


	IPropDefine* pPropDefine = pEntityFactory->getPropDefine("EntityPropDef");
	assert(pPropDefine);

	for(size_t i = 0; i < configList.size()/4; i++)
	{
		EquipRandomPropItem tmpItem;
	
		tmpItem.iPropID = pPropDefine->getPropKey(configList[i*4]);
		assert(tmpItem.iPropID >= 0);
		tmpItem.iMinValue = AdvanceAtoi(configList[i*4+1]);
		tmpItem.iMaxValue= AdvanceAtoi(configList[i*4+2]);

		assert(tmpItem.iMinValue <= tmpItem.iMaxValue);
		tmpItem.iChance = AdvanceAtoi(configList[i*4+3]);

		randomPropList.push_back(tmpItem);
	}
}

int ItemFactory::getXilianLvLimit()
{
	return this->m_xilianLvLimit;
}

void ItemFactory::loadEquip()
{
	ITable* pEquipTb = getCompomentObjectManager()->findTable(TABLENAME_Equip);
	assert(pEquipTb);

	IPropDefine* pPropDefine = getPropDefine("ItemPropDef");
	assert(pPropDefine);

	int iRecordCount = pEquipTb->getRecordCount();
	for(int i = 0; i < iRecordCount; i++)
	{
		PropertySet itemStaticProp;
		pEquipTb->initPropSet(pPropDefine, i, itemStaticProp, false);
		itemStaticProp.setInt(PROP_ITEM_TYPE, (int)GSProto::en_ItemType_Equip);
		itemStaticProp.setInt(PROP_ITEM_MAXSTACKCOUNT, (int)1);

		int iItemID = itemStaticProp.getInt(PROP_ITEM_BASEID);
		assert(iItemID > 0);

		assert(m_itemStaticPropMap.find(iItemID) == m_itemStaticPropMap.end() );
		
		m_itemStaticPropMap[iItemID] = itemStaticProp;


		// 战斗属性
		
		EquipProp tmpEquipProp;
		string strBaseProp = itemStaticProp.getString(PROP_EQUIP_BASEPROP, "");
		string strSpecialProp = itemStaticProp.getString(PROP_EQUIP_SPECIALPROP, "");

		parseEquipProp(strBaseProp, tmpEquipProp.baseProp);
		parseEquipProp(strSpecialProp, tmpEquipProp.specialProp);

		assert(m_equipPropMap.find(iItemID) == m_equipPropMap.end() );
		m_equipPropMap[iItemID] = tmpEquipProp;
	}
}

const EquipProp* ItemFactory::getEquipFightProp(int iEquipID)
{
	MapEquipProp::iterator it = m_equipPropMap.find(iEquipID);
	if(it != m_equipPropMap.end() )
	{
		return &(it->second);
	}

	return NULL;
}


void ItemFactory::parseEquipProp(const string& strPopDesc, map<int, int>& propMap)
{
	IEntityFactory* pEntityFactory = getComponent<IEntityFactory>("EntityFactory", IID_IEntityFactory);
	assert(pEntityFactory);

	vector<string> strPropList = TC_Common::sepstr<string>(strPopDesc, Effect_MagicSep);
	assert(strPropList.size() % 2 == 0);

	for(size_t i = 0; i < strPropList.size()/2; i++)
	{
		string strPropName = strPropList[i*2];
		int iValue = TC_Common::strto<int>(strPropList[i*2+1]);
		int iPropID = pEntityFactory->getPropDefine("EntityPropDef")->getPropKey(strPropName);
		assert(iPropID >= 0);

		propMap[iPropID] += iValue;
	}
}


void ItemFactory::loadEquipCombine()
{
	ITable* pEquipCombineTb = getCompomentObjectManager()->findTable(TABLENAME_EquipCombine);
	assert(pEquipCombineTb);

	int iRecordCount = pEquipCombineTb->getRecordCount();
	for(int i = 0; i < iRecordCount; i++)
	{
		EquipChipCombine tmpChipCombine;
		tmpChipCombine.iEquipChipID = pEquipCombineTb->getInt(i, "材料ID");
		tmpChipCombine.iNeedCount = pEquipCombineTb->getInt(i, "材料数量");
		tmpChipCombine.iCreateEquipID = pEquipCombineTb->getInt(i, "产出ID");
		tmpChipCombine.iBaseCostSilver = pEquipCombineTb->getInt(i, "消耗铜币");

		assert(m_mapEquipChipCombine.find(tmpChipCombine.iEquipChipID) == m_mapEquipChipCombine.end() );

		m_mapEquipChipCombine[tmpChipCombine.iEquipChipID] = tmpChipCombine;
	}
}

void ItemFactory::loadEquipSuit()
{
	ITable* pEquipShitTb = getCompomentObjectManager()->findTable("EquipSuit");
	assert(pEquipShitTb);

	int iRecordCount = pEquipShitTb->getRecordCount();

	for(int i = 0; i < iRecordCount; i++)
	{
		EquipSuit tmpSuit;

		tmpSuit.iSuitID = pEquipShitTb->getInt(i, "套装ID");

		string strEquipList = pEquipShitTb->getString(i, "套装列表");
		tmpSuit.equipIDList = TC_Common::sepstr<int>(strEquipList, Effect_MagicSep);
		tmpSuit.suitPropList.resize(3);
		
		for(int iIndex = 0; iIndex <3; iIndex++)
		{
			stringstream ss;
			ss<<(iIndex+2)<<"件属性";
			string strKey = ss.str();

			string strPropDesc = pEquipShitTb->getString(i, strKey.c_str() );

			if(strPropDesc.size() == 0) continue;

			parseSuitProp(strPropDesc, tmpSuit.suitPropList[iIndex]);
		}

		m_mapEquipSuitCfg[tmpSuit.iSuitID] = tmpSuit;

		// 建立装备到套装的索引
		for(vector<int>::iterator it = tmpSuit.equipIDList.begin(); it != tmpSuit.equipIDList.end(); it++)
		{
			m_equipSuitIDMap[*it] = tmpSuit.iSuitID;
		}
	}
}


void ItemFactory::parseSuitProp(const string& strDesc, map<int, int>& propMap)
{
	assert(strDesc.size() > 0);

	vector<string> propList = TC_Common::sepstr<string>(strDesc, Effect_MagicSep);
	assert(propList.size() % 2 == 0);

	IEntityFactory* pEntityFactory = getComponent<IEntityFactory>("EntityFactory", IID_IEntityFactory);
	assert(pEntityFactory);

	for(size_t i = 0; i < propList.size()/2; i++)
	{
		int iPropID = pEntityFactory->getPropDefine("EntityPropDef")->getPropKey(propList[i*2]);
		int iValue = AdvanceAtoi(propList[i*2+1]);

		assert(iPropID >= 0);

		propMap[iPropID] = iValue;
	}
}


int ItemFactory::querySuitIDFromEquip(int iEquipID)
{
	map<int, int>::iterator it = m_equipSuitIDMap.find(iEquipID);
	if(it != m_equipSuitIDMap.end() )
	{
		return it->second;
	}

	return 0;
}

const EquipSuit* ItemFactory:: querySuitCfg(int iSuitID)
{
	map<Int32, EquipSuit>::iterator it = m_mapEquipSuitCfg.find(iSuitID);
	if(it != m_mapEquipSuitCfg.end() )
	{
		return &(it->second);
	}

	return NULL;
}


const EquipChipCombine* ItemFactory::queryEquipChipCombine(int iEquipChipID)
{
	map<Int32, EquipChipCombine>::iterator it = m_mapEquipChipCombine.find(iEquipChipID);
	if(it == m_mapEquipChipCombine.end() )
	{
		return NULL;
	}

	return &(it->second);
}

void ItemFactory::loadGlobal()
{
	IGlobalCfg* globalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(globalCfg);
	this->m_xilianLvLimit = globalCfg->getInt("洗练等级限制", 0);
	printf("ItemFactory::loadGlobal() : xilianLvLimit:%u\n", this->m_xilianLvLimit);
}

void ItemFactory::loadItem()
{
	ITable* pItemTb = getCompomentObjectManager()->findTable(TABLENAME_Item);
	assert(pItemTb);

	IEntityFactory* pEntityFactory = getComponent<IEntityFactory>("EntityFactory", IID_IEntityFactory);
	assert(pEntityFactory);

	IPropDefine* pPropDefine = getPropDefine("ItemPropDef");
	assert(pPropDefine);

	int iRecordCount = pItemTb->getRecordCount();
	for(int i = 0; i < iRecordCount; i++)
	{
		PropertySet itemStaticProp;
		pItemTb->initPropSet(pPropDefine, i, itemStaticProp, false);

		// 去掉，物品和碎片都在这里配置
		//itemStaticProp.setInt(PROP_ITEM_TYPE, (int)GSProto::en_ItemType_Item);

		//cout<<"DROPID ================="<<itemStaticProp.getInt(PROP_EQUIP_DROPID)<<endl;

		int iItemID = itemStaticProp.getInt(PROP_ITEM_BASEID);
		assert(iItemID > 0);

		assert(m_itemStaticPropMap.find(iItemID) == m_itemStaticPropMap.end() );

		m_itemStaticPropMap[iItemID] = itemStaticProp;

		// 喜好品数值
		string strPropInfo = itemStaticProp.getString(PROP_FAVORITE_PROP, "");
		if(strPropInfo.size() > 0)
		{
			vector<string> szPropList = TC_Common::sepstr<string>(strPropInfo, "#");
			assert(szPropList.size() % 2 == 0);

			for(size_t iPropIndex = 0; iPropIndex < szPropList.size()/2; iPropIndex++)
			{
				int iPropID = pEntityFactory->getPropDefine("EntityPropDef")->getPropKey(szPropList[2*iPropIndex]);
				assert(iPropID >= 0);

				int iValue = AdvanceAtoi(szPropList[iPropIndex*2 + 1]);
				m_favoritePropCfg[iItemID][iPropID] = iValue;
			}
		}
		
	}
}

const map<int, int>* ItemFactory::queryFavoriteProp(int iItemID)
{
	map<int, map<int, int> >::iterator it = m_favoritePropCfg.find(iItemID);
	if(it == m_favoritePropCfg.end() )
	{
		return NULL;
	}

	return &(it->second);
}


const PropertySet* ItemFactory::getItemPropset(int iItemID)
{
	MapItemStaticProp::iterator it = m_itemStaticPropMap.find(iItemID);
	if(it == m_itemStaticPropMap.end() )
	{
		return NULL;
	}

	return &(it->second);
}

bool ItemFactory::createItemFromID(int iItemID, int iCount, vector<HItem>& itemList)
{
	const PropertySet* pItemPropset = getItemPropset(iItemID);
	if(!pItemPropset) return false;

	const EquipProp* pEquipProp = getEquipFightProp(iItemID);

	int iMaxStackCount = pItemPropset->getInt(PROP_ITEM_MAXSTACKCOUNT);
	int iHItemCount = iCount/iMaxStackCount;
	int iLastLeftCount = iCount % iMaxStackCount;

	for(int i = 0; i < iHItemCount; i++)
	{
		Item* pNewItem = new Item();
		pNewItem->getPropertySet() += *pItemPropset;
		
		if(pEquipProp)
		{
			pNewItem->m_pEquipProp = (EquipProp*)pEquipProp;
			pNewItem->randomEquipProp();
		}
		
		// 设置StackCount;
		pNewItem->setProperty(PROP_ITEM_STACKCOUNT, iMaxStackCount);
		itemList.push_back(pNewItem->getHandle() );
	}

	// 最后一个物品
	if(iLastLeftCount > 0)
	{
		Item* pNewItem = new Item();
		pNewItem->getPropertySet() += *pItemPropset;

		if(pEquipProp)
		{
			pNewItem->m_pEquipProp = (EquipProp*)pEquipProp;
			pNewItem->randomEquipProp();
		}
		
		// 设置StackCount;
		pNewItem->setProperty(PROP_ITEM_STACKCOUNT, iLastLeftCount);
		itemList.push_back(pNewItem->getHandle() );
	}

	return true;
}

IItem* ItemFactory::createItemFromDB(const ServerEngine::ItemData& itemSaveData)
{
	vector<HItem> itemList;
	if(!createItemFromID(itemSaveData.iItemID, itemSaveData.iStackCount, itemList) )
	{
		return NULL;
	}

	assert(itemList.size() == 1);

	IItem* pNewItem = getItemFromHandle(itemList[0]);
	if(!pNewItem) return NULL;

	// 装备特殊属性存档,EquipData
	int iItemType = pNewItem->getProperty(PROP_ITEM_TYPE, 0);
	if( (GSProto::en_ItemType_Equip == iItemType) && (itemSaveData.strEquipData.size() > 0) )
	{
		ServerEngine::EquipData tmpEquipData;
		ServerEngine::JceToObj(itemSaveData.strEquipData, tmpEquipData);

		pNewItem->setProperty(PROP_EQUIP_LEVEL, tmpEquipData.iLevel);
		pNewItem->setProperty(PROP_EQUIP_ENHANCEMONEY, tmpEquipData.iEnhanceMoney);

		if(tmpEquipData.iRandomPropID > 0)
		{
			pNewItem->setProperty(PROP_EQUIP_RANDOMPROP, tmpEquipData.iRandomPropID);
			pNewItem->setProperty(PROP_EQUIP_RANDOMVALUE, tmpEquipData.iRandomValue);
		}

		// 读入洗练数据
		XilianPropItem tmpItem;
		Item* itemE = (Item*)pNewItem;
		itemE->xilianProp.clear();
		for(size_t i = 0; i < tmpEquipData.xilianPropList.size(); i ++){
			const ServerEngine::XilianPropItem& itemJCE = tmpEquipData.xilianPropList[i];
			int propId = itemJCE.propId;
			int value = itemJCE.value;
			tmpItem.val1 = propId;
			tmpItem.val2 = value;
			itemE->xilianProp.push_back(tmpItem);
		}
	}

	return pNewItem;
}

void ItemFactory::loadConvertCode()
{
	ITable* pConvertCodeTable = getCompomentObjectManager()->findTable("ConvertCode");
	assert(pConvertCodeTable);

	int iRecordCount = pConvertCodeTable->getRecordCount();

	for(int i = 0; i < iRecordCount; i++)
	{
		string strConvetGiftId = pConvertCodeTable->getString(i,"兑换礼包ID");
		vector<string> idVec = TC_Common::sepstr<string>(strConvetGiftId, "#");
		assert(idVec.size() == 3);
		stringstream oss;
		oss<<idVec[0]<<idVec[1];
		
		int iDropId = pConvertCodeTable->getInt(i, "掉落ID");
		assert(iDropId > 0);

		int iFlag = pConvertCodeTable->getInt(i, "礼包类型");
		bool bFlag = (iFlag == 1);

		ConvertCodeItem item;
		item.iDropId = iDropId;
		item.bFlag = bFlag;
		m_ConvertCodeMap[oss.str()] = item;
	}
}

int ItemFactory::getDropIdByConvertGiftId(const string& strConvertGiftId)
{
	map<string , ConvertCodeItem>::iterator iter = m_ConvertCodeMap.find(strConvertGiftId);
	if(iter != m_ConvertCodeMap.end())
	{
		return iter->second.iDropId;
	}
	return -1;
}

bool ItemFactory::bActorCanRepeatGetFlag(const string& strConvertGiftId)
{
	map<string , ConvertCodeItem>::iterator iter = m_ConvertCodeMap.find(strConvertGiftId);
	if(iter != m_ConvertCodeMap.end())
	{
		return iter->second.bFlag;
	}
	return false;
}



