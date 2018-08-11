#ifndef __TARGETSELECT_TARGETCOL_H__
#define __TARGETSELECT_TARGETCOL_H__

#include "TargetSelect_Normal.h"


class SkillTargetSelect_TargetCol:public SkillTargetSelect_Normal
{
public:

	virtual void selectTarget(HEntity hAttacker, vector<HEntity>& resultList, const vector<HEntity>& memberList);
};


void SkillTargetSelect_TargetCol::selectTarget(HEntity hAttacker, vector<HEntity>& resultList, const vector<HEntity>& memberList)
{
	vector<HEntity> tmpResultList;
	SkillTargetSelect_Normal::selectTarget(hAttacker, tmpResultList, memberList);

	if(tmpResultList.size() == 0)
	{
		return;
	}

	assert(tmpResultList.size() == 1);

	HEntity hTarget = tmpResultList[0];

	int iPos = getFixBattlePos(hTarget);
	
	int iCurCol = getCol(iPos);
	int iCurRow = getRow(iPos);

	int iBeginRow = 3;
	int iEndRow = 6;

	if(iCurRow < 3)
	{
		iBeginRow = 0;
		iEndRow = 3;
	}

	resultList.push_back(hTarget);

	for(int i = iBeginRow; i < iEndRow; i++)
	{
		int iTmpPos = getPos(i, iCurCol);
		HEntity hTmp = memberList[iTmpPos];

		// 目标特殊处理，要放在最前面
		if(hTmp == hTarget) continue;

		IEntity* pTmp = getEntityFromHandle(hTmp);
		if(!pTmp) continue;

		int iTmpHP = pTmp->getProperty(PROP_ENTITY_HP, 0);
		if(iTmpHP <= 0) continue;

		resultList.push_back(hTmp);
	}
}

#endif
