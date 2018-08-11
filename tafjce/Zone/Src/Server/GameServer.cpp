#include "GameServer.h"
#include "GameServantImp.h"
#include "WatchThread.h"
#include "PacketSendThread.h"

using namespace MINIAPR;
using namespace std;

GameServer g_app;
WatchThread g_watchThread;
PacketSendThread g_packerSendThread;

#ifndef RLOG
#define RLOG (LOG->debug())
#endif


void GameServer::initialize()
{
	RLOG<<"Begin to init GameServer..."<<endl;
	//initialize application here:

	addConfig(ServerConfig::ServerName + ".conf");
	addConfig("GameServer.xml");
	
	string sGameObj = ServerConfig::Application + "." + ServerConfig::ServerName + ".GameObj";
	addServant<GameServantImp>(sGameObj);
	RLOG<<"GameServer Init OK..."<<endl;
}   
/////////////////////////////////////////////////////////////////
void GameServer::destroyApp()
{
	//destroy application here:
	//...
}
/////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
	try
	{
		g_app.main(argc, argv);
		g_app.waitForShutdown();
		g_packerSendThread.StopSelf();
		g_packerSendThread.getThreadControl().join();
		g_watchThread.StopSelf();
		g_watchThread.getThreadControl().join();
		
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

