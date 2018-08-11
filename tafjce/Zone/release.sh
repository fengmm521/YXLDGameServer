cp ./GameServer /home/feiwu/app/taf/data/ServerEngine/GameServer$1/GameServer$1 -rf
cp ./GameServer.xml  /home/feiwu/app/taf/data/ServerEngine/GameServer$1/
cp ./data -rf  /home/feiwu/app/taf/data/ServerEngine/GameServer$1/
echo "./GameServer$1 --config=./GameServer$1.config.conf &"> /home/feiwu/app/taf/data/ServerEngine/GameServer$1/start.sh
