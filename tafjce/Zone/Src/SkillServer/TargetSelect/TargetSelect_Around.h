#ifndef __TARGETSELECT_AROUND_H__
#define __TARGETSELECT_AROUND_H__

#include "TargetSelect_Base.h"


class SkillTargetSelect_ARound:public SkillTargetSelect_Base
{
public:

	SkillTargetSelect_ARound(int iFirstTargetSelect):m_iFirstTargetStrategy(iFirstTargetSelect){}

	virtual void selectTarget(HEntity hAttacker, vector<HEntity>& resultList, const vector<HEntity>& memberList);

	bool checkEntityCanTarget(HEntity hEntity);

private:

	int m_iFirstTargetStrategy;
};


bool SkillTargetSelect_ARound::checkEntityCanTarget(HEntity hEntity)
{
	IEntity* pEntity = getEntityFromHandle(hEntity);
	if(!pEntity) return false;

	int iTmpHP = pEntity->getProperty(PROP_ENTITY_HP, 0);
	if(iTmpHP <= 0) return false;

	return true;
}


void SkillTargetSelect_ARound::selectTarget(HEntity hAttacker, vector<HEntity>& resultList, const vector<HEntity>& memberList)
{
	ISkillFactory* pSkillFactory = getComponent<ISkillFactory>(COMPNAME_SkillFactory, IID_ISkillFactory);
	assert(pSkillFactory);

	vector<HEntity> unfilterTargetList;
	pSkillFactory->selectTarget(hAttacker, m_iFirstTargetStrategy, unfilterTargetList, memberList);

	if(unfilterTargetList.size() == 0) return;

	HEntity hCenterTarget = unfilterTargetList[0];

	int iCenterPos = getFixBattlePos(hCenterTarget);
	int iRow = getRow(iCenterPos);
	int iCol = getCol(iCenterPos);

	int iSide = iRow/3;

	resultList.push_back(hCenterTarget);
	for(int i = -1; i < 2; i++)
	{
		if(i == 0) continue;

		// 计算横排的,注意，横排可能会切换到对方这里要过滤，保证在同一方
		int iTmpPos = -1;
		int iTmpRow = iRow + i;
		if(iSide == iTmpRow/3)
		{
			if( (iTmpRow >= 0) && (iTmpRow < GSProto::MAX_BATTLE_MEMBER_SIZE/3) )
			{
				iTmpPos = getPos(iTmpRow, iCol);
			}

			if( (iTmpPos >= 0) && checkEntityCanTarget(memberList[iTmpPos]) )
			{
				resultList.push_back(memberList[iTmpPos]);
			}
		}

		// 计算纵列的
		iTmpPos = -1;
		int iTmpCol = iCol + i;
		if( (iTmpCol >= 0) && (iTmpCol < 3) )
		{
			iTmpPos = getPos(iRow, iTmpCol);
		}

		if( (iTmpPos >= 0) && checkEntityCanTarget(memberList[iTmpPos]) )
		{
			resultList.push_back(memberList[iTmpPos]);
		}
	}
}


#endif
