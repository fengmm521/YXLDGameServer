#include "MiniApr.h"

using namespace MINIAPR;
using namespace std;


struct ReplyHelper
{
	ReplyHelper(redisReply* pReply):m_pSaveReply(pReply){}
	~ReplyHelper()
	{
		if(m_pSaveReply)
		{
			freeReplyObject(m_pSaveReply);
			m_pSaveReply = NULL;
		}
	}
	redisReply* m_pSaveReply;
};


RedisClient::RedisClient():m_pRedisClient(NULL)
{
}

RedisClient::~RedisClient()
{
	if(m_pRedisClient)
	{
		redisFree(m_pRedisClient);
		m_pRedisClient = NULL;
	}
}

bool RedisClient::initlize(const std::string& strIp, int iPort, int iMillSecond)
{
	// 先忽略SIGPIPE
	signal(SIGPIPE,SIG_IGN);	

	m_strRedisIP = strIp;
	m_iRedisPort = iPort;

	struct timeval timeout = {iMillSecond/1000, (iMillSecond%1000)*1000 };
	m_pRedisClient = redisConnectWithTimeout((char*)strIp.c_str(), iPort, timeout);
	if (m_pRedisClient->err) 
	{
		MINIAPR_ERROR("redis connect error|%s|%d|%s", strIp.c_str(), iPort, m_pRedisClient->errstr);
		redisFree(m_pRedisClient);
		m_pRedisClient = NULL;
		
		return false;
	}

	return true;
}

bool RedisClient::reconnectRedis()
{
	if(!m_pRedisClient || (m_pRedisClient->err & (REDIS_ERR_IO | REDIS_ERR_EOF) ) )
	{
		redisFree(m_pRedisClient);
		m_pRedisClient = NULL;
		struct timeval timeout = {0, 500000 };
		m_pRedisClient = redisConnectWithTimeout((char*)m_strRedisIP.c_str(), m_iRedisPort, timeout);

		// 记录日志
		if (m_pRedisClient->err) 
		{
			MINIAPR_ERROR("redis reconnect error|%s|%d|%s", m_strRedisIP.c_str(), m_iRedisPort, m_pRedisClient->errstr);
		}
		else
		{
			MINIAPR_TRACE("redis reconnect success|%s|%d", m_strRedisIP.c_str(), m_iRedisPort);
			return true;
		}
	}

	return false;
}

int RedisClient::getString(const std::string& strKey, std::string& strValue)
{
	reconnectRedis();
	if(m_pRedisClient->err)
	{
		return en_RedisRet_SysError;
	}

	redisReply *reply = (redisReply*)redisCommand(m_pRedisClient, "get %s", strKey.c_str() );
	if(!reply) return en_RedisRet_SysError;

	ReplyHelper keeper(reply);

	// 如果连接断了,重新执行
	if(reconnectRedis() )
	{
		return getString(strKey, strValue);
	}

	if(reply->type == REDIS_REPLY_STRING)
	{
		strValue = reply->str;
		return en_RedisRet_OK;
	}
	else if(reply->type == REDIS_REPLY_NIL)
	{
		return en_RedisRet_NoData;
	}
	else
	{
		return en_RedisRet_SysError;
	}
}

int RedisClient::setString(const std::string& strKey, const std::string& strValue)
{
	reconnectRedis();
	if(m_pRedisClient->err)
	{
		return en_RedisRet_SysError;
	}

	redisReply *reply = (redisReply*)redisCommand(m_pRedisClient, "set %s %s", strKey.c_str(), strValue.c_str() );
	if(!reply) return en_RedisRet_SysError;

	ReplyHelper keeper(reply);

	// 如果连接断了,重新执行
	if(reconnectRedis() )
	{
		return setString(strKey, strValue);
	}

	if( (reply->type == REDIS_REPLY_STATUS) && (strcmp(reply->str, "OK") == 0) )
	{
		return en_RedisRet_OK;
	}		

	return en_RedisRet_SysError;
}

int RedisClient::delString(const std::string& strKey)
{
	reconnectRedis();
	if(m_pRedisClient->err)
	{
		return en_RedisRet_SysError;
	}

	redisReply *reply = (redisReply*)redisCommand(m_pRedisClient, "del %s", strKey.c_str() );
	if(!reply) return en_RedisRet_SysError;
	
	ReplyHelper keeper(reply);

	if(reconnectRedis() )
	{
		return delString(strKey);
	}

	if(reply->type == REDIS_REPLY_INTEGER)
	{
		if(reply->integer == 1)
		{
			return en_RedisRet_OK;
		}
		else
		{
			return en_RedisRet_NoData;
		}
	}	

	return en_RedisRet_SysError;
}

int RedisClient::getHString(const std::string& strHashName, const std::string& strKey, std::string& strValue)
{
	reconnectRedis();
	if(m_pRedisClient->err)
	{
		return en_RedisRet_SysError;
	}

	redisReply *reply = (redisReply*)redisCommand(m_pRedisClient, "hget %s %s",strHashName.c_str(), strKey.c_str() );
	if(!reply) return en_RedisRet_SysError;

	ReplyHelper keeper(reply);

	// 如果连接断了,重新执行
	if(reconnectRedis() )
	{
		return getHString(strHashName, strKey, strValue);
	}

	if(reply->type == REDIS_REPLY_STRING)
	{
		strValue = reply->str;
		return en_RedisRet_OK;
	}
	else if(reply->type == REDIS_REPLY_NIL)
	{
		return en_RedisRet_NoData;
	}
	else
	{
		return en_RedisRet_SysError;
	}
}
	
int RedisClient::setHString(const std::string& strHashName, const std::string& strKey, const std::string& strValue)
{
	reconnectRedis();
	if(m_pRedisClient->err)
	{
		return en_RedisRet_SysError;
	}

	redisReply *reply = (redisReply*)redisCommand(m_pRedisClient, "hset %s %s %s", strHashName.c_str(), strKey.c_str(), strValue.c_str() );
	if(!reply) return en_RedisRet_SysError;

	ReplyHelper keeper(reply);

	// 如果连接断了,重新执行
	if(reconnectRedis() )
	{
		return setHString(strHashName, strKey, strValue);
	}

	if(reply->type == REDIS_REPLY_INTEGER)
	{
		// 返回0,表示这是一次覆盖写行为
		if( (reply->integer == 1) || (reply->integer == 0) )
		{
			return en_RedisRet_OK;
		}
	}

	return en_RedisRet_SysError;
}

int RedisClient::delHString(const std::string& strHashName, const std::string& strKey)
{
	reconnectRedis();
	if(m_pRedisClient->err)
	{
		return en_RedisRet_SysError;
	}

	redisReply *reply = (redisReply*)redisCommand(m_pRedisClient, "hdel %s %s", strHashName.c_str(), strKey.c_str() );
	if(!reply) return en_RedisRet_SysError;
	
	ReplyHelper keeper(reply);

	if(reconnectRedis() )
	{
		return delHString(strHashName, strKey);
	}

	if(reply->type == REDIS_REPLY_INTEGER)
	{
		if(reply->integer == 1)
		{
			return en_RedisRet_OK;
		}
		else
		{
			return en_RedisRet_NoData;
		}
	}	

	return en_RedisRet_SysError;
}



