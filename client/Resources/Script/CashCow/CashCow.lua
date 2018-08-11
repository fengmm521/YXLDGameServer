function CashCow_Create()
    local tab = GameServer_pb.CMD_COINTREE_SHAKE_QUERY_CS();
    tab.enType = 1
    Packet_Full(GameServer_pb.CMD_COINTREE_SHAKE_QUERY, tab);
    ShowWaiting()
end
function CashCow_CreateLayer(pkg)
    local info = GameServer_pb.CMD_COINTREE_SHAKE_QUERY_SC()
    info:ParseFromString(pkg)
    Log("info===="..tostring(info))
    EndWaiting()

    if ( info.detail.enType ==  2)then
        --十连抽数据
        CashCow_ManyTimes(info)
        return 
    end
    CashCow_RefreshData(info.detail)
end
--刷新单抽数据
function CashCow_RefreshData(detail)
    local widget = UI_GetBaseWidgetByName("CashCow")
    if widget == nil then
        widget = UI_CreateBaseWidgetByFileName("CashCow.json")
        UI_GetUIImageView(widget, 4670552):setVisible(false)
    else
        UI_GetUIImageView(widget, 4670552):setVisible(true)
    end
    UI_GetUILayout(widget, 4670515):addTouchEventListener(PublicCallBackWithNothingToDo)

    local function close(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            UI_CloseCurBaseWidget(EUIOpenAction_None)
        end
    end

    UI_GetUIButton(widget, 4670524):addTouchEventListener(close)
    --今日可摇
    UI_GetUILabel(widget, 4670637):setText(detail.iRemaindShakeTimes.."/"..detail.iTotalShakeTimes)
    --摇一次
    local function once(sender,eventType)
        if (eventType == TOUCH_EVENT_ENDED) then
            CashCow_Konck(1)
        end
    end
    --连续摇
    local function ManyTimes(sender,eventType)
        if (eventType == TOUCH_EVENT_ENDED) then
            local tab = GameServer_pb.CMD_COINTREE_SHAKE_QUERY_CS();
            tab.enType = 2
            Packet_Full(GameServer_pb.CMD_COINTREE_SHAKE_QUERY, tab);
            ShowWaiting()
        end
    end
    --查看VIp
    local function vip(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            VIPInfo()
        end
    end
    
    local isHaveEnoughTime = nil
    if detail.iRemaindShakeTimes>0 then
        isHaveEnoughTime = true
    else
        isHaveEnoughTime = false
    end
    print("-----------------------1----------------------")
    if isHaveEnoughTime == true then
        local layout = UI_GetUILayout(widget, 4687742)
        --此次元宝消耗
        UI_GetUILabel(layout, 4670542):setText(detail.iCost)
        --此次能获得的铜钱
        UI_GetUILabel(layout, 4670544):setText(detail.iGet)
       
        --摇一次
        UI_GetUIButton(widget, 4670547):addTouchEventListener(once)
        UI_GetUIButton(widget, 4670547):setTitleText(FormatString("CrashCowOnce"))
        --连续摇
        UI_GetUIButton(widget, 4670550):addTouchEventListener(ManyTimes)
        UI_GetUIButton(widget, 4670550):setTitleText(FormatString("CrashCowN"))

        layout:setVisible(true)
        UI_GetUILayout(widget, 4670652):setVisible(false)
        
    else
        UI_GetUILayout(widget, 4670652):setVisible(true)
        UI_GetUILayout(widget, 4687742):setVisible(false)

        --关闭
        UI_GetUIButton(widget, 4670547):addTouchEventListener(close)
        UI_GetUIButton(widget, 4670547):setTitleText(FormatString("CrashCowClose"))
        --查看VIP
        UI_GetUIButton(widget, 4670550):addTouchEventListener(vip)
        UI_GetUIButton(widget, 4670550):setTitleText(FormatString("CrashCowVIP"))
    end
end
--连续摇
function CashCow_ManyTimes(info)
    local widget = UI_CreateBaseWidgetByFileName("CashCowN.json") 
    UI_GetUILayout(widget, 4670629):addTouchEventListener(PublicCallBackWithNothingToDo)
    local function close(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            UI_CloseCurBaseWidget()
        end
    end
    UI_GetUIButton(widget, 4670627):addTouchEventListener(close)
    --摇一摇的次数
    UI_GetUILabel(widget, 4670602):setText(info.iCanShakeTimes)
    --消耗的元宝
    UI_GetUILabel(UI_GetUIImageView(widget, 4670608), 4670612):setText(info.detail.iCost)
    --获得的铜币
    UI_GetUILabel(UI_GetUIImageView(widget, 4670617), 4670620):setText(info.detail.iGet)
    --确定连抽
    local function determine(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            UI_CloseCurBaseWidget()
            CashCow_Konck(2)
        end
    end
    UI_GetUIButton(widget, 4670621):addTouchEventListener(determine)
end
--屏蔽更改摇钱的按钮的点击状态
function CashCow_CashSate(isTouchEnable)
    local widget = UI_GetBaseWidgetByName("CashCow") 
    if widget then
        UI_GetUIButton(widget, 4670547):setTouchEnabled(isTouchEnable)
        UI_GetUIButton(widget, 4670550):setTouchEnabled(isTouchEnable)
    end
end
--显示所摇到的铜币
function CashCow_AddAward(pkg)

    local info = GameServer_pb.CMD_COINTREE_SHAKE_SC()
    info:ParseFromString(pkg)
    Log("info===="..tostring(info))
    EndWaiting()

    CashCow_CashSate(false)
    local widget = UI_GetBaseWidgetByName("CashCow")
    if widget ~= nil then
        local num = 1
        local listView = UI_GetUIListView(widget, 4670554)
        --结果显示的动画
        local function PlayAniamtion(oneLine)
            local waitTime = 0.01
            if info.szShakeResoult[num].iKnock>1 then
                UI_GetUILabelBMFont(oneLine, 4670632):setScale(3)
                UI_GetUILabelBMFont(oneLine, 4670632):setVisible(true)

                local actionArry = CCArray:create()
                local scale = CCScaleTo:create(0.15,1)
                actionArry:addObject(scale)
                local seqaction = CCSequence:create(actionArry)

                UI_GetUILabelBMFont(oneLine, 4670632):runAction(seqaction)
                waitTime = waitTime+0.15
            end   
              
            local panel = UI_GetUILayout(oneLine, 4691716)     
            
            local function ShowMessage()
                panel:setVisible(true)
            end

            local actionArry_ = CCArray:create()
            local delayTime = CCDelayTime:create(waitTime)
            actionArry_:addObject(delayTime)
            local callback = CCCallFuncN:create(ShowMessage)
            actionArry_:addObject(callback)
            local fadeIn =  CCFadeIn:create(0.3)
            actionArry_:addObject(fadeIn)

            local seqaction_ = CCSequence:create(actionArry_)

            panel:runAction(seqaction_)

            return waitTime
        end
        -- 添加抽取结果
        local function addMessage()

            if num > #info.szShakeResoult then
                CashCow_CashSate(true)
                return
            end

            local oneLine = UI_GetUILayout(widget, 4670556):clone()
            oneLine:setVisible(true)
            local panel = UI_GetUILayout(oneLine, 4691716)
            panel:setVisible(false)
            UI_GetUILabelBMFont(oneLine, 4670632):setVisible(false)
            --花费
            UI_GetUILabel(panel, 4670564):setText(info.szShakeResoult[num].iCost)
            --获得
            UI_GetUILabel(panel, 4670574):setText(info.szShakeResoult[num].iGet)
            --暴击
            if info.szShakeResoult[num].iKnock>1 then
                UI_GetUILabelBMFont(oneLine, 4670632):setText(FormatString("CrashCowCrit",info.szShakeResoult[num].iKnock))
            end
            listView:pushBackCustomItem(oneLine)

            local waitTime = PlayAniamtion(oneLine)

            print("waitTime ==========================="..waitTime)

            local actionArry = CCArray:create()
            local delayTime = CCDelayTime:create(0.01)
            actionArry:addObject(delayTime)
            local callback = CCCallFuncN:create(CashCow_listViewJump)
            actionArry:addObject(callback)
            local delayTime_ = CCDelayTime:create(waitTime+0.4)
            actionArry:addObject(delayTime_)
            local callback_ = CCCallFuncN:create(addMessage)
            actionArry:addObject(callback_)
            local seqaction = CCSequence:create(actionArry)

            listView:runAction(seqaction)
            
            num = num+1
        end

        addMessage()
    end

    CashCow_RefreshData(info.detail)
end
--移动到最下一个数据
function CashCow_listViewJump()
    local widget = UI_GetBaseWidgetByName("CashCow")
    if widget ~= nil then
        local listView = UI_GetUIListView(widget, 4670554)
        listView:jumpToBottom()
    end
end

--摇
function CashCow_Konck(konckType)
    local tab = GameServer_pb.CMD_COINTREE_SHAKE_CS();
    tab.enType = konckType
    Packet_Full(GameServer_pb.CMD_COINTREE_SHAKE, tab);
    ShowWaiting()
end
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_COINTREE_SHAKE_QUERY, "CashCow_CreateLayer" );
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_COINTREE_SHAKE, "CashCow_AddAward" );