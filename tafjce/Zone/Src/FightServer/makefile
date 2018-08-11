
APP       := Aegis
TARGET    := ../../../lib/libFightServer.a
LFLAGS:= -fPIC
MFLAGS    := 64
CONFIG    := 
STRIP_FLAG:= N

include ../../CommInclude.mk

INCLUDE	+=-I../../../../tafjce/Common/uuid

LOCAL_SRC	+= $(wildcard *.cpp)

include ../../../../tafjce/tafjce/ServerEngine/FightDataServer/FightDataServer.mk	
include ../../../../tafjce/tafjce/ServerEngine/DALComm/DALComm.mk
include ../../../../tafjce/tafjce/ServerEngine/CommInclude.mk
include ../../../../tafjce/tafjce/ServerEngine/CommLib.mk
include ../../../../tafjce/tafjce/ServerEngine/RoleServer/RoleServer.mk
include ../../../../tafjce/tafjce/ServerEngine/LegionServer/LegionServer.mk 
include ../../../../tafjce/tafjce/ServerEngine/ConvertCodeServer/ConvertCodeServer.mk
include ../../../../tools/taf/makefile.taf

JCE_SRC	+= $(wildcard Src/*.jce)

