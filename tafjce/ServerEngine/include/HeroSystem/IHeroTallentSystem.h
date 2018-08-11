#ifndef __IHERO_TALLENT_SYSTEM_H__
#define __IHERO_TALLENT_SYSTEM_H__

#include "IEntity.h"


class IHeroTallentSystem:public IEntitySubsystem
{
public:

	virtual bool getTallentLevel(int iTallentID, int& iLevel) = 0;

	virtual void checkAutoUpgrade() = 0;
};


#endif
