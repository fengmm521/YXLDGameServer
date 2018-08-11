#ifndef _IDREAMLANDSYSTEM_H_
#define _IDREAMLANDSYSTEM_H_

#include "IEntity.h"


class IDreamLandSystem:public IEntitySubsystem
{
public:
	virtual void GMRequsetFight(int iSection, int iSceneID) = 0;
	virtual void GMRequsetFormation(int iSection, int iSceneID) = 0;
	virtual void CMClearData() = 0;
	virtual void GMAddDreamLandResetTimes() = 0;
};


#endif

























