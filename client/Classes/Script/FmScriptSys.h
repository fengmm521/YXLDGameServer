/********************************************************************
created:	2012-06-04
author:		Fish (于国平)
summary:	脚本系统
*********************************************************************/
#pragma once

#include "FmNet.h"
extern "C" {
#include "lua.h"
#include "tolua_fix.h"
}

NS_FM_BEGIN

class Player;

class ScriptSys
{
public:
	static uint s_RobetId;
protected:
	typedef UNORDERED_MAP< int, string >	ScriptFuncMap;
	ScriptFuncMap	m_ScriptFuncs;
	string			m_ScriptPath;			// 脚本目录
public:
	ScriptSys();
	~ScriptSys();

	SINGLETON_MODE( ScriptSys );

	void Init();

	// 注册脚本消息回调
	// 回调脚本函数格式: function OnPacket_XXXX( packetArg )
	void RegisterScriptFunc( int packetId, const char* scriptFunc );
	// 取消注册脚本消息回调
	void UnRegisterScriptFunc( int packetId );
	// 处理消息
	bool DispatchPacket( int iCmd, const string& pkg );

	// 获取lua函数的映射id
	// 参数1: lua函数
	static int GetLuaFuncId( lua_State* L );

	// 加载lua脚本文件
	// 参数1: lua文件名
	static int L_LoadFile( lua_State* L );
	static bool LoadFile( const char* luafile );

	static int ExecuteBuffer( lua_State* L, const char *buf, size_t size);

	// 记日志
	static int L_LogFile( lua_State* L );
	static int L_IsDebug( lua_State* L );
	static int L_GetPlatform( lua_State* L );

	bool Execute( const char* funcName );

	bool ExecuteString( const char* str );

	// 执行lua函数,带一个参数
	bool Execute_1( const char* funcName, void* value, const char* type );

	// 执行lua函数,带2个参数
	bool Execute_2( const char* funcName, void* value, const char* type, void* value2, const char* type2 );

	// 执行lua函数,带3个参数
	bool Execute_3( const char* funcName, void* value, const char* type, void* value2, const char* type2, void* value3, const char* type3 );

	bool Execute_4(const char* funcName, 
				   void* value, const char* type, 
				   void* value2, const char* type2, 
				   void* value3, const char* type3,
				   void* value4, const char* type4);

	static void ExecuteSchedulerScriptHandler( const char* script, Entity* entity, ScheduleFunctor* functor, uint delta );
	void ExecuteSchedulerScript( const char* script, Entity* entity, ScheduleFunctor* functor, uint delta );

	// 32位整数的位操作
	static int L_ByteOp( lua_State* L );
	bool Execute_number( const char* funcName, int number );
	bool Execute_number( const char* funcName, int number1, int number2 );
	bool Execute_number( const char* funcName, int number1, int number2, int number3 );
};

NS_FM_END