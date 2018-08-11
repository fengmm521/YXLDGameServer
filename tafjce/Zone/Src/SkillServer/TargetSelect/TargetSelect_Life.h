#ifndef __TARGETSELECT_LIFE_H__
#define __TARGETSELECT_LIFE_H__

#include "TargetSelect_Base.h"

class SkillTargetSelect_Life:public SkillTargetSelect_Base
{
public:

	SkillTargetSelect_Life(bool bSelf, bool bMax):m_bSelf(bSelf), m_bMax(bMax){}

	virtual void selectTarget(HEntity hAttacker, vector<HEntity>& resultList, const vector<HEntity>& memberList);

private:

	bool m_bSelf;
	bool m_bMax;
};


void SkillTargetSelect_Life::selectTarget(HEntity hAttacker, vector<HEntity>& resultList, const vector<HEntity>& memberList)
{
	vector<HEntity> unfilterTargetList;
	selectAll(m_bSelf, hAttacker, unfilterTargetList, memberList);

	HEntity hRealTarget;

	int iResultHP = std::numeric_limits<int>::min();
	if(!m_bMax) iResultHP = std::numeric_limits<int>::max();
	
	for(size_t i = 0; i < unfilterTargetList.size(); i++)
	{
		IEntity* pTmp = getEntityFromHandle(unfilterTargetList[i]);
		if(!pTmp) continue;

		int iTmpHP = pTmp->getProperty(PROP_ENTITY_HP, 0);
		if(iTmpHP == 0) continue;

		if(m_bMax && (iTmpHP > iResultHP) )
		{
			iResultHP = iTmpHP;
			hRealTarget = pTmp->getHandle();
		}

		if(!m_bMax && (iTmpHP < iResultHP) )
		{
			iResultHP = iTmpHP;
			hRealTarget = pTmp->getHandle();
		}
	}

	if(0 == hRealTarget)
	{
		return;
	}

	resultList.push_back(hRealTarget);
}


#endif
