#ifndef __REDIS_CLIENT_H__
#define __REDIS_CLIENT_H__

#include "hiredis/hiredis.h"

BEGIN_MINIAPR_NAMESPACE

enum
{
	en_RedisRet_OK = 0,
	en_RedisRet_NoData,
	en_RedisRet_SysError,
};

class RedisClient
{
public:

	RedisClient();
	~RedisClient();

	bool initlize(const std::string& strIp, int iPort, int iMillSecond);

	// globl key/value operation
	int getString(const std::string& strKey, std::string& strValue);

	int setString(const std::string& strKey, const std::string& strValue);

	int delString(const std::string& strKey);

	// hash key/value operation
	int getHString(const std::string& strHashName, const std::string& strKey, std::string& strValue);

	int setHString(const std::string& strHashName, const std::string& strKey, const std::string& strValue);

	int delHString(const std::string& strHashName, const std::string& strKey);

protected:

	bool reconnectRedis();

protected:

	std::string m_strRedisIP;
	int m_iRedisPort;
	redisContext* m_pRedisClient;
};

END_MINIAPR_NAMESPACE

#endif
