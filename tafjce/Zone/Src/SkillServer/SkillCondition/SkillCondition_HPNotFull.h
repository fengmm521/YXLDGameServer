#ifndef __SKILLCONDITION_HPNOTFULL_H__
#define __SKILLCONDITION_HPNOTFULL_H__

class SkillCondition_HPNotFull:public SkillCondition_Base
{
public:

	SkillCondition_HPNotFull(int iTargetSelectStrategy):m_iTargetSelectStrategy(iTargetSelectStrategy){}
	~SkillCondition_HPNotFull(){}

	// SkillCondition_Base interface
	virtual bool checkCondition(HEntity hEntity, const EffectContext& effectContext)
	{
		//int iTargetStrategy = effectContext.getInt(PROP_EFFECT_TARGETSELECT);
		const IFightFactory::BattleList* pMemberList = (const IFightFactory::BattleList*)effectContext.getInt64(PROP_EFFECT_FIGHTMEMBERLIST, 0);
		if(!pMemberList) return false;

		ISkillFactory* pSkillFactory = getComponent<ISkillFactory>(COMPNAME_SkillFactory, IID_ISkillFactory);
		assert(pSkillFactory);

		// 选出目标
		IFightFactory::BattleList resultList;
		pSkillFactory->selectTarget(hEntity, m_iTargetSelectStrategy, resultList, *pMemberList);

		for(size_t i = 0; i < resultList.size(); i++)
		{
			HEntity hTmp = resultList[i];
			IEntity* pTmp = getEntityFromHandle(hTmp);
			if(!pTmp) continue;

			int iHP = pTmp->getProperty(PROP_ENTITY_HP, 0);
			int iMaxHP = pTmp->getProperty(PROP_ENTITY_MAXHP, 0);

			if(iHP < iMaxHP)
			{
				return true;
			}
		}

		return false;
	}
	
	virtual SkillCondition_Base* clone()const{return new SkillCondition_HPNotFull(m_iTargetSelectStrategy);}

private:
	
	int m_iTargetSelectStrategy;
};


#endif
