#ifndef __OUTPRX_IMP_H__
#define __OUTPRX_IMP_H__

class FightRecordQueryCb:public ServerEngine::FightDataPrxCallback
{
public:

	FightRecordQueryCb(const ServerEngine::PKFight& fightKey, const DelegateQueryFight& cb):m_fightKey(fightKey), m_callBack(cb){}

	virtual void callback_getFightData(taf::Int32 ret,  const std::string& strBattleData);
	virtual void callback_getFightData_exception(taf::Int32 ret);

private:
	
	ServerEngine::PKFight m_fightKey;
	DelegateQueryFight m_callBack;
};

class FightRecordSaveCb:public ServerEngine::FightDataPrxCallback
{
public:

	virtual void callback_saveFightData(taf::Int32 ret);
	virtual void callback_saveFightData_exception(taf::Int32 ret);
};

class DelBuffActionListener:public Detail::EventHandle
{
public:

	DelBuffActionListener(GSProto::FightDataAll& wholeData, const vector<HEntity>& memberList, const vector<HEntity>& externList);
	~DelBuffActionListener();
	void onEventDelBuff(EventArgs& args);

private:

	GSProto::FightDataAll& m_wholeData;
	vector<HEntity> m_memberList;
	vector<HEntity> m_externList;
};

class PropChgResultListener:public Detail::EventHandle
{
public:
	
	PropChgResultListener(HEntity hEntity);
	~PropChgResultListener();
	
	void onEventDoPropChg(EventArgs& args);

private:

	HEntity m_hEntity;
};

class BuffAddListener:public Detail::EventHandle
{
public:

	BuffAddListener(const vector<HEntity>& memberList);
	~BuffAddListener();

	void onEventBuffAdd(EventArgs& args);

private:

	vector<HEntity> m_memberList;
};


class SkillActionListener:public Detail::EventHandle
{
public:

	SkillActionListener(GSProto::FightDataAll& wholeData, HEntity hAttacker, const vector<HEntity>& memberList, const vector<HEntity>& externList);
	~SkillActionListener();

	void onEventUseSkill(EventArgs& args);
	void onEventSkillUseFinish(EventArgs& args);
	void onEventDoDamage(EventArgs& args);
	void onEventBeDamage(EventArgs& args);
	void onEventDoFightPropChg(EventArgs& args);
	void onEventContinueSkill(EventArgs& args);
	void onEventAddBuff(EventArgs& args);
	void onEventDelBuff(EventArgs& args);
	void onEventKill(EventArgs& args);

	void onEventZiBao(EventArgs& args);

	void onEventOtherUseSkill(EventArgs& args);
	void onEventPreExeEffect(EventArgs& args);

	void processNormalDamage(EventArgsDamageCtx& damageCtx);
	void processBackDamage(EventArgsDamageCtx& damageCtx);

	void triggerResultSet();
	GSProto::FightAction* getCurrentAction();

	int calcDropItemCount();

private:
	
	//GSProto::FightAction& m_fightAction;
	//GSProto::FightAction* m_pFightAction;
	GSProto::FightDataAll& m_whleData;
	HEntity m_hAttacker;
	vector<HEntity> m_memberList;
	vector<HEntity> m_externList;
	bool m_bBatterSkill;
	int m_iActionIndex;
};

#endif
