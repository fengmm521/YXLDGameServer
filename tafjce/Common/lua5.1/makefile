#==============================================================
# makefile for command handle
#==============================================================
include ../../Make/makefile_lib
OBJ_FILE = lapi.o lcode.o ldebug.o ldo.o ldump.o lfunc.o lgc.o llex.o lmem.o \
	lobject.o lopcodes.o lparser.o lstate.o lstring.o ltable.o ltm.o  \
	lundump.o lvm.o lzio.o lauxlib.o lbaselib.o ldblib.o liolib.o lmathlib.o loslib.o ltablib.o \
	lstrlib.o loadlib.o linit.o

LUA_FLAGS=-DLUA_USE_MKSTEMP

%.o:%.c
	$(CC) $(C_FLAGS) $(LUA_FLAGS)  -c $< -I./ -I../msg_layer -I../comm_func -I../protocol 

all:$(OBJ_FILE)
	$(AR) q ../$(LIB_FILE) $(OBJ_FILE)

clean:
	rm -rf *.o

release: all
	cp *.h ../../include 
