
#include "LayerExten.h"

LayerExten * LayerExten::create( void )
{
	LayerExten *pRet = new LayerExten();
	if (pRet && pRet->init())
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		CC_SAFE_DELETE(pRet);
		return NULL;
	}
}


LayerExten::LayerExten()
{
	m_dtTime = 0.05f;
	m_color.a = 0;
	m_color.r = 0;
	m_color.g = 0;
	m_color.b = 0;
}

LayerExten::~LayerExten()
{

}

void LayerExten::SetBlendGray( bool enable ,bool onTop)
{
	if(enable)
	{
		if(getChildByTag(EExtenLayerTag_BgColor) != NULL)
		{
			return;
		}
		CCLayerColor* colorLayer = CCLayerColor::create( ccc4(0, 0, 0, 0) );
		int z = onTop?EExtenLayerZorder_MaxDepth:-EExtenLayerZorder_MaxDepth;
		addChild(colorLayer,z,EExtenLayerTag_BgColor);
		colorLayer->runAction(  CCFadeIn::create( m_dtTime/2 ) );
		colorLayer->runAction(  CCFadeOut::create( m_dtTime/2 ) );
		//schedule( schedule_selector(LayerExten::UpdateBlendFun), m_dtTime);
	}
	else
	{
		//colorLayer->runAction( CCFadeOut::create(m_dtTime/2));
		//unschedule(schedule_selector(LayerExten::UpdateBlendFun));
		CCLayerColor *layer = (CCLayerColor*)getChildByTag(EExtenLayerTag_BgColor);
		if ( layer )
		{
			layer->runAction(  CCFadeOut::create( m_dtTime/2 ) );
		}
		removeChildByTag(EExtenLayerTag_BgColor,false);
	}
}

void LayerExten::UpdateBlendFun( float dt )
{
	CCLayerColor *layer = (CCLayerColor*)getChildByTag(EExtenLayerTag_BgColor);
	bool removeSchedule = false;
	if(layer != NULL)
	{
		GLubyte opicaty = layer->getOpacity();
		opicaty += 40;
		if(opicaty > 220)
		{
			opicaty = 220;
			removeSchedule = true;
		}
		layer->setOpacity(opicaty);
	}
	else
	{
		removeSchedule = true;
	}
	
	if(removeSchedule)
	{
		unschedule(schedule_selector(LayerExten::UpdateBlendFun));
	}
}


void LayerExten::SetBgColor( const ccColor4B& color,bool enable )
{
	if(enable)
	{
		if(getChildByTag(EExtenLayerTag_BgColor) != NULL)
		{
			return;
		}
		CCLayerColor* colorLayer = CCLayerColor::create( color );
		addChild(colorLayer,-EExtenLayerZorder_MaxDepth,EExtenLayerTag_BgColor);
	}
	else
	{
		removeChildByTag(EExtenLayerTag_BgColor,false);
	}
}

void LayerExten::StartBlendGray()
{
	if ( m_dtTime == 0 )
	{
		m_color.a = 255;
	}
	if(getChildByTag(EExtenLayerTag_BgColor) != NULL)
	{
		removeChildByTag(EExtenLayerTag_BgColor,true);
	}
	CCLayerColor* colorLayer = CCLayerColor::create( m_color );
	addChild(colorLayer,10,EExtenLayerTag_BgColor);
	if ( m_dtTime > 0)
	{
		colorLayer->runAction(  CCFadeIn::create( m_dtTime ) );
	}
}

void LayerExten::EndBlendGray()
{
	CCLayerColor *layer = (CCLayerColor*)getChildByTag(EExtenLayerTag_BgColor);
	if ( !layer )
	{
		return;
	}
	CCLabelTTF* flable = (CCLabelTTF* )layer->getChildByTag( EExtenLayerTag_BgTTFlabel );
	if ( flable )
	{
		layer->removeChildByTag( EExtenLayerTag_BgTTFlabel,true );
	}
	layer->runAction(  CCFadeOut::create( m_dtTime ) );
	//removeChildByTag(EExtenLayerTag_BgColor,false);
}


//bool LayerExten::ccTouchBegan( CCTouch *pTouch, CCEvent *pEvent )
//{
//	if (!this->isVisible() || getParent() == NULL )
//	{
//		return false;
//	}
//	CCSize size = getContentSize();
//	CCPoint point = getPosition();
//	CCPoint frameOriginal = this->getParent()->convertToWorldSpace(this->getPosition());
//	CCRect frame = CCRectMake(frameOriginal.x, frameOriginal.y, size.width, size.height);
//
//	//dispatcher does not know about clipping. reject touches outside visible bounds.
//	if (!frame.containsPoint(this->convertToWorldSpace(this->convertTouchToNodeSpace(pTouch))))
//	{
//		return false;
//	}
//
//	return true;
//}
static int s_LifeNodeIndex = 0;

static std::map<int,LifeNode*> s_LifeNodes;
LifeNode* LifeNode::create( float life )
{
	LifeNode *pRet = new LifeNode();
	if (pRet)
	{
		pRet->m_uId = s_LifeNodeIndex++;
		pRet->SetLife(life);
		pRet->autorelease();
		return pRet;
	}
	else
	{
		CC_SAFE_DELETE(pRet);
		return NULL;
	}
}

LifeNode* LifeNode::create()
{
	LifeNode *pRet = new LifeNode();
	if (pRet)
	{
		pRet->m_uId = s_LifeNodeIndex++;
		pRet->autorelease();
		pRet->m_Life = -1;
		return pRet;
	}
	else
	{
		CC_SAFE_DELETE(pRet);
		return NULL;
	}
}


void LifeNode::SetLife( float life )
{
	if(life > 0)
	{
		m_Life = life*1000;
		s_LifeNodes[this->m_uId] = this;
		this->retain();
	}
	//schedule( schedule_selector(LifeNode::UpdateLife), 0.1f);
}

//void LifeNode::UpdateLife( float dt )
//{
//	m_Life -= dt;
//	if(m_Life <= 0)
//	{
//		removeFromParent();
//		map<int,LifeNode*>::iterator it = s_LifeNodes.find(m_uId);
//		if(it != s_LifeNodes.end())
//		{
//			s_LifeNodes.erase(it);
//			release();
//		}
//	}
//}

void LifeNode::UpdateAllLifeNode( uint dt )
{
	for(std::map<int,LifeNode*>::iterator it = s_LifeNodes.begin();it != s_LifeNodes.end();)
	{
		LifeNode* node = it->second;
		if(node)
		{
			node->m_Life -= dt;
			//CCLog("LifeNode : curLife %d, %d",node->m_Life,dt);
			if(node->m_Life <= 0)
			{
				node->removeFromParent();
				s_LifeNodes.erase(it++);
				node->release();
				continue;
			}
		}
		it++;
	}
}

//
//LightSprite* LightSprite::create( const char* path )
//{
//	LightSprite *pRet = new LightSprite();
//	if (pRet && pRet->initWithFile(path))
//	{
//		pRet->autorelease();
//		return pRet;
//	}
//	else
//	{
//		CC_SAFE_DELETE(pRet);
//		return NULL;
//	}
//}
//
//void LightSprite::draw()
//{
//	if(m_MaskLayer)
//		m_MaskLayer->begin();
//
//	// glClear(GL_COLOR_BUFFER_BIT);
//	glBlendFunc(GL_ZERO, 
//		GL_ONE_MINUS_SRC_ALPHA); //
//	glColorMask(0.0f, 0.0f, 0.0f, 
//		1.0f);//¹Ø¼ü¾ä
//#define kQuadSize sizeof(m_sQuad.bl)
//	if 
//		(m_pobTexture)
//	{
//		glBindTexture(GL_TEXTURE_2D, 
//			m_pobTexture->getName());
//	}
//	else
//	{
//		glBindTexture(GL_TEXTURE_2D, 
//			0);
//	}
//	long offset = (long)&m_sQuad;
//	// vertex
//		int diff = offsetof(ccV3F_C4B_T2F, 
//		vertices);
//	glVertexPointer(3, GL_FLOAT, kQuadSize, (void*)(offset + 
//		diff));
//	// color
//	diff = offsetof( ccV3F_C4B_T2F, 
//		colors);
//	glColorPointer(4, GL_UNSIGNED_BYTE, kQuadSize, (void*)(offset 
//		+ diff));
//	// tex coords
//	diff = offsetof( ccV3F_C4B_T2F, 
//		texCoords);
//	glTexCoordPointer(2, GL_FLOAT, kQuadSize, (void*)(offset + 
//		diff));
//	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
//
//	glColorMask(1.0f, 1.0f, 1.0f, 
//		1.0f);
//	if(m_MaskLayer)
//		m_MaskLayer->end();
//}
//
//LightSprite::LightSprite():m_MaskLayer(NULL)
//{
//
//}
//
//LightSprite::~LightSprite()
//{
//
//}
//
//bool LightMaskLayer::InitWithSizeAndSprite(const CCSize& size,const char* path)
//{
//	setContentSize(size);
//	CCRenderTexture * darknessLayer = CCRenderTexture::create(size.width, size.height);
//	darknessLayer->setPosition(ccp( size.width /2 , size.height/2 ));
//	addChild(darknessLayer,20,1);
//	darknessLayer->clear(0,0,0,0.5f);
//	LightSprite* light = LightSprite::create(path);
//	light->setPosition(ccp( size.width /2 , size.height/2 ));
//	light->SetMaskLayer(darknessLayer);
//	addChild(light,2,2);
//	//this->setScale(10);
//	return true;
//}
//
//LightMaskLayer* LightMaskLayer::create(const CCSize& size,const char* path)
//{
//	LightMaskLayer *pRet = new LightMaskLayer();
//	if (pRet && pRet->InitWithSizeAndSprite( size,path))
//	{
//		pRet->autorelease();
//		return pRet;
//	}
//	else
//	{
//		CC_SAFE_DELETE(pRet);
//		return NULL;
//	}
//}
//
//LightMaskLayer::LightMaskLayer()
//{
//
//}
//
//LightMaskLayer::~LightMaskLayer()
//{
//
//}

TouchEnableLayer * TouchEnableLayer::create( void )
{
	TouchEnableLayer *pRet = new TouchEnableLayer();
	if (pRet && pRet->init())
	{
		pRet->setTouchMode(kCCTouchesOneByOne);
		pRet->autorelease();
		return pRet;
	}
	else
	{
		CC_SAFE_DELETE(pRet);
		return NULL;
	}
}

bool TouchEnableLayer::ccTouchBegan( CCTouch *pTouch, CCEvent *pEvent )
{
	return true;
}

TouchEnableLayer::TouchEnableLayer()
{

}

TouchEnableLayer::~TouchEnableLayer()
{

}
