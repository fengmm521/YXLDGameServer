APP       := ServerEngine
TARGET    := PayReturnServer
#CFLAGS := -fPIC
MFLAGS    := 64
CONFIG    := 
STRIP_FLAG:= N

INCLUDE += -I/usr/include/hiredis/ -I/home/mage/tafjce/ServerEngine/include/MiniApr/Include/ -I/home/mage/tafjce/Common/lua5.1/ -I/home/mage/tafjce/Common/ToLua++/ -I/home/mage/tafjce/Common/CommFunc/
LIB_DIR	+= -l:libhiredis.a -lpthread


LOCAL_SRC	+= $(wildcard *.cpp)

include /home/tafjce/ServerEngine/CommInclude.mk
include /home/tafjce/ServerEngine/CommLib.mk
include /home/tafjce/ServerEngine/DbAccess/DbAccess.mk
include /home/mage/Documents/tool/taf/makefile.taf

