#ifndef __SKILLEFFECT_PROPENHANCE_H__
#define __SKILLEFFECT_PROPENHANCE_H__

#include "SkillEffect_Helper.h"
#include "EffectSystem.h"

class SkillEffect_PropEnhance:public SkillEffect_Helper
{
public:

	virtual bool doSignleEffect(HEntity hEntity, const EffectContext& effectContext);
	virtual bool parseEffect(const std::string& strEffectCmd, EffectContext& context);
	virtual int getEffectType(){return en_SkillEffect_PropEnhance;}
	virtual vector<string> getTypeString();
};


bool SkillEffect_PropEnhance::doSignleEffect(HEntity hEntity, const EffectContext& effectContext)
{
	//HEntity hGiver = effectContext.getInt(PROP_EFFECT_GIVER);
	//int iBuffID = effectContext.getInt(PROP_EFFECT_ADDBUFFID);
	bool isNegativeEffect = effectContext.getInt(PROP_EFFECT_NEGATIVE) == 1;

	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	EffectSystem* pEffectSys = static_cast<EffectSystem*>(pEntity->querySubsystem(IID_IEffectSystem));
	assert(pEffectSys);

	int iEnhanceSkillID = effectContext.getInt(PROP_EFFECT_PROPENHANCE_SKILL);
	int iEnhancePropID = effectContext.getInt(PROP_EFFECT_PROPENHANCE_PROP);
	int iEnhanceValue = effectContext.getInt(PROP_EFFECT_PROPENHANCE_VALUE);
	int iEnhancePercent = effectContext.getInt(PROP_EFFECT_PROPENHANCE_PERCENT);

	if(!isNegativeEffect)
	{
		pEffectSys->addPropEnhance(iEnhanceSkillID, iEnhancePropID, iEnhanceValue);
		pEffectSys->addPropPercentEnhance(iEnhanceSkillID, iEnhancePropID, iEnhancePercent);
	}
	else
	{
		pEffectSys->subPropEnhance(iEnhanceSkillID, iEnhancePropID, iEnhanceValue);
		pEffectSys->subPropPercentEnhance(iEnhanceSkillID, iEnhancePropID, iEnhancePercent);
	}

	return true;
}


bool SkillEffect_PropEnhance::parseEffect(const std::string& strEffectCmd, EffectContext& context)
{
	vector<string> paramList = TC_Common::sepstr<string>(strEffectCmd, Effect_MagicSep);
	assert(paramList.size() > 0);

	if( (paramList.size() == 4) || (paramList.size() == 5) )
	{
		context.setInt(PROP_EFFECT_TYPE, (Int32)getEffectType() );
		context.setInt(PROP_EFFECT_TARGETSELECT, en_SkillTargetSelect_Self);

		int iSkillID = AdvanceAtoi(paramList[1]);
		int iPropID = AdvanceAtoi(paramList[2]);
		int iValue = AdvanceAtoi(paramList[3]);
			
		context.setInt(PROP_EFFECT_PROPENHANCE_SKILL, iSkillID);
		context.setInt(PROP_EFFECT_PROPENHANCE_PROP, iPropID);
		context.setInt(PROP_EFFECT_PROPENHANCE_VALUE, iValue);

		if(paramList.size() == 5)
		{
			//PROP_EFFECT_PROPENHANCE_PERCENT
			int iEnhancePercent = AdvanceAtoi(paramList[4]);
			context.setInt(PROP_EFFECT_PROPENHANCE_PERCENT, iEnhancePercent);
		}

		return true;
	}

	return false;
}


vector<string> SkillEffect_PropEnhance::getTypeString()
{
	vector<string> resultList;

	resultList.push_back("属性增强");

	return resultList;
}


#endif

