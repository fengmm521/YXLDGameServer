function Legion_CreateNewLegionSend(name,imageID)
    local tab = GameServer_pb.CMD_CREATE_LEGION_CS()
    tab.strLegionName = name
    tab.iLegionIconId = imageID
    Packet_Full(GameServer_pb.CMD_CREATE_LEGION,tab)

    ShowWaiting()
end

function LegionCreateNewLegion()
    local widget = UI_CreateBaseWidgetByFileName("CreatingLegionBase.json")
    UI_GetUILayout(widget, 1124781090):addTouchEventListener(PublicCallBackWithNothingToDo)
    UI_GetUIButton(widget, 2):addTouchEventListener(UI_ClickCloseCurBaseWidgetWithNoAction)

    --输入军团名
    local function enter(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            UI_GetUITextField(widget:getChildByTag(3), 1):attachWithIME()
        end
    end
    UI_GetUIImageView(widget, 3):setTouchEnabled(true)
    UI_GetUIImageView(widget, 3):addTouchEventListener(enter)

    LegionCreateNewLegion_setLegionImage(1001)
    
    --更换军团图标
    local function changeLegionImage(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            LegionCreateNewLegion_ImageList(LegionCreateNewLegion_setLegionImage)  
        end
    end
    UI_GetUIButton(widget, 5):addTouchEventListener(changeLegionImage)

    --消耗铜币/元宝数量
    UI_GetUILabel(widget:getChildByTag(4560856), 1211):setText(g_CreatingLegionSliver)
    --创建军团
    local function create(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            Legion_CreateNewLegionSend(UI_GetUITextField(widget:getChildByTag(3), 1):getStringValue(),g_LegionCreateNewLegion_ImageID)
        end
    end
    UI_GetUIButton(widget, 4581605):addTouchEventListener(create)
end
--设置军团图标
function LegionCreateNewLegion_setLegionImage(id)
    local widget = UI_GetBaseWidgetByName("CreatingLegionBase")
    if widget == nil then
        return 
    end
    g_LegionCreateNewLegion_ImageID = id
    print("-------------------------id========="..id)
    UI_GetUIImageView(widget:getChildByTag(4560860), 4560862):loadTexture("Icon/Skill/"..id..".png")
end
--图标列表
function LegionCreateNewLegion_ImageList(callfunc)
    local widget = UI_CreateBaseWidgetByFileName("ChangeHead.json")
    --UI_GetUIImageView(widget, 4535114):addTouchEventListener(PublicCallBackWithNothingToDo)
    
    local listView = UI_GetUIListView(widget, 4532522)

    local layout = UI_GetUILayout(widget, 1124781096):clone()
    layout:setVisible(true)
    listView:pushBackCustomItem(layout)
    --图标回调
    function chickIcon(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            local image = tolua.cast(sender,"ImageView")
            callfunc(image:getTag())
            UI_CloseCurBaseWidget()
        end
    end
    layout = nil
    local listViewSize = listView:getSize()
    --放置图标
    for i=1,52,1 do
        if (i-1)%5==0 then
            layout = Layout:create()
            layout:setSize(CCSizeMake(listViewSize.width,100));
            layout:setTouchEnabled(false)
            listView:pushBackCustomItem(layout)
        end
        local icon = ImageView:create();
        icon:loadTexture("Common/Icon_Bg_007.png")
        icon:addChild(UI_GetSkillIcon(1000+i),0,1)
        icon:setTouchEnabled(true)
        icon:addTouchEventListener(chickIcon)
        icon:setTag(i + 1000)
        icon:setPosition(ccp((i-1)%5*102+50,50))
        layout:addChild(icon)
    end

end