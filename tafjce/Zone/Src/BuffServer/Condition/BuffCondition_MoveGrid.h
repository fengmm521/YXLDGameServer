#ifndef __BUFF_CONDITION_MOVEGRID_H__
#define __BUFF_CONDITION_MOVEGRID_H__

#include "BuffCondition_Base.h"

class BuffCondition_MoveGrid:public BuffCondition_Base
{
public:

	BuffCondition_MoveGrid(bool bSpecialEffect, int iLimitCount):
		BuffCondition_Base(bSpecialEffect), m_iLimitCount(iLimitCount), m_iCount(0){}
	~BuffCondition_MoveGrid()
	{
		IEntity* pMaster = getEntityFromHandle(m_hMaster);
		if(pMaster)
		{
			pMaster->getEventServer()->unsubscribeEvent(EVENT_ENTITY_MOVEGRID, this, &BuffCondition_MoveGrid::onEventMoveGrid);
		}
	}

	virtual bool initlize(HEntity hMaster, IEntityBuff* pBuff)
	{
		BuffCondition_Base::initlize(hMaster, pBuff);
		IEntity* pMaster = getEntityFromHandle(hMaster);
		assert(pMaster);

		pMaster->getEventServer()->subscribeEvent(EVENT_ENTITY_MOVEGRID, this, &BuffCondition_MoveGrid::onEventMoveGrid);

		return true;
	}

	virtual BuffCondition_Base* clone()const{return new BuffCondition_MoveGrid(m_bSpecialEffect, m_iLimitCount);}

	void onEventMoveGrid(EventArgs& args)
	{
		m_iCount++;
		if(m_iCount >= m_iLimitCount)
		{
			doTrigger();
		}
	}

private:
	int m_iLimitCount;
	int m_iCount;
};


#endif

