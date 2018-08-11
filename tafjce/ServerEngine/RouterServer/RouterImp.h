#ifndef __ROUTER_IMP_H__
#define __ROUTER_IMP_H__

#include <iostream>
#include "servant/Application.h"
#include "util/tc_timeout_queue.h"
#include "GameInterface.h"
#include "util/tc_config.h"
#include "Snapshot.h"

enum
{
	en_ConnState_Begin,
	en_ConnState_Half,
	en_ConnState_Connected,
};


struct ConnectionInfo:public taf::TC_HandleBase
{
	ConnectionInfo():ddConnID(0), iState(en_ConnState_Begin), iWorldID(-1){}

	string strAccount;
	taf::JceCurrentPtr current;
	int64_t ddConnID;
	int iState;
	int iWorldID;


	// 连接后，数据
	string strGameObject;
	ServerEngine::GamePrx gamePrx;
};

typedef taf::TC_AutoPtr<ConnectionInfo> ConnectionInfoPtr;


class ConnectionManager:public taf::TC_TimeoutQueue<ConnectionInfoPtr>, public TC_Singleton<ConnectionManager>
{
public:

	ConnectionManager();
	~ConnectionManager();

	void init();

	// 功能: 更新连接信息
	// 参数: [ddConnID] 连接信息
	// 参数: [conInfo] 连接信息
	void update(int64_t ddConnID,  ConnectionInfoPtr conInfo);

	// 功能: 销毁连接
	// 参数: [conInfo]连接信息
	void destroyConn(ConnectionInfoPtr& conInfo);

	ConnectionInfoPtr get(int64_t ddConnID);
};

int64_t makeConKey(const string& strIp, int iPort);

class RouterImp: public taf::Servant
{
public:

	RouterImp();
	~RouterImp();

	 /**
     * 对象初始化
     */
    virtual void initialize();

    /**
     * 处理客户端的主动请求
     * @param current
     * @param response
     * @return int
     */
    virtual int doRequest(taf::JceCurrentPtr current, vector<char>& response);

    /**
     * @param resp
     * @return int
     */
    virtual int doResponse(ReqMessagePtr resp);

	/**
     * @param resp
     * @return int
     */
	int doResponseException(ReqMessagePtr resp);

	/**
     * @param resp
     * @return ints
     */
	int doResponseNoRequest(ReqMessagePtr resp);

    /**
     * 对象销毁
     */
	virtual void destroy();

private:

	void processNewConnection(taf::JceCurrentPtr current);
	void transToGameServer(taf::JceCurrentPtr current);
	void processHalfRequest(taf::JceCurrentPtr current);
	void loadSnapshotServer();

private:

	string m_strLocalPushObject;
	ServerEngine::SnapshotPrx m_snapshotPrx;
};



#endif
