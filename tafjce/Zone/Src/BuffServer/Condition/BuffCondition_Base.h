#ifndef __BUFF_CONDITION_BASE_H__
#define __BUFF_CONDITION_BASE_H__

class BuffCondition_Base: public Detail::EventHandle
{
public:

	BuffCondition_Base(bool bSpecialEffect):m_bSpecialEffect(bSpecialEffect), m_iCurrentRound(0){}
	virtual ~BuffCondition_Base()
	{
		IEntity* pMaster = getEntityFromHandle(m_hMaster);
		if(pMaster)
		{
			pMaster->getEventServer()->unsubscribeEvent(EVENT_ENTITY_BEGIN_ROUND, this, &BuffCondition_Base::onEventBeginRound);
		}
	}

	virtual bool initlize(HEntity hMaster, IEntityBuff* pBuff)
	{
		assert(pBuff);
		m_hMaster = hMaster;
		m_strBuffUUID = pBuff->getBuffUUID();

		IEntity* pMaster = getEntityFromHandle(hMaster);
		if(pMaster)
		{
			pMaster->getEventServer()->subscribeEvent(EVENT_ENTITY_BEGIN_ROUND, this, &BuffCondition_Base::onEventBeginRound);
		}

		return true;
	}

	virtual void releaseBuff()
	{
		IEntity* pMaster = getEntityFromHandle(m_hMaster);
		if(!pMaster)
		{
			return;
		}

		IBuffSystem* pBuffSystem = static_cast<IBuffSystem*>(pMaster->querySubsystem(IID_IBuffSystem) );
		assert(pBuffSystem);

		pBuffSystem->delBuffByUUID(m_strBuffUUID);
	}

	virtual void doSpecialEffect()
	{
		IEntity* pMaster = getEntityFromHandle(m_hMaster);
		if(!pMaster)
		{
			return;
		}

		IBuffSystem* pBuffSystem = static_cast<IBuffSystem*>(pMaster->querySubsystem(IID_IBuffSystem) );
		assert(pBuffSystem);

		IEntityBuff* pEntityBuff = pBuffSystem->getBuff(m_strBuffUUID);
		if(!pEntityBuff) return;

		pEntityBuff->doSpecialEffect();
	}

	void doTrigger()
	{
		if(m_bSpecialEffect)
		{
			doSpecialEffect();
		}
		else
		{
			releaseBuff();
		}
	}

	void onEventBeginRound(EventArgs& args)
	{
		m_iCurrentRound = args.context.getInt("round");
	}

	virtual BuffCondition_Base* clone()const {return NULL;}

protected:

	HEntity m_hMaster;
	string m_strBuffUUID;
	bool m_bSpecialEffect;
	int m_iCurrentRound;
};


#endif
