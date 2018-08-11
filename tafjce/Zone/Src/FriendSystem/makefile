
APP       := Aegis
TARGET    := ../../../lib/libFriendSystem.a
LFLAGS:= -fPIC
MFLAGS    := 64
CONFIG    := 
STRIP_FLAG:= N

PCH       := BuffServerPch.h.gch
PCH_HEADER:= BuffServerPch.h

include ../../CommInclude.mk

INCLUDE +=-I/home/mage/tafjce/Common/uuid
	 	
LOCAL_SRC	+= $(wildcard *.cpp)


include /home/tafjce/ServerEngine/DALComm/DALComm.mk
include /home/tafjce/ServerEngine/CommInclude.mk
include /home/tafjce/ServerEngine/CommLib.mk
include /home/tafjce/ServerEngine/RoleServer/RoleServer.mk
include /home/tafjce/ServerEngine/FightDataServer/FightDataServer.mk
include /home/tafjce/ServerEngine/LegionServer/LegionServer.mk 
include /home/mage/Documents/tool/taf/makefile.taf

