if (g_RecordUserData == nil) then
	g_RecordUserData = {};
end



-- 注册网络事件
local function __registerScriptFunc()
    ScriptSys:GetInstance():RegisterScriptFunc( RouterServer_pb.RS_ROUTER_AUTH, "Login_Auth" );
    ScriptSys:GetInstance():RegisterScriptFunc( RouterServer_pb.RS_ROUTER_WORLDLIST, "Login_WorldList" );
    ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_ALOGIN, "Login_AloginResult" );
    ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_KICKOFF_ACCOUNT, "Kickoff" );
    --ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_ROLE_FIN, "Login_RoleFin" );
    ScriptSys:GetInstance():RegisterScriptFunc( RouterServer_pb.RS_ROUTER_ERROR, "LoginError" );
end

-- 删除网络事件
local function __unregisterScriptFunc()
    ScriptSys:GetInstance():UnRegisterScriptFunc( RouterServer_pb.RS_ROUTER_AUTH);
    ScriptSys:GetInstance():UnRegisterScriptFunc( RouterServer_pb.RS_ROUTER_WORLDLIST);
    ScriptSys:GetInstance():UnRegisterScriptFunc( GameServer_pb.CMD_ALOGIN);
    ScriptSys:GetInstance():UnRegisterScriptFunc( GameServer_pb.CMD_KICKOFF_ACCOUNT);
    --ScriptSys:GetInstance():UnRegisterScriptFunc( GameServer_pb.CMD_ROLE_FIN);
    ScriptSys:GetInstance():UnRegisterScriptFunc( RouterServer_pb.RS_ROUTER_ERROR);
end



local function setAccountCtrlVisible(widget, visible)
	local tagTb = {4, 4600586, 2, }
	local layout = UI_GetUILayout(widget, 12345)
	for i = 1, #tagTb do
		local child = tolua.cast( layout:getChildByTag(tagTb[i]), "Widget" )
		child:setEnabled(visible);
	end
end

local function setServerListCtrlVisible(widget, visible)
	local tagTb = {5, 15, 3,}
    local layout = UI_GetUILayout(widget, 12345)
	if getChannelID() == "QQYYB" then 
	   local imagevisible = false
	   if visible then 
          imagevisible = false
	   else
	      imagevisible = true
	   end 
	   UI_GetUIImageView(layout,2144):setVisible(imagevisible)
	else 
	   UI_GetUIImageView(layout,2144):setVisible(false)
	end 
	for i = 1, #tagTb do
		local child = tolua.cast( layout:getChildByTag(tagTb[i]), "Widget" )
		child:setEnabled(visible);
	end
end


local function setPlayBtonVisible(widget, visible)
	local layout = UI_GetUILayout(widget, 12345)
	local child = tolua.cast( layout:getChildByTag(1), "Widget" )
	child:setEnabled(visible)
end


local function setWxLoginBtonVisible(widget, visible)
	local layout = UI_GetUILayout(widget, 12345)
	local child = tolua.cast( layout:getChildByTag(1124779290), "Widget" )
	child:setEnabled(visible)
end

local function setLoginBtonVisible(widget, visible)
	local layout = UI_GetUILayout(widget, 12345)
	local child = tolua.cast( layout:getChildByTag(4777036), "Widget" )
	child:setEnabled(visible)
	
	setWxLoginBtonVisible(widget, visible)
end



local function setQuitBtonVisible(widget, visible)
	local layout = UI_GetUILayout(widget, 12345)
	local child = tolua.cast( layout:getChildByTag(4777037), "Widget" )
	child:setEnabled(visible)
end


function Login_Create()
    -- 注册网络事件
    __registerScriptFunc();

    --当前选择的服务器ID，当玩家从服务器选择界面出来时使用，否则使用默认的服务器ID
    g_ChooseServerID = nil
	local widget = UI_CreateBaseWidgetByFileName("MainMenu.json");
	
	local armature = GetUIArmature("Loding")
	armature:getAnimation():playWithIndex(0)
	widget:getChildByName("BG"):addNode(armature, 1);
	armature:setPosition(ccp(0, 0) );
	--local action = ActionManager:shareManager():playActionByName("MainMenu.json","Animation0"); 
    --if(action)then 
     --   action:play(); 
	--else 
     --   Log("Action null..."); 
    --end
	
	g_login = false;
	Socket_Connect();
	MainMenu_Refresh(widget)
end

function MainMenu_Refresh(widget)
    if getChannelID() == "TBT" then
        setAccountCtrlVisible(widget, false)
        setServerListCtrlVisible(widget, true)
        setPlayBtonVisible(widget, true)
        setLoginBtonVisible(widget, false);
        setQuitBtonVisible(widget, true)
        if TBTSDK:GetInstance():TBIsLogined() then
        else
           TBTSDK:GetInstance():IOSTBLogin(0)
        end
    elseif getChannelID() == "UC" then 
	    setAccountCtrlVisible(widget, false)
        setServerListCtrlVisible(widget, true)
        setPlayBtonVisible(widget, true)
        setLoginBtonVisible(widget, false);
        setQuitBtonVisible(widget, false)
		if TBTSDK:GetInstance().sdkInitisSuccess == true and TBTSDK:GetInstance().s_channelSdkInfo.UserId == "" then 
		   TBTSDK:GetInstance():callsdkLogin()
		else 
		end 
    elseif getChannelID() == "LJ" then 
        setAccountCtrlVisible(widget, false)
        setServerListCtrlVisible(widget, true)
        setPlayBtonVisible(widget, true)
        setLoginBtonVisible(widget, false);
        setQuitBtonVisible(widget, false)
        if TBTSDK:GetInstance():LjSDKisHaveLogin() == false then 
           TBTSDK:GetInstance():LjSDKLogin()
        else 
        end 
    elseif getChannelID() == "JSFUCK" then 
        setAccountCtrlVisible(widget, false)
        setServerListCtrlVisible(widget, true)
        setPlayBtonVisible(widget, true)
        setLoginBtonVisible(widget, false);
        setQuitBtonVisible(widget, false)
        if TBTSDK:GetInstance().sdkInitisSuccess == true and TBTSDK:GetInstance().s_channelSdkInfo.UserId == "" then 
		   TBTSDK:GetInstance():callsdkLogin()
		else 
		end 
    elseif getChannelID() == "VIVO" then
        setAccountCtrlVisible(widget, false)
        setServerListCtrlVisible(widget, true)
        setPlayBtonVisible(widget, true)
        setLoginBtonVisible(widget, false);
        setQuitBtonVisible(widget, true)
Log("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA")
         
        TBTSDK:GetInstance():VIVOSDKLogin()
    else
	    if (g_RecordUserData.UserName == nil or g_RecordUserData.UserName == "")then
		    if getChannelID() == "PC" then
			    Login_RegisterCreate();
			    setAccountCtrlVisible(widget, true)
			    setServerListCtrlVisible(widget, true)
			    setPlayBtonVisible(widget, true)
			    setLoginBtonVisible(widget, false);
			    setQuitBtonVisible(widget, false)
		    elseif getChannelID() == "UC" then 		
		        setAccountCtrlVisible(widget, false)
                setServerListCtrlVisible(widget, true)
                setPlayBtonVisible(widget, true)
                setLoginBtonVisible(widget, false);
                setQuitBtonVisible(widget, false)
		    elseif getChannelID() == "QQYYB" then
			    setAccountCtrlVisible(widget, false)
			    setServerListCtrlVisible(widget, false)
			    setPlayBtonVisible(widget, false)
			    setLoginBtonVisible(widget, true);
			    setQuitBtonVisible(widget, false)
            elseif getChannelID() == "LJ" then 
                setAccountCtrlVisible(widget, false)
                setServerListCtrlVisible(widget, true)
                setPlayBtonVisible(widget, true)
                setLoginBtonVisible(widget, false);
                setQuitBtonVisible(widget, false)
            elseif getChannelID() == "JSFUCK" then 
                setAccountCtrlVisible(widget, false)
                setServerListCtrlVisible(widget, true)
                setPlayBtonVisible(widget, true)
                setLoginBtonVisible(widget, false);
                setQuitBtonVisible(widget, false)
		    elseif getChannelID() == "VIVO" then
                setAccountCtrlVisible(widget, false)
                setServerListCtrlVisible(widget, true)
                setPlayBtonVisible(widget, true)
                setLoginBtonVisible(widget, false);
                setQuitBtonVisible(widget, false)
            end
	    else
		    if getChannelID() == "PC" then
			    setAccountCtrlVisible(widget, true)
			    setServerListCtrlVisible(widget, true)
			    setPlayBtonVisible(widget, true)
			    setLoginBtonVisible(widget, false);
			    setQuitBtonVisible(widget, false)
            else
			    setAccountCtrlVisible(widget, false)
			    setServerListCtrlVisible(widget, true)
			    setPlayBtonVisible(widget, true)
			    setLoginBtonVisible(widget, false);
			    setQuitBtonVisible(widget, false)
		    end
	    end
    end

	Log("----------MainMenu_Refresh");
	local function Swicth_Account(sender,eventType) 
		Log("Swicth_Account")
		if (eventType == TOUCH_EVENT_ENDED) then
			Login_AccountCreate();
		end
	end 
    local layout = UI_GetUILayout(widget, 12345)
	local accountButton = UI_GetUIImageView(layout, 2);
	accountButton:addTouchEventListener(Swicth_Account);
	
	if  g_RecordUserData~=nil and g_RecordUserData.UserName ~= nil then
		UI_GetUILabel(layout, 4):setText(g_RecordUserData.UserName);
	end
	--local infoCompLabel = CompLabel:GetCompLabelWithSpaces( FormatString("AccountEmpty"), 200, kCCTextAlignmentLeft,1,5);
	--accountButton:addNode(infoCompLabel);
	local function Swicth_Server(sender,eventType) 
		if (eventType == TOUCH_EVENT_ENDED and g_RecordUserData.UserName ~= nil and g_RecordUserData.UserName ~= "") then
			Login_ServerListCreate();
            layout:setVisible(false)
		end
	end 
	local serverButton = UI_GetUIImageView(layout, 3);
	serverButton:addTouchEventListener(Swicth_Server);
	
	if g_RecordUserData.ServerId ~= nil and g_RecordUserData.ServerName ~= nil then
		UI_GetUILabel(layout, 15):setText(FormatString("Login_ServerID",g_RecordUserData.ServerId+1).." "..g_RecordUserData.ServerName);
	end
	
	local function Enter_Game(sender,eventType) 
		if (eventType == TOUCH_EVENT_ENDED) then
		    if getChannelID() == "TBT" then
               if TBTSDK:GetInstance():TBIsLogined() then
                  if g_ChooseServerID == nil then
                  else
                    g_RecordUserData.ServerId = g_worldListData[g_ChooseServerID].iWorldID
                    g_RecordUserData.ServerName = g_worldListData[g_ChooseServerID].strWorldName
                  end
                  Login_ALogin(g_RecordUserData.ServerId);
               else 
                  TBTSDK:GetInstance():IOSTBLogin(0)
               end
			elseif getChannelID() == "UC" then 
			    --callstaticSDKHandler()
				if TBTSDK:GetInstance().s_channelSdkInfo.UserId == "" then 
		           TBTSDK:GetInstance():callsdkLogin()
		        else 
				   if g_ChooseServerID == nil then
                    else
                       g_RecordUserData.ServerId = g_worldListData[g_ChooseServerID].iWorldID
                       g_RecordUserData.ServerName = g_worldListData[g_ChooseServerID].strWorldName
                    end
                     Login_ALogin(g_RecordUserData.ServerId);
				end 
            elseif getChannelID() == "LJ" then 
                if TBTSDK:GetInstance():LjSDKisHaveLogin() == false then 
                   TBTSDK:GetInstance():LjSDKLogin()
                else
                   if g_ChooseServerID == nil then 
                   else
                     g_RecordUserData.ServerId = g_worldListData[g_ChooseServerID].iWorldID
                     g_RecordUserData.ServerName = g_worldListData[g_ChooseServerID].strWorldName  
                   end   
                   Login_ALogin(g_RecordUserData.ServerId);
                end 
            elseif getChannelID() == "JSFUCK" then 
                if TBTSDK:GetInstance().s_channelSdkInfo.UserId == "" then 
		        else 
				   if g_ChooseServerID == nil then
                    else
                       g_RecordUserData.ServerId = g_worldListData[g_ChooseServerID].iWorldID
                       g_RecordUserData.ServerName = g_worldListData[g_ChooseServerID].strWorldName
                    end
                     Login_ALogin(g_RecordUserData.ServerId);
				end
            elseif getChannelID() == "VIVO" then 
            
                if TBTSDK:GetInstance().s_VIVOSDKInfo.uid == "" then 
		               TBTSDK:GetInstance():VIVOSDKLogin()
		        else 
				   if g_ChooseServerID == nil then
                    else
                       g_RecordUserData.ServerId = g_worldListData[g_ChooseServerID].iWorldID
                       g_RecordUserData.ServerName = g_worldListData[g_ChooseServerID].strWorldName
                    end
                     Login_ALogin(g_RecordUserData.ServerId);
				end
            else
			if (g_RecordUserData.UserName ~= nil and g_RecordUserData.UserName ~= "" and (g_RecordUserData.ServerId ~= nil or g_ChooseServerID ~= nil)) then
				Log("g_RecordUserData.ServerId"..g_RecordUserData.ServerId)
                if g_ChooseServerID == nil then
                else
                    --玩家进行了服务器选择，保存玩家的选择
                    g_RecordUserData.ServerId = g_worldListData[g_ChooseServerID].iWorldID
                    g_RecordUserData.ServerName = g_worldListData[g_ChooseServerID].strWorldName
                end
			    Login_ALogin(g_RecordUserData.ServerId);
			end
            end
		end
	end 
	-- 应用宝退出帐号
	local function quitAccount(sender,eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
           if getChannelID() == "QQYYB" then
			-- 调用平台API退出帐号
			callLogOut();
			clearLocalLoginInfo(); -- 清除本地缓存
			
			g_RecordUserData.UserName = ""
			g_login = false;
			
			-- 恢复到没有登录帐号的情况
			if getChannelID() == "QQYYB" then
				setAccountCtrlVisible(widget, false)
				setServerListCtrlVisible(widget, true)
				setPlayBtonVisible(widget, false)
				setLoginBtonVisible(widget, true);
				setQuitBtonVisible(widget, false)
			end
			MainMenu_Refresh(widget);
			Socket_Connect();
            elseif getChannelID() == "TBT" then
                g_RecordUserData.UserName = ""
                g_login = false;
                TBTSDK:GetInstance():TBSwitchAccount()
            elseif getChannelID() == "VIVO" then
                TBTSDK:GetInstance():VIVOSDKChangeAccount()
            end
		end
	end
	
	local function loginAccount(sender,eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			-- 渠道
			Log("******Click EnterGame");
			local channel = getChannelID();
			Log("**************channel:"..channel);
			
			if g_login== false and channel == "QQYYB" then
				callQQYYBLogin();
				
				Log("---call QQLogin");
				
				-- 显示菊花
				ShowWaiting();
				
				--[[strChannel = "QQYYB"
				strOpenID = "DDDD"
				g_login = true;
				if true then
					g_RecordUserData.UserName = strChannel.."_"..strOpenID;
					Log("Use Account:"..g_RecordUserData.UserName);
					sendAuthPkg();
				end--]]
				
				return;
			end
		end
	end
	
	
	local function wxLoginAccount(sender,eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			-- 渠道
			Log("******Click EnterGame");
			local channel = getChannelID();
			Log("**************channel:"..channel);
			
			if g_login== false and channel == "QQYYB" then
				
				callWeiXinLogin();
				Log("---call WxLogin");
				
				-- 显示菊花
				ShowWaiting();
				
				
				return;
			end
		end
	end
	
	
	Log("----------RegisterLoginEvent");
	local enterButton = UI_GetUIButton(layout, 1);
	enterButton:addTouchEventListener(Enter_Game);
	
	local quitLabel = UI_GetUILabel(layout, 4777037)
	quitLabel:setTouchEnabled(true)
	quitLabel:addTouchEventListener(quitAccount);
	
	local loginButton = UI_GetUIButton(layout, 4777036);
	loginButton:addTouchEventListener(loginAccount);
	
	local wxLoginButton = UI_GetUIButton(layout, 1124779290);
	wxLoginButton:addTouchEventListener(wxLoginAccount);
	
	local version = "";
	local channelinfor = Common:GetChannelInfor();
	local jValue = ParseJson(channelinfor)
	if jValue~=nil then
		version = Cjson:GetStr(jValue,"Version")
	end
	local versionLabel = CompLabel:GetDefaultLabScript(version, 20);
	versionLabel:setPosition(ccp(50, 620));
	layout:addChild(versionLabel)
end

function TBTChannelLogin_CallBack(info)
   EndWaiting();
   local userId = "tbt_tbt_"..info.userID
   local sessenId = info.sessionID
   Log("userId==="..tostring(userId))
   Log("sessenId==="..tostring(sessendId))
   
   g_RecordUserData.UserName = userId
   if g_login == false then
        sendAuthPkg();
        g_login = true;
    end
end

function UCChannelLogin_CallBack(info)
   local userId = "UC_"..info.UserId
   Log("userId====="..tostring(userID))
   Log("userId222===="..userId)
   g_RecordUserData.UserName = userId
   if g_login == false then
        sendAuthPkg();
        g_login = true;
    end
end

function JSFuckCHannelLogin_CallBack(info)
   local userId = "2144_"..info.UserId
   g_RecordUserData.UserName = userId
   if g_login == false then
        sendAuthPkg();
        g_login = true;
   end
end 

function LjSDKChannelLogin_CallBack(info)
   g_RecordUserData.UserName = info.userId
   if g_login == false then 
      sendAuthPkg();
      g_login = true;
   end 
end

function VIVOLogin_CallBack(info)
   local userId = "vivo_"..info.uid
   g_RecordUserData.UserName = userId
   if g_login == false then
        sendAuthPkg();
        g_login = true;
   end
end

function channelLogin_Callback(loginInfo)
	Log("channelLogin_Callback---");
	EndWaiting();
	local bOk = loginInfo.bSuccess;
	local strChannel = loginInfo.strChannel;
	local strOpenID = loginInfo.strOpenID;
	
	Log("channelLogin_Callback---:"..tostring(bOk).."|"..strChannel);
	
	if bOk  then
		g_RecordUserData.UserName = strChannel.."_"..strOpenID;
		g_RecordUserData.AccessToken = loginInfo.strOpenKey;
		g_RecordUserData.PayToken = loginInfo.strPayToken;
		g_RecordUserData.Pf = loginInfo.strPf;
		g_RecordUserData.PfKey = loginInfo.strPfKey;
		Log("Use Account:"..g_RecordUserData.UserName);
			
		if g_login == false then
			sendAuthPkg();
			g_login = true;
		end
	end
end

function notifyNotIntallWx()
	Log("lua----notifyNotIntallWx");
	Messagebox_Create({info = FormatString("WxNotInstall"), msgType = EMessageType_Middle, deltaTime=3000000})
end

local g_bClicked = false;

function GameExit_ByLj()
   if g_bClicked then
	  return;
   end
	local function quitGame()
		g_bClicked = false;
		CCDirector:sharedDirector():endToLua();  
	end

	local function cancelQuit()
		g_bClicked = false;
	end

	g_bClicked = true;
	Messagebox_Create({info = FormatString("QuitGame"), msgType = EMessageType_LeftRight, rightFun=cancelQuit,leftFun = quitGame})
end
function onClickSysBack()
	Log("--------------onClickSysBack");
	if getChannelID() == "UC" then 
	    TBTSDK:GetInstance():callSdkLogout()
    elseif getChannelID() == "LJ" then 
        Log("ExitGame")
        TBTSDK:GetInstance():LjExitGame()
	else 
		if g_bClicked then
			return;
		end

		local function quitGame()
			g_bClicked = false;
			CCDirector:sharedDirector():endToLua();  
		end

		local function cancelQuit()
			g_bClicked = false;
		end

		g_bClicked = true;
		Messagebox_Create({info = FormatString("QuitGame"), msgType = EMessageType_LeftRight, rightFun=cancelQuit,leftFun = quitGame})
	end 
end

function onClickSysMenu()
	Log("--------------onClickSysMenu");

end

function requestRefreshGold()
	Log("--------requestRefreshGold-----");

	local refreshTab = GameServer_pb.CMD_REQUEST_REFRESHGOLD_CS();
	
	
	Log("--------requestRefreshGold-----1");
	refreshTab.strChannel = getChannelID();
	
	if g_RecordUserData.AccessToken ~= nil then
		refreshTab.strAccessKey = g_RecordUserData.AccessToken;
	end
	
	if g_RecordUserData.PayToken ~= nil then
		refreshTab.strPayToken = g_RecordUserData.PayToken;
	end
	
	if g_RecordUserData.Pf ~= nil then
		refreshTab.strPf = g_RecordUserData.Pf;
	end
	
	if g_RecordUserData.PfKey ~= nil then
		refreshTab.strPfKey = g_RecordUserData.PfKey;
	end
	
	Log("--------requestRefreshGold-----2");
	
	Packet_Full(GameServer_pb.CMD_REQUEST_REFRESHGOLD, refreshTab);
	
	Log("--------requestRefreshGold-----3");
    RechargeSystem_ShowLayout()
end

function sendAuthPkg()
	Log(">>>>>>> send Auth:"..g_RecordUserData.UserName);

	local authTab= RouterServer_pb.CS_RS_Auth()
	authTab.strAccount = g_RecordUserData.UserName
	authTab.strMd5Passwd = "11"
		
	Packet_Full(RouterServer_pb.RS_ROUTER_AUTH, authTab);
end

function Login_AccountCreate()
    Log(">>>>>>>>>>>>>>>>>>>>> Login_AccountCreate");
	local widget = UI_CreateBaseWidgetByFileName("Register.json", EUIOpenAction_Enlarge);
	if (widget == nil)then
		Log("Login_AccountCreate error");
		return;
	end

	local closeButton = UI_GetUIButton(widget, 1);
	closeButton:addTouchEventListener(UI_ClickCloseCurBaseWidget);
	
	local function Account_comfire(sender,eventType) 
		if (eventType == TOUCH_EVENT_ENDED) then
			local accountTextField = UI_GetUITextField(widget:getChildByTag(2), 1);
			local accountStr = accountTextField:getStringValue();
			if (accountStr ~= "") then
				g_RecordUserData.UserName = accountStr;
				Socket_Connect();
				UI_RefreshBaseWidgetByName("MainMenu")
				UI_CloseCurBaseWidget();
			else			
				Messagebox_Create({info = FormatString("AccountEmpty"), msgType = EMessageType_LeftRight})
			end
		end
	end 
	local confireButton = UI_GetUIButton(widget, 4);
	confireButton:addTouchEventListener(Account_comfire);
end

function Login_ServerListCreate()
	local widget = UI_CreateBaseWidgetByFileName("ServerList.json", EUIOpenAction_MoveIn_Top, 1.5); 
	UI_GetUILayout(widget, 4722820):setTouchEnabled(false)
    local function exit(sender,eventType)
        if sender == TOUCH_EVENT_ENDED then
            
        end
    end

    local function chooseServer(sender,eventType)
        
            print("Test1--------------------------------------------------")
        if eventType == TOUCH_EVENT_ENDED then
            print("Test2--------------------------------------------------")
            UI_ClickCloseCurBaseWidgetWithNoAction(sender, eventType)
            local MainMenuWidget = UI_GetBaseWidgetByName("MainMenu")
            local layout = UI_GetUILayout(MainMenuWidget, 12345)
            layout:setVisible(true)
            local tag = tolua.cast(sender,"Layout"):getTag()
            g_ChooseServerID = tag - 1520
            
            UI_GetUILabel(layout, 15):setText(FormatString("Login_ServerID",g_worldListData[g_ChooseServerID].iWorldID+1).." "..g_worldListData[g_ChooseServerID].strWorldName)

            
        end
    end

	if (g_RecordUserData.ServerName ~= nil and g_RecordUserData.ServerName ~= "")then
		for i=1, #g_worldListData do
			if (g_worldListData[i].strWorldName == g_RecordUserData.ServerName)then
                local lastServer = UI_GetUILayout(widget, 1123)
                lastServer:removeAllChildren()
                
                local oneServer = Login_InitOneServer(g_worldListData[i],i,false,false,true,chooseServer)
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
        local oneServer = Login_InitOneServer(g_worldListData[i],i,true,true,true,chooseServer)

        if (#g_worldListData-i)%2 == 0 then
            listViewOne = tolua.cast(UI_GetUIListView(widget, 1021):clone(),"ListView")
            listViewOne:setVisible(true)
            listView:pushBackCustomItem(listViewOne)
            print(111)
        end

        if oneServer ~= nil then
            listViewOne:pushBackCustomItem(oneServer)
        end
	end
end
--初始化一个服务器的数据
function Login_InitOneServer(data,tag,isCheckHero,isShowLine,isTouchEnabled,Touch)
    local widget = UI_GetBaseWidgetByName("ServerList")
    if widget then
        local oneServer = UI_GetUILayout(widget, 1122):clone()
        oneServer:setVisible(true)
        oneServer:setTag(tag+1520)
        if isTouchEnabled == true then
            print("Test-------------------------------------------")
            oneServer:setTouchEnabled(true)
            oneServer:addTouchEventListener(Touch)
        else
            oneServer:setTouchEnabled(false)
        end

        local colorID = ccc3(255,255,255)
        if data.iOnlineStatus ~= 1 then--离线
            colorID = ccc3(175,175,175)
        elseif data.iAtt == 0 then --火爆
            UI_GetUIImageView(oneServer, 2101):setVisible(true)
            colorID = ccc3(255,130,40)
        else--新区
            UI_GetUIImageView(oneServer, 2102):setVisible(true)
            colorID = ccc3(255,230,40)
        end

        --服务器名字           
        UI_GetUILabel(oneServer, 1002):setText(data.strWorldName)
        UI_GetUILabel(oneServer, 1002):setColor(colorID)
        UI_GetUILabel(oneServer, 1002):enableStroke()
        --local nameLab_1 = CompLabel:GetCompLabel(FormatString("Login_ServerColor_"..colorID, data.strWorldName), 500);
--        nameLab_1:setAnchorPoint(ccp(0.5,0.5))
--        UI_GetUILabel(oneServer, 1002):addChild(nameLab_1)
        ----服务器状态
        --local nameLab_2
        --if (data.iAtt == 0 and data.iOnlineStatus ~= 1) then
        --    nameLab_2 = CompLabel:GetCompLabel(FormatString("Login_ServerColor_"..colorID,FormatString("Login_ServerState_"..colorID)), 500)
        --else
        --    nameLab_2 = CompLabel:GetCompLabel(FormatString("Login_ServerColor_"..colorID,FormatString("Login_ServerState_"..colorID)), 500) 
        --end
        --UI_GetUILabel(oneServer, 1003):setText("")
        --nameLab_2:setAnchorPoint(ccp(0.5,0.5))
        --UI_GetUILabel(oneServer, 1003):addChild(nameLab_2)

        --服务器ID
        --local nameLab_3 = CompLabel:GetCompLabel(FormatString("Login_ServerColor_"..colorID,FormatString("Login_ServerID",data.iWorldID+1)), 500)
        UI_GetUILabel(oneServer, 1001):setText(FormatString("Login_ServerID",data.iWorldID+1))
        UI_GetUILabel(oneServer, 1001):setColor(colorID)
        UI_GetUILabel(oneServer, 1001):enableStroke()
--        nameLab_3:setAnchorPoint(ccp(0,0.5))
--        UI_GetUILabel(oneServer, 1001):addChild(nameLab_3)

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
function Login_GetServerStatusStr(busyStatus)
	if (busyStatus == RouterServer_pb.en_BusyStatus_Free) then
		return FormatString("en_BusyStatus_Free")
	elseif (busyStatus == RouterServer_pb.en_BusyStatus_Good) then
		return FormatString("en_BusyStatus_Good")
	elseif (busyStatus == RouterServer_pb.en_BusyStatus_Hot) then
		return FormatString("en_BusyStatus_Hot")
	elseif (busyStatus == RouterServer_pb.en_BusyStatus_Full) then
		return FormatString("en_BusyStatus_Full")
	end
	
	return FormatString("en_BusyStatus_Free")
end

function Login_RegisterCreate()
	local widget = UI_CreateBaseWidgetByFileName("Register.json", EUIOpenAction_Enlarge);
	if (widget == nil)then
		Log("Login_RegisterCreate error");
		return;
	end

	local closeButton = UI_GetUIButton(widget, 1);
	closeButton:addTouchEventListener(UI_ClickCloseCurBaseWidget);
	
	local function Register_comfire(sender,eventType) 
		if (eventType == TOUCH_EVENT_ENDED) then
			local registerTextField = UI_GetUITextField(widget:getChildByTag(2), 1);
			local registerStr = registerTextField:getStringValue();
			if (registerStr ~= "") then
				g_RecordUserData.UserName = registerStr;
				Socket_Connect();
				UI_CloseCurBaseWidget();
			else			
				Messagebox_Create({info = FormatString("AccountEmpty"), msgType = EMessageType_LeftRight})
			end
		end
	end 
	local confireButton = UI_GetUIButton(widget, 4);
	confireButton:addTouchEventListener(Register_comfire);
end

function Login_LoadPhoneRecordUserData()
    local isPopup = CCFileUtils:sharedFileUtils():isPopupNotify()
	CCFileUtils:sharedFileUtils():setPopupNotify( false )
	local xmlWrapper = XmlWrapper:new_local();
	local doc = xmlWrapper:GetDoc();
	if( xmlWrapper:LoadFile( (CCFileUtils:sharedFileUtils():getWritablePath()).."RecordUserData.xml" ) )then
		local rootNode = doc:FirstChildElement( "UserDatas" );
		if( rootNode ~= nil )then
			local lastLoginInfoNode = rootNode:FirstChildElement( "LastLoginInfo" );
			if( lastLoginInfoNode ~= nil )then
				g_RecordUserData.ServerId = TypeConvert:ToUint( lastLoginInfoNode:Attribute( "ServerId" ) )
				g_RecordUserData.ServerName = lastLoginInfoNode:Attribute( "ServerName" );
				Log( string.format("LastLoginInfo serverId=%d serverName=%s", g_RecordUserData.ServerId, g_RecordUserData.ServerName) )
			end
		end
		doc:Clear();
	end
	CCFileUtils:sharedFileUtils():setPopupNotify( isPopup )
end

-- 加载保存的用户数据
function Login_LoadRecordUserData()
--	-- 应用宝不保存
--	if getChannelID() == "QQYYB" then
--		return;
--	end
--    -- TBT 不保存
--    if getChannelID() == "TBT" then
--       return ;
--    end

--	if getChannelID() == "UC" then 
--	   return ;
--	end 

--    if getChannelID() == "LJ" then 
--       return ;
--    end 
--    if getChannelID() == "JSFUCK" then
--       return ;
--    end 
    if getChannelID() ~= "PC" then 
       Login_LoadPhoneRecordUserData()
       return ;
    end 

	Log("Login_LoadRecordUserData")
	Log(CCFileUtils:sharedFileUtils():getWritablePath());
	local isPopup = CCFileUtils:sharedFileUtils():isPopupNotify()
	CCFileUtils:sharedFileUtils():setPopupNotify( false )
	local xmlWrapper = XmlWrapper:new_local();
	local doc = xmlWrapper:GetDoc();
	if( xmlWrapper:LoadFile( (CCFileUtils:sharedFileUtils():getWritablePath()).."RecordUserData.xml" ) )then
		local rootNode = doc:FirstChildElement( "UserDatas" );
		if( rootNode ~= nil )then
			-- 手机标识
			--local deviceNode = rootNode:FirstChildElement( "DeviceInfo" );
			--if( deviceNode ~= nil )then
				--g_RecordUserData.Identity = deviceNode:Attribute( "Identity" );
			--end
			-- 上次的登陆信息
			local lastLoginInfoNode = rootNode:FirstChildElement( "LastLoginInfo" );
			if( lastLoginInfoNode ~= nil )then
				g_RecordUserData.UserName = lastLoginInfoNode:Attribute( "UserName" );
				g_RecordUserData.ServerId = TypeConvert:ToUint( lastLoginInfoNode:Attribute( "ServerId" ) )
				g_RecordUserData.ServerName = lastLoginInfoNode:Attribute( "ServerName" );
				g_RecordUserData.AccessToken = lastLoginInfoNode:Attribute( "AccessToken" );
				g_RecordUserData.PayToken = lastLoginInfoNode:Attribute( "PayToken" );
				g_RecordUserData.Pf = lastLoginInfoNode:Attribute( "Pf" );
				g_RecordUserData.PfKey = lastLoginInfoNode:Attribute( "PfKey" );
				Log( string.format("LastLoginInfo user=%s serverId=%d serverName=%s",
					g_RecordUserData.UserName, g_RecordUserData.ServerId, g_RecordUserData.ServerName) )
			end
		end
		doc:Clear();
	end
	CCFileUtils:sharedFileUtils():setPopupNotify( isPopup )
	-- 检查g_RecordUserData.Identity是否匹配
    print("---------------+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++")
end

-- 保存用户数据
function Logion_SavePhoneUserData()
   local xmlWrapper = XmlWrapper:new_local();
   local doc = xmlWrapper:GetDoc();
   local rootNode = doc:NewElement( "UserDatas" )
   local lastLoginInfoNode = doc:NewElement( "LastLoginInfo" )
   lastLoginInfoNode:SetAttribute( "ServerId", TypeConvert:ToString(g_RecordUserData.ServerId) );
   lastLoginInfoNode:SetAttribute( "ServerName", g_RecordUserData.ServerName );
   rootNode:InsertEndChild( lastLoginInfoNode )
		
   doc:InsertEndChild( rootNode )
   doc:SaveFile( (CCFileUtils:sharedFileUtils():getWritablePath()).."RecordUserData.xml", false )
   doc:Clear();
end 

function Login_SaveRecordUserData()
	
	-- 应用宝不保存
	if getChannelID() == "QQYYB" then
		return;
	end
    if getChannelID() == "LJ" then 
       return;
    end 
    if getChannelID() == "JSFUCK" then 
       return 
    end 
    if getChannelID() == "VIVO" then 
       return 
    end 
	local xmlWrapper = XmlWrapper:new_local();
	local doc = xmlWrapper:GetDoc();
	local rootNode = doc:NewElement( "UserDatas" )
	local lastLoginInfoNode = doc:NewElement( "LastLoginInfo" )
	lastLoginInfoNode:SetAttribute( "UserName", g_RecordUserData.UserName );
	lastLoginInfoNode:SetAttribute( "ServerId", TypeConvert:ToString(g_RecordUserData.ServerId) );
	lastLoginInfoNode:SetAttribute( "ServerName", g_RecordUserData.ServerName );
		
	if g_RecordUserData.AccessToken ~= nil then
		lastLoginInfoNode:SetAttribute( "AccessToken", g_RecordUserData.AccessToken );
	end
		
	if g_RecordUserData.PayToken ~= nil then
		lastLoginInfoNode:SetAttribute( "PayToken", g_RecordUserData.PayToken );
	end
		
	if g_RecordUserData.Pf ~= nil then
		lastLoginInfoNode:SetAttribute( "Pf", g_RecordUserData.Pf );
	end
		
	if g_RecordUserData.PfKey ~= nil then
		lastLoginInfoNode:SetAttribute( "PfKey", g_RecordUserData.PfKey );
	end
	rootNode:InsertEndChild( lastLoginInfoNode )
		
	doc:InsertEndChild( rootNode )
	doc:SaveFile( (CCFileUtils:sharedFileUtils():getWritablePath()).."RecordUserData.xml", false )
	doc:Clear();
	--end
end

function Login_ConnectSuc()
	EndWaiting();
	if (g_RecordUserData ~= nil and g_RecordUserData.UserName ~= nil and g_RecordUserData.UserName ~= "")then
		-- 如果有帐号了，发送登录信息
        Log("xxxxxxxxxxxxxxxxxxxaaaaa")
		sendAuthPkg();
	else
		Log("----Check AutoLogin");
		local loginInfo = getLocalLoginInfo(); -- 有可能在启动lua前已经到平台鉴权完毕
		channelLogin_Callback(loginInfo);
	end
	
	Log("Login_ConnectSuc------");
end

function Login_Auth(pkg)
	local tmp = RouterServer_pb.SC_RS_Auth();
	tmp:ParseFromString(pkg)
    Log("SC_RS_Auth======"..tostring(tmp))
	local strAccount = tmp.strAccount
	local iRetCode = tmp.iRetCode
	
	if (iRetCode == RouterServer_pb.RS_AUTU_OK)then
        --Log("time===="..os.date() )
		Packet_Cmd(RouterServer_pb.RS_ROUTER_WORLDLIST);
        ShowWaiting()
	end
end

if (g_worldListData == nil)then
	g_worldListData = {}
end

function Login_WorldList(pkg)
    EndWaiting()
    --Log("time===="..os.date())
	g_worldListData = {}
	
	local tmp = RouterServer_pb.SC_RS_WorldList();
	tmp:ParseFromString(pkg)
	Log("Login_WorldList"..tostring(tmp))
	for i=1,#tmp.worldList do
		g_worldListData[#g_worldListData+1] = tmp.worldList[i]
	end

	local widget = UI_GetBaseWidgetByName("MainMenu");
	if (widget and #g_worldListData) then
	    local layout = UI_GetUILayout(widget, 12345)
		local accountButton = UI_GetUIImageView(layout, 2);
		UI_GetUILabel(layout, 4):setText(g_RecordUserData.UserName);
		
		local serverButton = UI_GetUIImageView(layout, 3);
		if (g_RecordUserData.ServerName == nil or g_RecordUserData.ServerName == "") then
               UI_GetUILabel(layout, 15):setText(FormatString("Login_ServerID",g_worldListData[#g_worldListData].iWorldID+1).." "..g_worldListData[#g_worldListData].strWorldName);
			   g_RecordUserData.ServerId = g_worldListData[#g_worldListData].iWorldID
			   g_RecordUserData.ServerName = g_worldListData[#g_worldListData].strWorldName
            if getChannelID() == "PC" then 
               Login_SaveRecordUserData();
            else 
               Logion_SavePhoneUserData()
            end 
			--MainMenu_Refresh(widget)
		else
			UI_GetUILabel(layout, 15):setText(FormatString("Login_ServerID",g_RecordUserData.ServerId+1).." "..g_RecordUserData.ServerName);
		end
		MainMenu_Refresh(widget)
	end
end

function Login_ALogin(serverId)
	Log("Login_ALogin serverID ="..serverId)
	
	local version = "";
	local channelinfor = Common:GetChannelInfor();
	local jValue = ParseJson(channelinfor)
	if jValue~=nil then
		version = Cjson:GetStr(jValue,"Version")
	end
	version = "1.2.3"
	
	local versionTab = {};
	local function setValue(a)
		versionTab[#versionTab + 1] = a
	end
	string.gsub(version, "(%w+)", setValue);
	local aloginTab = GameServer_pb.Cmd_Cs_Alogin();
	aloginTab.iIsVersionCheck = 1;
	aloginTab.iVersionType = 1;
	aloginTab.iVersionMain = TypeConvert:ToInt(versionTab[1]);
	aloginTab.iVersionFeature = TypeConvert:ToInt(versionTab[2]);
	aloginTab.iVersionBuild = TypeConvert:ToInt(versionTab[3]);
    if getChannelID() == "PC" then 
       aloginTab.strChannelFlag = "PC"
    elseif getChannelID() == "LJ" then 
       aloginTab.strChannelFlag = "LJ"
       aloginTab.strChannelId = TBTSDK:GetInstance().s_ljsdkLoginInfo.channelId
       aloginTab.strUserId = TBTSDK:GetInstance().s_ljsdkLoginInfo.userId
       aloginTab.strToken = TBTSDK:GetInstance().s_ljsdkLoginInfo.tokenId
       aloginTab.strProductCode = TBTSDK:GetInstance().s_ljsdkLoginInfo.productId
       aloginTab.strChannelLabel = TBTSDK:GetInstance().s_ljsdkLoginInfo.channelLabel
    elseif getChannelID() == "QQYYB" then 
       aloginTab.strChannelFlag = "QQ"
       if g_RecordUserData.AccessToken ~= nil then
		  aloginTab.strAccessKey = g_RecordUserData.AccessToken;
	   end

	   if g_RecordUserData.PayToken ~= nil then
		  aloginTab.strPayToken = g_RecordUserData.PayToken;
	   end

	   if g_RecordUserData.Pf ~= nil then
		  aloginTab.strPf = g_RecordUserData.Pf;
	   end

	   if g_RecordUserData.PfKey ~= nil then
		  aloginTab.strPfKey = g_RecordUserData.PfKey;
	   end
    elseif getChannelID() == "UC" then 
       aloginTab.strChannelFlag = "UC"
    elseif getChannelID() == "TBT" then 
       aloginTab.strChannelFlag = "TBT"
    elseif getChannelID() == "JSFUCK" then 
       aloginTab.strChannelFlag = "2144"
    elseif getChannelID() == "VIVO" then 
       aloginTab.strChannelFlag = "vivo"
       aloginTab.strUserId = "vivo_"..TBTSDK:GetInstance().s_VIVOSDKInfo.uid
       aloginTab.strToken = TBTSDK:GetInstance().s_VIVOSDKInfo.token
    end 	

	local msg = GameServer_pb.CSMessage();
	msg.iCmd = GameServer_pb.CMD_ALOGIN;
	if (aloginTab) then
		msg.strMsgBody = aloginTab:SerializeToString();
	end
	
	
	
	local msgStr = msg:SerializeToString();
    Log(">>>>>>>>> aloginTable start.");
    Log(tostring(aloginTab));
    Log(">>>>>>>>> aloginTable end.");
	Log(">>>>>>>>>>>>>>>>>>>>>>>>> +++ login request");
	Log(msgStr);
	Log(">>>>>>>>>>>>>>>>>>>>>>>>> +++ login end");
	
	local rsAloginTab = RouterServer_pb.CS_RS_ALogin();
	rsAloginTab.iWorldID = serverId;
	rsAloginTab.strTransMsg = msgStr;
		
	Packet_Full(RouterServer_pb.RS_ROUTER_ALOGIN, rsAloginTab);
	ClientSink:GetInstance():SetIsConnectGameServer(true);
    if getChannelID() == "PC" then 
       	Login_SaveRecordUserData();
    else 
        Logion_SavePhoneUserData()
    end 

end

function Login_AloginResult(pkg)
	Log("Login_AloginResult")
	local tmp = GameServer_pb.Cmd_Sc_Alogin();
	tmp:ParseFromString(pkg)
	
	if (tmp.iResult == GameServer_pb.en_LoginResult_OK)then	
		Packet_Cmd(GameServer_pb.CMD_GETROLE);
	elseif (tmp.iResult == GameServer_pb.en_LoginResult_SuggestUpdata)then
	
	elseif (tmp.iResult == GameServer_pb.en_LoginResult_MustUpdata)then
	
	elseif (tmp.iResult == GameServer_pb.en_LoginResult_RoleOnLine) then
		Packet_Cmd(GameServer_pb.CMD_GETROLE);
	end
	
	GetGlobalEntity():GetScheduler():RegisterInfiniteScript( "Ping_Update", 30*1000 )

	-- GetGlobalEntity():SetExtraBool("SoundOn",true)
end

function Ping_Update()
	Log("ping--------------------------------")
	Log("time------------------"..os.date())
	Packet_Cmd(GameServer_pb.CMD_PING);
end

function Login_BackToMaiMenu()
	g_dungeonList = {};
	g_openUIType = EUIOpenType_None
	Guide_End()
	UI_CloseAllBaseWidget();
	GetAudioMgr():stopBackgroundMusic()
	
	-- 清除引导
	g_curGuideId = 0;
	g_curGuideStep = 0;
	g_isGuide = false;
	g_nextGuide = {}
	g_clipGuide = nil;
	
	Login_Create();
end

function Socket_OnClose()
    Log(">>>>>>>>>>>> Socket_OnClose() invoked.");

	GetGlobalEntity():GetScheduler():UnRegisterScript("Ping_Update")
	
	if (MainScene:GetInstance():GetState() == EState_Gaming)then
		SceneMgr:GetInstance():SwitchToMainMenu();
	else
		Login_BackToMaiMenu();
	end
end

function Socket_Connect()
	Log("g_newworkData.IP"..g_newworkData.IP)
	Log("g_newworkData.Port"..g_newworkData.Port)
	ClientSink:Connect(g_newworkData.IP, g_newworkData.Port);
	ShowWaiting();
	
	local channelinfor = Common:GetChannelInfor();
	local channel = getChannelID();
	Log("channelinfor ="..channelinfor)
	Log("*****channel="..channel)
end

if (g_newworkData == nil)then
	g_newworkData = {}
end

--奖励配置
function NetWork_Load()
	g_newworkData = {}
	local xmlWrapper = XmlWrapper:new_local();
	local doc = xmlWrapper:GetDoc();
	if( xmlWrapper:LoadFile( "Config/Network.xml" ) )then
		local rootNode = doc:FirstChildElement( "Servers" );
		if( rootNode ~= nil )then
			local valueNode = rootNode:FirstChildElement( "Server" );
			g_newworkData.IP = valueNode:Attribute( "IP" )
			g_newworkData.Port = TypeConvert:ToUint( valueNode:Attribute( "Port" ) )							
		end
		doc:Clear();
	end
end

NetWork_Load()

function Kickoff(pkg)
	local tmp = GameServer_pb.Cmd_Sc_Kickoff();
	tmp:ParseFromString(pkg)
	Log("*********"..tostring(tmp))
	
	Login_BackToMaiMenu()
end

function ParseJson(jstr)
	local jValue = Value:new_local();
	if jstr == nil then
		return jValue
	end
	local jReader = Reader:new_local();
	if (jReader:parse(jstr, jValue, false)) == false then
		Log( "ParseJson Error: "..jstr);
	end
	return jValue
end


--登陆错误 
function LoginError(pkg)
    local tmp = RouterServer_pb.SC_RS_Error();
	tmp:ParseFromString(pkg)
	Log("*********"..tostring(tmp))

    local tab = {}
    tab.info = FormatString("Legion_Error")
    tab.msgType = EMessageType_Middle
    Messagebox_Create(tab) 
end

function Login_UCSub()
   Log("aaaaaaaaaaaaaaaaa")
   local zonename = "";
    if g_RecordUserData.ServerName ~= "" and g_RecordUserData.ServerName ~= nil then 
	   zonename = g_RecordUserData.ServerName
    else 
      zonename = FormatString("No_SeverName")
    end 
    local zoneId = g_RecordUserData.ServerId
    local rolename = GetLocalPlayer():GetEntityName()
    local playerlevel = GetLocalPlayer():GetInt(EPlayer_Lvl)
    local roleId = TBTSDK:GetInstance():getSdkUid()
    submitExternData(zoneId,roleId,rolename,playerlevel,zonename)
end


function SetExtraData(_id)
    local zonename = "";
    if g_RecordUserData.ServerName ~= "" and g_RecordUserData.ServerName ~= nil then 
	   zonename = g_RecordUserData.ServerName
    else 
      zonename = FormatString("No_SeverName")
    end 
	local zoneId = ""..g_RecordUserData.ServerId + 1
   TBTSDK:GetInstance():SetExtRoleData(zonename,zoneId,_id)
end 

function SetExtraDataNewRole()
   local _id = "createRole"
   SetExtraData(_id)
end

function SetExtraDataEnterSever()
   local _id = "enterServer"
   SetExtraData(_id)
end

function SetExtraDataLevelUp()
   local _id = "levelUp"
   SetExtraData(_id)
end
