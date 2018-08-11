function FirstPay_Create()
    
    Packet_Cmd(GameServer_pb.CMD_PAYMENT_FIRST_QUERY);
    ShowWaiting()
    --FirstPay_InitData()
end

function FirstPay_InitData(pkg)
    EndWaiting()
    local info = GameServer_pb.CMD_PAYMENT_FIRST_QUERY_SC()
    info:ParseFromString(pkg)
    Log("info==========="..tostring(info))

    local widget = UI_CreateBaseWidgetByFileName("FirstPay.json")
    UI_GetUILayout(widget, 3101):addTouchEventListener(PublicCallBackWithNothingToDo)    
    UI_GetUILayout(widget, 3102):addTouchEventListener(UI_ClickCloseCurBaseWidgetWithNoAction)

    local function firstPay(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            UI_ClickCloseCurBaseWidgetWithNoAction(sender, eventType)
            RechargeSystem_ShowLayout()
        end
    end 
    local function getFirstPay(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            UI_ClickCloseCurBaseWidgetWithNoAction(sender, eventType)
            Packet_Cmd(GameServer_pb.CMD_PAYMENT_FIRST_PAY);
            ShowWaiting()
        end
    end 
    if info.bCanGet == false then --充点儿小钱
        UI_GetUIButton(widget, 2001):addTouchEventListener(firstPay)
        UI_GetUIButton(widget, 2001):setVisible(true)
        UI_GetUIButton(widget, 2002):setVisible(false)
    else --领取首冲奖励
        UI_GetUIButton(widget, 2002):addTouchEventListener(getFirstPay)
        UI_GetUIButton(widget, 2002):setVisible(true)
        UI_GetUIButton(widget, 2001):setVisible(false)
    end
    --铜币
    UI_GetUIImageView(widget:getChildByTag(1001), 1001):loadTexture("LifeAttIcon/1_1.png")
    UI_GetUILabel(widget:getChildByTag(1001), 1002):setText(FormatString("TenThousand",info.resoult.szAwardPropList[1].iValue/10000))
    UI_GetUILabel(widget:getChildByTag(1001), 1002):setVisible(true)
    UI_GetUILabel(widget:getChildByTag(1001), 1002):enableStroke()

    --修为
    UI_GetUIImageView(widget:getChildByTag(1002), 1001):loadTexture("LifeAttIcon/4-4.png")
    UI_GetUILabel(widget:getChildByTag(1002), 1002):setText(FormatString("TenThousand",info.resoult.szAwardPropList[2].iValue/10000))
    UI_GetUILabel(widget:getChildByTag(1002), 1002):setVisible(true)
    UI_GetUILabel(widget:getChildByTag(1002), 1002):enableStroke()

    local function showMessage(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            local tag = tolua.cast(sender,"ImageView"):getTag()
            if tag > 1000 and tag<1100 then--英雄
                FirstPay_ShowHero(widget,tag)
            elseif tag >200000 and tag <299999 then -- 装备
                FirstPay_ShowEquip(widget,tag)
            end
        end
    end

    --装备
    local idequip = 200151
    UI_GetUIImageView(widget:getChildByTag(1003), 1001):loadTexture("Icon/Equip/"..idequip..".png")
    UI_GetUIImageView(widget:getChildByTag(1003), 1001):setTag(idequip)
    UI_GetUIImageView(widget:getChildByTag(1003), idequip):setTouchEnabled(true)
    UI_GetUIImageView(widget:getChildByTag(1003), idequip):addTouchEventListener(showMessage)
    --英雄
    local idhero = 1002
    UI_GetUIImageView(widget:getChildByTag(1004), 1001):loadTexture("Icon/HeroIcon/"..idhero..".png")
    UI_GetUIImageView(widget:getChildByTag(1004), 1001):setTag(idhero)
    UI_GetUIImageView(widget:getChildByTag(1004), idhero):setTouchEnabled(true)
    UI_GetUIImageView(widget:getChildByTag(1004), idhero):addTouchEventListener(showMessage)

end
function FirstPay_exit(sender,eventType)
    if eventType == TOUCH_EVENT_ENDED then
        tolua.cast(sender,"Layout"):setVisible(false)
    end
end
 function FirstPay_ShowEquip(widget,tag)
    local data = UI_GetUILayout(widget, 8002)
    data:setVisible(true)
    data:addTouchEventListener(FirstPay_exit)

    local equipData = GetGameData(DataFileEquip, tag, "stEquipData");

    local icon =  UI_ItemIconFrame(FirstPay_getEquipIcon(tag),4)  
    icon:setTag(1001)
    UI_GetUIImageView(data, 1001):removeChildByTag(1001)
    UI_GetUIImageView(data, 1001):addChild(icon)
 end
 function FirstPay_ShowHero(widget,tag)
    local data = UI_GetUILayout(widget, 8001)
    data:setVisible(true)
    data:addTouchEventListener(FirstPay_exit)

    local icon =  UI_ItemIconFrame(UI_GetHeroIcon(tag),2)
    icon:setTag(1001)
    UI_GetUIImageView(data, 1001):removeChildByTag(1001)
    UI_GetUIImageView(data, 1001):addChild(icon)
 end

 function FirstPay_getEquipIcon(id)
    local image = ImageView:create()

    image:loadTexture("Icon/Equip/"..id..".png")
    return image
 end
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_PAYMENT_FIRST_QUERY, "FirstPay_InitData" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_PAYMENT_FIRST_PAY, "PAYMENT_FIRST_PAY" )
function PAYMENT_FIRST_PAY(pkg)
    EndWaiting()
    local info = GameServer_pb.CMD_PAYMENT_FIRST_PAY_SC()
    info:ParseFromString(pkg)
    Log("info==========="..tostring(info))

    if info.resoult.szAwardHeroList ~= nil and info.resoult.szAwardHeroList ~={} then
        Shop_CallHero(info.resoult.szAwardHeroList[1].iHeroID,info.resoult.szAwardHeroList[1].iCount,false,nil,info.resoult.szAwardHeroList[1].bHaveChgSoul)
    end
    local widget = UI_GetBaseWidgetByName("MainUi")
    if widget ~= nil then
        UI_GetUIButton(widget:getChildByTag(1401), 1001):setVisible(false)
    end
end