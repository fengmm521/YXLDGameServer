function LegionShopLayout_Create()
    Packet_Cmd(GameServer_pb.CMD_OPEN_LEGIONSHOP);
    ShowWaiting()
    --LegionShopLayout_InitData()
end
function LegionShopLayout_InitData(info)
    local widget = UI_GetBaseWidgetByName("LegionShop")
    if widget == nil then
        widget = UI_CreateBaseWidgetByFileName("LegionShop.json")
    end
    UI_GetUILayout(widget, 11):addTouchEventListener(PublicCallBackWithNothingToDo)
    UI_GetUIButton(widget, 12):addTouchEventListener(UI_ClickCloseCurBaseWidgetWithNoAction)

    LegionShopLayout_RefreshData(info)

end
function LegionShopLayout_RefreshData(info) 
    local widget = UI_GetBaseWidgetByName("LegionShop")
    if widget == nil then
        return
    end

    local layout = UI_GetUIImageView(widget, 1113)
    local listView = UI_GetUIListView(layout, 1001)
    listView:removeAllItems()
    local allGoods = nil
    --兑换物品的回调
    local function buy(sender , eventType)
        if eventType==TOUCH_EVENT_ENDED then
            local tag = tolua.cast(sender,"ImageView"):getTag()-1110
            Shop_BuySomething(info.szGoodList[tag].iGoodId,tag-1,LegionShopLayout_buy,9)
        end
    end
    --初始化兑换列表
    for i=1,#info.szGoodList,1 do
        if(i%2==1) then
            allGoods = UI_GetUILayout(widget, 1007):clone()
            listView:pushBackCustomItem(allGoods)
            allGoods:setVisible(true)

            UI_GetUIImageView(allGoods, 20):setVisible(false)
        end
        --显示数据的框
        local itemBox = nil
        if(i%2==1) then
            itemBox = UI_GetUIImageView(allGoods, 21)
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
        UI_GetUILabel(itemBox, 1):setText(data.m_name)
        --消耗
        UI_GetUILabel(itemBox:getChildByTag(3), 2):setText(goodData.m_price)
        UI_GetUILabel(itemBox:getChildByTag(3), 2):enableStroke()

        if info.iContribute < goodData.m_price then
            UI_GetUILabel(itemBox:getChildByTag(3), 2):setColor(ccc3(230,0,0))
        end

        if info.szGoodList[i].bIsSale == true then
            UI_GetUILayout(itemBox, 122121):setVisible(true)
            itemBox:setTouchEnabled(false)
        end
            
        if info.szGoodList[i].bCan == false then
            UI_GetUILabel(itemBox, 4):setText(FormatString("Legion_BuyLevel",info.szGoodList[i].iCanBuyLevel))
            UI_GetUILabel(itemBox, 4):setVisible(true)
            UI_GetUIImageView(itemBox, 3):setVisible(false)
            itemBox:setTouchEnabled(false)
        end

    end
    --更新时间
    if info.bIsToday == true then
        UI_GetUILabel(layout, 4607228):setText(FormatString("TodayUpdate",info.iUpdateMoment/3600))
    else
        UI_GetUILabel(layout, 4607228):setText(FormatString("TomorrowUpdate",info.iUpdateMoment/3600))
    end
    --更新按钮
    local function UserUpdata(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            LegionShopLayout_RefreshList()
        end
    end
    UI_GetUIButton(layout, 4607230):addTouchEventListener(UserUpdata)

    --ShopRefreshData(4776444)


    LegionBuildingLayout_RefreshData(UI_GetUILayout(widget, 1002),info.iContribute)
    
end
--兑换界面
--function LegionShopLayout_BuySomething(szShopList,callBack,costType)
--    local widget = UI_CreateBaseWidgetByFileName("ShopItemInfo.json")
--    local image = UI_GetUILayout(widget, 4624741)
    
--    --local goodData = GetGameData(DataFileHonorGoods, info.szGoodList[i].iGoodId, "stHonorGoodsData")
--    local data = GetGameData(DataFileItem, szShopList.item.iItemID, "stItemData");
--    local itemNumInBag = GetLocalPlayer():GetPlayerBag():GetItemCountByItemId(szShopList.item.iItemID)

--    if costType ~= nil then
--        UI_GetUIImageView(widget:getChildByTag(4624741), 4624894):loadTexture("Common/ico_jinbi_0"..costType..".png")
--    end
--    --icon
--    local icon = UI_ItemIconFrame(UI_GetItemIcon(data.m_icon),data.m_quality)
--    UI_GetUIImageView(image, 4624744):addChild(icon)
--    --名字
--    UI_GetUILabel(image, 4624755):setText(data.m_name)
--    --拥有数量
--    UI_GetUILabel(image,4624747):setText(FormatString("HaveNum",itemNumInBag))
--    if (szShopList.item.iItemID>=4001 and szShopList.item.iItemID<=4123) then --喜好品和装备等有属性的道具
--        --属性
--        UI_GetUILabel(image, 4624750):setText(data.m_attinfo)
--        --描述
--        local desc = UI_GetUILabel(UI_GetUILayout(image, 4624900), 4624898)
--        desc:setText(data.m_desc)
--        local temp = #data.m_desc/39 - #data.m_desc/39%1 
--        desc:setPosition(ccp(0,(temp+1)*27-81))
--    else --没有属性的道具
--    --描述
--       UI_GetUILabel(image, 4624750):setText(data.m_desc)
--    end
--    --消耗
--    UI_GetUILabel(image,4624896):setText(100)
--    Shop_SetLabelColor(UI_GetUILabel(image,4624896),20,100)
--    --购买？件
--    UI_GetUILabel(image,4624889):setText(FormatString("BuyNum",szShopList.item.iCount))
--    --买
--    local function buy ( sender,eventType )
--        if eventType==TOUCH_EVENT_ENDED then
--            callBack(szShopList)
--            UI_CloseCurBaseWidget()
--        end
--    end
--    UI_GetUIButton(image, 4624752):addTouchEventListener(buy)
--end
function LegionShopLayout_buy(index) 
    
    local tab = GameServer_pb.CMD_SELECT_LEGIONSHOP_CS();
    tab.iIndex = index
	Packet_Full(GameServer_pb.CMD_SELECT_LEGIONSHOP, tab);

    ShowWaiting()
end

function  LegionShopLayout_RefreshList()
    Packet_Cmd(GameServer_pb.CMD_REFRESH_LEGIONSHOP);
    ShowWaiting()
end
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_OPEN_LEGIONSHOP, "LEGIONSHOP" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_REFRESH_LEGIONSHOP, "REFRESH_LEGIONSHOP" )
function LEGIONSHOP(pkg)
    EndWaiting();
	local tmp = GameServer_pb.CMD_OPEN_LEGIONSHOP_SC();
	tmp:ParseFromString(pkg)
	Log("*********"..tostring(tmp))

    LegionShopLayout_InitData(tmp)
end
function REFRESH_LEGIONSHOP(pkg)
    EndWaiting();
	local tmp = GameServer_pb.CMD_REFRESH_LEGIONSHOP_SC();
	tmp:ParseFromString(pkg)
	Log("*********"..tostring(tmp))

    LegionShopLayout_RefreshData(tmp)
end