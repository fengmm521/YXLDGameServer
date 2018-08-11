#ifndef __SCRIPT_ENGINE_H__
#define __SCRIPT_ENGINE_H__

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "tolua++.h"

#include "ScriptEngine/IScriptEngine.h"
#include "Component/ComponentBase.h"

struct lua_State;

BEGIN_MINIAPR_NAMESPACE
	
class ScriptEngine:public ComponentBase<IScriptEngine, IID_IScriptEngine>
{
public:

	// Constructor/Destructor
	ScriptEngine();
	~ScriptEngine();

	// IComponent Interface
	bool initlize(const PropertySet& propSet);

	// IScriptEngine Interface
	void bindState(lua_State* L);
	lua_State* getLuaState();
	void runFunction(const char* strName);
	void runFunction(const char* strName, void* userData, const char* strTypeName);
	void runFunction(const char* strName, void*, const char*, void*, const char*);
	void runFunction(const char* strName, void*, const char*, void*, const char*, void*, const char*);
	
	std::string runFunction_String(const char* strName);
	std::string runFunction_String(const char* strName, void* userData, const char* strTypeName);
	std::string runFunction_String(const char* strName, void*, const char*, void*, const char*);

	bool runFunction_Bool(const char* strName);
	bool runFunction_Bool(const char* strName, void* userData, const char* strTypeName);
	bool runFunction_Bool(const char* strName,  void*, const char*, void*, const char*);
	bool runFunction_Bool(const char* strName, void*, const char*, void*, const char*, void*, const char*);

	int runFunction_Int(const char* strName);
	int runFunction_Int(const char* strName, void* userData, const char* strTypeName);
	int runFunction_Int(const char* strName,	void*, const char*, void*, const char*);
	int runFunction_Int(const char* strName, void*, const char*, void*, const char*, void*, const char*);
	
	void runFile(const char* strFileName);

	Uint32 GetScriptUseMem();

private:
	lua_State*	m_luaState;
	bool		m_bOwn;
};

END_MINIAPR_NAMESPACE

#endif

