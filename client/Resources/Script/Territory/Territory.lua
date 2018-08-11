--玩家自己的领地
function MyTerritory_Create()
	local widget = UI_CreateBaseWidgetByFileName("MyTerritory.json");
	if (widget == nil)then
		Log("MyTerritory_Create error");
		return;
	end
    NoticeFunction_UpdateTerritoryButton()
    -- 铜钱tip 修为tip 弱引导
    if UI_GetUIButton(widget,94):getChildByTag(1000) then 
       UI_GetUIButton(widget,94):removeChildByTag(1000)
    end 
    if UI_GetUIButton(widget,93):getChildByTag(1000) then
       UI_GetUIButton(widget,93):removeChildByTag(1000) 
    end 
    if g_SliverTip_ClickShouhuo then 
       Log("arrowwidth")
       local arrow = UI_GetArrow()
       UI_GetUIButton(widget,94):addChild(arrow,1000,1000)
       local arrowwidth = arrow:getSize().width 
       arrow:setPosition(ccp(-arrowwidth,0))
    end 
    if g_SliverTip_ClickNueduo then 
       local arrow = UI_GetArrow()
       UI_GetUIButton(widget,93):addChild(arrow,1000,1000)
       local arrowwidth = arrow:getSize().width 
       arrow:setPosition(ccp(-arrowwidth,0))
    end 
	--bgmusic
	if GetAudioMgr():isEnabled() == true then
    	CreateGameBgMusic(g_bgMusicFileTable["Territory"])
    end

    --回到主界面
	local function ClickMainUi(sender,eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			--endTasklead()
			g_myTerritoryInfo = {};
			UI_CloseCurBaseWidget(EUICloseAction_None);
			SceneMgr:GetInstance():SwitchToMainUi();
            --关闭 铜钱tip 修为tip 弱引导
            --SliverTip_CloseTishi()
		end
	end
	UI_GetUIButton(widget, 95):addTouchEventListener(ClickMainUi);
	--领地日志
	local function ClickTerritoryLog(sender,eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			LogAndEnternal_createLog();
            
		end
	end
	UI_GetUIButton(widget, 90):addTouchEventListener(ClickTerritoryLog);
	local bg = UI_GetUIImageView(widget, 1);
	
	local tiejiangpuBtn = UI_GetUIButton(bg, 22);	
	if (tiejiangpuBtn:getNodeByTag(1) == nil)then
		local armature1 = GetUIArmature("tiejiangpu")
		armature1:getAnimation():playWithIndex(0)
		tiejiangpuBtn:addNode(armature1, -1, 1)
	end

	local wuhundianBtn = UI_GetUIButton(bg, 21);	
	if (wuhundianBtn:getNodeByTag(1) == nil)then
		local armature2 = GetUIArmature("wuhundian")
		armature2:getAnimation():playWithIndex(0)
		wuhundianBtn:addNode(armature2, -1, 1)
	end
	
	MyTerritory_Refresh(widget)
	
	if (bg:getNodeByTag(1) == nil)then
		local lingdiArmature = GetUIArmature("lingdiEff")
		lingdiArmature:getAnimation():playWithIndex(0)
		bg:addNode(lingdiArmature, -1, 1)
	end
	
	UIMgr:GetInstance():PlayOpenAction(widget, EUIOpenAction_FadeIn, 1);

    --vip
    local function ShowVip(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            VIPInfo()
        end
    end
    local vip = UI_GetUIImageView(widget:getChildByTag(1129), 4741029)
    vip:setTouchEnabled(true)
    vip:addTouchEventListener(ShowVip)

    --保护
    local function TerritoryProtect(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            TerritoryProtect_Create()
        end
    end
    UI_GetUIImageView(widget, 1254):setTouchEnabled(true)
    UI_GetUIImageView(widget, 1254):addTouchEventListener(TerritoryProtect)
    --描边
    UI_GetUILabel(widget:getChildByTag(4579492), 4579493):enableStroke()
    UI_GetUILabel(widget:getChildByTag(4735178), 111):enableStroke()
    UI_GetUILabel(widget:getChildByTag(4735196), 111):enableStroke()
    UI_GetUILabel(widget:getChildByTag(53), 1):enableStroke()
    UI_GetUILabel(widget:getChildByTag(4735178), 222):enableStroke()
    UI_GetUILabel(widget:getChildByTag(4735196), 222):enableStroke()
    UI_GetUILabel(widget:getChildByTag(94), 1):enableStroke()
    UI_GetUILabel(widget:getChildByTag(93), 4656211):setText(g_myTerritoryInfo.iSearchCost)
    UI_GetUILabel(widget:getChildByTag(93), 4656211):enableStroke()
    UI_GetUILabel(widget:getChildByTag(1254), 1253):enableStroke()
end

function Territory_ShowJingliHuifuInit(widget)
   --当前系统时间
   local layout = UI_GetUILayout(widget,220)
   local systemTime = GetLocalPlayer():getCurrentTime()
   UI_GetUILabel(layout,1):setOtherTimer(systemTime,TimeFormatType_HourMinSec)

   -- 可购买次数

   --下点体力恢复时间
   local function TimeEnd(sender,eventType)
      Territory_ShowJingliHuifuInit(widget)
   end 
   if GetLocalPlayer():GetInt(EPlayer_Vigor) >= g_HeroTerritoryVim then 
      UI_GetUILabel(layout,3):setText(FormatString("MyInfoView_JingliMan"))
      UI_GetUILabel(layout,4):setText(FormatString("MyInfoView_JingliMan"))
   else 
      local min = GetLocalPlayer():getCurrentMinTime()
      --if min > 30 * 60 then 
         UI_GetUILabel(layout,3):setTimer(60 * 60 - min,TimeFormatType_HourMinSec)
      --else 
         --UI_GetUILabel(layout,3):setTimer(30 * 60 - min,TimeFormatType_HourMinSec)
      --end 
      UI_GetUILabel(layout,3):addEventListenerTimer(TimeEnd)
      local needTili = g_HeroTerritoryVim - GetLocalPlayer():GetInt(EPlayer_Vigor)
      local needsec = 0
      --if min > 30 * 60 then
         --needsec = needTili * 30 * 60 - min
      --else 
         needsec = needTili * 60 * 60 - min
      --end  

      UI_GetUILabel(layout,4):setTimer(needsec,TimeFormatType_HourMinSec)
      UI_GetUILabel(layout,4):addEventListenerTimer(TimeEnd)
   end 
end 

function Territory_StopJingliHuifuInit(widget)
   local layout = UI_GetUILayout(widget,220)
   UI_GetUILabel(layout,1):stopOtherTimer()
   UI_GetUILabel(layout,3):stopTimer()
   UI_GetUILabel(layout,4):stopTimer()
end 

function MyTerritory_Refresh(widget)
	EndWaiting();
	if (widget == nil)then
		Log("MyTerritory_Refresh error");
		return;
	end
	local imageAndAtt = UI_GetUILayout(widget, 1129)
    
	UI_GetUILabelBMFont(imageAndAtt:getChildByTag(101), 1):setText(UI_GetMoneyStr(GetLocalPlayer():GetInt(EPlayer_Gold)));
	UI_GetUILabelBMFont(imageAndAtt:getChildByTag(100), 1):setText(UI_GetMoneyStr(GetLocalPlayer():GetInt(EPlayer_Silver)));
	UI_GetUILabelBMFont(imageAndAtt:getChildByTag(102), 1):setText(""..GetLocalPlayer():GetInt(EPlayer_Vigor).."/"..g_HeroTerritoryVim);
	UI_GetUIImageView(imageAndAtt:getChildByTag(100), 2):addTouchEventListener(ClickGold)
    UI_GetUIImageView(imageAndAtt:getChildByTag(101), 2):addTouchEventListener(Clickyuanbao)
    --vip
    UI_GetUIImageView(imageAndAtt,4741029):loadTexture(MainUi_VIPPath(GetLocalPlayer():GetInt(EPlayer_VipLevel)))
    --UI_GetUIImageView(widget,4741029):setScale(0.8)
    if (UI_GetUIButton(imageAndAtt, 4741029):getNodeByTag(1) == nil) then
		local armature = GetUIArmature("Effvip")
		armature:getAnimation():playWithIndex(0)
		UI_GetUIButton(imageAndAtt,4741029):addNode(armature, 1, 1);
	end
    --精力恢复展示
    local function ShowPlayerjingliHuifu(sender,eventType)
       if eventType == TOUCH_EVENT_BEGAN then 
          UI_GetUILayout(widget,220):setVisible(true)
          Territory_ShowJingliHuifuInit(widget)
       end 
       if eventType == TOUCH_EVENT_CANCELED then 
          UI_GetUILayout(widget,220):setVisible(false)
          Territory_StopJingliHuifuInit(widget)
       end 
       if eventType == TOUCH_EVENT_ENDED then
          UI_GetUILayout(widget,220):setVisible(false)
          Territory_StopJingliHuifuInit(widget) 
       end 
    end 
    UI_GetUILabel(imageAndAtt:getChildByTag(102), 1):setTouchEnabled(false)
    UI_GetUIImageView(imageAndAtt, 102):addTouchEventListener(ShowPlayerjingliHuifu)
	
	local bg = UI_GetUIImageView(widget, 1);
	
	bg:removeNodeByTag(1);
	local lingdiArmature = GetUIArmature("lingdiEff")
	lingdiArmature:getAnimation():playWithIndex(0)
	bg:addNode(lingdiArmature, -1, 1)
		
	local infoImage = UI_GetUIImageView(bg, 100)
	
	-- 有引导，不能隐藏
	if g_isGuide == false then
		infoImage:setVisible(false);
	end
	
	local function ClickBg(sender, eventType)
		if(eventType == TOUCH_EVENT_ENDED)then
			infoImage:setVisible(false);
		end
	end
	bg:addTouchEventListener(ClickBg);
    local headImg = CCSprite:create("Icon/HeroIcon/x"..GetLocalPlayer():GetInt(EPlayer_HeadId)..".png")
	UI_GetUIImageView(imageAndAtt, 110):removeAllNodes();
    local headImage = UIMgr:GetInstance():createMaskedSprite(headImg,"zhujiemian/bg_4.png")
    headImage:setPosition(ccp(-75,0))
    UI_GetUIImageView(imageAndAtt, 110):addNode(headImage,0,2)
	
	UI_GetUILabel(imageAndAtt, 111):setText(GetLocalPlayer():GetEntityName());	
	UI_GetUILabelBMFont(widget, 113):setText(""..GetLocalPlayer():GetInt(EPlayer_FightValue));
	UI_GetUILabel(imageAndAtt, 112):setText(""..GetLocalPlayer():GetInt(EPlayer_Lvl));
	
	UI_GetUILabel(widget:getChildByTag(4735178), 111):setText(FormatString("Territory_Hour",g_myTerritoryInfo.iSilverProductSpeed));

	UI_GetUILabel(widget:getChildByTag(4735196), 111):setText(FormatString("Territory_Hour",g_myTerritoryInfo.iHeroExpProductSpeed));  

	UI_GetUILabel(widget:getChildByTag(53), 1):setText(""..(g_myTerritoryInfo.iTotalBuildTeams - g_myTerritoryInfo.iNowBulidTeams).."/"..g_myTerritoryInfo.iTotalBuildTeams);

	UI_GetUILabel(widget:getChildByTag(4735178), 222):setText(g_myTerritoryInfo.iCurSilverPutOut);

    UI_GetUILoadingBar(widget:getChildByTag(4735178), 333):setPercent(g_myTerritoryInfo.iCurSilverPutOut*100/g_myTerritoryInfo.iSilverTotalProduct)
	
    UI_GetUILabel(widget:getChildByTag(4735196), 222):setText(g_myTerritoryInfo.iCurHeroExpPutOut);

    UI_GetUILoadingBar(widget:getChildByTag(4735196), 333):setPercent(g_myTerritoryInfo.iCurHeroExpPutOut*100/g_myTerritoryInfo.iHeroExpTotalProduct)

	UI_GetUILabel(widget:getChildByTag(94), 1):setText(FormatString("Territory_Havest", g_myTerritoryInfo.iCurCanHavestTimes, g_myTerritoryInfo.iTotalHavestTimes));


	local function LeftTimeEnd(sender, eventType)
		Packet_Cmd(GameServer_pb.CMD_MANOR_QUERY);
	end
	
	local function SetResInfo(detail, pos)
		
		local idStr = ""..detail.iResId..detail.iResLevel
		local data = GetGameData(DataFileManorResLevel, TypeConvert:ToInt(idStr), "stManorResLevelData")
		Log("xxxxxxxxxxxxxx===="..data.m_cost)
		if data.m_cost == -1 then 
           UI_GetUIImageView(infoImage,11):setVisible(false)
           UI_GetUIImageView(infoImage,4739607):setVisible(false)
           UI_GetUIButton(infoImage,7):setVisible(false)
           UI_GetUILabel(infoImage,10):setVisible(true)
           UI_GetUILabel(infoImage,6):setVisible(false)
        else 
           UI_GetUIImageView(infoImage,11):setVisible(true)
           UI_GetUIImageView(infoImage,4739607):setVisible(true)
           UI_GetUIButton(infoImage,7):setVisible(true)
           UI_GetUILabel(infoImage,10):setVisible(false)
           UI_GetUILabel(infoImage,6):setVisible(true)
           if GetLocalPlayer():GetInt(EPlayer_Silver) < data.m_cost then 
              UI_GetUILabel(infoImage,6):setColor(ccc3(255,0,0))
           else 
              UI_GetUILabel(infoImage,6):setColor(ccc3(255,255,255))
           end 
        end 
        infoImage:setVisible(true);
		infoImage:setPosition(pos)
		UI_GetUILabel(infoImage, 1):setText(data.m_name.."Lv."..detail.iResLevel);
		UI_GetUILabel(infoImage, 2):setText(FormatString("Territory_Store", data.m_store));
		UI_GetUILabel(infoImage, 3):setText(FormatString("Territory_Speed", data.m_output));
		UI_GetUILabel(infoImage, 6):setText(data.m_cost);
		
		local idStr1 = ""..detail.iResId..(detail.iResLevel+1)
		local data1 = GetGameData(DataFileManorResLevel, TypeConvert:ToInt(idStr1), "stManorResLevelData")
        
		if (data1)then
			UI_GetUILabel(infoImage, 4):setText("+"..(data1.m_store - data.m_store));
			UI_GetUILabel(infoImage, 5):setText("+"..(data1.m_output - data.m_output));
			UI_GetUIButton(infoImage, 7):setVisible(true)
		else
			UI_GetUILabel(infoImage, 4):setVisible(false);
			UI_GetUILabel(infoImage, 5):setVisible(false);
			UI_GetUIButton(infoImage, 7):setVisible(false)
		end
		
		local function ClickLvlUp(sender, eventType)
			if (eventType == TOUCH_EVENT_ENDED)then
				local tab = GameServer_pb.CMD_MANOR_RES_COMMON_LEVELUP_CS()
				tab.iResId = detail.iResId
				tab.iResType = detail.iResType
				Packet_Full(GameServer_pb.CMD_MANOR_RES_COMMON_LEVELUP, tab)
				ShowWaiting();
				infoImage:setVisible(false)
				
				if (g_isGuide) then
					Guide_GoNext();
				end
			end
		end
		
		UI_GetUIButton(infoImage, 7):addTouchEventListener(ClickLvlUp);

        --添加项：升级时间
        local idStr2 = ""..detail.iResId..detail.iResLevel
		local data2 = GetGameData(DataFileManorResLevel, TypeConvert:ToInt(idStr2), "stManorResLevelData")
        UI_GetUILabel(infoImage, 1501):setText(LogAndEnternal_IntToTime(data2.m_timeLevelUp))
	end
	
	local function ClickRes(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED)then
			if (g_isGuide) then
				Guide_GoNext();
			end
			local sender = tolua.cast(sender, "Button");
			local tag = sender:getTag();
			local detail = g_myTerritoryInfo.szManorSilverResDetail[tag];
			if (detail.bResInBuild)then
				local tab = GameServer_pb.CMD_MANOR_RES_USE_GOLD_LEVELUP_CS()
				tab.iResId = detail.iResId
				tab.iResType = detail.iResType
				Packet_Full(GameServer_pb.CMD_MANOR_RES_USE_GOLD_LEVELUP, tab)
				ShowWaiting();
			else
				SetResInfo(detail, ccp(sender:getPositionX(), sender:getPositionY()))
			end
		end
	end
	
	local function ClickHeroRes(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED)then
			if (g_isGuide) then
				Guide_GoNext();
			end
			local sender = tolua.cast(sender, "Button");
			local tag = sender:getTag()-10;
			local detail = g_myTerritoryInfo.szManorHeroExpResDetail[tag];
			if (detail.bResInBuild)then
				local tab = GameServer_pb.CMD_MANOR_RES_USE_GOLD_LEVELUP_CS()
				tab.iResId = detail.iResId
				tab.iResType = detail.iResType
				Packet_Full(GameServer_pb.CMD_MANOR_RES_USE_GOLD_LEVELUP, tab)
				ShowWaiting();
			else
				SetResInfo(detail, ccp(sender:getPositionX(), sender:getPositionY()))
			end
		end
	end
	
	local function ArmatureEnd(armature, movementType, movementID)
		if (movementType == LOOP_COMPLETE)then
			if (movementID == "1")then
				armature:setVisible(false);
			end
		end
	end
	--初始化单个建筑的数据
	local function InitResBuild(detail, resBuild, resName)
		local armature = tolua.cast( resBuild:getNodeByTag(2), "CCArmature" )
		if (armature == nil)then
			armature = GetUIArmature("xiujian")
			resBuild:addNode(armature, 2, 2)
			armature:setVisible(false);
			armature:getAnimation():setMovementEventCallFunc(ArmatureEnd)
		end
	    --移除可升级的提示
        if resBuild:getChildByTag(11456) then
            resBuild:removeChildByTag(11456)
        end
        --添加可升级的提示
        local function AddLevelUpIcon(resBuild)
            local lu = ImageView:create()
            lu:loadTexture("Common/Icon_010.png")
            lu:setPosition(ccp(0,10))
            lu:setTag(11456)
            --向上移动的动作
            local actionArry = CCArray:create()
            local moveBy1 = CCMoveBy:create(1.0,ccp(0,20))
            local wait = CCDelayTime:create(0.3)
            local moveBy2 = CCMoveBy:create(0.01,ccp(0,-20))
            actionArry:addObject(moveBy1)
            actionArry:addObject(wait)
            actionArry:addObject(moveBy2)
            local seqaction = CCSequence:create(actionArry)
            local action_ = CCRepeatForever:create(seqaction)
            lu:runAction(action_)
            --消失出现的效果
            local actionArry2 = CCArray:create()
            local fideIn = CCFadeIn:create(0.1)
            local wait1 = CCDelayTime:create(0.4)
            local fideout = CCFadeOut:create(0.5)
            local wait2 = CCDelayTime:create(0.31)
            actionArry2:addObject(fideIn)
            actionArry2:addObject(wait1)
            actionArry2:addObject(fideout)
            actionArry2:addObject(wait2)
            local seqaction2 = CCSequence:create(actionArry2)
            local action_2 = CCRepeatForever:create(seqaction2)
            lu:runAction(action_2)
            resBuild:addChild(lu,100)
        end
		UI_GetUILabel(resBuild, 1):setVisible(detail.bResInBuild)
        UI_GetUILabel(resBuild, 11):setVisible(detail.bResInBuild)
		if (detail.bResInBuild)then
			UI_GetUILabel(resBuild, 1):setTimer(detail.iRemainSecond, TimeFormatType_HourMinSec)
			UI_GetUILabel(resBuild, 1):addEventListenerTimer(LeftTimeEnd);
			armature:setVisible(true)
			armature:getAnimation():playWithIndex(0)
		else
			if (armature:isVisible())then
				armature:getAnimation():playWithIndex(1);
			end
		end
		UI_GetUILabel(resBuild, 2):setText(""..detail.iResLevel);
		
		if (resName ~= nil)then
			local armature1 = tolua.cast( resBuild:getNodeByTag(1), "CCArmature" )
			if (armature1 == nil)then
				armature1 = GetUIArmature(resName)
				resBuild:addNode(armature1, -1, 1)
			end
			local idStr = ""..detail.iResId..detail.iResLevel
			local data = GetGameData(DataFileManorResLevel, TypeConvert:ToInt(idStr), "stManorResLevelData")
			armature1:getAnimation():playWithIndex(data.m_resID)
            if   GetLocalPlayer():GetInt(EPlayer_Lvl)/3 - detail.iResLevel >=1 and detail.bResInBuild == false then
                AddLevelUpIcon(resBuild)
            end
		end
	end
	
	for i=1, 4 do
		local resBuild = UI_GetUIButton(bg, i);
		if (i <= #g_myTerritoryInfo.szManorSilverResDetail) then
			resBuild:setVisible(true);
			resBuild:addTouchEventListener(ClickRes);
			
			InitResBuild(g_myTerritoryInfo.szManorSilverResDetail[i], resBuild, "jinkuang")
		else
			resBuild:setVisible(false);
		end
	end
	for i=1, 4 do
		local resBuild = UI_GetUIButton(bg, i+10);
		if (i <= #g_myTerritoryInfo.szManorHeroExpResDetail) then
			resBuild:setVisible(true);
			resBuild:addTouchEventListener(ClickHeroRes);
			InitResBuild(g_myTerritoryInfo.szManorHeroExpResDetail[i], resBuild, "xiuweikuang")
		else
			resBuild:setVisible(false);
		end
	end
	
	local function ClickHarvest(sender, eventType)
		if(eventType == TOUCH_EVENT_ENDED)then
			Packet_Cmd(GameServer_pb.CMD_MANOR_RES_HARVEST);
			ShowWaiting();
			
			if g_isGuide then
				Guide_GoNext()
			end
			
		end
	end
	UI_GetUIButton(widget, 94):addTouchEventListener(ClickHarvest);
	
	local function ClickBlacksmith(sender, eventType)
		if(eventType == TOUCH_EVENT_ENDED)then
			LogAndEnternal_createBlacksmith();
		end
	end
	
	local tiejiangpuBtn = UI_GetUIButton(bg, 22);	
	tiejiangpuBtn:addTouchEventListener(ClickBlacksmith);
	
	InitResBuild(g_myTerritoryInfo.TieJiangPuDetail, tiejiangpuBtn)
	UI_GetUIImageView(tiejiangpuBtn, 3):setVisible(g_myTerritoryInfo.TieJiangPuDetail.bCanHarvest)
	
	
	local function ClickEnternal(sender, eventType)
		if(eventType == TOUCH_EVENT_ENDED)then
			LogAndEnternal_createEnternal()
		end
	end
	
	local wuhundianBtn = UI_GetUIButton(bg, 21);	
	wuhundianBtn:addTouchEventListener(ClickEnternal);

	InitResBuild(g_myTerritoryInfo.WuHunDianDetail, wuhundianBtn)
	UI_GetUIImageView(wuhundianBtn, 3):setVisible(g_myTerritoryInfo.WuHunDianDetail.bCanHarvest)
	
	
	local function ClickLoot(sender, eventType)
		if(eventType == TOUCH_EVENT_ENDED)then
			Log("-----------------ClickSearchLoot2");
			if (g_isGuide) then
				Guide_GoNext();
			end
		
			Packet_Cmd(GameServer_pb.CMD_MANOR_REQUEST_LOOT)
			ShowWaiting();
			
			if g_isGuide then
				g_arrowLayout:setVisible(false);
				local guideImg = UI_GetUIImageView(g_guideWidget, 1);
				guideImg:setVisible(false)
			end
		end
	end

	UI_GetUIButton(widget, 93):addTouchEventListener(ClickLoot);

    --剩余保护时间
    UI_GetUILabel(widget:getChildByTag(1254), 1253):setTimer(g_myTerritoryInfo.iRemaindProtectSecond,TimeFormatType_HourMinSec)
end

function MANOR_QUERY(pkg)
	EndWaiting();
	local tmp = GameServer_pb.CMD_MANOR_QUERY_SC();
	tmp:ParseFromString(pkg)
	Log("*********"..tostring(tmp))
	g_myTerritoryInfo = tmp;	

	if (UI_GetBaseWidgetByName("MyTerritory")) then
		UI_RefreshBaseWidgetByName("MyTerritory")
	else
		SceneMgr:GetInstance():SwitchToMyTerritory();
	end
    
end

--他人的领地
function Territory_Create()
	local widget = UI_CreateBaseWidgetByFileName("Territory.json",EUIOpenAction_None);
	if (widget == nil)then
		Log("Territory_Create error");
		return;
	end

	--bgmusic
	if GetAudioMgr():isEnabled() == true then
    	CreateGameBgMusic(g_bgMusicFileTable["Territory"])
    end

    --回到主界面
	local function ClickMainUi(sender,eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			--endTasklead()
			g_territoryInfo = {};
			Packet_Cmd(GameServer_pb.CMD_MANOR_QUERY);
			ShowWaiting();
		end
	end
	UI_GetUIButton(widget, 95):addTouchEventListener(ClickMainUi);

        --阵容
    --local function othersA(sender,eventType)
    --    if eventType == TOUCH_EVENT_ENDED then
    --        TerritoryOhtersLineUp_Create(g_territoryInfo.szEnemyHeroFormation,g_territoryInfo.beLootActorInfo.strActorName)
    --    end
    --end
    --UI_GetUIButton(widget, 4603975):addTouchEventListener(othersA)
	
	local bg = UI_GetUIImageView(widget, 1);
	
	local tiejiangpuBtn = UI_GetUIButton(bg, 22);	
	if (tiejiangpuBtn:getNodeByTag(1) == nil)then
		local armature1 = GetUIArmature("tiejiangpu")
		armature1:getAnimation():playWithIndex(0)
		tiejiangpuBtn:addNode(armature1, -1, 1)

	end
	
	local wuhundianBtn = UI_GetUIButton(bg, 21);	
	if (wuhundianBtn:getNodeByTag(1) == nil)then
		local armature2 = GetUIArmature("wuhundian")
		armature2:getAnimation():playWithIndex(0)
		wuhundianBtn:addNode(armature2, -1, 1)
	end
	Territory_Refresh(widget)
	
	if (bg:getNodeByTag(1) == nil)then
		local armature = GetUIArmature("lingdiEff")
		armature:getAnimation():playWithIndex(0)
		bg:addNode(armature, -1, 1)
	end

        --提示别人的领地无法操控
    local function tips(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            createPromptBoxlayout(FormatString("OthersTerritory"))
        end
    end
    UI_GetUILayout(tiejiangpuBtn, 1301):addTouchEventListener(tips)
    UI_GetUILayout(wuhundianBtn, 1301):addTouchEventListener(tips)
	
	--UIMgr:GetInstance():PlayOpenAction(widget, EUIOpenAction_FadeIn, 1);
    --描边
      UI_GetUILabel(UI_GetUIImageView(widget,4579492),4579493):enableStroke()
      UI_GetUILabel(UI_GetUIImageView(widget,4593861),4593863):enableStroke()
      UI_GetUILabel(UI_GetUIImageView(widget,4593864),4593866):enableStroke()
      UI_GetUILabel(UI_GetUIImageView(widget,54),1):enableStroke()
      UI_GetUILabel(UI_GetUIImageView(widget,55),1):enableStroke()
      UI_GetUILabel(UI_GetUIImageView(widget,91),1):enableStroke()
end

function Cloud_ArmatureEnd(armature, movementType, movementID)
	if (movementType == LOOP_COMPLETE)then
        Log("xxxxxxxxxxxxxxxxxxxxxx")
		armature:removeFromParentAndCleanup(true)
        local widget = UI_GetBaseWidgetByName("Territory")
        if widget then 
           Territory_Refresh(widget)
           UI_GetUIButton(widget,91):setTouchEnabled(true)
        else 
           UI_GetUIButton(UI_GetBaseWidgetByName("MyTerritory"),93):setTouchEnabled(true)
           SceneMgr:GetInstance():SwitchToTerritory()
        end       
	end
end

function Territory_CloudEffect(widget)
   	local sousuoArmature = GetUIArmature("sousuo")
	sousuoArmature:getAnimation():playWithIndex(0)
	sousuoArmature:getAnimation():setMovementEventCallFunc(Cloud_ArmatureEnd)
	sousuoArmature:setPosition(ccp(480, 320))
	widget:addNode(sousuoArmature, 99999)
    if UI_GetBaseWidgetByName("Territory") then 
       UI_GetUIButton(widget,91):setTouchEnabled(false)
    else 
       UI_GetUIButton(widget,93):setTouchEnabled(false)
    end 
end 

function Territory_Refresh(widget)
	EndWaiting();
	if (widget == nil)then
		Log("Territory_Refresh error");
		return;
	end
    local headAndAtt = UI_GetUILayout(widget, 1129)
    --Territory_CloudEffect(widget)
--	local sousuoArmature = GetUIArmature("sousuo")
--	sousuoArmature:getAnimation():playWithIndex(0)
--	sousuoArmature:getAnimation():setMovementEventCallFunc(ArmatureEnd)
--	sousuoArmature:setPosition(ccp(480, 320))
--	widget:addNode(sousuoArmature, 99999)
	UI_GetUILabelBMFont(headAndAtt:getChildByTag(101), 1):setText(UI_GetMoneyStr(GetLocalPlayer():GetInt(EPlayer_Gold)));
	UI_GetUILabelBMFont(headAndAtt:getChildByTag(100), 1):setText(UI_GetMoneyStr(GetLocalPlayer():GetInt(EPlayer_Silver)));
	UI_GetUILabelBMFont(headAndAtt:getChildByTag(102), 1):setText(""..GetLocalPlayer():GetInt(EPlayer_Vigor).."/"..g_HeroTerritoryVim);
	UI_GetUIImageView(headAndAtt:getChildByTag(100), 2):addTouchEventListener(ClickGold)
    UI_GetUIImageView(headAndAtt:getChildByTag(101), 2):addTouchEventListener(Clickyuanbao)

     --精力恢复展示
    local function ShowPlayerjingliHuifu(sender,eventType)
       if eventType == TOUCH_EVENT_BEGAN then 
          UI_GetUILayout(widget,220):setVisible(true)
          Territory_ShowJingliHuifuInit(widget)
       end 
       if eventType == TOUCH_EVENT_CANCELED then 
          UI_GetUILayout(widget,220):setVisible(false)
          Territory_StopJingliHuifuInit(widget)
       end 
       if eventType == TOUCH_EVENT_ENDED then
          UI_GetUILayout(widget,220):setVisible(false)
          Territory_StopJingliHuifuInit(widget) 
       end 
    end 
    UI_GetUILabel(headAndAtt:getChildByTag(102), 1):setTouchEnabled(false)
    UI_GetUIImageView(headAndAtt, 102):addTouchEventListener(ShowPlayerjingliHuifu)
	
	local bg = UI_GetUIImageView(widget, 1);
	
	--local img = CCSprite:create("zhujiemian/bg_4.png");
	--local clip = CCClippingNode:create(img);
	--clip:setAlphaThreshold(0.0005);
	--UI_GetUIImageView(widget, 210):removeAllNodes();
	--UI_GetUIImageView(widget, 210):addNode(clip, 99);
	
	--local data = GetGameData(DataFileHeroBorn, g_territoryInfo.beLootActorInfo.iHeadID, "stHeroBornData")
	--local heroIcon = UI_GetHeroIcon(data.m_icon);
	--clip:addChild(heroIcon);
    local headImg = CCSprite:create("Icon/HeroIcon/x"..g_territoryInfo.beLootActorInfo.iHeadID..".png")
	UI_GetUIImageView(headAndAtt, 110):removeAllNodes();
	local headImage = UIMgr:GetInstance():createMaskedSprite(headImg,"zhujiemian/bg_4.png")
    headImage:setPosition(ccp(-75,0))
    UI_GetUIImageView(headAndAtt, 110):addNode(headImage,0,2)

	UI_GetUILabel(headAndAtt, 111):setText(g_territoryInfo.beLootActorInfo.strActorName);	
	UI_GetUILabelBMFont(widget, 113):setText(""..g_territoryInfo.beLootActorInfo.iFightValue);
	UI_GetUILabel(headAndAtt, 112):setText(""..g_territoryInfo.beLootActorInfo.iActorLevel);
	
	UI_GetUILabel(widget:getChildByTag(54), 1):setText(""..g_territoryInfo.iCanLootSilver);
	UI_GetUILabel(widget:getChildByTag(55), 1):setText(""..g_territoryInfo.iCanLootHeroExp);
	UI_GetUILabel(widget:getChildByTag(91), 1):setText(""..g_territoryInfo.iRefreshCost);

    local function tips(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            createPromptBoxlayout(FormatString("OthersTerritory"))
        end
    end

	for i=1, 4 do
		local resBuild = UI_GetUIButton(bg, i);
		if (i <= #g_territoryInfo.silverRes) then
			resBuild:setVisible(true);
			local detail = g_territoryInfo.silverRes[i];
			UI_GetUILabel(resBuild, 2):setText(""..detail.iResLevel);
			
			resBuild:removeAllNodes();
			local idStr = ""..detail.iResId..detail.iResLevel
			local data = GetGameData(DataFileManorResLevel, TypeConvert:ToInt(idStr), "stManorResLevelData")
			local armature = GetUIArmature("jinkuang")
			armature:getAnimation():playWithIndex(data.m_resID)
			resBuild:addNode(armature, -1)

            resBuild:addTouchEventListener(tips)
		else
			resBuild:setVisible(false);
		end
	end
	for i=1, 4 do
		local resBuild = UI_GetUIButton(bg, i+10);
		if (i <= #g_territoryInfo.heroExpRes) then
			resBuild:setVisible(true);
			
			local detail = g_territoryInfo.heroExpRes[i];
			UI_GetUILabel(resBuild, 2):setText(""..detail.iResLevel);
			
			resBuild:removeAllNodes();
			local idStr = ""..detail.iResId..detail.iResLevel
			local data = GetGameData(DataFileManorResLevel, TypeConvert:ToInt(idStr), "stManorResLevelData")
			local armature = GetUIArmature("xiuweikuang")
			armature:getAnimation():playWithIndex(data.m_resID)
			resBuild:addNode(armature, -1)

            resBuild:addTouchEventListener(tips)
		else
			resBuild:setVisible(false);
		end
	end
	
	
	local tiejiangpuBtn = UI_GetUIButton(bg, 22);	
	UI_GetUILabel(tiejiangpuBtn, 2):setText(""..g_territoryInfo.tieJiangyPu.iResLevel);

	local wuhundianBtn = UI_GetUIButton(bg, 21);	
	UI_GetUILabel(wuhundianBtn, 2):setText(""..g_territoryInfo.wuHunDian.iResLevel);
	
	local function ClickLoot(sender,eventType)
		if(eventType == TOUCH_EVENT_ENDED)then
            --SliverTip_CloseTishi()
            TerritoryOhtersLineUp_Create(g_territoryInfo)
		end
	end

	UI_GetUIButton(widget, 93):addTouchEventListener(ClickLoot);
	
	local function ClickSearchLoot(sender,eventType)
		if(eventType == TOUCH_EVENT_ENDED)then
			Log("-----------------ClickSearchLoot");
			if (g_isGuide) then
				Guide_GoNext();
			end
		
			Packet_Cmd(GameServer_pb.CMD_MANOR_REQUEST_LOOT)
			ShowWaiting();
		end
	end

	UI_GetUIButton(widget, 91):addTouchEventListener(ClickSearchLoot);
    
end

function MANOR_REQUEST_LOOT(pkg)
	EndWaiting();
	local tmp = GameServer_pb.CMD_MANOR_REQUEST_LOOT_SC();
	tmp:ParseFromString(pkg)
	Log("*********"..tostring(tmp))
	g_territoryInfo = tmp;
	
	if (UI_GetBaseWidgetByName("Territory")) then
		--UI_RefreshBaseWidgetByName("Territory")
        Territory_CloudEffect(UI_GetBaseWidgetByName("Territory"))
	else
        Territory_CloudEffect(UI_GetBaseWidgetByName("MyTerritory"))
		--SceneMgr:GetInstance():SwitchToTerritory();
	end
    
end

ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_MANOR_QUERY, "MANOR_QUERY" );
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_MANOR_REQUEST_LOOT, "MANOR_REQUEST_LOOT" );

