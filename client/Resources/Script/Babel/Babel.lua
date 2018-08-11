function Babel_Create()
	local widget = UI_CreateBaseWidgetByFileName("Babel.json");
	if (widget == nil)then
		Log("Babel_Create error");
		return;
	end
	
	local function ClickClose(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			g_babelQuickState = false;
			UI_CloseCurBaseWidget()
		end
	end

	UI_GetUIButton(widget, 1):addTouchEventListener(ClickClose);
	Babel_Refresh(widget)
end

function Babel_Refresh(widget)
	if (widget == nil or g_babelDetail.iTotalLayers == nil)then
		Log("Babel_Refresh error");
		return;
	end
	EndWaiting();
	
	Babel_RefreshLayout();
	
	local listView = UI_GetUIListView(widget, 2);
	local layout = ImageView:create();
	layout:loadTexture("tongtianta/bg_2_01.png")
	listView:setItemModel(layout);
	if (listView:getItem(0) == nil)then
		for i=1,g_babelDetail.iTotalLayers+1 do
			local layout = ImageView:create();
			if (i%2 == 0)then
				layout:loadTexture("tongtianta/bg_2_01.png")
			else
				layout:loadTexture("tongtianta/bg_2_02.png")
			end
			local label = nil;
			if (i == g_babelDetail.iTotalLayers+1)then
				label = CompLabel:GetDefaultLabWithSize(FormatString("Bable_Entrance"), 24)
			else
				label = CompLabel:GetDefaultLabWithSize(FormatString("Bable_Level", 101 - i), 24)
			end
			
			--label:setPosition(ccp(200, 75));
			
			layout:addChild(label);
			listView:pushBackCustomItem(layout);
		end
	end
	
	local armatureLayout = UI_GetUILayout(widget, 6);
	local armature = tolua.cast( armatureLayout:getNodeByTag(1), "CCArmature" )
	if (armature == nil)then
		armature = GetArmature("zhaoyunrun");
		armatureLayout:addNode(armature, 1, 1);
	end
	armature:getAnimation():playWithIndex(0);
	armature:stopAllActions();
	armature:setScale(0.7)
	if (g_babelDetail.iCurrentLayer%2 == 0) then
		armature:setScaleX(0.7);
		armature:setPosition(ccp(0, 0));
	else
		armature:setPosition(ccp(90, 0));
		armature:setScaleX(-0.7);
	end
	
	listView:refreshView();
	--if (g_babelDetail.iCurrentLayer == 0) then
	--	listView:jumpContainerToSelectedIndex(100);
	--else
		listView:jumpContainerToSelectedIndex(96 - g_babelDetail.iCurrentLayer);
	--end
	
	local layout1 = UI_GetUILayout(widget, 5);
	--layout1:setVisible(g_babelQuickState == false);
	UI_GetUILabel(layout1, 1):setText(FormatString("BabelCount", g_babelDetail.iClimbResetCount));
	UI_GetUILabel(layout1, 3):setText(FormatString("Babel_CurLvl", g_babelDetail.iCurrentLayer));
	UI_GetUILabel(layout1, 2):setText(FormatString("Babel_BestLvl", g_babelDetail.iHightestLayer));
	UI_GetUILabel(layout1, 4):setText(FormatString("Bable_AllHeroExp", g_babelDetail.iTotalCultivation));
	UI_GetUILabel(layout1, 5):setText(FormatString("Bable_NextHeroExp", g_babelDetail.iNextLayerCultivation));
	
	local function ClickReset(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then	
			if (g_babelDetail.iClimbResetCount > 0) then
				local tab = {};
				tab.info = FormatString("Babel_ResetNotice");
				tab.leftFun = Babel_Reset;
				
				 Messagebox_Create(tab)
			else
				Packet_Cmd(GameServer_pb.CMD_RESET_CLIMBTOWER);
				ShowWaiting();
			end
		end
	end
	UI_GetUIButton(layout1, 6):addTouchEventListener(ClickReset);
	
	local function ClickResetCount(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then	
			Packet_Cmd(GameServer_pb.CMD_TOWER_ADD_RESETCOUNT);
			ShowWaiting();
		end
	end
	UI_GetUIButton(layout1, 9):addTouchEventListener(ClickResetCount);
	
	local function ClickChallenge(sender, eventType)
		--Log("ClickChallenge")
		if (eventType == TOUCH_EVENT_ENDED) then	
			if (GetGlobalEntity():GetScheduler():HasScheduleScript("SendChallenge") == false)then
				GetGlobalEntity():GetScheduler():RegisterScript( "SendChallenge", 1000, 1 )
				listView:jumpContainerToSelectedIndex(96 - g_babelDetail.iCurrentLayer - 1, 1);
				armature:getAnimation():playWithIndex(1);
				if (g_babelDetail.iCurrentLayer%2 == 1) then
					local actionArry = CCArray:create()
					actionArry:addObject(CCMoveTo:create(1, ccp(0, 0)))
					actionArry:addObject(CCCallFunc:create(Babel_Armature))
					armature:runAction(CCSequence:create(actionArry));
				else
					local actionArry = CCArray:create()
					actionArry:addObject(CCMoveTo:create(1, ccp(90, 0)))
					actionArry:addObject(CCCallFunc:create(Babel_Armature))
					armature:runAction(CCSequence:create(actionArry));
					
					--if (g_babelDetail.iCurrentLayer == 0)then
					--	UI_GetUIImageView(widget, 7):runAction(CCMoveBy:create(1, ccp(0, -layout:getSize().height)))
					--end
				end
			end
		end
	end
	UI_GetUIButton(layout1, 8):addTouchEventListener(ClickChallenge);
	
	local function ClickFastChallenge(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then	
			if (g_babelDetail.iCurrentLayer >= g_babelDetail.iHightestLayer)then			
				Messagebox_Create({info = FormatString("Babel_Notice"), msgType = EMessageType_None})
				return;
			end
			if (GetGlobalEntity():GetScheduler():HasScheduleScript("SendFastChallenge") == false)then
				GetGlobalEntity():GetScheduler():RegisterScript( "SendFastChallenge", 2000, 1 )
				listView:jumpContainerToSelectedIndex(96 - g_babelDetail.iCurrentLayer - 1, 1);
				armature:getAnimation():playWithIndex(1);
				if (g_babelDetail.iCurrentLayer%2 == 1) then
					local actionArry = CCArray:create()
					actionArry:addObject(CCMoveTo:create(1, ccp(0, 0)))
					actionArry:addObject(CCCallFunc:create(Babel_Armature))
					armature:runAction(CCSequence:create(actionArry));
				else
					local actionArry = CCArray:create()
					actionArry:addObject(CCMoveTo:create(1, ccp(90, 0)))
					actionArry:addObject(CCCallFunc:create(Babel_Armature))
					armature:runAction(CCSequence:create(actionArry));
					--if (g_babelDetail.iCurrentLayer == 0)then
						--UI_GetUIImageView(widget, 7):runAction(CCMoveBy:create(1, -ccp(0, layout:getSize().height)))
					--end
				end
				g_babelQuickState = true;
				
				Babel_RefreshLayout();
			end
		end
	end
	UI_GetUIButton(layout1, 7):addTouchEventListener(ClickFastChallenge);
	
	local layout2 = UI_GetUILayout(widget, 4);
	--layout1:setVisible(g_babelQuickState);
	local function ClickStopFastChallenge(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then	
			Packet_Cmd(GameServer_pb.CMD_QUERY_CLIMBTOWERDETAIL);
			g_babelQuickState = false;
			ShowWaiting();
		end
	end
	UI_GetUIButton(layout2, 1):addTouchEventListener(ClickStopFastChallenge);
	
	local listView2 = UI_GetUIListView(layout2, 2);
	listView2:removeAllItems();
	
	--if (g_babelDetail.iCurrentLayer == 0)then
	--	UI_GetUIImageView(widget, 7):setPosition(ccp(300, 16));
	--	UI_GetUIImageView(widget, 7):setVisible(true);
	--else
	--	UI_GetUIImageView(widget, 7):setVisible(false);
	--end
end

function Babel_Armature()
	local widget = UI_GetBaseWidgetByName("Babel")
	if (widget) then
		local armatureLayout = UI_GetUILayout(widget, 6);
		local armature = tolua.cast( armatureLayout:getNodeByTag(1), "CCArmature" )
		armature:getAnimation():playWithIndex(0);
		armature:setScaleX(-armature:getScaleX());
	end
end

function Babel_RefreshLayout()
	local widget = UI_GetBaseWidgetByName("Babel")
	if (widget) then
		local layout1 = UI_GetUILayout(widget, 5);
		layout1:setVisible(g_babelQuickState == false);
		
		local layout2 = UI_GetUILayout(widget, 4);
		layout2:setVisible(g_babelQuickState);
	end
end

function Babel_Reset()
	Packet_Cmd(GameServer_pb.CMD_RESET_CLIMBTOWER);
	ShowWaiting();
end

if (g_babelDetail == nil)then
	g_babelDetail = {}
end

if (g_babelQuickState == nil)then
	g_babelQuickState = false;
end


function GetClimbTowerDetail(pkg)
	EndWaiting();
	local tmp = GameServer_pb.CMD_QUERY_CLIMBTOWERDETAIL_SC();
	tmp:ParseFromString(pkg)
	Log("GetClimbTowerDetail"..tostring(tmp))
	g_babelDetail = tmp.detail;
	--Log("g_myTerritoryInfo.maidInfo.selfMaid.iLoveCount"..g_myTerritoryInfo.maidInfo.selfMaid.iLoveCount)
	if (UI_GetBaseWidgetByName("Babel")) then
		UI_RefreshBaseWidgetByName("Babel");
	else
		Babel_Create();
	end
end

function SendChallenge()
	local widget = UI_GetBaseWidgetByName("Babel")
	if (widget) then
		local tab = GameServer_pb.CMD_CHALLENGE_TOWER_CS();
		tab.iTowerLayerId = g_babelDetail.iCurrentLayer + 1;
		Packet_Full(GameServer_pb.CMD_CHALLENGE_TOWER, tab);
		g_openUIType = EUIOpenType_Babel;
		ShowWaiting();
	end
end

function SendFastChallenge()
	local widget = UI_GetBaseWidgetByName("Babel")
	if (widget == nil) then
		g_babelQuickState = false;
	end
	
	if (g_babelQuickState) then
		local tab = GameServer_pb.CMD_FAST_CHALLENGE_TOWER_CS();
		tab.iTowerLayerId = g_babelDetail.iCurrentLayer + 1;
		Packet_Full(GameServer_pb.CMD_FAST_CHALLENGE_TOWER, tab);	
		ShowWaiting();
	else
		GetGlobalEntity():GetScheduler():UnRegisterScript( "SendFastChallenge")
	end
end

function GetTowFastChallenge(pkg)
	EndWaiting();
	local tmp = GameServer_pb.CMD_TOWER_FAST_CHALLENGE_RESOULT_SC();
	tmp:ParseFromString(pkg)
	Log("*********"..tostring(tmp))

	local widget = UI_GetBaseWidgetByName("Babel")
	if (widget) then
		local layout = UI_GetUILayout(widget, 4);
		local listView = UI_GetUIListView(layout, 2);
		local armatureLayout = UI_GetUILayout(widget, 6);
		local armature = tolua.cast( armatureLayout:getNodeByTag(1), "CCArmature" )
		g_babelDetail.iCurrentLayer = tmp.iChallengeLayerId
		if (tmp.bIsWin)then
			listView:pushBackCustomItem(Babel_GetFastListItem(tmp))
			listView:refreshView();
			listView:scrollToBottom(1.0, false);
			if (g_babelDetail.iCurrentLayer >= g_babelDetail.iHightestLayer) then
				Packet_Cmd(GameServer_pb.CMD_QUERY_CLIMBTOWERDETAIL);		
				g_babelQuickState = false;
				return;
			end
		else
			Packet_Cmd(GameServer_pb.CMD_QUERY_CLIMBTOWERDETAIL);	
			g_babelQuickState = false;
			return;
		end
		
		local listViewLevel = UI_GetUIListView(widget, 2);
		if (g_babelQuickState) then
			GetGlobalEntity():GetScheduler():RegisterScript( "SendFastChallenge", 2000, 1 )
			listViewLevel:jumpContainerToSelectedIndex(96 - g_babelDetail.iCurrentLayer - 1, 1);
			armature:getAnimation():playWithIndex(1);
			if (g_babelDetail.iCurrentLayer%2 == 1) then
				local actionArry = CCArray:create()
				actionArry:addObject(CCMoveTo:create(1, ccp(0, 0)))
				actionArry:addObject(CCCallFunc:create(Babel_Armature))
				armature:runAction(CCSequence:create(actionArry));
			else
				local actionArry = CCArray:create()
				actionArry:addObject(CCMoveTo:create(1, ccp(90, 0)))
				actionArry:addObject(CCCallFunc:create(Babel_Armature))
				armature:runAction(CCSequence:create(actionArry));
			end
		else
			listViewLevel:jumpContainerToSelectedIndex(96 - g_babelDetail.iCurrentLayer);
			armature:getAnimation():playWithIndex(0);
			if (g_babelDetail.iCurrentLayer%2 == 0) then
				armature:setScaleX(0.7);
				armature:setPosition(ccp(0, 0));
			else
				armature:setPosition(ccp(90, 0));
				armature:setScaleX(-0.7);
			end
		end
	end
end

function Babel_GetFastListItem(msg)
	local layout = Layout:create();
	layout:setSize(CCSizeMake(300, 100))
	
	local title = CompLabel:GetDefaultLabWithSize(FormatString("Bable_Level", msg.iChallengeLayerId), 24)
	title:setAnchorPoint(ccp(0, 0.5));
	title:setPosition(ccp(10, 80));
	layout:addChild(title)
	
	local awardPropList = msg.awrdRes.szAwardPropList;
	local awardItemList = msg.awrdRes.szAwardItemList;
	local str = "";

	for i=1, #awardPropList do
		if (i ~= 1)then
			str = str..FormatString("comma")
		end
		str = str..FormatString("BabelAttName", awardPropList[i].iValue..GetAttName(awardPropList[i].iLifeAttID) );		
	end
	--Log("str ="..str)
	local propLabel = CompLabel:GetDefaultCompLabel(str, 280);
	propLabel:setPosition(ccp(10, 60));
	layout:addChild(propLabel)
	
	str = "";
	for i=1, #awardItemList do
		if (i ~= 1)then
			str = str..FormatString("comma")
		end
		local data = GetGameData(DataFileItem, awardItemList[i].iItemID, "stItemData");
		str = str..FormatString("ItemName", UI_GetStringColorNew(data.m_quality), data.m_name);
	end
	--Log("str ="..str)
	local itemLabel = CompLabel:GetDefaultCompLabel(str, 280);
	itemLabel:setPosition(ccp(10, 30));
	layout:addChild(itemLabel)
	
	return layout
end

ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_TOWER_FAST_CHALLENGE_RESOULT, "GetTowFastChallenge" );
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_QUERY_CLIMBTOWERDETAIL, "GetClimbTowerDetail" );