#ifndef __IFIGHTSOUL_SYSTEM_H__
#define __IFIGHTSOUL_SYSTEM_H__

class IFightSoulContainer
{
public:

	virtual ~IFightSoulContainer(){}

	// 功能: 总空间大小
	virtual Int32 getSpace() = 0;

	// 功能: 可用空间大小
	virtual Int32 getSize() = 0;

	// 功能: 获取武魂
	virtual Uint32 getFightSoul(int iPos) = 0;

	// 功能: 设置武魂
	virtual bool setFightSoul(int iPos, Uint32 hFightSoulHandle, GSProto::FightSoulChgItem* pOutItem) = 0;

	virtual HEntity getOwner() = 0;

	// 功能: 判断是否可以Set上
	virtual bool canSetFightSoul(int iPos, Uint32 hFightSoulHandle, bool bNoitifyErr) = 0;

	// 功能: 通知容器位置更新
	virtual void sendContainerChg(const vector<int>& posList) = 0;
};


class IFightSoulSystem:public IEntitySubsystem
{
public:

	// 功能:添加武魂
	// 参数: [iFightSoulID]武魂ID
	// 参数: [bNotifyErr] 是否提示客户端错误
	virtual Uint32 addFightSoul(int iFightSoulID, bool bNotifyErr) = 0;

	// 功能: 根据位置获取武魂
	virtual Uint32 getHFightSoulFromPos(int iPos) = 0;

	// 功能: 修炼
	virtual bool practice(bool bNotofyError) = 0;

	// 功能: 自动合并
	virtual void autoCombine() = 0;

	// 功能: 启用高级模式,普通模式隐藏
	virtual void enableAdvanceMode() = 0;

	// 功能:兑换
	virtual void exchangeFightSoul(int iExchangeID) = 0;

	// 功能: 执行合成
	virtual void combine(int iSrcConType, int iSrcPos, int iDstConType, int iDstPos, Uint32 dwHeroObjectID) = 0;
};


#endif


