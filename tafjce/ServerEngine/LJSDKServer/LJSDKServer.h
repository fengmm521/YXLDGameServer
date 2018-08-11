#ifndef __LJSDK_SERVER_H__
#define __LJSDK_SERVER_H__

#include <iostream>
#include "servant/Application.h"

using namespace taf;

/*
class LJSDKServantHandle: public ServantHandle
{
public:
	
    virtual void handleClose(const TC_EpollServer::tagRecvData &stRecvData);

};

*/

class LJSDKServer: public Application
{
public:

	LJSDKServer();
	~LJSDKServer();

	virtual void initialize();

	virtual void destroyApp();

	//virtual void setHandle(TC_EpollServer::BindAdapterPtr& adapter);
	
};

extern LJSDKServer g_app;


#endif

