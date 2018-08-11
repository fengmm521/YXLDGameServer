function TerritoryProtect_Create()
    Packet_Cmd(GameServer_pb.CMD_MANORSYSTEM_QUERY_PROTECT);
    ShowWaiting()
end
function TerritoryProtect_init(info)
    
    local widget = UI_GetBaseWidgetByName("TerritoryProtect")
    if widget == nil then
        widget = UI_CreateBaseWidgetByFileName("TerritoryProtect.json")
    end
    UI_GetUILayout(widget, 1003):addTouchEventListener(LogAndEnternal_Exit)
    local function setOneProtectData(tag)
        local protectData = GetGameData(DataFileManorProtect, tag, "stManorProtectData")
        local image = UI_GetUIImageView(widget, 1000+tag)
        local waitTimeTag = TerritoryProtect_FindData(info,tag)
        --购买冷却时间
        if waitTimeTag ~= 0 then
            UI_GetUILabel(image, 2002):setVisible(false)
            UI_GetUILabel(image, 2003):setVisible(false)         
            UI_GetUILabel(image, 2004):setVisible(true)         
            UI_GetUILabel(image, 2004):setTimer(info.szProtectDetail[waitTimeTag].iRemaindSecond,TimeFormatType_HourMinSec)
            local function refreshData()
                Packet_Cmd(GameServer_pb.CMD_MANORSYSTEM_QUERY_PROTECT);
                ShowWaiting()
            end
            UI_GetUILabel(image, 2004):addEventListenerTimer(refreshData)
        else
            UI_GetUILabel(image, 2002):setVisible(true)
            UI_GetUILabel(image, 2003):setVisible(true)         
            UI_GetUILabel(image, 2004):setVisible(false)  
            UI_GetUILabel(image, 2003):setText(LegionMemberList_getOutLIneTime(protectData.m_CD,"Day"))
        end
        --价格
        UI_GetUILabel(image:getChildByTag(2005), 3001):setText(protectData.m_cost)
        --名字
        UI_GetUILabel(image, 2001):setText(protectData.m_name)
        --购买护盾
        local function Buy(sender,event)
            local function sendBuy()
                local tab = GameServer_pb.CMD_MANORSYTEM_BUY_PROTECT_CS();		
			    tab.iProtectId = tag;	
			    Packet_Full(GameServer_pb.CMD_MANORSYTEM_BUY_PROTECT, tab);
            end
            if event== TOUCH_EVENT_ENDED then
                if waitTimeTag ~= 0 then
                    createPromptBoxlayout(FormatString("TerritoryProtect_CD"))
                else
                    local tab = {}
                    tab.info = protectData.m_info
                    tab.msgType = EMessageType_LeftRight
                    tab.leftFun = sendBuy
                    Messagebox_Create(tab)
                end
            end
        end
        image:addTouchEventListener(Buy)
    end
    --一天护盾
    setOneProtectData(1)
    --两天护盾
    setOneProtectData(2)
end

function TerritoryProtect_FindData(info,tag)
    if info~=nil then
        for i=1,#info.szProtectDetail,1 do
            if info.szProtectDetail[i].iProtectId == tag then
                return i
            end
        end
    end
    return 0
end

ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_MANORSYSTEM_QUERY_PROTECT, "MANORSYSTEM_QUERY_PROTECT" );
function MANORSYSTEM_QUERY_PROTECT(pkg)
    EndWaiting();
	local tmp = GameServer_pb.CMD_MANORSYSTEM_QUERY_PROTECT_SC();
	tmp:ParseFromString(pkg)
	Log("*********"..tostring(tmp))
	TerritoryProtect_init(tmp)
end