--
-- TipTable
-- tipStr,confirmFun,
--
if( s_Messages == nil )then
	s_Messages = {}
end

if (s_MessageBoxIsShow == nil)then
	s_MessageBoxIsShow = false;
end

if (g_Dir == nil)then
	g_Dir = {x=0, y=0, z=0};
end

if (g_TurnDir == nil)then
	g_TurnDir = 0;
end

if (g_Timestamp == nil)then
	g_Timestamp = 0;
end

EMessageType_LeftRight=0
EMessageType_Middle=1
EMessageType_None=2
--EMessageType_Shake=3
--message
-- {
-- msgType    // (0, left right) (1, only middle) (2, none)
-- title,
-- info
-- leftText
-- leftFun
-- leftParams	--参数可以是一个table，也可以是一个数值。
-- rightText
-- rightFun
-- rightParams	--参数可以是一个table，也可以是一个数值。
-- closeFun
-- closeParams
-- textAlignment
-- deltaTime
-- width		--宽
-- height		--高
-- }

--添加提示框
function Messagebox_Create(message)
	Log("Messagebox_Create")
	s_Messages[#s_Messages+1] = message;
	Messagebox_Active();
end


--激活提示框
function Messagebox_Active()
	if (#s_Messages == 0 or s_MessageBoxIsShow)then
		return;
	end

	Log("Messagebox_Active")
	Messagebox_Show(s_Messages[1]);
end

function Messagebox_CallBack()
	s_MessageBoxIsShow = false;
	table.remove( s_Messages, 1 )	
	Messagebox_Active();
end

function Messagebox_OnClose()
	Log("Messagebox_OnClose")

	UIMgr:GetInstance():CloseMsgBaseWidget();
	--MainScene:GetInstance():removeChildByTag(EMSTag_Msg);
end

function Messagebox_Clear()
	local size = #s_Messages
	for i=1,size do
		local idx = size-i+1
		Messagebox_OnClose()
		table.remove( s_Messages, idx )	
	end
end

--显示Messagebox
function Messagebox_Show(message)	

	EndWaiting();

	--左键
	local function ClickLeft(sender,eventType)
		if eventType == TOUCH_EVENT_ENDED then
			Messagebox_OnClose();
			if (message.leftFun ~= nil)then
				message.leftFun(message.leftParams);
			end
		end
	end
	--右键
	local function ClickRight(sender,eventType)
		if eventType == TOUCH_EVENT_ENDED then
			Messagebox_OnClose();
			if (message.rightFun ~= nil)then
				message.rightFun(message.rightParams);
			end
		end
	end
	--close
	local function ClickClose(sender,eventType)
		if eventType == TOUCH_EVENT_ENDED then
			Messagebox_OnClose();
			if (message.closeFun ~= nil)then
				message.closeFun(message.closeParams);
			end
		end
	end
	local function onTouch(sender,eventType)
		if eventType == TOUCH_EVENT_ENDED then
			Messagebox_OnClose();
        end
	end
	
	
	--[[function On_Accelerate(x, y, z, timestamp)
		Log("On_Accelerate x="..x)
		local timels = os.time();
		
		if (x >= 0.15 and g_Dir.x < 0.15)then
			g_Dir.x = x;
			g_TurnDir = g_TurnDir + 1;
			g_Timestamp = timels;
		elseif (x <= -0.15 and g_Dir.x > -0.15)then
			g_Dir.x = x;
			g_TurnDir = g_TurnDir + 1;
			g_Timestamp = timels;
		elseif (y >= 0.15 and g_Dir.y < -0.15)then
			g_Dir.y = y;
			g_TurnDir = g_TurnDir + 1;
			g_Timestamp = timels;
		elseif (y <= -0.15 and g_Dir.y > -0.15)then
			g_Dir.y = y;
			g_TurnDir = g_TurnDir + 1;
			g_Timestamp = timels;
		elseif (z >= 0.15 and g_Dir.z < 0.15)then
			g_Dir.z = z;
			g_TurnDir = g_TurnDir + 1;
			g_Timestamp = timels;
		elseif (z <= -0.15 and g_Dir.z > -0.15)then
			g_Dir.z = z;
			g_TurnDir = g_TurnDir + 1;
			g_Timestamp = timels;
		end
		
		if (g_TurnDir > 0 and g_TurnDir < 10)then
			if (timels - g_Timestamp >= 2)then
				g_Dir = {x=0, y=0, z=0};
				g_TurnDir = 0;
				g_Timestamp = 0;
			end
		elseif (g_TurnDir >= 10)then
			if (message.leftFun ~= nil)then
				message.leftFun(message.leftParams);
			end
			
			Messagebox_OnClose();
		end
		
		--Log("g_TurnDir="..g_TurnDir)
	end
	
	g_Dir = {x=0, y=0, z=0};
	g_TurnDir = 0;
	g_Timestamp = 0;]]
	--local layerTouchProp = -200;
	--local buttonTouchProp = -210;
	local messageWidget = UI_CreateBaseWidgetByFileName("MessageBox.json", EUIOpenAction_Enlarge);
    
	--messageWidget:setPosition(MainScene:GetInstance():GetDesignSize().width/2, MainScene:GetInstance():GetDesignSize().height/2);
	local designSize = MainScene:GetInstance():GetDesignSize();
	local msgWidth = designSize.width/2 - 40;
	if(message.textAlignment == nil)then
		message.textAlignment = kCCTextAlignmentLeft;
	end
	--1水平间距 5垂直间距
	local infoCompLabel = CompLabel:GetCompLabelWithSpaces( message.info, msgWidth - 70, message.textAlignment,1,5);
	--Log("infoCompLabel:getContentSize().width"..infoCompLabel:getContentSize().width)
	--Log("infoCompLabel:getContentSize().height"..infoCompLabel:getContentSize().height)
	--infoCompLabel:setAnchorPoint(ccp(0.5, 0.5));

	local msgHeight = infoCompLabel:getContentSize().height + 200;
	local buttomOffY = -15
	--Log("msgWidth"..msgWidth.."msgHeight"..msgHeight)
	
	if message.width ~= nil and message.width > 0 then
		msgWidth = message.width
	end
	
	if message.height ~= nil and message.height > 0 then
		msgHeight = message.height
	end
	
	--if message.title == nil or message.title == "" then
		infoCompLabel:setPosition(ccp(480 - msgWidth/2 + 35, 320 + infoCompLabel:getContentSize().height/2 + 50));
	--else
		--infoCompLabel:setPosition(ccp(480 - msgWidth/2 + 50, 320 + infoCompLabel:getContentSize().height/2 + 10));
	--end
	local msgBG = nil
	if message.msgType == EMessageType_None or message.title == nil or message.title == "" then
		--local bgName = "ImgUi/popup4.png"
		msgBG = UI_GetUIImageView(messageWidget, 64);
	--elseif message.msgType == EMessageType_Shake then
		--local bgName = "ImgUi/popup4.png"
		--msgBG = CCSprite:create(bgName);
		--msgBG:setContentSize(CCSizeMake(msgWidth, msgHeight));	
	else
		--local bgName = "ImgUi/popup3.png"
		--msgBG = CCScale9Sprite:create(CCRectMake(30, 85, 30, 30),bgName);
		msgBG = UI_GetUIImageView(messageWidget, 63);
		
	end
	--Log("msgWidth"..msgWidth.."msgHeight"..msgHeight)
	msgBG:setSize(CCSizeMake(msgWidth, msgHeight));	
	msgBG:setVisible(true);
	msgBG:setPosition(ccp(designSize.width/2,designSize.height/2));
	local infoBG = UI_GetUIImageView(messageWidget, 1);
	infoBG:setSize(CCSizeMake(msgWidth-40, msgHeight-120))
	infoBG:setVisible(true);
	infoBG:setPosition(ccp(designSize.width/2,designSize.height/2+25));
	infoBG:setZOrder(5);
	messageWidget:addChild(infoCompLabel, 10, 100);
	
	if (message.deltaTime ~= nil and message.deltaTime > 0)then
		GetGlobalEntity():GetScheduler():RegisterScript( "Messagebox_OnClose", message.deltaTime, 1 )
	end
	
	--message.title = "ttttttttTTttt"
	--设置title
	if (message.title ~= nil)then
	
		-- local ttf = CompLabel:GetMenuTTFLab(message.title)
		-- ttf:setColor(ccc3(252, 0, 0));
		-- local skttf = CompLabel:createStroke(ttf, 2, ccc3(252,252,252));
		-- skttf:setPosition(-msgWidth/2 + 20, msgHeight/2 - 70);
		-- messageWidget:addChild(skttf);
		--local stitle = FormatString("CompTitleMake", message.title)
		--local titleLable = CompLabel:GetCompLabel(stitle, msgWidth, kCCTextAlignmentLeft);
		--titleLable:setPosition(ccp(480 - msgWidth/2 + 25, 320 + msgHeight/2 - 50));
		--titleLable:setAnchorPoint(ccp(0, 0))
		--messageWidget:addChild(titleLable, 10, 101);
	end
	
	--添加各种按钮
	if (message.msgType ~= nil and message.msgType == EMessageType_None)then --没有任何
        UI_GetUILayout(messageWidget, 4643996):addTouchEventListener( onTouch )
	--elseif (message.msgType ~= nil and message.msgType == EMessageType_Shake)then
		--local layer = tolua.cast( MainScene:GetInstance():GetUILayer():getChildByTag(EUiLayer_Mid), "CCLayer" );	
		--messageWidget:setAccelerometerEnabled(true);
		--messageWidget:registerScriptAccelerateHandler( On_Accelerate );
		--messageWidget:registerScriptTouchHandler( onTouch ,false,buttonTouchProp,true)
	else
		--添加关闭按钮
		--local closeButton = UI_GetUIButton( messageWidget, 67);
		--closeButton:setVisible(true);
		--closeButton:setPreferredSize(CCSize(75, 75));
		--closeButton:setPosition(ccp(480 + msgWidth/2 -7- (closeButton:getSize().width)/2, 320 + msgHeight/2 -7- (closeButton:getSize().height/2)));
		--closeButton:setTouchPriority(buttonTouchProp);
		--closeButton:setTouchEnabled(true);
		--messageWidget:addChild(closeButton);
	    --屏蔽背景的响应
		UI_GetUILayout(messageWidget, 4643996):setTouchEnabled(false)
		if (message.msgType ~= nil and message.msgType == EMessageType_Middle)then --有一个居中的按钮
			--closeButton:addTouchEventListener(ClickRight);
			if (message.leftText == nil)then
				message.leftText = "queding";
			end
			message.leftText = StringMgr:GetInstance():GetString(message.leftText);
			local leftButton = UI_GetUIButton( messageWidget, 65);--UIUtil_CreateLabelButton( "ImgUi/button/btnk1_1.png", "ImgUi/button/btnk1_2.png", message.leftText, ClickLeft )
			leftButton:setPosition(ccp(480, 320 + buttomOffY-msgHeight/2 + leftButton:getSize().height));
			leftButton:setTitleText(message.leftText);
			--leftButton:setTouchPriority(buttonTouchProp);
			--leftButton:setTouchEnabled(true);
			--messageWidget:addChild(leftButton);		
			leftButton:addTouchEventListener(ClickLeft);
			leftButton:setVisible(true)
		else
			--closeButton:addTouchEventListener(ClickLeft);
			--Log("3");
			--设置左边按钮
			if (message.leftText == nil)then
				message.leftText = "queding";
			end
			message.leftText = StringMgr:GetInstance():GetString(message.leftText);
			local leftButton = UI_GetUIButton( messageWidget, 65);--UIUtil_CreateLabelButton( "ImgUi/button/btnk1_1.png", "ImgUi/button/btnk1_2.png", message.leftText, ClickLeft )
			--leftButton:setPosition(ccp(480 - msgWidth/2 + leftButton:getSize().width/2 + 20, 320 + buttomOffY-msgHeight/2 + leftButton:getSize().height));
			leftButton:setPosition(ccp(480 + msgWidth/2 - leftButton:getSize().width/2 - 20, 320 + buttomOffY-msgHeight/2 + leftButton:getSize().height))
            leftButton:setTitleText(message.leftText);
			leftButton:addTouchEventListener(ClickLeft);
			leftButton:setVisible(true)
			--设置右边按钮
			if (message.rightText == nil)then
				message.rightText = "quxiao";
			end
			message.rightText = StringMgr:GetInstance():GetString(message.rightText);
			--[[local rightButton = UIUtil_CreateLabelButton( "ImgUi/button/btnk1_1.png", "ImgUi/button/btnk1_2.png", message.rightText, ClickRight )
			rightButton:setPosition(msgWidth/2 - rightButton:getContentSize().width - 5,buttomOffY -msgHeight/2 + rightButton:getContentSize().height);
			rightButton:setTouchPriority(buttonTouchProp);
			rightButton:setTouchEnabled(true);
			messageWidget:addChild(rightButton);]]
			local rightButton = UI_GetUIButton( messageWidget, 66);--UIUtil_CreateLabelButton( "ImgUi/button/btnk1_1.png", "ImgUi/button/btnk1_2.png", message.leftText, ClickLeft )
			--rightButton:setPosition(ccp(480 + msgWidth/2 - rightButton:getSize().width/2 - 20, 320 + buttomOffY-msgHeight/2 + rightButton:getSize().height));
			rightButton:setPosition(ccp(480 - msgWidth/2 + rightButton:getSize().width/2 + 20, 320 + buttomOffY-msgHeight/2 + rightButton:getSize().height))
            rightButton:setTitleText(message.rightText);
			rightButton:addTouchEventListener(ClickRight);
			rightButton:setVisible(true)
		end
		
		--if (message.closeFun ~= nil)then
			--Log("registerScriptTapHandler ClickClose")
			--closeButton:registerScriptTapHandler(GetLuaFuncId(ClickClose));
			--closeButton:addTouchEventListener(ClickClose);
		--end
	end
	
	--Log("showUI");
	s_MessageBoxIsShow = true;
end

function Reply_Server(params)
	local tab = GameServer_pb.Cmd_Cs_HyperLink();
	tab.strHyperLinkCmd = params;
	Packet_Full(GameServer_pb.CMD_HHYPER_LINK, tab);
	
	if (g_isGuide) then
		Guide_GoNext();
	end
end

function SC_Message(pkg)
	local tmp = GameServer_pb.Cmd_Sc_CommDlg();
	tmp:ParseFromString(pkg)
	local tab = {};
	Log(tmp.strMessage)
	tab.info = tmp.strMessage;
	tab.leftText = tmp.confirmData.strLeftText;
	tab.leftParams = tmp.confirmData.strLeftCmd;
	tab.leftFun = Reply_Server;
	tab.rightText = tmp.confirmData.strRightText;
	tab.rightParams = tmp.confirmData.strRightCmd;
	tab.rightFun = Reply_Server;
	tab.deltaTime = 60000;
	Messagebox_Create(tab);
end

ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_COMM_DLG, "SC_Message" );