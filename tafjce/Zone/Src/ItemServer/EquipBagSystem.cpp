#include "ItemServerPch.h"
#include "ItemSystem.h"
#include "ErrorCodeDef.h"
#include "ItemFactory.h"
#include "IJZMessageLayer.h"
#include "ILegionFactory.h"
#include "LogHelp.h"
#include "IVIPFactory.h"
#include "IJZEntityFactory.h"
#include "IGodAnimalSystem.h"
#include "EquipBagSystem.h"
#include "IDungeonFactory.h"
#include "HeroRealEquipSystem.h"
#include "Item.h"


extern "C" IObject* createEquipBagSystem()
{
	return new EquipBagSystem;
}



EquipBagSystem::EquipBagSystem():
m_xilianOBJId(0),
m_xilianPropList()
{
}

EquipBagSystem::~EquipBagSystem()
{
	for(size_t i = 0; i < m_equipList.size(); i++)
	{
		IItem* pItem = getItemFromHandle(m_equipList[i]);
		if(!pItem) continue;

		delete pItem;
	}

	m_equipList.clear();
}


Uint32 EquipBagSystem::getSubsystemID() const
{
	return IID_IEquipBagSystem;
}

Uint32 EquipBagSystem::getMasterHandle()
{
	return m_hEntity;
}

bool EquipBagSystem::create(IEntity* pEntity, const std::string& strData)
{
	m_hEntity = pEntity->getHandle();
	m_equipList.resize(getSize(), 0);

	if(strData.size() > 0)
	{
		ServerEngine::EquipBagSystemData saveData;
		ServerEngine::JceToObj(strData, saveData);
		initFromDB(saveData);
	}

	pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_SENDACTOR_TOCLIENT, this, &EquipBagSystem::onEventSend2Client);
	
	return true;
}

void EquipBagSystem::onEventSend2Client(EventArgs& args)
{
	GSProto::CMD_EQUIP_BAG_SC scMsg;
	for(size_t i = 0; i < m_equipList.size(); i++)
	{
		IItem* pTmpEquip = getItemFromHandle(m_equipList[i]);
		if(!pTmpEquip) continue;
	
		GSProto::EquipInfo* pNewEquipInfo = scMsg.add_szequiplist();
		pTmpEquip->packScEquipInfo(*pNewEquipInfo, i);
	}

	//debug
	/*
	printf("\n\n");
	printf("onEventSend2Client\n");
	printf("======================================================\n");
	size_t equipCount = scMsg.szequiplist_size();
	printf("equip count:%u\n", equipCount);
	for(size_t i = 0; i < equipCount; i ++){
		const GSProto::EquipInfo& equipE = scMsg.szequiplist(i);
		printf(">>>>[%u]object_id:%u item_id:%u pos:%i\n", 
			i, 
			equipE.dwobjectid(),
			equipE.iitemid(),
			equipE.ipos());
		size_t xilianPropCount = equipE.xilianproplist_size();
		for(size_t j = 0; j < xilianPropCount; j ++){
			const GSProto::PropItem& propItem = equipE.xilianproplist(j);
			printf("------ prop_type:%u value:%u\n", 
				propItem.ilifeattid(),
				propItem.ivalue());
		}
	}
	printf("======================================================\n");
	printf("\n\n");
	*/
	//debug end

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	pEntity->sendMessage(GSProto::CMD_EQUIP_BAG, scMsg);
}


void EquipBagSystem::initFromDB(const ServerEngine::EquipBagSystemData& equipData)
{
	IItemFactory* pItemFactory = getComponent<IItemFactory>(COMPNAME_ItemFactory, IID_IItemFactory);
	assert(pItemFactory);

	for(map<taf::Int32, ServerEngine::ItemData>::const_iterator it = equipData.equipList.begin(); it != equipData.equipList.end(); it++)
	{
		int iPos = it->first;
		const ServerEngine::ItemData& refItemData = it->second;

		IItem* pEquip = pItemFactory->createItemFromDB(refItemData);
		assert(pEquip);

		m_equipList[iPos] = pEquip->getHandle();
	}
}


bool EquipBagSystem::createComplete()
{
	return true;
}

const std::vector<Uint32>& EquipBagSystem::getSupportMessage()
{
	static vector<Uint32> resultList;
	if(resultList.size() == 0)
	{
		resultList.push_back(GSProto::CMD_QUERY_HEROEQUIP);
		resultList.push_back(GSProto::CMD_EQUIP_MOVE);
		resultList.push_back(GSProto::CMD_EQUIP_SELLBYPOS);
		resultList.push_back(GSProto::CMD_EQUIP_SELLBYQUALITY);
		resultList.push_back(GSProto::CMD_EQUIP_COMBINE);
		resultList.push_back(GSProto::CMD_QUERY_EQUIPDESC_BYID);
		resultList.push_back(GSProto::CMD_QUERY_EQUIPSOURCE);

		resultList.push_back(GSProto::CMD_EQUIP_QUERYENHANCE);
		resultList.push_back(GSProto::CMD_EQUIP_ENHANCE);
		resultList.push_back(GSProto::CMD_EQUIP_CHIPSELL);
		resultList.push_back(GSProto::CMD_EQUIP_CHIPCOMBINE);

		resultList.push_back(GSProto::CMD_EQUIP_RONGLIAN);
		resultList.push_back(GSProto::CMD_EQUIP_XILIAN_DATA);
		resultList.push_back(GSProto::CMD_EQUIP_XILIAN_GEN);
		resultList.push_back(GSProto::CMD_EQUIP_XILIAN_REPLACE);
		resultList.push_back(GSProto::CMD_EQUIP_XILIAN_TRANSFER);
		resultList.push_back(GSProto::CMD_EQUIP_XILIAN_BUYLOCK);
	}

	return resultList;
}

void EquipBagSystem::onMessage(QxMessage* pMessage)
{	
	assert(pMessage->dwMsgLen == sizeof(GSProto::CSMessage) );
	const GSProto::CSMessage& msg = *(const GSProto::CSMessage*)(pMessage->pMsgDataBuff);

	switch(msg.icmd() )
	{
		case GSProto::CMD_QUERY_HEROEQUIP:
			onReqQueryHeroEquip(msg);
			break;

		case GSProto::CMD_EQUIP_MOVE:
			onReqMoveEquip(msg);
			break;

		case GSProto::CMD_EQUIP_SELLBYPOS:
			onReqSellByPos(msg);
			break;

		case GSProto::CMD_EQUIP_SELLBYQUALITY:
			onReqSellByQuality(msg);
			break;

		case GSProto::CMD_EQUIP_COMBINE:
			//onReqEquipCombine(msg);
			break;

		case GSProto::CMD_QUERY_EQUIPDESC_BYID:
			onReqQueryEquipByID(msg);
			break;

		case GSProto::CMD_QUERY_EQUIPSOURCE:
			onReqEquipSource(msg);
			break;

		case GSProto::CMD_EQUIP_QUERYENHANCE:
			onReqQueryEnhance(msg);
			break;

		case GSProto::CMD_EQUIP_ENHANCE:
			onReqEquipEnhance(msg);
			break;

		case GSProto::CMD_EQUIP_CHIPSELL:
			onReqEquipChipSell(msg);
			break;

		case GSProto::CMD_EQUIP_CHIPCOMBINE:
			onReqChipCombine(msg);
			break;

		case GSProto::CMD_EQUIP_RONGLIAN:
			onReqEquipRonglian(msg);
			break;

		case GSProto::CMD_EQUIP_XILIAN_DATA:
			onReqEquipXilianData(msg);
			break;
		case GSProto::CMD_EQUIP_XILIAN_GEN:
			onReqEquipXilianGen(msg);
			break;
		case GSProto::CMD_EQUIP_XILIAN_REPLACE:
			onReqEquipXilianReplace(msg);
			break;
		case GSProto::CMD_EQUIP_XILIAN_TRANSFER:
			onReqEquipXilianTransfer(msg);
			break;
		case GSProto::CMD_EQUIP_XILIAN_BUYLOCK:
			onReqEquipXilianBuyLock(msg);
			break;
	}
}


void EquipBagSystem::onReqQueryEnhance(const GSProto::CSMessage& msg)
{
	GSProto::CMD_EQUIP_QUERYENHANCE_CS req;
	if(!req.ParseFromString(msg.strmsgbody() ) )
	{
		return;
	}

	Uint32 dwObjectID = req.dwheroobjectid();
	int iPos = req.iequippos();

	IEntity* pEntity = getEntityFromHandle(dwObjectID);
	if(!pEntity) return;

	int iClassID = pEntity->getProperty(PROP_ENTITY_CLASS, 0);

	if( ( (GSProto::en_class_Hero == iClassID) && ( (Uint32)pEntity->getProperty(PROP_ENTITY_MASTER, 0)  != m_hEntity) ) || ( (GSProto::en_class_Actor == iClassID) && (dwObjectID != m_hEntity) ) )
	{
		return;
	}

	IEquipContainer* pEquipContainer = getEquipContainer(dwObjectID);
	if(!pEquipContainer) return;

	HItem hEquip = pEquipContainer->getEquip(iPos);
	
	IItem* pEquip = getItemFromHandle(hEquip);
	if(!pEquip) return;

	int iCurEquipLevel = pEquip->getProperty(PROP_EQUIP_LEVEL, 0);
	if(!checkEquuipLevelLimit(iCurEquipLevel + 1, true) )
	{
		return;
	}
	
	GSProto::CMD_EQUIP_QUERYENHANCE_SC scMsg;
	scMsg.set_dwheroobjectid(dwObjectID);
	scMsg.set_iequippos(iPos);
	scMsg.set_icurlevel(iCurEquipLevel);
	scMsg.set_inextlevel(iCurEquipLevel+1);

	{
		GSProto::EquipInfo tmpCurEquipInfo;
		pEquip->packScEquipInfo(tmpCurEquipInfo, iPos);
		*scMsg.mutable_szcurproplist() = *tmpCurEquipInfo.mutable_szbaseproplist();
	}

	{
		GSProto::EquipInfo tmpCurEquipInfo;
		pEquip->setProperty(PROP_EQUIP_LEVEL, iCurEquipLevel+1);
		pEquip->packScEquipInfo(tmpCurEquipInfo, iPos);
		*scMsg.mutable_sznextproplist() = *tmpCurEquipInfo.mutable_szbaseproplist();

		// 恢复
		pEquip->setProperty(PROP_EQUIP_LEVEL, iCurEquipLevel);
	}

	// 填充需要的金钱
	ITable* pEquipEnhanceTb = getCompomentObjectManager()->findTable(TABLENAME_EquipEnhance);
	assert(pEquipEnhanceTb);

	int iRecord = pEquipEnhanceTb->findRecord(iCurEquipLevel);
	assert(iRecord >= 0);

	int iBaseCostSilver = pEquipEnhanceTb->getInt(iRecord, "强化消化");
	int iCostParam = pEquip->getProperty(PROP_EQUIP_ENHANCE_COSTPARAM, 0);

	int iCostSilver = (double)iBaseCostSilver * (double)iCostParam/10000;
	scMsg.set_icostsilver(iCostSilver);

	IEntity* pMaster = getEntityFromHandle(m_hEntity);
	assert(pMaster);

	pMaster->sendMessage(GSProto::CMD_EQUIP_QUERYENHANCE, scMsg);
}


bool EquipBagSystem::checkEquuipLevelLimit(int iLevel, bool bNotify)
{
	IEntity* pMaster = getEntityFromHandle(m_hEntity);
	assert(pMaster);

	int iMasterLevel = pMaster->getProperty(PROP_ENTITY_LEVEL, 0);
	
	// 验证等级上限
	if(iLevel > iMasterLevel*2)
	{
		if(bNotify) pMaster->sendErrorCode(ERROR_EQUIP_MASTERLV_LIMIT);
		return false;
	}

	// 验证是否达到装备等级上限了
	ITable* pEquipEnhanceTb = getCompomentObjectManager()->findTable(TABLENAME_EquipEnhance);
	assert(pEquipEnhanceTb);

	int iNextRecord = pEquipEnhanceTb->findRecord(iLevel);
	if(iNextRecord < 0)
	{
		if(bNotify) pMaster->sendErrorCode(ERROR_EQUIP_MAXLEVEL);
		return false;
	}

	return true;
}

void EquipBagSystem::onReqEquipEnhance(const GSProto::CSMessage& msg)
{
	GSProto::CMD_EQUIP_ENHANCE_CS req;
	if(!req.ParseFromString(msg.strmsgbody() ) )
	{
		return;
	}

	Uint32 dwObjectID = req.dwheroobjectid();
	int iPos = req.iequippos();

	IEntity* pEntity = getEntityFromHandle(dwObjectID);
	if(!pEntity) return;

	int iClassID = pEntity->getProperty(PROP_ENTITY_CLASS, 0);
	
	if( ( (GSProto::en_class_Hero == iClassID) && ( (Uint32)pEntity->getProperty(PROP_ENTITY_MASTER, 0) != m_hEntity) ) || ( (GSProto::en_class_Actor == iClassID) && (dwObjectID != m_hEntity) ) )
	{
		return;
	}
	

	IEquipContainer* pEquipContainer = getEquipContainer(dwObjectID);
	if(!pEquipContainer) return;

	HItem hEquip = pEquipContainer->getEquip(iPos);
	
	IItem* pEquip = getItemFromHandle(hEquip);
	if(!pEquip) return;

	IEntity* pMaster = getEntityFromHandle(m_hEntity);
	assert(pMaster);

	GSProto::CMD_EQUIP_ENHANCE_SC scMsg;

	int iCurEquipLevel = pEquip->getProperty(PROP_EQUIP_LEVEL, 0);
	if(!checkEquuipLevelLimit(iCurEquipLevel + 1, true) )
	{
		scMsg.set_bsuccess(false);
		pMaster->sendMessage(GSProto::CMD_EQUIP_ENHANCE, scMsg);
		return;
	}

	// 验证金钱是否足够
	ITable* pEquipEnhanceTb = getCompomentObjectManager()->findTable(TABLENAME_EquipEnhance);
	assert(pEquipEnhanceTb);

	int iRecord = pEquipEnhanceTb->findRecord(iCurEquipLevel);
	assert(iRecord >= 0);

	int iBaseCostSilver = pEquipEnhanceTb->getInt(iRecord, "强化消化");
	int iCostParam = pEquip->getProperty(PROP_EQUIP_ENHANCE_COSTPARAM, 0);
	int iCostSilver = (double)iBaseCostSilver * (double)iCostParam/10000;

	

	int iTkSilver = pMaster->getProperty(PROP_ACTOR_SILVER, 0);
	if(iTkSilver < iCostSilver)
	{
		pMaster->sendErrorCode(ERROR_NEED_SILVER);
		return;
	}

	
	int iAddEquipLevel = randEquipLevelUp();

	// 扣钱
	pMaster->changeProperty(PROP_ACTOR_SILVER, 0-iCostSilver, GSProto::en_Reason_EquipLevelUp);

	// 升级，如果达到最大等级，限制下
	int iResultEquipLevel = iCurEquipLevel+iAddEquipLevel;
	if(pEquipEnhanceTb->findRecord(iResultEquipLevel) < 0)
	{
		iResultEquipLevel = pEquipEnhanceTb->getInt(pEquipEnhanceTb->getRecordCount() - 1, "等级");
	}
	
	pEquip->setProperty(PROP_EQUIP_LEVEL, iResultEquipLevel);

	// 累计消耗金钱数目增加
	int iCurSumMoney = pEquip->getProperty(PROP_EQUIP_ENHANCEMONEY, 0);
	iCurSumMoney += iCostSilver;
	pEquip->setProperty(PROP_EQUIP_ENHANCEMONEY, iCurSumMoney);
	
	// 通知客户端
	scMsg.set_bsuccess(true);
	scMsg.set_iaddlevel(iAddEquipLevel);
	
	pMaster->sendMessage(GSProto::CMD_EQUIP_ENHANCE, scMsg);
}


int EquipBagSystem::randEquipLevelUp()
{
	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	string strEquipLevelChanceCfg = pGlobalCfg->getString("装备升级暴击", "5000#3000#2000");
	vector<int> chanceList = TC_Common::sepstr<int>(strEquipLevelChanceCfg, Effect_MagicSep);

	int iSumChance = 0;

	IRandom* pRamdom = getComponent<IRandom>(COMPNAME_Random, IID_IMiniAprRandom);
	assert(pRamdom);

	int iRandV = pRamdom->random() % 10000;

	for(size_t i = 0; i < chanceList.size(); i++)
	{
		iSumChance += chanceList[i];
		if(iRandV < iSumChance)
		{
			return (int)i+1;
		}
	}

	return 1;
}


void EquipBagSystem::onReqEquipChipSell(const GSProto::CSMessage& msg)
{
	GSProto::CMD_EQUIP_CHIPSELL_CS req;
	if(!req.ParseFromString(msg.strmsgbody() ) )
	{
		return;
	}

	int iItemID = req.iequipcipid();
	int iSellCount = req.icount();

	IItemFactory* pItemFactory = getComponent<IItemFactory>(COMPNAME_ItemFactory, IID_IItemFactory);
	assert(pItemFactory);

	const PropertySet* pItemPropset = pItemFactory->getItemPropset(iItemID);
	if(!pItemPropset) return;

	int iItemType = pItemPropset->getInt(PROP_ITEM_TYPE, 0);
	if(iItemType != GSProto::en_ItemType_Material)
	{
		return;
	}

	IEntity* pMaster = getEntityFromHandle(m_hEntity);
	assert(pMaster);

	IItemSystem* pItemSystem = static_cast<IItemSystem*>(pMaster->querySubsystem(IID_IItemSystem));
	assert(pItemSystem);

	int iOwnCount = pItemSystem->getItemCount(iItemID);

	if(iOwnCount < iSellCount)
	{
		return;
	}

	int iSellSilver = pItemPropset->getInt(PROP_ITEM_PRICE, 0);
	int iTotalGet = iSellSilver * iSellCount;

	if(!pItemSystem->removeItem(iItemID, iSellCount, GSProto::en_Reason_SellEquipChip) )
	{
		return;
	}

	pMaster->changeProperty(PROP_ACTOR_SILVER, iTotalGet, GSProto::en_Reason_SellEquipChip);

	pMaster->sendMessage(GSProto::CMD_EQUIP_CHIPSELL);
}


void EquipBagSystem::onReqChipCombine(const GSProto::CSMessage& msg)
{
	GSProto::CMD_EQUIP_CHIPCOMBINE_CS req;
	if(!req.ParseFromString(msg.strmsgbody() ) )
	{
		return;
	}

	int iItemID = req.iequipchipid();

	ItemFactory* pItemFactory = static_cast<ItemFactory*>(getComponent<IItemFactory>(COMPNAME_ItemFactory, IID_IItemFactory) );
	assert(pItemFactory);

	const EquipChipCombine* pCombineCfg = pItemFactory->queryEquipChipCombine(iItemID);
	if(!pCombineCfg) return;

	IEntity* pMaster = getEntityFromHandle(m_hEntity);
	assert(pMaster);

	IItemSystem* pItemSystem = static_cast<IItemSystem*>(pMaster->querySubsystem(IID_IItemSystem));
	assert(pItemSystem);

	int iOwnCount = pItemSystem->getItemCount(iItemID);
	if(iOwnCount < pCombineCfg->iNeedCount)
	{
		return;
	}

	//验证银币
	int iTkSilver = pMaster->getProperty(PROP_ACTOR_SILVER, 0);
	if(iTkSilver < pCombineCfg->iBaseCostSilver)
	{
		pMaster->sendErrorCode(ERROR_NEED_SILVER);
		return;
	}

	// 验证装备背包是否满了
	if(getFreeSize() == 0)
	{
		pMaster->sendErrorCode(ERROR_EQUIPBAG_FULL);
		return;
	}

	// 扣钱
	pMaster->changeProperty(PROP_ACTOR_SILVER, 0-pCombineCfg->iBaseCostSilver, GSProto::en_Reason_EquipChipCombine);

	//扣除物品
	if(!pItemSystem->removeItem(iItemID, pCombineCfg->iNeedCount, GSProto::en_Reason_EquipChipCombine))
	{
		return;
	}

	// 添加装备
	addEquip(pCombineCfg->iCreateEquipID, GSProto::en_Reason_EquipChipCombine, true);

	pMaster->sendMessage(GSProto::CMD_EQUIP_CHIPCOMBINE);
}


void EquipBagSystem::onReqEquipSource(const GSProto::CSMessage& msg)
{
	GSProto::CMD_QUERY_EQUIPSOURCE_CS reqMsg;
	if(!reqMsg.ParseFromString(msg.strmsgbody() ) )
	{
		return;
	}

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	int iEquipID = reqMsg.iequipid();

	IItemFactory* pItemFactory = getComponent<IItemFactory>(COMPNAME_ItemFactory, IID_IItemFactory);
	assert(pItemFactory);

	const PropertySet* pEquipPropset = pItemFactory->getItemPropset(iEquipID);
	if(!pEquipPropset) return;

	string strSource = pEquipPropset->getString(PROP_EQUIP_DROPSCENE, "");

	vector<int> sceneList = TC_Common::sepstr<int>(strSource, Effect_MagicSep);

	GSProto::CMD_QUERY_EQUIPSOURCE_SC scMsg;
	scMsg.set_iequipid(iEquipID);

	IDungeonFactory* pDungeonFactory = getComponent<IDungeonFactory>(COMPNAME_DungeonFactory, IID_IDungeonFactory);
	assert(pDungeonFactory);

	IDungeonSystem* pDungeonSystem = static_cast<IDungeonSystem*>(pEntity->querySubsystem(IID_IDungeonSystem) );
	assert(pDungeonSystem);

	for(size_t i = 0; i < sceneList.size(); i++)
	{
		int iTmpSceneID = sceneList[i];
		const DungeonScene* pDungeonScene = pDungeonFactory->queryScene(iTmpSceneID);
		if(!pDungeonScene) continue;
	
		GSProto::EquipSource* pNewSource = scMsg.add_szequipsource();
		pNewSource->set_isectionid(pDungeonScene->iOwnSectionID);
		pNewSource->set_isceneid(iTmpSceneID);

		int iNoUseStar = 0;
		bool bOpened = pDungeonSystem->getSceneStar(iTmpSceneID, iNoUseStar);
		pNewSource->set_bopened(bOpened);
	}

	string strSpecialSource = pEquipPropset->getString(PROP_EQUIP_SPECOAL_DROPSOURCE, "");
	if(strSpecialSource.size() > 0)
	{
		scMsg.set_strspecialsource(strSpecialSource);
	}

	pEntity->sendMessage(GSProto::CMD_QUERY_EQUIPSOURCE, scMsg);
}


void EquipBagSystem::onReqQueryEquipByID(const GSProto::CSMessage& msg)
{
	GSProto::CMD_QUERY_EQUIPDESC_BYID_CS reqMsg;
	if(!reqMsg.ParseFromString(msg.strmsgbody() ) )
	{
		return;
	}

	int iEquipID = reqMsg.iequipid();

	IItemFactory* pItemFactory = getComponent<IItemFactory>(COMPNAME_ItemFactory, IID_IItemFactory);
	assert(pItemFactory);

	const PropertySet* pPropset = pItemFactory->getItemPropset(iEquipID);
	if(!pPropset) return;

	int iItemType = pPropset->getInt(PROP_ITEM_TYPE, 0);
	if(iItemType != GSProto::en_ItemType_Equip)
	{
		return;
	}

	vector<HItem> itemList;
	if(!pItemFactory->createItemFromID(iEquipID, 1, itemList) )
	{
		return;
	}

	assert(itemList.size() == 1);
	
	GSProto::CMD_QUERY_EQUIPDESC_BYID_SC scMsg;
	IItem* pEquip = getItemFromHandle(itemList[0]);
	assert(pEquip);

	pEquip->packScEquipInfo(*scMsg.mutable_equipdata(), 0);
	delete pEquip;

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	pEntity->sendMessage(GSProto::CMD_QUERY_EQUIPDESC_BYID, scMsg);
}


void EquipBagSystem::onReqSellByQuality(const GSProto::CSMessage& msg)
{
	GSProto::CMD_EQUIP_SELLBYQUALITY_CS reqMsg;
	if(!reqMsg.ParseFromString(msg.strmsgbody() ) )
	{
		return;
	}

	bool bSellGreen = reqMsg.bsellgreen();
	bool bSellBlue = reqMsg.bsellblue();
	bool bSellPurple = reqMsg.bsellpurple();

	int iSumSilver = 0;
	int iSumGold = 0;
	vector<int> posList;

	stringstream ss;
	for(size_t i = 0; i < m_equipList.size(); i++)
	{
		IItem* pEquip = getItemFromHandle(m_equipList[i]);
		if(!pEquip) continue;

		int iQuality = pEquip->getProperty(PROP_ITEM_QUALITY, 0);


		bool bNeedSell  = false;

		if(bSellGreen && iQuality == GSProto::en_Quality_Green)
		{
			bNeedSell = true;
		}
		
		else if(bSellBlue && iQuality == GSProto::en_Quality_Blue)
		{
			bNeedSell = true;
		}
		
		else if(bSellPurple && iQuality == GSProto::en_Quality_Pure)
		{
			bNeedSell = true;
		}
		
		if(!bNeedSell)
		{
			continue;
		}

		int iTmpItemBaseID = pEquip->getProperty(PROP_ITEM_BASEID, 0);
		ss<<iTmpItemBaseID<<"|";

		int iSellType = pEquip->getProperty(PROP_EQUIP_SELLTYPE, 0);
		int iSellValue = pEquip->getProperty(PROP_EQUIP_SELLPRICE, 0);

		// 强化消耗要返还
		iSumSilver += pEquip->getProperty(PROP_EQUIP_ENHANCEMONEY, 0);

		if(GSProto::en_EquipSellType_Silver == iSellType)
		{
			iSumSilver += iSellValue;
			setEquip(i, 0, NULL);
			delete pEquip;
			posList.push_back(i);
		}
		else if(GSProto::en_EquipSellType_Gold == iSellType)
		{
			iSumGold += iSellValue;
			setEquip(i, 0, NULL);
			delete pEquip;
			posList.push_back(i);
		}
		else
		{
			continue;
		}
	}

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	pEntity->changeProperty(PROP_ACTOR_SILVER, iSumSilver, GSProto::en_Reason_SellEquip);
	pEntity->changeProperty(PROP_ACTOR_GOLD, iSumGold, GSProto::en_Reason_SellEquip);

	sendContainerChg(posList);

	PLAYERLOG(pEntity)<<"SellEquipByType|"<<ss.str()<<endl;
}


void EquipBagSystem::onReqSellByPos(const GSProto::CSMessage& msg)
{
	GSProto::CMD_EQUIP_SELLBYPOS_CS reqMsg;
	if(!reqMsg.ParseFromString(msg.strmsgbody() ) )
	{
		return;
	}

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	int iEquipPos = reqMsg.iequippos();
	HItem hEquip = getEquip(iEquipPos);

	IItem* pEquip = getItemFromHandle(hEquip);
	if(!pEquip) return;

	int iEquipID = pEquip->getProperty(PROP_ITEM_BASEID, 0);

	int iSellType = pEquip->getProperty(PROP_EQUIP_SELLTYPE, 0);
	int iSellValue = pEquip->getProperty(PROP_EQUIP_SELLPRICE, 0);

	int iEnhanceSilver = pEquip->getProperty(PROP_EQUIP_ENHANCEMONEY, 0);
	pEntity->changeProperty(PROP_ACTOR_SILVER, iEnhanceSilver, GSProto::en_Reason_SellEquip);

	if(GSProto::en_EquipSellType_NULL == iSellType)
	{
		pEntity->sendErrorCode(ERROR_EQUIP_CANNOTSELL);
		return;
	}
	else if(GSProto::en_EquipSellType_Silver == iSellType)
	{
		pEntity->changeProperty(PROP_ACTOR_SILVER, iSellValue, GSProto::en_Reason_SellEquip);
		setEquip(iEquipPos, 0, NULL);
		delete pEquip;
	}
	else if(GSProto::en_EquipSellType_Gold == iSellType)
	{
		pEntity->changeProperty(PROP_ACTOR_GOLD, iSellValue, GSProto::en_Reason_SellEquip);
		setEquip(iEquipPos, 0, NULL);
		delete pEquip;
	}
	else
	{
		return;
	}
	vector<int> posList;
		posList.push_back(iEquipPos);
		sendContainerChg(posList);
	
		PLAYERLOG(pEntity)<<"SellEquipByPos|"<<iEquipID<<endl;
	}

	


void EquipBagSystem::onReqMoveEquip(const GSProto::CSMessage& msg)
{
	GSProto::CMD_EQUIP_MOVE_CS reqMsg;
	if(!reqMsg.ParseFromString(msg.strmsgbody() ) )
	{
		return;
	}

	HEntity hSrcEntity = reqMsg.dwsrcobjectid();
	HEntity hDstEntity = reqMsg.dwdstobjectid();

	int iSrcPos = reqMsg.isrcpos();
	int iDstPos = reqMsg.idstpos();

	if(!checkMoveValid(hSrcEntity, hDstEntity, iSrcPos, iDstPos) )
	{
		return;
	}

	IEquipContainer* pSrcContainer = getEquipContainer(hSrcEntity);
	IEquipContainer* pDstContainer = getEquipContainer(hDstEntity);
	
	assert(pSrcContainer && pDstContainer);
	
	Uint32 dwSrcEquip = pSrcContainer->getEquip(iSrcPos);
	Uint32 dwDstEquip = pDstContainer->getEquip(iDstPos);

	if(!pSrcContainer->canSetEquip(iSrcPos, dwDstEquip, true) || !pDstContainer->canSetEquip(iDstPos, dwSrcEquip, true) )
	{
		return;
	}

	pSrcContainer->setEquip(iSrcPos, dwDstEquip, NULL);
	pDstContainer->setEquip(iDstPos, dwSrcEquip, NULL);

	if(pSrcContainer == pDstContainer)
	{
		vector<int> posList;
		posList.push_back(iSrcPos);
		posList.push_back(iDstPos);
		pSrcContainer->sendContainerChg(posList);
	}
	else
	{
		
		{
			vector<int> posList;
			posList.push_back(iDstPos);
			pDstContainer->sendContainerChg(posList);
		}


		{
			vector<int> posList;
			posList.push_back(iSrcPos);
			pSrcContainer->sendContainerChg(posList);
		}
	}
}


bool EquipBagSystem::checkMoveValid(HEntity hSrcEntity, HEntity hDstEntity, int iSrcPos, int& iDstPos)
{
	if(!checkEntityValid(hSrcEntity) )
	{
		return false;
	}

	if(!checkEntityValid(hDstEntity) )
	{
		return false;
	}

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	IEntity* pSrcEntity = getEntityFromHandle(hSrcEntity);
	assert(pSrcEntity);

	IEntity* pDstEntity = getEntityFromHandle(hDstEntity);
	assert(pDstEntity);

	int iSrcClass = pSrcEntity->getProperty(PROP_ENTITY_CLASS, 0);
	int iDstClass = pDstEntity->getProperty(PROP_ENTITY_CLASS, 0);

	// 禁止从英雄拖到英雄
	if( (iSrcClass == iDstClass) && (iSrcClass == GSProto::en_class_Hero) )
	{
		return false;
	}

	IEquipContainer* pSrcContainer = getEquipContainer(hSrcEntity);
	IEquipContainer* pDstContainer = getEquipContainer(hDstEntity);

	if(!pSrcContainer || !pDstContainer)
	{
		return false;
	}

	// 验证位置合法
	if( (iSrcPos < 0) || (iSrcPos >= pSrcContainer->getSize() ) || (iDstPos >= pDstContainer->getSize() ) )
	{
		return false;
	}

	// 验证Src装备合法性
	HItem hSrcEquip = pSrcContainer->getEquip(iSrcPos);
	IItem* pSrcEquip = getItemFromHandle(hSrcEquip);
	if(!pSrcEquip) return false;

	int iItemType = pSrcEquip->getProperty(PROP_ITEM_TYPE, 0);
	if(iItemType != GSProto::en_ItemType_Equip)
	{
		return false;
	}

	if(iDstPos < 0)
	{
		if(iDstClass == GSProto::en_class_Actor)
		{
			iDstPos = getFirstFreePos();
			if(iDstPos < 0)
			{
				pEntity->sendErrorCode(ERROR_EQUIPBAG_FULL);
				return false;
			}
		}
		else if(iDstClass == GSProto::en_class_Hero)
		{
			iDstPos = pSrcEquip->getProperty(PROP_EQUIP_POS, 0);
			if( (iDstPos >= pDstContainer->getSize() ) )
			{
				return false;
			}
		}
	}

	return true;
}


int EquipBagSystem::getFirstFreePos()
{
	for(size_t i = 0; i < m_equipList.size(); i++)
	{
		if(0 == m_equipList[i])
		{
			return (int)i;
		}
	}

	return -1;
}


bool EquipBagSystem::checkEntityValid(HEntity hEntity)
{
	if(hEntity == m_hEntity) return true;

	IEntity* pHero = getEntityFromHandle(hEntity);
	if(!pHero) return false;

	HEntity hMaster = pHero->getProperty(PROP_ENTITY_MASTER, 0);
	if(hMaster != m_hEntity)
	{
		return false;
	}

	return true;
}


IEquipContainer* EquipBagSystem::getEquipContainer(HEntity hEntity)
{
	IEntity* pEntity = getEntityFromHandle(hEntity);
	if(!pEntity) return NULL;

	int iClassID = pEntity->getProperty(PROP_ENTITY_CLASS, 0);
	if(GSProto::en_class_Actor == iClassID)
	{
		return static_cast<IEquipContainer*>(pEntity->querySubsystem(IID_IEquipBagSystem)->queryInterface(IID_IEquipContainer) );
	}
	else if(GSProto::en_class_Hero == iClassID)
	{
		return static_cast<IEquipContainer*>(pEntity->querySubsystem(IID_IHeroRealEquupSystem)->queryInterface(IID_IEquipContainer) );
	}

	return NULL;
}


void EquipBagSystem::onReqQueryHeroEquip(const GSProto::CSMessage& msg)
{
	GSProto::CMD_QUERY_HEROEQUIP_CS reqMsg;
	if(!reqMsg.ParseFromString(msg.strmsgbody() ) )
	{
		return;
	}

	HEntity hHero = reqMsg.dwheroobjectid();
	IEntity* pHero = getEntityFromHandle(hHero);
	if(!pHero) return;

	HEntity hMaster = pHero->getProperty(PROP_ENTITY_MASTER, 0);
	if(hMaster != m_hEntity)
	{
		return;
	}

	IEntitySubsystem* pHeroRealEquipSys = pHero->querySubsystem(IID_IHeroRealEquupSystem);
	assert(pHeroRealEquipSys);

	IEquipContainer* pEquipContainer = static_cast<IEquipContainer*>(pHeroRealEquipSys->queryInterface(IID_IEquipContainer) );
	assert(pEquipContainer);

	GSProto::CMD_QUERY_HEROEQUIP_SC scMsg;
	scMsg.set_dwheroobjectid(hHero);

	for(int i = 0; i < pEquipContainer->getSize(); i++)
	{
		IItem* pTmpEquip = getItemFromHandle(pEquipContainer->getEquip(i) );
		if(!pTmpEquip) continue;

		GSProto::EquipInfo* pNewEquipInfo = scMsg.add_szequiplist();
		pTmpEquip->packScEquipInfo(*pNewEquipInfo, i);
	}

	(static_cast<HeroRealEquipSystem*>(pHeroRealEquipSys))->fillSuitState(scMsg.mutable_szsuitstatelist() );

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	pEntity->sendMessage(GSProto::CMD_QUERY_HEROEQUIP, scMsg);
}



// ronglian, xilian related function
#define __LOG_EQUIP_RONGLIAN_VERBOSE	FDLOG("EquipRonglianVerbose")
#define __LOG_EQUIP_XILIAN_VERBOSE		FDLOG("EquipXilianVerbose")


typedef struct
{
	int lv;
	int equipType;
	vector<int>	blueIds;
	vector<int>	pureIds;
	vector<int>	goldIds;

	vector<int>* getIds(int _equipQuality){
		switch(_equipQuality)
		{
			case GSProto::en_Quality_Blue:	return &blueIds;
			case GSProto::en_Quality_Pure:	return &pureIds;
			case GSProto::en_Quality_Gold:	return &goldIds;
			default:
				assert(false);
				return NULL;
		}
	}
	
}EquipRonglianGenRule;

typedef struct
{
	int value;
	int costRatio;
}EquipRonglianLvRule;
typedef map<int, EquipRonglianLvRule> EquipRonglianLvMap;

typedef struct EquipRonglianQualityRule
{
	int value;
	int costRatio;
	EquipRonglianQualityRule():
	value(0),
	costRatio(0)
	{
	}
	
}EquipRonglianQualityRule;
typedef vector<EquipRonglianQualityRule> EquipRonglianQualityVector;


bool EquipBagSystem::getEquipRonglianLvRule(	int _equipLv,
														int* _outValue,
														int* _outCostRatio)
{
	// expand data
	static EquipRonglianLvMap s_equipRonglianLvKeyMap;
	if(s_equipRonglianLvKeyMap.empty()){
		ITable* table = getCompomentObjectManager()->findTable(TABLENAME_EquipRonglianLvKey);
		assert(table);
		vector<int> lvTemp;
		EquipRonglianLvRule ruleTemp;
		Int32 recordCount = table->getRecordCount();
		for(Int32 i = 0; i < recordCount; i ++){
			string lvString = table->getString(i, "等级", "");
			int value = table->getInt(i, "参数", 0);
			int costRatio = table->getInt(i, "价格系数", 0);

			lvTemp.clear();
			lvTemp = TC_Common::sepstr<int>(lvString, "#");
			assert(lvTemp.size() == 2);
			for(int lv = lvTemp[0]; lv <= lvTemp[1]; lv ++){
				ruleTemp.value = value;
				ruleTemp.costRatio = costRatio;
				s_equipRonglianLvKeyMap[lv] = ruleTemp;
			}
		}
	}

	// return value
	assert(!s_equipRonglianLvKeyMap.empty());
	EquipRonglianLvMap::iterator it = s_equipRonglianLvKeyMap.find(_equipLv);
	if(it != s_equipRonglianLvKeyMap.end()){
		const EquipRonglianLvRule& rule = it->second;
		if(_outValue != NULL)		* _outValue = rule.value;
		if(_outCostRatio != NULL)	* _outCostRatio = rule.costRatio;
		return true;
	}else{
		__LOG_EQUIP_RONGLIAN_VERBOSE<<"error:can't ind ronglian lv key with lv:"<<_equipLv<<endl;
		return false;
	}
}


const EquipRonglianQualityRule c_emptyRonglianQualityRule = EquipRonglianQualityRule();
bool EquipBagSystem::getEquipRonglianQualityRule(	int 	_quality,
																int*	_outValue,
																int*	_outCostRatio)
{
	// expand data
	static EquipRonglianQualityVector s_equipRonglianQualityMap;
	if(s_equipRonglianQualityMap.empty()){
		s_equipRonglianQualityMap.assign(GSProto::en_Quality_End - 1, c_emptyRonglianQualityRule);
		ITable* table = getCompomentObjectManager()->findTable(TABLENAME_EquipRonglianQuality);
		assert(table);
		Int32 recordCount = table->getRecordCount();
		assert(recordCount == GSProto::en_Quality_End - 1);
		EquipRonglianQualityRule ruleTemp;
		for(Int32 i = 0; i < recordCount; i ++){
			int quality = table->getInt(i, "品质", 0);
			int value 	= table->getInt(i, "参数", 0);
			int costRatio = table->getInt(i, "价格系数", 0);
			ruleTemp.value = value;
			ruleTemp.costRatio = costRatio;
			s_equipRonglianQualityMap[quality - 1] = ruleTemp;
		}
	}

	// get data
	int index = _quality - 1;
	if(index < 0 || index >= (int)s_equipRonglianQualityMap.size()){
		return false;
	}
	if(_outValue != NULL) 		* _outValue = s_equipRonglianQualityMap[_quality - 1].value;
	if(_outCostRatio != NULL)	* _outCostRatio = s_equipRonglianQualityMap[_quality - 1].costRatio;
	return true;
}



#define __CHANCE_UNIT 10000

bool EquipBagSystem::getEquipRonglianQualityChance(	int _b,
																vector<int>& _outChanceVec)
{
	// get value
	int blueK = 0;
	int pureK = 0;
	int goldK = 0;
	this->getEquipRonglianQualityRule(GSProto::en_Quality_Blue, &blueK);
	this->getEquipRonglianQualityRule(GSProto::en_Quality_Pure, &pureK);
	this->getEquipRonglianQualityRule(GSProto::en_Quality_Gold, &goldK);

	// caculate chance vec
	_outChanceVec.assign(GSProto::en_Quality_End - 1, 0);
	if(_b < blueK){
		_outChanceVec[GSProto::en_Quality_Blue - 1] = __CHANCE_UNIT;
	}else if(_b >= blueK && _b < pureK){
		_outChanceVec[GSProto::en_Quality_Blue - 1] = (1.0F - (float)_b / (float) pureK) * __CHANCE_UNIT;
		_outChanceVec[GSProto::en_Quality_Pure - 1] = __CHANCE_UNIT - _outChanceVec[GSProto::en_Quality_Blue - 1];
	}else if(_b >= pureK && _b < goldK){
		_outChanceVec[GSProto::en_Quality_Pure - 1] = (1.0F - (float)_b / (float) goldK) * __CHANCE_UNIT;
		_outChanceVec[GSProto::en_Quality_Gold - 1] = __CHANCE_UNIT - _outChanceVec[GSProto::en_Quality_Pure - 1];
	}else{
		_outChanceVec[GSProto::en_Quality_Gold - 1] = __CHANCE_UNIT;
	}
	return true;
}



static string __equipRonglianGenRuleToString(const EquipRonglianGenRule& _rule)
{
	string temp;
	temp += "lv:";
	temp += TC_Common::tostr(_rule.lv);
	temp += " equipType:";
	temp += TC_Common::tostr(_rule.equipType);

	temp += " blueIds:";
	for(size_t i = 0; i < _rule.blueIds.size(); i ++){
		temp += TC_Common::tostr(_rule.blueIds[i]);
		temp += "#";
	}

	for(size_t i = 0; i < _rule.pureIds.size(); i ++){
		temp += TC_Common::tostr(_rule.pureIds[i]);
		temp += "#";
	}

	for(size_t i = 0; i < _rule.goldIds.size(); i ++){
		temp += TC_Common::tostr(_rule.goldIds[i]);
		temp += "#";
	}
	return temp;
}

vector<int>* EquipBagSystem::getEquipRonglianIds(	int _lv,
													int _equipType,
													int _equipQuality)
{
	static std::map<string, EquipRonglianGenRule>	s_equipRonglianMap;
	// expand data
	if(s_equipRonglianMap.empty()){
		ITable* table = getCompomentObjectManager()->findTable(TABLENAME_EquipRonglianMap);
		assert(table);
		vector<int> temp;
		EquipRonglianGenRule tempRule;
		Int32 recordCount = table->getRecordCount();
		for(Int32 i = 0; i < recordCount; i ++){
			string key = table->getString(i, "等级部位", "");
			string blueIds = table->getString(i, "蓝色品质", "");
			string pureIds = table->getString(i, "紫色品质", "");
			string goldIds = table->getString(i, "橙色品质", "");

			temp.clear();
			temp = TC_Common::sepstr<int>(key, "#");
			assert(temp.size() == 2);

			tempRule.lv = temp[0];
			tempRule.equipType = temp[1];
			tempRule.blueIds = TC_Common::sepstr<int>(blueIds, "#");
			tempRule.pureIds = TC_Common::sepstr<int>(pureIds, "#");
			tempRule.goldIds = TC_Common::sepstr<int>(goldIds, "#");

			s_equipRonglianMap[key] = tempRule;
		}

		// debug
		__LOG_EQUIP_RONGLIAN_VERBOSE<<"expand ronglian map data."<<endl;
		map<string, EquipRonglianGenRule>::iterator it = s_equipRonglianMap.begin();
		while(it != s_equipRonglianMap.end()){
			const string& key = it->first;
			const EquipRonglianGenRule& rule = it->second;
			__LOG_EQUIP_RONGLIAN_VERBOSE<<"key:"<<key
				<< "rule:"
				<< __equipRonglianGenRuleToString(rule)<<endl;
			it ++;
		}
	}

	// check _equipQuality
	if(	_equipQuality != GSProto::en_Quality_Blue &&
		_equipQuality != GSProto::en_Quality_Pure &&
		_equipQuality != GSProto::en_Quality_Gold)
		{
			__LOG_EQUIP_RONGLIAN_VERBOSE<<"invalid _equipQuality:"<<_equipQuality<<endl;
			return NULL;
		}


	// try to find data.
	string key;
	int lv = _lv;
	while(lv > 0){
		key.clear();
		key += TC_Common::tostr(lv);
		key += "#";
		key += TC_Common::tostr(_equipType);

		map<string, EquipRonglianGenRule>::iterator it = s_equipRonglianMap.find(key);
		if(it != s_equipRonglianMap.end()){
			EquipRonglianGenRule& rule = it->second;
			return rule.getIds(_equipQuality);
		}else{
			lv --;
		}
	}
	return NULL;

}



static int __random(int _min, int _max)
{
	IRandom* random = getComponent<IRandom>(COMPNAME_Random, IID_IMiniAprRandom);
	assert(random);
	return (random->random() %(_max -_min +1)) +_min;
}

static string __itemArrayToString(const vector<IItem*>& _array)
{
	string temp;
	for(size_t i = 0; i < _array.size(); i ++){
		IItem* itemE = _array[i];
		int id = itemE->getProperty(PROP_ITEM_BASEID, 0);
		temp += TC_Common::tostr(id);
		if(i != _array.size() - 1){
			temp += "#";
		}
	}
	return temp;
}


void EquipBagSystem::onReqEquipRonglian(const GSProto::CSMessage& _msg)
{
	// parse message
	GSProto::CMD_EQUIP_RONGLIAN_CS request;
	if(!request.ParseFromString(_msg.strmsgbody() ) )
	{
		return;
	}

	// check size
	// must >=2 and < 6
	int posCount = request.equipposlist_size();
	if(posCount < 2 || posCount > 6){
		__LOG_EQUIP_RONGLIAN_VERBOSE<<"invalid pos count:"<<posCount<<endl;
		return;
	}

	// check and prepare some params
	int equipPos = -1;				// equip pos, like weapon, cloth, etc
	vector<int>		changePosArray;	// posArray
	vector<IItem*> 	itemArray;		// item entity array.
	int maxLv = 0;					// max level
	int totalScore = 0;				// total score of all equip
	int totalSilverCost = 0;
	for(int i = 0; i < posCount; i ++){
		int pos = request.equipposlist(i);
		HItem hEquip = getEquip(pos);
		if((Uint32)(hEquip) == 0){
			__LOG_EQUIP_RONGLIAN_VERBOSE<<"pos:"<<pos<<"has no equip."<<endl;
			return;
		}
		IItem* equipE = getItemFromHandle(hEquip);
		assert(equipE);

		// get some params
		int itemType = equipE->getProperty(PROP_ITEM_TYPE, 0);
		int tmpEquipPos = equipE->getProperty(PROP_EQUIP_POS, -1);
		int level = equipE->getProperty(PROP_ITEM_LVLLIMIT, 0);
		int score = equipE->getProperty(PROP_EQUIP_SCORE, 0);
		int quality = equipE->getProperty(PROP_ITEM_QUALITY, 0);

		// check item type
		if(itemType != GSProto::en_ItemType_Equip){
			__LOG_EQUIP_RONGLIAN_VERBOSE<<"invalid item type:"<<itemType<<" of pos:"<<pos<<endl;
			return;
		}

		// check equip pos
		if(equipPos == -1){
			equipPos = tmpEquipPos;
		}else{
			if(equipPos != tmpEquipPos){
				__LOG_EQUIP_RONGLIAN_VERBOSE<<"not in same equip pos:"<<endl;
				return;
			}
		}

		// update max level
		if(level > maxLv){
			maxLv = level;
		}

		// updte price
		int silverPrice = 0;
			{
				int k1 = 0;
				int k2 = 0;
				bool ret = this->getEquipRonglianLvRule(level, NULL, &k1);
				if(!ret){
					__LOG_EQUIP_RONGLIAN_VERBOSE<<"get lv rule of level:"<<level<<" failed."<<endl;
					return;
				}
				ret = this->getEquipRonglianQualityRule(quality, NULL, &k2);
				if(!ret){
					__LOG_EQUIP_RONGLIAN_VERBOSE<<"get quality rule of quality:"<<quality<<" failed."<<endl;
					return;
				}
				silverPrice = k1 * k2;
			}
		totalSilverCost += silverPrice;

		// update total score
		totalScore += score;
		itemArray.push_back(equipE);
		changePosArray.push_back(pos);
	}

	// check total silver
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	int hasSilver = pEntity->getProperty(PROP_ACTOR_SILVER, 0);
	if(totalSilverCost > hasSilver)
	{
		pEntity->sendErrorCode(ERROR_NEED_SILVER);
		return;
	}

	// get key
	int key = 0;
	this->getEquipRonglianLvRule(maxLv, &key);
	if(key <= 0){
		__LOG_EQUIP_RONGLIAN_VERBOSE<<	"invalid key:"<<key<<endl;
		return;
	}
	int b = (int)((float)totalScore / (float)key);

	// get rank chance vector
	vector<int> qualityChance;
	this->getEquipRonglianQualityChance(b, qualityChance);

	// caculate rank
	int quality = GSProto::en_Quality_Begin;
		{
			int random = __random(0, 9999);
			int temp = 0;
			for(int i = GSProto::en_Quality_Begin + 1; i < GSProto::en_Quality_End; i ++){
				int chance = qualityChance[i - 1];
				temp += chance;
				if(random < temp){
					quality = i;
					break;
				}
			}
			if(quality == GSProto::en_Quality_Begin){
				__LOG_EQUIP_RONGLIAN_VERBOSE<< "caculate quality failed." << endl;
				return;
			}
		}

	// get ids
	vector<int>* ids = this->getEquipRonglianIds(maxLv, equipPos, quality);
	if(ids == NULL){
		__LOG_EQUIP_RONGLIAN_VERBOSE	<< " match ids for maxLv:" << maxLv
										<< " equipPos:" << equipPos
										<< " quality:" << quality
										<< " failed." <<endl;
		return;
	}
	assert(!ids->empty());

	// dicide equip id.
	int equipId = 0;
		{
			int index = __random(0, ids->size() - 1);
			equipId = ids->operator[](index);
		}

	// 扣除银币
	pEntity->changeProperty(PROP_ACTOR_SILVER, - totalSilverCost, GSProto::en_Reason_Equip_Ronglian);

	// clear equip
	string debugDelItems = __itemArrayToString(itemArray);
	for(size_t i = 0; i <changePosArray.size(); i ++){
		int pos = changePosArray[i];
		IItem* equipE = itemArray[i];
		setEquip(pos, 0, NULL);
		delete equipE;
	}
	

	// generate equip
	int newPos = 0;
	HItem newHEquip = this->addEquip(equipId, GSProto::en_Reason_Equip_Ronglian, true, &newPos);
	IItem* newEquipE = getItemFromHandle(newHEquip);

	sendContainerChg(changePosArray);

	// send response
	GSProto::CMD_EQUIP_RONGLIAN_SC response;
	response.set_retcode(0);
	newEquipE->packScEquipInfo(*response.mutable_equipdata(), newPos);
	pEntity->sendMessage(GSProto::CMD_EQUIP_RONGLIAN, response);

	// debug
	/*
	__LOG_EQUIP_RONGLIAN_VERBOSE<<	"attemp to ronglian, equipPos:"	<<equipPos
								<<	" maxLv:" 			<< maxLv
								<<	" totalScore:"		<<totalScore
								<<	" key:"				<<key
								<<	" equipCount:"		<<itemArray.size()
								<<	" white_chance:"	<<qualityChance[GSProto::en_Quality_White - 1]
								<<	" green_chance:"	<<qualityChance[GSProto::en_Quality_Green - 1]
								<<	" blue_chance:"		<<qualityChance[GSProto::en_Quality_Blue - 1]
								<<	" pure_chance:"		<<qualityChance[GSProto::en_Quality_Pure - 1]
								<<	" gold_chance:"		<<qualityChance[GSProto::en_Quality_Gold - 1]
								<<	" red_chance:"		<<qualityChance[GSProto::en_Quality_Red - 1]
								<<	endl;
	*/
	PLAYERLOG(pEntity)	<< "EquipRonglian|"<< maxLv
						<< "|"	<< totalScore
						<< "|"	<< debugDelItems
						<< "|"	<< newEquipE->getProperty(PROP_ITEM_BASEID, 0)
						<< "|"	<< totalSilverCost
						<< endl;
}


#define __MAX_XILIAN_ATTRI_COUNT	4
#define VERBOSE(__playerE)	PLAYER_LOG(__playerE, "EquipXilianGen")


bool EquipBagSystem::__checkXilianFunctionOpen(bool _notifyErrorCode)
{
	IEntity* playerE = getEntityFromHandle(m_hEntity);
	int level = playerE->getProperty(PROP_ENTITY_LEVEL, 0);
	bool ret = false;
	if(level >= 40)
	{
		ret = true;
	}
	else
	{
		ret = false;
	}
	if(_notifyErrorCode && !ret)
	{
		static IJZEntityFactory* jzEntityFactory = NULL;
		if(jzEntityFactory == NULL)
		{
			jzEntityFactory = getComponent<IJZEntityFactory>(COMPNAME_EntityFactory, IID_IJZEntityFactory);
			assert(jzEntityFactory);
		}
		const FunctionOpenDesc* functionOpenDesc = jzEntityFactory->getFunctionData(GSProto::en_Function_EquipXilian);
		if(functionOpenDesc != NULL)
		{
			playerE->sendErrorCode(999);
		}
		else
		{
			DEBUG_LOG(playerE)<<"can't find function open desc with function id:"<<GSProto::en_Function_EquipXilian<<endl;
		}
	}
	return ret;

#if 0
	// 获取角色实体
	IEntity* playerE = getEntityFromHandle(m_hEntity);
	if(playerE->isFunctionOpen(GSProto::en_Function_EquipXilian))
	{
		return true;
	}
	else
	{
		if(_notifyErrorCode)
		{
			static IJZEntityFactory* jzEntityFactory = NULL;
			if(jzEntityFactory == NULL)
			{
				jzEntityFactory = getComponent<IJZEntityFactory>(COMPNAME_EntityFactory, IID_IJZEntityFactory);
				assert(jzEntityFactory);
			}
			const FunctionOpenDesc* functionOpenDesc = jzEntityFactory->getFunctionData(GSProto::en_Function_EquipXilian);
			if(functionOpenDesc != NULL)
			{
				playerE->sendErrorCode(functionOpenDesc->iErrorCode);
			}
			else
			{
				DEBUG_LOG(playerE)<<"can't find function open desc with function id:"<<GSProto::en_Function_EquipXilian<<endl;
			}
			
		}
		return false;
	}
#endif
}



void EquipBagSystem::onReqEquipXilianData(const GSProto::CSMessage& _msg)
{
	// 获取角色实体
	IEntity* playerE = getEntityFromHandle(m_hEntity);

	// 检查function open
	if(!this->__checkXilianFunctionOpen())
	{
		DEBUG_LOG(playerE)<<"function not open."<<endl;
		return;
	}
	
	// 返回消息
	GSProto::CMD_EQUIP_XILIAN_DATA_SC response;
	response.set_objid(this->m_xilianOBJId);
	for(size_t i = 0; i < this->m_xilianPropList.size(); i ++){
		const XilianPropItem& xilianProp = this->m_xilianPropList[i];
		GSProto::PropItem* xilianPropPB = response.add_proplist();
		int lifeAttId = Item::convertAddPropIdToLifeAttri(xilianProp.val1);
		if(lifeAttId != -1){
			xilianPropPB->set_ilifeattid(lifeAttId);
			xilianPropPB->set_ivalue(xilianProp.val2);
		}else{
			// 不正确的属性
			VERBOSE(playerE)<<"invalid prop id:"<<xilianProp.val1<<endl;
			continue;
		}
	}

	// 发回消息
	playerE->sendMessage(GSProto::CMD_EQUIP_XILIAN_DATA, response);
}






static XilianCfg s_xilianCfg;
void EquipBagSystem::__initXilianConfigData()
{
	if(s_xilianCfg.empty()){
		IEntityFactory* entityFactory = getComponent<IEntityFactory>(COMPNAME_EntityFactory, IID_IEntityFactory);
		assert(entityFactory);
		ITable* table = getCompomentObjectManager()->findTable(TABLENAME_EquipXilian);
		assert(table);
		Int32 recordCount = table->getRecordCount();
		for(Int32 i = 0; i < recordCount; i ++){
			int equipPos = table->getInt(i, "部位", 0);
			string propName = table->getString(i, "属性", "");
			int propChance = table->getInt(i, "属性概率", 0);
			int maxValue = table->getInt(i, "属性上限", 0);
			string strValueChance = table->getString(i, "概率", "");
			int propId = entityFactory->getPropDefine("EntityPropDef")->getPropKey(propName);
			assert(propId != -1);

			XilianCfg_EquipPos* cfgEquipPos = NULL;
			XilianCfg::iterator posIt = s_xilianCfg.find(equipPos);
			if(posIt != s_xilianCfg.end()){
				cfgEquipPos = posIt->second;
			}else{
				cfgEquipPos = new XilianCfg_EquipPos();
				cfgEquipPos->equipPos = equipPos;
				s_xilianCfg[equipPos] = cfgEquipPos;
			}

			XilianCfg_Prop* cfgProp = new XilianCfg_Prop();
			cfgProp->propId = propId;
			cfgProp->chance = propChance;
			cfgProp->maxValue = maxValue;
			

			vector<int> temp = TC_Common::sepstr<int>(strValueChance, "#");
			assert(temp.size() % 3 == 0);
			size_t valueCount = temp.size() / 3;
			for(size_t i = 0; i < valueCount; i ++){
				int min = temp[i * 3];
				int max = temp[i * 3 + 1];
				int chance = temp[i * 3 + 2];
				XilianCfg_ValueChance* cfgValueChance = new XilianCfg_ValueChance();
				cfgValueChance->min = min;
				cfgValueChance->max = max;
				cfgValueChance->chance = chance;
				cfgProp->valueChanceList.push_back(cfgValueChance);
			}
			cfgEquipPos->propMap[propId] = cfgProp;
		}
	}
}


XilianCfg_EquipPos* EquipBagSystem::getXilianCfgByEquipPos(int _equipPos)
{
	this->__initXilianConfigData();
	XilianCfg::iterator it = s_xilianCfg.find(_equipPos);
	if(it != s_xilianCfg.end()){
		return it->second;
	}else{
		return NULL;
	}
}





void EquipBagSystem::onReqEquipXilianGen(const GSProto::CSMessage& _msg)
{
	// 解析消息
	GSProto::CMD_EQUIP_XILIAN_GEN_CS req;
	if(!req.ParseFromString(_msg.strmsgbody() ) )
	{
		return;
	}
	
	// 获取角色实体
	IEntity* playerE = getEntityFromHandle(m_hEntity);

	// 检查function open
	if(!this->__checkXilianFunctionOpen())
	{
		DEBUG_LOG(playerE)<<"function not open."<<endl;
		return;
	}

	// 校验角色等级是否满足条件
	IItemFactory* itemFactory = getComponent<IItemFactory>(COMPNAME_ItemFactory, IID_IItemFactory);
	assert(itemFactory);
	int level = playerE->getProperty(PROP_ENTITY_LEVEL, 0);
	if(level < itemFactory->getXilianLvLimit())
	{
		playerE->sendErrorCode(ERROR_EQUIP_XILIAN_LV_LIMIT);
		return;
	}
	
	
	// 获取配置, 银币消耗,洗练锁id
	Uint32 silverCost = 0;
	Uint32 lockId = 0;
		{
			IGlobalCfg* globalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
			assert(globalCfg);
			silverCost = globalCfg->getInt("装备洗炼银币消耗", 10000);
			lockId = globalCfg->getInt("洗炼锁ID", 90002);
		}
	
	// 收集客户端发来的数据
	Uint32 cobjId = 0;
	int pos = 0;
	vector<bool> lockStatus;
	Uint32 needLockCount = 0;
		{
			lockStatus.assign(__MAX_XILIAN_ATTRI_COUNT, false);
			cobjId = req.cobjid();
			pos = req.pos();
			for(int i = 0; i < req.lockposlist_size(); i ++){
				int lockPos = req.lockposlist(i);
				if(lockPos >= __MAX_XILIAN_ATTRI_COUNT){
					// 不正确的锁定位置.
					VERBOSE(playerE)<<"invalid lock pos:"<<lockPos<<endl;
					continue;
				}else{
					lockStatus[lockPos] = true;
					needLockCount ++;
				}
			}
		}

	// 获取装备
	Uint32 equipH = 0;
	bool ret = this->queryEquip(cobjId, pos, equipH);
	if(!ret){
		VERBOSE(playerE)<<"[ERROR] can't find equip at "<<cobjId<<";"<<pos<<endl;
		return;
	}

	// 获取装备实体
	IItem* equipE = getItemFromHandle(equipH);
	if(!equipE){
		VERBOSE(playerE)<<"[ERROR] can't find equip e with id:"<<equipH<<endl;
		return;
	}

	// 获取相关参数
	int equipPos = equipE->getProperty(PROP_EQUIP_POS, 0);
	

	// 检查银币是否足够
		{
			Uint32 silverHas = playerE->getProperty(PROP_ACTOR_SILVER, 0);
			if(silverHas < silverCost){
				VERBOSE(playerE)<<"no enough silver"<<endl;
				return;
			}
		}

	// 检查洗练锁是否足够
	IItemSystem* itemSystem = static_cast<IItemSystem*>(playerE->querySubsystem(IID_IItemSystem));
	assert(itemSystem);
		{
			int lockHas = itemSystem->getItemCount(lockId);
			if((Uint32)lockHas < needLockCount){
				VERBOSE(playerE)<<"no enough lock, lock id:"<<lockId<<" lock has:"<<lockHas<<endl;
				return;
			}
		}

	// 扣除银币
	playerE->changeProperty(PROP_ACTOR_SILVER, - silverCost, GSProto::en_Reason_Equip_Xilian);

	// 扣除洗练锁
	ret = itemSystem->removeItem(lockId, needLockCount, GSProto::en_Reason_Equip_Xilian);
	if(!ret){
		VERBOSE(playerE)<<"update xilian lock count failed."<<endl;
	}

	// 准备新的洗练属性结果
	XilianPropVec newXilianPropVec;
	newXilianPropVec.assign(__MAX_XILIAN_ATTRI_COUNT, XilianPropItem(0, 0));

	// 获取装备洗练属性
	XilianPropVec& equipXilianPropVec = ((Item*)equipE)->getXilianProp();

	// 拷贝锁定属性
	set<int> alreadyHasProp;
		{
			size_t count = equipXilianPropVec.size();
			if(count > lockStatus.size()){
				count = lockStatus.size();
			}
			for(size_t i = 0; i < count; i ++){
				const XilianPropItem& item = equipXilianPropVec[i];
				if(lockStatus[i]){
					newXilianPropVec[i] = item;
					alreadyHasProp.insert(item.val1);
				}
			}
		}

	// 根据部位获取洗练配置
	XilianCfg_EquipPos* cfgEquipPos = this->getXilianCfgByEquipPos(equipPos);
	assert(cfgEquipPos);

	// 生成随机属性表
	vector<XilianCfg_Prop*>	randomVector;
		{
			map<int, XilianCfg_Prop*>::iterator it = cfgEquipPos->propMap.begin();
			while(it != cfgEquipPos->propMap.end()){
				XilianCfg_Prop* cfgProp = it->second;
				if(alreadyHasProp.count(cfgProp->propId) == 0){
					randomVector.push_back(cfgProp);
				}
				it ++;
			}
		}

	
	// 生成随机属性
		{
			for(size_t i = 0; i < newXilianPropVec.size(); i ++){
				XilianPropItem& item = newXilianPropVec[i];
				if(item.val1 == 0){
					assert(!randomVector.empty());
					Uint32 totalDice = 0;
					for(size_t j = 0; j < randomVector.size(); j ++){
						XilianCfg_Prop* cfgProp = randomVector[i];
						totalDice += cfgProp->chance;
					}

					// 掷骰
					Uint32 dice = __random(0, totalDice - 1);


					// 确定类型
					XilianCfg_Prop* targetCfgProp = NULL;
					Uint32 temp = 0;
					vector<XilianCfg_Prop*>::iterator it = randomVector.begin();
					while(it != randomVector.end()){
						XilianCfg_Prop* cfgProp = * it;
						temp += cfgProp->chance;
						if(dice < temp){
							targetCfgProp = cfgProp;
							randomVector.erase(it);
							break;
						}
						it ++;
					}
					assert(targetCfgProp);

					// 确定属性值
					Uint32 totalValDice = 0;
					for(size_t j = 0; j < targetCfgProp->valueChanceList.size(); j ++){
						const XilianCfg_ValueChance* cfgValueChance = targetCfgProp->valueChanceList[j];
						totalValDice += cfgValueChance->chance;
					}

					dice = __random(0, totalValDice - 1);
					temp = 0;
					XilianCfg_ValueChance* targetCfgValueChance = NULL;
					for(size_t j = 0; j < targetCfgProp->valueChanceList.size(); j ++){
						XilianCfg_ValueChance* cfgValueChance = targetCfgProp->valueChanceList[j];
						temp += cfgValueChance->chance;
						if(dice < temp){
							targetCfgValueChance = cfgValueChance;
							break;
						}
					}

					int value = __random(targetCfgValueChance->min, targetCfgValueChance->max);

					item.val1 = targetCfgProp->propId;
					item.val2 = value;
				}
			}
		}

	// 保存新属性数据
	this->m_xilianOBJId = equipH;
	this->m_xilianPropList = newXilianPropVec;

	// 发送response给客户端
	GSProto::CMD_EQUIP_XILIAN_GEN_SC response;
	response.set_retcode(0);
	response.set_objid(this->m_xilianOBJId);
	for(size_t i = 0; i < this->m_xilianPropList.size(); i ++){
		const XilianPropItem& item = this->m_xilianPropList[i];
		GSProto::PropItem* xilianPropPB = response.add_proplist();
		int lifeAttId = Item::convertAddPropIdToLifeAttri(item.val1);
		if(lifeAttId != -1){
			xilianPropPB->set_ilifeattid(lifeAttId);
			xilianPropPB->set_ivalue(item.val2);
		}else{
			// 不正确的属性
			VERBOSE(playerE)<<"invalid prop id:"<<item.val1<<endl;
			continue;
		}
	}
	playerE->sendMessage(GSProto::CMD_EQUIP_XILIAN_GEN, response);
	
	
	
	// DEBUG
		{
			stringstream debugSS;
			debugSS<<"objId:"<<equipH<<" needLockCount:"<<needLockCount;
			debugSS<<" equipPos:"<<equipPos;
			debugSS<<"lockStatus:";
			for(size_t i = 0; i < lockStatus.size(); i ++){
				bool lock = lockStatus[i];
				debugSS<<lock;
				if(i != lockStatus.size() - 1)
					debugSS<<",";
			}
			debugSS<<" silverCost:"<<silverCost;

			debugSS<<" xilian props:"<<endl;
			for(size_t i = 0; i < this->m_xilianPropList.size(); i ++){
				const XilianPropItem& item = this->m_xilianPropList[i];
				debugSS<<"["<<i<<"] propId:"<<item.val1<<" value:"<<item.val2<<endl;
			}
			VERBOSE(playerE)<<debugSS.str()<<endl;
		}
	
	
	
}

int EquipBagSystem::getPosFromHandle(HItem _itemH)
{
	for(size_t i = 0; i < this->m_equipList.size(); i ++){
		HItem tempH = this->m_equipList[i];
		if(tempH == _itemH){
			return i;
		}
	}
	return -1;
}


void EquipBagSystem::onReqEquipXilianReplace(const GSProto::CSMessage& _msg)
{
	// 解析消息
	GSProto::CMD_EQUIP_XILIAN_REPLACE_CS req;
	if(!req.ParseFromString(_msg.strmsgbody() ) )
	{
		return;
	}
	
	// 获取角色实体
	IEntity* playerE = getEntityFromHandle(m_hEntity);

	// 检查function open
	if(!this->__checkXilianFunctionOpen())
	{
		DEBUG_LOG(playerE)<<"function not open."<<endl;
		return;
	}

	// 获取参数
	HEntity cobjId = req.cobjid();
	int pos = req.pos();

	// 获取装备
	Uint32 equipId = 0;
	IEquipContainer* container = NULL;
	bool ret = this->queryEquip(cobjId, pos, equipId, &container);
	if(!ret){
		VERBOSE(playerE)<<"[ERROR] can't find equip at"<<cobjId<<";"<<pos<<endl;
		return;
	}

	// 校验object id
	if(equipId != this->m_xilianOBJId)
	{
		VERBOSE(playerE)<<"[ERROR] dismatch obj id."<<endl;
		return;
	}

	// 获取对应实体
	Item* equipE = (Item*)getItemFromHandle((HItem)equipId);
	if(equipE == NULL)
	{
		VERBOSE(playerE)<<"[ERROR] can't find itemE with id:"<<equipId<<endl;
		return;
	}


	// 更新洗练属性&清空缓存洗练属性
	XilianPropVec& xilianPropVec = equipE->getXilianProp();
	xilianPropVec = this->m_xilianPropList;
	this->m_xilianOBJId = 0;
	this->m_xilianPropList.clear();

	// 更新对应的将领数据, 并获取pos
	int containerType = container->getContainerType();
	if(containerType == GSProto::en_EquipContainer_HeroWear){
		HEntity heroH = container->getOwner();
		equipE->changeProperty(heroH,false);
		equipE->changeProperty(heroH,true);
	}

	// 通知容器更新
	vector<int> posList;
	posList.push_back(pos);
	container->sendContainerChg(posList);
	

	// 下行数据到客户端
	GSProto::CMD_EQUIP_XILIAN_REPLACE_SC response;
	response.set_retcode(0);
	playerE->sendMessage(GSProto::CMD_EQUIP_XILIAN_REPLACE, response);

	// debug
	VERBOSE(playerE)<<"xilian replace to equip:"<<equipId<<endl;
	
}


void EquipBagSystem::onReqEquipXilianTransfer(const GSProto::CSMessage& _msg)
{
	// 解析消息
	GSProto::CMD_EQUIP_XILIAN_TRANSFER_CS req;
	if(!req.ParseFromString(_msg.strmsgbody() ) )
	{
		return;
	}

	// 获取参数
	HEntity srcCObjId = req.srccobjid();
	HEntity dstCObjId = req.dstcobjid();
	int srcPos = req.srcpos();
	int dstPos = req.dstpos();

	// 获取角色实体
	IEntity* playerE = getEntityFromHandle(m_hEntity);

	// 检查function open
	if(!this->__checkXilianFunctionOpen())
	{
		DEBUG_LOG(playerE)<<"function not open."<<endl;
		return;
	}

	// 获取相关实体
	Uint32 srcEquipId = 0;
	Uint32 dstEquipId = 0;
	IEquipContainer* srcContainer = NULL;
	IEquipContainer* dstContainer = NULL;
	bool ret = this->queryEquip(srcCObjId, srcPos, srcEquipId, &srcContainer);
	if(!ret){
		VERBOSE(playerE)<<"[ERROR] can't find src equip at"<<srcCObjId<<";"<<srcPos<<endl;
		return;
	}
	ret = this->queryEquip(dstCObjId, dstPos, dstEquipId, &dstContainer);
	if(!ret){
		VERBOSE(playerE)<<"[ERROR] can't find dst equip at"<<dstCObjId<<";"<<dstPos<<endl;
		return;
	}

	// 获取装备实体
	Item* srcEquipE = (Item*)getItemFromHandle(srcEquipId);
	Item* dstEquipE = (Item*)getItemFromHandle(dstEquipId);
	if(srcEquipE == NULL){
		VERBOSE(playerE)<<"[ERROR] can't find src equip e with objid:"<<srcEquipId<<endl;
		return;
	}
	if(dstEquipE == NULL){
		VERBOSE(playerE)<<"[ERROR] can't find dst equip e with objid:"<<dstEquipId<<endl;
	}

	// 一些基本的检查
	if(srcEquipE == dstEquipE){
		VERBOSE(playerE)<<"[ERROR] src == dst."<<endl;
		return;
	}
	XilianPropVec& srcPropVec = srcEquipE->getXilianProp();
	if(srcPropVec.empty()){
		playerE->sendErrorCode(ERROR_EQUIP_HASNO_XILIAN_PROP);
		return;
	}

	// 获取元宝消耗
	IGlobalCfg* globalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	int ybCost = globalCfg->getInt("洗炼转移元宝消耗", 400);
	int ybHas = playerE->getProperty(PROP_ACTOR_GOLD, 0);
	if(ybHas < ybCost){
		VERBOSE(playerE)<<"[ERROR] not enough yb."<<endl;
		playerE->sendErrorCode(ERROR_NEED_GOLD);
		return;
	}

	// 扣除
	playerE->changeProperty(PROP_ACTOR_GOLD, - ybCost, GSProto::en_Reason_Equip_XilianTransfer);

	// 进行转移
	XilianPropVec& dstPropVec = dstEquipE->getXilianProp();
	dstPropVec = srcPropVec;
	srcPropVec.clear();

	// 更新将领数据
	int containerType = srcContainer->getContainerType();
	if(containerType == GSProto::en_EquipContainer_HeroWear){
		HEntity heroH = srcContainer->getOwner();
		srcEquipE->changeProperty(heroH,false);
		srcEquipE->changeProperty(heroH,true);
	}
	containerType = dstContainer->getContainerType();
	if(containerType == GSProto::en_EquipContainer_HeroWear){
		HEntity heroH = dstContainer->getOwner();
		dstEquipE->changeProperty(heroH,false);
		dstEquipE->changeProperty(heroH,true);
	}
	

	// 进行容器改变通知
	vector<int> temp;
	temp.push_back(srcPos);
	srcContainer->sendContainerChg(temp);
	temp.clear();
	temp.push_back(dstPos);
	dstContainer->sendContainerChg(temp);


	// 响应客户端
	GSProto::CMD_EQUIP_XILIAN_TRANSFER_SC response;
	response.set_retcode(0);
	playerE->sendMessage(GSProto::CMD_EQUIP_XILIAN_TRANSFER, response);

	// debug
	VERBOSE(playerE)<<"onReqEquipXilianTransfer1 srcObjID:"<<srcEquipId<<" dstObjId:"<<dstEquipId<<endl;

}



void EquipBagSystem::onReqEquipXilianBuyLock(const GSProto::CSMessage& _msg)
{
	// 解析消息
	GSProto::CMD_EQUIP_XILIAN_BUYLOCK_CS req;
	if(!req.ParseFromString(_msg.strmsgbody() ) )
	{
		return;
	}
	Uint32 count = req.count();
	if(count == 0)
	{
		return;
	}

	// 获取角色实体
	IEntity* playerE = getEntityFromHandle(m_hEntity);

	// 检查function open
	if(!this->__checkXilianFunctionOpen())
	{
		DEBUG_LOG(playerE)<<"function not open."<<endl;
		return;
	}

	// 获取锁价格(元宝), 锁id
	IGlobalCfg* globalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(globalCfg);
	int lockId = globalCfg->getInt("洗炼锁ID", 90002);
	int lockPrice = globalCfg->getInt("洗练锁单价", 10);
	int ybCost = lockPrice * (int)count;
	
	
	// 检查玩家元宝数
	int ybHas = playerE->getProperty(PROP_ACTOR_GOLD, 0);
	if(ybHas < ybCost)
	{
		playerE->sendErrorCode(ERROR_NEED_GOLD);
		return;
	}

	// 扣除
	playerE->changeProperty(PROP_ACTOR_GOLD, - ybCost, GSProto::en_Reason_Equip_XilianBuyLock);

	// 增加元宝锁
	IItemSystem* itemSystem = static_cast<IItemSystem*>(playerE->querySubsystem(IID_IItemSystem));
	assert(itemSystem);
	itemSystem->addItem(lockId, count, GSProto::en_Reason_Equip_XilianBuyLock);

	// 返回response
	GSProto::CMD_EQUIP_XILIAN_BUYLOCK_SC response;
	response.set_retcode(0);
	playerE->sendMessage(GSProto::CMD_EQUIP_XILIAN_BUYLOCK, response);

	// 记录日志
	VERBOSE(playerE)<<" onReqEquipXilianBuyLock count:"	<< count
					<<" price:"							<< lockPrice
					<<" ybCost:"						<< ybCost;
}


#undef VERBOSE






void EquipBagSystem::packSaveData(string& data)
{
	ServerEngine::EquipBagSystemData saveData;
	for(size_t i = 0; i < m_equipList.size(); i++)
	{
		if(0 == m_equipList[i]) 
		{
			continue;
		}

		IItem* pEquip = getItemFromHandle(m_equipList[i]);
		if(!pEquip) continue;

		pEquip->packJce(saveData.equipList[i]);
	}

	data = ServerEngine::JceToStr(saveData);
}


int EquipBagSystem::addEquipByHandle(HItem hEquip, int iReason, bool bNotifyError)
{
	IItem* pEquip = getItemFromHandle(hEquip);
	if(!pEquip) return -1;

	int iItemID = pEquip->getProperty(PROP_ITEM_BASEID, 0);

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	for(size_t i = 0; i < m_equipList.size(); i++)
	{
		if(0 == m_equipList[i])
		{
			m_equipList[i] = hEquip;

			// 通知客户端
			vector<int> posList;
			posList.push_back(i);
			sendContainerChg(posList);

			PLAYERLOG(pEntity)<<"AddEquip|"<<iItemID<<"|"<<iReason<<endl;

			return (int)i;
		}
	}

	pEntity->sendErrorCode(ERROR_EQUIPBAG_FULL);

	return -1;
}


Uint32 EquipBagSystem::addEquip(	int iItemID, 
									int iReason, 
									bool bNotifyError,
									int* _outPos)
{
	IItemFactory* pItemFactory = getComponent<IItemFactory>(COMPNAME_ItemFactory, IID_IItemFactory);
	assert(pItemFactory);

	const PropertySet* pItemProp = pItemFactory->getItemPropset(iItemID);
	if(!pItemProp) return 0;

	int iItemType = pItemProp->getInt(PROP_ITEM_TYPE, 0);
	if(GSProto::en_ItemType_Equip != iItemType)
	{
		return 0;
	}

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	for(size_t i = 0; i < m_equipList.size(); i++)
	{
		if(0 == m_equipList[i])
		{
			vector<HItem> tmpItemList;
			bool bResult = pItemFactory->createItemFromID(iItemID, 1, tmpItemList);
			assert(bResult && (tmpItemList.size() == 1) );

			// 添加到装备背包
			m_equipList[i] = tmpItemList[0];

			if(_outPos != NULL){
				* _outPos = i;
			}

			// 通知客户端
			vector<int> posList;
			posList.push_back(i);
			sendContainerChg(posList);
	
			PLAYERLOG(pEntity)<<"AddEquip|"<<iItemID<<"|"<<iReason<<endl;

			return tmpItemList[0];
		}
	}

	pEntity->sendErrorCode(ERROR_EQUIPBAG_FULL);

	return 0;
}

int EquipBagSystem::getFreeSize()
{
	int iFreeSize = std::count_if(m_equipList.begin(), m_equipList.end(), std::bind1st(equal_to<int>(), 0) );

	return iFreeSize;
}

int EquipBagSystem::getSize()
{
	return GSProto::MAX_EQUIP_BAGSIZE;
}

Uint32 EquipBagSystem::getEquip(int iPos)
{
	if( (iPos < 0) || (iPos >= (int)m_equipList.size() ) )
	{
		return 0;
	}

	return m_equipList[iPos];
}

bool EquipBagSystem::setEquip(int iPos, Uint32 hEquipHandle, GSProto::EquipChgItem* pOutItem)
{
	if(!canSetEquip(iPos, hEquipHandle, false) )
	{
		return false;
	}
	
	m_equipList[iPos] = hEquipHandle;

	if(pOutItem)
	{
		if(0 == hEquipHandle)
		{
			pOutItem->set_ipos(iPos);
			pOutItem->set_bdel(true);
		}
		else
		{
			IItem* pItem = getItemFromHandle(hEquipHandle);
			assert(pItem);

			pOutItem->set_ipos(iPos);
			pOutItem->set_bdel(false);

			pItem->packScEquipInfo(*pOutItem->mutable_equipdata(), iPos);
		}
	}

	return true;
}

HEntity EquipBagSystem::getOwner()
{
	return m_hEntity;
}

void EquipBagSystem::sendContainerChg(const vector<int>& posList)
{
	GSProto::CMD_EQUIPCONTAINER_CHG_SC scMsg;
	scMsg.set_dwobjectid(m_hEntity);

	for(size_t i = 0; i < posList.size(); i++)
	{
		GSProto::EquipChgItem* pNewEquipChgItem = scMsg.add_szchglist();
		assert(pNewEquipChgItem);

		int iPos = posList[i];
		IItem* pItem = getItemFromHandle(getEquip(iPos) );
		if(!pItem)
		{
			pNewEquipChgItem->set_ipos(iPos);
			pNewEquipChgItem->set_bdel(true);
		}
		else
		{
			pNewEquipChgItem->set_ipos(iPos);
			pNewEquipChgItem->set_bdel(false);
			pItem->packScEquipInfo(*pNewEquipChgItem->mutable_equipdata(), iPos);
		}
	}

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	pEntity->sendMessage(GSProto::CMD_EQUIPCONTAINER_CHG, scMsg);
}


bool EquipBagSystem::canSetEquip(int iPos, Uint32 hEquipHandle, bool bNotifyError)
{
	if( (iPos <0) || (iPos >= getSize() ) )
	{
		return false;
	}

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	IItem* pEquip = getItemFromHandle(hEquipHandle);
	if(pEquip)
	{
		int iItemType = pEquip->getProperty(PROP_ITEM_TYPE, 0);
		if(iItemType != GSProto::en_ItemType_Equip)
		{
			if(bNotifyError) pEntity->sendErrorCode(ERROR_INVALID_EQUIP);
			return false;
		}
	}

	return true;
}


void* EquipBagSystem::queryInterface(int iInterfaceID)
{
	if(IID_IEquipContainer == iInterfaceID)
	{
		return (IEquipContainer*)this;
	}

	return NULL;
}


bool EquipBagSystem::queryEquip(	HEntity 			_cobjId, 
										int 				_pos,
										Uint32				&_outEquip,
										IEquipContainer**	_outContainer)
{
	// 检查是否合法
	if(!checkEntityValid(_cobjId)){
		return false;
	}

	// 获取数据
	IEquipContainer* container = this->getEquipContainer(_cobjId);
	if(container == NULL){
		return false;
	}
	if(_outContainer != NULL){
		* _outContainer = container;
	}
	_outEquip = container->getEquip(_pos);
	return (_outEquip != 0);
}



