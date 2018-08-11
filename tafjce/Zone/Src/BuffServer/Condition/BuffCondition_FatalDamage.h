#ifndef __BUFFCONDITION_FATALDAMAGE_H__
#define __BUFFCONDITION_FATALDAMAGE_H__

#include "BuffCondition_Base.h"

class BuffCondition_FatalDamage:public BuffCondition_Base
{
public:

	BuffCondition_FatalDamage(bool bSpecialEffect, int iCDRound, bool bFirstActive):BuffCondition_Base(bSpecialEffect),
		m_iLastTriggerRound(-1), m_iCDRound(iCDRound), m_bFirstActive(bFirstActive){}
	~BuffCondition_FatalDamage()
	{
		IEntity* pMaster = getEntityFromHandle(m_hMaster);
		if(pMaster)
		{
			pMaster->getEventServer()->unsubscribeEvent(EVENT_ENTITY_FATALDAMAGE, this, &BuffCondition_FatalDamage::onEventFatalDamage);
		}
	}

	virtual bool initlize(HEntity hMaster, IEntityBuff* pBuff)
	{
		BuffCondition_Base::initlize(hMaster, pBuff);

		IEntity* pMaster = getEntityFromHandle(hMaster);
		if(pMaster)
		{
			pMaster->getEventServer()->subscribeEvent(EVENT_ENTITY_FATALDAMAGE, this, &BuffCondition_FatalDamage::onEventFatalDamage);
		}

		return true;
	}

	void onEventFatalDamage(EventArgs& args)
	{	
		if(m_bFirstActive && (m_iLastTriggerRound < 0))
		{
			m_iLastTriggerRound = m_iCurrentRound;
			doTrigger();
			return;
		}

		if( (m_iCurrentRound - m_iLastTriggerRound) >= m_iCDRound)
		{
			m_iLastTriggerRound = m_iCurrentRound;
			doTrigger();
		}	
	}

	virtual BuffCondition_Base* clone()const{return new BuffCondition_FatalDamage(m_bSpecialEffect, m_iCDRound, m_bFirstActive);}

private:

	int m_iLastTriggerRound;
	int m_iCDRound;
	bool m_bFirstActive;
};

#endif
