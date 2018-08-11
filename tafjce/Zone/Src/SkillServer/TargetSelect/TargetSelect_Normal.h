#ifndef __TARGETSELECT_NORMAL_H__
#define __TARGETSELECT_NORMAL_H__

#include "TargetSelect_Base.h"


class SkillTargetSelect_Normal:public SkillTargetSelect_Base
{
public:

	SkillTargetSelect_Normal(){}
	~SkillTargetSelect_Normal(){}

	virtual void selectTarget(HEntity hAttacker, vector<HEntity>& resultList, const vector<HEntity>& memberList);
};


void SkillTargetSelect_Normal::selectTarget(HEntity hAttacker, vector<HEntity>& resultList, const vector<HEntity>& memberList)
{
	if(memberList.size() == 0)
	{
		return;
	}
	
	int iPos = getFixBattlePos(hAttacker);

	int iCurCol = getCol(iPos);
	int iCurRow = getRow(iPos);

	int iBeginRow = 3;
	int iEndRow = 6;

	if(iCurRow >= 3)
	{
		iBeginRow = 0;
		iEndRow = 3;
	}

	static vector<int> s_szColOrder[3];
	static bool s_bInited = false;

	if(!s_bInited)
	{
		s_bInited = true;
		s_szColOrder[0].push_back(0);
		s_szColOrder[0].push_back(1);
		s_szColOrder[0].push_back(2);
		
		s_szColOrder[1].push_back(1);
		s_szColOrder[1].push_back(0);
		s_szColOrder[1].push_back(2);

		s_szColOrder[2].push_back(2);
		s_szColOrder[2].push_back(1);
		s_szColOrder[2].push_back(0);
	}

	// 修改为优先打前排
	for(int i = iBeginRow; i < iEndRow; i++)
	{
		const vector<int>& colOrderList = s_szColOrder[iCurCol];
		for(vector<int>::const_iterator it = colOrderList.begin(); it != colOrderList.end(); it++)
		{
			int iTmpPos = getPos(i, *it);
			HEntity hTmpEntity = memberList[iTmpPos];

			IEntity* pTmpEntity = getEntityFromHandle(hTmpEntity);
			if(!pTmpEntity) continue;

			int iTmpHP = pTmpEntity->getProperty(PROP_ENTITY_HP, 0);
			if(iTmpHP <= 0) continue;

			resultList.push_back(hTmpEntity);
			return;
		}
	}
}


#endif


