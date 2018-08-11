#ifndef __LEGION_PCH_H__
#define __LEGION_PCH_H__


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

#include "ErrorCodeDef.h"

#include "IFightFactory.h"
#include "IFightSystem.h"

#include "IDropFactory.h"

#include "Legion.h"

#include "ILegionFactory.h"
#include "ILegionSystem.h"

#include "IJZMessageLayer.h"

#include "IReserveWordCheck.h"

#include "IBossFactory.h"
#include "IDungeonSystem.h"
#include "IVIPFactory.h"
#include "IShopFactory.h"
#include "IMailSystem.h"
#include "IModifyDelegate.h"
#include "IArenaFactory.h"
#endif

