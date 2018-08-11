
APP       := LZ
TARGET    := ../../../lib/libGameEngine.a
LFLAGS:= -fPIC
MFLAGS    := 64
CONFIG    := 
STRIP_FLAG:= N

PCH       := GameEnginePch.h.gch
PCH_HEADER:= GameEnginePch.h

include ../../CommInclude.mk

INCLUDE +=

LOCAL_SRC	+= $(wildcard *.cpp)

include /home/tafjce/ServerEngine/FightDataServer/FightDataServer.mk
include /home/tafjce/ServerEngine/NameServer/NameServer.mk
include /home/tafjce/ServerEngine/RoleServer/RoleServer.mk
include /home/tafjce/ServerEngine/RouterServer/RouterServer.mk
include /home/tafjce/ServerEngine/ConvertCodeServer/ConvertCodeServer.mk
include /home/tafjce/ServerEngine/PayReturnServer/PayReturnServer.mk
include /home/tafjce/ServerEngine/LJSDKServer/LJSDKServer.mk
include /home/mage/Documents/tool/taf/makefile.taf