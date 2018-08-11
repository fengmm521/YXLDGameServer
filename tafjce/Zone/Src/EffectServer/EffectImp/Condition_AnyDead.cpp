#include "EffectServerPch.h"
#include "EffectFactory.h"
#include "ErrorCodeDef.h"

#include "SkillEffect_CondEffect.h"
#include "Condition_AnyDead.h"

void Condition_AnyDead::registerEvent(HEntity hEntity, const EffectContext& ctx)
{
	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_BEGIN_ROUND, this, &Condition_AnyDead::onEventBeginRound);
	pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_PRE_RELEASE, this, &Condition_AnyDead::onEventPreRelease);
}

void Condition_AnyDead::unregisterEvent(HEntity hEntity)
{
	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_BEGIN_ROUND, this, &Condition_AnyDead::onEventBeginRound);
	pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_PRE_RELEASE, this, &Condition_AnyDead::onEventPreRelease);
}

bool Condition_AnyDead::parseConditon(vector<string>& paramList)
{
	assert(paramList.size() > 0);

	if(paramList[0] == "任何人死亡")
	{
		paramList.erase(paramList.begin() );

		return true;
	}

	return false;
}

void Condition_AnyDead::onEventBeginRound(EventArgs args)
{
	HEntity hMaster = args.context.getInt("entity");

	if(m_mapDeadEventMap.find(hMaster) == m_mapDeadEventMap.end() )
	{
		m_mapDeadEventMap[hMaster] = new AnyDeadEventHelper();
	}

	
	
	AnyDeadEventHelper* eventHelper = m_mapDeadEventMap[hMaster];
	eventHelper->m_hMaster = hMaster;
	eventHelper->m_pOwner = this;

	eventHelper->beginRound();
}


void Condition_AnyDead::onEventPreRelease(EventArgs& args)
{
	HEntity hEntity = args.context.getInt("entity");
	if(m_mapDeadEventMap.find(hEntity) != m_mapDeadEventMap.end() )
	{
		AnyDeadEventHelper* eventHelper = m_mapDeadEventMap[hEntity];
		delete eventHelper;
		m_mapDeadEventMap.erase(hEntity);
	}
}



void AnyDeadEventHelper::onEventAnyDead(EventArgs& args)
{
	EffectContext tmpCtx;
	m_pOwner->m_pCondEffect->doTriggerEffect(m_hMaster, tmpCtx);
}


void AnyDeadEventHelper::beginRound()
{
	// 先卸载原来的事件
	for(size_t i = 0; i < m_memberList.size(); i++)
	{
		IEntity* pMember = getEntityFromHandle(m_memberList[i]);
		if(!pMember) continue;

		if(m_hMaster == m_memberList[i]) continue;

		pMember->getEventServer()->unsubscribeEvent(EVENT_ENTITY_EFFECTBEKILL, this, &AnyDeadEventHelper::onEventAnyDead);
	}

	// 注册新的
	IFightFactory* pFightFactory = getComponent<IFightFactory>(COMPNAME_FightFactory, IID_IFightFactory);
	assert(pFightFactory);

	const vector<HEntity>* pMemberList = pFightFactory->getMemberList();
	assert(pMemberList);

	m_memberList = *pMemberList;

	for(size_t i = 0; i < pMemberList->size(); i++)
	{
		if(m_hMaster == m_memberList[i]) continue;
	
		IEntity* pMember = getEntityFromHandle((*pMemberList)[i]);
		if(!pMember) continue;

		pMember->getEventServer()->subscribeEvent(EVENT_ENTITY_EFFECTBEKILL, this, &AnyDeadEventHelper::onEventAnyDead);
	}
}



