#ifndef __FIGHT_DBACCESS_SERVER_H__
#define __FIGHT_DBACCESS_SERVER_H__

#include "ErrorHandle.h"
#include <iostream>
#include "servant/Application.h"
#include "MultiDbPool.h"
#include "DalFight.h"

using namespace taf;

class FightDbAccessServer:public Application
{
public:

	~FightDbAccessServer(){}

	virtual void initialize();

	/**
	 *
	 **/
	virtual void destroyApp();
};


extern FightDbAccessServer g_app;

#endif

