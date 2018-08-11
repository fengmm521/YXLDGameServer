#include "FaceMgr.h"

USING_NS_CC;
USING_NS_CC_EXT;

FaceMgr::FaceMgr()
{
	for(int i = 0 ; i < FACES_MAX; i++)
	{
		m_Anims[i] = NULL;
	}
}

FaceMgr::~FaceMgr()
{
}

Widget* FaceMgr::GetById( int id )
{
	if(id<0 || id >= FACES_MAX )
		return NULL;
	if(m_Anims[id] == NULL)
		return NULL;
	CCSprite* spr = CCSprite::create();
	spr->setContentSize(m_CtSize);
	spr->runAction( CCRepeatForever::create( CCAnimate::create(m_Anims[id]) ) );
	Widget* widget = Widget::create();
	widget->setSize(spr->getContentSize());
	widget->addNode(spr);
	return widget;
}

bool FaceMgr::LoadFile( const char* name )
{
	//ÐòÁÐÖ¡¶¯»­
	CCSpriteFrameCache* cache = CCSpriteFrameCache::sharedSpriteFrameCache();  
	char listname[64] = {0};
	char texname[64] = {0};
	sprintf(listname,"%s.plist",name);
	//sprintf(texname,"%s.pvr.ccz",name);
	cache->addSpriteFramesWithFile(listname);
	bool hasSetSize = false;
	for(size_t i = 0; i < FACES_MAX; i++)
	{
		CCArray* animFrames = CCArray::createWithCapacity(5);
		for(int k= 0 ; k < 10; k++)
		{
			char name[64] = {0};
			sprintf(name,"%d.png",i*100 + k);
			CCSpriteFrame* frame = cache->spriteFrameByName(name);
			if(frame != NULL)
			{
				if(!hasSetSize)
				{
					m_CtSize = frame->getRect().size;
					hasSetSize = true;
				}
				animFrames->addObject(frame);
				//frame->retain();
			}
		}
		if(animFrames->count()>0)
		{
			m_Anims[i] = CCAnimation::createWithSpriteFrames(animFrames, 0.15f);
			m_Anims[i]->retain();
		}
	}
	
	cache->removeSpriteFramesFromFile(listname);
	cache->removeUnusedSpriteFrames();
	return true;

}

void FaceMgr::Shutdown()
{
	for(int i = 0 ; i < FACES_MAX; i++)
	{
		if(m_Anims[i] != NULL)
			m_Anims[i]->release();
	}
}

