#ifndef __ICLEANUP_MANAGER_H__
#define __ICLEANUP_MANAGER_H__


#define IID_ICleanUpManager	MAKE_RID('c', 'l', 'm', 'g')

class ICleanUpManager:public IComponent
{
public:

	// 功能: 执行清理,主要处理玩家数据存档
	virtual void doCleanUp(bool boIsKickOffRole = true) = 0;
	virtual void heatBeat() = 0;
};


#endif

