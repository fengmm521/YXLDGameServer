/********************************************************************
created:	2013-3-13
author:		SXH
summary:	文件资源管理
*********************************************************************/
#pragma once
#include "FmConfig.h"
#include "cocos2d.h"
#include "cocos-ext.h"


USING_NS_CC;
USING_NS_CC_EXT;

NS_FM_BEGIN

class UrlFile;

typedef map<string,UrlFile*> UrlFileMap;

class FileMgr:public CCNode,public CCSAXDelegator
{
public:
	static bool VersionMatched(const FileMgr* fm1,const FileMgr* fm2);

	static bool UnCompress(const char* filename,const char* path);
	static bool createDirectory(const char *path);
public:
	FileMgr();
	virtual ~FileMgr();

	void Clear();
	//plist 结构解析
	// implement pure virtual methods of CCSAXDelegator
	void startElement(void *ctx, const char *name, const char **atts);
	void endElement(void *ctx, const char *name);
	void textHandler(void *ctx, const char *ch, int len);

	//本地文件管理初始化配置
	bool InitLocalMgr(const char* writePath,const char* resConfigName);
	//本地文件管理初始化
	bool LoadLocalConfig();

	void SetResUrl(const char* resUrl);

	//服务器文件管理初始化
	bool InitServerMgr(const char* resUrl,const char* versionNumName,const char* resConfigName);
	//服务器文件管理，获取版本号
	bool RequestVersionNum();
	//服务器文件管理，获取资源配置
	bool RequestResConfig();

	//是否获取到版本号
	bool HasGetVersionNum();

	bool IsPreapred(){return m_Prepared;};

	bool SynWithFileMgr(FileMgr* fmserver);

	//合并放在程序包的资源
	bool MergeWithFileMgr(FileMgr* fmApp);

	//重新安装包后，清理原来包文件
	bool CheckPackRes();

	UrlFile* GetFileByName(const char* name);

	const UrlFileMap& GetFileMap();

	bool ActiveNextRequest();
	bool StartRequest();
	bool IsRequestFinished(){return m_IsInRequest == false;};

	bool OnLoadFinished();

	bool SavingConfigFile(bool finishSave=true);

	int GetTotalDownSize(){return m_TotalLoadByteSize;};
	int GetCurDownSize(){return m_CurLoadByteSize;};

	bool CodeVersionMatched(const string codeVersion);

	void SetPrepare(bool pre){m_Prepared = pre;};

	bool CodeVersionIsLowerThan(const string codeVersion);

private:
	bool InitFilesWithBuff(const char* buffer);
	bool InitVersionNumWithBuff(const char* buffer);
	void RequestUrlFile(const char* url,const char* tag);
	void onHttpRequestCompleted(cocos2d::CCNode *sender, void *data);
private:
	//本地和服务端公用
	UrlFileMap	m_FileMap;
	bool		m_Prepared;
	string		m_Version;
	string		m_UpdateVersion;
	bool		m_VersionPreapred;

	string		m_CodeVersion;

	//资源配置文件名
	string		m_ConfigName;
	//版本号文件名
	string		m_VersionFileName;

	//本地存储路径
	string		m_WritePath;
	//服务器端URL
	string		m_ResUrl;

	bool		m_IsInRequest;
	int			m_TotalLoadByteSize;
	int			m_CurLoadByteSize;
	
	cc_timeval	m_LoadStartTime; 
	cc_timeval	m_LastSaveTime; 
};

NS_FM_END
