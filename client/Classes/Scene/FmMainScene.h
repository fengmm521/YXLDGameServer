/********************************************************************
created:	2012-12-05
author:		pelog
summary:	主场景:GameCanvas
			层次:
			1.地图层
			2.UI层
*********************************************************************/
#pragma once
#include "FmConfig.h"
#include "cocos2d.h"
#include "cocos-ext.h"
#include "Enum/FmCommonEnum.h"
#include "FmEntity.h"
NS_FM_BEGIN


USING_NS_CC;
USING_NS_CC_EXT;
using namespace gui;

class MainScene : public CCScene, public CCKeypadDelegate
{
public:
	virtual ~MainScene();
	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	SINGLETON_MODE(MainScene);

	MainScene();

	virtual void keyBackClicked();
    virtual void keyMenuClicked();

	// 主循环
	void MainLoop( uint delta );
	const CCSize& GetDesignSize(){return m_DesignSize;};
	const float GetScale(){return m_minScale;}
	void SwitchState( int newState );
	Entity* GetEntity(){return m_entity;}
	int GetState(){return m_state;}

	void AddChild(Widget* widget, int zOrder, int tag, bool isNeedTouch);

	float		m_minScale;
	float		m_maxScale;
	//CCComAudio*	m_audio;
private:
	CCSize		m_DesignSize;
	
	int			m_state;
	Entity*		m_entity;
	
};

static Entity* GetGlobalEntity(){return MainScene::GetInstance().GetEntity();}
static CCComAudio* GetAudioMgr(){
	return ((CCComAudio*)MainScene::GetInstance().getComponent("CCComAudio"));
}
NS_FM_END