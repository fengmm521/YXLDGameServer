
APP       := ServerEngine
TARGET    := RoleKey
#CFLAGS := -fPIC
MFLAGS    := 64
CONFIG    := 
STRIP_FLAG:= N

include ../../CommInclude.mk

INCLUDE +=

LOCAL_SRC	+= $(wildcard *.cpp)

LIB	+= 
	
LIB_DIR += -ldl -lrt

include /home/tafjce/ServerEngine/DALComm/DALComm.mk
include /home/tafjce/ServerEngine/CommInclude.mk
include /home/tafjce/ServerEngine/CommLib.mk
include /home/tafjce/ServerEngine/DbAccess/DbAccess.mk
include /home/tafjce/ServerEngine/NameServer/NameServer.mk
include /home/tafjce/ServerEngine/RoleServer/RoleServer.mk
include /home/tafjce/ServerEngine/RouterServer/RouterServer.mk
include /home/tafjce/ServerEngine/FightDataServer/FightDataServer.mk
include /home/tafjce/ServerEngine/LegionServer/LegionServer.mk
include /home/mage/Documents/tool/taf/makefile.taf

CleanTarget:
	rm -rf $(TARGET)
	
