#ifndef __ITEM_FACTORY_H__
#define __ITEM_FACTORY_H__

typedef Pair<int, int>			XilianPropItem;
typedef vector<XilianPropItem>	XilianPropVec;

struct EquipProp
{
	map<int, int> baseProp;
	map<int, int> specialProp;
};

struct EquipChipCombine
{
	EquipChipCombine():iEquipChipID(0), iNeedCount(0), iCreateEquipID(0), iBaseCostSilver(0){}

	int iEquipChipID;
	int iNeedCount;
	int iCreateEquipID;
	int iBaseCostSilver;
};


struct EquipSuit
{
	int iSuitID;
	vector<int> equipIDList;
	vector<map<int, int> > suitPropList;
};


struct EquipRandomPropItem
{
	int iPropID;
	int iMinValue;
	int iMaxValue;
	int iChance;
};

struct ConvertCodeItem
{
	int iDropId;
	bool bFlag;  //bFlag == 1 , 这一个类型的兑换码玩家可以多次使用
};

class ItemFactory:public ComponentBase<IItemFactory, IID_IItemFactory>
{
public:

	ItemFactory();
	~ItemFactory();

	// IComponent interface
	virtual bool initlize(const PropertySet& propSet);

	// IItemFactory Interface
	virtual const PropertySet* getItemPropset(int iItemID);
	virtual bool createItemFromID(int iItemID, int iCount, vector<HItem>& itemList);
	virtual IItem* createItemFromDB(const ServerEngine::ItemData& itemSaveData);
	virtual const map<int, int>* queryFavoriteProp(int iItemID);
	virtual int getShowLifeAtt(int iPropID);

	const EquipProp* getEquipFightProp(int iEquipID);
	const EquipChipCombine* queryEquipChipCombine(int iEquipChipID);

	int querySuitIDFromEquip(int iEquipID);
	const EquipSuit* querySuitCfg(int iSuitID);
	void parseSuitProp(const string& strDesc, map<int, int>& propMap);

	virtual int getDropIdByConvertGiftId(const string& strConvertGiftId);
	void fillEquipRandPropCfg(int iGroupID, vector<EquipRandomPropItem>& randomPropList, const vector<string>& connfigList);
	bool randomEquipProp(int iGroupID, int& iPropID, int& iValue);

	virtual bool bActorCanRepeatGetFlag(const string& strConvertGiftId);

	/**
	  * @brief 获取洗练等级限制
	  */
	virtual int getXilianLvLimit();

private:
	void loadGlobal();
	void loadItem();
	void loadEquip();
	void loadEquipCombine();
	void loadEquipSuit();
	void loadEquipRandom();
	void parseEquipProp(const string& strPopDesc, map<int, int>& propMap);
	void loadRandomPropShow();

	void loadConvertCode();

private:

	typedef std::map<Int32, PropertySet> MapItemStaticProp;
	MapItemStaticProp	m_itemStaticPropMap;

	typedef std::map<Int32, EquipProp> MapEquipProp;
	MapEquipProp m_equipPropMap;

	map<Int32, EquipChipCombine> m_mapEquipChipCombine;

	map<Int32, EquipSuit> m_mapEquipSuitCfg;
	map<int, int> m_equipSuitIDMap;

	map<int, map<int, int> > m_favoritePropCfg;

	map<string, ConvertCodeItem>  m_ConvertCodeMap;

	map<int, vector<EquipRandomPropItem> > m_equipRandomPropCfgMap;

	map<int, int> m_randomPropLifeAttMap;

	int m_xilianLvLimit;	// 洗练等级限制
};



#endif
