/********************************************************************
author:    由PropertyParser生成
summary:	根据属性配置文件,自动生成属性索引的枚举,不要手动修改该文件!
*********************************************************************/
#pragma once

NS_FM_BEGIN

enum EHeroPropertyIndex
{
	EHero_ID,	//唯一ID
	EHero_ObjectType,	//obj类型
	EHero_HeroId,	//英雄id
	EHero_Lvl,	//等级
	EHero_Anger,	//怒气
	EHero_LvlStep,	//等阶
	EHero_HeroExp,	//经验
	EHero_HP,	//当前生命值
	EHero_MaxHP,	//最大生命值
	EHero_IsDead,	//是否死亡
	EHero_InitAnger,	//怒气
	EHero_Att,	//攻击
	EHero_Hit,	//命中
	EHero_Doge,	//闪避
	EHero_Knock,	//暴击
	EHero_AntiKnock,	//韧性
	EHero_Wreck,	//破击
	EHero_Block,	//格挡
	EHero_Armor,	//护甲
	EHero_SunderArmor,	//破甲
	EHero_Skill,	//特殊技能
	EHero_FormationPos,	//阵形位置
	EHero_FightValue,	//战力
	EHero_HasFightSoul,	//是否携带武魂
	EHero_Price,	//出售价位
	EHero_HasEquip,	//是否携带装备
	EHero_Quality,	//品质
	EHero_Def,	//防御
};

enum EBagItemPropertyIndex
{
	EBagItem_ObjId,	//唯一ID
	EBagItem_Count,	//数量
	EBagItem_CombineNeedCount,	//合成需要的数量
};

enum EFightSoulPropertyIndex
{
	EFightSoul_ObjID,	//唯一ID
	EFightSoul_Exp,	//经验
	EFightSoul_NextLvExp,	//下一级经验
	EFightSoul_Pos,	//位置
	EFightSoul_Lock,	//锁定
	EFightSoul_Lvl,	//等级
	EFightSoul_HP,	//生命
	EFightSoul_InitAnger,	//怒气
	EFightSoul_Att,	//攻击
	EFightSoul_Hit,	//命中
	EFightSoul_Doge,	//闪避
	EFightSoul_Knock,	//暴击
	EFightSoul_AntiKnock,	//韧性
	EFightSoul_Wreck,	//破击
	EFightSoul_Block,	//格挡
	EFightSoul_Armor,	//护甲
	EFightSoul_SunderArmor,	//破甲
};

enum EPlayerPropertyIndex
{
	EPlayer_ObjId,	//id
	EPlayer_Lvl,	//等级
	EPlayer_Flag,	//标志位
	EPlayer_Exp,	//经验
	EPlayer_MaxExp,	//当前最大经验
	EPlayer_Silver,	//游戏币
	EPlayer_Gold,	//元宝
	EPlayer_Honor,	//荣誉
	EPlayer_FSChipCount,	//武魂碎片
	EPlayer_FormationLimit,	//阵形人数限制
	EPlayer_FightValue,	//战力
	EPlayer_Tili,	//体力
	EPlayer_Vigor,	//精力
	EPlayer_TiliMax,	//最大体力值
	EPlayer_HeroConvertCount,	//英雄当天转换次数
	EPlayer_HeroExp,	//修为
	EPlayer_VIP,	//VIP等级
	EPlayer_FunctionMask,	//功能标志位
	EPlayer_VipLevel,	//VIP等级
	EPlayer_VipExp,	//VIP当前经验
	EPlayer_VipLevelUpExp,	//VIP下级经验
	EPlayer_FunctionNotice,	//提示引导标志位
	EPlayer_HeadType,	//头像类型
	EPlayer_HeadId,	//头像ID
	EPlayer_LeftPhyStrength,	//已购买次数
	EPlayer_IsShowFirstPay,	//是否显示首冲
	EPlayer_LegionName,	//军团名字
};

enum EGodAnimalPropertyIndex
{
	EGodAnimal_ObjId,	//唯一ID
	EGodAnimal_Level,	//等级
	EGodAnimal_LevelStep,	//等阶
	EGodAnimal_IsActive,	//是否已经出战
};

enum EEquipPropertyIndex
{
	EEquip_ObjID,	//唯一ID
	EEquip_Pos,	//位置
	EEquip_SuitID,	//套装ID
	EEquip_SellMoney,	//出售价格
	EEquip_LvlLimit,	//等级
	EEquip_HP,	//生命
	EEquip_InitAnger,	//怒气
	EEquip_Att,	//攻击
	EEquip_Hit,	//命中
	EEquip_Doge,	//闪避
	EEquip_Knock,	//暴击
	EEquip_AntiKnock,	//韧性
	EEquip_Wreck,	//破击
	EEquip_Block,	//格挡
	EEquip_Armor,	//护甲
	EEquip_SkillDamage,	//绝技伤害
	EEquip_SunderArmor,	//破甲
};

NS_FM_END
