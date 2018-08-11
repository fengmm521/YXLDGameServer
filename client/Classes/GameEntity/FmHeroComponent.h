/********************************************************************
created:	2013-01-07
author:		Fish (于国平)
summary:	场景子对象的模块基类,提供一些通用接口
*********************************************************************/
#pragma once

#include "FmComponent.h"

NS_FM_BEGIN

class Hero;

class HeroComponent : public Component
{
public:
	HeroComponent( Entity* entity ) : Component( entity ) {}

	// 获取场景子对象
	Hero* GetHero();
};

NS_FM_END