#ifndef __IMESSAGE_LAYER_H__
#define __IMESSAGE_LAYER_H__

//#include "cs_msg_layer.h"
#include "Role.h"
#include "MsgCircleQueue.h"
//#include "Proxy.h"
#include "Name.h"
#include "FightData.h"
#include "ConvertCode.h"

#define IID_IMessageLayer		MAKE_RID('m','g','l','y')


template<class T>
class PrxCallbackHelper:public T
{
public:

	PrxCallbackHelper(const ServantPtr& servant, TC_AutoPtr<T> handle)
		:m_servant(servant),m_handle(handle)
	{
		assert(handle);
	}
	
	~PrxCallbackHelper(){}

	int onDispatch(ReqMessagePtr msg) 
	{ 
		ServantPtr tmpServant = m_servant;
		msg->callback = m_handle;
	    tmpServant->getResponseQueue().push_back(msg);
	    tmpServant->getHandle()->notifyFilter();

	    return 0;
	}
	
private:
	ServantPtr		m_servant;
	TC_AutoPtr<T>	m_handle;
};

void HelpMakeScMsg(GSProto::SCMessage& outPkg, int iCmd, const ::google::protobuf::Message& msgBody);

class IMessageLayer:public IComponent
{
public:
    virtual void sendPlayerMessage(const UserInfo *pUserInfo, const GSProto::SCMessage& msgPack) = 0;

	// 功能: 发送数据给客户端
	// 参数: [strRsAddress] RS地址
	// 参数: [ddCon]连接ID
	// 参数: [pMsgPack]消息对象
	virtual void sendMessage2Connection(const std::string& strRsAddress, Int64 ddCon, const GSProto::SCMessage& pMsgPack ) = 0;
    
	// 功能: 发送错误码消息包数据给客户端
	// 参数: [strRsAddress] RS地址
	// 参数: [ddCon]连接ID
	// 参数: [iErrorCode]错误码
	virtual void sendPlayerErrorMessage(const UserInfo *pUserInfo, 	int iErrorCode) = 0;
    virtual void sendErrorMessage2Connection( const std::string& strRsAddress, Int64 ddCon, int iErrorCode) = 0;

	virtual int NewRole(const std::string& strAccount, int iWorldID, int nRolePos, const std::string& strName, const ServerEngine::PIRole& data) = 0;

	// 功能: 写角色数据
	virtual int SaveRoleData(const ServerEngine::PKRole& key, const ServerEngine::PIRole& data) = 0;

	// 功能: 删除角色数据并置角色名为inactive
	virtual int DelRoleData(const ServerEngine::PKRole& key) = 0;
    

	// 功能: 获取角色数据
	virtual int GetRoleData(const ServerEngine::PKRole& key, ServerEngine::PIRole& data) = 0;

	// 功能: 创建角色异步接口
	virtual void AsyncNewRole(const std::string& strAccount, int iWorldID, int nRolePos, const std::string& strName, const ServerEngine::PIRole& data, ServerEngine::RolePrxCallbackPtr callback) = 0;

	// 功能: 异步更新角色数据
	virtual void AsyncUpdateRole(const ServerEngine::PKRole& key, const ServerEngine::PIRole& data, ServerEngine::RolePrxCallbackPtr callback) = 0;

	// 功能: 异步删除角色并置角色名为inactive
	virtual void AsyncDelRole(const ServerEngine::PKRole& key, const string &sRoleName, ServerEngine::RolePrxCallbackPtr callback) = 0;

    
	// 功能: 异步获取角色数据
	virtual void AsyncGetRole(const ServerEngine::PKRole& key, ServerEngine::RolePrxCallbackPtr callback) = 0;

   // 功能: 异步获取角色列表
	virtual void AsyncGetRoleList(const std::string& strAccount, ServerEngine::RolePrxCallbackPtr callback) = 0;

	// 功能: 获取CS发送消息队列
	virtual CMsgCircleQueue* getCsMessageQueue() = 0;

	// 功能: 验证名字是否可用
	virtual void AsyncGetNameDesc(const string& strName, ServerEngine::NamePrxCallbackPtr callback) = 0;

	virtual void AsyncSaveFightRecord(const ServerEngine::BattleData& fightData, ServerEngine::FightDataPrxCallbackPtr callback) = 0;

	virtual void AsyncGetFightRecord(const ServerEngine::PKFight& fightKey, ServerEngine::FightDataPrxCallbackPtr callback) = 0;

	virtual void AsyncDelFightRecord(const ServerEngine::PKFight& fightKey, ServerEngine::FightDataPrxCallbackPtr callback) = 0;

	virtual void AsyncGetConvertCode(const std::string& strName, ServerEngine::ConvertCodePrxCallbackPtr callback)= 0;

	virtual void AsyncUpdateConvertCode(const std::string& strName, int state ,ServerEngine::ConvertCodePrxCallbackPtr callback)= 0;

//	virtual void AsyncPayReturnMoneyAndVip(ServerEngine::PayReturnPrxCallbackPtr cb, const string& account)= 0;
};

#endif
