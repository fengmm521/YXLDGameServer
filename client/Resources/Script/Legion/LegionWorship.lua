g_LegionWorship_tili={
    [1]=10,
    [2]=30,
    [3]=60
}
g_LegionWorship_cost={
    [1]=0,
    [2]=30000,
    [3]=100
}
--自己的膜拜数据
function LegionWorship_createMine()
    Packet_Cmd(GameServer_pb.CMD_LEGION_QUERY_WORSHIP);
    ShowWaiting()
    g_LegionWorship_isLegionWorship_createMine = true
end
function LegionWorship_InitMineData(info)
    local widget = UI_GetBaseWidgetByName("LegionCliffordData")
    if widget == nil then
        widget = UI_CreateBaseWidgetByFileName("LegionCliffordData.json")
    end
    local function exit(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            UI_ClickCloseCurBaseWidgetWithNoAction(sender,eventType)
            NewLegionBase_Refresh_CS() 
        end
    end
    UI_GetUILayout(widget, 11):addTouchEventListener(exit)
    UI_GetUIButton(widget, 1005):setVisible(false)
    --剩余次数
    UI_GetUILabel(widget, 1001):setText(info.iRemainWorshipTimes.."/"..info.iTotalWorshipTimes)
    --被膜拜的奖励
    UI_GetUILabel(widget, 1002):setText(info.iCanGetWorshipReward)
    --去膜拜
    local function gotoWorship(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            UI_ClickCloseCurBaseWidgetWithNoAction(sender,eventType)
            LegionMemberList_Create()
        end
    end
    UI_GetUIButton(widget, 1101):addTouchEventListener(gotoWorship)
    --领取奖励
    local function getAward(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            Packet_Cmd(GameServer_pb.CMD_LEGION_GET_WORSHIP_REWARD);
            ShowWaiting()
        end
    end
    UI_GetUIButton(widget, 1102):addTouchEventListener(getAward)
    if info.iCanGetWorshipReward <= 0 then
        UI_GetUIButton(widget, 1102):setTouchEnabled(false)
    end

end
--膜拜界面
function LegionWorship_create(strName)
    g_LegionWorship_strName = strName

    Packet_Cmd(GameServer_pb.CMD_LEGION_QUERY_WORSHIP);
    ShowWaiting()
    g_LegionWorship_isLegionWorship_createMine = false

end
function LegionWorship_InitData()
    local strName = g_LegionWorship_strName
    local widget = UI_GetBaseWidgetByName("LegionClifford.json")
    if widget == nil then
        widget = UI_CreateBaseWidgetByFileName("LegionClifford.json")
    end
    local function exit (sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            
            UI_ClickCloseCurBaseWidgetWithNoAction(sender,eventType)
        end
    end
    UI_GetUILayout(widget, 11):addTouchEventListener(PublicCallBackWithNothingToDo)
    UI_GetUIButton(widget, 1):addTouchEventListener(UI_ClickCloseCurBaseWidgetWithNoAction)
    local layout = UI_GetUILayout(widget, 5)
    --膜拜
    for i=1,3,1 do
        local layoutOne = UI_GetUILayout(layout, i)
        --奖励
        UI_GetUILabel(layoutOne, 1001):setText("+"..g_LegionWorship_tili[i])
        --消耗
        if i==1 then 
        else
            UI_GetUILabel(layoutOne, 1002):setText(g_LegionWorship_cost[i])
        end
        --膜拜
        local function Worship(sender,eventType)
            if eventType==TOUCH_EVENT_ENDED then
                local button = tolua.cast(sender,"Button")

                local tab = GameServer_pb.CMD_LEGION_WORSHIP_MEMBER_CS()
			    tab.strMemberName = strName
                tab.worshipType = button:getTag()
			    Packet_Full(GameServer_pb.CMD_LEGION_WORSHIP_MEMBER,tab)
		        ShowWaiting() 
                UI_ClickCloseCurBaseWidgetWithNoAction(sender,eventType)
            end
        end
        UI_GetUIButton(layoutOne, 1003):addTouchEventListener(Worship)
        UI_GetUIButton(layoutOne, 1003):setTag(i)
    end
    
    local data = UI_GetUILayout(widget, 3001)
    LegionBuildingLayout_RefreshData(data,g_LegionWorship_Worship)
end
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_LEGION_QUERY_WORSHIP, "QUERY_WORSHIP" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_LEGION_WORSHIP_MEMBER, "LEGION_APPOINT" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_LEGION_GET_WORSHIP_REWARD, "WORSHIP_REWARD" )

function QUERY_WORSHIP(pkg)
    EndWaiting();
	local tmp = GameServer_pb.CMD_LEGION_QUERY_WORSHIP_SC();
	tmp:ParseFromString(pkg)
	Log("*********"..tostring(tmp))
    
    g_LegionWorship_Worship = tmp.iActorContribute
    if g_LegionWorship_isLegionWorship_createMine == true then
        LegionWorship_InitMineData(tmp)
    else
        LegionWorship_InitData()
    end
end
function WORSHIP_REWARD()
    EndWaiting();
    Packet_Cmd(GameServer_pb.CMD_LEGION_QUERY_WORSHIP);
    ShowWaiting()
end