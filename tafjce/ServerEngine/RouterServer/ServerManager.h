#ifndef __SERVER_MANAGER_H__
#define __SERVER_MANAGER_H__

#include "RouterComm.h"
#include "Snapshot.h"


struct ServerStatus
{
	ServerStatus():iWorldID(0), bAlive(false), iSordIndex(0), iAttr(0), iMemberCount(0){}

	int iWorldID;      // 服务器分区ID,配置获得
	string strName;    // 客户端显示名字，配置获得
	string strObject;  // 服务器OBJ.配置获得
	bool bAlive;       // 查询GS获得
	int iSordIndex;    // 服务器客户端显示排序，配置获得
	int iAttr;		  // 属性,配置获得
	int iMemberCount; // 在线人数,查询GS获得
	ServerEngine::GamePrx gamePrx;
};

class ServerManager:public TC_Thread, public TC_ThreadLock, public TC_HandleBase, public TC_Singleton<ServerManager>
{
public:

	ServerManager();
	~ServerManager();

	bool init();
	
	virtual void run();

	void reloadServerCfg();

	void terminate();

	bool getGameServer(int iIndex, ServerStatus& status);

	void checkAlive();

	void sendWorldList(taf::JceCurrentPtr current, const ServerEngine::RoleSnapshot& roleSnapshot);

	void setServerDown(int iWorldID);

	void setServerStatus(int iWorldID, const ServerEngine::GameQueryStatus& gameStatus);

	int calcBusyStatus(int iOnlineCount);

	void getResVersion(string& strUrlDir, string& strVersion, string& strMinVersion);

	string getSnapshotServerObj();

	/**
	 * @brief codeVersion是否存在于禁止更新列表中
	 */
	bool codeVersionNoResUpdate(const string& codeVersion);

private:

	typedef map<int, ServerStatus> ServerStatusMap;
	ServerStatusMap m_serverStatusMap;

	typedef map<string, ServerStatusMap::iterator> ObjStatusMap;
	ObjStatusMap m_objStatusMap;

	// 各类忙状态配置
	vector<int> m_busyCfgList;

	bool m_bTerminate;

	string m_strUrlDir;
	string m_strVersion;
	string m_strSnapshotServerObj;
	string m_strMinVersion;

	typedef set<string> CodeVersionMap;
	CodeVersionMap m_noResUpdateVersionMap;
};


#endif

