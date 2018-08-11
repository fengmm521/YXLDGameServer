LoadFile("json.lua");


--- ÓëLoginÍ¬¼¶µÄlua´úÂë, ÓÉÓÚLogin.lua ÒÑ¾­±È½ÏÂÒ, ¸Ä¶¯¿àÊÖ£¬ËùÒÔÖØÐÂ¸ãÒ»¸ö¡£


-- È«¾Ö±äÁ¿
-- È«¾Ö±äÁ¿ end
if (g_networkData == nil) then
    g_networkData = {};
end

if (g_worldListData == nil) then
	g_worldListData = {};
end

if (g_recordUserData == nil) then
	g_recordUserData = {};
end


local l_sceneWidget = nil;          -- ³¡¾°widget
local l_panelMainMenu = nil;        -- Ö÷²Ëµ¥
local l_panelACMainMenu = nil;      -- ÕÊºÅÖÐÐÄÖ÷²Ëµ¥
local l_panelACRegMenu = nil;	    -- ÕÊºÅÖÐÐÄ×¢²á²Ëµ¥
local l_panelACLoginMenu = nil;	    -- ÕÊºÅÖÐÐÄµÇÂ¼²Ëµ¥
local l_panelACSMSMenu	= nil;	    -- ÕÊºÅÖÐÐÄÊÖ»úºÅÊäÈë
local l_panelACSMSVerifyMenu = nil; -- ÊÖ»úÐ£ÑéÂëÊäÈë

local l_authedUserID = nil;		-- µ±Ç°Í¨¹ýÊÚÈ¨µÄÕÊºÅid

-- ÊÖ»úÐ£ÑéÏà¹Ø±äÁ¿
local l_mobile = nil;
local l_smsToken = nil;
local l_vcode = nil;
local l_smsWaiting = false;
local l_smsWaitSec = 0;

local l_resendName = nil;


-- ³õÊ¼»¯2144ÏµÍ³
-- TODO:ÕâÑù¸ã¿ÉÄÜ²»ÊÇºÜ°²È«
local ACC_2144_GUEST_REG_URL        = "http://notify.2144.cn/ios/guest/register";    -- ÓÎ¿Í´´½¨URL
local ACC_2144_GUEST_LOGIN_URL      = "http://notify.2144.cn/ios/guest/login";       -- ÓÎ¿ÍµÇÂ¼URL
local ACC_2144_REG_URL              = "http://notify.2144.cn/ios/user/register";     -- ×¢²áURL
local ACC_2144_LOGIN_URL            = "http://notify.2144.cn/ios/user/login";        -- µÇÂ¼URL
local ACC_2144_QUICK_LOGIN_URL      = "http://notify.2144.cn/ios/user/autologin";    -- ¿ìËÙµÇÂ¼URL
local ACC_2144_SMS_URL              = "http://notify.2144.cn/ios/sms/send";          -- ¶ÌÐÅ·¢ËÍURL
local ACC_2144_SMS_VERIFY_URL       = "http://notify.2144.cn/ios/sms/verify";        -- ¶ÌÐÅÐ£ÑéURL

local ACC_2144_APP_KEY      = "080bdae1feda212f";
local ACC_2144_PICI         = "HERO_20150518_B001";
local ACC_2144_APP_SECRET   = "afbfc29ad1380434";

-- ³õÊ¼»¯2144ÏµÍ³ END

local isTOuch = false;
-- ´òÓ¡ÕÊºÅµÇÂ¼¼ÇÂ¼
-- µ÷ÊÔ·½·¨
local function __printRecordUserData()
	for i = 1, #g_recordUserData do
		local record = g_recordUserData[i];
		Log("RECORD_USER_DATA["..i.."] serverID:"..record.serverID .." serverName:"..record.serverName.." userName:"..record.userName.." token:"..record.token.." isGuest:"..record.isGuest.." isLatest:"..record.isLatest);
	end
end

-- ±£´æÓÃ»§µÇÂ¼¼ÇÂ¼µ½ÎÄ¼þÖÐ
local function __saveRecordUserData()
    Log(">>>>>>>>>> __saveRecordUserData() invoked.");
	local xmlWrapper = XmlWrapper:new_local();
	local doc = xmlWrapper:GetDoc();
	local rootNode = doc:NewElement( "UserDatas" );
	-- for i = 1, #g_recordUserData do
	-- 	local loginRecord = g_recordUserData[i];
		-- local xmlLoginRecord = doc:NewElement( "LoginRecord" );
		-- if loginRecord.serverID ~= nil then
		-- xmlLoginRecord:SetAttribute( "ServerId", TypeConvert:ToString(loginRecord.serverID) );
		-- end
		-- if loginRecord.serverName ~= nil then
		-- xmlLoginRecord:SetAttribute( "ServerName", loginRecord.serverName );
		-- end
		-- if loginRecord.userID ~= nil then
		-- xmlLoginRecord:SetAttribute( "UserId", loginRecord.userID);
		-- end
		-- if loginRecord.userName ~= nil then
		-- xmlLoginRecord:SetAttribute( "UserName", loginRecord.userName);
		-- end
		-- if loginRecord.token ~= nil then
		-- xmlLoginRecord:SetAttribute( "Token", loginRecord.token);
		-- end
		-- if loginRecord.isGuest ~= nil then
		-- xmlLoginRecord:SetAttribute( "IsGuest", TypeConvert:ToString(loginRecord.isGuest));
		-- end
		-- if loginRecord.isLatest ~= nil then
		-- xmlLoginRecord:SetAttribute( "IsLatest", TypeConvert:ToString(loginRecord.isLatest));
		-- end
		-- rootNode:InsertEndChild( xmlLoginRecord );
	-- end

    local xmlLoginRecord = doc:NewElement( "LoginRecord" );

    xmlLoginRecord:SetAttribute( "ServerId", TypeConvert:ToString(0) );

    xmlLoginRecord:SetAttribute( "ServerName", "仗剑天涯" );


    xmlLoginRecord:SetAttribute( "UserId", "123457");


    xmlLoginRecord:SetAttribute( "UserName", "KKWout6");


    xmlLoginRecord:SetAttribute( "Token", "BgdLS1dvdXQwHCw=");

    xmlLoginRecord:SetAttribute( "IsGuest", TypeConvert:ToString(1));


    xmlLoginRecord:SetAttribute( "IsLatest", TypeConvert:ToString(1));

    rootNode:InsertEndChild( xmlLoginRecord );

	doc:InsertEndChild( rootNode )
	doc:SaveFile( (CCFileUtils:sharedFileUtils():getWritablePath()).."IOS_RecordUserData.xml", false );
	doc:Clear();
end


-- ´ÓÎÄ¼þ¼ÓÔØÕÊºÅµÇÂ¼¼ÇÂ¼
local function __loadRecordUserData()
	Log(">>>>>>>>>>>>>>> __loadRecordUserData invoked.");
	g_recordUserData = {};
	local isPopup = CCFileUtils:sharedFileUtils():isPopupNotify()
	CCFileUtils:sharedFileUtils():setPopupNotify( false )
	local xmlWrapper = XmlWrapper:new_local();
	local doc = xmlWrapper:GetDoc();
	if( xmlWrapper:LoadFile( (CCFileUtils:sharedFileUtils():getWritablePath()).."IOS_RecordUserData.xml" ) )then
		local i = 1;
		local rootNode = doc:FirstChildElement( "UserDatas" );
		if( rootNode ~= nil )then
			local xmlLoginRecord = rootNode:FirstChildElement("LoginRecord");
			while xmlLoginRecord ~= nil do
				local loginRecord = {};
				loginRecord.serverID = TypeConvert:ToUint(xmlLoginRecord:Attribute("ServerId"));
				loginRecord.serverName = xmlLoginRecord:Attribute("ServerName");
				loginRecord.userID = xmlLoginRecord:Attribute("UserId");
				loginRecord.userName = xmlLoginRecord:Attribute("UserName");
				loginRecord.token = xmlLoginRecord:Attribute("Token");
				loginRecord.isGuest = TypeConvert:ToUint(xmlLoginRecord:Attribute("IsGuest"));
				loginRecord.isLatest = TypeConvert:ToUint(xmlLoginRecord:Attribute("IsLatest"));
				
				g_recordUserData[i] = loginRecord;
				xmlLoginRecord = xmlLoginRecord:NextSiblingElement("LoginRecord");
				i = i + 1;
			end
		end
		doc:Clear();
	end
	CCFileUtils:sharedFileUtils():setPopupNotify(isPopup)
end

-- ´ÓÁÐ±íÖÐ»ñÈ¡µ±Ç°µÇÂ¼¼ÇÂ¼
local function __getLatestRecordUserData()
	local record = nil;
	for i = 1, #g_recordUserData do
		record = g_recordUserData[i];
		if record.isLatest == 1 then
			return record;
		end
	end
	return record;
end

-- Ìí¼ÓÐÂÕÊºÅÊý¾Ý
local function __addRecordUserData(_record)
	for i = 1, #g_recordUserData do
		local record = g_recordUserData[i];
		if record.userID == _record.userID then
            if  _record.serverID ~= nil then
                record.serverID = _record.serverID;
            end
            if  _record.serverName ~= nil then
                record.serverName = _record.serverName;
            end
            if  _record.userName ~= nil then
                record.userName = _record.userName;
            end
            if  _record.token ~= nil then
                record.token = _record.token;
            end
            if  _record.isGuest ~= nil then
                record.isGuest = _record.isGuest;
            end
            if  _record.isLatest ~= nil then
                record.isLatest = _record.isLatest;
            end
			return;
		end
	end
	g_recordUserData[#g_recordUserData + 1] = _record;
end

-- ÉèÖÃµ±Ç°Ñ¡ÖÐÕÊºÅ
local function __setLatestRecordUserData(_userName)
	for i = 1, #g_recordUserData do
		local record = g_recordUserData[i];
		if record.userName == _userName then
			record.isLatest = 1;
		else
			record.isLatest = 0;
		end
	end
end

-- ¼ÓÔØÍøÂçÅäÖÃ
local function __loadNetwork()
	local xmlWrapper = XmlWrapper:new_local();
	local doc = xmlWrapper:GetDoc();
	if( xmlWrapper:LoadFile( "Config/Network.xml" ) )then
		local rootNode = doc:FirstChildElement( "Servers" );
		if( rootNode ~= nil )then
			local valueNode = rootNode:FirstChildElement( "Server" );
			g_networkData.IP = valueNode:Attribute( "IP" )
			g_networkData.Port = TypeConvert:ToUint( valueNode:Attribute( "Port" ) )							
		end
		doc:Clear();
	end
end


-- ÄÚ²¿·½·¨ : ³õÊ¼»¯2144ÏµÍ³
local function __init2144()
    -- IOSSDK:setAcc2144URL( ACC_2144_GUEST_REG_URL,
				-- 	        ACC_2144_GUEST_LOGIN_URL,
				-- 	        ACC_2144_REG_URL,
				-- 	        ACC_2144_LOGIN_URL,
    --                         ACC_2144_QUICK_LOGIN_URL,
				-- 	        ACC_2144_SMS_URL,
				-- 	        ACC_2144_SMS_VERIFY_URL);

    -- IOSSDK:setAcc2144Info(    ACC_2144_APP_SECRET,
    --                             ACC_2144_PICI,
    --                             ACC_2144_APP_KEY);



    -- IOSSDK:acc2144SetLuaCallback("IOSLogin_on2144HttpResponse");
    IOSLogin_on2144HttpResponse( "none");
end


-- ÄÚ²¿·½·¨ : ×¢²áÍøÂçÊÂ¼þ
local function __registerScriptFunc()
    ScriptSys:GetInstance():RegisterScriptFunc( RouterServer_pb.RS_ROUTER_AUTH, "IOSLogin_OnAuthRsp" );
    ScriptSys:GetInstance():RegisterScriptFunc( RouterServer_pb.RS_ROUTER_WORLDLIST, "IOSLogin_OnWorldListRsp" );
    ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_ALOGIN, "IOSLogin_OnLoginRsp" );
    -- ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_KICKOFF_ACCOUNT, "Kickoff" );
    -- ScriptSys:GetInstance():RegisterScriptFunc( RouterServer_pb.RS_ROUTER_ERROR, "LoginError" );
end

-- ÄÚ²¿·½·¨ : È¡ÏûÍøÂçÊÂ¼þ
local function __unregisterScriptFunc()
    ScriptSys:GetInstance():UnRegisterScriptFunc( RouterServer_pb.RS_ROUTER_AUTH);
    ScriptSys:GetInstance():UnRegisterScriptFunc( RouterServer_pb.RS_ROUTER_WORLDLIST);
    ScriptSys:GetInstance():UnRegisterScriptFunc( GameServer_pb.CMD_ALOGIN);
    -- ScriptSys:GetInstance():UnRegisterScriptFunc( GameServer_pb.CMD_KICKOFF_ACCOUNT);
    -- ScriptSys:GetInstance():UnRegisterScriptFunc( RouterServer_pb.RS_ROUTER_ERROR);
end


-- ÏÔÊ¾ÕÊºÅÖÐÐÄÖ÷²Ëµ¥
-- »áÒþ²ØLoginÖ÷²Ëµ¥£¬²¢ÏÔÊ¾ÕÊºÅÖÐÐÄÖ÷²Ëµ¥
local function __showACMainMenu()
    l_panelMainMenu:setVisible(false);
    l_panelACMainMenu:setVisible(true);
	l_panelACRegMenu:setVisible(false);
	l_panelACLoginMenu:setVisible(false);
	l_panelACSMSMenu:setVisible(false);
    l_panelACSMSVerifyMenu:setVisible(false);
end

-- ÏÔÊ¾ÕÊºÅÖÐÐÄ×¢²á²Ëµ¥
local function __showACRegMenu()
	l_panelMainMenu:setVisible(false);
    l_panelACMainMenu:setVisible(false);
	l_panelACRegMenu:setVisible(true);
	l_panelACLoginMenu:setVisible(false);
	l_panelACSMSMenu:setVisible(false);
    l_panelACSMSVerifyMenu:setVisible(false);
end

-- ÏÔÊ¾Ö÷²Ëµ¥
local function __showMainMenu()
	l_panelMainMenu:setVisible(true);
	l_panelACMainMenu:setVisible(false);
	l_panelACRegMenu:setVisible(false);
	l_panelACLoginMenu:setVisible(false);
	l_panelACSMSMenu:setVisible(false);
    l_panelACSMSVerifyMenu:setVisible(false);
end

-- ÏÔÊ¾µÇÂ¼²Ëµ¥
local function __showACLoginMenu()
	l_panelMainMenu:setVisible(false);
	l_panelACMainMenu:setVisible(false);
	l_panelACRegMenu:setVisible(false);
	l_panelACLoginMenu:setVisible(true);
	l_panelACSMSMenu:setVisible(false);
    l_panelACSMSVerifyMenu:setVisible(false);
end

-- ÏÔÊ¾ÊÖ»úÊäÈë½çÃæ
local function __showACSMSMenu()
	l_panelMainMenu:setVisible(false);
	l_panelACMainMenu:setVisible(false);
	l_panelACRegMenu:setVisible(false);
	l_panelACLoginMenu:setVisible(false);
	l_panelACSMSMenu:setVisible(true);
    l_panelACSMSVerifyMenu:setVisible(false);
end

-- ÏÔÊ¾ÊÖ»úÐ£ÑéÂëÊäÈë
local function __showACSMSVerifyMenu()
	l_panelMainMenu:setVisible(false);
	l_panelACMainMenu:setVisible(false);
	l_panelACRegMenu:setVisible(false);
	l_panelACLoginMenu:setVisible(false);
	l_panelACSMSMenu:setVisible(false);
    l_panelACSMSVerifyMenu:setVisible(true);
end


-- ÄÚ²¿·½·¨ : Á¬½Ó·þÎñÆ÷
local function __connectServer()
    Log(">>>>>>>>>>>>>> Connect to server at "..g_networkData.IP..":"..g_networkData.Port);
	ClientSink:Connect(g_networkData.IP, g_networkData.Port);
end
-- xmlLoginRecord:SetAttribute( "UserId", "123457");


--     xmlLoginRecord:SetAttribute( "UserName", "KKWout6");


--     xmlLoginRecord:SetAttribute( "Token", "BgdLS1dvdXQwHCw=");
-- ·¢Æðauth ÇëÇó
local function __sendAuth()
    local authTab= RouterServer_pb.CS_RS_Auth();
	local record = __getLatestRecordUserData();
	authTab.strAccount = "KKWout6";
	authTab.strMd5Passwd = "BgdLS1dvdXQwHCw="
	Packet_Full(RouterServer_pb.RS_ROUTER_AUTH, authTab);
end

-- µÇÂ¼ÎÒÃÇµÄ·þÎñÆ÷
-- Í¬Ê±½«g_recordUserDataÐ´ÈëÎÄ¼þ
local function __login(_serverID)
	Log(">>>>>>>>> __login, serverID:".._serverID);
	local version = "";
	local channelinfor = Common:GetChannelInfor();
	local jValue = ParseJson(channelinfor)
	if jValue~=nil then
		version = Cjson:GetStr(jValue,"Version")
	end
	version = "1.1.6"
	
	local versionTab = {};
	local function setValue(a)
		versionTab[#versionTab + 1] = a
	end
	string.gsub(version, "(%w+)", setValue);
	local aloginTab = GameServer_pb.Cmd_Cs_Alogin();
	aloginTab.iIsVersionCheck = 0;
	aloginTab.iVersionType = 1;
	-- aloginTab.iVersionMain = TypeConvert:ToInt(versionTab[1]);
	-- aloginTab.iVersionFeature = TypeConvert:ToInt(versionTab[2]);
	-- aloginTab.iVersionBuild = TypeConvert:ToInt(versionTab[3]);
    aloginTab.iVersionMain = 1;
    aloginTab.iVersionFeature = 1;
    aloginTab.iVersionBuild = 6;
    aloginTab.strChannelFlag = "2144";    -- ÕâÀï2144²»ÖªµÀ¶Ô²»¶Ô

	local msg = GameServer_pb.CSMessage();
	msg.iCmd = GameServer_pb.CMD_ALOGIN;
	if (aloginTab) then
		msg.strMsgBody = aloginTab:SerializeToString();
	end
	
	local msgStr = msg:SerializeToString();
   
	local rsAloginTab = RouterServer_pb.CS_RS_ALogin();
	rsAloginTab.iWorldID = _serverID;
	rsAloginTab.strTransMsg = msgStr;
		
	Packet_Full(RouterServer_pb.RS_ROUTER_ALOGIN, rsAloginTab);
	ClientSink:GetInstance():SetIsConnectGameServer(true);
end


-- Ë¢ÐÂÖ÷²Ëµ¥, ÕÊºÅÑ¡Ôñ
local function __refreshACMainMenuAccountSelect()
	local btnFastGame = UI_GetUIButton(l_panelACMainMenu, 20001);
	local panelSelect = UI_GetUILayout(l_panelACMainMenu, 20005);
	if g_recordUserData == nil or #g_recordUserData == 0 then
		btnFastGame:setVisible(true);
		panelSelect:setVisible(false);
	else
		btnFastGame:setVisible(false);
		panelSelect:setVisible(true);
		
		-- ÏÔÊ¾µ±Ç°½ÇÉ«
		local latestRecord = __getLatestRecordUserData();
		local labelSelectUserName = UI_GetUILabel(panelSelect, 200052);
		labelSelectUserName:setText(latestRecord.userName);
		
		
		
		-- ÏÔÊ¾ÁÐ±í
		local itemTemplate = UI_GetUILayout(panelSelect, 200055);
		local imageListBG = UI_GetUIImageView(panelSelect, 200054);
		local listView = UI_GetUIListView(imageListBG, 2000541);
		listView:removeAllItems();
		
		local record = nil;
		local item = nil;
		local labelItem = nil;
		for i = 1, #g_recordUserData do
			record = g_recordUserData[i];
			
			if record.userID ~= latestRecord.userID then
				item = itemTemplate:clone();
				labelItem = UI_GetUILabel(item, 2000551);
				
				-- ¹Ø±ÕÁÐ±í
				-- ÇÐ»»Ñ¡ÖÐÕÊºÅ
				-- Ë¢ÐÂÏÔÊ¾
				local function __onItemClick(_sender, _eventType)
					if _eventType == TOUCH_EVENT_ENDED then
						local index = _sender:getTag();
						local clickRecord = g_recordUserData[index];
						Log("__onItemClick:"..clickRecord.userName);
						imageListBG:setVisible(false);
						__setLatestRecordUserData(clickRecord.userName);
                        __saveRecordUserData();
						IOSLogin_RefreshView();
					end
				end
				
				labelItem:setText(record.userName);
				item:setVisible(true);
				item:setTouchEnabled(true);
				item:setTag(i);
				item:addTouchEventListener(__onItemClick);
				listView:pushBackCustomItem(item);
			end
		end
		
		local function __onOtherItemClick(_sender, _eventType)
			if _eventType == TOUCH_EVENT_ENDED then
				imageListBG:setVisible(false);
				__showACLoginMenu();
			end
		end
		
		-- Ìí¼ÓÆäËûÕÊºÅitem
		item = itemTemplate:clone();
		item:setVisible(true);
		labelItem = UI_GetUILabel(item, 2000551);
		labelItem:setText(FormatString("OtherAccount"));
		item:setTouchEnabled(true);
		item:addTouchEventListener(__onOtherItemClick);
		listView:pushBackCustomItem(item);
		
		
		-- ÒÔÏÂ´úÂëÃ»Æð×÷ÓÃ
		-- local w = listView:getContentSize().width + 6;
		-- local h = listView:getContentSize().height + 6;
		-- imageListBG:setSize(CCSizeMake(w, h));
	end
end


-- ÄÚ²¿·½·¨ : Ë¢ÐÂÏÔÊ¾
-- ÏÔÊ¾Êý¾Ý½ö½öÒÀÀµÓÚg_RecordUserData
function IOSLogin_RefreshView()
	local record = __getLatestRecordUserData();
    -- ÏÔÊ¾ÓÃ»§Ãû
    local labelAccountName = UI_GetUILabel(l_panelMainMenu, 10001);
    if  record~=nil and record.userName ~= nil then
        labelAccountName:setText(record.userName);
    else
        labelAccountName:setText("");
    end

    -- ÏÔÊ¾ÊÀ½ç
    local labelServerName = UI_GetUILabel(l_panelMainMenu, 10005);
    if (record ~= nil and record.serverName ~= nil) then
        labelServerName:setText(FormatString("Login_ServerID",record.serverID+1).." "..record.serverName);
    else
        labelServerName:setText("");
    end
	
	__refreshACMainMenuAccountSelect();
end



-- ÄÚ²¿·½·¨ : ½øÈëÓÎÏ·°´Å¥µã»÷
-- 1) ¶ÁÈ¡UserName ºÍ accessToken
-- 2) Ìá½»2144¿ìËÙµÇÂ¼
local function __onEntergameBtnClick(_sender, _eventType)
    if (_eventType == TOUCH_EVENT_ENDED) then
		-- local record = __getLatestRecordUserData();
  --       ShowWaiting();
		-- if record.isGuest == 1 then
		-- 	IOSSDK:acc2144GuestLoginRequest(record.userID, record.token);
		-- else
  --           IOSSDK:acc2144QuickLoginRequest(record.userID, record.token);
		-- end
        Log("enter game btn touch");
        if isTOuch == false then
            IOSLogin_on2144HttpResponse("test");
            isTOuch = true;
        else
            __login(0);
        end
        -- 
        
    end
end

-- ÄÚ²¿·½·¨ : ÕÊºÅÖÐÐÄ°´Å¥µã»÷
local function __onAccountCenterBtnClick(_sender, _eventType)
	if (_eventType == TOUCH_EVENT_ENDED) then
		Log("__onAccountCenterBtnClick : invoked");
		__showACMainMenu();
	end
end

--³õÊ¼»¯Ò»¸ö·þÎñÆ÷µÄÊý¾Ý
-- DG*W&Y#&*^$#&@
function __initOnServer(data,tag,isCheckHero,isShowLine,isTouchEnabled,Touch)
    local widget = UI_GetBaseWidgetByName("ServerList")
    if widget then
        local oneServer = UI_GetUILayout(widget, 1122):clone()
        oneServer:setVisible(true)
        oneServer:setTag(tag+1520)
        if isTouchEnabled == true then
            oneServer:setTouchEnabled(true)
            oneServer:addTouchEventListener(Touch)
        else
            oneServer:setTouchEnabled(false)
        end

        local colorID = ccc3(255,255,255)
        if data.iOnlineStatus ~= 1 then--ÀëÏß
            colorID = ccc3(175,175,175)
        elseif data.iAtt == 0 then --»ð±¬
            UI_GetUIImageView(oneServer, 2101):setVisible(true)
            colorID = ccc3(255,130,40)
        else--ÐÂÇø
            UI_GetUIImageView(oneServer, 2102):setVisible(true)
            colorID = ccc3(255,230,40)
        end

        --·þÎñÆ÷Ãû×Ö           
        UI_GetUILabel(oneServer, 1002):setText(data.strWorldName)
        UI_GetUILabel(oneServer, 1002):setColor(colorID)
        UI_GetUILabel(oneServer, 1002):enableStroke()

        --·þÎñÆ÷ID
        UI_GetUILabel(oneServer, 1001):setText(FormatString("Login_ServerID",data.iWorldID+1))
        UI_GetUILabel(oneServer, 1001):setColor(colorID)
        UI_GetUILabel(oneServer, 1001):enableStroke()

        if isCheckHero==true and data.bHaveRole == true then
            UI_GetUIImageView(oneServer, 1004):setVisible(true)
        else
            UI_GetUIImageView(oneServer, 1004):setVisible(false)
        end
        if isCheckHero==true then
            UI_GetUIImageView(oneServer, 1005):setVisible(true)
        else
            UI_GetUIImageView(oneServer, 1005):setVisible(false)
        end
        return oneServer
    else
        return nil
    end
end


-- Ñ¡Ôñ·þÎñÆ÷°´Å¥µã»÷
-- ÏÔÊ¾·þÎñÆ÷ÁÐ±í
local function __onServerBtnClick(_sender, _eventType)
    if (_eventType == TOUCH_EVENT_ENDED) then
        Log(">>>>>>>>>>>>>>>> __onServerBtnClick invoked.");
		local record = __getLatestRecordUserData();
		
        local widget = UI_CreateBaseWidgetByFileName("ServerList.json", EUIOpenAction_MoveIn_Top, 1.5); 
	    UI_GetUILayout(widget, 4722820):setTouchEnabled(false)
        local function exit(sender,eventType)
            if sender == TOUCH_EVENT_ENDED then
            
            end
        end

        local function chooseServer(sender,eventType)
            if eventType == TOUCH_EVENT_ENDED then
                UI_ClickCloseCurBaseWidgetWithNoAction(sender, eventType);
                local tag = tolua.cast(sender,"Layout"):getTag();
                local choosedServerIndex = tag - 1520;
				record.serverID = g_worldListData[choosedServerIndex].iWorldID;
				record.serverName = g_worldListData[choosedServerIndex].strWorldName;
                --g_RecordUserData.ServerId = g_worldListData[choosedServerIndex].iWorldID;
                --g_RecordUserData.ServerName = g_worldListData[choosedServerIndex].strWorldName;
                IOSLogin_RefreshView();
            end
        end

	    --if (g_RecordUserData.ServerName ~= nil and g_RecordUserData.ServerName ~= "")then
		if (record.serverName ~= nil and record.serverName ~= "") then
		    for i=1, #g_worldListData do
			    if (g_worldListData[i].strWorldName == record.serverName)then
                    local lastServer = UI_GetUILayout(widget, 1123)
                    lastServer:removeAllChildren()
                
                    local oneServer = __initOnServer(g_worldListData[i],i,false,false,true,chooseServer)
                    if oneServer ~= nil then
                        oneServer:setPosition(ccp(0,0))
                        lastServer:addChild(oneServer)
                        UI_GetUIImageView(widget, 5101):setVisible(UI_GetUIImageView(oneServer, 2101):isVisible())
                        UI_GetUIImageView(widget, 5102):setVisible(UI_GetUIImageView(oneServer, 2102):isVisible())
                        UI_GetUIImageView(oneServer, 2101):setVisible(false)
                        UI_GetUIImageView(oneServer, 2102):setVisible(false)
                    end
				    break;
			    end
		    end		
	    end
	
	    local listView = UI_GetUIListView(widget, 1121);
        listView:removeAllItems()
        local listViewOne = nil
	    for i=#g_worldListData, 1,-1 do
            local oneServer = __initOnServer(g_worldListData[i],i,true,true,true,chooseServer)

            if (#g_worldListData-i)%2 == 0 then
                listViewOne = tolua.cast(UI_GetUIListView(widget, 1021):clone(),"ListView")
                listViewOne:setVisible(true)
                listView:pushBackCustomItem(listViewOne)
            end

            if oneServer ~= nil then
                listViewOne:pushBackCustomItem(oneServer)
            end
	    end
    end
end

-- ÏìÓ¦ÕÊºÅÖÐÐÄ, ·µ»Ø±êÇ©µã»÷
local function __onACMainMenuBackLabelClick(_sender, _eventType)
	if _eventType == TOUCH_EVENT_ENDED then
		local record = __getLatestRecordUserData();
		if record ~=nil and record.userID ~= l_authedUserID then
			ShowWaiting();
			__connectServer();
		end
		__showMainMenu();
	end
end

-- ÏìÓ¦ÕÊºÅÖÐÐÄ, ¿ìËÙÓÎÏ·°´Å¥µã»÷
local function __onACMainMenuFastGameBtnClick(_sender, _eventType)
	if _eventType == TOUCH_EVENT_ENDED then
		__onEntergameBtnClick(_sender, _eventType);
	end
end

-- ÕÊºÅÖÐÐÄ : µÇÂ¼°´Å¥µã»÷
local function __onACMainMenuLoginBtnClick(_sender, _eventType)
	if _eventType == TOUCH_EVENT_ENDED then
		local record = __getLatestRecordUserData();
		if record ~= nil then
			if record.userID ~= l_authedUserID then
				ShowWaiting();
				__connectServer();
			end
			__showMainMenu();
		end
	end
end

-- ÏìÓ¦ÕÊºÅÖÐÐÄ, ×¢²á°´Å¥µã»÷
local function __onACMainMenuRegBtnClick(_sender, _eventType)
	if _eventType == TOUCH_EVENT_ENDED then
		Log(">>>>>>>>>>>>>>> __onACMainMenuRegBtnClick invoked.");
		__showACRegMenu();
	end
end


-- ÏìÓ¦ÕÊºÅÖÐÐÄ, ×¢²áÃæ°å, ×¢²á°´Å¥µã»÷
-- ÊÕ¼¯ÕÊºÅÃûºÍÃÜÂë
-- Ìá½»2144ÇëÇó×¢²á
local function __onACRegMenuRegBtnClick(_sender, _eventType)
	if _eventType == TOUCH_EVENT_ENDED then
		Log(">>>>>>>>>>>>>>> __onACRegMenuRegBtnClick invoked.");
		local nameInput = UI_GetUITextField(l_panelACRegMenu, 30001);
		local passwordInput = UI_GetUITextField(l_panelACRegMenu, 30002);
		local name = nameInput:getStringValue();
		local password = passwordInput:getStringValue();
		Log(">>>>>>>>>>>>>>> name:"..name.." password:"..password);
		
		ShowWaiting();
        IOSSDK:acc2144RegRequest(name, password);
	end
end

-- ÕÊºÅÖÐÐÄ, ×¢²áÃæ°å, ·µ»Ø°´Å¥µã»÷
local function __onACRegMenuBackBtnClick(_sender, _eventType)
	if _eventType == TOUCH_EVENT_ENDED then
		__showACMainMenu();
	end
end

local function __onACLoginMenuBackBtnClick(_sender, _eventType)
	if _eventType == TOUCH_EVENT_ENDED then
		__showACMainMenu();
	end
end

local function __onACRegMenuMobileBtnClick(_sender, _eventType)
	if _eventType == TOUCH_EVENT_ENDED then
		__showACSMSMenu();
	end
end

local function __onACLoginMenuMobileBtnClick(_sender, _eventType)
	if _eventType == TOUCH_EVENT_ENDED then
		__showACSMSMenu();
	end
end

local function __onACSMSMenuBackBtnClick(_sender, _eventType)
	if _eventType == TOUCH_EVENT_ENDED then
		__showACMainMenu();
	end
end

local function __onACSMSVerifyMenuBackBtnClick(_sender, _eventType)
    if _eventType == TOUCH_EVENT_ENDED then
        __showACMainMenu();
    end
end

-- ÏìÓ¦ÕÊºÅÖÐÐÄ, ×¢²áÃæ°å, ÏÔÊ¾ÕÊºÅ²Ëµ¥
local function __onACRegMenuOpenListClick(_sender, _eventType)
	if _eventType == TOUCH_EVENT_ENDED then
		Log(">>>>>>>>>>>>>>>>> __onACRegMenuOpenListClick");
		local panelSelect = UI_GetUILayout(l_panelACMainMenu, 20005);
		local imageListBG = UI_GetUIImageView(panelSelect, 200054);
		if imageListBG:isVisible() then
			imageListBG:setVisible(false);
		else   
			imageListBG:setVisible(true);
		end
	end
end

-- µÇÂ¼°´Å¥µã»÷
-- ÇëÇó2144µÇÂ¼
local function __onACLoginMenuLoginBtnClick(_sender, _eventType)
	if _eventType == TOUCH_EVENT_ENDED then
		local nameInput = UI_GetUITextField(l_panelACLoginMenu, 40003);
		local passwordInput = UI_GetUITextField(l_panelACLoginMenu, 40004);
		local name = nameInput:getStringValue();
		local password = passwordInput:getStringValue();
		Log(">>>>>>>>>>>>>>> name:"..name.." password:"..password);
		
		ShowWaiting();
        IOSSDK:acc2144LoginRequest(name, password);
	end
end

-- ·¢ËÍÐ£ÑéÂë°´Å¥µã»÷
local function __onACSMSMenuSendSMSBtnClick(_sender, _eventType)
	if _eventType == TOUCH_EVENT_ENDED then
		local mobileInput = UI_GetUITextField(l_panelACSMSMenu, 50001);
		local mobile = mobileInput:getStringValue();
		Log(">>>>>>>>>>>>>>> mobile:"..mobile);
		
		ShowWaiting();
        l_mobile = mobile;
		IOSSDK:acc2144SMSRequest(mobile);
	end
end

-- Ð£Ñé°´Å¥µã»÷
local function __onACSMSVerifyMenuVerifyBtnClick(_sender, _eventType)
    if _eventType == TOUCH_EVENT_ENDED then
        local vcodeInput = UI_GetUITextField(l_panelACSMSVerifyMenu, 60001);
        local vcode = vcodeInput:getStringValue();
        Log(">>>>>>>>>>>>>>> vcode:"..vcode);

        ShowWaiting();
        l_vcode = vcode;
        IOSSDK:acc2144SMSVerifyRequest(l_mobile, vcode, l_smsToken);
    end
end

local function __onASSMSVerifyMenuResendBtnClick(_sender, _eventType)
    if _eventType == TOUCH_EVENT_ENDED and l_mobile ~= nil then
		ShowWaiting();
		IOSSDK:acc2144SMSRequest(l_mobile);
    end
end

function IOSLogin_Create()
    Log(">>>>>>>> IOSLogin_Create invoked.");
	
	__loadRecordUserData();
	--__printRecordUserData();
	
    -- °ó¶¨ÍøÂçÊÂ¼þ
    __registerScriptFunc();

    -- ¼ÓÔØÍøÂçÅäÖÃ
    __loadNetwork();

    l_sceneWidget = UI_CreateBaseWidgetByFileName("IOSLogin.json");
    l_panelMainMenu = UI_GetUILayout(l_sceneWidget, 10000);
	l_panelACMainMenu = UI_GetUILayout(l_sceneWidget, 20000);
	l_panelACRegMenu = UI_GetUILayout(l_sceneWidget, 30000);
	l_panelACLoginMenu = UI_GetUILayout(l_sceneWidget, 40000);
	l_panelACSMSMenu = UI_GetUILayout(l_sceneWidget, 50000);
    l_panelACSMSVerifyMenu = UI_GetUILayout(l_sceneWidget, 60000);

    -- ¼ÓÔØ¶¯»­
    
	local armature = GetUIArmature("Loding")
	armature:getAnimation():playWithIndex(0)
	l_sceneWidget:getChildByName("BG"):addNode(armature, 1);
	armature:setPosition(ccp(0, 0) );

    -- ³õÊ¼»¯2144½Ó¿Ú
    -- __init2144();

    -- °ó¶¨¸÷ÊÂ¼þ
    UI_GetUILabel(l_panelMainMenu, 10006):setTouchEnabled(true);
    UI_GetUILabel(l_panelMainMenu, 10006):addTouchEventListener(__onAccountCenterBtnClick);
	UI_GetUIButton(l_panelMainMenu, 10003):addTouchEventListener(__onEntergameBtnClick);
    UI_GetUILabel(l_panelMainMenu, 10004):setTouchEnabled(true);
    UI_GetUILabel(l_panelMainMenu, 10004):addTouchEventListener(__onServerBtnClick);
	
	UI_GetUIButton(l_panelACMainMenu, 20002):addTouchEventListener(__onACMainMenuLoginBtnClick);
	UI_GetUIButton(l_panelACMainMenu, 20003):addTouchEventListener(__onACMainMenuRegBtnClick);
	UI_GetUILabel(l_panelACMainMenu, 20004):setTouchEnabled(true);
	UI_GetUILabel(l_panelACMainMenu, 20004):addTouchEventListener(__onACMainMenuBackLabelClick);
	UI_GetUIButton(l_panelACMainMenu, 20001):addTouchEventListener(__onACMainMenuFastGameBtnClick);
	local panelSelect = UI_GetUILayout(l_panelACMainMenu, 20005);
	local btnOpenList = UI_GetUIButton(panelSelect, 200053);
	btnOpenList:addTouchEventListener(__onACRegMenuOpenListClick);
	
	UI_GetUIButton(l_panelACRegMenu, 30003):addTouchEventListener(__onACRegMenuMobileBtnClick);
	UI_GetUIButton(l_panelACRegMenu, 30004):addTouchEventListener(__onACRegMenuRegBtnClick);
	UI_GetUILabel(l_panelACRegMenu, 30005):setTouchEnabled(true);
	UI_GetUILabel(l_panelACRegMenu, 30005):addTouchEventListener(__onACRegMenuBackBtnClick);
	
	UI_GetUIButton(l_panelACLoginMenu, 40001):addTouchEventListener(__onACLoginMenuMobileBtnClick);
	UI_GetUIButton(l_panelACLoginMenu, 40002):addTouchEventListener(__onACLoginMenuLoginBtnClick);
	UI_GetUILabel(l_panelACLoginMenu, 40005):setTouchEnabled(true);
	UI_GetUILabel(l_panelACLoginMenu, 40005):addTouchEventListener(__onACLoginMenuBackBtnClick);
	
	UI_GetUIButton(l_panelACSMSMenu, 50002):addTouchEventListener(__onACSMSMenuSendSMSBtnClick);
	UI_GetUILabel(l_panelACSMSMenu, 50003):setTouchEnabled(true);
	UI_GetUILabel(l_panelACSMSMenu, 50003):addTouchEventListener(__onACSMSMenuBackBtnClick);

    UI_GetUIButton(l_panelACSMSVerifyMenu, 60002):addTouchEventListener(__onACSMSVerifyMenuVerifyBtnClick);
    UI_GetUILabel(l_panelACSMSVerifyMenu, 60004):setTouchEnabled(true);
    UI_GetUILabel(l_panelACSMSVerifyMenu, 60004):addTouchEventListener(__onACSMSVerifyMenuBackBtnClick);
    UI_GetUIButton(l_panelACSMSVerifyMenu, 60003):addTouchEventListener(__onASSMSVerifyMenuResendBtnClick);
    l_resendName = UI_GetUIButton(l_panelACSMSVerifyMenu, 60003):getTitleText();

    -- Ë¢ÐÂÏÔÊ¾
    IOSLogin_RefreshView();

    -- Èç¹ûµ±Ç°ÎÞÕÊºÅ, Ôò´´½¨ÓÎ¿ÍÕÊºÅ
    --if g_RecordUserData.UserName == nil or  g_RecordUserData.UserName== "" then
        --IOSSDK:acc2144GuestRegRequest();
    --end
    -- __init2144();
    -- Á¬½Ó·þÎñÆ÷
    -- ShowWaiting();
    __connectServer();
end

-- ·þÎñÆ÷Á¬½Ó³É¹¦»Øµ÷
-- Èç¹ûÓÐÄ¬ÈÏÕÊºÅ, ÔòÖ±½Ó½øÐÐauth
-- Èç¹ûÃ»ÓÐÄ¬ÈÏÕÊºÅ ÔòÇëÇó2144´´½¨Ä¬ÈÏÕÊºÅ
function IOSLogin_ConnectSuc()
    Log(">>>>>>>> IOSLogin_ConnectSuc invoked.");
	local record = __getLatestRecordUserData();
	if record == nil then
       IOSSDK:acc2144GuestRegRequest();
    else
       __sendAuth();
    end
end


-- ÊÚÈ¨Íê³É
-- ÇëÇóµØÍ¼Êý¾Ý
function IOSLogin_OnAuthRsp(_pkg)
    Log(">>>>>>>> IOSLogin_OnAuthRsp");
    local tmp = RouterServer_pb.SC_RS_Auth();
	tmp:ParseFromString(_pkg);
	local strAccount = tmp.strAccount
	local retCode = tmp.iRetCode
	if (retCode == RouterServer_pb.RS_AUTU_OK) then
		-- ½öÔÚÊÀ½çÎ´ÏÂÔØµÄÊ±ºòÇëÇó
		l_authedUserID = strAccount;
		if (g_worldListData == nil or #g_worldListData == 0) then
			Packet_Cmd(RouterServer_pb.RS_ROUTER_WORLDLIST);
		else
			EndWaiting();
		end
    else
        Log(">>>>>>> IOSLogin_OnAuthRsp() auth failed.");
		return;
	end
end


-- ÊÀ½çÐÅÏ¢ÏìÓ¦
-- »ñÈ¡ÊÀ½çÊý¾Ý
-- ¸üÐÂÏÔÊ¾
function IOSLogin_OnWorldListRsp(_pkg)
    Log(">>>>>>>> IOSLogin_OnWorldListRsp");
    local tmp = RouterServer_pb.SC_RS_WorldList();
	tmp:ParseFromString(_pkg)
	Log("IOSLogin_OnWorldListRsp"..tostring(tmp))
    g_worldListData = {};
	for i=1,#tmp.worldList do
		g_worldListData[#g_worldListData+1] = tmp.worldList[i]
	end

    -- ¸üÐÂµ±Ç°Ñ¡ÖÐµÄÊÀ½ç
	local record = __getLatestRecordUserData();
	if record.serverID == nil or record.serverID == "" then
		record.serverID = g_worldListData[#g_worldListData].iWorldID;
		record.serverName = g_worldListData[#g_worldListData].strWorldName;
	end
    --if( g_RecordUserData.ServerName == nil or g_RecordUserData.ServerName == "") then
        --g_RecordUserData.ServerName = g_worldListData[#g_worldListData].strWorldName;
        --g_RecordUserData.ServerId = g_worldListData[#g_worldListData].iWorldID;
    --end
    IOSLogin_RefreshView();
    EndWaiting();
end


-- µÇÂ¼ÇëÇó·µ»Ø
function IOSLogin_OnLoginRsp(_pkg)
    Log(">>>>>>>>>> IOSLogin_OnLoginRsp");
	
	--__saveRecordUserData();
	-- ½«Êý¾ÝÐ´µ½g_RecordUserDataÖÐ
	if	g_RecordUserData == nil then
		g_RecordUserData = {};
	end
	local latestRecord = __getLatestRecordUserData();
	g_RecordUserData.ServerId = latestRecord.serverID;
	g_RecordUserData.UserName = latestRecord.userName;
	
    local tmp = GameServer_pb.Cmd_Sc_Alogin();
	tmp:ParseFromString(_pkg)
	if (tmp.iResult == GameServer_pb.en_LoginResult_OK)then	
        log(">>>>GameServer_pb.en_LoginResult_OK");
		Packet_Cmd(GameServer_pb.CMD_GETROLE);
	elseif (tmp.iResult == GameServer_pb.en_LoginResult_SuggestUpdata)then
	   log(">>>>GameServer_pb.en_LoginResult_SuggestUpdata");
	elseif (tmp.iResult == GameServer_pb.en_LoginResult_MustUpdata)then
	   log(">>>>GameServer_pb.en_LoginResult_MustUpdata");
	elseif (tmp.iResult == GameServer_pb.en_LoginResult_RoleOnLine) then
        log(">>>>GameServer_pb.en_LoginResult_RoleOnLine");
		Packet_Cmd(GameServer_pb.CMD_GETROLE);
	end
	
	GetGlobalEntity():GetScheduler():RegisterInfiniteScript( "IOSLogin_PingUpdate", 30*1000 )
end


function IOSLogin_BackToMaiMenu()
    Log(">>>>>>>>>>>>>> IOSLogin_BackToMaiMenu() invoked.");
    g_dungeonList = {};
	g_openUIType = EUIOpenType_None
	Guide_End()
	UI_CloseAllBaseWidget();
	GetAudioMgr():stopBackgroundMusic()
	
	-- Çå³ýÒýµ¼
	g_curGuideId = 0;
	g_curGuideStep = 0;
	g_isGuide = false;
	g_nextGuide = {}
	g_clipGuide = nil;
	
	IOSLogin_Create();
end

function IOSLogin_PingUpdate()
	Log("ping--------------------------------")
	Log("time------------------"..os.date())
	Packet_Cmd(GameServer_pb.CMD_PING);
end


function IOSLogin_OnSocketClose()
    Log(">>>>>>>>>> IOSLogin_OnSocketClose() invoked.");
    GetGlobalEntity():GetScheduler():UnRegisterScript("IOSLogin_PingUpdate")
	
	if (MainScene:GetInstance():GetState() == EState_Gaming)then
		SceneMgr:GetInstance():SwitchToMainMenu();
	else
		IOSLogin_BackToMaiMenu();
	end
end

-- ÏìÓ¦ 2144 ÓÎ¿ÍÕÊºÅ´´
-- ½«µÇÂ¼¼ÇÂ¼Ð´ÈëÎÄ¼þÊ¹ÓÃ Login_SaveRecordUserData
-- ´ÓÎÄ¼þ¶ÁÈ¡µÇÂ¼¼ÇÂ¼Ê¹ÓÃ Login_LoadRecordUserData
-- g_RecordUserDataµÄ×Ö¶Î
-- g_RecordUserData.UserName
-- g_RecordUserData.ServerId
-- g_RecordUserData.ServerName
-- g_RecordUserData.AccessToken
-- g_RecordUserData.PayToken
-- g_RecordUserData.Pf
-- g_RecordUserData.PfKey

-- 1) ½«Êý¾ÝÐ´Èëg_userData
-- 2) Ë¢ÐÂ½çÃæ
-- 3) ÇëÇóÊÚÈ¨
local function __on2144GuestRegCallback(_uid, _username, _usertoken)
    Log(">>>>>>>>>>>>>>>>>>> __on2144GuestRegCallback() invoked.");
    -- g_RecordUserData.UserName = _uid;
    -- g_RecordUserData.AccessToken = _usertoken;
	local record = {};
	record.userID = _uid;
	record.userName = _username;
	record.token = _usertoken;
	record.isGuest = 1;
	__addRecordUserData(record);
	__setLatestRecordUserData(_username);
	__saveRecordUserData();
    IOSLogin_RefreshView();
    __sendAuth();    
end



-- ÄÚ²¿·½·¨ 2144 ÓÎ¿ÍµÇÂ¼³É¹¦
-- ½øÐÐºóÐø½øÈëÓÎÏ·Á÷³Ì(ÎÒÃÇµÄserver)
local function __on2144GuestLoginCallback(_uid, _username, _usertoken)
	local record = __getLatestRecordUserData();
    __saveRecordUserData();
	__login(record.serverID);
end

-- ÏìÓ¦2144ÕÊºÅµÇÂ¼³É¹¦
-- ½øÐÐºóÐø½øÈëÓÎÏ·Á÷³Ì(ÎÒÃÇµÄserver)
local function __on2144LoginCallback(_uid, _username, _usertoken)
	Log(">>>>>>>>>>>>>>>>>>> __on2144LoginCallback() invoked.");
	local latestRecord = __getLatestRecordUserData();
	local record = {};
	record.serverID = latestRecord.serverID;
	record.serverName = latestRecord.serverName;
	record.userID = _uid;
	record.userName = _username;
	record.token = _usertoken;
	record.isGuest = 0;
	__addRecordUserData(record);
	__setLatestRecordUserData(_username);
	__saveRecordUserData();
	IOSLogin_RefreshView();
	__connectServer();
	__showMainMenu();
end

local function __on2144QuickLoginCallback(_uid, _username, _usertoken)
    -- local record = __getLatestRecordUserData();
    Log("on login.....");
    Log("login:".."test");
    -- __saveRecordUserData();
    __login(0);
 --    local record = __getLatestRecordUserData();
	-- Log("login:"..record.userName);
 --    __saveRecordUserData();
	-- __login(record.serverID);
end


function IOSLogin_SMSResendTimer()
    Log(l_smsWaitSec);
    if  l_smsWaiting == true then
        l_smsWaitSec = l_smsWaitSec - 1;
        local resendBtn = UI_GetUIButton(l_panelACSMSVerifyMenu, 60003);
        if  l_smsWaitSec >= 0 then
            resendBtn:setTitleText(l_smsWaitSec.."S");
            resendBtn:setTouchEnabled(false);
        else
            l_smsWaiting = false;
            l_smsWaitSec = 0;
            resendBtn:setTouchEnabled(true);
            resendBtn:setTitleText(l_resendName);
            GetGlobalEntity():GetScheduler():UnRegisterScript("IOSLogin_SMSResendTimer");
        end
    end
end

-- Ìø×ªµ½Ð£ÑéÂëÊäÈë½çÃæ
local function __on2144SmsCallback(_mobile, _smsToken)
    Log(">>>>>>>>>>>>>>>>>>> __on2144SmsCallback() invoked.");
    if _mobile == l_mobile then
        EndWaiting();
        l_smsToken = _smsToken;
        __showACSMSVerifyMenu();

        -- Æô¶¯smsµ¹¼ÆÊ±
        l_smsWaiting = true;
        l_smsWaitSec = 60;
        local resendBtn = UI_GetUIButton(l_panelACSMSVerifyMenu, 60003);
        resendBtn:setTouchEnabled(false);
        GetGlobalEntity():GetScheduler():RegisterInfiniteScript( "IOSLogin_SMSResendTimer", 1000);

        -- set send vc to mobile label
        local labelText = FormatString("SendVcToMobile", l_mobile);
        local labelSendVCToMobile = UI_GetUILabel(l_panelACSMSVerifyMenu, "60005");
        labelSendVCToMobile:setText(labelText);
    end
end


-- ÊÖ»úÐ£Ñé
local function __on2144SmsVerifyCallback(_uid, _username, _usertoken)
    Log(">>>>>>>>>>>>>>>>>>> __on2144SmsVerifyCallback() invoked.");
    local latestRecord = __getLatestRecordUserData();
	local record = {};
	record.serverID = latestRecord.serverID;
	record.serverName = latestRecord.serverName;
	record.userID = _uid;
	record.userName = _username;
	record.token = _usertoken;
	record.isGuest = 0;
	__addRecordUserData(record);
	__setLatestRecordUserData(_username);
	__saveRecordUserData();
    IOSLogin_RefreshView();
	__connectServer();
	__showMainMenu();
end



-- ÏìÓ¦2144ÕÊºÅ×¢²á³É¹¦
-- ¼ÇÂ¼Êý¾Ý
-- Ë¢ÐÂ½çÃæ
-- ÖØÐÂÁ¬½Ó·þÎñÆ÷
-- ½øÈëÖ÷²Ëµ¥
local function __on2144RegCallback(_uid, _username, _usertoken)
	Log(">>>>>>>>>>>>>>>>>>> __on2144GuestRegCallback() invoked.");
	local latestRecord = __getLatestRecordUserData();
	
	local record = {};
	record.serverID = latestRecord.serverID;
	record.serverName = latestRecord.serverName;
	record.userID = _uid;
	record.userName = _username;
	record.token = _usertoken;
	record.isGuest = 0;
	__addRecordUserData(record);
	__setLatestRecordUserData(_username);
	__saveRecordUserData();
    IOSLogin_RefreshView();
	__connectServer();
	__showMainMenu();
end



-- xmlLoginRecord:SetAttribute( "UserId", "123457");
-- xmlLoginRecord:SetAttribute( "UserName", "KKWout6");
-- xmlLoginRecord:SetAttribute( "Token", "BgdLS1dvdXQwHCw=");

-- http ·µ»ØÏìÓ¦
-- ²ÎÊý _vars : ÀàÐÍÎªvector<Var>
-- vars[0] Îªtag
-- vars[1] ÎªnetSuccess (ÍøÂçÊÇ·ñ³ö´í)
-- vars[2] ÎªhttpStatusCode
-- vars[3] ÎªresponseData(Êµ¼ÊÎªjson ×Ö·û´®)
function IOSLogin_on2144HttpResponse( _vars)
    -- local jData = json["data"];
    local uid = "123457";
    local username = "KKWout6";
    local usertoken = "BgdLS1dvdXQwHCw=";
    __on2144GuestRegCallback(uid, username, usertoken);
    return;
-- 
 --    -- »ñÈ¡Êý¾Ý
 --    local tag = tostring(_vars[0]:ToString());
 --    local netSuccess =_vars[1]:Int();
 --    local httpStatusCode = _vars[2]:Int();
 --    local responseData = tostring(_vars[3]:ToString());

 --    Log(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>> IOSGameCenter_onHttpResponse");
 --    Log("tag:"..tag);
 --    Log("netSuccess:"..netSuccess);
 --    Log("httpStatusCode:"..httpStatusCode);
 --    Log("responseData:"..responseData);
 --    Log(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>> ");

 --    -- ´íÎó´¦Àí
 --    if netSuccess ~= 1 then 
 --        -- FIXME:Ã»ÓÐÊ¹ÓÃÅäÖÃ×Ö·û´®
	-- 	EndWaiting();
 --        createPromptBoxlayout("net error.");
 --        return;
 --    end

 --    if httpStatusCode ~= 200 then
 --        -- FIXME:Ã»ÓÐÊ¹ÓÃÅäÖÃ×Ö·û´®
	-- 	EndWaiting();
 --        createPromptBoxlayout("invalid http status code:"..httpStatusCode);
 --        return;
 --    end

 --    -- ½âÎöjson
 --    -- local json2lua = require "json";
 --    local json = g_json2lua.decode(responseData);
 --    local status = json["status"];
 --    if status ~= "0" then
 --        -- FIXME : Ö±½ÓÊ¹ÓÃÁË2144µÄ´íÎóÐÅÏ¢.
	-- 	EndWaiting();
 --        local errorMsg = json["msg"];
 --        Log(">>>>>>>>>>>>>>>> errorMsg:"..errorMsg);
 --        createPromptBoxlayout(errorMsg);
 --        return;
 --    end

 --    -- ÅÉ·¢ÊÂ¼þ
 --    if tag == "REG" then
 --        local jData = json["data"];
 --        local uid = jData["uid"];
 --        local username = jData["username"];
 --        local usertoken = jData["usertoken"];
 --        __on2144RegCallback(uid, username, usertoken);
 --        return;
 --    elseif tag == "GUEST_REG" then
 --        local jData = json["data"];
 --        local uid = jData["guid"];
 --        local username = jData["username"];
 --        local usertoken = jData["token"];
 --        __on2144GuestRegCallback(uid, username, usertoken);
 --        return;
 --    elseif tag == "GUEST_LOGIN" then
 --        local jData = json["data"];
 --        local uid = jData["guid"];
 --        local username = jData["username"];
 --        local usertoken = jData["token"];
 --        __on2144GuestLoginCallback(uid, username, usertoken);
 --        return;
	-- elseif tag == "LOGIN" then
	-- 	local jData = json["data"];
 --        local uid = jData["uid"];
 --        local username = jData["username"];
 --        local usertoken = jData["usertoken"];
 --        __on2144LoginCallback(uid, username, usertoken);
	-- 	return;
 --    elseif tag == "QUICK_LOGIN" then
 --        local jData = json["data"];
 --        local uid = jData["uid"];
 --        local username = jData["username"];
 --        local usertoken = jData["usertoken"];
 --        __on2144QuickLoginCallback(uid, username, usertoken);
 --        return;
 --    elseif tag == "SMS" then
 --        local jData = json["data"];
 --        local mobile = jData["mobile"];
 --        local smsToken = jData["token"];
 --        __on2144SmsCallback(mobile, smsToken);
 --        return;
 --    elseif tag == "SMS_VERIFY" then
 --        local jData = json["data"];
 --        local uid = jData["uid"];
 --        local username = jData["username"];
 --        local usertoken = jData["usertoken"];
 --        __on2144SmsVerifyCallback(uid, username, usertoken);
 --        return;
 --    end
    
end