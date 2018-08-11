#ifndef __GAMEENGINE_PCH__
#define __GAMEENGINE_PCH__

#include "MiniApr.h"

using namespace MINIAPR;
using namespace std;

#include "servant/Application.h"

#include "util/tc_file.h"
#include "define.h"
#include "comm_func.h"
#include "CommLogic.h"
#include "svr_log.h"

#include "DalCommDef.h"
#include "DalMoDef.h"
#include "MessageWrapper.h"

#include "GameServer.pb.h"
#include "GameServerExt.pb.h"

//#include "DalFacade.h"
//#include "AegisDataDef.h"

#include "IEntity.h"
#include "ComponentIdDef.h"
#include "MessageWrapper.h"
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


#endif

