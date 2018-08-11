#ifndef __LEGION_SERVER_H__
#define __LEGION_SERVER_H__

#include <iostream>
#include "servant/Application.h"

using namespace taf;


class ConvertCodeServer:public Application
{
public:

	ConvertCodeServer();
	~ConvertCodeServer();
	
	virtual void initialize();
	
	virtual void destroyApp();
};

extern ConvertCodeServer g_app;


#endif

