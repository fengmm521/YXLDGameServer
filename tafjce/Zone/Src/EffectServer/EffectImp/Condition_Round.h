#ifndef __CONDITION_ROUND_H__
#define __CONDITION_ROUND_H__

#include "Condition_Base.h"

class Condition_Round:public Condition_Base
{
public:

	Condition_Round(SkillEffect_CondEffect* pEffect):Condition_Base(pEffect), m_iRoundInterval(0){}

	virtual void registerEvent(HEntity hEntity, const EffectContext& ctx);
	virtual void unregisterEvent(HEntity hEntity);
	virtual bool parseConditon(vector<string>& paramList);

	void onPreAttack(EventArgs& args);

private:

	int m_iRoundInterval;
	//int m_iPreTriggerRound;

	map<HEntity, int> m_preRoundMap;
};

//EVENT_ENTITY_PRE_ATTACK
void Condition_Round::registerEvent(HEntity hEntity, const EffectContext& ctx)
{
	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_PRE_ATTACK, this, &Condition_Round::onPreAttack);
}

void Condition_Round::unregisterEvent(HEntity hEntity)
{
	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_PRE_ATTACK, this, &Condition_Round::onPreAttack);
	m_preRoundMap.erase(hEntity);
}

bool Condition_Round::parseConditon(vector<string>& paramList)
{
	assert(paramList.size() > 0);

	if( (paramList[0] == "间隔回合") && (paramList.size() > 2) )
	{
		m_iRoundInterval = AdvanceAtoi(paramList[1]);
		paramList.erase(paramList.begin(), paramList.begin() + 2);

		return true;
	}

	return false;
}

void Condition_Round::onPreAttack(EventArgs& args)
{
	HEntity hEntity = args.context.getInt("entity");

	int iPreRound = 0;
	if(m_preRoundMap.find(hEntity) != m_preRoundMap.end() )
	{
		iPreRound = m_preRoundMap[hEntity];
	}
	
	int iRound = args.context.getInt("round");

	//m_preRoundMap
	
	if(iRound != iPreRound + m_iRoundInterval)
	{
		return;
	}
	
	//m_iPreTriggerRound = iRound;
	m_preRoundMap[hEntity] = iRound;
	

	EffectContext tmpCtx;
	m_pCondEffect->doTriggerEffect(hEntity, tmpCtx);
}


#endif
