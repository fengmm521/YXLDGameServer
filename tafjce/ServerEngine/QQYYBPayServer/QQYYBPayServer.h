#ifndef __NAMESERVER_H__
#define __NAMESERVER_H__

#include <iostream>
#include "servant/Application.h"

using namespace taf;


class QQYYBPayServer:public Application
{
public:

	QQYYBPayServer();
	~QQYYBPayServer();
	
	virtual void initialize();
	
	virtual void destroyApp();
};

extern QQYYBPayServer g_app;

#endif
