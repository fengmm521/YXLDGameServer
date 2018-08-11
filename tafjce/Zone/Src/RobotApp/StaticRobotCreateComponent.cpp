#include "MiniApr.h"

using namespace MINIAPR;
using namespace std;
extern "C" IComponent* createTimerComponent(Int32 nVersion);
extern "C" IComponent* createProfilerManager(Int32 nVersion);
extern "C" IComponent* createProfileTimer(Int32 nVersion);
extern "C" IComponent* createProfileStrategyAverage(Int32 nVersion);
extern "C" IComponent* createScriptEngine(Int32 nVersion);
extern "C" IComponent* CreateSocketSystem(Int32 nVersion);
extern "C" IComponent* CreateSimComponent(Int32 nVersion);

typedef IComponent* (*COMPONENT_CREATEFUN)(Int32);
typedef IObject* (*OBJECT_CREATEFUN)();
static std::map<std::string, COMPONENT_CREATEFUN> s_mapComponentCreateFun;
static std::map<std::string, OBJECT_CREATEFUN> s_mapObjectCreateFun;

void initComponentCreateFun()
{
	static bool s_bInitComponentFun = false;
	if(!s_bInitComponentFun) 
		s_bInitComponentFun = true;
	else
 		return;
	s_mapComponentCreateFun["TimeAxis"] = &createTimerComponent;
	s_mapComponentCreateFun["ProfileManager"] = &createProfilerManager;
	s_mapComponentCreateFun["ProfileTimer"] = &createProfileTimer;
	s_mapComponentCreateFun["StrategyTimer"] = &createProfileStrategyAverage;
	s_mapComponentCreateFun["ScriptEngine"] = &createScriptEngine;
	s_mapComponentCreateFun["SocketSystem"] = &CreateSocketSystem;
	s_mapComponentCreateFun["ClientSim"] = &CreateSimComponent;
}

void initObjCreateFun()
{
	static bool s_bInitObjectFun = false;
	if(!s_bInitObjectFun)
		s_bInitObjectFun = true;
	else
		return;
}

extern "C" IComponent* createComponentStatic(const std::string& strComponentName, Int32 nVersion)
{

	initComponentCreateFun();
	std::map<std::string, COMPONENT_CREATEFUN>::iterator it = s_mapComponentCreateFun.find(strComponentName);
	if(it == s_mapComponentCreateFun.end() )
	{
		return NULL;
	}
	return (it->second)(nVersion);
}

extern "C" IObject* createObjectStatic(const std::string& strObjectName)
{
	initObjCreateFun();
	std::map<std::string, OBJECT_CREATEFUN>::iterator it = s_mapObjectCreateFun.find(strObjectName);
	if(it == s_mapObjectCreateFun.end() )
	{
		return NULL;
	}
	return (it->second)();
}
