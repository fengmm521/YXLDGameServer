/********************************************************************
created:	2012-12-09
author:		Fish (于国平)
summary:	本机玩家的相关接口
*********************************************************************/
#pragma once

#include "FmConfig.h"
#include "FmPlayer.h"

NS_FM_BEGIN

class LocalPlayer
{
protected:
	Player*	m_Player;	// 本机玩家对象
public:
	LocalPlayer();
	virtual ~LocalPlayer();

	SINGLETON_MODE( LocalPlayer );

	void SetPlayer( Player* player ) { m_Player = player; }
	Player* GetPlayer() { return m_Player; }

};

static Player* GetLocalPlayer()
{
	return LocalPlayer::GetInstance().GetPlayer();
}

NS_FM_END