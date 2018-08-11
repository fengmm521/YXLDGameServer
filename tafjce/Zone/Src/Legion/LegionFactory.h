#ifndef __LEGION_FACTORY_H__
#define __LEGION_FACTORY_H__

#include "Legion.h"


class LegionFactory;
class GetLegionListCallback:public ServerEngine::LegionPrxCallback
{
public:

	GetLegionListCallback(LegionFactory* pFactory):m_pLegionFactory(pFactory){}
	virtual void callback_getLegionList(taf::Int32 ret,  const ServerEngine::LegionList& legionList);
    virtual void callback_getLegionList_exception(taf::Int32 ret);

private:

	LegionFactory* m_pLegionFactory;
};

class DelLegionCallback:public ServerEngine::LegionPrxCallback
{
public:
	DelLegionCallback(const string& strName):m_strName(strName){}

	virtual void callback_delLegion(taf::Int32 ret);
    virtual void callback_delLegion_exception(taf::Int32 ret);

private:

	string m_strName;
};


class NewLegionCallback:public ServerEngine::LegionPrxCallback
{
public:

	NewLegionCallback(DelegateCreateLegion cb, const ServerEngine::LegionData& data):m_cb(cb), m_legionData(data){}

	virtual void callback_newLegion(taf::Int32 ret);
	virtual void callback_newLegion_exception(taf::Int32 ret);

private:

	DelegateCreateLegion m_cb;
	ServerEngine::LegionData m_legionData;
};


class GetLegionDataCallback:public ServerEngine::LegionPrxCallback
{
public:

	GetLegionDataCallback(LegionFactory* pLegionFactory, const string& strName, const vector<string>& list):m_pLegionFactory(pLegionFactory), m_strLegionName(strName), m_remainList(list){}
	virtual void callback_getLegion(taf::Int32 ret,  const std::string& strValue);
    virtual void callback_getLegion_exception(taf::Int32 ret);

private:

	LegionFactory* m_pLegionFactory;
	string m_strLegionName;
	vector<string> m_remainList;
};


class GlobalLegionDataSaveCallback:public ServerEngine::LegionPrxCallback
{
public:
	virtual void callback_setGlobalData(taf::Int32 ret);
    virtual void callback_setGlobalData_exception(taf::Int32 ret);
};

struct LegionLevelInfo
{
	LegionLevelInfo():iLevel(0), iNeedExp(0), iLeaderLimit(0), iViceLeaderLimitt(0), iMemLimit(0){}

	int iLevel;
	int iNeedExp;
	int iLeaderLimit;
	int iViceLeaderLimitt;
	int iMemLimit;
};

struct LegionContributeCfg
{
	LegionContributeCfg():iID(0), iCostSilver(0), iCostGold(0), iGetContribute(0){}

	int iID;
	int iCostSilver;
	int iCostGold;
	int iGetContribute;
};

struct LegionBlessCfg
{
	LegionBlessCfg():iID(0), iCostSilver(0), iCostGold(0), iGetContribute(0), iGetPhystrength(0){}
	int iID;
	int iCostSilver;
	int iCostGold;
	int iGetContribute;
	int iGetPhystrength;
};

struct LegionPayParam
{
	LegionPayParam():iMinRank(0), iMaxRank(0), iParam(0){}

	int iMinRank;
	int iMaxRank;
	int iParam;
};

struct ShopItemCfg
{
	ShopItemCfg():iType(0), iDropTypeID(0), iCount(0), iHeroLevelStep(0), iChance(0){}

	int iType;
	int iDropTypeID;
	int iCount;
	int iHeroLevelStep;
	int iChance;
};


struct LegionShopCfg
{
	LegionShopCfg():iID(0){}

	int iID;
	ShopItemCfg awardItem;
};



struct GiftCfg
{
	GiftCfg():iGiftID(0){}
	int iGiftID;
	vector<int> dropIDList;
};

struct LegionCityCfg;

class LegionCampBattle;
class LegionFactory:public ComponentBase<ILegionFactory, IID_ILegionFactory>, public ITimerCallback
{
public:

	LegionFactory();
	~LegionFactory();

	// IComponent Interface
	virtual bool initlize(const PropertySet& propSet);

	// ILegionFactory Interface
	virtual ILegion* findLegion(const string& strLegionName);
	virtual ILegion* getActorLegion(const string& strActorName);
	virtual bool newLegion(HEntity hCreater, const string& strLegionName, DelegateCreateLegion cb, int iLegionIconId);
	virtual bool isReady();
	virtual void cleanupSave();
	virtual void gmStartFight();
	virtual void updateActorInfo(HEntity hActor);
	virtual void actorLogionOut(HEntity hActor,bool bIsOut);

	virtual int GMOnlineGetLegionMemberNameList(string strLegionName, string &strJson);

	// ITimerCallback
	virtual void onTimer(int nEventId);

	void checkCleanLegion();

	//void randLegionShop(vector<int>& shopItemList, int  iSize);

	void fillNewLegionData(ServerEngine::LegionData& data, HEntity hCreater, const string& strLegionName, int iLegionIconId);
	void createLegion(const ServerEngine::LegionData& data);
	void initLegionList(const vector<string>& legionList);
	void setReady();

	void sendLegionPage(HEntity hActor, int iPageIndex);
	void resortRank();
	void loadLegionLevel();
	void loadLegionContribute();
	void loadLegionBless();
	void loadLegionPayParam();
	//void loadLegionShop();
	void loadGiftCfg();
	GSProto::FightAwardHero* fillToAward(const LegionShopCfg* pAward, GSProto::FightAwardResult& awardResult);
	bool isLegionCityBattle();
	
	const LegionLevelInfo* queryLevelInfo(int iLevel);
	const LegionContributeCfg* queryContributeCfg(int ContriID);
	const LegionBlessCfg* queryBlessCfg(int iBlessID);
	void fillLegionListItem(const string& strActorName, Uint32 dwLegionHandle, GSProto::LegionListItem* pListItem);
	int getPayParam(int iRank);
	//const LegionShopCfg* queryShopCfg(int iShopID);
	
	void addActorNameMap(const string& strName, Uint32 dwLegionHandle);
	void removeActorNameMap(const string& strName);
	void removeLegion(ILegion* pLegion);
	void initlizeLegionCity();
	void saveGlobalCityData(bool bSync);
	const LegionCityCfg* queryCityCfg(int iCityID);
	void initCityObj(const ServerEngine::GlobalLegionCityData& globalCityData, const LegionCityCfg& cityCfg);
	void makeCityInitData(const LegionCityCfg& cityCfg, ServerEngine::LegionCity& newCityDaya);
	void sendQueryBattleMsg(HEntity hActor);
	void sendCityOwnerInfo(HEntity hActor);
	int getReportCity(const string& strLegionName);
	int getJoinCity(const string& strActrorName);
	LegionCampBattle* getCampBattle(int iCityID);
	//void sendLastBattleList(HEntity hActor);
	//void sendSingleCityLastBattle(HEntity hActor, int iCityID);
	void sendQueryLegionReportInfo(HEntity hActor, int iQueryCityID);
	void reportFightCity(HEntity hActor, int iCityID);
	void onCheckLegionCityFight();
	int getFightWaitTime(bool noCheckToday);
	void sendCurBattlePage(HEntity hActor);
	void sendSelfBattleRecord(HEntity hActor);
	void sendCurHonorRank(HEntity hActor);
	void sendHisHonorRank(HEntity hActor, int iCityID);
	void sendHisBattleRecord(HEntity hActor, int iCityID, int iPage);
	void sendHisSelfBattleRecord(HEntity hActor, int iCityID);
	const GiftCfg* queryGiftCfg(int iGiftID);
	int getOwnCityCount(const string& strLegionName);
	int getDomainDayPay(const string& strLegionName);

//add by hyf
public:

	void addLegionContribute(HEntity hActor, int iValue);
	void onSaveThreeDayContribute();
	void updateActorLevelInfo(HEntity hActor);

	void fillMail(string& struuid, ServerEngine::MailData & mailData, int iCanContribute, int iBroadId);
	void sendMail(const ServerEngine::LegionMemberItem& item, int iBroadId);
private:

	void _destroy(ILegion* pLegion);

private:
//add for legionCampBattle
 	void fillCampListInfo(HEntity hEntity, GSProto::CMD_QUERY_LEGION_CAMPBATTLE_SC& scMsg);
	void getCampBattleDetail(int & iStaus, int & iRemaindSecond);
	void timeResetCampReport();
	
	int getLegionReportCampId(HEntity hEntity);

	unsigned int getCityBattleStatus(){ return m_iCityFightStaus;}

	void cleanCityDataBeforeReport();

	void pairCityBattle();
	
	void cityBattleOver();
	
public:
	int getCampBattleStatus(int & iRemaindSecond );
	void onQueryCampBattleInfo(HEntity hActor, int iCityID);
	void onJoinCampBattle(HEntity hActor, int iCityID);
	void onEnterCampBattle(HEntity hActor, int iCampId, string strVsLegionName);
	void onCampMemberBattle(HEntity hActor,int iCampId, int iBattleId ,string strMemName, int iStar);
	int iGetLegionMemCount(string strLegionName);
	
	int iCityBattleAddPercent();


	void updateActorFightValue(HEntity hEntity);

	void onQueryLastFightLog(HEntity hActor, string strActorLegionName);
	void onQueryLastFightSimpleLog(HEntity hActor, string strActorLegionName, int iCampId);
	void onQueryLastFightDetailLog(HEntity hActor, string strAttLegionName,int iCampId, string strActorLegionname);
	void onQueryCurFightCondition(HEntity hActor, int iCampId);
	void onQuerycurFightDetailLog(HEntity hActor, string strAttLegionName,int iCampId, string strActorLegionname);

	void addFightLogInBattle(int iCampId, string attLegionName, string strGuardLog, string strAttLog, string fightBaseLog,string strGuardName,bool addGuardLog);
	void addFightLogInBattleOver(string& strLegionName, int iCampId, string& strLog);
	void setLegionGuard(int iCampId, string strLegionName);
	void openCampBattle(int iReportBegin,int iBattleBegin, int iBattleOver);

	bool bLegionInCampBattle(string strLegionName);

	
private:

	map<string, ILegion*> m_mapLegion;
	map<string, Uint32> m_mapActorName2Legion;
	bool m_bReady;

	vector<Uint32> m_sortLegionList; // 军团名字
	map<int, LegionLevelInfo> m_legionLevelMap;
	map<int, LegionContributeCfg> m_legionContributeMap;
	map<int, LegionBlessCfg> m_legionBlessMap;
	vector<LegionPayParam> m_legionPayParamList;
	
	// 军团商店
	map<int, LegionShopCfg> m_shopItemMap;

	// 军团清理
	ITimerComponent::TimerHandle m_hCheckHandle;

	// 军团领地争夺战
	map<int, LegionCityCfg> m_legionCityCfgMap;
	map<int, LegionCampBattle*> m_legionCampBattleMap;
	map<string, int> m_ReportLegionMap;
	ServerEngine::CityCampFightLog m_cityCampFightLog;
	ServerEngine::CityCampFightLog m_curCityFightLog;
	int iHisDayNO;
	
	ITimerComponent::TimerHandle m_hGlobalSaveHanle;
	ITimerComponent::TimerHandle m_hCheckFightHandle;
	unsigned int m_iCityFightStaus;
	int m_iLastCityFightDay;

	// 礼包配置
	map<int, GiftCfg> m_mapGiftCfg;

	//计算三天贡献
	ITimerComponent::TimerHandle m_hSaveThreeContributeHandle;


	int	m_iReportBeginSecond;
	int	m_iFightBeginSecond ;
	int	m_iFightEndSecond ;

};


#endif

