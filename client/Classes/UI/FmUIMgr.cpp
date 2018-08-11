#include "FmUIMgr.h"
#include "Scene/FmMainScene.h"
#include "LayerExten.h"
#include "Effect/FmEffect.h"
#include "cocos-ext.h"
#include "cocos2d.h"
#include "Util/FmStringMgr.h"
#include "CompLabel.h"
#include "FmTypeConvert.h"
#include "Script/FmScriptSys.h"
#include "Scene/SceneMgr.h"

NS_FM_BEGIN

USING_NS_CC;
USING_NS_CC_EXT;

using namespace gui;

UIMgr::UIMgr()
{
	m_msgBoxWidget = NULL;
}

bool UIMgr::Init()
{
	return true;
}

void UIMgr::ShowWaiting( float time )
{
	ScriptSys::GetInstance().Execute("ShowWaiting");
}

void UIMgr::EndWaiting()
{
	ScriptSys::GetInstance().Execute("EndWaiting");
}

void UIMgr::ShutDown()
{
	m_baseWidgets.clear();
}

Widget* UIMgr::GetBaseWidgetByName( string widgetName )
{
	for (size_t i=0; i<m_baseWidgets.size(); i++)
	{
		if (m_baseWidgets[i] && m_baseWidgets[i]->getIsNeedRemove() == false && widgetName == m_baseWidgets[i]->getName())
		{
			return m_baseWidgets[i];
		}
	}

	return NULL;
}

bool UIMgr::IsSpecialUI(Widget* widget)
{
	if (strcmp("MainUi", widget->getName()) == 0
		|| strcmp("Territory", widget->getName()) == 0
		|| strcmp("MyTerritory", widget->getName()) == 0
		|| strcmp("MainUiTop", widget->getName()) == 0
		//|| strcmp("MyTerritory", widget->getName()) == 0
		//|| strcmp("Cg", widget->getName()) == 0
		|| strcmp("Fight", widget->getName()) == 0
	)
	{
		return true;
	}
	return false;
}

void UIMgr::RefreshSimpleChat()
{
	if (m_baseWidgets.size() > 0)
	{
		int index = m_baseWidgets.size() - 1;
		if (strcmp("MainUi", m_baseWidgets[index]->getName()) == 0
			//|| strcmp("Arena",  m_baseWidgets[index]->getName()) == 0
			//|| strcmp("WorldBoss",  m_baseWidgets[index]->getName()) == 0
			//|| strcmp("Dungeon",  m_baseWidgets[i]->getName()) == 0
			//|| strcmp("DungeonList",  m_baseWidgets[i]->getName()) == 0
			//|| strcmp("CompBattle", m_baseWidgets[index]->getName()) == 0
			)
		{
			TouchGroup* pLayer = static_cast<TouchGroup*>(m_baseWidgets[index]->getParent()->getParent());
			if (pLayer)
			{
				ScriptSys::GetInstance().Execute_2("UI_AddSimpleChat", m_baseWidgets[index], "Widget", pLayer, "TouchGroup");
				return;
			}		
		}
	}

	//ScriptSys::GetInstance().Execute("UI_HiddenSimpleChat");
}

void UIMgr::CheckNeedClose()
{
	UICloseActionCallBack();
}

Widget* UIMgr::CreateBaseWidgetByFileName( string fileName, int action, float time)
{
	CheckNeedClose();

	UILayer * pLayer = UILayer::create();
	CCSize designSize = m_mainScene->GetDesignSize();
	CCSize frameSize = CCEGLView::sharedOpenGLView()->getFrameSize();
	float scaleFactMin = MIN(frameSize.width/designSize.width,frameSize.height/designSize.height);
	float scaleFactMax = frameSize.width/designSize.width;//MAX(frameSize.width/designSize.width,frameSize.height/designSize.height);
	Layout* layout = Layout::create();
	layout->setTouchEnabled(true);
	layout->setAnchorPoint(ccp(0.5, 0.5));
	layout->setPosition(ccp(480, 320));
	layout->setSize(CCSizeMake(frameSize.width, frameSize.height));
	pLayer->addWidget(layout);

	Widget* widget = m_cacheWidgets[fileName];
	if (widget == NULL)
	{
		widget = GUIReader::shareReader()->widgetFromJsonFile(fileName.c_str());
	}

	if (widget)
	{
		widget->setScale(1);
		widget->setPosition(ccp(0, 0));
		widget->setOpacity(255);
		widget->setVisible(true);
		widget->setIsNeedRemove(false);

		pLayer->addWidget(widget);
		int tag = EMSTag_Ui + m_baseWidgets.size();
		if ( strcmp(widget->getName(), "MessageBox") == 0 )
		{
			m_mainScene->addChild(pLayer, EMSTag_Msg, EMSTag_Msg);
			pLayer->setTouchPriority(-EMSTag_Msg);
		
			if (m_msgBoxWidget)
			{
				m_msgBoxWidget->release();
			}
			m_msgBoxWidget = widget;
			m_msgBoxWidget->retain();
		}
		else if (strcmp("MainUiTop", widget->getName()) == 0)
		{
		}
		else
		{
			if (strcmp(widget->getName(), "TaskSystem") == 0)
			{
				for (size_t i=1; i<m_baseWidgets.size(); i++)
				{
					m_baseWidgets[i]->setVisible(false);
				}
			}
			pLayer->setTouchPriority(-tag);
			m_mainScene->addChild(pLayer, tag, tag);
			AddBaseWidget(widget);

			//pLayer->setTouchEnabled(false);
			//CCActionInterval* delay1 = CCDelayTime::create(0.3f);
			//CCActionInstant* callBack = CCCallFuncN::create(this, callfuncN_selector(UIMgr::SetTouch));
			//pLayer->runAction(CCSequence::create(delay1, callBack, NULL));
		}

		if (IsSpecialUI(widget) || strcmp("MyTerritory", widget->getName()) == 0 || strcmp("Territory", widget->getName()) == 0 || strcmp("MainUiTop", widget->getName()) == 0 || strcmp("MainRight", widget->getName()) == 0)
		{
			if (frameSize.height/designSize.height > frameSize.width/designSize.width)
			{
				CCSize uiSize = CCSizeMake(designSize.width*scaleFactMin,designSize.height*scaleFactMin);
				pLayer->setContentSize(uiSize);
				pLayer->setAnchorPoint(ccp(0,0));
				widget->setScale(scaleFactMin);
				CCPoint pos = ccp((frameSize.width - uiSize.width)/2,(frameSize.height - uiSize.height)/2);
				pLayer->setPosition(pos);
			}
			else
			{
				widget->setScaleX(frameSize.width/designSize.width);
				widget->setScaleY(frameSize.height/designSize.height);

				pLayer->setContentSize(frameSize);
				//pLayer->setPosition(ccp((frameSize.width - designSize.width)/2 * pLayer->getScaleX(),(frameSize.height - designSize.height )/2 * pLayer->getScaleY()));
				pLayer->setAnchorPoint(ccp(0,0));
			}
			
			
			ccArray* arrayItems = widget->getChildren()->data;
			int length = arrayItems->num;
			for (int i=0; i<length; i++)
			{	
				Widget* item = static_cast<Widget*>(arrayItems->arr[i]);

				if (strcmp("BG_1", item->getName()) != 0 && strcmp("BG_2", item->getName()) != 0 && strcmp("BG_3", item->getName()) != 0 && strcmp("BG", item->getName()) != 0)
				{
					item->setScaleX(scaleFactMin/widget->getScaleX());
					item->setScaleY(scaleFactMin/widget->getScaleY());
				}
				else
				{
					ccArray* arrayItems = item->getChildren()->data;
					for (int j=0; j<arrayItems->num; j++)
					{	
						Widget* item = static_cast<Widget*>(arrayItems->arr[j]);

						if (strcmp("BG_1", item->getName()) != 0 && strcmp("BG_2", item->getName()) != 0 && strcmp("BG_3", item->getName()) != 0 && strcmp("BG_4", item->getName()) != 0)
						{
							item->setScaleX(scaleFactMin/widget->getScaleX());
							item->setScaleY(scaleFactMin/widget->getScaleY());
						}	
					}
				}
			}

			//SetChildrenScale(widget, scaleFactMin);
		}
		else
		{	
			//pLayer->setScale(scaleFactMin);
			CCSize uiSize = CCSizeMake(designSize.width*scaleFactMin,designSize.height*scaleFactMin);
			pLayer->setContentSize(uiSize);
			pLayer->setAnchorPoint(ccp(0,0));
			widget->setScale(scaleFactMin);
			CCPoint pos = ccp((frameSize.width - uiSize.width)/2,(frameSize.height - uiSize.height)/2);
			widget->setPosition(pos);
			
			Widget* bgWidget = widget->getChildByName("BG");
			if (bgWidget)
			{	
				bgWidget->setAnchorPoint(ccp(0.5, 0.5));
				bgWidget->setPosition(ccp(480, 320));
				bgWidget->setScale(scaleFactMax/scaleFactMin+0.05f);
				bgWidget->setTouchEnabled(true);
				bgWidget->setVisible(true);

				if (bgWidget->getDescription() == "Layout")
					ScriptSys::GetInstance().Execute_1("UI_AddCloseEvent", bgWidget, "Widget");
			}	
		}	

		if (strcmp("MainUi", widget->getName()) == 0)
		{
			ScriptSys::GetInstance().Execute_2("UI_AddSimpleChat", widget, "Widget", pLayer, "TouchGroup");	
		}
		if (SceneMgr::GetInstance().GetGameState() == EGameState_MainUi)
			ScriptSys::GetInstance().Execute_1("UI_AddMainUiTop", widget, "Widget");

		PlayOpenAction(widget, action, time);
	}

	//RefreshSimpleChat();
	return widget;
}

void UIMgr::PlayOpenAction(Widget* widget, int action, float time, float delayTime)
{
	//float time = 0.25f;
	CCPoint pos = widget->getPosition();
	CCSize size = widget->getSize();
	CCPoint anchor = widget->getAnchorPoint();
	float scaleX = widget->getScaleX();
	float scaleY = widget->getScaleY();

	CCActionInterval* pAction = NULL;

	switch (action)
	{
	case EUIOpenAction_None:
		return;
	case EUIOpenAction_Enlarge:
		{
			CCPoint tmpPos1 = pos + ccp((0.5 - anchor.x)*size.width*scaleX, (0.5 - anchor.y)*size.height*scaleY);
			widget->setPosition(tmpPos1);
			widget->setScale(0.001f);
			CCActionInterval* move = CCEaseBackOut::create(CCMoveTo::create(time, pos));
			CCActionInterval* action = CCEaseBackOut::create(CCScaleTo::create(time, scaleX, scaleY));
			//CCActionInstant* callBack = CCCallFuncN::create(this, callfuncN_selector(UIMgr::ShowBg));
			pAction = CCSpawn::create(move, action, NULL);

			Widget* bgWidget = widget->getChildByName("BG");
			if (bgWidget)
			{	
				float scale = bgWidget->getScale();
				bgWidget->setScale(1000*scale);
				bgWidget->runAction(CCScaleTo::create(time, scale));
			}	

			//widget->runAction(move);
			//widget->runAction(action);
		}
		break;
	case EUIOpenAction_MoveIn_Left:
		{
			widget->setPosition(ccp(pos.x - size.width, pos.y));
			pAction = CCEaseBackOut::create(CCMoveTo::create(time, pos));
		}
		break;
	case EUIOpenAction_MoveIn_Right:
		{
			widget->setPosition(ccp(pos.x + size.width, pos.y));
			pAction = CCEaseBackOut::create(CCMoveTo::create(time, pos));
		}
		break;
	case EUIOpenAction_MoveIn_Top:
		{
			widget->setPosition(ccp(pos.x, pos.y + size.height));
			pAction = CCEaseBounceOut::create(CCMoveTo::create(time, pos));
		}
		break;
	case EUIOpenAction_MoveIn_Bottom:
		{
			widget->setPosition(ccp(pos.x,pos.y - size.height));
			pAction = CCEaseBackOut::create(CCMoveTo::create(time, pos));
		}
		break;
	case EUIOpenAction_FadeIn:
		{
			widget->setOpacity(0);
			pAction = CCFadeIn::create(time);
		}
		break;
	case EUIOpenAction_Move_Right_Left:
		{
			CCActionInterval* move1 = CCEaseExponentialOut::create(CCMoveBy::create(time/2, ccp(size.width,0)));
			CCActionInterval* move2 = CCEaseExponentialOut::create(CCMoveBy::create(time/2, ccp(-size.width, 0)));

			pAction = CCSequence::create(move1, move2, NULL);
		}
		break;
	case EUIOpenAction_Reel:
		{
			widget->setOpacity(0);

			CCActionInterval* delay = CCDelayTime::create(time);
			CCActionInterval* fadeIn = CCFadeIn::create(0.5);

			pAction = CCSequence::create(delay, fadeIn, NULL);

			LoadingBar* temp = LoadingBar::create();
			temp->setPosition(ccp(476, 311));
			temp->loadTexture("zhuxianfuben/bg_1.png");
			temp->setDirection(LoadingBarTypeRight);
			temp->setPercent(0);
			temp->loadLeftRightTexture("zhuxianfuben/bg_left.png", "zhuxianfuben/bg_right.png", 25);
			CCActionInterval* delay1 = CCDelayTime::create(time + 0.5);
			CCActionInstant* callBack = CCCallFuncN::create(this, callfuncN_selector(UIMgr::ShowBg));
			temp->runAction(CCSequence::create(delay1, callBack, NULL));


			temp->setPercentSlowly(100, time);
			widget->getParent()->addChild(temp, -1, -1);
		}
		break;
	}

	if (delayTime > 0)
	{
		CCActionInterval* delay = CCDelayTime::create(delayTime);
		widget->runAction(CCSequence::create(delay, pAction, NULL));
	}
	else
	{
		widget->runAction(pAction);
	}
	
}

void UIMgr::SetChildrenScale(Widget* widget, float s)
{
	ccArray* arrayItems = widget->getChildren()->data;
	int length = arrayItems->num;
	if (length == 0)
	{
		return;
	}

	for (int i=0; i<length; i++)
	{	
		Widget* item = static_cast<Widget*>(arrayItems->arr[i]);

		if (item)
		{
			item->setScale(s);
			if (item->getChildrenCount() > 0)
			{
				SetChildrenScale(item, s);
			}
		}
	}
}

void UIMgr::CloseCurBaseWidget(int action, float time)
{
	CheckNeedClose();	
	Widget* widget = GetCurWidget();

	if (widget && IsSpecialUI(widget) == false && widget->getIsNeedRemove() == false)
	{
		Widget* bgWidget = widget->getChildByName("BG");
		if (bgWidget)
		{	
			bgWidget->setVisible(false);
		}	

		CCPoint pos = widget->getPosition();
		CCSize size = widget->getSize();
		CCPoint anchor = widget->getAnchorPoint();
		widget->setIsNeedRemove(true); 

		if (SceneMgr::GetInstance().GetGameState() == EGameState_MainUi)
		{
			ScriptSys::GetInstance().Execute("UI_RefreshMainUiTop");
		}

		if (widget->getParent() && widget->getParent()->getParent())
		{
			TouchGroup* pLayer = dynamic_cast<TouchGroup*>(widget->getParent()->getParent());
			if (pLayer)
			{
				pLayer->setTouchEnabled(false);
			}
		}

		switch (action)
		{
		case EUICloseAction_None:
			RemoveBaseWidget(widget);

			//RefreshSimpleChat();
			break;
		case EUICloseAction_Lessen:
			{
				CCCallFunc* endCall = CCCallFunc::create(this, callfunc_selector(UIMgr::UICloseActionCallBack));
				CCActionInterval* action1 = CCEaseBackIn::create(CCMoveBy::create(time, ccp((0.5 - anchor.x)*size.width, (0.5 - anchor.y)*size.height)));
				CCActionInterval* action2 = CCEaseBackIn::create(CCScaleTo::create(time, 0.001f));
				widget->runAction(action1);
				widget->runAction(CCSequence::create(action2, endCall, NULL));
			}
			break;
		case EUICloseAction_MoveOut_Left:
			{
				CCCallFunc* endCall = CCCallFunc::create(this, callfunc_selector(UIMgr::UICloseActionCallBack));
				CCActionInterval* action1 = CCEaseExponentialIn::create(CCMoveBy::create(time, ccp(-size.width, 0)));
				widget->runAction(CCSequence::create(action1, endCall, NULL));
			}
			break;
		case EUICloseAction_MoveOut_Right:
			{
				CCCallFunc* endCall = CCCallFunc::create(this, callfunc_selector(UIMgr::UICloseActionCallBack));
				CCActionInterval* action1 = CCEaseExponentialIn::create(CCMoveTo::create(time, ccp(size.width, 0)));
				widget->runAction(CCSequence::create(action1, endCall, NULL));
			}
			break;
		case EUICloseAction_MoveOut_Top:
			{
				CCCallFunc* endCall = CCCallFunc::create(this, callfunc_selector(UIMgr::UICloseActionCallBack));
				CCActionInterval* action1 = CCEaseExponentialIn::create(CCMoveTo::create(time, ccp(0, size.height)));
				widget->runAction(CCSequence::create(action1, endCall, NULL));
			}
			break;
		case EUICloseAction_MoveOut_Bottom:
			{
				CCCallFunc* endCall = CCCallFunc::create(this, callfunc_selector(UIMgr::UICloseActionCallBack));
				CCActionInterval* action1 = CCEaseExponentialIn::create(CCMoveTo::create(time, ccp(0, -size.height)));
				widget->runAction(CCSequence::create(action1, endCall, NULL));
			}
			break;
		case  EUICloseAction_FadeOut:
			{
				CCCallFunc* endCall = CCCallFunc::create(this, callfunc_selector(UIMgr::UICloseActionCallBack));
				CCActionInterval* action1 = CCFadeOut::create(time);
				widget->runAction(CCSequence::create(action1, endCall, NULL));
			}
			break;
		case  EUICloseAction_Reel:
			{
				widget->setVisible(false);
				LoadingBar* temp = LoadingBar::create();
				temp->setPosition(ccp(476, 311));
				temp->loadTexture("zhuxianfuben/bg_1.png");
				temp->setDirection(LoadingBarTypeRight);
				temp->setPercent(100);
				temp->loadLeftRightTexture("zhuxianfuben/bg_left.png", "zhuxianfuben/bg_right.png", 25);

				temp->setPercentSlowly(0, time);
				widget->getParent()->addChild(temp, -1, -1);

				CCCallFunc* endCall = CCCallFunc::create(this, callfunc_selector(UIMgr::UICloseActionCallBack));
				CCActionInterval* action1 = CCDelayTime::create(time);
				widget->runAction(CCSequence::create(action1, endCall, NULL));
			}
			break;
		}
		
	}
}

void UIMgr::CloseAllSpecial()
{
	while (m_baseWidgets.size() > 0)
	{
		if (IsSpecialUI(GetCurWidget()) || strcmp(GetCurWidget()->getName(), "DungeonList") == 0 || strcmp(GetCurWidget()->getName(), "Dungeon") == 0 || strcmp(GetCurWidget()->getName(), "Arena") == 0)
		{
			CCLOG(GetCurWidget()->getName());
			break;
		}
		RemoveBaseWidget(GetCurWidget());
	}

	if (SceneMgr::GetInstance().GetGameState() == EGameState_MainUi && m_baseWidgets.size() > 0)
		ScriptSys::GetInstance().Execute_1("UI_AddMainUiTop", GetCurWidget(), "Widget");
}

void UIMgr::CloseAllBaseWidget(bool isMain)
{
	if (isMain)
	{
		RemoveAllBaseWidget();
		/*for (map<string, Widget*>::iterator it=m_cacheWidgets.begin(); it!=m_cacheWidgets.end(); it++)
		{
			Widget* widget = it->second;
			if (widget != NULL)
			{
				widget->release();
			}
		}

		m_cacheWidgets.clear();*/
	}
	else
	{
		while (m_baseWidgets.size() > 0)
		{
			if (IsSpecialUI(GetCurWidget()))
			{
				break;
			}
			RemoveBaseWidget(GetCurWidget());
		}
	}
	
	//RefreshSimpleChat();
}
/*
void UIMgr::CloseBaseWidgetByName( string& name )
{

}
*/
bool UIMgr::IsCurBaseWidget( Widget* widget )
{
	int size = m_baseWidgets.size();
	if (size > 0 && widget == m_baseWidgets[size - 1])
	{
		return true;
	}

	return false;
}

bool UIMgr::IsCurBaseWidgetByName( string name )
{
	int size = m_baseWidgets.size();
	if (size > 0 && name == m_baseWidgets[size - 1]->getName())
	{
		return true;
	}

	return false;
}

bool UIMgr::HasBaseWidget( string name )
{
	for (size_t i=0; i<m_baseWidgets.size(); i++)
	{
		if (m_baseWidgets[i] && name == m_baseWidgets[i]->getName())
		{
			return true;
		}
	}

	return false;
}

void UIMgr::AddBaseWidget( Widget* widget )
{
	if (widget)
	{
		m_baseWidgets.push_back(widget);
	}
	ScriptSys::GetInstance().Execute("Guide_Update");
}

void UIMgr::RemoveBaseWidget( Widget* widget )
{
	int size = m_baseWidgets.size();
	if (size > 0 && widget == m_baseWidgets[size - 1])
	{
		if (strcmp(widget->getName(), "TaskSystem") == 0)
		{
			for (size_t i=1; i<m_baseWidgets.size(); i++)
			{
				m_baseWidgets[i]->setVisible(true);
			}
		}

		m_mainScene->removeChildByTag(EMSTag_Ui + size - 1);
		m_baseWidgets.erase(--m_baseWidgets.end());
	}

	ScriptSys::GetInstance().Execute("Guide_Update");
}

void UIMgr::CloseMsgBaseWidget()
{
	if (m_msgBoxWidget && m_msgBoxWidget->getIsNeedRemove() == false)
	{
		m_msgBoxWidget->setIsNeedRemove(true);
		CCCallFunc* endCall = CCCallFunc::create(this, callfunc_selector(UIMgr::UICloseMsgCallBack));
		//CCActionInterval* action1 = CCEaseBackIn::create(CCMoveBy::create(0.3f, ccp(480, 320)));
		//CCActionInterval* action2 = CCEaseBackIn::create(CCScaleTo::create(0.3f, 0.001f));
		CCActionInterval* action1 = CCFadeOut::create(0.1f);
		//m_msgBoxWidget->runAction(action1);
		m_msgBoxWidget->runAction(CCSequence::create(action1, endCall, NULL));

		if (SceneMgr::GetInstance().GetGameState() == EGameState_MainUi)
		{
			ScriptSys::GetInstance().Execute("UI_RefreshMainUiTop");
		}
	}
}

void UIMgr::RemoveAllBaseWidget()
{
	for (size_t i=0; i<m_baseWidgets.size(); i++)
	{
		m_mainScene->removeChildByTag(EMSTag_Ui + i);
	}

	m_baseWidgets.clear();
}

Widget* UIMgr::GetCurWidget()
{
	int size = m_baseWidgets.size();
	if (size > 0)
	{
		return m_baseWidgets[size - 1];
	}

	return NULL;
}

UIMgr::~UIMgr()
{
	ShutDown();
}

void UIMgr::RefreshCurBaseWidget()
{
	Widget* widget = GetCurWidget();
	if (widget)
	{
		widget->setRefresh(true);
		//RefreshBaseWidget(widget);
	}
}

void UIMgr::RefreshBaseWidget( Widget* widget )
{
	if (widget)
	{
		widget->setRefresh(false);
		ScriptSys::GetInstance().Execute_1(StringMgr::GetInstance().Format("%s_Refresh", widget->getName()).c_str(), widget, "Widget");
	}
}

void UIMgr::RefreshAllBaseWidget()
{
	for (size_t i=0; i<m_baseWidgets.size(); i++)
	{
		if (m_baseWidgets[i] && IsSpecialUI(m_baseWidgets[i]) == false)
		{
			m_baseWidgets[i]->setRefresh(true);
			//RefreshBaseWidget(m_baseWidgets[i]);
		}
	}
}

void UIMgr::RefreshBaseWidgetByName( string name )
{
	Widget* widget = GetBaseWidgetByName(name);
	if (widget)
	{
		widget->setRefresh(true);
		//RefreshBaseWidget(widget);
	}
}

void UIMgr::Update( int delta )
{
	if (m_baseWidgets.size() > 0 && strcmp("MainUi", m_baseWidgets[0]->getName()) == 0)
	{
		bool isDisplay = true;
		if (m_baseWidgets.size() > 1)
		{
			isDisplay = false;
		}

		ccArray* arrayItems = m_baseWidgets[0]->getChildren()->data;
		int length = arrayItems->num;
		for (int i=0; i<length; i++)
		{	
			Widget* item = static_cast<Widget*>(arrayItems->arr[i]);

			if (strcmp("BG", item->getName()) != 0 
				&& strcmp("BG_1", item->getName()) != 0 
				&& strcmp("BG_2", item->getName()) != 0
				&& strcmp("BG_3", item->getName()) != 0 
				&& strcmp("BG_4", item->getName()) != 0
				//&& item->getTag() != 21
				//&& item->getTag() != 22
				//&& item->getTag() != 23
				)
			{
				item->setVisible(isDisplay);
			}
		}

		Widget* bg_1 = dynamic_cast<Widget*>(m_baseWidgets[0]->getChildByTag(100));
		ScrollView* bg = dynamic_cast<ScrollView*>(m_baseWidgets[0]->getChildByTag(1));
		Widget* bg_2 = dynamic_cast<Widget*>(m_baseWidgets[0]->getChildByTag(101));
		if (bg_1 && bg && bg_2)
		{
			CCPoint pos = bg->getInnerContainer()->getPosition();
			bg_1->setPosition(ccp(pos.x/1.5, 0));
			bg_2->setPosition(ccp(pos.x/3, 0));
		}
	}
	
	for (size_t i=0; i<m_baseWidgets.size(); i++)
	{
		if (m_baseWidgets[i] && m_baseWidgets[i]->getRefresh())
		{
			RefreshBaseWidget(m_baseWidgets[i]);
		}
	}
}
Widget* UIMgr::GetStarLayout(int starCount){
	Widget *layOut = Widget::create();

    cocos2d::ui::UIImageView* image = cocos2d::ui::UIImageView::create();
	image->loadTexture("HeroSystem/star_01.png"); 
	float width = image->getContentSize().width*starCount;
	layOut->setContentSize(CCSize(width,image->getContentSize().height));
	layOut->setAnchorPoint(ccp(0.5,0.5));
	for (size_t i  = 0; i < starCount; ++i)
	{
		cocos2d::ui::UIImageView* lImage = dynamic_cast<cocos2d::ui::UIImageView*>(image->clone());
		lImage->setAnchorPoint(ccp(0,0.5));
		CCPoint pos;
		pos.x = i*image->getContentSize().width;
		pos.y = image->getContentSize().height/2.0;
		lImage->setPosition(pos);
		layOut->addChild(lImage);
	}
	return layOut;
}

void UIMgr::UICloseActionCallBack()
{
	Widget* widget = GetCurWidget();
	if (widget && widget->getIsNeedRemove())
	{
		widget->stopAllActions();
		RemoveBaseWidget(widget);

		//RefreshSimpleChat();
	}
}

void UIMgr::UICloseMsgCallBack()
{
	if (m_msgBoxWidget && m_msgBoxWidget->getIsNeedRemove())
	{
		m_msgBoxWidget->stopAllActions();
		m_mainScene->removeChildByTag(EMSTag_Msg);
		m_msgBoxWidget->release();
		m_msgBoxWidget = NULL;

		ScriptSys::GetInstance().Execute("Messagebox_CallBack");
	}
}

UILayer* UIMgr::getMessageBoxLayer()
{
	if(m_msgBoxWidget)
	{
		UILayer* pRet = GetUILayarByWidget(m_msgBoxWidget);
		return pRet;
	}

	return NULL;
}

void UIMgr::ShowBg( CCNode* node )
{
	Widget* widget = dynamic_cast<Widget*>(node);
	if (widget)
	{
		Widget* bgWidget = widget->getChildByName("BG");
		if (bgWidget)
		{	
			bgWidget->setVisible(true);
		}	
	}
}

void UIMgr::AddWidgetInToCacheList( string name, Widget* widget )
{
	m_cacheWidgets[name] = widget;
}

UILayer* UIMgr::GetUILayarByWidget( Widget* widget )
{
	if (widget->getParent() && widget->getParent()->getParent())
	{
		return static_cast<UILayer*>(widget->getParent()->getParent());
	}
	return NULL;
}

CCNode * UIMgr::createMaskedSprite(CCSprite* src, const char* maskFile)
{
	CCSprite * mask = CCSprite::create(maskFile); 

	assert(src); 
	assert(mask); 

	CCSize srcContent = src->getContentSize(); 
	CCSize maskContent = mask->getContentSize(); 

	CCRenderTexture * rt = CCRenderTexture::create(srcContent.width, srcContent.height, kTexture2DPixelFormat_RGBA8888); 

	float ratiow = srcContent.width / maskContent.width; 
	float ratioh = srcContent.height / maskContent.height; 
	mask->setScaleX(ratiow); 
	mask->setScaleY(ratioh); 

	mask->setPosition(ccp(srcContent.width / 2, srcContent.height / 2)); 
	src->setPosition(ccp(srcContent.width / 2, srcContent.height / 2)); 

	ccBlendFunc blendFunc1 = { GL_ONE, GL_ZERO }; 
	mask->setBlendFunc(blendFunc1); 
	ccBlendFunc blendFunc2 = { GL_DST_ALPHA, GL_ZERO }; 
	src->setBlendFunc(blendFunc2); 

	rt->begin(); 
	mask->visit(); 
	src->visit(); 
	rt->end(); 

	//rt->listenToBackground(NULL);
	/*CCSprite * retval = CCSprite::createWithTexture(rt->getSprite()->getTexture()); 
	retval->setFlipY(true); 
	*/
	

	return rt;
}

void UIMgr::SetTouch( CCNode* node )
{
	TouchGroup* player = dynamic_cast<TouchGroup*>(node);
	if (player)
	{
		player->setTouchEnabled(true);
	}
}

Layout* UIMgr::getCloneHeroListItemTemplate()
{
	Layout *layout = Layout::create();
	ui::Layout * widget = static_cast<Layout*>(GUIReader::shareReader()->widgetFromJsonFile("UIExport/DemoLogin/HeroListItemTemplate.json"));
	Layout *templayout = dynamic_cast<Layout *>(widget->getChildByTag(1));
	layout = dynamic_cast<Layout *>(templayout->clone());
	return layout;
}

Widget* UIMgr::getWidgetFromFile(string fileName)
{
	return GUIReader::shareReader()->widgetFromJsonFile(fileName.c_str());
}

NS_FM_END
