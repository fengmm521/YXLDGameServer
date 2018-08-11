/********************************************************************
created:	2012-11-17
author:		Fish (于国平)
summary:	组件id
*********************************************************************/
#pragma once
#include "FmConfig.h"

NS_FM_BEGIN

	// 组件id
enum EComponentId
{
	EComponentId_DataSync = 1,		// 数据同步
	EComponentId_EntityRender,		// 实体渲染模块
	EComponentId_FightSoulBags,		// 武魂背包
	//EComponentId_Quest,				// 任务模块
	EComponentId_Bags,				// 背包模块
	EComponentId_EquipBags,				// 装备模块
	EComponentId_Materail,				// 装备碎片模块
	EComponentId_InnatesKill,           //天赋技能
	EComponentId_HeroSoul,              //英雄魂魄
	EComponentId_HeroFavorite,          //英雄喜好
};

NS_FM_END