#ifndef __SKILLEFFECT_ADDPROPERTY_H__
#define __SKILLEFFECT_ADDPROPERTY_H__

#include "SkillEffect_Helper.h"
#include "EffectSystem.h"


class SkillEffect_AddProperty:public SkillEffect_Helper
{
public:

	virtual bool doSignleEffect(HEntity hEntity, const EffectContext& effectContext);
	virtual bool parseEffect(const std::string& strEffectCmd, EffectContext& context);
	virtual int getEffectType(){return en_SkillEffect_AddProperty;}
	virtual vector<string> getTypeString();
};


bool SkillEffect_AddProperty::parseEffect(const std::string& strEffectCmd, EffectContext& context)
{
	IEntityFactory* pEntityFactory = getComponent<IEntityFactory>("EntityFactory", IID_IEntityFactory);
	assert(pEntityFactory);
	
	vector<string> paramList = TC_Common::sepstr<string>(strEffectCmd, Effect_MagicSep);
	assert(paramList.size() > 0);

	int iTargetSelect = 0;
	int iDstProperty = 0;
	int iAddPercent = 0;
	int iAddV = 0;
	int iSrcProperty = 0;
	bool bIsFromGiver = 0;

	if(paramList[0] == "改变属性")
	{
		if( (paramList.size() != 5) && (paramList.size() != 7) )
		{
			return false;
		}

		iTargetSelect = AdvanceAtoi(paramList[1]);
		string strDstProperty = paramList[2];
		iDstProperty = pEntityFactory->getPropDefine("EntityPropDef")->getPropKey(paramList[2]);
		assert(iDstProperty >= 0);
		
		iAddPercent =  AdvanceAtoi(paramList[3]);
		iAddV = AdvanceAtoi(paramList[4]);
		iSrcProperty = iDstProperty;

		if(paramList.size() == 7)
		{
			context.setInt(PROP_EFFECT_PROPHASLIMIT, 1);
			int iMinValue = AdvanceAtoi(paramList[5]);
			int iMaxValue = AdvanceAtoi(paramList[6]);
			context.setInt(PROP_EFFECT_PROP_MINVALUE, iMinValue);
			context.setInt(PROP_EFFECT_PROP_MAXVALUE, iMaxValue);
		}
	}
	else if(paramList[0] == "按参考属性改变属性")
	{
		if(paramList.size() != 6)
		{
			return false;
		}
	
		iTargetSelect = AdvanceAtoi(paramList[1]);
		iDstProperty = pEntityFactory->getPropDefine("EntityPropDef")->getPropKey(paramList[2]);
		assert(iDstProperty >= 0);

		iAddPercent =  AdvanceAtoi(paramList[3]);
		iAddV = AdvanceAtoi(paramList[4]);

		iSrcProperty = pEntityFactory->getPropDefine("EntityPropDef")->getPropKey(paramList[5]);
		assert(iSrcProperty >= 0);
	}
	else if(paramList[0] == "按自身参考属性改变属性")
	{
		if(paramList.size() != 6)
		{
			return false;
		}
	
		iTargetSelect = AdvanceAtoi(paramList[1]);
		iDstProperty = pEntityFactory->getPropDefine("EntityPropDef")->getPropKey(paramList[2]);
		assert(iDstProperty >= 0);

		iAddPercent =  AdvanceAtoi(paramList[3]);
		iAddV = AdvanceAtoi(paramList[4]);

		iSrcProperty = pEntityFactory->getPropDefine("EntityPropDef")->getPropKey(paramList[5]);
		assert(iSrcProperty >= 0);

		bIsFromGiver = true;
	}
	else
	{
		return false;
	}

	context.setInt(PROP_EFFECT_TYPE, (Int32)getEffectType() );
	context.setInt(PROP_EFFECT_TARGETSELECT, iTargetSelect);
	context.setInt(PROP_EFFECT_PROPERTYDST, iDstProperty);
	context.setInt(PROP_EFFECT_PROPERTYSRC, iSrcProperty);
	context.setInt(PROP_EFFECT_PROPERTYADDV, iAddV);
	context.setInt(PROP_EFFECT_PROPERTYADDPERCENT, iAddPercent);
	context.setInt(PROP_EFFECT_ISFROMGIVER, bIsFromGiver);

	return true;
}

vector<string> SkillEffect_AddProperty::getTypeString()
{
	vector<string> resultList;

	resultList.push_back("改变属性");
	resultList.push_back("按参考属性改变属性");
	resultList.push_back("按自身参考属性改变属性");
	

	return resultList;
}

struct EffectPropChgHold:public Detail::EventHandle 
{
	EffectPropChgHold(HEntity hEntity, const EffectContext& ctx, SkillEffect_AddProperty* pEffect):m_hEntity(hEntity), 
		m_effectCtx(ctx), m_pEffect(pEffect){}

	void onEventPropChg(EventArgs& args);

private:

	HEntity m_hEntity;
	const EffectContext& m_effectCtx;
	SkillEffect_AddProperty* m_pEffect;
};


void EffectPropChgHold::onEventPropChg(EventArgs& args)
{
	EventArgs_PropChange& propChgArgs = (EventArgs_PropChange&)args;

	HEntity hGiver = m_effectCtx.getInt(PROP_EFFECT_GIVER);

	// HP 特殊处理，如果导致死亡，需要触发事件
	if(propChgArgs.iPropID == PROP_ENTITY_HP)
	{
		m_pEffect->onChangeHPProcess(hGiver, m_hEntity, propChgArgs.iValue - propChgArgs.iOldValue, m_effectCtx);
		return;
	}

	// 判断属性是否下发,如果下发，触发效果属性改变事件
	IEffectFactory* pEffectFactory = getComponent<IEffectFactory>(COMPNAME_EffectFactory, IID_IEffectFactory);
	assert(pEffectFactory);

	int iShowPropID = 0;
	if(!pEffectFactory->isPropNotify(propChgArgs.iPropID, iShowPropID) ) return;

	m_pEffect->onChgPropProcess(hGiver, m_hEntity, propChgArgs.iPropID, propChgArgs.iValue - propChgArgs.iOldValue, m_effectCtx);
}


bool SkillEffect_AddProperty::doSignleEffect(HEntity hEntity, const EffectContext& effectContext)
{
	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);
	
	// 如果目标刚刚复活，不生效(特殊处理，状态不受限制)
	bool bJustRelive = pEntity->getProperty(PROP_ENTITY_JUSTRELIVE, 0) != 0;
	bool bBuffEffecy = effectContext.getInt(PROP_EFFECT_ISBUFFEFFECT) != 0;
	if(!bBuffEffecy && bJustRelive) return true;


	bool bIsCalcFromGiver = effectContext.getInt(PROP_EFFECT_ISFROMGIVER) == 1;

	HEntity hPropEntity = hEntity;
	if(bIsCalcFromGiver) 
	{
		hPropEntity = effectContext.getInt(PROP_EFFECT_GIVER);
	}

	IEntity* pPropEntity = getEntityFromHandle(hPropEntity);
	assert(pPropEntity);

	int iSrcPropID = effectContext.getInt(PROP_EFFECT_PROPERTYSRC);
	int iDstPropID = effectContext.getInt(PROP_EFFECT_PROPERTYDST);
	int iAddPercent = effectContext.getInt(PROP_EFFECT_PROPERTYADDPERCENT);
	int iAddV = effectContext.getInt(PROP_EFFECT_PROPERTYADDV);


	// 免疫状态，不扣血
	bool bImMunity = pEntity->getProperty(PROP_ENTITY_IMMUNITYDAMAGE, 0);
	if(bImMunity && (iDstPropID == PROP_ENTITY_HP) && (iAddPercent < 0) )
	{
		return true;
	}
	

	// 对BOSS进行的比例扣HP不生效
	int iClassID = pEntity->getProperty(PROP_ENTITY_CLASS, 0);
	if( (iClassID == GSProto::en_class_Monster) && (iDstPropID == PROP_ENTITY_HP) && (iAddPercent < 0) )
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

	HEntity hGiver = effectContext.getInt(PROP_EFFECT_GIVER);
	IEntity* pGiver = getEntityFromHandle(hGiver);
	assert(pGiver);

	// 特殊红色被动，万份比伤害
	int iSpecialDamage = 0;
	if( (iDstPropID == PROP_ENTITY_HP) && ( (iAddPercent < 0) || (iAddV < 0) ))
	{
		int iDamagePercent = pGiver->getProperty(PROP_ENTITY_PERCENTDAMAGE_ENHANCE, 0);
		int iEntityMaxHP = pEntity->getProperty(PROP_ENTITY_MAXHP, 0);
		iSpecialDamage = (double)iEntityMaxHP * (double)iDamagePercent/10000;
	}

	
	int iSkillID = effectContext.getInt(PROP_EFFECT_USESKILL_ID);

	EffectSystem* pEffectSys = static_cast<EffectSystem*>(pGiver->querySubsystem(IID_IEffectSystem));
	assert(pEffectSys);

	int iEnhanceValue = pEffectSys->getPropEnhance(iSkillID, iDstPropID);
	int iEnhancePercent = pEffectSys->getPropPercentEnhance(iSkillID, iDstPropID);

	iAddPercent += iEnhancePercent;
	
	int iSrcValue = pPropEntity->getProperty(iSrcPropID, 0);
	double dAddV = (double)iSrcValue * iAddPercent/10000 + iAddV;

	int iResultAddV = (int)dAddV + iEnhanceValue - iSpecialDamage;

	bool isNegativeEffect = effectContext.getInt(PROP_EFFECT_NEGATIVE) == 1;
	if(isNegativeEffect)
	{
		iResultAddV = 0 - iResultAddV;
	}

	bool bHasLimit = effectContext.getInt(PROP_EFFECT_PROPHASLIMIT) == 1;
	if(bHasLimit)
	{
		int iMinValue = effectContext.getInt(PROP_EFFECT_PROP_MINVALUE);
		int iMaxValue = effectContext.getInt(PROP_EFFECT_PROP_MAXVALUE);

		int iCurValue = pEntity->getProperty(iDstPropID, 0);
		int iResultValue = iCurValue + iResultAddV;

		if(iResultValue < iMinValue) iResultValue = iMinValue;
		if(iMinValue > iMaxValue) iResultValue = iMaxValue;

		iResultAddV = iResultValue - iCurValue;
	}

	EffectPropChgHold tmpHold(hEntity, effectContext, this);
	pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_PROPCHANGE, &tmpHold, &EffectPropChgHold::onEventPropChg);
	pEntity->changeProperty(iDstPropID, iResultAddV, GSProto::en_Reason_SkillUse);
	pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_PROPCHANGE, &tmpHold, &EffectPropChgHold::onEventPropChg);

	// 如果针对百分比伤害有吸血效果，处理
	if(iSpecialDamage > 0)
	{
		int iVampirePercent = pGiver->getProperty(PROP_ENTITY_PERCENTDAMAGE_VAMPIREPERCENT, 0);
		int iRebackHP =  (double)abs(iSpecialDamage) * (double)iVampirePercent/10000;
		EffectPropChgHold tmpHold(hGiver, effectContext, this);
		pGiver->changeProperty(PROP_ENTITY_HP, iRebackHP, 0);
	}
		
	return true;
}



#endif
