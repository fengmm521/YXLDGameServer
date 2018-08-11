#ifndef __NAME_DBACCESS_SERVER_H__
#define __NAME_DBACCESS_SERVER_H__

#include "ErrorHandle.h"
#include <iostream>
#include "servant/Application.h"
#include "MultiDbPool.h"
#include "DalName.h"

using namespace taf;

class NameDbAccessServer:public Application
{
protected:

	virtual void initialize();

	/**
	 *
	 **/
	virtual void destroyApp();
};


extern NameDbAccessServer g_app;

#endif
