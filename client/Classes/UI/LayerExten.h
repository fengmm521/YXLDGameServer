/********************************************************************
created:	2012-12-07
author:		SXH
summary:	对CCLayer扩展，主要提供游戏layer常用到的效果及其他扩展
*********************************************************************/
#pragma once
#include "FmPlatform.h"
#include "cocos2d.h"

USING_NS_CC;

enum EExtenLayerZorder
{
	EExtenLayerZorder_MaxDepth = 10000,
};

enum EExtenLayerTag
{
	EExtenLayerTag_BgColor = 100,
	EExtenLayerTag_BgTTFlabel = 101,
};
class LayerExten
	: public cocos2d::CCLayer
{
protected:
	float m_dtTime;
	ccColor4B m_color;
public:
	static LayerExten *create(void);

	LayerExten();
	virtual ~LayerExten();
	
	/*
		设置layer变灰效果 
			enable True变灰  False恢复
			onTop True该层最上面变灰  False该层底部变灰 Z 深度 EExtenLayerZorder_MaxDepth
	*/
	void SetBlendGray(bool enable,bool onTop);
	void UpdateBlendFun(float dt);

	void SetBgColor(const ccColor4B& color,bool enable);


	void SetDtTime( float time )
	{
		m_dtTime = time;
	}
	void SetBgColor(const ccColor4B& color)
	{
		m_color = color;
	}
	void StartBlendGray();
	void EndBlendGray();
	//virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);

};

class LifeNode:public CCLayer
{
public:
	static LifeNode* create(float life);
	static LifeNode* create();
	static void UpdateAllLifeNode(uint dt);
public:
	//void UpdateLife(float dt);
	void SetLife(float dt);
public:
	int m_Life;
	int		m_uId;
};

class TouchEnableLayer:public CCLayerColor
{
public:
	static TouchEnableLayer *create(void);

	TouchEnableLayer();
	virtual ~TouchEnableLayer();

	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
};
//
//class LightSprite:public CCSprite
//{
//public:
//	static LightSprite* create(const char* path);
//public:
//
//	LightSprite();
//	virtual ~LightSprite();
//
//	virtual void draw();
//
//	void SetMaskLayer(CCRenderTexture* node ){m_MaskLayer = node;};
//private:
//	CCRenderTexture* m_MaskLayer;
//
//};
//
//class LightMaskLayer:public CCNode
//{
//public:
//	static LightMaskLayer* create(const CCSize& size,const char* path);
//
//	LightMaskLayer();
//	virtual ~LightMaskLayer();
//
//public:
//	bool InitWithSizeAndSprite(const CCSize& size,const char* path);
//
//};
