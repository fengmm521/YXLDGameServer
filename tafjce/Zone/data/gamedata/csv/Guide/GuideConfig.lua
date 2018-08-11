
--[[ 	----------- 注释 -----------
1. showBeginPanel 	promptText	textPanel	showMessage -- 这几个属性的文本配置在 TrigerGuideString.xml
	代码仅需要在初始化,不等于nil就行

--]]

g_ideNotify = {left=false, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="主公，您可以去征战天下，不断历练，让自己变强哦！"/>', pos={x=620, y=300} }

if (g_GuideDataTable == nil) then
	g_GuideDataTable = 
	{	
		[100] = {	-- 引导喝药
				[1] = {
					baseWidgetName="Fight",
					--widgetName="img_Hero1", -- 可触摸node			
					textPanel = {title="", text = '<lb t="0" sz="24" cl="255 255 230" c="让他们看看我英雄之力的厉害！"/>', pos={x=220, y=320} },
					endCondition = 
					{
						--Type="None",
					}					
				},
				[2] = {
					baseWidgetName="Fight",
					--widgetName="img_Hero1", -- 可触摸node			
					textPanel = {title="", text = '<lb t="0" sz="24" cl="255 255 230" c="屏幕下方有怒气栏，敌人被消灭后，您会获得怒气点，利用怒气点可以施放主公技能！"/>', pos={x=220, y=320} },
					endCondition = 
					{
						--Type="None",
					}					
				},
				[3] = {
					baseWidgetName="Fight",
					widgetName="Img_pos1_0", -- 可触摸node			
					textPanel = {title="", text = '<lb t="0" sz="24" cl="255 255 230" c="哇！您有3点怒气了，您赶紧施放神霄万剑吧，那可是全屏攻击技能哦！"/>', pos={x=220, y=320} },
					endCondition = 
					{
						Type="None",
					}					
				},
		},
		[1] = {	-- 副本引导
				[1] = {
					baseWidgetName = "MainUi",
					textPanel = {left=false, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="目前九州大陆战火纷乱，如果您想变得更强，就要去征战天下，四处历练！"/>', pos={x=620, y=320} },
					endCondition = 
					{
						--Type="None",
					}	
				},
		
				[2] = { -- 打开章节列表
					baseWidgetName="MainUi",
					widgetName="Button_fuben", -- 可触摸node			
					textPanel = {left=false, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="现在，就让我陪您去征战天下，开启强者之路吧！"/>', pos={x=620, y=320} },
					endCondition = 
					{
						Type="UiOpen",
						args = {"DungeonList"},
					}					
				},
				[3] = { -- 开启战斗
					baseWidgetName="DungeonList",
					widgetName="zhangjie_1", -- 可触摸node		
					
					textPanel = {left=false, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="主公，这里是大清王朝的地界，据说他们的统治者是一个萌妹子！！！您可以去拿他们练练手！"/>', pos={x=480, y=280} },
					endCondition = 
					{
						Type="UiOpen",
						args = {"Dungeon"},
					}					
				},	
				[4] = { -- 开启战斗
					baseWidgetName="Dungeon",
					widgetName="Button_tiaozhan", -- 可触摸node		
					
					textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="敌人就在前方了，赶紧去挑战他们吧！"/>', pos={x=500, y=280} },
					endCondition = 
					{
						Type="UiClose",
						args = {"Dungeon"},
						sendServer=true,
					}					
				},	
		},

		[2] = {	-- 抽取英雄引导
				[1] = {
					baseWidgetName="MainUi",
					textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="主公，虽然您这次被打败了，但是别灰心，您要振作起来，重新组建自己的势力！"/>', pos={x=220, y=320} },
					endCondition = 
					{
						--Type="None",
					}	
				},
		
				[2] = { -- 打开商城
					baseWidgetName="MainUi",
					widgetName="Button_shangcheng", -- 可触摸node			
					textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="我知道有一个好地方，能招募到不少历史上的英雄豪杰，请跟我来吧！"/>', pos={x=220, y=320} },
					endCondition = 
					{
						Type="UiOpen",
						args = {"Shop"},
					}					
				},
				[3] = { -- 抽取英雄
					baseWidgetName="Shop",
					widgetName="diamondButtonChilk", -- 可触摸node			
					--textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="点击这里招募英雄"/>', pos={x=300, y=300} },
					endCondition = 
					{
						Type="None",
					}					
				},
				[4] = { -- 抽取英雄
					baseWidgetName="Shop",
					widgetName="Button_chouyici", -- 可触摸node			
					--textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="点击这里招募英雄"/>', pos={x=300, y=300} },
					endCondition = 
					{
						--Type="UiOpen",
						--args = {"ShopCallHero"},
						Type="None",
						sendServer=true,
					}					
				},
				
				[5] = {
					baseWidgetName="ShopCallHero",
					widgetName="Button_139",
					textPanel = {left=false, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="哇！主公，您这次赚了，居然招募到张三丰了，他可是太极的创始人，闪避型的肉盾英雄！"/>', pos={x=740, y=320} },
					endCondition = 
					{
						Type="UiClose",
						args = {"ShopCallHero"},
					}	
				},
				
				[6] = { -- 抽取英雄
					baseWidgetName="LuckyDraw",
					widgetName="determine", -- 可触摸node			
					--textPanel = {left=false, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="这次赚了，居然招募到张三丰！！他可是太极的创始人，闪避型肉盾！"/>', pos={x=300, y=300} },
					endCondition = 
					{
						Type="UiClose",
						args = {"LuckyDraw"},
					}					
				},
				
				
				
				[7] = { -- 关闭界面
					baseWidgetName="Shop",
					widgetName="Button_close", -- 可触摸node		
					
					--textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="每过一段时间都会有免费招募的机会哦！"/>', pos={x=720, y=400} },
					endCondition = 
					{
						Type="UiClose",
						args = {"Shop"},
					}					
				},
		},
		[3] = {	-- 布阵引导
				[1] = { -- 打开布阵
					baseWidgetName="MainUi",
					widgetName="Button_buzhen", -- 可触摸node			
					textPanel = { left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="让我们开始布置战斗阵型吧！"/>', pos={x=620, y=320} },
					endCondition = 
					{
						Type="UiOpen",
						args = {"FormationSecond"},
					}					
				},
				[2] = {
					baseWidgetName="FormationSecond",
					widgetName="FormationHero_0", -- 可触摸node			
					textPanel = { left=false, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="点击张三丰，让他进入战斗阵型！"/>', pos={x=280, y=320} },
					endCondition = 
					{
						Type="None",
					
						--args = {"HeroListSecond"},
					}					
				},
				
				[3] = {
					baseWidgetName="FormationSecond",
					widgetName = "Image_54_From",
					
					textPanel = { left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="张三丰身法灵活，是一位闪避型的肉盾英雄，您可以将他放在阵型的最前排！"/>', pos={x=280, y=320} },
					moveTargetWidget = "Image_54_To",
					endCondition = 
					{
						Type="None",
						--args = {"HeroListSecond"},
					}	
				},
				
				[4] = { 
					baseWidgetName="FormationSecond",
					widgetName="Button_176", -- 可触摸node		
					
					textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="主公，记得最后要保存阵型哦！"/>', pos={x=280, y=320} },
					endCondition = 
					{
						Type="None",
						--args = {"HeroListSecond"},
						sendServer=true,	
					}					
				},
				--[[[4] = { -- 关闭界面
					baseWidgetName="Formation",
					widgetName="Button_close", -- 可触摸node		
					
					--textPanel = { title="", text = '<lb t="0" sz="24" cl="255 255 230" c="点击关卡图标进入战斗"/>', pos={x=480, y=320} },
					endCondition = 
					{
						Type="UiClose",
						args = {"Formation"},
						
					}					
				},--]]
				
		},
		[4] = {	--英雄升级
				[1] = { -- 打开英雄界面
					baseWidgetName="MainUi",
					widgetName="Button_yingxiong", -- 可触摸node			
					textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="主公，您可以手动去提升伙伴的等级，那样他们会更厉害的！！！"/>', pos={x=620, y=350} },
					endCondition = 
					{
						Type="UiOpen",
						args = {"HeroListSecond"},
					}					
				},
				[2] = { -- 切换到英雄升级
					baseWidgetName="HeroListSecond",
					widgetName="hero_0", -- 可触摸node		
					widgetPos={x=198, y=478},	
					textPanel = {left=false, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="首先来提升这个英雄的等级！"/>', pos={x=480, y=420} },
					endCondition = 
					{
						Type="UiOpen",
						args = {"HeroSystemSecond"},
					}					
				},
				[3] = { -- 提示切换到升级界面
					baseWidgetName="HeroSystemSecond",
					widgetName="Button_qhsj", -- 可触摸node		
					textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="先切换到升级界面"/>', pos={x=480, y=320} },
					endCondition = 
					{
						Type="None",
						sendServer=true,
					}					
				},
				
				[4] = { -- 提示文字
					baseWidgetName="HeroSystemSecond",
					--widgetName="Button_qhsj", -- 可触摸node		
					textPanel = {left=false, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="主公，您所有的英雄都共用左边的修为池，您可以根据自己的喜好，给任意一位英雄提升等级！"/>', pos={x=620, y=320} },
					endCondition = 
					{
						Type="None",
						--sendServer=true,
					}					
				},
				
				[5] = { -- 点击升级
					baseWidgetName="HeroSystemSecond",
					widgetName="Button_sj", -- 可触摸node		
					textPanel = {left=false, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="点击升级按钮让英雄升级~"/>', pos={x=620, y=320} },
					endCondition = 
					{
						Type="None",
					}					
				},
				[6] = { -- 点击升级
					baseWidgetName="HeroSystemSecond",
					widgetName="Button_sj", -- 可触摸node		
					textPanel = {left=false, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="点击继续升级！"/>', pos={x=620, y=320} },
					endCondition = 
					{
						Type="None",
					}					
				},
				
				
				--- 提示切换到甄嬛
				[7] = { -- 切换到甄嬛
					baseWidgetName="HeroSystemSecond",
					widgetName="Image_69_1", -- 可触摸node	
					eventWidgetName = "Panel_74_1",
					textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="别忘了还有其他英雄哦！点击右边切换！"/>', pos={x=480, y=320} },
					endCondition = 
					{
						Type="None",
					}					
				},
				
				[8] = { -- 甄嬛升级
					baseWidgetName="HeroSystemSecond",
					widgetName="Button_sj", -- 可触摸node		
					textPanel = {left=false, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="让这个英雄也提升一下等级！"/>', pos={x=620, y=320} },
					endCondition = 
					{
						Type="None",
					}					
				},
				
				
				[9] = { -- 甄嬛升级
					baseWidgetName="HeroSystemSecond",
					widgetName="Button_sj", -- 可触摸node		
					textPanel = {left=false, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="主公，继续点击升级按钮提升英雄等级哦！"/>', pos={x=620, y=320} },
					endCondition = 
					{
						Type="None",
					}					
				},
				
				
				[10] = { -- 关闭界面
					baseWidgetName="HeroSystemSecond",
					widgetName="Button_close", -- 可触摸node		

					endCondition = 
					{
						Type="UiClose",
						args = {"HeroSystemSecond"},
					}					
				},
				[11] = { -- 打开章节列表
					baseWidgetName="MainUi",
					widgetName="Button_fuben", -- 可触摸node			
					textPanel = {left=false, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="英雄等级提升了，快去征战天下试试，他们一定更厉害了！"/>', pos={x=560, y=320} },
					endCondition = 
					{
						Type="UiOpen",
						args = {"DungeonList"},
					}					
				},
				[12] = { -- 开启战斗
					baseWidgetName="DungeonList",
					widgetName="zhangjie_1", -- 可触摸node		
					
					--textPanel = {left=false, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="主公，这里是大清王朝的地界，据说他们的统治者是一个萌妹子！！！您可以去拿他们练练手！"/>', pos={x=480, y=280} },
					endCondition = 
					{
						Type="UiOpen",
						args = {"Dungeon"},
					}					
				},	
				[13] = { -- 开启战斗
					baseWidgetName="Dungeon",
					widgetName="Button_tiaozhan", -- 可触摸node		
					
					--textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="敌人就在前方了，赶紧去挑战他们吧！"/>', pos={x=500, y=280} },
					endCondition = 
					{
						Type="UiClose",
						args = {"Dungeon"},
					}					
				},	
		},
		[5] = {	--英雄喜好品
				[1] = { -- 打开英雄界面
					baseWidgetName="MainUi",
					widgetName="Button_yingxiong", -- 可触摸node			
					textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="您的英雄都很喜欢珍宝，配上珍宝之后，他们的战斗力也会大大提升哦！"/>', pos={x=620, y=350} },
					endCondition = 
					{
						Type="UiOpen",
						args = {"HeroListSecond"},
					}					
				},
				[2] = { -- 
					baseWidgetName="HeroListSecond",
					widgetName="hero_1", -- 可触摸node		
					widgetPos={x=550, y=478},		
					textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="点击打开英雄界面~"/>', pos={x=420, y=420}  },
					endCondition = 
					{
						Type="UiOpen",
						args = {"HeroSystemSecond"},
					}					
				},
				[3] = { -- 点击喜好品
					baseWidgetName="HeroSystemSecond",
					widgetName="xihaopin_6", -- 可触摸node		
					textPanel = {left=false, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="主公，有绿色“+”号标志的珍宝表示当前可以装备。点击可打开珍宝界面"/>', pos={x=620, y=300} },
					endCondition = 
					{
						Type="UiOpen",
						args = {"HeroSystemLoveThingOne"},	
						sendServer=true,
					}					
				},
				
				[4] = {
					baseWidgetName="HeroSystemLoveThingOne",
					widgetName="Button_zhuangbei",
					
					endCondition = 
					{
						Type="UiClose",
						args = {"HeroSystemLoveThingOne"},	
						
					}			
				},
				
				
				[5] = { -- 关闭界面
					baseWidgetName="HeroSystemSecond",
					widgetName="Button_close", -- 可触摸node		

					endCondition = 
					{
						Type="UiClose",
						args = {"HeroSystemSecond"},
					}					
				},
				
				
				[6] = { -- 打开章节列表
					baseWidgetName="MainUi",
					widgetName="Button_fuben", -- 可触摸node			
					textPanel = {left=false, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="张三丰装备喜好品后变得更强力了呢，赶紧去征战天下吧！"/>', pos={x=620, y=320} },
					endCondition = 
					{
						Type="UiOpen",
						args = {"DungeonList"},
					}					
				},
				[7] = { -- 开启战斗
					baseWidgetName="DungeonList",
					widgetName="zhangjie_1", -- 可触摸node		
					
					--textPanel = {left=false, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="主公，这里是大清王朝的地界，据说他们的统治者是一个萌妹子！！！您可以去拿他们练练手！"/>', pos={x=480, y=280} },
					endCondition = 
					{
						Type="UiOpen",
						args = {"Dungeon"},
					}					
				},	
				[8] = { -- 开启战斗
					baseWidgetName="Dungeon",
					widgetName="Button_tiaozhan", -- 可触摸node		
					
					--textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="敌人就在前方了，赶紧去挑战他们吧！"/>', pos={x=500, y=280} },
					endCondition = 
					{
						Type="UiClose",
						args = {"Dungeon"},
					}					
				},	
		},
		[6] = {	-- 布阵引导
				[1] = { -- 打开布阵
					baseWidgetName="MainUi",
					widgetName="Button_buzhen", -- 可触摸node			
					textPanel = { left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="我们主公5级咯，可以上阵更多英雄啦，点开布阵界面把新队友排上战斗阵容吧！"/>', pos={x=620, y=320} },
					endCondition = 
					{
						Type="UiOpen",
						args = {"FormationSecond"},
						sendServer=true,
					}					
				},
				
				[2] = {
					baseWidgetName="FormationSecond",
					widgetName="FormationHero_3", -- 可触摸node			
					textPanel = { left=false, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="点击新队友，让他上阵！"/>', pos={x=300, y=320} },
					endCondition = 
					{
						Type="None",
						--args = {"HeroListSecond"},
					}					
				
				},
				
				[3] = { 
					baseWidgetName="FormationSecond",
					widgetName="Button_176", -- 可触摸node		
					
					textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="主公记得要保存阵形哦!"/>', pos={x=280, y=320} },
					endCondition = 
					{
						Type="None",
						--args = {"HeroListSecond"},
						sendServer=true,
					}					
				},
				
				[4] = { -- 打开章节列表
					baseWidgetName="MainUi",
					widgetName="Button_fuben", -- 可触摸node			
					textPanel = {left=false, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="有了新伙伴的加入，主公的势力更强了，赶紧去征战天下吧！"/>', pos={x=620, y=320} },
					endCondition = 
					{
						Type="UiOpen",
						args = {"DungeonList"},
					}					
				},
				[5] = { -- 开启战斗
					baseWidgetName="DungeonList",
					widgetName="zhangjie_1", -- 可触摸node		
					
					--textPanel = {left=false, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="主公，这里是大清王朝的地界，据说他们的统治者是一个萌妹子！！！您可以去拿他们练练手！"/>', pos={x=480, y=280} },
					endCondition = 
					{
						Type="UiOpen",
						args = {"Dungeon"},
					}					
				},	
				[6] = { -- 开启战斗
					baseWidgetName="Dungeon",
					widgetName="Button_tiaozhan", -- 可触摸node		
					
					--textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="敌人就在前方了，赶紧去挑战他们吧！"/>', pos={x=500, y=280} },
					endCondition = 
					{
						Type="UiClose",
						args = {"Dungeon"},
					}					
				},
				
		},
		[7] = {	--英雄技能
				[1] = { -- 打开英雄界面
					baseWidgetName="MainUi",
					widgetName="Button_yingxiong", -- 可触摸node			
					textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="主公，每个英雄都有独门技能的，您快去帮他们提升技能等级吧！"/>', pos={x=620, y=350} },
					endCondition = 
					{
						Type="UiOpen",
						args = {"HeroListSecond"},
					}					
				},
				[2] = { -- 切换到英雄
					baseWidgetName="HeroListSecond",
					widgetName="hero_1", -- 可触摸node		
					widgetPos={x=550, y=478},	
					textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="点击打开英雄界面~"/>', pos={x=480, y=420} },
					endCondition = 
					{
						Type="UiOpen",
						args = {"HeroSystemSecond"},
					}					
				},
				[3] = { -- 切换到技能界面
					baseWidgetName="HeroSystemSecond",
					widgetName="Button_dfdfd", -- 可触摸node		
					textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="先切换到技能界面~"/>', pos={x=480, y=320} },
					endCondition = 
					{
						Type="None",
					}					
				},
				[4] = { -- 点击技能升级
					baseWidgetName="HeroSystemSecond",
					widgetName="Button_jnsj", -- 可触摸node		
					textPanel = {left=false, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="为英雄提升技能等级吧，让他变得更强~"/>', pos={x=620, y=320} },
					endCondition = 
					{
						Type="None",
						sendServer=true,
					}					
				},
                [5] = { -- 退出
					baseWidgetName="HeroSystemSecond",
					widgetName="Button_close", -- 可触摸node		
					--textPanel = {left=false, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="任务完成，回到主界面领取奖励吧"/>', pos={x=620, y=320} },
					endCondition = 
					{
						Type="None",
					}					
				},
		},
		[8] = {	--竞技场
				[1] = { 
					baseWidgetName="MainUi",
					widgetName="Button_jingjichang", -- 可触摸node			
					textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="比武大会里有各路英雄豪杰，主公您去和他们切磋一下吧，获得的名次越高，奖励就越高哦！"/>', pos={x=240, y=220} },
					endCondition = 
					{
						Type="UiOpen",
						args = {"Arena"},
					}					
				},
                [2] = { -- 挑战按钮
					baseWidgetName="Arena",
					widgetName="Button_tiaozhan_a", -- 可触摸node		
					textPanel = {left=false, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="点击这里发起挑战"/>', pos={x=620, y=320} },
					endCondition = 
					{
						Type="None",
						sendServer=true,
					}					
				},
                
		},
		[9] = {	--领地
				[1] = { 
					baseWidgetName="MainUi",
					widgetName="Button_lingdi", -- 可触摸node			
					textPanel = {left=false, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="主公，您有一块领地，专门产出各种资源！快去领地巡视一番吧！"/>', pos={x=460, y=350} },
					endCondition = 
					{
						Type="UiClose",
						args = {"MainUi"},
					}					
				},
				
				[2] = { -- 提示文字
					baseWidgetName="MyTerritory",	
					textPanel = {left=false, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="领地里有铜矿、修为矿、铁匠铺、武魂殿等建筑，能产出海量的铜币、英雄修为、珍宝、武魂！！！"/>', pos={x=520, y=320} },
					endCondition = 
					{
						Type="None",
						--sendServer=true,
					}					
				},
				
				[3] = { -- 提示点击铜矿
					baseWidgetName="MyTerritory",
					widgetName="Button_jinkuang", -- 可触摸node		
					textPanel = {left=false, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="铜矿专门产出铜币，铜矿级别越高，产出铜币越多！主公，您先试着将铜矿升到1级！"/>', pos={x=520, y=320} },
					endCondition = 
					{
						Type="None",
						--sendServer=true,
					}					
				},
				
				[4] = { -- 点击升级一座铜矿
					baseWidgetName="MyTerritory",
					widgetName="Button_shengji", -- 可触摸node		
					--textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="点击升级按钮"/>', pos={x=480, y=320} },
					endCondition = 
					{
						Type="None",	
					}					
				},
				
				
				[5] = { -- 提示再次升级铜矿
					baseWidgetName="MyTerritory",
					widgetName="Button_1288_2", -- 可触摸node		
					textPanel = {left=false, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="再升级一座铜矿吧！那样获得的铜币就更多了！"/>', pos={x=520, y=320} },
					endCondition = 
					{
						Type="None",
						sendServer=true,
					}					
				},
				
				[6] = { -- 点击升级一座铜矿
					baseWidgetName="MyTerritory",
					widgetName="Button_shengji", -- 可触摸node
					endCondition = 
					{
						Type="None",	
					}					
				},
				
				
				[7] = { -- 点击修为矿
					baseWidgetName="MyTerritory",
					widgetName="Button_xiuweikuang", -- 可触摸node		
					textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="这蓝色石头是修为矿，产出英雄修为，修为矿的级别越高，产出修为越多！现在将修为矿升级到1级吧！"/>', pos={x=220, y=220} },
					endCondition = 
					{
						Type="None",
					}					
				},
				[8] = { -- 点击修为矿升级
					baseWidgetName="MyTerritory",
					widgetName="Button_shengji", -- 可触摸node		
					endCondition = 
					{
						Type="None",
					}					
				},
				
				
				[9] = { -- 再次升级修为矿
					baseWidgetName="MyTerritory",
					widgetName="Button_1288_3", -- 可触摸node		
					textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="修为矿也是一样，矿越多获得的修为就越多，您一定要常来升级哦！"/>', pos={x=220, y=220} },
					endCondition = 
					{
						Type="None",
					}					
				},
				[10] = { -- 点击修为矿升级
					baseWidgetName="MyTerritory",
					widgetName="Button_shengji", -- 可触摸node		
					--textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="点击升级按钮"/>', pos={x=480, y=320} },
					endCondition = 
					{
						Type="None",
					}					
				},
				
				
				[11] = { -- 点击收取
					baseWidgetName="MyTerritory",
					widgetName="Button_118", -- 可触摸node		
					textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="主公，领地右边有当前的产量，您可以随时进行收取，但是每天的收取次数是有限的，您要珍惜哦！"/>', pos={x=620, y=220} },
					endCondition = 
					{
						Type="None",
					}					
				},
				
				[12] = { -- 确认收取
					baseWidgetName="",
					UseMessageBox = true,
					widgetName="Button_157",
					--widgetPos={x=620, y=240},
					--textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="领地右边有当前产量，随时可以收取，每天收取次数有限，记得收取哦"/>', pos={x=220, y=220} },
					endCondition = 
					{
						--Type="UiClose",
						--args = {"MessageBox"},
					}					
				},
				
				
				
				[13] = {
					baseWidgetName="MyTerritory",
					widgetName="Button_fanhui", -- 可触摸node	
					endCondition = 
					{
						Type="UiClose",
						args = {"MyTerritory"},
					}
				},
		},
		[10] = {	--领地
				[1] = { 
					baseWidgetName="MainUi",
					widgetName="Button_lingdi", -- 可触摸node			
					textPanel = {left=false, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="主公，刚才您去领地升级了铜矿和修为矿，现在咱们去看看武魂殿和铁匠铺吧。"/>', pos={x=620, y=450} },
					endCondition = 
					{
						Type="UiClose",
						args = {"MainUi"},
					}					
				},
				
				
				[2] = { -- 点击武魂殿
					baseWidgetName="MyTerritory",
					widgetName="Button_wuhundian", -- 可触摸node		
					textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="武魂殿是专门产出英雄武魂的地方。"/>', pos={x=400, y=320} },
					endCondition = 
					{
						Type="UiOpen",
						args = {"TerritoryEnternal"},
					}					
				},
				
				[3] = { -- 介绍武魂殿
					baseWidgetName="TerritoryEnternal",
					--widgetName="Button_shengji", -- 可触摸node		
					textPanel = {left=false, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="武魂足够的时候，可以召唤英雄或者给英雄升星星哦！升星之后的英雄很厉害的！"/>', pos={x=480, y=320} },
					endCondition = 
					{
						--Type="None",	
					}					
				},
				
				[4] = { -- 介绍祭炼功能
					baseWidgetName="TerritoryEnternal",
					--widgetName="Button_shengji", -- 可触摸node		
					textPanel = {left=false, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="武魂殿不仅可以升级，还能将您不喜欢的武魂刷新掉，您可以看看武魂殿左上角的具体规则哦！"/>', pos={x=480, y=320} },
					endCondition = 
					{
						--Type="None",	
						sendServer=true,
					}					
				},
				
				
				
				
				[5] = { -- 
					baseWidgetName="TerritoryEnternal",
					widgetName="Button_kaishi", -- 祭奠下		
					textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="现在，您就先试着祭炼一次武魂吧！"/>', pos={x=450, y=220} },
					endCondition = 
					{
						Type="None",
						
					}					
				},
				
				[6] = { -- 关闭武魂殿
					baseWidgetName="TerritoryEnternal",
					widgetName="Button_146", -- 关闭界面		
					endCondition = 
					{
						Type="UiClose",
						args = {"TerritoryEnternal"},
					}					
				},
				
				-- 引导点击铁匠铺
				[7] = {
					baseWidgetName="MyTerritory",
					widgetName="Button_1288_2_3", 		
					textPanel = {left=false, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="铁匠铺可以打造英雄喜欢的各种珍宝~您去看看吧~"/>', pos={x=620, y=320} },
					endCondition = 
					{
						Type="UiOpen",
						args = {"TerritoryEnternal"},
					}				
				},
				
				-- 介绍珍宝功能
				[8] = {
					baseWidgetName="TerritoryEnternal",
					
					textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="给英雄配备珍宝之后，战斗力会大增，当英雄佩戴满珍宝之后，还可以升阶呢！"/>', pos={x=480, y=320} },
					endCondition = 
					{
						
					}				
				},
				
				-- 介绍铁匠铺规则
				[9] = {
					baseWidgetName="TerritoryEnternal",
					
					textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="主公，铁匠铺是可以升级的，也可以刷新当前的珍宝~您可以看看铁匠铺左上角的具体规则哦！"/>', pos={x=480, y=320} },
					endCondition = 
					{
						
					}				
				},
				
				
				-- 打造珍宝
				[10] = {
					baseWidgetName="TerritoryEnternal",
					widgetName="Button_kaishi", 
					textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="现在，您就试试，亲自打造珍宝吧！"/>', pos={x=480, y=320} },
					endCondition = 
					{
						
					}				
				},
				
				-- 引导退出
				[11] = {
					baseWidgetName="TerritoryEnternal",
					widgetName="Button_146", 
					--textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="主公，现在开始打造珍宝吧"/>', pos={x=220, y=320} },
					endCondition = 
					{
						Type="UiClose",
						args = {"TerritoryEnternal"},
					}				
				},
				
				[12] = {
					baseWidgetName="MyTerritory",
					widgetName="Button_fanhui", -- 可触摸node	
					endCondition = 
					{
						Type="UiClose",
						args = {"MyTerritory"},
					}
				},
				
		},
		[11] = {	--通天塔
				[1] = { 
					baseWidgetName="MainUi",
					widgetName="Button_tongtianta", -- 可触摸node			
					textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="主公，九天幻境中可产出大量的修为、金币、珍宝与英雄！点击这里进入九天幻境~"/>', pos={x=300, y=300} },
					endCondition = 
					{
						Type="UiOpen",
						args = {"NineSky"},
						sendServer=true,
					}					
				},
		},
		
		[12] = {
			[1] = {
				baseWidgetName="MainUi",
				widgetName="Button_4", -- 可触摸node		
				textPanel = {left=false, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="嘻嘻~主公，您刚才完成了一个任务，快去领取奖励吧！"/>', pos={x=300, y=300} },
				endCondition = 
				{
					Type="UiOpen",
					args = {"TaskSystem"},
					sendServer=false,
				}		
			},
			[2] = {
				baseWidgetName="TaskSystem",
				widgetName="Btn_func_1",
				--textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="嗯？主公，恭喜您了！这奖励是三国名将赵云的武魂！赶紧点击领取吧！"/>', pos={x=300, y=300} },
				endCondition = 
				{
					sendServer=true,
				}	
			}
		
		},
		
		[13] = {
			[1] = {
				baseWidgetName="MainUi",
				widgetName="Button_yingxiong", -- 可触摸node		
				textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="英雄的武魂凑齐之后，就可以召唤这个英雄啦！我们赶紧去将新英雄召唤出来吧！"/>', pos={x=550, y=400} },
				endCondition = 
				{
					Type="UiOpen",
					args = {"HeroListSecond"},
					sendServer=false,
				}		
			},
			[2] = {
				baseWidgetName="HeroListSecond",
				widgetName="Panel_35_0_1", -- 可触摸node		
				widgetPos={x=200, y=478},
				--textPanel = {left=false, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="打开英雄升级界面"/>', pos={x=480, y=320} },				
				endCondition = 
				{
					
				}	
			},
			[3] = {
				baseWidgetName="",
				UseMessageBox = true,
				widgetName="Button_157", -- 可触摸node
				--widgetPos={x=620, y=240},
				--textPanel = {left=false, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="点击召唤英雄"/>', pos={x=480, y=520} },
				endCondition = 
				{
					Type="UiOpen",
					args = {"ShopCallHero"},
					sendServer=true,
				}	
			},
			
			[4] = {
					baseWidgetName="ShopCallHero",
					widgetName="Button_139",
					
					endCondition = 
					{
						Type="UiClose",
						args = {"ShopCallHero"},
					}	
				},
		},
		
		[14] = {
			[1] = { 
					baseWidgetName="MainUi",
					widgetName="Button_lingdi", -- 可触摸node			
					textPanel = {left=false, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="主公，您可以通过自己的领地，进入其他人的领地，进行掠夺哦！每次掠夺都会获得丰富的资源！"/>', pos={x=460, y=350} },
					endCondition = 
					{
						Type="UiClose",
						args = {"MainUi"},
						sendServer=true,
					}					
				},
			
			[2] = { -- 点击搜索对手
					baseWidgetName="MyTerritory",
					widgetName="Button_118_0", -- 可触摸node		
					textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="让我们来搜索对手宰肥羊吧！去掠夺他们的铜币、修为、武魂、珍宝~哈哈哈~"/>', pos={x=500, y=320} },
					endCondition = 
					{
						Type="UiOpen",
						args = {"Territory"},
						--sendServer=true,
					}					
				},		

		},
		
		[15] = {
			[1] = {
					baseWidgetName="GoldBox",
					widgetName="Button_lingqu", -- 可触摸node			
					textPanel = {left=false, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="哇~主公，您三星通关了！获得了海量的奖励，快快领奖吧！"/>', pos={x=700, y=180} },
					endCondition = 
					{
						Type="UiClose",
						args = {"GoldBox"},
						--sendServer=true,
					}			
			},
			
			[2] = {
					baseWidgetName="MainUiTop",
					isMainUiTopWidget = true;
					widgetName="Button_40_0", -- 可触摸node			
					textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="您拥有好多好多的修为，赶紧去为您的英雄提升等级吧！"/>', pos={x=600, y=180} },
					endCondition = 
					{
						--Type="UiClose",
						--args = {"GoldBox"},
						--sendServer=true,
					}			
			},
			
			[3] = {
					baseWidgetName="MainRight",
					widgetName="Button_yingxiong", -- 可触摸node	
					endCondition = 
					{
						Type="UiOpen",
						args = {"HeroListSecond"},
						--sendServer=true,
					}		
			},
			
			[4] = { -- 
					baseWidgetName="HeroListSecond",
					widgetName="hero_1", -- 可触摸node		
					widgetPos={x=550, y=478},		
					textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="点击打开英雄界面"/>', pos={x=280, y=420} },
					endCondition = 
					{
						Type="UiOpen",
						args = {"HeroSystemSecond"},
					}					
				},
				
			[5] = { -- 提示切换到升级界面
					baseWidgetName="HeroSystemSecond",
					widgetName="Button_qhsj", -- 可触摸node		
					--textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="先切换到升级界面"/>', pos={x=480, y=320} },
					endCondition = 
					{
						Type="None",
						sendServer=true,
					}					
				},
			[6] = { -- 点击升级
					baseWidgetName="HeroSystemSecond",
					widgetName="Button_sj", -- 可触摸node		
					textPanel = {left=false, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="主公，您有好多修为呢，赶紧升级吧，手要一直点点点哦~"/>', pos={x=620, y=320} },
					endCondition = 
					{
						Type="None",
					}					
				},
			[7] = { -- 点击升级
					baseWidgetName="HeroSystemSecond",
					widgetName="Button_sj", -- 可触摸node		
					textPanel = {left=false, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="点击升级按钮让英雄升级~"/>', pos={x=620, y=320} },
					endCondition = 
					{
						Type="None",
					}					
				},
			
				
				--- 提示切换到甄嬛
			[8] = { -- 切换到甄嬛
					baseWidgetName="HeroSystemSecond",
					widgetName="Image_69_1", -- 可触摸node	
					eventWidgetName = "Panel_74_1",
					textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="别忘了还有其他英雄哦！点击右边切换！"/>', pos={x=480, y=320} },
					endCondition = 
					{
						Type="None",
					}					
				},
				
			[9] = { -- 甄嬛升级
					baseWidgetName="HeroSystemSecond",
					widgetName="Button_sj", -- 可触摸node		
					textPanel = {left=false, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="让这个英雄也提升一下等级！"/>', pos={x=620, y=320} },
					endCondition = 
					{
						Type="None",
					}					
				},
				
				
			[10] = { -- 甄嬛升级
					baseWidgetName="HeroSystemSecond",
					widgetName="Button_sj", -- 可触摸node		
					textPanel = {left=false, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="主公，继续点击升级按钮提升英雄等级哦！"/>', pos={x=620, y=320} },
					endCondition = 
					{
						Type="None",
					}					
				},
				
			[11] = { -- 切换到甄嬛
					baseWidgetName="HeroSystemSecond",
					widgetName="Image_69_1", -- 可触摸node	
					eventWidgetName = "Panel_74_1",
					--textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="别忘了还有其他英雄哦！点击右边切换！"/>', pos={x=480, y=320} },
					endCondition = 
					{
						Type="None",
					}					
				},
				
			[12] = { -- 甄嬛升级
					baseWidgetName="HeroSystemSecond",
					widgetName="Button_sj", -- 可触摸node		
					--textPanel = {left=false, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="让这个英雄也提升一下等级！"/>', pos={x=620, y=320} },
					endCondition = 
					{
						Type="None",
					}					
				},
				
				
			[13] = { -- 甄嬛升级
					baseWidgetName="HeroSystemSecond",
					widgetName="Button_sj", -- 可触摸node		
					--textPanel = {left=false, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="主公，继续点击升级按钮提升英雄等级哦！"/>', pos={x=620, y=320} },
					endCondition = 
					{
						Type="None",
					}					
				},
				
			[14] = { -- 关闭界面
					baseWidgetName="HeroSystemSecond",
					widgetName="Button_close", -- 可触摸node		

					endCondition = 
					{
						Type="UiClose",
						args = {"HeroSystemSecond"},
					}					
				},
			[15] = { -- 下一张
					baseWidgetName="Dungeon",
					widgetName="Button_Next", -- 可触摸node		

					endCondition = 
					{
						Type="None",
					}					
				},
			[16] = { -- 下一张
					baseWidgetName="Dungeon",
					widgetName="Button_tiaozhan", -- 可触摸node		

					endCondition = 
					{
						Type="None",
					}					
				},
		},
		
		[16] = {
			[1] = { 
					baseWidgetName="MainUi",
					widgetName="Button_shenshou", -- 可触摸node			
					textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="主公，您终于达到20级了，可以穿装备了哦，穿上后实力大增！"/>', pos={x=220, y=400} },
					endCondition = 
					{
						Type="UiOpen",
						args = {"EquipFuben"},
						--sendServer=true,
					}					
				},
				
			[2] = {
					baseWidgetName="EquipFuben",
					widgetName="Panel_31", -- 可触摸node
					needCenter = true,
					textPanel = {left=false, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="铸剑山庄分为多个区域，每个区域产出不同的装备，现在进入金戈铁马去获得武器吧！"/>', pos={x=460, y=350} },
					endCondition = 
					{
						Type="UiOpen",
						args = {"Dungeon"},
						--sendServer=true,
					}		
			},
			
			[3] = {
					baseWidgetName="Dungeon",
					widgetName="Button_tiaozhan", -- 可触摸node		
					--textPanel = {left=false, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="现在先打一件装备给英雄穿上"/>', pos={x=460, y=350} },
					
					endCondition = 
					{
						Type="UiClose",
						args = {"Dungeon"},
						sendServer=true,
					}
			},
		},
		
		[17] = {
			[1] = {
				baseWidgetName="MainUi",
				widgetName="Button_yingxiong", -- 可触摸node		
				textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="恭喜主公获得了新的装备，现在打开英雄界面为英雄穿上装备吧！"/>', pos={x=550, y=400} },
				endCondition = 
				{
					Type="UiOpen",
					args = {"HeroListSecond"},
					sendServer=false,
				}		
			},
			
			[2] = { -- 点击英雄
					baseWidgetName="HeroListSecond",
					widgetName="hero_0", -- 可触摸node		
					widgetPos={x=198, y=478},	
					--textPanel = {left=false, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="首先来提升这个英雄的等级！"/>', pos={x=480, y=420} },
					endCondition = 
					{
						Type="UiOpen",
						args = {"HeroSystemSecond"},
					}					
				},
				
			[3] = { -- 提示切换到装备界面
					baseWidgetName="HeroSystemSecond",
					widgetName="Button_29", -- 可触摸node		
					textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="点击这里切换到装备界面"/>', pos={x=280, y=320} },
					endCondition = 
					{
						Type="None",
						sendServer=true,
						--sendServer=true,
					}					
				},
				
			
			[4] = { -- 提示切换到装备界面
					baseWidgetName="HeroSystemSecond",
					widgetName="xihaopin_1", -- 可触摸node		
					widgetPos={x=159, y=470},		
					textPanel = {left=false, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="有绿色“+”号的装备部位表示现在有多余的装备可以穿戴"/>', pos={x=480, y=320} },
					endCondition = 
					{
						Type="None",
						--sendServer=true,
					}					
				},
			
			[5] = { -- 提示切换到装备界面
					baseWidgetName="HeroSystemEquipmentTwo",
					widgetName="Button_15", -- 可触摸node		
					textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="现在点击装备按钮，为英雄装备上武器吧"/>', pos={x=280, y=380} },
					endCondition = 
					{
						Type="None",
						
					}					
				},
			
		},
        [18] = {--日常任务完成的奖励
			[1] = {
				baseWidgetName="MainUi",
				widgetName="Button_renwu", -- 可触摸node		
				textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="主公，恭喜您刚才完成了一个新的日常任务，现在去领取奖励吧！"/>', pos={x=300, y=300} },
				endCondition = 
				{
					Type="UiOpen",
					args = {"TaskSystem"},
				}		
			},
			[2] = {--点击完成任务
				baseWidgetName="TaskSystem",
				widgetName="Btn_func_1",
				--textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="嗯？主公，恭喜您了！这奖励是三国名将赵云的武魂！赶紧点击领取吧！"/>', pos={x=300, y=300} },
				endCondition = 
				{
					sendServer=true,
				}	
			},
		    [3] = {--退出任务界面
				baseWidgetName="TaskSystem",
				widgetName="Button_2",
				--textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="嗯？主公，恭喜您了！这奖励是三国名将赵云的武魂！赶紧点击领取吧！"/>', pos={x=300, y=300} },
				endCondition = 
				{
                    Type="None",
				}	
			},
		},
         [19] = {--技能升级日常任务引导
			[1] = {--打开任务界面
				baseWidgetName="MainUi",
				widgetName="Button_renwu", -- 可触摸node		
				textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="主公，你有更多日常任务可以完成，现在就去看看吧！"/>', pos={x=300, y=300} },
				endCondition = 
				{
					Type="UiOpen",
					args = {"TaskSystem"},
				}		
			},
		    [2] = { -- 打开英雄界面
					baseWidgetName="TaskSystem",
					widgetName="Btn_func_1", -- 可触摸node			
					textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="点击“立即前往”就可以开始任务啦！"/>', pos={x=620, y=350} },
					endCondition = 
					{
						Type="UiOpen",
						args = {"HeroListSecond"},
					}					
				},
		    [3] = { -- 切换到英雄
					baseWidgetName="HeroListSecond",
					widgetName="hero_1", -- 可触摸node		
					widgetPos={x=550, y=478},	
					textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="点击打开英雄界面~"/>', pos={x=480, y=420} },
					endCondition = 
					{
						Type="UiOpen",
						args = {"HeroSystemSecond"},
					}					
				},
		    [4] = { -- 切换到技能界面
					baseWidgetName="HeroSystemSecond",
					widgetName="Button_dfdfd", -- 可触摸node		
					textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="先切换到技能界面~"/>', pos={x=480, y=320} },
					endCondition = 
					{
						Type="None",
					}					
				},
		    [5] = { -- 点击技能升级
					baseWidgetName="HeroSystemSecond",
					widgetName="Button_jnsj", -- 可触摸node		
					textPanel = {left=false, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="为英雄提升技能等级吧，这样我们就可以完成任务了"/>', pos={x=620, y=320} },
					endCondition = 
					{
						Type="None",
						sendServer=true,
					}					
				},
            [6] = { -- 退出
					baseWidgetName="HeroSystemSecond",
					widgetName="Button_close", -- 可触摸node		
					textPanel = {left=false, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="任务完成，回到主界面领取奖励吧"/>', pos={x=620, y=320} },
					endCondition = 
					{
						Type="None",
					}					
				},
		},
		[20] = {--日常任务完成的奖励
			[1] = {
				baseWidgetName="MainUi",
				widgetName="Button_renwu", -- 可触摸node		
				textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="主公，完成日常任务可以获得丰富的奖励哦，现在去领取吧！"/>', pos={x=300, y=300} },
				endCondition = 
				{
					Type="UiOpen",
					args = {"TaskSystem"},
				}		
			},
			[2] = {--点击完成任务
				baseWidgetName="TaskSystem",
				widgetName="Btn_func_1",
				--textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="嗯？主公，恭喜您了！这奖励是三国名将赵云的武魂！赶紧点击领取吧！"/>', pos={x=300, y=300} },
				endCondition = 
				{
					sendServer=true,
				}	
			},
		    [3] = {--退出任务界面
				baseWidgetName="TaskSystem",
				widgetName="Button_2",
				--textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="嗯？主公，恭喜您了！这奖励是三国名将赵云的武魂！赶紧点击领取吧！"/>', pos={x=300, y=300} },
				endCondition = 
				{
                    Type="None",
				}	
			},
		},

	        [21] = {--洗练界面
			[1] = {
				baseWidgetName="MainUi",
				widgetName="Button_xilian", -- 可触摸node		
				textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="主公，装备锻造功能开启了！在这里你可以融炼多余的装备，融炼后可重新生成装备附加属性，还有一定机率获得高品质的装备哦！"/>', pos={x=300, y=300} },
				endCondition = 
				{
					Type="UiOpen",
					args = {"EquipWashs"},
                    sendServer=true,
				}		
			},
		},

        
         [1001] = {--从主界面到选择关卡界面-第一章
            special=true,
			[1] = {--打开关卡列表
				baseWidgetName="MainUi",
				widgetName="Button_fuben", -- 可触摸node		
				textPanel = {left=false, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="主公，快去征战天下，开启强者之路吧！"/>', pos={x=560, y=320} },
				endCondition = 
				{
					Type="UiOpen",
					args = {"DungeonList"},
				}		
			},
		    [2] = { -- 打开第一章节
					baseWidgetName="DungeonList",
					widgetName="zhangjie_1", -- 可触摸node			
					--textPanel = {left=false, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="据说紫禁皇妃可是个大美女哦~"/>', pos={x=500, y=350} },
					endCondition = 
					{
						Type="UiOpen",
						args = {"Dungeon"},
					}					
				},
		    [3] = { -- 开始战斗
					baseWidgetName="Dungeon",
					widgetName="Button_tiaozhan", -- 可触摸node		
					--textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="点击“挑战”，开始战斗吧！"/>', pos={x=480, y=320} },
					endCondition = 
					{
						Type="None",                
						sendServer=true,
					}					
				},
		},
        [1002] = {--从主界面到选择关卡界面-第二章
            special=true,
			[1] = {--打开关卡列表
				baseWidgetName="MainUi",
				widgetName="Button_fuben", -- 可触摸node		
				textPanel = {left=false, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="主公，快去征战天下，开启强者之路吧！"/>', pos={x=560, y=320} },
				endCondition = 
				{
					Type="UiOpen",
					args = {"DungeonList"},
				}		
			},
		    [2] = { -- 打开第二章节
					baseWidgetName="DungeonList",
					widgetName="Button_16_0", -- 可触摸node			
					--textPanel = {left=false, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="据说紫禁皇妃可是个大美女哦~"/>', pos={x=500, y=350} },
					endCondition = 
					{
						Type="UiOpen",
						args = {"Dungeon"},
					}					
				},
		    [3] = { -- 开始战斗
					baseWidgetName="Dungeon",
					widgetName="Button_tiaozhan", -- 可触摸node		
					--textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="点击“挑战”，开始战斗吧！"/>', pos={x=480, y=320} },
					endCondition = 
					{
						Type="None",                
						sendServer=true,
					}					
				},
		},
        [1003] = {--直接开始战斗
            special=true,
		    [1] = { -- 开始战斗
					baseWidgetName="Dungeon",
					widgetName="Button_tiaozhan", -- 可触摸node		
					--textPanel = {left=true, title="", text = '<lb t="0" sz="24" cl="255 255 230" c="点击“挑战”，开始战斗吧！"/>', pos={x=480, y=320} },
					endCondition = 
					{
						Type="None",                
						sendServer=true,
					}					
				},
		},
	};
end

