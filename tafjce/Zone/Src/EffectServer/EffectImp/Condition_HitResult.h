#ifndef __CONDITION_HITRESULT_H__
#define __CONDITION_HITRESULT_H__

#include "Condition_Base.h"

class Condition_HitResult:public Condition_Base
{
public:

	Condition_HitResult(SkillEffect_CondEffect* pEffect):Condition_Base(pEffect), m_iEventID(0), m_iHitResult(0){}

	virtual void registerEvent(HEntity hEntity, const EffectContext& ctx);
	virtual void unregisterEvent(HEntity hEntity);
	virtual bool parseConditon(vector<string>& paramList);

	void onEventDoDamage(EventArgs& args);

private:

	int m_iEventID;
	int m_iHitResult;
};


void Condition_HitResult::onEventDoDamage(EventArgs& args)
{
	// 根据配置的命中，判断是否触发事件
	EventArgsDamageCtx& damageCtx = (EventArgsDamageCtx&)args;
	if( (0 != m_iHitResult) && (damageCtx.iHitResult != m_iHitResult) )
	{
		return;
	}
	
	EffectContext tmpCtx;

	if(EVENT_ENTITY_POSTDODAMAGE == m_iEventID)
	{
		m_pCondEffect->doTriggerEffect(damageCtx.hGiver, tmpCtx);
	}
	else if(EVENT_ENTITY_POSTBEDAMAGE == m_iEventID)
	{
		m_pCondEffect->doTriggerEffect(damageCtx.hEntity, tmpCtx);
	}
}

void Condition_HitResult::registerEvent(HEntity hEntity, const EffectContext& ctx)
{
	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	pEntity->getEventServer()->subscribeEvent(m_iEventID, this, &Condition_HitResult::onEventDoDamage);
}

void Condition_HitResult::unregisterEvent(HEntity hEntity)
{
	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	pEntity->getEventServer()->unsubscribeEvent(m_iEventID, this, &Condition_HitResult::onEventDoDamage);
}

bool Condition_HitResult::parseConditon(vector<string>& paramList)
{
	assert(paramList.size() > 0);
	if(paramList[0] == "暴击")
	{
		m_iHitResult = GSProto::en_SkillHitResult_Knock;
		paramList.erase(paramList.begin() );

		m_iEventID = EVENT_ENTITY_POSTDODAMAGE;

		return true;
	}
	else if(paramList[0] == "闪避")
	{
		m_iHitResult = GSProto::en_SkillHitResult_Doge;
		paramList.erase(paramList.begin() );
	
		m_iEventID = EVENT_ENTITY_POSTBEDAMAGE;

		return true;
	}
	else if(paramList[0] == "抵挡")
	{
		m_iHitResult = GSProto::en_SkillHitResult_Block;
		paramList.erase(paramList.begin() );
	
		m_iEventID = EVENT_ENTITY_POSTBEDAMAGE;

		return true;
	}
	else if(paramList[0] == "攻击")
	{
		m_iHitResult = 0;
		paramList.erase(paramList.begin() );

		m_iEventID = EVENT_ENTITY_POSTDODAMAGE;

		return true;
	}
	

	return false;
}


#endif
