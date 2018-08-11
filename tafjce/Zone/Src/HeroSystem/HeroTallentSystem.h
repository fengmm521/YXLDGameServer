#ifndef __HERO_TALLENT_SYSTEM_H__
#define __HERO_TALLENT_SYSTEM_H__

class HeroTallentSystem:public ObjectBase<IHeroTallentSystem>, public Detail::EventHandle
{
public:

	HeroTallentSystem();
	~HeroTallentSystem();

	virtual bool getTallentLevel(int iTallentID, int& iLevel);

	// IEntitySubsystem Interface
	virtual Uint32 getSubsystemID() const;
	virtual Uint32 getMasterHandle();
	virtual bool create(IEntity* pEntity, const std::string& strData);
	virtual bool createComplete();
	virtual const std::vector<Uint32>& getSupportMessage();
	virtual void onMessage(QxMessage* pMessage){}
	virtual void packSaveData(string& data);

	virtual void checkAutoUpgrade();

	void equipFavorite(int iPos, int iItemID);
	void equipFavoriteProp(int iItemID, bool bPuton);
	void calcQualityAndFavoriteProp(bool bPuton);
	void upgradeQuality();
	
	
	void fillHeroTallent(GSProto::HeroBaseData& scHeroBase);
	void initTallent();
	bool canTallentUpgrade(int iTallentID, int iLevel, bool bNotify);
	bool consumeTalllentUpgrade(int iTallentID, int iLevel);
	bool canAutoUpgrade(int iTallentID, int iLevel);
	
	void onEventLevelStepChg(EventArgs& args);
	void onEventMasterLevelChg(EventArgs& args);
	void onEventMasterSilverChg(EventArgs& args);
	void onEventItemChg(EventArgs& args);
	
	void tallentUpgrade(ServerEngine::TallentData& refTallentData);
	void tallentUpgrade(int iTallendID);

	void sendTallentUpdate(int iTallentID, int iLevel);

	void notifyUpgradeFlagChg();

	

private:

	HEntity m_hEntity;
	ServerEngine::HeroTallentSystemData m_tallentSystemData;
	map<int, bool> m_canUpgradeMap;
};


#endif
