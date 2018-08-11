#ifndef __ROUTER_SERVER_H__
#define __ROUTER_SERVER_H__

#include <iostream>
#include "servant/Application.h"

using namespace taf;


class RouterServantHandle: public ServantHandle
{
public:
	
    virtual void handleClose(const TC_EpollServer::tagRecvData &stRecvData);

};



class RouterServer: public Application
{
public:

	RouterServer();
	~RouterServer();

	virtual void initialize();

	virtual void destroyApp();

	virtual void setHandle(TC_EpollServer::BindAdapterPtr& adapter);
	
};

extern RouterServer g_app;


#endif

