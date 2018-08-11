function LegionSalary_Create()
    Packet_Cmd(GameServer_pb.CMD_QUERY_LEGIONPAY);
    ShowWaiting()
    --LegionSalary_initData()
end
function LegionSalary_initData(info)
    local szPayList = info.szPayList
    local widget = UI_GetBaseWidgetByName("LegionSalary")
    if widget == nil then
        widget = UI_CreateBaseWidgetByFileName("LegionSalary.json")
    end
    UI_GetUILayout(widget, 11):addTouchEventListener(PublicCallBackWithNothingToDo)
    UI_GetUIButton(widget, 1):addTouchEventListener(UI_ClickCloseCurBaseWidgetWithNoAction)

    local function getAward(sender,eventType)
        if eventType==TOUCH_EVENT_ENDED then
            local button = tolua.cast(sender,"Button")

            local tab = GameServer_pb.CMD_GET_LEGIONPAY_CS()
            if button:getTag() == 1204 then             
			    tab.iPayType = 1
            elseif button:getTag() == 1205 then
                tab.iPayType = 0
            end
			Packet_Full(GameServer_pb.CMD_GET_LEGIONPAY,tab)
		    ShowWaiting()
        end
    end
    --俸禄
    local layout = UI_GetUILayout(widget, 2)
    --周-铜币
    UI_GetUILabelBMFont(layout, 1201):setText(szPayList[2].iTotalSilver)
    --周-元宝
    UI_GetUILabelBMFont(layout, 1202):setText(szPayList[2].iTotalGold)
    --日-元宝
    UI_GetUILabelBMFont(layout, 1203):setText(szPayList[1].iTotalSilver)
    --周-领取
    UI_GetUIButton(layout, 1204):addTouchEventListener(getAward)
    --日-领取
    UI_GetUIButton(layout, 1205):addTouchEventListener(getAward)
    --数据
    LegionBuildingLayout_RefreshData(UI_GetUILayout(widget, 1212),info.iActorContribute)
    if szPayList[1].bGeted == true then
        UI_GetUIButton(layout, 1205):setTouchEnabled(false)
        UI_GetUIButton(layout, 1205):setTitleText(FormatString("Legion_Geted"))
        --UI_GetUIButton(layout, 1205):addColorGray()
    end
    if szPayList[2].bGeted == true then
        UI_GetUIButton(layout, 1204):setTouchEnabled(false)
        UI_GetUIButton(layout, 1204):setTitleText(FormatString("Legion_Geted"))
        --UI_GetUIButton(layout, 1204):addColorGray()
    end

    local function rule(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            LegionSalary_ShowRule()
        end
    end
    UI_GetUIButton(widget, 2001):addTouchEventListener(rule)
end

function LegionSalary_ShowRule()
    local widget = UI_CreateBaseWidgetByFileName("LegionSalaryRule.json")
    local listView = UI_GetUIListView(widget, 1001)
    for i=1,3,1 do 
        local infolab = CompLabel:GetDefaultCompLabel(FormatString("LegionSalary_Rule_"..i),listView:getSize().width)
        local tempLayout= UI_GetUILayout(widget, 1122):clone()
        --infolab:setColor(ccc3(164,240,25))
        listView:pushBackCustomItem(infolab)
        listView:pushBackCustomItem(tempLayout)
    end
end

ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_QUERY_LEGIONPAY, "LEGIONPAY" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_GET_LEGIONPAY, "GET_LEGIONPAY" )
function LEGIONPAY(pkg)
    EndWaiting();
	local tmp = GameServer_pb.CMD_QUERY_LEGIONPAY_SC();
	tmp:ParseFromString(pkg)
	Log("*********"..tostring(tmp))

    LegionSalary_initData(tmp)
end
function GET_LEGIONPAY(pkg)
    EndWaiting();
    Packet_Cmd(GameServer_pb.CMD_QUERY_LEGIONPAY);
    ShowWaiting()
end