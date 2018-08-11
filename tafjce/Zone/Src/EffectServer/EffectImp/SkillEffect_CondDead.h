#ifndef __SKILLEFFECT_CONDDEAD_H__
#define __SKILLEFFECT_CONDDEAD_H__

#include "SkillEffect_Helper.h"


class SkillEffect_CondDead:public SkillEffect_Helper
{
public:

	virtual bool doSignleEffect(HEntity hEntity, const EffectContext& effectContext);
	virtual bool parseEffect(const std::string& strEffectCmd, EffectContext& context);
	virtual int getEffectType(){return en_SkillEffect_CondDead;}
	virtual vector<string> getTypeString();
};

bool SkillEffect_CondDead::doSignleEffect(HEntity hEntity, const EffectContext& effectContext)
{
	bool isNegativeEffect = effectContext.getInt(PROP_EFFECT_NEGATIVE) == 1;
	if(isNegativeEffect)
	{
		SvrErrLog("SkillEffect_CondDead not support NegativeEffect");
		return true;
	}

	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	HEntity hGiver = effectContext.getInt(PROP_EFFECT_GIVER);
	IEntity* pGiver = getEntityFromHandle(hGiver);
	assert(pGiver);

	// 免疫了，不生效
	bool bImMunity = pEntity->getProperty(PROP_ENTITY_IMMUNITYDAMAGE, 0);
	if(bImMunity)
	{
		return true;
	}

	// 对BOSS不生效
	int iClassID = pEntity->getProperty(PROP_ENTITY_CLASS, 0);
	if(iClassID == GSProto::en_class_Monster)
	{
		int iMonsterID = pEntity->getProperty(PROP_ENTITY_BASEID, 0);
		ITable* pMonsterTb = getCompomentObjectManager()->findTable(TABLENAME_Monster);
		assert(pMonsterTb);

		int iRecordID = pMonsterTb->findRecord(iMonsterID);
		assert(iRecordID >= 0);

		int iMonsterType = pMonsterTb->getInt(iRecordID, "类型");
		if(iMonsterType == GSProto::en_MonsterType_WorldBoss)
		{
			return true;
		}
	}

	int iHP = pEntity->getProperty(PROP_ENTITY_HP, 0);
	int iMaxHP = pEntity->getProperty(PROP_ENTITY_MAXHP, 0);
	int iTmpPercent = (int)((double)iHP/(double)iMaxHP * 10000);

	int iCondPercent = effectContext.getInt(PROP_EFFECT_CONDDEAD_HPPERCENT);
	if(iTmpPercent >= iCondPercent) 
	{
		return true;
	}

	int iChance = effectContext.getInt(PROP_EFFECT_CONDDEAD_CHANCE);

	//PROP_ENTITY_CONDITIONDEAD
	int iAddtionChance = pGiver->getProperty(PROP_ENTITY_CONDITIONDEAD, 0);
	iChance += iAddtionChance;

	IRandom* pRandom = getComponent<IRandom>("Random", IID_IMiniAprRandom);
	assert(pRandom);
	
	int iRandV = pRandom->random() % 10000;

	if(iRandV >= iChance)
	{
		return true;
	}

	pEntity->changeProperty(PROP_ENTITY_HP, 0-iHP, 0);
	onChangeHPProcess(hGiver, hEntity, 0-iHP, effectContext);

	return true;
}

bool SkillEffect_CondDead::parseEffect(const std::string& strEffectCmd, EffectContext& context)
{
	vector<string> paramList = TC_Common::sepstr<string>(strEffectCmd, Effect_MagicSep);
	assert(paramList.size() > 0);

	if( (paramList[0] == "低血条件死亡") && (paramList.size() == 4) )
	{
		int iTargetSelect = AdvanceAtoi(paramList[1]);
		int iHPPercent = AdvanceAtoi(paramList[2]);
		int iDeadChance = AdvanceAtoi(paramList[3]);

		context.setInt(PROP_EFFECT_TYPE, (Int32)getEffectType() );
		context.setInt(PROP_EFFECT_TARGETSELECT, iTargetSelect);

		
		context.setInt(PROP_EFFECT_CONDDEAD_HPPERCENT, iHPPercent);
		context.setInt(PROP_EFFECT_CONDDEAD_CHANCE, iDeadChance);
	}

	return true;
}

vector<string> SkillEffect_CondDead::getTypeString()
{
	vector<string> resultList;

	resultList.push_back("低血条件死亡");

	return resultList;
}


#endif

