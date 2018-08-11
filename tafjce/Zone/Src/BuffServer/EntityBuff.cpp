#include "BuffServerPch.h"
#include "EntityBuff.h"
#include "Condition/BuffCondition_Base.h"


#define BUFF_END_TIMER		0

EntityBuff::EntityBuff():m_iBuffID(0), m_iBuffGroupID(0), m_iBuffType(0), m_dwLifeRound(0), m_bPostAttackCount(0), m_iMaxOverlapCount(0),
	m_iRoundInteval(0), m_bShowClient(false), m_iSpecialEffectID(0), m_iOverlapCount(0), m_iLastRoundIndex(0), m_iEffectRoundCnt(0), m_bSysRelease(false)
{
}

EntityBuff::~EntityBuff()
{
	IEntity* pMaster = getEntityFromHandle(m_hMaster);
	assert(pMaster);

	// 在之前执行
	EventArgs args;
	args.context.setInt("entity", m_hMaster);
	args.context.setInt("buffid", m_iBuffID);
	pMaster->getEventServer()->setEvent(EVENT_ENTITY_RELEASEBUFF, args);
	
	pMaster->getEventServer()->unsubscribeEvent(EVENT_ENTITY_PRE_ATTACK, this, &EntityBuff::onEventPreAttack);
	pMaster->getEventServer()->unsubscribeEvent(EVENT_ENTITY_DEAD_WITHCONTEXT, this, &EntityBuff::onEventDead);

	pMaster->getEventServer()->unsubscribeEvent(EVENT_ENTITY_PREBEGIN_FIGHT, this, &EntityBuff::onBeginFight);

	if(m_bPostAttackCount)
	{
		pMaster->getEventServer()->unsubscribeEvent(EVENT_ENTITY_POST_ATTACK, this, &EntityBuff::onEventPostAttack);
	}

	// 删除条件对象
	for(ConditionList::iterator it = m_specialConditionList.begin(); it != m_specialConditionList.end(); it++)
	{
		BuffCondition_Base* pCond = *it;
		delete pCond;
	}
	m_specialConditionList.clear();

	// 暂时放在状态析构的时候处理吧，可能存在问题
	if(!m_bSysRelease)
	{
		for(int i = 0; i < m_iOverlapCount; i++)
		{
			endEffect();
		}
	}
}

void EntityBuff::setSysReleaseFlag()
{
	m_bSysRelease = true;
}


EntityBuff::EntityBuff(const EntityBuff& rhs)
{
	m_iBuffID = rhs.m_iBuffID;
	m_strBuffName = rhs.m_strBuffName;
	m_iBuffGroupID = rhs.m_iBuffGroupID;
	m_iBuffType = rhs.m_iBuffType;
	m_dwLifeRound = rhs.m_dwLifeRound;
	m_bPostAttackCount = rhs.m_bPostAttackCount;
	m_iMaxOverlapCount = rhs.m_iMaxOverlapCount;
	m_initEffectList = rhs.m_initEffectList;
	m_normalEffectList = rhs.m_normalEffectList;
	m_iRoundInteval = rhs.m_iRoundInteval;
	m_bShowClient = rhs.m_bShowClient;
	
	m_roundEffectList = rhs.m_roundEffectList;

	m_iSpecialEffectID = rhs.m_iSpecialEffectID;
	for(size_t i = 0; i <  rhs.m_specialConditionList.size(); i++)
	{
		const BuffCondition_Base* pCond = rhs.m_specialConditionList[i];
		assert(pCond);

		BuffCondition_Base* pNewCond = pCond->clone();
		m_specialConditionList.push_back(pNewCond);
	}

	m_iOverlapCount = rhs.m_iOverlapCount;
	m_iLastRoundIndex = rhs.m_iLastRoundIndex;
	m_iEffectRoundCnt = rhs.m_iEffectRoundCnt;

	m_bSysRelease = rhs.m_bSysRelease;
}


bool EntityBuff::initlize(HEntity hGiver, HEntity hMaster, int iInitOverlap, const EffectContext& preContext)
{
	IEntity* pMaster = getEntityFromHandle(hMaster);
	assert(pMaster);

	m_preContext = preContext;
	m_iOverlapCount = iInitOverlap;
	
	m_hMaster = hMaster;
	m_hGiver = hGiver;

	// 创建uuid
	uuid_t itemuuid;
	uuid_generate(itemuuid);
	char szUUIDString[1024] = {0};
	uuid_unparse_upper(itemuuid, szUUIDString);
	m_strUUID = szUUIDString;

	// 触发事件EVENT_ENTITY_NEWBUFF(仅用于战斗通知),在最开始执行，防止BeginEffect删除导致无通知
	EventArgs args;
	args.context.setInt("entity", hMaster);
	args.context.setInt("buffid", m_iBuffID);
	pMaster->getEventServer()->setEvent(EVENT_ENTITY_NEWBUFF, args);

	// 初始化条件对象
	for(ConditionList::iterator it = m_specialConditionList.begin(); it != m_specialConditionList.end(); it++)
	{
		BuffCondition_Base* pCondition = *it;
		assert(pCondition);

		pCondition->initlize(hMaster, this);
	}

	// 战斗状态，注册Pre生效事件	
	pMaster->getEventServer()->subscribeEvent(EVENT_ENTITY_PRE_ATTACK, this, &EntityBuff::onEventPreAttack);
	pMaster->getEventServer()->subscribeEvent(EVENT_ENTITY_DEAD_WITHCONTEXT, this, &EntityBuff::onEventDead);

	pMaster->getEventServer()->subscribeEvent(EVENT_ENTITY_PREBEGIN_FIGHT, this, &EntityBuff::onBeginFight);

	if(m_bPostAttackCount)
	{
		pMaster->getEventServer()->subscribeEvent(EVENT_ENTITY_POST_ATTACK, this, &EntityBuff::onEventPostAttack);
	}
	
	// 执行普通开始效果
	Detail::EventHandle::Proxy tmpPrx = getEventHandle();
	beginEffect();
	if(!tmpPrx.get() )
	{
		return true;
	}

	return true;
}

void EntityBuff::onBeginFight(EventArgs& args)
{
	vector<HEntity>* pBattleMemList = (vector<HEntity>*)args.context.getInt64("memberlist", (Int64)0);
	if(!pBattleMemList) return;

	m_fightMemberList = *pBattleMemList;
}

void EntityBuff::doSpecialEffect()
{
	IEntity* pMaster = getEntityFromHandle(m_hMaster);
	assert(pMaster);

	IEffectSystem* pEffectSystem = static_cast<IEffectSystem*>(pMaster->querySubsystem(IID_IEffectSystem) );
	if(!pEffectSystem) return;

	EffectContext tmpContext = m_preContext;
	tmpContext.setInt(PROP_EFFECT_BUFFID, (Int32)m_iBuffID);
	//tmpContext.setInt(PROP_EFFECT_OVERLAPCOUNT, (Int32)m_iOverlapCount);
	tmpContext.setInt(PROP_EFFECT_GIVER, (Int32)m_hGiver);
	tmpContext.setInt(PROP_EFFECT_ISBUFFEFFECT, (Int32)1);
	tmpContext.setInt64(PROP_EFFECT_FIGHTMEMBERLIST, (Int64)&m_fightMemberList);

	pEffectSystem->addEffect(m_iSpecialEffectID, tmpContext);
}


bool EntityBuff::isImMunityBuff(int iBuffID)
{
	IEffectFactory* pEffectFactory = getComponent<IEffectFactory>(COMPNAME_EffectFactory, IID_IEffectFactory);
	assert(pEffectFactory);

	EffectContext tmpContext;
	tmpContext.setInt(PROP_EFFECT_BUFFID, (Int32)m_iBuffID);
	//tmpContext.setInt(PROP_EFFECT_OVERLAPCOUNT, (Int32)m_iOverlapCount);
	tmpContext.setInt(PROP_EFFECT_GIVER, (Int32)m_hGiver);
	tmpContext.setInt(PROP_EFFECT_ISBUFFEFFECT, (Int32)1);

	for(EffectList::iterator it = m_initEffectList.begin(); it != m_initEffectList.end(); it++)
	{
		if(pEffectFactory->isImMunityBuff(iBuffID, *it, tmpContext))
		{
			return true;
		}
	}

	for(EffectList::iterator it = m_normalEffectList.begin(); it != m_normalEffectList.end(); it++)
	{
		if(pEffectFactory->isImMunityBuff(iBuffID, *it, tmpContext))
		{
			return true;
		}
	}

	return false;
}


void EntityBuff::beginEffect()
{
	IEntity* pMaster = getEntityFromHandle(m_hMaster);
	assert(pMaster);

	IEffectSystem* pEffectSystem = static_cast<IEffectSystem*>(pMaster->querySubsystem(IID_IEffectSystem) );
	assert(pEffectSystem);

	EffectContext tmpContext = m_preContext;
	tmpContext.setInt(PROP_EFFECT_BUFFID, (Int32)m_iBuffID);
	//tmpContext.setInt(PROP_EFFECT_OVERLAPCOUNT, (Int32)m_iOverlapCount);
	tmpContext.setInt(PROP_EFFECT_GIVER, (Int32)m_hGiver);
	tmpContext.setInt(PROP_EFFECT_ISBUFFEFFECT, (Int32)1);

	for(EffectList::iterator it = m_initEffectList.begin(); it != m_initEffectList.end(); it++)
	{
		Detail::EventHandle::Proxy tmpPrx = getEventHandle();
		int iTmpEffectID = *it;
		pEffectSystem->addEffect(iTmpEffectID, tmpContext);
		if(!tmpPrx.get() )
		{
			return;
		}
	}
	
	for(EffectList::iterator it = m_normalEffectList.begin(); it != m_normalEffectList.end(); it++)
	{	
		Detail::EventHandle::Proxy tmpPrx = getEventHandle();
		int iTmpEffectID = *it;
		pEffectSystem->addEffect(iTmpEffectID, tmpContext);
		if(!tmpPrx.get() )
		{
			return;
		}
	}
}

void EntityBuff::endEffect()
{
	IEntity* pMaster = getEntityFromHandle(m_hMaster);
	assert(pMaster);

	IEffectSystem* pEffectSystem = static_cast<IEffectSystem*>(pMaster->querySubsystem(IID_IEffectSystem) );
	assert(pEffectSystem);

	EffectContext tmpContext = m_preContext;
	tmpContext.setInt(PROP_EFFECT_BUFFID, (Int32)m_iBuffID);
	//tmpContext.setInt(PROP_EFFECT_OVERLAPCOUNT, (Int32)m_iOverlapCount);
	tmpContext.setInt(PROP_EFFECT_GIVER, (Int32)m_hGiver);
	tmpContext.setInt(PROP_EFFECT_NEGATIVE, (Int32)1);
	tmpContext.setInt(PROP_EFFECT_ISBUFFEFFECT, (Int32)1);
	tmpContext.setInt(PROP_EFFECT_ISBUFFENDEFFECT, (Int32)1);
	
	
	for(EffectList::iterator it = m_normalEffectList.begin(); it != m_normalEffectList.end(); it++)
	{
		Detail::EventHandle::Proxy tmpPrx = getEventHandle();
		int iTmpEffectID = *it;
		pEffectSystem->addEffect(iTmpEffectID, tmpContext);
		if(!tmpPrx.get() )
		{
			return;
		}
	}
}

void EntityBuff::roundEffect()
{
	IEntity* pMaster = getEntityFromHandle(m_hMaster);
	assert(pMaster);

	IEffectSystem* pEffectSystem = static_cast<IEffectSystem*>(pMaster->querySubsystem(IID_IEffectSystem) );
	assert(pEffectSystem);

	EffectContext tmpContext = m_preContext;
	tmpContext.setInt(PROP_EFFECT_BUFFID, (Int32)m_iBuffID);
	//tmpContext.setInt(PROP_EFFECT_OVERLAPCOUNT, (Int32)m_iOverlapCount);
	tmpContext.setInt(PROP_EFFECT_GIVER, (Int32)m_hGiver);
	tmpContext.setInt(PROP_EFFECT_ISBUFFEFFECT, (Int32)1);
	
	for(EffectList::iterator it = m_roundEffectList.begin(); it != m_roundEffectList.end(); it++)
	{
		Detail::EventHandle::Proxy tmpPrx = getEventHandle();
		int iTmpEffectID = *it;
		pEffectSystem->addEffect(iTmpEffectID, tmpContext);
		if(!tmpPrx.get() )
		{
			return;
		}
	}
}

void EntityBuff::onEventPreAttack(EventArgs& args)
{
	int iRound = args.context.getInt("round");

	// 首次需要生效,否则派段间隔
	Detail::EventHandle::Proxy tmpPrx = getEventHandle();
	if(-1 == m_iLastRoundIndex)
	{
		roundEffect();
	}
	else
	{
		if( (iRound - m_iLastRoundIndex) > m_iRoundInteval)
		{
			roundEffect();
		}
	}
	if(!tmpPrx.get() )
	{
		return;
	}

	
	m_iLastRoundIndex = iRound;

	// 行动前计数的，才+1
	if(!m_bPostAttackCount)
	{
		incEffectCount();
	}

	// 不要在这后面写代码了，状态自身可能被删掉了
}

void EntityBuff::onEventDead(EventArgs& args)
{
	delSelf();
}

void EntityBuff::onEventPostAttack(EventArgs& args)
{
	if(m_bPostAttackCount)
	{
		incEffectCount();
	}

	// 不要在这里写代码，状态自身可能被删除了
}

void EntityBuff::incEffectCount()
{
	m_iEffectRoundCnt++;
	
	if(m_iEffectRoundCnt >= (int)m_dwLifeRound)
	{
		delSelf();
	}
}

void EntityBuff::resetEffectRound()
{
	m_iEffectRoundCnt = 0;
}


int EntityBuff::getOverlapCount()
{
	return m_iOverlapCount;
}

int EntityBuff::getMaxOverlapCount()
{
	return m_iMaxOverlapCount;
}

Int32 EntityBuff::getBuffID() const
{
	return m_iBuffID;
}

IEntityBuff* EntityBuff::clone()const
{
	return new EntityBuff(*this);
}

string EntityBuff::getBuffUUID()
{
	return m_strUUID;
}

HEntity EntityBuff::getMaster()
{
	return m_hMaster;
}

HEntity EntityBuff::getGiver()
{
	return m_hGiver;
}

void EntityBuff::setOverlapCount(int iNewOverlapCount)
{
	if(iNewOverlapCount == m_iOverlapCount)
	{
		return;
	}
	
	if(iNewOverlapCount > m_iOverlapCount)
	{
		for(int i = 0; i < iNewOverlapCount - m_iOverlapCount; i++)
		{
			Detail::EventHandle::Proxy tmpPrx = getEventHandle();
			beginEffect();
			if(!tmpPrx.get() )
			{
				return;
			}
		}
	}
	else
	{
		for(int i = 0; i < m_iOverlapCount - iNewOverlapCount; i++)
		{
			Detail::EventHandle::Proxy tmpPrx = getEventHandle();
			endEffect();
			if(!tmpPrx.get() )
			{
				return;
			}
		}
	}

	m_iOverlapCount = iNewOverlapCount;
}

int EntityBuff::getBuffType() const
{
	return m_iBuffType;
}

void EntityBuff::delSelf()
{
	IEntity* pMaster = getEntityFromHandle(m_hMaster);
	if(!pMaster)
	{
		return;
	}

	IBuffSystem* pBuffSystem = static_cast<IBuffSystem*>(pMaster->querySubsystem(IID_IBuffSystem) );
	assert(pBuffSystem);
	pBuffSystem->delBuffByUUID(m_strUUID);
}

bool EntityBuff::isShowClient() const
{
	return m_bShowClient;
}


