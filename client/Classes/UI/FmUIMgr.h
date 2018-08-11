#pragma once

#include "FmConfig.h"
#include "cocos2d.h"
#include "cocos-ext.h"
#include "Scene/FmMainScene.h"

NS_FM_BEGIN

USING_NS_CC;
USING_NS_CC_EXT;

using namespace gui;
typedef vector<Widget*> UIWidgetList;

class UIMgr : public CCObject
{

public:
	static void ShowWaiting(float time);
	static void EndWaiting();
protected:
	UIWidgetList	m_baseWidgets;
	MainScene* m_mainScene;

	Widget* m_msgBoxWidget;
	void AddBaseWidget(Widget* widget);
	void RemoveBaseWidget(Widget* widget);
	void RemoveAllBaseWidget();
	void RefreshBaseWidget(Widget* widget);
	UIMgr();
	map<string, Widget*>	m_cacheWidgets;
public:
	SINGLETON_MODE( UIMgr );

	// 初始化
	bool Init();

	void SetMainScene(MainScene* scene){m_mainScene = scene;}

	vector<Widget*>& GetWidgetList(){return m_baseWidgets;} 
	// 获取UI
	Widget* GetBaseWidgetByName( string widgetName );
	Widget* GetCurWidget();

	UILayer* GetUILayarByWidget(Widget* widget);

	UILayer* getMessageBoxLayer();

	Widget* CreateBaseWidgetByFileName( string fileName, int action = EUIOpenAction_None, float time = 0.25f);
	void CloseCurBaseWidget(int action = EUICloseAction_None, float time = 0.3f);
	void CloseAllBaseWidget(bool isMain = false);
	//void CloseBaseWidgetByName(string& name);

	bool IsCurBaseWidget(Widget* widget);
	bool IsCurBaseWidgetByName(string name);
	bool HasBaseWidget(string name);

	void RefreshCurBaseWidget();
	void RefreshAllBaseWidget();
	void RefreshBaseWidgetByName(string name);

	void Update(int delta);

	void ShutDown();
	~UIMgr();
	void SetChildrenScale(Widget* widget, float s);

	//获取玩家的星级Layout
	Widget* GetStarLayout(int starCount);
	bool IsSpecialUI(Widget* widget);
	void RefreshSimpleChat();
	void CheckNeedClose();

	void UICloseActionCallBack();
	void PlayOpenAction(Widget* widget, int action, float time, float delayTime = 0.0f);
	void CloseMsgBaseWidget();
	void UICloseMsgCallBack();
	void ShowBg(CCNode* node);

	map<string, Widget*>& GetCacheWidgets(){return m_cacheWidgets;}

	void AddWidgetInToCacheList(string name, Widget* widget);

	CCNode * createMaskedSprite(CCSprite* src, const char* maskFile);

	void SetTouch(CCNode* node);

	Layout* getCloneHeroListItemTemplate();
	Widget* getWidgetFromFile(string fileName );
	void CloseAllSpecial();
};
NS_FM_END