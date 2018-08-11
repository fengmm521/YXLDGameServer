#ifndef __MANOR_SYSTEM_H__
#define __MANOR_SYSTEM_H__

class ManorSystem : public ObjectBase<IManorSystem>,public Detail::EventHandle,public ITimerCallback 
{
public:
	ManorSystem();
	virtual ~ManorSystem();
public:
	// IEntitySubSystem Interface
	virtual Uint32 getSubsystemID() const;
	virtual Uint32 getMasterHandle();
	virtual bool create(IEntity* pEntity, const std::string& strData);
	virtual bool createComplete();
	virtual const std::vector<Uint32>& getSupportMessage();
	virtual void onMessage(QxMessage* pMessage);
	virtual void packSaveData(string& data);
	//IManorSystem
	virtual void manorSilverResGoldAddSpeed(int iCost, int iResID);
	virtual void manorHeroExpResGoldAddSpeed(int iCost, int iResID);
	virtual void manorGoldAddWuHunDianLevelUpSpeed(int iCost);
	virtual void manorGoldAddTieJiangPuLevelUpSpeed(int iCost);
	virtual void initMachineData(string strData);
	virtual void resetMachineData();

	virtual void GMLootFight(ServerEngine::PKRole pkRole) ;

	void getActorManorSysData(ServerEngine::ManorSystemData &monorSystemData);
	void sendWillBeLootManorInfo(HEntity targetHEntity);
	int calcSilver(bool bHaveLoot, HEntity hTargetEntity, int iFightStar = 0);
	int calcHeroExp(bool bHaveLoot, HEntity hTargetEntity, int iFightStar = 0);
	int calcTieJiangPuCanGetItemId();
	int calcWuHunDianCanGetIHeroId();
	void getActorFormation(google::protobuf::RepeatedPtrField< ::GSProto::ManorEnemyHeroFormation >* manorEnemyFormation);
	virtual void processLootFight(const ServerEngine::PKRole& targetKey, const ServerEngine::BattleData& data);
	void actorBeforeBattleDataCallBack(HEntity hTarget, ServerEngine::PIRole& roleInfo);
	bool getActorIsInLoot();
	void processFightFailed(HEntity hTarget, ServerEngine::PIRole& roleInfo,const ServerEngine::PKRole& pkRole, const ServerEngine::BattleData& battleData);
	void processFightWin(HEntity hTarget,ServerEngine::PIRole& roleInfo,const ServerEngine::PKRole& pkRole, const ServerEngine::BattleData& battleData);
	void AddManorLootLog(GSProto::ManorLootLog& log);
	void actorBeLootChangeData();
	virtual void onTimer(int nEventId);
	bool FindOtherLoot();
	virtual void getActorSilverRes(vector<ServerEngine::ManorResData>& silverRes);
	virtual void getActorHeroExpRes(vector<ServerEngine::ManorResData>& heroExpRes);

	virtual void getActorManorInfo(GSProto::CMD_MANOR_REQUEST_LOOT_SC & scMsg, HEntity targetHEntity);
	virtual void manorResHarvestReturn();
	virtual void resetHarvest();
	virtual void manorNoCanLootActorReturn();

	virtual int getTieJiangPuLevel()  ;
	virtual int getWuHunDianLevel();
	
protected:
	void onQueryManorInfo(const GSProto::CSMessage& msg);
	void onResLevelUpUseGold(const GSProto::CSMessage& msg);
	void onResLevelUpCommon(const GSProto::CSMessage& msg);
	void onHarvestRes(const GSProto::CSMessage& msg);
	void onQueryWuHunDian(const GSProto::CSMessage& msg);
	void onWuHunDianLevelUp(const GSProto::CSMessage& msg);
	void onWuHunDianBeginProduct(const GSProto::CSMessage& msg);
	void onWuHunDianSetHeroWuxiekeji(const GSProto::CSMessage& msg);
	void onWuHunDianRefresh(const GSProto::CSMessage& msg);
	void onQueryTieJiangPu(const GSProto::CSMessage& msg);
	void onTieJiangPuLevelUp(const GSProto::CSMessage& msg);
	void onTieJiangPuRefresh(const GSProto::CSMessage& msg);
	void onTieJiangPuBeginProduct(const GSProto::CSMessage& msg);
	void onTieJiangPuSetItemWuxiekeji(const GSProto::CSMessage& msg);
	void onQueryManorLog(const GSProto::CSMessage& msg);
	void onReqRevenge(const GSProto::CSMessage& msg);
	void onPublishReward(const GSProto::CSMessage& msg);
	void onWuHunDianBulidAddSpeed(const GSProto::CSMessage& msg);
	void onTieJiangPuBulidAddSpeed(const GSProto::CSMessage& msg);
	void onRequestLoot(const GSProto::CSMessage& msg);
	void onRequestLootBattle(const GSProto::CSMessage& msg);
	void onRequestOneKeyHarvestWuHunDian(const GSProto::CSMessage& msg);
	void onRequestOneKeyHarvestTieJiangPu(const GSProto::CSMessage& msg);

	void onQueryProtectInfo(const GSProto::CSMessage& msg);
	void onBuyProtect(const GSProto::CSMessage& msg);
	
	void checkVigor();
	void checkRes();

	void initManorSystemData();
	void fillResData(vector<ServerEngine::ManorResData>& resData, GSProto::ManorResType type, google::protobuf::RepeatedPtrField< ::GSProto::ManorResDetail >* manorRes);
	void getSilverSpeedAndTotolOutPut(int& speed, int& totalOutPut);
	void getHeroExpSpeedAndTotolOutPut(int& speed, int& totalOutPut);
	int calcSilverPutOut();
	int calcHeroExpPutOut();
	int calcProduct(int iBeginSecond, ManorResUnit unit, int iEndProductSecond = 0);
	double getProductRateBase();
	void pushManorInfo2Client();
	void fillWuHunDianDetail(GSProto::ManorResDetail* detail);
	void sendWuHunDian2Client();
	void sendTieJiangPu2Client();
	void fillTieJiangPuDetail(GSProto::ManorResDetail* detail);
	void checkReset();
	int getNewBuildTeams();
	bool manorBuildTeamsCanBulid();
	void fillTiejiangPuGeZiDetail(GSProto::ManorTieJiangPuItemBox& box, const ServerEngine::ManorWuHunAndItemUnit& unit, int index);
	void fillWuHunDianGeZiDetail(GSProto::ManorHeroSoulBox& box, const ServerEngine::ManorWuHunAndItemUnit& unit, int index);

	void CheckWuHunDianAndTieJiangPuBuild();

	void checkNotice();

	void getTotalProduct(int& heroExp,  int& silver);
	void getHeroExpTotalPutOut(int& heroExp);
	void getSilverTotalPutOut(int& silver);

	bool wuHunDianAndTieJiangPuIsOpen(bool bIsWuHunDian =false);

	void clearProtectItemCD();

	void pushProtectInfo();

	int searchCost(HEntity hEntity);
public:
	int getRemaindProtectSecond(const ServerEngine::ManorSystemData& manorSystemData );
	
private:
	HEntity m_hEntity;
	ServerEngine::ManorSystemData  m_manorSystemData;
	unsigned int iLootBeginSecond ;  //5分钟掠夺超时
	ITimerComponent::TimerHandle m_CheckVigorTimerHandle;
	int iLootCount;
	map<ServerEngine::PKRole, bool>  m_ActorHaveLootMap;
	ITimerComponent::TimerHandle m_CheckHeroSoulAndTieJiangPu;

	ServerEngine::PKRole gmRoleKey;
	bool gmFlag;

	//ITimerComponent::TimerHandle m_CheckNoticeHandle;		//检查提示
	
};

#endif
