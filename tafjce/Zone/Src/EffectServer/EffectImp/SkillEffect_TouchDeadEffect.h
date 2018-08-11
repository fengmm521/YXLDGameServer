#ifndef __SKILLEFFECT_TOUCHDEADEFFECT_H__
#define __SKILLEFFECT_TOUCHDEADEFFECT_H__

#include "SkillEffect_Helper.h"


class SkillEffect_TouchDeadEffect:public SkillEffect_Helper, public Detail::EventHandle
{
public:
	virtual bool doSignleEffect(HEntity hEntity, const EffectContext& effectContext);
	virtual bool parseEffect(const std::string& strEffectCmd, EffectContext& context);
	virtual int getEffectType(){return en_SkillEffect_TouchDeadEffect;}
	virtual vector<string> getTypeString();

	void onEventTouchDead(EventArgs& args);
	void onEventPreRelease(EventArgs& args);

private:

	map<HEntity, EffectContext> m_mapEffectCtx;

	//int m_iReliveHP;
	//EffectContext m_effectCtx;
	//vector<int> m_effectIDList;
};


bool SkillEffect_TouchDeadEffect::doSignleEffect(HEntity hEntity, const EffectContext& effectContext)
{
	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	bool isNegativeEffect = effectContext.getInt(PROP_EFFECT_NEGATIVE) == 1;
	if(isNegativeEffect)
	{
		pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_EFFECT_TOCHBEKILL, this, &SkillEffect_TouchDeadEffect::onEventTouchDead);
		pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_PRE_RELEASE, this, &SkillEffect_TouchDeadEffect::onEventPreRelease);
		
		return true;
	}

	pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_EFFECT_TOCHBEKILL, this, &SkillEffect_TouchDeadEffect::onEventTouchDead);
	pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_PRE_RELEASE, this, &SkillEffect_TouchDeadEffect::onEventPreRelease);

	m_mapEffectCtx[hEntity] = effectContext;

	return true;
}

bool SkillEffect_TouchDeadEffect::parseEffect(const std::string& strEffectCmd, EffectContext& context)
{
	context.setInt(PROP_EFFECT_TYPE, (Int32)getEffectType() );
	
	vector<string> paramList = TC_Common::sepstr<string>(strEffectCmd, Effect_MagicSep);
	assert(paramList.size() > 0);

	if( (paramList[0] == "回光返照") && (paramList.size() >= 3) )
	{
		int iReliveHP = AdvanceAtoi(paramList[1]);

		context.setInt(PROP_EFFECT_TOUDHDEADEFFECT_HP, iReliveHP);
		

		paramList.erase(paramList.begin(), paramList.begin() + 2);
		/*string strRealEffect = SkillEffect_CondEffect::concatString(paramList);

		IEffectFactory* pEffectFactory = getComponent<IEffectFactory>(COMPNAME_EffectFactory, IID_IEffectFactory);
		assert(pEffectFactory);

		pEffectFactory->parseEffect(strRealEffect, m_effectCtx);*/

		vector<int>* pEffectList = new vector<int>();
		for(size_t i = 0; i < paramList.size(); i++)
		{
			pEffectList->push_back(AdvanceAtoi(paramList[i]));
		}

		context.setInt64(PROP_EFFECT_TOUDHDEADEFFECT_EFFECTLIST, (Int64)pEffectList);

		return true;
	}

	return false;
}


vector<string> SkillEffect_TouchDeadEffect::getTypeString()
{
	vector<string> resultList;

	resultList.push_back("回光返照");

	return resultList;
}


void SkillEffect_TouchDeadEffect::onEventPreRelease(EventArgs& args)
{
	HEntity hEntity = args.context.getInt("entity");
	m_mapEffectCtx.erase(hEntity);
}


void SkillEffect_TouchDeadEffect::onEventTouchDead(EventArgs& args)
{
	//PROP_ENTITY_TOUCHDEADEFFECT_COUNT
	HEntity hEntity = args.context.getInt("entity");

	if(m_mapEffectCtx.find(hEntity) == m_mapEffectCtx.end() )
	{
		return;
	}

	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	int iUsedCount = pEntity->getProperty(PROP_ENTITY_TOUCHDEADEFFECT_COUNT, 0);
	if(iUsedCount > 0)
	{
		return;
	}

	const EffectContext effectCtx = m_mapEffectCtx[hEntity];
	
	// 设置新的HP
	int iResultHP = effectCtx.getInt(PROP_EFFECT_TOUDHDEADEFFECT_HP);
	pEntity->setProperty(PROP_ENTITY_HP, iResultHP);	
	pEntity->changeProperty(PROP_ENTITY_TOUCHDEADEFFECT_COUNT, 1, 0);
	
	args.context.setInt("fixNewHP", iResultHP);


	// 触发事件
	EffectContext nullCtx;
	EventArgs_FightPropChg fightPropChg;
	fightPropChg.hEventTrigger = hEntity;
	fightPropChg.hEntity = hEntity;
	fightPropChg.hGiver = hEntity;
	fightPropChg.iPropID = PROP_ENTITY_HP;
	fightPropChg.iChgValue = iResultHP;
	fightPropChg.iValue = pEntity->getProperty(PROP_ENTITY_HP, 0);
	fightPropChg.effectCtx = &nullCtx;
	fightPropChg.bRelive = true;

	pEntity->getEventServer()->setEvent(EVENT_ENTITY_DOFIGHTPROPCHG, (EventArgs&)fightPropChg);
	pEntity->getEventServer()->setEvent(EVENT_ENTITY_POSTDOFIGHTPROPCHG, (EventArgs&)fightPropChg);

	pEntity->getEventServer()->setEvent(EVENT_ENTITY_BEFIGHTPROPCHG, (EventArgs&)fightPropChg);
	pEntity->getEventServer()->setEvent(EVENT_ENTITY_POSTBEFIGHTPROPCHG, (EventArgs&)fightPropChg);

	// 运行新效果
	EffectContext tmpContext;
	
	tmpContext.setInt(PROP_EFFECT_GIVER, (Int32)hEntity);
	IFightFactory* pFightFactory = getComponent<IFightFactory>(COMPNAME_FightFactory, IID_IFightFactory);
	assert(pFightFactory);

	const vector<HEntity>* pMemberList = pFightFactory->getMemberList();
	tmpContext.setInt64(PROP_EFFECT_FIGHTMEMBERLIST, (Int64)pMemberList);

	
	IEffectSystem* pEffectSys = static_cast<IEffectSystem*>(pEntity->querySubsystem(IID_IEffectSystem) );
	assert(pEffectSys);

	vector<int>* pEffectIDList = (vector<int>*)effectCtx.getInt64(PROP_EFFECT_TOUDHDEADEFFECT_EFFECTLIST, 0);
	for(size_t i = 0; i < pEffectIDList->size(); i++)
	{
		pEffectSys->addEffect((*pEffectIDList)[i], tmpContext);
	}
	//pEffectSys->addEffect(tmpContext);
}


#endif

