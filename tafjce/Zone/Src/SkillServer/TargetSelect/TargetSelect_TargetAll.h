#ifndef __TARGETSELECT_TARGETALL_H__
#define __TARGETSELECT_TARGETALL_H__

#include "TargetSelect_Base.h"

class SkillTargetSelect_TargetAll:public SkillTargetSelect_Base
{
public:

	virtual void selectTarget(HEntity hAttacker, vector<HEntity>& resultList, const vector<HEntity>& memberList)
	{
		selectAll(false, hAttacker, resultList, memberList);
	}
};



#endif

