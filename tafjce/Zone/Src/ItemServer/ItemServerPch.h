#ifndef __ITEM_SERVER_PCH_H__
#define __ITEM_SERVER_PCH_H__


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

//#include "IContainerObj.h"
//#include "IContainerObjFactory.h"
//#include "IEquipSystem.h"
//#include "ContainerObjProperty.h"
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

#include "ItemProperty.h"

#include "IItemFactory.h"
#include "IItemSystem.h"
#include "IDungeonSystem.h"
#include "IEquipBagSystem.h"
#include "IHeroSystem.h"
#include "IGodAnimalSystem.h"

//#include "IInteractiveFactory.h"
//#include "IPetSystem.h"
//#include "IMagicGhostSystem.h"

//#include "RicherProperty.h"
//#include "IRicherFactory.h"
//#include "IRicherSystem.h"
#include "IZoneTime.h"
#include "IReserveWordCheck.h"
#include "IDropFactory.h"

#endif

