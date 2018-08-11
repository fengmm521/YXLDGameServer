--好友界面的标识。1.为好友列表 2.添加好友 3.好友申请 
local l_FriendViewState = nil 

local l_MyFriedViewInfo = nil 

local l_applicationFriendInfo = nil 

--好友列表中的list赠送index
local l_MyFriendListGiveClickIndex = nil 

--好友列表中的list收取index
local l_MyFriendListGetClickIndex = nil 

local l_RecommendFriendListInfo = nil 
--推荐好友中去掉已经申请的好友
local l_RecommendFriendListClickIndex = nil 
--申请列表中的list index
local l_applicationlistIndex = nil

--删除好友时候记录的index
local l_deletemyFirendListIndex = nil 

local function DeleteMyFriendlistClickIndex()
	-- body
	if l_deletemyFirendListIndex ~= nil then
       local widget = UI_GetBaseWidgetByName("Friend")
	   local layoutParent = UI_GetUILayout(widget,1)
	   local friendListLayout = UI_GetUILayout(layoutParent,5)
	   local friendList = UI_GetUIListView(friendListLayout,1)
	   friendList:removeItem(l_deletemyFirendListIndex)
	   l_deletemyFirendListIndex = nil 
    end
end

local function RemoveReconmmendFriendlistClickIndex()
	-- body
	if l_RecommendFriendListClickIndex ~= nil then
	   local widget = UI_GetBaseWidgetByName("Friend")
	   local layoutParent = UI_GetUILayout(widget,1)
	   local recommendParent = UI_GetUILayout(layoutParent,6)
       local recommendFriendList = UI_GetUIListView(recommendParent,3)
       recommendFriendList:removeItem(l_RecommendFriendListClickIndex)
       l_RecommendFriendListClickIndex = nil  
    end
end

function FriendBase(ishaveApplication)
	-- body
	local widget = UI_CreateBaseWidgetByFileName("Friend.json")

	local layoutParent = UI_GetUILayout(widget,1)

    local returnBt = UI_GetUIButton(layoutParent,1)
    returnBt:addTouchEventListener(UI_ClickCloseCurBaseWidgetWithNoAction)
    --test 
    if ishaveApplication == true then
       --打开申请界面
       l_FriendViewState = 3
       FriendApplicationInfo(widget)
    else
       --打开好友界面
       l_FriendViewState = 1
       FriendListView(widget)
    end

    --好友列表按钮
    local function MyFriendClickFunc(sender,eventType)
    	-- body
    	if eventType == TOUCH_EVENT_ENDED then
    		Packet_Cmd(GameServer_pb.CMD_FRIENDSYSTEM_QUERY_FRIENDLIST)
    		ShowWaiting()
    	end
    end
    UI_GetUIButton(layoutParent,2):addTouchEventListener(MyFriendClickFunc)

    --推荐列表
    local function RecommendFreindFunc(sender,eventType)
    	-- body
    	if eventType == TOUCH_EVENT_ENDED then
    	   Packet_Cmd(GameServer_pb.CMD_FRIENDSYSTEM_SEARCH_FRIEND)
    	   ShowWaiting()
    	end
    end
    UI_GetUIButton(layoutParent,3):addTouchEventListener(RecommendFreindFunc)

    --申请列表
    local function ApplicationFreindFunc(sender,eventType)
    	-- body
    	if eventType == TOUCH_EVENT_ENDED then
    	   Packet_Cmd(GameServer_pb.CMD_FRIENDSYSTEM_QUERY_REQUESTLIST)
    	   ShowWaiting()
    	end
    end
    UI_GetUIButton(layoutParent,4):addTouchEventListener(ApplicationFreindFunc)
end

--好友列表
function FriendListView(widget)
	-- body
	local layoutParent = UI_GetUILayout(widget,1)

	-- 显示好友列表
	local friendListLayout = UI_GetUILayout(layoutParent,5)
	friendListLayout:setVisible(true)
	--隐藏其他界面
	UI_GetUILayout(layoutParent,6):setVisible(false)
	UI_GetUILayout(layoutParent,7):setVisible(false)

	--设置按钮状态
	UI_GetUIButton(layoutParent,2):setButtonEnabled(false)
	UI_GetUIButton(layoutParent,3):setButtonEnabled(true)
	UI_GetUIButton(layoutParent,4):setButtonEnabled(true)

    UI_GetUIButton(layoutParent,2):setZOrder(100)
    UI_GetUIButton(layoutParent,3):setZOrder(1)
    UI_GetUIButton(layoutParent,4):setZOrder(1)
	--好友列表
    local friendList = UI_GetUIListView(friendListLayout,1)
    friendList:removeAllItems()
    --对好友进行排序，可以领取体力的排在前面
    local list1 = {}
    local list2 = {}
    for i=1,#l_MyFriedViewInfo.szFriendList,1 do
        if l_MyFriedViewInfo.szFriendList[i].iGetType == GameServer_pb.en_PhyStrengthType_Have then
            list1[#list1+1] = l_MyFriedViewInfo.szFriendList[i]
        else
            list2[#list2+1] = l_MyFriedViewInfo.szFriendList[i]
        end
    end
    local list = {}
    for i=1,#list1,1 do
        list[#list+1] = list1[i]
    end
    for i=1,#list2,1 do
        list[#list+1] = list2[i]
    end

    local templayout = Layout:create()
    local width = friendList:getContentSize().width 
    local height = friendList:getContentSize().height 
    local curlayout = nil 
    local function createMyFriendLayout(index)
    	-- body       
        --玩家头像 以玩家主将的头像作为玩家头像
        local data = GetGameData(DataFileHeroBorn, list[index].iVisibleHeroID, "stHeroBornData")
        --UI_GetHeroIncoByClone(data.m_icon,curlayout,1)
        local friendImg = CCSprite:create("Icon/HeroIcon/x"..data.m_icon..".png")
        local imag = UIMgr:GetInstance():createMaskedSprite(friendImg,"zhujiemian/bg_4.png")
        UI_GetUIImageView(curlayout, 8):addNode(imag)
      
	    local function clickMyFriendListFunc(sender,eventType)
	    	-- body
	        if eventType == TOUCH_EVENT_ENDED then
	        	tolua.cast(sender,"Layout")
	        	local clickIndex = friendList:getIndex(sender:getParent())
	        	Log("clickIndex========"..clickIndex)
	        	local friendMenubar = UI_CreateBaseWidgetByFileName("FriendMenuBar.json")
	        	UI_GetUILayout(friendMenubar,1):addTouchEventListener(UI_ClickCloseCurBaseWidgetWithNoAction)
	        	local friendMenubarlayout = UI_GetUILayout(UI_GetUILayout(friendMenubar,1),1)
	            local friendmenubarposition = friendList:getItem(clickIndex):getWorldPosition()

                local pos = friendList:convertToNodeSpace(friendmenubarposition)

	            friendMenubarlayout:setPosition(ccpAdd(pos,ccp(UI_GetUILayout(curlayout,6):getContentSize().width,height / 3)))
	            --查看功能
	            local function lookMyFriendInfoFunc(sender,eventType)
	            	-- body
	            	if eventType == TOUCH_EVENT_ENDED then
	            		Log("lookMyFriendInfoFunc")
	            		local tab = GameServer_pb.CMD_USENAME_FINED_ACTOR_INFO_CS()
	            		UI_CloseCurBaseWidget(EUICloseAction_None)
	            		tab.strActorName = list[index].strFriendName
	            		Packet_Full(GameServer_pb.CMD_USENAME_FINED_ACTOR_INFO,tab)
	            		ShowWaiting()
	            	end
	            end
	            UI_GetUIButton(friendMenubarlayout,1):addTouchEventListener(lookMyFriendInfoFunc)

	            --私聊功能
	            local function WhisperWithMyFirendFunc(sender,eventType)
	            	-- body
	            	if eventType == TOUCH_EVENT_ENDED then
	            		Log("WhisperWithMyFirendFunc")
                        UI_CloseAllBaseWidget()
                        Chat_Create()
                        g_whisperName = list[index].strFriendName
                        Chat_SetChanel(EChannelWhisper);
	            	end
	            end
	            UI_GetUIButton(friendMenubarlayout,2):addTouchEventListener(WhisperWithMyFirendFunc)

	            --删除好友
	            local function DeleteMyFriendFunc(sender,eventType)
	            	-- body
	            	if eventType == TOUCH_EVENT_ENDED then
	            		Log("DeleteMyFriendFunc")
	            		Log("DeleteMyFriendFuncclickIndex======"..clickIndex)
	            		l_deletemyFirendListIndex = clickIndex
	            		UI_CloseCurBaseWidget(EUICloseAction_None)
	            		--Log("deleteMyfirendName======="..tostring(l_MyFriedViewInfo.szFriendList[index].strFriendName))
	            		local tab = GameServer_pb.CMD_FRIENDSYSTEM_DELETE_FRIEND_CS()
	            		tab.strName = list[index].strFriendName
	            		Packet_Full(GameServer_pb.CMD_FRIENDSYSTEM_DELETE_FRIEND,tab)
	            		ShowWaiting()
	            	end
	            end
	            UI_GetUIButton(friendMenubarlayout,3):addTouchEventListener(DeleteMyFriendFunc)
	        end
	    end
        UI_GetUILayout(curlayout,6):setTouchEnabled(true)
        UI_GetUILayout(curlayout,6):addTouchEventListener(clickMyFriendListFunc)

        --玩家名字
        UI_SetLabelText(curlayout,2,list[index].strFriendName)
        --等级
        UI_SetLabelText(curlayout,3,FormatString("Friend_Level",list[index].iFriendLevel))
        --赠送按钮
        local function GiveFriedTiliFunc(sender,eventType)
        	-- body
        	if eventType == TOUCH_EVENT_ENDED then
        	   tolua.cast(sender,"Button")
        	   local ckickIndex = friendList:getIndex(sender:getParent())
               l_MyFriendListGiveClickIndex = ckickIndex 
               Log("xxxxxxxxx==="..ckickIndex)
               local tab = GameServer_pb.CMD_FRIENDSYSTEM_GIVE_PHYSTRENGTH_CS()
               tab.stName = list[index].strFriendName
               Packet_Full(GameServer_pb.CMD_FRIENDSYSTEM_GIVE_PHYSTRENGTH,tab)
               ShowWaiting()
        	end
        end
        UI_GetUIButton(curlayout,4):addTouchEventListener(GiveFriedTiliFunc)
        UI_GetUIButton(curlayout,4):setTouchEnabled(true)

        --领取按钮
        local function GetFriedTiliFunc(sender,eventType)
        	-- body
        	if eventType == TOUCH_EVENT_ENDED then
        	   tolua.cast(sender,"Button")
        	   local ckickIndex = friendList:getIndex(sender:getParent())
               l_MyFriendListGetClickIndex = ckickIndex 
               local tab = GameServer_pb.CMD_FRIENDSYSTEM_GET_PHYSTENGTH_CS()
               tab.stName = list[index].strFriendName
               Log("tab.stName"..tostring(tab.stName))
               Packet_Full(GameServer_pb.CMD_FRIENDSYSTEM_GET_PHYSTENGTH,tab)
               ShowWaiting()
        	end
        end
        UI_GetUIButton(curlayout,5):addTouchEventListener(GetFriedTiliFunc)
        UI_GetUIButton(curlayout,5):setTouchEnabled(true)

        if list[index].iHaveGive == true then
           UI_GetUIButton(curlayout,4):setButtonEnabled(false)
           --UI_GetUIButton(curlayout,4):setTitleText(FormatString("Friend_HvdGiven"))
          -- UI_GetUIButton(curlayout,4):setTitleFontSize(20)
        end

        if list[index].iGetType == GameServer_pb.en_PhyStrengthType_None then
           UI_GetUIButton(curlayout,5):setTouchEnabled(false)
           UI_GetUIButton(curlayout,5):addColorGrayWithButton()
        elseif list[index].iGetType == GameServer_pb.en_PhyStrengthType_Have then
           UI_GetUIButton(curlayout,5):setButtonEnabled(true)
        else
           UI_GetUIButton(curlayout,5):setButtonEnabled(false)
         --  UI_GetUIButton(curlayout,5):setTitleText(FormatString("Friend_HvdGet"))
         --  UI_GetUIButton(curlayout,5):setTitleFontSize(20)
        end
    end
	-- friendList:addEventListenerListView(clickMyFriendListFunc)
	-- friendList:setTouchEnabled(true)
    
    if list ~= nil then
	    for i = 1,#list do
	    	curlayout = UI_GetCloneLayout(UI_GetUILayout(widget,8))
	    	--curlayout:setSize(CCSizeMake(width,height / 3))
	    	curlayout:setVisible(true)
	    	--layout:addChild(curlayout)
	    	createMyFriendLayout(i)
	    	friendList:pushBackCustomItem(curlayout)
	    end
    end
    
    --好友个数
    local friendPlayerNumber = nil 
    if l_MyFriedViewInfo.szFriendList == nil then
        friendPlayerNumber = "0".."/"..l_MyFriedViewInfo.detail.iFrendCountLimit
    else
        friendPlayerNumber = #l_MyFriedViewInfo.szFriendList.."/"..l_MyFriedViewInfo.detail.iFrendCountLimit
    end
    UI_SetLabelText(friendListLayout,2,friendPlayerNumber)
    UI_GetUILabel(friendListLayout,2):enableStroke()
    --赠送次数
    FriendPhyStrength_Refresh(widget,l_MyFriedViewInfo.detail)

    --local function GetAll(sender, eventType) 
    --    local getTimes= 0
    --    --一键收取所有体力赠送
    --    if l_MyFriedViewInfo.szFriendList ~= nil then
    --        for i = 1,#l_MyFriedViewInfo.szFriendList do
	   -- 	    if l_MyFriedViewInfo.szFriendList[i].iGetType == GameServer_pb.en_PhyStrengthType_Have then
    --                   getTimes = getTimes+1
    --                   if getTimes+l_MyFriedViewInfo.detail.iGetTimes >= l_MyFriedViewInfo.detail.iTopGetTimes then
    --                        return 
    --                   end
    --                   l_MyFriendListGetClickIndex = i 
    --                   local tab = GameServer_pb.CMD_FRIENDSYSTEM_GET_PHYSTENGTH_CS()
    --                   tab.stName = l_MyFriedViewInfo.szFriendList[i].strFriendName
    --                   Log("tab.stName"..tostring(tab.stName))
    --                   Packet_Full(GameServer_pb.CMD_FRIENDSYSTEM_GET_PHYSTENGTH,tab)
    --                   ShowWaiting()
    --            end
	   --     end
    --    end
    --end
end

--单独更新
function FriendPhyStrength_Refresh(widget,friendDetail)
	-- body
    local layoutParent = UI_GetUILayout(widget,1)
    local friendListLayout = UI_GetUILayout(layoutParent,5)
    --接受次数
    local tiligiveNumber = friendDetail.iGetTimes.. "/" ..friendDetail.iTopGetTimes 
    UI_SetLabelText(friendListLayout,5,tiligiveNumber)   
    UI_GetUILabel(friendListLayout,5):enableStroke()

    --赠送
    local tiligetNumber = friendDetail.iGiveTimes.. "/" ..friendDetail.iTopGiveTimes 
    UI_SetLabelText(friendListLayout,4,tiligetNumber)
    UI_GetUILabel(friendListLayout,4):enableStroke()
end

function FriendApplicationInfo(widget)
	-- body
	local layoutParent = UI_GetUILayout(widget,1)

	-- 显示申请列表
	local applicationfriendListLayout = UI_GetUILayout(layoutParent,7)
	applicationfriendListLayout:setVisible(true)
	--隐藏其他界面
	UI_GetUILayout(layoutParent,5):setVisible(false)
	UI_GetUILayout(layoutParent,6):setVisible(false)

	--设置按钮状态
	UI_GetUIButton(layoutParent,4):setButtonEnabled(false)
	UI_GetUIButton(layoutParent,3):setButtonEnabled(true)
	UI_GetUIButton(layoutParent,2):setButtonEnabled(true)

    UI_GetUIButton(layoutParent,2):setZOrder(1)
    UI_GetUIButton(layoutParent,3):setZOrder(1)
    UI_GetUIButton(layoutParent,4):setZOrder(100)

	local applicationList = UI_GetUIListView(applicationfriendListLayout,1)
	applicationList:removeAllItems()
	local curlayout = nil 
    local function createapplicationFriend(index)
    	-- body
        --玩家头像 以玩家主将的头像作为玩家头像
        local data = GetGameData(DataFileHeroBorn, l_applicationFriendInfo.szFriendList[index].iVisibleHeroID, "stHeroBornData")
        local friendImg = CCSprite:create("Icon/HeroIcon/x"..data.m_icon..".png")
        local imag = UIMgr:GetInstance():createMaskedSprite(friendImg,"zhujiemian/bg_4.png")
        UI_GetUIImageView(curlayout, 1):addNode(imag)

        --玩家名字
        UI_GetUILabel(curlayout,2):setText(l_applicationFriendInfo.szFriendList[index].strFriendName)

        --等级
        UI_GetUILabel(curlayout,3):setText(FormatString("Friend_Level",l_applicationFriendInfo.szFriendList[index].iFriendLevel))

        --同意按钮
        local function agreeApplicationFunc(sender,eventType)
        	-- body
        	if eventType == TOUCH_EVENT_ENDED then
        		tolua.cast(sender,"Button")
        		local ckickIndex = applicationList:getIndex(sender:getParent())
        		Log("clickIndex......agree"..ckickIndex)
        		l_applicationlistIndex = ckickIndex  
        		local tab = GameServer_pb.CMD_FRIENDSYSTEM_ADDORDELET_REQUESTFRIEND_CS()
        		tab.bAgree = true 
        		tab.stName = l_applicationFriendInfo.szFriendList[index].strFriendName
        		Packet_Full(GameServer_pb.CMD_FRIENDSYSTEM_ADDORDELET_REQUESTFRIEND,tab)
        		ShowWaiting()
        	end
        end
        UI_GetUIButton(curlayout,4):addTouchEventListener(agreeApplicationFunc)

        --拒绝按钮
        local function RefusalApplicationFunc(sender,eventType)
        	-- body
        	if eventType == TOUCH_EVENT_ENDED then
        		tolua.cast(sender,"Button")
        		local clickIndex = applicationList:getIndex(sender:getParent())
        		Log("clickIndex....refusal"..clickIndex)
        		l_applicationlistIndex = clickIndex 
        		local tab = GameServer_pb.CMD_FRIENDSYSTEM_ADDORDELET_REQUESTFRIEND_CS()
        		tab.bAgree = false 
        		tab.stName = l_applicationFriendInfo.szFriendList[index].strFriendName
        		Packet_Full(GameServer_pb.CMD_FRIENDSYSTEM_ADDORDELET_REQUESTFRIEND,tab)
        		ShowWaiting()
        	end
        end
        UI_GetUIButton(curlayout,5):addTouchEventListener(RefusalApplicationFunc)

    end
    if l_applicationFriendInfo.szFriendList ~= nil then 
		for i = 1,#l_applicationFriendInfo.szFriendList do
		    curlayout = UI_GetCloneLayout(UI_GetUILayout(widget,9))
		    curlayout:setVisible(true)
		    createapplicationFriend(i)
		    applicationList:pushBackCustomItem(curlayout)
	    end
    end
end

function removeApplicationlistindex(widget)
	-- body
    local layoutParent = UI_GetUILayout(widget,1)

	-- 显示申请列表
	local applicationfriendListLayout = UI_GetUILayout(layoutParent,7)
	local applicationList = UI_GetUIListView(applicationfriendListLayout,1)
    Log("l_applicationlistIndex.."..l_applicationlistIndex)
    if l_applicationlistIndex ~= nil then
	   applicationList:removeItem(l_applicationlistIndex)
    end
end

function RecommendFriendInfo(widget)
	-- body
	--显示推荐界面
	local layoutParent = UI_GetUILayout(widget,1)
    local recommendParent = UI_GetUILayout(layoutParent,6)
    local recommendFriendList = UI_GetUIListView(recommendParent,3)
	recommendParent:setVisible(true)
	--隐藏其他界面
	UI_GetUILayout(layoutParent,5):setVisible(false)
	UI_GetUILayout(layoutParent,7):setVisible(false)

	UI_GetUIButton(layoutParent,3):setButtonEnabled(false)
	UI_GetUIButton(layoutParent,4):setButtonEnabled(true)
	UI_GetUIButton(layoutParent,2):setButtonEnabled(true)

    UI_GetUIButton(layoutParent,2):setZOrder(1)
    UI_GetUIButton(layoutParent,3):setZOrder(100)
    UI_GetUIButton(layoutParent,4):setZOrder(1)
    --推荐好友列表

    recommendFriendList:removeAllItems()

    local curlayout = nil 
    local function createRecommendFriend(index)
    	-- body
    	--imag 
        --玩家头像 以玩家主将的头像作为玩家头像
        local data = GetGameData(DataFileHeroBorn,l_RecommendFriendListInfo.szSearchResoultList[index].iVisibleHeroID, "stHeroBornData")
        local friendImg = CCSprite:create("Icon/HeroIcon/x"..data.m_icon..".png")
        local imag = UIMgr:GetInstance():createMaskedSprite(friendImg,"zhujiemian/bg_4.png")
        UI_GetUIImageView(curlayout, 1):addNode(imag)

        --玩家名字
        UI_SetLabelText(curlayout,2,l_RecommendFriendListInfo.szSearchResoultList[index].strFriendName)

        --等级
        UI_SetLabelText(curlayout,3,FormatString("Friend_Level",l_RecommendFriendListInfo.szSearchResoultList[index].iFriendLevel))

        local function AddFriendFunc(sender,eventType)
        	-- body
        	if eventType == TOUCH_EVENT_ENDED then
        		Log("Regereed")
        		tolua.cast(sender,"Button")
        		local clickIndex = recommendFriendList:getIndex(sender:getParent())
        		Log("clickIndex ==========="..clickIndex)
        		l_RecommendFriendListClickIndex = clickIndex 
        		local tab = GameServer_pb.CMD_FRIENDSYSTEM_REQUEST_CS()
        		tab.strName = l_RecommendFriendListInfo.szSearchResoultList[index].strFriendName
        		Packet_Full(GameServer_pb.CMD_FRIENDSYSTEM_REQUEST,tab)
        		ShowWaiting()
        	end
        end
        UI_GetUIButton(curlayout,4):addTouchEventListener(AddFriendFunc)

    end
    if l_RecommendFriendListInfo.szSearchResoultList ~= nil then
	    for i = 1,#l_RecommendFriendListInfo.szSearchResoultList do 
	        curlayout = UI_GetCloneLayout(UI_GetUILayout(widget,10))
	        curlayout:setVisible(true)
	        createRecommendFriend(i)
	        recommendFriendList:pushBackCustomItem(curlayout)
	    end
    end
    --输入框
    local function SearchFriendClickFunc(sender,eventType)
    	-- body
    	if eventType == TOUCH_EVENT_ENDED then
    		local strname = UI_GetUITextField(recommendParent,1):getStringValue()
           if strname ~= "" then
           	   Log("strname======="..tostring(strname))
           	   local tab = GameServer_pb.CMD_FRIENDSYSTEM_REQUEST_CS()
           	   tab.strName = strname
           	   Packet_Full(GameServer_pb.CMD_FRIENDSYSTEM_REQUEST,tab)
           else
           	   Log("message")
           	   Messagebox_Create({info = FormatString("Friend_SearcFriend"), msgType = EMessageType_LeftRight})
           end    		
    	end
    end
    UI_GetUIButton(recommendParent,2):addTouchEventListener(SearchFriendClickFunc)
end


local custonToString = tostring
function queryFriendInfo(pkg)
	-- body
	EndWaiting()
    local info = GameServer_pb.CMD_FRIENDSYSTEM_QUERY_SC()
    info:ParseFromString(pkg)
    Log("xxxxx ===="..custonToString(info))
    if info.bIsHaveRequest == true then
    	--有申请的 跳转到申请的界面
        l_applicationFriendInfo = info
    else 
    	--没有申请的 跳转到好友列表界面
    	Log("friend")
    	l_MyFriedViewInfo = info
    end 
    --
    FriendBase(info.bIsHaveRequest)
end
function GivePhysSeccessed(pkg)
	-- body
	EndWaiting()
    local widget = UI_GetBaseWidgetByName("Friend")
    if widget then
        local layoutParent = UI_GetUILayout(widget,1)
    	local friendListLayout = UI_GetUILayout(layoutParent,5)
    	local friendList = UI_GetUIListView(friendListLayout,1)
        UI_GetUIButton(friendList:getItem(l_MyFriendListGiveClickIndex),4):setButtonEnabled(false)
        local info = GameServer_pb.CMD_FRIENDSYSTEM_GIVE_PHYSTRENGTH_SC()
        info:ParseFromString(pkg)
        FriendPhyStrength_Refresh(widget,info.detail)
    end
end

function GetPhysSeccessed(pkg)
	-- body
    EndWaiting()
    Log("getSeccessed")
    local widget = UI_GetBaseWidgetByName("Friend")
    if widget then
        local layoutParent = UI_GetUILayout(widget,1)
    	local friendListLayout = UI_GetUILayout(layoutParent,5)
    	local friendList = UI_GetUIListView(friendListLayout,1)
    	UI_GetUIButton(friendList:getItem(l_MyFriendListGetClickIndex),5):setButtonEnabled(false)
    	local info = GameServer_pb.CMD_FRIENDSYSTEM_GET_PHYSTENGTH_SC()
        info:ParseFromString(pkg)
        FriendPhyStrength_Refresh(widget,info.detail)
    end
end

function getRecommendFriendInfo(pkg)
	-- body
    EndWaiting()
    local widget = UI_GetBaseWidgetByName("Friend")
    if widget then
        l_RecommendFriendListInfo = GameServer_pb.CMD_FRIENDSYSTEM_SEARCH_FRIEND_SC()
        l_RecommendFriendListInfo:ParseFromString(pkg)
        Log("RecommendFriendInfo ===="..custonToString(l_RecommendFriendListInfo))
        RecommendFriendInfo(widget)
    end
end

--很多地方会返回这个还有申请的消息，那么这里要注意判断是否在那个界面
function returnApplicationFriend(pkg)
	-- body
	EndWaiting()
	Log("applicationreturn")
	if UI_HasBaseWidget("Friend") then
	   local widget = UI_GetBaseWidgetByName("Friend")
	   local layoutParent = UI_GetUILayout(widget,1)
       local recommendParent = UI_GetUILayout(layoutParent,6)
       if recommendParent:isVisible() == true then
          RemoveReconmmendFriendlistClickIndex()
          --也许 或者 大概以后会有悬浮框提示
       end
    elseif UI_HasBaseWidget("LegionDetails") then
        createPromptBoxlayout(FormatString("Friend_AddByChat"))
    elseif UI_HasBaseWidget("Chat") then
        --也许 或者 大概以后会有悬浮框提示
        createPromptBoxlayout(FormatString("Friend_AddByChat"))
    end 
end

function returnProcessApplication(pkg)
	-- body
	EndWaiting()
	Log("app sever return")
    local widget = UI_GetBaseWidgetByName("Friend")
    if widget then
	   removeApplicationlistindex(widget)
    end
end

function lookUpMyFriendInfo(pkg)
	-- body
	EndWaiting()
	Log("LookUpmyFriend sever return")
    local widget = UI_GetBaseWidgetByName("Friend")
    if widget then 
    	local info = GameServer_pb.CMD_FRIENDSYSTEM_QUERY_FRIENDLIST_SC()
    	info:ParseFromString(pkg)
    	Log("xxxxx ===="..custonToString(info))
    	l_MyFriedViewInfo = info 
    	FriendListView(widget)
    end
end

function queryFriendApplicationInfo(pkg)
	-- body
	EndWaiting()
	Log("application sever return")
    local widget = UI_GetBaseWidgetByName("Friend")
    if widget then
    	local info = GameServer_pb.CMD_FRIENDSYSTEM_QUERY_REQUESTLIST_SC()
    	info:ParseFromString(pkg)
    	l_applicationFriendInfo = info 
        FriendApplicationInfo(widget)
    end
end

function deleteMyfriendSeccessed(pkg)
	-- body
	EndWaiting()
	Log("delete myFriedn")
	DeleteMyFriendlistClickIndex()
end

ScriptSys:GetInstance():RegisterScriptFunc(GameServer_pb.CMD_FRIENDSYSTEM_QUERY, "queryFriendInfo")
ScriptSys:GetInstance():RegisterScriptFunc(GameServer_pb.CMD_FRIENDSYSTEM_GIVE_PHYSTRENGTH, "GivePhysSeccessed")
ScriptSys:GetInstance():RegisterScriptFunc(GameServer_pb.CMD_FRIENDSYSTEM_GET_PHYSTENGTH, "GetPhysSeccessed")
ScriptSys:GetInstance():RegisterScriptFunc(GameServer_pb.CMD_FRIENDSYSTEM_SEARCH_FRIEND, "getRecommendFriendInfo")
ScriptSys:GetInstance():RegisterScriptFunc(GameServer_pb.CMD_FRIENDSYSTEM_REQUEST, "returnApplicationFriend")
ScriptSys:GetInstance():RegisterScriptFunc(GameServer_pb.CMD_FRIENDSYSTEM_ADDORDELET_REQUESTFRIEND, "returnProcessApplication")
ScriptSys:GetInstance():RegisterScriptFunc(GameServer_pb.CMD_FRIENDSYSTEM_QUERY_FRIENDLIST, "lookUpMyFriendInfo")
ScriptSys:GetInstance():RegisterScriptFunc(GameServer_pb.CMD_FRIENDSYSTEM_QUERY_REQUESTLIST, "queryFriendApplicationInfo")
ScriptSys:GetInstance():RegisterScriptFunc(GameServer_pb.CMD_FRIENDSYSTEM_DELETE_FRIEND, "deleteMyfriendSeccessed")