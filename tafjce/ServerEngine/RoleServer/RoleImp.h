#ifndef __ROLE_IMP_H__
#define __ROLE_IMP_H__

#include "Role.h"
#include "hiredis.h"
#include "MiniApr.h"
#include "DbAccess.h"
#include "Name.h"

using namespace MINIAPR;

class RoleDbAccessCb:public ServerEngine::DbAccessPrxCallback
{
public:

	RoleDbAccessCb(taf::JceCurrentPtr current, RedisClient* pRedisClient, const string& strKey):m_current(current), 
		m_pRedisClient(pRedisClient), m_strRoleKey(strKey){}

	virtual void callback_setString(taf::Int32 ret);
    virtual void callback_setString_exception(taf::Int32 ret);
    virtual void callback_delString(taf::Int32 ret);
    virtual void callback_delString_exception(taf::Int32 ret);
    virtual void callback_getString(taf::Int32 ret,  const std::string& strValue);
    virtual void callback_getString_exception(taf::Int32 ret);

private:

	taf::JceCurrentPtr m_current;
	RedisClient* m_pRedisClient;
	string m_strRoleKey;
};

class RoleImp:public ServerEngine::Role
{
public:

	RoleImp();
	~RoleImp();

	// taf::Servant Interface
	virtual void initialize();
    virtual void destroy();

	// ServerEngine::Role Interface
	virtual taf::Int32 newRole(const std::string & strAccount,taf::Int32 nPos,taf::Int32 nWorldID,const std::string & strName,const ServerEngine::PIRole & roleInfo,taf::JceCurrentPtr current);
	virtual taf::Int32 updateRole(const ServerEngine::PKRole & keyData,const ServerEngine::PIRole & roleInfo,taf::JceCurrentPtr current);
	virtual taf::Int32 delRole(const ServerEngine::PKRole & keyData,taf::JceCurrentPtr current);
	virtual taf::Int32 getRole(const ServerEngine::PKRole & keyData,ServerEngine::PIRole &roleInfo,taf::JceCurrentPtr current);

private:

	RedisClient m_redisClient;
	ServerEngine::NamePrx m_namePrx;
	ServerEngine::DbAccessPrx m_roleDbAccessPrx;
};

#endif
