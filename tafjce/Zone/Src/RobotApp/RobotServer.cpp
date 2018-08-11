#include "MiniApr.h"
#include "ISocketSystem.h"
#include "FakeRandom.h"
#include "RobotServer.h"
#include "RobotServantImp.h"

using namespace MINIAPR;
using namespace std;

RobotServer g_app;

#ifndef RLOG
#define RLOG (LOG->debug())
#endif


void
RobotServer::initialize()
{
	RLOG<<"Begin to init GameServer..."<<endl;
	//initialize application here:

	//addConfig("GameServer.xml");
	
	string sFightObj = ServerConfig::Application + "." + ServerConfig::ServerName + ".RobotObj";
	addServant<RobotServantImp>(sFightObj);
	RLOG<<"FightServer Init OK..."<<endl;
}   
/////////////////////////////////////////////////////////////////
void
RobotServer::destroyApp()
{
	//destroy application here:
	//...
}
/////////////////////////////////////////////////////////////////
int
main(int argc, char* argv[])
{
	try
	{
		g_app.main(argc, argv);
		g_app.waitForShutdown();
	}
	catch (std::exception& e)
	{
		cerr << "std::exception:" << e.what() << std::endl;
	}
	catch (...)
	{
		cerr << "unknown exception." << std::endl;
	}
	return -1;
}

