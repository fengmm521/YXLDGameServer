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

#ifndef __CC_MY_LUA_ENGINE_H__
#define __CC_MY_LUA_ENGINE_H__

extern "C" {
#include "lua.h"
}
#include "FmPlatform.h"
#include "CCLuaEngine.h"

NS_CC_BEGIN

// Lua support for cocos2d-x
class MyLuaEngine : public CCLuaEngine
{
public:
    static CCLuaEngine* Create(void);    
    virtual ~MyLuaEngine(void);

	virtual int executeFunctionByHandler_1int(int nHandler, int arg );

	//int executeGridCellHandle(GridCell* host,const GridContainer* container,int eventtype,int x1,int y1, int x2,int y2);

	int executeAttackShowHandle(int nHandler, uint64 casterEntityId, uint64 targetEntityId, int damageValue, int damageFlag, int skillID, bool isLive, bool isSrcTarget);

	
};

NS_CC_END

#endif // __CC_LUA_ENGINE_H__
