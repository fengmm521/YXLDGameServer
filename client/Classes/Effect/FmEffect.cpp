#include "TinyXml/tinyxml.h"
#include "FmTypeConvert.h"
#include "FmXmlWrapper.h"
#include "FmPropertyDesc.h"
#include "FmEffect.h"
#include "UI/LayerExten.h"
#include "Scene/FmMainScene.h"

USING_NS_CC;
USING_NS_CC_EXT;
using namespace gui;

NS_FM_BEGIN

EffectData::EffectData()
{
	m_Zorder = 20;
	m_Loop = -1;
	m_Life = -1;
	m_FrameDt = 100;
	m_Type = 0;
	m_X = 0;
	m_Y = 0;
	m_Dir = 0;
	m_Scale = 1.0f;
	m_isAnim = false;
}

EffectData::~EffectData()
{

}


EffectNode::EffectNode()
{
	m_ConfigData = NULL;
}

EffectNode* EffectNode::create( EffectData* data )
{
	EffectNode *pRet = new EffectNode(data);
	if (pRet)
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

EffectNode* EffectNode::create()
{
	EffectNode *pRet = new EffectNode();
	if (pRet)
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

EffectNode::EffectNode( EffectData* data )
{
	m_ConfigData = data;
	m_Life = m_ConfigData->m_Life;
	m_isFlipX = false;
	m_isFlipY = false;
}

EffectNode::~EffectNode()
{

}
int EffectNode::GetZOrder()
{
	if(m_ConfigData == NULL)
		return -1;
	return m_ConfigData->m_Zorder;
}

string& EffectNode::GetName()
{
	return m_ConfigData->m_Name;
}

int EffectNode::GetId()
{
	return m_ConfigData->m_Id;
}

int EffectNode::GetDir()
{
	return m_ConfigData->m_Dir;
}

int EffectNode::GetYOffsetType()
{
	return m_ConfigData->m_YOffsetType;
}

bool EffectNode::init()
{
	if(m_ConfigData == NULL)
		return false;
	CCNode* alayer = NULL;
	string val = m_ConfigData->m_Path;
	if(std::string::npos != val.find(".ExportJson"))
	{
		m_ConfigData->m_isAnim = true;
		CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo(val.c_str());

		CCArmature *armature = CCArmature::create(m_ConfigData->m_Name.c_str());
		if(m_ConfigData->m_Loop == 1)
		{
			armature->getAnimation()->play("0", -1, -1, 0);
		}
		else 
		{
			armature->getAnimation()->play("0", -1, -1, 1);
		}
		alayer = armature;
	}
	else
	{	
		CCSpriteFrameCache* cache = CCSpriteFrameCache::sharedSpriteFrameCache();  
		char listname[64] = {0};
		char texname[64] = {0};
		sprintf(listname,"%s.plist",val.c_str());
		sprintf(texname,"%s.pvr.ccz",val.c_str());
		cache->addSpriteFramesWithFile(listname);
		CCArray* animFrames = CCArray::createWithCapacity(5);
		for(int k= 0 ; k < MAX_EFFECT_FRAMES; k++)
		{
			char name[64] = {0};
			sprintf(name,"%d.png",k);
			CCSpriteFrame* frame = cache->spriteFrameByName(name);
			if(frame != NULL)
			{
				animFrames->addObject(frame);
				if (k==0)
				{
					this->setContentSize(frame->getOriginalSize());
				}
				//frame->retain();
			}
		}
		CCAnimation* ani = CCAnimation::createWithSpriteFrames(animFrames,m_ConfigData->m_FrameDt/1000.0f);
		
		cache->removeSpriteFramesFromFile(listname);
		cache->removeUnusedSpriteFrames();
		alayer = CCSprite::create();
		CCActionInterval* act = NULL;
		if(m_ConfigData->m_Loop < 0)
		{
			act = CCRepeatForever::create(CCAnimate::create(ani));
			alayer->runAction(act);
		}
		else
		{
			act = CCRepeat::create(CCAnimate::create(ani),m_ConfigData->m_Loop);
			CCFiniteTimeAction*  sqaction = CCSequence::create(
				act,
				CCCallFunc::create(this, callfunc_selector(EffectNode::callback)), 
				NULL);
			alayer->runAction(sqaction);			
		}
	}

	if(m_ConfigData->m_Life > 0)
		schedule( schedule_selector(EffectNode::updateTime), m_Life/1000.0f);

	alayer->setScale(m_ConfigData->m_Scale);
	alayer->setPosition(m_ConfigData->m_X, m_ConfigData->m_Y);
	removeChildByTag(1);
	addChild(alayer,0,1);
	SetFlipX(m_isFlipX);
	SetFlipY(m_isFlipY);

	return true;
}

void EffectNode::callback()
{
	//CCLog("Action Call Back ...");
	if(m_Life < 0)
		Dead();
}

void EffectNode::updateTime( float dt )
{
	unschedule( schedule_selector(EffectNode::updateTime));
	Dead();
}

void EffectNode::Dead()
{
	this->stopAllActions();
	CCNode* parent = getParent();
	if(parent != NULL)
	{
		parent->removeChild(this,true);
	}
}

void EffectNode::SetFlipX(bool b)
{
	//if(m_isFlipX != b)
	{
		m_isFlipX = b;

		CCNode* pSubNode = getChildByTag(1);
		if(pSubNode != NULL)
		{
			if (m_ConfigData->m_isAnim)
			{
				CCArmature* pSprite = (CCArmature*)pSubNode;
				pSprite->setScaleX(-getScaleX());
			}
			else
			{
				CCSprite* pSprite = (CCSprite*)pSubNode;
				pSprite->setFlipX(b);
			}	

			CCPoint pos;
			pos.x = m_isFlipX ? -m_ConfigData->m_X : m_ConfigData->m_X;
			pos.y = m_isFlipY ? -m_ConfigData->m_Y : m_ConfigData->m_Y;
			pSubNode->setPosition(pos);		
		}
	}
}


void EffectNode::SetFlipY(bool b)
{
	//if(m_isFlipY != b)
	{
		m_isFlipY = b;

		CCNode* pSubNode = getChildByTag(1);
		if(pSubNode != NULL)
		{
			if (m_ConfigData->m_isAnim)
			{
				CCArmature* pSprite = (CCArmature*)pSubNode;
				pSprite->setScaleY(-getScaleY());
			}
			else
			{
				CCSprite* pSprite = (CCSprite*)pSubNode;
				pSprite->setFlipY(b);
			}	

			CCPoint pos;
			pos.x = m_isFlipX ? -m_ConfigData->m_X : m_ConfigData->m_X;
			pos.y = m_isFlipY ? -m_ConfigData->m_Y : m_ConfigData->m_Y;
			pSubNode->setPosition(pos);	
		}
	}
}

void EffectNode::onExit()
{
	CCNode::onExit();
}

void EffectNode::onEnter()
{
	CCNode::onEnter();
}

void EffectNode::SetLife( float lf )
{
	if (lf > 0)
	{
		schedule( schedule_selector(EffectNode::updateTime), lf);
	}	
}


bool EffectMgr::Init()
{
	bool hasError = false;
	bool result = false;
	XmlWrapper xmlWrapper;
	FmXmlDocument& doc = xmlWrapper.GetDoc();
	do 
	{
		if( !xmlWrapper.LoadFile( "Data/Effect/Effects.xml" ) )
		{
			Assert( false );
			break;
		}
		FmXmlElement* rootNode = doc.FirstChildElement( "Effects" );
		if( rootNode == NULL )
		{
			Assert( false );
			break;
		}
		for ( FmXmlElement* node = rootNode->FirstChildElement( "Effect" ); node; node=node->NextSiblingElement("Effect") )
		{
			const char* tmpStr = 0;
			int tmpInt = 0;
			EffectData* effectData = new EffectData();
			effectData->m_Name = node->Attribute( "Name");
			effectData->m_Path = node->Attribute( "File");
			effectData->m_Id = node->IntAttribute( "Id" );
			effectData->m_Loop = node->IntAttribute( "Loop" );
			effectData->m_X = node->IntAttribute( "X" );
			effectData->m_Y = node->IntAttribute( "Y" );
			effectData->m_YOffsetType = node->IntAttribute( "YOffsetType" );
			effectData->m_Zorder = node->IntAttribute( "Z" );
			effectData->m_Life = node->IntAttribute( "Life" );
			effectData->m_FrameDt = node->IntAttribute( "Dt" );
			effectData->m_Dir = node->IntAttribute( "Dir" );
			effectData->m_Scale = node->FloatAttribute( "Scale" );
			m_EffectData[effectData->m_Name] = effectData;
		}
		result = true;
	} while (0);
	return result && (!hasError);
}

EffectData* EffectMgr::GetEffectDataById(const int id) const
{
	for (EffectDataMap::const_iterator it=m_EffectData.begin(); it!=m_EffectData.end(); it++)
	{
		EffectData* data = it->second;
		if (data && data->m_Id == id)
		{
			return it->second; 
		}
	}

	return NULL;
}


const EffectData* EffectMgr::GetEffectDataByName(const char* name) const
{
	EffectDataMap::const_iterator ed = m_EffectData.find(name);
	if( m_EffectData.end() != ed )
	{
		return ed->second; 
	}
	return NULL;
}

EffectMgr::EffectMgr()
{

}

EffectMgr::~EffectMgr()
{
	Shutdown();
}

EffectNode* EffectMgr::GetEffectNode( const char* name )
{
	EffectData* ed = (EffectData*)GetEffectDataByName(name);
	EffectNode* re = NULL;
	if(ed != NULL)
	{
		re = EffectNode::create(ed);
		re->init();
		return re;
	}
	else
		return NULL;
}

EffectNode* EffectMgr::GetEffectNodeById( int effectId )
{
	EffectData* ed = (EffectData*)GetEffectDataById(effectId);
	EffectNode* re = NULL;
	if(ed != NULL)
	{
		re = EffectNode::create(ed);
		re->init();
		return re;
	}
	else
		return NULL;
}

void EffectMgr::AddEffect( CCNode* panl,EffectNode* enode,int tag )
{
	if(panl == NULL || enode == NULL)
		return;
	panl->addChild(enode,enode->GetZOrder(),tag);
}

void EffectMgr::AddEffect( CCNode* panl,const char* name,int tag )
{
	EffectNode* enode = GetEffectNode(name);
	if(enode == NULL)
		return;
	AddEffect(panl, enode , tag);
}

void EffectMgr::StopEffect( CCNode* panl,EffectNode* enode )
{
	if(panl == NULL || enode == NULL )
		return;
	panl->removeChild( enode,true );
}

void EffectMgr::Shutdown()
{
	for ( EffectDataMap::iterator it=m_EffectData.begin(); it!=m_EffectData.end(); ++it )
	{
		delete it->second;
	}
	m_EffectData.clear();
}

void EffectMgr::AddButtonPromotToNode( Widget* node, int zOrder,int tag ,bool isCircle,int time)
{
	//Widget* parentnode = dynamic_cast<Widget*>(node->getParent());
	if(node == NULL)
		return;

	CCPoint point = node->getAnchorPoint();
	//bool flag = node->isIgnoreAnchorPointForPosition();

	EffectNode* eft = NULL;
	if ( isCircle )
	{
		eft = EffectMgr::GetInstance().GetEffectNode("bt_lights");
	}
	else
	{
		eft = EffectMgr::GetInstance().GetEffectNode("bt_light");
	}
	if(eft == NULL)
		return;
	//CCRect R = node->boundingBox();
	CCSize size = node->getContentSize();
	float scale = MainScene::GetInstance().getScale();
	eft->setScaleX(size.width/115*scale);
	eft->setScaleY(size.height/115*scale);
	
	eft->setPosition(ccp(size.width*(0.5 - point.x), size.height*(0.5 - point.y)));

	node->removeNodeByTag(tag);
	if ( time > 0 )
	{
		LifeNode* lifeNode = LifeNode::create(time);
		if ( lifeNode )
		{
			lifeNode->addChild(eft,zOrder,tag);
			node->addNode(lifeNode,zOrder,tag);
		}
	}
	else
	{
		node->addNode(eft,zOrder,tag);
	}
}


CloudEffect* CloudEffect::create(const char* path, const CCPoint& speed,const CCSize& size )
{
	CloudEffect *pRet = new CloudEffect();
	if (pRet)
	{
		pRet->initWithFile(path);
		pRet->SetProp(speed,size);
		pRet->autorelease();
		return pRet;
	}
	else
	{
		CC_SAFE_DELETE(pRet);
		return NULL;
	}
}

void CloudEffect::UpdateTime( float dt )
{
	setPosition(ccpAdd(getPosition(),ccpMult(m_Speed,dt)));

	if(getPositionX() < -m_Size.width && m_Speed.x < 0)
	{
		setPositionX(m_Size.width + getContentSize().width);
	}
	if(getPositionX() > getContentSize().width + m_Size.width && m_Speed.x > 0)
	{
		setPositionX(-getContentSize().width);
	}
}

void CloudEffect::SetProp( const CCPoint& speed,const CCSize& size )
{
	m_Speed = speed;
	m_Size = size;
	schedule( schedule_selector(CloudEffect::UpdateTime), 0.1f);

}




CCParticleBtnTip * CCParticleBtnTip::create(int num)
{
	CCParticleBtnTip *pRet = new CCParticleBtnTip();
	if (pRet && pRet->initWithTotalParticles(num))
	{
		pRet->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(pRet);
	}
	return pRet;
}

CCParticleBtnTip* CCParticleBtnTip::createWithTotalParticles(unsigned int numberOfParticles)
{
	CCParticleBtnTip* pRet = new CCParticleBtnTip();
	if (pRet && pRet->initWithTotalParticles(numberOfParticles))
	{
		pRet->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(pRet);
	}
	return pRet;
}
static const char s_fire[]                = "ImgUi/fire.png";
bool CCParticleBtnTip::initWithTotalParticles(unsigned int numberOfParticles)
{
	if( CCParticleSystemQuad::initWithTotalParticles(numberOfParticles) )
	{
		// additive
		this->setBlendAdditive(true);

		// duration
		m_fDuration = kCCParticleStartSizeEqualToEndSize;

		// Gravity Mode
		setEmitterMode(kCCParticleModeGravity);

		// Gravity Mode: gravity
		setGravity(ccp(0,0));

		// Gravity mode: radial acceleration
		setRadialAccel(0);
		setRadialAccelVar(0);

		// Gravity mode: speed of particles
		setSpeed(0);
		setSpeedVar(0);


		// angle
		m_fAngle = 180;
		m_fAngleVar =360;

		// emitter position
		CCSize winSize = CCDirector::sharedDirector()->getWinSize();
		this->setPosition(ccp(winSize.width/2, winSize.height/2));
		setPosVar(CCPointZero);

		// life of particles
		m_fLife = 2;
		m_fLifeVar = 0.0f;

		// size, in pixels
		m_fStartSize = 12.0f;
		m_fStartSizeVar = 2.0f;
		m_fEndSize = kCCParticleStartSizeEqualToEndSize;

		// emits per seconds
		m_fEmissionRate = m_uTotalParticles/m_fLife;

		// color of particles
		m_tStartColor.r = 0.2f;
		m_tStartColor.g = 0.2f;
		m_tStartColor.b = 0.6f;
		m_tStartColor.a = 1.0f;
		m_tStartColorVar.r = 0.0f;
		m_tStartColorVar.g = 0.0f;
		m_tStartColorVar.b = 0.0f;
		m_tStartColorVar.a = 0.0f;
		m_tEndColor.r = 0.02f;
		m_tEndColor.g = 0.01f;
		m_tEndColor.b = 0.99f;
		m_tEndColor.a = 1.0f;
		m_tEndColorVar.r = 0.0f;
		m_tEndColorVar.g = 0.0f;
		m_tEndColorVar.b = 0.0f;
		m_tEndColorVar.a = 0.0f;

		CCTexture2D* pTexture =  CCTextureCache::sharedTextureCache()->addImage(s_fire);
		if (pTexture != NULL)
		{
			setTexture(pTexture);
		}

		return true;
	}
	return false;
}


void CCParticleBtnTip::setRight()
{
	m_fAngle = 180;
}

void CCParticleBtnTip::setUp()
{
	m_fAngle = 270;
}

void CCParticleBtnTip::setLeft()
{
	m_fAngle = 0;
}

void CCParticleBtnTip::setDown()
{
	m_fAngle = 90;
}

void CCParticleBtnTip::setProperty( float life,float speed )
{
	m_fLife = life;
	m_fEmissionRate = m_uTotalParticles/m_fLife;
}


void CCParticleBtnTip::RunOnNode( CCNode* node,int zOrder,int tag, bool isRect)
{
	if(node == NULL)
			return;

	CCParticleSystemQuad* pt = CreateParticle( node->getPosition(),node->boundingBox(),isRect);
	
	node->removeChildByTag(tag,true);
	node->addChild(pt,zOrder,tag);
}

CCParticleSystemQuad* CCParticleBtnTip::CreateParticle( CCPoint pos,CCRect boundingBox, bool isRect/*=true*/ )
{
	//CCParticleSystemQuad* pt = CCParticleSystemQuad::create("Particle/sast.plist");
	
	CCParticleBtnTip * pt = CCParticleBtnTip::create(300);

	if(isRect)
	{
		float x0 = boundingBox.getMinX();
		float y0 = boundingBox.getMinY();
		CCPoint offP = ccp(boundingBox.size.width/2,boundingBox.size.height/2);
		CCPoint p0 = ccpSub(ccp(x0,y0),pos);
		p0 = ccpAdd(p0,offP);
		float x1 = boundingBox.getMaxX();
		float y1 = boundingBox.getMinY();
		CCPoint p1 = ccpSub(ccp(x1,y1),pos);
		p1 = ccpAdd(p1,offP);
		float x2 = boundingBox.getMaxX();
		float y2 = boundingBox.getMaxY();
		CCPoint p2 = ccpSub(ccp(x2,y2),pos);
		p2 = ccpAdd(p2,offP);
		float x3 = boundingBox.getMinX();
		float y3 = boundingBox.getMaxY();
		CCPoint p3 = ccpSub(ccp(x3,y3),pos);
		p3 = ccpAdd(p3,offP);
		float s = boundingBox.size.width + boundingBox.size.height;
		float v = 220;
		float t1 = boundingBox.size.width/v;
		float t2 = boundingBox.size.height/v;

		pt->setProperty(t1+t2,0);

		pt->setPosition(p0);
		CCCallFunc* d1 = CCCallFunc::create(pt, callfunc_selector(CCParticleBtnTip::setRight));
		CCCallFunc* d2 = CCCallFunc::create(pt, callfunc_selector(CCParticleBtnTip::setUp));
		CCCallFunc* d3 = CCCallFunc::create(pt, callfunc_selector(CCParticleBtnTip::setLeft));
		CCCallFunc* d4 = CCCallFunc::create(pt, callfunc_selector(CCParticleBtnTip::setDown));
		CCMoveTo* m1 = CCMoveTo::create(t1,p1);
		CCMoveTo* m2 = CCMoveTo::create(t2,p2);
		CCMoveTo* m3 = CCMoveTo::create(t1,p3);
		CCMoveTo* m4 = CCMoveTo::create(t2,p0);
		CCActionInterval*  seq = CCSequence::create(d1,m1,d2, m2,d3,m3,d4,m4, NULL);
		CCAction*  rep = CCRepeatForever::create(seq);
		pt->runAction(rep);
				
	}
	else
	{
		float time = 1.4f;
		CCPoint pos = ccp(boundingBox.size.width/2, boundingBox.size.height/2);
		CCRoundBy* round = CCRoundBy::create(1.4f, false, pos, boundingBox.size.width/2.0f, 360.0f);
		CCAction*  rep = CCRepeatForever::create(round);	
		pt->runAction(rep);
	}
	return pt;
}





NS_FM_END