function LegionBuildingLayout_Create()
    Packet_Cmd(GameServer_pb.CMD_QUERY_LELGION_CONTRIBUTE);
    ShowWaiting()
    --LegionBuildingLayout_Init()
end
function LegionBuildingLayout_Init(info)
    local widget = UI_GetBaseWidgetByName("Legionbuilding")
    if widget == nil then
        widget = UI_CreateBaseWidgetByFileName("Legionbuilding.json")
    end
    local function exit(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            UI_ClickCloseCurBaseWidgetWithNoAction(sender,eventType)
            NewLegionBase_Refresh_CS()
        end
    end
    UI_GetUILayout(widget, 123):addTouchEventListener(PublicCallBackWithNothingToDo)
    UI_GetUIButton(widget, 1):addTouchEventListener(exit)
    local layout = UI_GetUILayout(widget, 5)
    --军团等级
    UI_GetUILabel(layout, 1002):setText(FormatString("Legion_LegionLevelBuilding",info.iLevel))
    --进度条
    UI_GetUILoadingBar(layout, 1003):setPercent(info.iLegionExp*100/info.iLegionUpLevelExp)
    
    --进度条
    UI_GetUILabel(layout, 1701):setText(info.iLegionExp.."/"..info.iLegionUpLevelExp)
    UI_GetUILabel(layout, 1701):enableStroke()
    --建设
    local function building(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            local button = tolua.cast(sender,"Button")
            print("1-----------------------tag == "..button:getTag())
            local data = GetGameData(DataFileLegionContribute,button:getTag()*2-1,"stLegionContributeData")
            local tab = GameServer_pb.CMD_LEGION_CONTRIBUTE_CS()
            tab.iContributeType = data.m_ID
            Packet_Full(GameServer_pb.CMD_LEGION_CONTRIBUTE, tab)
            ShowWaiting()
        end
    end
    for i=1,3,1 do
        UI_GetUIButton(layout, i):addTouchEventListener(building)
    end
    LegionBuildingLayout_RefreshData(UI_GetUILayout(widget, 1212),info.iContribute)

    if info.iGetContribute ~= nil and info.iGetContribute ~= 0 then
        createPromptBoxlayout(FormatString("Legion_AddContribute",info.iGetContribute))
    end
end
--更新铜币等数据
function LegionBuildingLayout_RefreshData(layoutData,contribute)
    local function dealWithData(num)
        if num>100000000 then
            return FormatString("TenThousand",num/10000) 
        end
        return num 
    end
    local playerData = GetLocalPlayer()    
    --铜币   
    UI_GetUILabelBMFont(layoutData, 1):setText(dealWithData(playerData:GetInt(EPlayer_Silver)))
    --元宝
    UI_GetUILabelBMFont(layoutData, 2):setText(dealWithData(playerData:GetInt(EPlayer_Gold)))
    --贡献
    if contribute ~= nil then
        UI_GetUILabelBMFont(layoutData, 3):setText(contribute)
    else
        UI_GetUILabelBMFont(layoutData, 3):setText("test")
    end
end
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_QUERY_LELGION_CONTRIBUTE, "LELGION_CONTRIBUTE" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_LEGION_CONTRIBUTE, "CONTRIBUTE" )
function LELGION_CONTRIBUTE(pkg)
    EndWaiting()
    local info = GameServer_pb.CMD_QUERY_LELGION_CONTRIBUTE_SC()
    info:ParseFromString(pkg)
    Log("xxxxx ===="..tostring(info))
    LegionBuildingLayout_Init(info)
end
function CONTRIBUTE(pkg)
    -- body
    EndWaiting()
    Log("sever has return")
    local info = GameServer_pb.CMD_LEGION_CONTRIBUTE_SC()
    info:ParseFromString(pkg)
    Log("xxxxx ===="..tostring(info)) 
    LegionBuildingLayout_Init(info)   
end