#ifndef __IUSER_STATE_MANAGER_H__
#define __IUSER_STATE_MANAGER_H__

#include "RoleData.h"
#include "IEntity.h"

#define IID_IUserStateManager		MAKE_RID('u','s','m','g')

enum
{
	en_NPCRole_Account_Genius = 10000,
};
enum
{
	en_NPCRole_Channel_Genius = 1,
};

enum
{
	en_UserState_NULL,
	en_UserState_GetRole,
	en_UserState_NewRole,
	en_UserState_GetingRole,
	en_UserState_Running,
};

enum
{
	en_LoginResult_Success,
};


struct UserInfo
{
	UserInfo():nState(0), dwLoginTime(0), dwEnterTime(0), ddConnectID(0),hEntity(0), 
		dwLastPingTime(0){}

	Int32  nState;
	Uint32 dwLoginTime;
	Uint32 dwEnterTime;
	string strRsAddress;
	Int64  ddConnectID;
	Uint32	hEntity;
    DWORD   dwLastPingTime;

    DWORD  dwCltType;   // 客户端版本类型
    DWORD  dwCltVer;    // 客户端版本号

	string strChannelFlag;
	// 应用宝相关
	string strChannel;
	string strAccessToken;
	string strPayToken;
	string strPf;
	string strPfKey;

	//棱镜相关
	string strChannelId;
	string strUserId;
	string strToken;
	string strProductCode;
	string strChannelLabel;
};

class IUserStateManager
{
public:

	virtual ~IUserStateManager(){}

	// 功能: 根据QQ号码获取用户状态
	// 参数: [dwUin] QQ号码
	virtual Int32 getUserState(const std::string& strAccount) = 0;

	// 功能: 根据QQ号码获取用户信息
	// 参数: [dwUin] QQ号码
	virtual const UserInfo* getUserInfo(const std::string& strAccount) = 0;

	// 功能: 获取用户总数
	virtual size_t getUserSize() = 0;

	// 功能: 获取所有在线玩家的QQ
	virtual void getAllUserQQ(std::vector<std::string>& qqList) = 0;

	// 功能: 踢用户下线
	virtual void kickUser(const std::string& strAccount, int iReason = 0, bool bSynSave = false) = 0;

	// 功能: 根据名字获取角色
	virtual Uint32 getActorByName(const std::string& strName) = 0;

    // 功能: 获取消息服务器
	virtual EventServer* getEventServer() = 0;

	// 功能: 添加保留机器
	virtual void addReserveGhost(HEntity hEntity) = 0;

	// 功能: 获取用户信息,包括机器人
	virtual HEntity getRichEntityByAccount(const std::string& strAccount) = 0;

	virtual int getActorSize() = 0;

	//获取好友推荐列表
	virtual void getRecommendList(vector<HEntity>& vecHEntity) = 0;

	// 功能: 获取UserMap
	virtual const map<string, UserInfo>& getUserMap() = 0;

	virtual HEntity getRichEntityByName(const string& strName) = 0;
	
};

#endif
