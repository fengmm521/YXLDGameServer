#ifndef __BUFFCONDITION_BROKENSHIELD_H__
#define __BUFFCONDITION_BROKENSHIELD_H__

#include "BuffCondition_Base.h"

class BuffCondition_BrokenShield:public BuffCondition_Base
{
public:

	BuffCondition_BrokenShield(bool bSpecialEffect):BuffCondition_Base(bSpecialEffect){}
	~BuffCondition_BrokenShield()
	{
		IEntity* pMaster = getEntityFromHandle(m_hMaster);
		if(pMaster)
		{
			pMaster->getEventServer()->unsubscribeEvent(EVENT_ENTITY_BROKENSHIELD, this, &BuffCondition_BrokenShield::onEventBrokenShield);
		}
	}

	virtual bool initlize(HEntity hMaster, IEntityBuff* pBuff)
	{
		BuffCondition_Base::initlize(hMaster, pBuff);

		IEntity* pMaster = getEntityFromHandle(hMaster);
		if(pMaster)
		{
			pMaster->getEventServer()->subscribeEvent(EVENT_ENTITY_BROKENSHIELD, this, &BuffCondition_BrokenShield::onEventBrokenShield);
		}
	
		return true;
	}

	void onEventBrokenShield(EventArgs& args)
	{
		doTrigger();
	}

	virtual BuffCondition_Base* clone()const {return new BuffCondition_BrokenShield(m_bSpecialEffect);}
};


#endif
