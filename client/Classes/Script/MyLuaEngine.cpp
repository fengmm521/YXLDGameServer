/****************************************************************************
 Copyright (c) 2011 cocos2d-x.org

 http://www.cocos2d-x.org

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "MyLuaEngine.h"
#include "cocos2d.h"
#include "cocoa/CCArray.h"
#include "CCScheduler.h"
#include "FmTypeConvert.h"

USING_NS_FM;
NS_CC_BEGIN

CCLuaEngine* MyLuaEngine::Create(void)
{
    if (!m_defaultEngine)
    {
        CCLuaEngine::m_defaultEngine = new MyLuaEngine();
		CCLuaEngine::defaultEngine()->init();
    }
    return CCLuaEngine::m_defaultEngine;
}

MyLuaEngine::~MyLuaEngine(void)
{
	lua_close( CCLuaEngine::defaultEngine()->getLuaStack()->getLuaState());
}

int MyLuaEngine::executeFunctionByHandler_1int(int nHandler, int arg )
{
	int ret = 0;
	do 
	{
		int nScriptHandler = nHandler;
		CC_BREAK_IF(0 == nScriptHandler);

		m_stack->pushInt( arg );
		ret = m_stack->executeFunctionByHandler(nScriptHandler, 1);
	} while (0);
	return ret;
}
/*
int MyLuaEngine::executeGridCellHandle( GridCell* host,const GridContainer* container,int eventtype,int x1,int y1, int x2,int y2 )
{
	int ret = 0;
	do 
	{
		int nScriptHandler = host->getScriptHandler();
		CC_BREAK_IF(0 == nScriptHandler);

		//lua_newtable(m_state);
		//pushCCObject((CCObject*)host, "GridCell");
		//pushCCObject((CCObject*)container, "GridContainer");
		lua_pushnumber(m_stack->getLuaState(), host->getTag());
		lua_pushnumber(m_stack->getLuaState(), eventtype);
		lua_pushnumber(m_stack->getLuaState(), x1);
		lua_pushnumber(m_stack->getLuaState(), y1);
		lua_pushnumber(m_stack->getLuaState(), x2);
		lua_pushnumber(m_stack->getLuaState(), y2);
		ret = m_stack->executeFunctionByHandler(nScriptHandler, 6);
	} while (0);

	return ret;
}
*/
int MyLuaEngine::executeAttackShowHandle(int nHandler, uint64 casterEntityId, uint64 targetEntityId, int damageValue, int damageFlag, int skillID , bool isLive, bool isSrcTarget)
{
	int ret = 0;
	do 
	{
		CC_BREAK_IF(0 == nHandler);

		//lua_newtable(m_state);
		//pushCCObject((CCObject*)host, "GridCell");
		//pushCCObject((CCObject*)container, "GridContainer");
		m_stack->pushString(TypeConvert::ToString(casterEntityId).c_str());
		m_stack->pushString(TypeConvert::ToString(targetEntityId).c_str());
		lua_pushnumber(m_stack->getLuaState(), damageValue);
		lua_pushnumber(m_stack->getLuaState(), damageFlag);
		lua_pushnumber(m_stack->getLuaState(), skillID);
		lua_pushboolean(m_stack->getLuaState(), isLive);
		lua_pushboolean(m_stack->getLuaState(), isSrcTarget);
		ret = m_stack->executeFunctionByHandler(nHandler, 7);
	} while (0);

	return ret;
}


NS_CC_END
