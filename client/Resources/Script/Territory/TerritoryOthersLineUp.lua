function TerritoryOhtersLineUp_Create(info)

    local widget = UI_CreateBaseWidgetByFileName("TerritoryOthersTeam1.json")
    UI_GetUIButton(widget, 4724002):addTouchEventListener(UI_ClickCloseCurBaseWidgetWithNoAction)
    UI_GetUILayout(widget, 4723492):addTouchEventListener(PublicCallBackWithNothingToDo)
    --消耗
    UI_GetUILabel(widget, 10003):setText(info.iLootCostVigor)

    --对方的数据
    local right = UI_GetUILayout(widget, 10005)
    TerritoryOhtersLineUp_InitTeamData(right,info.beLootActorInfo.strActorName,info.beLootActorInfo.iActorLevel,
                info.beLootActorInfo.iFightValue,info.beLootActorInfo.iHeadID)
    local positionLayout = UI_GetUILayout(right, 20006)
    --对方的队伍
    local Objects = info.szEnemyHeroFormation
    for i=1,#Objects,1 do
        if Objects[i]~=nil and Objects[i].iPos ~= -1 then
            local location = UI_GetUILayout(positionLayout, 3000+Objects[i].iPos)
            local icon =  UI_IconFrame(UI_GetHeroIcon(Objects[i].iBaseId, Objects[i].iLevel), Objects[i].iLevelStep, Objects[i].iquality);
            icon:setPosition(ccp(44,44))
            location:addChild(icon,10)
         end
    end
    TerritoryOhtersLineUp_RefreshPlayerTeam()
    --布阵
    local function formation(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            Formation_Create()
        end
    end
    UI_GetUIButton(widget, 10001):addTouchEventListener(formation)
    --掠夺
    local function loot(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            UI_ClickCloseCurBaseWidgetWithNoAction(sender,eventType)
            local tab = GameServer_pb.CMD_MANOR_REQUEST_LOOT_BATTLE_CS()
            tab.enemy = g_territoryInfo.enemy
            Packet_Full(GameServer_pb.CMD_MANOR_REQUEST_LOOT_BATTLE, tab)
            ShowWaiting();
        end
    end
    UI_GetUIButton(widget, 10002):addTouchEventListener(loot)
end
--初始化某方的信息
function TerritoryOhtersLineUp_InitTeamData(layout,name,lvl,fightValue,headId)
    --名字
    UI_GetUILabel(layout, 20004):setText(name)
    --等级
    UI_GetUILabel(layout, 20003):setText(lvl)
    --战斗力
    UI_GetUILabel(layout, 20002):setText(fightValue)

    --头像
    local head = UI_GetUILayout(layout, 20005)
    local headImg = CCSprite:create("Icon/HeroIcon/"..headId..".png")
    head:removeNodeByTag(2);
    local headIcon = UIMgr:GetInstance():createMaskedSprite(headImg,"zhujiemian/bg_4.png")
    head:addNode(headIcon,0,2)      
end 
function TerritoryOthersTeam1_Refresh(widget)
    if UI_GetBaseWidgetByName("TerritoryOthersTeam1") then
        TerritoryOhtersLineUp_RefreshPlayerTeam()
    end
end
--刷新自己的数据
function TerritoryOhtersLineUp_RefreshPlayerTeam()
    local widget = UI_GetBaseWidgetByName("TerritoryOthersTeam1")
    local lift = UI_GetUILayout(widget, 10004)
    TerritoryOhtersLineUp_InitTeamData(lift,GetLocalPlayer():GetEntityName(),GetLocalPlayer():GetInt(EPlayer_Lvl),
                GetLocalPlayer():GetInt(EPlayer_FightValue),GetLocalPlayer():GetInt(EPlayer_HeadId))
    --自己的队伍
    local positionLayout = UI_GetUILayout(lift, 20006)
    local formationList = GetLocalPlayer():GetFormationList();
    for i=0, formationList:size()-1 do
		if (i+1)%3 ~= 0 then          
            local location = UI_GetUIImageView(positionLayout, 3000+i)
            location:removeChildByTag(100)
			local heroEntity = GetLocalPlayer():GetHeroByObjId(formationList[i]);
			if heroEntity ~= nil then
                local data = heroEntity:GetHeroData();
                local icon  = UI_IconFrame(UI_GetHeroIcon(data.m_ID, heroEntity:GetInt(EHero_Lvl)), heroEntity:GetInt(EHero_LvlStep), heroEntity:GetInt(EHero_Quality))
                location:addChild(icon,100,100)
                icon:setPosition(ccp(44,44))
            end 
        end
	end
end
function NineSkyOhtersLineUp_Create(info)
    local widget = UI_CreateBaseWidgetByFileName("TerritoryOthersTeam.json")
    UI_GetUILabel(widget, 4670716):addTouchEventListener(PublicCallBackWithNothingToDo)
    UI_GetUIButton(widget, 4674051):addTouchEventListener(UI_ClickCloseCurBaseWidget)

    --UI_GetUIImageView(widget, 4674043):setVisible(false)
    --等级
    UI_GetUILabel(widget, 4670726):setText(info.iActorLevel)
    --战斗力
    UI_GetUILabel(widget, 4670749):setText(info.iTotalFightVaule)
    
    local iconID = 0
    
    --神兽
    local baseID = 0
    local isGodAnimal = false
    for i=1,#info.szFormationItem,1 do
        if info.szFormationItem[i].bIsGodAnimal == true then
            isGodAnimal = true
            baseID = info.szFormationItem[i].iBaseId
        end
    end

    if baseID == 0 then
        --普通队伍
        local head = UI_GetUILayout(widget, 4670720)
        local headImg = CCSprite:create("Icon/HeroIcon/"..info.iActorHead..".png")
        head:removeNodeByTag(2);
        local headIcon = UIMgr:GetInstance():createMaskedSprite(headImg,"zhujiemian/bg_4.png")
        head:addNode(headIcon,0,2)
        --名字
        UI_GetUILabel(widget, 4670729):setText(info.strActorName)

        iconID = info.iActorHead

    else
        print("baseID================"..baseID)
        local animationData = GetGameData(DataFileMonster, baseID, "stMonsterData");

         --名字
        UI_GetUILabel(widget, 4670729):setText(animationData.m_name)
        iconID = animationData.m_icon
    end


        local Objects = info.szFormationItem
        local layout = UI_GetUILayout(widget, 4670775)
        --队伍
        local haveGodAnimal = false
        local isAllDeath = true
        for i=1,#Objects,1 do
            local pos = Objects[i].iPos
            if pos==-1 then
                pos = 3
            end
            if Objects[i]~=nil then
                local location = UI_GetUILayout(layout, 1200+Objects[i].iPos)
                local icon = nil
                if Objects[i].bIsGodAnimal == false then
                    icon =  UI_IconFrame(UI_GetHeroIcon(Objects[i].iBaseId, Objects[i].iLevel,nil,Objects[i].iHp*100/Objects[i].iMaxHP)
                                    , Objects[i].iLevelStep, Objects[i].iQuality);

                else
                    haveGodAnimal = true
                    local animationData = GetGameData(DataFileMonster, baseID, "stMonsterData");
                    icon =  UI_IconFrame(UI_GetHeroIcon(animationData.m_icon, Objects[i].iLevel,nil,Objects[i].iHp*100/Objects[i].iMaxHP)
                                    ,Objects[i].iLevelStep,13);
                                    --
                end
                print(Objects[i].iHp*100/Objects[i].iMaxHP)
                if Objects[i].iHp*100/Objects[i].iMaxHP<=0 then
                    --有个image的Tag值改漏了
                    if UI_GetUIImageView(location, 1124778950) then
                        UI_GetUIImageView(location, 1124778950):setTag(1122)
                    end
                    UI_GetUIImageView(location, 1122):setVisible(true)
                    
                else
                    isAllDeath= false
                end
                icon:setPosition(ccp(44,44))
                location:addChild(icon,10)
            end
        end
        --头像
    local head = UI_GetUILayout(widget, 4670720)
    local headImg
    if haveGodAnimal == false then
        headImg = CCSprite:create("Icon/HeroIcon/"..iconID..".png")
    else
        headImg = CCSprite:create("Icon/HeroIcon/"..iconID..".png")
    end
    head:removeNodeByTag(2);
    local headIcon = UIMgr:GetInstance():createMaskedSprite(headImg,"zhujiemian/bg_4.png")
    head:addNode(headIcon,0,2)

    --挑战
    local function fight(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            Packet_Cmd(GameServer_pb.CMD_DREAMLAND_GETACTOR_FORMATION_INFO);
			ShowWaiting();
			UI_CloseCurBaseWidget();
        end
    end
    local button =  UI_GetUIButton(widget, 4670776)
    button:addTouchEventListener(fight)
    --button:setTitleText(FormatString("NineSkyFight"))
    --button:setPosition(ccp(button:getPositionX(),button:getPositionY()-10))
    if isAllDeath then
        button:setTouchEnabled(false)
    end

    --关卡
    UI_GetUILabel(widget, 4724161):setText("("..g_nineSkyIndexID.."/16)")

    --军团
    print("info.strLegionName================"..info.strLegionName)
    if info.strLegionName ~= nil and info.strLegionName ~= "" then
        UI_GetUILabel(widget, 4670743):setText(info.strLegionName)
    else
        print("111111111111111111111111")
    end
end