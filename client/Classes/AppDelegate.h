#ifndef __APP_DELEGATE_H__
#define __APP_DELEGATE_H__

// 客户端宏
#define _CLIENT

#include "cocos2d.h"
#include "plugins/CCDirectorPlugin.h"
#include "NetWork/SocketSystem.h"
#include "NetWork/ClientSocket.h"
#include "cocoa/CCObject.h"
#include "CocoStudio/GUI/BaseClasses/UIWidget.h"


using namespace cocos2d;
using namespace cocos2d::gui;
/**
@brief    The cocos2d Application.

The reason for implement as private inheritance is to hide some interface call by CCDirector.
*/
class  AppDelegate : private cocos2d::CCApplication, public cocos2d::CCDirectorPlugin, public cocos2d::CCObject
{
public:
    AppDelegate();
    virtual ~AppDelegate();

    /**
    @brief    Implement CCDirector and CCScene init code here.
    @return true    Initialize success, app continue.
    @return false   Initialize failed, app terminate.
    */
    virtual bool applicationDidFinishLaunching();

    /**
    @brief  The function be called when the application enter background
    @param  the pointer of the application
    */
    virtual void applicationDidEnterBackground();

    /**
    @brief  The function be called when the application enter foreground
    @param  the pointer of the application
    */
    virtual void applicationWillEnterForeground();


	// CCDisplayLinkDirector调用drawScene之前调用
	virtual void BeforeDrawScene();

	// CCDisplayLinkDirector调用drawScene之后调用
	virtual void AfterDrawScene();

	// 主循环更新(挂起状态也会执行)
	virtual void MainLoop();

	/**
	 @ 
	 */
	virtual void onJavaCommand(const char* json);

	void touchCallBack(CCObject *pSender, TouchEventType type);
	void touchCallBack2(CCObject *pSender, TouchEventType type);
};

#endif  // __APP_DELEGATE_H__

