#include "FmMainScene.h"
#include "cocos2d.h"
#include "UI/FmUIMgr.h"
#include "Launcher.h"
#include "UI/LayerExten.h"
#include "SceneMgr.h"
#include "FmEntityMgr.h"
#include "Script/FmScriptSys.h"


USING_NS_CC;
USING_NS_FM;
USING_NS_CC_EXT;

using namespace cocos2d::gui;

NS_FM_BEGIN

MainScene::~MainScene()
{
	//SceneMgr::GetInstance().Shutdown();
	CCDirector::sharedDirector()->getKeypadDispatcher()->removeDelegate(this);
}

MainScene::MainScene()
{
	m_DesignSize = CCSizeMake(960,640);
	CCSize frameSize = CCEGLView::sharedOpenGLView()->getFrameSize();
	m_minScale = MIN(frameSize.width/m_DesignSize.width,frameSize.height/m_DesignSize.height);
	m_maxScale = frameSize.width/m_DesignSize.width;//MAX(frameSize.width/m_DesignSize.width,frameSize.height/m_DesignSize.height);
	UIMgr::GetInstance().SetMainScene(this);
	m_state = EState_Logo;
	SceneMgr::GetInstance();
	this->addComponent(CCComAudio::create());
	//this->getComponent("Audio")->setEnabled(true);
	//m_audio = CCComAudio::create();;
	//m_audio->setEnabled(true);

	CCDirector::sharedDirector()->getKeypadDispatcher()->addDelegate(this);

	m_entity = EntityMgr::GetInstance().CreateEntity(EEntityType_Entity, "", NULL);
	
	if (frameSize.height/m_DesignSize.height > frameSize.width/m_DesignSize.width)
	{
		CCSize uiSize = CCSizeMake(m_DesignSize.width*m_minScale,m_DesignSize.height*m_minScale);

		Layout* bottom = Layout::create();
		bottom->setSize(CCSizeMake(frameSize.width, (frameSize.height - uiSize.height)/2));
		bottom->setBackGroundColor(ccc3(0, 0, 0));
		bottom->setBackGroundColorOpacity(255);
		bottom->setPosition(ccp(0, 0));
		Layout* top = dynamic_cast<Layout*>(bottom->clone());
		top->setPosition(ccp(0, bottom->getSize().height + uiSize.height));

		this->addChild(bottom, 999999999);
		this->addChild(top, 999999999);
	}
	
}

void MainScene::keyBackClicked()
{
	ScriptSys::GetInstance().Execute("onClickSysBack");
}

void MainScene::keyMenuClicked()
{
	ScriptSys::GetInstance().Execute("onClickSysMenu");
}

void MainScene::MainLoop( uint delta )
{
	m_entity->Update(delta);
	switch (m_state)
	{
	case EState_Logo:
		Launcher::GetInstance()->MainLoop(delta);
		break;
	case EState_MainMenu:
		
		break;
	case EState_Gaming:
		SceneMgr::GetInstance().Update(delta);
		break;
	}

	LifeNode::UpdateAllLifeNode(delta);
	EntityMgr::GetInstance().Update(delta);
	UIMgr::GetInstance().Update(delta);
}

void MainScene::SwitchState( int newState )
{
	if (newState < EState_Logo || newState > EState_End - 1)
	{
		return;
	}

	m_state = newState;
}


void MainScene::AddChild(Widget* widget, int zOrder, int tag, bool isNeedTouch)
{
	CCSize designSize = this->GetDesignSize();
	CCSize frameSize = CCEGLView::sharedOpenGLView()->getFrameSize();
	float scaleFactMin = m_minScale;//MIN(frameSize.width/designSize.width,frameSize.height/designSize.height);
	float scaleFactMax = m_maxScale;//MAX(frameSize.width/designSize.width,frameSize.height/designSize.height);

	UILayer * pLayer = UILayer::create();

	if (isNeedTouch)
	{
		Layout* layout = Layout::create();
		layout->setTouchEnabled(true);
		layout->setAnchorPoint(ccp(0.5, 0.5));
		layout->setPosition(ccp(480, 320));
		layout->setSize(CCSizeMake(frameSize.width, frameSize.height));
		pLayer->addChild(layout, -9999);
	}
	
	pLayer->addWidget(widget);

	CCSize uiSize = CCSizeMake(designSize.width*scaleFactMin,designSize.height*scaleFactMin);
	pLayer->setContentSize(uiSize);
	pLayer->setAnchorPoint(ccp(0,0));
	pLayer->setScale(scaleFactMin);
	CCPoint pos = ccp((frameSize.width - uiSize.width)/2,(frameSize.height - uiSize.height)/2);
	pLayer->setPosition(pos);

	this->addChild(pLayer, zOrder, tag);
	pLayer->setTouchPriority(-zOrder);
	pLayer->setTouchEnabled(isNeedTouch);

	Widget* bgWidget = widget->getChildByName("BG");
	if (bgWidget)
	{	
		bgWidget->setAnchorPoint(ccp(0.5, 0.5));
		bgWidget->setPosition(ccp(480, 320));
		bgWidget->setScale(scaleFactMax/scaleFactMin);
	}	
}

NS_FM_END