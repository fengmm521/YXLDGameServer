#ifndef __SKILLEFFECT_CONTINUEKILL_H__
#define __SKILLEFFECT_CONTINUEKILL_H__

#include "SkillEffect_Helper.h"

class SkillEffect_ContinueKill:public SkillEffect_Helper, public Detail::EventHandle
{
public:

	virtual bool doSignleEffect(HEntity hEntity, const EffectContext& effectContext);
	virtual bool parseEffect(const std::string& strEffectCmd, EffectContext& context);
	virtual int getEffectType(){return en_SkillEffect_ContinueKill;}
	virtual vector<string> getTypeString();

	void onEventEffectKill(EventArgs& args);
	void onEventPreRelease(EventArgs& args);
	void onEventSkillUseFinish(EventArgs& args);
};


void SkillEffect_ContinueKill::onEventEffectKill(EventArgs& args)
{
	HEntity hGiver = args.context.getInt("entity");

	IEntity* pGiver = getEntityFromHandle(hGiver);
	assert(pGiver);

	pGiver->setProperty(PROP_ENTITY_CONTINUEKILL, 1);
}

void SkillEffect_ContinueKill::onEventPreRelease(EventArgs& args)
{
	HEntity hEntity = args.context.getInt("entity");
	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_EFFECTDOKILL, this, &SkillEffect_ContinueKill::onEventEffectKill);
	pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_PRE_RELEASE, this, &SkillEffect_ContinueKill::onEventPreRelease);
	pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_USESKILL_FINISH, this, &SkillEffect_ContinueKill::onEventSkillUseFinish);
}

void SkillEffect_ContinueKill::onEventSkillUseFinish(EventArgs& args)
{
	HEntity hEntity = args.context.getInt("entity");

	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_EFFECTDOKILL, this, &SkillEffect_ContinueKill::onEventEffectKill);
	pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_PRE_RELEASE, this, &SkillEffect_ContinueKill::onEventPreRelease);
	pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_USESKILL_FINISH, this, &SkillEffect_ContinueKill::onEventSkillUseFinish);
}



bool SkillEffect_ContinueKill::doSignleEffect(HEntity hEntity, const EffectContext& effectContext)
{
	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);
	
	bool isNegativeEffect = effectContext.getInt(PROP_EFFECT_NEGATIVE) == 1;
	if(isNegativeEffect)
	{
		pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_EFFECTDOKILL, this, &SkillEffect_ContinueKill::onEventEffectKill);
		pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_PRE_RELEASE, this, &SkillEffect_ContinueKill::onEventPreRelease);
		pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_USESKILL_FINISH, this, &SkillEffect_ContinueKill::onEventSkillUseFinish);

		return true;
	}

	pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_EFFECTDOKILL, this, &SkillEffect_ContinueKill::onEventEffectKill);
	pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_PRE_RELEASE, this, &SkillEffect_ContinueKill::onEventPreRelease);
	pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_USESKILL_FINISH, this, &SkillEffect_ContinueKill::onEventSkillUseFinish);
	
	
	return true;
}

bool SkillEffect_ContinueKill::parseEffect(const std::string& strEffectCmd, EffectContext& context)
{
	vector<string> paramList = TC_Common::sepstr<string>(strEffectCmd, Effect_MagicSep);
	assert(paramList.size() > 0);

	if(paramList[0] == "连杀")
	{
		context.setInt(PROP_EFFECT_TYPE, (Int32)getEffectType() );
		context.setInt(PROP_EFFECT_TARGETSELECT, en_SkillTargetSelect_Self);
		return true;
	}

	return false;
}

vector<string> SkillEffect_ContinueKill::getTypeString()
{
	vector<string> resultList;

	resultList.push_back("连杀");

	return resultList;
}


#endif

