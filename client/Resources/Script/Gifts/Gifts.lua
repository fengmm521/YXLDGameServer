local l_giftListInfo = nil 

--记录点击的是listview的哪个index
local l_giftlistViewIndex = nil 

local function setremovegiftlistviewindx(index)
      l_giftlistViewIndex = index
end

local function removehasgetGiftListReward()
	-- body
  local widget = UI_GetBaseWidgetByName("Gifts")
	local layoutPrarent = UI_GetUILayout(widget,1)
	local giftList = UI_GetUIListView(layoutPrarent,3)
	if l_giftlistViewIndex ~= nil then
		giftList:removeItem(l_giftlistViewIndex)
	end
end

function GiftsInfoBase()
	-- body
	local widget = UI_CreateBaseWidgetByFileName("Gifts.json")

	local layoutparent = UI_GetUILayout(widget,1)
	UI_GetUIButton(layoutparent,1):addTouchEventListener(UI_ClickCloseCurBaseWidget)

    GiftlistInfoRefresh(widget)
end

function GiftlistInfoRefresh(widget)
	-- body
	local layoutPrarent = UI_GetUILayout(widget,1)
	local giftList = UI_GetUIListView(layoutPrarent,3)
	giftList:removeAllItems()
  local width = giftList:getContentSize().width 
  local curlayout = nil
    local function createGiftInfo(index)
    	-- body
    	Log("index========"..index)
      local imag = UI_GetUIImageView(curlayout,1)
      local imagPox = imag:getPosition()
    	if l_giftListInfo.szGiftItem[index].bCustomGift == true then
           local strTitlelab = CompLabel:GetDefaultCompLabel(FormatString("Gift_infoString",l_giftListInfo.szGiftItem[index].cutomBody.strTitle),900)
           strTitlelab:setPosition(ccp(imagPox + imag:getContentSize().width + 10, 90))
           strTitlelab:setAnchorPoint(ccp(0,0.5))
           curlayout:addChild(strTitlelab)
           local floatWidth = 10
           if #l_giftListInfo.szGiftItem[index].cutomBody.szAwardPropList ~= nil then
              for i = 1,#l_giftListInfo.szGiftItem[index].cutomBody.szAwardPropList do
                  local lifeAttId = l_giftListInfo.szGiftItem[index].cutomBody.szAwardPropList[i].iLifeAttID
    			        local numberstirng = GetAttNameAndValue(lifeAttId,l_giftListInfo.szGiftItem[index].cutomBody.szAwardPropList[i].iValue)
    			        local numberlab = CompLabel:GetDefaultCompLabel(FormatString("Gift_infoString",numberstirng),900)
                  numberlab:setPosition(ccp(imagPox + imag:getContentSize().width + floatWidth , 60))
                  numberlab:setAnchorPoint(ccp(0,0.5))
                  floatWidth = numberlab:getContentSize().width + floatWidth
                  curlayout:addChild(numberlab)
              end
           end
           if l_giftListInfo.szGiftItem[index].cutomBody.iAwardedContribute ~= nil then
           	   local conrewardLab = CompLabel:GetDefaultCompLabel(FormatString("Gift_Hasbeen",l_giftListInfo.szGiftItem[index].cutomBody.iAwardedContribute),900)
               conrewardLab:setPosition(ccp(imagPox + imag:getContentSize().width + floatWidth + 10 , 60))
           	   conrewardLab:setAnchorPoint(ccp(0,0.5))
           	   floatWidth = floatWidth + conrewardLab:getContentSize().width 
           	   curlayout:addChild(conrewardLab)
           end
           local floatWidthTwo = 10
           if l_giftListInfo.szGiftItem[index].cutomBody.szAwardItemList ~= nil then
           	   for i = 1,#l_giftListInfo.szGiftItem[index].cutomBody.szAwardItemList do
           	   	   local itemId = l_giftListInfo.szGiftItem[index].cutomBody.szAwardItemList[i].iItemID
           	   	   Log("itemId======"..itemId)
           	   	   local Itemdata = GetGameData(DataFileItem,itemId,"stItemData")
           	   	   local itemNameAndNumber = Itemdata.m_name..": x "..l_giftListInfo.szGiftItem[index].cutomBody.szAwardItemList[i].iCount
           	   	   local itemNameAndNumberLab = CompLabel:GetDefaultCompLabel(FormatString("Gift_infoString",itemNameAndNumber),900)
           	   	   itemNameAndNumberLab:setAnchorPoint(ccp(0,0.5))
           	   	   itemNameAndNumberLab:setPosition(ccp(imagPox + imag:getContentSize().width + floatWidthTwo ,30))
           	   	   floatWidthTwo = itemNameAndNumberLab:getContentSize().width + floatWidthTwo
           	   	   curlayout:addChild(itemNameAndNumberLab)
           	   end
           end
        else
        	  local giftData = GetGameData(DataFileGift,l_giftListInfo.szGiftItem[index].iGiftID,"stGiftData")
        	  local titlelab = CompLabel:GetDefaultCompLabel(FormatString("Gift_infoString",giftData.m_title),900)
            titlelab:setAnchorPoint(ccp(0,0.5))
            titlelab:setPosition(ccp(imagPox + imag:getContentSize().width + 10 , 80))
            curlayout:addChild(titlelab)

            local rewardlab = CompLabel:GetDefaultCompLabel(FormatString("Gift_infoString",giftData.m_reward),900)
            rewardlab:setAnchorPoint(ccp(0,0.5))
            rewardlab:setPosition(ccp(imagPox + imag:getContentSize().width + 10 , 40))
            curlayout:addChild(rewardlab)
    	end
    	local function clickgetGiftFunc(sender,eventType)
    		-- body
    		if eventType == TOUCH_EVENT_ENDED then
    			tolua.cast(sender,"Button")
    			local clickIndex = giftList:getIndex(sender:getParent())
    			setremovegiftlistviewindx(clickIndex)
    			Log("clickIndex========"..index)
    			local tab = GameServer_pb.CMD_GET_GIFT_CS()
    			tab.strGiftUUID = l_giftListInfo.szGiftItem[index].strUUID
    			Packet_Full(GameServer_pb.CMD_GET_GIFT, tab)
    			ShowWaiting()
    		end
    	end
      UI_GetUIButton(curlayout,2):addTouchEventListener(clickgetGiftFunc)
    end
    if l_giftListInfo ~= nil then 
      for i = 1, #l_giftListInfo.szGiftItem do
      	curlayout = UI_GetCloneLayout(UI_GetUILayout(layoutPrarent,4))
        curlayout:setVisible(true)
        createGiftInfo(i)
      	giftList:pushBackCustomItem(curlayout)
      end
    end
end

--查找当前的giftlistview中对应的UUId的index
function lookupGiftListViewIndexByGiftListView(UUId)
	-- body
	for i = 1,#l_giftListInfo.szGiftItem do
		if l_giftListInfo.szGiftItem[i].strUUID == UUId then
			removehasgetGiftListReward()
		end
	end
end

local custonToString = tostring
function getGiftInfo(pkg)
	-- body
	EndWaiting()
	l_giftListInfo = GameServer_pb.CMD_QUERY_GIFT_SC()
	l_giftListInfo:ParseFromString(pkg)
	Log("xxxxx ===="..custonToString(l_giftListInfo))
	GiftsInfoBase()
end

function hasGetGiftInfo(pkg)
	-- body
	EndWaiting()
    local info = GameServer_pb.CMD_GET_GIFT_SC()
    info:ParseFromString(pkg)
    lookupGiftListViewIndexByGiftListView(info.strGiftUUID)
end
ScriptSys:GetInstance():RegisterScriptFunc(GameServer_pb.CMD_QUERY_GIFT, "getGiftInfo")
ScriptSys:GetInstance():RegisterScriptFunc(GameServer_pb.CMD_GET_GIFT, "hasGetGiftInfo")