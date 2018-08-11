#include "FmFightMgr.h"
#include "Enum/FmPropertyIndex.h"
#include "GameEntity/FmHero.h"
#include "Script/FmScriptSys.h"
#include "Scene/FmScene.h"
#include "Enum/FmCommonEnum.h"
#include "NetWork/ClientSinkCallbackMgr.h"
#include "SceneMgr.h"
#include "FmEntityMgr.h"
#include "GameEntity/FmEntityRender.h"
#include "UI/FmUIMgr.h"
#include "NetWork/ClientConect.h"
#include "Sound/AudioEngine.h"


#define K_LIANSHA_TIME 10000

NS_FM_BEGIN
using namespace GSProto;


FightMgr::FightMgr()
{	
	m_MainState.ResetState(EFightState_None, 0);
	m_isRevAll = false;
	m_actionHero = NULL;
	m_isCg = false;
	m_index.m_resultIndex = 0;
	m_isCanItem = false;
	m_isCanRevAcion = true;
	m_isSkip = false;
	m_canSkip = false;
	m_fightType = 0;
	m_replaceIndex = 0;

	ClientSinkCallbackMgr::GetInstance().Register(GSProto::CMD_FIGHT_MEMBER, OnFightMember);
	ClientSinkCallbackMgr::GetInstance().Register(GSProto::CMD_FIGHT_ACTION, OnFightAction);
	ClientSinkCallbackMgr::GetInstance().Register(GSProto::CMD_FIGHT_FIN, OnFightFin);
}

void FightMgr::OnFightMember( int iCmd, GSProto::SCMessage& pkg )
{
	GSProto::Cmd_Sc_FightMember fighterMember;
	if(fighterMember.ParseFromString(pkg.strmsgbody() ) )
	{
		if (fighterMember.ifighttype() == GSProto::en_FightType_FirstFight)
		{
			FightMgr::GetInstance().m_isCg = true;
		}
		FightMgr::GetInstance().m_fightType = fighterMember.ifighttype();
		FightMgr::GetInstance().m_isCanItem = fighterMember.bcanuseactorskill();
		FightMgr::GetInstance().m_itemCount = fighterMember.iitemcount();
		FightMgr::GetInstance().m_canSkip = fighterMember.bcanskipbattle();
		SceneMgr::GetInstance().SwitchFightingScene(fighterMember.isceneid());

		FightMgr::GetInstance().InitFight();
		FightMgr::GetInstance().m_fightMember.CopyFrom(fighterMember);
	}
	//CCTextureCache::sharedTextureCache()->setAutoAsyncLoadEnable(true);
}

void FightMgr::OnFightAction( int iCmd, GSProto::SCMessage& pkg )
{
	GSProto::Cmd_Sc_FightAction fighterAction;
	if(fighterAction.ParseFromString(pkg.strmsgbody() ) )
	{
		if (FightMgr::GetInstance().m_isCanRevAcion)
		{
			FightMgr::GetInstance().m_fightActionList.push_back(fighterAction);
			CCLOG("%s", fighterAction.DebugString().c_str() );

			ScriptSys::GetInstance().Execute("Fihgt_RevAction");
		}
	}
}

void FightMgr::OnFightFin( int iCmd, GSProto::SCMessage& pkg )
{
	FightMgr::GetInstance().m_isRevAll = true;
}

FightMgr::~FightMgr()
{

}

void FightMgr::DecodeOneResultSet()
{
	if (m_fightResultList.size() > m_index.m_resultIndex)
	{
		GSProto::FightResulSet resultSet = m_fightResultList[m_index.m_resultIndex];
		bool isAttacker = false;

		m_curSkillResultData.buttletIndex = 0;
		m_curSkillResultData.skillTargetList.clear();

		m_curSkillResultData.bContinueSkill = resultSet.bcontinueskill();
		m_curSkillResultData.bContinueKill = resultSet.bcontinuekill();

		for (size_t i=0; i<resultSet.szresult().size(); i++)
		{
			const FightResult& fighterResult = resultSet.szresult().Get(i);
			if (fighterResult.iresulttype() == en_FightResult_SkillDamage)
			{
				Hero* targetHero = GetHeroByPos(fighterResult.damageresult().itargetpos());
				if (targetHero == NULL)
				{
					continue;
				}
				isAttacker = true;
				SkillTargetData data;
				data.entityId = targetHero->GetEntityId();
				data.damageHP = fighterResult.damageresult().idamagehp();
				data.hitResult = fighterResult.damageresult().ihitresult();
				data.curHP = fighterResult.damageresult().iresulthp();
				data.curAnger = fighterResult.damageresult().iresultanger();
				m_curSkillResultData.type = 0;

				m_curSkillResultData.skillTargetList.push_back(data);
				m_finalHero = targetHero;
			}
			else if (fighterResult.iresulttype() == en_FightResult_BackDamage)
			{
				Hero* attackHero = GetHeroByPos(fighterResult.backresult().iattackpos());
				if (attackHero == NULL)
				{
					assert(false);
				}
				
				SkillTargetData data;
				data.entityId = m_actionHero->GetEntityId();
				data.damageHP = fighterResult.backresult().idamagehp();
				data.curHP = fighterResult.backresult().itargetresulthp();
				//data.hitResult = fighterResult.backresult().ihitresult();
				//data.curAnger = m_actionHero->GetInt(EHero_InitAnger);
				m_curSkillResultData.skillTargetList.push_back(data);
				//反击是只有普攻
				m_curSkillResultData.skillId = fighterResult.backresult().ibackskillid();
				m_curSkillResultData.type = 1;
				//反击情况  直接跳出
				attackHero->GetRender()->PlayAttAnim(&m_curSkillResultData);
				m_MainState.ResetState(EFightState_PlayOneResultSet, 6000);
				m_finalHero = m_actionHero;
				return;
			}
			else if (fighterResult.iresulttype() == en_FightResult_ZiBao)
			{
				Hero* targetHero = GetHeroByPos(fighterResult.zibaoresult().ipos());
				if (targetHero == NULL)
				{
					continue;
				}
				SkillTargetData data;
				data.entityId = targetHero->GetEntityId();
				data.skipType = 4;
				m_curSkillResultData.skillTargetList.push_back(data);
			}
			else if (fighterResult.iresulttype() == en_FightResult_PropChg)
			{
				Hero* targetHero = GetHeroByPos(fighterResult.propchgresult().itargetpos());
				if (targetHero == NULL)
				{
					continue;
				}
				
				SkillTargetData data;
				data.entityId = targetHero->GetEntityId();
				if (m_curSkillResultData.bIsStageSkill)
				{
					data.skipType = 3;
					isAttacker = true;
				}
				else
				{
					data.skipType = 1;
				}
				m_curSkillResultData.skillTargetList.push_back(data);
			}
			else if (fighterResult.iresulttype() == en_FightResult_Relive)
			{
				Hero* targetHero = GetHeroByPos(fighterResult.reliveresult().itargetpos());
				if (targetHero == NULL)
				{
					continue;
				}
				SkillTargetData data;
				data.entityId = targetHero->GetEntityId();
				data.skipType = 2;
				m_curSkillResultData.skillTargetList.push_back(data);
			}
			else if (fighterResult.iresulttype() == en_FightResult_AddBuff)
			{
				Hero* targetHero = GetHeroByPos(fighterResult.addbuffresult().itargetpos());
				if (targetHero == NULL)
				{
					continue;
				}
				isAttacker = true;
				SkillTargetData data;
				data.entityId = targetHero->GetEntityId();
				data.skipType = 1;
				//m_curSkillResultData.type = 2;
				m_curSkillResultData.skillTargetList.push_back(data);
			}
		}

		if (m_curSkillResultData.skillTargetList.size() > 0)
		{
			// 感觉没必要，暂时屏蔽
			//if (m_actionHero && isAttacker)
			if (m_actionHero)
			{
				m_actionHero->GetRender()->PlayAttAnim(&m_curSkillResultData);

				m_MainState.ResetState(EFightState_PlayOneResultSet, 6000);
			}
			else
			{
				m_MainState.ResetState(EFightState_ChgProp, 1);
			}
		}
		else
		{
			m_MainState.ResetState(EFightState_EndOneResultSet, 1);
		}
		
	}
	else
	{
		m_MainState.ResetState(EFightState_EndOneAction, 1);
	}
}

void FightMgr::EndOneResultSet()
{
	if (m_fightResultList.size() > m_index.m_resultIndex)
	{
		map<Hero*, int> mapHeroLastHP;
		GSProto::FightResulSet resultSet = m_fightResultList[m_index.m_resultIndex];
		for (size_t i=0; i<resultSet.szresult().size(); i++)
		{
			const FightResult& fighterResult = resultSet.szresult().Get(i);
			if (fighterResult.iresulttype() == en_FightResult_SkillDamage)
			{
				Hero* targetHero = GetHeroByPos(fighterResult.damageresult().itargetpos());
				if (targetHero)
				{
					targetHero->SetUint(EHero_HP, fighterResult.damageresult().iresulthp());
					ScriptSys::GetInstance().Execute_2("EntityRender_RefreshHp", targetHero, "Hero", targetHero->GetRender(), "EntityRender");

					mapHeroLastHP[targetHero] = targetHero->GetUint(EHero_HP);
					//targetHero->SetUint(EHero_InitAnger, fighterResult.damageresult().iresultanger());
					/*if (targetHero->GetUint(EHero_HP) == 0)
					{
						targetHero->GetRender()->FadeOut(1000);
					}*/
				}
			}
			else if (fighterResult.iresulttype() == en_FightResult_BackDamage)
			{
				Hero* hero = GetHeroByPos(fighterResult.backresult().iattackpos());
				if (hero)
				{
					m_actionHero->SetUint(EHero_HP, fighterResult.backresult().itargetresulthp());
					ScriptSys::GetInstance().Execute_2("EntityRender_RefreshHp", m_actionHero, "Hero", m_actionHero->GetRender(), "EntityRender");
					
					if(m_actionHero)
					{
						mapHeroLastHP[m_actionHero] = m_actionHero->GetUint(EHero_HP);
					}
					/*if (m_actionHero && m_actionHero->GetUint(EHero_HP) == 0)
					{
						m_actionHero->GetRender()->FadeOut(1000);
					}*/

					//hero->GetRender()->BackToPosition();
				}
			}
			else if (fighterResult.iresulttype() == en_FightResult_CreateItem)
			{
				Hero* hero = GetHeroByPos(fighterResult.createitemresult().itargetpos());
				m_itemCount = fighterResult.createitemresult().iresultitemcount();
				//UIMgr::GetInstance().RefreshBaseWidgetByName("Fight");
				Var var(fighterResult.createitemresult().icreateitemcount());
				if (hero)
				{
					ScriptSys::GetInstance().Execute_2("Fight_ItemCountChg", hero, "Hero", &var, "Var");
				}			
			}
			else if (fighterResult.iresulttype() == en_FightResult_UseActorSkill)
			{
				m_itemCount = fighterResult.useactorskillresult().iresultitemcount();
				ScriptSys::GetInstance().Execute("Fight_ItemCountChg");
			}
			else if (fighterResult.iresulttype() == en_FightResult_PropChg)
			{
				Hero* targetHero = GetHeroByPos(fighterResult.propchgresult().itargetpos());
				if (targetHero)
				{
					int id = fighterResult.propchgresult().ilifeatt();
					int resultValue = fighterResult.propchgresult().iresultvalue();
					if(id == GSProto::en_LifeAtt_HP)
					{
						mapHeroLastHP[targetHero] = resultValue;
						targetHero->SetUint(EHero_HP, resultValue);
						ScriptSys::GetInstance().Execute_2("EntityRender_RefreshHp", targetHero, "Hero", targetHero->GetRender(), "EntityRender");
					}
				}
			}
			else if (fighterResult.iresulttype() == en_FightResult_Relive)
			{
				Hero* targetHero = GetHeroByPos(fighterResult.reliveresult().itargetpos());
				if (targetHero)
				{
					EntityRender* render = targetHero->GetRender();
					string name = "diaochanAim";
					string path = render->GetEffectPathByName(name);//StringMgr::GetInstance().Format("Effect/%s/%s.ExportJson", m_skillEffect->m_bulletName.c_str(), m_skillEffect->m_bulletName.c_str());
					// load resource directly
					CCArmatureDataManager::sharedArmatureDataManager()->removeArmatureFileInfo(path.c_str());
					CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo(path.c_str());

					CCArmature *armature = CCArmature::create(name.c_str());

					if(armature)
					{
						armature->getAnimation()->setMovementEventCallFunc(render, movementEvent_selector(EntityRender::animationEvent));
						armature->getAnimation()->play("0");

						//zidan->SetLife(nextTime);

						render->GetLayer(ERenderLayerType_Effect)->addChild(armature, 1, render->GetEffectTag());
					}
					int id = fighterResult.reliveresult().ilifeatt();
					int value = fighterResult.reliveresult().ichgvalue();
					vector<Var> vars;
					Var var1(id);
					Var var2(value);
					vars.push_back(var1);
					vars.push_back(var2);

					ScriptSys::GetInstance().Execute_3("EntityRender_ChgAtt", targetHero, "Hero", targetHero->GetRender(), "EntityRender", &vars, "vector<Var>");

					if (fighterResult.reliveresult().ilifeatt() == en_LifeAtt_HP)
					{
						targetHero->GetRender()->FadeIn(100);
						targetHero->SetInt(EHero_HP, fighterResult.reliveresult().iresultvalue());
						ScriptSys::GetInstance().Execute_2("EntityRender_RefreshHp", targetHero, "Hero", targetHero->GetRender(), "EntityRender");
						m_curSkillResultData.isHurt = true;
						
						mapHeroLastHP[targetHero] = targetHero->GetUint(EHero_HP);
					}
				}
			}
			else if (fighterResult.iresulttype() == en_FightResult_SyncHP)
			{
				Hero* targetHero = GetHeroByPos(fighterResult.synchpresult().itargetpos());
				if (targetHero)
				{
					targetHero->SetUint(EHero_HP, fighterResult.synchpresult().ihp());
					ScriptSys::GetInstance().Execute_2("EntityRender_RefreshHp", targetHero, "Hero", targetHero->GetRender(), "EntityRender");
					targetHero->SetUint(EHero_InitAnger, fighterResult.synchpresult().ianger());
					ScriptSys::GetInstance().Execute_2("EntityRender_RefreshAnger", targetHero, "Hero", targetHero->GetRender(), "EntityRender");

					mapHeroLastHP[targetHero] = targetHero->GetUint(EHero_HP);
				}
			}
		}

		for(map<Hero*, int>::iterator it = mapHeroLastHP.begin(); it != mapHeroLastHP.end(); it++)
		{
			if(it->second <= 0)
			{
				Hero* pHero = it->first;
				pHero->GetRender()->FadeOut(1000);
			}
		}

		m_index.m_resultIndex++;
	}
}

bool FightMgr::OnUpdate( uint delta )
{
	if (GetCurScene() && GetCurScene()->IsPause())
	{
		return false;
	}

	m_MainState.Update(delta);

	switch (m_MainState.GetState())
	{
	case EFightState_None:
		
		break;
	case EFightState_Pause:
		
		break;
	case EFightState_Init:
		if (m_MainState.IsExpired())
		{
			//if ( (m_fightActionList.size() > 0) && CCTextureCache::sharedTextureCache()->isAsyncLoadFinished() )
			if (m_fightActionList.size() > 0)
			{
				// 需要使用物品的战斗，由于有战前移动，可以异步，
				// 竞技场如果异步表现会有问题,会出现瞬间黑框
				if(m_isCanItem == false)
				{
					//CCTextureCache::sharedTextureCache()->setAutoAsyncLoadEnable(false);
				}

				initAllHeroUILayer();
				setAllFightObjVisible(true);

				PerformenceCounter tmpCounter("EFightState_Init");
				if (m_isCanItem)
				{
					ScriptSys::GetInstance().Execute("Fight_MoveInSceneFirst");
					//CCTextureCache::sharedTextureCache()->setAutoAsyncLoadEnable(false);
				}
				else
				{
					ScriptSys::GetInstance().Execute("Fight_StartAnim");
				}

				// 战斗中不允许异步加载
				CCTextureCache::sharedTextureCache()->setAutoAsyncLoadEnable(false);
				m_MainState.ResetState(EFightState_PlayStartAnim, 10000);
			}
		}
		break;
	case EFightState_PlayStartAnim:
		if (m_MainState.IsExpired())
		{
			if (m_isCanItem)
			{
				//ScriptSys::GetInstance().Execute_number("Fight_ShowWave", m_fightMember.bfinalfight() ? 1 : 0);
			}
			m_MainState.ResetState(EFightState_ReadNextAction, 1);	
		}
		break;
	case EFightState_ReadNextAction:
		{
			if (m_MainState.IsExpired())
			{
				if (UIMgr::GetInstance().IsCurBaseWidgetByName("Dialog_Left") || UIMgr::GetInstance().IsCurBaseWidgetByName("Dialog_Right"))
				{
					UIMgr::GetInstance().CloseCurBaseWidget();
				}
				
				if (m_index.m_actionIndex >= m_fightActionList.size())
				{
					if (m_isRevAll)
					{
						FinishFight();	
					}
				}
				else
				{
					m_MainState.ResetState(EFightState_DecodeOneAction, 1);
				}
			}
		}
		break;
	case EFightState_DecodeOneAction:
		{
			if (m_MainState.IsExpired())
			{
				//清除
				m_fightResultList.clear();
				m_actionHero = NULL;
				m_curSkillResultData.init();
				m_index.m_resultIndex = 0;

				const GSProto::Cmd_Sc_FightAction& action = m_fightActionList[m_index.m_actionIndex];
				for (size_t i=0; i<action.singleaction().szresultset().size(); i++)
				{
					m_fightResultList.push_back(action.singleaction().szresultset().Get(i));
				}
				if (action.singleaction().iactiontype() == en_FightAction_Skill)
				{
					const GSProto::FightSkillAction& skillAction = action.singleaction().skillaction();

					m_actionHero = GetHeroByPos(skillAction.iattackerpos());
					if (m_actionHero == NULL)
					{
						m_MainState.ResetState(EFightState_ReadNextAction, 1);
						m_index.m_actionIndex++;
						m_index.m_resultIndex = 0;
						return true;
					}

					m_curSkillResultData.skillId = skillAction.iskillid();
					m_curSkillResultData.aGodAnimalAnger = skillAction.iagodanimalanger();
					m_curSkillResultData.tGodAnimalAnger = skillAction.itgodanimalanger();
					m_curSkillResultData.angerAfterSkill = skillAction.iangerafterskill();
					m_curSkillResultData.bIsStageSkill = skillAction.bisstageskill();
				}
				else if (action.singleaction().iactiontype() == en_FightAction_DelBuff)
				{
					const GSProto::FightDelBuffAction& delAction = action.singleaction().delbuffaction();

					Hero* hero = GetHeroByPos(delAction.itargetpos());

					if (hero && hero->GetRender())
					{
						hero->GetRender()->RemoveBuff(delAction.ibuffid());
					}
				}	
				else if(action.singleaction().iactiontype() == en_FightAction_ReplaceMember)
				{
					const GSProto::ReplaceMemberAction& replaceAction = action.singleaction().replacememberaction();
					SceneMgr& sceneMgr = SceneMgr::GetInstance();
					sceneMgr.CleanEntitys(replaceAction.battackerreplace());


					ScriptSys::GetInstance().Execute("Fight_MoveToNextScene");
					m_MainState.ResetState(EFightState_ReplaceMember, 10000);
					return false;
				}
				else if (action.singleaction().iactiontype() == en_FightAction_Dlg)
				{
					const GSProto::DialogAction& dlgAction = action.singleaction().dlgaction();

					Var var(dlgAction.idlgid());

					ScriptSys::GetInstance().Execute_1("Fight_Dlg", &var, "Var");

					m_MainState.ResetState(EFightState_ReadNextAction, 10*1000);
					m_index.m_actionIndex++;
					m_index.m_resultIndex = 0;
					return true;
				}
				else if (action.singleaction().iactiontype() == en_FightAction_GuideUseItem)
				{
					ScriptSys::GetInstance().Execute("Fight_AddHp");
					m_MainState.ResetState(EFightState_ReadNextAction, 10000*1000);
					m_index.m_actionIndex++;
					m_index.m_resultIndex = 0;
					return true;
				}

				m_MainState.ResetState(EFightState_DecodeOneResultSet, 1);
			}
		}
		break;
	case EFightState_ReplaceMember:
		{
			if (m_MainState.IsExpired())
			{
				const GSProto::Cmd_Sc_FightAction& action = m_fightActionList[m_index.m_actionIndex];
				if(action.singleaction().iactiontype() == en_FightAction_ReplaceMember)
				{
					const GSProto::ReplaceMemberAction& replaceAction = action.singleaction().replacememberaction();
					if (m_isCanItem)
					{
						ScriptSys::GetInstance().Execute_number("Fight_ShowWave", replaceAction.bfinalfight() ? 1 : 0);
					}
					uint size = replaceAction.sznewfightobject_size();

					for (uint i=0; i<size; i++)
					{
						SceneMgr::GetInstance().AddEntityInToScene(replaceAction.sznewfightobject().Get(i), true);
					}
					initAllHeroUILayer();
				}
		
				m_MainState.ResetState(EFightState_DecodeOneResultSet, 10000);
			}
		}
		break;
	case EFightState_DecodeOneResultSet:
		{
			if (m_MainState.IsExpired())
			{
				DecodeOneResultSet();
			}
		}
		break;
	case EFightState_PlayOneResultSet:
		{
			if (m_MainState.IsExpired())
			{
				m_MainState.ResetState(EFightState_ChgProp, 1);
			}
		}
		break;
	case EFightState_ChgProp:
		{
			if (m_MainState.IsExpired())
			{
				if (m_index.m_resultIndex >= m_fightResultList.size())
				{
					m_MainState.ResetState(EFightState_EndOneResultSet, 1);
					return false;
				}

				map<Hero*, int> heroHPMap;
				GSProto::FightResulSet resultSet = m_fightResultList[m_index.m_resultIndex];
				for (size_t i=0; i<resultSet.szresult().size(); i++)
				{
					const FightResult& fighterResult = resultSet.szresult().Get(i);
					if (fighterResult.iresulttype() == en_FightResult_PropChg)
					{
						Hero* targetHero = GetHeroByPos(fighterResult.propchgresult().itargetpos());
						if (targetHero)
						{
							int id = fighterResult.propchgresult().ilifeatt();
							int value = fighterResult.propchgresult().ichgvalue();
							vector<Var> vars;
							Var var1(id);
							Var var2(value);
							vars.push_back(var1);
							vars.push_back(var2);

							ScriptSys::GetInstance().Execute_3("EntityRender_ChgAtt", targetHero, "Hero", targetHero->GetRender(), "EntityRender", &vars, "vector<Var>");

							if (fighterResult.propchgresult().ilifeatt() == en_LifeAtt_HP)
							{
								targetHero->SetInt(EHero_HP, fighterResult.propchgresult().iresultvalue());
								ScriptSys::GetInstance().Execute_2("EntityRender_RefreshHp", targetHero, "Hero", targetHero->GetRender(), "EntityRender");
								m_curSkillResultData.isHurt = true;

								heroHPMap[targetHero] = targetHero->GetUint(EHero_HP);
								/*if (targetHero->GetUint(EHero_HP) == 0)
								{
									targetHero->GetRender()->FadeOut(1000);
								}*/
							}
							else if (fighterResult.propchgresult().ilifeatt() == en_LifeAtt_MaxHP)
							{
								targetHero->SetInt(EHero_MaxHP, fighterResult.propchgresult().iresultvalue());
							}
							else if (fighterResult.propchgresult().ilifeatt() == en_LifeAtt_Anger)
							{
								targetHero->SetInt(EHero_InitAnger, fighterResult.propchgresult().iresultvalue());
								ScriptSys::GetInstance().Execute_2("EntityRender_RefreshAnger", targetHero, "Hero", targetHero->GetRender(), "EntityRender");
							}
						}
					}
					else if (fighterResult.iresulttype() == en_FightResult_AddBuff)
					{
						Hero* hero = GetHeroByPos(fighterResult.addbuffresult().itargetpos());

						if (hero && hero->GetRender())
						{
							hero->GetRender()->AddBuff(fighterResult.addbuffresult().ibuffid());
						}
					}
					else if (fighterResult.iresulttype() == en_FightResult_DelBuff)
					{
						Hero* hero = GetHeroByPos(fighterResult.delbuffresult().itargetpos());

						if (hero && hero->GetRender())
						{
							hero->GetRender()->RemoveBuff(fighterResult.delbuffresult().ibuffid());
						}
					}
					/*else if (fighterResult.iresulttype() == en_FightResult_CreateItem)
					{
						Hero* hero = GetHeroByPos(fighterResult.createitemresult().itargetpos());
						m_itemCount = fighterResult.createitemresult().iresultitemcount();
						//UIMgr::GetInstance().RefreshBaseWidgetByName("Fight");
						Var var(fighterResult.createitemresult().icreateitemcount());
						if (hero)
						{
							ScriptSys::GetInstance().Execute_2("Fight_ItemCountChg", hero, "Hero", &var, "Var");
						}			
					}
					else if (fighterResult.iresulttype() == en_FightResult_UseItem)
					{
						m_itemCount = fighterResult.useitemresult().iresultitemcount();
						ScriptSys::GetInstance().Execute("Fight_ItemCountChg");
					}*/
				}

				for(map<Hero*, int>::iterator it = heroHPMap.begin(); it != heroHPMap.end(); it++)
				{
					if(it->second == 0)
					{
						Hero* pHero = it->first;
						pHero->GetRender()->FadeOut(1000);
					}
				}

				if (m_curSkillResultData.bIsStageSkill)
				{
					m_MainState.ResetState(EFightState_EndOneResultSet, 600);
				}
				else
				{
					m_MainState.ResetState(EFightState_EndOneResultSet, 1);
				}			
			}	
		}
		break;
	case EFightState_EndOneResultSet:
		{
			if (m_MainState.IsExpired())
			{
				EndOneResultSet();
				
				if (m_fightResultList.size() <= m_index.m_resultIndex)
				{
					m_MainState.ResetState(EFightState_EndOneAction, 1);
				}
				else
				{
					m_MainState.ResetState(EFightState_DecodeOneResultSet, 1);
				}	
			}	
		}
		break;
	case EFightState_EndOneAction:
		{
			if (m_MainState.IsExpired())
			{
				if (m_fightActionList.size() <= m_index.m_actionIndex)
				{
					m_MainState.ResetState(EFightState_ReadNextAction, 1);
					return false;
				}
				const GSProto::Cmd_Sc_FightAction& action = m_fightActionList[m_index.m_actionIndex];

				FightResulSet fighterResult;
				if (action.singleaction().iactiontype() == en_FightAction_Skill)
				{
					const GSProto::FightSkillAction& skillAction = action.singleaction().skillaction();

					// 存在小招这种情况，怒气不是在效果里加的，这里同步下
					Hero* attacker = GetHeroByPos(skillAction.iattackerpos());
					if (attacker)
					{
						attacker->SetUint(EHero_InitAnger, skillAction.iangerafterskill());
						ScriptSys::GetInstance().Execute_2("EntityRender_RefreshAnger", attacker, "Hero", attacker->GetRender(), "EntityRender");
					}			
				}
				bool isNeedMove = false;
				if (m_actionHero && m_actionHero->GetUint(EHero_HP) > 0)
				{
					isNeedMove = m_actionHero->GetRender()->BackToPosition();
				}

				m_index.m_actionIndex++;
				m_index.m_resultIndex = 0;
				if(action.singleaction().iactiontype() == en_FightAction_ReplaceMember)
				{
					m_MainState.ResetState(EFightState_ReadNextAction, 1000);
				}
				else
				{
					if (isNeedMove)
					{
						m_MainState.ResetState(EFightState_ReadNextAction, 150);
					}
					else
					{
						m_MainState.ResetState(EFightState_ReadNextAction, 1);
					}
				}
			}
		}
		break;
	case EFightState_UI:	
		if (m_MainState.IsExpired())
		{
			if (m_isCg)
			{
				m_MainState.ResetState(EFightState_Pause, 1);
				ScriptSys::GetInstance().Execute("CreateRole_Third");
				m_isCg = false;
			}
			else
			{
				m_MainState.ResetState(EFightState_End, 3000*1000);
				ScriptSys::GetInstance().Execute("OpenFightResultUi");
			}
		}
		break;
	case EFightState_End:
		if (m_MainState.IsExpired())
		{
			m_MainState.ResetState(EFightState_None, 0);
			if (SceneMgr::GetInstance().GetLastState() == EGameState_Territory)
			{
				ServerEngine::CSMessage pkg;
				pkg.set_icmd(GSProto::CMD_MANOR_QUERY);
				ClientSink::GetInstance().SendPkg(pkg);
			}
			else if (SceneMgr::GetInstance().GetLastState() == EGameState_MyTerritory)
			{
				ServerEngine::CSMessage pkg;
				pkg.set_icmd(GSProto::CMD_MANOR_QUERY);
				ClientSink::GetInstance().SendPkg(pkg);
			}
			else
			{
				SceneMgr::GetInstance().SwitchToMainUi();
			}	
		}
		break;
	}
	return true;
}

void FightMgr::InitFight()
{
	m_actionHero = NULL;
	m_fightActionList.clear();
	m_isRevAll = false;
	m_MainState.ResetState(EFightState_Init, 1);
	m_index.m_actionIndex = 0;
	m_index.m_resultIndex = 0;

	m_lastIndex.m_actionIndex = -1;
	m_lastIndex.m_resultIndex = -1;

	m_fightResultList.clear();
	m_isCanRevAcion = true;

	ScriptSys::GetInstance().Execute("Fight_Init");
}

void FightMgr::Replay()
{
	if (m_MainState.GetState() == EFightState_End || m_MainState.GetState() == EFightState_UI)
	{
		m_index.m_actionIndex = 0;
		m_index.m_resultIndex = 0;
		m_canSkip = true;
		m_isCanItem = false;
		//UIMgr::GetInstance().CloseAllBaseWidget();
		SceneMgr::GetInstance().SwitchFightingScene(GetCurScene()->GetSceneId());
		m_MainState.ResetState(EFightState_Init, 1);
	}
}

Hero* FightMgr::GetHeroByPos(int pos)
{
	if (pos < 0 || pos > 19)
	{
		return NULL;
	}
	if (SceneMgr::GetInstance().GetCurScene())
	{
		set<uint>& heros = SceneMgr::GetInstance().GetCurScene()->GetEntityIds();

		for (set<uint>::iterator it=heros.begin(); it!=heros.end(); ++it)
		{
			Hero* hero = dynamic_cast<Hero*>(EntityMgr::GetInstance().GetEntityById(*it));
			if (hero)
			{
				if (hero->GetInt(EHero_FormationPos) == pos)
				{
					return hero;
				}
			}
		}
	}

	return NULL;
}

void FightMgr::setAllFightObjVisible(bool bVisible)
{
	if (SceneMgr::GetInstance().GetCurScene())
	{
		set<uint>& heros = SceneMgr::GetInstance().GetCurScene()->GetEntityIds();

		for (set<uint>::iterator it=heros.begin(); it!=heros.end(); ++it)
		{
			Hero* hero = dynamic_cast<Hero*>(EntityMgr::GetInstance().GetEntityById(*it));
			if (hero)
			{
				hero->GetRender()->setVisible(bVisible);
			}
		}
	}
}

void FightMgr::initAllHeroUILayer()
{
	if (SceneMgr::GetInstance().GetCurScene())
	{
		set<uint>& heros = SceneMgr::GetInstance().GetCurScene()->GetEntityIds();

		for (set<uint>::iterator it=heros.begin(); it!=heros.end(); ++it)
		{
			Hero* hero = dynamic_cast<Hero*>(EntityMgr::GetInstance().GetEntityById(*it));
			if (hero)
			{
				hero->GetRender()->initUILayer();
			}
		}
	}
}

void FightMgr::GetFightMgrIndex( FightMgrIndex& indexSt )
{
	if (m_MainState.m_State > EFightState_EndOneAction && m_MainState.m_State < EFightState_ReadNextAction)
	{
		indexSt.m_actionIndex = -1;
		indexSt.m_resultIndex = -1;
	}
	else
	{
		if (m_index.m_actionIndex >= m_fightActionList.size() 
			|| m_index.m_resultIndex >= m_fightResultList.size()
			|| (m_index.m_actionIndex == m_fightActionList.size() - 1 && m_curSkillResultData.isHurt == true) )
		{
			indexSt.m_actionIndex = -1;
			indexSt.m_resultIndex = -1;
		}
		else
		{
			const GSProto::Cmd_Sc_FightAction& action = m_fightActionList[m_index.m_actionIndex];
			if (action.singleaction().iactiontype() == en_FightAction_ReplaceMember
			)
			{
				indexSt.m_actionIndex = -1;
				indexSt.m_resultIndex = -1;
			}
			else
			{
				indexSt = m_index;
				m_isSkip = false;
				if (m_curSkillResultData.isHurt == false)
				{
					indexSt.m_isFirst = true;
				}
				else
				{
					indexSt.m_isFirst = false;
				}
			}
		}
	}

	if (indexSt.m_actionIndex < m_lastIndex.m_actionIndex || (indexSt.m_actionIndex == m_lastIndex.m_actionIndex && indexSt.m_resultIndex <= m_lastIndex.m_resultIndex))
	{
		CCLOG("ERROR index");
		indexSt.m_actionIndex = -1;
		indexSt.m_resultIndex = -1;
	}

	if (indexSt.m_actionIndex != -1 || indexSt.m_resultIndex != -1)
	{
		m_lastIndex = indexSt;
		m_isRevAll = false;
		m_isCanRevAcion = false;
		m_fightActionList.erase(m_fightActionList.begin() + indexSt.m_actionIndex, m_fightActionList.end());
	}
}

void FightMgr::FightResume()
{
	if (m_isSkip)
	{
		return;
	}

	//更新数据
	m_fightResultList.clear();

	if (m_index.m_actionIndex >= m_fightActionList.size())
	{
		return;
	}

	const GSProto::Cmd_Sc_FightAction& action = m_fightActionList[m_index.m_actionIndex];
	for (size_t i=0; i<action.singleaction().szresultset().size(); i++)
	{
		m_fightResultList.push_back(action.singleaction().szresultset().Get(i));
	}

	if (m_fightResultList.size() <= m_index.m_resultIndex)
	{
		return;
	}
	
	//加血
	//this->m_finalHero = NULL;
	
	Hero* pFinalHero = NULL;
	int iDamageCount = 0;
	GSProto::FightResulSet resultSet = m_fightResultList[m_index.m_resultIndex];
	for (size_t i=0; i<resultSet.szresult().size(); i++)
	{
		const FightResult& fighterResult = resultSet.szresult().Get(i);
		if (fighterResult.iresulttype() == en_FightResult_PropChg)
		{
			Hero* targetHero = GetHeroByPos(fighterResult.propchgresult().itargetpos());
			if (targetHero)
			{
				int id = fighterResult.propchgresult().ilifeatt();
				int value = fighterResult.propchgresult().ichgvalue();
				vector<Var> vars;
				Var var1(id);
				Var var2(value);
				vars.push_back(var1);
				vars.push_back(var2);

				ScriptSys::GetInstance().Execute_3("EntityRender_ChgAtt", targetHero, "Hero", targetHero->GetRender(), "EntityRender", &vars, "vector<Var>");

				if (fighterResult.propchgresult().ilifeatt() == en_LifeAtt_HP)
				{
					targetHero->SetInt(EHero_HP, fighterResult.propchgresult().iresultvalue());
					ScriptSys::GetInstance().Execute_2("EntityRender_RefreshHp", targetHero, "Hero", targetHero->GetRender(), "EntityRender");
					if (value > 0)
					{
						//GetAudioMgr()->playEffect("Data/Sound/ziliao.wav",false);
						targetHero->GetRender()->PlayUIEffect("jineng03Aim");
						//SimpleAudioEngine::sharedEngine()->playEffect("Data/Sound/zhugongji_kmfc2.wav");
					}
					else
					{
						targetHero->GetRender()->SetAnimation(EAnimsHurt);
						pFinalHero = targetHero;
						//SimpleAudioEngine::sharedEngine()->playEffect("Data/Sound/zhugongji_sxwj2.wav");
					}
				
					if (targetHero->GetUint(EHero_HP) == 0)
					{
						targetHero->GetRender()->stopAllActions();
						targetHero->GetRender()->markActorSkillKilled();
						targetHero->GetRender()->FadeOut(1000);
					}
				}
			}
		}
		else if(fighterResult.iresulttype() == en_FightResult_SkillDamage)
		{
			iDamageCount++;
			/*Hero* targetHero = GetHeroByPos(fighterResult.damageresult().itargetpos());
			if(targetHero)
			{
				int iResultHP = fighterResult.damageresult().idamagehp();
				lastHPMap[targetHero] = iResultHP;
			}*/
		}
		else if(fighterResult.iresulttype() == en_FightResult_BackDamage)
		{
			iDamageCount++;
			/*Hero* attackHero = GetHeroByPos(fighterResult.backresult().iattackpos());
			if(attackHero)
			{
				lastHPMap[attackHero] = ;
			}*/
		}
		else if (fighterResult.iresulttype() == en_FightResult_AddBuff)
		{
			Hero* hero = GetHeroByPos(fighterResult.addbuffresult().itargetpos());

			if (hero && hero->GetRender())
			{
				hero->GetRender()->AddBuff(fighterResult.addbuffresult().ibuffid());
				hero->GetRender()->stopAllActions();
			
				// 主公技能眩晕
				if(fighterResult.addbuffresult().ibuffid() == 12)
				{
					hero->GetRender()->SetAnimation(EAnimsIdle);
					//SimpleAudioEngine::sharedEngine()->playEffect("Data/Sound/zhugongji_lyhx2.wav");
				}
			}
		}
		else if (fighterResult.iresulttype() == en_FightResult_UseActorSkill)
		{
			m_itemCount = fighterResult.useactorskillresult().iresultitemcount();
			//UIMgr::GetInstance().RefreshBaseWidgetByName("Fight");
			ScriptSys::GetInstance().Execute("Fight_ItemCountChg");
		}
		else if (fighterResult.iresulttype() == en_FightResult_CreateItem)
		{
			Hero* hero = GetHeroByPos(fighterResult.createitemresult().itargetpos());
			m_itemCount = fighterResult.createitemresult().iresultitemcount();
			//UIMgr::GetInstance().RefreshBaseWidgetByName("Fight");
			Var var(fighterResult.createitemresult().icreateitemcount());
			if (hero)
			{
				ScriptSys::GetInstance().Execute_2("Fight_ItemCountChg", hero, "Hero", &var, "Var");
			}			
		}
	}

	m_index.m_resultIndex++;

	if(0 == iDamageCount)
	{
		if(pFinalHero)
		{
			this->m_finalHero = pFinalHero;
		}
		else
		{
			m_MainState.ResetState(EFightState_EndOneResultSet, 600);
		}
	}

	// 保护下，新手阶段需要这个
	if (m_index.m_resultIndex >= m_fightResultList.size())
	{
		m_index.m_actionIndex++;
		m_MainState.ResetState(EFightState_ReadNextAction, 600);
	}
	
	//更新下一个resultset
	/*if (m_index.m_resultIndex >= m_fightResultList.size())
	{
		m_index.m_actionIndex++;
		m_MainState.ResetState(EFightState_ReadNextAction, 600);
	}
	else
	{
		if(iCount == 0)
		{
			m_MainState.ResetState(EFightState_EndOneResultSet, 600);
		}
	}*/

	/*if (m_curSkillResultData.skillTargetList.size() == 0)
	{
		return;
	}

	resultSet = m_fightResultList[m_index.m_resultIndex];

	int index = 0;
	for (size_t i=0; i<resultSet.szresult().size(); i++)
	{
		if (index >= m_curSkillResultData.skillTargetList.size())
		{
			SkillTargetData data;
			m_curSkillResultData.skillTargetList.push_back(data);
		}
		const FightResult& fighterResult = resultSet.szresult().Get(i);
		if (fighterResult.iresulttype() == en_FightResult_SkillDamage)
		{
			Hero* targetHero = GetHeroByPos(fighterResult.damageresult().itargetpos());
			if (targetHero == NULL)
			{
				continue;
			}
			SkillTargetData& data = m_curSkillResultData.skillTargetList[index];
			data.entityId = targetHero->GetEntityId();
			data.damageHP = fighterResult.damageresult().idamagehp();
			data.hitResult = fighterResult.damageresult().ihitresult();
			data.curHP = fighterResult.damageresult().iresulthp();
			data.curAnger = fighterResult.damageresult().iresultanger();
			m_curSkillResultData.type = 0;
		}
		else if (fighterResult.iresulttype() == en_FightResult_BackDamage)
		{
			Hero* attackHero = GetHeroByPos(fighterResult.backresult().iattackpos());
			if (attackHero == NULL)
			{
				assert(false);
			}

			SkillTargetData& data = m_curSkillResultData.skillTargetList[index];
			data.entityId = m_actionHero->GetEntityId();
			data.damageHP = fighterResult.backresult().idamagehp();
			data.curHP = fighterResult.backresult().itargetresulthp();
			//data.hitResult = fighterResult.backresult().ihitresult();
			//data.curAnger = m_actionHero->GetInt(EHero_InitAnger);
			m_curSkillResultData.skillTargetList.push_back(data);
			//反击是只有普攻
			m_curSkillResultData.skillId = fighterResult.backresult().ibackskillid();
			m_curSkillResultData.type = 1;
			break;
		}
		else if (fighterResult.iresulttype() == en_FightResult_PropChg)
		{
			Hero* targetHero = GetHeroByPos(fighterResult.propchgresult().itargetpos());
			if (targetHero == NULL)
			{
				continue;
			}
			SkillTargetData& data = m_curSkillResultData.skillTargetList[index];
			data.entityId = targetHero->GetEntityId();
			data.skipType = 1;
		}
		else if (fighterResult.iresulttype() == en_FightResult_Relive)
		{
			Hero* targetHero = GetHeroByPos(fighterResult.reliveresult().itargetpos());
			if (targetHero == NULL)
			{
				continue;
			}
			SkillTargetData& data = m_curSkillResultData.skillTargetList[index];
			data.entityId = targetHero->GetEntityId();
			data.skipType = 2;
		}
		else if (fighterResult.iresulttype() == en_FightResult_AddBuff)
		{
			Hero* targetHero = GetHeroByPos(fighterResult.addbuffresult().itargetpos());
			if (targetHero == NULL)
			{
				continue;
			}
			SkillTargetData& data = m_curSkillResultData.skillTargetList[index];
			data.entityId = targetHero->GetEntityId();
			data.skipType = 1;
		}

		index++;
	}*/
}

void FightMgr::FinishFight()
{
	if (m_isCanItem && m_isCg == false)
	{
		ScriptSys::GetInstance().Execute("Fight_SendFinish");
		m_MainState.ResetState(EFightState_Pause, 0);
	}
	else
	{
		ScriptSys::GetInstance().Execute("Fight_ResultSound");
		m_MainState.ResetState(EFightState_UI, 1);
	}
}


FightMember FightMgr::getFightMember(){
	FightMember fightMember = FightMember();

	fightMember.leftPlayer.iHeadID = m_fightMember.lefthead().iheadid();
	fightMember.leftPlayer.strActorName = m_fightMember.lefthead().stractorname();
	fightMember.leftPlayer.iLevel = m_fightMember.lefthead().ilevel();

	fightMember.rightPlayer.iHeadID = m_fightMember.righthead().iheadid();
	fightMember.rightPlayer.strActorName = m_fightMember.righthead().stractorname();
	fightMember.rightPlayer.iLevel = m_fightMember.righthead().ilevel();

	fightMember.iIsAttackerWin = m_fightMember.iisattackerwin();
	
	uint size = m_fightMember.szfightobject_size();
	

	for (uint i=0; i<size; i++)
	{
		const FightObj& data = m_fightMember.szfightobject().Get(i);

		ObjectData_ newObject = ObjectData_();
		newObject.iBaseID = data.hero().ibaseid();
		newObject.iLevel = data.hero().ilevel();
		newObject.iLevelStep = data.hero().ilevelstep();
		newObject.iQuality = data.hero().iquality();
		newObject.location = data.ipos();

		fightMember.Objects[i] = newObject;
	}
	return fightMember;
}
NS_FM_END