#ifndef __HERO_SYSTEM_H__
#define __HERO_SYSTEM_H__

class HeroSystem:public ObjectBase<IHeroSystem>, public Detail::EventHandle,public ITimerCallback 
{
public:

	HeroSystem();
	~HeroSystem();

	// IEntitySubSystem Interface
	virtual Uint32 getSubsystemID() const;
	virtual Uint32 getMasterHandle();
	virtual bool create(IEntity* pEntity, const std::string& strData);
	virtual bool createComplete();
	virtual const std::vector<Uint32>& getSupportMessage();
	virtual void onMessage(QxMessage* pMessage);
	virtual void packSaveData(string& data);
	
	// IHeroSystem Interface
	virtual vector<HEntity> getHeroList();
	virtual HEntity addHero(int iHeroID, bool bNotifyError, int iReason);
	virtual HEntity addHeroWithLevelStep(int iHeroID, int iLevelStep, bool bNotifyError, int iReason);
 	virtual bool destroyHero(HEntity hHero, bool bNotifyError);
	virtual HEntity getHeroFromUUID(const string& strUUID);
	virtual bool doHeroLevelStepUp(HEntity hTargetHero, bool bNotifyMsg);
	virtual bool checkHeroFull();
	virtual void gmSetAllHeroLevelStep(int iLevelStep);	
	virtual void gmSetAllHeroLevel(int iLevel);
	virtual void packHeroBaseData(IEntity* pHero, GSProto::HeroBaseData& scHeroBase);
	virtual void sendHeroUpdateInfo(HEntity hHero);
	virtual void fillFemaleHeroList(vector<ServerEngine::FemailHeroDesc>& femaleList);
	virtual int getHeroItemID(int iHeroID);
	virtual HEntity callHero(int iHeroID);
	virtual void GMOnlineGetRoleHeroData(string& strJson);

	void initHeroData(const std::string& strData);
	void onEventActorCreateFinish(EventArgs& args);
	
	void onGetHeroDescMsg(const GSProto::CSMessage& message);
	void onHeroConvertMsg(const GSProto::CSMessage& message);
	void onHyperLinkMsg(const GSProto::CSMessage& message);
	void onQueryHeroLevelInfo(const GSProto::CSMessage& message);
	void onHeroLevelUp(const GSProto::CSMessage& message);
	void onQueryHeroTip(const GSProto::CSMessage& message);
	//void onQueryLevelStepInfo(const GSProto::CSMessage& message);
	void onExeLevelStepUp(const GSProto::CSMessage& message);
	void onQueryProgress(const GSProto::CSMessage& message);
	void onReqHeroSangong(const GSProto::CSMessage& message);
	void onReqQueryConvert(const GSProto::CSMessage& message);
	//void onReqConvertConfirm(const GSProto::CSMessage& message);
	void onReqTallentUpgrade(const GSProto::CSMessage& message);
	void onReqCallHero(const GSProto::CSMessage& message);
	void onReqEquipFavorite(const GSProto::CSMessage& message);
	void onReqCombineFavorite(const GSProto::CSMessage& message);
	void onReqHeroQualityUp(const GSProto::CSMessage& message);
	void onReqQuerySangong(const GSProto::CSMessage& message);

	bool checkFavorite(int iItemID, int iCount, int& iTkSilver, bool bFirstLv, map<int, int>& consumeMap);
	void consumeFavoriteCombine(int iItemID, int iCount, bool bFirstLv);
	
	
	//add by hyf 英雄出售
	void onReqSellHero(const GSProto::CSMessage& message);
	void onCheckOpenCovertView(const GSProto::CSMessage& message);
	
	void fillNewHeroData(ServerEngine::RoleSaveData& roleSaveData, int iHeroBaseID);
	HEntity _addHero(int iHeroID, int iLevelStep, bool bNotifyError, int iReason);
	//bool checkLevelStepCost(HEntity hHostHero, const set<HEntity>& usedList, bool bNotify, int& iNeedSilver, int& iSumProgress, int& iResultLevelStep, int& iResultLevel, int& iResultHeroExp);
	void packLevelInfo(HEntity hHero, GSProto::HeroLevelUpInfo* pLevelInfo);
	void sendHeroProgress(HEntity hHero);
	void notifyLevelStepUp(int iHeroID, int iPreLevelStep, int iCurLevelStep);
	void _notifyLevelStepUp(int iHeroID, int iLevelStep);
	int calcSangoHeroExp(HEntity hHero);

	// ITimerCallback Interface
	virtual void onTimer(int nEventId);
	bool noticeHeroLevelUp();
	bool noticeHeroStepLevelUp();

	bool heroCanLevelUp(IEntity* pHero);
	bool herocanStepLevelUp(IEntity* pHero);

	void checkNotice();

	void fillLevelUpNotice(GSProto::NoticeHeroList& heroList);
	void fillStepLevelUpNotice(GSProto::NoticeHeroList& heroList);

	//void addHeroSoul(int iHeroID, int iCount);
	//bool removeHeroSoul(int iHeroID, int iCount);
	int getHeroSoulCount(int iHeroID);
	int getHeroCallNeedCount(int iHeroID);
	
	//hyf
	virtual int calcHeroSoulCount(int iLevelStep);

	virtual int getHeroCountByQuality(int iQuality) ;

	virtual int getHeroCount();
	
private:

	HEntity m_hEntity;
	vector<HEntity> m_heroList;
	TimeRangeKeeper m_heroConvertData;
	//map<int, int> m_heroSoulList;

	int m_iSrcHeroID;
	int m_iConvtTargetHeroID; // 未确定的转换目标ID
	int m_iConvrtCount;
	ITimerComponent::TimerHandle m_CheckHeroTimerHandle;
};


#endif

