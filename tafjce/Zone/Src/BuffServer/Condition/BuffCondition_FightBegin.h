#ifndef __BUFFCONDITION_FIGHTBEGIN_H__
#define __BUFFCONDITION_FIGHTBEGIN_H__

#include "BuffCondition_Base.h"

class BuffCondition_FightBegin:public BuffCondition_Base
{
public:

	BuffCondition_FightBegin(bool bSpecialEffect):BuffCondition_Base(bSpecialEffect){}
	~BuffCondition_FightBegin()
	{
		IEntity* pMaster = getEntityFromHandle(m_hMaster);
		if(pMaster)
		{
			pMaster->getEventServer()->unsubscribeEvent(EVENT_ENTITY_BEGIN_FIGHT, this, &BuffCondition_FightBegin::onEventBeginFight);
	
		}
	}

	virtual bool initlize(HEntity hMaster, IEntityBuff* pBuff)
	{
		BuffCondition_Base::initlize(hMaster, pBuff);

		IEntity* pMaster = getEntityFromHandle(hMaster);
		if(pMaster)
		{
			pMaster->getEventServer()->subscribeEvent(EVENT_ENTITY_BEGIN_FIGHT, this, &BuffCondition_FightBegin::onEventBeginFight);
		}
		
		return true;
	}

	void onEventBeginFight(EventArgs& args)
	{
		doTrigger();
	}

	virtual BuffCondition_Base* clone()const{return new BuffCondition_FightBegin(m_bSpecialEffect);}
};

#endif
