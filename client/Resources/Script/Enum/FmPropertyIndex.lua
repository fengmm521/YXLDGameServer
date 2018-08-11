--[[
author:   由PropertyParser生成
summary:  根据属性配置文件,自动生成属性索引的枚举,不要手动修改该文件!
]]--


--------------Begin of enum EHeroPropertyIndex--------------------------------
EHero_ID = 0;		--唯一ID
EHero_ObjectType = 1;		--obj类型
EHero_HeroId = 2;		--英雄id
EHero_Lvl = 3;		--等级
EHero_Anger = 4;		--怒气
EHero_LvlStep = 5;		--等阶
EHero_HeroExp = 6;		--经验
EHero_HP = 7;		--当前生命值
EHero_MaxHP = 8;		--最大生命值
EHero_IsDead = 9;		--是否死亡
EHero_InitAnger = 10;		--怒气
EHero_Att = 11;		--攻击
EHero_Hit = 12;		--命中
EHero_Doge = 13;		--闪避
EHero_Knock = 14;		--暴击
EHero_AntiKnock = 15;		--韧性
EHero_Wreck = 16;		--破击
EHero_Block = 17;		--格挡
EHero_Armor = 18;		--护甲
EHero_SunderArmor = 19;		--破甲
EHero_Skill = 20;		--特殊技能
EHero_FormationPos = 21;		--阵形位置
EHero_FightValue = 22;		--战力
EHero_HasFightSoul = 23;		--是否携带武魂
EHero_Price = 24;		--出售价位
EHero_HasEquip = 25;		--是否携带装备
EHero_Quality = 26;		--品质
EHero_Def = 27;		--防御
--------------End of enum EHeroPropertyIndex--------------------------------


--------------Begin of enum EBagItemPropertyIndex--------------------------------
EBagItem_ObjId = 0;		--唯一ID
EBagItem_Count = 1;		--数量
EBagItem_CombineNeedCount = 2;		--合成需要的数量
--------------End of enum EBagItemPropertyIndex--------------------------------


--------------Begin of enum EFightSoulPropertyIndex--------------------------------
EFightSoul_ObjID = 0;		--唯一ID
EFightSoul_Exp = 1;		--经验
EFightSoul_NextLvExp = 2;		--下一级经验
EFightSoul_Pos = 3;		--位置
EFightSoul_Lock = 4;		--锁定
EFightSoul_Lvl = 5;		--等级
EFightSoul_HP = 6;		--生命
EFightSoul_InitAnger = 7;		--怒气
EFightSoul_Att = 8;		--攻击
EFightSoul_Hit = 9;		--命中
EFightSoul_Doge = 10;		--闪避
EFightSoul_Knock = 11;		--暴击
EFightSoul_AntiKnock = 12;		--韧性
EFightSoul_Wreck = 13;		--破击
EFightSoul_Block = 14;		--格挡
EFightSoul_Armor = 15;		--护甲
EFightSoul_SunderArmor = 16;		--破甲
--------------End of enum EFightSoulPropertyIndex--------------------------------


--------------Begin of enum EPlayerPropertyIndex--------------------------------
EPlayer_ObjId = 0;		--id
EPlayer_Lvl = 1;		--等级
EPlayer_Flag = 2;		--标志位
EPlayer_Exp = 3;		--经验
EPlayer_MaxExp = 4;		--当前最大经验
EPlayer_Silver = 5;		--游戏币
EPlayer_Gold = 6;		--元宝
EPlayer_Honor = 7;		--荣誉
EPlayer_FSChipCount = 8;		--武魂碎片
EPlayer_FormationLimit = 9;		--阵形人数限制
EPlayer_FightValue = 10;		--战力
EPlayer_Tili = 11;		--体力
EPlayer_Vigor = 12;		--精力
EPlayer_TiliMax = 13;		--最大体力值
EPlayer_HeroConvertCount = 14;		--英雄当天转换次数
EPlayer_HeroExp = 15;		--修为
EPlayer_VIP = 16;		--VIP等级
EPlayer_FunctionMask = 17;		--功能标志位
EPlayer_VipLevel = 18;		--VIP等级
EPlayer_VipExp = 19;		--VIP当前经验
EPlayer_VipLevelUpExp = 20;		--VIP下级经验
EPlayer_FunctionNotice = 21;		--提示引导标志位
EPlayer_HeadType = 22;		--头像类型
EPlayer_HeadId = 23;		--头像ID
EPlayer_LeftPhyStrength = 24;		--已购买次数
EPlayer_IsShowFirstPay = 25;		--是否显示首冲
EPlayer_LegionName = 26;		--军团名字
--------------End of enum EPlayerPropertyIndex--------------------------------


--------------Begin of enum EGodAnimalPropertyIndex--------------------------------
EGodAnimal_ObjId = 0;		--唯一ID
EGodAnimal_Level = 1;		--等级
EGodAnimal_LevelStep = 2;		--等阶
EGodAnimal_IsActive = 3;		--是否已经出战
--------------End of enum EGodAnimalPropertyIndex--------------------------------


--------------Begin of enum EEquipPropertyIndex--------------------------------
EEquip_ObjID = 0;		--唯一ID
EEquip_Pos = 1;		--位置
EEquip_SuitID = 2;		--套装ID
EEquip_SellMoney = 3;		--出售价格
EEquip_LvlLimit = 4;		--等级
EEquip_HP = 5;		--生命
EEquip_InitAnger = 6;		--怒气
EEquip_Att = 7;		--攻击
EEquip_Hit = 8;		--命中
EEquip_Doge = 9;		--闪避
EEquip_Knock = 10;		--暴击
EEquip_AntiKnock = 11;		--韧性
EEquip_Wreck = 12;		--破击
EEquip_Block = 13;		--格挡
EEquip_Armor = 14;		--护甲
EEquip_SkillDamage = 15;		--绝技伤害
EEquip_SunderArmor = 16;		--破甲
--------------End of enum EEquipPropertyIndex--------------------------------
