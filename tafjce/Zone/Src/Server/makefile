
APP       := ServerEngine
TARGET    := GameServer
#CFLAGS := -fPIC
MFLAGS    := 64
CONFIG    := 
STRIP_FLAG:= N

include ../../CommInclude.mk

INCLUDE += -I/home/mage/tafjce/Common/uuid/

LOCAL_SRC	+= $(wildcard *.cpp)

REMOTE_OBJ	+=  ../../Proto/*.pb.cc ../../Jce/Fight.o 

               
LIB	+= -L../../../lib -lOperateSystem -lManorSystem -lDreamLandSystem -lTaskSystem -lGuideSystem -lFriendSystem -lCampBattleSystem -lArenaServer -lShopSystem -lClimbTowerSystem -lLegion -lGodAnimalSystem -lFightSoulSystem -lLuaWrapper  -lDungeonServer -lItemServer -lFormationSystem -lFightServer  -lHeroSystem -lBuffServer  -lSkillServer  -lEffectServer  -lCoreImp\
 -lMailSystem  -lGameEngine -l:libprotobuf.a
	
LIB_DIR += -ldl -lrt

include /home/tafjce/ServerEngine/DALComm/DALComm.mk
include /home/tafjce/ServerEngine/CommInclude.mk
include /home/tafjce/ServerEngine/CommLib.mk
include /home/tafjce/ServerEngine/SnapshotServer/SnapshotServer.mk
include /home/tafjce/ServerEngine/DbAccess/DbAccess.mk
include /home/tafjce/ServerEngine/NameServer/NameServer.mk
include /home/tafjce/ServerEngine/RoleServer/RoleServer.mk
include /home/tafjce/ServerEngine/RouterServer/RouterServer.mk
include /home/tafjce/ServerEngine/FightDataServer/FightDataServer.mk
include /home/tafjce/ServerEngine/LegionServer/LegionServer.mk
include /home/tafjce/ServerEngine/ConvertCodeServer/ConvertCodeServer.mk
include /home/tafjce/ServerEngine/LJSDKServer/LJSDKServer.mk
include /home/mage/Documents/tool/taf/makefile.taf

#all:
#	if [[ -f /home/mage/Desktop/ServerEngine/Game0/GameServer0/GameServer0 ]];then
#		rm /home/mage/Desktop/ServerEngine/Game0/GameServer0/GameServer0
#	endif
#	cp ./GameServer /home/mage/Desktop/ServerEngine/Game0/GameServer0/GameServer0

CleanTarget:
	rm -rf $(TARGET)
	
