#ifndef __IBUFFSYSTEM_H__
#define __IBUFFSYSTEM_H__

class IBuffSystem:public IEntitySubsystem
{
public:

	// 功能: 添加状态
	// 参数: [hGiver] 状态添加者
	// 参数: [iBuffID] 状态ID
	virtual IEntityBuff* addBuff(HEntity hGiver, int iBuffID, const EffectContext& preContext=EffectContext() ) = 0;

	// 功能: 根据状态UUID获取状态
	// 参数: [strUUID] 状态UUID
	virtual IEntityBuff* getBuff(const std::string& strUUID) = 0;

	// 功能:根据ID 删除状态
	virtual void delBuff(int iBaseID) = 0;

	// 功能: 根据UUID删除状态
	virtual void delBuffByUUID(const std::string& strUUID) = 0;

	// 功能: 根据状态类型删除
	// 参数: [iBuffType] 状态类型
	virtual void delBuffByType(int iBuffType) = 0;

	// 功能: 按照状态系列来删除
	// 参数: [iGroupID] 系列ID
	virtual void delBuffByGroup(int iGroupID) = 0;

	// 功能: 判断是否免疫状态
	// 参数: [iBuffID] 状态ID
	virtual bool isImMinityBuff(int iBuffID) = 0;

	// 功能: 随机删除一个指定类型的状态
	virtual bool randomDelBuffByType(int iBuffType) = 0;

	virtual bool existBuff(int iBuffID) = 0;
};

#endif
