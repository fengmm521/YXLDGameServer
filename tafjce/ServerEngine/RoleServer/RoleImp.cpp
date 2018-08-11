#include "RoleImp.h"
#include "CommLogic.h"
#include "svr_log.h"
#include "util/tc_base64.h"
#include "SyncSettingThread.h"


extern SyncSettingThread g_syncSettingThread;

RoleImp::RoleImp()
{
}

RoleImp::~RoleImp()
{
}

void RoleImp::initialize()
{
	string strConfigFile = ServerConfig::ServerName + ".conf";
	TC_Config tmpCfg;
	tmpCfg.parseFile(strConfigFile);

	string strRedisIp = tmpCfg.get("/RoleServer<redisHost>", "127.0.0.1");
	int iRedisPort = TC_Common::strto<int>(tmpCfg.get("/RoleServer<redisPort>", "6379"));

	if(!m_redisClient.initlize(strRedisIp, iRedisPort, 1500) )
	{
		assert(false);
		return;
	}

	string strNameObj = tmpCfg.get("/RoleServer<NameObject>", "");
	assert(strNameObj.size() > 0);
	m_namePrx = Application::getCommunicator()->stringToProxy<ServerEngine::NamePrx>(strNameObj);


	string strRoleDbAccessObj = tmpCfg.get("/RoleServer<RoleDbAccessObject>", "");
	assert(strRoleDbAccessObj.size() > 0);

	m_roleDbAccessPrx = Application::getCommunicator()->stringToProxy<ServerEngine::DbAccessPrx>(strRoleDbAccessObj);

	g_syncSettingThread.setDbAccess(m_roleDbAccessPrx);
	g_syncSettingThread.start();
}

void RoleImp::destroy()
{
}

void RoleDbAccessCb::callback_setString(taf::Int32 ret)
{
	SvrRunLog("setString ret|%s|%d", m_strRoleKey.c_str(), ret);
}

void RoleDbAccessCb::callback_setString_exception(taf::Int32 ret)
{
	SvrErrLog("setString exception|%s|%d", m_strRoleKey.c_str(), ret);
}

void RoleDbAccessCb::callback_delString(taf::Int32 ret)
{
	SvrRunLog("delString ret|%s|%d", m_strRoleKey.c_str(), ret);
}

void RoleDbAccessCb::callback_delString_exception(taf::Int32 ret)
{
	SvrErrLog("delString exception|%s|%d", m_strRoleKey.c_str(), ret);
}

void RoleDbAccessCb::callback_getString(taf::Int32 ret,  const std::string& strValue)
{
	SvrRunLog("getString ret|%s|%d", m_strRoleKey.c_str(), ret);
	ServerEngine::PIRole roleInfo;
	if(ret == ServerEngine::eDbSucc)
	{
		m_pRedisClient->setString(m_strRoleKey, strValue);
	
		string strOrgData = TC_Base64::decode(strValue);
		ServerEngine::JceToObj(strOrgData, roleInfo);
		ServerEngine::Role::async_response_getRole(m_current, ServerEngine::en_RoleRet_OK, roleInfo);
	}
	else if(ret == ServerEngine::eDbRecordNotExist)
	{
		ServerEngine::Role::async_response_getRole(m_current, ServerEngine::en_RoleRet_NotExist, roleInfo);
	}
	else
	{
		ServerEngine::Role::async_response_getRole(m_current, ServerEngine::en_RoleRet_SysError, roleInfo);
	}
}

void RoleDbAccessCb::callback_getString_exception(taf::Int32 ret)
{
	SvrErrLog("getString exception|%s|%d", m_strRoleKey.c_str(), ret);
}


taf::Int32 RoleImp::newRole(const std::string & strAccount,taf::Int32 nPos,taf::Int32 nWorldID,const std::string & strName,const ServerEngine::PIRole & roleInfo,taf::JceCurrentPtr current)
{
	// 验证这个位置是否存在角色(实际上，这里的验证是不严谨的,目前这里是依赖游戏服来保证这点的)
	ServerEngine::PKRole roleKey;
	roleKey.strAccount = strAccount;
	roleKey.rolePos = nPos;
	roleKey.worldID = nWorldID;

	string strOrgKey = ServerEngine::JceToStr(roleKey);
	string strBase64Key = TC_Base64::encode(strOrgKey);

	string strTmpData;
	if(m_redisClient.getString(strBase64Key, strTmpData) == en_RedisRet_OK)
	{
		return ServerEngine::en_RoleRet_PosConflict;
	}

	// 验证名字是否存在
	int iAddNameRet = m_namePrx->doAddWorldName(strName, nWorldID, strAccount, nPos);
	if(ServerEngine::en_NameRet_OK != iAddNameRet)
	{
		if(iAddNameRet == ServerEngine::en_NameRet_Duplicate)
		{
			return ServerEngine::en_RoleRet_NameDunplicated;
		}
		
		return ServerEngine::en_RoleRet_SysError;
	}
	
	// 创建
	string strOrgData = ServerEngine::JceToStr(roleInfo);
	string strBase64Data = TC_Base64::encode(strOrgData);

	if(m_redisClient.setString(strBase64Key, strBase64Data) == en_RedisRet_OK)
	{
		try
		{
			//ServerEngine::DbAccessPrxCallbackPtr cb = new RoleDbAccessCb(current, &m_redisClient,strBase64Key);
			//m_roleDbAccessPrx->async_setString(cb, strBase64Key, strBase64Data);
			g_syncSettingThread.setSettingData(strBase64Key, strBase64Data);
		}
		catch(std::exception& e)
		{
			SvrErrLog("new Role DbAccess exception|%s|%s",strBase64Key.c_str(), e.what() );
		}
		catch(...)
		{
			SvrErrLog("new Role DbAccess unknow exception|%s", strBase64Key.c_str() );
		}
		
		return ServerEngine::en_RoleRet_OK;
	}

	m_namePrx->delWorldName(strBase64Key);

	return ServerEngine::en_RoleRet_SysError;
}

taf::Int32 RoleImp::updateRole(const ServerEngine::PKRole & keyData,const ServerEngine::PIRole & roleInfo,taf::JceCurrentPtr current)
{
	string strOrgKey = ServerEngine::JceToStr(keyData);
	string strBase64Key = TC_Base64::encode(strOrgKey);

	string strOrgData = ServerEngine::JceToStr(roleInfo);
	string strBase64Data = TC_Base64::encode(strOrgData);

	if(m_redisClient.setString(strBase64Key, strBase64Data) == en_RedisRet_OK)
	{
		try
		{
			//ServerEngine::DbAccessPrxCallbackPtr cb = new RoleDbAccessCb(current, &m_redisClient,strBase64Key);
			//m_roleDbAccessPrx->async_setString(cb, strBase64Key, strBase64Data);
			g_syncSettingThread.setSettingData(strBase64Key, strBase64Data);
		}
		catch(std::exception& e)
		{
			SvrErrLog("updateRole DbAccess exception|%s|%s", strBase64Key.c_str(), e.what() );
		}
		catch(...)
		{
			SvrErrLog("updateRole DbAccess unknow exception|%s", strBase64Key.c_str() );
		}
		
		return ServerEngine::en_RoleRet_OK;
	}

	return ServerEngine::en_RoleRet_SysError;
}

taf::Int32 RoleImp::delRole(const ServerEngine::PKRole & keyData,taf::JceCurrentPtr current)
{
	string strOrgKey = ServerEngine::JceToStr(keyData);
	string strBase64Key = TC_Base64::encode(strOrgKey);

	int iRet = m_redisClient.delString(strBase64Key);
	if( (iRet == en_RedisRet_OK) || (iRet == en_RedisRet_NoData) )
	{
		try
		{
			//ServerEngine::DbAccessPrxCallbackPtr cb = new RoleDbAccessCb(current, &m_redisClient,strBase64Key);
			//m_roleDbAccessPrx->async_delString(cb, strBase64Key);
			// 走同步接口吧
			g_syncSettingThread.eraseData(strBase64Key);
			//m_roleDbAccessPrx->delString(strBase64Key);
		}
		catch(std::exception& e)
		{
			SvrErrLog("delRole DbAccess exception|%s|%s", strBase64Key.c_str(), e.what() );
		}
		catch(...)
		{
			SvrErrLog("delRole DbAccess unknow exception|%s", strBase64Key.c_str() );
		}
	
		return ServerEngine::en_RoleRet_OK;
	}

	return ServerEngine::en_RoleRet_SysError;
}

taf::Int32 RoleImp::getRole(const ServerEngine::PKRole & keyData,ServerEngine::PIRole &roleInfo,taf::JceCurrentPtr current)
{
	string strOrgKey = ServerEngine::JceToStr(keyData);
	string strBase64Key = TC_Base64::encode(strOrgKey);

	string strTmpData;
	int iGetRet = m_redisClient.getString(strBase64Key, strTmpData);

	// modified by fwiwu, redis如果不存在数据，先要到setting队列里查找，可能还没写入mysql
	if( (en_RedisRet_OK != iGetRet) && !g_syncSettingThread.getSettingData(strBase64Key, strTmpData) )
	{
		if(en_RedisRet_NoData == iGetRet)
		{
			if(g_syncSettingThread.isDeleting(strBase64Key) )
			{
				return ServerEngine::en_RoleRet_NotExist;
			}
			
			try
			{
				SvrRunLog("getRole From DbAccess|%s", strBase64Key.c_str() );
				current->setResponse(false);
				ServerEngine::DbAccessPrxCallbackPtr cb = new RoleDbAccessCb(current, &m_redisClient,strBase64Key);
				m_roleDbAccessPrx->async_getString(cb, strBase64Key);
			}
			catch(std::exception& e)
			{
				SvrErrLog("getRole DbAccess  exception|%s|%s", strBase64Key.c_str(), e.what() );
				return ServerEngine::en_RoleRet_SysError;
			}
			catch(...)
			{
				SvrErrLog("getRole DbAccess unknow exception|%s", strBase64Key.c_str() );
				return ServerEngine::en_RoleRet_SysError;
			}
		
			return ServerEngine::en_RoleRet_OK;
		}

		return ServerEngine::en_RoleRet_SysError;
	}

	string strOrgData = TC_Base64::decode(strTmpData);
	ServerEngine::JceToObj(strOrgData, roleInfo);
	
	return ServerEngine::en_RoleRet_OK;
}


