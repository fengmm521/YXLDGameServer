#==============================================================
# makefile for template
#==============================================================
include ../../Make/makefile_lib

#OBJ_FILE = template.o
OBJ_FILE = 
%.o:%.cpp
	$(CXX) $(CXX_FLAGS) -c $<   -I../

all:$(OBJ_FILE)
	$(AR) q ../$(LIB_FILE) $(OBJ_FILE)

clean:
	rm -rf *.o
