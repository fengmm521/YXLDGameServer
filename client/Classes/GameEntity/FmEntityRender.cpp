#include "FmEntityRender.h"
#include "Scene/FmMainScene.h"
#include "FmMath.h"
#include "Script/FmScriptSys.h"
#include "actions/CCActionInstant.h"
#include "actions/CCAction.h"
#include "actions/CCActionInterval.h"
#include "Enum/FmComponentEnum.h"
#include "Util/FmStringMgr.h"
#include "Sound/AudioEngine.h"
#include "Scene/SceneMgr.h"
#include "FmHero.h"
#include "Enum/FmPropertyIndex.h"
#include "Scene/FmFightMgr.h"
#include "FmEntityMgr.h"
#include "Numeric/BuffData.h"


NS_FM_BEGIN

//TMX 显示层最多数量为 10
static int s_gUnTagId = 1;
int EntityRender::GetAutoTagId()
{
	return s_gUnTagId++;
}

EntityRender::EntityRender(Hero* hero)
{
	m_currentAnimation = m_nextAnimation = EAnimsIdle;
	m_tagId = GetAutoTagId();
	//m_IsCCbAnim = false;

	m_contentSize = CCSizeMake(120,144);
	for (uint i = 0; i < ERenderLayerType_Max; ++i)
	{
		m_Layer[i] = RenderModeLayer::create(this);
		this->addChild(m_Layer[i], i, i);
	}
	m_hero = hero;
	m_skill = NULL;
	m_data = NULL;
	m_skillEffect = NULL;
	m_pAttackerHero = NULL;
	m_bFadeouted = false;
	m_bMarkActorSkillKilled = false;
	m_bUILayerInited = false;

	m_MainState.ResetState(EAttackRenderState_None, 0);
	this->retain();
}

EntityRender::~EntityRender()
{
	/*for (uint i = 0; i < ERenderLayerType_Max; ++i)
	{
		if( m_Layer[i] )
			m_Layer[i]->release();
	}*/

	//CCArmatureDataManager::sharedArmatureDataManager()->removeArmatureFileInfo(GetAnimPath().c_str());
	
	if(m_armature)
	{
		// 清理下
		m_armature->getAnimation()->setMovementEventCallFunc(NULL, NULL);
		m_armature->getAnimation()->setFrameEventCallFunc(NULL, NULL);
	}

	this->removeFromParent();
	this->release();
}

void EntityRender::SetVisible(bool visible)
{
	for (uint i = 0; i < ERenderLayerType_Max; ++i)
	{
		bool curVisible = m_Layer[i]->isVisible();

		if( m_Layer[i] 
		&& visible != curVisible)
		{			
			m_Layer[i]->setVisible(visible);
		}
	}
}

void EntityRender::InitAnim()
{
	if(m_Layer[ERenderLayerType_Model] == NULL)
		return;
	if(m_hero == NULL)
		return;

	string path = GetAnimPath();

	PerformenceCounter EntityRenderCounter(path.c_str() );
	
	if (path == "")
	{
		return;
	}

	int iHeroID = m_hero->GetInt(EHero_HeroId);

	m_armature = SceneMgr::GetInstance().queryCacheArmature(iHeroID);
	if(m_armature && m_armature->retainCount() > 1)
	{
		m_armature = NULL;
	}
	//SceneMgr::GetInstance().queryCacheArmature();
	//queryCacheArmature

	//CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfoAsync(path.c_str(), this, (SEL_SCHEDULE)&EntityRender::onAnimationLoadCallback);

	// load resource directly
	if(m_armature == NULL)
	{
		// 对于不存在Cache数据的,直接Add
		if(!CCArmatureDataManager::sharedArmatureDataManager()->getAnimationData(GetAnimName().c_str() ) )
		{
			CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo(path.c_str());
		}
		else
		{
			// 存在Cache的有可能SpriteFrameCache不存在，需要再次添加下
			CCArmatureDataManager::sharedArmatureDataManager()->rebuildArmatureSpriteFrame(path.c_str());
		}
			
		m_armature = CCArmature::create(GetAnimName().c_str() );
	}

	string strPerfTest = "Post|"+path;
	PerformenceCounter initRenderPost(strPerfTest.c_str() );
	
	m_armature->setOpacity(255);
	m_armature->getAnimation()->setMovementEventCallFunc(m_Layer[ERenderLayerType_Model], movementEvent_selector(RenderModeLayer::animationEvent));
	m_armature->getAnimation()->setFrameEventCallFunc(m_Layer[ERenderLayerType_Model], frameEvent_selector(RenderModeLayer::onFrameEvent));
	
	//设置缩放和颜色
	float scale = GetAnimScale();
	//if(scale != m_armature->getScale())
	//{
	m_armature->setScale(scale);
	//}
	m_armature->setPosition(0, 0);

	m_Layer[ERenderLayerType_Model]->removeAllChildren();
	m_Layer[ERenderLayerType_Model]->addChild(m_armature, 1, 1);
	
	SetAnimation(EAnimsIdle, -1);
	SetFlipX(GetFlipByFormationPos());
	//更新模型坐标
	if (FightMgr::GetInstance().m_isCanItem && (m_hero->GetInt(EHero_FormationPos) < 9 || m_hero->GetInt(EHero_FormationPos) == 18))
	{
		SetPosition(ccp(-200, 0));
	}
	else
	{
		SetPosition(m_hero->GetPosition());
	}
}

void EntityRender::initUILayer()
{
	if(m_bUILayerInited)
	{
		return;
	}

	m_bUILayerInited = true;
	ScriptSys::GetInstance().Execute_1("EntityRender_InitUiLayer", m_hero, "Hero");
}

void EntityRender::SetAnimation( EAnims ani )
{
	SetAnimation( ani,-1);
}

void EntityRender::SetAnimation( EAnims ani,int loop )
{
	CCArmature* armature = GetArmature();
	if (armature)
	{
		m_lastAnimation = m_currentAnimation == EAnimsHurt ? EAnimsIdle : m_currentAnimation;
		m_lastFrame = EAnimsHurt ? -1 : armature->getAnimation()->getCurrentFrameIndex();
		m_currentAnimation = ani;

		char szAniName[128] = {0};
		snprintf(szAniName, sizeof(szAniName), "%d", ani);
		armature->getAnimation()->play(szAniName, -1, -1, loop);
	}
}

void RenderModeLayer::onFrameEvent(cocos2d::extension::CCBone *bone, const char *evt, int originFrameIndex, int currentFrameIndex)
{
	if (strcmp(evt, "shoot") == 0 || strcmp(evt, "hurt") == 0)
	{	
		
		//else
		{
			// 自爆类似的技能，不能重复播放(很多英雄同时有Shoot和Hurt)
			if (m_Host->m_skillEffect&& m_Host->m_skillEffect->m_bulletType != 0)
			{
				if( (m_Host->m_skillEffect->m_bulletType!=1) || (m_Host->m_data->buttletIndex == 0) )
				{
					m_Host->PlayButtletEffect();
					m_Host->m_MainState.ResetState(EAttackRenderState_BulletEffect, 5000);
				}
			}
			else
			{
				m_Host->m_MainState.ResetState(EAttackRenderState_HurtEffect, 1);
			}

			if (m_Host->m_skillEffect && (m_Host->m_skillEffect->m_attSound != "") )
			{
				string soundPath = "Data/Sound/" + m_Host->m_skillEffect->m_attSound;
				SimpleAudioEngine::sharedEngine()->playEffect(soundPath.c_str());
			}
		}
	}
}

void RenderModeLayer::animationEvent(cocos2d::extension::CCArmature *armature, MovementEventType movementType, const char *movementID)
{
	std::string id = movementID;

	if (movementType == LOOP_COMPLETE)
	{
		if (id.compare("1") == 0 || id.compare("2") == 0 || id.compare("7") == 0 || id.compare("10") == 0)
		{
			//CCActionInterval *actionMove = CCMoveTo::create(1, ccp(VisibleRect::right().x - 50, VisibleRect::right().y));
			//armature->stopAllActions();
			//armature->runAction(CCSequence::create(actionMove,  CCCallFunc::create(this, callfunc_selector(EntityRender::callback1)), NULL));
		
			// 自爆特殊处理，直接切换到剩余伤害处理
			m_Host->SetAnimation(EAnimsIdle);

			if(id.compare("10") == 0)
			{
				m_Host->m_pAttackerHero->GetRender()->m_MainState.ResetState(EAttackRenderState_ZiBaoHurtEffect, 1);
				//m_Host->m_pAttackerHero->GetRender()->FadeOut(0.001);
				m_Host->m_pAttackerHero = NULL;
			}

		}
		else if (id.compare("3") == 0 || id.compare("4") == 0)	//伤害或者闪避
		{
			m_Host->RevertAnim();

			if (m_Host->GetHero() == FightMgr::GetInstance().m_finalHero)
			{
				if (FightMgr::GetInstance().GetMainState().GetState() == EFightState_PlayOneResultSet)
				{
					FightMgr::GetInstance().GetMainState().ResetState(EFightState_ChgProp, 250);
				}
			}
			else
			{
				// 如果是多段攻击，可能存在这个情况,要处理
				if(m_Host->GetHero()->GetRender()->isMultiDamageSkill() )
				{
					FightMgr::GetInstance().GetMainState().ResetState(EFightState_ChgProp, 250);
				}
				else
				{
					CCLOG("FUCK--------------------------KaSi-----------------------");
				}
			}
		}
		else if(id.compare("5") == 0)
		{
			//m_Host->SetAnimation(EAnimsIdle);
		}
	}
}


bool EntityRender::isMultiDamageSkill()
{
	if(m_skill && m_skill->m_bMultiDamage)
	{
		return true;
	}

	return false;
}

void EntityRender::PlayZiBaoHurtEffect()
{
	PlayEffect("sunwuzibaoAim");
	CCLOG("---------------------*******PlayZiBao--------");
}

void EntityRender::PlayEffect(const string& strEffectName)
{
	string strTargetName = strEffectName;
	string path = GetEffectPathByName(strTargetName);//StringMgr::GetInstance().Format("Effect/%s/%s.ExportJson", skillEffect->m_targetEffectName.c_str(), skillEffect->m_targetEffectName.c_str());
	
	// load resource directly
	
	CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo(path.c_str());
	CCArmature *hurt = CCArmature::create(strTargetName.c_str() );
	CCArmatureDataManager::sharedArmatureDataManager()->removeArmatureFileInfo(path.c_str());

	if(hurt)
	{
		hurt->getAnimation()->setMovementEventCallFunc(this, movementEvent_selector(EntityRender::animationEvent));
		hurt->getAnimation()->play("0");
		hurt->setPosition(ccp(0, GetHeight() / 2));
		m_Layer[ERenderLayerType_Effect]->addChild(hurt, 1, GetEffectTag());
	}
}

void EntityRender::PlayUIEffect(const string& strEffectName)
{
	string strTargetName = strEffectName;
	string path = string("UIEffect/") + strEffectName + "/" + strEffectName+".ExportJson";//GetEffectPathByName(strTargetName);//StringMgr::GetInstance().Format("Effect/%s/%s.ExportJson", skillEffect->m_targetEffectName.c_str(), skillEffect->m_targetEffectName.c_str());
	
	// load resource directly
	
	CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo(path.c_str());
	CCArmature *hurt = CCArmature::create(strTargetName.c_str() );
	CCArmatureDataManager::sharedArmatureDataManager()->removeArmatureFileInfo(path.c_str());

	if(hurt)
	{
		hurt->getAnimation()->setMovementEventCallFunc(this, movementEvent_selector(EntityRender::animationEvent));
		hurt->getAnimation()->play("0");
		hurt->setPosition(ccp(0, GetHeight() / 2));
		m_Layer[ERenderLayerType_Effect]->addChild(hurt, 1, GetEffectTag());
	}
}

void EntityRender::PlayHurtEffect(stSkillEffectData* skillEffect)
{
	if (skillEffect && skillEffect->m_targetEffectName != "")
	{
		string path = GetEffectPathByName(skillEffect->m_targetEffectName).c_str();//StringMgr::GetInstance().Format("Effect/%s/%s.ExportJson", skillEffect->m_targetEffectName.c_str(), skillEffect->m_targetEffectName.c_str());
		// load resource directly
		CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo(path.c_str());
		CCArmature *hurt = CCArmature::create(skillEffect->m_targetEffectName.c_str());
		CCArmatureDataManager::sharedArmatureDataManager()->removeArmatureFileInfo(path.c_str());
		if(hurt)
		{
			hurt->getAnimation()->setMovementEventCallFunc(this, movementEvent_selector(EntityRender::animationEvent));
			hurt->getAnimation()->play("0");
			hurt->setPosition(ccp(0, GetHeight() / 2));
			m_Layer[ERenderLayerType_Effect]->addChild(hurt, 1, GetEffectTag());

			if(GetFlipByTargetFormationPos(m_hero) )
			{
				hurt->setScaleX(-hurt->getScaleX() );
			}
		}

		if (skillEffect->m_targetSound != "")
		{
			string soundPath = "Data/Sound/" + skillEffect->m_targetSound;
			SimpleAudioEngine::sharedEngine()->playEffect(soundPath.c_str());
		}
	}
}

void EntityRender::PlayAttAnim( SkillResultData* data )
{
	this->setZOrder(this->GetZorder() + 1);
	m_data = data;

	m_skill = GetSkillDataByID(data->skillId);
	if (m_skill == NULL)
	{
		assert(false);
	}
	m_skillEffect = GetSkillEffectDataBySkillID(data->skillId);
	if (m_data)
	{
		if (m_data->type == 1)
		{
			m_MainState.ResetState(EAttackRenderState_AttAnim, 200);
			ScriptSys::GetInstance().Execute_1("EntityRender_ShowBackAtt", m_hero, "Hero");
			return;
		}

		if(m_data->bContinueKill)
		{
			m_MainState.ResetState(EAttackRenderState_AttAnim, 200);
			Var boolV(true);
			ScriptSys::GetInstance().Execute_2("EntityRender_ShowContinueAttAtt", m_hero, "Hero", &boolV, "Var");
			return;
		}
		else if (m_data->bContinueSkill)
		{
			Var boolV(false);
			m_MainState.ResetState(EAttackRenderState_AttAnim, 200);
			ScriptSys::GetInstance().Execute_2("EntityRender_ShowContinueAttAtt", m_hero, "Hero", &boolV, "Var");
			return;
		}

		if (m_skill) 
		{
			if (m_skill->m_rangeType == 0) //远程攻击
			{
				if (m_skill->m_type != 0 && m_skillEffect)
				{
					//todo施法特效，神兽
					if(m_skillEffect->m_attEffectType == 1)
					{
						//m_MainState.ResetState(EAttackRenderState_AttEffect, 0);
						//return;
					}
				}
			}
		}
	}	

	m_MainState.ResetState(EAttackRenderState_AttAnim, 1);
}

void EntityRender::PlayNormalFarAtt(EAnims anim)
{
	if (m_skillEffect->m_gasSound != "")
	{
		string soundPath = "Data/Sound/" + m_skillEffect->m_gasSound;
		SimpleAudioEngine::sharedEngine()->playEffect(soundPath.c_str());
	}
	SetAnimation(anim, 1);
	m_MainState.ResetState(EAttackRenderState_AttAniming, 5000);
}

void EntityRender::PlayNormalNearAtt(EAnims anim)
{
	if (m_skillEffect->m_gasSound != "")
	{
		string soundPath = "Data/Sound/" + m_skillEffect->m_gasSound;
		SimpleAudioEngine::sharedEngine()->playEffect(soundPath.c_str());
	}
	float time = 0.15f;

	Hero* targetHero = NULL;
	for (size_t i= 0; i < m_data->skillTargetList.size(); i++)
	{
		if (m_data->skillTargetList[0].skipType == 0)
		{
			targetHero = dynamic_cast<Hero*>(EntityMgr::GetInstance().GetEntityById(m_data->skillTargetList[i].entityId));
			break;
		}	
	}

	if (targetHero && m_data->type != 1)
	{
		SetFlipX(GetFlipByTargetFormationPos(targetHero));
		if (fabsf(GetDestPosByHero(targetHero).x - GetCurPos().x) > 100)
		{
			MoveAction(GetDestPosByHero(targetHero), time, EAnimsRun);
			m_nextAnimation = anim;
			m_MainState.ResetState(EAttackRenderState_Move, (time + 0.3)*1000);
			return;
		}
		else
		{
			SetPosition(GetDestPosByHero(targetHero));		
		}
	}
	
	SetAnimation(anim, 1);
	m_MainState.ResetState(EAttackRenderState_AttAniming, 5000);
}

void EntityRender::PlayButtletEffect()
{
	CheckIsNeedStop();
	if (m_skillEffect && m_skillEffect->m_bulletType == 1)
	{
		if (m_data)
		{
			Hero* lastHero = NULL;
			for (size_t i=m_data->buttletIndex; i<m_data->skillTargetList.size(); i++)
			{
				if (m_data->skillTargetList[i].skipType == 4)
				{
					// 自爆自身要跳过, 在回调的时候设置
					m_data->buttletIndex++;
					break;
					//continue;
				}

				if (m_data->skillTargetList[i].skipType)
				{
					continue;
				}

				Hero* hero = dynamic_cast<Hero*>(EntityMgr::GetInstance().GetEntityById(m_data->skillTargetList[i].entityId));
				if (hero)
				{
					if (hero == lastHero)
					{
						break;
					}

					string path = GetEffectPathByName(m_skillEffect->m_bulletName).c_str();//StringMgr::GetInstance().Format("Effect/%s/%s.ExportJson", m_skillEffect->m_bulletName.c_str(), m_skillEffect->m_bulletName.c_str());
					// load resource directly
					CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo(path.c_str());
					CCArmature *zidan = CCArmature::create(m_skillEffect->m_bulletName.c_str());
					CCArmatureDataManager::sharedArmatureDataManager()->removeArmatureFileInfo(path.c_str());
					if(zidan)
					{
						zidan->setUserData(&m_data->skillTargetList[i]);

						zidan->getAnimation()->setMovementEventCallFunc(this, movementEvent_selector(EntityRender::animationEvent));
						zidan->getAnimation()->play("0");

						//zidan->SetLife(nextTime);

						m_Layer[ERenderLayerType_Effect]->addChild(zidan, 1, GetEffectTag());

						ScriptSys::GetInstance().Execute_3("EntityRender_BulMove", this, "EntityRender", hero->GetRender(), "EntityRender", zidan, "CCArmature");
					}
					lastHero = hero;
				}

				m_data->buttletIndex++;
			}
		}
	}
	else if ( (m_skillEffect->m_bulletType == 2) || (m_skillEffect->m_bulletType == 3) )
	{
		string path = GetEffectPathByName(m_skillEffect->m_bulletName).c_str();//StringMgr::GetInstance().Format("Effect/%s/%s.ExportJson", m_skillEffect->m_bulletName.c_str(), m_skillEffect->m_bulletName.c_str());
		// load resource directly
		CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo(path.c_str());
		CCArmature *zidan = CCArmature::create(m_skillEffect->m_bulletName.c_str());
		CCArmatureDataManager::sharedArmatureDataManager()->removeArmatureFileInfo(path.c_str());

		if(zidan)
		{
			Hero* hero = this->GetHero();
			vector<CCPoint>& posList = hero->GetScene()->GetHeroPosList();
			zidan->getAnimation()->setMovementEventCallFunc(this, movementEvent_selector(EntityRender::animationEvent));
			zidan->getAnimation()->setFrameEventCallFunc(this, frameEvent_selector(EntityRender::onFrameEvent));
			zidan->getAnimation()->play("0");
			CCPoint point = ccp(posList[13].x, posList[13].y + 80);

			/*if (hero->GetInt(EHero_FormationPos) < 9 || this->GetHero()->GetInt(EHero_FormationPos) == 18)
			{
			}
			else*/

			int iHeroPos = this->GetHero()->GetInt(EHero_FormationPos);

			bool bRightSide = (m_skillEffect->m_bulletType == 2) && ( (iHeroPos < 9) || (iHeroPos == GSProto::ATT_GOLDANIMAL_POS) );
			bRightSide = bRightSide || ( (m_skillEffect->m_bulletType == 3) && ( ( (iHeroPos >= 9) && (iHeroPos < GSProto::MAX_BATTLE_MEMBER_SIZE) ) || (iHeroPos == GSProto::TARGET_GOLDANIMAL_POS) ) ); 
			if(!bRightSide)
			{
				point = ccp(posList[4].x, posList[4].y + 80);
				zidan->setScaleX(-zidan->getScaleX());
			}

			zidan->setPosition(point);

			hero->GetScene()->GetEffectLayer()->addChild(zidan);
		}
	}
}

void EntityRender::CheckIsNeedStop()
{
	return;
	if (m_hero->GetInt(EHero_HP) <= 0 || m_Layer[ERenderLayerType_Buff]->getChildByTag(12))
	{
		if (FightMgr::GetInstance().GetMainState().GetState() == EFightState_PlayOneResultSet)
		{
			FightMgr::GetInstance().GetMainState().ResetState(EFightState_ChgProp, 1);
		}

		m_MainState.m_State = EAttackRenderState_None;
		SetAnimation(EAnimsIdle);

		return;
	}
}


void EntityRender::Update( uint detla )
{
	m_MainState.Update(detla);
	if (m_MainState.m_State != EAttackRenderState_None)
	{
		CheckIsNeedStop();
	}

	switch (m_MainState.m_State)
	{
	case EAttackRenderState_None:
		break;
	case EAttackRenderState_AttEffect:
		break;
	case EAttackRenderState_AttAnim:
		{
			if (m_MainState.IsExpired())
			{
				if (m_skill) 
				{
					if (m_skill->m_type)	//漂技能
					{
						ScriptSys::GetInstance().Execute_2("EntityRender_ShowSkillName", m_hero, "Hero", GetSkillDataByID(m_data->skillId), "stSkillData");
					}

					if (m_skill->m_rangeType == 0) //远程攻击
					{			
						if (m_skill->m_type == 1 && m_hero->GetInt(EHero_ObjectType) != GSProto::en_class_GodAnimal)
						{
							PlayNormalFarAtt(EAnimsAtt2);
						}
						else if (m_skill->m_type == 4 && m_hero->GetInt(EHero_ObjectType) != GSProto::en_class_GodAnimal)
						{
							PlayNormalFarAtt(EAnimsAtt3);
						}
						else
						{
							PlayNormalFarAtt();
						}
					}
					else//近战
					{
						if (m_skill->m_type == 1 && m_hero->GetInt(EHero_ObjectType) != GSProto::en_class_GodAnimal)
						{
							PlayNormalNearAtt(EAnimsAtt2);
						}
						else if (m_skill->m_type == 4 && m_hero->GetInt(EHero_ObjectType) != GSProto::en_class_GodAnimal)
						{
							PlayNormalNearAtt(EAnimsAtt3);
						}
						else//普通攻击
						{
							PlayNormalNearAtt();
						}
					}
				}
				else
				{
					PlayNormalNearAtt();
				}	
			}	
		}
		break;
	case EAttackRenderState_Move:
		if (m_MainState.IsExpired())
		{
			//this->stopAllActions();
			SetAnimation(m_nextAnimation);
			m_MainState.ResetState(EAttackRenderState_AttAniming, 5000);
		}
		break;
	case EAttackRenderState_AttAniming:
		if (m_MainState.IsExpired())
		{
			m_MainState.ResetState(EAttackRenderState_HurtEffect, 1);
		}
		break;
	case EAttackRenderState_BulletEffect:
		{
			
		}
		break;

	case EAttackRenderState_ZiBao:
		if (m_MainState.IsExpired())
		{
			m_MainState.ResetState(EAttackRenderState_HurtEffect, 1);
		}
		break;
		
	case EAttackRenderState_ZiBaoHurtEffect:
	case EAttackRenderState_HurtEffect:
		if (m_MainState.IsExpired())
		{
			if (m_data)
			{
				bool isNeedReset = true;
				Hero* lastHero = NULL;
				for (size_t i=m_data->buttletIndex; i<m_data->skillTargetList.size(); i++)
				{
					// 如果已经被标记为主公技能击杀，不处理伤害
					if(m_bMarkActorSkillKilled)
					{
						break;
					}

					Hero* hero = dynamic_cast<Hero*>(EntityMgr::GetInstance().GetEntityById(m_data->skillTargetList[i].entityId));
					if (hero)
					{
						// 注掉这个break,否则会导致卡死 hurt的Hero和finalHero不一致
						/*if ( (hero->GetInt(EHero_HP) <= 0) && (m_data->skillTargetList[i].skipType != 4) )
						{
							break;
						}*/
						
						// 不能注释，这个是用于多段攻击的。。。。
						// modified by feiwu, 好像没啥用，屏蔽
						if (m_skill&& m_skill->m_bMultiDamage && (hero == lastHero) )
						{
							break;
						}
						
						if (m_data->skillTargetList[i].skipType == 2)
						{
							/*string name = "diaochanAim";
							string path = GetEffectPathByName(name);//StringMgr::GetInstance().Format("Effect/%s/%s.ExportJson", m_skillEffect->m_bulletName.c_str(), m_skillEffect->m_bulletName.c_str());
							// load resource directly
							CCArmatureDataManager::sharedArmatureDataManager()->removeArmatureFileInfo(path.c_str());
							CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo(path.c_str());

							CCArmature *armature = CCArmature::create(name.c_str());

							if(armature)
							{
								armature->getAnimation()->setMovementEventCallFunc(this, movementEvent_selector(EntityRender::animationEvent));
								armature->getAnimation()->playByIndex(0);

								//zidan->SetLife(nextTime);

								m_Layer[ERenderLayerType_Effect]->addChild(armature, 1, GetEffectTag());
							}	*/					
						}
						else if (m_data->skillTargetList[i].skipType == 1)
						{

						}
						else if (m_data->skillTargetList[i].skipType == 4)
						{
							CCLog("ZiBao-------------");
							//todo	
							hero->GetRender()->SetAnimation(EAnimsZiBao, 1);
							hero->GetRender()->setAttacker(m_hero);
							m_data->buttletIndex++;
							m_MainState.ResetState(EAttackRenderState_ZiBao, 50000);
							return;
						}
						else if (m_data->skillTargetList[i].skipType == 3)
						{
							isNeedReset = false;

							if(m_MainState.m_State == EAttackRenderState_ZiBaoHurtEffect)
							{
								hero->GetRender()->PlayZiBaoHurtEffect();
							}
							else
							{
								hero->GetRender()->PlayHurtEffect(m_skillEffect);
							}

							if (m_skillEffect->m_targetSound != "")
							{
								string soundPath = "Data/Sound/" + m_skillEffect->m_targetSound;
								SimpleAudioEngine::sharedEngine()->playEffect(soundPath.c_str());
							}

							if (FightMgr::GetInstance().GetMainState().GetState() == EFightState_PlayOneResultSet)
							{
								FightMgr::GetInstance().GetMainState().ResetState(EFightState_ChgProp, 1);
							}
						}
						else
						{
							isNeedReset = false;
							if (m_data->skillTargetList[i].hitResult == GSProto::en_SkillHitResult_Doge) //闪避
							{
								hero->GetRender()->SetAnimation(EAnimsDoge, 1);
							}
							else
							{
								hero->GetRender()->SetAnimation(EAnimsHurt, 1);
							}

							/*if (m_skillEffect->m_targetEffectType == 2)
							{
								if (i == 0)
								{
									//todo 全屏特效
								}
							}
							else if (m_skillEffect->m_targetEffectType == 1)*/
							{
								if(m_MainState.m_State == EAttackRenderState_ZiBaoHurtEffect)
								{
									hero->GetRender()->PlayZiBaoHurtEffect();
								}
								else
								{
									hero->GetRender()->PlayHurtEffect(m_skillEffect);
								}

								if (m_skillEffect->m_targetSound != "")
								{
									string soundPath = "Data/Sound/" + m_skillEffect->m_targetSound;
									SimpleAudioEngine::sharedEngine()->playEffect(soundPath.c_str());
								}
							}
							ScriptSys::GetInstance().Execute_2("EntityRender_ShowDamageValue", hero, "Hero", &m_data->skillTargetList[i], "SkillTargetData");

							hero->SetUint(EHero_HP, m_data->skillTargetList[i].curHP);
							if (m_data->type == 0)
							{
								hero->SetUint(EHero_InitAnger, m_data->skillTargetList[i].curAnger);
								ScriptSys::GetInstance().Execute_2("EntityRender_RefreshAnger", hero, "Hero", hero->GetRender(), "EntityRender");
							}

							ScriptSys::GetInstance().Execute_2("EntityRender_RefreshHp", hero, "Hero", hero->GetRender(), "EntityRender");	
							m_data->isHurt = true;

							lastHero = hero;			
						}		
						m_data->buttletIndex++;
					}
				}

				if (isNeedReset)
				{
					if (FightMgr::GetInstance().GetMainState().GetState() == EFightState_PlayOneResultSet)
					{
						FightMgr::GetInstance().GetMainState().ResetState(EFightState_ChgProp, 1);
					}
					m_MainState.ResetState(EAttackRenderState_None, 0);
				}

				if (m_data->type == 0)
				{
					this->GetHero()->SetUint(EHero_InitAnger, m_data->angerAfterSkill);
					ScriptSys::GetInstance().Execute_2("EntityRender_RefreshAnger", this->GetHero(), "Hero", this, "EntityRender");
				}	

				Hero* godAnimal_1 = FightMgr::GetInstance().GetHeroByPos(GSProto::ATT_GOLDANIMAL_POS);
				if (godAnimal_1)
				{
					godAnimal_1->SetInt(EHero_InitAnger, m_data->aGodAnimalAnger);
					ScriptSys::GetInstance().Execute_2("EntityRender_RefreshAnger", godAnimal_1, "Hero", godAnimal_1->GetRender(), "EntityRender");
				}
				Hero* godAnimal_2 = FightMgr::GetInstance().GetHeroByPos(GSProto::TARGET_GOLDANIMAL_POS);
				if (godAnimal_2)
				{
					godAnimal_2->SetInt(EHero_InitAnger, m_data->tGodAnimalAnger);
					ScriptSys::GetInstance().Execute_2("EntityRender_RefreshAnger", godAnimal_2, "Hero", godAnimal_2->GetRender(), "EntityRender");
				}
			}

			m_MainState.ResetState(EAttackRenderState_None, 0);
		}
		break;
	}
}

EAnims EntityRender::GetCurrentAnimation()
{
	return m_currentAnimation;
}

void EntityRender::SetFlipX( bool flip )
{
	CCArmature* pnode = GetArmature();
	if(pnode)
	{
		float scaleX = fabsf(pnode->getScaleX());
		if (flip)
		{
			pnode->setScaleX(-scaleX);
		}
		else
		{
			pnode->setScaleX(scaleX);
		}
	}
}

bool EntityRender::IsFlipX()
{
	CCArmature* pnode = GetArmature();
	
	if (pnode)
	{
		return pnode->getScaleX() < 0;
	}
	
	return false;
}

void EntityRender::SetDirection( FmPoint targetPt )
{
	if( m_hero == NULL )
		return;
	float x = m_hero->GetPosition().x - targetPt.x;
	if( Math::IsFloatEqual( x, 0.0f ) )
	{
		return;
	}
	if( x > 0.0f )
		SetFlipX( true );
	else
		SetFlipX( false );
}

bool EntityRender::GetFlipByFormationPos()
{
	int pos = m_hero->GetInt(EHero_FormationPos);

	switch (pos)
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 18:
		return false;
	}

	return true;
}

bool EntityRender::GetFlipByTargetFormationPos(Hero* hero)
{
	int pos = hero->GetInt(EHero_FormationPos);

	switch (pos)
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 18:
		return true;
	}

	return false;
}

std::string EntityRender::GetAnimPath()
{
	return StringMgr::GetInstance().Format("AnimExport/%s/%s.ExportJson", GetAnimName().c_str(), GetAnimName().c_str());
}

float EntityRender::GetAnimScale()
{
	return m_hero->GetExtraInt("AnimScale")/10000.0f;
}


int EntityRender::GetZorder()
{
	return 1000 - m_hero->GetPosition().y;
}

cocos2d::CCSize EntityRender::GetContentSize()
{
	/*if (GetArmature())
	{
		return GetArmature()->getContentSize()*GetArmature()->getScaleY();
	}*/
	return m_contentSize*GetAnimScale()*getScale();
}


float EntityRender::GetHeight()
{
	return GetContentSize().height;
}

float EntityRender::GetHeight(int type)
{
	float yPos = 0.f;
	switch( type )
	{
	case ERenderYOffset_Foot:
		yPos = 0.f;
		break;
	case ERenderYOffset_HalfBody:
		yPos = GetHeight()/2.0f;
		break;
	case ERenderYOffset_Head:
		yPos = GetHeight();
		break;
	default:
		Assert("获取角色参考点高度错误,枚举类型错误!");
	}
	return yPos;
}

bool EntityRender::BackToPosition()
{
	if (m_Layer[ERenderLayerType_Buff]->getChildByTag(12))
	{
		return false;
	}
	if (getScale() != 1)
	{
		this->runAction(CCScaleTo::create(0.1f, 1));
	}
	this->setZOrder(GetZorder());
	SetFlipX(GetFlipByFormationPos());
	return MoveAction(m_hero->GetPosition(), 0.15f, EAnimsBack);
}

bool EntityRender::MoveAction(CCPoint targetPos,float time, EAnims anim)
{
	if (fabsf(targetPos.x - GetCurPos().x) > 100)
	{
		CCActionInterval* action1 = CCEaseSineOut::create(CCMoveTo::create(time, targetPos));
		//CCActionInterval* action2 = CCEaseSineOut::create(CCMoveBy::create(time/2, ccp(0, 60)));
		//CCActionInterval* action3 = CCEaseSineIn::create(CCMoveBy::create(time/2, ccp(0, -60)));
		CCCallFunc* endCall = CCCallFunc::create(this, callfunc_selector(EntityRender::MoveEndCall));
		//this->runAction(CCSequence::create(action2,  action3, NULL));
		this->runAction(CCSequence::create(action1,  endCall, NULL));
		SetAnimation(anim);
		return true;
	}
	else
	{
		SetPosition(targetPos);
		return false;
	}
}

void EntityRender::AddEffectByName( const char* name, const CCPoint& posOffset)
{
	RemoveEffect( name );
	EffectNode* enode = EffectMgr::GetInstance().GetEffectNode(name);
	if(enode)
	{
		AddEffect(enode,posOffset);
	}
}

void EntityRender::AddEffectById( int effectId )
{
	AddEffectById(effectId, CCPointZero);
}

void EntityRender::AddEffectById( int effectId, const CCPoint& posOffset)
{
	RemoveEffectById( effectId );
	EffectNode* enode = EffectMgr::GetInstance().GetEffectNodeById(effectId);
	if(enode)
	{
		AddEffect(enode,posOffset);
	}
}

void EntityRender::AddEffectByName( const char* name )
{
	AddEffectByName(name, CCPointZero);
}

void EntityRender::AddEffect( EffectNode* enode,const CCPoint& posOffset)
{
	int tag = GetEffectTag();
	if(tag != -1)
	{
		CCPoint finalPosOffset = posOffset;				
		finalPosOffset.y += GetHeight( enode->GetYOffsetType() );
		
		bool bNeedFilpX = false;
		if(enode->GetDir() == 1)
			bNeedFilpX = true;
		else if(enode->GetDir() == 2 && IsFlipX() == false)
			bNeedFilpX = true;
		else if(enode->GetDir() == 3 && IsFlipX())
			bNeedFilpX = true;

		
		if( bNeedFilpX )
		{
			enode->SetFlipX(bNeedFilpX);
			enode->setPosition(ccpAdd(ccp(-enode->getPosition().x, enode->getPosition().y),finalPosOffset));
		}
		else
		{
			enode->setPosition(ccpAdd(enode->getPosition(),finalPosOffset));
		}
		EffectMgr::GetInstance().AddEffect(m_Layer[ERenderLayerType_Effect],enode,tag);
	}
}

void EntityRender::AddEffect( EffectNode* enode )
{
	AddEffect(enode, CCPointZero);
}

void EntityRender::RemoveEffect( const char* name )
{
	for(int i = EEntityRenderTag_EffectStart;i<EEntityRenderTag_EffectEnd;i++)
	{
		EffectNode* enode = dynamic_cast<EffectNode*>(m_Layer[ERenderLayerType_Effect]->getChildByTag(i));
		if(enode == NULL)
			continue;
		if(enode->GetName().compare(name) == 0)
			m_Layer[ERenderLayerType_Effect]->removeChildByTag(i);
	}
}

void EntityRender::RemoveAllEffect()
{
	for(int i = EEntityRenderTag_EffectStart;i<EEntityRenderTag_EffectEnd;i++)
	{
		if (m_Layer[ERenderLayerType_Effect]->getChildByTag(i) != NULL)
			m_Layer[ERenderLayerType_Effect]->removeChildByTag(i);
	}
}

int EntityRender::GetEffectTag()
{
	int tag = -1;
	for(int i = EEntityRenderTag_EffectStart;i<EEntityRenderTag_EffectEnd;i++)
	{
		if(m_Layer[ERenderLayerType_Effect]->getChildByTag(i) == NULL)
		{
			tag = i;
			break;
		}
	}
	return tag;
}


CCLayer* EntityRender::GetLayer( uint type )
{
	if (type < ERenderLayerType_Max)
	{
		return m_Layer[type]; 
	}

	Assert(0);
	return NULL;
}

void EntityRender::FadeOut( int time )
{
	if(m_bFadeouted) return;

	m_bFadeouted = true;

	float fadeOutTime = ((float)time)/1000.0f;

	for (uint i = 0; i < ERenderLayerType_Max; ++i)
	{
		if( m_Layer[i] )
		{
			CCArray* children = m_Layer[i]->getChildren();
			CCObject* child = NULL;
			CCARRAY_FOREACH(children, child)
			{
				 CCNode* node = dynamic_cast<CCNode*>(child);
				 CCRGBAProtocol *pRGBAProtocol = dynamic_cast<CCRGBAProtocol*>(child);

				 if (pRGBAProtocol && node)
				 {
					 CCFadeOut* fadeOut = CCFadeOut::create(fadeOutTime);
					 CCCallFunc* endCall = CCCallFunc::create(node, callfunc_selector(EntityRender::FadeEndCall));
					 CCFiniteTimeAction* acts = CCSequence::create(fadeOut,endCall, NULL);
					 node->stopActionByTag(E_ActionType_FadeOut);
					 acts->setTag(E_ActionType_FadeOut);
					 node->runAction(acts);
				 }
			}
		}
	}
}

void EntityRender::Pause()
{
	this->pauseSchedulerAndActions();
	for (uint i = 0; i < ERenderLayerType_Max; ++i)
	{
		if( m_Layer[i] )
		{
			CCArray* children = m_Layer[i]->getChildren();
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
	}
}

void EntityRender::moveWithParabola(CCNode* node, CCPoint startPoint, CCPoint endPoint, float startAngle, float endAngle, float time)
{
		node->setRotation(startAngle);
		ccBezierConfig bezier;
		bezier.controlPoint_1 = startPoint;
		bezier.controlPoint_2 = ccp((startPoint.x + endPoint.x)/2, startPoint.y + 400);
		bezier.endPosition = endPoint;
		CCBezierTo* actionMove = CCBezierTo::create(time, bezier);
		CCRotateTo* actionRotate = CCRotateTo::create(time, endAngle);

		node->runAction(actionMove);
		node->runAction(actionRotate);
}

void EntityRender::Resume(int iSkillID)
{
	if (GetHero()->GetInt(EHero_HP) <= 0)
	{
		return;
	}

	this->resumeSchedulerAndActions();

	for (uint i = 0; i < ERenderLayerType_Max; ++i)
	{
		if( m_Layer[i] )
		{
			CCArray* children = m_Layer[i]->getChildren();
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
	}
}

void EntityRender::FadeEndCall()
{
	//m_NeedUpdateData = true;
}

void EntityRender::FadeIn( int time )
{
	float fadeOutTime = ((float)time)/1000.0f;

	for (uint i = 0; i < ERenderLayerType_Max; ++i)
	{
		if( m_Layer[i] )
		{
			CCArray* children = m_Layer[i]->getChildren();
			CCObject* child = NULL;
			CCARRAY_FOREACH(children, child)
			{
				CCNode* node = dynamic_cast<CCNode*>(child);
				CCRGBAProtocol *pRGBAProtocol = dynamic_cast<CCRGBAProtocol*>(child);

				if (pRGBAProtocol && node)
				{
					CCFadeIn* fadeIn = CCFadeIn::create(fadeOutTime);
					CCCallFunc* endCall = CCCallFunc::create(node, callfunc_selector(EntityRender::FadeEndCall));

					CCFiniteTimeAction* acts = CCSequence::create(fadeIn, endCall, NULL);
					node->stopActionByTag(E_ActionType_FadeIn);
					acts->setTag(E_ActionType_FadeIn);
					node->runAction(acts);
				}
			}
		}
	}
}

void EntityRender::RemoveEffectById( int effectId )
{
	for(int i = EEntityRenderTag_EffectStart;i<EEntityRenderTag_EffectEnd;i++)
	{
		EffectNode* enode = dynamic_cast<EffectNode*>(m_Layer[ERenderLayerType_Effect]->getChildByTag(i));
		if(enode == NULL)
			continue;
		if(enode->GetId() == effectId)
			m_Layer[ERenderLayerType_Effect]->removeChildByTag(i);
	}
 }
/*
void EntityRender::RemoveAllFightUI()
{
	//for(int i = EEntityRenderTag_FightUI;i<EEntityRenderTag_FightUI_End;i++)
	//{
		if (m_Layer[ERenderLayerType_UI]->getChildByTag(EEntityRenderTag_FightUI) != NULL)
		{
			//m_Layer[ERenderLayerType_UI]->getChildByTag(EEntityRenderTag_FightUI)->unscheduleAllSelectors();
			m_Layer[ERenderLayerType_UI]->getChildByTag(EEntityRenderTag_FightUI)->removeAllChildrenWithCleanup(true);
			//m_Layer[ERenderLayerType_UI]->removeChildByTag(EEntityRenderTag_FightUI);
		}
	//}
}
*/

string EntityRender::GetEffectPathByName(string name)
{
	return StringMgr::GetInstance().Format("Effect/%s/%s.ExportJson", name.c_str(), name.c_str());
}

void EntityRender::AddBuff(int id)
{
	/*if (GetHero()->GetInt(EHero_HP) <= 0)
	{
		return;
	}*/

	stBuffData* data = CDataManager::GetInstance().GetGameData<stBuffData>(DataFileBuff, id);
	if (data && data->m_name != "")
	{
		string path = GetEffectPathByName(data->m_name).c_str();//StringMgr::GetInstance().Format("Effect/%s/%s.ExportJson", m_skillEffect->m_bulletName.c_str(), m_skillEffect->m_bulletName.c_str());
		// load resource directly
		CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo(path.c_str());
		CCArmature *buff = CCArmature::create(data->m_name.c_str());
		CCArmatureDataManager::sharedArmatureDataManager()->removeArmatureFileInfo(path.c_str());
		if (buff)
		{
			buff->setPosition(ccp(0, 0));
			buff->getAnimation()->play("0");
			m_Layer[ERenderLayerType_Buff]->removeChildByTag(data->m_ID);
			m_Layer[ERenderLayerType_Buff]->addChild(buff, 0, data->m_ID);
			
			int pos = m_hero->GetInt(EHero_FormationPos);
			if ((pos >= 9 && pos <= 17) || pos == 19)
			{
				buff->setScaleX(-buff->getScaleX());
			}
		}
	}
}

void EntityRender::RemoveBuff(int id)
{
	stBuffData* data = CDataManager::GetInstance().GetGameData<stBuffData>(DataFileBuff, id);
	if (data)
	{
		m_Layer[ERenderLayerType_Buff]->removeChildByTag(data->m_ID);
	}
}

void EntityRender::SetPosition( CCPoint pos )
{
	/*for (uint i = 0; i < ERenderLayerType_Max; ++i)
	{
		m_Layer[i]->getParent()->setPosition(pos);
		break;
	}*/
	this->setPosition(pos);
}

string EntityRender::GetAnimName()
{
	return m_hero->GetExtraString("AnimName");
}

CCArmature* EntityRender::GetArmature()
{
	if (m_Layer[ERenderLayerType_Model])
	{
		return (CCArmature*)(m_Layer[ERenderLayerType_Model]->getChildByTag(1));
	}

	return NULL;
}

CCPoint EntityRender::GetDestPosByHero(Hero* hero)
{
	int pos = hero->GetUint(EHero_FormationPos);

	if (pos < 0 || pos >= 20 || hero->GetRender() == NULL)
	{
		return ccp(0,0);
	}

	CCPoint point = hero->GetRender()->GetCurPos();

	if (pos < 9 || pos == 18 )
	{
		point.x += 120;
	}
	else
	{
		point.x -= 120;
	}
	return point;
}

cocos2d::CCPoint EntityRender::GetCurPos()
{
	/*if (m_Layer[ERenderLayerType_Model]->getParent())
	{
	return m_Layer[ERenderLayerType_Model]->getParent()->getPosition();
	}*/
	return this->getPosition();
}

void EntityRender::MoveEndCall()
{
	SetAnimation(EAnimsIdle);
}

void EntityRender::onFrameEvent(cocos2d::extension::CCBone *bone, const char *evt, int originFrameIndex, int currentFrameIndex)
{
	if (strcmp(evt, "hurt") == 0)
	{
		m_MainState.ResetState(EAttackRenderState_HurtEffect, 1);
	}
}

void EntityRender::animationEvent(cocos2d::extension::CCArmature *armature, MovementEventType movementType, const char *movementID)
{
	if (movementType == LOOP_COMPLETE)
	{
		if (m_skillEffect && m_MainState.m_State == EAttackRenderState_BulletEffect)
		{
			if (m_skillEffect->m_bulletType == 1)
			{
				if (armature->getUserData())
				{
					ButtleEndCall(armature->getUserData());
				}
			}	
			else
			{
				m_MainState.ResetState(EAttackRenderState_HurtEffect, 1);
			}
		}
		
		
		armature->removeFromParent();
	}
}

void EntityRender::ButtleEndCall( void* data )
{
	CheckIsNeedStop();
	SkillTargetData* skillTarget = static_cast<SkillTargetData*>(data);
	if (skillTarget && m_data && m_skillEffect)
	{
		Hero* hero = dynamic_cast<Hero*>(EntityMgr::GetInstance().GetEntityById(skillTarget->entityId));
		if (hero)
		{
			bool isFinal = false;
			bool bSelfZiBao = false;
			for (int i=m_data->skillTargetList.size() -1; i>=0; i--)
			{
				if (m_data->skillTargetList[i].skipType == 4)
				{
					if(skillTarget->entityId == m_data->skillTargetList[i].entityId)
					{
						bSelfZiBao = true;
						break;
					}
				}
			}

			for (int i=m_data->skillTargetList.size() -1; i>=0; i--)
			{
				if (m_data->skillTargetList[i].skipType != 0)
				{
					continue;
				}

				if (&m_data->skillTargetList[i] == skillTarget)
				{
					isFinal = true;
				}

				break;
			}
			
			if (skillTarget->hitResult == GSProto::en_SkillHitResult_Doge) //闪避
			{
				hero->GetRender()->SetAnimation(EAnimsDoge, 1);
			}
			else
			{
				hero->GetRender()->SetAnimation(EAnimsHurt, 1);
			}

			/*if (m_skillEffect->m_targetEffectType == 2)
			{
				if (isFinal)
				{
					//todo 全屏特效
				}
			}
			else if (m_skillEffect->m_targetEffectType == 1)*/
			{
				hero->GetRender()->PlayHurtEffect(m_skillEffect);
			}

			if (m_data->type == 0)
			{
				hero->SetUint(EHero_InitAnger, skillTarget->curAnger);
				ScriptSys::GetInstance().Execute_2("EntityRender_RefreshAnger", hero, "Hero", hero->GetRender(), "EntityRender");
			}
			hero->SetUint(EHero_HP, skillTarget->curHP);
			ScriptSys::GetInstance().Execute_2("EntityRender_RefreshHp", hero, "Hero", hero->GetRender(), "EntityRender");
			m_data->isHurt = true;
			ScriptSys::GetInstance().Execute_2("EntityRender_ShowDamageValue", hero, "Hero", skillTarget, "SkillTargetData");

			if (isFinal)
			{
				if (m_data->type == 0)
				{
					this->GetHero()->SetUint(EHero_InitAnger, m_data->angerAfterSkill);
					ScriptSys::GetInstance().Execute_2("EntityRender_RefreshAnger", this->GetHero(), "Hero", this, "EntityRender");
				}
				
				Hero* godAnimal_1 = FightMgr::GetInstance().GetHeroByPos(GSProto::ATT_GOLDANIMAL_POS);
				if (godAnimal_1)
				{
					godAnimal_1->SetInt(EHero_InitAnger, m_data->aGodAnimalAnger);
					ScriptSys::GetInstance().Execute_2("EntityRender_RefreshAnger", godAnimal_1, "Hero", godAnimal_1->GetRender(), "EntityRender");
				}
				Hero* godAnimal_2 = FightMgr::GetInstance().GetHeroByPos(GSProto::TARGET_GOLDANIMAL_POS);
				if (godAnimal_2)
				{
					godAnimal_2->SetInt(EHero_InitAnger, m_data->tGodAnimalAnger);
					ScriptSys::GetInstance().Execute_2("EntityRender_RefreshAnger", godAnimal_2, "Hero", godAnimal_2->GetRender(), "EntityRender");
				}

				m_MainState.ResetState(EAttackRenderState_None, 0);
			}

			if(bSelfZiBao)
			{
				CCLog("ZiBao-------------");
				hero->GetRender()->SetAnimation(EAnimsZiBao, 1);
				hero->GetRender()->setAttacker(m_hero);
				m_MainState.ResetState(EAttackRenderState_ZiBao, 50000);
			}
		}
	}
}

void EntityRender::RevertAnim()
{
	CCArmature* armature = GetArmature();
	if (armature)
	{
		m_currentAnimation = m_lastAnimation;

		char szAnimationName[128] = {0};
		snprintf(szAnimationName, sizeof(szAnimationName), "%d", m_currentAnimation);
		armature->getAnimation()->play(szAnimationName, m_lastFrame, -1, -1);

		m_lastAnimation = EAnimsIdle;
		m_lastFrame = -1;
	}
}

RenderModeLayer* RenderModeLayer::create(EntityRender* host)
{
	RenderModeLayer *pRet = new RenderModeLayer();
	if (pRet)
	{
		pRet->m_Host = host;
		pRet->autorelease();
		return pRet;
	}
	else
	{
		CC_SAFE_DELETE(pRet);
		return NULL;
	}
}

RenderModeLayer::RenderModeLayer()
{

}

RenderModeLayer::~RenderModeLayer()
{

}

void RenderModeLayer::onEnter()
{
	CCLayer::onEnter();

	if(m_Host != NULL)
	{
		//char script[256] = { NULL };
		//sprintf( script, "EntityRender_UpdateData( '%s' , '%s' )", TypeConvert::ToString(m_Host->GetHero()->GetEntityId()).c_str(), "true" );
		//ScriptSys::GetInstance().ExecuteString( script );
	}
}

NS_FM_END