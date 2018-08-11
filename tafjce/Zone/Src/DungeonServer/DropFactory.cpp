#include "DungeonServerPch.h"
#include "DropFactory.h"
#include "IHeroSystem.h"
#include "IEquipBagSystem.h"

extern int LifeAtt2Prop(int iLifeAtt);

extern "C" IComponent* createDropFactory(Int32)
{
	return new DropFactory;
}


DropFactory::DropFactory()
{
}

DropFactory::~DropFactory()
{
}

bool DropFactory::initlize(const PropertySet& propSet)
{
	loadDropCfg();

	return true;
}


void DropFactory::loadDropCfg()
{
	ITable* pDropTb = getCompomentObjectManager()->findTable(TABLENAME_Drop);
	assert(pDropTb);

	int iRecordCount = pDropTb->getRecordCount();
	for(int i = 0; i < iRecordCount; i++)
	{
		int iDropID = pDropTb->getInt(i, "掉落ID");
		int iTag = pDropTb->getInt(i, "次数");

		DropSingleItem tmpItem;
		//tmpItem.iDropTypeID = pDropTb->getInt(i, "类型Id");
		string strTypeString = pDropTb->getString(i, "类型Id");
		vector<int> typeIDList =  TC_Common::sepstr<int>(strTypeString, "#");
		if(typeIDList.size() == 1)
		{
			tmpItem.iDropTypeID = typeIDList[0];
			if(typeIDList[0] < GSProto::en_LifeAtt_End)
			{
				tmpItem.iType = en_DropType_Prop;
			}
			else
			{
				tmpItem.iType = en_DropType_Item;
			}
		}
		else if(typeIDList.size() == 2)
		{
			tmpItem.iType = en_DropType_Hero;
			tmpItem.iDropTypeID = typeIDList[0];
			tmpItem.iHeroLevelStep = typeIDList[1];
		}
		else
		{
			assert(false);
		}
		

		tmpItem.iCount = pDropTb->getInt(i, "数量");
		tmpItem.iChance = pDropTb->getInt(i, "机率");

		m_mapDropGroup[iDropID].mapDropList[iTag].push_back(tmpItem);
	}
}

void DropFactory::excuteDrop(HEntity hActor,  GSProto::FightAwardResult& awardResult, int iReason)
{
	CloseAttCommUP close(hActor);
	_excuteDrop(hActor, awardResult, iReason);
}


void DropFactory::_excuteDrop(HEntity hActor,  GSProto::FightAwardResult& awardResult, int iReason)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	assert(pActor);

	for(int i = 0; i < awardResult.szawardproplist().size(); i++)
	{
		const GSProto::PropItem& pPropAward = awardResult.szawardproplist().Get(i);
		
		int iLifeAttID = pPropAward.ilifeattid();
		int iValue = pPropAward.ivalue();

		_awardOneProp(hActor, iLifeAttID, iValue, iReason);
	}

	for(int i = 0; i < awardResult.szawarditemlist().size(); i++)
	{
		GSProto::FightAwardItem* pItemAward = ((GSProto::FightAwardResult&)awardResult).mutable_szawarditemlist(i);
		_awardOneItem(hActor, *pItemAward, pItemAward->iitemid(), pItemAward->icount(), iReason);
	}

	for(int i = 0; i < awardResult.szawardherolist().size(); i++)
	{
		GSProto::FightAwardHero* pAwardHero = ((GSProto::FightAwardResult&)awardResult).mutable_szawardherolist(i);
		_awardOneHero(hActor, *pAwardHero, iReason);
	}

	// pay 
	for(int i = 0; i < awardResult.szawardpaylist().size(); i ++)
	{
		GSProto::AwardPay* pPay = ((GSProto::FightAwardResult&)awardResult).mutable_szawardpaylist(i);
		_awardOnePay(hActor, *pPay, iReason);
	}

	
	
}


void DropFactory::excuteDropWithNotify(HEntity hActor,  GSProto::FightAwardResult& awardResult, int iReason)
{
	_excuteDrop(hActor, awardResult, iReason);
}

void DropFactory::_awardOnePay(HEntity hActor, GSProto::AwardPay & awardPay,int reason)
{
	
	IEntity* roleE = getEntityFromHandle(hActor);
	assert(roleE);
	IShopSystem * shopSystem = static_cast<IShopSystem*>(roleE->querySubsystem(IID_IShopSystem));
	assert(shopSystem);

	int ybAmount = 10 * awardPay.money();
	int count = awardPay.count();
	for(int i = 0; i < count ; i ++)
	{
		shopSystem->addGoldForPayment(ybAmount, "mail");
	}
	
}

void DropFactory::_awardOneHero(HEntity hActor, GSProto::FightAwardHero& awardHero, int iReason)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	assert(pActor);
	IHeroSystem* pHeroSys = static_cast<IHeroSystem*>(pActor->querySubsystem(IID_IHeroSystem));
	assert(pHeroSys);

	//int iCount = awardHero.icount();
	//for(int i = 0; i < iCount; i++)
	//{
		AddHeroReturn addHeroReturn(hActor);
		HEntity hHeroEntity = pHeroSys->addHeroWithLevelStep(awardHero.iheroid(), awardHero.ilevelstep(), false, iReason);
		if(0 == hHeroEntity)
		{
			int iCount = addHeroReturn.iSoulCount;
			int iSoulId = addHeroReturn.iSoulId;
			awardHero.set_icount( iCount);
			awardHero.set_isoulid( iSoulId);
			awardHero.set_bhavechgsoul(true);
			//cout<<iCount << "|" << iSoulId <<endl;
		}
		else
		{
			awardHero.set_bhavechgsoul(false);
		}
	//}
}


void DropFactory::_awardOneProp(HEntity hActor, int iLifeAttID, int iValue, int iReason)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	assert(pActor);

	if(GSProto::en_LifeAtt_Exp == iLifeAttID)
	{
		pActor->addExp(iValue);
	}
	else
	{
		int iPropID = LifeAtt2Prop(iLifeAttID);
		assert(iPropID >= 0);

		pActor->changeProperty(iPropID, iValue, iReason);
	}
}

void DropFactory::_awardOneItem(HEntity hActor, GSProto::FightAwardItem& pItemAward, int iItemID, int iCount, int iReason)
{
	IEntity* pActor = getEntityFromHandle(hActor);
	assert(pActor);

	IItemSystem* pItemSys = static_cast<IItemSystem*>(pActor->querySubsystem(IID_IItemSystem));
	assert(pItemSys);

	// 装备特殊处理
	IItemFactory* pItemFactory = getComponent<IItemFactory>(COMPNAME_ItemFactory, IID_IItemFactory);
	assert(pItemFactory);

	const PropertySet* pItemProp = pItemFactory->getItemPropset(iItemID);
	assert(pItemProp);

	int iItemType = pItemProp->getInt(PROP_ITEM_TYPE, 0);

	if(iItemType == GSProto::en_ItemType_Equip)
	{
		vector<HItem> itemList;
		if(pItemFactory->createItemFromID(iItemID, 1, itemList) )
		{
			assert(itemList.size() > 0);
			HItem hEquip = itemList[0];
			IItem* pEquip = getItemFromHandle(hEquip);
			assert(pEquip);

			int iDynamicPropID = pEquip->getProperty(PROP_EQUIP_RANDOMPROP, 0);
			int iDynamicValue = pEquip->getProperty(PROP_EQUIP_RANDOMVALUE, 0);

			if(iDynamicPropID > 0)
			{
				int iLifeAtt = pItemFactory->getShowLifeAtt(iDynamicPropID);

				GSProto::PropItem* pPropItem = pItemAward.mutable_dynamicequipprop();
				pPropItem->set_ilifeattid(iLifeAtt);
				pPropItem->set_ivalue(iDynamicValue);
			}

			IEquipBagSystem* pEquipBagSys = static_cast<IEquipBagSystem*>(pActor->querySubsystem(IID_IEquipBagSystem));
			assert(pEquipBagSys);

			pEquipBagSys->addEquipByHandle(hEquip, iReason, false);
		}
		else
		{
			assert(false);
		}
	}
	else
	{
		pItemSys->addItem(iItemID, iCount, iReason);
	}
}

bool DropFactory::calcDrop(int iDropID, GSProto::FightAwardResult& awardResult)
{
	MapDropGroup::iterator it = m_mapDropGroup.find(iDropID);
	if(it == m_mapDropGroup.end() )
	{
		return false;
	}

	const DropGroup& dropGroupData = it->second;

	_calcDropGroup(dropGroupData, awardResult);

	return awardResult.IsInitialized();
}

void DropFactory::_calcDropGroup(const DropGroup& groupData, GSProto::FightAwardResult& awardResult)
{
	for(map<int, vector<DropSingleItem> >::const_iterator it = groupData.mapDropList.begin(); it != groupData.mapDropList.end(); it++)
	{
		const vector<DropSingleItem>& dropItemList = it->second;
		_calcDropOneTag(dropItemList, awardResult);
	}
}

void DropFactory::addItemToResult(GSProto::FightAwardResult& awardResult, int iItemID, int iItemCount)
{
	for(int i = 0; i < awardResult.mutable_szawarditemlist()->size(); i++)
	{
		GSProto::FightAwardItem* pAwardItem = awardResult.mutable_szawarditemlist(i);
		if(pAwardItem->iitemid() != iItemID)
		{
			continue;
		}

		pAwardItem->set_icount(pAwardItem->icount() + iItemCount);
		return;
	}

	// 添加新的吧
	GSProto::FightAwardItem* pNewItem = awardResult.mutable_szawarditemlist()->Add();
	pNewItem->set_iitemid(iItemID);
	pNewItem->set_icount(iItemCount);
}

void DropFactory::addPropToResult(GSProto::FightAwardResult& awardResult, int iLifeAttID, int iValue)
{
	for(int i = 0; i < awardResult.mutable_szawardproplist()->size(); i++)
	{
		GSProto::PropItem* pPropItem = awardResult.mutable_szawardproplist(i);
		if(pPropItem->ilifeattid() != iLifeAttID)
		{
			continue;
		}

		pPropItem->set_ivalue(pPropItem->ivalue() + iValue);
		return;
	}

	// 添加新的吧
	GSProto::PropItem* pNewPropItem = awardResult.mutable_szawardproplist()->Add();
	pNewPropItem->set_ilifeattid(iLifeAttID);
	pNewPropItem->set_ivalue(iValue);
}

void DropFactory::addHeroToResult(GSProto::FightAwardResult& awardResult, int iHeroID, int iLevelStep, int iCount)
{
	GSProto::FightAwardHero* pNewAward = awardResult.mutable_szawardherolist()->Add();
	pNewAward->set_iheroid(iHeroID);
	pNewAward->set_ilevelstep(iLevelStep);
	pNewAward->set_icount(iCount);
	pNewAward->set_bhavechgsoul(false);

	ITable* pHeroTb = getCompomentObjectManager()->findTable(TABLENAME_Hero);
	assert(pHeroTb);
	
	int iRecord = pHeroTb->findRecord(iHeroID);
	assert(iRecord >= 0);

	int iQuality = pHeroTb->getInt(iRecord, "初始品质");
	pNewAward->set_iquality(iQuality);
}


void DropFactory::_calcDropOneTag(const vector<DropSingleItem>& itemList, GSProto::FightAwardResult& awardResult)
{
	IRandom* pRandom = getComponent<IRandom>("Random", IID_IMiniAprRandom);
	assert(pRandom);

	int iRandV = pRandom->random() % 10000;
	
	int iSumChance = 0;
	for(vector<DropSingleItem>::const_iterator it = itemList.begin(); it != itemList.end(); it++)
	{
		const DropSingleItem& tmpDropSingleItem = *it;
		iSumChance += tmpDropSingleItem.iChance;
		if(iRandV < iSumChance)
		{
			if(tmpDropSingleItem.iType == en_DropType_Prop)
			{
				addPropToResult(awardResult, tmpDropSingleItem.iDropTypeID, tmpDropSingleItem.iCount);
			}
			else if(tmpDropSingleItem.iType == en_DropType_Item)
			{
				addItemToResult(awardResult, tmpDropSingleItem.iDropTypeID, tmpDropSingleItem.iCount);
			}
			else if(tmpDropSingleItem.iType == en_DropType_Hero)
			{
				addHeroToResult(awardResult, tmpDropSingleItem.iDropTypeID, tmpDropSingleItem.iHeroLevelStep, tmpDropSingleItem.iCount);
			}
			
			return;
		}
	}
}




