#ifndef __SKILLEFFECT_RANDDELBUFF_H__
#define __SKILLEFFECT_RANDDELBUFF_H__

#include "SkillEffect_Helper.h"

class SkillEffect_RandDelBuff:public SkillEffect_Helper
{
public:

	virtual bool doSignleEffect(HEntity hEntity, const EffectContext& effectContext);
	virtual bool parseEffect(const std::string& strEffectCmd, EffectContext& context);
	virtual int getEffectType(){return en_SkillEffect_RandDelBuff;}
	virtual vector<string> getTypeString();
};


bool SkillEffect_RandDelBuff::doSignleEffect(HEntity hEntity, const EffectContext& effectContext)
{
	bool isNegativeEffect = effectContext.getInt(PROP_EFFECT_NEGATIVE) == 1;
	if(isNegativeEffect)
	{
		SvrErrLog("SkillEffect_RandDelBuff not support NegativeEffect");
		return true;
	}

	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	IBuffSystem* pBuffSys = static_cast<IBuffSystem*>(pEntity->querySubsystem(IID_IBuffSystem));
	assert(pBuffSys);

	int iDelType = effectContext.getInt(PROP_EFFECT_RANDDELTYPE);
	pBuffSys->randomDelBuffByType(iDelType);

	return true;
}

bool SkillEffect_RandDelBuff::parseEffect(const std::string& strEffectCmd, EffectContext& context)
{
	vector<string> paramList = TC_Common::sepstr<string>(strEffectCmd, Effect_MagicSep);
	assert(paramList.size() > 0);
	
	if( (paramList[0] == "按类型随机删除Buff") && (paramList.size() == 3) )
	{
		int iTargetSelect = AdvanceAtoi(paramList[1]);
		int iDelType = AdvanceAtoi(paramList[2]);

		context.setInt(PROP_EFFECT_TYPE, (Int32)getEffectType() );
		context.setInt(PROP_EFFECT_TARGETSELECT, iTargetSelect);
		context.setInt(PROP_EFFECT_RANDDELTYPE, iDelType);
	}
	else
	{
		return false;
	}


	return true;
}

vector<string> SkillEffect_RandDelBuff::getTypeString()
{
	vector<string> resultList;

	resultList.push_back("按类型随机删除Buff");

	return resultList;
}


#endif
