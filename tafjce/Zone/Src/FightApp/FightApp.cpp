#include "FightApp.h"
#include "FightImp.h"

FightApp g_app;

FightApp::FightApp()
{
}

FightApp::~FightApp()
{
}
	
void FightApp::initialize()
{
	signal(SIGPIPE,SIG_IGN);
	string sNameObj = ServerConfig::Application + "." + ServerConfig::ServerName + ".FightObj";
	addServant<FightImp>(sNameObj);
}
	
void FightApp::destroyApp()
{
	
}


int main(int argc, char** argv)
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

	return 0;
}
