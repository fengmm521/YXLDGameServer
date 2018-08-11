#ifndef __FIGHTSOUL_FACTORY_H__
#define __FIGHTSOUL_FACTORY_H__


struct FightSoulData
{
	FightSoulData():iFightSoulID(0), iQuality(0), iTypeID(0), iInitExp(0){}
	int iFightSoulID;
	int iQuality;
	int iTypeID;
	int iInitExp;
	string strName;
	vector<map<int, int> > levelPropList;
};

struct FightSoulChance
{
	FightSoulChance():iFightSoulID(0), iChance(0){}

	int iFightSoulID;
	int iChance;
};

struct FightPractice
{
	FightPractice():iPracticeGrade(0), iType(0), iNextGradeChance(0){}

	int iPracticeGrade;
	int iType;
	vector<FightSoulChance> chanceList;
	int iNextGradeChance;
};

struct FightSoulExchange
{
	FightSoulExchange():iExChangeID(0), iTargetBaseID(0), iNeedChipCount(0){}

	int iExChangeID;
	int iTargetBaseID;
	int iNeedChipCount;
};

// ÐÞÁ¶Ä£Ê½
enum
{
	en_practiceMode_Normal,
	en_practiceMode_Advance,
};

class FightSoulFactory:public ComponentBase<IFightSoulFactory, IID_IFightSoulFactory>
{
public:

	friend class FightSoul;

	FightSoulFactory();
	~FightSoulFactory();

	// IComponent Interface
	virtual bool initlize(const PropertySet& propSet);

	// IFightSoulFactory interface
	virtual IFightSoul* createFightSoul(int iFightSoulID);
	virtual IFightSoul* createFightSoulFromDB(const ServerEngine::FightSoulItem& fightSoulData);
	virtual const map<int, int>* getEffectMap(int iFightSoulID, int iLevel);
	int getNextGradeChance(int iGrade, int iType);
	const FightPractice* querFightPracticeCfg(int iGrade, int iType);
	const FightSoulExchange* queryExchange(int iExchangeID);
	int calcPracticeSilver(int iGrade);
	int calcEquipLimitCount(int iLevel);
	int getMaxExpByQuality(int iQuality);
	int calcLevel(int iFightSoulID, int iExp);
	
private:
	
	int getLevelNeedExp(int iFightSoulID, int iLevel);
	void loadFightSoul();
	void loadQualityExp();
	void loadPractice();
	void loadPracticeSilverCost();
	void loadHeroEquipLimieMap();
	void loadFightSoulExchange();
	
	bool parsePropMap(const string& strValue, map<int, int>& propMap);
	bool parseChaceList(const string& strChanceList, vector<FightSoulChance>& chanceList);
	const FightSoulData* queryFightSoulData(int iFightSoulID);
	
	
private:

	typedef map<int, FightSoulData> MapFightSoulData;
	MapFightSoulData m_mapFightSoulData;

	typedef map<int, vector<int> > MapQualityExpTable;
	MapQualityExpTable m_tbQualityExp;

	typedef map<Int64, FightPractice> MapPractice;
	MapPractice m_mapPractice;

	map<int, int> m_silverCostMap;

	map<int, int> m_heroEquipLimitMap;

	map<int, FightSoulExchange> m_exchangeMap;
};



#endif
