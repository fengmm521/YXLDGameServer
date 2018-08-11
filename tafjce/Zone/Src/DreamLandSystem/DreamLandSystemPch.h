#ifndef __CLIMBTOWER_SYSTEM_PCH_H__
#define __CLIMBTOWER_SYSTEM_PCH_H__


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


#include "MessageWrapper.h"
#include "IUserStateManager.h"
#include "IEntityFactory.h"
#include "IMessageDispatch.h"
#include "IMessageLayer.h"
#include "EntityProperty.h"
#include "EntityEventDef.h"
//#include "IActorObserveSystem.h"
#include "IUserStateManager.h"
//#include "ISceneManager.h"
#include "IGlobalCfg.h"
#include "Role.h"

#include "uuid.h"

#include "IEffectFactory.h"
#include "IEffectSystem.h"
#include "EffectDefine.h"
#include "EffectProperty.h"

#include "ISkillFactory.h"
#include "ISkillSystem.h"

#include "ComponentIdDef.h"
#include "IBuffFactory.h"
#include "IBuffSystem.h"
#include "RoleData.h"


#include "IClimbTowerFactory.h"
#include "IClimbTowerSystem.h"


#include "ErrorCodeDef.h"

#include "IFightFactory.h"
#include "IFightSystem.h"

#include "IDropFactory.h"
#include "IZoneTime.h"

#include "LogHelp.h"
#include "IDungeonSystem.h"
#include "IVIPFactory.h"


#include "IDreamLandSystem.h"
#include "IDreamLandFactory.h"
#include "IHeroSystem.h"
#include "IFormationSystem.h"
#include "IArenaFactory.h"
#include "IJZEntityFactory.h"
#include "IModifyDelegate.h"
#include "IGodAnimalSystem.h"
#include "IShopSystem.h"
#include "ILegionFactory.h"

#endif

