function LegionMemberList_Create()
    Packet_Cmd(GameServer_pb.CMD_QUERY_LEGION_MEMBERLIST);
    ShowWaiting()
    --LegionMenberList_Init()
end
function LegionMenberList_Init(MemberList)

    local widget = UI_GetBaseWidgetByName("NewLegionNumberList")
    if widget == nil then
        widget = UI_CreateBaseWidgetByFileName("NewLegionNumberList.json")
    end
    local function exit(sender,eventType)
    if eventType == TOUCH_EVENT_ENDED then
            UI_ClickCloseCurBaseWidgetWithNoAction(sender,eventType)
            NewLegionBase_Refresh_CS() 
        end
    end
    UI_GetUILayout(widget, 12223):addTouchEventListener(PublicCallBackWithNothingToDo)
    UI_GetUIButton(widget, 1):addTouchEventListener(exit)
    local listView = UI_GetUIListView(widget, 3)
    listView:removeAllItems()
    --膜拜
    local function worship(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            local button = tolua.cast(sender,"Button")
            print("tag============================="..button:getTag())
            UI_ClickCloseCurBaseWidgetWithNoAction()
            LegionWorship_create(MemberList[button:getTag()-1500].strMemName)
        end
    end
    --玩家的权限
    local Occupation = nil;

    --操作
    local function operation(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            local button = tolua.cast(sender,"Button")
            local layout = listView:getItem(button:getTag()-1-1000):clone()
            LegionMenber_Operation(button:getTag(),MemberList[button:getTag()-1000],Occupation)
            print("111111111111111111111111111111111111111")
        end
    end

    for i=1,#MemberList,1 do
        local member = MemberList[i];
        for j=i+1,#MemberList,1 do
            if member.iOccupation>MemberList[j].iOccupation then
                MemberList[i] = MemberList[j]
                MemberList[j] = member;
                member = MemberList[i]
            end 
        end
    end

    for i=1,#MemberList,1 do
        local layout = UI_GetUILayout(widget, 2):clone()
        layout:setVisible(true)

        listView:pushBackCustomItem(layout)
        --名字
        UI_GetUILabel(layout, 1):setText(MemberList[i].strMemName)
        --职位
        UI_GetUILabel(layout, 2):setText(FormatString("Legion_Position_"..MemberList[i].iOccupation))
        --三日贡献
        UI_GetUILabel(layout, 5):setText(MemberList[i].iDayContribute)
        --总贡献
        UI_GetUILabel(layout, 6):setText(MemberList[i].iTotalContribute)
        --等级
        UI_GetUILabelBMFont(layout, 7):setText(MemberList[i].iLevel)
        --在线状态
        UI_GetUILabel(layout, 11144):setText(LegionMemberList_getOutLIneTime(MemberList[i].outLineSecond,"Max"))
        --头像
        local head = UI_GetUIImageView(layout, 8)
        local headImg = CCSprite:create("Icon/HeroIcon/x"..(1000+i)..".png")
        head:removeNodeByTag(2);
        local headIcon = UIMgr:GetInstance():createMaskedSprite(headImg,"zhujiemian/bg_4.png")
        head:addNode(headIcon,0,2)
        if MemberList[i].bIsCanWorship then
            local button = UI_GetUIButton(layout, 1301)
            button:setVisible(true)
            button:setTag(1500+i)
            button:addTouchEventListener(worship)
        end
        --操作
        UI_GetUILayout(layout, 444):addTouchEventListener(operation)
        UI_GetUILayout(layout, 444):setTag(1000+i)
        if MemberList[i].strMemName == GetLocalPlayer():GetEntityName() then
            UI_GetUIImageView(layout, 1021):loadTexture("Common/Bg_Common_15.png")
            UI_GetUILayout(layout, 1000+i):setTouchEnabled(false)
            if UI_GetUIButton(layout, 1500+i) ~= nil then
                UI_GetUIButton(layout, 1500+i):setVisible(false)
            end
            Occupation = MemberList[i].iOccupation
        end
        

    end
end
--对成员的操作
function LegionMenber_Operation(tag,Member,Occupation)
    local widget = UI_CreateBaseWidgetByFileName("LegionMemeberListCon.json")
    if widget == nil then
        return
    end
    
    UI_GetUILayout(widget, 1124806888):addTouchEventListener(PublicCallBackWithNothingToDo)
    UI_GetUIButton(widget, 4121):addTouchEventListener(UI_ClickCloseCurBaseWidgetWithNoAction)
    --操作界面
    local layoutOperation = UI_GetUILayout(widget, 114) 
    --加入角色信息--------------------------------------
    local location = UI_GetUILayout(widget, 115)
    --重新生成头像
    local head = UI_GetUIImageView(location, 8)
    local headImg = CCSprite:create("Icon/HeroIcon/"..tag..".png")
    head:removeNodeByTag(2);
    local headIcon = UIMgr:GetInstance():createMaskedSprite(headImg,"zhujiemian/bg_4.png")
    head:addNode(headIcon,0,2)
    --名字
    UI_GetUILabel(location, 1):setText(Member.strMemName)
    --职位
    UI_GetUILabel(location, 2):setText(FormatString("Legion_Position_"..Member.iOccupation))
    --三日贡献
    UI_GetUILabel(location, 5):setText(Member.iDayContribute)
    --总贡献
    UI_GetUILabel(location, 6):setText(Member.iTotalContribute)
    --等级
    UI_GetUILabel(location, 7):setText(Member.iLevel)

    -----------------------------------------------------
    --设置操作--------------------------------------
    --加好友
    local function addFriend(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            local tab = GameServer_pb.CMD_FRIENDSYSTEM_REQUEST_CS()
            tab.strName = Member.strMemName
            Packet_Full(GameServer_pb.CMD_FRIENDSYSTEM_REQUEST, tab)
            ShowWaiting()
            print("addFriend")
        end
    end
    UI_GetUIButton(layoutOperation, 1001):addTouchEventListener(addFriend)
    --查看
    local function chick(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then

            local tab = GameServer_pb.CMD_USENAME_FINED_ACTOR_INFO_CS()
			tab.strActorName = Member.strMemName
			Packet_Full(GameServer_pb.CMD_USENAME_FINED_ACTOR_INFO,tab)
		    ShowWaiting()
            print("chick")
        end
    end
    UI_GetUIButton(layoutOperation, 1002):addTouchEventListener(chick)
    --私聊
    local function chat(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
			UI_CloseCurBaseWidget(EUIOpenAction_None);
            g_whisperName = Member.strMemName;
            Chat_Create()
            Chat_SetChanel(EChannelWhisper);
            print("chat")   
        end 
    end
    UI_GetUIButton(layoutOperation, 1003):addTouchEventListener(chat)
    --踢出军团
    local function remove(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            local function removeMember()
                local tab = GameServer_pb.CMD_KICK_LEGIONMEMBER_CS()
			    tab.strLegionMemName = Member.strMemName
			    Packet_Full(GameServer_pb.CMD_KICK_LEGIONMEMBER,tab)
		        ShowWaiting()
                UI_ClickCloseCurBaseWidgetWithNoAction(sender, eventType)
            end
       
            LegionMemberList_MessageBox("Legion_RemoveMember",Member.strMemName,removeMember)
        end
    end
    UI_GetUIButton(layoutOperation, 1004):addTouchEventListener(remove)
    --副团长
    local function vice(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            local function setVice()
                local tab = GameServer_pb.CMD_LEGION_APPOINT_CS()
		        tab.strLegionMemName = Member.strMemName
                if Member.iOccupation == 2 then
                    tab.bIsAppoint = false
                else
                   tab.bIsAppoint = true 
                end
		        Packet_Full(GameServer_pb.CMD_LEGION_APPOINT,tab)
		        ShowWaiting()
                
                UI_ClickCloseCurBaseWidgetWithNoAction(sender, eventType)
            end
            if Member.iOccupation == 2 then
                LegionMemberList_MessageBox("Legion_RemoveVice",Member.strMemName,setVice)
            else
                LegionMemberList_MessageBox("Legion_addVice",Member.strMemName,setVice)
            end
        end
    end
    UI_GetUIButton(layoutOperation, 1005):addTouchEventListener(vice)
    if Member.iOccupation == 2 then
        UI_GetUIButton(layoutOperation, 1005):setTitleText(FormatString("Legion_Vice_2"))
    else
        UI_GetUIButton(layoutOperation, 1005):setTitleText(FormatString("Legion_Vice_1"))
    end
    --转让团长
    local function change(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            local function changeLegionHead()
                local tab = GameServer_pb.CMD_TRANSFER_LEGIONLEADER_CS()
			    tab.strTargetMemName = Member.strMemName
			    Packet_Full(GameServer_pb.CMD_TRANSFER_LEGIONLEADER,tab)
		        ShowWaiting()
                UI_ClickCloseCurBaseWidgetWithNoAction(sender, eventType)
            end
       
            LegionMemberList_MessageBox("Legion_ChangLegionHead",Member.strMemName,changeLegionHead)
            print("change")
        end
    end
    UI_GetUIButton(layoutOperation, 1006):addTouchEventListener(change)
    if Occupation == 3 then
      UI_GetUIButton(layoutOperation, 1006):setVisible(false)
      UI_GetUIButton(layoutOperation, 1005):setVisible(false)
      UI_GetUIButton(layoutOperation, 1004):setVisible(false)
        local image = UI_GetUIImageView(location, 1100)
        image:setSize(CCSizeMake(image:getSize().width,image:getSize().height - 60))
    elseif Occupation == 2 then
      UI_GetUIButton(layoutOperation, 1006):setVisible(false)
      UI_GetUIButton(layoutOperation, 1005):setVisible(false)
    elseif Occupation == 1 then
    end 
end

function LegionMemberList_MessageBox(strIndex,name,callBack)
    local tab = {}
    tab.info = FormatString(strIndex,name)
    tab.msgType = EMessageType_LeftRight
    tab.leftFun = callBack
    Messagebox_Create(tab)
end
    function LegionMemberList_getOutLIneTime(time,kind)
        local timeStr = ""
        local timeD = ""
        local timeH = ""
        local timeM = ""
        local timeS = ""
        if time == 0 then
            return FormatString("Legion_Online")
        end
        if time<=60 then
            return FormatString("Legion_Min",1)
        end
        if time/86400>1 then
            timeD = FormatString("Legion_Day",ToInt(time/86400))
        end
        if (time%86400)/3600>1 then
            timeH = FormatString("Legion_Hour",ToInt((time%86400)/3600))
        end
        if (time%3600)/60>1 then
            timeM = FormatString("Legion_Min",ToInt((time%3600)/60))
        else
            timeM = FormatString("Legion_Min",1)
        end
        if kind == "Day" then
            return timeD
        elseif kind == "DayHour" then
            return timeD..timeH
        elseif kind == "DayHourMin" then
            return timeD..timeH..timeM
        elseif kind == "Hour" then
            return timeH
        elseif kind == "HourMin" then
            return timeH..timeM
        elseif kind == "Min" then
            return timeM
        elseif kind == "Max" then
            if timeD ~= "" then
                return timeD
            elseif timeH ~= "" then
                return timeH
            elseif timeM ~= "" then
                return timeM
            end
        end

        return timeStr
    end
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_QUERY_LEGION_MEMBERLIST, "LEGION_MEMBERLIST" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_LEGION_APPOINT, "LEGION_APPOINT" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_KICK_LEGIONMEMBER, "LEGION_APPOINT" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_TRANSFER_LEGIONLEADER, "LEGION_APPOINT" )

function LEGION_MEMBERLIST(pkg)
    EndWaiting();
	local tmp = GameServer_pb.CMD_QUERY_LEGION_MEMBERLIST_SC();
	tmp:ParseFromString(pkg)
	Log("*********"..tostring(tmp))

    LegionMenberList_Init(tmp.szMemberList)
end
function LEGION_APPOINT(pkg)
    EndWaiting();
    Packet_Cmd(GameServer_pb.CMD_QUERY_LEGION_MEMBERLIST);
    ShowWaiting()
end