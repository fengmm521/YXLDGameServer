#ifndef __IACTOR_SAVE_SYSTEM_H__
#define __IACTOR_SAVE_SYSTEM_H__

#include "IEntity.h"

class IActorSaveSystem:public IEntitySubsystem
{
public:

	virtual void doSave(bool bSync) = 0;
};


#endif
