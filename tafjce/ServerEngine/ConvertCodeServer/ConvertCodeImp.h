#ifndef __LEGION_IMP_H__
#define __LEGION_IMP_H__

#include "ConvertCode.h"
#include "hiredis.h"
#include "MiniApr.h"

using namespace MINIAPR;


class ConvertCodeImp:public ServerEngine::ConvertCode
{
public:

	ConvertCodeImp();
	~ConvertCodeImp();

	// taf::Servant Interface
	virtual void initialize();
    	virtual void destroy();

	// ServerEngine::ConvertCode Interface
	virtual taf::Int32  updateConvertCode(const std::string & convertCode,taf::Int32 &state,taf::Int32 updateState,taf::JceCurrentPtr current);
    virtual taf::Int32 getConvertCode(const std::string & convertCode,taf::Int32 &state,taf::JceCurrentPtr current);
	
private:
	RedisClient m_redisClient;
};

#endif

