#pragma once
#include "FmConfig.h"
NS_FM_BEGIN
enum EMSTag  //main scene layer tag
{
	EMSTag_Map = 1, 
	EMSTag_Obj = 2,
	EMStag_Effect = 3,
	EMSTag_Ui = 4, //UI
	EMStag_SimpleChat = 20,
	EMSTag_DungeonEffect,
	EMSTag_GuideLayer,
	EMSTag_Guide_Notice,
	EMSTag_Ide_Notice, // 闲置提示
	EMSTag_LevelUp,
	EMSTag_EquipCard,
	EMSTag_EquipCardLook,
	EMSTag_Msg,
	EMSTag_Guide,
	EMSTag_GradeOpen,
	EMSTag_AccomplishTask,
	EMSTag_HaveNewTask,
	EMSTag_PromptBox, // 上浮提示框
	EMSTag_OtherPromptBox, 
	EMSTag_Marquee,
	EMSTag_Waiting, //菊花

};

enum EState
{
	EState_Logo,
	EState_MainMenu,
	EState_Gaming,

	EState_End,
};

enum EFormation
{
	EFormation_Max = 9,
};

enum E_ActionType
{
	E_ActionType_Anim,
	E_ActionType_FadeOut,
	E_ActionType_FadeIn,
};

enum EAnims
{
	EAnimsIdle = 0,
	EAnimsAtt,
	EAnimsAtt2,
	EAnimsHurt,
	EAnimsDoge,
	EAnimsRun,
	EAnimsBack,
	EAnimsAtt3,
	EAnimsMax,
	EAnimsNone,
	EAnimsZiBao,
};

enum EGameState
{
	EGameState_None = 0,
	EGameState_LoadScene, 
	EGameState_MainUi, //
	EGameState_MyTerritory,	//领地
	EGameState_Territory,
	EGameState_Fighting,	//战斗
};

enum ESLoadStep
{
	ESLoadStep_Init,
	ESLoadStep_ServerData,
	ESLoadStep_FreeLastScene,
	ESLoadStep_FreeTexTure,
	ESLoadStep_LoadTileMap,
	ESLoadStep_LoadEnity,
	ESLoadStep_Finished,
	ESLoadStep_SwitchMainMenu,
};

enum EFightState
{
	EFightState_None,
	EFightState_Init,
	EFightState_PlayStartAnim,
	EFightState_ReadNextAction,
	EFightState_DecodeOneAction,
	EFightState_ReplaceMember,
	EFightState_DecodeOneResultSet,
	EFightState_PlayOneResultSet,
	EFightState_ChgProp,
	EFightState_EndOneResultSet,
	EFightState_EndOneAction,
	EFightState_UI,
	EFightState_End,
	EFightState_Pause,
};

enum EAttackRenderState
{
	EAttackRenderState_None,
	EAttackRenderState_AttEffect,
	EAttackRenderState_AttAnim,
	EAttackRenderState_Move,
	EAttackRenderState_AttAniming,
	EAttackRenderState_BulletEffect,
	EAttackRenderState_ZiBao,
	EAttackRenderState_HurtEffect,
	EAttackRenderState_ZiBaoHurtEffect,
	EAttackRenderState_End,
};

enum EUIOpenType
{
	EUIOpenType_None,
	EUIOpenType_Dugeon,
	EUIOpenType_Formation,
	EUIOpenType_LegionBoss,
	EUIOpenType_Arena,
	EUIOpenType_LegionFighting,
	EUIOpenType_LastLegionFighting,
	EUIOpenType_CompBattle,
	EUIOpenType_CompBattleResult,
	EUIOpenType_WorldBoss,
	EUIOpenType_Hero,
	EUIOpenType_GodAnim,
	EUIOpenType_FightSoul,
	EUIOpenType_Babel,
	EUIOpenType_NineSky,
	EUIOpenType_EquipFuben,
};

enum EUIOpenAction
{
	EUIOpenAction_None,
	EUIOpenAction_Enlarge, //放大
	EUIOpenAction_MoveIn_Left,
	EUIOpenAction_MoveIn_Right,
	EUIOpenAction_MoveIn_Top,
	EUIOpenAction_MoveIn_Bottom,
	EUIOpenAction_FadeIn,
	EUIOpenAction_Reel,
	EUIOpenAction_Move_Right_Left,
};

enum EUICloseAction
{
	EUICloseAction_None,
	EUICloseAction_Lessen, //缩小
	EUICloseAction_MoveOut_Left,
	EUICloseAction_MoveOut_Right,
	EUICloseAction_MoveOut_Top,
	EUICloseAction_MoveOut_Bottom,
	EUICloseAction_FadeOut,
	EUICloseAction_Reel,
};


NS_FM_END