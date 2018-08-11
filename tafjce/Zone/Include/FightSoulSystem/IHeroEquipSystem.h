#ifndef __IHEROEQUIP_SYSTEM_H__
#define __IHEROEQUIP_SYSTEM_H__

#include "IFightSoulFactory.h"
#include "IFightSoulSystem.h"

class IHeroEquipSystem:public IEntitySubsystem
{
public:

	virtual bool hasFightSoul() = 0;
};



#endif
