--武魂花费
g_fightSoulCostMoney = {
	[1]=6000,
	[2]=8000,
	[3]=12000,
	[4]=20000,
	[5]=40000,
}
--开启等级

g_fightSoulLimitLvl = {
	[1]=0,
	[2]=29,
	[3]=59,
	[4]=89,
	[5]=119,
	[6]=149,
	[7]=179,
	[8]=209,
}

--神兽培养
g_GodAnimalDevelopCostMoney = {
	[1]=3000,
}

--英雄转换银币开销 (由于涉及到公式，所以这里用基数)
g_heroConVertMoney = 2000

--英雄转换道具ID，三星及其以上
g_heroConvertItemId = 10004

--现在1.0.7之后英雄转换不需要道具了，需要元宝，这里是消耗元宝的基数
g_heroConvertGod = 20

--创建军团消耗银币
g_CreatingLegionSliver = 500


--军团boss鼓舞加成 一次加成比例
g_LegionBossInspirationAtt = 20

--军团boss鼓舞最大次数
g_LegionBossInspirationMaxNum = 10

--军团领地颜色配置
g_LegionTerritoryColor = {
--1到9
  ccc3(0,255,255),
  ccc3(0,0,255),
  ccc3(138,43,226),
  ccc3(210,105,30),
  ccc3(255,140,0),
  ccc3(255,215,255),
  ccc3(245,223,179),
  ccc3(147,112,219),
  ccc3(218,112,214),
}

--世界Boss鼓舞加成比例
g_WorldBosssInspirationAtt = 20

--世界Boss鼓舞最大次数
g_WorldBossInspirationMaxNum = 5


--世界Boss名字
g_WorldBossName = 
{
	[10066] = "董卓",
}

--世界Boss图标
g_WorldBossImage = 
{
	[10066] = "WorldBoss_1.png"
}
--武魂殿格子开启等级限制
g_Eternal={
    [1]=1,
    [2]=1,
    [3]=3,
    [4]=5,
}
--铁匠铺格子开启等级限制
g_Blacksmith={
    [1]=1,
    [2]=1,
    [3]=3,
    [4]=5,
}
--抽取消耗
g_ShopCost={
	[1]=1,
	[2]=9,
	[3]=299,
	[4]=2688,
	[5]=1000,
	[6]=10000,
	[7]=399,
	[8]=3688,
	[9]=15000,	
	
}
--世界boss复活消耗
g_WorldBossResurrectionCost = 20

--公告开启等级限制
g_AnnouceMentShowLvllimit = 15

--主城界面没有操作的时间配置 秒 
g_ZhuChengFubenTishiTime = 10 

--主城界面没有操作的提示等级限制
g_ZhuChengFubenTishiLevelLimit = 20

--英雄魂魄出售单价
g_HeroSoulSellPiece = 1000

--英雄魂魄转换单价
g_HeroSoulConverPiece = 1000

--领地精力上限
g_HeroTerritoryVim = 10

--时间段
g_TaskTiliTimeAm_ShowTime = 11 * 3600 
g_TaskTiliTimeAm_TimeBegin = 12 * 3600

g_TaskTiliTimeAm_TimeEnd = 14 * 3600

g_TaskTiliTimePm_ShowTime = 15 * 3600 
g_TaskTiliTimePm_TimeBegin = 18 * 3600

g_TaskTiliTimePm_TimeEnd = 20 * 3600

--1、11点-14点显示豪华午餐，12-14点可领取，领取后豪华午餐就不见了
--2、15-20点显示豪华晚餐，16-20点可领取，领取后豪华晚餐就不见了
g_TaskTili_Info = {{"豪华午餐","每天12:00到14:00之间可领取豪华午餐","60"},
                   {"豪华晚餐","每天18:00到20:00之间可领取豪华晚餐","60"},
			      }
				  
				  
--商店高级VIP抽取英雄
g_HighVip={1001,1009,1020,1026,1032}

g_PhoneNumber={"130","131","132","155","156","176","185","186","134","135","136","137","138","139","150","151","152","157","158","159","182","183","187","188","147","133","153","181","180","189","177"}

g_ReChargeSystem_Info={"月卡","元宝","元宝","元宝","元宝","元宝","元宝"}