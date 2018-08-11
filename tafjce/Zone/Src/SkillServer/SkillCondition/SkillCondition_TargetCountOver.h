#ifndef __SKILLCONDITION_TARGETCOUNTOVER_H__
#define __SKILLCONDITION_TARGETCOUNTOVER_H__

class SkillCondition_TargetCountOver:public SkillCondition_Base
{
public:

	SkillCondition_TargetCountOver(int iTargetSelect, int iCount):m_iTargetSelectStrategy(iTargetSelect), m_iCount(iCount){}

	// SkillCondition_Base interface
	virtual bool checkCondition(HEntity hEntity, const EffectContext& effectContext)
	{
		const IFightFactory::BattleList* pMemberList = (const IFightFactory::BattleList*)effectContext.getInt64(PROP_EFFECT_FIGHTMEMBERLIST, 0);
		if(!pMemberList) return false;

		ISkillFactory* pSkillFactory = getComponent<ISkillFactory>(COMPNAME_SkillFactory, IID_ISkillFactory);
		assert(pSkillFactory);

		// 选出目标
		IFightFactory::BattleList resultList;
		pSkillFactory->selectTarget(hEntity, m_iTargetSelectStrategy, resultList, *pMemberList);

		return resultList.size() > (size_t)m_iCount;
	}
	
	virtual SkillCondition_Base* clone()const {return new SkillCondition_TargetCountOver(m_iTargetSelectStrategy, m_iCount);}

private:

	int m_iTargetSelectStrategy;
	int m_iCount;
};


#endif
