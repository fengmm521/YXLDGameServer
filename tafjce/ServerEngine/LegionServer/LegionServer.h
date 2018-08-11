#ifndef __LEGION_SERVER_H__
#define __LEGION_SERVER_H__

#include <iostream>
#include "servant/Application.h"

using namespace taf;


class LegionServer:public Application
{
public:

	LegionServer();
	~LegionServer();
	
	virtual void initialize();
	
	virtual void destroyApp();
};

extern LegionServer g_app;


#endif

