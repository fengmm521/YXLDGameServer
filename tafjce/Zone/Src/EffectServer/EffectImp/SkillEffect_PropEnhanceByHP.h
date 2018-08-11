#ifndef __SKILLEFFECT_DAMAGEENHANCEBYHP_H__
#define __SKILLEFFECT_DAMAGEENHANCEBYHP_H__

#include "SkillEffect_Helper.h"

class SkillEffect_PropEnhanceByHP:public SkillEffect_Helper, public Detail::EventHandle
{
public:

	SkillEffect_PropEnhanceByHP(){}

	virtual bool doSignleEffect(HEntity hEntity, const EffectContext& effectContext);
	virtual bool parseEffect(const std::string& strEffectCmd, EffectContext& context);
	virtual int getEffectType(){return en_SkillEffect_PropEnhanceByHP;}
	virtual vector<string> getTypeString();

	void onEventHPChg(EventArgs& args);
	void onEventPreRelease(EventArgs& args);

private:

	//map<HEntity, int> m_mapPercent;
	//int m_iPropID;
	//int m_iFixValue;

	map<HEntity, EffectContext> m_mapEffectCtx;
};


bool SkillEffect_PropEnhanceByHP::doSignleEffect(HEntity hEntity, const EffectContext& effectContext)
{
	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	
	bool isNegativeEffect = effectContext.getInt(PROP_EFFECT_NEGATIVE) == 1;

	if(isNegativeEffect)
	{
		pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_PROPCHANGE, this, &SkillEffect_PropEnhanceByHP::onEventHPChg);
		pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_PRE_RELEASE, this, &SkillEffect_PropEnhanceByHP::onEventPreRelease);
	}
	else
	{
		pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_PROPCHANGE, this, &SkillEffect_PropEnhanceByHP::onEventHPChg);
		pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_PRE_RELEASE, this, &SkillEffect_PropEnhanceByHP::onEventPreRelease);
	}

	m_mapEffectCtx[hEntity] = effectContext;
	
	return true;
}

void SkillEffect_PropEnhanceByHP::onEventPreRelease(EventArgs& args)
{
	HEntity hEntity = args.context.getInt("entity");
	m_mapEffectCtx.erase(hEntity);
}


void SkillEffect_PropEnhanceByHP::onEventHPChg(EventArgs& args)
{
	EventArgs_PropChange& propArgs = (EventArgs_PropChange&)args;

	if(propArgs.iPropID != PROP_ENTITY_HP)
	{
		return;
	}

	IEntity* pEntity = getEntityFromHandle(propArgs.hEntity);
	assert(pEntity);

	// 如果有主人，必须等创建完成
	IEntity* pMaster = getEntityFromHandle(pEntity->getProperty(PROP_ENTITY_MASTER, 0) );
	if(pMaster)
	{
		bool bCreateFinish = pMaster->getProperty(PROP_ACTOR_CREATEFINISH, 0) != 0;
		if(!bCreateFinish) return;
	}

	if(m_mapEffectCtx.find(propArgs.hEntity) == m_mapEffectCtx.end() )
	{
		return;
	}

	const EffectContext& ctx = m_mapEffectCtx[propArgs.hEntity];

	int iPropID = ctx.getInt(PROP_EFFECT_PROPENHANCEJP_PROPID, 0);
	int iFixValue = ctx.getInt(PROP_EFFECT_PROPENHANCEJP_FIXVALUE, 0);

	// 计算新的
	int iMaxHP = pEntity->getProperty(PROP_ENTITY_MAXHP, 0);
	int iChgHP = propArgs.iOldValue - propArgs.iValue;

	double dLostPercent = (double)iChgHP/(double)iMaxHP;
	double dEnhancePercent = dLostPercent * iFixValue;


	pEntity->changeProperty(iPropID, (int)dEnhancePercent, 0);
}


bool SkillEffect_PropEnhanceByHP::parseEffect(const std::string& strEffectCmd, EffectContext& context)
{
	vector<string> paramList = TC_Common::sepstr<string>(strEffectCmd, Effect_MagicSep);
	assert(paramList.size() > 0);

	IEntityFactory* pEntityFactory = getComponent<IEntityFactory>(COMPNAME_EntityFactory, IID_IEntityFactory);
	assert(pEntityFactory);
	
	if( (paramList[0] == "损失HP增强属性") && (paramList.size() == 3) )
	{
		int iPropID = pEntityFactory->getPropDefine("EntityPropDef")->getPropKey(paramList[1]);
		assert(iPropID >= 0);
		int iFixValue = AdvanceAtoi(paramList[2]);

		context.setInt(PROP_EFFECT_PROPENHANCEJP_PROPID, iPropID);
		context.setInt(PROP_EFFECT_PROPENHANCEJP_FIXVALUE, iFixValue);

		context.setInt(PROP_EFFECT_TYPE, (Int32)getEffectType() );
		
		return true;	
	}

	return false;
}

vector<string> SkillEffect_PropEnhanceByHP::getTypeString()
{
	vector<string> resultList;

	resultList.push_back("损失HP增强属性");

	return resultList;
}


#endif
