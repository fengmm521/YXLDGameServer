#include "SkillServerPch.h"
#include "Skill.h"
#include "SkillFactory.h"
#include "IEffectSystem.h"

Skill::Skill():m_nSkillID(0), m_iActiveLevelStep(0), m_iActiveQuality(0), m_nSkillType(0),  m_iBatterAttack(0), 
	m_iSkillGroupID(0), m_nSkillLv(0), m_iMaxLvl(1), m_iCDRound(0), m_bCanBackAttack(false), m_iSmallSkillChance(0), m_iNeedAnger(0), m_iConsumeAnger(0),
	m_iLastUseRound(0), m_iCurrentRound(0), m_pSkillSystem(NULL)
{
}

Skill::~Skill()
{
	for(size_t i = 0; i < m_conditionList.size(); i++)
	{
		delete m_conditionList[i];
	}
	m_conditionList.clear();

	if(m_iCDRound)
	{
		unRegisterFightEvent();
	}

	for(size_t i = 0; i <m_specialResultList.size(); i++)
	{
		delete m_specialResultList[i];
	}

	// 如果已经挂载到玩家，并且是被动技能。需要清除效果
	if( (en_SkillType_Talent == getSkillType() ) && (m_pSkillSystem) )
	{
		usePassiveSkillNeg();
	}
}

Skill::Skill(const Skill& rhs)
{
	m_nSkillID = rhs.m_nSkillID;
	m_iActiveLevelStep = rhs.m_iActiveLevelStep;
	m_iActiveQuality = rhs.m_iActiveQuality;
	m_nSkillType = rhs.m_nSkillType;

	m_iBatterAttack = rhs.m_iBatterAttack;
	m_iSkillGroupID   = rhs.m_iSkillGroupID;
	m_nSkillLv = rhs.m_nSkillLv;
	m_iMaxLvl = rhs.m_iMaxLvl;
	m_iCDRound = rhs.m_iCDRound;
	m_bCanBackAttack = rhs.m_bCanBackAttack;
	m_iSmallSkillChance = rhs.m_iSmallSkillChance;
	
	for(size_t i = 0; i < rhs.m_conditionList.size(); i++)
	{
		SkillCondition_Base* pNewCond = rhs.m_conditionList[i]->clone();
		m_conditionList.push_back(pNewCond);
	}

	for(size_t i = 0; i < rhs.m_specialResultList.size(); i++)
	{
		SkillResult_Base* pNewResult = rhs.m_specialResultList[i]->clone();
		m_specialResultList.push_back(pNewResult);
	}
	
	m_effectList = rhs.m_effectList;
	m_iNeedAnger = rhs.m_iNeedAnger;
	m_iConsumeAnger = rhs.m_iConsumeAnger;
	m_iLastUseRound = rhs.m_iLastUseRound;
	m_iCurrentRound = rhs.m_iCurrentRound;
	m_pSkillSystem = NULL;
}


bool Skill::init(ISkillSystem* pSkillSystem)
{
	m_pSkillSystem = pSkillSystem;

	if( (en_SkillType_Talent == getSkillType() ) && isActive() )
	{
		usePassiveSkill();
	}

	// 如果有冷却回合，注册战斗开始、开始攻击事件
	if(m_iCDRound)
	{
		registerFightEvent();
	}
	
	return true;
}



bool Skill::isActive()
{
	if(!m_pSkillSystem) return false;

	HEntity hMaster = m_pSkillSystem->getMasterHandle();
	IEntity* pMaster = getEntityFromHandle(hMaster);
	if(!pMaster) return false;

	// 怪物无限制
	int iClassID = pMaster->getProperty(PROP_ENTITY_CLASS, 0);
	if(GSProto::en_class_Monster == iClassID)
	{
		return true;
	}
	
	int iLevelStep = pMaster->getProperty(PROP_ENTITY_LEVELSTEP, 0);

	if(iLevelStep < m_iActiveLevelStep)
	{
		return false;
	}

	int iQuality = pMaster->getProperty(PROP_ENTITY_QUALITY, 0);

	return iQuality >= m_iActiveQuality;
}

void Skill::onLevelStepChg(int iOldLevelStep, int iNewLevelStep)
{
	// 这里只处理被动技能
	if(en_SkillType_Talent != getSkillType() )
	{
		return;
	}

	if( (iOldLevelStep < m_iActiveLevelStep) && (iNewLevelStep >= m_iActiveLevelStep) )
	{
		usePassiveSkill();
		return;
	}

	// 降级，执行反效果
	if( (iOldLevelStep >= m_iActiveLevelStep) && (iNewLevelStep < m_iActiveLevelStep) )
	{
		usePassiveSkillNeg();
	}
}


void Skill::usePassiveSkillNeg()
{
	EffectContext preContext;
	preContext.setInt(PROP_EFFECT_NEGATIVE, 1);
	preContext.setInt(PROP_EFFECT_DELTALENT, 1);
	doCommonEffect(NULL, preContext);
	doSpecialResult(NULL, preContext);
}


void Skill::usePassiveSkill()
{
	EffectContext preContext;
	doCommonEffect(NULL, preContext);
	doSpecialResult(NULL, preContext);
}

void Skill::registerFightEvent()
{
	HEntity hMaster = m_pSkillSystem->getMasterHandle();
	IEntity* pEntity = getEntityFromHandle(hMaster);
	assert(pEntity);

	pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_BEGIN_FIGHT, this, &Skill::onEventBeginFight);
	pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_BEGIN_ROUND, this, &Skill::onEventBeginRound);
}

void Skill::unRegisterFightEvent()
{
	HEntity hMaster = m_pSkillSystem->getMasterHandle();
	IEntity* pEntity = getEntityFromHandle(hMaster);
	assert(pEntity);

	pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_BEGIN_FIGHT, this, &Skill::onEventBeginFight);
	pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_BEGIN_ROUND, this, &Skill::onEventBeginRound);
}

void Skill::onEventBeginFight(EventArgs& args)
{
	// 清空历史战斗数据
	m_iCurrentRound = 0;
	m_iLastUseRound = -1;
}

void Skill::onEventBeginRound(EventArgs& args)
{
	m_iCurrentRound = args.context.getInt("round");
}

bool Skill::canUseSkill(const vector<HEntity>& targetItem)
{
	if(!isActive() ) return false;

	// 取消技能回合准备
	// 判断冷却(按照回合处理)
	if(m_iCDRound && (m_iLastUseRound >= 0)&&( (m_iCurrentRound - m_iLastUseRound) <= m_iCDRound) )
	{
		return false;
	}

	// 验证特殊条件
	HEntity hMaster = m_pSkillSystem->getMasterHandle();
	EffectContext tmpContext;
	tmpContext.setInt(PROP_EFFECT_USESKILL_ID, (Int32)m_nSkillID);
	tmpContext.setInt(PROP_EFFECT_USESKILL_LV, (Int32)m_nSkillLv);
	tmpContext.setInt(PROP_EFFECT_GIVER, (Int32)hMaster);
	tmpContext.setInt64(PROP_EFFECT_FIGHTMEMBERLIST, (Int64)&targetItem);
	for(size_t i = 0; i < m_conditionList.size(); i++)
	{
		if(m_conditionList[i]->checkCondition(hMaster, tmpContext) == false)
		{
			return false;
		}
	}

	// 判断技能怒气
	IEntity* pMaster = getEntityFromHandle(hMaster);
	assert(pMaster);

	int iTkAnger = pMaster->getProperty(PROP_ENTITY_ANGER, (Int32)0);
	if(iTkAnger < m_iNeedAnger)
	{
		return false;
	}

	if(iTkAnger >= m_iNeedAnger)
	{
		return true;
	}

	return true;
}

bool Skill::useSkill(const vector<HEntity>& memberList, const EffectContext& preContext)
{
	// 法宝、昏迷状态下，目标选择需要特殊处理,要预处理选择目标所使用位置
	// 修改:这类规则转移到目标选择策略处理
	
	m_iLastUseRound = m_iCurrentRound;

	// 被动技能,不走普通流程
	if(en_SkillType_Talent == getSkillType() )
	{
		usePassiveSkill();
		return true;
	}

	ISkillFactory* pSkillFactory = getComponent<ISkillFactory>("SkillFactory", IID_ISkillFactory);
	assert(pSkillFactory);

	HEntity hMaster = m_pSkillSystem->getMasterHandle();
	IEntity* pMaster = getEntityFromHandle(hMaster);
	assert(pMaster);

	// 扣除怒气(普通攻击应该配置为0)
	pMaster->changeProperty(PROP_ENTITY_ANGER, 0 - m_iConsumeAnger, 0);
	
	EventArgs args;
	args.context.setInt("entity", hMaster);
	args.context.setInt("SkillID", m_nSkillID);
	args.context.setInt("SkillType", m_nSkillType);
	if(1 == m_iBatterAttack)
	{
		args.context.setInt("isBatchPlay", 0);
	}
	else
	{
		args.context.setInt("isBatchPlay", 1);
	}

	bool bBackAttack = preContext.getInt(PROP_EFFECT_ISBACKATTACK) == 1;
	args.context.setInt("isBackAttack", (int)bBackAttack);
	
	args.context.setInt("round", m_iCurrentRound);
	pMaster->getEventServer()->setEvent(EVENT_ENTITY_USESKILL, args);

	bool bContKill = false;
	bool bContinueSkill = false;
	int iContinueRate = pMaster->getProperty(PROP_ENTITY_CONTINUESKILL_RATE, 0);
	bool bUsedContinueRate = false;

	IRandom* pRandom = getComponent<IRandom>(COMPNAME_Random, IID_IMiniAprRandom);
	assert(pRandom);
	
	do
	{
		// 如果是连杀, 需要触发连杀事件
		bContKill = pMaster->getProperty(PROP_ENTITY_CONTINUEKILL, 0) != 0;
		
		if(bContKill || bContinueSkill)
		{
			// 重置连杀标记,连击一次只能用一回，无需重置
			pMaster->setProperty(PROP_ENTITY_CONTINUEKILL, 0);
		
			// 触发事件，通知战斗模块新启一个ResultSet
			EventArgs args;
			args.context.setInt("entity", hMaster);
			args.context.setInt("skillid", m_nSkillID);
			args.context.setInt("continuekill", bContKill);
			
			pMaster->getEventServer()->setEvent(EVENT_ENTITY_CONTINUESKILL, args);

			bContKill = false;
			bContinueSkill = false;
		}
	
		doCommonEffect(&memberList, preContext);
		doSpecialResult(&memberList, preContext);

		bContKill = pMaster->getProperty(PROP_ENTITY_CONTINUEKILL, 0) != 0;

		// 有连击率，随机下
		if( (!bContKill && !bContinueSkill) && !bUsedContinueRate)
		{
			int iRandV = pRandom->random() % 10000;
			if(iRandV < iContinueRate)
			{
				bContinueSkill = true;
				bUsedContinueRate = true;
			}
		}

		//只有连杀才会需要判断目标是否存在，普通技能可能memberList为空(例如反击是固定目标，memberList为空)
		if(bContKill || bContinueSkill)
		{
			vector<HEntity> tmpResultList;
			pSkillFactory->selectTarget(hMaster, en_SkillTargetSelect_Normal, tmpResultList, memberList);

			// 避免下一回合播放连杀
			if(tmpResultList.size() == 0)
			{
				pMaster->setProperty(PROP_ENTITY_CONTINUEKILL, 0);
				break;
			}
		}
	}while(bContKill || bContinueSkill);

	
	if( (getSkillType() == en_SkillType_SmallActive) && (pMaster->getProperty(PROP_ENTITY_HP, 0) > 0) )
	{
		IGlobalCfg* pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
		assert(pGlobal);
		int iAddValue = pGlobal->getInt("攻击增加怒气", 20);
		int iMaxAngerValue = pGlobal->getInt("最大怒气", 100);
		
		pMaster->changeProperty(PROP_ENTITY_ANGER, iAddValue, 0);
		if(pMaster->getProperty(PROP_ENTITY_ANGER, 0) > iMaxAngerValue)
		{
			pMaster->setProperty(PROP_ENTITY_ANGER, iMaxAngerValue);
		}
	}

	pMaster->getEventServer()->setEvent(EVENT_ENTITY_USESKILL_FINISH, args);
	
	return true;
}



void Skill::doCommonEffect(const vector<HEntity>* pMemberList, const EffectContext& preContext)
{
	HEntity hMaster = m_pSkillSystem->getMasterHandle();
	IEntity* pMaster = getEntityFromHandle(hMaster);
	assert(pMaster);

	IEffectSystem* pMasterEffectSystem = static_cast<IEffectSystem*>(pMaster->querySubsystem(IID_IEffectSystem) );
	if(!pMasterEffectSystem) return;
	
	EffectContext tmpContext = preContext;
	tmpContext.setInt(PROP_EFFECT_USESKILL_ID, (Int32)m_nSkillID);
	tmpContext.setInt(PROP_EFFECT_USESKILL_LV, (Int32)m_nSkillLv);
	tmpContext.setInt(PROP_EFFECT_GIVER, (Int32)hMaster);
	tmpContext.setInt64(PROP_EFFECT_FIGHTMEMBERLIST, (Int64)pMemberList);

	for(size_t i = 0; i < m_effectList.size(); i++)
	{
		int iTmpEffectID = m_effectList[i];
		pMasterEffectSystem->addEffect(iTmpEffectID, tmpContext);

		// 如果自身已经死亡中断
		int iHP = pMaster->getProperty(PROP_ENTITY_HP, 0);
		if(0 == iHP) break;
	}
}


void Skill::doSpecialResult(const vector<HEntity>* pMemberList, const EffectContext& preContext)
{
	HEntity hMaster = m_pSkillSystem->getMasterHandle();
	IEntity* pMaster = getEntityFromHandle(hMaster);
	assert(pMaster);

	IEffectSystem* pMasterEffectSystem = static_cast<IEffectSystem*>(pMaster->querySubsystem(IID_IEffectSystem) );
	if(!pMasterEffectSystem) return;
	
	for(std::vector<SkillResult_Base*>::iterator it = m_specialResultList.begin(); it != m_specialResultList.end(); it++)
	{
		SkillResult_Base* pSkillResultBase = *it;
		assert(pSkillResultBase);
		
		EffectContext tmpContext = preContext;
		tmpContext.setInt(PROP_EFFECT_USESKILL_ID, (Int32)m_nSkillID);
		tmpContext.setInt(PROP_EFFECT_USESKILL_LV, (Int32)m_nSkillLv);
		tmpContext.setInt(PROP_EFFECT_GIVER, (Int32)hMaster);
		tmpContext.setInt64(PROP_EFFECT_FIGHTMEMBERLIST, (Int64)pMemberList);
		pSkillResultBase->doResult(hMaster, tmpContext);
	}
}

ISkill* Skill::clone() const
{
	return new Skill(*this);
}

void Skill::packData(int& skillData)
{
	skillData = m_nSkillID;
}



