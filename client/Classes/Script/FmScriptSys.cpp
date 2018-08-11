#include "FmScriptSys.h"
#include "CCLuaEngine.h"
#include "tolua++.h"
#include "cocos2d.h"
#include "Asset/Encrypt.h"

USING_NS_CC;

NS_FM_BEGIN

uint ScriptSys::s_RobetId = 0;


ScriptSys::ScriptSys()
{
	m_ScriptPath = "Script/";
	LogMgr::GetInstance().AddLog( "Script.log" );
}

ScriptSys::~ScriptSys()
{

}

void ScriptSys::Init()
{
	CCLuaEngine* pEngine = (CCLuaEngine*)CCScriptEngineManager::sharedManager()->getScriptEngine();
	lua_State* L = pEngine->getLuaStack()->getLuaState();

	lua_register( L, "GetLuaFuncId", ScriptSys::GetLuaFuncId );
	lua_register( L, "LoadFile", ScriptSys::L_LoadFile );
	lua_register( L, "LogFile", ScriptSys::L_LogFile );
	lua_register( L, "IsDebug", ScriptSys::L_IsDebug );
	lua_register( L, "ByteOp", ScriptSys::L_ByteOp );
	lua_register( L, "GetPlatform", ScriptSys::L_GetPlatform );

	Scheduler::SetExecuteSchedulerScriptHandler( ScriptSys::ExecuteSchedulerScriptHandler );
}

void ScriptSys::RegisterScriptFunc( int packetId, const char* scriptFunc )
{
	if( !scriptFunc )
		return;
	m_ScriptFuncs[packetId] = scriptFunc;
}

void ScriptSys::UnRegisterScriptFunc( int packetId )
{
	ScriptFuncMap::iterator it = m_ScriptFuncs.find( packetId );
	if( it != m_ScriptFuncs.end() )
	{
		m_ScriptFuncs.erase( it );
	}
}

bool ScriptSys::DispatchPacket( int iCmd, const string& pkg )
{
	ScriptFuncMap::iterator it = m_ScriptFuncs.find( iCmd );
	if( it != m_ScriptFuncs.end() )
	{
		do 
		{
			const char* funcName = (it->second).c_str();
			//Execute_1(funcName, &pkg, "string");

			CCLuaEngine* pEngine = (CCLuaEngine*)CCScriptEngineManager::sharedManager()->getScriptEngine();
			lua_State* L = pEngine->getLuaStack()->getLuaState();
			int oldStackPos = lua_gettop( L );
			lua_getglobal( L, funcName );
			if ( lua_isnil( L, -1 ) )
			{
				lua_pop( L, 1 );
				CCLOG("[LUA ERROR] not find function %s", funcName );
				LogMgr::GetInstance().LogFile( "Script.log", "[LUA ERROR] not find function %s", funcName );
				return false;
			}
			if ( lua_isfunction( L, -1 ) == false )
			{
				lua_pop( L, 1 );
				CCLOG("[LUA ERROR] not a function %s", funcName );
				LogMgr::GetInstance().LogFile( "Script.log", "[LUA ERROR] not a function %s", funcName );
				return false;
			}
			int stackPos = oldStackPos + 1;

			lua_pushlstring( L, pkg.c_str(), pkg.size() );

			if ( lua_pcall( L, lua_gettop( L )-stackPos, 0, 0 ) != 0 )
			{
				const char* str = lua_tostring( L, -1 );
				if( str != NULL )
				{
					CCLog( "lua_pcall: %s", str );
					LogMgr::GetInstance().LogFile( "Script.log", "lua_pcall: %s", str );
				}
				lua_settop( L, stackPos-1 );
				return false;
			}

			return true;
		} while (0);

		return true;
	}
	return false;
}

int ScriptSys::GetLuaFuncId( lua_State* L )
{
	LUA_FUNCTION funcID = (  toluafix_ref_function(L,1,0));
	lua_pushnumber( L, funcID );
	return 1;
}

int ScriptSys::L_LoadFile( lua_State* L )
{
	if( lua_gettop( L ) != 1 )
		return 0;
	const char* fileName = lua_tostring( L, 1 );
	lua_pushboolean( L, LoadFile( fileName ) );
	return 1;
}

bool ScriptSys::LoadFile( const char* luafile )
{
	Assert( luafile );
	if( luafile == 0 )
		return false;

/*#ifndef SCRIPT_NOT_ENCRYPT
	uint dataSize = 0; 
	std::string path = CCFileUtils::sharedFileUtils()->fullPathForFilename((ScriptSys::GetInstance().m_ScriptPath + luafile).c_str());
	uint8* buffer = Encrypt::GetFileData( path.c_str(), "rb", &dataSize );
	if( buffer == NULL )
	{
		return false;
	}
	CCLuaEngine* pEngine = (CCLuaEngine*)CCScriptEngineManager::sharedManager()->getScriptEngine();
	int result = ExecuteBuffer( pEngine->getLuaStack()->getLuaState(), (const char*)buffer, dataSize );
	return (result == 0);
#else*/
	int result = -1;
	CCLuaEngine* pEngine = (CCLuaEngine*)CCScriptEngineManager::sharedManager()->getScriptEngine();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	CCString* pstrFileContent = CCString::createWithContentsOfFile((ScriptSys::GetInstance().m_ScriptPath + luafile).c_str());
	if (pstrFileContent)
	{
		result = pEngine->executeString(pstrFileContent->getCString());
	}
#else
	std::string path = CCFileUtils::sharedFileUtils()->fullPathForFilename((ScriptSys::GetInstance().m_ScriptPath + luafile).c_str());
	result = pEngine->executeScriptFile(path.c_str());
#endif
	if( result != 0 )
	{
		LogMgr::GetInstance().LogFile( "Script.log", "LoadFile error : %s", luafile );
	}
	return (result == 0);
//#endif
}

int ScriptSys::ExecuteBuffer( lua_State* L, const char *buf, size_t size)
{
	int ret = luaL_loadbuffer( L, buf, size, NULL );
	if ( ret != 0 )
	{
		const char* str = lua_tostring( L, -1 );
		if ( str )
		{
			CCLog( "ExecuteBuffer luaL_loadbuffer: %s", str );
			lua_pop( L, 1 );
		}
		return ret;
	}

	ret = lua_pcall( L, 0, LUA_MULTRET, 0 );
	if ( ret != 0 )
	{
		const char* str = lua_tostring( L, -1 );
		if ( str )
		{
			CCLog( "ExecuteBuffer lua_pcall: %s", str );
			lua_pop( L, 1 );
		}
	}

	return ret;
}

int ScriptSys::L_LogFile( lua_State* L )
{
	int argCount = lua_gettop( L );
	if( argCount < 1 || argCount > 2 )
	{
		Assert( false );
		return 0;
	}
	const char* fileName = NULL;
	const char* msg = NULL;
	if( argCount == 1 )
	{
		msg = lua_tostring( L, 1 );
		LogMgr::GetInstance().LogMsg( msg );
	}
	else if( argCount == 2 )
	{
		fileName = lua_tostring( L, 1 );
		msg = lua_tostring( L, 2 );
		if( fileName && msg )
			LogMgr::GetInstance().LogFile( fileName, msg );
	}
	return 0;
}

int ScriptSys::L_IsDebug( lua_State* L )
{
#ifdef _DEBUG
	lua_pushboolean( L, 1 );
	return 1;
#else
	lua_pushboolean( L, 0 );
	return 1;
#endif
}

int ScriptSys::L_GetPlatform( lua_State* L )
{
	lua_pushnumber( L, CC_TARGET_PLATFORM );
	return 1;
}

bool ScriptSys::Execute_number( const char* funcName, int number )
{
	CCLuaEngine* pEngine = (CCLuaEngine*)CCScriptEngineManager::sharedManager()->getScriptEngine();
	lua_State* L = pEngine->getLuaStack()->getLuaState();
	int oldStackPos = lua_gettop( L );
	lua_getglobal( L, funcName );
	if ( lua_isnil( L, -1 ) )
	{
		lua_pop( L, 1 );
		CCLOG("[LUA ERROR] not find function %s", funcName );
		LogMgr::GetInstance().LogFile( "Script.log", "[LUA ERROR] not find function %s", funcName );
		return false;
	}
	if ( lua_isfunction( L, -1 ) == false )
	{
		lua_pop( L, 1 );
		CCLOG("[LUA ERROR] not a function %s", funcName );
		LogMgr::GetInstance().LogFile( "Script.log", "[LUA ERROR] not a function %s", funcName );
		return false;
	}
	int stackPos = oldStackPos + 1;

	tolua_pushnumber( L, number );

	if ( lua_pcall( L, lua_gettop( L )-stackPos, 0, 0 ) != 0 )
	{
		const char* str = lua_tostring( L, -1 );
		if( str != NULL )
		{
			CCLog( "lua_pcall: %s", str );
			LogMgr::GetInstance().LogFile( "Script.log", "lua_pcall: %s", str );
		}
		lua_settop( L, stackPos-1 );
		return false;
	}

	return true;
}

bool ScriptSys::Execute_number( const char* funcName, int number1, int number2 )
{
	CCLuaEngine* pEngine = (CCLuaEngine*)CCScriptEngineManager::sharedManager()->getScriptEngine();
	lua_State* L = pEngine->getLuaStack()->getLuaState();
	int oldStackPos = lua_gettop( L );
	lua_getglobal( L, funcName );
	if ( lua_isnil( L, -1 ) )
	{
		lua_pop( L, 1 );
		CCLOG("[LUA ERROR] not find function %s", funcName );
		LogMgr::GetInstance().LogFile( "Script.log", "[LUA ERROR] not find function %s", funcName );
		return false;
	}
	if ( lua_isfunction( L, -1 ) == false )
	{
		lua_pop( L, 1 );
		CCLOG("[LUA ERROR] not a function %s", funcName );
		LogMgr::GetInstance().LogFile( "Script.log", "[LUA ERROR] not a function %s", funcName );
		return false;
	}
	int stackPos = oldStackPos + 1;

	tolua_pushnumber( L, number1 );
	tolua_pushnumber( L, number2 );

	if ( lua_pcall( L, lua_gettop( L )-stackPos, 0, 0 ) != 0 )
	{
		const char* str = lua_tostring( L, -1 );
		if( str != NULL )
		{
			CCLog( "lua_pcall: %s", str );
			LogMgr::GetInstance().LogFile( "Script.log", "lua_pcall: %s", str );
		}
		lua_settop( L, stackPos-1 );
		return false;
	}

	return true;
}

bool ScriptSys::Execute_number( const char* funcName, int number1, int number2, int number3 )
{
	CCLuaEngine* pEngine = (CCLuaEngine*)CCScriptEngineManager::sharedManager()->getScriptEngine();
	lua_State* L = pEngine->getLuaStack()->getLuaState();
	int oldStackPos = lua_gettop( L );
	lua_getglobal( L, funcName );
	if ( lua_isnil( L, -1 ) )
	{
		lua_pop( L, 1 );
		CCLOG("[LUA ERROR] not find function %s", funcName );
		LogMgr::GetInstance().LogFile( "Script.log", "[LUA ERROR] not find function %s", funcName );
		return false;
	}
	if ( lua_isfunction( L, -1 ) == false )
	{
		lua_pop( L, 1 );
		CCLOG("[LUA ERROR] not a function %s", funcName );
		LogMgr::GetInstance().LogFile( "Script.log", "[LUA ERROR] not a function %s", funcName );
		return false;
	}
	int stackPos = oldStackPos + 1;

	tolua_pushnumber( L, number1 );
	tolua_pushnumber( L, number2 );
	tolua_pushnumber( L, number3 );

	if ( lua_pcall( L, lua_gettop( L )-stackPos, 0, 0 ) != 0 )
	{
		const char* str = lua_tostring( L, -1 );
		if( str != NULL )
		{
			CCLog( "lua_pcall: %s", str );
			LogMgr::GetInstance().LogFile( "Script.log", "lua_pcall: %s", str );
		}
		lua_settop( L, stackPos-1 );
		return false;
	}

	return true;
}

bool ScriptSys::Execute( const char* funcName )
{
	if( !funcName )
		return false;
    
    if(strcmp(funcName, "Script_Main") == 0){
        CCLOG("Script_Main invoked.");
    }
    
	CCLuaEngine* pEngine = (CCLuaEngine*)CCScriptEngineManager::sharedManager()->getScriptEngine();
	lua_State* L = pEngine->getLuaStack()->getLuaState();
	int oldStackPos = lua_gettop( L );
	lua_getglobal( L, funcName );
	if ( lua_isnil( L, -1 ) )
	{
		lua_pop( L, 1 );
		CCLOG("[LUA ERROR] name '%s' does not exist", funcName );
		LogMgr::GetInstance().LogFile( "Script.log", "[LUA ERROR] name '%s' does not exist", funcName );
		return false;
	}
	if ( lua_isfunction( L, -1 ) == false )
	{
		lua_pop( L, 1 );
		CCLOG("[LUA ERROR] name '%s' not a Lua function", funcName);
		LogMgr::GetInstance().LogFile( "Script.log", "[LUA ERROR] name '%s' not a Lua function", funcName );
		return false;
	}
	int stackPos = oldStackPos + 1;

	if ( lua_pcall( L, lua_gettop( L )-stackPos, 0, 0 ) != 0 )
	{
		const char* str = lua_tostring( L, -1 );
		if( str != NULL )
		{
			CCLog( "lua_pcall: %s", str );
			LogMgr::GetInstance().LogFile( "Script.log", "lua_pcall: %s", str );
		}
		lua_settop( L, stackPos-1 );
		return false;
	}

	return true;
}

bool ScriptSys::ExecuteString( const char* str )
{
	if( str == NULL )
		return false;
	CCLuaEngine* pEngine = (CCLuaEngine*)CCScriptEngineManager::sharedManager()->getScriptEngine();
	if( pEngine->executeString( str ) != 0 )
	{
		LogMgr::GetInstance().LogFile( "Script.log", "ExecuteString err: %s", str );
		return false;
	}
	return true;
}

bool ScriptSys::Execute_1( const char* funcName, void* value, const char* type )
{
	if( !funcName || !type )
		return false;
	CCLuaEngine* pEngine = (CCLuaEngine*)CCScriptEngineManager::sharedManager()->getScriptEngine();
	lua_State* L = pEngine->getLuaStack()->getLuaState();
	int oldStackPos = lua_gettop( L );
	lua_getglobal( L, funcName );
	if ( lua_isnil( L, -1 ) )
	{
		lua_pop( L, 1 );
		CCLOG("[LUA ERROR] not find function %s", funcName );
		LogMgr::GetInstance().LogFile( "Script.log", "[LUA ERROR] not find function %s", funcName );
		return false;
	}
	if ( lua_isfunction( L, -1 ) == false )
	{
		lua_pop( L, 1 );
		CCLOG("[LUA ERROR] not a function %s", funcName );
		LogMgr::GetInstance().LogFile( "Script.log", "[LUA ERROR] not a function %s", funcName );
		return false;
	}
	int stackPos = oldStackPos + 1;

	tolua_pushusertype( L, value, type );

	if ( lua_pcall( L, lua_gettop( L )-stackPos, 0, 0 ) != 0 )
	{
		const char* str = lua_tostring( L, -1 );
		if( str != NULL )
		{
			CCLog( "lua_pcall: %s", str );
			LogMgr::GetInstance().LogFile( "Script.log", "lua_pcall: %s", str );
		}
		lua_settop( L, stackPos-1 );
		return false;
	}

	return true;
}

bool ScriptSys::Execute_2( const char* funcName, void* value, const char* type, void* value2, const char* type2 )
{
	if( !funcName || !type || !type2 )
		return false;
	CCLuaEngine* pEngine = (CCLuaEngine*)CCScriptEngineManager::sharedManager()->getScriptEngine();
	lua_State* L = pEngine->getLuaStack()->getLuaState();
	int oldStackPos = lua_gettop( L );
	lua_getglobal( L, funcName );
	if ( lua_isnil( L, -1 ) )
	{
		lua_pop( L, 1 );
		CCLOG("[LUA ERROR] not find function %s", funcName );
		LogMgr::GetInstance().LogFile( "Script.log", "[LUA ERROR] not find function %s", funcName );
		return false;
	}
	if ( lua_isfunction( L, -1 ) == false )
	{
		lua_pop( L, 1 );
		CCLOG("[LUA ERROR] not a function %s", funcName );
		LogMgr::GetInstance().LogFile( "Script.log", "[LUA ERROR] not a function %s", funcName );
		return false;
	}
	int stackPos = oldStackPos + 1;

	tolua_pushusertype( L, value, type );
	tolua_pushusertype( L, value2, type2 );

	if ( lua_pcall( L, lua_gettop( L )-stackPos, 0, 0 ) != 0 )
	{
		const char* str = lua_tostring( L, -1 );
		if( str != NULL )
		{
			CCLog( "lua_pcall: %s", str );
			LogMgr::GetInstance().LogFile( "Script.log", "lua_pcall: %s", str );
		}
		lua_settop( L, stackPos-1 );
		return false;
	}

	return true;
}

bool ScriptSys::Execute_3( const char* funcName, void* value, const char* type, void* value2, const char* type2, void* value3, const char* type3 )
{
	if( !funcName || !type  || !type2 || !type3 )
		return false;
	CCLuaEngine* pEngine = (CCLuaEngine*)CCScriptEngineManager::sharedManager()->getScriptEngine();
	lua_State* L = pEngine->getLuaStack()->getLuaState();
	int oldStackPos = lua_gettop( L );
	lua_getglobal( L, funcName );
	if ( lua_isnil( L, -1 ) )
	{
		lua_pop( L, 1 );
		CCLOG("[LUA ERROR] not find function %s", funcName );
		return false;
	}
	if ( lua_isfunction( L, -1 ) == false )
	{
		lua_pop( L, 1 );
		CCLOG("[LUA ERROR] not a function %s", funcName );
		return false;
	}
	int stackPos = oldStackPos + 1;

	tolua_pushusertype( L, value, type );
	tolua_pushusertype( L, value2, type2 );
	tolua_pushusertype( L, value3, type3 );

	if ( lua_pcall( L, lua_gettop( L )-stackPos, 0, 0 ) != 0 )
	{
		const char* str = lua_tostring( L, -1 );
		if( str != NULL )
		{
			CCLog( "lua_pcall: %s", str );
			LogMgr::GetInstance().LogFile( "Script.log", "lua_pcall: %s", str );
		}
		lua_settop( L, stackPos-1 );
		return false;
	}

	return true;
}

bool ScriptSys::Execute_4(const char* funcName,
						  void* value, const char* type,
						  void* value2, const char* type2,
						  void* value3, const char* type3,
						  void* value4, const char* type4)
{
	if (!funcName || !type || !type2 || !type3 || type4)
		return false;
	CCLuaEngine* pEngine = (CCLuaEngine*)CCScriptEngineManager::sharedManager()->getScriptEngine();
	lua_State* L = pEngine->getLuaStack()->getLuaState();
	int oldStackPos = lua_gettop(L);
	lua_getglobal(L, funcName);
	if (lua_isnil(L, -1))
	{
		lua_pop(L, 1);
		CCLOG("[LUA ERROR] not find function %s", funcName);
		return false;
	}
	if (lua_isfunction(L, -1) == false)
	{
		lua_pop(L, 1);
		CCLOG("[LUA ERROR] not a function %s", funcName);
		return false;
	}
	int stackPos = oldStackPos + 1;

	tolua_pushusertype(L, value, type);
	tolua_pushusertype(L, value2, type2);
	tolua_pushusertype(L, value3, type3);
	tolua_pushusertype(L, value4, type4);

	if (lua_pcall(L, lua_gettop(L) - stackPos, 0, 0) != 0)
	{
		const char* str = lua_tostring(L, -1);
		if (str != NULL)
		{
			CCLog("lua_pcall: %s", str);
			LogMgr::GetInstance().LogFile("Script.log", "lua_pcall: %s", str);
		}
		lua_settop(L, stackPos - 1);
		return false;
	}

	return true;
}

void ScriptSys::ExecuteSchedulerScriptHandler( const char* funcName, Entity* entity, ScheduleFunctor* functor, uint delta )
{
	ScriptSys::GetInstance().ExecuteSchedulerScript( funcName, entity, functor, delta );
}

void ScriptSys::ExecuteSchedulerScript( const char* funcName, Entity* entity, ScheduleFunctor* functor, uint delta )
{
	if( !funcName || !entity || !functor )
		return;

	CCLuaEngine* pEngine = (CCLuaEngine*)CCScriptEngineManager::sharedManager()->getScriptEngine();
	lua_State* L = pEngine->getLuaStack()->getLuaState();
	int oldStackPos = lua_gettop( L );
	lua_getglobal( L, funcName );
	if ( lua_isnil( L, -1 ) )
	{
		lua_pop( L, 1 );
		CCLOG("[LUA ERROR] not find function %s", funcName );
		return;
	}
	if ( lua_isfunction( L, -1 ) == false )
	{
		lua_pop( L, 1 );
		CCLOG("[LUA ERROR] not a function %s", funcName );
		return;
	}
	int stackPos = oldStackPos + 1;

	tolua_pushusertype( L, entity, "Entity" );
	tolua_pushusertype( L, functor, "ScheduleFunctor" );
	tolua_pushnumber( L, delta );
	
	if ( lua_pcall( L, lua_gettop( L )-stackPos, 0, 0 ) != 0 )
	{
		const char* str = lua_tostring( L, -1 );
		if( str != NULL )
		{
			CCLog( "lua_pcall: %s", str );
			LogMgr::GetInstance().LogFile( "Script.log", "lua_pcall: %s", str );
		}
		lua_settop( L, stackPos-1 );
		return;
	}
}

int ScriptSys::L_ByteOp( lua_State* L )
{
	int argCount = lua_gettop( L );
	if( argCount != 3 )
	{
		Assert( false );
		return 0;
	}
	uint a = (uint)lua_tonumber( L, 1 );
	const char* op = lua_tostring( L, 2 );
	uint b = (uint)lua_tonumber( L, 3 );
	if( op == NULL )
	{
		Assert( false );
		return 0;
	}
	uint result = 0;
	if( strcmp( op, "<<" ) == 0 )
	{
		result = (a << b);
	}
	else if( strcmp( op, ">>" ) == 0 )
	{
		result = (a >> b);
	}
	else if( strcmp( op, "|" ) == 0 )
	{
		result = (a | b);
	}
	else if( strcmp( op, "&" ) == 0 )
	{
		result = (a & b);
	}
	lua_pushnumber( L, result );
	return 1;
}



NS_FM_END