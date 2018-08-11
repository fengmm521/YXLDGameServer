#include "SnapshotServer.h"
#include "SnapshotImp.h"
#include "comm_func.h"
#include "svr_log.h"
#include "CommLogic.h"

#include "util/tc_base64.h"


SnapshotImp::SnapshotImp()
{
}

SnapshotImp::~SnapshotImp()
{
}


void SnapshotImp::initialize()
{
	string strConfigFile = ServerConfig::ServerName + ".conf";
	TC_Config tmpCfg;
	tmpCfg.parseFile(strConfigFile);

	string strRedisIp = tmpCfg.get("/SnapshotServer<redisHost>", "127.0.0.1");
	int iRedisPort = TC_Common::strto<int>(tmpCfg.get("/SnapshotServer<redisPort>", "6379"));

	if(!m_redisClient.initlize(strRedisIp, iRedisPort, 1500) )
	{
		assert(false);
		return;
	}
}

void SnapshotImp::destroy()
{
}

taf::Int32 SnapshotImp::addWorld(const std::string & strAccount,taf::Int32 iWorldID,taf::JceCurrentPtr current)
{
	string strBase64Account = TC_Base64::encode(strAccount);

	string strBase64Data;
	int iRet = m_redisClient.getString(strBase64Account, strBase64Data);

	if(iRet == en_RedisRet_OK)
	{
		string strOrgData = TC_Base64::decode(strBase64Data);
		ServerEngine::RoleSnapshot snapshotData;
		ServerEngine::JceToObj(strOrgData, snapshotData);

		if(find(snapshotData.worldList.begin(), snapshotData.worldList.end(), iWorldID) != snapshotData.worldList.end() )
		{
			return ServerEngine::en_SnapshotRet_OK;
		}

		snapshotData.worldList.push_back(iWorldID);

		string strNewData = ServerEngine::JceToStr(snapshotData);
		string strBase64SaveData = TC_Base64::encode(strNewData);

		if(m_redisClient.setString(strBase64Account, strBase64SaveData) != en_RedisRet_OK)
		{
			SvrErrLog("write Redis Fail");
			return ServerEngine::en_SnapshotRet_Fail;
		}

		return ServerEngine::en_SnapshotRet_OK;
	}
	else if(iRet == en_RedisRet_NoData)
	{
		ServerEngine::RoleSnapshot snapshotData;
		snapshotData.worldList.push_back(iWorldID);

		string strNewData = ServerEngine::JceToStr(snapshotData);
		string strBase64SaveData = TC_Base64::encode(strNewData);

		if(m_redisClient.setString(strBase64Account, strBase64SaveData) != en_RedisRet_OK)
		{
			SvrErrLog("write Redis Fail");
			return ServerEngine::en_SnapshotRet_Fail;
		}

		return ServerEngine::en_SnapshotRet_OK;
	}
	else
	{
		return ServerEngine::en_SnapshotRet_Fail;
	}

	return ServerEngine::en_SnapshotRet_Fail;
}

taf::Int32 SnapshotImp::getRoleWorldDesc(const std::string & strAccount,ServerEngine::RoleSnapshot &snapDesc,taf::JceCurrentPtr current)
{
	string strBase64Account = TC_Base64::encode(strAccount);

	string strBase64Data;
	int iRet = m_redisClient.getString(strBase64Account, strBase64Data);

	if(iRet == en_RedisRet_OK)
	{
		string strOrgData = TC_Base64::decode(strBase64Data);
		ServerEngine::JceToObj(strOrgData, snapDesc);

		return ServerEngine::en_SnapshotRet_OK;
	}
	else if(iRet == en_RedisRet_NoData)
	{
		return ServerEngine::en_SnapshotRet_NoData;
	}
	
	return ServerEngine::en_SnapshotRet_Fail;
}



