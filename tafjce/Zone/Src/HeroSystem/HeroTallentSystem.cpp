#include "HeroSystemPch.h"
#include "HeroTallentSystem.h"
#include "IEffectSystem.h"
#include "HeroTallentFactory.h"
#include "ErrorCodeDef.h"
#include "IItemFactory.h"
#include "IItemSystem.h"
#include "EffectProperty.h"
#include "ItemProperty.h"

extern "C" IObject* createTallentSystem()
{
	return new HeroTallentSystem;
}


HeroTallentSystem::HeroTallentSystem()
{
}

HeroTallentSystem::~HeroTallentSystem()
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	if(!pEntity) return;
		

	HEntity hMaster = pEntity->getProperty(PROP_ENTITY_MASTER, 0);
	IEntity* pMaster = getEntityFromHandle(hMaster);
	if(pMaster)
	{
		pMaster->getEventServer()->unsubscribeEvent(EVENT_ENTITY_LEVELUP, this, &HeroTallentSystem::onEventMasterLevelChg);
		pMaster->getEventServer()->unsubscribeEvent(EVENT_ENTITY_PROPCHANGE, this, &HeroTallentSystem::onEventMasterSilverChg);
		pMaster->getEventServer()->unsubscribeEvent(EVENT_ENTITY_ITEMCHG, this, &HeroTallentSystem::onEventItemChg);
	}
}

Uint32 HeroTallentSystem::getSubsystemID() const
{
	return IID_IHeroTallentSystem;
}


Uint32 HeroTallentSystem::getMasterHandle()
{
	return m_hEntity;
}

void HeroTallentSystem::initTallent()
{
	ITable* pHeroTable = getCompomentObjectManager()->findTable(TABLENAME_Hero);
	assert(pHeroTable);

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	int iHeroID = pEntity->getProperty(PROP_ENTITY_BASEID, 0);
	int iRecord = pHeroTable->findRecord(iHeroID);
	assert(iRecord >= 0);

	string strTallentList = pHeroTable->getString(iRecord, "天赋技能列表");
	vector<int> tallentList = TC_Common::sepstr<int>(strTallentList, "#");

	HeroTallentFactory* pHeroTallentFactory = getComponent<HeroTallentFactory>("HeroTallentFactory", IID_IHeroTallentFactory);
	assert(pHeroTallentFactory);

	for(size_t i = 0; i < tallentList.size(); i++)
	{
		ServerEngine::TallentData tmpTallentData;
		tmpTallentData.iTallentID = tallentList[i];
	
		// 验证是否激活
		const HeroTallentCfg* pTallentCfg = pHeroTallentFactory->queryTallentCfg(tallentList[i], 0);
		assert(pTallentCfg);

		if(canAutoUpgrade(tallentList[i], 0) )
		{
			tmpTallentData.iLevel = 1;
		}
		else
		{
			tmpTallentData.iLevel = 0;
		}
		
		m_tallentSystemData.tallentList.push_back(tmpTallentData);
	}
}

const std::vector<Uint32>& HeroTallentSystem::getSupportMessage()
{
	static std::vector<Uint32> resultList;

	return resultList;
}

void HeroTallentSystem::fillHeroTallent(GSProto::HeroBaseData& scHeroBase)
{
	google::protobuf::RepeatedPtrField<GSProto::HeroTallent>* pScTallentList = scHeroBase.mutable_sztallentlist();

	for(size_t i = 0; i < m_tallentSystemData.tallentList.size(); i++)
	{
		const ServerEngine::TallentData& refTallentData = m_tallentSystemData.tallentList[i];
	
		GSProto::HeroTallent* pNewHeroTallent = pScTallentList->Add();
		pNewHeroTallent->set_itallentid(refTallentData.iTallentID);
		pNewHeroTallent->set_ilevel(refTallentData.iLevel);
			
		bool bCanUpgrade = canTallentUpgrade(refTallentData.iTallentID, refTallentData.iLevel, false);
		m_canUpgradeMap[refTallentData.iTallentID] = bCanUpgrade;
		pNewHeroTallent->set_bcanupgrade(bCanUpgrade);
	}

	for(map<int, int>::const_iterator it = m_tallentSystemData.favoriteIDList.begin(); it != m_tallentSystemData.favoriteIDList.end(); it++)
	{
		GSProto::HeroFavoriteDesc* pNewFav = scHeroBase.mutable_szfavoritelist()->Add();
		pNewFav->set_iitemid(it->second);
		pNewFav->set_ipos(it->first);
	}

	/*for(size_t i = 0; i < m_tallentSystemData.favoriteIDList.size(); i++)
	{
		scHeroBase.add_szfavoritelist(m_tallentSystemData.favoriteIDList[i]);
	}*/
}


bool HeroTallentSystem::canAutoUpgrade(int iTallentID, int iLevel)
{
	HeroTallentFactory* pHeroTallentFactory = getComponent<HeroTallentFactory>("HeroTallentFactory", IID_IHeroTallentFactory);
	assert(pHeroTallentFactory);

	IEntity* pHero = getEntityFromHandle(m_hEntity);
	assert(pHero);

	//int iLevelStep = pHero->getProperty(PROP_ENTITY_LEVELSTEP, 0);
	int iQuality = pHero->getProperty(PROP_ENTITY_QUALITY, 0);

	const HeroTallentCfg* pTallentCfg = pHeroTallentFactory->queryTallentCfg(iTallentID, iLevel);
	if(!pTallentCfg)
	{
		return false;
	}

	IEntity* pMaster = getEntityFromHandle(pHero->getProperty(PROP_ENTITY_MASTER, 0) );
	if(!pMaster)
	{
		return false;
	}
	
	int iMasterLv = pMaster->getProperty(PROP_ENTITY_LEVEL, 0);
	if(iMasterLv < pTallentCfg->iMasterLv)
	{
		return false;
	}

	if(!pHeroTallentFactory->queryTallentCfg(iTallentID, iLevel + 1) )
	{
		return false;
	}

	if(iQuality < pTallentCfg->iUpgradeQuality)
	{
		return false;
	}

	if( (pTallentCfg->upgradeMaterialMap.size() == 0) && (pTallentCfg->iUpgradeSilver == 0) )
	{
		return true;
	}
	
	return false;
}


bool HeroTallentSystem::getTallentLevel(int iTallentID, int& iLevel)
{
	for(size_t i = 0; i < m_tallentSystemData.tallentList.size(); i++)
	{
		if(m_tallentSystemData.tallentList[i].iTallentID == iTallentID)
		{
			iLevel = m_tallentSystemData.tallentList[i].iLevel;
			return true;
		}
	}

	return false;
}


bool HeroTallentSystem::canTallentUpgrade(int iTallentID, int iLevel, bool bNotify)
{
	HeroTallentFactory* pHeroTallentFactory = getComponent<HeroTallentFactory>("HeroTallentFactory", IID_IHeroTallentFactory);
	assert(pHeroTallentFactory);

	IEntity* pHero = getEntityFromHandle(m_hEntity);
	assert(pHero);

	//int iLevelStep = pHero->getProperty(PROP_ENTITY_LEVELSTEP, 0);
	int iQuality = pHero->getProperty(PROP_ENTITY_QUALITY, 0);

	const HeroTallentCfg* pTallentCfg = pHeroTallentFactory->queryTallentCfg(iTallentID, iLevel);
	if(!pTallentCfg)
	{
		return false;
	}

	
	IEntity* pMaster = getEntityFromHandle(pHero->getProperty(PROP_ENTITY_MASTER, 0) );
	if(!pMaster) return false;

	if(!pHeroTallentFactory->queryTallentCfg(iTallentID, iLevel + 1) )
	{
		if(bNotify) pMaster->sendErrorCode(ERROR_HEROTALLENT_TOCHMAXLV);
		return false;
	}

	int iMasterLevel = pMaster->getProperty(PROP_ENTITY_LEVEL, 0);
	if(iMasterLevel < pTallentCfg->iMasterLv)
	{
		if(bNotify) pMaster->sendErrorCode(ERROR_HEROTALLENT_NEEDMASTERLV);
		return false;
	}

	if(iQuality < pTallentCfg->iUpgradeQuality)
	{
		if(bNotify) pMaster->sendErrorCode(ERROR_TALLENT_NEEDLEVELSTEP);
		return false;
	}

	IItemSystem* pItemSystem = static_cast<IItemSystem*>(pMaster->querySubsystem(IID_IItemSystem));
	if(!pItemSystem) return false;

	for(map<int, int>::const_iterator it = pTallentCfg->upgradeMaterialMap.begin(); it != pTallentCfg->upgradeMaterialMap.end(); it++)
	{
		int iItemID = it->first;
		int iItemCount = it->second;

		if(pItemSystem->getItemCount(iItemID) < iItemCount)
		{
			if(bNotify) pMaster->sendErrorCode(ERORR_TALLENT_NEEDITEM);
			return false;
		}
	}

	//if(bCheckSilver)
	{
		int iTkSilver = pMaster->getProperty(PROP_ACTOR_SILVER, 0);
		if(iTkSilver < pTallentCfg->iUpgradeSilver)
		{
			if(bNotify) pMaster->sendErrorCode(ERROR_NEED_SILVER);
			return false;
		}
	}
	
	return true;
}

bool HeroTallentSystem::consumeTalllentUpgrade(int iTallentID, int iLevel)
{
	if(!canTallentUpgrade(iTallentID, iLevel, false))
	{
		return false;
	}
	
	HeroTallentFactory* pHeroTallentFactory = getComponent<HeroTallentFactory>("HeroTallentFactory", IID_IHeroTallentFactory);
	assert(pHeroTallentFactory);

	const HeroTallentCfg* pTallentCfg = pHeroTallentFactory->queryTallentCfg(iTallentID, iLevel);
	assert(pTallentCfg);

	IEntity* pHero = getEntityFromHandle(m_hEntity);
	assert(pHero);

	IEntity* pMaster = getEntityFromHandle(pHero->getProperty(PROP_ENTITY_MASTER, 0) );
	if(!pMaster)
	{
		return false;
	}
	
	IItemSystem* pItemSystem = static_cast<IItemSystem*>(pMaster->querySubsystem(IID_IItemSystem));
	assert(pItemSystem);

	// 消耗材料
	for(map<int, int>::const_iterator it = pTallentCfg->upgradeMaterialMap.begin(); it != pTallentCfg->upgradeMaterialMap.end(); it++)
	{
		int iItemID = it->first;
		int iItemCount = it->second;

		bool bResult = pItemSystem->removeItem(iItemID, iItemCount, GSProto::en_Reason_TallentUpgrade);
		assert(bResult);
	}

	// 消耗金钱
	pMaster->changeProperty(PROP_ACTOR_SILVER, 0-pTallentCfg->iUpgradeSilver, GSProto::en_Reason_TallentUpgrade);

	return true;
}

bool HeroTallentSystem::create(IEntity* pEntity, const std::string& strData)
{
	m_hEntity = pEntity->getHandle();
	if(strData.size() == 0)
	{
		initTallent();
	}
	else
	{
		ServerEngine::JceToObj(strData, m_tallentSystemData);
	}

	// 注册等阶上升事件
	pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_LEVELSTEPCHG, this, &HeroTallentSystem::onEventLevelStepChg);

	HEntity hMaster = pEntity->getProperty(PROP_ENTITY_MASTER, 0);
	IEntity* pMaster = getEntityFromHandle(hMaster);
	if(pMaster)
	{
		pMaster->getEventServer()->subscribeEvent(EVENT_ENTITY_LEVELUP, this, &HeroTallentSystem::onEventMasterLevelChg);
		pMaster->getEventServer()->subscribeEvent(EVENT_ENTITY_PROPCHANGE, this, &HeroTallentSystem::onEventMasterSilverChg);
		pMaster->getEventServer()->subscribeEvent(EVENT_ENTITY_ITEMCHG, this, &HeroTallentSystem::onEventItemChg);
	}

	return true;
}


void HeroTallentSystem::notifyUpgradeFlagChg()
{
	for(size_t i = 0; i < m_tallentSystemData.tallentList.size(); i++)
	{
		bool bRet = canTallentUpgrade(m_tallentSystemData.tallentList[i].iTallentID, m_tallentSystemData.tallentList[i].iLevel, false);
		if(bRet == m_canUpgradeMap[m_tallentSystemData.tallentList[i].iTallentID] )
		{
			continue;
		}

		m_canUpgradeMap[m_tallentSystemData.tallentList[i].iTallentID] = bRet;
		sendTallentUpdate(m_tallentSystemData.tallentList[i].iTallentID, m_tallentSystemData.tallentList[i].iLevel);
	}
}


void HeroTallentSystem::checkAutoUpgrade()
{
	for(size_t i = 0; i < m_tallentSystemData.tallentList.size(); i++)
	{
		ServerEngine::TallentData& refTallentData = m_tallentSystemData.tallentList[i];
		while(canAutoUpgrade(refTallentData.iTallentID, refTallentData.iLevel) )
		{
			tallentUpgrade(refTallentData);
		}
	}

	notifyUpgradeFlagChg();
}


void HeroTallentSystem::onEventLevelStepChg(EventArgs& args)
{
	checkAutoUpgrade();
}


void HeroTallentSystem::onEventMasterLevelChg(EventArgs& args)
{
	notifyUpgradeFlagChg();
}

void HeroTallentSystem::onEventMasterSilverChg(EventArgs& args)
{
	EventArgs_PropChange& propChgArgs = (EventArgs_PropChange&)args;
	if(propChgArgs.iPropID != PROP_ACTOR_SILVER)
	{
		return;
	}

	notifyUpgradeFlagChg();
}

void HeroTallentSystem::onEventItemChg(EventArgs& args)
{
	//int iItemID = args.context.getInt("item");
	//int iChgCount = args.context.getInt("count");

	//if(iChgCount < 0) return;

	notifyUpgradeFlagChg();
}

void HeroTallentSystem::tallentUpgrade(int iTallendID)
{
	for(size_t i = 0; i < m_tallentSystemData.tallentList.size(); i++)
	{
		if(iTallendID == m_tallentSystemData.tallentList[i].iTallentID)
		{
			tallentUpgrade(m_tallentSystemData.tallentList[i]);
			return;
		}
	}
}


void HeroTallentSystem::equipFavorite(int iPos, int iItemID)
{
	

	// 判断自身是否已经装备了
	/*if(find(m_tallentSystemData.favoriteIDList.begin(), m_tallentSystemData.favoriteIDList.end(), iItemID) != m_tallentSystemData.favoriteIDList.end() )
	{
		return;
	}*/

	// 判断当前品质是否需要这个物品
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	int iQuality = pEntity->getProperty(PROP_ENTITY_QUALITY, 0);
	
	HeroTallentFactory* pHeroTallentFactory = getComponent<HeroTallentFactory>("HeroTallentFactory", IID_IHeroTallentFactory);
	assert(pHeroTallentFactory);

	int iHeroBaseID = pEntity->getProperty(PROP_ENTITY_BASEID, 0);
	const FavoriteConsume* pFavoriteConsume = pHeroTallentFactory->queryQualityConsume(iHeroBaseID, iQuality);
	assert(pFavoriteConsume);

	// 验证位置合法
	if( (pFavoriteConsume->favoriteItemList.find(iPos) == pFavoriteConsume->favoriteItemList.end() ) || (pFavoriteConsume->favoriteItemList.at(iPos) != iItemID) )
	{
		return;
	}

	// 判断身上是否有这个喜好品
	if(m_tallentSystemData.favoriteIDList.find(iPos) != m_tallentSystemData.favoriteIDList.end() )
	{
		return;
	}

	HEntity hMaster = pEntity->getProperty(PROP_ENTITY_MASTER, 0);
	IEntity* pMaster = getEntityFromHandle(hMaster);
	assert(pMaster);

	IItemSystem* pItemSystem = static_cast<IItemSystem*>(pMaster->querySubsystem(IID_IItemSystem));
	assert(pItemSystem);

	if(pItemSystem->getItemCount(iItemID) == 0)
	{
		return;
	}

	// 验证装备等级是否足够
	IItemFactory* pItemFactory = getComponent<IItemFactory>(COMPNAME_ItemFactory, IID_IItemFactory);
	assert(pItemFactory);

	const PropertySet* pItemPropset = pItemFactory->getItemPropset(iItemID);
	if(!pItemPropset) return;

	int iCondLevel = pItemPropset->getInt(PROP_ITEM_LVLLIMIT, 0);
	//int iHeroLevel = pEntity->getProperty(PROP_ENTITY_LEVEL, 0);
	int iMasterLevel = pMaster->getProperty(PROP_ENTITY_LEVEL, 0);
	if(iMasterLevel < iCondLevel) return;

	if(!pItemSystem->removeItem(iItemID, 1, GSProto::en_Reason_EquipFavorite) )
	{
		return;
	}

	m_tallentSystemData.favoriteIDList[iPos] = iItemID;

	equipFavoriteProp(iItemID, true);

	// 通知客户端
	GSProto::CMD_HERO_EQUIP_FAVORITE_SC scMsg;
	scMsg.set_dwheroobjectid(m_hEntity);
	scMsg.set_bresult(true);
	scMsg.set_iitemid(iItemID);
	scMsg.set_ipos(iPos);

	pMaster->sendMessage(GSProto::CMD_HERO_EQUIP_FAVORITE, scMsg);

	//任务装备喜好品
	{
		EventArgs args;
		args.context.setInt("times",1);
		pMaster->getEventServer()->setEvent(EVENT_ENTITY_TASK_EQUIPLOVEQUIP , args);
	}
}


void HeroTallentSystem::equipFavoriteProp(int iItemID, bool bPuton)
{
	IItemFactory* pItemFactory = getComponent<IItemFactory>(COMPNAME_ItemFactory, IID_IItemFactory);
	assert(pItemFactory);

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	const map<int, int>* pItemPropMap = pItemFactory->queryFavoriteProp(iItemID);
	assert(pItemPropMap);

	for(map<int, int>::const_iterator it = pItemPropMap->begin(); it != pItemPropMap->end(); it++)
	{
		if(bPuton)
		{
			pEntity->changeProperty(it->first, it->second, 0);
		}
		else
		{
			pEntity->changeProperty(it->first, -it->second, 0);
		}
	}
}

void HeroTallentSystem::calcQualityAndFavoriteProp(bool bPuton)
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	int iQuality = pEntity->getProperty(PROP_ENTITY_QUALITY, 0);

	HeroTallentFactory* pHeroTallentFactory = getComponent<HeroTallentFactory>(COMPNAME_HeroTallentFactory, IID_IHeroTallentFactory);
	assert(pHeroTallentFactory);

	int iHeroID = pEntity->getProperty(PROP_ENTITY_BASEID, 0);
	if(iQuality > 1)
	{
		const QualityProp* pQualityProp = pHeroTallentFactory->queryQualityPropMap(iHeroID, iQuality);
		assert(pQualityProp);

		for(map<int, int>::const_iterator it = pQualityProp->addPropMap.begin(); it != pQualityProp->addPropMap.end(); it++)
		{
			if(bPuton)
			{
				pEntity->changeProperty(it->first, it->second, 0);
			}
			else
			{
				pEntity->changeProperty(it->first, -it->second, 0);
			}
		}
	}

	// 身上的喜好品数值
	/*for(size_t i = 0; i < m_tallentSystemData.favoriteIDList.size(); i++)
	{
		equipFavoriteProp(m_tallentSystemData.favoriteIDList[i], bPuton);
	}*/

	for(map<int, int>::const_iterator it = m_tallentSystemData.favoriteIDList.begin(); it != m_tallentSystemData.favoriteIDList.end(); it++)
	{
		equipFavoriteProp(it->second, bPuton);
	}
}


extern int Prop2LifeAtt(int iPropID);

void HeroTallentSystem::upgradeQuality()
{
	IEntity* pHero = getEntityFromHandle(m_hEntity);
	assert(pHero);

	IEntity* pMaster = getEntityFromHandle(pHero->getProperty(PROP_ENTITY_MASTER, 0) );
	assert(pMaster);

	int iQuality = pHero->getProperty(PROP_ENTITY_QUALITY, 0);

	HeroTallentFactory* pHeroTallentFactory = getComponent<HeroTallentFactory>("HeroTallentFactory", IID_IHeroTallentFactory);
	assert(pHeroTallentFactory);

	int iHeroBaseID = pHero->getProperty(PROP_ENTITY_BASEID, 0);
	const FavoriteConsume* pFavoriteConsume = pHeroTallentFactory->queryQualityConsume(iHeroBaseID, iQuality);
	assert(pFavoriteConsume);

	// 判断是否有下一级品质
	if(!pHeroTallentFactory->queryQualityConsume(iHeroBaseID, iQuality + 1) )
	{
		pMaster->sendErrorCode(ERROR_QUALITY_TOUCHMAX);
		return;
	}

	/*for(size_t i = 0; i < pFavoriteConsume->favoriteItemList.size(); i++)
	{
		int iNeedItem = pFavoriteConsume->favoriteItemList[i];
		if(find(m_tallentSystemData.favoriteIDList.begin(), m_tallentSystemData.favoriteIDList.end(), iNeedItem) == m_tallentSystemData.favoriteIDList.end() )
		{
			return;
		}
	}*/

	for(map<int, int>::const_iterator it = pFavoriteConsume->favoriteItemList.begin(); it != pFavoriteConsume->favoriteItemList.end(); it++)
	{
		if(m_tallentSystemData.favoriteIDList.find(it->first) == m_tallentSystemData.favoriteIDList.end() )
		{
			return;
		}

		if(it->second != m_tallentSystemData.favoriteIDList.at(it->first) )
		{
			return;
		}
	}

	
	static const int szPropList[] = {PROP_ENTITY_MAXHP, PROP_ENTITY_DEF, PROP_ENTITY_ATT, };
	int szAddValue[sizeof(szPropList)/sizeof(int)] = {0};
	
	for(size_t i = 0; i < sizeof(szPropList)/sizeof(int); i++)
	{
		szAddValue[i] = pHero->getProperty(szPropList[i], 0);
	}

	calcQualityAndFavoriteProp(false);
	m_tallentSystemData.favoriteIDList.clear();
	pHero->setProperty(PROP_ENTITY_QUALITY, iQuality + 1);
	
	EventArgs args;
	args.context.setInt("entity", m_hEntity);
	pHero->getEventServer()->setEvent(EVENT_ENTITY_CALCGROW, args);

	calcQualityAndFavoriteProp(true);
	

	// 通知成功
	GSProto::CMD_HERO_QUALITY_UP_SC scMsg;
	scMsg.set_dwheroobjectid(m_hEntity);

	for(size_t i = 0; i < sizeof(szPropList)/sizeof(int); i++)
	{
		int iAddValue = pHero->getProperty(szPropList[i], 0) - szAddValue[i];
		GSProto::PropItem* pNewItem = scMsg.mutable_szpropaddvaluelist()->Add();
		pNewItem->set_ilifeattid(Prop2LifeAtt(szPropList[i]));
		pNewItem->set_ivalue(iAddValue);
	}

	pMaster->sendMessage(GSProto::CMD_HERO_QUALITY_UP, scMsg);

	//任务系统
	{
		EventArgs ags;
		args.context.setInt("quality", iQuality + 1 );
		pMaster->getEventServer()->setEvent(EVENT_ENTITY_TASK_HEROQUALITY,args);
	}

	checkAutoUpgrade();
}


void HeroTallentSystem::tallentUpgrade(ServerEngine::TallentData& refTallentData)
{
	refTallentData.iLevel++;

	// 属性修改
	HeroTallentFactory* pHeroTallentFactory = getComponent<HeroTallentFactory>("HeroTallentFactory", IID_IHeroTallentFactory);
	assert(pHeroTallentFactory);
	
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	IEffectSystem* pEffectSystem = static_cast<IEffectSystem*>(pEntity->querySubsystem(IID_IEffectSystem));
	assert(pEffectSystem);

	// 先去掉原属性
	EffectContext tmpCtx;
	tmpCtx.setInt(PROP_EFFECT_GIVER, (Int32)m_hEntity);
	tmpCtx.setInt(PROP_EFFECT_NEGATIVE, 1);
	tmpCtx.setInt(PROP_EFFECT_HEROTALLENT_ID, refTallentData.iTallentID);

	const HeroTallentCfg* pOldTallentCfg = pHeroTallentFactory->queryTallentCfg(refTallentData.iTallentID, refTallentData.iLevel - 1);
	assert(pOldTallentCfg);
	for(size_t i = 0; i < pOldTallentCfg->effectList.size(); i++)
	{
		pEffectSystem->addEffect(pOldTallentCfg->effectList[i], tmpCtx);
	}

	// 添加新的
	const HeroTallentCfg* pNewTallentCfg = pHeroTallentFactory->queryTallentCfg(refTallentData.iTallentID, refTallentData.iLevel);
	assert(pNewTallentCfg);

	tmpCtx.setInt(PROP_EFFECT_NEGATIVE, 0);
	for(size_t i = 0; i < pNewTallentCfg->effectList.size(); i++)
	{
		pEffectSystem->addEffect(pNewTallentCfg->effectList[i], tmpCtx);
	}

	// 通知客户端天赋更新
	sendTallentUpdate(refTallentData.iTallentID, refTallentData.iLevel);
	
	IEntity* pMaster = getEntityFromHandle(pEntity->getProperty(PROP_ENTITY_MASTER, 0) );
	if(pMaster)
	{
		pMaster->sendMessage(GSProto::CMD_HERO_TALLENT_LEVELUP);
	}
}


void HeroTallentSystem::sendTallentUpdate(int iTallentID, int iLevel)
{
	GSProto::CMD_HERO_TALLENT_UPDATE_SC scMsg;

	scMsg.set_dwobjectid(m_hEntity);
	
	GSProto::HeroTallent* pScTallentData = scMsg.mutable_tallentdata();
	pScTallentData->set_itallentid(iTallentID);
	pScTallentData->set_ilevel(iLevel);

	bool bCanUpdate = canTallentUpgrade(iTallentID, iLevel, false);

	m_canUpgradeMap[iTallentID] = bCanUpdate;
	
	pScTallentData->set_bcanupgrade(bCanUpdate);

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	IEntity* pMaster = getEntityFromHandle(pEntity->getProperty(PROP_ENTITY_MASTER, 0) );
	{
		pMaster->sendMessage(GSProto::CMD_HERO_TALLENT_UPDATE, scMsg);
	}
}


bool HeroTallentSystem::createComplete()
{
	// 附加属性
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	IEffectSystem* pEffectSystem = static_cast<IEffectSystem*>(pEntity->querySubsystem(IID_IEffectSystem));
	assert(pEffectSystem);

	HeroTallentFactory* pHeroTallentFactory = getComponent<HeroTallentFactory>("HeroTallentFactory", IID_IHeroTallentFactory);
	assert(pHeroTallentFactory);
	
	for(size_t i = 0; i < m_tallentSystemData.tallentList.size(); i++)
	{
		const ServerEngine::TallentData& refTallentData = m_tallentSystemData.tallentList[i];
		
		const HeroTallentCfg* pTallentCfg = pHeroTallentFactory->queryTallentCfg(refTallentData.iTallentID, refTallentData.iLevel);
		assert(pTallentCfg);

		EffectContext tmpCtx;
		tmpCtx.setInt(PROP_EFFECT_GIVER, (Int32)m_hEntity);
		tmpCtx.setInt(PROP_EFFECT_HEROTALLENT_ID, refTallentData.iTallentID);
		for(vector<int>::const_iterator it = pTallentCfg->effectList.begin(); it != pTallentCfg->effectList.end(); it++)
		{
			pEffectSystem->addEffect(*it, tmpCtx);
		}
	}

	// 附加喜好品属性
	calcQualityAndFavoriteProp(true);
	
	return true;
}

void HeroTallentSystem::packSaveData(string& data)
{
	data = ServerEngine::JceToStr(m_tallentSystemData);
}



