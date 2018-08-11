#include "MiniApr.h"
#include "ScriptEngine.h"
#include "lstate.h"

using namespace MINIAPR;
using namespace std;


extern "C" MINIAPR::IComponent* createScriptEngine(Int32 nVersion)
{
	return new ScriptEngine;
}


#define SCRIPT_MODULE	"Script"

#define BEGIN_RUNFUNCTION(strName, ret)\
LuaStackHelper helper(m_luaState);\
lua_getglobal(m_luaState, strName);\
if (!lua_isfunction(m_luaState, -1) )\
{\
	FDLOG("Script")<<"[ScriptEngine::runFunction][:"<<strName<<"] is not a function"<<endl;\
	return ret;\
}


#define END_RUNFUNCTION(ret)\
if (error)\
{\
	std::string errMsg = lua_tostring(m_luaState,-1);\
	FDLOG("Script")<<"run lua function:["<<strName<<"]error:["<<errMsg.c_str()<<"]"<<endl;\
	return ret;\
}


#define END_STRINGRET_FUNCTION()\
	END_RUNFUNCTION("")\
	if(!lua_isstring(m_luaState, -1) )\
	{\
		FDLOG("Script")<<"runFunction_String:"<<strName<<"|return not string\r\n";\
		return "";\
	}\
	std::string strResult = lua_tostring(m_luaState, -1);\
	return strResult;


#define END_BOOLRET_FUNCTION()\
	END_RUNFUNCTION(false)\
	if(!lua_isboolean(m_luaState, -1) )\
	{\
		FDLOG("Script")<<"runFunction_Bool:"<<strName<<"|return not bool\r\n";\
		return false;\
	}\
	bool bResult = lua_toboolean(m_luaState, -1);\
	return bResult;


#define END_INTRET_FUNCTION()\
	END_RUNFUNCTION(0)\
	if(!lua_isnumber(m_luaState, -1) )\
	{\
		FDLOG("Script")<<"runFunction_Int:"<<strName<<"|return not Int\r\n";\
		return 0;\
	}\
	int iResult = (int)lua_tonumber(m_luaState, -1);\
	return iResult;


class LuaStackHelper
{
public:
	
	LuaStackHelper(lua_State* L)
	{
		m_luaState = L;
		m_nTop = lua_gettop(L);
	}

	~LuaStackHelper()
	{
		lua_settop(m_luaState, m_nTop);
	}

private:
	lua_State*	m_luaState;
	int			m_nTop;
};

ScriptEngine::ScriptEngine():m_luaState(NULL), m_bOwn(false)
{
}

ScriptEngine::~ScriptEngine()
{
	if(m_bOwn && m_luaState)
	{
		lua_close(m_luaState);
		m_luaState = NULL;
	}
}

bool ScriptEngine::initlize(const PropertySet& propSet)
{
	m_luaState = lua_open();
	m_bOwn = true;
	luaL_openlibs(m_luaState);
	
	return true;
}

void ScriptEngine::bindState(lua_State* L)
{
	if(m_luaState == L) return;

	if(m_bOwn && m_luaState)
	{
		lua_close(m_luaState);
		m_luaState = NULL;
	}

	m_luaState = L;
	m_bOwn = false;
}

lua_State* ScriptEngine::getLuaState()
{
	return m_luaState;
}

void ScriptEngine::runFunction(const char* strName)
{
	BEGIN_RUNFUNCTION(strName, void() );
	int error = lua_pcall(m_luaState, 0, 0, 0);
	END_RUNFUNCTION(void() );
}

void ScriptEngine::runFunction(const char* strName, void* userData, const char* strTypeName)
{
	BEGIN_RUNFUNCTION(strName, void() );
	
	tolua_pushusertype(m_luaState,userData, strTypeName);
	int error = lua_pcall(m_luaState, 1, 0, 0);
	END_RUNFUNCTION(void() );
}

void ScriptEngine::runFunction(const char* strName, void* u1, const char* t1, void* u2, const char* t2)
{
	BEGIN_RUNFUNCTION(strName, void() );
	
	tolua_pushusertype(m_luaState,u1, t1);
	tolua_pushusertype(m_luaState,u2, t2);
	int error = lua_pcall(m_luaState, 2, 0, 0);
	END_RUNFUNCTION(void() );
}

void ScriptEngine::runFunction(const char* strName, void*u1, const char*t1, void*u2, const char*t2, void*u3, const char*t3)
{
	BEGIN_RUNFUNCTION(strName, void() );
	
	tolua_pushusertype(m_luaState,u1, t1);
	tolua_pushusertype(m_luaState,u2, t2);
	tolua_pushusertype(m_luaState,u3, t3);
	int error = lua_pcall(m_luaState, 3, 0, 0);
	END_RUNFUNCTION(void() );
}


std::string ScriptEngine::runFunction_String(const char* strName)
{
	BEGIN_RUNFUNCTION(strName, "");

	int error = lua_pcall(m_luaState, 0, 1, 0);
	
	END_STRINGRET_FUNCTION();
}

std::string ScriptEngine::runFunction_String(const char* strName, void* userData, const char* strTypeName)
{
	BEGIN_RUNFUNCTION(strName, "");
	
	tolua_pushusertype(m_luaState,userData, strTypeName);
	int error = lua_pcall(m_luaState, 1, 1, 0);
	
	END_STRINGRET_FUNCTION();
}

std::string ScriptEngine::runFunction_String(const char* strName, void* u1, const char* t1, void* u2, const char* t2)
{
	BEGIN_RUNFUNCTION(strName, "");
	tolua_pushusertype(m_luaState,u1, t1);
	tolua_pushusertype(m_luaState,u2, t2);

	int error = lua_pcall(m_luaState, 2, 1, 0);
	
	END_STRINGRET_FUNCTION();
}

bool ScriptEngine::runFunction_Bool(const char* strName)
{
	BEGIN_RUNFUNCTION(strName, false);
	int error = lua_pcall(m_luaState, 0, 1, 0);

	END_BOOLRET_FUNCTION();
}

bool ScriptEngine::runFunction_Bool(const char* strName, void* userData, const char* strTypeName)
{
	BEGIN_RUNFUNCTION(strName, false);
	
	tolua_pushusertype(m_luaState,userData, strTypeName);
	int error = lua_pcall(m_luaState, 1, 1, 0);

	END_BOOLRET_FUNCTION();
}

bool ScriptEngine::runFunction_Bool(const char* strName, void* u1, const char* t1, void* u2, const char* t2)
{
	BEGIN_RUNFUNCTION(strName, false);
	
	tolua_pushusertype(m_luaState,u1, t1);
	tolua_pushusertype(m_luaState,u2, t2);
	
	int error = lua_pcall(m_luaState, 1, 1, 0);

	END_BOOLRET_FUNCTION();
}

bool ScriptEngine::runFunction_Bool(const char* strName, void*u1, const char*t1, void*u2, const char*t2, void*u3, const char*t3)
{
	BEGIN_RUNFUNCTION(strName, false);
	
	tolua_pushusertype(m_luaState,u1, t1);
	tolua_pushusertype(m_luaState,u2, t2);
	tolua_pushusertype(m_luaState,u3, t3);
	
	int error = lua_pcall(m_luaState, 1, 1, 0);

	END_BOOLRET_FUNCTION();
}

int ScriptEngine::runFunction_Int(const char* strName)
{
	BEGIN_RUNFUNCTION(strName, 0);
	int error = lua_pcall(m_luaState, 0, 1, 0);

	END_INTRET_FUNCTION();
}

int ScriptEngine::runFunction_Int(const char* strName, void* userData, const char* strTypeName)
{
	BEGIN_RUNFUNCTION(strName, 0);
	
	tolua_pushusertype(m_luaState,userData, strTypeName);
	int error = lua_pcall(m_luaState, 1, 1, 0);

	END_INTRET_FUNCTION();
}

int ScriptEngine::runFunction_Int(const char* strName, void* u1, const char* t1, void* u2, const char* t2)
{
	BEGIN_RUNFUNCTION(strName, 0);
	
	tolua_pushusertype(m_luaState,u1, t1);
	tolua_pushusertype(m_luaState,u2, t2);
	
	int error = lua_pcall(m_luaState, 1, 1, 0);

	END_INTRET_FUNCTION();
}

int ScriptEngine::runFunction_Int(const char* strName, void*u1, const char*t1, void*u2, const char*t2, void*u3, const char*t3)
{
	BEGIN_RUNFUNCTION(strName, 0);
	
	tolua_pushusertype(m_luaState,u1, t1);
	tolua_pushusertype(m_luaState,u2, t2);
	tolua_pushusertype(m_luaState,u3, t3);
	
	int error = lua_pcall(m_luaState, 1, 1, 0);

	END_INTRET_FUNCTION();
}


Uint32 ScriptEngine::GetScriptUseMem()
{
	if(!m_luaState) return 0;

	global_State *g = G(m_luaState);
	if(!g) return 0;

	Uint32 dwTotalBytes = (Uint32)g->totalbytes;

	return dwTotalBytes;
}

void ScriptEngine::runFile(const char* strFileName)
{
	LuaStackHelper helper(m_luaState);

	FILE* pFile = fopen(strFileName, "rb");
	if(!pFile)
	{
		FDLOG("Script")<<"ScriptEngine::runFile Error Open File:["<<strFileName<<"]Error"<<endl;
		return;
	}

	size_t fileLen = 0;
	fseek(pFile, 0, SEEK_END);
	fileLen = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	char* pFileBuff = new char[fileLen + 1];
	size_t rdLen = fread(pFileBuff, 1, fileLen, pFile);
	fclose(pFile);

	if(!rdLen)
	{
		FDLOG("Script")<<"open file["<<strFileName<<"] Fail"<<endl;
		return;
	}

	pFileBuff[rdLen] = '\0';

	int loaderr = luaL_loadbuffer(m_luaState, (char*)pFileBuff, rdLen, strFileName);
	delete []pFileBuff;
	if(loaderr)
	{
		std::string errMsg = lua_tostring(m_luaState,-1);
		FDLOG("Script")<<"runFile:["<<strFileName<<"] Fail:"<<errMsg.c_str()<<endl;
		return;
	}

	if (lua_pcall(m_luaState,0,0,0))
	{
		std::string errMsg = lua_tostring(m_luaState,-1);
		FDLOG("Script")<<"runFile:["<<strFileName<<"] Error:"<<errMsg.c_str()<<endl;
		return;
	}
}
