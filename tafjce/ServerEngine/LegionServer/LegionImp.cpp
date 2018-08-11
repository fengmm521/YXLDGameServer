#include "LegionImp.h"
#include "CommLogic.h"
#include "svr_log.h"
#include "util/tc_base64.h"

#define LEGIONLISTKEY	"LegionListKey"
#define GLOBALVALUEKEY  "GlobalValueKey*"

LegionImp::LegionImp()
{
}

LegionImp::~LegionImp()
{
}

void LegionImp::initialize()
{	
	string strConfigFile = ServerConfig::ServerName + ".conf";
	TC_Config tmpCfg;
	tmpCfg.parseFile(strConfigFile);

	string strRedisIp = tmpCfg.get("/LegionServer<redisHost>", "127.0.0.1");
	int iRedisPort = TC_Common::strto<int>(tmpCfg.get("/LegionServer<redisPort>", "6379"));

	if(!m_redisClient.initlize(strRedisIp, iRedisPort, 1500) )
	{
		assert(false);
		return;
	}

	string strNameObj = tmpCfg.get("/LegionServer<NameObject>", "");
	assert(strNameObj.size() > 0);
	m_namePrx = Application::getCommunicator()->stringToProxy<ServerEngine::NamePrx>(strNameObj);
	
}

void LegionImp::destroy()
{
}

taf::Int32 LegionImp::getLegionList(ServerEngine::LegionList &legionList,taf::JceCurrentPtr current)
{
	string strBase64Key = TC_Base64::encode(LEGIONLISTKEY);

	string strValue;
	int iRedisRet = m_redisClient.getString(strBase64Key, strValue);

	if(en_RedisRet_NoData == iRedisRet)
	{
		return ServerEngine::en_DataRet_OK;
	}
	else if(en_RedisRet_SysError == iRedisRet)
	{
		return ServerEngine::en_DataRet_SysError;
	}
	else if(en_RedisRet_OK == iRedisRet)
	{
		string strOrgValue = TC_Base64::decode(strValue);
		ServerEngine::JceToObj(strOrgValue, legionList);

		return ServerEngine::en_DataRet_OK;
	}

	return ServerEngine::en_DataRet_SysError;
}

int LegionImp::delKeyFromLegionList(const string& strLegionName)
{
	string strListData;
	string strListNameKey = TC_Base64::encode(LEGIONLISTKEY);
	int iRedisRet = m_redisClient.getString(strListNameKey, strListData);
	if(en_RedisRet_SysError ==  iRedisRet)
	{
		return ServerEngine::en_DataRet_SysError;
	}
	
	ServerEngine::LegionList legionList;
	if(strListData.size() > 0)
	{
		string strOrgListData = TC_Base64::decode(strListData);
		ServerEngine::JceToObj(strOrgListData, legionList);
	}

	vector<string>::iterator it = find(legionList.legionList.begin(), legionList.legionList.end(), strLegionName);
	if(it != legionList.legionList.end() )
	{
		legionList.legionList.erase(it);
	}
	
	string strOrgNewData = ServerEngine::JceToStr(legionList);
	string strBase64NewData = TC_Base64::encode(strOrgNewData);

	if(m_redisClient.setString(strListNameKey, strBase64NewData) != en_RedisRet_OK)
	{
		return ServerEngine::en_DataRet_SysError;
	}

	return ServerEngine::en_DataRet_OK;
}

int LegionImp::addNewLegionKey(const string& strLegionName)
{
	string strListData;
	string strListNameKey = TC_Base64::encode(LEGIONLISTKEY);
	int iRedisRet = m_redisClient.getString(strListNameKey, strListData);
	if(en_RedisRet_SysError ==  iRedisRet)
	{
		return ServerEngine::en_DataRet_SysError;
	}
	
	ServerEngine::LegionList legionList;
	if(strListData.size() > 0)
	{
		string strOrgListData = TC_Base64::decode(strListData);
		ServerEngine::JceToObj(strOrgListData, legionList);
	}
	legionList.legionList.push_back(strLegionName);
	string strOrgNewData = ServerEngine::JceToStr(legionList);
	string strBase64NewData = TC_Base64::encode(strOrgNewData);

	if(m_redisClient.setString(strListNameKey, strBase64NewData) != en_RedisRet_OK)
	{
		return ServerEngine::en_DataRet_SysError;
	}

	return ServerEngine::en_DataRet_OK;
}


taf::Int32 LegionImp::newLegion(const std::string & strLegionName,const std::string & data,taf::JceCurrentPtr current)
{
	// 先验证名字
	int iAddNameRet = m_namePrx->doAddWorldName(strLegionName, 0, "", 0);
	if(ServerEngine::en_NameRet_OK != iAddNameRet)
	{
		if(iAddNameRet == ServerEngine::en_NameRet_Duplicate)
		{
			return ServerEngine::en_DataRet_Dunplicated;
		}
		
		return ServerEngine::en_DataRet_SysError;
	}

	// 存储Key
	int iAddKeyRet = addNewLegionKey(strLegionName);
	if(iAddKeyRet != ServerEngine::en_DataRet_OK)
	{
		return iAddKeyRet;
	}

	// 存储数据
	return updateLegion(strLegionName, data, current);
}


taf::Int32 LegionImp::getLegion(const std::string & strKey,std::string &strValue,taf::JceCurrentPtr current)
{
	string sLowerNameKey;
	ServerEngine::StringToLower(strKey, sLowerNameKey);
	string strBase64NameKey = TC_Base64::encode(sLowerNameKey);

	string strTmpValue;
	int iRedisRet = m_redisClient.getString(strBase64NameKey, strTmpValue);
	if(en_RedisRet_NoData == iRedisRet)
	{
		return ServerEngine::en_DataRet_NotExist;
	}
	else if(en_RedisRet_SysError == iRedisRet)
	{
		return ServerEngine::en_DataRet_OK; 
	}
	else if(en_RedisRet_OK == iRedisRet)
	{
		strValue = TC_Base64::decode(strTmpValue);
		return ServerEngine::en_DataRet_OK;
	}
	
	return ServerEngine::en_DataRet_SysError;
}

taf::Int32 LegionImp::updateLegion(const std::string & strKey,const std::string & strValue,taf::JceCurrentPtr current)
{
	string sLowerNameKey;
	ServerEngine::StringToLower(strKey, sLowerNameKey);
	string strBase64NameKey = TC_Base64::encode(sLowerNameKey);
	
	string strBase64Value = TC_Base64::encode(strValue);

	int iRedisRet = m_redisClient.setString(strBase64NameKey, strBase64Value);
	if(en_RedisRet_OK == iRedisRet)
	{
		return ServerEngine::en_DataRet_OK;
	}

	return ServerEngine::en_DataRet_SysError;
}

taf::Int32 LegionImp::delLegion(const std::string & strKey, taf::JceCurrentPtr current)
{
	int iDelKeyRet = delKeyFromLegionList(strKey);
	if(iDelKeyRet != ServerEngine::en_DataRet_OK)
	{
		return iDelKeyRet;
	}

	string sLowerNameKey;
	ServerEngine::StringToLower(strKey, sLowerNameKey);
	string strBase64NameKey = TC_Base64::encode(sLowerNameKey);

	int iRedisRet = m_redisClient.delString(strBase64NameKey);
	if(en_RedisRet_OK == iRedisRet)
	{	
		return ServerEngine::en_DataRet_OK;
	}
	else if(en_RedisRet_NoData == iRedisRet)
	{
		return ServerEngine::en_DataRet_NotExist;
	}

	return ServerEngine::en_DataRet_SysError;
}


taf::Int32 LegionImp::setGlobalData(const std::string & strKey,const std::string & strValue,taf::JceCurrentPtr current)
{	
	string sLowerNameKey;
	ServerEngine::StringToLower(strKey, sLowerNameKey);
	string strBase64NameKey = TC_Base64::encode(sLowerNameKey);
	
	string strBase64Value = TC_Base64::encode(strValue);

	int iRedisRet = m_redisClient.setHString(GLOBALVALUEKEY, strBase64NameKey, strBase64Value);
	if(en_RedisRet_OK == iRedisRet)
	{
		return ServerEngine::en_DataRet_OK;
	}

	return ServerEngine::en_DataRet_SysError;
}

taf::Int32 LegionImp::getGlobalData(const std::string & strKey,std::string &strValue,taf::JceCurrentPtr current)
{
	string sLowerNameKey;
	ServerEngine::StringToLower(strKey, sLowerNameKey);
	string strBase64NameKey = TC_Base64::encode(sLowerNameKey);

	string strTmpValue;
	int iRedisRet = m_redisClient.getHString(GLOBALVALUEKEY, strBase64NameKey, strTmpValue);
	if(en_RedisRet_NoData == iRedisRet)
	{
		return ServerEngine::en_DataRet_NotExist;
	}
	else if(en_RedisRet_SysError == iRedisRet)
	{
		return ServerEngine::en_DataRet_OK; 
	}
	else if(en_RedisRet_OK == iRedisRet)
	{
		strValue = TC_Base64::decode(strTmpValue);
		return ServerEngine::en_DataRet_OK;
	}
	
	return ServerEngine::en_DataRet_SysError;
}

taf::Int32 LegionImp::delGlobalData(const std::string & strKey,taf::JceCurrentPtr current)
{
	string sLowerNameKey;
	ServerEngine::StringToLower(strKey, sLowerNameKey);
	string strBase64NameKey = TC_Base64::encode(sLowerNameKey);

	int iRedisRet = m_redisClient.delHString(GLOBALVALUEKEY, strBase64NameKey);
	if(en_RedisRet_OK == iRedisRet)
	{	
		return ServerEngine::en_DataRet_OK;
	}
	else if(en_RedisRet_NoData == iRedisRet)
	{
		return ServerEngine::en_DataRet_NotExist;
	}

	return ServerEngine::en_DataRet_SysError;
}


