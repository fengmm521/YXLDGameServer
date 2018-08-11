#ifndef __LEGION_IMP_H__
#define __LEGION_IMP_H__

#include "Legion.h"
#include "hiredis.h"
#include "MiniApr.h"
#include "Name.h"

using namespace MINIAPR;


class LegionImp:public ServerEngine::Legion
{
public:

	LegionImp();
	~LegionImp();

	// taf::Servant Interface
	virtual void initialize();
    virtual void destroy();

	// ServerEngine::Legion Interface
	virtual taf::Int32 getLegionList(ServerEngine::LegionList &legionList,taf::JceCurrentPtr current);
    virtual taf::Int32 newLegion(const std::string & strLegionName,const std::string & data,taf::JceCurrentPtr current);
	virtual taf::Int32 getLegion(const std::string & strKey,std::string &strValue,taf::JceCurrentPtr current);
	virtual taf::Int32 updateLegion(const std::string & strKey,const std::string & strValue,taf::JceCurrentPtr current);
	virtual taf::Int32 delLegion(const std::string & strKey, taf::JceCurrentPtr current);
	
    virtual taf::Int32 setGlobalData(const std::string & strKey,const std::string & strValue,taf::JceCurrentPtr current);
    virtual taf::Int32 getGlobalData(const std::string & strKey,std::string &strValue,taf::JceCurrentPtr current);
    virtual taf::Int32 delGlobalData(const std::string & strKey,taf::JceCurrentPtr current);

	int addNewLegionKey(const string& strLegionName);
	int delKeyFromLegionList(const string& strLegionName);

private:

	RedisClient m_redisClient;
	ServerEngine::NamePrx m_namePrx;
};

#endif

