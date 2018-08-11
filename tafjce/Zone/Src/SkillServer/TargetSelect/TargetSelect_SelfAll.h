#ifndef __TARGETSELECT_SELFALL_H__
#define __TARGETSELECT_SELFALL_H__

#include "TargetSelect_Base.h"

class SkillTargetSelect_SelfAll:public SkillTargetSelect_Base
{
public:

	virtual void selectTarget(HEntity hAttacker, vector<HEntity>& resultList, const vector<HEntity>& memberList)
	{
		selectAll(true, hAttacker, resultList, memberList);
	}
};



#endif
