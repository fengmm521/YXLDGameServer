#ifndef __CONDITION_ANYDEAD_H__
#define __CONDITION_ANYDEAD_H__

#include "Condition_Base.h"


class Condition_AnyDead;

struct AnyDeadEventHelper:public Detail::EventHandle
{
	AnyDeadEventHelper():m_pOwner(NULL){}
	void onEventAnyDead(EventArgs& args);
	void beginRound();
	
	Condition_AnyDead* m_pOwner;
	HEntity m_hMaster;
	vector<HEntity> m_memberList;	
};

class Condition_AnyDead:public Condition_Base
{
public:

	friend class AnyDeadEventHelper;

	Condition_AnyDead(SkillEffect_CondEffect* pEffect):Condition_Base(pEffect){}

	virtual void registerEvent(HEntity hEntity, const EffectContext& ctx);
	virtual void unregisterEvent(HEntity hEntity);
	virtual bool parseConditon(vector<string>& paramList);

	void onEventBeginRound(EventArgs args);
	void onEventAnyDead(EventArgs& args);
	void onEventPreRelease(EventArgs& args);

	map<HEntity, AnyDeadEventHelper*> m_mapDeadEventMap;
};



#endif
