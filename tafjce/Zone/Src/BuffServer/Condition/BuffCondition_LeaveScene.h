#ifndef __BUFF_CONDITION_LEAVESCENE_H__
#define __BUFF_CONDITION_LEAVESCENE_H__

#include "BuffCondition_Base.h"

class BuffCondition_LeaveScene:public BuffCondition_Base
{
public:

	BuffCondition_LeaveScene(bool bSpecialEffect):BuffCondition_Base(bSpecialEffect){}
	~BuffCondition_LeaveScene()
	{
		IEntity* pMaster = getEntityFromHandle(m_hMaster);
		if(pMaster)
		{
			pMaster->getEventServer()->unsubscribeEvent(EVENT_ENTITY_LEAVESCENE, this, &BuffCondition_LeaveScene::onEventMoveGrid);
		}
	}

	virtual bool initlize(HEntity hMaster, IEntityBuff* pBuff)
	{
		BuffCondition_Base::initlize(hMaster, pBuff);
		IEntity* pMaster = getEntityFromHandle(hMaster);
		assert(pMaster);
		
		pMaster->getEventServer()->subscribeEvent(EVENT_ENTITY_LEAVESCENE, this, &BuffCondition_LeaveScene::onEventMoveGrid);
	
		return true;
	}

	virtual BuffCondition_Base* clone()const{return new BuffCondition_LeaveScene(m_bSpecialEffect);}

	void onEventMoveGrid(EventArgs& args)
	{
		doTrigger();
	}

};


#endif

