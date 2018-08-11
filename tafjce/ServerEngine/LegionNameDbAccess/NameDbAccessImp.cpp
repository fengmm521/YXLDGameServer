#include "NameDbAccessImp.h"
#include "DbPool.h"
#include "DalName.h"

using namespace ServerEngine;

NameDbAccessImp::NameDbAccessImp()
{
}

NameDbAccessImp::~NameDbAccessImp()
{
}

void NameDbAccessImp::initialize()
{
	// nothing
}

void NameDbAccessImp::destroy()
{
	// nothing 
}


taf::Int32 NameDbAccessImp::setString(const std::string & strKey,const std::string & strValue,taf::JceCurrentPtr current)
{
	RLOG<<"NameDbAccessImp setString:"<<strKey<<"|"<<endl;
    pool_object<DefaultNameDb> aDbConn(DbPool<DefaultNameDb>::getInstance());
    
    return aDbConn->Set(strKey, strValue);
}

taf::Int32 NameDbAccessImp::delString(const std::string & strKey,taf::JceCurrentPtr current)
{
	pool_object<DefaultNameDb> aDbConn(DbPool<DefaultNameDb>::getInstance());
    return aDbConn->Del(strKey);
}

taf::Int32 NameDbAccessImp::getString(const std::string & strKey,std::string &strValue,taf::JceCurrentPtr current)
{
	RLOG<<"NameDbAccessImp getString:"<<strKey<<"|"<<endl;
	//从连接池获取一个连接
    pool_object<DefaultNameDb> aDbConn(DbPool<DefaultNameDb>::getInstance());

    return aDbConn->Get(strKey, strValue);
}



