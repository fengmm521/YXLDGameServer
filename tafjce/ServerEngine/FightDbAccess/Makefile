APP       := ServerEngine
TARGET    := FightDbAccessServer
#CFLAGS := -fPIC
MFLAGS    := 64
CONFIG    := 
STRIP_FLAG:= N

INCLUDE += -I/usr/local/include/hiredis/ -I/home/tafjce/ServerEngine/DALComm/ -I../../Common/CommFunc/ -I/usr/include/mysql/
LIB_DIR	+= -lutil -L/usr/lib/x86_64-linux-gnu/ -lmysqlclient  -lrt

LOCAL_SRC	+= $(wildcard *.cpp)

include /home/tafjce/ServerEngine/DbAccess/DbAccess.mk
include /home/tafjce/ServerEngine/CommLib.mk
include /home/mage/Documents/tool/taf/makefile.taf