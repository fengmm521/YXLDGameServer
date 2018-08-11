
APP       := Aegis
TARGET    := ../../../lib/libLuaWrapper.a
LFLAGS:= -fPIC
MFLAGS    := 64
CONFIG    := 
STRIP_FLAG:= N

PCH       := SkillServerPch.h.gch
PCH_HEADER:= SkillServerPch.h

include ../../CommInclude.mk

INCLUDE +=-I/home/mage/tafjce/Common/uuid/

LOCAL_SRC	+= $(wildcard *.cpp)

	

include /home/tafjce/ServerEngine/DALComm/DALComm.mk
include /home/tafjce/ServerEngine/CommInclude.mk
include /home/tafjce/ServerEngine/CommLib.mk
include /home/tafjce/ServerEngine/RoleServer/RoleServer.mk
include /home/tafjce/ServerEngine/FightDataServer/FightDataServer.mk
include /home/mage/Documents/tool/taf/makefile.taf

JCE_SRC	+= $(wildcard Src/*.jce)

