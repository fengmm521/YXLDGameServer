#==============================================================
# makefile for command handle
#==============================================================
include ../../Make/makefile_lib
OBJ_FILE = tolua_event.o tolua_is.o tolua_map.o tolua_push.o tolua_to.o

%.o:%.c
	$(CC) $(C_FLAGS) -c $< -I../lua5.1 -I./ -I../msg_layer -I../comm_func -I../protocol -I../lua5.1

all:$(OBJ_FILE)
	$(AR) q ../$(LIB_FILE) $(OBJ_FILE)

clean:
	rm -rf *.o

release: all
	cp *.h ../../include 
