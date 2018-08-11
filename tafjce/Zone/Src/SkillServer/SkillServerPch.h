#ifndef __SKILLSERVER_PCH_H__
#define __SKILLSERVER_PCH_H__

#include "MiniApr.h"

using namespace MINIAPR;
using namespace std;

#include "servant/Application.h"
//#include "cs_msg_layer.h"
//#include "ss_msg_layer.h"

#include "util/tc_file.h"
#include "define.h"
#include "comm_func.h"
#include "CommLogic.h"
#include "svr_log.h"

#include "GameServer.pb.h"
#include "GameServerExt.pb.h"


#include "DalCommDef.h"
#include "DalMoDef.h"
//#include "DalFacade.h"
//#include "AegisDataDef.h"

#include "ComponentIdDef.h"
#include "ErrorCodeDef.h"
#include "MessageWrapper.h"
#include "IUserStateManager.h"
#include "IEntityFactory.h"
#include "IMessageDispatch.h"
#include "IMessageLayer.h"
#include "EntityProperty.h"
#include "EffectProperty.h"

#include "EntityEventDef.h"
//#include "IActorObserveSystem.h"
#include "IUserStateManager.h"
//#include "ISceneManager.h"
#include "IGlobalCfg.h"
#include "Role.h"
//#include "IContainerObj.h"
//#include "IContainerSystem.h"
//#include "IContainerObjFactory.h"
//#include "IFightFactory.h"
#include "IEffectFactory.h"
#include "IEffectSystem.h"


#include "ISkillFactory.h"
#include "ISkillSystem.h"

#include "LogHelp.h"

#endif

