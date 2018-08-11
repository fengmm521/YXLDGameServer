#ifndef __BUFFCONDITION_DEAD_H__
#define __BUFFCONDITION_DEAD_H__

#include "BuffCondition_Base.h"

class BuffCondition_Dead:public BuffCondition_Base
{
public:

	BuffCondition_Dead(bool bSpecialEffect, bool bSelfMember):BuffCondition_Base(bSpecialEffect),
		m_bSelfMember(bSelfMember){}
	
	~BuffCondition_Dead()
	{
		IEntity* pMaster = getEntityFromHandle(m_hMaster);
		if(!pMaster) return;

		pMaster->getEventServer()->unsubscribeEvent(EVENT_ENTITY_BEGIN_FIGHT, this, &BuffCondition_Dead::onEventBeginFight);

		for(size_t i = 0; i < m_memberList.size(); i++)
		{
			HEntity hMember = m_memberList[i];
			IEntity* pMember = getEntityFromHandle(hMember);
			if(!pMember) continue;

			pMember->getEventServer()->unsubscribeEvent(EVENT_ENTITY_DEAD_WITHCONTEXT, this, &BuffCondition_Dead::onEventMemberDead);
		}
	}

	virtual bool initlize(HEntity hMaster, IEntityBuff* pBuff)
	{		
		BuffCondition_Base::initlize(hMaster, pBuff);

		IEntity* pMaster = getEntityFromHandle(hMaster);
		if(!pMaster) return false;

		pMaster->getEventServer()->subscribeEvent(EVENT_ENTITY_BEGIN_FIGHT, this, &BuffCondition_Dead::onEventBeginFight);
		
		return true;
	}


	void onEventBeginFight(EventArgs& args)
	{
		IFightFactory::BattleList* pMemberList = (IFightFactory::BattleList*)args.context.getInt64("memberlist", 0);
		assert(pMemberList);

		m_memberList = *pMemberList;

		for(size_t i = 0; i < m_memberList.size(); i++)
		{
			HEntity hMember = m_memberList[i];
			IEntity* pMember = getEntityFromHandle(hMember);
			if(!pMember) continue;

			pMember->getEventServer()->subscribeEvent(EVENT_ENTITY_DEAD_WITHCONTEXT, this, &BuffCondition_Dead::onEventMemberDead);
		}
	}

	void onEventMemberDead(EventArgs& args)
	{
		HEntity hDeadEntity = args.context.getInt("entity");
		IEntity* pDeadEntity = getEntityFromHandle(hDeadEntity);
		if(!pDeadEntity) return;

		IEntity* pMaster = getEntityFromHandle(m_hMaster);
		if(!pMaster) return;

		int iDeadBattlePos = pDeadEntity->getProperty(PROP_ENTITY_BATTLEPOS, 0);
		int iMasterPos = pMaster->getProperty(PROP_ENTITY_BATTLEPOS, 0);

		int iDeadTeam = iDeadBattlePos/(CProto::MAX_BATTLE_MEMBER_SIZE/2);
		int iMasterTeam = -1;

		if(iMasterPos < CProto::MAX_BATTLE_MEMBER_SIZE)
		{
			iMasterTeam = iMasterPos/(CProto::MAX_BATTLE_MEMBER_SIZE/2);
		}
		else if( (iMasterPos == CProto::FIGHT_SPECIALPOS_ATTMAGICGHOST) || (iMasterPos == CProto::FIGHT_SPECIALPOS_ATTRESERVE) )
		{
			iMasterTeam = 0;
		}
		else if( (iMasterPos == CProto::FIGHT_SPECIALPOS_DEFMAGICGHOST) || (iMasterPos == CProto::FIGHT_SPECIALPOS_DEFRESERVE) )
		{
			iMasterTeam = 1;
		}

		if(iMasterTeam < 0)
		{
			return;
		}
		
		if(m_bSelfMember && (iMasterTeam == iDeadTeam) )
		{
			doTrigger();
		}
		else if(!m_bSelfMember && (iMasterTeam != iDeadTeam) )
		{
			doTrigger();
		}
	}

	virtual BuffCondition_Base* clone()const{return new BuffCondition_Dead(m_bSpecialEffect, m_bSelfMember);}

private:

	bool m_bSelfMember;
	IFightFactory::BattleList m_memberList;
};


#endif

