#ifndef __SKILLEFFECT_HELPER_H__
#define __SKILLEFFECT_HELPER_H__

class SkillEffect_Helper:public ObjectBase<ISkillEffect>
{
public:

	bool doEffect(HEntity hEntity, const EffectContext& effectContext)
	{
		vector<HEntity> targetList;
		selectTarget(hEntity, effectContext, targetList);

		IEntity* pEntity = getEntityFromHandle(hEntity);
		assert(pEntity);

		bool bResult = true;
		for(size_t i = 0; i < targetList.size(); i++)
		{
			EffectContext tmpContext = effectContext;
			tmpContext.setInt(PROP_EFFECT_CURTARGETCOUNT, (int)targetList.size() );
			HEntity hTarget = targetList[i];

			// 特殊处理下
			IEntity* pTarget = getEntityFromHandle(hTarget);
			int iTmpHP = pTarget->getProperty(PROP_ENTITY_HP, 0);
			if(0 == iTmpHP)
			{
				if(!((hEntity == hTarget) && (pEntity->getProperty(PROP_ENTITY_PREDEAD_USESKILLFLAG, 0) != 0) ) )
				{
					continue;
				}
			}

			// 触发个事件
			EventArgs args;
			args.context.setInt("entity", hTarget);
			args.context.setInt64("ctx", (Int64)&tmpContext);
			pTarget->getEventServer()->setEvent(EVENT_ENTITY_PREEXE_EFFECT, args);

			if(!canUseEffect(hEntity, hTarget, effectContext) )
			{
				continue;
			}
			 
			if(!doSignleEffect(hTarget, tmpContext) )
			{
				bResult = false;
			}
		}

		return bResult;
	}


	virtual bool canUseEffect(HEntity hGiver, HEntity hTarget, const EffectContext& effectContext)
	{
		IEntity* pGiver = getEntityFromHandle(hGiver);
		assert(pGiver);

		// 攻击者死亡不能攻击(神兽无生命。特例)
		int iEntityClass = pGiver->getProperty(PROP_ENTITY_CLASS, 0);
		bool bGiverDeadSkill = pGiver->getProperty(PROP_ENTITY_PREDEAD_USESKILLFLAG, 0) != 0;
		
		int iGiverHP = pGiver->getProperty(PROP_ENTITY_HP, 0);
		if( (GSProto::en_class_GodAnimal != iEntityClass) && (0 >= iGiverHP) && !bGiverDeadSkill)
		{
			return false;
		}

		// 目标死亡不能攻击
		IEntity* pTarget = getEntityFromHandle(hTarget);
		assert(pTarget);

		int iTargetHP = pTarget->getProperty(PROP_ENTITY_HP, 0);
		if( (iTargetHP <= 0) && (pGiver != pTarget) )
		{
			return false;
		}

		// 眩晕不能释放技能, 状态效果无限制
		bool bDizz = pGiver->getProperty(PROP_ENTITY_DIZZ, 0);
		int iSkillID = effectContext.getInt(PROP_EFFECT_USESKILL_ID);
		bool bBuffEffect = effectContext.getInt(PROP_EFFECT_ISBUFFEFFECT) != 0;
		if(bDizz && (iSkillID > 0) && !bBuffEffect)
		{
			return false;
		}

		return true;
	}

	virtual bool isImMunityBuff(int iBuffID, const EffectContext& effectContext){return false;}
	
	virtual bool doSignleEffect(HEntity hEntity, const EffectContext& effectContext) = 0;

	void selectTarget(HEntity hEntity, const EffectContext& effectContext, 
		vector<HEntity>& targetList)
	{
		// Buff 目标，直接设置为自己
		bool isBuffEffect = (effectContext.getInt(PROP_EFFECT_ISBUFFEFFECT) == 1);
		if(isBuffEffect)
		{
			targetList.push_back(hEntity);
			return;
		}
	
		ISkillFactory* pSkillFactory = getComponent<ISkillFactory>("SkillFactory", IID_ISkillFactory);
		assert(pSkillFactory);

		int iTargetSelect = effectContext.getInt(PROP_EFFECT_TARGETSELECT, 0);

		// 是否有固定目标?
		HEntity hFixTarget = effectContext.getInt(PROP_EFFECT_FIXTARGET, 0);
		if(0 != hFixTarget)
		{
			targetList.push_back(hFixTarget);
			return;
		}

		// 特殊处理,替换选择策略
		IEntity* pEntity = getEntityFromHandle(hEntity);
		assert(pEntity);

		int iFixTargetSelect = pEntity->getProperty(PROP_ENTITY_FIXTARGETSELECT, 0);
		if(iFixTargetSelect > 0)
		{
			iTargetSelect = iFixTargetSelect;
		}
		
		const vector<HEntity>* pMemberList = (const vector<HEntity>*)effectContext.getInt64(PROP_EFFECT_FIGHTMEMBERLIST, 0);
		if(!pMemberList)
		{
			targetList.push_back(hEntity);
			return;
		}

		// 目标是否普攻，是否需要替换普攻目标策略
		int iSkillID = effectContext.getInt(PROP_EFFECT_USESKILL_ID);
		int iSkillType = pSkillFactory->getSkillType(iSkillID);
		int iCommSkillReplaceTargetSelect = pEntity->getProperty(PROP_ENTITY_COMMSKILL_REPLACETTARGET, 0);
		if( (en_SkillType_ComAttack == iSkillType) && (iCommSkillReplaceTargetSelect > 0) )
		{
			iTargetSelect = iCommSkillReplaceTargetSelect;
		}

		bool bConfuse = pEntity->getProperty(PROP_ENTITY_CONFUSE, 0) != 0;
		if(bConfuse)
		{
			vector<HEntity> tmpTargetList;
			pSkillFactory->selectTarget(hEntity, en_SkillTargetSelect_SelfAll, tmpTargetList, *pMemberList);
			if(tmpTargetList.size() > 1)
			{
				vector<HEntity>::iterator it = std::find(tmpTargetList.begin(), tmpTargetList.end(), hEntity);
				//assert(it != tmpTargetList.end() );
				if(it != tmpTargetList.end() )
				{
					tmpTargetList.erase(it);
				}

				IRandom* pRandom = getComponent<IRandom>("Random", IID_IMiniAprRandom);
				assert(pRandom);

				int iTmpRandV = pRandom->random() % tmpTargetList.size();
				targetList.push_back(tmpTargetList[iTmpRandV]);
				return;
			}
		}

		// 放到这里吧，优先级最低, 如果使用前一个效果的目标，不计算了,但是要判断目标是否活着
		bool bPreTarget = effectContext.getInt(PROP_EFFECT_USEPRETARGET, 0) != 0;
		vector<HEntity>* pReTarget = (vector<HEntity>*)(effectContext.getInt64(PROP_EFFECT_PRETARGET, 0) );
		if(!bPreTarget || !pReTarget || pReTarget->size() == 0)
		{
			pSkillFactory->selectTarget(hEntity, iTargetSelect, targetList, *pMemberList);
			*pReTarget = targetList;
		}
		else
		{
			// 填充活着的目标
			for(size_t i = 0; i < pReTarget->size(); i++)
			{
				IEntity* pTmpTarget = getEntityFromHandle((*pReTarget)[i]);
				if(!pTmpTarget) continue;

				if(pTmpTarget->getProperty(PROP_ENTITY_HP, 0) > 0)
				{
					targetList.push_back( (*pReTarget)[i]);
				}
			}

			// 这里就不填充了
			//*pReTarget = targetList;
		}
	}


	static void onDeadProcess(HEntity hGiver, HEntity hEntity)
	{
		IEntity* pEntity = getEntityFromHandle(hEntity);
		assert(pEntity);

		IEntity* pGiver = getEntityFromHandle(hGiver);
		assert(pGiver);
	
		int iFixNewHP = pEntity->getProperty(PROP_ENTITY_HP, 0);
	
		{
			EventArgs args;
			args.context.setInt("entity", hGiver);
			pGiver->getEventServer()->setEvent(EVENT_ENTITY_EFFECT_TOCHDOKILL, args);
		}

		{
			EventArgs args;
			args.context.setInt("entity", hEntity);
			pEntity->getEventServer()->setEvent(EVENT_ENTITY_EFFECT_TOCHBEKILL, args);
			int iTmpFixNewHP = args.context.getInt("fixNewHP");
			if(iTmpFixNewHP != 0) iFixNewHP = iTmpFixNewHP;
			
		}

		if(0 == iFixNewHP)
		{
			{
				EventArgs args;
				args.context.setInt("entity", hGiver);
				args.context.setInt("target", hEntity);
				pGiver->getEventServer()->setEvent(EVENT_ENTITY_EFFECTDOKILL, args);
			}
		
			{
				EventArgs args;
				args.context.setInt("entity", hEntity);
				args.context.setInt("killer", hGiver);
				pEntity->getEventServer()->setEvent(EVENT_ENTITY_EFFECTBEKILL, args);
			}
		}
		else
		{
			pEntity->setProperty(PROP_ENTITY_HP, iFixNewHP);

			// 触发复活事件?
			
		}	
	}

	static void onChgPropProcess(HEntity hGiver, HEntity hEntity, int iPropID, int iChgValue, const EffectContext& ctx, bool bReboundDamage = false, bool bRelive = false)
	{
		IEntity* pEntity = getEntityFromHandle(hEntity);
		assert(pEntity);

		IEntity* pGiver = getEntityFromHandle(hGiver);
		assert(pGiver);
	
		EventArgs_FightPropChg fightPropChg;
		fightPropChg.hEventTrigger = hGiver;
		fightPropChg.hEntity = hEntity;
		fightPropChg.hGiver = hGiver;
		fightPropChg.iPropID = iPropID;
		fightPropChg.iChgValue = iChgValue;
		fightPropChg.iValue = pEntity->getProperty(iPropID, 0);
		fightPropChg.effectCtx = &ctx;

		
		fightPropChg.bReboundDamage = bReboundDamage;
		fightPropChg.bRelive = bRelive;
		

		pGiver->getEventServer()->setEvent(EVENT_ENTITY_DOFIGHTPROPCHG, (EventArgs&)fightPropChg);
		pGiver->getEventServer()->setEvent(EVENT_ENTITY_POSTDOFIGHTPROPCHG, (EventArgs&)fightPropChg);

		fightPropChg.hEventTrigger = hEntity;
		fightPropChg.hEntity = hEntity;
		fightPropChg.hGiver = hGiver;
		pEntity->getEventServer()->setEvent(EVENT_ENTITY_BEFIGHTPROPCHG, (EventArgs&)fightPropChg);
		pEntity->getEventServer()->setEvent(EVENT_ENTITY_POSTBEFIGHTPROPCHG, (EventArgs&)fightPropChg);
	}

	static void onChangeHPProcess(HEntity hGiver, HEntity hEntity, int iChgValue, const EffectContext& ctx)
	{
		onChgPropProcess(hGiver, hEntity, PROP_ENTITY_HP, iChgValue, ctx);

		IEntity* pEntity = getEntityFromHandle(hEntity);
		assert(pEntity);
		
		int iLastHP = pEntity->getProperty(PROP_ENTITY_HP, 0);
		if(0 == iLastHP)
		{
			onDeadProcess(hGiver, hEntity);
		}
	}

};


extern void onSkillEffectChangeHPProcess(HEntity hGiver, HEntity hEntity, int iChgValue, const EffectContext& ctx);

#endif
