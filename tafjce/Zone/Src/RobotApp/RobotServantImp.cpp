#include "RobotServerPch.h"
#include "RobotServantImp.h"
#include "ClientSimComponent.h"

taf::Servant* g_pMainServant = NULL;
CFakeRandom g_oFakeRandom;

Uint32 g_lastUpdateTime;


bool isGameServer(){return false;}
extern "C" int luaopen_pb (lua_State *L);


//extern int luaopen_AegisServerExport (lua_State* tolua_S);
//extern int luaopen_AegisRobotInnerExport(lua_State* tolua_S);

extern int luaopen_AegisRobotInnerExport (lua_State* tolua_S);

string getDataPath()
{
	return taf::ServerConfig::DataPath;
}

int getRunSecond()
{
	struct timespec tmpSpec;
	clock_gettime(CLOCK_MONOTONIC, &tmpSpec);
		
	return tmpSpec.tv_sec;
}


void logMsg(const string& strMgs)
{
	FDLOG("Robot")<<strMgs<<endl;
}


void RobotServantImp::initialize()
{
	g_oFakeRandom.SetSeed( time( NULL));
	bool bResult = getCompomentObjectManager()->loadScheme(taf::ServerConfig::BasePath + "RobotServer.xml");
	if(!bResult)
	{
		assert(0);
		return;
	}

	// lua×¢²á
	IScriptEngine* pScriptEngine = getComponent<IScriptEngine>(COMPNAME_ScriptEngine, IID_IScriptEngine);
	assert(pScriptEngine);

	//luaopen_AegisServerExport(pScriptEngine->getLuaState() );
	//luaopen_AegisRobotInnerExport(pScriptEngine->getLuaState() );
	luaopen_AegisRobotInnerExport(pScriptEngine->getLuaState() );
	luaopen_pb(pScriptEngine->getLuaState() );

	string strFilePath = taf::ServerConfig::DataPath + "/script/robot/RobotAIMain.lua";
	pScriptEngine->runFile(strFilePath.c_str() );

	srand(time(NULL) );
}

void RobotServantImp::destroy()
{
}

int RobotServantImp::doCustomMessage(bool bExpectIdle)
{
	ISocketSystem* pSocketSystem = getComponent<ISocketSystem>("SocketSystem", IID_ISocektSystem);
	assert(pSocketSystem);

	pSocketSystem->RunSystem();
	return 0;
}

