#ifndef __TARGETSELECT_LASTTARGETROW_H__
#define __TARGETSELECT_LASTTARGETROW_H__

#include "TargetSelect_Normal.h"


class SkillTargetSelect_LastTargetRow:public SkillTargetSelect_Normal
{
public:

	virtual void selectTarget(HEntity hAttacker, vector<HEntity>& resultList, const vector<HEntity>& memberList);
};


void SkillTargetSelect_LastTargetRow::selectTarget(HEntity hAttacker, vector<HEntity>& resultList, const vector<HEntity>& memberList)
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

	int iBeginRow = iRow/3*3 + 2;
	int iEndRow = iRow/3*3 - 1;

	for(int i = iBeginRow; i > iEndRow; i--)
	{
		bool bFind = false;
		for(int iCol = 0; iCol < 3; iCol++)
		{
			int iTmpPos = getPos(i, iCol);
			HEntity hTmp = memberList[iTmpPos];

			IEntity* pTmp = getEntityFromHandle(hTmp);
			if(!pTmp) continue;

			int iTmpHP = pTmp->getProperty(PROP_ENTITY_HP, 0);
			if(iTmpHP <= 0) continue;

			if(!bFind) bFind = true;
			resultList.push_back(hTmp);
		}

		if(bFind) break;
	}
}


#endif
