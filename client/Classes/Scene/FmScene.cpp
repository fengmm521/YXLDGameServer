#include "FmScene.h"
#include "FmEntityMgr.h"

#include "Enum/FmComponentEnum.h"
#include "GameEntity/FmEntityRender.h"
#include "Scene/SceneMgr.h"
#include "FmMainScene.h"
#include "Script/FmScriptSys.h"
#include "GameData/DataManager.h"
#include "Numeric/SceneData.h"
#include "Enum/FmPropertyIndex.h"
#include "FmFightMgr.h"


NS_FM_BEGIN
USING_NS_CC;


Scene::Scene()
{
	m_mapLayer = NULL;
	m_objLayer = NULL;
	m_SceneId = 0;
	m_pause = false;

	m_heroPosList.push_back(ccp(340,670-400));//0
	m_heroPosList.push_back(ccp(300,670-480));
	m_heroPosList.push_back(ccp(300,670-560));
	m_heroPosList.push_back(ccp(230,670-400));
	m_heroPosList.push_back(ccp(190,670-480));
	m_heroPosList.push_back(ccp(190,670-560));
	m_heroPosList.push_back(ccp(120,670-400));
	m_heroPosList.push_back(ccp(80,670-480));
	m_heroPosList.push_back(ccp(80,670-560));
	m_heroPosList.push_back(ccp(640,670-400));//9
	m_heroPosList.push_back(ccp(600,670-480));
	m_heroPosList.push_back(ccp(600,670-560));
	m_heroPosList.push_back(ccp(750,670-400));
	m_heroPosList.push_back(ccp(710,670-480));
	m_heroPosList.push_back(ccp(710,670-560));
	m_heroPosList.push_back(ccp(860,670-400));
	m_heroPosList.push_back(ccp(820,670-480));
	m_heroPosList.push_back(ccp(820,670-560));
	m_heroPosList.push_back(ccp(150,650-220));//18
	m_heroPosList.push_back(ccp(810,650-220));
}

Scene::~Scene()
{
	MainScene::GetInstance().removeChildByTag(EMSTag_Map);
	MainScene::GetInstance().removeChildByTag(EMSTag_Obj);
	MainScene::GetInstance().removeChildByTag(EMStag_Effect);
}

void Scene::init(uint sceneId)
{
	m_SceneId = sceneId;
}

set<uint>& Scene::GetEntityIds()
{
	return m_SceneEntitys.GetSet();
}

void Scene::GetObjectList( vector<uint>& objectIds )
{
	set<uint>& objects = GetEntityIds();
	for ( set<uint>::iterator it=objects.begin(); it!=objects.end(); ++it )
	{
		objectIds.push_back( *it );
	}
}

void Scene::AddEntity( Hero* sceneEntity )
{
	if( sceneEntity == NULL )
	{
		Assert( false );
		return;
	}
	if( m_SceneEntitys.GetSet().find( sceneEntity->GetEntityId() ) != m_SceneEntitys.GetSet().end() )
		return;
	m_SceneEntitys.Add( sceneEntity->GetEntityId() );
	
	sceneEntity->SetPosition(m_heroPosList[sceneEntity->GetUint(EHero_FormationPos)]);
	
	EntityRender* render = new EntityRender(sceneEntity);
	sceneEntity->AddRender( render );
	AddEntityRender(render);
	sceneEntity->OnEntryScene( this );
	/*if (sceneEntity->GetExtraBool("IsSky"))
	{
		render->SetPosition(ccp(sceneEntity->GetPosition().x, 1000));
		render->runAction(CCMoveTo::create(0.5f, sceneEntity->GetPosition()));
		ScriptSys::GetInstance().Execute_1("Fight_PlayReplaceMemAnim", &sceneEntity->GetPosition(), "CCPoint");
	}*/
}

void Scene::AddEntityRender( EntityRender* entityRender)
{
	if( !entityRender )
		return;
	CCLayer* layerModel  = entityRender->GetLayer(ERenderLayerType_Model);
	CCLayer* layerEffect = entityRender->GetLayer(ERenderLayerType_Effect);
	CCLayer* layerUI     = entityRender->GetLayer(ERenderLayerType_UI);
	if(layerModel != NULL && layerEffect!= NULL && layerUI!=NULL)
	{
		if(m_objLayer != NULL)
		{
			m_objLayer->removeChild(entityRender);
			m_objLayer->addChild(entityRender, entityRender->GetZorder(),entityRender->GetTagId());

			entityRender->InitAnim();
			entityRender->SetAnimation(EAnimsIdle);
		}
	}
}

void Scene::RemoveEntity( uint entityId )
{
	m_SceneEntitys.Remove( entityId );
	Hero* entity = EntityMgr::GetInstance().GetEntity<Hero>( entityId );
	if( entity )
	{	
		entity->OnLeaveScene( this );
		EntityMgr::GetInstance().RemoveEntity(entityId);
	}
}

void Scene::Update( uint delta )
{
	m_SceneEntitys.BeforeTraverse();
	for ( set<uint>::iterator it=m_SceneEntitys.GetSet().begin(); it!=m_SceneEntitys.GetSet().end(); ++it )
	{
		Entity* entity = EntityMgr::GetInstance().GetEntityById( *it );
		if( entity )
		{
			entity->Update( delta );
		}
	}
}

void Scene::ReleaseAllEntity()
{
	set<uint> entitys = m_SceneEntitys.GetSet();
	for ( set<uint>::iterator it=entitys.begin(); it!=entitys.end(); ++it )
	{
		uint id = *it;

		RemoveEntity(id);
		EntityMgr::GetInstance().RemoveEntity(id);
	}
}



Scene* Scene::Create( uint sceneId )
{
	Scene* scene = new Scene;
	scene->init(sceneId);

	return scene;
}

void Scene::Pause()
{
	if (m_pause)
	{
		return;
	}
	m_pause = true;
	set<uint> entitys = m_SceneEntitys.GetSet();
	for ( set<uint>::iterator it=entitys.begin(); it!=entitys.end(); ++it )
	{
		uint id = *it;

		Hero* hero = EntityMgr::GetInstance().GetEntity<Hero>(id);
		if (hero)
		{
			EntityRender* render = hero->GetRender();
			if (render)
			{
				render->Pause();
			}
		}
	}
	//m_effectLayer->pauseSchedulerAndActions();

	CCArray* children = m_effectLayer->getChildren();
	CCObject* child = NULL;
	CCARRAY_FOREACH(children, child)
	{
		CCArmature* armature = dynamic_cast<CCArmature*>(child);
		if (armature)
		{
			armature->getAnimation()->pause();
		}
	}
}

void Scene::ChangeMap(int index)
{
	stSceneData* data = CDataManager::GetInstance().GetGameData<stSceneData>(DataFileScene, m_SceneId);

	if (data)
	{
		vector<string> paths = StringUtil::Split(data->m_path, "#");
		if (paths.size() > index)
		{
			m_mapLayer->changeNextMap(paths[index].c_str());
		}
	}
	
}

void Scene::LoadTileMap()
{
	string path = "TileMap/tm01.jpg";
	stSceneData* data = CDataManager::GetInstance().GetGameData<stSceneData>(DataFileScene, m_SceneId);

	if (data)
	{
		vector<string> paths = StringUtil::Split(data->m_path, "#");
		path = paths[0];
	}
	m_mapLayer = MapLayer::create(path.c_str());
	if (m_mapLayer)
	{
		MainScene::GetInstance().addChild(m_mapLayer, EMSTag_Map, EMSTag_Map);
	}

	m_objLayer = CCLayer::create();

	MainScene::GetInstance().addChild(m_objLayer, EMSTag_Obj, EMSTag_Obj);

	CCSize designSize = MainScene::GetInstance().GetDesignSize();
	CCSize frameSize = CCEGLView::sharedOpenGLView()->getFrameSize();
	float scaleFactMin = MainScene::GetInstance().m_minScale;
	float scaleFactMax = MainScene::GetInstance().m_maxScale;

	m_objLayer->setScale(scaleFactMin);
	CCSize objSize = CCSizeMake(designSize.width*scaleFactMin,designSize.height*scaleFactMin);
	m_objLayer->setContentSize(objSize);
	m_objLayer->setAnchorPoint(ccp(0,0));
	//左下角坐标
	m_objLayer->setPosition(ccp((frameSize.width - objSize.width)/2,(frameSize.height - objSize.height)/2));



	m_effectLayer = CCLayer::create();
	MainScene::GetInstance().addChild(m_effectLayer, EMStag_Effect, EMStag_Effect);

	m_effectLayer->setScale(scaleFactMin);
	m_effectLayer->setContentSize(objSize);
	m_effectLayer->setAnchorPoint(ccp(0,0));
	//左下角坐标
	m_effectLayer->setPosition(ccp((frameSize.width - objSize.width)/2,(frameSize.height - objSize.height)/2));
}

void Scene::Resume(int iSkillID)
{
	if (m_pause == false)
	{
		return;
	}

	m_pause = false;
	set<uint> entitys = m_SceneEntitys.GetSet();
	for ( set<uint>::iterator it=entitys.begin(); it!=entitys.end(); ++it )
	{
		uint id = *it;

		Hero* hero = EntityMgr::GetInstance().GetEntity<Hero>(id);
		if (hero)
		{
			EntityRender* render = hero->GetRender();
			if (render)
			{
				render->Resume(iSkillID);
			}
		}
	}
	m_effectLayer->resumeSchedulerAndActions();

	CCArray* children = m_effectLayer->getChildren();
	CCObject* child = NULL;
	CCARRAY_FOREACH(children, child)
	{
		CCArmature* armature = dynamic_cast<CCArmature*>(child);
		if (armature)
		{
			armature->getAnimation()->resume();
		}
	}
}

void Scene::GetLeftHeroList(vector<Hero*>& heroList)
{
	set<uint> entitys = m_SceneEntitys.GetSet();
	for ( set<uint>::iterator it=entitys.begin(); it!=entitys.end(); ++it )
	{
		uint id = *it;

		Hero* hero = EntityMgr::GetInstance().GetEntity<Hero>(id);
		if (hero && hero->GetInt(EHero_FormationPos) < 9)
		{
			heroList.push_back(hero);
		}
	}
}

Hero* Scene::GetLeftGodAnimal()
{
	set<uint> entitys = m_SceneEntitys.GetSet();
	for ( set<uint>::iterator it=entitys.begin(); it!=entitys.end(); ++it )
	{
		uint id = *it;

		Hero* hero = EntityMgr::GetInstance().GetEntity<Hero>(id);
		if (hero && hero->GetInt(EHero_FormationPos) == 18)
		{
			return hero;
		}
	}

	return NULL;
}

NS_FM_END