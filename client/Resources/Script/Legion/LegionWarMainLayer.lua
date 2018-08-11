--已经被占领的领地
local l_OccupiedTerritory = nil

local l_LegionCityInfo = nil
function PowerdistributionInfo()
	-- body
    local widget = UI_CreateBaseWidgetByFileName("Powerdistribution.json")
    
    --return bt
	local returnBt = UI_GetUIButton(widget,1)
    returnBt:addTouchEventListener(UI_ClickCloseCurBaseWidget)

    local image = UI_GetUIImageView(widget,2)
    local label = UI_GetUILabel(widget,3)


    --宣战
    local function clickDeclarationFunc(sender,eventType)
    	-- body
    	if eventType == TOUCH_EVENT_ENDED then
    		local sender = tolua.cast(sender,"Button")
	    	local tag = sender:getTag()
	    	Log("tag ========"..tag)
            if tag ~= 10 then
                if l_LegionCityInfo.szCityList[tag].bCanReportFight == true then
                    local tab = GameServer_pb.CMD_QUERY_LEGIONBATTLE_REPORT_CS()
            		tab.iQueryCityID = l_LegionCityInfo.szCityList[tag].iCityID
                    Packet_Full(GameServer_pb.CMD_QUERY_LEGIONBATTLE_REPORT, tab)
                    ShowWaiting()
                else
                    Messagebox_Create({title = FormatString("LegionBoss_Tip"),info = FormatString("LegionWar_joinTip"), msgType = EMessageType_Middle,});
                end
            else
                Messagebox_Create({title = FormatString("LegionBoss_Tip"),info = FormatString("LegionWar_Imperial"), msgType = EMessageType_Middle,});
            end
	    end
    end
    local Legionmapparent = UI_GetUILayout(widget,2)
    for i = 1, 10 do
        UI_GetUIButton(Legionmapparent,i):addTouchEventListener(clickDeclarationFunc)
    end
    LookUpColor(widget)
end

--选取颜色
function LookUpColor(widget)
	-- body
	local Legionmapparent = UI_GetUILayout(widget,2)
	--保存帮主的名字
	local legionHeaderName = {}
	local LegionMapColor = {}
	local function hasotherSameName(strname)
		-- body
		Log("legionHeaderName========="..#legionHeaderName)
		for i = 1,#legionHeaderName do
            if legionHeaderName[i] == strname then
            	return true
            end
		end
		return false
	end
	for i = 1,#l_LegionCityInfo.szCityList do 
		if l_LegionCityInfo.szCityList[i].bMonsterOwner == false then
			if hasotherSameName(legionHeaderName,l_LegionCityInfo.szCityList[i].strOwner) == false then  
				Log("index..==="..i)
			    table.insert(legionHeaderName,l_LegionCityInfo.szCityList[i].strOwner)
			    table.insert(LegionMapColor,g_LegionTerritoryColor[i])
		    end
	    end 
    end
    for i = 1,#legionHeaderName do
        for j = 1,#l_LegionCityInfo.szCityList do
            if legionHeaderName[i] == l_LegionCityInfo.szCityList[j].strOwner then
            	Log("xxxxxxxxxxx===="..i,j)
            	UI_GetUIButton(Legionmapparent,j):setColor(LegionMapColor[i])
            	UI_GetUILabel(UI_GetUIButton(Legionmapparent,j),1):setText("【"..legionHeaderName[i].."】")
            	UI_GetUILabel(UI_GetUIButton(Legionmapparent,j),1):setVisible(true)
            	UI_GetUILabel(UI_GetUIButton(Legionmapparent,j),1):setFontSize(20)
            	UI_GetUILabel(UI_GetUIButton(Legionmapparent,j),1):setColor(ccc3(0,0,0))
            end
        end
    end
end

local custonToString = tostring
function LegionPowerDistributionInfo(pkg)
	-- body
 	EndWaiting()
    l_LegionCityInfo = GameServer_pb.CMD_QUERY_BATTLEOWNER_SC()
    l_LegionCityInfo:ParseFromString(pkg)
    Log("xxxxx ===="..custonToString(l_LegionCityInfo))  
    PowerdistributionInfo() 
end
ScriptSys:GetInstance():RegisterScriptFunc(GameServer_pb.CMD_QUERY_BATTLEOWNER, "LegionPowerDistributionInfo")