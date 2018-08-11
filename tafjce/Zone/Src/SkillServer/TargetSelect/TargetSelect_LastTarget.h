#ifndef __TARGETSELECT_LASTTARGET_H__
#define __TARGETSELECT_LASTTARGET_H__

#include "TargetSelect_Normal.h"


class SkillTargetSelect_LastTarget:public SkillTargetSelect_Normal
{
public:

	virtual void selectTarget(HEntity hAttacker, vector<HEntity>& resultList, const vector<HEntity>& memberList);
};


void SkillTargetSelect_LastTarget::selectTarget(HEntity hAttacker, vector<HEntity>& resultList, const vector<HEntity>& memberList)
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

	int iRow = getRow(iTargetPos);
	int iCol = getCol(iTargetPos);

	int iBeginRow = iRow/3*3;
	int iEndRow = iBeginRow + 3;

	HEntity hResult;
	for(int i = iBeginRow; i < iEndRow; i++)
	{
		int iTmpPos = getPos(i, iCol);
		HEntity hTmp = memberList[iTmpPos];

		IEntity* pTmp = getEntityFromHandle(hTmp);
		if(!pTmp) continue;

		int iHP = pTmp->getProperty(PROP_ENTITY_HP, 0);
		if(iHP <= 0) continue;

		hResult = hTmp;
	}

	assert(0 != hResult);

	resultList.push_back(hResult);
}


#endif
