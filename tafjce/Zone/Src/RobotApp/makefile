
APP       := ServerEngine
TARGET    := RobotServer
#CFLAGS := -fPIC
MFLAGS    := 64
CONFIG    := 
STRIP_FLAG:= N

include ../../CommInclude.mk

INCLUDE += -I../../Proto/  -I./ClientState

LOCAL_SRC	+= $(wildcard *.cpp *.cc ClientState/*.cpp *.c) 

REMOTE_OBJ	+=  ../../Proto/*.pb.cc ../../Jce/Fight.o 


LIB_DIR	+=  -ldl -lrt -l:libprotobuf.a
	
#LIB_DIR += -ltcmalloc

include /home/tafjce/ServerEngine/RouterServer/RouterServer.mk
include /home/tafjce/ServerEngine/CommInclude.mk
include /home/tafjce/ServerEngine/CommLib.mk
include /home/mage/Documents/tool/taf/makefile.taf

	
