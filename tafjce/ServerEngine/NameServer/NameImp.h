#ifndef __NAMEIMP_H__
#define __NAMEIMP_H__

#include "Name.h"
#include "DbAccess.h"
#include "hiredis.h"
#include "MiniApr.h"

using namespace MINIAPR;

class NameImp:public ServerEngine::Name
{
public:

	NameImp();
	~NameImp();

	// Servant Interface
	virtual void initialize();
    virtual void destroy();

	virtual taf::Int32 doAddWorldName(const std::string & strName,taf::Int32 iWorldID,const std::string & strAccount,taf::Int32 iRolsPos,taf::JceCurrentPtr current);
   	virtual taf::Int32 delWorldName(const std::string & strName,taf::JceCurrentPtr current);
	virtual taf::Int32 getNameDesc(const std::string & strName,ServerEngine::NameDesc &descInfo,taf::JceCurrentPtr current);

private:

	int getNameFromRedis(const string& strKey, ServerEngine::NameDesc& nameInfo);
	int delNameFromRedis(const string& strKey);
	int setNameValue(const string& strKey, const ServerEngine::NameDesc& nameInfo);

private:

	//redisContext* m_pRedisContext;
	RedisClient m_redisClient;
	//ServerEngine::DbAccessPrx m_nameDbAccessPrx;
};


#endif
