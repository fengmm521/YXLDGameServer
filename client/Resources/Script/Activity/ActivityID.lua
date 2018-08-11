function ActivityID_Create()
    local widget = UI_CreateBaseWidgetByFileName("EnterActivityID.json")
    UI_GetUILayout(widget, 4724018):addTouchEventListener(PublicCallBackWithNothingToDo)

    --退出
    UI_GetUIButton(widget, 4724037):addTouchEventListener(UI_ClickCloseCurBaseWidgetWithNoAction)
    --发送
    local function send(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            local str = UI_GetUITextField(widget, 4724030):getStringValue()

            if str ~= nil and str ~="" then
                UI_ClickCloseCurBaseWidgetWithNoAction(sender, eventType)

                local tab = GameServer_pb.CMD_SEND_COVERT_CODE_CS();
	            tab.strCovertCode = str
	            Packet_Full(GameServer_pb.CMD_SEND_COVERT_CODE, tab);
                ShowWaiting()
            end
        end
    end
    --确定
    UI_GetUIButton(widget, 4724035):addTouchEventListener(send)
    --激活输入框
    local function enter(sender,eventType) 
        if eventType == TOUCH_EVENT_ENDED then
            UI_GetUITextField(widget, 4724030):attachWithIME()
        end
    end
    UI_GetUIImageView(widget, 4724028):setTouchEnabled(true)
    UI_GetUIImageView(widget, 4724028):addTouchEventListener(PublicCallBackWithNothingToDo)
    UI_GetUIImageView(widget, 4724028):addTouchEventListener(enter)
end
--获得的道具
function ActivityID_Award(pkg)
    EndWaiting();
	local info = GameServer_pb.CMD_SEND_COVERT_CODE_SC();
	info:ParseFromString(pkg)
	Log("*********"..tostring(info))

    ActivityID_ShowItem(info.converResoult)
end
function ActivityID_ShowItem(infoData,str)
    local widget = UI_CreateBaseWidgetByFileName("ActivityAward.json")
    UI_GetUILayout(widget, 4724043):addTouchEventListener(PublicCallBackWithNothingToDo)

    print("TTTTTTTTTTTTTTTTTTTTTTTTTTTTTT")
    local listView = UI_GetUIListView(widget:getChildByTag(4724045), 4724046)
    listView:removeAllItems()

	local propList = infoData.szAwardPropList
    local itemList = infoData.szAwardItemList
    local heroList = infoData.szAwardHeroList
    --当只有一个奖励时，宽度为两个奖励的宽度，因此在此填充站位的layout
    if #itemList+#propList+#heroList <2 then
        local tempLayout = UI_GetUILayout(widget, 4747081):clone()
        listView:pushBackCustomItem(tempLayout)   
    end
    print("TTTTTTTTTTTTTTTTTTTTTTTTTTTTTT")
    print(#heroList);
    for i= 1 ,#propList , 1 do
        local icon = ImageView:create()
        --经验
        local name="";
        if propList[i].iLifeAttID == 3 then
            icon:loadTexture("LifeAttIcon/3.png") 
            name = FormatString("en_LifeAtt_Exp");
        elseif propList[i].iLifeAttID == 1 then --铜钱
            icon:loadTexture("LifeAttIcon/1.png") 
            name = FormatString("en_LifeAtt_Silver");
        elseif propList[i].iLifeAttID == 4 then --修为
            icon:loadTexture("LifeAttIcon/4.png") 
            name = FormatString("en_LifeAtt_HeroExp");
        elseif propList[i].iLifeAttID == 20 then --荣誉
            icon:loadTexture("LifeAttIcon/20.png") 
            name = FormatString("en_LifeAtt_Honor");
        elseif propList[i].iLifeAttID == 17 then --体力
            icon:loadTexture("LifeAttIcon/17.png") 
            name = FormatString("en_LifeAtt_Tili");
        elseif propList[i].iLifeAttID == 2 then --元宝
            icon:loadTexture("LifeAttIcon/2.png")
            name = FormatString("en_LifeAtt_Gold"); 
        end
        local oneItem = UI_GetUILayout(widget, 4724051):clone()
        UI_GetUIImageView(oneItem, 4724053):addChild(icon,10,10)
        UI_GetUILabel(oneItem, 4724055):setText(name.."x"..propList[i].iValue)
        listView:pushBackCustomItem(oneItem)
    end
    for i=1,#itemList,1 do
        local icon = nil;
        local name ="";
        if itemList[i].iItemID>200000 and itemList[i].iItemID<299999 then--装备
            local data = GetGameData(DataFileEquip, itemList[i].iItemID, "stEquipData");
		    icon = UI_ItemIconFrame(UI_GetEquipIcon(data.m_icon),data.m_quality);
            name = data.m_name;
        else
            local data = GetGameData(DataFileItem, itemList[i].iItemID, "stItemData");
		    icon = UI_ItemIconFrame(UI_GetItemIcon(data.m_icon, 0), data.m_quality);
            name = data.m_name;
        end

		local oneItem = UI_GetUILayout(widget, 4724051):clone()
        UI_GetUIImageView(oneItem, 4724053):addChild(icon,10,10)
        UI_GetUILabel(oneItem, 4724055):setText(name.."x"..itemList[i].iCount)
        listView:pushBackCustomItem(oneItem)   
    end
    for i=1,#heroList,1 do
        local name = "";
        if heroList[i].iCount > 1 then
            --英雄转换为武魂数量绝对大于1，因此显示出来的是武魂
            itemData = GetGameData(DataFileItem, heroList[i].iSoulId, "stItemData");
            icon = UI_ItemIconFrame(UI_GetItemIcon(itemData.m_icon),itemData.m_quality)
            name = itemData.m_name
        else
            heroData = GetGameData(DataFileHeroBorn, heroList[i].iHeroID, "stHeroBornData");
            --英雄不转换为武魂，则显示英雄
            icon = UI_IconFrame(UI_GetHeroIcon(heroData.m_icon),heroData.m_heroInitStar)
            name = heroData.m_name
        end

		local oneItem = UI_GetUILayout(widget, 4724051):clone()
        UI_GetUIImageView(oneItem, 4724053):addChild(icon,10,10)
        UI_GetUILabel(oneItem, 4724055):setText(name.."x"..heroList[i].iCount)
        UI_GetUILabel(oneItem, 4724055):setFontSize(20)
        listView:pushBackCustomItem(oneItem)   
    end
    local determine = UI_GetUILayout(widget, 4724060):clone()
    UI_GetUIImageView(widget, 4724045):addChild(determine,100,100)   
    UI_GetUIButton(determine, 4724066):addTouchEventListener(UI_ClickCloseCurBaseWidgetWithNoAction)

    --调整界面显示区域以及位置
    local oneItem = UI_GetUILayout(widget, 4724051):clone()
    local height = oneItem:getSize().height*(#itemList+#propList+#heroList)
    if #itemList+#propList+#heroList <2 then
        height = oneItem:getSize().height*2
    elseif #itemList+#propList+#heroList>5 then
        height = 270
    end
    --local height = oneItem:getSize().height*(#propList)
    --if #propList <2 then
    --    height = oneItem:getSize().height*2
    --elseif #propList>5 then
    --    height = 270
    --end
    local image1 = UI_GetUIImageView(UI_GetUIImageView(widget, 4724045), 4724068)
    image1:setSize(CCSizeMake(image1:getSize().width,height+20))
    local image2 = UI_GetUIImageView(widget, 4724045)
    image2:setSize(CCSizeMake(image2:getSize().width,height+100+determine:getSize().height))

    image2:setPosition(ccp(image2:getPositionX(),320+(height+determine:getSize().height+20)/2))
    --Y坐标=listview的上下间隔+listView高度+“确定所在容器层”高度+73的基本高度
    determine:setPosition(ccp(20,-20-height-determine:getSize().height-73))
    if str ~=nil and str ~= "" then
        UI_GetUILabel(widget:getChildByTag(4724045), 4724080):setText(FormatString(str))
    end
end
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_SEND_COVERT_CODE, "ActivityID_Award" );