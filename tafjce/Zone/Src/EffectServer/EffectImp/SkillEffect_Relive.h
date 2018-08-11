#ifndef __SKILLEFFECT_RELIVE_H__
#define __SKILLEFFECT_RELIVE_H__

#include "SkillEffect_Helper.h"


class SkillEffect_Relive:public SkillEffect_Helper, public Detail::EventHandle
{
public:

	virtual bool doSignleEffect(HEntity hEntity, const EffectContext& effectContext);
	virtual bool parseEffect(const std::string& strEffectCmd, EffectContext& context);
	virtual int getEffectType(){return en_SkillEffect_Relive;}
	virtual vector<string> getTypeString();

	void onEventTouchDead(EventArgs& args);
	void onEventPreRelease(EventArgs& args);
	void onEndRound(EventArgs& args);
};


void SkillEffect_Relive::onEventTouchDead(EventArgs& args)
{
	HEntity hEntity = args.context.getInt("entity");

	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	int iReliveCount = pEntity->getProperty(PROP_ENTITY_RELIVECOUNT, 0);

	if(iReliveCount <= 0) return;

	

	// 计算复活的HP
	int iMaxHP = pEntity->getProperty(PROP_ENTITY_MAXHP, 0);
	//int iLevelStep = pEntity->getProperty(PROP_ENTITY_LEVELSTEP, 0);

	/*ITable* pTable = getCompomentObjectManager()->findTable("Relive");
	assert(pTable);

	int iRecord = pTable->findRecord(iLevelStep);
	if(iRecord < 0) return;

	int iPercent = pTable->getInt(iRecord, "复活HP万份比");*/
	pEntity->changeProperty(PROP_ENTITY_RELIVECOUNT, -1, 0);

	// 加血效果增强
	int iPercent = 0;
	int iEnhanceValue = pEntity->getProperty(PROP_ENTITY_RELIVEENHANCE, 0);
	iPercent += iEnhanceValue;

	int iResultHP = (int)( (double)iMaxHP * iPercent/10000);
	pEntity->setProperty(PROP_ENTITY_HP, iResultHP);
	pEntity->setProperty(PROP_ENTITY_JUSTRELIVE, 1);
	args.context.setInt("fixNewHP", iResultHP);

	EffectContext nullCtx;
	onChgPropProcess(hEntity, hEntity, PROP_ENTITY_HP, iResultHP, nullCtx, false, true);
	
	// 触发事件
	/*EffectContext nullCtx;
	EventArgs_FightPropChg fightPropChg;
	fightPropChg.hEventTrigger = hEntity;
	fightPropChg.hEntity = hEntity;
	fightPropChg.hGiver = hEntity;
	fightPropChg.iPropID = PROP_ENTITY_HP;
	fightPropChg.iChgValue = iResultHP;
	fightPropChg.iValue = pEntity->getProperty(PROP_ENTITY_HP, 0);
	fightPropChg.effectCtx = &nullCtx;
	fightPropChg.bRelive = true;

	pEntity->getEventServer()->setEvent(EVENT_ENTITY_DOFIGHTPROPCHG, (EventArgs&)fightPropChg);
	pEntity->getEventServer()->setEvent(EVENT_ENTITY_POSTDOFIGHTPROPCHG, (EventArgs&)fightPropChg);

	pEntity->getEventServer()->setEvent(EVENT_ENTITY_BEFIGHTPROPCHG, (EventArgs&)fightPropChg);
	pEntity->getEventServer()->setEvent(EVENT_ENTITY_POSTBEFIGHTPROPCHG, (EventArgs&)fightPropChg);*/
}

void SkillEffect_Relive::onEventPreRelease(EventArgs& args)
{
	HEntity hEntity = args.context.getInt("entity");
	IEntity* pEntity = getEntityFromHandle(hEntity);
	if(!pEntity) return;

	pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_EFFECT_TOCHBEKILL, this, &SkillEffect_Relive::onEventTouchDead);
	pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_PRE_RELEASE, this, &SkillEffect_Relive::onEventPreRelease);
	pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_END_ROUND, this, &SkillEffect_Relive::onEndRound);
}

void SkillEffect_Relive::onEndRound(EventArgs& args)
{
	HEntity hEntity = args.context.getInt("entity", 0);
	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	pEntity->setProperty(PROP_ENTITY_JUSTRELIVE, 0);
}

bool SkillEffect_Relive::doSignleEffect(HEntity hEntity, const EffectContext& effectContext)
{
	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	bool isNegativeEffect = effectContext.getInt(PROP_EFFECT_NEGATIVE) == 1;
	if(isNegativeEffect)
	{
		pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_EFFECT_TOCHBEKILL, this, &SkillEffect_Relive::onEventTouchDead);
		pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_PRE_RELEASE, this, &SkillEffect_Relive::onEventPreRelease);
		pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_END_ROUND, this, &SkillEffect_Relive::onEndRound);

		pEntity->setProperty(PROP_ENTITY_RELIVECOUNT, 0);
	
		return true;
	}

	pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_EFFECT_TOCHBEKILL, this, &SkillEffect_Relive::onEventTouchDead);
	pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_PRE_RELEASE, this, &SkillEffect_Relive::onEventPreRelease);
	pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_END_ROUND, this, &SkillEffect_Relive::onEndRound);

	int iReliveCount = effectContext.getInt(PROP_EFFECT_RELIVE_LIMIT);
	pEntity->setProperty(PROP_ENTITY_RELIVECOUNT, iReliveCount);

	return true;
}

bool SkillEffect_Relive::parseEffect(const std::string& strEffectCmd, EffectContext& context)
{
	vector<string> paramList = TC_Common::sepstr<string>(strEffectCmd, Effect_MagicSep);
	assert(paramList.size() > 0);

	if( (paramList[0] == "复活") && (paramList.size() == 2) )
	{
		int iReliveLimit = AdvanceAtoi(paramList[1]);

		
		context.setInt(PROP_EFFECT_TYPE, (Int32)getEffectType() );
		context.setInt(PROP_EFFECT_TARGETSELECT, en_SkillTargetSelect_Self);
		context.setInt(PROP_EFFECT_RELIVE_LIMIT, iReliveLimit);

		return true;
	}

	return false;
}

vector<string> SkillEffect_Relive::getTypeString()
{
	vector<string> resultList;

	resultList.push_back("复活");

	return resultList;
}


#endif
