#ifndef __BUFFCONDITION_SKILLUSE_POST_H__
#define __BUFFCONDITION_SKILLUSE_POST_H__

#include "BuffCondition_Base.h"

class BuffCondition_SkillUsePost:public BuffCondition_Base
{
public:

	BuffCondition_SkillUsePost(bool bSpecialEffect, int iCDRound, bool bFirstEffect, int iHPPercentCheck, int iChance):BuffCondition_Base(bSpecialEffect)
		, m_iCDRound(iCDRound), m_iLastTriggerRound(-1), m_bFirstEffect(bFirstEffect), 
		m_iHPCheckPercent(iHPPercentCheck), m_iChance(iChance){}

	~BuffCondition_SkillUsePost()
	{
		IEntity* pMaster = getEntityFromHandle(m_hMaster);
		if(!pMaster) return;

		pMaster->getEventServer()->unsubscribeEvent(EVENT_ENTITY_USESKILL_FINISH, this, &BuffCondition_SkillUsePost::onSkillUsePostFinish);
	}


	virtual bool initlize(HEntity hMaster, IEntityBuff* pBuff)
	{
		BuffCondition_Base::initlize(hMaster, pBuff);

		IEntity* pEntity = getEntityFromHandle(hMaster);
		if(!pEntity) return false;

		pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_USESKILL_FINISH, this, &BuffCondition_SkillUsePost::onSkillUsePostFinish);
	
		return true;
	}

	void onSkillUsePostFinish(EventArgs& args)
	{
		IEntity* pMaster = getEntityFromHandle(m_hMaster);
		if(!pMaster) return;

		if(m_iHPCheckPercent > 0)
		{
			int iHP = pMaster->getProperty(PROP_ENTITY_HP, 0);
			int iMaxHP = pMaster->getProperty(PROP_ENTITY_MAXHP, 0);

			int iPercent = (int)((double)iHP/iMaxHP*10000);
			if(iPercent >= m_iHPCheckPercent)
			{
				return;
			}
		}

		IRandom* pRandom = getComponent<IRandom>("Random", IID_IMiniAprRandom);
		assert(pRandom);

		// ÑéÖ¤¸ÅÂÊ
		int iChance = pRandom->random() % 10000;
		if(iChance >= m_iChance)
		{
			return;
		}
	
		if(m_bFirstEffect && m_iLastTriggerRound < 0)
		{
			m_iLastTriggerRound = m_iCurrentRound;
			doTrigger();
			return;
		}
		
		if(m_iCurrentRound - m_iLastTriggerRound >= m_iCDRound)
		{
			m_iLastTriggerRound = m_iCurrentRound;
			doTrigger();
		}
	}

	virtual BuffCondition_Base* clone() const{return new BuffCondition_SkillUsePost(m_bSpecialEffect, m_iCDRound, m_bFirstEffect, m_iHPCheckPercent, m_iChance);}

private:
	
	int m_iCDRound;
	int m_iLastTriggerRound;
	bool m_bFirstEffect;
	int m_iHPCheckPercent;
	int m_iChance;
};

#endif
