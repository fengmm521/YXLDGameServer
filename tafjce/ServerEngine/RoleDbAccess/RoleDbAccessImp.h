#ifndef __ROLEDBACCESS_IMP_H__
#define __ROLEDBACCESS_IMP_H__

#include "DbAccess.h"
#include "servant/Application.h"
#include "DalRole.h"
#include "ErrorHandle.h"
#include "CommLogic.h"
#include "MultiDbPool.h"

class RoleDbAccessImp:public ServerEngine::DbAccess
{
public:

	RoleDbAccessImp();
	~RoleDbAccessImp();

	// Servant Interface
	virtual void initialize();
	virtual void destroy();

	// DbAccess Interface
	virtual taf::Int32 setString(const std::string & strKey,const std::string & strValue,taf::JceCurrentPtr current);
	virtual taf::Int32 delString(const std::string & strKey,taf::JceCurrentPtr current);
	virtual taf::Int32 getString(const std::string & strKey,std::string &strValue,taf::JceCurrentPtr current);
};


#endif

