
APP       := LZ
TARGET    := ../../../lib/libFormationSystem.a
LFLAGS:= -fPIC
MFLAGS    := 64
CONFIG    := 
STRIP_FLAG:= N

include ../../CommInclude.mk

INCLUDE +=

LOCAL_SRC	+= $(wildcard *.cpp)

include /home/tafjce/ServerEngine/FightDataServer/FightDataServer.mk
include /home/tafjce/ServerEngine/CommInclude.mk
include /home/tafjce/ServerEngine/CommLib.mk
include /home/tafjce/ServerEngine/NameServer/NameServer.mk
include /home/tafjce/ServerEngine/RoleServer/RoleServer.mk 
include /home/mage/Documents/tool/taf/makefile.taf