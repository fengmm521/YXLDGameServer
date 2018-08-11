#ifndef __SKILLEFFECT_ADDBUFF_H__
#define __SKILLEFFECT_ADDBUFF_H__

#include "SkillEffect_Helper.h"
#include "EffectSystem.h"


class SkillEffect_AddBuff:public SkillEffect_Helper
{
public:

	virtual bool doSignleEffect(HEntity hEntity, const EffectContext& effectContext);
	virtual bool parseEffect(const std::string& strEffectCmd, EffectContext& context);
	virtual int getEffectType(){return en_SkillEffect_AddBuff;}
	virtual vector<string> getTypeString();
};


bool SkillEffect_AddBuff::doSignleEffect(HEntity hEntity, const EffectContext& effectContext)
{
	HEntity hGiver = effectContext.getInt(PROP_EFFECT_GIVER);
	int iBuffID = effectContext.getInt(PROP_EFFECT_ADDBUFFID);
	bool isNegativeEffect = effectContext.getInt(PROP_EFFECT_NEGATIVE) == 1;

	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	IBuffSystem* pBuffSystem = static_cast<IBuffSystem*>(pEntity->querySubsystem(IID_IBuffSystem));
	assert(pBuffSystem);

	if(isNegativeEffect)
	{
		bool bIsTalentDel = effectContext.getInt(PROP_EFFECT_DELTALENT);
		if(!bIsTalentDel)
		{
			SvrErrLog("SkillEffect_AddBuff not support NagativeEffect");
			return false;
		}

		// 删除状态
		pBuffSystem->delBuff(iBuffID);
		return true;
	}

	if(pBuffSystem->isImMinityBuff(iBuffID) )
	{
		return true;
	}	

	IBuffSystem* pBuffSys = static_cast<IBuffSystem*>(pEntity->querySubsystem(IID_IBuffSystem));
	assert(pBuffSys);

	// 计算几率
	int iChance = effectContext.getInt(PROP_EFFECT_ADDBUFFCHANCE);

	// 几率修正
	int iSkillID = effectContext.getInt(PROP_EFFECT_USESKILL_ID);
	
	IEntity* pGiver = getEntityFromHandle(hGiver);
	assert(pGiver);

	EffectSystem* pEffectSys = static_cast<EffectSystem*>(pGiver->querySubsystem(IID_IEffectSystem));
	assert(pEffectSys);

	int iEnhanceChance = pEffectSys->getBuffEnhance(iSkillID, iBuffID);
	iChance += iEnhanceChance;

	IRandom* pRandom = getComponent<IRandom>("Random", IID_IMiniAprRandom);
	assert(pRandom);
	
	int iRandV = pRandom->random() % 10000;
	if(iRandV >= iChance) return true;


	EffectContext tmpCtx;
	tmpCtx.setInt(PROP_EFFECT_GIVER, hGiver);
	tmpCtx.setInt(PROP_EFFECT_USESKILL_ID, iSkillID);

	IEntityBuff* pResultBuff = pBuffSys->addBuff(hGiver, iBuffID, tmpCtx);

	return pResultBuff != NULL;
}


bool SkillEffect_AddBuff::parseEffect(const std::string& strEffectCmd, EffectContext& context)
{
	vector<string> paramList = TC_Common::sepstr<string>(strEffectCmd, Effect_MagicSep);
	assert(paramList.size() > 0);

	if( (paramList[0] == "添加状态") && ( (paramList.size() == 3) || (paramList.size() == 4) ) )
	{
		int iTargetSelect = AdvanceAtoi(paramList[1]);
		int iBuffID = AdvanceAtoi(paramList[2]);

		context.setInt(PROP_EFFECT_TYPE, (Int32)getEffectType() );
		context.setInt(PROP_EFFECT_TARGETSELECT, iTargetSelect);
		context.setInt(PROP_EFFECT_ADDBUFFID, iBuffID);

		int iChance = 10000;
		if(paramList.size() == 4)
		{
			iChance = AdvanceAtoi(paramList[3]);
		}

		context.setInt(PROP_EFFECT_ADDBUFFCHANCE, iChance);

		IBuffFactory* pBuffFactory = getComponent<IBuffFactory>(COMPNAME_BuffFactory, IID_IBuffFactory);
		assert(pBuffFactory);

		const IEntityBuff* pProto = pBuffFactory->getBuffPrototype(iBuffID);
		assert(pProto);
	}
	else
	{
		return false;
	}

	return true;
}


vector<string> SkillEffect_AddBuff::getTypeString()
{
	vector<string> resultList;

	resultList.push_back("添加状态");

	return resultList;
}


#endif

