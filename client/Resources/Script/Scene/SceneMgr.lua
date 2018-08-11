---临时方法
if g_AnnouceMentSwitch == nil then
	g_AnnouceMentSwitch = 1
end

--判断主城界面是否有操作
if g_ZhuchenScollViewHasTouch == nil then 
   g_ZhuchenScollViewHasTouch = false  
end 

g_NoticeGuideClone = nil;
g_NoticeArrowClone = nil;
g_lastNotifyTime = os.time();
function ClickHeroList(sender,eventType) 
	if (eventType == TOUCH_EVENT_ENDED) then
		if (UI_GetBaseWidgetByName("NewHeroList"))then
			return;
		end
		
		UI_CloseAllSpecial();
		
		--if(UI_GetBaseWidgetByName("MainRight"))then
		--	UI_CloseCurBaseWidget(EUICloseAction_None)
		--end
		--g_SelectIndex = 0
		--local heroList = GetLocalPlayer():GetHeroList()
		--local heroEntity = GetEntityPById(heroList[0], "Hero")
		----[[if (heroEntity:GetExtraBool("IsQueryEquip") == false) then
		--	local tab = GameServer_pb.CMD_QUERY_HEROEQUIP_CS();
		--	tab.dwHeroObjectID = heroEntity:GetUint(EHero_ID);
		--	Packet_Full(GameServer_pb.CMD_QUERY_HEROEQUIP, tab);
		--end]]
			
		--local tab = GameServer_pb.Cmd_Cs_QueryHeroLevelInfo()
		--tab.dwHeroObjectID = heroEntity:GetUint(EHero_ID)
		--Packet_Full(GameServer_pb.CMD_QUERY_HERO_LEVELINFO,tab)
  --      ShowWaiting()
       --NewHeroList_InitNewHeroList()
       HeroListSecond_CreateHeroListLayout()
	end
end 

function ClickFormation(sender,eventType) 
	if (eventType == TOUCH_EVENT_ENDED) then
		if (UI_GetBaseWidgetByName("Formation"))then
			return;
		end
		UI_CloseAllSpecial();
		
		Formation_Create();
	end
end 

function ClickBayLayer(sender,eventType) 
	if (eventType == TOUCH_EVENT_ENDED) then
		if (UI_GetBaseWidgetByName("BagLayer"))then
			return;
		end
		UI_CloseAllSpecial();
		
		BagLayer_create();
	end
end

function ClickFriend(sender,eventType)
    -- body
    if eventType == TOUCH_EVENT_ENDED then
		if (UI_GetBaseWidgetByName("Friend"))then
			return;
		end
		UI_CloseAllSpecial();
    	Packet_Cmd(GameServer_pb.CMD_FRIENDSYSTEM_QUERY)
        ShowWaiting()
    end
end

function ClickTaskFunc(sender,eventType)
	-- body
	if eventType == TOUCH_EVENT_ENDED then 
		if (UI_GetBaseWidgetByName("TaskSystem"))then
			return;
		end
		UI_CloseAllSpecial();
        local tab = GameServer_pb.CMD_TASK_QUERY_TASKINFO_CS()
        tab.type = GameServer_pb.enTaskType_DayHave
        Packet_Full(GameServer_pb.CMD_TASK_QUERY_TASKINFO,tab)
        --Packet_Cmd(GameServer_pb.CMD_TASK_QUERY_TASKINFO)
        ShowWaiting()
        -- CreateHaveNewTask()
	end
end


function ClickTaskTwoFunc(sender,eventType)
   -- body
	if eventType == TOUCH_EVENT_ENDED then 
		if (UI_GetBaseWidgetByName("TaskSystem"))then
			return;
		end
		UI_CloseAllSpecial();
        local tab = GameServer_pb.CMD_TASK_QUERY_TASKINFO_CS()
        tab.type = GameServer_pb.enTaskType_MainLineAndBranchLine
        Packet_Full(GameServer_pb.CMD_TASK_QUERY_TASKINFO,tab)
        --Packet_Cmd(GameServer_pb.CMD_TASK_QUERY_TASKINFO)
        ShowWaiting()
        -- CreateHaveNewTask()
	end
end 

function ClickGold(sender,eventType)
	if (eventType == TOUCH_EVENT_ENDED) then
		CashCow_Create()
	end
end

function ClickBuyTili(sender, eventType)
	if (eventType == TOUCH_EVENT_ENDED) then
		Packet_Cmd(GameServer_pb.CMD_BUY_PHYSTRENGTH);
	end
end

g_iSaveMoney = 0;
function startSaveMoney(iMoney)
	g_iSaveMoney = iMoney;
	Packet_Cmd(GameServer_pb.CMD_QUERY_CANSAVEMONEY);
	ShowWaiting();
end


function QuerySaveMonet_SC(pkg)
	EndWaiting();

	local tmp = GameServer_pb.CMD_QUERY_CANSAVEMONEY_SC();
	tmp:ParseFromString(pkg)
	
	
	local zoneID = tmp.iZoneID;
	
	Log("QuerySaveMonet_SC---:"..zoneID);
	callRequestSaveMoney(zoneID, g_iSaveMoney);
end

function Clickyuanbao(sender, eventType)
    if(eventType == TOUCH_EVENT_ENDED)then
        --createPromptBoxlayout(FormatString("NotOpenYuanBao"));
        RechargeSystem_ShowLayout()
    end
end
function SceneMgr_OpenMainUi()
    Log("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa")
	g_lastNotifyTime = os.time();
	local widget = UI_CreateBaseWidgetByFileName("MainUi.json");
	local scrollView = UI_GetUIScrollView(widget, 1);
	local rightLayout = UI_GetUIImageView(widget:getChildByTag(110), 1);
    if rightLayout:getPositionY() == -256 then 
       UI_GetUIButton(widget, 111):setBright(false)
    elseif (rightLayout:getPositionY() == 265) then 
       UI_GetUIButton(widget, 111):setBright(true)
    end 
	--rightLayout:setPosition(ccp(45, -265))
	--UI_GetUIButton(widget, 111):setBright(false);
	local function MoveAction(sender, eventType)
		if(eventType == TOUCH_EVENT_ENDED)then
			if (rightLayout:getPositionY() == -265)then
				rightLayout:runAction(CCEaseBackOut:create(CCMoveTo:create(0.3, ccp(45, 265))))
				UI_GetUIButton(widget, 111):setBright(true);
			elseif (rightLayout:getPositionY() == 265)then
				rightLayout:runAction(CCEaseExponentialOut:create(CCMoveTo:create(0.3, ccp(45, -265))))
				UI_GetUIButton(widget, 111):setBright(false);
			end
            NoticeFunction_UpdateRightButton()
            NoticeFunction_UpdateHeroButton()
		end
	end
	
	UI_GetUIButton(widget, 111):addTouchEventListener(MoveAction);
	
    local function scrollViewClickFunction(sender,eventType)
       --if eventType == TOUCH_EVENT_BEGAN then 
          if UI_GetUIButton(widget:getChildByTag(1), 4):getChildByTag(1000)  then 
             UI_GetUIButton(widget:getChildByTag(1), 4):removeChildByTag(1000)
          end
          g_ZhuchenScollViewHasTouch = true 
      --end
    end
    scrollView:addTouchEventListener(scrollViewClickFunction)
    if GetLocalPlayer():GetInt(EPlayer_Lvl) <= g_ZhuChengFubenTishiLevelLimit then 
		SceneMgr_FubenTishiUpdate(widget)
    end
	--bgmusic
    if GetAudioMgr():isEnabled() == true then
    	CreateGameBgMusic(g_bgMusicFileTable["MainUI"])
    end

    local heroIcon = GetLocalPlayer():GetInt(EPlayer_HeadId)
    local friendImg = CCSprite:create("Icon/HeroIcon/x"..heroIcon..".png")
    Log("heroIcon======="..heroIcon)
    local imag = UIMgr:GetInstance():createMaskedSprite(friendImg,"zhujiemian/bg_4.png")
    if UI_GetUIImageView(widget:getChildByTag(2), 1001):getNodeByTag(2) then 
       UI_GetUIImageView(widget:getChildByTag(2), 1001):removeNodeByTag(2);
    end 
    imag:setPosition(ccp(-75,0))
	UI_GetUIImageView(widget:getChildByTag(2), 1001):addNode(imag, 0, 2);
	
	local heroListButton = UI_GetUIButton(rightLayout, 12);
	heroListButton:addTouchEventListener(ClickHeroList);
	
	--[[local function FightSoul(sender,eventType) 
		if (eventType == TOUCH_EVENT_ENDED) then
			Packet_Cmd(GameServer_pb.CMD_QUERY_FIGHTSOUL);
			ShowWaiting();
		end
	end 
	local fightSoulButton = UI_GetUIButton(widget:getChildByTag(1), 7);
	fightSoulButton:addTouchEventListener(FightSoul);
	if (UI_GetUIButton(widget:getChildByTag(1), 7):getNodeByTag(1) == nil) then
		local fsArmature = GetUIArmature("wuhun")
		fsArmature:getAnimation():playWithIndex(0)
		UI_GetUIButton(widget:getChildByTag(1), 7):addNode(fsArmature, 1, 1);
	end]]
	
	local formationButton = UI_GetUIButton(rightLayout, 11);
	formationButton:addTouchEventListener(ClickFormation);
	
	local function DungeonList(sender,eventType) 
        if eventType == TOUCH_EVENT_BEGAN then 
           local sender = tolua.cast(sender,"Button")
           UI_OpenButtonSoftLight(sender,1.2,1.2)
        end 
        if eventType == TOUCH_EVENT_CANCELED then 
           UI_RemoveButtonSoftLight()
        end 
		if (eventType == TOUCH_EVENT_ENDED) then
            UI_RemoveButtonSoftLight()
			Dungeon_ListCreate(true);
			--[[local tab = GameServer_pb.Cmd_Cs_QueryDSectionDetail();
			tab.iSectionID = g_selectSectionID;
			Packet_Full(GameServer_pb.CMD_QUERY_DSECTIONDETAIL, tab);
			ShowWaiting();]]
		end
	end 
	--local DungeonListButton = UI_GetUIButton(widget, 16);
	--DungeonListButton:addTouchEventListener(DungeonList);
	UI_GetUIButton(widget:getChildByTag(1), 4):addTouchEventListener(DungeonList);
	if (UI_GetUIButton(widget:getChildByTag(1), 4):getNodeByTag(1) == nil) then
		local armature = GetUIArmature("fuben")
		armature:getAnimation():playWithIndex(0)
		UI_GetUIButton(widget:getChildByTag(1), 4):addNode(armature, 1, 1);
	end

	--背包 add by huyanfei
	-- local  function onclickBagBtton(sender, eventType)
	-- 	if(eventType == TOUCH_EVENT_ENDED)then
	-- 		BagSysMainLayer_create();
	-- 	end	
	-- end
	
	-- local  BagButton = UI_GetUIButton(widget, 14);
	-- BagButton:addTouchEventListener(onclickBagBtton);

	-- 神兽 add by huyanfei 2014/3/31
    local function onClickAnimal(sender,eventType)
    	-- body
    	if eventType == TOUCH_EVENT_BEGAN then 
           local sender = tolua.cast(sender,"Button")
           UI_OpenButtonSoftLight(sender,1.2,1.2)
        end 
        if eventType == TOUCH_EVENT_CANCELED then 
           UI_RemoveButtonSoftLight()
        end 
		if(eventType == TOUCH_EVENT_ENDED)then
           UI_RemoveButtonSoftLight()
           createPromptBoxlayout(FormatString("MainUINotOpen"))
		end	
    end

    local  animalbutton = UI_GetUIButton(scrollView, 1001);
	animalbutton:addTouchEventListener(onClickAnimal);

	local  function onclickGodAnimalBtn(sender, eventType)
        if eventType == TOUCH_EVENT_BEGAN then 
           local sender = tolua.cast(sender,"Button")
           UI_OpenButtonSoftLight(sender,1.2,1.2)
        end 
        if eventType == TOUCH_EVENT_CANCELED then 
           UI_RemoveButtonSoftLight()
        end 
		if(eventType == TOUCH_EVENT_ENDED)then
           UI_RemoveButtonSoftLight()
           if HeroEquipIsOpen() == false then 
              createPromptBoxlayout(FormatString("EquipFuben_OpenLimit"))
              return 
           end 
           EquipFuben_InitLayout()
		end	
	end
	local  GodAnimalBtn = UI_GetUIButton(scrollView, 3);
	GodAnimalBtn:addTouchEventListener(onclickGodAnimalBtn);
	if (UI_GetUIButton(scrollView, 3):getNodeByTag(1) == nil) then
		local armature = GetUIArmature("zhuangbeifuben")
		armature:getAnimation():playWithIndex(0)
		UI_GetUIButton(scrollView, 3):addNode(armature, 1, 1);
	end
	
	local  function ReloadLua(sender, eventType)
		if(eventType == TOUCH_EVENT_ENDED)then
			CCTextureCache:sharedTextureCache():dumpCachedTextureInfo();
			Reload();
			--Log("Ready Request Money");
			--startSaveMoney(100);
		end	
	end
	local  ReloadLuaButton = UI_GetUIButton(widget, 15);
	ReloadLuaButton:setTouchEnabled(true);
	ReloadLuaButton:addTouchEventListener(ReloadLua);
    ReloadLuaButton:setEnabled(false)
	
	--------首冲-------------------------------------------
    local  function FirstPay(sender, eventType)
		if(eventType == TOUCH_EVENT_ENDED)then
			FirstPay_Create()
		end	
	end
	
	local  firstPay = UI_GetUIButton(widget:getChildByTag(1401), 1001);
	firstPay:setTouchEnabled(true);
	firstPay:addTouchEventListener(FirstPay);
    if GetLocalPlayer():GetBool(EPlayer_IsShowFirstPay) ~= true then
        firstPay:setVisible(false)
    end
    ------------------------------------------------------------
    --------签到-------------------------------------------
    local  function SignIn(sender, eventType)
		if(eventType == TOUCH_EVENT_ENDED)then
			SignIn_Create()
		end	
	end
	
	local  signIn = UI_GetUIButton(widget:getChildByTag(1401), 1002);
	signIn:setTouchEnabled(true);
	signIn:addTouchEventListener(SignIn);
    -----------------------------------------------------------
    --------活动-------------------------------------------
    local  function Activity(sender, eventType)
		if(eventType == TOUCH_EVENT_ENDED)then
			--Activity_Create()
            Packet_Cmd(GameServer_pb.CMD_QUERY_OPERATE_ACTIVE)
            ShowWaiting()
		end	
	end
	
	local  activity = UI_GetUIButton(widget:getChildByTag(1401), 1004);
	activity:setTouchEnabled(true);
	activity:addTouchEventListener(Activity);
    -----------------------------------------------------------
	--------充值-------------------------------------------
    local  function SignIn(sender, eventType)
		if(eventType == TOUCH_EVENT_ENDED)then
			RechargeSystem_ShowLayout()
		end	
	end
	
	local  signIn = UI_GetUIButton(widget:getChildByTag(1401), 1003);
	signIn:setTouchEnabled(true);
	signIn:addTouchEventListener(SignIn);
    -----------------------------------------------------------
    -------仙宠---------------------------------------------
    if (UI_GetUIButton(scrollView, 1001):getNodeByTag(1) == nil) then
		local armature = GetUIArmature("xianchong")
		armature:getAnimation():playWithIndex(0)
		UI_GetUIButton(scrollView, 1001):addNode(armature, 1, 1);
	end
    --------------------------------------------------
	local  function ClickTerritory(sender, eventType)
        if eventType == TOUCH_EVENT_BEGAN then 
           local sender = tolua.cast(sender,"Button")
           UI_OpenButtonSoftLight(sender,1.8,1.6)
        end 
        if eventType == TOUCH_EVENT_CANCELED then 
           UI_RemoveButtonSoftLight()
        end 
		if(eventType == TOUCH_EVENT_ENDED)then
            UI_RemoveButtonSoftLight()
			Packet_Cmd(GameServer_pb.CMD_MANOR_QUERY);
			ShowWaiting();
		end	
	end
	
	if (UI_GetUIButton(scrollView, 17):getNodeByTag(1) == nil) then
		local armature = GetUIArmature("lingdi")
		armature:getAnimation():playWithIndex(0)
		UI_GetUIButton(scrollView, 17):addNode(armature, 1, 1);
	end
	
	UI_GetUIButton(scrollView, 17):setPosition(ccp(207, 401))
	
	local move = CCMoveBy:create(3, ccp(0, -15)) 
	local seqaction = CCSequence:createWithTwoActions( move, move:reverse() )
	local reaction = CCRepeatForever:create(seqaction)
	
	UI_GetUIButton(scrollView, 17):runAction(reaction)

	UI_GetUIButton(scrollView, 17):addTouchEventListener(ClickTerritory);
	
    --军团 yjj 2014.4.10
    local function ClickLegion(sender,eventType)
   	 -- body
   	 Log("Legion")
        if eventType == TOUCH_EVENT_BEGAN then 
           local sender = tolua.cast(sender,"Button")
           UI_OpenButtonSoftLight(sender,1.7,1.2)
        end 
        if eventType == TOUCH_EVENT_CANCELED then 
           UI_RemoveButtonSoftLight()
        end 
        if eventType == TOUCH_EVENT_ENDED then
           --LegionListInfo()
           --Legionapplicationlist()
           --LegionDetailsBaseInfo()
           UI_RemoveButtonSoftLight()
           Packet_Cmd(GameServer_pb.CMD_CLICK_LEGION)
           ShowWaiting()
        end
--        if eventType == TOUCH_EVENT_ENDED then
--            createPromptBoxlayout(FormatString("MainUINotOpen"))
--        end
    end
	UI_GetUIButton(widget:getChildByTag(1), 30):addTouchEventListener(ClickLegion)
    UI_GetUIButton(widget:getChildByTag(1), 30):setTouchEnabled(true)
	if (UI_GetUIButton(scrollView, 30):getNodeByTag(1) == nil) then
		local armature = GetUIArmature("juntuan")
		armature:getAnimation():playWithIndex(0)
		UI_GetUIButton(scrollView, 30):addNode(armature, 1, 1);
	end
	
	--礼包系统 add yjj
	local function ClickGifts(sender,eventType)
		-- body
        if eventType == TOUCH_EVENT_BEGAN then 
           local sender = tolua.cast(sender,"Button")
           UI_OpenButtonSoftLight(sender,0.5,0.5)
        end 
        if eventType == TOUCH_EVENT_CANCELED then 
           UI_RemoveButtonSoftLight()
        end 
		if eventType == TOUCH_EVENT_ENDED then
           UI_RemoveButtonSoftLight()
			--GiftsInfoBase()
		    --Packet_Cmd(GameServer_pb.CMD_QUERY_GIFT)
      --      ShowWaiting()
      --跑马灯 test
            
           -- MarqueeFunction()
      --上浮提示框test

        -- createPromptBoxlayout("xxxxxxxxxxxxxxxxxxx")
        --邮箱测试
            Packet_Cmd(GameServer_pb.CMD_QUERY_MAILLIST)
            ShowWaiting()
--EquipFuben_InitLayout()
		end
	end
	UI_GetUIButton(scrollView, 24):addTouchEventListener(ClickGifts)
	if (UI_GetUIButton(scrollView, 24):getNodeByTag(1) == nil) then
		local armature = GetUIArmature("feigechuanshu")
		armature:getAnimation():playWithIndex(0)
		UI_GetUIButton(scrollView, 24):addNode(armature, 1, 1);
	end
	
	--UI_AddCircleButtonEffect(UI_GetUIButton(widget, 24));
	--世界boss add yjj
	local function ClickWorldBoss(sender,eventType)
		-- body
        if eventType == TOUCH_EVENT_BEGAN then 
           local sender = tolua.cast(sender,"Button")
           UI_OpenButtonSoftLight(sender,0.8,1.5)
        end 
        if eventType == TOUCH_EVENT_CANCELED then 
           UI_RemoveButtonSoftLight()
        end 
		if eventType == TOUCH_EVENT_ENDED then
           UI_RemoveButtonSoftLight()
			--WorldBossBaseInfo()
		    Packet_Cmd(GameServer_pb.CMD_QUERY_WORLDBOSS)
            ShowWaiting()			
		end
	end
    UI_GetUIButton(widget:getChildByTag(1), 9):addTouchEventListener(ClickWorldBoss)
	if (UI_GetUIButton(scrollView, 9):getNodeByTag(1) == nil) then
		local armature = GetUIArmature("shijieboss")
		armature:getAnimation():playWithIndex(0)
		UI_GetUIButton(scrollView, 9):addNode(armature, 1, 1);
	end
	
    --好友 add by yjj 改接口地方了
	UI_GetUIButton(rightLayout, 27):addTouchEventListener(ClickFriend)

	--轮回 (原英雄转换)
	--[[local function ClickHeroConver(sender,eventType)
		-- body
		if eventType == TOUCH_EVENT_ENDED then
          -- HeroConVertInfo(0)
           Packet_Cmd(GameServer_pb.CMD_HERO_COVERT_OPEN)
           ShowWaiting()
		end
	end]]
	UI_GetUIButton(rightLayout, 29):addTouchEventListener(ClickBayLayer)
	
	--系统
	--[[local function CkickSystemSetting(sender,eventType)
		-- body
		if eventType == TOUCH_EVENT_ENDED then
		   SystemSettings()
		end
	end
    UI_GetUIButton(widget,28):addTouchEventListener(CkickSystemSetting)]]
	--阵营战
	local function ClickCompBattle(sender,eventType)
        if eventType == TOUCH_EVENT_BEGAN then 
           local sender = tolua.cast(sender,"Button")
           UI_OpenButtonSoftLight(sender,1.1,1)
        end 
        if eventType == TOUCH_EVENT_CANCELED then 
           UI_RemoveButtonSoftLight()
        end
		if eventType == TOUCH_EVENT_ENDED then
            UI_RemoveButtonSoftLight()
            Packet_Cmd(GameServer_pb.CMD_CAMPBATTLE_OPEN)
            ShowWaiting()
         end
    end
	UI_GetUIButton(widget:getChildByTag(1), 8):addTouchEventListener(ClickCompBattle)
	if (UI_GetUIButton(scrollView, 8):getNodeByTag(1) == nil) then
		local armature = GetUIArmature("qunyingzhuan")
		armature:getAnimation():playWithIndex(0)
		UI_GetUIButton(scrollView, 8):addNode(armature, 1, 1);
	end
	
	--UI_GetUIButton(widget:getChildByTag(23), 2):addTouchEventListener(ClickBuyTili)
	--金矿
	--UI_GetUIButton(widget:getChildByTag(22), 2):addTouchEventListener(ClickGold)

	--[[local function ClickEquip(sender,eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			Equip_Create()
		end
	end
	UI_GetUIButton(widget, 34):addTouchEventListener(ClickEquip)]]
	
	local function ClickBabel(sender,eventType)
        if eventType == TOUCH_EVENT_BEGAN then 
           local sender = tolua.cast(sender,"Button")
           UI_OpenButtonSoftLight(sender,1.2,1.2)
        end 
        if eventType == TOUCH_EVENT_CANCELED then 
           UI_RemoveButtonSoftLight()
        end
        if eventType == TOUCH_EVENT_ENDED then
           UI_RemoveButtonSoftLight()
			Packet_Cmd(GameServer_pb.CMD_DREAMLAND_OPEN_REQSECTON);
			ShowWaiting()
        end
    end
	UI_GetUIButton(widget:getChildByTag(1), 5):addTouchEventListener(ClickBabel)
	if (UI_GetUIButton(scrollView, 5):getNodeByTag(1) == nil) then
		local armature = GetUIArmature("jiutian")
		armature:getAnimation():playWithIndex(0)
		UI_GetUIButton(scrollView, 5):addNode(armature, 1, 1);
	end
	UI_GetUIButton(scrollView, 5):setPosition(ccp(1147, 472))
	local move = CCMoveBy:create(3, ccp(0, -15)) 
	local seqaction = CCSequence:createWithTwoActions( move, move:reverse() )
	local reaction = CCRepeatForever:create(seqaction)
	
	UI_GetUIButton(scrollView, 5):runAction(reaction)
	
	--招募池
	local function ClickShop(sender,eventType)
        if eventType == TOUCH_EVENT_BEGAN then 
           local sender = tolua.cast(sender,"Button")
           UI_OpenButtonSoftLight(sender,1.2,1)
        end 
        if eventType == TOUCH_EVENT_CANCELED then 
           UI_RemoveButtonSoftLight()
        end 
        if eventType == TOUCH_EVENT_ENDED then
            UI_RemoveButtonSoftLight()

            Shop_Create()
        end
    end
	UI_GetUIButton(scrollView, 18):addTouchEventListener(ClickShop)
	if (UI_GetUIButton(scrollView, 18):getNodeByTag(1) == nil) then
		local armature = GetUIArmature("zhaomuchi")
		armature:getAnimation():playWithIndex(0)
		UI_GetUIButton(scrollView, 18):addNode(armature, 1, 1);
	end
    --商城
    local function ClickOneShop(sender,eventType)
        if eventType == TOUCH_EVENT_BEGAN then 
           local sender = tolua.cast(sender,"Button")
           UI_OpenButtonSoftLight(sender,1,1)
        end 
        if eventType == TOUCH_EVENT_CANCELED then 
           UI_RemoveButtonSoftLight()
        end 
        if eventType == TOUCH_EVENT_ENDED then
            UI_RemoveButtonSoftLight()

            createPromptBoxlayout(FormatString("MainUINotOpen"))
        end
    end
	UI_GetUIButton(scrollView, 1231):addTouchEventListener(ClickOneShop)
	if (UI_GetUIButton(scrollView, 1231):getNodeByTag(1) == nil) then
		local armature = GetUIArmature("shangcheng")
		armature:getAnimation():playWithIndex(0)
		UI_GetUIButton(scrollView, 1231):addNode(armature, 1, 1);
	end

	--竞技场
	local function ClickArena(sender,eventType)
        if eventType == TOUCH_EVENT_BEGAN then 
           local sender = tolua.cast(sender,"Button")
           UI_OpenButtonSoftLight(sender,1.2,1)
        end 
        if eventType == TOUCH_EVENT_CANCELED then 
           UI_RemoveButtonSoftLight()
        end 
        if eventType == TOUCH_EVENT_ENDED then
            UI_RemoveButtonSoftLight()
			Packet_Cmd(GameServer_pb.CMD_OPEN_ARENA);
			ShowWaiting();
        end
    end
	UI_GetUIButton(widget:getChildByTag(1), 6):addTouchEventListener(ClickArena)
	if (UI_GetUIButton(scrollView, 6):getNodeByTag(1) == nil) then
		local armature = GetUIArmature("leitai")
		armature:getAnimation():playWithIndex(0)
		UI_GetUIButton(scrollView, 6):addNode(armature, 1, 1);
	end
	
	if (scrollView:getNodeByTag(9999) == nil) then
		local armature = GetUIArmature("beijinEff")
		armature:setPosition(ccp(1400, 320))
		armature:getAnimation():playWithIndex(0)
		scrollView:addNode(armature, 1, 9999);
	end
	
	UI_GetUILayout(widget, 101):removeNodeByTag(10000)
	local feiniao_1 = GetUIArmature("feiniao")
	feiniao_1:getAnimation():playWithIndex(0)
	UI_GetUILayout(widget, 101):addNode(feiniao_1, 100, 10000);
	
	UI_GetUILayout(widget, 100):removeNodeByTag(10000)
	local feiniao_2 = GetUIArmature("feiniao")
	feiniao_2:getAnimation():playWithIndex(0)
	UI_GetUILayout(widget, 100):addNode(feiniao_2, 100, 10000);
	
	SceneMgr_Feiniao_1();
    SceneMgr_Feiniao_2();
	GetGlobalEntity():GetScheduler():RegisterInfiniteScript( "SceneMgr_Feiniao_1", 16*1000 )
	GetGlobalEntity():GetScheduler():RegisterInfiniteScript( "SceneMgr_Feiniao_2", 20*1000 )
	
	--[[scrollView:removeNodeByTag(10001)
	scrollView:removeNodeByTag(10002)
	scrollView:removeNodeByTag(10003)
	local shuibo01 = GetUIArmature("shuibo01")
	shuibo01:getAnimation():playWithIndex(0)
	shuibo01:setPosition(ccp(335 + 960, 320 -24))
	scrollView:addNode(shuibo01, 1, 10001);
	
	local shuibo02 = GetUIArmature("shuibo02")
	shuibo02:getAnimation():playWithIndex(0)
	shuibo02:setPosition(ccp(735 + 960, 320 - 120))
	scrollView:addNode(shuibo02, 1, 10002);
	
	local shuibo03 = GetUIArmature("shuibo03")
	shuibo03:getAnimation():playWithIndex(0)
	shuibo03:setPosition(ccp( 861 + 960, 320 - 81))
	scrollView:addNode(shuibo03, 1, 10003);
	]]
	MainUi_Refresh(widget);
	
	--if (IsDebug() == false)then
		--UI_GetUIButton(widget, 13):setVisible(false);
		--UI_GetUIButton(widget, 15):setEnabled(false);
	--end
	
	--[[local img = CCSprite:create("tongyong/btn_close_01.png");
	local clip = CCClippingNode:create(img);
	clip:setPosition(ccp(300, 500));
	clip:setAlphaThreshold(0.0005);
	widget:addNode(clip, 99);

	local image = CCSprite:create("tongyong/btn_tongyong_01.png");
	clip:addChild(image);]]
	
    --国王战
    local function KingWorldFighting(sender,eventType)
       --if eventType == TOUCH_EVENT_BEGAN then 
       --    local sender = tolua.cast(sender,"Button")
       --    UI_OpenButtonSoftLight(sender,1.2,1.2)
       -- end 
       -- if eventType == TOUCH_EVENT_CANCELED then 
       --    UI_RemoveButtonSoftLight()
       -- end 
       if eventType == TOUCH_EVENT_ENDED then 
          createPromptBoxlayout(FormatString("MainUINotOpen"))
       end 
    end 
    UI_GetUIButton(widget:getChildByTag(1), 11):addTouchEventListener(KingWorldFighting)
    UI_GetUIButton(widget:getChildByTag(1), 11):setTouchEnabled(true)
	if (UI_GetUIButton(scrollView, 11):getNodeByTag(1) == nil) then
		local armature = GetUIArmature("fengwangzhan")
		armature:getAnimation():playWithIndex(0)
		UI_GetUIButton(scrollView, 11):addNode(armature, 1, 1);
	end
	
	
	--点击经验球
	local function lookMyinfoView(sender,eventType)
		-- body
		if eventType == TOUCH_EVENT_ENDED then
			--MyInfoView()
			Packet_Cmd(GameServer_pb.CMD_QUERY_ACTORINFO)
			ShowWaiting()	          
		end
	end
	local function lookVipInfoView(sender,eventType)
		-- body
		if eventType == TOUCH_EVENT_ENDED then
			--VIP信息
			VIPInfo()
		end
	end
    UI_GetUIImageView(widget:getChildByTag(2), 102):setTouchEnabled(true)
	UI_GetUIImageView(widget:getChildByTag(2), 102):addTouchEventListener(lookVipInfoView)
    UI_GetUIImageView(widget:getChildByTag(2),1001):setTouchEnabled(true)
	UI_GetUIImageView(widget:getChildByTag(2),1001):addTouchEventListener(lookMyinfoView)

    if (UI_GetUIButton(widget:getChildByTag(2), 1345):getNodeByTag(1) == nil) then
		local armature = GetUIArmature("Effvip")
		armature:getAnimation():playWithIndex(0)
		UI_GetUIButton(widget:getChildByTag(2), 1345):addNode(armature, 1, 1);
	end

	if g_AnnouceMentSwitch == 1 and GetLocalPlayer():GetInt(EPlayer_Lvl) >= g_AnnouceMentShowLvllimit then
	   g_AnnouceMentSwitch = 2
	   AnnouceMentInfo()
	end

	--任务
	UI_GetUIButton(rightLayout, 32):addTouchEventListener(ClickTaskFunc)
    --任务2
    UI_GetUIButton(widget,260):addTouchEventListener(ClickTaskTwoFunc)
--成功打住在loading界面和战斗界面之后的显示
	--GetNoticeFlag()
    --若引导
    NoticeFunction_UpdateRightButton()
    
    NoticeFunction_UpdateHeroButton()
    NoticeFunction_UpdateMainUIButton()
	if g_AccomplishTaskCanShow == true then
        createAccomplishTask()
	end

	if g_havenewtaskIsShow == true then
		CreateHaveNewTask()
	end
	
	--有这些引导在的时候，不能滚动
	if g_isGuide == false or (g_curGuideId ~= 1 and g_curGuideId ~= 2 and g_curGuideId ~= 3 and g_curGuideId ~= 9 and g_curGuideId ~= 10 and g_curGuideId ~= 14 and g_curGuideId ~= 16 and g_curGuideId ~= 17)  then
		scrollView:jumpToPercentHorizontal(40);
	end
		--开启界面
	SceneMgr_OpenUi();
	
	SceneMgr_Guide();

    
    Chat_CreateSimpleChat()
end

function SceneMgr_Guide()
	if (g_curGuideStep ~= 1)then
		return;
	end
	
	if g_curGuideId == 15 then
		return;
	end
	
	
	local widget = UI_GetBaseWidgetByName("MainUi");
	if (widget)then
		local scrollView = UI_GetUIScrollView(widget, 1);
		local rightLayout = UI_GetUIImageView(widget:getChildByTag(110), 1);

		--if (g_curGuideStep == 1)then
			g_openUIType = EUIOpenType_None	
			rightLayout:setPosition(ccp(45, 265))
			Log("SceneMgr_Guide CloseAllWidget-------------------------------------------------------");
			UI_CloseAllBaseWidget();
		
			rightLayout:setPosition(ccp(45, 265))
		
			if (g_curGuideId == 1)then
				scrollView:jumpToPercentHorizontal(35);
			elseif (g_curGuideId == 2)then
				scrollView:jumpToPercentHorizontal(50);
			elseif (g_curGuideId == 3)then
				scrollView:jumpToPercentHorizontal(35);
			elseif (g_curGuideId == 9 or g_curGuideId == 10 or g_curGuideId == 14)then
				scrollView:jumpToPercentHorizontal(0);
			elseif g_curGuideId == 16 then
				scrollView:jumpToPercentHorizontal(10);
			elseif g_curGuideId == 17 then
				scrollView:jumpToPercentHorizontal(35);
			end
		--end
	end
end

function SceneMgr_Feiniao_1()
	local widget = UI_GetBaseWidgetByName("MainUi");
	if (widget)then
		local feiniao_1 = UI_GetUILayout(widget, 101):getNodeByTag(10000);
		feiniao_1:stopAllActions();
		feiniao_1:setPosition(ccp(2100, 440));
	
		local moveAction_1 = CCJumpTo:create(7, ccp(-150,440), 30, 2);
		feiniao_1:runAction(moveAction_1);
		
        local actionArry = CCArray:create()
        actionArry:addObject(CCScaleTo:create(7,0.4))
        actionArry:addObject(CCScaleTo:create(0.01,1))

        local scaleAction =  CCSequence:create(actionArry)
        feiniao_1:runAction(scaleAction);
	else
		GetGlobalEntity():GetScheduler():UnRegisterScript( "SendFastChallenge")
	end
end
function SceneMgr_Feiniao_2()
    local widget = UI_GetBaseWidgetByName("MainUi");
	if (widget)then
		local feiniao_2 = UI_GetUILayout(widget, 100):getNodeByTag(10000);
		feiniao_2:stopAllActions();
		feiniao_2:setPosition(ccp(-480, 30));
		feiniao_2:setScale(0.5);
		feiniao_2:setScaleX(-0.5);
		
		local moveAction_2 = CCJumpTo:create(7, ccp(480,50), 15, 2);		
		feiniao_2:runAction(moveAction_2);
	else
		GetGlobalEntity():GetScheduler():UnRegisterScript( "SendFastChallenge")
	end
end
--VIP图片路径
function MainUi_VIPPath(vip)
    if vip>=10 then
        return "VIP/frot_VIP_0"..vip..".png"
    else
        return "VIP/frot_VIP_00"..vip..".png"
    end
end

function MainUi_Refresh(widget)
	UI_GetUILabel(widget:getChildByTag(2), 5):setText(""..GetLocalPlayer():GetInt(EPlayer_Lvl));
	--UI_GetBMFontFightNumber(widget,6,GetLocalPlayer():GetInt(EPlayer_FightValue))
	--UI_GetUILabelBMFont(widget, 7):setText(""..GetLocalPlayer():GetInt(EPlayer_VipLevel));
    UI_GetUIImageView(widget:getChildByTag(2), 1345):loadTexture(MainUi_VIPPath(GetLocalPlayer():GetInt(EPlayer_VipLevel)))
	--UI_GetUILabel(widget:getChildByTag(21), 1):setText(UI_GetMoneyStr(GetLocalPlayer():GetInt(EPlayer_Gold)));
	--UI_GetUILabel(widget:getChildByTag(22), 1):setText(UI_GetMoneyStr(GetLocalPlayer():GetInt(EPlayer_Silver)));
	UI_GetUILabel(widget:getChildByTag(2), 20):setText(GetLocalPlayer():GetEntityName());
	--UI_GetUILabel(widget:getChildByTag(23), 1):setText(""..GetLocalPlayer():GetInt(EPlayer_Tili).."/"..GetLocalPlayer():GetInt(EPlayer_TiliMax));
	
	local expPercent = ToInt(GetLocalPlayer():GetInt(EPlayer_Exp)*100/GetLocalPlayer():GetInt(EPlayer_MaxExp));
	Log("expPercent"..expPercent)
	--UI_GetUILoadingBar(widget:getChildByTag(2), 1):setPercent(expPercent);
	
	--[[local tiliPercent = ToInt(GetLocalPlayer():GetInt(EPlayer_Tili)*100/GetLocalPlayer():GetInt(EPlayer_TiliMax));
	if (tiliPercent > 100)then
		tiliPercent = 100;
	end
	UI_GetUILoadingBar(widget, 3):setPercent(tiliPercent);]]
	--[[local rightLayout = UI_GetUIImageView(widget:getChildByTag(110), 1);
	UI_GetUIButton(rightLayout, 12):setButtonEnabled(UI_GetBaseWidgetByName("NewHeroList") == nil);
	UI_GetUIButton(rightLayout, 29):setButtonEnabled(UI_GetBaseWidgetByName("BagLayer") == nil);
	UI_GetUIButton(rightLayout, 11):setButtonEnabled(UI_GetBaseWidgetByName("Formation") == nil);
	UI_GetUIButton(rightLayout, 32):setButtonEnabled(UI_GetBaseWidgetByName("TaskSystem") == nil);
	UI_GetUIButton(rightLayout, 27):setButtonEnabled(UI_GetBaseWidgetByName("Friend") == nil);
	]]
	MainUiTop_Refresh();
end

function GM_Create()
	local widget = UI_CreateBaseWidgetByFileName("ChatTmp.json");
	if (widget == nil)then
		Log("GM_Create error")
		return;
	end
	
	local function SendGM(sender,eventType) 
		if (eventType == TOUCH_EVENT_ENDED) then
			local textField = UI_GetUITextField(widget, 1);
			local str = textField:getStringValue();
			local tab = GameServer_pb.Cmd_Cs_GmMsg();
			tab.strCmd = str;
			
			Packet_Full(GameServer_pb.CMD_GM_MSG, tab);
			UI_CloseCurBaseWidget();
			--UI_CloseCurBaseWidget(EUICloseAction_None)
		end
	end 
	local sendButton = UI_GetUIButton(widget, 2);
	sendButton:addTouchEventListener(SendGM);
end

function SceneMgr_OpenUi()
	
	-- 坑
	if (g_curGuideStep == 1 and g_curGuideId ~= 15)then
		g_openUIType = EUIOpenType_None	
	end
	
	Log("g_openUIType========="..g_openUIType)
	
	if (g_openUIType == EUIOpenType_Dugeon) then
		--Dungeon_ListCreate(false);
		local tab = GameServer_pb.Cmd_Cs_QueryDSectionDetail();
		tab.iSectionID = g_selectSectionID;
		Packet_Full(GameServer_pb.CMD_QUERY_DSECTIONDETAIL, tab);
		ShowWaiting();
	elseif (g_openUIType == EUIOpenType_Formation) then
		Formation_Create();
	elseif (g_openUIType == EUIOpenType_LegionBoss) then
        Packet_Cmd(GameServer_pb.CMD_QUERY_LEGIONBOSS)
        ShowWaiting()
	elseif (g_openUIType == EUIOpenType_Arena)then	
		Packet_Cmd(GameServer_pb.CMD_OPEN_ARENA);
		ShowWaiting();
    elseif (g_openUIType == UIOpenType_LegionFighting)then
        Packet_Cmd(GameServer_pb.CMD_QUERY_LEGIONBATTLE)
        ShowWaiting()
    elseif (g_openUIType == EUIOpenType_LastLegionFighting) then
        if returnlastLegionWarLookupcityId() ~= nil then
	    	local tab = GameServer_pb.CMD_QUERY_SINGLE_LASTBATTLE_CS()
	    	tab.iCityID = returnlastLegionWarLookupcityId()
	    	Packet_Full(GameServer_pb.CMD_QUERY_SINGLE_LASTBATTLE, tab)
	    end
	elseif (g_openUIType == EUIOpenType_CompBattle) then
		Packet_Cmd(GameServer_pb.CMD_CAMPBATTLE_OPEN)
        ShowWaiting()
	elseif (g_openUIType == EUIOpenType_CompBattleResult) then
		CompBattle_CreateResult();
	elseif (g_openUIType == EUIOpenType_WorldBoss) then
	    Packet_Cmd(GameServer_pb.CMD_QUERY_WORLDBOSS)
	    ShowWaiting()
	elseif (g_openUIType == EUIOpenType_Hero) then
        --[[local heroEntity = GetEntityById(GetLocalPlayer():GetHeroList()[0], "Hero")
		
		if (heroEntity:GetExtraBool("IsQueryEquip") == false) then
			local tab = GameServer_pb.CMD_QUERY_HEROEQUIP_CS();
			tab.dwHeroObjectID = heroEntity:GetUint(EHero_ID);
			Packet_Full(GameServer_pb.CMD_QUERY_HEROEQUIP, tab);
		end
	
		local tab = GameServer_pb.Cmd_Cs_QueryHeroLevelInfo()
		tab.dwHeroObjectID = heroEntity:GetUint(EHero_ID)
		Packet_Full(GameServer_pb.CMD_QUERY_HERO_LEVELINFO,tab)
		ShowWaiting()--]]
		HeroListSecond_CreateHeroListLayout()
	elseif (g_openUIType == EUIOpenType_GodAnim) then
		if( GetLocalPlayer():GetGodAnimalList():size()>0)then
			EnterToGoaAnimalScene(0);
		end
	elseif (g_openUIType == EUIOpenType_FightSoul) then
		Packet_Cmd(GameServer_pb.CMD_QUERY_FIGHTSOUL);
		ShowWaiting();
	elseif (g_openUIType == EUIOpenType_Babel) then
		Packet_Cmd(GameServer_pb.CMD_QUERY_CLIMBTOWERDETAIL);
	elseif (g_openUIType == EUIOpenType_NineSky) then
         UI_RemoveButtonSoftLight()
         print("------------------------------EUIOpenType_NineSky")
		local tab = GameServer_pb.CMD_DREAMLAND_QUERY_SCENEINFO_CS();
		tab.iSectionID = g_nineSkyID;
		Packet_Full(GameServer_pb.CMD_DREAMLAND_QUERY_SCENEINFO, tab);
		ShowWaiting();
    elseif (g_openUIType == EUIOpenType_EquipFuben) then 
		
		local tab = GameServer_pb.Cmd_Cs_QueryDSectionDetail();
		tab.iSectionID = g_EquipFuben_SelectIndex;
		Packet_Full(GameServer_pb.CMD_QUERY_DSECTIONDETAIL, tab);
		ShowWaiting();
		--EquipFuben_InitLayout()
	end
	
	g_openUIType = EUIOpenType_None
end

if (g_openUIType == nil) then
	g_openUIType = EUIOpenType_None
end

function ACTOR_LEVELUP(pkg)
	local tmp = GameServer_pb.Cmd_Sc_ActorLevelUp();
	tmp:ParseFromString(pkg)
	
	GetLocalPlayer():SetInt(EPlayer_Lvl, tmp.iLevel);
	GetLocalPlayer():SetInt(EPlayer_Exp, tmp.iExp);
	GetLocalPlayer():SetInt(EPlayer_MaxExp, tmp.iLevelUpExp);
	
	Log("info...====="..tostring(tmp))
	if tmp.iPreLevel ~= tmp.iLevel then
		g_lastGradeOpenFlag = GetLocalPlayer():GetInt(EPlayer_FunctionMask)
		Log("g_lastGradeOpenFlag"..g_lastGradeOpenFlag)
		InitPlayerlevelupInfo(tmp)
    end
	UI_RefreshBaseWidgetByName("MainUi");
    GetLocalPlayer():PlayerHeroFavoriteCheck()
   if getChannelID() == "LJ" then 
       SetExtraDataLevelUp()
    end 
end

function SceneMgr_CacheUI()
	for i=1, #g_cacheWidget do
		local widget = GUIReader:shareReader():widgetFromJsonFile(g_cacheWidget[i]);
		widget:retain();
		UIMgr:GetInstance():AddWidgetInToCacheList(g_cacheWidget[i], widget);
	end	
end

if (g_cacheWidget == nil)then
	--g_cacheWidget = {"MainUi.json", "HeroSystem.json", "Formation.json", "DungeonList.json", "Dungeon.json", "MyTerritory.json", "Territory.json"}
	g_cacheWidget = {"MainUi.json", "DungeonList.json", "Dungeon.json"}
end

SceneMgr_CacheUI();

ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_ACTOR_LEVELUP, "ACTOR_LEVELUP" );


--logo tip
function LogoTip(widget)
	-- body
	math.randomseed(os.clock() * 10000)
	local index = math.random(1,#g_logotipConfig)
	Log("index==="..index)
	UI_SetLabelText(widget,2,g_logotipConfig[index])
end

--添加一个计时器，主城界面幾秒秒做一个检测
function SceneMgr_FubenTishiUpdate(widget)
    --开启
    --[[if UI_GetUIButton(widget:getChildByTag(1), 4):getChildByTag(1000)  then 
         UI_GetUIButton(widget:getChildByTag(1), 4):removeChildByTag(1000)
    end
	--]]
	
    GetGlobalEntity():GetScheduler():RegisterInfiniteScript( "SceneMgr_Update", 100) 
end

local function recalcIdeArrowPos()
   if g_NoticeArrowClone == nil then
	 return;
   end
   
   local widget = UI_GetBaseWidgetByName("MainUi")
   local curLayer = UIMgr:GetInstance():GetUILayarByWidget(widget);
   if curLayer ~= nil then
		local btnWidget = curLayer:getWidgetByName("Button_fuben");
		local worldPos = btnWidget:getWorldPosition();
		g_NoticeArrowClone:setPosition(worldPos)
   end
end

local function setIdeNoticeVisible(bVisible)
	if g_NoticeGuideClone ~= nil then
		g_NoticeGuideClone:setVisible(bVisible)
	end
	
	if g_NoticeArrowClone ~= nil then
		g_NoticeArrowClone:setVisible(bVisible)
	end
end


function SceneMgr_Update()
   local widget = UI_GetBaseWidgetByName("MainUi")
   --关闭计时器的逻辑判断
   if GetLocalPlayer() == nil or GetLocalPlayer():GetInt(EPlayer_Lvl) >= g_ZhuChengFubenTishiLevelLimit or widget == nil  then 
      GetGlobalEntity():GetScheduler():UnRegisterScript("SceneMgr_Update")
      --GetGlobalEntity():GetScheduler():UnRegisterScript("CheckFubenCantishi")
	  setIdeNoticeVisible(false)
      --[[if widget and UI_GetUIButton(widget:getChildByTag(1), 4):getChildByTag(1000) then 
         UI_GetUIButton(widget:getChildByTag(1), 4):removeChildByTag(1000)
      end
	  --]]  
      return 
   end
   
   -- 有引导或者当前界面不是主界面，隐藏
   if g_isGuide or UIMgr:GetInstance():IsCurBaseWidget(widget) == false or g_ZhuchenScollViewHasTouch then
		g_lastNotifyTime = os.time();
		g_ZhuchenScollViewHasTouch = false;
		setIdeNoticeVisible(false)
		return;
   end
   
   if os.time() - g_lastNotifyTime > g_ZhuChengFubenTishiTime then
		g_lastNotifyTime = os.time();
		if g_NoticeGuideClone == nil or  g_NoticeGuideClone:isVisible() == false then
			CheckFubenCantishi();
		end
   end
   
   --recalcIdeArrowPos();
   
   --local fubenbt = UI_GetUIButton(widget:getChildByTag(1), 4)
   --[[if ( g_ZhuchenScollViewHasTouch == true)then
         g_ZhuchenScollViewHasTouch = false
         GetGlobalEntity():GetScheduler():UnRegisterScript("CheckFubenCantishi")
		 g_NoticeGuideClone:setVisible(false)
   else
        if (GetGlobalEntity():GetScheduler():HasScheduleScript("CheckFubenCantishi") == false)then
             GetGlobalEntity():GetScheduler():RegisterInfiniteScript("CheckFubenCantishi", g_ZhuChengFubenTishiTime *1000)
        end
   end
   --]]

end

local function createIdeArrow()
		if g_NoticeArrowClone ~= nil then
			return;
		end

		g_NoticeArrowClone = Widget:create();
	
		g_NoticeArrowClone:setVisible(true);
		
		local arrowNode = CCSprite:create("xinshouyindao/ico.png");
		arrowNode:setPosition(ccp(30, -30))
		g_NoticeArrowClone:addNode(arrowNode, 2, 3);
		arrowNode:setVisible(false)
		
		local arrowArmature = GetUIArmature("Effmaobi")
		arrowArmature:getAnimation():playWithIndex(0)
		g_NoticeArrowClone:addNode(arrowArmature, 1, 2);
		g_NoticeArrowClone:setScale(MainScene:GetInstance().m_minScale);
		g_NoticeArrowClone:setName("FFFFF");
		
		
		MainScene:GetInstance():addChild(g_NoticeArrowClone, EMSTag_Ide_Notice, EMSTag_Ide_Notice);
		
		return g_NoticeArrowClone;
end


function CheckFubenCantishi()
   
   local guideImg = nil;
   local guideInfo = nil;
   if g_NoticeGuideClone == nil then
		g_NoticeGuideClone = GUIReader:shareReader():widgetFromJsonFile("Guide.json");
		g_NoticeGuideClone:retain();
		
		MainScene:GetInstance():AddChild(g_NoticeGuideClone, EMSTag_Ide_Notice, EMSTag_Ide_Notice, false);
		
		guideImg = UI_GetUIImageView(g_NoticeGuideClone, 1);
		guideImg:setVisible(false);
		UI_GetUIImageView(g_NoticeGuideClone, 2):setVisible(false);
		UI_GetUIImageView(g_NoticeGuideClone, 3):setVisible(false);
		
		guideImg:removeChildByTag(10);
		
		guideInfo = CompLabel:GetDefaultCompLabel(g_ideNotify.text, 410);
		guideImg:addChild(guideInfo, 10, 10);
		guideImg:setVisible(true);
		
		guideInfo:setAnchorPoint(ccp(0.5, 0.5));
		
		local clickConWidget = UI_GetUIImageView(guideImg, 4532482);
		local clickArrowWidget = UI_GetUIImageView(guideImg, 4532483);
		clickConWidget:setVisible(false);
		clickArrowWidget:setVisible(true);
		
		guideImg:setPosition(ccp(g_ideNotify.pos.x, g_ideNotify.pos.y))
		if (g_ideNotify.left == false) then
			guideInfo:setPosition(ccp(0, -110) );
			guideInfo:setScaleX(-1);
			guideImg:setScaleX(-1);
			
			clickArrowWidget:setScaleX(-1);
			local x, y = clickArrowWidget:getPosition();
			clickArrowWidget:setPosition(ccp(-x, y) );
		else
			guideInfo:setPosition(ccp(0, -110) );
			guideImg:setScaleX(1);
		end
		
		
		
		-- 小手
		createIdeArrow();
   else
		guideImg = UI_GetUIImageView(g_NoticeGuideClone, 1);
		guideInfo = CompLabel:GetDefaultCompLabel(g_ideNotify.text, 410);
   end
   
   local widget = UI_GetBaseWidgetByName("MainUi");
   local scrollView = UI_GetUIScrollView(widget, 1);
   scrollView:jumpToPercentHorizontal(35);
   
   setIdeNoticeVisible(true)
   recalcIdeArrowPos()
   
   
   --Button_fuben
   
   --g_NoticeArrowClone:setPosition();
end


if (g_mainUiTopWidget == nil)then
	g_mainUiTopWidget = UI_CreateBaseWidgetByFileName("MainUiTop.json");
	g_mainUiTopWidget:retain();
	
	local function ClickMoveIn(sender, eventType)
		if(eventType == TOUCH_EVENT_ENDED)then
			MainUiRight_Create();
			if g_isGuide then
				Guide_GoNext()
			end
		end
	end
	local layout = g_mainUiTopWidget:getChildByTag(1)
	UI_GetUIButton(g_mainUiTopWidget, 10):addTouchEventListener(ClickMoveIn);
	UI_GetUIButton(layout:getChildByTag(1), 2):addTouchEventListener(ClickGold);
    UI_GetUIButton(layout:getChildByTag(2), 2):addTouchEventListener(Clickyuanbao);
	UI_GetUIButton(layout:getChildByTag(3), 2):addTouchEventListener(ClickBuyTili);


    local function ShowTiliHuifuFunction(sender,eventType)
       if eventType == TOUCH_EVENT_BEGAN then 
          UI_GetUILayout(g_mainUiTopWidget,2):setVisible(true)
          ShowTiliHuifuInit(g_mainUiTopWidget)
       end 
       if eventType == TOUCH_EVENT_CANCELED then 
          UI_GetUILayout(g_mainUiTopWidget,2):setVisible(false)
          StopTiliHuifuInit(g_mainUiTopWidget)
       end 
       if eventType == TOUCH_EVENT_ENDED then
          UI_GetUILayout(g_mainUiTopWidget,2):setVisible(false)
          StopTiliHuifuInit(g_mainUiTopWidget) 
       end 
    end 
    UI_GetUIImageView(layout,3):setTouchEnabled(true)
    UI_GetUIImageView(layout,3):addTouchEventListener(ShowTiliHuifuFunction)
    --ShowTiliHuifuInit(g_mainUiTopWidget)
end

function ShowTiliHuifuInit(widget)
   --当前系统时间
   local layout = UI_GetUILayout(widget,2)
   local systemTime = GetLocalPlayer():getCurrentTime()
   UI_GetUILabel(layout,1):setOtherTimer(systemTime,TimeFormatType_HourMinSec)

   -- 可购买次数
   local havebuynumber = GetLocalPlayer():GetUint(EPlayer_LeftPhyStrength)
   local viplevel = GetLocalPlayer():GetUint(EPlayer_VipLevel)
   Log("viplevel===="..viplevel)
   local vipdata = GetGameData(DataFileVip, viplevel, "stVipData")
   --havebuynumber = vipdata.m_BuyTiLiNum - havebuynumber
      Log("havebuynumber===="..vipdata.m_BuyTiLiNum)
   UI_GetUILabel(layout,2):setText(havebuynumber.."/"..vipdata.m_BuyTiLiNum)

   --下点体力恢复时间
   local function TimeEnd(sender,eventType)
      ShowTiliHuifuInit(g_mainUiTopWidget)
   end 
   if GetLocalPlayer():GetInt(EPlayer_Tili) >= GetLocalPlayer():GetInt(EPlayer_TiliMax) then 
      UI_GetUILabel(layout,3):setText(FormatString("MyInfoView_TitiMan"))
      UI_GetUILabel(layout,4):setText(FormatString("MyInfoView_TitiMan"))
   else 
      local min = GetLocalPlayer():getCurrentMinTime()
	  local minone = math.floor(min / 60 / 5)
	  local mintwo = (min / 60) % 5 
	  Log("minone======"..minone.."    ".."mintwo========"..mintwo)
      -- if mintwo > 5 then 
         -- UI_GetUILabel(layout,3):setTimer(60 * 60 - min,TimeFormatType_HourMinSec)
      -- else 
         -- UI_GetUILabel(layout,3):setTimer(30 * 60 - min,TimeFormatType_HourMinSec)
      -- end 
	  UI_GetUILabel(layout,3):setTimer((5 - mintwo) * 60,TimeFormatType_HourMinSec)
      UI_GetUILabel(layout,3):addEventListenerTimer(TimeEnd)
      local needTili = GetLocalPlayer():GetInt(EPlayer_TiliMax) - GetLocalPlayer():GetInt(EPlayer_Tili)
      local needsec = 0
      -- if min > 30 * 60 then
         -- needsec = needTili * 30 * 60 - min
      -- else 
         -- needsec = needTili * 30 * 60 - min
      -- end  
      needsec = needTili * 5 * 60 - mintwo
      UI_GetUILabel(layout,4):setTimer(needsec,TimeFormatType_HourMinSec)
      UI_GetUILabel(layout,4):addEventListenerTimer(TimeEnd)
   end 
end 

function StopTiliHuifuInit(widget)
   local layout = UI_GetUILayout(widget,2)
   UI_GetUILabel(layout,1):stopOtherTimer()
   UI_GetUILabel(layout,3):stopTimer()
   UI_GetUILabel(layout,4):stopTimer()
end 
function MainUiTop_Refresh()
    --九天幻境不显示右边的动作框
    if UI_GetBaseWidgetByName("NineSky") or UI_GetBaseWidgetByName("NineSkyMap.json") then
        UI_GetUIButton(g_mainUiTopWidget, 10):setVisible(false)
    end

	local layout = g_mainUiTopWidget:getChildByTag(1)
	UI_GetUILabelBMFont(layout:getChildByTag(2), 1):setText(UI_GetMoneyStr(GetLocalPlayer():GetInt(EPlayer_Gold)));
	UI_GetUILabelBMFont(layout:getChildByTag(1), 1):setText(UI_GetMoneyStr(GetLocalPlayer():GetInt(EPlayer_Silver)));
	UI_GetUILabelBMFont(layout:getChildByTag(3), 1):setText(""..GetLocalPlayer():GetInt(EPlayer_Tili).."/"..GetLocalPlayer():GetInt(EPlayer_TiliMax));
end

function MainUiRight_Create()
	UI_GetUIButton(g_mainUiTopWidget, 10):setVisible(false)
	local widget = UI_CreateBaseWidgetByFileName("MainRight.json");
	local rightLayout = UI_GetUIImageView(widget:getChildByTag(1), 1);
	rightLayout:setPosition(ccp(45, -265))
	rightLayout:runAction(CCEaseBackOut:create(CCMoveTo:create(0.3, ccp(45, 265))))
	local function CloseCurWidget()
		UI_CloseCurBaseWidget(EUICloseAction_None)
	end
	local function ClickClose(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			local actionArry = CCArray:create()
			actionArry:addObject(CCDelayTime:create(0.3))
			actionArry:addObject(CCCallFunc:create(CloseCurWidget))
			rightLayout:runAction(CCSequence:create(actionArry));
			rightLayout:runAction(CCEaseExponentialOut:create(CCMoveTo:create(0.35, ccp(45, -265))))
			UI_GetUIButton(widget, 2):setBright(true);
		end
	end
	
	if (UI_GetBaseWidgetByName("NewHeroList"))then
		
	end
	UI_GetUIButton(rightLayout, 1):setButtonEnabled(UI_GetBaseWidgetByName("NewHeroList") == nil);
	UI_GetUIButton(rightLayout, 2):setButtonEnabled(UI_GetBaseWidgetByName("BagLayer") == nil);
	UI_GetUIButton(rightLayout, 3):setButtonEnabled(UI_GetBaseWidgetByName("Formation") == nil);
	UI_GetUIButton(rightLayout, 4):setButtonEnabled(UI_GetBaseWidgetByName("TaskSystem") == nil);
	UI_GetUIButton(rightLayout, 5):setButtonEnabled(UI_GetBaseWidgetByName("Friend") == nil);
	
	UI_GetUILayout(widget, 3):addTouchEventListener(ClickClose);
	UI_GetUIButton(widget, 2):addTouchEventListener(ClickClose);
	UI_GetUIButton(widget, 2):setBright(false); 
	NoticeFunction_UpdateRightButton()
    NoticeFunction_UpdateHeroButton()
	UI_GetUIButton(rightLayout, 1):addTouchEventListener(ClickHeroList);
	UI_GetUIButton(rightLayout, 2):addTouchEventListener(ClickBayLayer);
	UI_GetUIButton(rightLayout, 3):addTouchEventListener(ClickFormation);
	UI_GetUIButton(rightLayout, 4):addTouchEventListener(ClickTaskFunc);
	UI_GetUIButton(rightLayout, 5):addTouchEventListener(ClickFriend);
end

if (g_needMainUiTopWidget == nil)then
	g_needMainUiTopWidget = {"MainUi", "DungeonList", "Dungeon", "Formation", "HeroSystemSecond", "HeroListSecond", "BagLayer",
	"Chat", "Friend", "TaskSystem", "GodAnimalSystem", "NineSky", "NineSkyMap", "Arena", "CompBattle","EquipFuben"}
end


function UI_OpenButtonSoftLight(sender,scaleX,scaleY)
   local widget = UI_GetBaseWidgetByName("MainUi")
   if widget then 
       local layout = UI_GetUIScrollView(widget, 1)
       local softImage = CCSprite:create("Common/main_button_press.jpg")
       local belen = ccBlendFunc:new()
       belen.src = GL_SRC_ALPHA
       belen.dst = GL_ONE
       softImage:setBlendFunc(belen)
       if layout:getNodeByTag(1000) then 
          layout:removeNodeByTag(1000)
       end 
       layout:addNode(softImage,1,1000)
       softImage:setPosition(sender:getPosition())        
       softImage:setScaleX(scaleX)
       softImage:setScaleY(scaleY)
   end 
end 

function UI_RemoveButtonSoftLight()
   local widget = UI_GetBaseWidgetByName("MainUi")
   if widget then
      local layout = UI_GetUIScrollView(widget, 1)
      if layout:getNodeByTag(1000) then 
          layout:removeNodeByTag(1000)
      end  
   end 
end 



ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_QUERY_CANSAVEMONEY, "QuerySaveMonet_SC" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_GODANIMAL_EMPTY, "OpenPlayerGodAnimal_SC" )

function OpenPlayerGodAnimal_SC(pkg)
   EndWaiting()
   GodAnimalSystem_create()
end 