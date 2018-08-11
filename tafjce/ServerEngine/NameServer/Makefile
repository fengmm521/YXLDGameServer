APP       := ServerEngine
TARGET    := NameServer
#CFLAGS := -fPIC
MFLAGS    := 64
CONFIG    := 
STRIP_FLAG:= N

INCLUDE += -I/usr/include/hiredis/ -I/home/mage/tafjce/ServerEngine/include/MiniApr/Include -I/home/tafjce/ServerEngine/include -I/home/mage/Documents/kaifa/code/Server/Common/lua5.1 -I/home/mage/Documents/kaifa/code/Server/Common/ToLua++ -I/home/mage/Documents/kaifa/code/Server/Common/CommFunc
LIB_DIR	+= -l:libhiredis.a -L/home/mage/Documents/kaifa/code/Server/lib -lpthread


LOCAL_SRC	+= $(wildcard *.cpp)

include /home/tafjce/ServerEngine/CommInclude.mk
include /home/tafjce/ServerEngine/CommLib.mk
include /home/tafjce/ServerEngine/DbAccess/DbAccess.mk
include /home/mage/Documents/tool/taf/makefile.taf

