#ifndef __IEQUIPBAGSYSTEM_H__
#define __IEQUIPBAGSYSTEM_H__


#define IID_IEquipContainer  1

class IEquipContainer
{
public:

	virtual ~IEquipContainer(){}

	virtual int getSize() = 0;

	virtual Uint32 getEquip(int iPos) = 0;

	// 功能: 设置装备
	virtual bool setEquip(int iPos, Uint32 hEquipHandle, GSProto::EquipChgItem* pOutItem) = 0;

	virtual HEntity getOwner() = 0;

	// 功能: 通知容器位置更新
	virtual void sendContainerChg(const vector<int>& posList) = 0;

	virtual bool canSetEquip(int iPos, Uint32 hEquipHandle, bool bNotifyError) = 0;

	virtual int getContainerType() = 0;
};


class IEquipBagSystem:public IEntitySubsystem
{
public:

	virtual Uint32 addEquip(int iItemID, int iReason, bool bNotifyError, int* _outPos = NULL) = 0;

	virtual int getFreeSize() = 0;

	virtual int addEquipByHandle(HItem hEquip, int iReason, bool bNotifyError) = 0;
};


class IHeroRealEquipSystem:public IEntitySubsystem
{
public:

	virtual bool hasEquip() = 0;
};


#endif
