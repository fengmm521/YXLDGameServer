local l_MyInfoView = nil 

function MyInfoView()
	-- body
	local widget = UI_CreateBaseWidgetByFileName("MyInfoView.json")
    
    local layoutparent = UI_GetUILayout(widget,1)
	--return 
	UI_GetUIButton(widget,10011):addTouchEventListener(UI_ClickCloseCurBaseWidget)

    --name 
    UI_SetLabelText(layoutparent,2,GetLocalPlayer():GetEntityName())
    UI_GetUILabel(layoutparent,2):enableStroke()
    --lvl 
    UI_SetLabelText(layoutparent,3,GetLocalPlayer():GetInt(EPlayer_Lvl))
    UI_GetUILabel(layoutparent,3):enableStroke()
    --exp
    UI_SetLabelText(layoutparent,4,GetLocalPlayer():GetInt(EPlayer_Exp).."/"..GetLocalPlayer():GetInt(EPlayer_MaxExp))
    UI_GetUILabel(layoutparent,4):enableStroke()
    UI_GetUILoadingBar(layoutparent,40):setPercent(GetLocalPlayer():GetInt(EPlayer_Exp) / GetLocalPlayer():GetInt(EPlayer_MaxExp) *100)

    --战斗力
    UI_SetLabelText(layoutparent,41,GetLocalPlayer():GetInt(EPlayer_FightValue))
    UI_GetUILabel(layoutparent,41):enableStroke()
    --头像 
    print("headID="..GetLocalPlayer():GetInt(EPlayer_HeadId))
    local head = UI_GetUIImageView(layoutparent, 50)
    
    local headImg = CCSprite:create("Icon/HeroIcon/"..GetLocalPlayer():GetInt(EPlayer_HeadId)..".png")
    UI_GetUIImageView(layoutparent, 50):addNode(UIMgr:GetInstance():createMaskedSprite(headImg,"zhujiemian/bg_4.png"),0,2)

    --所属军团
    if l_MyInfoView.strLegionName == nil or l_MyInfoView.strLegionName == "" then
        UI_GetUILabel(layoutparent, 42):setText(FormatString("OtherPlayerInfo_NoLegion"))
    else
        UI_GetUILabel(layoutparent, 42):setText(l_MyInfoView.strLegionName)
    end
    --英雄等级上限
    UI_GetUILabel(layoutparent, 43):setText(GetLocalPlayer():GetInt(EPlayer_Lvl)*2)
    UI_GetUILabel(layoutparent,43):enableStroke()
 --   local heroIcon = GetLocalPlayer():GetInt(EPlayer_HeadId)
 --   local friendImg = CCSprite:create("Icon/HeroIcon/"..heroIcon..".png")
 --   local imag = UIMgr:GetInstance():createMaskedSprite(friendImg,"zhujiemian/bg_4.png")
	--UI_GetUIImageView(widget, 2):removeNodeByTag(2);
	--UI_GetUIImageView(widget, 2):addNode(imag, 0, 2);

    --yuanbao 
    --UI_SetLabelText(layoutparent,8,FormatString("MyInfoView_Gold",GetLocalPlayer():GetInt(EPlayer_Gold)))

    --tongbi 
    --UI_SetLabelText(layoutparent,9,FormatString("MyInfoView_Siliver",GetLocalPlayer():GetInt(EPlayer_Silver)))

    --xiuwei 
    --UI_SetLabelText(layoutparent,11,FormatString("MyInfoView_Xiuwei",GetLocalPlayer():GetInt(EPlayer_HeroExp)))

    --rongyu 
    --UI_SetLabelText(layoutparent,10,FormatString("MyInfoView_Honor",GetLocalPlayer():GetInt(EPlayer_Honor)))
    
    --refresh
    --MyInfoRefresh(widget)

--    --设置接口
--    local function MyInfoView_ShowSystem(sender,eventType)
--       if eventType == TOUCH_EVENT_ENDED then 
--          SystemSettings()
--       end 
--    end 
--    UI_GetUIButton(layoutparent, 4497959):addTouchEventListener(MyInfoView_ShowSystem)
    --更换头像
    local function ChangeHead(sender,eventType)
       if eventType == TOUCH_EVENT_ENDED then 
          MyInfoView_ChangeHead()
       end 
    end 
    UI_GetUIButton(layoutparent, 44):addTouchEventListener(ChangeHead)
    --
	local function ChangeAccountNumberFunction(sender,eventType)
		-- body
		if eventType == TOUCH_EVENT_ENDED then
            Reload()
            SceneMgr:GetInstance():ShowMainMenuSceneUi()
            --切换时清除所有聊天记录
            for i=1, 4 do
		        g_chatChannel[i] = {};
	        end
		end
	end
    UI_GetUIButton(layoutparent,45):addTouchEventListener(ChangeAccountNumberFunction)

    local function MuteFunction(sender,eventType)
		-- body
		if eventType == TOUCH_EVENT_ENDED then
			if GetAudioMgr():isEnabled() == false then
                UI_GetUIButton(widget, 4):setBright(true)
				GetAudioMgr():setEnabled(true)  
                --SimpleAudioEngine:sharedEngine():setEnableEffect(true) 
                SimpleAudioEngine:sharedEngine():setEffectsVolume(1.0)       
			else
                UI_GetUIButton(widget, 4):setBright(false)
				GetAudioMgr():setEnabled(false)
               -- SimpleAudioEngine:sharedEngine():setEnableEffect(false) 
                SimpleAudioEngine:sharedEngine():setEffectsVolume(0)
			end
		end
	end
    UI_GetUIButton(layoutparent,46):addTouchEventListener(MuteFunction)

    local function AnnouncementFunction(sender,eventType)
		-- body
		if eventType == TOUCH_EVENT_ENDED then
			UI_CloseCurBaseWidget(EUICloseAction_None)
           AnnouceMentInfo()
		end
	end
	UI_GetUIButton(layoutparent,47):addTouchEventListener(AnnouncementFunction)

    --发送
    local function send(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            local str = UI_GetUITextField(layoutparent, 500):getStringValue()

            if str ~= nil and str ~="" then
                UI_ClickCloseCurBaseWidgetWithNoAction(sender, eventType)

                local tab = GameServer_pb.CMD_SEND_COVERT_CODE_CS();
	            tab.strCovertCode = str
	            Packet_Full(GameServer_pb.CMD_SEND_COVERT_CODE, tab);
                ShowWaiting()
            end
        end
    end
    --确定
    UI_GetUIButton(layoutparent, 48):addTouchEventListener(send)
    --激活输入框
--    local function enter(sender,eventType) 
--        if eventType == TOUCH_EVENT_ENDED then
--            UI_GetUITextField(layoutparent, 49):attachWithIME()
--        end
--    end
--    UI_GetUIImageView(layoutparent, 1124838251):setTouchEnabled(true)
--    UI_GetUIImageView(layoutparent, 1124838251):addTouchEventListener(PublicCallBackWithNothingToDo)
--    UI_GetUIImageView(layoutparent, 1124838251):addTouchEventListener(enter)
 --初始化静音按钮的状态
    if GetAudioMgr():isEnabled() then 
        UI_GetUIButton(layoutparent, 46):setBright(true)
    else 
        UI_GetUIButton(layoutparent, 46):setBright(false)
    end
    --静音
	Mute(layoutparent)
end

--考虑到玩家体力更新
--function MyInfoRefresh(widget)
--	-- body
--	local layoutparent = UI_GetUILayout(widget,1)
--    --tili
--    UI_SetLabelText(layoutparent,5,GetLocalPlayer():GetInt(EPlayer_Tili).."/"..GetLocalPlayer():GetInt(EPlayer_TiliMax))
--    if GetLocalPlayer():GetInt(EPlayer_Tili) >= GetLocalPlayer():GetInt(EPlayer_TiliMax) then
--       UI_GetUILabel(layoutparent,6):setText(FormatString("MyInfoView_TitiMan"))
--       UI_GetUILabel(layoutparent,7):setText(FormatString("MyInfoView_TitiMan"))
--       return
--    end

--    --tilihuifu
--    local function KillCD(sender,eventType)
--    	-- body
--    	Packet_Cmd(GameServer_pb.CMD_QUERY_ACTORINFO)
--    	ShowWaiting()
--    end
--    UI_GetUILabel(layoutparent,6):setTimer(l_MyInfoView.iPhyResumeCD + 1,TimeFormatType_HourMinSec )
--    UI_GetUILabel(layoutparent,6):addEventListenerTimer(KillCD)
--    --tiliquanbu
--    UI_GetUILabel(layoutparent,7):setTimer(l_MyInfoView.iFullPhyResumeCD + 1,TimeFormatType_HourMinSec )
--    UI_GetUILabel(layoutparent,7):addEventListenerTimer(KillCD)
--end
function MyInfoSeverInfo(pkg)
	-- body
	EndWaiting()
	l_MyInfoView = GameServer_pb.CMD_QUERY_ACTORINFO_SC()
    l_MyInfoView:ParseFromString(pkg)
    Log("info===="..tostring(l_MyInfoView))
    local widget = UI_GetBaseWidgetByName("MyInfoView")
	if widget then
		Log("fresh")
        MyInfoRefresh(widget)
	else
		Log("create")
		MyInfoView()
	end
end

ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_QUERY_ACTORINFO, "MyInfoSeverInfo" )

--------------------------------------------ADD---------------------------------------------
--发送消息：设置头像
function MyInfoView_ChangeHeadMessage(headID)
    local tab = GameServer_pb.CMD_CHG_ACTOR_HEAD_CS()
    tab.enHeadType = 1
    tab.iHeadId = headID
    Packet_Full(GameServer_pb.CMD_CHG_ACTOR_HEAD,tab)

    ShowWaiting()
end
--更换头像
function MyInfoView_ChangeHead()
    local widget = UI_CreateBaseWidgetByFileName("ChangeHead.json")

    local listView = UI_GetUIListView(widget, 4532522)

    local temp = UI_GetUILayout(widget, 4532517):clone()
    temp:setVisible(true)
    listView:pushBackCustomItem(temp)

    local listViewSize = listView:getSize()

    --头像回调
    local function chickIcon(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            local headID = tolua.cast(sender,"Button"):getTag()-1300
            print("headID=="..headID)
            MyInfoView_ChangeHeadMessage(headID)
        end
    end

    local heroList = GetLocalPlayer():GetHeroList(false)
    local layout = nil
    for i=0,heroList:size()-1,1 do
        if i%5==0 then
            layout = Layout:create()
            layout:setSize(CCSizeMake(listViewSize.width,100));
            layout:setTouchEnabled(false)
            listView:pushBackCustomItem(layout)
            print("======newLayout=============")
        end
        print("=================="..heroList[i])
        local hero = GetEntityById(heroList[i], "Hero")
        --GetLocalPlayer():GetHeroByObjId(heroList[i])
        local heroID = hero:GetUint(EHero_HeroId)
        --local heroData = GetGameData(DataFileHeroBorn, heroID, "stHeroBornData")
        --local heroSoulData = GetGameData(DataFileItem, heroData.m_heroSoulItemId, "stItemData")
        local icon = UI_ItemIconFrame(UI_GetHeroIcon(heroID, 0), 3)
        icon:setTouchEnabled(true)
        icon:addTouchEventListener(chickIcon)
        icon:setTag(heroID + 1300)
        icon:setPosition(ccp(i%5*102+50,50))
        layout:addChild(icon)

        if GetLocalPlayer():GetInt(EPlayer_HeadId) == heroID then
            local use = ImageView:create()
            use:loadTexture("Main/mark_01.png")
            icon:addChild(use,10000)
            use:setPosition(ccp(-15,15))
        end
    end
end
--网络消息：修改头像
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_CHG_ACTOR_HEAD, "ChangeActorHead" )

function ChangeActorHead(pkg)
    EndWaiting()
    --信息界面头像
    UI_CloseCurBaseWidget()
    local widget = UI_GetBaseWidgetByName("MyInfoView")
    
    local layoutparent = UI_GetUILayout(widget,1)
    local head = UI_GetUIImageView(layoutparent, 50)
    local headImg = CCSprite:create("Icon/HeroIcon/"..GetLocalPlayer():GetInt(EPlayer_HeadId)..".png")
    --head:removeNodeByTag(2);
    head:removeAllNodes()
    local headIcon = UIMgr:GetInstance():createMaskedSprite(headImg,"zhujiemian/bg_4.png")
    head:addNode(headIcon,0,2)
    --主场景头像
    local widgetScene = UI_GetBaseWidgetByName("MainUi")
    local headImgScene = CCSprite:create("Icon/HeroIcon/x"..GetLocalPlayer():GetInt(EPlayer_HeadId)..".png")
    local headIconScene = UIMgr:GetInstance():createMaskedSprite(headImgScene,"zhujiemian/bg_4.png")
    if UI_GetUIImageView(widgetScene:getChildByTag(2), 1001):getNodeByTag(2) then 
       UI_GetUIImageView(widgetScene:getChildByTag(2), 1001):removeNodeByTag(2);
    end 
    headIconScene:setPosition(ccp(-75,0))
	UI_GetUIImageView(widgetScene:getChildByTag(2), 1001):addNode(headIconScene, 0, 2);
    --移除头像选择界面
    --UI_ClickCloseCurBaseWidget(UI_GetUIImageView(widget, 4535114),TOUCH_EVENT_ENDED)
end