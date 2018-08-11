#include "CoreImpPch.h"
#include "../GameEngine/Entity.h"
#include "Monster.h"


extern "C" IObject* createMonster()
{
	return new Monster;
}


Monster::Monster():m_bFinished(false)
{
	setProperty(PROP_ENTITY_CLASS, GSProto::en_class_Monster);
}

bool Monster::createEntity(const std::string& strData)
{
	ServerEngine::MNCreateContext monsterCreateCtx;
	ServerEngine::JceToObj(strData, monsterCreateCtx);

	setProperty(PROP_ENTITY_BASEID, monsterCreateCtx.nBaseId);

	initBasePop(monsterCreateCtx.nBaseId);
	initMonsterSubSystem(strData);
	completeSubsystem();

	// 设置怪物的HP
	int iMaxHP = getProperty(PROP_ENTITY_MAXHP, 0);
	setProperty(PROP_ENTITY_HP, iMaxHP);

	// 设置下怒气
	int iInitAnger = getProperty(PROP_ENTITY_INITANGER, 0);
	setProperty(PROP_ENTITY_ANGER, iInitAnger);

	m_bFinished = true;

	return true;
}


void Monster::initMonsterSubSystem(const std::string& strData)
{
	for(SubsystemMap::iterator it = m_subsystemMap.begin(); it != m_subsystemMap.end(); it++)
	{
		IEntitySubsystem* pSubSystem = it->second;
		assert(pSubSystem);
		
		if(!pSubSystem->create(this, strData) )
		{
			assert(false);
		}
	}
}


void Monster::initBasePop(int iMonsterID)
{
	ITable* pMonsterTb = getCompomentObjectManager()->findTable("Monster");
	assert(pMonsterTb);

	int iRecord = pMonsterTb->findRecord(iMonsterID);
	if(iRecord < 0)
	{
		assert(false);
		return;
	}

	int iBaseMaxHP = pMonsterTb->getInt(iRecord, "生命");
	int iBaseAtt = pMonsterTb->getInt(iRecord, "攻击");
	int iBaseHit = pMonsterTb->getInt(iRecord, "命中");
	int iBaseDoge = pMonsterTb->getInt(iRecord, "闪避");
	int iBaseKnock = pMonsterTb->getInt(iRecord, "暴击");
	int iBaseAntiKnock = pMonsterTb->getInt(iRecord, "韧性");
	int iBaseBlock = pMonsterTb->getInt(iRecord, "抵挡");
	int iBaseWreck = pMonsterTb->getInt(iRecord, "破击");
	int iBaseArmor = pMonsterTb->getInt(iRecord, "护甲");
	int iBaseSunder = pMonsterTb->getInt(iRecord, "破甲");
	int iBaseInitAnger = pMonsterTb->getInt(iRecord, "初始怒气");
	int iLevel = pMonsterTb->getInt(iRecord, "等级");
	int iLevelStep = pMonsterTb->getInt(iRecord, "怪物等阶");
	int iSex = pMonsterTb->getInt(iRecord, "性别");

	setProperty(PROP_ENTITY_BASEMAXHP, iBaseMaxHP);
	setProperty(PROP_ENTITY_BASEATT, iBaseAtt);
	setProperty(PROP_ENTITY_BASEHIT, iBaseHit);
	setProperty(PROP_ENTITY_BASEDOGE, iBaseDoge);
	setProperty(PROP_ENTITY_BASEKNOCK, iBaseKnock);
	setProperty(PROP_ENTITY_BASEANTIKNOCK, iBaseAntiKnock);
	setProperty(PROP_ENTITY_BASEBLOCK, iBaseBlock);
	setProperty(PROP_ENTITY_BASEWRECK, iBaseWreck);
	setProperty(PROP_ENTITY_BASEARMOR, iBaseArmor);
	setProperty(PROP_ENTITY_BASESUNDER, iBaseSunder);
	setProperty(PROP_ENTITY_BASEINITANGER, iBaseInitAnger);
	setProperty(PROP_ENTITY_LEVEL, iLevel);
	setProperty(PROP_ENTITY_LEVELSTEP, iLevelStep);
	setProperty(PROP_ENTITY_SEX, iSex);

	setProperty(PROP_ENTITY_BASEPROP_FIXPARAM, 10000);
}

void Monster::setProperty(PropertySet::PropertyKey key, Int32 nValue)
{	
	// HP/MaxHP 处理
	if( ( (PROP_ENTITY_HP == key) || (PROP_ENTITY_MAXHP == key) ) && (nValue < 0) )
	{
		nValue = 0;
	}

	if(PROP_ENTITY_HP == key)
	{
		int iMaxHP = getProperty(PROP_ENTITY_MAXHP, 0);
		if(nValue > iMaxHP)
		{
			nValue = iMaxHP;
		}
	}

	int iOrgValue = getProperty(key, 0);
	Entity::setProperty(key, nValue);

	// MaxHP处理
	if( (PROP_ENTITY_MAXHP == key) && m_bFinished)
	{
		int iMaxHP = getProperty(PROP_ENTITY_MAXHP, 0);
		int iCurHP = getProperty(PROP_ENTITY_HP, 0);

		// 如果生命上限+了，生命也要+
		if(nValue > iOrgValue)
		{
			setProperty(PROP_ENTITY_HP, iCurHP + (nValue - iOrgValue) );
			iCurHP = getProperty(PROP_ENTITY_HP, 0);
		}
		
		if(iCurHP > iMaxHP)
		{
			Entity::setProperty(PROP_ENTITY_HP, iMaxHP);
		}
	}
}



