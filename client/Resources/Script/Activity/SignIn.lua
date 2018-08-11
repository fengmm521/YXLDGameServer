function SignIn_Create()
    Packet_Cmd(GameServer_pb.CMD_OPERATESYSTEM_QUERY_CHECKIN);
    ShowWaiting()
end

function SignIn_Init(pkg)
    EndWaiting()
    local info = GameServer_pb.CMD_OPERATESYSTEM_QUERY_CHECKIN_SC()
    info:ParseFromString(pkg)
    Log("info==========="..tostring(info))

    local widget = UI_GetBaseWidgetByName("SignIn")
    if widget == nil then
        widget = UI_CreateBaseWidgetByFileName("SignIn.json")
    else
        if g_SignIn_CallHero ~= nil then
            Shop_CallHero(g_SignIn_CallHero[1],g_SignIn_CallHero[2],false)
            g_SignIn_CallHero = nil
        end  
    end
    UI_GetUIButton(widget, 7410):addTouchEventListener(UI_ClickCloseCurBaseWidgetWithNoAction)
    UI_GetUILayout(widget, 1124809466):addTouchEventListener(PublicCallBackWithNothingToDo)
    local month = os.date("%m", os.time())

    local listView = UI_GetUIListView(widget, 1001)
    listView:removeAllItems()
    local title = UI_GetUILayout(widget, 1003):clone()
    UI_GetUILabel(title, 4002):enableStroke()
    UI_GetUILabel(title, 4004):enableStroke()
    UI_GetUILabel(title, 4003):enableStroke()

    --说明
    local function showRule(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            SignIn_Rule() 
        end
    end
    UI_GetUIImageView(title, 2001):addTouchEventListener(showRule)
    UI_GetUIImageView(title, 2001):setTouchEnabled(true)

    --签到次数
    if info.iHaveClickTimes==0 then
        UI_GetUILabel(title, 4003):setText(info.iHaveCheckInCount-1)
    else
        UI_GetUILabel(title, 4003):setText(info.iHaveCheckInCount)
    end
    listView:pushBackCustomItem(title)
    title:setVisible(true)

    --显示物品属性
    local function ShowInfo(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            local tag = tolua.cast(sender,"ImageView"):getTag()
            if tag%100 > info.iHaveCheckInCount then
                SignIn_ShowTip(tag,widget)
            else
                SignIn_Sign(tag,widget,info.iHaveClickTimes)
            end
        end
    end

    local line= nil
    for i=month*100+1,month*100+31,1 do
        local data = GetGameData(DataFileCheckIn, i, "stCheckInData");
        if data == nil then
            break
        end
        if (i-1)%5 == 0 then
            line = UI_GetUILayout(widget, 1002):clone()
            line:setVisible(true)
            listView:pushBackCustomItem(line)
        end
        local layoutShow = UI_GetUILayout(line, 2000+(i-1)%5+1) 
        layoutShow:setVisible(true)
        local award = SignIn_GetAward(data.m_Award)
        local icon = SignIn_CreateIcon(award,widget)
        if icon ~= nil then
            icon:setPosition(ccp(45,45))
            layoutShow:addChild(icon)
            
            icon:setTouchEnabled(true)
            icon:setTag(i)
            icon:addTouchEventListener(ShowInfo)

            if data.m_isDouble == 1 then
                UI_GetUIImageView(layoutShow, 3002):setVisible(true)
                UI_GetUIImageView(layoutShow, 3002):loadTexture("qiandao/mark_qiandao_V"..data.m_VipConditions..".png")
            end

            if info.iHaveCheckInCount > i%100 then
                UI_GetUIImageView(layoutShow, 5001):setVisible(true)
                UI_GetUIImageView(layoutShow, 5001):setZOrder(12)
                UI_GetUIImageView(layoutShow, 3001):setZOrder(9)
                icon:setTouchEnabled(false)
            elseif info.iHaveCheckInCount == i%100 and info.bLastCanClick == false then
                UI_GetUIImageView(layoutShow, 5001):setVisible(true)
                UI_GetUIImageView(layoutShow, 5001):setZOrder(12)
                UI_GetUIImageView(layoutShow, 3001):setZOrder(9)
                icon:setTouchEnabled(false)
            elseif info.iHaveCheckInCount == i%100 and info.iHaveClickTimes ==1  then
                UI_GetUIImageView(layoutShow, 5001):setVisible(true)
                UI_GetUIImageView(layoutShow, 5001):setZOrder(12)
            end
        end
    end
end
function SignIn_GetAward(str)
    local temp={
        [1]=0,
        [2]=0,
        [3]=0
    }
    local kind={
        ["lifeatt"] = 1,
        ["item"] = 2,
        ["hero"] = 3,
        ["LifeAtt"] = 1,
        ["Item"] = 2,
        ["Hero"] = 3,
    }
    local startNum = 1
    local endNum = string.find(str, "#",start)
    temp[1] = kind[string.sub(str, startNum, endNum-1)]
    startNum = endNum+1
    endNum = string.find(str, "#",startNum)
    temp[2] = string.sub(str, startNum, endNum-1)
    temp[3] = string.sub(str, endNum+1)
    return temp
end
function SignIn_CreateIcon(award,widget)
    if award[1] ==1 then--铜币，体力等...
        local path = "LifeAttIcon/"
        if award[2] == "1" then--铜币
            path = path.."1_1.png"
        elseif award[2] == "2" then--元宝
            path = path.."2_2.png"
        elseif award[2] == "4" then--修为
            path = path.."4-4.png"
        elseif award[2] == "17" then--体力
            path = path.."17-17.png"  
        else
            print("LifeAtt is "..award[2].." and Can`t find")
            return nil
        end
        local image = UI_GetUIImageView(widget, 7001):clone()
        image = tolua.cast(image,"ImageView")
        image:setPosition(ccp(0,0))
        image:loadTexture(path)
        image:setVisible(true)

        local countLab = LabelBMFont:create()
        countLab:setFntFile("zhandou/nuqi.fnt")
        countLab:setText(award[3])
        countLab:setAnchorPoint(ccp(1,0))
	    countLab:setPosition(ccp(image:getSize().width/2 - 5, -image:getSize().height/2-2));
	    image:addChild(countLab,1,1);

        return UI_ItemIconFrame(image, 3)
    elseif award[1] == 2 then -- 道具
        local data = GetGameData(DataFileItem, award[2] , "stItemData");
        local icon = UI_ItemIconFrame(UI_GetItemIcon(data.m_icon, tonumber(award[3])), data.m_quality)
        
        return icon
    elseif award[1] == 3 then--英雄
        local data = GetGameData(DataFileHeroBorn, award[2] , "stHeroBornData");
        local icon = UI_IconFrame(UI_GetHeroIcon(award[2]), data.m_heroInitStar)

        return icon
    else
        return nil 
    end 
    return nil
end

function Sign_exit(sender,eventType)
    if eventType == TOUCH_EVENT_ENDED then
        tolua.cast(sender,"Layout"):setVisible(false)
    end
end
function SignIn_ShowTip(id,widget)
    local tip = UI_GetUILayout(widget, 1004)
    tip:setVisible(true)
    tip:addTouchEventListener(Sign_exit)
    local data = GetGameData(DataFileCheckIn,id, "stCheckInData");
    local award = SignIn_GetAward(data.m_Award)
    local icon = SignIn_CreateIcon(award,widget)

    if icon then
        --图标
        UI_GetUIImageView(tip, 2001):removeChildByTag(2020)
        UI_GetUIImageView(tip, 2001):addChild(icon)
        icon:setTag(2020)
        --名字
        if award[1] == 1 then --体力等
            UI_GetUILabel(tip, 2002):setText(FormatString("SignIn_LifeAtt_"..award[2]))
        elseif award[1] == 2 then -- 道具
            local dataItem = GetGameData(DataFileItem, award[2] , "stItemData");
            UI_GetUILabel(tip, 2002):setText(dataItem.m_name)
        elseif award[1] == 3 then -- 英雄
            local dataHero = GetGameData(DataFileHeroBorn, award[2] , "stHeroBornData");
            UI_GetUILabel(tip, 2002):setText(dataHero.m_name)
        end
        --说明
        UI_GetUILabel(tip, 2003):setText(data.m_des)
		UI_GetUILabel(tip, 2004):enableStroke()
        --第X次
        UI_GetUILabel(tip, 2004):setText(FormatString("SignIn_NeedTime",data.m_allTimes))
    end
end
function SignIn_Sign(id,widget,ClickTimes)
    local sign = UI_GetUILayout(widget, 1005)
    sign:setVisible(true)
    sign:addTouchEventListener(Sign_exit)
    local data = GetGameData(DataFileCheckIn,id, "stCheckInData");
    local award = SignIn_GetAward(data.m_Award)
    local icon = SignIn_CreateIcon(award,widget)

    if icon then
        --图标
        UI_GetUIImageView(sign, 1001):removeChildByTag(2020)
        UI_GetUIImageView(sign, 1001):addChild(icon)
        icon:setTag(2020) 
    end 
     --名字
    if award[1] == 1 then --体力等
        UI_GetUILabel(sign, 1002):setText(FormatString("SignIn_LifeAtt_"..award[2]))
    elseif award[1] == 2 then -- 道具
        local dataItem = GetGameData(DataFileItem, award[2] , "stItemData");
    UI_GetUILabel(sign, 1002):setText(dataItem.m_name)
    elseif award[1] == 3 then -- 英雄
        local dataHero = GetGameData(DataFileHeroBorn, award[2] , "stHeroBornData");
        UI_GetUILabel(sign, 1002):setText(dataHero.m_name)
    end
    if data.m_isDouble == 1 then
    --第X次
        UI_GetUILabel(sign, 1003):setText(FormatString("SignIn_VipCon",data.m_VipConditions))
    else
        UI_GetUILabel(sign, 1003):setText("")
    end
	UI_GetUILabel(sign, 1003):enableStroke()
    local function sendMessage()
        local tab = GameServer_pb.CMD_OPERATESYSTEM_CHECKIN_CS();
	    tab.iWillCheckInID = id%100;
	    Packet_Full(GameServer_pb.CMD_OPERATESYSTEM_CHECKIN, tab);
        ShowWaiting()
        Sign_exit(sign,TOUCH_EVENT_ENDED)
    end
    local function Sign(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            if ClickTimes == 0 then
                sendMessage()
            else
                if data.m_VipConditions > GetLocalPlayer():GetInt(EPlayer_VipLevel) then
                    local tab = {}
                    tab.info = FormatString("SignIn_GetDouble",data.m_VipConditions)
                    tab.msgType = EMessageType_LeftRight
                    tab.leftFun = RechargeSystem_ShowLayout
                    Messagebox_Create(tab)
                    Sign_exit(sign,TOUCH_EVENT_ENDED)
                else
                    sendMessage()
                end
            end
        end
    end
    UI_GetUIButton(sign, 1004):addTouchEventListener(Sign)
end

function SignIn_Rule()
    local widget = UI_CreateBaseWidgetByFileName("TerritoryEnternalRule.json")
    local listView = UI_GetUIListView(widget, 4670692)
    UI_GetUILabel(widget, 4556518):setText(FormatString("SignIn_Rule"))
    
    for i=1,3,1 do 
        local infolab = CompLabel:GetDefaultCompLabel(FormatString("SignIn_Rule_"..i),listView:getSize().width)
        local tempLayout= UI_GetUILayout(widget, 1122):clone()
        --infolab:setColor(ccc3(164,240,25))
        listView:pushBackCustomItem(infolab)
        listView:pushBackCustomItem(tempLayout)
    end
end

ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_OPERATESYSTEM_QUERY_CHECKIN, "SignIn_Init" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_OPERATESYSTEM_CHECKIN, "OPERATESYSTEM_CHECKIN" )
function OPERATESYSTEM_CHECKIN(pkg)
    EndWaiting()
    local info = GameServer_pb.CMD_PAYMENT_FIRST_PAY_SC()
    info:ParseFromString(pkg)
    Log("info==========="..tostring(info))


    Packet_Cmd(GameServer_pb.CMD_OPERATESYSTEM_QUERY_CHECKIN);
    ShowWaiting()

    if info.szAwardHeroList ~= nil and info.szAwardHeroList ~={} then
        g_SignIn_CallHero = {}
        g_SignIn_CallHero[1] =  info.szAwardHeroList[1].iHeroId
        g_SignIn_CallHero[2] =  info.szAwardHeroList[1].info.szAwardHeroList[1].iCount
    end
    local widget = UI_GetBaseWidgetByName("MainUi")
    UI_GetUIButton(widget:getChildByTag(1401):getChildByTag(1002), 1002):setVisible(false)
end