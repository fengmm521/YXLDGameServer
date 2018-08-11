#include "LegionServer.h"
#include "LegionImp.h"

LegionServer g_app;

LegionServer::LegionServer()
{
}

LegionServer::~LegionServer()
{
}
	
void LegionServer::initialize()
{
	signal(SIGPIPE,SIG_IGN);
	string sNameObj = ServerConfig::Application + "." + ServerConfig::ServerName + ".LegionObj";
	addServant<LegionImp>(sNameObj);
}
	
void LegionServer::destroyApp()
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

