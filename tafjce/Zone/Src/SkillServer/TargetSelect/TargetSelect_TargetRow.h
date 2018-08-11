#ifndef __TARGETSELECT_TARGETROW_H__
#define __TARGETSELECT_TARGETROW_H__

#include "TargetSelect_Normal.h"


class SkillTargetSelect_TargetRow:public SkillTargetSelect_Normal
{
public:

	virtual void selectTarget(HEntity hAttacker, vector<HEntity>& resultList, const vector<HEntity>& memberList);
};



void SkillTargetSelect_TargetRow::selectTarget(HEntity hAttacker, vector<HEntity>& resultList, const vector<HEntity>& memberList)
{
	vector<HEntity> tmpResultList;
	SkillTargetSelect_Normal::selectTarget(hAttacker, tmpResultList, memberList);

	if(tmpResultList.size() == 0)
	{
		return;
	}

	assert(tmpResultList.size() == 1);

	HEntity hTarget = tmpResultList[0];
	int iTargetPos = getFixBattlePos(hTarget);

	resultList.push_back(hTarget);
	int iRow = getRow(iTargetPos);

	for(int i = 0; i < 3; i++)
	{
		int iTmpPos = getPos(iRow, i);
		HEntity hTmp = memberList[iTmpPos];

		// 目标不能重复处理，目标要放在最前面
		if(hTmp == hTarget) continue;

		IEntity* pTmp = getEntityFromHandle(hTmp);
		if(!pTmp) continue;

		int iTmpHP = pTmp->getProperty(PROP_ENTITY_HP, 0);
		if(iTmpHP <= 0) continue;

		resultList.push_back(hTmp);
	}
}


#endif

