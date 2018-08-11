#ifndef __NAMESERVER_H__
#define __NAMESERVER_H__

#include <iostream>
#include "servant/Application.h"

using namespace taf;


class NameServer:public Application
{
public:

	NameServer();
	~NameServer();
	
	virtual void initialize();
	
	virtual void destroyApp();
};

extern NameServer g_app;

#endif
