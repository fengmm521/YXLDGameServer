#ifndef __ROBOT_SERVER_H__
#define __ROBOT_SERVER_H__

#include "servant/Application.h"

//using namespace taf;

/**
 *
 **/
class RobotServer : public Application
{
public:
	/**
	 *
	 **/
	virtual ~RobotServer() {};

	/**
	 *
	 **/
	virtual void initialize();

	/**
	 *
	 **/
	virtual void destroyApp();
};

extern RobotServer g_app;


#endif


