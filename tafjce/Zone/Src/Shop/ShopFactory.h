#ifndef SHOPFACTORY_H_
#define SHOPFACTORY_H_


struct RandHero
{
	int iHeroId;
	int iHeroLevelStep;
	int iChance;

	RandHero(): iHeroId(0), 
		iHeroLevelStep(0),
		iChance(0)
	{
	
	}
	RandHero(const RandHero& hero)
	{
		iHeroId = hero.iHeroId;
		iHeroLevelStep = hero.iHeroLevelStep;
		iChance = hero.iChance;
	}

	virtual RandHero& operator= (const RandHero& hero)
		{
			iHeroId = hero.iHeroId;
			iHeroLevelStep = hero.iHeroLevelStep;
			iChance = hero.iChance;
			
			return	*this;
		}
};

struct RandItem
{
	int iItemId;
	int iChance;
	int iCount;
	int iQuality; //只做区分不做使用，
	
	RandItem(): iItemId(0), 
			iChance(0),
			iCount(0),
			iQuality(-1)
	{
	
	}
	RandItem( const RandItem& item)
	{
		iItemId = item.iItemId;
		iChance = item.iChance;
		iCount = item.iCount;
		iQuality = item.iQuality;
	}

	virtual RandItem& operator=(const RandItem& item)
		{
			iItemId = item.iItemId;
			iChance = item.iChance;
			iCount = item.iCount;
			iQuality = item.iQuality;
			
			return *this;
		}
};


struct HonorConvertShop
{
	int iShopId;
	int iLowLevel;
	int iHightLevel;
	vector<ShopGoodUnit> heroSoulList;
	vector<ShopGoodUnit> equipList;
	vector<ShopGoodUnit> FavConsumeList;
	int iHeroSoulTotalChance;
	int iEquipTotalChance;
	int iFavConsumeChance;

	HonorConvertShop()
		:iShopId(0),
		iLowLevel(0),
		iHightLevel(0),
		iHeroSoulTotalChance(0),
		iEquipTotalChance(0),
		iFavConsumeChance(0)
	{
	}

	HonorConvertShop(const HonorConvertShop& shop)
	{
		iShopId = shop.iShopId;
		iLowLevel = shop.iLowLevel;
		iHightLevel = shop.iHightLevel;
		heroSoulList = shop.heroSoulList;
		equipList = shop.equipList;
		FavConsumeList = shop.FavConsumeList;
		iHeroSoulTotalChance = shop.iHeroSoulTotalChance;
		iEquipTotalChance = shop.iEquipTotalChance;
		iFavConsumeChance = shop.iFavConsumeChance;
	}

	virtual HonorConvertShop& operator=(const HonorConvertShop& shop)
		{
			iShopId = shop.iShopId;
			iLowLevel = shop.iLowLevel;
			iHightLevel = shop.iHightLevel;
			heroSoulList = shop.heroSoulList;
			equipList = shop.equipList;
			FavConsumeList = shop.FavConsumeList;
			iHeroSoulTotalChance = shop.iHeroSoulTotalChance;
			iEquipTotalChance = shop.iEquipTotalChance;
			iFavConsumeChance = shop.iFavConsumeChance;
			
			return *this;
		}
	
};

//摇钱树
struct CoinTreeKnock
{
	CoinTreeKnock()
		:iKnockDouble(0),
		iChance(0)
	{
	}
	CoinTreeKnock(const CoinTreeKnock & knock)
	{
		iKnockDouble = knock.iKnockDouble;
		iChance = knock.iChance;
		
	}
	virtual CoinTreeKnock&  operator= (const CoinTreeKnock & knock)
	{
		iKnockDouble = knock.iKnockDouble;
		iChance = knock.iChance;
		return *this;
	}

	int iKnockDouble;
	int iChance;
};

struct CoinTreeShakeCost
{
	int iHightTimes;
	int iLowTimes;
	int iCost;
	CoinTreeShakeCost():iHightTimes(0),iLowTimes(0),iCost(0)
	{
		
	}

	 CoinTreeShakeCost(const CoinTreeShakeCost & cost)
	{
		iHightTimes = cost.iHightTimes;
		iLowTimes = cost.iLowTimes;
		iCost = cost.iCost;
		
	}
	virtual CoinTreeShakeCost& operator= (const CoinTreeShakeCost & cost)
	{
		iHightTimes = cost.iHightTimes;
		iLowTimes = cost.iLowTimes;
		iCost = cost.iCost;
		return *this;
	}
};

struct RandTemplateUnit
{
	int iType;
	int iNum;

	RandTemplateUnit():
		iType(-1),
		iNum(0)
		{
		}

		RandTemplateUnit(const RandTemplateUnit& unit)
		{
			iType = unit.iType;
			iNum = unit.iNum;
		}

		virtual RandTemplateUnit& operator= (const RandTemplateUnit& unit)
		{
			iType = unit.iType;
			iNum = unit.iNum;
			return *this;
		}
	
};

struct RandTemplate
{
	int iTmplateId;
	int iChance;
	vector< RandTemplateUnit > heroVec;
	vector< RandTemplateUnit > itemVec;
	vector< RandTemplateUnit > equipVec;
	RandTemplate():iTmplateId(0),iChance(0)
	{
		heroVec.clear();
		itemVec.clear();
		equipVec.clear();
	}

	virtual ~RandTemplate()
	{
		heroVec.clear();
		itemVec.clear();
		equipVec.clear();
	}

	RandTemplate(const RandTemplate& temp)
	{
		heroVec = temp.heroVec;
		itemVec = temp.itemVec;
		equipVec = temp.equipVec;
		iTmplateId = temp.iTmplateId;
		iChance = temp.iChance;
	}

	virtual RandTemplate& operator=(const RandTemplate& temp)
	{
		heroVec = temp.heroVec;
		itemVec = temp.itemVec;
		equipVec = temp.equipVec;
		iTmplateId = temp.iTmplateId;
		iChance = temp.iChance;
		
		return *this;
	}
};


struct VIPRandUnit
{
	int iId;
	bool bIsHero;
	int iStepLevel;
	int iQuality;
	int iChance;
	int iCount;

	VIPRandUnit():iId(0),bIsHero(false),iStepLevel(0),iQuality(0),iChance(0),iCount(0)
	{
	}

	VIPRandUnit(const VIPRandUnit& unit)
	{
		iId = unit.iId;
		bIsHero = unit.bIsHero;
		iStepLevel = unit.iStepLevel;
		iQuality = unit.iQuality;
		iChance = unit.iChance;
		iCount = unit.iCount;
	}

	virtual VIPRandUnit& operator=(const VIPRandUnit& unit)
	{
		iId = unit.iId;
		bIsHero = unit.bIsHero;
		iStepLevel = unit.iStepLevel;
		iQuality = unit.iQuality;
		iChance = unit.iChance;
		iCount = unit.iCount;
		
		return *this;
	}
};


class ShopFactory : public ComponentBase<IShopFactory,IID_IShopFactory>
{
public:
	ShopFactory();
	virtual ~ShopFactory();
    // IComponent interface
	virtual bool initlize(const PropertySet& propSet);
	virtual bool randHeroAndItem(vector<RandHero>& heroVec, vector<RandItem>& itemVec, GSProto::RandomType type,  bool bOneHero,bool bIsFrist = false);
	virtual bool getActorHonorShop(HEntity hEntity, vector<ShopGoodUnit> &vec );
	virtual bool getNormalShop(HEntity _hEntity, vector<ShopGoodUnit>& _outVec) ;
	virtual bool getGoodById(int id , ShopGoodBaseInfo& info);
	virtual int getRefreshCost(int irefreshTimes);

	virtual int getShakeCost(int iTimes);
	virtual int getKnock();

	virtual bool getActorContributeShop(HEntity hEntity, vector<ShopGoodUnit> &vec );
	
	virtual const vector<int>& getNormalShopRefreshTime();
	
protected: 
	bool loadAllShopData();
	bool randOneHero(bool bUseGold, RandHero& randHero);
	bool randOneItem(bool bUseGold, RandItem& randItem);
	void randHeroSoul( vector<ShopGoodUnit> &vec, const HonorConvertShop &shop);
	void randEquip( vector<ShopGoodUnit> &vec, const HonorConvertShop &shop);
	void randFavor( vector<ShopGoodUnit> &vec, const HonorConvertShop &shop);

	void randNormalShopHeroSoul(vector<ShopGoodUnit>& _vec, const HonorConvertShop& _shop);
	void randNormalShopEquip(vector<ShopGoodUnit>& _vec, const HonorConvertShop& _shop);
	void randNormalShopFavor(vector<ShopGoodUnit>& _vec, const HonorConvertShop& _shop);

	
	void initRandHeroData();
	void initRandItemData();
	void initHonorShopData();
	void initHonorGoodsData();
	void loadCoinTreeData();
	void checkGoldRandData();
	void loadRandHeroTemplateData();

	bool randHeroWithTemplate(vector<RandHero>& heroVec, vector<RandItem>& itemVec, const RandTemplate&  temp);
	bool randHeroWithCount(vector<RandHero>& heroVec, const RandTemplateUnit& unit);
	bool randItemWithCount(vector<RandItem>& itemVec, const RandTemplateUnit& unit);

	void initVipRandData();
	
	bool randVIPHeroWithHeroSoul(vector<RandHero>& heroVec, vector<RandItem>& itemVec, int iCount);
	bool randOneVipHero(RandHero &hero);

	bool randOneWuHun(RandItem& item);
private:
	vector<RandHero> m_RandHeroUseTokenList;
	vector<RandHero> m_RandHeroUseGoldList;
	vector<RandItem> m_RandItemUseTokenList;
	vector<RandItem> m_RandItemUseGoldList;
	map<int , HonorConvertShop> m_HonorShopMap;
	map<int , ShopGoodBaseInfo> m_GoodsMap;
	int m_iRandHeroUseTokenTotalChance;
	int m_iRandHeroUseGoldTotalChance;
	int m_iRandItemUseTokenTotalChance;
	int m_iRandItemUseGoldTotalChance;

	int iUseTokenRandHeroChance;
	int iUseTokenRandItemChance;
	int iUseGoldRandHeroChance;
	int iUseGoldRandItemChance;
	
	int iRefreshHeroSoulCount;
	int iRefreshEquipCount;
	int iRefreshFavorCount;

	int m_normalShopRefreshHeroSoulCount;
	int m_normalShopRefreshEquipCount;
	int m_normalShopRefreshFavorCount;
	map<int, int> m_HonorRefreshMap;

	vector<CoinTreeKnock>  m_CoinTreeKnock;
	int m_CoinTreeKnocKTotalChance;
	vector<CoinTreeShakeCost> m_CoinTreeShakeCost;

	//军团商店
	map<int, HonorConvertShop> m_LegionShopMap;

	map<int, vector<RandHero> > m_GoldRandStepHeroList;
	map<int, int> m_GoldRandStepChance;

	map<int, vector<RandItem> > m_GoldRandQualityItemList;
	map<int, int> m_GoldRandQualityChance;

	vector<RandTemplate> m_GlodRandTemplate;
	int iTotalChance;


	vector<VIPRandUnit> m_VipRandList;
	int m_iVipRandChance;
	int m_iVipHeroChance;

	vector<RandItem> m_TokenWuHunList;
	int m_iTokenWuHunChance;
	
	map<int,HonorConvertShop> m_normalShopMap;	// 普通商店
	vector<int>	m_normalShopRefreshTime;			// 普通商店刷新时间数组(整点,小时)
};

#endif

