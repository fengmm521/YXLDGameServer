#include "FightDataServantImp.h"
#include "CommLogic.h"
#include "svr_log.h"
#include "util/tc_base64.h"
#include "SyncSettingThread.h"

extern SyncSettingThread g_syncSettingThread;



void FightDataDbAccessCb::callback_setString(taf::Int32 ret)
{
	SvrRunLog("setString ret|%s|%d", m_strKey.c_str(), ret);
}

void FightDataDbAccessCb::callback_setString_exception(taf::Int32 ret)
{
	SvrErrLog("setString exception|%s|%d", m_strKey.c_str(), ret);
}

void FightDataDbAccessCb::callback_delString(taf::Int32 ret)
{
	SvrRunLog("delString ret|%s|%d", m_strKey.c_str(), ret);
}

void FightDataDbAccessCb::callback_delString_exception(taf::Int32 ret)
{
	SvrErrLog("delString exception|%s|%d", m_strKey.c_str(), ret);
}

void FightDataDbAccessCb::callback_getString(taf::Int32 ret,  const std::string& strValue)
{
	SvrRunLog("getString ret|%s|%d", m_strKey.c_str(), ret);
	
	if(ret == ServerEngine::eDbSucc)
	{
		m_pRedisClient->setString(m_strKey, strValue);
	
		string strOrgData = TC_Base64::decode(strValue);
		ServerEngine::FightData::async_response_getFightData(m_current, ServerEngine::en_FightDataRet_OK, strOrgData);
	}
	else if(ret == ServerEngine::eDbRecordNotExist)
	{
		ServerEngine::FightData::async_response_getFightData(m_current, ServerEngine::en_FightDataRet_NotExist, "");
	}
	else
	{
		ServerEngine::FightData::async_response_getFightData(m_current, ServerEngine::en_FightDataRet_SysError, "");
	}
}

void FightDataDbAccessCb::callback_getString_exception(taf::Int32 ret)
{
	SvrErrLog("getString exception|%s|%d", m_strKey.c_str(), ret);
}



FightDataImp::FightDataImp()
{
}

FightDataImp::~FightDataImp()
{
}

void FightDataImp::initialize()
{
	string strConfigFile = ServerConfig::ServerName + ".conf";
	TC_Config tmpCfg;
	tmpCfg.parseFile(strConfigFile);

	string strRedisIp = tmpCfg.get("/FightDataServer<redisHost>", "127.0.0.1");
	int iRedisPort = TC_Common::strto<int>(tmpCfg.get("/FightDataServer<redisPort>", "6379"));

	if(!m_redisClient.initlize(strRedisIp, iRedisPort, 1500) )
	{
		assert(false);
		return;
	}

	string strFightDbAccessObj = tmpCfg.get("/FightDataServer<FightDbAccessObject>", "");
	assert(strFightDbAccessObj.size() > 0);

	m_fightDbAccessPrx = Application::getCommunicator()->stringToProxy<ServerEngine::DbAccessPrx>(strFightDbAccessObj);

	g_syncSettingThread.setDbAccess(m_fightDbAccessPrx);
	g_syncSettingThread.start();
}

taf::Int32 FightDataImp::saveFightData(const ServerEngine::PKFight & keyData,const std::string & strBattleData,taf::JceCurrentPtr current)
{
	string strOrgKey = ServerEngine::JceToStr(keyData);
	string strBase64Key = TC_Base64::encode(strOrgKey);

	string strBase64Data = TC_Base64::encode(strBattleData);

	if(m_redisClient.setString(strBase64Key, strBase64Data) == en_RedisRet_OK)
	{
		try
		{
			//ServerEngine::DbAccessPrxCallbackPtr cb = new FightDataDbAccessCb(current, &m_redisClient,strBase64Key);
			//m_fightDbAccessPrx->async_setString(cb, strBase64Key, strBase64Data);
			g_syncSettingThread.setSettingData(strBase64Key, strBase64Data);
		}
		catch(std::exception& e)
		{
			SvrErrLog("FightDataImp::saveFightData exception|%s|%s", strBase64Key.c_str(), e.what() );
		}
		catch(...)
		{
			SvrErrLog("FightDataImp::saveFightData unknow exception|%s", strBase64Key.c_str() );
		}
		
		return ServerEngine::en_FightDataRet_OK;
	}

	return ServerEngine::en_FightDataRet_SysError;

	return 0;
}

taf::Int32 FightDataImp::getFightData(const ServerEngine::PKFight & keyData,std::string &strBattleData,taf::JceCurrentPtr current)
{
	string strOrgKey = ServerEngine::JceToStr(keyData);
	string strBase64Key = TC_Base64::encode(strOrgKey);

	string strTmpData;
	int iGetRet = m_redisClient.getString(strBase64Key, strTmpData);
	if(en_RedisRet_OK != iGetRet)
	{
		if(en_RedisRet_NoData == iGetRet)
		{
			try
			{
				// 如果已经有删除操作了,直接返回无数据
				if(g_syncSettingThread.isDeleting(strBase64Key))
				{
					return ServerEngine::en_FightDataRet_NotExist;
				}
			
				SvrRunLog("FightDataImp::getFightData From DbAccess|%s", strBase64Key.c_str() );
				current->setResponse(false);
				ServerEngine::DbAccessPrxCallbackPtr cb = new FightDataDbAccessCb(current, &m_redisClient,strBase64Key);
				m_fightDbAccessPrx->async_getString(cb, strBase64Key);
			}
			catch(std::exception& e)
			{
				SvrErrLog("FightDataImp::getFightData DbAccess  exception|%s|%s", strBase64Key.c_str(), e.what() );
			}
			catch(...)
			{
				SvrErrLog("FightDataImp::getFightData DbAccess unknow exception|%s", strBase64Key.c_str() );
			}
		
			return ServerEngine::en_FightDataRet_OK;
		}

		return ServerEngine::en_FightDataRet_SysError;
	}

	strBattleData = TC_Base64::decode(strTmpData);
	
	return ServerEngine::en_FightDataRet_OK;
}


taf::Int32 FightDataImp::delFightData(const ServerEngine::PKFight & keyData,taf::JceCurrentPtr current)
{
	string strOrgKey = ServerEngine::JceToStr(keyData);
	string strBase64Key = TC_Base64::encode(strOrgKey);

	int iRet = m_redisClient.delString(strBase64Key);
	if( (iRet == en_RedisRet_OK) || (iRet == en_RedisRet_NoData) )
	{
		try
		{
			//ServerEngine::DbAccessPrxCallbackPtr cb = new FightDataDbAccessCb(current, &m_redisClient,strBase64Key);
			g_syncSettingThread.eraseData(strBase64Key);
			//m_fightDbAccessPrx->async_delString(cb, strBase64Key);
			//m_fightDbAccessPrx->delString(strBase64Key);
		}
		catch(std::exception& e)
		{
			SvrErrLog("FightDataImp::delFightData DbAccess exception|%s|%s", strBase64Key.c_str(), e.what() );
		}
		catch(...)
		{
			SvrErrLog("FightDataImp::delFightData DbAccess unknow exception|%s", strBase64Key.c_str() );
		}
	
		return ServerEngine::en_FightDataRet_OK;
	}

	return ServerEngine::en_FightDataRet_SysError;
}





