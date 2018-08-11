#ifndef __DOMAINSYSTEM_PCH_H__
#define __DOMAINSYSTEM_PCH_H__

#include "MiniApr.h"

using namespace MINIAPR;
using namespace std;

#include "servant/Application.h"

#include "util/tc_file.h"
#include "define.h"
#include "comm_func.h"
#include "CommLogic.h"
#include "svr_log.h"

#include "MessageWrapper.h"
#include "GameServer.pb.h"
#include "GameServerExt.pb.h"


#include "DalCommDef.h"
#include "DalMoDef.h"
//#include "DalFacade.h"
//#include "AegisDataDef.h"

#include "ComponentIdDef.h"
//#include "Account.h"
#include "MessageWrapper.h"
#include "IEntity.h"
#include "IUserStateManager.h"
#include "IEntityFactory.h"
#include "IMessageDispatch.h"
#include "IMessageLayer.h"
#include "EntityProperty.h"


#include "IZoneTime.h"
#include "EntityEventDef.h"

#include "IUserStateManager.h"

#include "IGlobalCfg.h"
#include "Role.h"

#include "IHeroSystem.h"
#include "uuid.h"

#include "ITimeRangeValue.h"
#include "LogHelp.h"

#include "ItemProperty.h"
#include "IItemFactory.h"
#include "IItemSystem.h"


#include "ErrorCodeDef.h"

#include "IFightFactory.h"
#include "IFightSystem.h"

#include "IDropFactory.h"

#include "IFormationSystem.h"
#include "IJZEntityFactory.h"
#include "IArenaFactory.h"
#include "IModifyDelegate.h"
#include "IManorSystem.h"
#include "IManorFactory.h"
#include "ManorFactory.h"
#include "IDungeonSystem.h"
#include "IArenaFactory.h"
#include "IVIPFactory.h"

#endif

