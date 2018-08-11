#ifndef __MANOR_FACTORY_H__
#define __MANOR_FACTORY_H__

struct ManorResUnit
{
	ManorResUnit():iResId(0),
		resType(GSProto::en_ManorResType_NONE),
		iLevel(0),
		iProductRate(0),
		iLevelUpCost(0),
		iLevelUpSecond(0),
		iResTotalProduct(0),
		strName("")
	{
	}
	
	ManorResUnit(const ManorResUnit& unit)
	{
		iResId = unit.iResId;
		resType = unit.resType;
		iLevel = unit.iLevel;
		iProductRate = unit.iProductRate;
		iLevelUpCost = unit.iLevelUpCost;
		iLevelUpSecond = unit.iLevelUpSecond;
		iResTotalProduct = unit.iResTotalProduct;
		strName = unit.strName;
	}

	ManorResUnit& operator=(const ManorResUnit& unit)
	{
		iResId = unit.iResId;
		resType = unit.resType;
		iLevel = unit.iLevel;
		iProductRate = unit.iProductRate;
		iLevelUpCost = unit.iLevelUpCost;
		iLevelUpSecond = unit.iLevelUpSecond;
		iResTotalProduct = unit.iResTotalProduct;
		strName = unit.strName;
		return *this;
	}
	
	int iResId;
	GSProto::ManorResType resType;
	int iLevel;
	int iProductRate;
	int iLevelUpCost;
	int iLevelUpSecond;
	int iResTotalProduct;
	string strName;
};

struct ManorItem
{
	ManorItem()
	:iId(0),
	iCount(0),
	iChance(0)
	{
		
	}
	int iId;
	int iCount;
	int iChance;
	ManorItem(const ManorItem& item)
	{
		iId = item.iId;
		iCount = item.iCount;
		iChance = item.iChance;
		
	}
    inline ManorItem&  operator= (const ManorItem& item)
	{
		iId = item.iId;
		iCount = item.iCount;
		iChance = item.iChance;
		return *this;
	}
	
};

//铁匠铺更武魂殿结构相同
struct HeroSoulOrItem
{
	int iLevel;
	int iLevelUpCost;
	int iLevelUpSecond;
	int iProductSecond;
	vector<ManorItem> manorItemList;
	int iTotalChance;
	int iOpenGeZiCount;
	
	HeroSoulOrItem():
		iLevel(0),
		iLevelUpCost(0),
		iLevelUpSecond(0),
		iProductSecond(0),
		iTotalChance(0),
		iOpenGeZiCount(0)
	{
	}

	virtual ~HeroSoulOrItem()
	{
	}
	
	
	HeroSoulOrItem(const HeroSoulOrItem& item)
	{
		iLevel = item.iLevel;
		iLevelUpCost = item.iLevelUpCost;
		iLevelUpSecond = item.iLevelUpSecond;
		iProductSecond = item.iProductSecond;
		manorItemList = item.manorItemList;
		iTotalChance = item.iTotalChance;
		iOpenGeZiCount = item.iOpenGeZiCount;
	}

	inline HeroSoulOrItem&  operator=(const HeroSoulOrItem& item)
	{
		iLevel = item.iLevel;
		iLevelUpCost = item.iLevelUpCost;
		iLevelUpSecond = item.iLevelUpSecond;
		iProductSecond = item.iProductSecond;
		manorItemList = item.manorItemList;
	  	iTotalChance = item.iTotalChance;
		iOpenGeZiCount = item.iOpenGeZiCount;
		return *this;
	}

	void randManorItem(vector<ManorItem>& itemList, int iCount)
	{
		IRandom *pRand = getComponent<IRandom>(COMPNAME_Random, IID_IMiniAprRandom);
		assert(pRand);
		
		for(int i = 0; i < iCount; ++i)
		{
			int iChance = pRand->random()%iTotalChance;
			for(size_t ii = 0; ii < manorItemList.size(); ++ii )
			{
				ManorItem& item = manorItemList[ii];
				iChance -= item.iChance;
				if(iChance <= 0)
				{
					itemList.push_back(item);
					break;
				}
			}
		}
	}
};

struct ManorRes
{
	int iResId;
	GSProto::ManorResType iResType;
	int iOpenLevel;
	
	ManorRes()
		:iResId(0),
		iResType(GSProto::en_ManorResType_NONE),
		iOpenLevel(0)
	{
	}

	ManorRes(const ManorRes& manorRes)
	{
		iResId = manorRes.iResId;
		iResType = manorRes.iResType;
		iOpenLevel = manorRes.iOpenLevel;
	}

	virtual void operator=(const ManorRes& manorRes)
	{
		iResId = manorRes.iResId;
		iResType = manorRes.iResType;
		iOpenLevel = manorRes.iOpenLevel;
	}
};

struct CRefreshCost
{
	int iLow;
	int iHight;
	int iCost;
	CRefreshCost():iLow(0),iHight(0),iCost(0)
	{
	}
	virtual ~CRefreshCost(){}
};

struct ManorProtect
{
	int iProtectId;
	int iPrice;
	int iBuycdSecond;
	int iProtectSecond;

	ManorProtect()
		:iProtectId(0),
		iPrice(0),
		iBuycdSecond(0),
		iProtectSecond(0)
	{
		
	}

	ManorProtect(const ManorProtect& unit)
	{
		iProtectId = unit.iProtectId;
		iPrice = unit.iPrice;
		iBuycdSecond = unit.iBuycdSecond;
		iProtectSecond = unit.iProtectSecond;
	}

	virtual ManorProtect& operator=(ManorProtect& unit)
	{
		iProtectId = unit.iProtectId;
		iPrice = unit.iPrice;
		iBuycdSecond = unit.iBuycdSecond;
		iProtectSecond = unit.iProtectSecond;
		return *this;
	}
	
};

class ManorFactory:public ComponentBase<IManorFactory, IID_IManorFactory>,public ITimerCallback
{	
public:
	ManorFactory();
	virtual ~ManorFactory();
public: 
	virtual bool initlize(const PropertySet& propSet);
public:
	virtual bool getActorOpenSilverResList(vector<ManorRes>& openResVec,HEntity hEntity);
	virtual bool getActorOPenHeroExpResList(vector<ManorRes>& openResVec,HEntity hEntity);
	virtual bool getActorSilverResByIdAndLevel(ManorResUnit& unit, int ResId, int iResLevel);
	virtual bool getActorHeroExpResByIdAndLevel(ManorResUnit& unit, int ResId, int iResLevel);
	virtual bool getActorWuHunDianByLevel(HeroSoulOrItem& unit, int iLevel);
	virtual bool getActorTieJiangPuByLevel(HeroSoulOrItem& unit, int iLevel); 
	virtual void getWuHunByLevel(vector<ManorItem>& wuHunList, int iCount, int level);
	virtual void getTieJiangPuByLevel(vector<ManorItem>& tieJiangPuList, int iCount, int level);

	virtual void onTimer(int nEventId);
	void loadMachine();
	void resetMachine();
	void loadManorProtect();

	virtual int getRefreshTieJiangPuOrWuHunDianCost(int iNowRefreshTimes);

	virtual  bool getManorProtectById(int id, ManorProtect& protect);
private:
	typedef map<int, map<int, ManorResUnit> > ResDetailMap;
	ResDetailMap m_SilverResMap;
	ResDetailMap m_HeroExpResMap;

	map<int, HeroSoulOrItem> m_WuHunDianMap;
	map<int, HeroSoulOrItem> m_TieJaingPuMap;
	
	typedef vector<ManorRes> ManorResVec;
	ManorResVec m_ManorSilverRes;
	ManorResVec m_ManorHeroExpRes;

	//vector<int> m_WuHunDianOpenLevelList;
	//vector<int> m_TieJiangPuOpenLevelList;

	ITimerComponent::TimerHandle m_LoadMachineTimerHandle;
	ITimerComponent::TimerHandle m_ResetMachineTimerHandle;

	vector<HEntity> m_machinePlayer;
	unsigned int dwChgTime;


	vector<CRefreshCost*> m_RefreshCostList;

	map<int, ManorProtect> m_ManorProtect;
};


#endif
