#if defined(WIN32) && defined(_WINDOWS)
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#include <winsock2.h>
#include <windows.h>
#endif
#endif

#include "cocos2d.h"
#include "CCEGLView.h"
#include "AppDelegate.h"
#include "FmSysUtil.h"
#include "Script/MyLuaEngine.h"
#include "Script/FmScriptSys.h"
#include "FmIOUtil.h"
//#include "Asset/Launcher.h"
#include "Sound/AudioEngine.h"
#include "Util/FmStringMgr.h"
#include "cocos-ext.h"
#include "CocoStudio/Reader/SceneReader.h"
#include "Util/ustring.h"

#include "NetWork/ClientConect.h"
#include "NetWork/SocketSystem.h"
#include "CocoStudio/GUI/BaseClasses/UIWidget.h"
#include "cocoa/CCObject.h"
#include "Scene/FmMainScene.h"
#include "UI/FmUIMgr.h"

USING_NS_CC;
USING_NS_FM;
USING_NS_CC_EXT;

using namespace cocos2d::gui;

extern "C" int luaopen_pb (lua_State *L);

uint8* Cocos2dGetFileDataAPI( const char* fileName, const char* mode, uint* dataSize )
{
	unsigned long size = 0;
	uint8* pData = CCFileUtils::sharedFileUtils()->getFileData( CCFileUtils::sharedFileUtils()->fullPathForFilename(fileName).c_str(), mode, &size );
	if( dataSize )
		*dataSize = size;
	return pData;
}

AppDelegate::AppDelegate()
{
	IOUtil::s_GetFileDataAPI = Cocos2dGetFileDataAPI;
	Random::SetSeed_Now();
}

AppDelegate::~AppDelegate()
{
    SimpleAudioEngine::end();
}

bool AppDelegate::applicationDidFinishLaunching()
{
	InitSocket();
	
	StringMgr::GetInstance().Init();
	StringMgr::GetInstance().LoadStringWithPath("Config","LaunchString.xml");

	CCSize designResolutionSize = cocos2d::CCSizeMake(960, 640);
    // initialize director
    CCDirector *pDirector = CCDirector::sharedDirector();

	CCDisplayLinkDirector* displayLinkDirector = dynamic_cast<CCDisplayLinkDirector*>( pDirector );
	displayLinkDirector->m_DirectorPlugin = this;

	CCEGLView* pEGLView = CCEGLView::sharedOpenGLView();
	pDirector->setOpenGLView(pEGLView);

	//ipad2 CCMove闪屏？？
	CCDirector::sharedDirector()->setDepthTest(false); 
	//or 
	CCDirector::sharedDirector()->setProjection(kCCDirectorProjection2D);  

	//// Set the design resolution
	//pEGLView->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, kResolutionNoBorder);
	//CCSize frameSize = pEGLView->getFrameSize();
	//pDirector->setContentScaleFactor(MIN(frameSize.height/designResolutionSize.height, frameSize.width/designResolutionSize.width));

    // turn on display FPS
#ifdef _DEBUG
		 pDirector->setDisplayStats(true);
#else
	    pDirector->setDisplayStats(false);
#endif


    // set FPS. the default value is 1.0/60 if you don't call this
	pDirector->setAnimationInterval(1.0 / 60);


	CCFileUtils::sharedFileUtils()->addSearchPath("UIExport/DemoLogin/");
	CCFileUtils::sharedFileUtils()->addSearchPath("Script/protobuf");


	std::vector<std::string> paths = CCFileUtils::sharedFileUtils()->getSearchPaths();

	std::string strWriteAblePath = CCFileUtils::sharedFileUtils()->getWritablePath();
	paths.insert( paths.begin(), strWriteAblePath);
	paths.insert( paths.begin(), strWriteAblePath + "/UIExport/DemoLogin/");
	paths.insert( paths.begin(), strWriteAblePath + "/Script/protobuf/");
	paths.insert( paths.begin(), strWriteAblePath + "/AnimExport/");
	paths.insert( paths.begin(), strWriteAblePath + "/numeric/");

	CCFileUtils::sharedFileUtils()->setSearchPaths(paths);
	
	vector<string> vpath = CCFileUtils::sharedFileUtils()->getSearchPaths();
	for(int i = 0 ; i < vpath.size(); i++)
	{
		CCLog("ResPath =======================================================================================%s",vpath[i].c_str());
	}

	//CCFileUtils::sharedFileUtils()->addSearchPath("AnimExport/");

	//vector<string> vpath;
	//vpath.push_back(CCFileUtils::sharedFileUtils()->getWritablePath());
	////CCFileUtils::sharedFileUtils()->setSearchPaths(vpath);
	//CCFileUtils::sharedFileUtils()->addSearchPath(CCFileUtils::sharedFileUtils()->getWritablePath().c_str());
	//CCLog("=======================================================================================%s",CCFileUtils::sharedFileUtils()->getWritablePath().c_str());

	// register lua engine
	CCLuaEngine* pEngine = CCLuaEngine::defaultEngine();
	CCScriptEngineManager::sharedManager()->setScriptEngine(pEngine);

#ifndef WIN32
	luaopen_pb(pEngine->getLuaStack()->getLuaState() );
#endif
	//bool CheckVersion = !false;
	//if(CheckVersion)
	{
		//CCFileUtils::sharedFileUtils()->addSearchPath("/mnt/sdcard/muzhi/");

		// create a scene. it's an autorelease object
		//CCScene *pScene = Launcher::scene();
		// run
		//pDirector->runWithScene(pScene);
	}
	//else
	//{
	//	// create a scene. it's an autorelease object
	//	CCScene *pScene = MainScene::scene();
	//	// run
	//	pDirector->runWithScene(pScene);
	//}

	//CCMenuItem::s_ControlCallback = (ControlCallBack)(&Guide_BtnClicked);
	//CCControlButton::s_ControlCallback = (ControlCallBack)(&Guide_BtnClicked);

	CCTextureCache::sharedTextureCache()->setAutoAsyncLoadEnable(true);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground()
{
    CCDirector::sharedDirector()->stopAnimation();
    GetAudioMgr()->pauseBackgroundMusic();
	//GetAudioMgr()->pauseAllEffects();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground()
{
    CCDirector::sharedDirector()->startAnimation();
	if (GetAudioMgr()->isEnabled())
	{
        #if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
            GetAudioMgr()->playBackgroundMusic();
        #endif
		GetAudioMgr()->resumeBackgroundMusic();
	}

	//GetAudioMgr()->resumeAllEffects();
}

void AppDelegate::BeforeDrawScene()
{

}

void AppDelegate::AfterDrawScene()
{

}

void AppDelegate::MainLoop()
{
	static uint s_LastFrameTick = SysUtil::GetMSTime();
	uint nowTick = SysUtil::GetMSTime();
	uint delta = (nowTick - s_LastFrameTick)*CCDirector::sharedDirector()->getScaleTime();
	s_LastFrameTick = nowTick;

	CCDirector *pDirector = CCDirector::sharedDirector();
	pDirector->setOpenGLView(CCEGLView::sharedOpenGLView());
	MainScene* scene = dynamic_cast<MainScene*>(pDirector->getRunningScene());
	if (scene == NULL)
	{
		scene = &(MainScene::GetInstance());
		pDirector->runWithScene(scene);
	}
	else
	{
		UIMgr::GetInstance().SetMainScene(scene);
		scene->MainLoop(delta);
		ISocketSystem* socket = GetSocketSystem();
		if (socket)
		{
			socket->runSystem();
		}
	}
}

void AppDelegate::onJavaCommand( const char* json )
{
	CCLog("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||| On Java Cmd%s",json);
	string jstr = json;
	CCString* tag = CCString::create(jstr);
	CCLog("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||| On Java Cmd cvt  %s ",jstr.c_str());
	ScriptSys::GetInstance().Execute_1( "OnJavaCommand",tag,"CCString");
	
	//if(GetLocalPlayer() != NULL)
	//{
	//	if(NetConnection::GetInstance().GetConnectionType() == EConnectionType_GameServer)
	//	{
	//		PacketHeader packet;
	//		packet.m_Command = (EDataSync_QuitGame);
	//		packet.m_Length = sizeof(PacketHeader);
	//		NetConnection::GetInstance().Send( &packet );
	//		UIMgr::ShowWaiting( 30 );
	//		return;
	//	}
	//	else
	//	{
	//		
	//	}
	//}
	//SceneMgr::GetInstance().SwitchToMainMenu();
	//char script[2048] = { NULL };
	//sprintf( script, "OnJavaCommand('%s')", json);
	//ScriptSys::GetInstance().ExecuteString( script );
}

void AppDelegate::touchCallBack(CCObject *pSender, TouchEventType type)  
{  
	CCLog("callback eventType = %d", type);
	//will play   
} 

void AppDelegate::touchCallBack2(CCObject *pSender, TouchEventType type)  
{  
	CCLog("callback eventType = %d", type);
	UIMgr::GetInstance().CloseCurBaseWidget();
	//will play   
} 