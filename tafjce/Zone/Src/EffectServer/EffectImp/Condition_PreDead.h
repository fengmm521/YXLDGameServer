#ifndef __CONDITION_PREDEAD_H__
#define __CONDITION_PREDEAD_H__

#include "Condition_Base.h"

class Condition_PreDead:public Condition_Base
{
public:

	Condition_PreDead(SkillEffect_CondEffect* pEffect):Condition_Base(pEffect){}

	virtual void registerEvent(HEntity hEntity, const EffectContext& ctx);
	virtual void unregisterEvent(HEntity hEntity);
	virtual bool parseConditon(vector<string>& paramList);

	void onEventPreDead(EventArgs& args);
};


void Condition_PreDead::registerEvent(HEntity hEntity, const EffectContext& ctx)
{
	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_EFFECTBEKILL, this, &Condition_PreDead::onEventPreDead);
}

void Condition_PreDead::unregisterEvent(HEntity hEntity)
{
	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_EFFECTBEKILL, this, &Condition_PreDead::onEventPreDead);
}

bool Condition_PreDead::parseConditon(vector<string>& paramList)
{
	assert(paramList.size() > 0);

	if(paramList[0] == "死亡")
	{
		paramList.erase(paramList.begin() );

		return true;
	}

	return false;
}

void Condition_PreDead::onEventPreDead(EventArgs& args)
{
	HEntity hEntity = args.context.getInt("entity");

	// 设置个标记吧
	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	pEntity->setProperty(PROP_ENTITY_PREDEAD_USESKILLFLAG, 1);

	EffectContext tmpCtx;
	m_pCondEffect->doTriggerEffect(hEntity, tmpCtx);

	// 去掉标记
	pEntity->setProperty(PROP_ENTITY_PREDEAD_USESKILLFLAG, 0);
}


#endif
