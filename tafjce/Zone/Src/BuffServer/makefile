
APP       := Aegis
TARGET    := ../../../lib/libBuffServer.a
LFLAGS:= -fPIC
MFLAGS    := 64
CONFIG    := 
STRIP_FLAG:= N

PCH       := BuffServerPch.h.gch
PCH_HEADER:= BuffServerPch.h

include ../../CommInclude.mk

INCLUDE +=-I../../../../tafjce/Common/uuid/
	 	
LOCAL_SRC	+= $(wildcard *.cpp)


include ../../../../tafjce/ServerEngine/DALComm/DALComm.mk
include ../../../../tafjce/ServerEngine/CommInclude.mk
include ../../../../tafjce/ServerEngine/CommLib.mk
include ../../../../tafjce/ServerEngine/RoleServer/RoleServer.mk
include ../../../../tafjce/ServerEngine/FightDataServer/FightDataServer.mk
include ../../../../tools/taf/makefile.taf

