-- 背包tips

local function showBagItemTips(bagItem)
	local  widget = UI_CreateBaseWidgetByFileName("BagTips.json");
	if(widget == nil)then
		Log("[ERROR]: getBagTips.json failed");
		return;
	end
	-- ICON
	local  iconImage = UI_GetUIImageView(widget,6);
	local heroIconImage = ImageView:create();
	local imageName = "Icon/Item/"..bagItem:getItemData().m_icon..".png";
	heroIconImage:loadTexture(imageName);
	local  posx, posy = iconImage:getPosition();
	heroIconImage:setPosition(ccp(posx, posy));
	heroIconImage:setAnchorPoint(iconImage:getAnchorPoint());
	heroIconImage:setScale(iconImage:getScale());
	iconImage:getParent():addChild(heroIconImage,10);
	iconImage:removeFromParent();

	--名字
	UI_SetLabelText(widget,3,bagItem:getItemData().m_name);
	--品质
	--bagItem:getItemData().m_quality
	local pingziStr = FormatString("Item_quality",bagItem:getItemData().m_quality);
	UI_SetLabelText(widget,7,pingziStr);
	--拥有数量
	local  countStr = FormatString("Item_yongyou",bagItem:GetInt(EBagItem_Count));
	UI_SetLabelText(widget,8,countStr);
	--道具功能描述
	UI_SetLabelText(widget,9,bagItem:getItemData().m_desc);
	--道具详细描述
	UI_SetLabelText(widget,10,bagItem:getItemData().m_descFull);
	--关闭按钮
	local  function useThisItem(sender, eventType)
	Log("fdsfsfsfs");
	 Log("bagID = ",bagItem:getItemData().m_icon);
	 	if(eventType == TOUCH_EVENT_ENDED)then
	 		if(bagItem:getItemData().m_lvLimits > GetLocalPlayer():GetUint(EPlayer_Lvl))then
					Messagebox_Create({info = FormatString("Item_dengjibuzhu",bagItem:getItemData().m_lvLimits), msgType = EMessageType_Middle,});
			else
				    Log("bagID = ",bagItem:getItemData().m_icon);
					local tab = GameServer_pb.Cmd_Cs_ItemUse();
					tab.dwItemObjectID = bagItem:GetUint(EBagItem_ObjId);
					Packet_Full(GameServer_pb.CMD_ITEM_USE, tab);
			end
	 	end
	end

	local UseBtn = UI_GetUIButton(widget, 11);
	if(bagItem:getItemData().m_useEffect == 1)then
		local  title = FormatString("Item_queren");
		UseBtn:setTitleText(title);
	    UseBtn:addTouchEventListener(useThisItem);
	   else
	   	UseBtn:addTouchEventListener(UI_ClickCloseCurBaseWidget);
	end
	--右上角关闭
	local closeBtn = UI_GetUIButton(widget, 2);
	closeBtn:addTouchEventListener(UI_ClickCloseCurBaseWidget);
end

-- 创建背包界面
function BagSysMainLayer_create()
	--点击关闭事件处理
	local function onclickCloseBtn( sender, eventType )
		Log("onclickCloseBtn")
		if(eventType == TOUCH_EVENT_ENDED) then
			UI_CloseCurBaseWidget();
		end
	end
	-- pageView 滑动回调
	local function pageViewEvent(pSender ,type )
		if(eventType == TOUCH_EVENT_ENDED)then
			Log("pageViewEvent");
		end
		
	end
	local widget = UI_CreateBaseWidgetByFileName("BagLayer.json");
	if(widget) then
		Log("BagSysMainLayer_create");
	else
			return;
	end
	--添加关闭点击事件
	local closeBtn = UI_GetUIButton(widget, 4)
	closeBtn:addTouchEventListener(UI_ClickCloseCurBaseWidgetWithNoAction)
	-- 获取背包tableview 
	local  pageView =  UI_GetUIPageView(widget,3);

	if(pageView ~= nil)then
		Log("getPageView");
		pageView:setSize(CCSizeMake(880,500));
		local  pageSize = CCSizeMake(880,500);
	   local  Itemlist = GetLocalPlayer():GetPlayerBag():GetBagList(); 
	    for i = 0, GetLocalPlayer():GetPlayerBag():GetBagList():size()-1 do
	    	Log(GetLocalPlayer():GetPlayerBag():GetBagList()[i]:getItemData().m_icon);
	    end

		local listLen =  GetLocalPlayer():GetPlayerBag():GetBagList():size();
		Log("GetLocalPlayer():GetPlayerBag():GetBagList():size()"..listLen);
		local index = 0;
		while index < listLen do
			if(index%5 == 0)then
				local PageUnityLayout = Layout:create();
			 	PageUnityLayout:setSize(CCSizeMake(880,500));
			 	Log("Size OK ");

				for j=1,5 do

					-- layout Touch function callback
					local  function touchLayOut( sender, eventType)
						local btn = tolua.cast(sender,"Layout");
						if(eventType == TOUCH_EVENT_ENDED)then
							Log("show layout"..btn:getTag().."touch end");
							showBagItemTips(GetLocalPlayer():GetPlayerBag():GetBagList()[btn:getTag()]);
						end
					end

					local  layout = Layout:create();
					layout:setSize(CCSizeMake(160,pageSize.height));
					layout:setPosition(CCPointMake(8+80*(j)+(80+16)*(j-1),pageSize.height/2.0));
					layout:setBackGroundColor(ccc3(255, 0, 0));
					layout:setAnchorPoint(ccp(0.5,0.5));
					layout:setBackGroundColorType(LAYOUT_COLOR_SOLID);
					layout:setTag(index);
					layout:setTouchEnabled(true);
					layout:addTouchEventListener(touchLayOut);
					if(index < listLen) then
						-- Icon
						local heroIconImage = ImageView:create();
						local imageName = "Icon/Item/"..GetLocalPlayer():GetPlayerBag():GetBagList()[index]:getItemData().m_icon..".png";
						heroIconImage:loadTexture(imageName);
						heroIconImage:setPosition(CCPointMake(layout:getSize().width/2.0,layout:getSize().height/2.0));
						heroIconImage:setAnchorPoint(CCPointMake(0.5,0.5));

						-- name

						local  nameLabel = CompLabel:GetDefaultLab(GetLocalPlayer():GetPlayerBag():GetBagList()[index]:getItemData().m_name);
						nameLabel:setPosition(CCPointMake(80,layout:getSize().height/2.0+heroIconImage:getSize().height));

						--des
						local des = GetLocalPlayer():GetPlayerBag():GetBagList()[index]:getItemData().m_desc..GetLocalPlayer():GetPlayerBag():GetBagList()[index]:getItemData().m_descFull;
						local desLabel = CompLabel:GetDefaultLab(des);
						desLabel:setPosition(CCPointMake(80,layout:getSize().height/2.0-heroIconImage:getSize().height))

						-- item count
						local  countStr = FormatString("Item_yongyou", GetLocalPlayer():GetPlayerBag():GetBagList()[index]:GetInt(EBagItem_Count));
						local  countLabel =  CompLabel:GetDefaultLab(countStr);
						countLabel:setPosition(ccp(80,layout:getSize().height/2.0-heroIconImage:getSize().height-40));

						--user button  使用效果 为1 的时候显示使用 
						if (GetLocalPlayer():GetPlayerBag():GetBagList()[index]:getItemData().m_useEffect == 1)then

							local  function useThisItem(sender, eventType)
									local btn = tolua.cast(sender,"Button");
								 	if(eventType == TOUCH_EVENT_ENDED)then
										local tag = btn:getTag();
										Log("Click btn Tag == ".. tag);
										if(tag < GetLocalPlayer():GetPlayerBag():GetBagList():size() and tag >= 0) then
											Log("enter if ");
											local  bagItem = GetLocalPlayer():GetPlayerBag():GetBagList()[tag];
											if(bagItem:getItemData().m_lvLimits > GetLocalPlayer():GetUint(EPlayer_Lvl))then
												Messagebox_Create({info = FormatString("Item_dengjibuzhu",bagItem:getItemData().m_lvLimits), msgType = EMessageType_Middle,});
											else
												local tab = GameServer_pb.Cmd_Cs_ItemUse();
												tab.dwItemObjectID = bagItem:GetUint(EBagItem_ObjId);
												Packet_Full(GameServer_pb.CMD_ITEM_USE, tab);
											end
								 	end
										
									end
							end

							local  useBtn = Button:create();
							useBtn:loadTextures("btng1_1.png","btng1_2.png","", UI_TEX_TYPE_PLIST);
							useBtn:setTouchEnabled(true);
							--useBtn:setPressedActionEnabled(true);
							local  title = FormatString("Item_queren");
							useBtn:setTitleText(title);
							useBtn:setTitleFontSize(24);
							useBtn:setAnchorPoint(ccp(0.5,0.5));
							useBtn:setPosition(CCPointMake(80,layout:getSize().height/2.0-2*heroIconImage:getSize().height));
							useBtn:setTag(index);
							useBtn:addTouchEventListener(useThisItem);
							layout:addChild(useBtn);
						end
						

						layout:addChild(nameLabel);
						layout:addChild(heroIconImage);
						layout:addChild(desLabel);
						layout:addChild(countLabel);
						

						PageUnityLayout:addChild(layout);
						Log("create pageview");
						index = index + 1;
					else
						--空背包
						--todo
					end
				end
				pageView:addPage(PageUnityLayout);
				pageView:addEventListenerPageView(pageViewEvent);
			end
		end
	end
	
end
	
	