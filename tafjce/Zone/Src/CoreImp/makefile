
APP       := LZ
TARGET    := ../../../lib/libCoreImp.a
LFLAGS:= -fPIC
MFLAGS    := 64
CONFIG    := 
STRIP_FLAG:= N

include ../../CommInclude.mk

INCLUDE +=-I../../../../tafjce/Common/uuid

LOCAL_SRC	+= $(wildcard *.cpp)

include ../../../../tafjce/ServerEngine/SnapshotServer/SnapshotServer.mk
include ../../../../tafjce/ServerEngine/FightDataServer/FightDataServer.mk
include ../../../../tafjce/ServerEngine/CommInclude.mk
include ../../../../tafjce/ServerEngine/CommLib.mk
include ../../../../tafjce/ServerEngine/NameServer/NameServer.mk
include ../../../../tafjce/ServerEngine/RoleServer/RoleServer.mk 
include ../../../../tafjce/ServerEngine/LegionServer/LegionServer.mk 
include ../../../../tafjce/ServerEngine/ConvertCodeServer/ConvertCodeServer.mk

include ../../../../tools/taf/makefile.taf
