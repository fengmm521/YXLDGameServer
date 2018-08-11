#ifndef __LEGION_SSYSTEM_H__
#define __LEGION_SSYSTEM_H__

#include "ILegionSystem.h"


struct LegionShopCfg;
struct ShopGoodBaseInfo;
struct ShopGoodUnit;

class LegionSystem:public ObjectBase<ILegionSystem>, public Detail::EventHandle, public ITimerCallback
{
public:

	LegionSystem();
	~LegionSystem();

	// ILegionSystem Interface
	virtual string addGift(int iGiftID);
	virtual string addGift(const ServerEngine::CustomGiftData& customGift);
	virtual void contributeShopRefreshConfirm() ;
	virtual void confirmLeaveLegion();
	// ITimerCallback Interface
	virtual void onTimer(int nEventId);

	// IEntitySubSystem  Interface
	virtual Uint32 getSubsystemID() const;
	virtual Uint32 getMasterHandle();
	virtual bool create(IEntity* pEntity, const std::string& strData);
	virtual bool createComplete();
	virtual const std::vector<Uint32>& getSupportMessage();
	virtual void onMessage(QxMessage* pMessage);
	virtual void packSaveData(string& data);

	void onReqClickLegion(const GSProto::CSMessage& msg);
	void dispatchLegion(const GSProto::CSMessage& msg);
	void onReqQueryLegionList(const GSProto::CSMessage& msg);
	void onReqAppLegion(const GSProto::CSMessage& msg);
	void onReqCreateLegion(const GSProto::CSMessage& msg);
	void onReqOpenShop(const GSProto::CSMessage& msg);
	void onReqRefreshShop(const GSProto::CSMessage& msg);
	//void onReqGiveupShp(const GSProto::CSMessage& msg);
	void onReqSelectShop(const GSProto::CSMessage& msg);
	void onReqCancelApp(const GSProto::CSMessage& msg);
	void onReqQueryOtherLegion(const GSProto::CSMessage& msg);
	void onReqQueryCommBattle(const GSProto::CSMessage& msg);
	void onReqQueryGift(const GSProto::CSMessage& msg);
	void onReqGetGift(const GSProto::CSMessage& msg);
	void onReqLeave(const GSProto::CSMessage& msg);

	ServerEngine::TimeResetValue& getBlessData(){return m_blessData;}
	ServerEngine::TimeResetValue& getDayAwardData(){return m_dayAwardFlag;}
	ServerEngine::TimeResetValue& getWeekAwardData(){return m_weekAwardFlag;}
	
	
	void onEventBeginFight(EventArgs& args);
	
	//void fillScShopItem(const ServerEngine::LegionShopItem& shopItemData,  const LegionShopCfg* pShopCfg, GSProto::LegionShopItem* pScShopItem);
	

	bool initData(const ServerEngine::LegionSystemData& data);
	//void fillShopPage(GSProto::ShopPage* pPage);
	void refreshShop();
	//int randShopPos();
	void checkGiftTimeout();
	void onEventBossFightBegin(EventArgs& args);
	void onEventCityFightBegin(EventArgs& args);
	//int getBuySize();
//	void randShopSort(int iReqPos, int iAwardPos);
	void fillCustomBody(const ServerEngine::CustomGiftData& customData, GSProto::GiftCustomBody* pScCustom);
	void doGift(int iGiftID);
	void doCustomGift(const ServerEngine::CustomGiftData& customData);

	void onReqQueryWorldBoss(const GSProto::CSMessage& msg);
	void onReqCheerWorldBoss(const GSProto::CSMessage& msg);
	void onReqFightWorldBoss(const GSProto::CSMessage& msg);
	void onReqWBFightRelive(const GSProto::CSMessage& msg);

	bool checlWorldBossLevelCond();
	bool checkLegionCond();
	void notiifyShopHero(GSProto::FightAwardHero* pHeroAward);

	//add by hyf 
	void checkGiftNotice();
	void onEventPropertyChg(const EventArgs& args);
	void updateActorLevelInfo(const EventArgs& args);
	void fillContributeShopDetail(int iLegionLevel , google::protobuf::RepeatedPtrField< ::GSProto::GoodDetail >* pszGoodDetail);
	void sendContributeInfo(bool bHaveRefresh = false);

	void addJionLegionTimes(int iTimes);
	ServerEngine::TimeResetValue& getWorshipTimes();
	map<string,unsigned int >& getActorHaveWorshipList();
	//void addBeWorshipContribute(int iContribute);
	//int getCanGetBeWorshipContribute() { return m_beWorshipGetContribute;}
	ServerEngine::TimeResetValue& getContributeTimes() { return getDayResetValue( m_contributeTimes);}

	virtual void gmAddLegionExp(int iExp) ;

	void checkNotice();

	void onEventFightValueChg(const EventArgs& args);

	virtual void setLegionGuard(int iCampId, string strLegionName);
	virtual void openCampBattle(int iReportBegin,int iBattleBegin, int iBattleOver);
private:

	HEntity m_hEntity;
	//vector<int> m_shopList;
	//set<int> m_buyList;
	
	vector<ServerEngine::LegionShopItem> m_shopList;
	map<string, ServerEngine::GiftData> m_giftList;
	ServerEngine::TimeResetValue m_blessData;
	ServerEngine::TimeResetValue m_dayAwardFlag;
	ServerEngine::TimeResetValue m_weekAwardFlag;
	
	ITimerComponent::TimerHandle m_hCheckHandle;
	
	ITimerComponent::TimerHandle m_hCheckNoticeHandle;

	unsigned int m_dwLastRefreshSecond;
	ServerEngine::TimeResetValue m_contributeShopRefreshTimes;

	map<string, unsigned int> m_actorHaveAddLegionList;
	ServerEngine::TimeResetValue m_actorWeekAddLegionTimes;

	ServerEngine::TimeResetValue  m_useWorshipTimes;
	int m_beWorshipGetContribute;
    map<string,unsigned int >  m_actorHaveWorship; //每天清理

	ServerEngine::TimeResetValue m_contributeTimes;
};

#endif
