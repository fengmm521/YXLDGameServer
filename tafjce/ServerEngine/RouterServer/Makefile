APP       := ServerEngine
TARGET    := RouterServer
#CFLAGS := -fPIC
MFLAGS    := 64
CONFIG    := 
STRIP_FLAG:= N


INCLUDE += -I../SnapshotServer

LOCAL_SRC	+= $(wildcard *.cpp *.cc)

               
LIB	+= -l:libprotobuf.a

protofile:
	cd Proto && protoc --cpp_out=../ RouterServer.proto
	@$(MAKE) all


include /home/tafjce/ServerEngine/SnapshotServer/SnapshotServer.mk
include /home/mage/Documents/tool/taf/makefile.taf
include /home/tafjce/ServerEngine/DALComm/DALComm.mk