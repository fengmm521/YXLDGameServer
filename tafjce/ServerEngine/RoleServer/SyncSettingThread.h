#ifndef __SYNCSETTINGTHREAD_H__
#define __SYNCSETTINGTHREAD_H__

#include "servant/Application.h"
#include "util/tc_monitor.h"
#include "DbAccess.h"


enum
{
	en_Operate_Null,
	en_Operate_Set,
	en_Operate_Del,
};


struct OperateData
{
	OperateData():iOperate(en_Operate_Null){}
	int iOperate;
	string strValue;
};


class SyncSettingThread:public TC_Thread, public TC_ThreadLock, public TC_HandleBase
{
public:

	SyncSettingThread();
	~SyncSettingThread();

	void setDbAccess(ServerEngine::DbAccessPrx prx);
	virtual void run();
	void setSettingData(const string& strKey, const string& strValue);
	bool getSettingData(const string& strKey, string& strValue);
	void eraseData(const string& strKey);
	void terminate();
	bool isDeleting(const string& strKey);

private:

	bool m_bTerminate;
	ServerEngine::DbAccessPrx m_dbaccessPrx;
	std::map<string, OperateData> m_settingMap;
};



#endif
