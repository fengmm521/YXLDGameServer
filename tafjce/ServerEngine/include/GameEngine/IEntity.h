#ifndef __IENTITY_H__
#define __IENTITY_H__

#include "EntityProperty.h"
#include "RoleData.h"


/*enum
{
	en_class_Null,
	en_class_Actor,
	en_class_Hero,
	en_class_Monster,
	en_class_Npc,
	en_class_Pet,
	en_class_Trigger,
	en_class_MagicGhost,
	en_class_Ghost,	// 玩家战斗实体
	en_class_Max,
};*/

#define MAX_INT_VALUE	2000000000

class IEntity;
typedef Handle<IEntity*> HEntity;

class IEntitySubsystem:public IObject
{
public:

	// 功能: 获取实体子系统ID
	virtual Uint32 getSubsystemID() const = 0;

	// 功能: 获取所属实体句柄
	virtual Uint32 getMasterHandle() = 0;

	// 功能: 创建子系统
	// 参数: [pEntity] 实体对象
	// 参数: [pContext] 创建子系统所需现场数据
	// 参数: [nLen] 创建子系统所需数据长度
	virtual bool create(IEntity* pEntity, const std::string& strData) = 0;

	// 功能: 初始化完毕
	virtual bool createComplete() = 0;

	// 功能: 获取子系统负责的消息
	virtual const std::vector<Uint32>& getSupportMessage() = 0;

	// 功能: 消息处理
	virtual void onMessage(QxMessage* pMessage) = 0;

	// 功能: 打包子系统数据
	virtual void packEnterViewData(GSProto::SCMessage& pMessage, HEntity hRcvEntity){}

	// 功能:打包子系统数据,用于存储DB
	virtual void packSaveData(string& data) = 0;
    
    // 功能: 打包查看别人详情的时候的社交信息部分数据
    virtual void packViewOtherRoleInfo(HEntity hQueryEntity, GSProto::SCMessage& refMessage) {};
    
    // 功能: 打包查看别人摘要数据的时候的社交信息部分数据
    virtual void packViewOtherRoleSimpleInfo(HEntity hQueryEntity, GSProto::SCMessage& refMessage) {};

	// 功能: 扩展查询接口
	virtual void* queryInterface(int iInterfaceID){return NULL;}
};



class IEntity:public IObject
{
public:


	// 功能: 查询子系统
	// 参数: [dwSubsystemID] 子系统ID
	virtual IEntitySubsystem* querySubsystem(Uint32 dwSubsystemID) = 0;

	// 功能: 获取子系统列表
	virtual std::vector<Uint32> getSubsystemList() = 0;

	// 功能: 创建实体对象
	// 参数: [propSet] 属性集
	virtual bool createEntity(const std::string& strData) = 0;

	// 功能: 获取实体句柄
	virtual HEntity getHandle() = 0;

	// 功能: 获取时间服务器
	virtual EventServer* getEventServer() = 0;
	
	// 功能: 处理客户端消息
	virtual void onMessage(QxMessage* pMessage) = 0;

	// 功能: 打包存储结构
	virtual void packSaveData(ServerEngine::RoleSaveData& data) = 0;

	// 功能: 存储数据
	virtual void save() = 0;

	// 功能: 属性修改Change接口
	// 参数: [iPropertyKey] 属性ID
	// 参数: [iChgValue] 修改数值
	virtual void changeProperty(PropertySet::PropertyKey iKey, int iChgValue, int iReason) = 0;

	// 功能: 发送消息
	virtual void sendMessage(const GSProto::SCMessage& scMessage) = 0;

	// 功能: 发送错误码
	virtual void sendErrorCode(int iErrorCode) = 0;

	// 功能: 添加经验
	virtual void addExp(int iExp) = 0;

	virtual void sendMessage(int iCmd, const ::google::protobuf::Message& msgBody) = 0;

	virtual void sendMessage(int iCmd) = 0;

	virtual bool isFunctionOpen(int iFunctionID) = 0;

	virtual void enableFunction(int iFunctionID) = 0;
	virtual void addVipExp(int iExp) = 0;
	virtual void chgNotice(int iNoticeId,bool haveNotice) = 0;
};

IEntity* getEntityFromHandle(HEntity hEntity);

#endif

