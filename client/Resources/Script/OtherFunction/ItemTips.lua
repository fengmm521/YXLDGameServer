function ShowItemTips(itemId)
	-- body
	local widget = UI_CreateBaseWidgetByFileName("ItemTips.json")
	local ItemData = GetGameData(DataFileItem,itemId,"stItemData")
    
    --return 
    UI_GetUIButton(widget,1):addTouchEventListener(UI_ClickCloseCurBaseWidget)

    UI_GetUILayout(widget,2):addTouchEventListener(UI_ClickCloseCurBaseWidget)

    --iteminfo
    local iteminfoparent = UI_GetUIImageView(widget,3)

    --name
    itemname = FormatString("ItemName", UI_GetStringColorNew(ItemData.m_quality), ItemData.m_name)
    local itemnamelab = CompLabel:GetDefaultCompLabel(itemname, 250)
    itemnamelab:setAnchorPoint(ccp(0,0))
    UI_GetUILayout(iteminfoparent,1):removeAllChildren()
    UI_GetUILayout(iteminfoparent,1):addChild(itemnamelab)

    --icon
    local imageName = "Icon/Item/"..ItemData.m_icon..".png";
	UI_GetUIImageView(UI_GetUIImageView(iteminfoparent,2),1):loadTexture(imageName)

	--frame
	UI_IconFrameWithSkillOrItem(UI_GetUIImageView(iteminfoparent,2),2,nil,ItemData.m_quality)

    --pingjie 
    Log("pingjie==="..ItemData.m_quality)
    UI_SetLabelText(iteminfoparent,3,""..ItemData.m_quality)

    --number
    UI_SetLabelText(iteminfoparent,4,""..GetLocalPlayer():GetPlayerBag():GetItemCountByItemId(itemId))

    --功能描述
    UI_SetLabelText(iteminfoparent,5,ItemData.m_desc)

    --详细描述
    UI_SetLabelText(iteminfoparent,6,ItemData.m_descFull)
end

function CreateTouchShowItemTips(layout,itemId)
	-- body
	local function CreateItemFunc(sender,eventType)
		-- body
		if eventType == TOUCH_EVENT_ENDED then
			ShowItemTips(itemId)
		end
	end
    layout:setTouchEnabled(true)
    layout:addTouchEventListener(CreateItemFunc)
end