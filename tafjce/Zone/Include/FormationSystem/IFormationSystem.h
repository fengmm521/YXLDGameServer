#ifndef __IFORMATIONSYSTEM_H__
#define __IFORMATIONSYSTEM_H__

class IFormationSystem:public IEntitySubsystem
{
public:

	// 功能: 获取实体列表
	// 参数: [entityList] 上阵实体列表
	virtual void getEntityList(vector<HEntity>& entityList) = 0;

	// 功能: 获取特定位置上的出战单元
	// 参数: [iPos] 位置
	virtual HEntity getEntityFromPos(int iPos) = 0;


	// 功能: 获取实体在阵形位置
	virtual int getEntityPos(HEntity hEntity) = 0;

	virtual int getVisibleHeroID() = 0;

	virtual void setChgFormationSwitchState(bool bClose) = 0;

	// 功能: 获取当前阵形限制
	virtual int getFormationHeroLimit() = 0;

	virtual int getFormationHeroCount() = 0;
};


#endif
