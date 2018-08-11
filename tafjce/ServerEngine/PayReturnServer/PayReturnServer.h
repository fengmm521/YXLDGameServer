#ifndef __LEGION_SERVER_H__
#define __LEGION_SERVER_H__

#include <iostream>
#include "servant/Application.h"

using namespace taf;


class PayReturnServer:public Application
{
public:

	PayReturnServer();
	~PayReturnServer();
	
	virtual void initialize();
	
	virtual void destroyApp();
};

extern PayReturnServer g_app;


#endif

