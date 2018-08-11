#ifndef __HEROTALLENT_FACTORY_H__
#define __HEROTALLENT_FACTORY_H__


struct HeroConvertCfg
{
	HeroConvertCfg():iSrcHeroID(0){}

	int iSrcHeroID;
	map<int, int> targetChanceList;
	
	//vector<int> targetIDList;
	//vector<int> targetChanceList;
	//int iTotalChance;
};

struct HeroTallentCfg
{
	HeroTallentCfg():iTallentID(0), iLevel(0), iQuality(0), iUpgradeSilver(0), iUpgradeQuality(0), iMasterLv(0){}

	int iTallentID;
	int iLevel;
	int iQuality;

	// 升级条件
	map<int, int> upgradeMaterialMap;
	int iUpgradeSilver;
	//int iUpgradeLevelStep;
	int iUpgradeQuality;
	int iMasterLv;

	// 效果
	vector<int> effectList;
};

// 喜好品消耗
struct FavoriteConsume
{
	map<int, int> favoriteItemList;
};

// 英雄在特定品质的属性加成(累加值)
struct QualityProp
{
	map<int, int> addPropMap;
};


struct FavoriteCombine
{
	FavoriteCombine():iItemID(0), iNeedSilver(0){}

	int iItemID;
	map<int, int> needItemMap;
	int iNeedSilver;
};

class HeroTallentFactory:public ComponentBase<IComponent, IID_IHeroTallentFactory>
{
public:

	// IComponent Interface
	virtual bool initlize(const PropertySet& propSet);
	const HeroConvertCfg* queryConvertCfg(int iHeroID);
	const HeroTallentCfg* queryTallentCfg(int iTallentID, int iLevel);
	
	const FavoriteConsume* queryQualityConsume(int iHeroID, int iQuality);
	const QualityProp* queryQualityPropMap(int iHeroID, int iQuality);
	int queryQualityPropParam(int iQuality);
	const FavoriteCombine* queryFavoriteCombineInfo(int iItemID);

private:

	void loadHeroConvert();
	void loadHeroTallent();

	void loadQualityPropParam();
	void loadQualityConsume();
	void loadFavoriteCombine();

	void parseItemPropMap(const vector<int>& itemList, map<int, int>& addPropMap);

private:

	map<int, HeroConvertCfg> m_mapConvertCfg;
	map<int, map<int, HeroTallentCfg> > m_mapTallentCfg;
	
	map<int, map<int, FavoriteConsume> > m_mapQualityUpgradeCfg;
	map<int, map<int, QualityProp> > m_mapQualityPropCfg;
	map<int, FavoriteCombine> m_mapFavoriteCombineCfg;
	map<int, int> m_mapPropParamCfg;
};



#endif

