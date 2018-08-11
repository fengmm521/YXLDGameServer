#ifndef __SKILLEFFECT_DAMAGE_H__
#define __SKILLEFFECT_DAMAGE_H__

#include "SkillEffect_Helper.h"

class SkillEffect_Damage:public SkillEffect_Helper
{
public:

	virtual bool doSignleEffect(HEntity hEntity, const EffectContext& effectContext);
	virtual bool parseEffect(const std::string& strEffectCmd, EffectContext& context);
	virtual int getEffectType(){return en_SkillEffect_DoDamage;}
	virtual vector<string> getTypeString();

	bool _doSingleEffect(HEntity hEntity, const EffectContext& effectContext);
	void processAnger(HEntity hEntity, const EffectContext& effectContext);
	void processBackAtt(HEntity hEntity, const EffectContext& effectContext);
	void logDamageInfo(HEntity hEntity, int iHitResult, int iResultDamage, const EffectContext& effectContext);
	int calcHitResult(HEntity hGiver, HEntity hEntity,  const EffectContext& effectContext);
	void triggerEvent(HEntity hEntity, int iHitResult, int iResultDamage, const EffectContext& effectContext);
};

int SkillEffect_Damage::calcHitResult(HEntity hGiver, HEntity hEntity,  const EffectContext& effectContext)
{
	ISkillFactory* pSkillFactory = getComponent<ISkillFactory>(COMPNAME_SkillFactory, IID_ISkillFactory);
	assert(pSkillFactory);

	// 如果预设了固定命中,使用固定命中
	int iHitResult = GSProto::en_SkillHitResult_Hit;
	int iFixHitResult = effectContext.getInt(PROP_EFFECT_FIXHITRESULT);
	if(0 != iFixHitResult)
	{
		iHitResult = iFixHitResult;
	}
	else
	{
		iHitResult = pSkillFactory->calcDamageHitResult(hGiver, hEntity, 0);
	}

	return iHitResult;
}


void SkillEffect_Damage::processAnger(HEntity hEntity, const EffectContext& effectContext)
{
	bool bBackAttack = effectContext.getInt(PROP_EFFECT_ISBACKATTACK) == 1;
	if(bBackAttack) return;
	
	int iSkillID = effectContext.getInt(PROP_EFFECT_USESKILL_ID);

	ISkillFactory* pSkillFactory = getComponent<ISkillFactory>(COMPNAME_SkillFactory, IID_ISkillFactory);
	assert(pSkillFactory);

	const ISkill* pTmpSkill = pSkillFactory->getSkillPrototype(iSkillID);
	if(!pTmpSkill) return;

	if( (pTmpSkill->getSkillType() != en_SkillType_ComAttack) && (pTmpSkill->getSkillType() != en_SkillType_SmallActive) )
	{
		return;
	}

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	int iAddValue = pGlobalCfg->getInt("攻击增加怒气", 20);
	int iMaxAngerValue = pGlobalCfg->getInt("最大怒气", 100);

	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	HEntity hGiver = effectContext.getInt(PROP_EFFECT_GIVER);
	IEntity* pGiver = getEntityFromHandle(hGiver);
	assert(pGiver);

	int iEntityHP = pEntity->getProperty(PROP_ENTITY_HP, 0);
	int iGiverHP = pGiver->getProperty(PROP_ENTITY_HP, 0);

	if(iEntityHP > 0)
	{
		pEntity->changeProperty(PROP_ENTITY_ANGER, iAddValue, 0);
		if(pEntity->getProperty(PROP_ENTITY_ANGER, 0) > iMaxAngerValue)
		{
			pEntity->setProperty(PROP_ENTITY_ANGER, iMaxAngerValue);
		}
	}

	if( (iGiverHP > 0) && (pTmpSkill->getSkillType() == en_SkillType_ComAttack) ) 
	{
		pGiver->changeProperty(PROP_ENTITY_ANGER, iAddValue, 0);
		if(pGiver->getProperty(PROP_ENTITY_ANGER, 0) > iMaxAngerValue)
		{
			pGiver->setProperty(PROP_ENTITY_ANGER, iMaxAngerValue);
		}
	}
	
	int iBattlePos = pEntity->getProperty(PROP_ENTITY_BATTLEPOS, 0);
	int iResultAnger = pEntity->getProperty(PROP_ENTITY_ANGER, 0);
	FDLOG("Fight")<<"AngerAdd|"<<iBattlePos<<"|"<<iAddValue<<"|Result|"<<iResultAnger<<endl;
}


bool SkillEffect_Damage::_doSingleEffect(HEntity hEntity, const EffectContext& effectContext)
{
	bool isNegativeEffect = effectContext.getInt(PROP_EFFECT_NEGATIVE) == 1;
	if(isNegativeEffect)
	{
		SvrErrLog("SkillEffect_Damage not support NegativeEffect");
		return true;
	}

	// 这里可能是多重伤害，要判断下目标HP
	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	// 如果目标刚刚复活，不生效(特殊处理，状态不受限制)
	bool bJustRelive = pEntity->getProperty(PROP_ENTITY_JUSTRELIVE, 0) != 0;
	bool bBuffEffecy = effectContext.getInt(PROP_EFFECT_ISBUFFEFFECT) != 0;
	if(!bBuffEffecy && bJustRelive) return true;

	int iBeforHP = pEntity->getProperty(PROP_ENTITY_HP, 0);
	if(0 == iBeforHP) return true;

	HEntity hGiver = effectContext.getInt(PROP_EFFECT_GIVER);
	IEntity* pGiver = getEntityFromHandle(hGiver);
	assert(pGiver);


	// 命中计算
	int iHitResult = calcHitResult(hGiver, hEntity, effectContext);

	if(GSProto::en_SkillHitResult_Doge == iHitResult)
	{
		triggerEvent(hEntity, iHitResult, 0, effectContext);
		return true;
	}

	int iAttValue = pGiver->getProperty(PROP_ENTITY_ATT, 0);
	int iDamagePercent = effectContext.getInt(PROP_EFFECT_DAMAGE_PERCENT);
	int iAddValue = effectContext.getInt(PROP_EFFECT_DAMAGE_ADDVALUE);

	// 如果本次技能是绝技，iAddValue需要增加绝技伤害PROP_ENTITY_SKILLDAMAGE
	ISkillFactory* pSkillFactory = getComponent<ISkillFactory>(COMPNAME_SkillFactory, IID_ISkillFactory);
	assert(pSkillFactory);

	int iSkillID = effectContext.getInt(PROP_EFFECT_USESKILL_ID);
	if(!bBuffEffecy && (en_SkillType_Active == pSkillFactory->getSkillType(iSkillID) ) )
	{
		int iExternSkillDamage = pGiver->getProperty(PROP_ENTITY_SKILLDAMAGE, 0);
		
		// 绝技伤害-绝技防御
		iAddValue += iExternSkillDamage;
	}

	if(!bBuffEffecy && (en_SkillType_SmallActive == pSkillFactory->getSkillType(iSkillID) ) )
	{
		int iExternSmallSkillDamage = pGiver->getProperty(PROP_ENTITY_SMALLSKILLDAMAGE, 0);
		
		// 绝技伤害-绝技防御
		iAddValue += iExternSmallSkillDamage;
	}

	int iEntityExternSkillDef = pEntity->getProperty(PROP_ENTITY_SKILLDEF, 0);
	iAddValue -= iEntityExternSkillDef;

	//计算 防御?
	int iDefValue = pEntity->getProperty(PROP_ENTITY_DEF, 0);
	//dResultV -= iDefValue;

	int iEntityLevel = pEntity->getProperty(PROP_ENTITY_LEVEL, 0);

	// 修改，加入等级规则
	double dResultV = ((double)iAttValue * ((double)iDamagePercent)/10000 + iAddValue)/(1.0f + (double)iEntityLevel/480.0f);

	// 随机[-3,3]
	IRandom* pRandom = getComponent<IRandom>("Random", IID_IMiniAprRandom);
	assert(pRandom);
	
	int iRandV = pRandom->random() % 7 - 3;
	dResultV = dResultV * (1.0 + (double)iRandV/100);

	dResultV = dResultV*(double)iAttValue/(iAttValue + iDefValue * 2);

	// 计算伤害加成(破甲-护甲)
	int iSunder = pGiver->getProperty(PROP_ENTITY_SUNDER, 0);
	int iArmor = pEntity->getProperty(PROP_ENTITY_ARMOR, 0);

	// 注意，这里是千分制(由策划确定)
	double dFixRate = 1.0 + ((double)iSunder - (double)iArmor)/1000;
	dFixRate = std::max(std::min(dFixRate, 2.0), 0.15);
	
	dResultV = dResultV * dFixRate;

	// 技能命中修正
	double dHitResultFix = 1.0;
	if(GSProto::en_SkillHitResult_Knock == iHitResult)
	{
		int iKnockEnhancePercent = pGiver->getProperty(PROP_ENTITY_KNOCKENHANCEPERCENT, 0);
		dHitResultFix = 2.0 + ((double)iKnockEnhancePercent)/10000;
	}
	else if(GSProto::en_SkillHitResult_Block == iHitResult)
	{
		dHitResultFix = 0.5;
	}

	dResultV = dResultV * dHitResultFix;


	// 是否有条件伤害加成条件
	int iCondHPPercent = effectContext.getInt(PROP_EFFECT_CONDHPPERCENT, 0);
	int iEntityMaxHP = pEntity->getProperty(PROP_ENTITY_MAXHP, 0);
	int iTmpHPPercent = (int)((double)iBeforHP/(double)iEntityMaxHP * 10000);
	if(iTmpHPPercent <= iCondHPPercent)
	{
		int iExtraDamageAddPercent = effectContext.getInt(PROP_EFFECT_CONDEXTRADAMAGEPERCENT);
		dResultV = dResultV * (1.0 + (double)iExtraDamageAddPercent/(double)10000);
	}

	// 是否有性别加成
	bool bHasSexAddPercent = effectContext.getInt(PROP_EFFECT_HASSEXADDPERCENT, 0) != 0;
	if(bHasSexAddPercent)
	{
		int iCondSex = effectContext.getInt(PROP_EFFECT_CONDSEX);
		int iTmpSex = pEntity->getProperty(PROP_ENTITY_SEX, 0);
		if(iTmpSex == iCondSex)
		{
			int iSexAddPercent = effectContext.getInt(PROP_EFFECT_CONDSEXDAMAGEADDPERCENT);
			dResultV = dResultV * (1.0 + (double)iSexAddPercent/(double)10000);
		}
	}

	int iResultDamage = (int)dResultV;
	if(iResultDamage <= 0) iResultDamage = 1;


	//Xionhai modify:万份比扣除伤害不应对世界boss生效
	//-- before
	//// 处理红色天赋万份比扣除伤害的
	//int iDamageExternPercent = pGiver->getProperty(PROP_ENTITY_PERCENTDAMAGE_ENHANCE, 0);
	//printf("--damage extern percent:%i\n", iDamageExternPercent);
	//-- after
	//TODO:该方法调用可能比较频繁，也许会有较大性能负面影响.
	int iDamageExternPercent = pGiver->getProperty(PROP_ENTITY_PERCENTDAMAGE_ENHANCE, 0);
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
			iDamageExternPercent = 0;
		}
	}
	//Xionhai modify: end
	
	int iSpecialDamage = (double)iEntityMaxHP * (double)iDamageExternPercent/10000;

	iResultDamage += iSpecialDamage;

	// 免疫伤害处理
	bool bImMunity = pEntity->getProperty(PROP_ENTITY_IMMUNITYDAMAGE, 0);
	if(bImMunity && (iResultDamage > 0) )
	{
		iResultDamage = 0;
		iHitResult = GSProto::en_SkillHitResult_ImMunity;
	}

	pEntity->changeProperty(PROP_ENTITY_HP, 0- iResultDamage, 0);

	// 处理怒气
	processAnger(hEntity, effectContext);
	
	// 触发事件
	triggerEvent(hEntity, iHitResult, iResultDamage, effectContext);
	
	// 处理吸血
	bool bHasVampire = effectContext.getInt(PROP_EFFECT_HASVAMPIRE);
	int iKnockVampirePercent = pGiver->getProperty(PROP_ENTITY_KNOCK_REBACKPERCENT, 0);
	if(bHasVampire || (iSpecialDamage > 0) || ( (iHitResult == GSProto::en_SkillHitResult_Knock) && (iKnockVampirePercent > 0) ) )
	{
		int iVampireHP = 0;

		if(bHasVampire)
		{
			int iVampirePercent = effectContext.getInt(PROP_EFFECT_VAMPIREPERCENT);
			int iEnhanceVampirePercent = pGiver->getProperty(PROP_ENTITY_ENHANCE_VAMPIREPERCENT, 0);
			iVampirePercent += iEnhanceVampirePercent;
			
			iVampireHP += (int) ((double)iResultDamage * (double)iVampirePercent/10000);
		}
		else
		{
			iVampireHP = abs(iResultDamage) * (double)iKnockVampirePercent/10000;
		}

		int iVampireDamagePercent = pGiver->getProperty(PROP_ENTITY_PERCENTDAMAGE_VAMPIREPERCENT, 0);
		iVampireHP += (double)iSpecialDamage * (double)iVampireDamagePercent/10000;
		
		if(iVampireHP <= 0) iVampireHP = 1;
		pGiver->changeProperty(PROP_ENTITY_HP, iVampireHP, 0);

		EventArgs_FightPropChg fightPropChg;
		fightPropChg.hEventTrigger = hGiver;
		fightPropChg.hEntity = hGiver;
		fightPropChg.hGiver = hGiver;
		fightPropChg.iPropID = PROP_ENTITY_HP;
		fightPropChg.iChgValue = iVampireHP;
		fightPropChg.iValue = pGiver->getProperty(PROP_ENTITY_HP, 0);
		fightPropChg.effectCtx = &effectContext;

		pGiver->getEventServer()->setEvent(EVENT_ENTITY_DOFIGHTPROPCHG, (EventArgs&)fightPropChg);
		pGiver->getEventServer()->setEvent(EVENT_ENTITY_POSTDOFIGHTPROPCHG, (EventArgs&)fightPropChg);

		pGiver->getEventServer()->setEvent(EVENT_ENTITY_BEFIGHTPROPCHG, (EventArgs&)fightPropChg);
		pGiver->getEventServer()->setEvent(EVENT_ENTITY_POSTBEFIGHTPROPCHG, (EventArgs&)fightPropChg);
	}

	// 处理伤害并加状态
	int iTmpTargetHP = pEntity->getProperty(PROP_ENTITY_HP, 0);
	int iDamageBuffID = effectContext.getInt(PROP_EFFECT_DAMAGEADDBUFF);
	int iDamageBuffChance = effectContext.getInt(PROP_EFFECT_DAMAGEBUFFCHANCE);
	bool bHasSexAddBuffCond = effectContext.getInt(PROP_EFFECT_HASBUFFSEXCOND);
	int iAddBuffSexCond = effectContext.getInt(PROP_EFFECT_CONDSEX);
	
	if( (iTmpTargetHP > 0) && (iDamageBuffID > 0) && (iDamageBuffChance > 0) )
	{
		bool bSexCheck = true;
		if(bHasSexAddBuffCond)
		{
			int iTmpSex = pEntity->getProperty(PROP_ENTITY_SEX, 0);
			bSexCheck = (iTmpSex == iAddBuffSexCond);
		}

		if(bSexCheck)
		{
			int iRandBuffV = pRandom->random() % 10000;
			if(iRandBuffV < iDamageBuffChance)
			{
				IBuffSystem* pBuffSys = static_cast<IBuffSystem*>(pEntity->querySubsystem(IID_IBuffSystem));
				assert(pBuffSys);

				// 免疫了不生效
				if(!pBuffSys->isImMinityBuff(iDamageBuffID) )
				{
					pBuffSys->addBuff(hGiver, iDamageBuffID);
				}
			}
		}
	}

	// 打日志
	logDamageInfo(hEntity, iHitResult, iResultDamage, effectContext);	

	// 如果击杀了对方,触发事件
	int iEntityLastHP = pEntity->getProperty(PROP_ENTITY_HP, 0);
	if(0 == iEntityLastHP)
	{
		onDeadProcess(hGiver, hEntity);
	}

	bool bBuffDamage = effectContext.getInt(PROP_EFFECT_ISBUFFEFFECT) == 1;
	if(!bBuffDamage && GSProto::en_SkillHitResult_Block == iHitResult)
	{
		// 过滤下，连击不能反击,远程技能不能反击
		const ISkill* pSkillPrototype = pSkillFactory->getSkillPrototype(iSkillID);
		if(pSkillPrototype)
		{
			// 非连击的近战技能才能反击, 如果目标多人，也不能反击, 如果攻击者在混乱状态，也不能反击
			int iTargetCount = effectContext.getInt(PROP_EFFECT_CURTARGETCOUNT);
			bool bAttackerConfuse = pGiver->getProperty(PROP_ENTITY_CONFUSE, 0) != 0;
			if(!pSkillPrototype->isBattleSkill() && pSkillPrototype->canBackAttack() && (iTargetCount == 1) && !bAttackerConfuse)
			{
				processBackAtt(hEntity, effectContext);
			}
		}
	}

	return true;
}


bool SkillEffect_Damage::doSignleEffect(HEntity hEntity, const EffectContext& effectContext)
{
	//PROP_EFFECT_MULTDAMAGECOUNT
	bool bHasMultDamage = effectContext.getInt(PROP_EFFECT_ISMULTDAMAGE) == 1;
	int iMultCount = effectContext.getInt(PROP_EFFECT_MULTDAMAGECOUNT);

	if(!bHasMultDamage)
	{
		return _doSingleEffect(hEntity, effectContext);
	}

	for(int i = 0; i < iMultCount; i++)
	{
		_doSingleEffect(hEntity, effectContext);
	}

	return true;
}

void SkillEffect_Damage::triggerEvent(HEntity hEntity, int iHitResult, int iResultDamage, const EffectContext& effectContext)
{
	EventArgsDamageCtx eventCtx;

	eventCtx.hEntity = hEntity;
	eventCtx.hGiver = effectContext.getInt(PROP_EFFECT_GIVER);
	eventCtx.iHitResult = iHitResult;
	eventCtx.iResultDamage = iResultDamage;
	eventCtx.effectCtx = &effectContext;

	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	IEntity* pGiver = getEntityFromHandle(eventCtx.hGiver);
	assert(pGiver);

	pGiver->getEventServer()->setEvent(EVENT_ENTITY_PREDODAMAGE, (EventArgs&)eventCtx);
	pEntity->getEventServer()->setEvent(EVENT_ENTITY_PREBEDAMAGE, (EventArgs&)eventCtx);

	
	pGiver->getEventServer()->setEvent(EVENT_ENTITY_DODAMAGE, (EventArgs&)eventCtx);
	pEntity->getEventServer()->setEvent(EVENT_ENTITY_BEDAMAGE, (EventArgs&)eventCtx);

	pGiver->getEventServer()->setEvent(EVENT_ENTITY_POSTDODAMAGE, (EventArgs&)eventCtx);
	pEntity->getEventServer()->setEvent(EVENT_ENTITY_POSTBEDAMAGE, (EventArgs&)eventCtx);
}

void SkillEffect_Damage::logDamageInfo(HEntity hEntity, int iHitResult, int iResultDamage, const EffectContext& effectContext)
{
	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	HEntity hGiver = effectContext.getInt(PROP_EFFECT_GIVER);
	IEntity* pGiver = getEntityFromHandle(hGiver);
	assert(pGiver);
	
	int iGiverPos = pGiver->getProperty(PROP_ENTITY_BATTLEPOS, 0);
	int iEntityPos = pEntity->getProperty(PROP_ENTITY_BATTLEPOS, 0);
	int iTriggerSkillID = effectContext.getInt(PROP_EFFECT_USESKILL_ID);
	int iLeftHP = pEntity->getProperty(PROP_ENTITY_HP, 0);
	bool bBackAttack = effectContext.getInt(PROP_EFFECT_ISBACKATTACK) == 1;
	string strAttackPrx = "|Normal|";
	if(bBackAttack)
	{
		strAttackPrx = "|backAtt|";
	}
	FDLOG("Fight")<<"Damage|"<<iGiverPos<<strAttackPrx<<"|Fight|"<<iEntityPos<<"|Skill|"<<iTriggerSkillID<<"|HitResult|"<<iHitResult<<"|Damage|"<<iResultDamage
	<<"|leftHP|"<<iLeftHP
	<<endl;
}


void SkillEffect_Damage::processBackAtt(HEntity hEntity, const EffectContext& effectContext)
{
	// 如果触发抵挡,并且玩家未死亡，执行一次反击
	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);
	
	int iHPAfterAtt = pEntity->getProperty(PROP_ENTITY_HP, 0);
	if(iHPAfterAtt <= 0)
	{
		return;
	}

	//PROP_EFFECT_ISBACKATTACK
	HEntity hGiver = effectContext.getInt(PROP_EFFECT_GIVER);
	ISkillSystem* pSkillSystem = static_cast<ISkillSystem*>(pEntity->querySubsystem(IID_ISkillSystem));
	assert(pSkillSystem);

	pSkillSystem->backAttack(hGiver);
}


bool SkillEffect_Damage::parseEffect(const std::string& strEffectCmd, EffectContext& context)
{
	vector<string> paramList = TC_Common::sepstr<string>(strEffectCmd, Effect_MagicSep);
	assert(paramList.size() > 0);

	if(paramList[0] == "执行伤害")
	{
		if(paramList.size() != 4)
		{
			return false;
		}

		int iTargetSelect = AdvanceAtoi(paramList[1]);
		int iDamagePercent = AdvanceAtoi(paramList[2]);
		int iAddDamageV = AdvanceAtoi(paramList[3]);

		context.setInt(PROP_EFFECT_TYPE, (Int32)getEffectType() );
		context.setInt(PROP_EFFECT_TARGETSELECT, iTargetSelect);
		context.setInt(PROP_EFFECT_DAMAGE_PERCENT, iDamagePercent);
		context.setInt(PROP_EFFECT_DAMAGE_ADDVALUE, iAddDamageV);
	}
	else if( (paramList[0] == "执行伤害并加状态") || (paramList[0] == "执行伤害并给女性添加状态") )
	{
		if( (paramList.size() != 5) && (paramList.size() != 6) )
		{
			return false;
		}
	
		int iTargetSelect = AdvanceAtoi(paramList[1]);
		int iDamagePercent = AdvanceAtoi(paramList[2]);
		int iAddDamageV = AdvanceAtoi(paramList[3]);
		int iBuffID = AdvanceAtoi(paramList[4]);
		int iBuffChance = 10000;
		if(paramList.size() == 6)
		{
			iBuffChance = AdvanceAtoi(paramList[5]);
		}

		context.setInt(PROP_EFFECT_TYPE, (Int32)getEffectType() );
		context.setInt(PROP_EFFECT_TARGETSELECT, iTargetSelect);
		context.setInt(PROP_EFFECT_DAMAGE_PERCENT, iDamagePercent);
		context.setInt(PROP_EFFECT_DAMAGE_ADDVALUE, iAddDamageV);

		context.setInt(PROP_EFFECT_DAMAGEADDBUFF, iBuffID);
		context.setInt(PROP_EFFECT_DAMAGEBUFFCHANCE, iBuffChance);

		if(paramList[0] == "执行伤害并给女性添加状态")
		{
			context.setInt(PROP_EFFECT_CONDSEX, GSProto::en_Sex_FeMale);
			context.setInt(PROP_EFFECT_HASBUFFSEXCOND,  1);
		}
	}
	else if(paramList[0] == "暴击执行伤害")
	{
		int iTargetSelect = AdvanceAtoi(paramList[1]);
		int iDamagePercent = AdvanceAtoi(paramList[2]);
		int iAddDamageV = AdvanceAtoi(paramList[3]);

		context.setInt(PROP_EFFECT_TYPE, (Int32)getEffectType() );
		context.setInt(PROP_EFFECT_TARGETSELECT, iTargetSelect);
		context.setInt(PROP_EFFECT_DAMAGE_PERCENT, iDamagePercent);
		context.setInt(PROP_EFFECT_DAMAGE_ADDVALUE, iAddDamageV);
		context.setInt(PROP_EFFECT_FIXHITRESULT, GSProto::en_SkillHitResult_Knock);
	}
	else if(paramList[0] == "多重伤害")
	{
		if(paramList.size() != 5)
		{
			return false;
		}

		int iTargetSelect = AdvanceAtoi(paramList[1]);
		int iDamagePercent = AdvanceAtoi(paramList[2]);
		int iAddDamageV = AdvanceAtoi(paramList[3]);
		int iMultDamageCount = AdvanceAtoi(paramList[4]);

		context.setInt(PROP_EFFECT_TYPE, (Int32)getEffectType() );
		context.setInt(PROP_EFFECT_TARGETSELECT, iTargetSelect);
		context.setInt(PROP_EFFECT_DAMAGE_PERCENT, iDamagePercent);
		context.setInt(PROP_EFFECT_DAMAGE_ADDVALUE, iAddDamageV);
		context.setInt(PROP_EFFECT_ISMULTDAMAGE, 1);
		context.setInt(PROP_EFFECT_MULTDAMAGECOUNT, iMultDamageCount);
	}
	else if(paramList[0] == "执行伤害并吸血")
	{
		if(paramList.size() != 5)
		{
			return false;
		}

		int iTargetSelect = AdvanceAtoi(paramList[1]);
		int iDamagePercent = AdvanceAtoi(paramList[2]);
		int iAddDamageV = AdvanceAtoi(paramList[3]);
		int iVampirePercent = AdvanceAtoi(paramList[4]);

		context.setInt(PROP_EFFECT_TYPE, (Int32)getEffectType() );
		context.setInt(PROP_EFFECT_TARGETSELECT, iTargetSelect);
		context.setInt(PROP_EFFECT_DAMAGE_PERCENT, iDamagePercent);
		context.setInt(PROP_EFFECT_DAMAGE_ADDVALUE, iAddDamageV);
		context.setInt(PROP_EFFECT_HASVAMPIRE, 1);
		context.setInt(PROP_EFFECT_VAMPIREPERCENT, iVampirePercent);
	}
	else if( (paramList[0] == "低血多倍伤害") && (paramList.size() == 6) )
	{
		int iTargetSelect = AdvanceAtoi(paramList[1]);
		int iDamagePercent = AdvanceAtoi(paramList[2]);
		int iAddDamageV = AdvanceAtoi(paramList[3]);
		int iHPCondPercent = AdvanceAtoi(paramList[4]);
		int iCondExtraDamagePercent = AdvanceAtoi(paramList[5]);

		context.setInt(PROP_EFFECT_TYPE, (Int32)getEffectType() );
		context.setInt(PROP_EFFECT_TARGETSELECT, iTargetSelect);
		context.setInt(PROP_EFFECT_DAMAGE_PERCENT, iDamagePercent);
		context.setInt(PROP_EFFECT_DAMAGE_ADDVALUE, iAddDamageV);
	
		context.setInt(PROP_EFFECT_CONDHPPERCENT, iHPCondPercent);
		context.setInt(PROP_EFFECT_CONDEXTRADAMAGEPERCENT, iCondExtraDamagePercent);
	}
	else if( (paramList[0] == "女性多倍伤害") && (paramList.size() == 5) )
	{
		int iTargetSelect = AdvanceAtoi(paramList[1]);
		int iDamagePercent = AdvanceAtoi(paramList[2]);
		int iAddDamageV = AdvanceAtoi(paramList[3]);
		int iSexAddPercent = AdvanceAtoi(paramList[4]);

		context.setInt(PROP_EFFECT_TYPE, (Int32)getEffectType() );
		context.setInt(PROP_EFFECT_TARGETSELECT, iTargetSelect);
		context.setInt(PROP_EFFECT_DAMAGE_PERCENT, iDamagePercent);
		context.setInt(PROP_EFFECT_DAMAGE_ADDVALUE, iAddDamageV);

		context.setInt(PROP_EFFECT_HASSEXADDPERCENT, 1);
		context.setInt(PROP_EFFECT_CONDSEX, GSProto::en_Sex_FeMale);
		context.setInt(PROP_EFFECT_CONDSEXDAMAGEADDPERCENT, iSexAddPercent);
		
	}
	else
	{
		return false;
	}

	return true;
}

vector<string> SkillEffect_Damage::getTypeString()
{
	vector<string> resultList;

	resultList.push_back("执行伤害");
	resultList.push_back("多重伤害");
	resultList.push_back("执行伤害并吸血");
	resultList.push_back("低血多倍伤害");
	resultList.push_back("女性多倍伤害");
	resultList.push_back("暴击执行伤害");

	resultList.push_back("执行伤害并加状态");
	resultList.push_back("执行伤害并给女性添加状态");

	//PROP_EFFECT_HASBUFFSEXCOND

	return resultList;
}


#endif


