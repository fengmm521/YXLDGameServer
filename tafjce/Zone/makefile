Dirs = Src/GameEngine Src/CoreImp  Src/FightServer/  Src/HeroSystem Src/FormationSystem  Src/EffectServer/ Src/SkillServer/  Src/BuffServer Src/ItemServer\
 Src/DungeonServer Src/LuaWrapper Src/FightSoulSystem Src/GodAnimalSystem Src/Legion Src/ClimbTowerSystem Src/ArenaServer Src/CampBattleSystem Src/Shop\
Src/GuideSystem Src/FriendSystem  Src/TaskSystem Src/DreamLandSystem Src/MailSystem Src/ManorSystem Src/OperateSystem Src/Server
       
PropSetDir = Include/Property
##CurDir=$(PWD)
CurDir=/home/mage/Documents/kaifa/code/Server/Zone
PropSetDefHeaders = $(basename $(notdir $(wildcard $(PropSetDir)/*.h)))

LUA_DIR= /home/mage/Documents/kaifa/code/Server/Zone/Tools

all:
	echo $(LUA_DIR)
	cd $(CurDir)/Tools && $(LUA_DIR)/lua Component.lua ../GameServer.xml  ../Src/Server/StaticCreateComponent.cpp ../Include/ComponentConstDef.h
	@for PropItem in $(PropSetDefHeaders); do cd $(CurDir)/Tools && $(LUA_DIR)/lua PropertyMake.lua ../$(PropSetDir)/$$PropItem.h ../data/propcsv/ $$PropItem.csv;done;
	@for PropItem in $(PropSetDefHeaders); do cd $(CurDir)/Tools && $(LUA_DIR)/lua PropertyExport.lua ../$(PropSetDir)/$$PropItem.h ../Pkg $$PropItem.pkg;done;
	chmod +x ./tolua++
	cd Pkg && ../tolua++ -n JZServerExport -o ../Src/Server/JZServerExport.cpp JZServer.pkg
	cd Jce && make
	#优化协议生成
	cd Proto && $(LUA_DIR)/lua $(CurDir)/Tools/MakeProtobuf.lua GameServer.proto
	cd Proto && $(LUA_DIR)/lua $(CurDir)/Tools/MakeProtobuf.lua GameServerExt.proto
	#cd Proto && protoc --cpp_out=./ GameServer.proto
	cd ../lib && rm -rf *.a
	cp ../Common/libcommD.a ../lib/
	@for dir in $(Dirs); do make -C $$dir all -j4; echo; done
	cp Src/Server/GameServer ./GameServer
	cp -f Src/Server/GameServer /home/mage/Desktop/ServerEngine/Game0/GameServer0/GameServer0
TopDir = ../
	
rebuild:comm-prebuild
	@$(MAKE) -f makefileGameServer

include ../common-prebuild.inc
	
clean:comm-prebuild
	cd Jce && make clean
	@for dir in $(Dirs); do cd $(CURDIR)/$$dir && rm -rf Pch.depend && make clean; done
	
cleanall:comm-prebuild
	cd Jce && make clean
	@for dir in $(Dirs); do cd $(CURDIR)/$$dir && rm -rf Pch.depend && make cleanall; done
	
tar:
	cd Src/Server && make tar
	cp Src/Server/*.tgz ./
