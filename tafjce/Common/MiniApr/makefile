include ../../Make/makefile_lib


INCLUDE	+= -I./Include -I./Src -I../lua5.1 -I../ToLua++
LOCAL_SRC	+= $(wildcard Src/*.cpp Src/Component/*.cpp Src/XmlParse/*.cpp \
               Src/Stream/*.cpp  Src/XmlParse/TinyXml/*.cpp Src/TimerManager/*.cpp \
               Src/ProfileManager/*.cpp Src/lua5.1/*.c  Src/tolua++/*.c Src/ScriptEngine/*.cpp Src/RedisClient/*.cpp\
			   Src/Random/*.cpp)

OBJ_FILE+=$(patsubst %.cpp,%.o, $(patsubst %.c,%.o, $(LOCAL_SRC)))

CFLAGS+= -D_STATIC_MINIAPR_


all:$(OBJ_FILE)
	$(AR) q ../$(LIB_FILE) $(OBJ_FILE)
               
LIB	+= -ldl

include /home/mage/Documents/tool/taf/makefile.taf



