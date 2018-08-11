/********************************************************************
created:	2013-3-21
author:		SXH
summary:	launcher Scene
*********************************************************************/
#pragma once
#include "FmConfig.h"
#include "cocos2d.h"
#include "cocos-ext.h"
#include "NetWork/RouterServer.pb.h"

NS_FM_BEGIN


USING_NS_CC;
USING_NS_CC_EXT;

using namespace gui;


class FileMgr;

enum E_LauncherStep
{
	E_LauncherStep_None,
	E_LauncherSteo_GetVersion,
	E_LauncherSteo_MediaVerCheck,	//渠道版本检查
	E_LauncherStep_Init,			//初始化状态
	E_LauncherStep_LoadConfig,		//加载配置
	E_LauncherStep_VerCheck,		//版本检查
	E_LauncherStep_CheckUrlFile,	//计算需要加载的文件
	E_LauncherStep_LoadUrlFile,		//下载资源
	E_LauncherStep_LocalVerify,		//本机效验
	E_LauncherStep_ResLoadFinished,	//资源加载完成
	E_LauncherStep_ResInit,			//资源初始化
	E_LauncherStep_ResInit2,		//资源初始化2
	E_LauncherStep_ResInitEnd,		//资源初始化结束
	E_LauncherStep_SwitchGame,		//切换到游戏场景
	E_LauncherStep_Error,			//加载错误
	E_LauncherStep_MsgBox,			//对话框状态
};

enum E_LauncherTag
{
	E_LauncherTag_Tips,
	E_LauncherTag_LoadTip,
	E_LauncherTag_LoadProgress,
	E_LauncherTag_MsgBox,
	E_LauncherTag_LoadBar,

};

const std::string s_VersionNumFileName = "version.config";
const std::string s_ResConfigFileName = "res.config";

class FmAudioEngineDelegate:public AudioEngineDelegate
{
public:
	SINGLETON_MODE( FmAudioEngineDelegate );
public:
	virtual void playEffect(const char* pszFilePath, bool bLoop);
	virtual void playBackgroundMusic(const char* pszFilePath, bool bLoop);
};
class Launcher:public CCLayer
{
public:
	static Launcher* GetInstance();
	static Launcher* create();
public:
	Launcher();
	virtual ~Launcher();
	virtual bool init();

	void SwitchStep(E_LauncherStep step);
	void MainLoop( uint delta );

	void UpdateTips(const char* remind);

	void UpdateLoadTips(const char* filename);
	void UpdateLoadProgress(int total,int cur);
	
	void UpdateLoadBar(const char* tips,int val);

	void ShowMessageBox( const char * titleStr,const char* msgStr,const char* leftStr,const char* rightStr, int tag );
	void HandleMessageBox( CCObject *pSender, TouchEventType type );
	void StartLoadRes();

	void OnVersionResponse(const char * resp);

	void UpdateLoadDummyProgressTips(float delta);
	void InitLoadProgressVal(int total,int destVal);
	void SetDestProgressValue(int val){m_DestProgressValue = val;};

	static void RouterVersion( int iCmd, ServerEngine::SCMessage& pkg );

private:
	bool ResInitLoad1();
	bool ResInitLoad2();
	bool ResInitLoad3();
	void WaitRS(float time);
	void Connect();
	void ReConnect(float time);
private:
	FileMgr*		m_AppResMgr;//apk 或 boundle里面的资源管理
	FileMgr*		m_LocalMgr; //本地文件资源管理
	FileMgr*		m_ServerMgr;//URL服务器文件资源管理
	bool			m_Finished;
	bool			m_HasGetVersion; //是否获取到版本号
	float			m_uiScale;
	int			m_DestProgressValue;
	int			m_TotalProgressValue;
	int			m_CurProgressValue;

	E_LauncherStep m_LauncherStep;
public:
	int				m_RequestFailCounter; //请求失败次数
	string			m_url;
	string			m_version;
	string			m_UpdateVerUrl;
};


NS_FM_END
