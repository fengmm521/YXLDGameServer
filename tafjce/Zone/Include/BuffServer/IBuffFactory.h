#ifndef __IBUFF_FACTORY_H__
#define __IBUFF_FACTORY_H__

#define INFINITE            0xFFFFFFFF  // Infinite timeout(永久)


// 状态类型
enum
{
	en_buffType_Normal,		// 增益
	en_buffType_Damage,		// 伤害
	en_buffType_Debuf,		// 减益
	en_buffType_Control,		// 控制
};

// 状态删除策略
enum
{
	en_buffDelStrategy_Begin,
	en_buffDelStrategy_ID,
	en_buffDelStrategy_Group,
	en_buffDelStrategy_Type,
};

// 状态免疫类型
enum
{
	en_buffImMunity_Begin,
	en_buffImMunity_ID,			// 根据ID免疫
	en_buffImMunity_GroupID,		// 根据系列免疫
	en_buffImMunity_Type,			// 根据类型免疫
};

class IEntityBuff
{
public:

	virtual ~IEntityBuff(){}

	// 功能: 获取状态叠加数
	virtual int getOverlapCount() = 0;

	// 功能: 获取状态最大叠加数
	virtual int getMaxOverlapCount() = 0;

	// 功能: 获取状态配置表ID
	virtual Int32 getBuffID() const = 0;

	// 功能:状态对象克隆
	virtual IEntityBuff* clone()const  = 0;

	// 功能: 获取状态的UUID(唯一标识一个状态)
	virtual string getBuffUUID() = 0;

	// 功能: 获取Buff宿主
	virtual HEntity getMaster() = 0;

	// 功能: 获取状态Giver
	virtual HEntity getGiver() = 0;

	// 功能: 这只叠加数
	// 参数: [iNewOverlapCount] 新的叠加数
	virtual void setOverlapCount(int iNewOverlapCount) = 0;

	// 功能: 获取状态类型(增益、减益)
	virtual int getBuffType() const = 0;

	// 功能: 获取状态组ID
	virtual int getBuffGroupID() const = 0;

	// 功能: 执行特殊效果
	virtual void doSpecialEffect() = 0;

	virtual bool isShowClient() const = 0;

	virtual void setSysReleaseFlag() = 0;
};

class IBuffFactory:public IComponent
{
public:

	// 功能: 获取状态原型
	// 参数: [iBuffID] 状态ID
	virtual const IEntityBuff* getBuffPrototype(Int32 iBuffID) = 0;
};

#endif
