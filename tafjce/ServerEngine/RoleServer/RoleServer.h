#ifndef __ROLE_SERVER_H__
#define __ROLE_SERVER_H__

#include <iostream>
#include "servant/Application.h"

using namespace taf;


class RoleServer:public Application
{
public:

	RoleServer();
	~RoleServer();
	
	virtual void initialize();
	
	virtual void destroyApp();
};

extern RoleServer g_app;


#endif
