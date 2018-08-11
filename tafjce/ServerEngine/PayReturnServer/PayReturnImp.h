#ifndef __LEGION_IMP_H__
#define __LEGION_IMP_H__

#include "PayReturn.h"
#include "hiredis.h"
#include "MiniApr.h"

using namespace MINIAPR;


class PayReturnImp:public ServerEngine::PayReturn
{
public:

	PayReturnImp();
	~PayReturnImp();

	// taf::Servant Interface
	virtual void initialize();
    	virtual void destroy();

	// ServerEngine::ConvertCode Interface
	virtual taf::Int32 payReturnMoneyAddVIP(const std::string & strAccount,taf::Int32 &state,taf::Int32 &iMoney,taf::Int32 &VipLevel,taf::JceCurrentPtr current);
	
private:
	RedisClient m_redisClient;
};

#endif

