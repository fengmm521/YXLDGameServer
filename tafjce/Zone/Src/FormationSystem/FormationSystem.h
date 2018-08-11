#ifndef __FORMATION_SYSTEM_H__
#define __FORMATION_SYSTEM_H__

class FormationSystem:public ObjectBase<IFormationSystem>, public Detail::EventHandle
{
public:

	FormationSystem();
	~FormationSystem();

	// IEntitySubsystem interface
	virtual Uint32 getSubsystemID() const;
	virtual Uint32 getMasterHandle();
	virtual bool create(IEntity* pEntity, const std::string& strData);
	virtual bool createComplete();
	virtual const std::vector<Uint32>& getSupportMessage();
	virtual void onMessage(QxMessage* pMessage);
	virtual void packSaveData(string& data);

	// IFormationSystem interface
	virtual void getEntityList(vector<HEntity>& entityList);
	virtual HEntity getEntityFromPos(int iPos);
	virtual int getEntityPos(HEntity hEntity);
	virtual int getVisibleHeroID();
	virtual void setChgFormationSwitchState(bool bClose);
	virtual int getFormationHeroLimit();
		
	void onSendClientFormation(EventArgs& args);
	void onEventDestroyHero(EventArgs& args);
	void sendFormationData();
	void onFormationChgMsg(const GSProto::CSMessage& message);
	void onEventLevelUp(EventArgs& args);
	void onEventActiveGodAnimalChg(EventArgs& args);
	void onEventGodAnimaFightValueChg(EventArgs& args);
	void registerGodAnimalEvent(HEntity hOld, HEntity hCur);

	virtual int getFormationHeroCount() ;
private:

	void initFormation(const string& strFormationData);
//	void putDown(HEntity hMoveEntity);
	//void putOn(HEntity hMoveEntity, int iPos);
	//void chagePos(HEntity hMoveEntity, int iSrcPos, int iTargetPos);
	int getHeroPosFromBaseID(int iBaseID);
	int calcFormationLimit(int iLevel);
	void sumFightValue();
	void registerHeroFightValueChg();
	void unRegisterHeroFightValueChg();
	void onEventHeroFightChg(EventArgs& args);
	
private:

	HEntity m_hEntity;
	vector<HEntity> m_formationList;

	string m_strCacheData;

	//add by hyf 2014/5/7
	bool m_bClose;
	
};

#endif
