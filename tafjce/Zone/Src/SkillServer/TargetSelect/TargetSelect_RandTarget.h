#ifndef __TARGETSELECT_RANDTARGET_H__
#define __TARGETSELECT_RANDTARGET_H__

#include "TargetSelect_Normal.h"


class SkillTargetSelect_RandTarget:public SkillTargetSelect_Normal
{
public:

	virtual void selectTarget(HEntity hAttacker, vector<HEntity>& resultList, const vector<HEntity>& memberList);
};


void SkillTargetSelect_RandTarget::selectTarget(HEntity hAttacker, vector<HEntity>& resultList, const vector<HEntity>& memberList)
{
	vector<HEntity> tmpResultList;
	selectAll(false, hAttacker, tmpResultList, memberList);

	if(tmpResultList.size() == 0)
	{
		return;
	}

	IRandom* pRandom = getComponent<IRandom>("Random", IID_IMiniAprRandom);
	assert(pRandom);

	HEntity hTarget = tmpResultList[pRandom->random() % tmpResultList.size()];

	resultList.push_back(hTarget);
}


#endif
