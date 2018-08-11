#ifndef __CONDITION_FIGHT_BEGIN_H__
#define __CONDITION_FIGHT_BEGIN_H__


#include "Condition_Base.h"



class Condition_FightBegin:public Condition_Base
{
public:

	Condition_FightBegin(SkillEffect_CondEffect* pEffect):Condition_Base(pEffect){}

	virtual void registerEvent(HEntity hEntity, const EffectContext& ctx);
	virtual void unregisterEvent(HEntity hEntity);
	virtual bool parseConditon(vector<string>& paramList);
	void onEventFightBegin(EventArgs& args);
};


void Condition_FightBegin::registerEvent(HEntity hEntity, const EffectContext& ctx)
{
	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_BEGIN_FIGHT, this, &Condition_FightBegin::onEventFightBegin);
}


void Condition_FightBegin::unregisterEvent(HEntity hEntity)
{
	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);
	
	pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_BEGIN_FIGHT, this, &Condition_FightBegin::onEventFightBegin);
}

bool Condition_FightBegin::parseConditon(vector<string>& paramList)
{
	assert(paramList.size() > 0);

	if(paramList[0] == "开始战斗")
	{
		paramList.erase(paramList.begin() );
		return true;
	}

	return false;

	return true;
}

void Condition_FightBegin::onEventFightBegin(EventArgs& args)
{
	HEntity hEntity = args.context.getInt("entity");
	int iLoopCount = args.context.getInt("loopcount");

	if(0 != iLoopCount) return;
	
	EffectContext tmpCtx;
	m_pCondEffect->doTriggerEffect(hEntity, tmpCtx);
}


#endif

