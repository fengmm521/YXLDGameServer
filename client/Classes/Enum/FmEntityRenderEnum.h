/********************************************************************
created:	2013-01-22
author:		Fish (于国平)
summary:	渲染相关的枚举
*********************************************************************/
#pragma once

NS_FM_BEGIN


enum EEntityRenderTag
{
	EEntityRenderTag_Begin = 100,	// 100以下都是换装使用

	EEntityRenderTag_EntityName,	// 名字
	//EEntityRenderTag_GuildName,		// 帮派名字
	//EEntityRenderTag_FighterName,	// 同伴名
	EEntityRenderTag_HPBg,			// 血条背景
	EEntityRenderTag_AngerBg,		// 怒气条背景
	EEntityRenderTag_HP,			// 血条
	EEntityRenderTag_Anger,			// 怒气条
	EEntityRenderTag_AngerFull,
	EEntityRenderTag_Lvl,		// 等级背景
	//EEntityRenderTag_Shout,			// 喊话
	EEntityRenderTag_ChengHao,		// 称号

	//-------------------------------
	//EEntityRenderTag_FightUI,	

	EEntityRenderTag_SkillEffect	= 130,	// 伤害值
	//EEntityRenderTag_BuffText	= 160,	// 属性buff提示文字
	//-------------------------------

	EEntityRenderTag_EffectStart = 200,
	EEntityRenderTag_EffectEnd = 220,
};

//用于计算y方向的遮挡关系
enum
{
	MAX_MAP_HEIGHT = 10000,
};
enum EZorderProperty
{
	EZorderProperty_BackBg ,
};

// 朝向
enum EDirection
{
	EDirection_Left = 0,	// 左
	EDirection_Right		// 右
};

//角色参考点高度类型
enum ERenderYOffsetType 
{
	ERenderYOffset_Foot,		// 脚底
	ERenderYOffset_HalfBody,	// 半身
	ERenderYOffset_Head,		// 头顶
};

//角色层级
enum ERenderLayerType
{
	ERenderLayerType_Model,		// 模型
	ERenderLayerType_Buff,
	ERenderLayerType_Effect,	// 特效
	ERenderLayerType_UI,		// UI(血条,名字)

	ERenderLayerType_Max,
};

NS_FM_END