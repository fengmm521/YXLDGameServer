#ifndef __SKILLEFFECT_ZIBAO_H__
#define __SKILLEFFECT_ZIBAO_H__

#include "SkillEffect_Helper.h"


class SkillEffect_ZiBao:public ObjectBase<ISkillEffect>
{
public:

	//virtual bool doSignleEffect(HEntity hEntity, const EffectContext& effectContext);

	virtual bool doEffect(HEntity hEntity, const EffectContext& effectContext);
	virtual bool parseEffect(const std::string& strEffectCmd, EffectContext& context);
	virtual int getEffectType(){return en_SkillEffect_ZiBao;}
	virtual bool isImMunityBuff(int iBuffID, const EffectContext& effectContext){return false;}
	virtual vector<string> getTypeString();
};


bool SkillEffect_ZiBao::doEffect(HEntity hEntity, const EffectContext& effectContext)
{
	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	EventArgs args;
	args.context.setInt("entity", hEntity);
	pEntity->getEventServer()->setEvent(EVENT_ENTITY_ZIBAO, args);

	return true;
}

bool SkillEffect_ZiBao::parseEffect(const std::string& strEffectCmd, EffectContext& context)
{
	vector<string> paramList = TC_Common::sepstr<string>(strEffectCmd, Effect_MagicSep);
	assert(paramList.size() > 0);

	
	context.setInt(PROP_EFFECT_TARGETSELECT, en_SkillTargetSelect_Self);
	context.setInt(PROP_EFFECT_TYPE, (Int32)getEffectType() );

	return true;
}
	
vector<string> SkillEffect_ZiBao::getTypeString()
{
	vector<string> resultList;

	resultList.push_back("自爆表现");

	return resultList;
}



#endif

