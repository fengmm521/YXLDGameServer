#==============================================================
# makefile for command handle
#==============================================================
include ../../Make/makefile_lib
OBJ_FILE = clear.o  compare.o  copy.o  gen_uuid.o   isnull.o  pack.o  parse.o   unpack.o  unparse.o  uuid_time.o

%.o:%.c
	$(CC) $(C_FLAGS) -c $< -I../lua5.1 -I./ -I../msg_layer -I../comm_func -I../protocol -I../

all:$(OBJ_FILE)
	$(AR) q ../$(LIB_FILE) $(OBJ_FILE)

clean:
	rm -rf *.o
