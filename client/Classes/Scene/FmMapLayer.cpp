#include "FmMapLayer.h"

USING_NS_CC;
NS_FM_BEGIN

MapLayer * MapLayer::create( const char* file )
{
	MapLayer* layer = new MapLayer;
	if (layer && layer->init(file))
	{
		layer->autorelease();

		return layer;
	}
	else
	{
		FM_SAFE_DELETE(layer);
		return NULL;
	}
}

bool MapLayer::init( const char* file )
{
	// 先添加背景地图信息，加速Loading
	TextureDesc desc;
	desc.textureRect = CCSize(960, 640);
	desc.iPixelWidth = 960;
	desc.iPixelHeight = 640;
	desc.hasPremultipliedAlpha = false;
	desc.bPVRHaveAlphaPremultiplied = true;
	desc.pixelFormat = kCCTexture2DPixelFormat_RGB888;
	desc.fMaxS = 1.0f;
	desc.fMaxT = 1.0f;
	desc.bHasMipmaps = false;
	CCTextureCache::sharedTextureCache()->addImageDesc(file, desc);

	CCSprite *map = CCSprite::create(file);
	if (map)
	{
		m_ScreenSize = map->getContentSize();
		CCSize designSize = CCSize(960, 640);
		CCSize frameSize = CCEGLView::sharedOpenGLView()->getFrameSize();
		float scaleFactMax = frameSize.width/designSize.width;//MAX(frameSize.width/designSize.width,frameSize.height/designSize.height);

		map->setScale(scaleFactMax);
		CCSize mapSize = CCSizeMake(designSize.width*scaleFactMax,designSize.height*scaleFactMax);
		map->setContentSize(mapSize);
		map->setAnchorPoint(ccp(0,0));
		//左下角坐标
		map->setPosition(ccp((frameSize.width - mapSize.width)/2,(frameSize.height - mapSize.height)/2));
		
		this->addChild(map, 1, 1);
		m_file = file;
		return true;
	}

	CCLOG("Loading map error!~~~~~~~~~");
	return false;
}

void MapLayer::changeNextMap(const char* file)
{
	CCSprite *map = CCSprite::create(file);
	if (map)
	{
		m_ScreenSize = map->getContentSize();
		CCSize designSize = CCSize(960, 640);
		CCSize frameSize = CCEGLView::sharedOpenGLView()->getFrameSize();
		float scaleFactMax = frameSize.width/designSize.width;//MAX(frameSize.width/designSize.width,frameSize.height/designSize.height);

		map->setScale(scaleFactMax);
		CCSize mapSize = CCSizeMake(designSize.width*scaleFactMax,designSize.height*scaleFactMax);
		map->setContentSize(mapSize);
		map->setAnchorPoint(ccp(0,0));
		//左下角坐标
		map->setPosition(ccp((frameSize.width - mapSize.width)/2,(frameSize.height - mapSize.height)/2));

		this->removeChildByTag(1);
		this->addChild(map, 1, 1);
		m_file = file;
	}
}

NS_FM_END