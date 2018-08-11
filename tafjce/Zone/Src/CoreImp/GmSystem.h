#ifndef __GM_SYSTEM_H__
#define __GM_SYSTEM_H__


struct NullPVECb
{
	NullPVECb(){}

	NullPVECb(HEntity hEntity, int iMonsterGrp, const vector<int>& monsterGrpList, const ServerEngine::FightContext& ctx):m_hEntity(hEntity), 
		m_iMonsterGrpID(iMonsterGrp), m_monsterGrpList(monsterGrpList), m_fightCtx(ctx){}
	
	void operator()(taf::Int32 iRet, const ServerEngine::BattleData& data);
	

	HEntity m_hEntity;
	int m_iMonsterGrpID;
	vector<int> m_monsterGrpList;
	ServerEngine::FightContext m_fightCtx;
};


class GMSystem:public ObjectBase<IEntitySubsystem>
{
public:

	friend class NullPVECb;

	virtual Uint32 getSubsystemID() const{return IID_IGMSystem;}
	virtual Uint32 getMasterHandle(){return m_hEntity;}
	virtual bool create(IEntity* pEntity, const std::string& strData);
	virtual bool createComplete(){return true;}
	virtual const std::vector<Uint32>& getSupportMessage();
	virtual void onMessage(QxMessage* pMessage);
	virtual void packSaveData(string& data){}


	void doPVEFight(int iMonsterGrpID);
	void addItem(int iItemID, int iItemCount);
	void removeItem(int iItemID, int iItemCount);
	void addHero(int iHeroID, int iCount);
	
	void fightSoulPractice();
	void enableFightSoulAdvance();
	void autoCombineFightSoul();
	void setLifeAtt(int iID, int iValue);
	void fightSoulExchange(int iID);
	void addGodAnimal(int iGodAnimalID);
	void autoGodAnimal();
	void setHeroLevelStep(int iLevelStep);
	void setHeroLevel(int iLevel);
	void enableDomain();
	void setLevel(int iLevel);
	void setLegionLevel(int iLevel);
	void startLegionFight();
	void sysInfo();
	void sendSysMsg(const string& strMsg);
	void startCampBattle(string startTime,string preTime, string endTime);
	void addNormalGift(int iGiftID);
	void addCustomGift(int iItemID);
	void addFightSoul(int iFightSoulID);
	void testRechareLog();
	void multiFight(const vector<int>& monsterGrpList);
	void pvpFight(const string& strAccount, int iPos, int iWorldID);
	void lootFight(const string& strAccount, int iPos, int iWorldID);
	
	void postMarquee(vector<string>& cmdList);
	void replay(int iActionIndex, int iResultSetIndex, int iTargetPos);
	void addEquip(int iItemID);

	void addHeroWithStep(int iHeroID, int istep);
	void callHero(int iHeroID);
	void setAllHeroQuality(int iQuality);


public:
	void GMTestDreamLand(int iSectionID, int iSceneId);
	void GMRequsetFormation(int iSectionID, int iSceneId);
	void GMClearDreamLandData();
	void GMAddDreamLandResetTimes();
private:

	HEntity m_hEntity;

	NullPVECb m_lastPVEFight;
	vector<taf::Int64> m_randValueList;
};



#endif
