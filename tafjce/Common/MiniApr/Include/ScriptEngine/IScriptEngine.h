#ifndef __ISCRIPT_ENGINE_H__
#define __ISCRIPT_ENGINE_H__

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "tolua++.h"

#define IID_IScriptEngine	MAKE_RID('s','c', 'e', 'g')

BEGIN_MINIAPR_NAMESPACE

class IScriptEngine:public IComponent
{
public:

	virtual void bindState(lua_State* L) = 0;
	
	virtual lua_State* getLuaState() = 0;

	// void
	virtual void runFunction(const char* strName) = 0;
	virtual void runFunction(const char* strName, void* userData, const char* strTypeName) = 0;
	virtual void runFunction(const char* strName, void*, const char*, void*, const char*) = 0;
	virtual void runFunction(const char* strName, void*, const char*, void*, const char*, void*, const char*) = 0;

	// string
	virtual std::string runFunction_String(const char* strName) = 0;
	virtual std::string runFunction_String(const char* strName, void* userData, const char* strTypeName) = 0;
	virtual std::string runFunction_String(const char* strName, void*, const char*, void*, const char*) = 0;

	// bool
	virtual bool runFunction_Bool(const char* strName) = 0;
	virtual bool runFunction_Bool(const char* strName, void* userData, const char* strTypeName) = 0;
	virtual bool runFunction_Bool(const char* strName,  void*, const char*, void*, const char*) = 0;
	virtual bool runFunction_Bool(const char* strName, void*, const char*, void*, const char*, void*, const char*) = 0;

	// int
	virtual int runFunction_Int(const char* strName) = 0;
	virtual int runFunction_Int(const char* strName, void* userData, const char* strTypeName) = 0;
	virtual int runFunction_Int(const char* strName,	void*, const char*, void*, const char*) = 0;
	virtual int runFunction_Int(const char* strName, void*, const char*, void*, const char*, void*, const char*) = 0;
	
	virtual void runFile(const char* strFileName) = 0;

	virtual Uint32 GetScriptUseMem() = 0;
	
};

END_MINIAPR_NAMESPACE

#endif
