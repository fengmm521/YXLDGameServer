
APP       := Aegis
TARGET    := ../../../lib/libEffectServer.a
LFLAGS:= -fPIC
MFLAGS    := 64
CONFIG    := 
STRIP_FLAG:= N

PCH       := EffectServerPch.h.gch
PCH_HEADER:= EffectServerPch.h

include ../../CommInclude.mk

INCLUDE	+=-I../../../../tafjce/Common/uuid/

LOCAL_SRC	+= $(wildcard *.cpp EffectImp/*.cpp)

	
include ../../../../tafjce/ServerEngine/DALComm/DALComm.mk
include ../../../../tafjce/ServerEngine/CommInclude.mk
include ../../../../tafjce/ServerEngine/CommLib.mk
include ../../../../tafjce/ServerEngine/RoleServer/RoleServer.mk
#include ../../../../DCache/ProxyServer/ProxyServer.mk
include ../../../../tafjce/ServerEngine/FightDataServer/FightDataServer.mk
include ../../../../tools/taf/makefile.taf

JCE_SRC	+= $(wildcard Src/*.jce)

