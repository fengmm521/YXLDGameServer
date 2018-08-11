--获取抽取武将的消耗数据
function Shop_getDataLuckyDraw()
    Packet_Cmd(GameServer_pb.CMD_SHOPSYSTEM_RAND_QUERY);
    ShowWaiting()
end
--获取兑换列表
function Shop_getDataExchagne()
    Packet_Cmd(GameServer_pb.CMD_SHOPSYSTEM_HONORCONVERT_QUERY);
    ShowWaiting()
end
--兑换
function Shop_BuyItem(iGoodId)
    local tab = GameServer_pb.CMD_SHOPSYSTEM_HONORCONVERT_CONVERT_CS();
    tab.iIndex = iGoodId
	Packet_Full(GameServer_pb.CMD_SHOPSYSTEM_HONORCONVERT_CONVERT, tab);

    ShowWaiting()
end
--获取新的兑换列表
function Shop_RefreshList()
    Packet_Cmd(GameServer_pb.CMD_SHOPSYSTEM_HONORCONVERT_REFRESH);
    ShowWaiting()
end
--抽取
function Shop_RandItemOrHero(RandmType)
    if RandmType == 5 or RandmType == 6 then
        if GetLocalPlayer():GetInt(EPlayer_VipLevel)<10 then
            createPromptBoxlayout(FormatString("Shop_NoVIP"))
            return
        end
    end
    local tab = GameServer_pb.CMD_SHOPSYSTEM_RANDITEMORHERO_CS();
    tab.enType = RandmType
	Packet_Full(GameServer_pb.CMD_SHOPSYSTEM_RANDITEMORHERO, tab);

    ShowWaiting()
end
--商城查询
function Shop_Query()
    Packet_Cmd(GameServer_pb.CMD_SHOPSYSTEM_QUERY);
    ShowWaiting()
end
--------------------------------------------------------------------------
function Shop_CreateExchange()
    Shop_InitLayer(true)
    Shop_getDataExchagne()
end
function Shop_Create()
    Shop_Query()
end
function Shop_InitLayer(isDisappear,info)
    local widget = UI_GetBaseWidgetByName("Shop")
    if( widget == nil )then
        widget = UI_CreateBaseWidgetByFileName("Shop.json")
    end
    if isDisappear == true then
        widget:setVisible(false)
    end
    local function closeLayer(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            UI_CloseCurBaseWidget(EUIOpenAction_None)
        end
    end
    UI_GetUIButton(widget, 4):addTouchEventListener(closeLayer)
    
    if info ~= nil then
        Shop_InitLuckyDraw(info)
        Shop_ChangeLayer("LuckyDraw")   
    end


    ShopRefreshData(4606874)
end
function ShopRefreshData(id)
    local widget = UI_GetBaseWidgetByName("Shop")

    UI_GetUILayout(widget, 4606874):setVisible(id == 4606874)
    UI_GetUILayout(widget, 4776444):setVisible(id == 4776444)

    local data = UI_GetUILayout(widget, id)
    local playerData = GetLocalPlayer()    

    local function dealWithData(num)
        if num>100000000 then
            return FormatString("TenThousand",num/10000) 
        end
        return num 
    end

    local function setTextFunc(index,value)
        if UI_GetUILabelBMFont(data, index) then
            UI_GetUILabelBMFont(data, index):setText(value)
        end
    end
    --铜币   
    setTextFunc(9,dealWithData(playerData:GetInt(EPlayer_Silver)))
     --元宝
    setTextFunc(6,dealWithData(playerData:GetInt(EPlayer_Gold)))
    --荣誉
    setTextFunc(7,dealWithData(playerData:GetInt(EPlayer_Honor)))
    --将星令
    setTextFunc(8,dealWithData(GetLocalPlayer():GetPlayerBag():GetItemCountByItemId(19998)))
end
----------------------------------------抽取-----------------------------------------------
--初始化召唤界面
function Shop_InitLuckyDraw(info)
 
    local widget = UI_GetBaseWidgetByName("Shop")
    if widget == nil then
        return
    end
    
    local LuckyDraw = UI_GetUIImageView(widget, 11)
    --抽奖的图片
    local tokenImage = UI_GetUIImageView(LuckyDraw:getChildByTag(1101):getChildByTag(1101), 1101)
    local diamondImage = UI_GetUIImageView(LuckyDraw:getChildByTag(1102):getChildByTag(1101), 1101) 
    local diamondImageHero = UI_GetUIImageView(LuckyDraw:getChildByTag(1103):getChildByTag(1101), 1101)
    if GetLocalPlayer():GetInt(EPlayer_VipLevel)>=0 then
        LuckyDraw:getChildByTag(1103):setVisible(true)
        local herotag = {1401,1402,1403,1404}
        local heroCardLayout = UI_GetUILayout(UI_GetUILayout(UI_GetUIImageView(LuckyDraw,1103),1101),1101)
        for i = 1,#herotag do 
            local data = GetGameData(DataFileHeroBorn, g_HighVip[i], "stHeroBornData")
            --icon
            UI_GetHeroIncoByClone(data.m_icon,UI_GetUILayout(heroCardLayout,herotag[i]),1)
        end    
    else
        LuckyDraw:getChildByTag(1101):setPosition(ccp(220,254))
        LuckyDraw:getChildByTag(1102):setPosition(ccp(620,254))
        LuckyDraw:getChildByTag(1103):setVisible(false)
    end

    --获得修为
    UI_GetUILabel(tokenImage, 2101):setText(FormatString("Shop_addxiuwei",g_ShopCost[5]))
    UI_GetUILabel(diamondImage, 2101):setText(FormatString("Shop_addxiuwei",g_ShopCost[6]))
    UI_GetUILabel(diamondImageHero, 2101):setText(FormatString("Shop_addxiuwei",g_ShopCost[9]))
    UI_GetUILabel(tokenImage, 2101):enableStroke()
    UI_GetUILabel(diamondImage, 2101):enableStroke()
    UI_GetUILabel(diamondImageHero, 2101):enableStroke()
    UI_GetUILabel(tokenImage, 1124821340):enableStroke()
    UI_GetUILabel(diamondImage, 1124800263):enableStroke()
    UI_GetUILabel(diamondImageHero, 1124828096):enableStroke()
    UI_GetUILabel(tokenImage, 1124800186):enableStroke()
    UI_GetUILabel(diamondImage, 1124800253):enableStroke()
    UI_GetUILabel(diamondImageHero, 1124828088):enableStroke()

    --查看按钮
    local tokenButtonChick =  UI_GetUIButton(tokenImage, 2104)
    tokenButtonChick:setName("tokenButtonChick")
    local diamondButtonChilk = UI_GetUIButton(diamondImage, 2104)
    diamondButtonChilk:setName("diamondButtonChilk")
    local diamondButtonHeroChilk = UI_GetUIButton(diamondImageHero, 2104)
    diamondButtonHeroChilk:setName("diamondButtonHeroChilk")
    --上滑按钮
    local tokenButtonChickUp =  UI_GetUILayout(tokenImage, 9001)
    tokenButtonChickUp:setName("tokenButtonChickUp")
    local diamondButtonChilkUp = UI_GetUILayout(diamondImage, 9001)
    diamondButtonChilkUp:setName("diamondButtonChilkUp")
    local diamondButtonHeroChilkUp = UI_GetUILayout(diamondImageHero, 9001)
    diamondButtonHeroChilkUp:setName("diamondButtonHeroChilkUp")

    Shop_fade(tokenImage:getChildByTag(2105))
    Shop_fade(diamondImage:getChildByTag(2105))
    Shop_fade(diamondImageHero:getChildByTag(2105))
    --单次召唤 
    local tokenCallOnce = UI_GetUIButton(tokenImage, 2108)
    if tokenCallOnce == nil then
        tokenCallOnce = UI_GetUIButton(tokenImage, 1)
    end
    tokenCallOnce:setTag(1)
    local diamondCallOnce = UI_GetUIButton(diamondImage, 2108)
    if diamondCallOnce == nil then
        diamondCallOnce = UI_GetUIButton(tokenImage, 3)
    end
    diamondCallOnce:setTag(3)
    local diamondHeroCallOnce = UI_GetUIButton(diamondImageHero, 2108)
    if diamondHeroCallOnce == nil then
        diamondHeroCallOnce = UI_GetUIButton(tokenImage, 5)
    end
    diamondHeroCallOnce:setTag(5)
    --十连抽
    local tokenCallTen = UI_GetUIButton(tokenImage, 2110)
    if tokenCallTen == nil then
        tokenCallTen = UI_GetUIButton(tokenImage, 2)
    end
    tokenCallTen:setTag(2)
    local diamondCallTen = UI_GetUIButton(diamondImage, 2110)
    if tokenCallTen == nil then
        tokenCallTen = UI_GetUIButton(tokenImage, 4)
    end
    diamondCallTen:setTag(4)
    local diamondHeroCallTen = UI_GetUIButton(diamondImageHero, 2110)
    if tokenCallTen == nil then
        tokenCallTen = UI_GetUIButton(tokenImage, 6)
    end
    diamondHeroCallTen:setTag(6)
    Shop_reset(info)
    --点击查看按钮过后的移动效果
    local function ImageMove(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            
            local actionArry = CCArray:create()
            local callback = CCMoveTo:create(0.3,ccp(0,380))
            actionArry:addObject(callback)
            local seqaction = CCSequence:create(actionArry)

            local actionArry1 = CCArray:create()
            local callback1 = CCMoveTo:create(0.3,ccp(0,-20))
            actionArry1:addObject(callback1)
            local seqaction1 = CCSequence:create(actionArry1)
            if tolua.cast(sender,"Button"):getName() == "tokenButtonChick" then
                tokenImage:runAction(seqaction)
            elseif tolua.cast(sender,"Button"):getName() == "diamondButtonChilk" then
                diamondImage:runAction(seqaction)
                if (g_isGuide) then
					Guide_GoNext();
				end
            elseif tolua.cast(sender,"Button"):getName() == "diamondButtonHeroChilk" then
                diamondImageHero:runAction(seqaction)
            elseif tolua.cast(sender,"Layout"):getName() == "tokenButtonChickUp" then
                tokenImage:runAction(seqaction1)
            elseif tolua.cast(sender,"Layout"):getName() == "diamondButtonChilkUp" then
                diamondImage:runAction(seqaction1)
            elseif tolua.cast(sender,"Layout"):getName() == "diamondButtonHeroChilkUp" then
                diamondImageHero:runAction(seqaction1)
            end
        end
    end
    --查看按钮
    tokenButtonChick:addTouchEventListener(ImageMove)
    diamondButtonChilk:addTouchEventListener(ImageMove)
    diamondButtonHeroChilk:addTouchEventListener(ImageMove)
    --上滑按钮
    tokenButtonChickUp:addTouchEventListener(ImageMove)
    diamondButtonChilkUp:addTouchEventListener(ImageMove)
    diamondButtonHeroChilkUp:addTouchEventListener(ImageMove)
    --抽取
    local function Random(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            local tag = tolua.cast(sender,"Button"):getTag()
            g_Shop_RandomType = tag
			print("tag ===================="..tag)
            Shop_RandItemOrHero(g_Shop_RandomType)
			-- if have guide ,hide the arrow
			if g_isGuide == true then
				g_arrowLayout:setVisible(false);
				Guide_GoNext();
			end
        end
    end
    --抽取
    tokenCallOnce:addTouchEventListener(Random)
    diamondCallOnce:addTouchEventListener(Random)
    diamondHeroCallOnce:addTouchEventListener(Random)
    tokenCallTen:addTouchEventListener(Random)
    diamondCallTen:addTouchEventListener(Random)
    diamondHeroCallTen:addTouchEventListener(Random)

    --加特效
    local function AddAni(layout11)
        if (layout11:getNodeByTag(3) == nil) then
            local layout = Layout:create();
            local godanimalanimatrue = GetUIArmature("chouherotishi")
            godanimalanimatrue:setPosition(ccp(0,0))
            godanimalanimatrue:getAnimation():playWithIndex(0) 
            layout:addNode(godanimalanimatrue)
            local x = UI_GetUIImageView(layout11, 4001):getPositionX();
            local y = UI_GetUIImageView(layout11, 4001):getPositionY();
            layout:setPosition(ccp(x,y))
            layout11:addChild(layout,0,3)
        print("222222222222222222222222222222")
        end
        print("11111111111111111111111111111111")
    end
    
    AddAni(diamondImage)
    AddAni(diamondImageHero)
end
--重新设置消耗以及冷却
function Shop_reset(info)
    local widget = UI_GetBaseWidgetByName("Shop")
    local LuckyDraw = UI_GetUIImageView(widget, 11)
    --抽奖的图片
    local tokenImage = UI_GetUIImageView(LuckyDraw:getChildByTag(1101):getChildByTag(1101), 1101)
    local diamondImage = UI_GetUIImageView(LuckyDraw:getChildByTag(1102):getChildByTag(1101), 1101)
    local diamondImageHero = UI_GetUIImageView(LuckyDraw:getChildByTag(1103):getChildByTag(1101), 1101)
    
    --单次消耗
    local tokenOnceCostUp = UI_GetUILabel(tokenImage, 2103)
    local diamondOnceCostUp = UI_GetUILabel(diamondImage, 2103)
    local diamondOnceHeroCostUp = UI_GetUILabel(diamondImageHero, 2103)
    --单次消耗
    local tokenOnceCost = UI_GetUILabel(tokenImage, 2107)
    local diamondOnceCost = UI_GetUILabel(diamondImage, 2107)
    local diamondOnceHeroCost = UI_GetUILabel(diamondImageHero, 2107)
    --十连消耗
    local tokenTenCost = UI_GetUILabel(tokenImage, 2109)
    local diamondTenCost = UI_GetUILabel(diamondImage, 2109)
    local diamondTenHeroCost = UI_GetUILabel(diamondImageHero, 2109)

    local function setVisbleForLabel(layout,index,isTimeOzer)        
        UI_GetUILabel(layout, 2100+index):setVisible(isTimeOzer == true)
        UI_GetUILabel(layout, 2200+index):setVisible(isTimeOzer ~= true)
        UI_GetUILabel(layout, 2300+index):setVisible(isTimeOzer ~= true)
    end
    local function setTakenOnceCost()
        tokenOnceCostUp:setText(g_ShopCost[1])
        Shop_SetLabelColor(tokenOnceCostUp,19998,g_ShopCost[1])
        tokenOnceCost:setText(g_ShopCost[1])
        Shop_SetLabelColor(tokenOnceCost,19998,g_ShopCost[1])
    end

    local function refresh()
        Shop_Query()
    end

    --将星令类别
    if info.bTokenHaveFreeTimes == false then --没有免费次数      
        UI_GetUILabel(tokenImage, 2102):setText(FormatString("Shop_NoFreeTimes")) 
        UI_GetUILabel(tokenImage, 2106):setText(FormatString("Shop_NoFreeTimes"))  
        setVisbleForLabel(tokenImage,2,true)
        setVisbleForLabel(tokenImage,6,true)

        setTakenOnceCost()
    elseif info.bHaveTime == true then -- 免费未冷却
        UI_GetUILabel(tokenImage, 2202):setTimer(info.iSecond,TimeFormatType_MinSec) 
        UI_GetUILabel(tokenImage, 2206):setTimer(info.iSecond,TimeFormatType_MinSec)      
        UI_GetUILabel(tokenImage, 2206):addEventListenerTimer(refresh)
        setVisbleForLabel(tokenImage,2,false)   
        setVisbleForLabel(tokenImage,6,false)

        setTakenOnceCost()
    else--显示免费次数
        UI_GetUILabel(tokenImage, 2102):setText(FormatString("Shop_FreeTimes")..info.iRemaindFreeTimes.."/"..info.iTotalFreeTimes)       
        UI_GetUILabel(tokenImage, 2106):setText(FormatString("Shop_FreeTimes")..info.iRemaindFreeTimes.."/"..info.iTotalFreeTimes)       
        setVisbleForLabel(tokenImage,2,true)
        setVisbleForLabel(tokenImage,6,true)
        
        tokenOnceCostUp:setText(FormatString("Shop_Free2"))
        tokenOnceCost:setText(FormatString("Shop_Free2"))
    end
    --钻石类别
    local function setDiamondOnceCost()
        diamondOnceCostUp:setText(g_ShopCost[3])
        Shop_SetLabelColor(diamondOnceCostUp,2,g_ShopCost[3])
        diamondOnceCost:setText(g_ShopCost[3])
        Shop_SetLabelColor(diamondOnceCost,2,g_ShopCost[3])
    end
    if info.bGoldCanFree == false then -- 免费未冷却
        UI_GetUILabel(diamondImage, 2202):setTimer(info.iRemainSecond,TimeFormatType_HourMinSec)      
        setVisbleForLabel(diamondImage,2,false) 
        UI_GetUILabel(diamondImage, 2206):setTimer(info.iRemainSecond,TimeFormatType_HourMinSec)      
        setVisbleForLabel(diamondImage,6,false) 
        setDiamondOnceCost()
       
        UI_GetUILabel(diamondImage, 2206):addEventListenerTimer(refresh)
    else --已冷却
        UI_GetUILabel(diamondImage, 2102):setText(FormatString("Shop_Free1"))
        setVisbleForLabel(diamondImage,2,true) 
        UI_GetUILabel(diamondImage, 2106):setText(FormatString("Shop_Free1"))
        setVisbleForLabel(diamondImage,6,true) 

        diamondOnceCostUp:setText(FormatString("Shop_Free2"))
        diamondOnceCost:setText(FormatString("Shop_Free2"))
    end
    ----消耗
    tokenTenCost:setText(g_ShopCost[2])
    Shop_SetLabelColor(tokenTenCost,19998,g_ShopCost[2])
    diamondTenCost:setText(g_ShopCost[4])
    Shop_SetLabelColor(diamondTenCost,2,g_ShopCost[4])

    --第三类没有免费次数
    diamondOnceHeroCostUp:setText(g_ShopCost[7])
    Shop_SetLabelColor(diamondOnceHeroCostUp,2,g_ShopCost[7])
    diamondOnceHeroCost:setText(g_ShopCost[7])
    Shop_SetLabelColor(diamondOnceHeroCost,2,g_ShopCost[7])
    diamondTenHeroCost:setText(g_ShopCost[8])
    Shop_SetLabelColor(diamondTenHeroCost,2,g_ShopCost[8])
end

function Shop_LuckyDraw(pkg)
    local info = GameServer_pb.CMD_SHOPSYSTEM_RANDITEMORHERO_SC()
    info:ParseFromString(pkg)
   -- Log("info===="..tostring(info))
    g_shop_info = info
    EndWaiting()

    g_isLuckyDraw = true
    Shop_Query()
end
function Shop_ShowLuckyDraw()
    g_isLuckyDraw = false
    local widget = UI_CreateBaseWidgetByFileName("LuckyDraw.json")
    --屏蔽背景的按键响应
    UI_GetUILayout(widget, 4609559):addTouchEventListener(PublicCallBackWithNothingToDo)

    local layout = Layout:create();
    local godanimalanimatrue = GetUIArmature("baoxiang02")
    godanimalanimatrue:setPosition(ccp(0,0))
    godanimalanimatrue:getAnimation():playWithIndex(0) 
    godanimalanimatrue:getAnimation():setMovementEventCallFunc(Shop_ShowAllItem) 
    layout:addNode(godanimalanimatrue)
    layout:setPosition(ccp(480,320))
    widget:addChild(layout,0,0)

    --再抽一次和取消等控件
    UI_GetUILayout(widget, 4619540):setVisible(false)
end
--分发道具
function Shop_ShowAllItem(armature, movementType, movementID)
    local widget = UI_GetBaseWidgetByName("LuckyDraw")

    CreateGameEffectMusic("choujiang.wav")

    if (movementType == LOOP_COMPLETE)then
		armature:removeFromParentAndCleanup(true);
	end

    local layout = Layout:create();
    local godanimalanimatrue = GetUIArmature("baoxiang02")
    godanimalanimatrue:setPosition(ccp(0,-3))
    godanimalanimatrue:getAnimation():playWithIndex(1) 
    layout:addNode(godanimalanimatrue)
    layout:setPosition(ccp(480,320))
    widget:addChild(layout,0,0)
    
    local layout = UI_GetUILayout(widget, 4609584)
    --再抽消耗元宝在g_ShopCost中的索引
    local ShopCostIndex;
    --如果是单抽
    if #g_shop_info.szReturn==1 then     
        g_Shop_animationId = 15
        UI_GetUILabel(widget:getChildByTag(4609584), 10001):setVisible(true)
        UI_GetUILayout(widget, 4619540):setPosition(ccp(0,80))
        print("g_Shop_RandomType==========="..g_Shop_RandomType)
        if g_Shop_RandomType == 5 then 
           UI_GetUILabel(layout, 10001):setText(FormatString("Shop_LifeAtt4",g_ShopCost[9]))
           ShopCostIndex = 7;
        else 
            UI_GetUILabel(layout, 10001):setText(FormatString("Shop_LifeAtt4",g_ShopCost[5+ToInt(g_Shop_RandomType/2)]))
            ShopCostIndex = g_Shop_RandomType;
        end 
    else
        print("g_Shop_RandomType==========="..g_Shop_RandomType)
        if g_Shop_RandomType == 6 then
           UI_GetUILabel(layout, 10002):setText(FormatString("Shop_LifeAtt4",g_ShopCost[9]*10))
           ShopCostIndex = 8;
        else  
           UI_GetUILabel(layout, 10002):setText(FormatString("Shop_LifeAtt4",g_ShopCost[4+ToInt(g_Shop_RandomType/2)]*10))
            ShopCostIndex = g_Shop_RandomType;
        end 
        g_Shop_animationId = 1
        --将显示顺序打乱
        g_Shop_Index = {}
        local num = 1
        math.randomseed(tostring(os.time()):reverse():sub(1, 6)) 
        while num<11 do
            local index = math.random()
            index = index*10 - index*10%1
            local isAdd = true
            for j=1,num-1,1 do
                if g_Shop_Index[j] == index+1 then
                    isAdd = false
                end
            end
            if isAdd == true then
                g_Shop_Index[num]=index+1
                num = num+1
            end
        end
        UI_GetUILabel(widget:getChildByTag(4609584), 10002):setVisible(true)
    end

    local actionArry = CCArray:create()
    local delayTime = CCDelayTime:create(0.3)
    actionArry:addObject(delayTime)
    local callback = CCCallFuncN:create(Shop_LuckyDrawAnimation)
    actionArry:addObject(callback)

    local seqaction = CCSequence:create(actionArry)
    widget:runAction(seqaction)

    local buttons = UI_GetUILayout(widget, 4619540)
    --再抽一次的消耗
    UI_GetUILabel(buttons, 4609571):setText( g_ShopCost[ShopCostIndex]) 
    --消耗类型的图片
    if g_Shop_RandomType == 1 or g_Shop_RandomType ==2 then--如果消耗令牌
        UI_GetUIImageView(buttons, 11):loadTexture("Common/ico_jinbi_10.png")
        --所需消耗的道具是否足够
        Shop_SetLabelColor(UI_GetUILabel(buttons, 4609571),19998,g_ShopCost[ShopCostIndex])
        
    else--如果消耗元宝
        UI_GetUIImageView(buttons, 11):loadTexture("Common/ico_jinbi_01.png")
        --所需消耗的道具是否足够
        Shop_SetLabelColor(UI_GetUILabel(buttons, 4609571),2,g_ShopCost[ShopCostIndex])
    end
    UI_GetUIButton(buttons, 4609579):setTouchEnabled(false)
    UI_GetUIButton(buttons, 4609574):setTouchEnabled(false)
    --再抽一次
    local function onceMore( sender,eventType )
        if eventType == TOUCH_EVENT_ENDED then
            UI_CloseCurBaseWidget()
            Shop_RandItemOrHero(g_Shop_RandomType)
        end
    end
    UI_GetUIButton(buttons, 4609579):addTouchEventListener(onceMore)
    if g_Shop_RandomType == 1 or g_Shop_RandomType ==3 or g_Shop_RandomType ==5 then
        UI_GetUIButton(buttons, 4609579):setTitleText(FormatString("OnceMore1"))
    else
        UI_GetUIButton(buttons, 4609579):setTitleText(FormatString("OnceMore10"))
    end
    --确认
    local function determine( sender,eventType )
        if eventType == TOUCH_EVENT_ENDED then
            UI_CloseCurBaseWidget()
        end
    end
    UI_GetUIButton(buttons, 4609574):addTouchEventListener(determine)


    ShopRefreshData(4606874)   
    Shop_reset(g_ShopQueryInfo)
end
--动画
function Shop_LuckyDrawAnimation()
    if g_Shop_animationId > #g_shop_info.szReturn and g_Shop_animationId~=15 then
        local widget = UI_GetBaseWidgetByName("LuckyDraw") 
        local buttons = UI_GetUILayout(widget, 4619540)
        buttons:setVisible(true)
        UI_GetUIButton(buttons, 4609579):setTouchEnabled(true)
        UI_GetUIButton(buttons, 4609574):setTouchEnabled(true)
        return 
    end
    if g_Shop_animationId == 15 then
        Shop_showLuckyDrawAnimation(1)
    else
        Shop_showLuckyDrawAnimation(g_Shop_Index[g_Shop_animationId])
    end
    g_Shop_animationId = g_Shop_animationId+1
    Log("g_shop_info.szReturn====="..#g_shop_info.szReturn)
end
--播放动画
function Shop_showLuckyDrawAnimation(num)
    local index = g_Shop_animationId
    local enType = g_shop_info.szReturn[num].enType
    local id = g_shop_info.szReturn[num].id
    local levelstep = g_shop_info.szReturn[num].iLevelUp
    local count = g_shop_info.szReturn[num].iCount

    local widget = UI_GetBaseWidgetByName("LuckyDraw")
    local step = GodAnimalSystem_getStep(levelstep)

    if widget == nil then
        return
    end

    local itemData = nil
    local name = nil
    local icon = nil
    local step = 0
    local itemNum = 0
    local heroId = id

    local function AddItemTipcall(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            AddItemTip(id)
        end
    end

    local function toCallHero()
        print("heroId================="..heroId)
        heroData = GetGameData(DataFileHeroBorn, heroId, "stHeroBornData");

        if itemNum > 1 then
            --英雄转换为武魂数量绝对大于1，因此显示出来的是武魂
            itemData = GetGameData(DataFileItem, id, "stItemData");
            icon = UI_ItemIconFrame(UI_GetItemIcon(itemData.m_icon,count),itemData.m_quality)
            name = itemData.m_name
            icon:setTouchEnabled(true)
            icon:addTouchEventListener(AddItemTipcall)
        else
            --英雄不转换为武魂，则显示英雄
            icon = UI_IconFrame(UI_GetHeroIcon(heroData.m_icon),heroData.m_heroInitStar)
            name = heroData.m_name
        end
        
        Shop_CallHero(heroId,itemNum,true,nil,itemNum>1)
        Shop_InitShop(icon,name,levelstep)

        local itemlocation = UI_GetUILayout(widget, 4609584)
        UI_GetUILayout(itemlocation, index):addChild(g_Shop_oneItem)
        UI_GetUILabel(g_Shop_oneItem, 4609588):setVisible(true)

        local layout = Layout:create();
        local godanimalanimatrue = GetUIArmature("chouherotishi")
        godanimalanimatrue:setPosition(ccp(0,0))
        godanimalanimatrue:getAnimation():playWithIndex(0) 
        layout:addNode(godanimalanimatrue)
        layout:setPosition(ccp(0,0))
        UI_GetUILayout(itemlocation, index):addChild(layout,0,0)

    end
    local function isHero()
        local actionArry = CCArray:create()
        local delayTime = CCDelayTime:create(0.7)
        actionArry:addObject(delayTime)

        local callback = CCCallFuncN:create(toCallHero)
        actionArry:addObject(callback)

        local seqaction = CCSequence:create(actionArry)
        UI_GetUILayout(UI_GetUILayout(widget, 4609584), index):runAction(seqaction)
    end
    if enType == 1 then--英雄
        isHero()
        return
    else--道具
        if g_shop_info.szReturn[num].iHeroId ~= nil and g_shop_info.szReturn[num].iHeroId ~= 0  then
            --由英雄产生的武魂
            itemNum = g_shop_info.szReturn[num].iCount
            heroId = g_shop_info.szReturn[num].iHeroId
            isHero()
            return
        end
        itemData = GetGameData(DataFileItem, id, "stItemData");
        name = itemData.m_name

        if (count>1) then
            icon = UI_ItemIconFrame(UI_GetItemIcon(itemData.m_icon,count),itemData.m_quality)
        else
            icon = UI_ItemIconFrame(UI_GetItemIcon(itemData.m_icon),itemData.m_quality)
        end
        step = itemData.m_quality

        icon:setTouchEnabled(true)
        icon:addTouchEventListener(AddItemTipcall)
    end
    --道具
    Shop_InitShop(icon,name,step)
    g_Shop_oneItem:setScale(0.01)        
    
    --所有飞出道具的位置
    local itemlocation = UI_GetUILayout(widget, 4609584)
    --将道具放入容器层
    UI_GetUILayout(itemlocation, index):addChild(g_Shop_oneItem)
    
    --飞出道具的宝箱
    local chest = UI_GetUIImageView(widget, 4609562)
    --获取飞到位置的容器和做标
    local X= UI_GetUILayout(itemlocation, index):getPositionX()
    local Y= UI_GetUILayout(itemlocation, index):getPositionY()
    --将放置道具的容器层放到飞出道具的宝箱中
    UI_GetUILayout(itemlocation, index):setPosition(ccp(chest:getPositionX(),chest:getPositionY()))
    --容器层移动
    local actionArry = CCArray:create()
    
    local moveTo = CCMoveTo:create(0.2,ccp(X,Y))
    actionArry:addObject(moveTo)
    local callback = CCCallFuncN:create(Shop_ShowName)
    actionArry:addObject(callback)

    local seqaction = CCSequence:create(actionArry)  
    UI_GetUILayout(itemlocation, index):runAction(seqaction)
    --道具旋转
    local actionArry = CCArray:create()
    local rotate = CCRotateBy:create(0.2,-360*4)
    actionArry:addObject(rotate)

    local seqaction = CCSequence:create(actionArry)
    g_Shop_oneItem:runAction(seqaction)
    --道具缩放
    local actionArry = CCArray:create()
    local rotateToBig = CCScaleTo:create(0.3,1)
    actionArry:addObject(rotateToBig)

    local seqaction = CCSequence:create(actionArry)
    g_Shop_oneItem:runAction(seqaction)
end
--初始化道具数据
function Shop_InitShop(icon,name,step)
    local widget = UI_GetBaseWidgetByName("LuckyDraw")
    if widget == nil then
        return
    end
    g_Shop_oneItem = UI_GetUIImageView(widget, 222222):clone()
    g_Shop_oneItem:setPosition(ccp(0,0))
    g_Shop_oneItem:addChild(icon)
    if g_Shop_animationId ~= 15  then  --如果不是单抽则要缩小名字     
            UI_GetUILabel(g_Shop_oneItem, 4609588):setFontSize(18)
            print("#name================="..#name)
            if #name >= 21 then
                UI_GetUILabel(g_Shop_oneItem, 4609588):setFontSize(16)
            end
    end
    UI_GetUILabel(g_Shop_oneItem, 4609588):setVisible(false)
    UI_GetUILabel(g_Shop_oneItem, 4609588):setText(name)
    UI_GetUILabel(g_Shop_oneItem, 4609588):setColor(ccc3(255,255,255))

    g_Shop_oneItem:setVisible(true)
end
--显示道具数据
function Shop_ShowName()
    UI_GetUILabel(g_Shop_oneItem, 4609588):setVisible(true)
    Shop_LuckyDrawAnimation()
end
--召唤英雄
function Shop_CallHero(id,num,isShop,callBack,isSoul)
    local widget = UI_CreateBaseWidgetByFileName("ShopCallHero.json")
    ----是背景的按键响应无效以保证动画播放完成
    UI_GetUILayout(widget, 4670794):setTouchEnabled(false)
    local location = UI_GetUIImageView(widget, 4670840)

    CreateGameEffectMusic("gethero.wav")
    ----放置英雄
    local heroData = GetGameData(DataFileHeroBorn, id, "stHeroBornData");
    Formation_AddAnimation(location,heroData.m_animName,199)
    --名字
    UI_GetUILabel(widget, 4670800):setText(heroData.m_name)
    UI_GetUILabel(widget, 4670800):enableStroke()
    --星
    UI_GetUILayout(widget, 1011+heroData.m_heroInitStar):setVisible(true)

    --继续显示
    local function nextItem(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            UI_CloseCurBaseWidget(EUIOpenAction_None)
            Shop_LuckyDrawAnimation()
        end
    end
    --继续显示
    local function callBackOther(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            UI_CloseCurBaseWidget(EUIOpenAction_None)
            if callBack ~= nil then
                callBack()
            end
        end
    end
    if isShop==nil or isShop == true then
        UI_GetUIButton(widget, 4670838):addTouchEventListener(nextItem)
    elseif isShop == false then
        UI_GetUIButton(widget, 4670838):addTouchEventListener(callBackOther)
    end
    --如果是英雄转化的武魂
    local info_ = UI_GetUILayout(widget, 4670836)
    info_:setVisible(false)
    if isSoul==true then
        info_:setVisible(true)
        UI_GetUILabel(info_, 4670830):setText(FormatString("HeroToSoul",heroData.m_heroInitStar+1))
        UI_GetUILabel(info_, 4670832):setText(FormatString("HeroToSoulNum",num))
    end
    --特效

    local function nextAnimation(armature, movementType, movementID)
	    if (movementType == LOOP_COMPLETE)then
		    armature:getAnimation():playWithIndex(1);
	    end
    end

    local layout = Layout:create();
	local animation = GetUIArmature("chouhero")
	animation:getAnimation():playWithIndex(0)
	animation:getAnimation():setMovementEventCallFunc(nextAnimation);
	animation:setPosition(ccp( 480, 320 ))

    layout:addNode(animation)

    widget:addChild(layout,1,1)
end
------------------------------------------兑换-----------------------------------------
--初始化兑换界面
function Shop_InitExchange(pkg)
    Shop_ChangeLayer("exchange")
    local info = GameServer_pb.CMD_SHOPSYSTEM_HONORCONVERT_QUERY_SC()
    info:ParseFromString(pkg)
    Log("info===="..tostring(info))
    EndWaiting()
    
    local widget = UI_GetBaseWidgetByName("Shop")
    if widget == nil then
        return
    end
    widget:setVisible(true)

    local exchange = UI_GetUIImageView(widget, 12)
    local listView = UI_GetUIListView(exchange, 111)
    listView:removeAllItems()
    local allGoods = nil
    --兑换物品的回调
    local function buy(sender , eventType)
        if eventType==TOUCH_EVENT_ENDED then
            local tag = tolua.cast(sender,"ImageView"):getTag()-1110
            Shop_BuySomething(info.szGoodList[tag].iGoodId,tag-1,Shop_BuyItem)
        end
    end
    --初始化兑换列表
    for i=1,#info.szGoodList,1 do
        if(i%2==1) then
            allGoods = UI_GetUILayout(widget, 4607187):clone()
            listView:pushBackCustomItem(allGoods)
            allGoods:setVisible(true)

            UI_GetUIImageView(allGoods, 20):setVisible(false)
        end
        --显示数据的框
        local itemBox = nil
        if(i%2==1) then
            itemBox = UI_GetUIImageView(allGoods, 4607201)
        else
            itemBox = UI_GetUIImageView(allGoods, 20)
            UI_GetUIImageView(allGoods, 20):setVisible(true)
        end
        itemBox:setVisible(true)
        local goodData = GetGameData(DataFileHonorGoods, info.szGoodList[i].iGoodId, "stHonorGoodsData")
        --获取道具头像和名字
	    local data = GetGameData(DataFileItem, goodData.m_goodID, "stItemData");
        local itemNum = 0
        if goodData.m_count>1 then
            itemNum = goodData.m_count
        end
	    local icon = UI_ItemIconFrame(UI_GetItemIcon(data.m_icon,itemNum),data.m_quality)
        --显示数据
        UI_GetUIImageView(itemBox, 10000):addChild(icon)
        itemBox:setTouchEnabled(true)
        itemBox:setTag( i+1110 )
        itemBox:addTouchEventListener(buy)
        --名字
        UI_GetUILabel(itemBox, 10001):setText(data.m_name)
        --消耗
        UI_GetUILabel(itemBox, 10004):setText(goodData.m_price)
        UI_GetUILabel(itemBox, 10004):enableStroke()
        Shop_SetLabelColor(UI_GetUILabel(itemBox, 10004),goodData.m_costType,goodData.m_price)
        ----货币的图片
        UI_GetUIImageView(itemBox, 10005):loadTexture("LifeAttIcon/s_"..goodData.m_costType..".png")

        if info.szGoodList[i].bIsSale == true then
            UI_GetUILayout(itemBox, 122121):setVisible(true)
            itemBox:setTouchEnabled(false)
        end
    end
    --更新时间
    if info.bIsToday == true then
        UI_GetUILabel(exchange, 4607228):setText(FormatString("TodayUpdate",info.iUpdateMoment))
    else
        UI_GetUILabel(exchange, 4607228):setText(FormatString("TomorrowUpdate",info.iUpdateMoment))
    end
    --更新按钮
    local function UserUpdata(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            Shop_RefreshList()
        end
    end
    UI_GetUIButton(exchange, 4607230):addTouchEventListener(UserUpdata)

    ShopRefreshData(4776444)
end
--兑换界面
function Shop_BuySomething(id,index,callBack,costType)
    local sellID = id
    local widget = UI_CreateBaseWidgetByFileName("ShopItemInfo.json")
    local image = UI_GetUILayout(widget, 4624741)
    
    local goodData = GetGameData(DataFileHonorGoods, id, "stHonorGoodsData")
    local data = GetGameData(DataFileItem, goodData.m_goodID, "stItemData");
    local itemNumInBag = GetLocalPlayer():GetPlayerBag():GetItemCountByItemId(goodData.m_goodID)

    if costType ~= nil then
        UI_GetUIImageView(widget:getChildByTag(4624741), 4624894):loadTexture("Common/ico_jinbi_0"..costType..".png")
    end
    --icon
    local icon = UI_ItemIconFrame(UI_GetItemIcon(data.m_icon),data.m_quality)
    UI_GetUIImageView(image, 4624744):addChild(icon)
    --名字
    UI_GetUILabel(image, 4624755):setText(data.m_name)
    --拥有数量
    UI_GetUILabel(image,4624747):setText(FormatString("HaveNum",itemNumInBag))
    if (goodData.m_goodID>=4001 and goodData.m_goodID<=4123) then --喜好品和装备等有属性的道具
        --属性
        UI_GetUILabel(image, 4624750):setText(data.m_attinfo)
        --描述
        local desc = UI_GetUILabel(UI_GetUILayout(image, 4624900), 4624898)
        desc:setText(goodData.m_desc)
        local temp = #goodData.m_desc/39 - #goodData.m_desc/39%1 
        desc:setPosition(ccp(0,(temp+1)*27-70))
    else --没有属性的道具
    --描述
       UI_GetUILabel(image, 4624750):setText(goodData.m_desc)
    end
    --消耗
    UI_GetUILabel(image,4624896):setText(goodData.m_price)
    Shop_SetLabelColor(UI_GetUILabel(image,4624896),goodData.m_costType,goodData.m_price)
    --购买？件
    UI_GetUILabel(image,4624889):setText(FormatString("BuyNum",goodData.m_count))
    --买
    local function buy ( sender,eventType )
        if eventType==TOUCH_EVENT_ENDED then
            callBack(index)
            UI_CloseCurBaseWidget()
        end
    end
    UI_GetUIButton(image, 4624752):addTouchEventListener(buy)
end
----------------------------------------------------------------------------------------
--设置文本框颜色
function Shop_SetLabelColor(label ,costType,price)
    if costType == 20 then -- 消耗荣誉 
        if GetLocalPlayer():GetInt(EPlayer_Honor) < price then
            label:setColor(ccc3(230,0,0))
        end
    elseif costType == 1 then --铜币
        if GetLocalPlayer():GetInt(EPlayer_Silver) < price then
             label:setColor(ccc3(230,0,0))
        end
    elseif costType == 2 then--元宝
        if GetLocalPlayer():GetInt(EPlayer_Gold) < price then
            label:setColor(ccc3(230,0,0))
        end
    elseif costType == 19998 then--将星令
        if GetLocalPlayer():GetPlayerBag():GetItemCountByItemId(19998) < price then
            label:setColor(ccc3(230,0,0))
        end
    end
end
function Shop_ChangeLayer(name)
    local widget = UI_GetBaseWidgetByName("Shop")
    if widget == nil then
        return
    end
    local LuckyDraw= nil
    local exchange = nil
    if name == "exchange" or name == "LuckyDraw" then
        --“抽奖”“兑换”按钮
        --UI_GetUIButton(widget, 1):setButtonEnabled(name == "exchange")
        --UI_GetUIButton(widget, 2):setButtonEnabled(name == "LuckyDraw")
        --抽奖界面
        LuckyDraw = UI_GetUIImageView(widget, 11)
        LuckyDraw:setVisible(name == "LuckyDraw")
        --UI_GetUIButton(LuckyDraw:getChildByTag(4607112):getChildByTag(4607115):getChildByTag(4607116), 4607117):setTouchEnabled(name == "LuckyDraw")
        --UI_GetUIButton(LuckyDraw:getChildByTag(4606924):getChildByTag(4607115):getChildByTag(4607116), 4607117):setTouchEnabled(name == "LuckyDraw")
        --兑换界面
        exchange = UI_GetUIImageView(widget, 12)
        exchange:setVisible(name == "exchange")
        --UI_GetUIListView(exchange, 1):setTouchEnabled(name == "exchange")
        --UI_GetUIButton(exchange, 4607230):setTouchEnabled(name == "exchange")
    end
    --还原抽奖界面
    if name ~= "LuckyDraw" then
        --UI_GetUIImageView(LuckyDraw:getChildByTag(4607112):getChildByTag(4607115), 4607116):setPosition(ccp(155,275))
        --UI_GetUIImageView(LuckyDraw:getChildByTag(4606924):getChildByTag(4607115), 4607116):setPosition(ccp(155,275))
    elseif name ~= "exchange" then
    end
end
----------------------------------------------------------------------------------------
--网络消息：商城查询
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_SHOPSYSTEM_QUERY, "Shop_SHOPSYSTEM_QUERY" )
--网络消息：获取兑换数据
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_SHOPSYSTEM_HONORCONVERT_QUERY, "Shop_InitExchange" )
--网络消息：抽取操作
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_SHOPSYSTEM_RANDITEMORHERO, "Shop_LuckyDraw" )

function Shop_SHOPSYSTEM_QUERY(pkg)
    EndWaiting();
	local tmp = GameServer_pb.CMD_SHOPSYSTEM_QUERY_SC();
	tmp:ParseFromString(pkg)
	Log("*********"..tostring(tmp))

    local widget = UI_GetBaseWidgetByName("Shop")
    if g_isLuckyDraw == false or g_isLuckyDraw == nil then
        Shop_InitLayer(false,tmp)
    else    
	    Shop_ShowLuckyDraw()
        g_ShopQueryInfo = tmp
    end
end

------------------------------------------------------------------------
function Shop_fade(widget)
    local fadein = CCFadeIn:create(1)  
    local fadeout = CCFadeOut:create(1)  
    local sequence = CCSequence:createWithTwoActions(fadein, fadeout) 
    local action = CCRepeatForever:create(sequence)
    widget:runAction(action)
end