#ifndef __FIGHT_DATASERVANT_IMP_H__
#define __FIGHT_DATASERVANT_IMP_H__

#include "FightData.h"
#include "hiredis.h"
#include "MiniApr.h"
#include "DbAccess.h"
#include "Name.h"

using namespace MINIAPR;

class FightDataDbAccessCb:public ServerEngine::DbAccessPrxCallback
{
public:

	FightDataDbAccessCb(taf::JceCurrentPtr current, RedisClient* pRedisClient, const string& strKey):m_current(current), 
		m_pRedisClient(pRedisClient), m_strKey(strKey){}

	virtual void callback_setString(taf::Int32 ret);
    virtual void callback_setString_exception(taf::Int32 ret);
    virtual void callback_delString(taf::Int32 ret);
    virtual void callback_delString_exception(taf::Int32 ret);
    virtual void callback_getString(taf::Int32 ret,  const std::string& strValue);
    virtual void callback_getString_exception(taf::Int32 ret);

private:

	taf::JceCurrentPtr m_current;
	RedisClient* m_pRedisClient;
	string m_strKey;
};

class FightDataImp:public ServerEngine::FightData
{
public:

	FightDataImp();
	~FightDataImp();

	// taf::Servant Interface
	virtual void initialize();
    virtual void destroy(){}

	// ServerEngine::Role Interface
	virtual taf::Int32 saveFightData(const ServerEngine::PKFight & keyData,const std::string & strBattleData,taf::JceCurrentPtr current);
    virtual taf::Int32 getFightData(const ServerEngine::PKFight & keyData,std::string &strBattleData,taf::JceCurrentPtr current);
	virtual taf::Int32 delFightData(const ServerEngine::PKFight & keyData,taf::JceCurrentPtr current);

private:

	RedisClient m_redisClient;
	ServerEngine::DbAccessPrx m_fightDbAccessPrx;
};



#endif
