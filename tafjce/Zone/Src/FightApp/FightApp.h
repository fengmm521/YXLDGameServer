#ifndef __FIGHTAPP_H__
#define __FIGHTAPP_H__

#include <iostream>
#include "servant/Application.h"

using namespace taf;


class FightApp:public Application
{
public:

	FightApp();
	~FightApp();
	
	virtual void initialize();
	
	virtual void destroyApp();
};

extern FightApp g_app;


#endif
