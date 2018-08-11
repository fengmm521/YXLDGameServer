#ifndef ISHOPFACTORY_H_
#define ISHOPFACTORY_H_


enum enGoodType
{
	enGoodType_None  =0,
	enGoodType_HeroSoul = 1,
	enGoodType_Equip = 2,
	enGoodType_FavorConsume = 3,
};

struct ShopGoodBaseInfo{
public:
	int iId; 				//商品Id
	enGoodType iType;        //商品类型
	int iGoodsId;         	//商品在各类型中的ID 
	int iGoodCount;       	//商品数量
	int iCostType ;      	//消耗类型
	int iGoodPrice; 		//商品价格
	int iLegionLevel;
	ShopGoodBaseInfo()
		:iId(0),
		iType(enGoodType_None),
		iGoodsId(0),
		iGoodCount(0),
		iCostType(0),
		iGoodPrice(0),
		iLegionLevel(0)
	{
	}
	virtual ShopGoodBaseInfo& operator= (const ShopGoodBaseInfo info)
		{
			iId = info.iId;
			iType = info.iType;
			iGoodsId = info.iGoodsId;
			iGoodCount = info.iGoodCount;
			iCostType = info.iCostType;
			iGoodPrice = info.iGoodPrice;
			iLegionLevel = info.iLegionLevel;
			
			return  *this;
		}
		ShopGoodBaseInfo(const ShopGoodBaseInfo& info)
		{
			iId = info.iId;
			iType = info.iType;
			iGoodsId = info.iGoodsId;
			iGoodCount = info.iGoodCount;
			iCostType = info.iCostType;
			iGoodPrice = info.iGoodPrice;
			iLegionLevel = info.iLegionLevel;
		}
	
};

struct RandHero;
struct RandItem;
struct ShopGoodUnit
{
	int iGoodId;
	int iChance;
	ShopGoodUnit(): iGoodId(0), 
			iChance(0)
	{
	
	}
	ShopGoodUnit(const ShopGoodUnit& unit)
	{
		iGoodId = unit.iGoodId;
		iChance = unit.iChance;
	}

	virtual ShopGoodUnit& operator=(const ShopGoodUnit& unit)
		{
			iGoodId = unit.iGoodId;
			iChance = unit.iChance;
			return *this;
		}
};


class IShopFactory : public IComponent
{
public:
	virtual bool randHeroAndItem(vector<RandHero>& heroVec, vector<RandItem>& itemVec, GSProto::RandomType type, bool bOneHero, bool bIsFrist = false) = 0;
	virtual bool getActorHonorShop(HEntity hEntity, vector<ShopGoodUnit> &vec )= 0;

	/**
	 * 生成普通商店数据
	 */
	virtual bool getNormalShop(HEntity _hEntity, vector<ShopGoodUnit>& _outVec) = 0;
	
	virtual bool getGoodById(int id , ShopGoodBaseInfo& info)= 0;
	virtual int getRefreshCost(int irefreshTimes) = 0;
	virtual int getShakeCost(int iTimes) = 0;
	virtual int getKnock() = 0;
	virtual bool getActorContributeShop(HEntity hEntity, vector<ShopGoodUnit> &vec ) = 0;

	/**
	 * @brief 获取普通商店刷新时间
	 */ 
	virtual const vector<int>& getNormalShopRefreshTime() = 0;
};

#endif
