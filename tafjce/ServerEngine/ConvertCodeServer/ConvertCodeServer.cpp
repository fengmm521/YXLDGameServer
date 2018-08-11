#include "ConvertCodeServer.h"
#include "ConvertCodeImp.h"

ConvertCodeServer g_app;

ConvertCodeServer::ConvertCodeServer()
{
}

ConvertCodeServer::~ConvertCodeServer()
{
}
	
void ConvertCodeServer::initialize()
{
	signal(SIGPIPE,SIG_IGN);
	string sNameObj = ServerConfig::Application + "." + ServerConfig::ServerName + ".ConvertCodeObj";
	addServant<ConvertCodeImp>(sNameObj);
}
	
void ConvertCodeServer::destroyApp()
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

