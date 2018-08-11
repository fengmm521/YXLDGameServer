#ifndef __FIGHTDATA_SERVER_H__
#define __FIGHTDATA_SERVER_H__

#include <iostream>
#include "servant/Application.h"

using namespace taf;


class FightDataServer:public Application
{
public:

	FightDataServer();
	~FightDataServer();
	
	virtual void initialize();
	
	virtual void destroyApp();
};

extern FightDataServer g_app;




#endif
