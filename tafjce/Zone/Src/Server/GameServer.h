#ifndef __GAME_SERVER_H__
#define __GAME_SERVER_H__

#include "MiniApr.h"

using namespace MINIAPR;

#include "servant/Application.h"

#include "util/tc_file.h"
#include "define.h"
#include "comm_func.h"
#include "CommLogic.h"
#include "svr_log.h"
#include "GameServer.pb.h"
#include "GameServerExt.pb.h"



//using namespace taf;

/**
 *
 **/
class GameServer : public Application
{
public:
	/**
	 *
	 **/
	virtual ~GameServer() {};

	/**
	 *
	 **/
	virtual void initialize();

	/**
	 *
	 **/
	virtual void destroyApp();
};

extern GameServer g_app;


#endif

