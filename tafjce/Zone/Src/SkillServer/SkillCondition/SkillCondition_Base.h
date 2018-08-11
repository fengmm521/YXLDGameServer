#ifndef __SKILLCONDITION_BASE_H__
#define __SKILLCONDITION_BASE_H__

class SkillCondition_Base
{
public:

	virtual ~SkillCondition_Base(){}
	virtual bool checkCondition(HEntity hEntity, const EffectContext& effectContext) = 0;
	virtual SkillCondition_Base* clone()const = 0;
};


#endif
