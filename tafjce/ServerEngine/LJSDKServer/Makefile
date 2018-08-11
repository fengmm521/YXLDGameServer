APP       := ServerEngine
TARGET    := LJSDKServer
#CFLAGS := -fPIC
MFLAGS    := 64
CONFIG    := 
STRIP_FLAG:= N


INCLUDE +=-I../../Common/CommFunc -I/home/mage/tafjce/ServerEngine/include/MiniApr/Include/ -I/home/mage/tafjce/Common/lua5.1/ -I/home/mage/tafjce/Common/ToLua++/

LOCAL_SRC	+= $(wildcard *.cpp *.cc)

LIB_DIR += -lpthread

               
LIB	+= -l:libprotobuf.a -l:liboauth.a -lssl -lcrypto -lcurl
include /home/tafjce/ServerEngine/CommInclude.mk
include /home/tafjce/ServerEngine/CommLib.mk
include /home/tafjce/ServerEngine/DbAccess/DbAccess.mk
include /home/tafjce/ServerEngine/RouterServer/RouterServer.mk
include /home/mage/Documents/tool/taf/makefile.taf
include /home/tafjce/ServerEngine/DALComm/DALComm.mk

