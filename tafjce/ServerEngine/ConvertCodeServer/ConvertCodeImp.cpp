#include "ConvertCodeImp.h"
#include "CommLogic.h"
#include "svr_log.h"
#include "util/tc_base64.h"

#define CONVERTCODEKEY 	"covertcodekey"

ConvertCodeImp::ConvertCodeImp()
{
}

ConvertCodeImp::~ConvertCodeImp()
{
}

void ConvertCodeImp::initialize()
{	
	string strConfigFile = ServerConfig::ServerName + ".conf";
	TC_Config tmpCfg;
	tmpCfg.parseFile(strConfigFile);

	string strRedisIp = tmpCfg.get("/ConvertCodeImp<redisHost>", "127.0.0.1");
	int iRedisPort = TC_Common::strto<int>(tmpCfg.get("/ConvertCodeImp<redisPort>", "6378"));

	if(!m_redisClient.initlize(strRedisIp, iRedisPort, 1500) )
	{
		assert(false);
		return;
	}

}

void ConvertCodeImp::destroy()
{
}

taf::Int32 ConvertCodeImp::updateConvertCode(const std::string & convertCode,taf::Int32 &state,taf::Int32 updateState,taf::JceCurrentPtr current)
{

	if(updateState >ServerEngine::en_ConvertCodeState_HaveConvert || updateState< 0)
	{
		state = ServerEngine::en_ConvertCodeState_None;
		return  en_RedisRet_SysError;
	}
	
	string strValue;
	int iRedisRet1= m_redisClient.getHString(CONVERTCODEKEY, convertCode,strValue);
	if(iRedisRet1 == en_RedisRet_OK)
	{
		int iState = atoi(strValue.c_str());
		if(iState == ServerEngine::en_ConvertCodeState_HaveConvert)
		{
			state = ServerEngine::en_ConvertCodeState_HaveConvert;
			return  en_RedisRet_SysError;
		}
		
	}
	else
	{
		state = ServerEngine::en_ConvertCodeState_None;
		return  en_RedisRet_SysError;
	}
	
	stringstream ss;
	ss<<updateState; 
	int iRedisRet = m_redisClient.delHString(CONVERTCODEKEY, convertCode);
	if( en_RedisRet_OK == iRedisRet)
	{
		int iRet =  m_redisClient.setHString(CONVERTCODEKEY,convertCode, ss.str());
		if( en_RedisRet_OK == iRet)
		{
			state = ServerEngine::en_ConvertCodeState_CanConvert;
			return en_RedisRet_OK;
		}
	}
	state = ServerEngine::en_ConvertCodeState_None;
	return en_RedisRet_SysError;
	
}

taf::Int32 ConvertCodeImp::getConvertCode(const std::string & convertCode,taf::Int32 &state,taf::JceCurrentPtr current)
{
	string strValue;
	int iRedisRet = m_redisClient.getHString(CONVERTCODEKEY, convertCode,strValue);

	if(en_RedisRet_NoData == iRedisRet)
	{
		state = ServerEngine::en_ConvertCodeState_None;
		return en_RedisRet_NoData;
	}
	else if(en_RedisRet_SysError == iRedisRet)
	{
		state = ServerEngine::en_ConvertCodeState_None;
		return en_RedisRet_SysError;
	}
	else if(en_RedisRet_OK == iRedisRet)
	{
		//string strOrgValue = TC_Base64::decode(strValue);
		state = atoi(strValue.c_str());
		return en_RedisRet_OK;
	}

	return  en_RedisRet_SysError;
}

