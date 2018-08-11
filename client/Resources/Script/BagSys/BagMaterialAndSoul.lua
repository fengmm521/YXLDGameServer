
--创建背包
function BagLayer_create()
    local widget = UI_CreateBaseWidgetByFileName("UIExport/DemoLogin/BagLayer.json")
    if (widget == nil)then
        Log("BagMaterialAndSoul_create error")
    end

    g_BagLayerWidget = widget
    UI_GetUIButton(widget, 1013):addTouchEventListener(UI_ClickCloseCurBaseWidgetWithNoAction);
    UI_GetUILayout(widget, 4742868):addTouchEventListener(PublicCallBackWithNothingToDo)
    --左边三个标签页
    --默认为显示全部
    --UI_GetUIButton(widget, 1009):addTouchEventListener(BagLayer_CallBack)
    --UI_GetUIButton(widget, 1009):setZOrder(3)
    --UI_GetUIButton(widget, 1009):setBrightStyle(BRIGHT_HIGHLIGHT)

    --UI_GetUIButton(widget, 1010):addTouchEventListener(BagLayer_CallBack)
    --UI_GetUIButton(widget, 1011):addTouchEventListener(BagLayer_CallBack)

    g_BagLayer_showType =2201
    BagLayer_Refresh(widget);   
    BagLayer_ButtonState()

    local function changeLayer(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            local tag = tolua.cast(sender,"Button"):getTag()
            g_BagLayer_showType = tag
            BagLayer_ButtonState()
            BagLayer_Refresh(widget);  
        end
    end
    for i=2201,2206,1 do
        UI_GetUIButton(widget, i):addTouchEventListener(changeLayer)
    end
end
--显示物品说明
function BagLayer_ShowMessage(tag,kind)
    local widget = UI_GetBaseWidgetByName("BagLayer");
    local bagList = GetLocalPlayer():GetPlayerBag():GetBagList()
    local equipList = GetLocalPlayer():GetEquipBags():GetEquipList();
    if (widget and (bagList:size() > 0 or equipList:size() > 0 ))then
        local data = nil
        local addAtt = ""
        local icon = nil 
        if kind == 0 then --道具
            local bagItem = bagList[tag];
            data = bagItem:getItemData();
            icon = UI_ItemIconFrame(UI_GetItemIcon(data.m_icon, -1), data.m_quality)
        else--装备   
            print("tag=============="..tag)       
            data = equipList[tag]:GetData();
            addAtt = GetAttNameAndValue(equipList[tag]:getAddAttribute().lifeattId,equipList[tag]:getAddAttribute().lifeattValue)
            icon = UI_ItemIconFrame(UI_GetEquipIcon(data.m_icon), data.m_quality)
        end
        local layout = UI_GetUILayout(widget, 2001);
        if ( UI_GetUIImageView(layout, 4626402):getChildByTag(1111) ) then         
            UI_GetUIImageView(layout, 4626402):removeChildByTag(1111)
        end
        UI_GetUIImageView(layout, 4626402):addChild(icon,1,1111)
        UI_GetUILabel(layout, 1002):setText(data.m_name)
        print("data.m_quality======================="..data.m_quality)
        UI_GetUILabel(layout, 1002):setColor(Util_GetColor(data.m_quality))
        UI_GetUILabel(layout, 1002):enableStroke()

        if #data.m_name>18 then
            UI_GetUILabel(layout, 1002):setFontSize(20)
        elseif #data.m_name>24 then
            UI_GetUILabel(layout, 1002):setFontSize(18)
        elseif #data.m_name>27 then
            UI_GetUILabel(layout, 1002):setFontSize(16)
        else
            UI_GetUILabel(layout, 1002):setFontSize(24)
        end
        --UI_GetUILabel(layout, 1002):setColor(UI_GetColor(data.m_quality))

        if kind == 0 then--道具
            UI_GetUILabel(layout, 1003):setText(FormatString("IteNum1")..bagList[tag]:GetInt(EBagItem_Count)..FormatString("IteNum2"))           
            UI_GetUILabel(layout, 1005):setText(data.m_sellMoney)
        else
            UI_GetUILabel(layout, 1005):setText(data.m_sellCount)
            local limitStr= FormatString("BagLayer_limit_"..data.m_zhiyeLimit)

            UI_GetUILabel(layout, 1003):setText(FormatString("BagLayer_limit1",limitStr).."\n"..FormatString("BagLayer_limit2",data.m_lvlLimit))
        end


        if ((data.m_ID>=4001 and data.m_ID<=4123) or( data.m_ID>=200000 and data.m_ID<=230000 ) ) then --喜好品有属性的道具
            if kind == 0 then
                local label = UI_GetUILabel(layout, 1004)
                label:setVisible(true)
                label:setText(data.m_attinfo)
                UI_GetUILayout(layout, 1701):setVisible(false)
            else
                UI_GetUILabel(layout, 1004):setVisible(false)
                local layout = UI_GetUILayout(layout, 1701)
                layout:setVisible(true)
                local label1 = UI_GetUILabel(layout, 1101)
                local label2 = UI_GetUILabel(layout, 1102)
                --基础属性
                label2:setText(data.m_attinfo)
                local line = 1
                local start = 0
                while string.find(data.m_attinfo, "\n",start+1) ~= nil do
                    line = line +1
                    print(string.find(data.m_attinfo, "\n")) 
                    start = string.find(data.m_attinfo, "\n",start+1)
                end

                local labelSize = Label:create()
                labelSize:setText("123")
                labelSize:setFontSize(20)

                UI_GetUILabel(layout, 1202):setText(addAtt)
                UI_GetUILabel(layout, 1201):setPosition(ccp(label1:getPositionX(),label1:getPositionY() - (line+0.5)*labelSize:getSize().height))
                UI_GetUILabel(layout, 1202):setPosition(ccp(label2:getPositionX(),label2:getPositionY() - (line+0.5)*labelSize:getSize().height))

                
                UI_GetUILabel(layout, 1302):setText("this is test \nthis is test \nthis is test ")
                UI_GetUILabel(layout, 1301):setPosition(ccp(label1:getPositionX(),label1:getPositionY() - (line+2)*labelSize:getSize().height))
                UI_GetUILabel(layout, 1302):setPosition(ccp(label2:getPositionX(),label2:getPositionY() - (line+2)*labelSize:getSize().height))
                
                UI_GetUILabel(layout, 1301):setVisible(false)
                UI_GetUILabel(layout, 1302):setVisible(false)
            end
            --描述
            local desc = UI_GetUILabel(layout, 4626397)
            desc:setText(data.m_desc)
        else --没有属性的道具
        --描述               
            UI_GetUILayout(layout, 1701):setVisible(false)
            UI_GetUILabel(layout, 1004):setVisible(true)
            UI_GetUILabel(layout, 1004):setText(data.m_desc)
            UI_GetUILabel(layout, 4626397):setText("")
        end
        local function ClickSell(sender, eventType)
            if (eventType == TOUCH_EVENT_ENDED)then
                BagLayer_Sell(tag,kind)
            end
        end
        --卖出
        UI_GetUILabel(layout, 1006):addTouchEventListener(ClickSell)
        UI_GetUILabel(layout, 1006):setPosition(ccp(-81,-211))

        if data.m_ID>19000 and data.m_ID<19998 then
            local function ItemUse(sender, eventType)
                if (eventType == TOUCH_EVENT_ENDED)then
                    local tab = GameServer_pb.Cmd_Cs_ItemUse();	
                    print(bagList[tag]:GetInt(EBagItem_ObjId))	
			        tab.dwItemObjectID = bagList[tag]:GetInt(EBagItem_ObjId);	
			        Packet_Full(GameServer_pb.CMD_ITEM_USE, tab);
                    ShowWaiting()
                end
            end
            --礼包使用
            UI_GetUIButton(layout, 1007):addTouchEventListener(ItemUse)
            UI_GetUIButton(layout, 1007):setTitleText(FormatString("BagLayer_Use"))
            UI_GetUIButton(layout, 1007):setVisible(true)
        elseif kind~=0 then
            local function EquipSell(sender, eventType)
                if (eventType == TOUCH_EVENT_ENDED)then
                    BagLayer_SellEquip()
                end
            end
            --一键出售
            UI_GetUIButton(layout, 1007):addTouchEventListener(EquipSell)
            UI_GetUIButton(layout, 1007):setTitleText(FormatString("BagLayer_Sell"))
            UI_GetUIButton(layout, 1007):setVisible(true)
        else
            --是否显示“使用”或者“转换”
            UI_GetUIButton(layout, 1007):setVisible(false)
            UI_GetUILabel(layout, 1006):setPosition(ccp(0,-211))
        end
    end
end
--更新背包的内容
function BagLayer_Refresh(widget)
    if widget == nil then
        return 
    end
    local bag = GetLocalPlayer():GetPlayerBag();
    local equip = GetLocalPlayer():GetEquipBags();
    local bagList = bag:GetBagList()
    local EquipList = equip:GetEquipList()
    --将道具信息显示设为不可见
    local layoutMsg = UI_GetUIImageView(widget, 2001)
    layoutMsg:setVisible(false);
    UI_GetUILabel(layoutMsg, 1006):setTouchEnabled(false)

    local listView = UI_GetUIListView(widget:getChildByTag(2002), 1008);
    listView:removeAllItems();
    listView:jumpToTop()

    local listViewSize = UI_GetUIListView(g_BagLayerWidget:getChildByTag(2002), 1008):getSize()

    local function ClickIcon(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            local tag = tolua.cast(sender,"ImageView"):getTag()
            if tag<11000 then        
                BagLayer_ShowMessage(tag-1300,0)
            else             
                BagLayer_ShowMessage(tag-11300,1)
            end
            --print(tag)
        end
    end

    local layout=nil
    local IDMin=nil;
    local IDMax=nil
    --设置过滤器
    if g_BagLayer_showType~=nil and g_BagLayer_showType == 2201 then--全部
        IDMin = 0
        IDMax = 9999999
    elseif g_BagLayer_showType~=nil and g_BagLayer_showType == 2202 then--喜好品
        IDMin = 4000
        IDMax = 4300
    elseif g_BagLayer_showType~=nil and g_BagLayer_showType == 2203 then--装备
        IDMin = 200001
        IDMax = 299999
    elseif g_BagLayer_showType~=nil and g_BagLayer_showType == 2204 then--武魂
        IDMin = 13000
        IDMax = 13100
    elseif g_BagLayer_showType~=nil and g_BagLayer_showType == 2205 then--神兽魂
        IDMin = 10002
        IDMax = 10020
    elseif g_BagLayer_showType~=nil and g_BagLayer_showType == 2206 then--其他
        IDMin = 19000
        IDMax = 19999
    else
        return
    end
    --添加图标---------------------------------------------------------------
    -------道具--
    local tableTempItem = {}
    local tableItem={}--符合要求的道具的列表
    local numItem = 0 --符合要求的道具或者装备的数量
    if IDMin  ~= 200001 then --如果不是只是显示装备
        for i=0, bagList:size()-1 do--创建所有道具图标并将其保存
            local data = bag:GetBagItemByPos(i):getItemData();

            if data.m_ID>=IDMin and data.m_ID<=IDMax then
                tableItem[numItem+1] = i
                local icon = UI_ItemIconFrame(UI_GetItemIcon(data.m_icon, bagList[i]:GetInt(EBagItem_Count)), data.m_quality)
                icon:setTouchEnabled(true)
                icon:addTouchEventListener(ClickIcon)
                tableTempItem[numItem+1] = icon
                icon:setTag(i + 1300)

                numItem = numItem +1
            end
       end
   end
   -------------
   --------装备
   local tableTempEquip = {}
   local tableEquip={}--符合要求的道具的列表
   local numEquip = 0
   if IDMin == 200001 or  IDMin == 0 then -- 如果要显示装备
        --print("EquipList:size()=========================="..EquipList:size())
        for i=0, EquipList:size()-1 do--创建所有道具图标并将其保存
            if EquipList[i] ~= nil then    
                --print("i=========================="..i)        
                local data = EquipList[i]:GetData();
            
                tableEquip[numEquip+1] = i
                local icon = UI_ItemIconFrame(UI_GetEquipIcon(data.m_icon),data.m_quality)
                icon:setTouchEnabled(true)
                icon:addTouchEventListener(ClickIcon)
                tableTempEquip[numEquip+1] = icon
                icon:setTag(i + 11300)

                numEquip = numEquip +1
            end
       end 
   end
   ---------
   UI_GetUIImageView(widget, 4702501):setVisible(numItem+numEquip <= 0)
    local function GetLast()--寻找下一个应该放置的图标/排序
        local tag={[1]=0,[2]=0}--道具总列表中符合要求的物品在的位置
        local isHaveItem = false
        local isHaveEquip = false
        local tagItem = 0
        local tagEquip = 0
        --首先从道具中查找
        for i=0,numItem-1,1 do
            if tableTempItem[tagItem+1] == "NULL" or tableTempItem[tagItem+1] ==nil then
                tagItem = tagItem+1
            else 
                if tableTempItem[i+1] ~= "NULL" and tableTempItem[i+1] ~= nil then
                    local TagData = bagList[tableItem[tagItem+1]]:getItemData()
                    local IData = bagList[tableItem[i+1]]:getItemData()
                    if TagData.m_quality < IData.m_quality then--比较品阶，高阶在前
                        tagItem = i
                    elseif TagData.m_quality == IData.m_quality and TagData.m_ID > IData.m_ID then--同阶比较ID，ID小在前
                        tagItem = i
                    end
               end
            end
        end
        --再从装备列表里找
        for i=0,numEquip-1,1 do
            if tableTempEquip[tagEquip+1] == nil or tableTempEquip[tagEquip+1] == "NULL" then
                tagEquip = tagEquip+1
            else 
                if tableTempEquip[i+1] ~= nil and tableTempEquip[i+1] ~= "NULL"  then
                    local TagData = EquipList[tableEquip[tagEquip+1]]:GetData()
                    local IData = EquipList[tableEquip[i+1]]:GetData()
                    if TagData.m_quality < IData.m_quality then--比较品阶，高阶在前
                        tagEquip = i
                    elseif TagData.m_quality == IData.m_quality and TagData.m_ID > IData.m_ID then--同阶比较ID，ID小在前
                        tagEquip = i
                    end
                end
            end
        end
        
        --对比装备和道具，谁该放在前面
        if tagItem>=#tableTempItem then
            tag[1] = tagEquip
            tag[2] = 1
        elseif tagEquip>=#tableTempEquip then
            tag[1] = tagItem
            tag[2] = 0
        else
            local item = bagList[tableItem[tagItem+1]]:getItemData()
            local equip = EquipList[tableEquip[tagEquip+1]]:GetData()
            if item.m_quality ~= equip.m_quality then
                if item.m_quality < equip.m_quality then--比较品阶，高阶在前
                    tag[1] = tagEquip
                    tag[2] = 1
                else
                    tag[1] = tagItem
                    tag[2] = 0
                end
            else
                if item.m_ID > equip.m_ID then--同阶比较ID，ID小在前
                    tag[1] = tagEquip
                    tag[2] = 1
                else
                    tag[1] = tagItem
                    tag[2] = 0
                end
            end
        end
        return tag
    end

    -- 排序
    local IconNum = 0
    for i=0, numItem+numEquip-1,1 do 
        local lastTag = GetLast()
        local icon = nil
        --print("lastTag[1]+1======================="..lastTag[1]+1)
        if lastTag[2]==0 then
        --道具
            icon = tableTempItem[lastTag[1]+1]
            tableTempItem[lastTag[1]+1]="NULL"
        else
        --装备
            icon = tableTempEquip[lastTag[1]+1]
            tableTempEquip[lastTag[1]+1]="NULL"
        end
        if icon ~= nil then
            if IconNum%4 == 0 then
                layout = Layout:create();
                layout:setSize(CCSizeMake(listViewSize.width,100));
                layout:setTouchEnabled(false)
                listView:pushBackCustomItem(layout)
            end

            icon:setPosition(ccp(i%4*108+50,50))

            layout:addChild(icon)
            IconNum = IconNum +1
        else
          -- print("-------------------------------------------NULL")
        end
        --显示第一个物品的数据
        if IconNum==1  then
            layoutMsg:setVisible(true);
            UI_GetUIImageView(layoutMsg, 1006):setTouchEnabled(true)
            if lastTag[2] == 0 then 
                BagLayer_ShowMessage(tableItem[lastTag[1]+1],0)
            else
                BagLayer_ShowMessage(tableEquip[lastTag[1]+1],1)   
            end
        end
    end
    ---------------------------------------------------------------------
    EndWaiting()
end
--出售物品
function BagLayer_Sell(tag , kind)
    local widget = UI_CreateBaseWidgetByFileName("UIExport/DemoLogin/BagSell.json")

    local bagList = GetLocalPlayer():GetPlayerBag():GetBagList()
    local equipList = GetLocalPlayer():GetEquipBags():GetEquipList();
    
    local bagItem = nil
    local data = nil
    if kind ==0 then
        data = bagList[tag]:getItemData()
        bagItem = bagList[tag]
    else
        data = equipList[tag]:GetData()
    end
    local layout = widget:getChildByTag(4517109)

    --描边   
    UI_GetUILabel(layout, 4517132):enableStroke()
    UI_GetUILabel(layout, 4517115):enableStroke()
    UI_GetUILabel(layout, 4517124):enableStroke()
    UI_GetUILabel(layout, 4517114):enableStroke()
    UI_GetUILabel(layout, 4517117):enableStroke()
    --出售数量
    local sellNum = 1
    --名字
    UI_GetUILabel(layout, 4517132):setText(data.m_name)
    UI_GetUILabel(layout, 4517132):setColor(Util_GetColor(data.m_quality))
   
    local icon = nil 
    if kind ==0 then
        --数量
        UI_GetUILabel(layout, 4517115):setText(bagItem:GetInt(EBagItem_Count))
        --单价
        UI_GetUILabel(layout, 4517124):setText(bagItem:getItemData().m_sellMoney)
        --出售数量
        UI_GetUILabel(layout, 4517114):setText(sellNum.."/"..bagItem:GetInt(EBagItem_Count))      
        --出售总价
        UI_GetUILabel(layout, 4517117):setText(bagItem:getItemData().m_sellMoney)
        icon = UI_ItemIconFrame(UI_GetItemIcon(data.m_icon, -1), data.m_quality)
    else
        --单价
        UI_GetUILabel(layout, 4517124):setText(data.m_sellCount)
        --限制
        local limitStr= FormatString("BagLayer_limit_"..data.m_zhiyeLimit)
        UI_GetUILabel(layout, 4517115):setText(FormatString("BagLayer_limit1",limitStr).."\n"..FormatString("BagLayer_limit2",data.m_lvlLimit))     
        --出售数量
        UI_GetUILabel(layout, 4517114):setText("1/1")   
        --出售总价
        UI_GetUILabel(layout, 4517117):setText(data.m_sellCount)
        UI_GetUILabel(layout, 4517146):setVisible(false)
        UI_GetUILabel(layout, 4517140):setVisible(false)

        UI_GetUILabel(layout, 4517115):setAnchorPoint(ccp(0,0.5))
        UI_GetUILabel(layout, 4517115):setPosition(ccp(-40,130))
        print("data.m_quality============"..data.m_quality)
        icon = UI_ItemIconFrame(UI_GetEquipIcon(data.m_icon), data.m_quality)
    end
    local pointx,pointy = UI_GetUIImageView(layout, 4517111):getPosition()
    icon:setPosition(ccp(pointx,pointy))
    layout:addChild(icon,10,10)
    --“出售”--------------------------------------------------------------
    local function Sell(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            if kind == 0 then
                local tab = GameServer_pb.CMD_ITEM_SELLITEM_CS();
			    tab.iItemBaseId = bagItem:getItemData().m_ID;
                tab.iItemCount = sellNum
			    Packet_Full(GameServer_pb.CMD_ITEM_SELLITEM, tab);
            else
                local tab = GameServer_pb.CMD_EQUIP_SELLBYPOS_CS();
				
			    tab.iEquipPos = equipList[tag]:GetInt(EEquip_Pos);	
			    Packet_Full(GameServer_pb.CMD_EQUIP_SELLBYPOS, tab);
            end


            UI_CloseCurBaseWidget()

            ShowWaiting()
        end
    end
    --出售
    UI_GetUIButton(layout, 4517113):addTouchEventListener(Sell)
    --增加或者减少数量----------------------------------------------------
    local function AddNum(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            local tag = tolua.cast(sender,"Button"):getTag()
            if tag == 4517126 and sellNum > 1 then--减
                sellNum = sellNum - 1       
            elseif tag == 4517112 and sellNum < bagItem:GetInt(EBagItem_Count) then -- 加
                sellNum = sellNum + 1
            elseif tag == 4517125 then--最大
                sellNum = bagItem:GetInt(EBagItem_Count)
            else 
                print("addNum:tag == "..tag.."|||+ 4517112|-4517126|max 4517125")
            end
             --数量
            UI_GetUILabel(layout, 4517114):setText(sellNum.."/"..bagItem:GetInt(EBagItem_Count))

            --出售总价
            UI_GetUILabel(layout, 4517117):setText(bagItem:getItemData().m_sellMoney*sellNum)
        end
    end
    --减
    UI_GetUIButton(layout, 4517126):addTouchEventListener(AddNum)
    --加
    UI_GetUIButton(layout, 4517112):addTouchEventListener(AddNum)
    --最大
    UI_GetUIButton(layout, 4517125):addTouchEventListener(AddNum)
    if kind == 1 then
        --减
        UI_GetUIButton(layout, 4517126):setTouchEnabled(false)
        --加
        UI_GetUIButton(layout, 4517112):setTouchEnabled(false)
        --最大
        UI_GetUIButton(layout, 4517125):setTouchEnabled(false)
    end
end

function BagLayer_ButtonState()
    local widget = UI_GetBaseWidgetByName("BagLayer") 
    for i=2201,2206,1 do
        if i==g_BagLayer_showType then
            UI_GetUIButton(widget, i):setButtonEnabled(false)
        else
            UI_GetUIButton(widget, i):setButtonEnabled(true)
        end
    end
end

-- 一键出售
function BagLayer_SellEquip()
    local widget = UI_CreateBaseWidgetByFileName("BagLayerSellEquip.json")
    UI_GetUIListView(widget, 1124828173):addTouchEventListener(PublicCallBackWithNothingToDo)
    --出售类型
    local sellTab={
        [1]=true,
        [2]=false,
        [3]=false
    }

    local function ChengeChooseState(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            local tag = tolua.cast(sender,"CheckBox"):getTag()
            sellTab[tag-1000] = sellTab[tag-1000]==false
        end
    end

    --绿色装备
    local ChieckBoxG = UI_GetUICheckBox(widget, 1001)
    --蓝色装备
    local ChieckBoxB = UI_GetUICheckBox(widget, 1002)
    --紫色装备
    local ChieckBoxP = UI_GetUICheckBox(widget, 1003)

    ChieckBoxG:addTouchEventListener(ChengeChooseState)
    ChieckBoxB:addTouchEventListener(ChengeChooseState)
    ChieckBoxP:addTouchEventListener(ChengeChooseState)
    ChieckBoxG:setSelectedState(true)
    ChieckBoxB:setSelectedState(false)
    ChieckBoxP:setSelectedState(false)
    --一键出售
    local function sendMessage()
        UI_ClickCloseCurBaseWidgetWithNoAction(widget, TOUCH_EVENT_ENDED)
        local tab = GameServer_pb.CMD_EQUIP_SELLBYQUALITY_CS()
        tab.bSellGreen = sellTab[1]
        tab.bSellBlue = sellTab[2]
        tab.bSellPurple = sellTab[3]
        Packet_Full(GameServer_pb.CMD_EQUIP_SELLBYQUALITY,tab)
        ShowWaiting()
    end

    local function sell(sender,eventType)
        
        if eventType == TOUCH_EVENT_ENDED then
            if sellTab[1]==false and sellTab[2]== false and sellTab[3]==false then
                local tab = {}
                tab.info = FormatString("BagLayer_ChooseSellKind")
                tab.msgType = EMessageType_Middle
                Messagebox_Create(tab) 
            else
                local str = FormatString("BagLayer_SellKindInfo_1")

                local isHaveKind = false
                for i=1,#sellTab,1 do
                    if sellTab[i] == true then
                        if isHaveKind == false then
                            str = str..FormatString("BagLayer_SellKind_"..i)
                            isHaveKind = true
                        else
                            str = str..FormatString("BagLayer_SellKind")..FormatString("BagLayer_SellKind_"..i)
                        end
                    end
                end
                str = str..FormatString("BagLayer_SellKindInfo_2")

                local tab = {}
                tab.info = str
                tab.msgType = EMessageType_LeftRight
                tab.leftFun = sendMessage
                Messagebox_Create(tab) 
            end
        end
    end
    UI_GetUIButton(widget, 1005):addTouchEventListener(sell)
    UI_GetUIButton(widget, 1004):addTouchEventListener(UI_ClickCloseCurBaseWidgetWithNoAction)
end
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_ITEM_USE, "ITEM_USE" );
function ITEM_USE(pkg)
    EndWaiting();
	local info = GameServer_pb.CMD_ITEM_USE_SC();
	info:ParseFromString(pkg)
	Log("*********"..tostring(info))

    ActivityID_ShowItem(info.awardresoult,"BagLayer_UseItemAward")
end
