#ifndef __SNAPSHOT_SERVER_H__
#define __SNAPSHOT_SERVER_H__


#include <iostream>
#include "servant/Application.h"

using namespace taf;


class SnapshotServer:public Application
{
public:

	SnapshotServer();
	~SnapshotServer();
	
	virtual void initialize();
	
	virtual void destroyApp();
};

extern SnapshotServer g_app;



#endif


