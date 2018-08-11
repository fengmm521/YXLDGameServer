function Chat_Create()
	local widget = UI_CreateBaseWidgetByFileName("Chat.json");
	if (widget == nil)then
		Log("Chat_Create error");
		return;
	end
	g_curChannel = EChannelWorld;
	UI_GetUIButton(widget, 5):addTouchEventListener(UI_ClickCloseCurBaseWidgetWithNoAction);
	UI_GetUILayout(widget, 4461727):addTouchEventListener(PublicCallBackWithNothingToDo)
	for i=1, 4 do
		local listView = UI_GetUIListView(widget:getChildByTag(i), 1);
		for j=1, #g_chatChannel[i],1 do
            if j<=20 then
                local layout
                if g_chatChannel[i][j] ~= nil then
                    if i==1 and g_chatChannel[i][j].strRecver~=nil then
                        layout = GetOneLayout(g_chatChannel[i][j], 2, j);
                    --elseif 
                    elseif g_chatChannel[i][j].iChannel~= nil and g_chatChannel[i][j].iChannel == 4 then
                        layout = GetOneLayout(g_chatChannel[i][j],4 , j,true);
                    else
			            layout = GetOneLayout(g_chatChannel[i][j],i , j);
                    end
                    Chat_ShowData(listView,layout)
                end
            else
                break
            end
		end
		listView:refreshView();
		--listView:scrollToBottom(1, false);
	end
	Chat_Refresh(widget)
    
    
    Packet_Cmd(GameServer_pb.CMD_CHAT_CHANNEL_QUERY)
    print("chatcreate=====================================")
end

function Chat_SetChanel(index)
	local widget = UI_GetBaseWidgetByName("Chat")
	if (widget) then
		g_curChannel = index;
		for i=1, 4 do
			UI_GetUIButton(widget, 10 + i):setBright(i ~= index);
			local layout = UI_GetUILayout(widget, i);
			if (i == index)then
				layout:setVisible(true);
				--UIMgr:GetInstance():PlayOpenAction(layout, EUIOpenAction_MoveIn_Right, 0.3);
			else
				layout:setVisible(false);
			end
			if (i == 2)then
				UI_GetUILabel(layout, 2):setText(g_whisperName);
				--[[if (g_whisperName == "")then
					layout:getChildByTag(2):setVisible(false);
					layout:getChildByTag(3):setVisible(false);
					layout:getChildByTag(4):setVisible(false);
					layout:getChildByTag(5):setVisible(false);
					layout:getChildByTag(6):setVisible(false);
					layout:getChildByTag(7):setVisible(false);
				else
					layout:getChildByTag(2):setVisible(true);
					layout:getChildByTag(3):setVisible(true);
					layout:getChildByTag(4):setVisible(true);
					layout:getChildByTag(5):setVisible(true);
					layout:getChildByTag(6):setVisible(true);
					layout:getChildByTag(7):setVisible(true);
					UI_GetUILabel(layout, 2):setText(g_whisperName);
				end]]
			end
		end
	end
end

function Chat_Refresh(widget)
	EndWaiting();
	if (widget == nil)then
		Log("Chat_Refresh error");
		return;
	end
	
	local function SendMsg(sender, eventType)
		--Log("SendMsg")
		if (eventType == TOUCH_EVENT_ENDED) then
			Log("g_curChannel"..g_curChannel)
			local layout = UI_GetUILayout(widget, g_curChannel);
			local msg = UI_GetUITextField(UI_GetUILayout(layout,1500), 3):getStringValue();
			UI_GetUITextField(UI_GetUILayout(layout,1500), 3):setText("");
			if (msg == "") then
				return;
			end
			
			if (g_curChannel == EChannelWhisper)then
				if (g_whisperName == "") then
					--Messagebox_Create({info = FormatString("ChatNameEmpty"), msgType = EMessageType_None})
                    createPromptBoxlayout(FormatString("ChatNameEmpty"))
					return;
				end
				Log("g_whisperName= "..g_whisperName)
				local tab = GameServer_pb.CMD_WHISPER_CS();
				tab.strRecver = g_whisperName;
				tab.strMsg = msg
				Packet_Full(GameServer_pb.CMD_WHISPER, tab);
			else
				Log("msg"..msg)
				local index = string.find(msg, '/gm ')
				if (index == 1)then
					local tab = GameServer_pb.Cmd_Cs_GmMsg();
					tab.strCmd = string.gsub(msg, "/gm ", "") ;
					Packet_Full(GameServer_pb.CMD_GM_MSG, tab);
				else
                    local function sendMessage()      
					    local tab = GameServer_pb.CMD_CHANNEL_CHAT_CS();
					    tab.iChancel = g_curChannel;
					    tab.strMsg = msg
					    Packet_Full(GameServer_pb.CMD_CHANNEL_CHAT, tab);
                    end
                    --如果在世界频道发送消息
                    if g_curChannel == EChannelWorld then
                        if g_ChatWorldTimes<=0 then
                            local tab = {}
                            tab.info = FormatString("ChatNoTimes",g_ChatWorldCost)
                            tab.msgType = EMessageType_LeftRight
                            tab.leftFun = sendMessage
                            Messagebox_Create(tab) 
                        else
                            sendMessage()
                        end
                    else
                        sendMessage()
                    end
                    
				end
			end
		end
	end
	
	local function ClickChannel(sender, eventType)
		--Log("ClickChannel")
		if (eventType == TOUCH_EVENT_ENDED) then
			local sender = tolua.cast(sender, "Button");		
			local tag = sender:getTag();
			
			Chat_SetChanel(tag - 10);
		end
	end
	
	--[[local function AddFace(sender, eventType)
		--Log("ClickChannel")
		if (eventType == TOUCH_EVENT_ENDED) then
			local sender = tolua.cast(sender, "Button");		
			local tag = sender:getTag();
			
			Face_create();
		end
	end]]
	

    local function changeTextFiledLocation(textField)
        print("textField:getStringValue()======"..textField:getStringValue())
        local width = textField:getContentSize().width

        print("width======"..width)
        if UI_GetUILayout(UI_GetUILayout(widget, 1),1500):getSize().width < width+50 then
            textField:setPosition(ccp(UI_GetUILayout(UI_GetUILayout(widget, 1),1500):getSize().width - width-50,17))
        else
            textField:setPosition(ccp(0,17))
        end
    end

	local function InputInfo(sender, eventType)
		if (eventType == TEXTFIELD_EVENT_ATTACH_WITH_IME)then
			--widget:setPosition(ccp(widget:getPositionX(), widget:getPositionY() + 300));
		elseif (eventType == TEXTFIELD_EVENT_DETACH_WITH_IME)then
			--widget:setPosition(ccp(widget:getPositionX(), widget:getPositionY() - 300));
            local textField = tolua.cast(sender,"TextField")
            changeTextFiledLocation(textField)
		elseif eventType == TEXTFIELD_EVENT_INSERT_TEXT then
            local textField = tolua.cast(sender,"TextField")
            changeTextFiledLocation(textField)
        elseif eventType == TEXTFIELD_EVENT_DELETE_BACKWARD then
            local textField = tolua.cast(sender,"TextField")
            changeTextFiledLocation(textField)
        end


	end
	
	for i=1, 4 do
		UI_GetUIButton(widget, 10 + i):addTouchEventListener(ClickChannel);
		local layout = UI_GetUILayout(widget, i);
		local listView = UI_GetUIListView(layout, 1);
		local chatList = g_chatChannel[i];
		
        local function enter(sender,eventType)
            if eventType == TOUCH_EVENT_ENDED then
                UI_GetUITextField(UI_GetUILayout(layout,1500), 3):attachWithIME()
            end
        end

		if (i ~= 4) then
			UI_GetUIButton(layout, 4):addTouchEventListener(SendMsg);
			Log("GetPlatform() "..GetPlatform())
			--if (GetPlatform() == 1)then
		    UI_GetUITextField(UI_GetUILayout(layout,1500), 3):addEventListenerTextField(InputInfo)
            UI_GetUILayout(layout,1500):addTouchEventListener(enter)
			--end
			--UI_GetUIButton(layout, 5):addTouchEventListener(AddFace);
		end
	end
	
	Chat_SetChanel(g_curChannel);
end

EChannelWorld=1
EChannelWhisper=2
EChannelLegion=3
EChannelSystem=4

if (g_chatChannel == nil) then
	g_chatChannel = {};
	
	for i=1, 4 do
		g_chatChannel[i] = {};
	end
end

if (g_curChannel == nil)then
	g_curChannel = EChannelWorld;
end

if (g_whisperName == nil)then
	g_whisperName = "";
end

if (g_selectedWhisperName == nil)then
	g_selectedWhisperName = "";
end

function GetChanelName(channel)
	if (channel == EChannelWorld)then
		return FormatString("EChannelWorld")
	elseif (channel == EChannelWhisper)then
		return FormatString("EChannelWhisper")
	elseif (channel == EChannelLegion)then
		return FormatString("EChannelLegion")
	end
	
	return "";
end

function ChatMenu_Create(pos)
	local widget = UI_CreateBaseWidgetByFileName("ChatMenu.json");
	if (widget == nil)then
		Log("ChatMenu_Create error");
		return;
	end
	
	local layout = UI_GetUILayout(widget, 1);
	
	local function ClickFriend(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			Log("name ===== "..tostring(g_selectedWhisperName))
			UI_CloseCurBaseWidget(EUICloseAction_None)
            local tab = GameServer_pb.CMD_FRIENDSYSTEM_REQUEST_CS()
            tab.strName = g_selectedWhisperName
            Packet_Full(GameServer_pb.CMD_FRIENDSYSTEM_REQUEST, tab)
            ShowWaiting()
		end
	end
	UI_GetUIButton(layout, 1):addTouchEventListener(ClickFriend);
	
	local function ClickPlayerInfo(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			local tab = GameServer_pb.CMD_USENAME_FINED_ACTOR_INFO_CS()
    		UI_CloseCurBaseWidget(EUICloseAction_None)
    		tab.strActorName = g_selectedWhisperName
    		Packet_Full(GameServer_pb.CMD_USENAME_FINED_ACTOR_INFO,tab)
    		ShowWaiting()
		end
	end
	UI_GetUIButton(layout, 2):addTouchEventListener(ClickPlayerInfo);
	
	local function ClickWhisper(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			g_whisperName = g_selectedWhisperName;
			Chat_SetChanel(EChannelWhisper);
			UI_CloseCurBaseWidget(EUIOpenAction_None);
		end
	end
	UI_GetUIButton(layout, 3):addTouchEventListener(ClickWhisper);
	if (pos.x > 800)then
		x = 800
	end
	
	if (pos.y < 200)then
		y = 200
	end
    local maxY = CCDirector:sharedDirector():getVisibleSize().height;
	layout:setPosition(ccp(pos.x,pos.y*640/maxY));
    local function close(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			UI_CloseCurBaseWidget(EUIOpenAction_None);
		end
	end
	widget:addTouchEventListener(close);
end

function Chat_ClickName(sender, eventType)
	if (eventType == TOUCH_EVENT_ENDED) then
		--Log("Chat_ClickName")
		local sender = tolua.cast(sender, "Widget");		
		local tag = sender:getTag();
		if (channel == EChannelWhisper)then
			if (g_chatChannel[g_curChannel][tag].strSender == GetLocalPlayer():GetEntityName())then
				g_selectedWhisperName = g_chatChannel[g_curChannel][tag].strRecver;
			else
				g_selectedWhisperName = g_chatChannel[g_curChannel][tag].strSender;
			end
		else
			if (g_chatChannel[g_curChannel][tag].strSender == GetLocalPlayer():GetEntityName())then
				return;
			end
			g_selectedWhisperName = g_chatChannel[g_curChannel][tag].strSender;
		end

		local pos = tolua.cast(sender,"Layout"):getWorldPosition()

		ChatMenu_Create(pos);
	end
end

function GetOneLayout(msg, channel, tag, cantClickName)
    local widget = UI_GetBaseWidgetByName("Chat")
    if widget == nil then
        return
    end
	local layout = UI_GetUILayout(widget, 1335):clone()
    layout:setVisible(true)
    --消息来源
    local info
    local msgD
    if channel == EChannelSystem then
	    info = CompLabel:GetDefaultCompLabel(FormatString("ChatChannelSystem"), 2000);
        msgD = CompLabel:GetDefaultCompLabel(FormatString("ChatSystem",msg.strMsg),700);
    else
	    info = CompLabel:GetDefaultCompLabel(FormatString("ChatChannelWorld"), 600);
        msgD = CompLabel:GetDefaultCompLabel(FormatString("ChatWorld",msg.strSender,msg.strMsg),700);
        print("channel=================="..channel)
        if (channel == EChannelWhisper) then
        print("channel-----------------------"..channel)
             info = nil 
	        info = CompLabel:GetDefaultCompLabel(FormatString("ChatChannelWhisper"), 2000);
            if (msg.strSender == GetLocalPlayer():GetEntityName()) then
                msgD = CompLabel:GetDefaultCompLabel(FormatString("ChatWhisperTo",msg.strRecver,msg.strMsg),700);
	        else    
		        msgD = CompLabel:GetDefaultCompLabel(FormatString("ChatWhisperWith",msg.strSender,msg.strMsg),700);
	        end  
        elseif (channel == EChannelLegion) then
	        info = CompLabel:GetDefaultCompLabel(FormatString("ChatChannelLegion"), 2000);
            msgD = CompLabel:GetDefaultCompLabel(FormatString("ChatLegion",msg.strSender,msg.strMsg),700);
        end
    end
    info:setAnchorPoint(ccp(0,1))
    layout:addChild(info)

    layout:addChild(msgD)
    msgD:setAnchorPoint(ccp(0,1))

    layout:setSize(CCSizeMake(810, msgD:getSize().height+20))

    info:setPosition(ccp(10,msgD:getSize().height+10))
    msgD:setPosition(ccp(10+info:getSize().width,msgD:getSize().height+10))

    msgD:setTag(tag)
    if (cantClickName == true)then
    else
	    msgD:setTouchEnabled(true);
	    msgD:addTouchEventListener(Chat_ClickName);
    end
	--if (msg.iChannel ~= nil) then
	--	channel = msg.iChannel;
	--else
	--	channel = EChannelWhisper;
	--end
	
	--if (channel == EChannelSystem)then
 --       print("11111111111111111111111111")
	--	local info = CompLabel:GetDefaultCompLabel(FormatString("EChannelSystem", msg.strMsg), 2000);
	--	info:setPosition(ccp(10, info:getContentSize().height + 5));
	--	layout:setSize(CCSizeMake(900, info:getContentSize().height + 11));
	--	layout:addChild(info);
	--else
	--	local posx = 10;
	--	local channelName = CompLabel:GetDefaultCompLabel(GetChanelName(channel), 100);
	--	channelName:setPosition(ccp(posx, 30));
	--	layout:addChild(channelName);
		
	--	posx = posx + channelName:getContentSize().width;
	--	local nameLabel = nil;
	--	if (channel == EChannelWhisper) then
	--		if (msg.strSender == GetLocalPlayer():GetEntityName()) then
                
	--			nameLabel = CompLabel:GetDefaultCompLabel(FormatString("WhisperName_2", msg.strRecver), 240);
	--		else    
	--			nameLabel = CompLabel:GetDefaultCompLabel(FormatString("WhisperName_1", msg.strSender), 240);
	--		end
	--	else
            
	--		nameLabel = CompLabel:GetDefaultCompLabel(FormatString("ChatName", msg.strSender), 180);
	--	end
	--	nameLabel:setPosition(ccp(posx, 30));
	--	if (cantClickName == true)then
	--	else
	--		nameLabel:setTouchEnabled(true);
	--		nameLabel:addTouchEventListener(Chat_ClickName);
	--	end
	--	layout:addChild(nameLabel, 1, tag);
	--	posx = posx + nameLabel:getContentSize().width;
		
	--	local info = nil
	--	if (channel == EChannelWorld) then
	--		info = CompLabel:GetDefaultCompLabel(FormatString("WorldChatInfo", msg.strMsg), 600);
	--	elseif (channel == EChannelWhisper) then
	--		info = CompLabel:GetDefaultCompLabel(FormatString("WhisperChatInfo", msg.strMsg), 2000);
	--	elseif (channel == EChannelLegion) then
	--		info = CompLabel:GetDefaultCompLabel(FormatString("LegionChatInfo", msg.strMsg), 2000);
	--	end
		
	--	if (info) then
 --           info:setAnchorPoint(ccp(0,1))
	--		info:setPosition(ccp(posx, info:getSize().height));
	--		layout:addChild(info);  
	--	    layout:setSize(CCSizeMake(900, info:getSize().height));
	--	end
	--end
	
	return layout;
end

function ChannelChat(pkg)
	local tmp = GameServer_pb.CMD_CHANNEL_CHAT_SC();
	tmp:ParseFromString(pkg)
	Log("*********"..tostring(tmp))
	
	Chat_SaveData(g_chatChannel[EChannelWorld],tmp)

	if (tmp.iChannel == EChannelLegion) then
        Chat_SaveData(g_chatChannel[EChannelLegion],tmp)
	elseif (tmp.iChannel == EChannelSystem) then
        Chat_SaveData(g_chatChannel[EChannelSystem],tmp)
	end
	
	local widget = UI_GetBaseWidgetByName("Chat")
	if (widget) then
        if (tmp.iChannel == EChannelWorld) then
		    local worldList = UI_GetUIListView(widget:getChildByTag(1), 1);
		    local layout1 = GetOneLayout(tmp, EChannelWorld, #g_chatChannel[EChannelWorld]);
		    Chat_ShowData(worldList,layout1)
		    worldList:refreshView();
		elseif (tmp.iChannel == EChannelLegion) then
            local worldList = UI_GetUIListView(widget:getChildByTag(1), 1);
		    local layout1 = GetOneLayout(tmp, EChannelLegion, #g_chatChannel[EChannelWorld]);
		    Chat_ShowData(worldList,layout1)
		    worldList:refreshView();

			local listView = UI_GetUIListView(widget:getChildByTag(3), 1);
			local layout2 = GetOneLayout(tmp, EChannelLegion, #g_chatChannel[EChannelLegion]);
			Chat_ShowData(listView,layout2)	
			listView:refreshView();
		elseif (tmp.iChannel == EChannelSystem) then
            local worldList = UI_GetUIListView(widget:getChildByTag(1), 1);
		    local layout1 = GetOneLayout(tmp, EChannelSystem, #g_chatChannel[EChannelWorld],true);
		    Chat_ShowData(worldList,layout1)
		    worldList:refreshView();

			local listView = UI_GetUIListView(widget:getChildByTag(4), 1);
			local layout2 = GetOneLayout(tmp, EChannelSystem, #g_chatChannel[EChannelSystem],true);
			Chat_ShowData(listView,layout2)	
			listView:refreshView();
		end
	end
end

function GetWhisper(pkg)
	local tmp = GameServer_pb.CMD_WHISPER_SC();
	tmp:ParseFromString(pkg)
    Log("*********"..tostring(tmp))

	--g_chatChannel[EChannelWorld][#g_chatChannel[EChannelWorld] + 1] = tmp;
	--g_chatChannel[EChannelWhisper][#g_chatChannel[EChannelWhisper] + 1] = tmp;
	Chat_SaveData(g_chatChannel[EChannelWorld],tmp)
    Chat_SaveData(g_chatChannel[EChannelWhisper],tmp)
	local widget = UI_GetBaseWidgetByName("Chat");
	if (widget) then
		local worldList = UI_GetUIListView(widget:getChildByTag(1), 1);
		local layout1 = GetOneLayout(tmp, EChannelWhisper, #g_chatChannel[EChannelWorld]);
		Chat_ShowData(worldList,layout1)
		
		local listView = UI_GetUIListView(widget:getChildByTag(2), 1);
		local layout2 = GetOneLayout(tmp, EChannelWhisper, #g_chatChannel[EChannelWhisper]);
		Chat_ShowData(listView,layout2)
	end
	if UI_GetBaseWidgetByName("Chat") then
    else
	    Chat_RefreshSimpleChat()
    end
end

function Chat_RefreshSimpleChat()
    local blink = CCBlink:create(100000,200000)
    g_Chat_Sprite:runAction(blink)
end

function Chat_CreateSimpleChat()
	local function ClickChat(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
            g_Chat_Sprite:stopAllActions()
            g_Chat_Sprite:setVisible(true)
			Chat_Create();
		end
	end
--	g_simpleChatUi = GUIReader:shareReader():widgetFromJsonFile("SimpleChat.json");
--	g_simpleChatUi:retain();

    g_Chat_Sprite = CCSprite:create("UIExport/DemoLogin/zhujiemian/msn_01.png")
    g_Chat_Sprite:setPosition(25,25)
    g_Chat_Sprite:setZOrder(100)


    local widget = UI_GetBaseWidgetByName("MainUi")
    if widget then
        UI_GetUILayout(widget, 1212):addNode(g_Chat_Sprite)
        UI_GetUILayout(widget, 1212):addTouchEventListener(ClickChat)
    end
--	UI_GetUILayout(g_simpleChatUi:getChildByTag(1), 4670843):addTouchEventListener(ClickChat); 
--    UI_GetUILayout(g_simpleChatUi:getChildByTag(1), 4670843):addNode(g_Chat_Sprite)
end

function Face_create()
	local widget = UI_CreateBaseWidgetByFileName("Face.json");
	
	widget:addTouchEventListener(UI_ClickCloseCurBaseWidget);
	local function ClickFace(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
		
		end
	end
	local tmpLayout1 = Layout:create(400, 40);
	local tmpLayout2 = Layout:create(400, 40);
	
	for i=1, 20 do
		local layout = Layout:create(40, 40);
		layout:addTouchEventListener(ClickFace)
		layout:setTouchEnabled(true)
		local face = FaceMgr:GetInstance():GetById(i - 1);
		layout:addNode(face)
		if (i > 10)then
			tmpLayout2:addChild(layout, i, i)
		else
			tmpLayout1:addChild(layout, i, i)
		end
	end
	UI_GetUIListView(widget, 1):pushBackCustomItem(tmpLayout1);
	UI_GetUIListView(widget, 1):pushBackCustomItem(tmpLayout2);
end

--if (g_simpleChatUi == nil)then
--	Chat_CreateSimpleChat();
--end

function Chat_ChannelQuery(pkg)
    print("---------=-=-=-=-=-=-=-=-==-=-=-=-=-==--=--=-")
	local tmp = GameServer_pb.CMD_CHAT_CHANNEL_QUERY_SC();
	tmp:ParseFromString(pkg)
	Log("*********"..tostring(tmp))
    g_ChatWorldTimes = tmp.iRemaindTimes
    
    local widget = UI_GetBaseWidgetByName("Chat")
    if widget then
        local layout = UI_GetUILayout(widget, 1)
        local layoutTimes = UI_GetUILayout(layout, 4667358)
        local layoutCost = UI_GetUILayout(layout, 4667368)
        layoutTimes:setVisible(false)
        layoutCost:setVisible(false)

        if tmp.iRemaindTimes>0 then
            UI_GetUILabel(layoutTimes, 4667366):setText(FormatString("ChatFreeTimes",tmp.iRemaindTimes.."/"..tmp.iTotalTimes))
            layoutTimes:setVisible(true)
        else
            UI_GetUILabel(layoutCost, 4667372):setText(tmp.iCost)
            g_ChatWorldCost = tmp.iCost
            layoutCost:setVisible(true)
        end
    end
end
--保存来自服务器的聊天记录
function Chat_SaveData(lab,data)
    lab[#lab+1]=data
    if #lab>20 then  
        lab[#lab-21] = nil 
    end
    return lab
end

function sendGMMsg(cmd)
	local tab = GameServer_pb.Cmd_Cs_GmMsg();
	tab.strCmd = cmd;
	Packet_Full(GameServer_pb.CMD_GM_MSG, tab);
end


--添加消息到列表
function Chat_ShowData(listView,layout)
    listView:removeItem(19)
    listView:insertCustomItem(layout,0) 
end
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_WHISPER, "GetWhisper" );
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_CHANNEL_CHAT, "ChannelChat" );
--查询世界聊天的免费次数
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_CHAT_CHANNEL_QUERY, "Chat_ChannelQuery" );