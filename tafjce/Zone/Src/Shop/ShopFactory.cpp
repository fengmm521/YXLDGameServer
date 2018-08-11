#include "ShopSystemPch.h"
#include "ShopFactory.h"

extern "C" IComponent* createShopFactory(INT32)
{
	return new ShopFactory;
}

ShopFactory::ShopFactory():
	m_iRandHeroUseTokenTotalChance(0),
	m_iRandHeroUseGoldTotalChance(0),
	m_iRandItemUseTokenTotalChance(0),
	m_iRandItemUseGoldTotalChance(0),   
	iUseTokenRandHeroChance(0),
	iUseTokenRandItemChance(0),
	iUseGoldRandHeroChance(0),
	iUseGoldRandItemChance(0),
	iRefreshHeroSoulCount(0),
	iRefreshEquipCount(0),
	iRefreshFavorCount(0),
	m_CoinTreeKnocKTotalChance(0),
	m_iVipRandChance(0),
	m_iVipHeroChance(0),
	m_iTokenWuHunChance(0),
	m_normalShopRefreshHeroSoulCount(0),
	m_normalShopRefreshEquipCount(0),
	m_normalShopRefreshFavorCount(0)
{
	
}

ShopFactory::~ShopFactory()
{
}
    // IComponent interface
bool ShopFactory::initlize(const PropertySet& propSet)
{
	return loadAllShopData();
}

bool ShopFactory::loadAllShopData()
{
	initRandHeroData();
	initRandItemData();
	initHonorGoodsData();
	initHonorShopData();

	loadCoinTreeData();
	initVipRandData();
	
	IGlobalCfg *pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert( pGlobal);
	string strTokenItemAndHeroChance = pGlobal->getString("商城令牌单抽概率分布英雄权重在前", "50#50");
	string strGoldItemAndHeroChance = pGlobal->getString("商城元宝单抽概率分布英雄权重在前", "50#50");
	string strHonorShopGetCount = pGlobal->getString("荣誉商店武魂装备喜好品分布", "3#0#6");
	string normalShopCountString = pGlobal->getString("普通商店物品分布", "3#0#6");
	vector<int> chanceVec1 = TC_Common::sepstr<int>(strTokenItemAndHeroChance, "#") ;
	vector<int> chanceVec2 = TC_Common::sepstr<int>(strGoldItemAndHeroChance, "#") ;
	vector<int> countVec = TC_Common::sepstr<int>(strHonorShopGetCount, "#") ;
	vector<int> normalShopCount = TC_Common::sepstr<int>(normalShopCountString, "#");
	assert(chanceVec1.size() == 2);
	assert(chanceVec2.size() == 2);
	assert(countVec.size() == 3);
	assert(normalShopCount.size() == 3);

	// 获取普通商店刷新时间间隔
	//this->m_normalShopRefreshInterval = pGlobal->getInt("普通商店刷新间隔秒数", 1000);
	string strNormalShopRefreshTime = pGlobal->getString("普通商店刷新时间", "");
	this->m_normalShopRefreshTime = TC_Common::sepstr<int>(strNormalShopRefreshTime, "#");

 	iUseTokenRandHeroChance = chanceVec1[0];
	iUseTokenRandItemChance = chanceVec1[1];
	iUseGoldRandHeroChance = chanceVec2[0] ;
	iUseGoldRandItemChance = chanceVec2[1];
	iRefreshHeroSoulCount =  countVec[0];
	iRefreshEquipCount =  countVec[1];
	iRefreshFavorCount =  countVec[2];

	this->m_normalShopRefreshHeroSoulCount = normalShopCount[0];
	this->m_normalShopRefreshEquipCount = normalShopCount[1];
	this->m_normalShopRefreshFavorCount = normalShopCount[2];
	printf(	"normalShopRefreshHeroSoulCount:%u "
			"normalShopRefreshEquipCount:%u "
			"normalShopRefreshFavorCount:%u ",
			this->m_normalShopRefreshHeroSoulCount,
			this->m_normalShopRefreshEquipCount,
			this->m_normalShopRefreshFavorCount);

	//m_HonorRefreshMap
	ITable* pRefreshTable = getCompomentObjectManager()->findTable(TABLENAME_HonorShopRefreshCost);
	assert(pRefreshTable);
	int iRefreshRecord = pRefreshTable->getRecordCount();
	for(int i = 0; i < iRefreshRecord; ++i)
	{
		int iRefreshCount = pRefreshTable->getInt( i, "刷新次数");
		int iRefreshCost = pRefreshTable->getInt( i, "刷新消耗");
		m_HonorRefreshMap.insert(make_pair(iRefreshCount,iRefreshCost));
	}

	//check Data
	checkGoldRandData();
	loadRandHeroTemplateData();
	
	return true;
}

void ShopFactory::initVipRandData()
{
	ITable* pRandTable = getCompomentObjectManager()->findTable(TABLENAME_VipRand);
	assert(pRandTable);
	
	int iRecord = pRandTable->getRecordCount();
	for(int i = 0; i < iRecord; ++i)
	{
		int iId = pRandTable->getInt(i, "英雄ID");
		int iHeroFlag = pRandTable->getInt(i, "是英雄");
		int iQuality = pRandTable->getInt(i, "品质");
		int iLevelStep = pRandTable->getInt(i, "等阶");
		int iChance = pRandTable->getInt(i, "机率");
		int iCount = pRandTable->getInt(i, "数量");
		
		VIPRandUnit unit;
		unit.iId = iId;
		unit.bIsHero = (iHeroFlag == 1);
		unit.iQuality = iQuality;
		unit.iStepLevel = iLevelStep;
		unit.iChance = iChance;
		unit.iCount = iCount;


		if(unit.bIsHero )
		{
			m_iVipHeroChance += iChance;
		}
		
		m_VipRandList.push_back(unit);
		
		m_iVipRandChance += unit.iChance;
	}
}


void ShopFactory::loadRandHeroTemplateData()
{
	ITable* pRandTemplateTable = getCompomentObjectManager()->findTable(TABLENAME_RandTemplate);
	int iRecord = pRandTemplateTable->getRecordCount();
	
	for(int i = 0; i < iRecord; ++i)
	{
		int iId = pRandTemplateTable->getInt(i, "模板ID");
		string strHero = pRandTemplateTable->getString(i, "英雄");
		string strItem = pRandTemplateTable->getString(i, "物品");
		string strEquip = pRandTemplateTable->getString(i, "装备");
		int iChance = pRandTemplateTable->getInt(i, "概率");

		iTotalChance += iChance;

		vector<int> heroList = TC_Common::sepstr<int>(strHero, "#");
		vector<int> itemList = TC_Common::sepstr<int>(strItem, "#");
		vector<int> equipList = TC_Common::sepstr<int>(strEquip, "#");
		assert(heroList.size() % 2 == 0);
		assert(itemList.size() % 2 == 0);
		assert(equipList.size() % 2 == 0);
		RandTemplate temp;
		temp.iTmplateId = iId;
		temp.iChance = iChance ;
		for(size_t i = 0; i < heroList.size(); i = i + 2)
		{
			RandTemplateUnit unit;
			unit.iType =  heroList[i];
			unit.iNum = heroList[i + 1];
			temp.heroVec.push_back( unit );
		}

		for(size_t i = 0; i < itemList.size(); i = i + 2)
		{
			RandTemplateUnit unit;
			unit.iType =  itemList[i];
			unit.iNum = itemList[i + 1];
			temp.itemVec.push_back( unit );
		}

		for(size_t i = 0; i < equipList.size(); i = i + 2)
		{
			RandTemplateUnit unit;
			unit.iType =  itemList[i];
			unit.iNum = itemList[i + 1];
			temp.equipVec.push_back( unit );
		}

		m_GlodRandTemplate.push_back(temp);
	}
}


void ShopFactory::checkGoldRandData()
{
	for(size_t i = 0; i < m_RandHeroUseGoldList.size(); ++i)
	{
		RandHero &hero = m_RandHeroUseGoldList[i];
		map<int, vector<RandHero> >::iterator iter = m_GoldRandStepHeroList.find(hero.iHeroLevelStep);
		if(iter == m_GoldRandStepHeroList.end())
		{
			vector<RandHero> vec;
			vec.push_back(hero);
			m_GoldRandStepHeroList.insert( make_pair(hero.iHeroLevelStep, vec));
		}
		else
		{
			vector<RandHero>& vec =  iter->second;
			vec.push_back(hero);
		}
	}

	map<int, vector<RandHero> >::iterator heroListIter = m_GoldRandStepHeroList.begin();
	for(;heroListIter != m_GoldRandStepHeroList.end(); ++heroListIter)
	{
		vector<RandHero>& vec = heroListIter->second;
		int iTmepChance = 0;
		for(size_t i = 0; i < vec.size(); ++i)
		{
			RandHero &hero = vec[i];
			iTmepChance += hero.iChance;
		}
		m_GoldRandStepChance[heroListIter->first] =  iTmepChance;
	}


	for(size_t i = 0; i < m_RandItemUseGoldList.size(); ++i)
	{
		RandItem& item = m_RandItemUseGoldList[i];
		map<int, vector<RandItem> >::iterator iter = m_GoldRandQualityItemList.find(item.iQuality);
		if(iter == m_GoldRandQualityItemList.end())
		{
			vector<RandItem> vec;
			vec.push_back(item);
			m_GoldRandQualityItemList.insert( make_pair(item.iQuality, vec));
		}
		else
		{
			vector<RandItem>& vec =  iter->second;
			vec.push_back(item );
		}
	}

	map<int, vector<RandItem> >::iterator itemListIter = m_GoldRandQualityItemList.begin();
	for(;itemListIter != m_GoldRandQualityItemList.end(); ++itemListIter)
	{
		vector<RandItem>& vec = itemListIter->second;
		int iTmepChance = 0;
		for(size_t i = 0; i < vec.size(); ++i)
		{
			RandItem &item = vec[i];
			iTmepChance += item.iChance;
		}
		m_GoldRandQualityChance[itemListIter->first] =  iTmepChance;
	}
	
	
}

void ShopFactory::loadCoinTreeData()
{
	ITable* pCoinTreeKnockTable = getCompomentObjectManager()->findTable(TABLENAME_CoinTreeKnock);
	assert(pCoinTreeKnockTable);
	int iKnockRecord = pCoinTreeKnockTable->getRecordCount();
	for(int i = 0; i < iKnockRecord; ++i)
	{
		int iDouble = pCoinTreeKnockTable->getInt(i, "暴击倍");
		int iChance = pCoinTreeKnockTable->getInt(i, "万分几率");
		CoinTreeKnock knock;
		knock.iKnockDouble = iDouble;
		knock.iChance = iChance;
		m_CoinTreeKnocKTotalChance += iChance;
		m_CoinTreeKnock.push_back(knock);
	}
	assert(m_CoinTreeKnocKTotalChance >0);

	ITable* pCoinTreeShakeCostTable = getCompomentObjectManager()->findTable( TABLENAME_CoinTreeShakeCost);
	assert(pCoinTreeShakeCostTable);
	int iCostRecord = pCoinTreeShakeCostTable->getRecordCount();
	for(int i = 0; i < iCostRecord; ++i )
	{
		string strTimes = pCoinTreeShakeCostTable->getString(i, "次数");
		int iCost = pCoinTreeShakeCostTable->getInt(i, "消耗元宝");
		vector<int> timesVec = TC_Common::sepstr<int>(strTimes , "#");
		assert(timesVec.size() == 2);

		CoinTreeShakeCost shakeCost;
		shakeCost.iLowTimes = timesVec[0];
		shakeCost.iHightTimes = timesVec[1];
		shakeCost.iCost = iCost;

		m_CoinTreeShakeCost.push_back(shakeCost);
	}
}

int ShopFactory::getShakeCost(int iTimes)
{
	for(size_t i = 0; i < m_CoinTreeShakeCost.size(); ++i)
	{
		const CoinTreeShakeCost &unit = m_CoinTreeShakeCost[i];
		if(unit.iLowTimes <= iTimes && iTimes <= unit.iHightTimes)
		{
			return unit.iCost;
		}
	}
	return -1;
}

int ShopFactory::getKnock()
{
	IRandom *pRandom = getComponent<IRandom>(COMPNAME_Random,  IID_IMiniAprRandom);
	assert(pRandom);
	int iChance = pRandom->random()%m_CoinTreeKnocKTotalChance;
	for(size_t i = 0; i < m_CoinTreeKnock.size(); ++i)
	{
		const CoinTreeKnock& knock = m_CoinTreeKnock[i];
		iChance -= knock.iChance;
		if(iChance <= 0)
		{
			return knock.iKnockDouble;
		}
	}
	return -1;
}


void  ShopFactory::initRandHeroData()
{
	ITable* RandHeroTable = getCompomentObjectManager()->findTable(TABLENAME_RandHero);
	assert(RandHeroTable);
	int RandHeroRecord =  RandHeroTable->getRecordCount();
	for(int i = 0; i < RandHeroRecord; ++i)
	{	
		int iHeroId = RandHeroTable->getInt(i,"英雄ID");
		int iLevelStep = RandHeroTable->getInt(i,"等阶");
		int iChance = RandHeroTable->getInt(i,"机率");
		string strRandType = RandHeroTable->getString(i,"抽取类型");

		RandHero hero;
		hero.iHeroId = iHeroId;
		hero.iHeroLevelStep = iLevelStep;
		hero.iChance = iChance;
		if(strRandType == "令牌")
		{
			m_RandHeroUseTokenList.push_back( hero);
			m_iRandHeroUseTokenTotalChance += hero.iChance;
		}
		else if(strRandType == "元宝")
		{
			m_RandHeroUseGoldList.push_back( hero);
			m_iRandHeroUseGoldTotalChance += hero.iChance;
		}
		else
		{
			assert(0);
		}
		
	}
	assert(m_iRandHeroUseGoldTotalChance > 0);
	assert(m_iRandHeroUseTokenTotalChance > 0);
}

void  ShopFactory::initRandItemData()
{
	ITable* pRandItemTable = getCompomentObjectManager()->findTable(TABLENAME_RandItem);
	assert(pRandItemTable);
	int iRandItemRecord =  pRandItemTable->getRecordCount();
	for(int i = 0; i < iRandItemRecord; ++i)
	{	
		int iItemId = pRandItemTable->getInt(i,"物品ID");
		int iChance = pRandItemTable->getInt(i,"概率");
		int iCount = pRandItemTable->getInt(i,"数量");
		int iQuality = pRandItemTable->getInt(i, "品质");

		int iIsWuhun = pRandItemTable->getInt(i, "是武魂");
		
		string strRandType = pRandItemTable->getString(i,"抽取类型");

		RandItem item;
		item.iItemId = iItemId;
		item.iChance = iChance;
		item.iCount = iCount;
		item.iQuality = iQuality;
		
		if(strRandType == "令牌")
		{
			m_RandItemUseTokenList.push_back( item);
			m_iRandItemUseTokenTotalChance += item.iChance;

			if(iIsWuhun == 1)
			{
				m_iTokenWuHunChance +=  item.iChance;
	 			m_TokenWuHunList.push_back( item);
			}
			 
		}
		else if(strRandType == "元宝")
		{
			m_RandItemUseGoldList.push_back( item);
			m_iRandItemUseGoldTotalChance += item.iChance;
		}
		else
		{
			assert(0);
		}
	}
	assert(m_iRandItemUseTokenTotalChance > 0);
	assert(m_iRandItemUseGoldTotalChance > 0);
}

void  ShopFactory::initHonorShopData()
{
	ITable *pHonorShopTable = getCompomentObjectManager()->findTable(TABLENAME_HonorShop);
	assert(pHonorShopTable);
	int iHonorShopRecord = pHonorShopTable->getRecordCount();
	for(int i = 0; i <iHonorShopRecord; ++i )
	{
		int iShopId = pHonorShopTable->getInt( i , "商店ID");
		string strOpenLevel = pHonorShopTable->getString( i , "商店获取等级");
		string strHeroSoulGoods = pHonorShopTable->getString( i , "武魂商店列表");
		string strEquipGoods = pHonorShopTable->getString( i , "装备列表");
		string strFavorGoods =  pHonorShopTable->getString( i , "喜好品列表");
		vector<int> levelVec = TC_Common::sepstr<int>( strOpenLevel, "#");
		vector<int> heroSoulVec = TC_Common::sepstr<int>(strHeroSoulGoods, "#");
		vector<int> equipVec = TC_Common::sepstr<int>(strEquipGoods, "#");
		vector<int> favorGoodsVec = TC_Common::sepstr<int>(strFavorGoods, "#");

		string strShopType =  pHonorShopTable->getString( i , "商店类型");
			
		assert( levelVec.size() == 2);
		assert( heroSoulVec.size()%2 == 0);
		assert( equipVec.size()%2 == 0);
		assert( favorGoodsVec.size()%2 == 0);

		HonorConvertShop shop;
		shop.iShopId = iShopId;
		shop.iLowLevel =  levelVec[0];
		shop.iHightLevel = levelVec[1];

		for(size_t  ii = 0; ii <  heroSoulVec.size(); ii = ii +2)
		{
			ShopGoodUnit unit;
			unit.iGoodId = heroSoulVec[ii];
			unit.iChance = heroSoulVec[ii +1];
			shop.heroSoulList.push_back( unit); 
			shop.iHeroSoulTotalChance += unit.iChance;

			ShopGoodBaseInfo info;
			bool res = getGoodById(unit.iGoodId,  info);
			assert(res);
		}

		for(size_t  ii = 0; ii < equipVec.size(); ii = ii +2)
		{
			ShopGoodUnit unit;
			unit.iGoodId = equipVec[ii];
			unit.iChance = equipVec[ii + 1];
			shop.equipList.push_back(unit);
			shop.iEquipTotalChance += unit.iChance;

				ShopGoodBaseInfo info;
			bool res = getGoodById(unit.iGoodId,  info);
			assert(res);
		}

		for(size_t ii = 0; ii < favorGoodsVec.size();  ii = ii +2)
		{
			ShopGoodUnit unit;
			unit.iGoodId = favorGoodsVec[ii];
			unit.iChance = favorGoodsVec[ii + 1];
			shop.FavConsumeList.push_back(unit);
			shop.iFavConsumeChance += unit.iChance;
			ShopGoodBaseInfo info;
			bool res = getGoodById(unit.iGoodId,  info);
			assert(res);
		}
	//	assert(shop.iHeroSoulTotalChance > 0);
	//	assert(shop.iEquipTotalChance > 0);
	//	assert(shop.iFavConsumeChance > 0);

	
	
		if(strShopType == "荣誉商店")
		{
			m_HonorShopMap.insert( make_pair(shop.iShopId, shop));
		}
		else if(strShopType == "军团商店")
		{
			m_LegionShopMap.insert( make_pair(shop.iShopId, shop));
		}
		else if(strShopType == "普通商店")
		{
			this->m_normalShopMap.insert(make_pair(shop.iShopId, shop));
		}
		else
		{
			assert(0);
		}
		
	}

	// debug, 打印normalShopData
	/*
#define __SHOP_GOODS_UNIT_LIST_TO_SS(_ss, _name, _goodsList)\
{\
	_ss <<_name<<endl;\
	_ss <<"----------------------------------------------"<<endl;\
	for(size_t i = 0; i < _goodsList.size(); i ++)\
		{\
			const ShopGoodUnit& __goodsUnit = _goodsList[i];\
			_ss	<<" goods id:"<<__goodsUnit.iGoodId\
				<<" chance:"<<__goodsUnit.iChance<<endl;\
		}\
	_ss <<"----------------------------------------------"<<endl<<endl<<endl;\
}

	stringstream ss;
	map<int,HonorConvertShop>::iterator it = this->m_normalShopMap.begin();
	while(it != this->m_normalShopMap.end())
	{
		const HonorConvertShop& shop = it->second;
		ss.clear();
		ss	<<" shop id:"<<shop.iShopId
			<<" low level:"<<shop.iLowLevel
			<<" high level:"<<shop.iHightLevel
			<<" hero soul total chance:"<<shop.iHeroSoulTotalChance
			<<" equip total chance:"<<shop.iEquipTotalChance
			<<" fav consume chance:"<<shop.iFavConsumeChance<<endl;
		__SHOP_GOODS_UNIT_LIST_TO_SS(ss, "hero soul list", shop.heroSoulList);
		__SHOP_GOODS_UNIT_LIST_TO_SS(ss, "equip list", shop.equipList);
		__SHOP_GOODS_UNIT_LIST_TO_SS(ss, "fav consume list", shop.FavConsumeList);
		string debug = ss.str();
		printf(debug.c_str());
		it ++;
	}
#undef __SHOP_GOODS_UNIT_TO_SS
	*/
}

void  ShopFactory::initHonorGoodsData()
{
	ITable *pHonorGoodTable = getCompomentObjectManager()->findTable(TABLENAME_HonorGoods);
	assert( pHonorGoodTable);
	int iHonorGoodRecord = pHonorGoodTable->getRecordCount();
	for(int i = 0; i < iHonorGoodRecord; ++i)
	{
		int iId =  pHonorGoodTable->getInt(i, "商品ID");
		string strType = pHonorGoodTable->getString(i, "商品类型");
		int id = pHonorGoodTable->getInt(i, "ID");
		int iCount = pHonorGoodTable->getInt(i, "商品数量");
		int iLifeAtt = pHonorGoodTable->getInt(i, "消耗类型");
		int iCost = pHonorGoodTable->getInt(i, "商品价格");
		int iLegionLevel = pHonorGoodTable->getInt(i, "最低军团等级开放");
		ShopGoodBaseInfo info;
		info.iId = iId;
		info.iGoodsId = id;
		info.iGoodCount = iCount;
		info.iGoodPrice = iCost;
		info.iCostType = iLifeAtt;
		info.iLegionLevel = iLegionLevel;
		if(strType == "武魂")
		{
			info.iType = enGoodType_HeroSoul;
		}
		else if(strType == "装备")
		{
			info.iType = enGoodType_Equip;
		}
		else if(strType == "喜好品")
		{
			info.iType = enGoodType_FavorConsume;
		}
		else
		{
			assert(0);
		}
		m_GoodsMap.insert(make_pair(info.iId, info));
	}
}


bool ShopFactory::randHeroAndItem(vector<RandHero>& heroVec, vector<RandItem>& itemVec, GSProto::RandomType type , bool bOneHero, bool bIsFrist /*= false*/)
{
	IRandom *pRandom = getComponent<IRandom>(COMPNAME_Random,  IID_IMiniAprRandom);
	assert(pRandom);

	if( type == GSProto::en_RandType_Token_RandOne)
	{
		if(bOneHero)
		{
			RandHero hero;
			bool res = randOneHero(false,hero);
			assert(res);
			heroVec.push_back(hero);
			return true;
		}

		int iTotalTokenChance  = iUseTokenRandHeroChance+ iUseTokenRandItemChance;
		int iRandChance = pRandom->random()%iTotalTokenChance;
		if(iRandChance <= iUseTokenRandHeroChance)
		{
			RandHero hero;
			bool res = randOneHero(false,hero);
			assert(res);
			heroVec.push_back(hero);
		}
		else
		{
			RandItem item;
			bool res = randOneItem(false, item);
			assert(res);
			itemVec.push_back(item);
		}
		return true;
	}
	else if( type == GSProto::en_RandType_Token_RandTen)
	{
		/*RandHero hero;
		bool res = randOneHero(false,hero);
		assert(res);
		heroVec.push_back(hero);*/
		RandItem randSpcialItem;
		bool res0 = randOneWuHun(randSpcialItem);
		assert(res0);
		itemVec.push_back(randSpcialItem);	
		
		int iTotalTokenChance  = iUseTokenRandHeroChance+ iUseTokenRandItemChance;
		for(size_t i = 0; i < 9; ++i)
		{
			int iRandChance = pRandom->random()%iTotalTokenChance;
			if(iRandChance <= iUseTokenRandHeroChance)
			{
				RandHero hero;
				bool res = randOneHero(false,hero);
				assert(res);
				heroVec.push_back(hero);
			}
			else
			{
				RandItem item;
				bool res = randOneItem(false, item);
				assert(res);
				itemVec.push_back(item);
			}
		}
		return true;
	}
	else if( type == GSProto::en_RandType_Gold_RandOne)
	{
		if(bOneHero)
		{
			RandHero hero;
			bool res = randOneHero(true,hero);
			assert(res);
			heroVec.push_back(hero);
			return true;
		}
		int iTotalGoldChance  = iUseGoldRandHeroChance+ iUseGoldRandItemChance;
		int iRandChance = pRandom->random()%iTotalGoldChance;
		if(iRandChance <= iUseGoldRandHeroChance)
		{
			RandHero hero;
			bool res = randOneHero(true,hero);
			assert(res);
			heroVec.push_back(hero);
		}
		else
		{
			RandItem item;
			bool res = randOneItem(true, item);
			assert(res);
			itemVec.push_back(item);
		}
		return true;
	}
	else if( type == GSProto::en_RandType_Gold_RandTen)
	{
		//修改代码
		if(bIsFrist)
		{
			IGlobalCfg* pGlobal  = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
			assert(pGlobal  );

			int iFirstRandTenTemplate = pGlobal->getInt("首次10连抽模板ID", 11);
			
			for(size_t i = 0; i < m_GlodRandTemplate.size(); ++i)
			{
				const RandTemplate& temp = m_GlodRandTemplate[i];
				if(temp.iTmplateId == iFirstRandTenTemplate)
				{
					bool res = randHeroWithTemplate(heroVec, itemVec,temp);
					assert(heroVec.size() + itemVec.size() == 10 );
					return res;
				}
			}
			return false;
		}
		
		int iRandChance = pRandom->random()%iTotalChance;
		for(size_t i = 0; i < m_GlodRandTemplate.size(); ++i)
		{
			const RandTemplate& temp = m_GlodRandTemplate[i];
			iRandChance = iRandChance - temp.iChance;
			if(iRandChance <= 0)
			{
				bool res = randHeroWithTemplate(heroVec, itemVec,temp);
				assert(heroVec.size() + itemVec.size() == 10 );
				return res;
			}
		}
		
		return false;
	}
	else if(GSProto::en_RandType_VIP_RandOne == type)
	{
		int iRandChance = pRandom->random()%m_iVipRandChance;
		for(size_t i = 0; i < m_VipRandList.size(); ++i)
		{
			const VIPRandUnit& temp = m_VipRandList[i];
			iRandChance = iRandChance - temp.iChance;
			if(iRandChance <= 0)
			{
				if(temp.bIsHero)
				{
					RandHero hero;
					hero.iHeroId = temp.iId;
					hero.iHeroLevelStep = temp.iStepLevel;
					heroVec.push_back(hero);
				}
				else
				{
					RandItem item;
					item.iItemId = temp.iId;
					item.iQuality = temp.iQuality;
					item.iCount = temp.iCount;
					itemVec.push_back(item);
				}
				return true;
			}
		}
	}
	else if(GSProto::en_RandType_VIP_RandTen == type)
	{
		//先给个3星英雄
		RandHero hero;
		bool res = randOneVipHero(hero);
		assert(res);
		
		heroVec.push_back(hero);
		//在随机9次
		randVIPHeroWithHeroSoul(heroVec, itemVec, 9);
		return true;
	}

	return false;
}

bool ShopFactory::randOneWuHun(RandItem& item)
{
	IRandom *pRandom = getComponent<IRandom>(COMPNAME_Random,  IID_IMiniAprRandom);
	assert(pRandom);

	int iRandChance = pRandom->random()%m_iTokenWuHunChance;
	
	for(size_t i = 0; i < m_TokenWuHunList.size(); ++i)
	{
		const RandItem &tempItem = m_TokenWuHunList[i];
		iRandChance =  iRandChance - tempItem.iChance;
		if( iRandChance <=0)
		{
			item = tempItem; 
			return true;
		}
	}

	return false;
}

bool ShopFactory::randOneVipHero(RandHero &hero)
{
	IRandom *pRandom = getComponent<IRandom>(COMPNAME_Random,  IID_IMiniAprRandom);
	assert(pRandom);

	int iRandChance = pRandom->random()%m_iVipHeroChance;
	
	for(size_t i = 0; i < m_VipRandList.size(); ++i)
	{
		const VIPRandUnit& temp = m_VipRandList[i];
		if(temp.bIsHero)
		{
			iRandChance = iRandChance - temp.iChance;
			if(iRandChance <= 0)
			{
				if(temp.bIsHero)
				{
					hero.iHeroId = temp.iId;
					hero.iHeroLevelStep = temp.iStepLevel;
					return true;
				}
			}
		}
	}
	return false;	
}

bool ShopFactory::randVIPHeroWithHeroSoul(vector<RandHero>& heroVec, vector<RandItem>& itemVec, int iCount)
{
	IRandom *pRandom = getComponent<IRandom>(COMPNAME_Random,  IID_IMiniAprRandom);
	assert(pRandom);

	for(int i = 0; i < iCount; ++i)
	{
		int iRandChance = pRandom->random()%(m_iVipRandChance-m_iVipHeroChance);
		for(size_t ii = 0; ii < m_VipRandList.size(); ++ii)
		{
			const VIPRandUnit& temp = m_VipRandList[ii];
			//跟策划沟通只出一个三星英雄
			if(temp.bIsHero)
			{
				continue;
			}
				
			iRandChance = iRandChance - temp.iChance;
			if(iRandChance <= 0)
			{
				if(temp.bIsHero)
				{
					RandHero hero;
					hero.iHeroId = temp.iId;
					hero.iHeroLevelStep = temp.iStepLevel;
					heroVec.push_back(hero);
					break;
				}
				else
				{
					RandItem item;
					item.iItemId = temp.iId;
					item.iQuality = temp.iQuality;
					item.iCount = temp.iCount;
					itemVec.push_back(item);
					break;
				}
			}
		}
	}
	return true;
}

bool ShopFactory::randHeroWithTemplate(vector<RandHero>& heroVec, vector<RandItem>& itemVec, const RandTemplate&  temp)
{
	for(size_t i = 0; i < temp.heroVec.size(); ++i)
	{
		const RandTemplateUnit& unit = temp.heroVec[i];
		if(unit.iNum != 0)
		{
			randHeroWithCount(heroVec, unit);
		}
	}

	for(size_t i = 0; i < temp.itemVec.size(); ++i)
	{
		const RandTemplateUnit& unit = temp.itemVec[i];
		if(unit.iNum != 0)
		{
			randItemWithCount(itemVec, unit);
		}
	}

	for(size_t i = 0; i < temp.equipVec.size(); ++i)
	{
		//to do
	}
	return true;
}

bool ShopFactory::randHeroWithCount(vector<RandHero>& heroVec, const RandTemplateUnit& unit)
{
	map<int, vector<RandHero> >::const_iterator stepListIter = m_GoldRandStepHeroList.find( unit.iType);
	assert(stepListIter != m_GoldRandStepHeroList.end());
	
	map<int, int>::const_iterator chanceIter =  m_GoldRandStepChance.find( unit.iType);
	assert(chanceIter != m_GoldRandStepChance.end());

	const vector<RandHero>&  vec = stepListIter->second;
	
	int iTotalChance = chanceIter->second;

	IRandom *pRandom = getComponent<IRandom>(COMPNAME_Random,  IID_IMiniAprRandom);
	assert(pRandom);
	
	for(int i = 0; i < unit.iNum; ++i)
	{
		int iRandChance = pRandom->random()%iTotalChance;
		for(size_t ii = 0; ii < vec.size(); ++ii)
		{
			const RandHero& hero = vec[ii];
			iRandChance -= hero.iChance;
			if(iRandChance <= 0)
			{
				heroVec.push_back(hero);
				break;
			}
		}
	}

	return (int)heroVec.size() == unit.iNum;
	
}

bool ShopFactory::randItemWithCount(vector<RandItem>& itemVec, const RandTemplateUnit& unit)
{
	map<int, vector<RandItem> >::const_iterator qualityListIter = m_GoldRandQualityItemList.find( unit.iType);
	assert(qualityListIter != m_GoldRandQualityItemList.end());
	
	map<int, int>::const_iterator chanceIter =  m_GoldRandQualityChance.find( unit.iType);
	assert(chanceIter != m_GoldRandQualityChance.end());

	const vector<RandItem>&  vec = qualityListIter->second;
	
	int iTotalChance = chanceIter->second;

	IRandom *pRandom = getComponent<IRandom>(COMPNAME_Random,  IID_IMiniAprRandom);
	assert(pRandom);
	
	for(int i = 0; i < unit.iNum; ++i)
	{
		int iRandChance = pRandom->random()%iTotalChance;
		for(size_t ii = 0; ii < vec.size(); ++ii)
		{
			const RandItem& item = vec[ii];
			iRandChance -= item.iChance;
			if(iRandChance <= 0)
			{
				itemVec.push_back(item);
				break;
			}
		}
	}

	return (int)itemVec.size() == unit.iNum;
}


bool ShopFactory::getActorHonorShop(HEntity hEntity, vector<ShopGoodUnit> &vec )
{
	IEntity *pEntity = getEntityFromHandle( hEntity);
	assert(pEntity);
	int iActorLevel = pEntity->getProperty(PROP_ENTITY_LEVEL,0);
	map<int , HonorConvertShop>::iterator iter = m_HonorShopMap.begin();
	for( ; iter != m_HonorShopMap.end(); ++iter)
	{
		const HonorConvertShop &shop = iter->second;
		if( shop.iLowLevel <= iActorLevel && iActorLevel <= shop.iHightLevel)
		{
			randHeroSoul(vec,shop);
			randEquip(vec,shop);
			randFavor(vec,shop);
			return true;
		}
	}
	return false;
}

bool ShopFactory::getNormalShop(HEntity _hEntity, vector<ShopGoodUnit>& _outVec) 
{
	IEntity *actorE = getEntityFromHandle( _hEntity);
	assert(actorE);
	int actorLevel = actorE->getProperty(PROP_ENTITY_LEVEL, 0);
	map<int , HonorConvertShop>::iterator it = this->m_normalShopMap.begin();
	while(it != this->m_normalShopMap.end())
	{
		const HonorConvertShop& shop = it->second;
		if(shop.iLowLevel <= actorLevel && actorLevel <= shop.iHightLevel)
		{
			//randHeroSoul(_outVec, shop);
			//randEquip(_outVec, shop);
			//randFavor(_outVec, shop);
			this->randNormalShopHeroSoul(_outVec, shop);
			this->randNormalShopEquip(_outVec, shop);
			this->randNormalShopFavor(_outVec, shop);
		}
		it ++;
	}
	return false;
}


bool ShopFactory::getActorContributeShop(HEntity hEntity, vector<ShopGoodUnit> &vec )
{
	IEntity *pEntity = getEntityFromHandle( hEntity);
	assert(pEntity);
	int iActorLevel = pEntity->getProperty(PROP_ENTITY_LEVEL,0);
	map<int , HonorConvertShop>::iterator iter = m_LegionShopMap.begin();
	for( ; iter != m_LegionShopMap.end(); ++iter)
	{
		const HonorConvertShop &shop = iter->second;
		if( shop.iLowLevel <= iActorLevel && iActorLevel <= shop.iHightLevel)
		{
			randHeroSoul(vec,shop);
			randEquip(vec,shop);
			randFavor(vec,shop);
			return true;
		}
	}
	return false;
}

const vector<int>& ShopFactory::getNormalShopRefreshTime()
{
	return this->m_normalShopRefreshTime;
}
	

void ShopFactory::randHeroSoul( vector<ShopGoodUnit>& vec, const HonorConvertShop& shop)
{
	IRandom *pRandom = getComponent<IRandom>(COMPNAME_Random,  IID_IMiniAprRandom);
	assert(pRandom);
	
	for(int i = 0 ; i < iRefreshHeroSoulCount; ++i)
	{
		int iRandChance = pRandom->random()%shop.iHeroSoulTotalChance;
		for(size_t ii =0; ii < shop.heroSoulList.size(); ++ii)
		{
			const ShopGoodUnit& unit =  shop.heroSoulList[ii];
			iRandChance -= unit.iChance;
			if(iRandChance<=0)
			{
				vec.push_back( unit);
				break;
			}
		}
	}
}

void ShopFactory::randEquip( vector<ShopGoodUnit> &vec, const HonorConvertShop &shop)
{
	IRandom *pRandom = getComponent<IRandom>(COMPNAME_Random,  IID_IMiniAprRandom);
	assert(pRandom);
	
	for(int i = 0 ; i < iRefreshEquipCount; ++i)
	{
		int iRandChance = pRandom->random()%shop.iEquipTotalChance;
		for(size_t ii =0; ii < shop.equipList.size(); ++ii)
		{
			const  ShopGoodUnit& unit =  shop.equipList[ii];
			iRandChance -= unit.iChance;
			if(iRandChance<=0)
			{
				vec.push_back( unit);
				break;
			}
		}
	}
}

void ShopFactory::randFavor( vector<ShopGoodUnit> &vec, const HonorConvertShop &shop)
{
	IRandom *pRandom = getComponent<IRandom>(COMPNAME_Random,  IID_IMiniAprRandom);
	assert(pRandom);
	
	for(int i = 0 ; i < iRefreshFavorCount; ++i)
	{
		int iRandChance = pRandom->random()%shop.iFavConsumeChance;
		for(size_t ii =0; ii < shop.FavConsumeList.size(); ++ii)
		{
			const  ShopGoodUnit& unit =  shop.FavConsumeList[ii];
			iRandChance -=  unit.iChance;
			if(iRandChance<=0)
			{
				vec.push_back( unit);
				break;
			}
		}
	}
}


void ShopFactory::randNormalShopHeroSoul(vector<ShopGoodUnit>& _vec, const HonorConvertShop& _shop)
{
	
	IRandom *pRandom = getComponent<IRandom>(COMPNAME_Random,  IID_IMiniAprRandom);
	assert(pRandom);
	
	for(int i = 0 ; i < this->m_normalShopRefreshHeroSoulCount; ++i)
	{
		int iRandChance = pRandom->random()%_shop.iHeroSoulTotalChance;
		for(size_t ii =0; ii < _shop.heroSoulList.size(); ++ii)
		{
			const ShopGoodUnit& unit =	_shop.heroSoulList[ii];
			iRandChance -= unit.iChance;
			if(iRandChance<=0)
			{
				_vec.push_back( unit);
				break;
			}
		}
	}
}

void ShopFactory::randNormalShopEquip(vector<ShopGoodUnit>& _vec, const HonorConvertShop& _shop)
{
	IRandom *pRandom = getComponent<IRandom>(COMPNAME_Random,  IID_IMiniAprRandom);
	assert(pRandom);
	
	for(int i = 0 ; i < this->m_normalShopRefreshEquipCount; ++i)
	{
		int iRandChance = pRandom->random()%_shop.iEquipTotalChance;
		for(size_t ii =0; ii < _shop.equipList.size(); ++ii)
		{
			const  ShopGoodUnit& unit =  _shop.equipList[ii];
			iRandChance -= unit.iChance;
			if(iRandChance<=0)
			{
				_vec.push_back( unit);
				break;
			}
		}
	}
}

void ShopFactory::randNormalShopFavor(vector<ShopGoodUnit>& _vec, const HonorConvertShop& _shop)
{
	IRandom *pRandom = getComponent<IRandom>(COMPNAME_Random,  IID_IMiniAprRandom);
	assert(pRandom);
	
	for(int i = 0 ; i < this->m_normalShopRefreshFavorCount; ++i)
	{
		int iRandChance = pRandom->random()%_shop.iFavConsumeChance;
		for(size_t ii =0; ii < _shop.FavConsumeList.size(); ++ii)
		{
			const  ShopGoodUnit& unit =  _shop.FavConsumeList[ii];
			iRandChance -=  unit.iChance;
			if(iRandChance<=0)
			{
				_vec.push_back( unit);
				break;
			}
		}
	}
}


bool ShopFactory::getGoodById(int id , ShopGoodBaseInfo& info)
{
	map<int , ShopGoodBaseInfo>::iterator iter = m_GoodsMap.find(id);
	if( iter == m_GoodsMap.end())
	{
		return false;
	}
	info = iter->second;
	return true;
}

bool  ShopFactory::randOneHero(bool bUseGold, RandHero& randHero)
{
	IRandom *pRandom = getComponent<IRandom>(COMPNAME_Random,  IID_IMiniAprRandom);
	assert(pRandom);
	
	if(bUseGold)
	{
		int randChance =  pRandom->random()%m_iRandHeroUseGoldTotalChance;
		for(size_t i = 0; i < m_RandHeroUseGoldList.size(); ++i)
		{
			RandHero& hero = m_RandHeroUseGoldList[i];
			randChance -=  hero.iChance;
			if(randChance<= 0 )
			{
				randHero = hero;
				return true;
			}
		}
	}
	else
	{
		int randChance =  pRandom->random()%m_iRandHeroUseTokenTotalChance;
		for(size_t i = 0; i < m_RandHeroUseTokenList.size(); ++i)
		{
			RandHero& hero = m_RandHeroUseTokenList[i];
			randChance -=  hero.iChance;
			if(randChance<= 0 )
			{
				randHero = hero;
				return true;
			}
		}
	}

	return false;
}

bool ShopFactory::randOneItem(bool bUseGold,RandItem& randItem)
{
	IRandom *pRandom = getComponent<IRandom>(COMPNAME_Random,  IID_IMiniAprRandom);
	assert(pRandom);
	
	if(bUseGold)
	{
		int randChance =  pRandom->random()%m_iRandItemUseGoldTotalChance;
		for(size_t i = 0; i < m_RandItemUseGoldList.size(); ++i)
		{
			RandItem& item = m_RandItemUseGoldList[i];
			randChance -=  item.iChance;
			if(randChance<= 0 )
			{
				randItem = item;
				return true;
			}
		}
	}
	else
	{
		int randChance =  pRandom->random()%m_iRandItemUseTokenTotalChance;
		for(size_t i = 0; i < m_RandItemUseTokenList.size(); ++i)
		{
			RandItem& item = m_RandItemUseTokenList[i];
			randChance -=  item.iChance;
			if(randChance<= 0 )
			{
				randItem = item;
				return true;
			}
		}
	}
	return false;
}

int ShopFactory::getRefreshCost(int iRefreshTimes)
{
	IGlobalCfg *pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);
	string strRefreshLimit = pGlobalCfg->getString("荣誉兑换刷新消耗元宝上限", "13#200 ");
	vector<int> refreshLimitVec = TC_Common::sepstr<int>(strRefreshLimit, "#");
	assert(refreshLimitVec.size() == 2);
	if( iRefreshTimes > refreshLimitVec[0])
	{
		return refreshLimitVec[1];
	}

	map<int, int>::iterator iter =  m_HonorRefreshMap.find(iRefreshTimes);
	if(iter == m_HonorRefreshMap.end())
	{
		return -1;
	}

	return iter->second;
	
}

