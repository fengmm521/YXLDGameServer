#include "RouterComm.h"
#include "GameInterface.h"
#include "ServerManager.h"
#include "util/tc_config.h"
#include "util/tc_common.h"
#include "RouterServer.pb.h"

ServerManager::ServerManager():m_bTerminate(false)
{
	init();
}

ServerManager::~ServerManager()
{
	
}

bool ServerManager::init()
{
	reloadServerCfg();
	
	return true;
}

void ServerManager::terminate()
{
	TC_ThreadLock::Lock lock(*this);
	m_bTerminate = true;
	notify();
}

void ServerManager::run()
{
	while(!m_bTerminate)
	{
		TC_ThreadLock::Lock lock(*this);

		reloadServerCfg();

		checkAlive();

		timedWait(10000);

		if(m_bTerminate) return;
	}
}

class QueryGameStatusCb:public ServerEngine::GamePrxCallback
{
public:

	QueryGameStatusCb(ServerManager& svrManager, int iServerWorldID):m_serverManager(svrManager), m_iWorldID(iServerWorldID){}
	virtual void callback_getGameStatus(const ServerEngine::GameQueryStatus& gameStatus)
    { 
    	LOG->info()<<"setServerStatus Alive|"<<m_iWorldID<<"|"<<gameStatus.iMemberCount<<endl;
		m_serverManager.setServerStatus(m_iWorldID, gameStatus);
	}

	virtual void callback_getGameStatus_exception(taf::Int32 ret)
	{
		LOG->info()<<"setServerStatus Down|"<<m_iWorldID<<"|"<<ret<<endl;
		m_serverManager.setServerDown(m_iWorldID);
	}

private:

	ServerManager& m_serverManager;
	int m_iWorldID;
};

void ServerManager::setServerDown(int iWorldID)
{
	TC_ThreadLock::Lock lock(*this);

	ServerStatusMap::iterator it = m_serverStatusMap.find(iWorldID);
	if(it != m_serverStatusMap.end() )
	{
		ServerStatus& refStatus = it->second;
		refStatus.bAlive = false;
		refStatus.iMemberCount = 0;
	}
	FDLOG("connection")<<"ServerDown|"<<iWorldID<<endl;
}

void ServerManager::setServerStatus(int iWorldID, const ServerEngine::GameQueryStatus& gameStatus)
{
	TC_ThreadLock::Lock lock(*this);

	ServerStatusMap::iterator it = m_serverStatusMap.find(iWorldID);
	if(it != m_serverStatusMap.end() )
	{
		ServerStatus& refStatus = it->second;
		refStatus.bAlive = true;
		refStatus.iMemberCount = gameStatus.iMemberCount;
	}
}


int ServerManager::calcBusyStatus(int iOnlineCount)
{
	int iRet = -1;
	for(size_t i = 0; i < m_busyCfgList.size(); i++)
	{
		if(iOnlineCount < m_busyCfgList[i])
		{
			iRet = i;
			break;
		}
	}

	// 如果找到合适的区间了，返回
	if(iRet >= 0)
	{
		return iRet;
	}

	// 还可能是达到上限了
	return (int)m_busyCfgList.size();
}

void ServerManager::checkAlive()
{
	for(ServerStatusMap::iterator it = m_serverStatusMap.begin(); it != m_serverStatusMap.end(); it++)
	{
		ServerEngine::GamePrx tmpGamePrx = it->second.gamePrx;
		try
	    {
	    	ServerEngine::GamePrxCallbackPtr cb = new QueryGameStatusCb(*this, it->first);
			tmpGamePrx->async_getGameStatus(cb);
	    }
	    catch(TafException & ex)
	    {
	    	// 这里直接设置了
	    	it->second.bAlive = false;
			it->second.iMemberCount = 0;
	    	string result = it->second.strObject + "ping fail:" + ex.what();
	    	LOG->error()<< result << endl;
	    }
	}
}

void ServerManager::getResVersion(string& strUrlDir, string& strVersion, string& strMinVersion)
{
	strUrlDir = m_strUrlDir;
	strVersion = m_strVersion;
	strMinVersion = m_strMinVersion;
}

string ServerManager::getSnapshotServerObj()
{
	return m_strSnapshotServerObj;
}



void ServerManager::reloadServerCfg()
{
	string strConfigFile = ServerConfig::ServerName + ".conf";
	TC_Config tmpCfg;
	tmpCfg.parseFile(strConfigFile);

	vector<string> gameNameList;
	if(!tmpCfg.getDomainVector("/RouterServer/GameServerlist", gameNameList) )
	{
		throw runtime_error("invalid config /RouterServer/GameServerlist");
		return;
	}

	ServerStatusMap newServerStatusMap;
	ObjStatusMap newObjStatusMap;

	m_strUrlDir = tmpCfg.get("/RouterServer/Property<UrlDir>", "");
	m_strVersion = tmpCfg.get("/RouterServer/Property<Version>", "");
	m_strSnapshotServerObj = tmpCfg.get("/RouterServer/Property<SnapshotServer>", "");
	m_strMinVersion = tmpCfg.get("/RouterServer/Property<MinVersion>", "");

	// 加载版本更新配置
	this->m_noResUpdateVersionMap.clear();
	string noResUpdateVersionString = tmpCfg.get("/RouterServer/Property<noResUpdateVersion>", "");
	if(!noResUpdateVersionString.empty())
	{
		vector<string> noResUpdateVersionVector = TC_Common::sepstr<string>(noResUpdateVersionString, "#");
		vector<string>::iterator it = noResUpdateVersionVector.begin();
		while(it != noResUpdateVersionVector.end())
		{
			const string& codeVersion = * it;
			this->m_noResUpdateVersionMap.insert(codeVersion);
			it ++;
		}
	}
	
	for(size_t i = 0; i < gameNameList.size(); i++)
	{
		string strTmpServerName = gameNameList[i];
		string strSection = string("/RouterServer/GameServerlist/") + strTmpServerName;
		
		ServerStatus tmpStatus;
		tmpStatus.iWorldID = TC_Common::strto<int>(tmpCfg.get(strSection + "<WorldID>", "-1") );
		if(tmpStatus.iWorldID < 0) continue;

		tmpStatus.strName = tmpCfg.get(strSection + "<Name>", "");
		tmpStatus.strObject = tmpCfg.get(strSection + "<GameObj>", "");
		tmpStatus.iSordIndex = TC_Common::strto<int>(tmpCfg.get(strSection + "<index>", "0"));
		tmpStatus.iAttr = TC_Common::strto<int>(tmpCfg.get(strSection + "<Attr>", "0"));
		tmpStatus.gamePrx = Application::getCommunicator()->stringToProxy<ServerEngine::GamePrx>(tmpStatus.strObject);

		// 从历史状态获取状态
		if(m_objStatusMap.find(tmpStatus.strObject) != m_objStatusMap.end() )
		{
			ServerStatusMap::iterator it = m_objStatusMap[tmpStatus.strObject];
			tmpStatus.bAlive = it->second.bAlive;
		}
		
		assert(newServerStatusMap.find(tmpStatus.iWorldID) == newServerStatusMap.end() );

		std::pair<ServerStatusMap::iterator, bool> ret = newServerStatusMap.insert(std::make_pair(tmpStatus.iWorldID, tmpStatus) );
		assert(ret.second);
	}

	m_serverStatusMap = newServerStatusMap;
	m_objStatusMap.clear();
	for(ServerStatusMap::iterator it = m_serverStatusMap.begin(); it != m_serverStatusMap.end(); it++)
	{
		m_objStatusMap[it->second.strObject] = it;
	}

	// 加载忙配置
	string strBusyCfg = tmpCfg.get("/RouterServer/Property<busycfg>", "");
	m_busyCfgList =  TC_Common::sepstr<int>(strBusyCfg, "#");
	assert(m_busyCfgList.size() <= (size_t)ServerEngine::en_BusyStatus_End);
}

bool ServerManager::getGameServer(int iIndex, ServerStatus& status)
{
	TC_ThreadLock::Lock lock(*this);
	ServerStatusMap::iterator it = m_serverStatusMap.find(iIndex);
	if(it != m_serverStatusMap.end() )
	{
		status = it->second;
		return true;
	}
	FDLOG("connection")<<"game server not live"<<m_serverStatusMap.size()<<endl;
	return false;
}

extern void sendScMessage(taf::JceCurrentPtr current, const ServerEngine::SCMessage& scMsg);
void ServerManager::sendWorldList(taf::JceCurrentPtr current, const ServerEngine::RoleSnapshot& roleSnapshot)
{
	TC_ThreadLock::Lock lock(*this);

	ServerEngine::SCMessage scMesssage;
	scMesssage.set_icmd(ServerEngine::RS_ROUTER_WORLDLIST);

	const vector<int>& refWorldList = roleSnapshot.worldList;

	ServerEngine::SC_RS_WorldList scWorldList;
	for(ServerStatusMap::iterator it = m_serverStatusMap.begin(); it != m_serverStatusMap.end(); it++)
	{
		const ServerStatus& tmpServerStatus = it->second;
	
		ServerEngine::RSWorld* pWorldItem = scWorldList.add_worldlist();
		assert(pWorldItem);

		pWorldItem->set_iworldid(it->first);
		pWorldItem->set_strworldname(tmpServerStatus.strName);
		pWorldItem->set_iatt(tmpServerStatus.iAttr);  // 暂无
		pWorldItem->set_ionlinestatus(tmpServerStatus.bAlive);

		if(find(refWorldList.begin(), refWorldList.end(), it->first) != refWorldList.end() )
		{
			pWorldItem->set_bhaverole(true);
		}
		else
		{
			pWorldItem->set_bhaverole(false);
		}

		int iBusyStatus = calcBusyStatus(tmpServerStatus.iMemberCount);
		pWorldItem->set_ibusystatus(iBusyStatus);
	}
	
	scWorldList.SerializeToString(scMesssage.mutable_strmsgbody() );

	sendScMessage(current, scMesssage);
}


bool ServerManager::codeVersionNoResUpdate(const string& codeVersion)
{
	CodeVersionMap::iterator it = this->m_noResUpdateVersionMap.find(codeVersion);
	if(this->m_noResUpdateVersionMap.end() == it)
	{
		return false;
	}
	else
	{
		return true;
	}
}




