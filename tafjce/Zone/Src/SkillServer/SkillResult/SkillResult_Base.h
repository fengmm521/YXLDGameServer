#ifndef __SKILL_RESULT_BASE_H__
#define __SKILL_RESULT_BASE_H__

class SkillResult_Base
{
public:

	SkillResult_Base(){}
	virtual ~SkillResult_Base(){}

	virtual bool doResult(HEntity hMaster, EffectContext& effectContext) = 0;
	virtual SkillResult_Base* clone() const = 0;
};

#endif



