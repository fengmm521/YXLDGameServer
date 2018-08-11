/********************************************************************
created:	2013-01-07
author:		Fish (于国平)
summary:	玩家的模块基类,提供一些通用接口
*********************************************************************/
#pragma once
#include "FmEntity.h"

//#include "GameEntity/FmSceneEntityComponent.h"

NS_FM_BEGIN

class Player;

class PlayerComponent : public Component
{
public:
	PlayerComponent( Entity* entity ) : Component( entity ) {}

	// 获取玩家对象
	Player* GetPlayer();
};

NS_FM_END