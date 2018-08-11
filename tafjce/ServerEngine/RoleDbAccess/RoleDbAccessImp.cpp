#include "RoleDbAccessImp.h"
#include "DbPool.h"
#include "DalRole.h"

using namespace ServerEngine;

RoleDbAccessImp::RoleDbAccessImp()
{
}

RoleDbAccessImp::~RoleDbAccessImp()
{
}

void RoleDbAccessImp::initialize()
{
	// nothing
}

void RoleDbAccessImp::destroy()
{
	// nothing 
}


taf::Int32 RoleDbAccessImp::setString(const std::string & strKey,const std::string & strValue,taf::JceCurrentPtr current)
{
	RLOG<<"RoleDbAccessImp setString:"<<strKey<<"|"<<endl;
    pool_object<DefaultRoleDb> aDbConn(DbPool<DefaultRoleDb>::getInstance());
    
    return aDbConn->Set(strKey, strValue);
}

taf::Int32 RoleDbAccessImp::delString(const std::string & strKey,taf::JceCurrentPtr current)
{
	pool_object<DefaultRoleDb> aDbConn(DbPool<DefaultRoleDb>::getInstance());
    return aDbConn->Del(strKey);
}

taf::Int32 RoleDbAccessImp::getString(const std::string & strKey,std::string &strValue,taf::JceCurrentPtr current)
{
	RLOG<<"RoleDbAccessImp getString:"<<strKey<<"|"<<endl;
	//从连接池获取一个连接
    pool_object<DefaultRoleDb> aDbConn(DbPool<DefaultRoleDb>::getInstance());

    return aDbConn->Get(strKey, strValue);
}

