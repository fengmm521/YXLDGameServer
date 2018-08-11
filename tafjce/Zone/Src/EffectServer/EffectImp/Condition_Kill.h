#ifndef __CONDITION_KILL_H__
#define __CONDITION_KILL_H__


#include "Condition_Base.h"


class Condition_Kill:public Condition_Base
{
public:
	
	Condition_Kill(SkillEffect_CondEffect* pEffect):Condition_Base(pEffect){}

	virtual void registerEvent(HEntity hEntity, const EffectContext& ctx);
	virtual void unregisterEvent(HEntity hEntity);
	virtual bool parseConditon(vector<string>& paramList);

	void onEventKill(EventArgs& args);
};


void Condition_Kill::registerEvent(HEntity hEntity, const EffectContext& ctx)
{
	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_EFFECTDOKILL, this, &Condition_Kill::onEventKill);
}


void Condition_Kill::unregisterEvent(HEntity hEntity)
{
	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_EFFECTDOKILL, this, &Condition_Kill::onEventKill);
}

bool Condition_Kill::parseConditon(vector<string>& paramList)
{
	assert(paramList.size() > 0);

	if(paramList[0] == "击杀")
	{
		paramList.erase(paramList.begin() );
		return true;
	}

	return false;
}


void Condition_Kill::onEventKill(EventArgs& args)
{
	HEntity hEntity = args.context.getInt("entity");
	
	EffectContext tmpCtx;
	m_pCondEffect->doTriggerEffect(hEntity, tmpCtx);
}


#endif
