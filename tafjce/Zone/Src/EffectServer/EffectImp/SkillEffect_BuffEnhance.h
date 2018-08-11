#ifndef __SKILLEFFECT_BUFFENHANCE_H__
#define __SKILLEFFECT_BUFFENHANCE_H__


#include "SkillEffect_Helper.h"
#include "EffectSystem.h"


class SkillEffect_BuffEnhance:public SkillEffect_Helper
{
public:

	virtual bool doSignleEffect(HEntity hEntity, const EffectContext& effectContext);
	virtual bool parseEffect(const std::string& strEffectCmd, EffectContext& context);
	virtual int getEffectType(){return en_SkillEffect_BuffEnhance;}
	virtual vector<string> getTypeString();
};


bool SkillEffect_BuffEnhance::doSignleEffect(HEntity hEntity, const EffectContext& effectContext)
{
	//HEntity hGiver = effectContext.getInt(PROP_EFFECT_GIVER);
	//int iBuffID = effectContext.getInt(PROP_EFFECT_ADDBUFFID);
	bool isNegativeEffect = effectContext.getInt(PROP_EFFECT_NEGATIVE) == 1;

	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	EffectSystem* pEffectSys = static_cast<EffectSystem*>(pEntity->querySubsystem(IID_IEffectSystem));
	assert(pEffectSys);

	int iEnhanceSkillID = effectContext.getInt(PROP_EFFECT_BUFFENHANCE_SKILL);
	int iEnhanceBuffID = effectContext.getInt(PROP_EFFECT_BUFFENHANCE_BUFF);
	int iEnhanceValue = effectContext.getInt(PROP_EFFECT_BUFFENHANCE_VALUE);

	if(!isNegativeEffect)
	{
		pEffectSys->addBuffChance(iEnhanceSkillID, iEnhanceBuffID, iEnhanceValue);
	}
	else
	{
		pEffectSys->subBuffChance(iEnhanceSkillID, iEnhanceBuffID, iEnhanceValue);
	}

	return true;
}


bool SkillEffect_BuffEnhance::parseEffect(const std::string& strEffectCmd, EffectContext& context)
{
	vector<string> paramList = TC_Common::sepstr<string>(strEffectCmd, Effect_MagicSep);
	assert(paramList.size() > 0);

	if(paramList.size() == 4)
	{
		context.setInt(PROP_EFFECT_TYPE, (Int32)getEffectType() );
		context.setInt(PROP_EFFECT_TARGETSELECT, en_SkillTargetSelect_Self);

		int iSkillID = AdvanceAtoi(paramList[1]);
		int iPropID = AdvanceAtoi(paramList[2]);
		int iValue = AdvanceAtoi(paramList[3]);
			
		context.setInt(PROP_EFFECT_BUFFENHANCE_SKILL, iSkillID);
		context.setInt(PROP_EFFECT_BUFFENHANCE_BUFF, iPropID);
		context.setInt(PROP_EFFECT_BUFFENHANCE_VALUE, iValue);

		return true;
	}

	return false;
}


vector<string> SkillEffect_BuffEnhance::getTypeString()
{
	vector<string> resultList;

	resultList.push_back("状态增强");

	return resultList;
}



#endif
