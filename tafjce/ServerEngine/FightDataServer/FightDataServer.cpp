#include "FightDataServer.h"
#include "FightDataServantImp.h"
#include "SyncSettingThread.h"

FightDataServer g_app;
SyncSettingThread g_syncSettingThread;


FightDataServer::FightDataServer()
{
}

FightDataServer::~FightDataServer()
{
}
	
void FightDataServer::initialize()
{
	signal(SIGPIPE,SIG_IGN);
	string sNameObj = ServerConfig::Application + "." + ServerConfig::ServerName + ".FightDataObj";

	addServant<FightDataImp>(sNameObj);
}
	
void FightDataServer::destroyApp()
{
	
}


int main(int argc, char** argv)
{
	try
	{
		g_app.main(argc, argv);
		g_app.waitForShutdown();
		g_syncSettingThread.terminate();
		g_syncSettingThread.getThreadControl().join();
	}
	catch (std::exception& e)
	{
		cerr << "std::exception:" << e.what() << std::endl;
	}
	catch (...)
	{
		cerr << "unknown exception." << std::endl;
	}

	return 0;
}

