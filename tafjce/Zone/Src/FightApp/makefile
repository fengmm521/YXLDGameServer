
APP       := ServerEngine
TARGET    := FightServer
#CFLAGS := -fPIC
MFLAGS    := 64
CONFIG    := 
STRIP_FLAG:= N

include ../../CommInclude.mk

INCLUDE +=-I../../../../tafjce/Common/uuid/

LOCAL_SRC	+= $(wildcard *.cpp)

REMOTE_OBJ	+=  ../../Proto/*.pb.cc ../../Jce/Fight.o 

               
LIB	+= -L../../../lib -lOperateSystem -lDreamLandSystem -lTaskSystem -lGuideSystem -lFriendSystem -lCampBattleSystem -lArenaServer -lShopSystem -lClimbTowerSystem -lLegion -lGodAnimalSystem -lFightSoulSystem -lLuaWrapper  -lDungeonServer -lItemServer -lFormationSystem -lFightServer  -lHeroSystem -lBuffServer  -lSkillServer  -lEffectServer\
 -lCoreImp -lMailSystem -lManorSystem  -lGameEngine -l:libprotobuf.a
	
LIB_DIR += -ldl -lrt

include ../../../../tafjce/ServerEngine/DALComm/DALComm.mk
include ../../../../tafjce/ServerEngine/CommInclude.mk
include ../../../../tafjce/ServerEngine/CommLib.mk
include ../../../../tafjce/ServerEngine/SnapshotServer/SnapshotServer.mk
include ../../../../tafjce/ServerEngine/DbAccess/DbAccess.mk
include ../../../../tafjce/ServerEngine/NameServer/NameServer.mk
include ../../../../tafjce/ServerEngine/RoleServer/RoleServer.mk
include ../../../../tafjce/ServerEngine/RouterServer/RouterServer.mk
include ../../../../tafjce/ServerEngine/FightDataServer/FightDataServer.mk
include ../../../../tafjce/ServerEngine/LegionServer/LegionServer.mk
include ../../../../tafjce/ServerEngine/ConvertCodeServer/ConvertCodeServer.mk
include ../../../../tools/taf/makefile.taf

CleanTarget:
	rm -rf $(TARGET)
	
