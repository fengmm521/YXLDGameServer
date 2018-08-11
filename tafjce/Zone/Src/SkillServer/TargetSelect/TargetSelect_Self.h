#ifndef __TARGETSELECT_SELF_H__
#define __TARGETSELECT_SELF_H__

#include "TargetSelect_Base.h"

class SkillTargetSelect_Self:public SkillTargetSelect_Base
{
public:

	virtual void selectTarget(HEntity hAttacker, vector<HEntity>& resultList, const vector<HEntity>& memberList)
	{
		resultList.push_back(hAttacker);
	}
};



#endif
