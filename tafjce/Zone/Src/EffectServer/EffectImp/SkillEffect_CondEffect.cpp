#include "EffectServerPch.h"
#include "EffectFactory.h"
#include "ErrorCodeDef.h"

#include "SkillEffect_CondEffect.h"
#include "Condition_HitResult.h"
#include "Condition_PreDead.h"
#include "Condition_Round.h"
#include "Condition_Kill.h"
#include "Condition_AnyDead.h"
#include "Condition_FightBegin.h"



void SkillEffect_CondEffect::doTriggerEffect(HEntity hEntity, const EffectContext& preCtx)
{
	EffectContext tmpContext = preCtx;
	
	tmpContext.setInt(PROP_EFFECT_GIVER, (Int32)hEntity);

	IFightFactory* pFightFactory = getComponent<IFightFactory>(COMPNAME_FightFactory, IID_IFightFactory);
	assert(pFightFactory);

	const vector<HEntity>* pMemberList = pFightFactory->getMemberList();
	tmpContext.setInt64(PROP_EFFECT_FIGHTMEMBERLIST, (Int64)pMemberList);

	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	IEffectSystem* pEffectSys = static_cast<IEffectSystem*>(pEntity->querySubsystem(IID_IEffectSystem) );
	assert(pEffectSys);

	assert(m_mapContextMap.find(hEntity) != m_mapContextMap.end() );

	vector<int>* pEffectIDList = (vector<int>*)m_mapContextMap[hEntity].getInt64(PROP_EFFECT_CONDITIONEFFECT_EFFECT, 0);
	assert(pEffectIDList);

	int iLimitCount = m_mapContextMap[hEntity].getInt(PROP_EFFECT_CONDITIONEFFECT_ROUND_LIMITTRIGGER);

	// 回合限制次数
	if(iLimitCount > 0)
	{
		int iTriggeredCount = m_mapContextMap[hEntity].getInt(PROP_EFFECT_CONDITIONEFFECT_TRIGGERCOUNT);
		int iLastTriggerRound = m_mapContextMap[hEntity].getInt(PROP_EFFECT_CONDITIONEFFECT_LASTROUND);

		int iCurRound = pFightFactory->getCurrentRound();
		if(iCurRound == iLastTriggerRound)
		{
			if(iTriggeredCount >= iLimitCount)
			{
				return;
			}
			else
			{
				m_mapContextMap[hEntity].setInt(PROP_EFFECT_CONDITIONEFFECT_TRIGGERCOUNT, iTriggeredCount+1);
			}
		}
		else
		{
			m_mapContextMap[hEntity].setInt(PROP_EFFECT_CONDITIONEFFECT_TRIGGERCOUNT, 1);
			m_mapContextMap[hEntity].setInt(PROP_EFFECT_CONDITIONEFFECT_LASTROUND, iCurRound);
		}
	}

	for(size_t i = 0; i < pEffectIDList->size(); i++)
	{
		pEffectSys->addEffect((*pEffectIDList)[i], tmpContext);
	}
}


void SkillEffect_CondEffect::onEventPreRelease(EventArgs& args)
{
	HEntity hEntity = args.context.getInt("entity");

	if(m_mapContextMap.find(hEntity) == m_mapContextMap.end() )
	{
		return;
	}

	Condition_Base* pCondition = (Condition_Base*)(m_mapContextMap[hEntity].getInt64(PROP_EFFECT_CONDITIONEFFECT_CONDITION, 0) );
	if(pCondition)
	{
		pCondition->unregisterEvent(hEntity);
	}
	
	m_mapContextMap.erase(hEntity);
}


bool SkillEffect_CondEffect::doSignleEffect(HEntity hEntity, const EffectContext& effectContext)
{
	//assert(m_pTriggerCondition);

	
	//context.setInt64(PROP_EFFECT_CONDITIONEFFECT_CONDITION, (Int64)pCondition);
	//context.setInt64(PROP_EFFECT_CONDITIONEFFECT_EFFECT, (Int64)pEffectIDList);

	Condition_Base* pCondition = (Condition_Base*)(effectContext.getInt64(PROP_EFFECT_CONDITIONEFFECT_CONDITION, 0) );
	assert(pCondition);

	bool isNegativeEffect = effectContext.getInt(PROP_EFFECT_NEGATIVE) == 1;

	// 注意，效果系统不负责任何情况下执行负效果
	if(!isNegativeEffect)
	{
		pCondition->registerEvent(hEntity, effectContext);
		m_mapContextMap[hEntity] = effectContext;
	}
	else
	{
		pCondition->unregisterEvent(hEntity);
		m_mapContextMap.erase(hEntity);

		IEntity* pEntity = getEntityFromHandle(hEntity);
		assert(pEntity);

		pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_PRE_RELEASE, this, &SkillEffect_CondEffect::onEventPreRelease);
	}

	return true;
}

string SkillEffect_CondEffect::concatString(const vector<string>& paramList)
{
	string strResult;
	for(size_t i = 0; i < paramList.size(); i++)
	{
		if(0 == i)
		{
			strResult = paramList[i];
		}
		else
		{
			strResult += string("#") + paramList[i];
		}
	}

	return strResult;
}


bool SkillEffect_CondEffect::parseEffect(const std::string& strEffectCmd, EffectContext& context)
{
	vector<string> paramList = TC_Common::sepstr<string>(strEffectCmd, Effect_MagicSep);
	assert(paramList.size() > 0);

	context.setInt(PROP_EFFECT_TYPE, (Int32)getEffectType() );

	Condition_Base* pCondition = NULL;
	vector<int>* pEffectIDList = new vector<int>();
	if(paramList[0] == "条件效果")
	{
		paramList.erase(paramList.begin() );

		assert(paramList.size() > 0);
		string strTriggerLimit = paramList.front();
		paramList.erase(paramList.begin() );
		
		int iTriggerLimit = AdvanceAtoi(strTriggerLimit);
		
		if( (paramList[0] == "暴击") || (paramList[0] == "攻击") || (paramList[0] == "闪避") || (paramList[0] == "抵挡") )
		{
			pCondition = new Condition_HitResult(this);
		}
		else if(paramList[0] == "死亡")
		{
			pCondition = new Condition_PreDead(this);
		}
		else if(paramList[0] == "间隔回合")
		{
			pCondition = new Condition_Round(this);
		}
		else if(paramList[0] == "击杀")
		{
			pCondition = new Condition_Kill(this);
		}
		else if(paramList[0] == "任何人死亡")
		{
			pCondition = new Condition_AnyDead(this);
		}
		else if(paramList[0] == "开始战斗")
		{
			pCondition = new Condition_FightBegin(this);
		}
		else
		{
			return false;
		}
		
		
		bool bResult = pCondition->parseConditon(paramList);
		assert(bResult);

		for(size_t i = 0; i < paramList.size(); i++)
		{
			pEffectIDList->push_back(AdvanceAtoi(paramList[i]) );
		}

		context.setInt64(PROP_EFFECT_CONDITIONEFFECT_CONDITION, (Int64)pCondition);
		context.setInt64(PROP_EFFECT_CONDITIONEFFECT_EFFECT, (Int64)pEffectIDList);
		context.setInt(PROP_EFFECT_CONDITIONEFFECT_ROUND_LIMITTRIGGER, iTriggerLimit);

		/*string strRealEffect = concatString(paramList);
		pEffectFactory->parseEffect(strRealEffect, m_triggerEffectCtx);

		
		*/

		return true;
	}
	
	return false;
}

vector<string> SkillEffect_CondEffect::getTypeString()
{
	vector<string> resultList;

	resultList.push_back("条件效果");
	
	return resultList;
}



