local l_OtherPlayerInfo = nil 
function OtherPlayerInfo()
	-- body
	local widget = UI_CreateBaseWidgetByFileName("OtherPlayerInfo.json")
    
   

    local OtherPlayerInfolayout = UI_GetUILayout(widget,1)
    
    --名字
    UI_SetLabelText(OtherPlayerInfolayout,2,l_OtherPlayerInfo.detail.strActorName)

    UI_GetUIButton(OtherPlayerInfolayout,1):addTouchEventListener(UI_ClickCloseCurBaseWidget)
    OtherPlayerInfolayout:addTouchEventListener(UI_ClickCloseCurBaseWidget)

    --等级
    UI_SetLabelText(widget,3,l_OtherPlayerInfo.detail.iActorLevel)

    --竞技场排名
    if l_OtherPlayerInfo.detail.iAreanRank == 0 then
    	UI_SetLabelText(OtherPlayerInfolayout,4,FormatString("OtherPlayerInfo_NoCurRank"))
    else
    	UI_SetLabelText(OtherPlayerInfolayout,4,l_OtherPlayerInfo.detail.iAreanRank)
    end

    --所属军团
    if l_OtherPlayerInfo.detail.strBelongLegionName ~= nil 
        and l_OtherPlayerInfo.detail.strBelongLegionName ~= "" then
    	Log("xxxxxxxxxxxx")
    	UI_SetLabelText(OtherPlayerInfolayout,5,l_OtherPlayerInfo.detail.strBelongLegionName)
    else 
    	UI_SetLabelText(OtherPlayerInfolayout,5,FormatString("OtherPlayerInfo_NoLegion"))
    end

    --战斗力
    UI_SetLabelText(OtherPlayerInfolayout,6,l_OtherPlayerInfo.detail.iTotalFightStrength)

    for i = 1,#l_OtherPlayerInfo.detail.szHeroList do

        local layout = UI_GetUILayout(widget, 4651039)
        local image = UI_GetUIImageView(layout, 1200+l_OtherPlayerInfo.detail.szHeroList[i].ipos)

        local icon = UI_IconFrame(UI_GetHeroIcon(l_OtherPlayerInfo.detail.szHeroList[i].iHeroID
                                                , l_OtherPlayerInfo.detail.szHeroList[i].iLevel)
                                    ,l_OtherPlayerInfo.detail.szHeroList[i].iLevelStep
                                    ,l_OtherPlayerInfo.detail.szHeroList[i].iQuality)
        image:addChild(icon,3,3)
    end

    --头像
    local head = UI_GetUILayout(widget, 4680450)
    
    local headImg = CCSprite:create("Icon/HeroIcon/x"..l_OtherPlayerInfo.detail.iHeadId..".png")
    head:addNode(UIMgr:GetInstance():createMaskedSprite(headImg,"zhujiemian/bg_4.png"),0,2)
    --head:setScale(0.7)
end

function OtherPlayerInfoView(pkg)
	-- body
	EndWaiting()
    l_OtherPlayerInfo = GameServer_pb.CMD_FINED_ACTOR_INFO_SC()
    l_OtherPlayerInfo:ParseFromString(pkg)
    Log("OtherPlayerInfo=========="..tostring(l_OtherPlayerInfo))
    OtherPlayerInfo()
end
ScriptSys:GetInstance():RegisterScriptFunc(GameServer_pb.CMD_USENAME_FINED_ACTOR_INFO, "OtherPlayerInfoView")
