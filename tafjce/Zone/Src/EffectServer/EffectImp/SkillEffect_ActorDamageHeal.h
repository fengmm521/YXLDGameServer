#ifndef __SKILLEFFECT_ACTORDAMAGEHEAL_H__
#define __SKILLEFFECT_ACTORDAMAGEHEAL_H__


#include "SkillEffect_Helper.h"


class SkillEffect_ActorDamageAndHeal:public SkillEffect_Helper
{
public:

	virtual bool doSignleEffect(HEntity hEntity, const EffectContext& effectContext);
	virtual bool parseEffect(const std::string& strEffectCmd, EffectContext& context);
	virtual int getEffectType(){return en_SkillEffect_ActorDamageAndHeal;}
	virtual vector<string> getTypeString();
};


bool SkillEffect_ActorDamageAndHeal::doSignleEffect(HEntity hEntity, const EffectContext& effectContext)
{
	HEntity hGiver = effectContext.getInt(PROP_EFFECT_GIVER);

	IEntity* pGiver = getEntityFromHandle(hGiver);
	assert(pGiver);

	HEntity hActor = pGiver->getProperty(PROP_ENTITY_MASTER, 0);
	IEntity* pActor = getEntityFromHandle(hActor);
	assert(pActor);

	int iFightValue = pActor->getProperty(PROP_ENTITY_FIGHTVALUE_PREFIGHT, 0);

	int iFixPercent = effectContext.getInt(PROP_EFFECT_ACOTR_DAMAGEPERCENT);
	int iFixValue = effectContext.getInt(PROP_EFFECT_ACTOR_DAMAGEVALUE);

	int iResultChgValue = ((double)iFightValue * iFixPercent/10000) + (double)iFixValue;

	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	int iCurHP = pEntity->getProperty(PROP_ENTITY_HP, 0);
	if( (iResultChgValue < 0) && (iResultChgValue + iCurHP < 0) )
	{
		iResultChgValue = 0 - iCurHP;
	}
	
	pEntity->changeProperty(PROP_ENTITY_HP, iResultChgValue, 0);

	EffectContext tmpEffectCtx;
	onSkillEffectChangeHPProcess(hGiver, hEntity, iResultChgValue, tmpEffectCtx);

	return true;
}


bool SkillEffect_ActorDamageAndHeal::parseEffect(const std::string& strEffectCmd, EffectContext& context)
{
	vector<string> paramList = TC_Common::sepstr<string>(strEffectCmd, Effect_MagicSep);
	assert(paramList.size() > 0);

	if( (paramList[0] == "团队伤害") && (paramList.size() == 4) )
	{
		int iTargetSelect = AdvanceAtoi(paramList[1]);
		int iFixPercent = AdvanceAtoi(paramList[2]);
		int iFixValue = AdvanceAtoi(paramList[3]);
	
		context.setInt(PROP_EFFECT_TYPE, (Int32)getEffectType() );

		context.setInt(PROP_EFFECT_TARGETSELECT, iTargetSelect);
		context.setInt(PROP_EFFECT_ACOTR_DAMAGEPERCENT, iFixPercent);
		context.setInt(PROP_EFFECT_ACTOR_DAMAGEVALUE, iFixValue);

		return true;
	}
	

	return false;
}


vector<string> SkillEffect_ActorDamageAndHeal::getTypeString()
{
	vector<string> resultList;

	resultList.push_back("团队伤害");


	return resultList;
}



#endif

