#ifndef __SKILL_FACTORY_H__
#define __SKILL_FACTORY_H__

#include "ISkillFactory.h"
#include "TargetSelect/TargetSelect_Base.h"
#include "SkillCondition/SkillCondition_Base.h"

class SkillFactory:public ComponentBase<ISkillFactory, IID_ISkillFactory>
{
public:

	struct SkillLvCond
	{
		SkillLvCond():iLevel(0), iFightValue(0), iSkillLv(0){}
		int iLevel;
		int iFightValue;
		int iSkillLv;
	};

	SkillFactory();
	~SkillFactory();

	// IComponent Interface
	virtual bool initlize(const PropertySet& propSet);

	// ISkillFactory Interface
	virtual const ISkill* getSkillPrototype(int nSillID);
	virtual int calcDamageHitResult(HEntity hAttacker, HEntity hTarget, int iDamageType);
	virtual void selectTarget(HEntity hAttacker ,int iSelectStrategy, vector<HEntity>& resultList, const vector<HEntity>& memberList);
	virtual int getSkillType(int iSkillID);
	virtual int getSkillID(int iSkillGrpID, int iLevel);
	
public:
	
	SkillTargetSelect_Base* getTargetSelect(int nSelectStrategy);
    ISkill* getNextLvlSkillPrototype(ISkill* pCurSkill);
private:

	bool loadSkill();
	bool loadTargetSelectStrategy();

	SkillCondition_Base* parseSkillCondition(const std::string& strCondition);
	bool calcHit(HEntity hAttacker, HEntity hTarget);
	bool calcBlock(HEntity hAttacker, HEntity hTarget);
	bool calcKnock(HEntity hAttacker, HEntity hTarget);
	int rangeValue(int iMinValue, int iMaxValue, int iValue);

private:

	typedef std::map<int, ISkill*> SkillMap;
	SkillMap m_skillProtoMap;

  
    map<int,map<int,ISkill*> >   m_skillGroup;
    
	std::map<int, SkillTargetSelect_Base*> m_targetSelectList;

	typedef map<int, vector<SkillLvCond> > MapSkillLvRule;
	MapSkillLvRule m_actorSkillLevelRule;
	MapSkillLvRule m_petSkillLevelRule;
};

#endif
