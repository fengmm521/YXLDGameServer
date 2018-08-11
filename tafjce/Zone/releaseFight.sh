cp ./FightServer /home/feiwu/app/taf/data/ServerEngine/FightServer$1/FightServer$1 -rf
#cp ./FightServer.xml  /home/feiwu/app/taf/data/ServerEngine/FightServer$1/
cp ./data -rf  /home/feiwu/app/taf/data/ServerEngine/FightServer$1/
echo "./FightServer$1 --config=./FightServer$1.config.conf &"> /home/feiwu/app/taf/data/ServerEngine/FightServer$1/start.sh
