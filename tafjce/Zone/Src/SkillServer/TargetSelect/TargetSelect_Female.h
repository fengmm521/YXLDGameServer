#ifndef __TARGETSELECT_FEMALE_H__
#define __TARGETSELECT_FEMALE_H__

#include "TargetSelect_Base.h"

class SkillTargetSelect_Female:public SkillTargetSelect_Base
{
public:

	virtual void selectTarget(HEntity hAttacker, vector<HEntity>& resultList, const vector<HEntity>& memberList);
};


void SkillTargetSelect_Female::selectTarget(HEntity hAttacker, vector<HEntity>& resultList, const vector<HEntity>& memberList)
{
	vector<HEntity> tmpResultList;
	selectAll(false, hAttacker, tmpResultList, memberList);

	vector<HEntity> femaleList;
	for(size_t i = 0; i < tmpResultList.size(); i++)
	{
		IEntity* pTmpEntity = getEntityFromHandle(tmpResultList[i]);
		if(!pTmpEntity) continue;

		int iSex = pTmpEntity->getProperty(PROP_ENTITY_SEX, 0);
		if(iSex == GSProto::en_Sex_FeMale)
		{
			femaleList.push_back(tmpResultList[i]);
		}
	}

	IRandom* pRandom = getComponent<IRandom>(COMPNAME_Random, IID_IMiniAprRandom);
	assert(pRandom);

	if(femaleList.size() > 0)
	{
		HEntity hRet = femaleList[pRandom->random()%femaleList.size()];
		resultList.push_back(hRet);
		return;
	}

	if(tmpResultList.size() > 0)
	{
		HEntity hRet = tmpResultList[pRandom->random()%tmpResultList.size()];
		resultList.push_back(hRet);
	}
}


#endif
