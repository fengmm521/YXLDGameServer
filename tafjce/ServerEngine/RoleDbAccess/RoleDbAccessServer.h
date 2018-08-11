#ifndef __ROLE_DBACCESS_SERVER_H__
#define __ROLE_DBACCESS_SERVER_H__

#include "ErrorHandle.h"
#include <iostream>
#include "servant/Application.h"
#include "MultiDbPool.h"
#include "DalRole.h"

using namespace taf;

class RoleDbAccessServer:public Application
{
public:

	~RoleDbAccessServer(){}

	virtual void initialize();

	/**
	 *
	 **/
	virtual void destroyApp();
};


extern RoleDbAccessServer g_app;

#endif

