--获取日志列表
function LogAndEternal_getLog()
    Packet_Cmd(GameServer_pb.CMD_MANOR_QUERY_LOG);
    ShowWaiting()
end
--报复
function LogAndEternal_getBack(enemy)
    local tab = GameServer_pb.CMD_MANOR_REVENGE_CS();
	tab.enemy = enemy
	Packet_Full(GameServer_pb.CMD_MANOR_REVENGE, tab);

    ShowWaiting()
end
--发布悬赏
function LogAndEternal_offAReward()
    local tab = GameServer_pb.CMD_MANOR_REVENGE_CS();
	--tab.enemy = enemy
	--Packet_Full(GameServer_pb.CMD_MANOR_PUBLISH, tab);

    --ShowWaiting()
end
--获取武魂殿数据
function LogAndEternal_getEnternal()
    Packet_Cmd(GameServer_pb.CMD_MANOR_WUHUNDIAN_QUERY);
    ShowWaiting()
end
--获取铁匠铺数据
function LogAndEternal_getBlacksmith()
    Packet_Cmd(GameServer_pb.CMD_MANOR_TIEJIANGPU_QUERY);
    ShowWaiting()
    print("---------------------1----------------------")
end
--刷新武魂殿英雄
function LogAndEternal_refreshEnternal()
    Packet_Cmd(GameServer_pb.CMD_MANOR_WUHUNDIAN_REFRESH);
    ShowWaiting()
end
--刷新铁匠铺英雄
function LogAndEternal_refreshBlacksmith()
    Packet_Cmd(GameServer_pb.CMD_MANOR_TIEJIANGPU_REFRESH);
    ShowWaiting()
end
--开始祭炼-武魂殿
function LogAndEternal_refiningEnternal()
    Packet_Cmd(GameServer_pb.CMD_MANOR_WUHUNDIAN_BEGIN_PRODUCT);
    ShowWaiting()
end
--开始打造-铁匠铺
function LogAndEternal_refiningBlacksmith()
    Packet_Cmd(GameServer_pb.CMD_MANOR_TIEJIANGPU_BEGIN_PRODUCT);
    ShowWaiting()
end
--无懈可击-武魂殿
function LogAndEternal_WatertightEnternal(iBoxIndex)
    local tab = GameServer_pb.CMD_MANOR_WUHUNDIAN_WUXIEKEJIE_CS();
	tab.iBoxIndex = iBoxIndex
	Packet_Full(GameServer_pb.CMD_MANOR_WUHUNDIAN_WUXIEKEJIE, tab);

    ShowWaiting()
end
--无懈可击-铁匠铺
function LogAndEternal_WatertightBlacksmith(iBoxIndex)
    local tab = GameServer_pb.CMD_MANOR_TIEJIANGPU_WUXIEKEJI_CS();
	tab.iBoxIndex = iBoxIndex
	Packet_Full(GameServer_pb.CMD_MANOR_TIEJIANGPU_WUXIEKEJI, tab);

    ShowWaiting()
end
--一键收取-武魂殿
function LogAndEternal_getEnternalItem()
    Packet_Cmd(GameServer_pb.CMD_MANOR_WUHUNDIAN_ONEKEY_HARVEST);
    ShowWaiting()
end
--一键收取-铁匠铺
function LogAndEternal_getBlacksmithItem()
    Packet_Cmd(GameServer_pb.CMD_MANOR_TIEJIANGPU_ONEKEY_HARVEST);
    ShowWaiting()
end
--掠夺
function LogAndEternal_plunder()
    local tab = GameServer_pb.Cmd_Cs_QueryGodAnimalDetai();
	tab.dwGodAnimalObjectID = EGodAnimal_ObjId
	Packet_Full(GameServer_pb.CMD_GODANIMAL_DETAIL, tab);

    ShowWaiting()
end
--升级-武魂殿
function LogAndEternal_LevelUpEnternal()
    Packet_Cmd(GameServer_pb.CMD_MANOR_WUHUNDIAN_LEVELUP);
    ShowWaiting()
end
--升级-铁匠铺
function LogAndEternal_LevelUpBlacksmith()
    Packet_Cmd(GameServer_pb.CMD_MANOR_TIEJIANGPU_LEVELUP);
    ShowWaiting()
end
--加速-武魂殿
function LogAndEternal_SpeedUpEnternal()
    Packet_Cmd(GameServer_pb.CMD_MANOR_WUHUNDIAN_BULID_ADDSPEED);
    ShowWaiting()
end
--加速-铁匠铺
function LogAndEternal_SpeedUpBlacksmith()
    Packet_Cmd(GameServer_pb.CMD_MANOR_TIEJIANGPU_BULID_ADDSPEED);
    ShowWaiting()
end
---------------------------------------------------------------------
----------------------------------日志-------------------------------
--外部用于初始化日志界面的接口
function LogAndEnternal_createLog()
    LogAndEternal_getLog()
    --LogAndEnternal_createLogLayer()
end
--从网络获取的数据进行初始化日志界面
function LogAndEnternal_createLogLayer(pkg) 
    EndWaiting()
    local info = GameServer_pb.CMD_MANOR_QUERY_LOG_SC()
    info:ParseFromString(pkg)
    Log("info===="..tostring(info))
    local widget = UI_GetBaseWidgetByName("TerritoryLog")

    if widget == nil then
        widget = UI_CreateBaseWidgetByFileName("TerritoryLog.json")
    end
    UI_GetUIButton(widget, 2):addTouchEventListener(LogAndEnternal_Exit)
    UI_GetUILayout(widget, 4619554):addTouchEventListener(PublicCallBackWithNothingToDo)
    --报复
    local function revenge (send,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            local tag = tolua.cast(send,"Button"):getTag()-1900
            UI_ClickCloseCurBaseWidgetWithNoAction(send, eventType)
            LogAndEternal_getBack(info.szManorLootLog[tag].attacker)
        end
    end
    --悬赏
    local function offerAReward (send,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            local tag = tolua.cast(send,"Button"):getTag()-1900
            createPromptBoxlayout(FormatString("LogNotOpen"))
        end
    end
    local listView = UI_GetUIListView(widget, 1)
    listView:removeAllItems()
    --添加消息
    for i=1,#info.szManorLootLog,1 do
        local oneMessage = nil 
        --消息内容
        if info.szManorLootLog[i].bAttackerWin == 0 then --掠夺失败
            oneMessage = UI_GetUILayout(widget, 4558304):clone()
            local message = FormatString("PlunderFail",info.szManorLootLog[i].strAttackerName)
            local label = UI_GetUILabel(oneMessage, 4558312)
            label:setText("")
            local messageLabel = CompLabel:GetDefaultCompLabel(message,480)
            messageLabel:setAnchorPoint(ccp(0,0.5))
            messageLabel:setPosition(ccp(label:getPosition()))
            oneMessage:addChild(messageLabel,11,11)
            listView:pushBackCustomItem(oneMessage)
        else--掠夺成功
            oneMessage = UI_GetUILayout(widget, 4603981):clone()
            local message = FormatString("PlunderSuccess",info.szManorLootLog[i].strAttackerName)

            local szAwardPropList = info.szManorLootLog[i].award.szAwardPropList
            local szAwardItemList = info.szManorLootLog[i].award.szAwardItemList
            local szAwardHeroList = info.szManorLootLog[i].award.szAwardHeroList
            --修为和铜币
            local tempStr = ""
            --if szAwardPropList ~= nil or szAwardItemList~= nil or szAwardHeroList ~= nil  then
                tempStr = tempStr.. FormatString("PlunderSuccessAward")
            --end
            for j=1,szAwardPropList == nil or #szAwardPropList,1 do
                if j ~= 1 then
                    tempStr = tempStr..","
                end
                tempStr = tempStr..FormatString("Att_"..szAwardPropList[j].iLifeAttID,szAwardPropList[j].iValue)
            end
            --道具
            for j=1,szAwardItemList == nil or #szAwardItemList,1 do
                if( szAwardItemList[j].iItemID ~= -1 ) then
                    local itemdata = GetGameData(DataFileItem,szAwardItemList[j].iItemID,"stItemData")
                    tempStr = tempStr..","..itemdata.m_name.."x"..szAwardItemList[j].iCount
                end
            end
            --武魂
            for j=1,szAwardHeroList == nil or #szAwardHeroList,1 do
                if( szAwardHeroList[j].iItemID ~= -1 ) then
                    local herodata = GetGameData(DataFileHeroBorn,szAwardHeroList[j].iHeroID,"stHeroBornData")
                    tempStr = tempStr..","..FormatString("Enternal_E",itemdata.m_name).."x"..szAwardItemList[j].iCount
                end
            end
            local label = UI_GetUILabel(oneMessage, 4558312)
            label:setText("")
            local messageLabel = CompLabel:GetDefaultCompLabel(message..FormatString("Enternal_None",tempStr),label:getSize().width)
            messageLabel:setAnchorPoint(ccp(0,0.5))
            messageLabel:setPosition(ccp(label:getPosition()))
            oneMessage:addChild(messageLabel,11,11)
            listView:pushBackCustomItem(oneMessage)

            --悬赏
            UI_GetUIButton(oneMessage, 4603992):addTouchEventListener(offerAReward)
            UI_GetUIButton(oneMessage, 4603992):setTag(2000+i)
        end
        oneMessage:setVisible(true)
        --按钮回调-报复
        UI_GetUIButton(oneMessage, 4558315):addTouchEventListener(revenge)
        UI_GetUIButton(oneMessage, 4558315):setTag(1900+i)
        UI_GetUIImageView(widget, 101060):setTouchEnabled(true)
    end
    UI_GetUIImageView(widget, 4702500):setVisible(#info.szManorLootLog <= 0)
end
---------------------------------------------------------------------------------------
------------------------------武魂殿---------------------------------------------------
function LogAndEnternal_Exit(sender,eventType)
    if eventType == TOUCH_EVENT_ENDED then 
        UI_ClickCloseCurBaseWidgetWithNoAction(sender,eventType)
        Packet_Cmd(GameServer_pb.CMD_MANOR_QUERY);
        ShowWaiting()
    end
end
--外部用于初始化武魂殿界面的接口
function LogAndEnternal_createEnternal()
    g_LogAndEnternal_EnternalOrBlacksmith = "Enternal"
    LogAndEternal_getEnternal()
    --LogAndEnternal_createEnternalLayer()
end
--从网络获取的数据进行初始化武魂殿界面
function LogAndEnternal_createEnternalLayer(pkg)
    EndWaiting()
    local widget = UI_GetBaseWidgetByName("TerritoryEnternal")

    if widget == nil then
        widget = UI_CreateBaseWidgetByFileName("TerritoryEnternal.json")
    end
    UI_GetUIButton(widget, 4556493):addTouchEventListener(LogAndEnternal_Exit)
    UI_GetUILayout(widget, 4619553):addTouchEventListener(PublicCallBackWithNothingToDo)
    local info = GameServer_pb.CMD_MANOR_WUHUNDIAN_QUERY_SC()
    info:ParseFromString(pkg)
    Log("info===="..tostring(info))

    --"武魂殿"
    UI_GetUIImageView(UI_GetUIImageView(widget, 4591787), 1001):loadTexture("titleFont/titleFont_wuhundian.png")
    UI_GetUILabel(widget, 7201):setVisible(true)
    UI_GetUILabel(UI_GetUILabel(widget, 7201), 1001):setText("Lv."..info.iLevelID)
    
    --初始化英魂殿英雄
    LogAndEnternal_initEnternalHero(info.szManorHeroSoulBox)
    --LogAndEnternal_initEnternalHero()
    LogAndEnternal_showLayer(info)
    --规则
    local function Rule(send,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            local tag = tolua.cast(send,"Button"):getTag()
            --显示规则
            LogAndEnternal_TerritoryEnternalRole("EnternalRole",6)
        end
    end
    --规则
    UI_GetUIButton(widget, 4556407):addTouchEventListener(Rule)
    UI_GetUIImageView(widget, 4556350):setTouchEnabled(true)
end
--根据不同的状态显示不同的界面
function LogAndEnternal_showLayer(info)
    --如果是自己的领地
        --如果没有开始祭炼
        if info.bIsProduct == false then
            LogAndEnternal_enternalNotStart(info)
            --LogAndEnternal_enternalNotStart()
            LogAndEnternal_Enternal(true,false,false)
        else
        --如果开始祭炼
            if info.iProductReaminSecond >0 then --如果没有结束
                LogAndEnternal_enternalStarted(info)
                --LogAndEnternal_enternalStarted()
                LogAndEnternal_Enternal(false,true,false)
            else--如果祭炼结束
                LogAndEnternal_Get()
                LogAndEnternal_Enternal(false,false,true)
            end
        end
        --升级和加速
        LogAndEnternal_levelUpAddSpeedUp(info)
    --如果不是自己的领地
        --如果开始祭炼
            --LogAndEnternal_enternalStartedOthers()
            --LogAndEnternal_Enternal(false,false,true)
        --如果没有开始祭炼
            --LogAndEnternal_Enternal(false,false,false)
end
--英魂殿-自己- 未开始祭炼
function LogAndEnternal_enternalNotStart(info)
    local widget = UI_GetBaseWidgetByName("TerritoryEnternal")
    local MineNoteStart = UI_GetUILayout(widget, 4556421)
        --自己-刷新
    local function refreshEnternal (send,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            --发送刷新信息           
            if g_LogAndEnternal_EnternalOrBlacksmith == "Enternal" then
                LogAndEternal_refreshEnternal()
            elseif g_LogAndEnternal_EnternalOrBlacksmith == "Blacksmith" then
                LogAndEternal_refreshBlacksmith()
            end
        end
    end
    --自己-开始祭炼
    local function refining(send,eventType)
        if eventType == TOUCH_EVENT_ENDED then
			if (g_isGuide) then
				Guide_GoNext();
			end
            local tag = tolua.cast(send,"Button"):getTag()
            --开始祭炼
            if g_LogAndEnternal_EnternalOrBlacksmith == "Enternal" then
                LogAndEternal_refiningEnternal()
            elseif g_LogAndEnternal_EnternalOrBlacksmith == "Blacksmith" then
                LogAndEternal_refiningBlacksmith()
            end
        end
    end
    --刷新
    UI_GetUIButton(MineNoteStart, 4556431):addTouchEventListener(refreshEnternal)
    --开始祭炼
    UI_GetUIButton(MineNoteStart, 4556434):addTouchEventListener(refining)
    UI_GetUIButton(MineNoteStart, 4556434):setTitleText(FormatString("Start"..g_LogAndEnternal_EnternalOrBlacksmith))
    --消耗
    if info.iRemianRefreshTimes<=0 then
        UI_GetUILabel(MineNoteStart, 4556423):setText(info.iRefreshCost)
        --UI_GetUILabel(MineNoteStart, 4556423):setText("10")

        UI_GetUIImageView(MineNoteStart, 4556424):setVisible(true)
    else
        UI_GetUIImageView(MineNoteStart, 4556424):setVisible(false)
        UI_GetUILabel(MineNoteStart, 4556423):setText(info.iRemianRefreshTimes.."/"..info.iTotalRemainRefreshTimes)
    end

    
    --清除在祭炼阶段的控件
    for i=1,4,1 do
        ClearAndAddChild(widget,nil,10,1200+i)
    end
end
--英魂殿-自己-开始祭炼
function LogAndEnternal_enternalStarted(info)
    local allObjects = LogAndEnternal_getAllObjects(info)
    local widget = UI_GetBaseWidgetByName("TerritoryEnternal")
    local MineStarted = UI_GetUILayout(widget, 4556436)
    --无懈可击的英雄的ID
    local iBoxIndex = nil
    --无懈可击确定
    local function determine()
        --无懈可击
        if g_LogAndEnternal_EnternalOrBlacksmith == "Enternal" then
            LogAndEternal_WatertightEnternal(iBoxIndex)
        elseif g_LogAndEnternal_EnternalOrBlacksmith == "Blacksmith" then
            LogAndEternal_WatertightBlacksmith(iBoxIndex)
        end
    end
    --自己-无懈可击
    local function Watertight(send,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            local tag = tolua.cast(send,"Button"):getTag()-1200
            local data = nil
            local message = ""
            if g_LogAndEnternal_EnternalOrBlacksmith == "Enternal" then
                data = GetGameData(DataFileHeroBorn,allObjects[tag].detail.iHeroSoulId,"stHeroBornData")
                message = FormatString("WatertightCost","20",FormatString("Enternal_E",data.m_name,allObjects[tag].detail.iHeroSoulCount))
            elseif g_LogAndEnternal_EnternalOrBlacksmith == "Blacksmith" then
                data = GetGameData(DataFileItem,allObjects[tag].itemDetail.iItemId,"stItemData")
                message = FormatString("WatertightCost","20",data.m_name.."x"..allObjects[tag].itemDetail.iItemCount)
            end
            iBoxIndex = LogAndEnternal_getID(allObjects,tag)
            local tab = {}
            tab.info = message
            tab.msgType = EMessageType_LeftRight
            tab.leftFun = determine
            Messagebox_Create(tab)
        end
    end

    for i=1,4,1 do
        if LogAndEnternal_getDetail(allObjects,i) ~= nil then -- 如果此处有英雄
        print("i==============================================="..i)
            if LogAndEnternal_IsWuXieKeJi(allObjects,i) == false and
                LogAndEnternal_BeLoot(allObjects,i) == false  then --如果没有被无懈可击,没有被掠夺
                local enternalBt = UI_GetUIButton(widget, 2202):clone()
                enternalBt:setVisible(true)

                enternalBt:setPosition(ccp(185+(i-1)*197,180))               

                enternalBt:addTouchEventListener(Watertight)
                ClearAndAddChild(widget,enternalBt,10,1200+i)
                --enternalBt:setTitleText(FormatString("Watertight"))

            elseif LogAndEnternal_BeLoot(allObjects,i) == true then--被掠夺
                local protect = UI_GetUIImageView(widget, 4600498):clone()

                protect:setPosition(ccp(185+(i-1)*197,190))               

                ClearAndAddChild(widget,protect,10,1200+i)
                protect:setVisible(true)
                --铁匠铺被掠夺过后，装备会被变成灰色
                if g_LogAndEnternal_EnternalOrBlacksmith == "Blacksmith" then
                    UI_GetUIImageView(widget:getChildByTag(1300+i):getChildByTag(4579504):getChildByTag(1234), 1):addColorGray()
                end
            elseif LogAndEnternal_IsWuXieKeJi(allObjects,i) == true then--被保护
                local protect = UI_GetUIImageView(widget, 4571978):clone()
                protect:setPosition(ccp(185+(i-1)*197,190))               
                ClearAndAddChild(widget,protect,10,1200+i)
                protect:setVisible(true)
            end  
            --加载祭炼特效
            if g_LogAndEnternal_EnternalOrBlacksmith == "Enternal" then
                local hero = UI_GetUIImageView(widget, 1200+i)
                if hero:getChildByTag(1007) then
                    hero:removeChildByTag(1007)
                end
                local layout = Layout:create();
                local godanimalanimatrue = GetUIArmature("jilian")
                print("hero:getSize().height/2======================"..hero:getSize().height/2)
                godanimalanimatrue:getAnimation():playWithIndex(0) 
                godanimalanimatrue:setPosition(ccp(0,95))
                layout:setAnchorPoint(ccp(0.5,0))
                layout:addNode(godanimalanimatrue)
                hero:addChild(layout,5,1007)
            end
        end   
    end
    --完成时间
    UI_GetUILabel(MineStarted, 4556455):setTimer(info.iProductReaminSecond,TimeFormatType_HourMinSec)
    if g_LogAndEnternal_EnternalOrBlacksmith == "Enternal" then      
        UI_GetUILabel(MineStarted, 4556455):addEventListenerTimer(LogAndEternal_getEnternal)
    elseif g_LogAndEnternal_EnternalOrBlacksmith == "Blacksmith" then  
        UI_GetUILabel(MineStarted, 4556455):addEventListenerTimer(LogAndEternal_getBlacksmith)
    end
    UI_GetUILabel(MineStarted, 4556452):setText(FormatString(g_LogAndEnternal_EnternalOrBlacksmith.."_Text"))
    --
end
--武魂殿-别人-开始祭炼
function LogAndEnternal_enternalStartedOthers()
    local widget = UI_GetBaseWidgetByName("TerritoryEnternal")
    local others = UI_GetUILayout(widget, 4556459)
    --掠夺
    local function plunder(send,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            local tag = tolua.cast(send,"Button"):getTag()-1200-1
            --掠夺
            LogAndEternal_plunder()
        end
    end

    for i=1,4,1 do
        --移除原有的掠夺选项
        -- if  then --如果被保护了
            --local image = UI_GetUIImageView(widget, 4571978)
            --image:setPosition(ccp(250+(i-1)*230,100))
            --ClearAndAddChild(widget,enternalBt,10,1300+i)
            --image:setVisible(true)
        --else--如果没有被保护
            local robbed = UI_GetUIImageView(widget, 1112):clone()
            robbed:setTag(2201+i)
            robbed:setVisible(true)
            --掠夺消耗
            UI_GetUILabel(robbed, 4556475):setText("6")
            --掠夺按钮
            UI_GetUIButton(robbed, 4556476):addTouchEventListener(plunder)

            robbed:setPosition(ccp(141+226*(i-1),135))
            ClearAndAddChild(widget,robbed,10,1200+i)
        --end
    end
end
--初始化英魂殿英雄
function LogAndEnternal_initEnternalHero(szManorHeroSoulBox)
    local widget = UI_GetBaseWidgetByName("TerritoryEnternal")
    for i=1,4,1 do
        local hero
        if szManorHeroSoulBox[i] ~= nil and szManorHeroSoulBox[i].bIsOpen then --如果此位置已经开启
            hero = UI_GetUIImageView(widget, 1399):clone()
            if szManorHeroSoulBox[i].detail.iHeroSoulId ~= 0 then --如果此位置有英雄
                local heroLocalData = GetGameData(DataFileHeroBorn,szManorHeroSoulBox[i].detail.iHeroSoulId,"stHeroBornData")
                Formation_AddAnimation(hero,heroLocalData.m_animName,199)
                UI_GetUILabel(hero, 1599):setText(
                    FormatString("Enternal_E",
                    heroLocalData.m_name,szManorHeroSoulBox[i].detail.iHeroSoulCount
                    -szManorHeroSoulBox[i].detail.iHeroHaveSoulLootCount))           
            else
                UI_GetUILabel(hero, 1599):setText("")        
            end
        else--如果此位置未开启
            
            hero = UI_GetUILayout(widget, 5002):clone()   
            UI_GetUILabel(hero, 4001):setText(g_Eternal[i])   
            --lock:setVisible(true)
            --lock:setPosition(ccp(0,0))
            --UI_GetUILabel(hero, 1599):setText(FormatString("EnternalRole_lock",g_Eternal[i]))
            --UI_GetUILabel(hero, 1599):setColor(ccc3(200,20,20))
            --hero:addChild(lock)
        end
        hero:setPosition(ccp(185+(i-1)*197,278))
        hero:setVisible(true)
        ClearAndAddChild(widget,hero,10,1300+i)
    end
end
--收取
function LogAndEnternal_Get()
    local widget = UI_GetBaseWidgetByName("TerritoryEnternal") 
    --清除“无懈可击”“已保护”“已被掠夺”等提示
    for i=1,4,1 do
        ClearAndAddChild(widget,nil,10,1200+i)
    end
    --收取
    local layout= UI_GetUILayout(widget, 4579619)

    --UI_GetUILabel(layout, 4579620):setText(FormatString(g_LogAndEnternal_EnternalOrBlacksmith.."_OK"))

    local function get(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            if g_LogAndEnternal_EnternalOrBlacksmith == "Enternal" then
                LogAndEternal_getEnternalItem()				
            elseif g_LogAndEnternal_EnternalOrBlacksmith == "Blacksmith" then
                LogAndEternal_getBlacksmithItem()
            end
			
			if g_isGuide then
				Guide_GoNext();
			end
        end
    end
    --一键收取
    UI_GetUIButton(layout, 4579623):addTouchEventListener(get)
end
--英魂殿：显示或者屏蔽某些层
function LogAndEnternal_Enternal(isV1,isV2,isV3)
    local widget = UI_GetBaseWidgetByName("TerritoryEnternal")
    UI_GetUILayout(widget, 4556421):setVisible(isV1)
    UI_GetUILayout(widget, 4556436):setVisible(isV2)
    UI_GetUILayout(widget, 4579619):setVisible(isV3)
end

-----------------------------------------铁匠铺-------------------------
--外部用于初始化铁匠铺界面的接口
function LogAndEnternal_createBlacksmith()
    g_LogAndEnternal_EnternalOrBlacksmith = "Blacksmith"
    LogAndEternal_getBlacksmith()
    
    --LogAndEnternal_createBlacksmithLayer()
end
--从网络获取的数据进行初始化铁匠铺界面
function LogAndEnternal_createBlacksmithLayer(pkg)
    EndWaiting()
    local widget = UI_GetBaseWidgetByName("TerritoryEnternal")
    
    print("---------------------3----------------------")
    if widget == nil then
        widget = UI_CreateBaseWidgetByFileName("TerritoryEnternal.json")
    end
    local info = GameServer_pb.CMD_MANOR_TIEJIANGPU_QUERY_SC()
    info:ParseFromString(pkg)
    Log("info===="..tostring(info))

    UI_GetUIButton(widget, 4556493):addTouchEventListener(LogAndEnternal_Exit)
    UI_GetUILayout(widget, 4619553):addTouchEventListener(PublicCallBackWithNothingToDo)
    --"铁匠铺"  
    UI_GetUIImageView(UI_GetUIImageView(widget, 4591787), 1001):loadTexture("titleFont/titleFont_tiejiangpu.png")
    UI_GetUILabel(widget, 7201):setVisible(true)
    UI_GetUILabel(UI_GetUILabel(widget, 7201), 1001):setText("Lv."..info.iLevelID)
    --初始化铁匠铺装备
    LogAndEnternal_initBlacksmith(info)
    
    LogAndEnternal_showLayer(info)
    --规则
    local function Rule(send,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            local tag = tolua.cast(send,"Button"):getTag()
            --显示规则
            LogAndEnternal_TerritoryEnternalRole("Blacksmith",6)
        end
    end
    --规则
    UI_GetUIButton(widget, 4556407):addTouchEventListener(Rule)
    UI_GetUIImageView(widget, 4556350):setTouchEnabled(true)
end
--初始化铁匠铺装备
function LogAndEnternal_initBlacksmith(info)
    local widget = UI_GetBaseWidgetByName("TerritoryEnternal")
    for i=1,4,1 do

        local equip;
        if info.szManorTieJiangPuItemBox[i]~=nil and info.szManorTieJiangPuItemBox[i].bIsOpen == true then--此位置已解锁
            equip= UI_GetUIImageView(widget, 4579495):clone()       
            local itemDetail = info.szManorTieJiangPuItemBox[i].itemDetail
            if itemDetail.iItemId ~= 0 then --此位置有装备  
                   
                local itemData = GetGameData(DataFileItem,itemDetail.iItemId,"stItemData")
                --名字
                local name = itemData.m_name
                UI_GetUILabel(equip, 4579500):setText(name)
		UI_GetUILabel(equip, 4579500):setColor(Util_GetColor(itemData.m_quality))
                if #name>18 then
                    UI_GetUILabel(equip, 4579500):setFontSize(20)
                elseif #name>24 then
                    UI_GetUILabel(equip, 4579500):setFontSize(18)
                elseif #name>27 then
                    UI_GetUILabel(equip, 4579500):setFontSize(16)
                else
                    UI_GetUILabel(equip, 4579500):setFontSize(24)
                end
        
                --属性
                print("itemData.m_attinfo========"..itemData.m_attinfo)
                if itemData.m_attinfo ~= "" and itemData.m_attinfo ~= nil then
                    local att = itemData.m_attinfo
                    local text = Label:create()
                    text:setText(att)
                    text:setFontName("Fonts/FZZhunYuan-M02S.ttf")
                    text:setFontSize(18)
                    UI_GetUIListView(equip, 4656250):pushBackCustomItem(text)
                else
                    local listView = UI_GetUIListView(equip, 4656250);
                    local label = CompLabel:GetDefaultCompLabel(FormatString("Blacksmith_DESC", itemData.m_desc), listView:getSize().width);
                    listView:pushBackCustomItem(label)
                end
                
                UI_GetUIImageView(equip, 4579504):addChild(
                    UI_ItemIconFrame(
                        UI_GetItemIcon(itemData.m_icon,
                            itemDetail.iItemCount-itemDetail.iItemHaveLootCount),
                        itemData.m_quality),1234,1234)
            else
                UI_GetUILabel(equip, 4579500):setText("")
                --UI_GetUILabel(equip, 4579505):setText("")
            end
        else--未解锁
            equip= UI_GetUILayout(widget, 5101):clone();
            UI_GetUILabel(equip, 4001):setText(g_Blacksmith[i])
            --UI_GetUILabel(equip, 4579500):setText("")

            --local text = Label:create()
            --text:setFontSize(20)
            --text:setColor(ccc3(200,20,20))
            --text:setPosition(ccp(0,50))
            --UI_GetUIImageView(equip, 4579502):addChild(text,3,3)

            --text:setText(FormatString("Blacksmith_lock",g_Blacksmith[i]))
            --text:setPosition(ccp(text:getPositionX(),text:getPositionY()-30))

            --local lock = ImageView:create()
            --lock:loadTexture("lingdi/bg_004.png")

            --local item = UI_GetItemIcon(1001,0)
            --item:addColorGray()

            --local frame = UI_ItemIconFrame(item,2)
            --frame:addChild(lock,10)

            --UI_GetUIImageView(equip, 4579504):addChild(frame)
        end
        equip:setPosition(ccp(185+(i-1)*197,340))
        equip:setVisible(true)
        ClearAndAddChild(widget,equip,10,1300+i)
    end
end
-----------------------------------------可共用的函数--------------------
--更新组件
function ClearAndAddChild(widget,child,zOrder,tag)
    if widget:getChildByTag(tag) then
        widget:removeChildByTag(tag)
    end
    if child ~= nil then
        widget:addChild(child,zOrder,tag)
    end
end
--加速和升级
function LogAndEnternal_levelUpAddSpeedUp(info)
    local widget = UI_GetBaseWidgetByName("TerritoryEnternal")
    --升级
    local function LevelUp(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            LogAndEnternal_LevelUp(info.iLevelID)
        end
    end

    --加速
    local function SpeedUp(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            if g_LogAndEnternal_EnternalOrBlacksmith == "Enternal" then
                LogAndEternal_SpeedUpEnternal()
            elseif g_LogAndEnternal_EnternalOrBlacksmith == "Blacksmith" then
                LogAndEternal_SpeedUpBlacksmith()
            end
        end
    end

    local levelup = UI_GetUILayout(widget, 4571947)

    if info.bIsBulid == false then
        --升级
        UI_GetUIButton(levelup, 201):addTouchEventListener(LevelUp)
        UI_GetUIButton(levelup, 202):setVisible(false)
        UI_GetUIButton(levelup, 202):setTouchEnabled(false)
        UI_GetUIButton(levelup, 201):setTouchEnabled(true)
        UI_GetUIButton(levelup, 201):setVisible(true)
    else
        --加速
        UI_GetUIButton(levelup, 202):addTouchEventListener(SpeedUp)
        --时间
        UI_GetUILabel(UI_GetUIButton(levelup, 202), 1001):setTimer(info.iBulidRemainSecond,TimeFormatType_HourMinSec)
        UI_GetUILabel(UI_GetUIButton(levelup, 202), 1002):setText(FormatString("LevelUpA"))
        UI_GetUILabel(UI_GetUIButton(levelup, 202), 1001):addEventListenerTimer(LogAndEternal_getEnternal)

        UI_GetUILabel(widget, 7201):setVisible(false)
        UI_GetUIButton(levelup, 201):setVisible(false)
        UI_GetUIButton(levelup, 201):setTouchEnabled(false)
        UI_GetUIButton(levelup, 202):setTouchEnabled(true)
        UI_GetUIButton(levelup, 202):setVisible(true)
    end
end
--升级界面
function LogAndEnternal_LevelUp(iLevel)
    --名字和等级
    local name = nil
    if g_LogAndEnternal_EnternalOrBlacksmith == "Enternal" then 
        name = FormatString("EnternalRole_name")
    else
        name = FormatString(g_LogAndEnternal_EnternalOrBlacksmith.."_name")
    end
    local level = nil
    local levelNext = nil
    if g_LogAndEnternal_EnternalOrBlacksmith == "Enternal" then
        level = GetGameData(DataFileManorWuHunDian,iLevel,"stManorWuHunDianData")
        levelNext = GetGameData(DataFileManorWuHunDian,(iLevel+1),"stManorWuHunDianData")
    elseif g_LogAndEnternal_EnternalOrBlacksmith == "Blacksmith" then
        level = GetGameData(DataFileManorTieJiangPu,iLevel,"stManorTieJiangPuData")
        levelNext = GetGameData(DataFileManorTieJiangPu,(iLevel+1),"stManorTieJiangPuData")
    end

    if levelNext == nil then--如果没有下一等级
        createPromptBoxlayout(FormatString("EnternalAndBlacksmithLevelMax"))
        return
    end
    local widget = UI_CreateBaseWidgetByFileName("TerritoryEnternalLevelUp.json")

    
    name = name.."Lv."..(iLevel+1)
    UI_GetUILabel(widget, 4579630):setText(name)
    --XX数量
    UI_GetUILabel(widget, 4579665):setText(FormatString(g_LogAndEnternal_EnternalOrBlacksmith.."_num"))
    --XX时间
    UI_GetUILabel(widget, 4579664):setText(FormatString(g_LogAndEnternal_EnternalOrBlacksmith.."_timestr"))
    --原本消耗时间
    UI_GetUILabel(widget, 4579667):setTimer(level.m_timeProduct,TimeFormatType_HourMinSec)
    UI_GetUILabel(widget, 4579667):stopTimer()
    --升级减少时间
    UI_GetUILabel(widget, 4579669):setTimer(levelNext.m_timeProduct,TimeFormatType_HourMinSec)
    UI_GetUILabel(widget, 4579669):stopTimer()
    --升级所需时间
    UI_GetUILabel(widget, 8001):setText(LogAndEnternal_IntToTime(level.m_timeLevelUp))
    --原本的数量
    UI_GetUILabel(widget, 4579666):setText(level.m_boxNum)
    --升级增加的数量
    UI_GetUILabel(widget, 4579670):setText(levelNext.m_boxNum)
    --费用
    UI_GetUILabel(widget, 4579668):setText(level.m_cost)
    local function levelUp(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then 
            if g_LogAndEnternal_EnternalOrBlacksmith == "Enternal" then
                LogAndEternal_LevelUpEnternal()
            elseif g_LogAndEnternal_EnternalOrBlacksmith == "Blacksmith" then
                LogAndEternal_LevelUpBlacksmith()
            end
            UI_ClickCloseCurBaseWidget(sender, eventType)
        end
    end
    UI_GetUIButton(widget, 4579659):addTouchEventListener(levelUp)
end
--显示规则
function LogAndEnternal_TerritoryEnternalRole(name,num)
    local widget = UI_CreateBaseWidgetByFileName("TerritoryEnternalRule.json")
    --标题
    UI_GetUILabel(widget,4556518):setText(FormatString(name.."_Rule"))
    --说明
    --UI_GetUILabel(widget,4556522):setText(FormatString(name))
    --local message = UI_GetUILabel(widget, 4556525)
    
    --local rule = ""
    --for i=1,num,1 do
    --    if rule~="" then
    --        rule = rule.."\n"
    --    end
    --    rule = rule..FormatString(name.."_"..i)
    --end
    --message:setText(rule)

    local listView = UI_GetUIListView(widget, 4670692)
    for i=1,num,1 do 
        local infolab = CompLabel:GetDefaultCompLabel(FormatString(name.."_"..i),listView:getSize().width)
        local tempLayout= UI_GetUILayout(widget, 1122):clone()
        --infolab:setColor(ccc3(164,240,25))
        listView:pushBackCustomItem(infolab)
        listView:pushBackCustomItem(tempLayout)
    end
end
--获取正在祭炼或者打造的所有对象
function LogAndEnternal_getAllObjects(info)
    if g_LogAndEnternal_EnternalOrBlacksmith == "Enternal" then
        return info.szManorHeroSoulBox
    elseif g_LogAndEnternal_EnternalOrBlacksmith == "Blacksmith" then
        return info.szManorTieJiangPuItemBox
    else 
        print("Type Error")
        return nil
    end
end
--获取对象详情
function LogAndEnternal_getDetail(allObjects,index)
    if g_LogAndEnternal_EnternalOrBlacksmith == "Enternal" then
        if allObjects[index] ~= nil and allObjects[index].detail.iHeroSoulId ~= 0 then
            return allObjects[index].detail
        else
            return nil
        end
    else
        if allObjects[index] ~=nil and allObjects[index].itemDetail.iItemId ~= 0 then
            return allObjects[index].itemDetail
        else 
            return nil
        end
    end
end
--此对象ID
function LogAndEnternal_getID(allObjects,index)
    if g_LogAndEnternal_EnternalOrBlacksmith == "Enternal" then
        return allObjects[index].iBoxIndex
    elseif g_LogAndEnternal_EnternalOrBlacksmith == "Blacksmith" then
        return allObjects[index].iBoxIndex
    else 
        return false
    end
end
--此对象是否被无懈可击
function LogAndEnternal_IsWuXieKeJi(allObjects,index)
    if g_LogAndEnternal_EnternalOrBlacksmith == "Enternal" then
        return allObjects[index].detail.bHeroIsWuXieKeJi
    elseif g_LogAndEnternal_EnternalOrBlacksmith == "Blacksmith" then
        return allObjects[index].itemDetail.bItemIsWuXieKeJi
    else 
        return false
    end
end
--此对象是否被掠夺
function LogAndEnternal_BeLoot(allObjects,index)
    if g_LogAndEnternal_EnternalOrBlacksmith == "Enternal" then
        if allObjects[index].detail.iHeroHaveSoulLootCount == allObjects[index].detail.iHeroSoulCount then
            return true
        else
            return false
        end
    elseif g_LogAndEnternal_EnternalOrBlacksmith == "Blacksmith" then
        if allObjects[index].itemDetail.iItemCount == allObjects[index].itemDetail.iItemHaveLootCount then
            return true
        else
            return false
        end
    else 
        return false
    end
end

function LogAndEnternal_IntToTime(data)
    local function intToStr(int)
        if int<10 then
            return "0"..int
        end
        return ""..int
    end
    if data <=0 then
        return "0:00:00"
    end
    if data <60 then
        return "0:00:"..intToStr(data)
    end 
    if data <3600 then
        
        return "0:"..intToStr(ToInt(data/60))..":"..intToStr(data%60)
    end
    if data>=3600 then
        return intToStr(ToInt(data/3600))..":"..intToStr(ToInt((data%3600)/60))..":"..intToStr(data/60)
    end
end

---------------------------------------------------------------------------------------------
--网络消息：获取武魂殿数据
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_MANOR_WUHUNDIAN_QUERY, "LogAndEnternal_createEnternalLayer" )
--网络消息：获取铁匠铺数据
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_MANOR_TIEJIANGPU_QUERY, "LogAndEnternal_createBlacksmithLayer" )
--网络消息：获取日志数据
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_MANOR_QUERY_LOG, "LogAndEnternal_createLogLayer" )
--------------------------------------------------------------------------------
--网络消息：武魂殿升级限制
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_MANOR_WUHUNDIAN_LEVELUP, "CMD_MANOR_WUHUNDIAN_LEVELUP_F" )
--网络消息：铁匠铺升级限制
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_MANOR_TIEJIANGPU_LEVELUP, "CMD_MANOR_TIEJIANGPU_LEVELUP_F" )
--网络消息：铁匠铺升级限制
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_MANOR_RES_COMMON_LEVELUP, "CMD_MANOR_RES_COMMON_LEVELUP_F" )
function CMD_MANOR_WUHUNDIAN_LEVELUP_F(pkg)
    EndWaiting()
    local info = GameServer_pb.CMD_MANOR_WUHUNDIAN_LEVELUP_SC()
    info:ParseFromString(pkg)
    Log("info===="..tostring(info))

    createPromptBoxlayout(FormatString("NeedLevel",info.iNeedActorLevel))
end 
function CMD_MANOR_TIEJIANGPU_LEVELUP_F(pkg)
    EndWaiting()
    local info = GameServer_pb.CMD_MANOR_WUHUNDIAN_LEVELUP_SC()
    info:ParseFromString(pkg)
    Log("info===="..tostring(info))

    createPromptBoxlayout(FormatString("NeedLevel",info.iNeedActorLevel))
end 

function CMD_MANOR_RES_COMMON_LEVELUP_F(pkg)
    EndWaiting()
    local info = GameServer_pb.CMD_MANOR_WUHUNDIAN_LEVELUP_SC()
    info:ParseFromString(pkg)
    Log("info===="..tostring(info))

    createPromptBoxlayout(FormatString("NeedLevel",info.iNeedActorLevel))
end 
