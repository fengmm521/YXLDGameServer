#ifndef __SKILLEFFECT_REBOUNDDAMAGE_H__
#define __SKILLEFFECT_REBOUNDDAMAGE_H__

#include "SkillEffect_Helper.h"


class SkillEffect_ReboundDamage:public SkillEffect_Helper, public Detail::EventHandle
{
public:

	virtual bool doSignleEffect(HEntity hEntity, const EffectContext& effectContext);
	virtual bool parseEffect(const std::string& strEffectCmd, EffectContext& context);
	virtual int getEffectType(){return en_SkillEffect_ReboundDamage;}
	virtual vector<string> getTypeString();

	void onEventPostBeDamage(EventArgs& args);
	//void onEventPostBeFightPropChg(EventArgs& args);
	void onEventPreRelease(EventArgs& args);

private:

	map<HEntity, int> m_reboundCtxMap;
};


void SkillEffect_ReboundDamage::onEventPreRelease(EventArgs& args)
{
	HEntity hEntity = args.context.getInt("entity");
	m_reboundCtxMap.erase(hEntity);

	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_POSTBEDAMAGE, this, &SkillEffect_ReboundDamage::onEventPostBeDamage);
	//pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_POSTBEFIGHTPROPCHG, this, &SkillEffect_ReboundDamage::onEventPostBeFightPropChg);
	pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_PRE_RELEASE, this, &SkillEffect_ReboundDamage::onEventPreRelease);
}



void SkillEffect_ReboundDamage::onEventPostBeDamage(EventArgs& args)
{
	EventArgsDamageCtx& damageCtx = (EventArgsDamageCtx&)args;

	// BUff伤害跳过
	bool bIsBuff = damageCtx.effectCtx->getInt(PROP_EFFECT_ISBUFFEFFECT) != 0;
	if(bIsBuff) return;

	IEntity* pGiver = getEntityFromHandle(damageCtx.hGiver);
	assert(pGiver);

	// 神兽跳过
	int iGiverClass = pGiver->getProperty(PROP_ENTITY_CLASS, 0);
	if(GSProto::en_class_GodAnimal == iGiverClass) return;

	if(m_reboundCtxMap.find(damageCtx.hEntity) == m_reboundCtxMap.end() )
	{
		return;
	}

	int iClassID = pGiver->getProperty(PROP_ENTITY_CLASS, 0);
	if(iClassID == GSProto::en_class_Monster)
	{
		int iMonsterID = pGiver->getProperty(PROP_ENTITY_BASEID, 0);
		ITable* pMonsterTb = getCompomentObjectManager()->findTable(TABLENAME_Monster);
		assert(pMonsterTb);

		int iRecordID = pMonsterTb->findRecord(iMonsterID);
		assert(iRecordID >= 0);

		int iMonsterType = pMonsterTb->getInt(iRecordID, "类型");
		if(iMonsterType == GSProto::en_MonsterType_WorldBoss)
		{
			return;
		}
	}
	

	int iPercent = m_reboundCtxMap[damageCtx.hEntity];

	int iReboundDamage = (int) ((double)damageCtx.iResultDamage * iPercent/10000);

	bool bImMunity = pGiver->getProperty(PROP_ENTITY_IMMUNITYDAMAGE, 0);
	if(bImMunity) iReboundDamage = 0;
	
	pGiver->changeProperty(PROP_ENTITY_HP, 0-iReboundDamage, 0);

	EffectContext tmpCtx;
	onChgPropProcess(damageCtx.hEntity, damageCtx.hGiver, PROP_ENTITY_HP, -iReboundDamage, tmpCtx, true);

	int iGiverHP = pGiver->getProperty(PROP_ENTITY_HP, 0);
	if(0 == iGiverHP)
	{
		onDeadProcess(damageCtx.hEntity, damageCtx.hGiver);
	}
}
	

bool SkillEffect_ReboundDamage::doSignleEffect(HEntity hEntity, const EffectContext& effectContext)
{
	bool isBuffEffect = effectContext.getInt(PROP_EFFECT_ISBUFFEFFECT) != 0;
	if(!isBuffEffect)
	{
		SvrErrLog("SkillEffect_ReboundDamage Must Use In BuffEffect");
		return true;
	}

	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	bool isNegativeEffect = effectContext.getInt(PROP_EFFECT_NEGATIVE) == 1;
	if(isNegativeEffect)
	{
		m_reboundCtxMap.erase(hEntity);

		pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_POSTBEDAMAGE, this, &SkillEffect_ReboundDamage::onEventPostBeDamage);
		//pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_POSTBEFIGHTPROPCHG, this, &SkillEffect_ReboundDamage::onEventPostBeFightPropChg);
		pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_PRE_RELEASE, this, &SkillEffect_ReboundDamage::onEventPreRelease);
		
		return true;
	}

	int iPercent = effectContext.getInt(PROP_EFFECT_REBOUNDDAMAGE_PERCENT);

	//PROP_ENTITY_REBOUNDPERCENT
	// 反弹能力加成
	int iAddtionValue = pEntity->getProperty(PROP_ENTITY_REBOUNDPERCENT, 0);
	iPercent += iAddtionValue;

	m_reboundCtxMap[hEntity] = iPercent;
	pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_POSTBEDAMAGE, this, &SkillEffect_ReboundDamage::onEventPostBeDamage);
	//pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_POSTBEFIGHTPROPCHG, this, &SkillEffect_ReboundDamage::onEventPostBeFightPropChg);
	pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_PRE_RELEASE, this, &SkillEffect_ReboundDamage::onEventPreRelease);
	

	return true;
}

bool SkillEffect_ReboundDamage::parseEffect(const std::string& strEffectCmd, EffectContext& context)
{
	vector<string> paramList = TC_Common::sepstr<string>(strEffectCmd, Effect_MagicSep);
	assert(paramList.size() > 0);

	if( (paramList[0] == "反弹伤害") && (paramList.size() == 2) )
	{
		int iReboundDamagePercent = AdvanceAtoi(paramList[1]);
		
		context.setInt(PROP_EFFECT_TYPE, (Int32)getEffectType() );
		context.setInt(PROP_EFFECT_TARGETSELECT, en_SkillTargetSelect_Self);
		context.setInt(PROP_EFFECT_REBOUNDDAMAGE_PERCENT, iReboundDamagePercent);
	}

	return true;
}

vector<string> SkillEffect_ReboundDamage::getTypeString()
{
	vector<string> resultList;

	resultList.push_back("反弹伤害");

	return resultList;
}



#endif
