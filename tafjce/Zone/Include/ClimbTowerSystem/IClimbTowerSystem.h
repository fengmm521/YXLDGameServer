#ifndef _ICLIMBTOWERSYSTEM_H_
#define _ICLIMBTOWERSYSTEM_H_

#include "IEntity.h"


class IClimbTowerSystem:public IEntitySubsystem
{
public:
	virtual void addTowerResetCount(bool ensuer)= 0;

	virtual bool canSkip() = 0;

	virtual int getTowerTop() = 0;
};


#endif

























