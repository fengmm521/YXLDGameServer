#ifndef __BUFFCONDTION_TRY_ADDBUFF_H__
#define __BUFFCONDTION_TRY_ADDBUFF_H__

#include "BuffCondition_Base.h"

class BuffCondition_TryAddBuff:public BuffCondition_Base
{
public:

	BuffCondition_TryAddBuff(bool bSpecialEffect, const vector<int>& buffTypeList, int iCDRound, bool bFirstActive):
		BuffCondition_Base(bSpecialEffect), m_buffTypeList(buffTypeList), m_iCDRound(iCDRound)
			,m_iLastTriggerRound(-1), m_bFirstActive(bFirstActive){}
	~BuffCondition_TryAddBuff()
	{
		IEntity* pMaster = getEntityFromHandle(m_hMaster);
		if(pMaster)
		{
			pMaster->getEventServer()->unsubscribeEvent(EVENT_ENTITY_TRYADDBUFF, this, &BuffCondition_TryAddBuff::onEventTryAddBuff);
		}
	}

	virtual bool initlize(HEntity hMaster, IEntityBuff* pBuff)
	{
		BuffCondition_Base::initlize(hMaster, pBuff);

		IEntity* pMaster = getEntityFromHandle(hMaster);
		if(pMaster)
		{
			pMaster->getEventServer()->subscribeEvent(EVENT_ENTITY_TRYADDBUFF, this, &BuffCondition_TryAddBuff::onEventTryAddBuff);
		}

		return true;
	}

	void onEventTryAddBuff(EventArgs& args)
	{
		int iBuffID = args.context.getInt("buffID", 0);
		IBuffFactory* pBuffFactory = getComponent<IBuffFactory>(COMPNAME_BuffFactory, IID_IBuffFactory);
		assert(pBuffFactory);

		const IEntityBuff* pBuffProtoType = pBuffFactory->getBuffPrototype(iBuffID);
		if(!pBuffProtoType) return;

		int iBuffType = pBuffProtoType->getBuffType();
		bool bHit = std::find(m_buffTypeList.begin(), m_buffTypeList.end(), iBuffType) != m_buffTypeList.end();
		if(!bHit) return;

		if(m_bFirstActive && (m_iLastTriggerRound < 0) )
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

	virtual BuffCondition_Base* clone()const {return new BuffCondition_TryAddBuff(m_bSpecialEffect, m_buffTypeList, m_iCDRound, m_bFirstActive);}

private:

	vector<int> m_buffTypeList;
	int m_iCDRound;
	int m_iLastTriggerRound;
	bool m_bFirstActive;
};

#endif
