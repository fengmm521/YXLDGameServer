/********************************************************************
created:	2012-12-06
author:		Fish (于国平)
summary:	更新事件
*********************************************************************/
#pragma once

#include "FmEvent.h"

NS_FM_BEGIN

	// 更新事件
enum EUpdateEvent
{
	EUpdateEvent_Begin = 0x0200,

	EUpdateEvent_Update,			// 更新

	EUpdateEvent_End	 = 0x02FF
};

class EventUpdate : public Event
{
public:
	uint	m_Delta;	// 上一帧的间隔

	EventUpdate( uint64 entityId, uint delta )
		: Event( EUpdateEvent_Update, entityId ), m_Delta( delta )
	{
	}
};

NS_FM_END