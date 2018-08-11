--提示性引导标志位
if g_NoticeFunctionFlag == nil then 
	g_NoticeFunctionFlag = 0
end
local FlagPositionNumTab = {}
function GetNoticeFlag()
	if (g_noticeFlagIsShow == false)then
		return;
	end
	-- body
	if GetLocalPlayer() == nil then
		return 
	end
	g_NoticeFunctionFlag = GetLocalPlayer():GetInt(EPlayer_FunctionNotice)

    --这里暂时只有4个
    for i = 0 , GameServer_pb.en_NoticeGuid_ALL - 1 do 
    	local temp = Math:GetAndFlagByPosition(g_NoticeFunctionFlag,i)
        Log("temp===="..temp)
        FlagPositionNumTab[i] = temp
    end
    --RefreshMainUINoticeIcon(FlagPositionNumTab)
end

if (g_noticeFlagIsShow == nil)then
	g_noticeFlagIsShow = true;
end

function ShowNoticeFlag()
	g_noticeFlagIsShow = true;
	--GetNoticeFlag()
end

function  HideNoticeFlag()
	g_noticeFlagIsShow = false;
	for i = 0 , GameServer_pb.en_NoticeGuid_ALL - 1 do 
        FlagPositionNumTab[i] = 0
    end
    Log("HideNoticeFlag")
    RefreshMainUINoticeIcon(FlagPositionNumTab)
end

function RefreshMainUINoticeIcon(FlagPositionNumTab)
	-- body
	Log("hah")
	if UI_HasBaseWidget("Dungeon") then
	   Log("Dungeon")
	   local widget = UI_GetBaseWidgetByName("Dungeon")
	   if FlagPositionNumTab[GameServer_pb.en_NoticeGuid_TASK] == 1 then
		   	  if UI_GetUIButton(widget,101):getNodeByTag(9999) == nil then
	    		-- local frame = ImageView:create()
	    	 --    frame:loadTexture("zhujiemian/bg_tishi.png")
		    	-- UI_GetUIButton(widget,32):addChild(frame)
		    	local effect = GetUIArmature("Effrenwutisi")
		    	UI_GetUIButton(widget,101):addNode(effect,100,9999)
		    	effect:getAnimation():playWithIndex(0)
		      end
		else 
	    	--UI_GetUIButton(widget,32):removeAllChildren()
	    	UI_GetUIButton(widget,101):removeAllNodes()
	    end
        if FlagPositionNumTab[GameServer_pb.en_NoticeGuid_Formation] == 1 then 
            Log("Formation")
            if UI_GetUIButton(widget, 7):getNodeByTag(9999) == nil then
	            if UI_GetUIButton(widget, 7):getChildrenCount() == 0 then
		         --    local frame = ImageView:create()
		    	    -- frame:loadTexture("zhujiemian/bg_tishi.png")
		         --    UI_GetUIButton(widget, 11):addChild(frame)
		            local effect = GetUIArmature("Effrenwutisi")
			    	UI_GetUIButton(widget,7):addNode(effect,100,9999)
			    	effect:getAnimation():playWithIndex(0)
		        end
	        end
        else
           -- UI_GetUIButton(widget,11):removeAllChildren()
            UI_GetUIButton(widget,7):removeAllNodes()
        end
    end
    if UI_HasBaseWidget("DungeonList") then
	   Log("DungeonList")
	   local widget = UI_GetBaseWidgetByName("DungeonList")
	   if FlagPositionNumTab[GameServer_pb.en_NoticeGuid_TASK] == 1 then
		   	  if UI_GetUIButton(widget,11):getNodeByTag(9999) == nil then
	    		-- local frame = ImageView:create()
	    	 --    frame:loadTexture("zhujiemian/bg_tishi.png")
		    	-- UI_GetUIButton(widget,32):addChild(frame)
		    	local effect = GetUIArmature("Effrenwutisi")
		    	UI_GetUIButton(widget,11):addNode(effect,100,9999)
		    	effect:getAnimation():playWithIndex(0)
		      end
		else 
	    	--UI_GetUIButton(widget,32):removeAllChildren()
	    	UI_GetUIButton(widget,11):removeAllNodes()
	    end
        if FlagPositionNumTab[GameServer_pb.en_NoticeGuid_Formation] == 1 then 
            Log("Formation")
            if UI_GetUIButton(widget, 5):getNodeByTag(9999) == nil then
	            if UI_GetUIButton(widget, 5):getChildrenCount() == 0 then
		         --    local frame = ImageView:create()
		    	    -- frame:loadTexture("zhujiemian/bg_tishi.png")
		         --    UI_GetUIButton(widget, 11):addChild(frame)
		            local effect = GetUIArmature("Effrenwutisi")
			    	UI_GetUIButton(widget,5):addNode(effect,100,9999)
			    	effect:getAnimation():playWithIndex(0)
		        end
	        end
        else
           -- UI_GetUIButton(widget,11):removeAllChildren()
            UI_GetUIButton(widget,5):removeAllNodes()
        end
    end
    
   if SceneMgr:GetInstance():GetGameState() == EGameState_MainUi then
        local widget = UI_GetBaseWidgetByName("MainUi")
    	--task
    	if FlagPositionNumTab[GameServer_pb.en_NoticeGuid_TASK] == 1 then
    		Log("Task")
    		if UI_GetUIButton(widget,32):getNodeByTag(9999) == nil then
	    		-- local frame = ImageView:create()
	    	 --    frame:loadTexture("zhujiemian/bg_tishi.png")
		    	-- UI_GetUIButton(widget,32):addChild(frame)
		    	local effect = GetUIArmature("Effrenwutisi")
		    	UI_GetUIButton(widget,32):addNode(effect,100,9999)
		    	effect:getAnimation():playWithIndex(0)
	        end
	    else 
	    	--UI_GetUIButton(widget,32):removeAllChildren()
	    	UI_GetUIButton(widget,32):removeAllNodes()
        end
    	--shop 
    	if FlagPositionNumTab[GameServer_pb.en_NoticeGuid_SHOP] == 1 then
            Log("shop")
            if UI_GetUIButton(widget,18):getNodeByTag(9999) == nil then
	         --    local frame = ImageView:create()
	    	    -- frame:loadTexture("zhujiemian/bg_tishi.png")
	    	    -- UI_GetUIButton(widget,18):addChild(frame)
	    	    local effect = GetUIArmature("Effrenwutisi")
		    	UI_GetUIButton(widget,18):addNode(effect,100,9999)
		    	effect:getAnimation():playWithIndex(0)
	        end
    	else 
    		--UI_GetUIButton(widget,18):removeAllChildren()
    		UI_GetUIButton(widget,18):removeAllNodes()
    	end
    	--friend
    	if FlagPositionNumTab[GameServer_pb.en_NoticeGuid_Friend] == 1 then
           Log("Friend")
           if UI_GetUIButton(widget,27):getNodeByTag(9999) == nil then
	         --   local frame = ImageView:create()
	    	    -- frame:loadTexture("zhujiemian/bg_tishi.png")
	         --    UI_GetUIButton(widget,27):addChild(frame)
	            local effect = GetUIArmature("Effrenwutisi")
		    	UI_GetUIButton(widget,27):addNode(effect,100,9999)
		    	effect:getAnimation():playWithIndex(0)
	        end
        else
           --UI_GetUIButton(widget,27):removeAllChildren()
           UI_GetUIButton(widget,27):removeAllNodes()
        end 

        --Formation 
        if FlagPositionNumTab[GameServer_pb.en_NoticeGuid_Formation] == 1 then 
            Log("Formation")
            if UI_GetUIButton(widget, 11):getNodeByTag(9999) == nil then
	            if UI_GetUIButton(widget, 11):getChildrenCount() == 0 then
		         --    local frame = ImageView:create()
		    	    -- frame:loadTexture("zhujiemian/bg_tishi.png")
		         --    UI_GetUIButton(widget, 11):addChild(frame)
		            local effect = GetUIArmature("Effrenwutisi")
			    	UI_GetUIButton(widget,11):addNode(effect,100,9999)
			    	effect:getAnimation():playWithIndex(0)
		        end
	        end
        else
           -- UI_GetUIButton(widget,11):removeAllChildren()
            UI_GetUIButton(widget,11):removeAllNodes()
        end

        --level up and StepLevelUp 
        if FlagPositionNumTab[GameServer_pb.en_NoticeGuid_LevelUp] == 1 or FlagPositionNumTab[GameServer_pb.en_NoticeGuid_StepLevelUp] == 1 then
           Log("hero")
            if UI_GetUIButton(widget, 12):getNodeByTag(9999) == nil then
           	    if UI_GetUIButton(widget, 12):getChildrenCount() == 0 then
           	   	   local effect = GetUIArmature("Effrenwutisi")
			       UI_GetUIButton(widget,12):addNode(effect,100,9999)
			       effect:getAnimation():playWithIndex(0)
           	    end
            end
        else
            UI_GetUIButton(widget,12):removeAllNodes()
        end

        --gift
        if FlagPositionNumTab[GameServer_pb.en_NoticeGuid_HaveGift] == 1 then
           Log("Gift")
           if UI_GetUIButton(widget, 24):getNodeByTag(9999) == nil then 
              if UI_GetUIButton(widget, 24):getChildrenCount() == 0 then 
                 local effect = GetUIArmature("Effrenwutisi")
                 UI_GetUIButton(widget, 24):addNode(effect,100,9999)
                 effect:getAnimation():playWithIndex(0)
              end
           end
        else 
            UI_GetUIButton(widget, 24):removeAllNodes()
        end
   end
end

--任务 弱引导
if g_isTaskLead == nil then 
   g_isTaskLead = false
end

function startTasklead()
	-- body
	g_isTaskLead = true
end

function isTasklead()
	-- body
	return g_isTaskLead
end

function endTasklead()
	-- body
    g_isTaskLead = false
end

function TaskLeadTerritoryHarry(widget,index,isCanDomainIndex)
	-- body
	if isTasklead() then
		if index == 1 then
	       ShowTaskleadTerritoryHarry(widget)
	    elseif index == 2 then
           UI_GetUIButton(UI_GetUILayout(widget,4),11):removeAllChildren()
        elseif index == 3 then
           for i = 1,5 do
               UI_GetUILayout(UI_GetUIImageView(widget,4),5 + i):removeAllChildren()
           end
           endTasklead()
        elseif index == 4 then 
           ShowTaskLeadPlayerList(widget,isCanDomainIndex)
	    end
    end
end

function ShowTaskleadTerritoryHarry(widget)
	-- body
	--[[local arrow = ImageView:create()
	arrow:loadTexture("tongyong/duihuaqipao_02.png")
	arrow:setFlipX(true)
	--arrow:setScale(0.5)
	local tishiFrame = ImageView:create()
	tishiFrame:loadTexture("tongyong/duihuaqipao_01.png")
	local tishilab = Label:create()
	tishilab:setText(FormatString("askleadTerritoryHarryOne"))
	tishilab:setFontSize(20)
	tishiFrame:setScale(0.9)
	local layout = UI_GetUILayout(widget, 4)
	UI_GetUIButton(layout, 11):addChild(arrow)
	UI_GetUIButton(layout, 11):addChild(tishiFrame)
	tishiFrame:addChild(tishilab)
	UI_MoveForeverAndBack(arrow)
	arrow:setPosition(ccp(-UI_GetUIButton(layout, 11):getContentSize().width , 0))
    tishiFrame:setPosition(ccp(-UI_GetUIButton(layout, 11):getContentSize().width - tishiFrame:getContentSize().width / 2 , 0))]]
end

function ShowTaskLeadPlayerList(widget,isCanDomainIndex)
	-- body
	for i = 1,5 do
        UI_GetUILayout(UI_GetUIImageView(widget,4),5 + i):removeAllChildren()
    end
	local layout = UI_GetUILayout(UI_GetUIImageView(widget, 4),5 + isCanDomainIndex)
	if layout:getChildrenCount() == 0 then 
		local arrow = ImageView:create()
		arrow:loadTexture("tongyong/duihuaqipao_02.png")
	    arrow:setFlipX(true)
		local tishiFrame = ImageView:create()
	    tishiFrame:loadTexture("tongyong/duihuaqipao_01.png")
	    local tishilab = Label:create()
	    tishilab:setText(FormatString("askleadTerritoryHarryTwo"))
	    tishilab:setFontSize(20)
	    tishiFrame:setScale(0.9)
        layout:addChild(tishiFrame)
	    layout:addChild(arrow)
	    tishiFrame:addChild(tishilab)
	    arrow:setPosition(ccp(0,layout:getContentSize().height / 2))
	    tishiFrame:setPosition(ccp(-arrow:getContentSize().width / 2 - tishiFrame:getContentSize().width / 2 + 20,layout:getContentSize().height / 2))
	    UI_MoveForeverAndBack(arrow)
    end
end



--出征英雄升级 升阶提示
function ShowHeroLevelUpOrAscendingOrderNotice(widget)
	-- body
	local flag = {}
	--出征英雄的升级和升阶提示对应
	for i = 0 , GameServer_pb.en_NoticeGuid_ALL - 1 do 
    	local temp = Math:GetAndFlagByPosition(g_NoticeFunctionFlag,i)
        flag[i] = temp
    end

    --升级
    Log("xxxxxx==="..flag[GameServer_pb.en_NoticeGuid_LevelUp])
    if flag[GameServer_pb.en_NoticeGuid_LevelUp] == 1 then
       if UI_GetUIButton(widget,2):getChildByTag(9999) == nil  then
       	  if UI_GetUIButton(widget,2):getChildrenCount() == 0 then
       	  	 local tishiImage = ImageView:create()
       	  	 tishiImage:loadTexture("Common/tishijiantou.png")
       	  	 UI_GetUIButton(widget,2):addChild(tishiImage,9999,9999)
       	  	 tishiImage:setPosition(ccp(UI_GetUIButton(widget,2):getContentSize().width / 2 - 10,10))
       	  end
       end
    else
    	UI_GetUIButton(widget,2):removeAllChildren()
    end

    --升阶
    if flag[GameServer_pb.en_NoticeGuid_StepLevelUp] == 1 then
       if UI_GetUIButton(widget,3):getChildByTag(9999) == nil  then
       	  if UI_GetUIButton(widget,3):getChildrenCount() == 0 then
       	  	 local tishiImage = ImageView:create()
       	  	 tishiImage:loadTexture("Common/tishijiantou.png")
       	  	 UI_GetUIButton(widget,3):addChild(tishiImage,9999,9999)
       	  	 tishiImage:setPosition(ccp(UI_GetUIButton(widget,3):getContentSize().width / 2 - 10,10))
       	  end
       end
    else
    	UI_GetUIButton(widget,3):removeAllChildren()
    end
end

function ShowInFormationHero(widget,IsHeroLevelUp,info)
	-- body
    --移除当前出战的英雄的标识
    RemoveInForMationHeroNotice(widget)
    if IsHeroLevelUp == true then 
       ShowInFormationHeroLevelUp(widget,info)
    else
       ShowInForMationHeroAscendingOrder(widget,info)
    end
end

function RemoveInForMationHeroNotice(widget)
	local listlayout = UI_GetUILayout(widget, 7)
	local herolist = UI_GetUIListView(listlayout,1)
    local heroInForMationSize = GetLocalPlayer():GetHeroInFormationCount()
    --备注下  tag==55 是出战的标识 这里定为66
    for i = 0,heroInForMationSize - 1 do
    	if herolist:getItem(i):getNodeByTag(66) ~= nil then  
    	    herolist:getItem(i):removeAllNodes()
    	end
    end
end

function haveNoticeherobyObjectId(index,info)
	-- body
    local InForMationList = vector_uint_:new_local()
    GetLocalPlayer():GetFormationHeroList(InForMationList)
    Log("heroInForMationSize==="..#info.szdwHeroObjectID )
    local heroEntity = GetEntityById(InForMationList[index],"Hero")
    for i = 0,#info.szdwHeroObjectID do 
    	if heroEntity:GetUint(EHero_ID) == info.szdwHeroObjectID[i] then
    		return true
    	end
    end
    return false
end
--herolist出征英雄有升级提示 
function ShowInFormationHeroLevelUp(widget,info)
	-- body
    local listlayout = UI_GetUILayout(widget, 7)
	local herolist = UI_GetUIListView(listlayout,1)
	local heroInForMationSize = GetLocalPlayer():GetHeroInFormationCount()
	for i = 0,heroInForMationSize - 1 do
		if haveNoticeherobyObjectId(i,info) == true then
	    	if herolist:getItem(i):getNodeByTag(66) == nil then 
	    	    local effect = GetUIArmature("Effkeshenji")
		    	herolist:getItem(i):addNode(effect,100,66)
		    	effect:getAnimation():playWithIndex(0,5) 
		    	effect:getAnimation():setSpeedScale(0.5)
                local width = herolist:getItem(i):getChildByTag(1):getContentSize().width
                local height = herolist:getItem(i):getChildByTag(1):getContentSize().height
	    	    effect:setPosition(ccp(width / 2 + 10,height / 4 + 10))
	    	end
	    else
	    	if herolist:getItem(i):getNodeByTag(66) ~= nil then
	    		herolist:getItem(i):removeAllNodes()
	    	end
        end
    end
end

--herolist出征英雄有升阶提示 
function ShowInForMationHeroAscendingOrder(widget,info)
	-- body
    local listlayout = UI_GetUILayout(widget, 7)
	local herolist = UI_GetUIListView(listlayout,1)
    local heroInForMationSize = GetLocalPlayer():GetHeroInFormationCount()
    for i = 0,heroInForMationSize - 1 do
    	if haveNoticeherobyObjectId(i,info) == true then
	    	if herolist:getItem(i):getNodeByTag(66) == nil then  
	    	    local effect = GetUIArmature("Effkeshenjie")
		    	herolist:getItem(i):addNode(effect,100,66)
		    	effect:getAnimation():playWithIndex(0,5) 
		    	effect:getAnimation():setSpeedScale(0.5)
                local width = herolist:getItem(i):getChildByTag(1):getContentSize().width
                local height = herolist:getItem(i):getChildByTag(1):getContentSize().height
	    	    effect:setPosition(ccp(width / 2 + 10,height / 4 + 10))
	    	end
	    else
	    	if herolist:getItem(i):getNodeByTag(66) ~= nil then
	    		herolist:getItem(i):removeAllNodes()
	    	end 
	    end
    end
end

--新需求，添加弱引导
function NoticeFunction_UpdateRightButton()
   --mainui
   local widget = UI_GetBaseWidgetByName("MainUi") 
   if widget then 
       local rightButton = UI_GetUIButton(widget, 111)
       local rightLayout = UI_GetUIImageView(UI_GetUILayout(widget, 110), 1)
       local tag = {11,32,27}
       NoticeFunction_RightButton_ShowOrFadeRedNode(rightButton,rightLayout,tag)
   end
   local otherWidget = UI_GetBaseWidgetByName("MainRight") 
   if otherWidget then 
      local rightButton = UI_GetUIButton(otherWidget, 2)
      local rightLayout = UI_GetUIImageView(UI_GetUILayout(otherWidget, 1), 1)
      local tag = {3,4,5}
      NoticeFunction_RightButton_ShowOrFadeRedNode(rightButton,rightLayout,tag)
   end 
end 

function NoticeFunction_RightButton_ShowOrFadeRedNode(rightbutton,rightlayout,tag)
   if rightbutton:isBright() == false then 
      if NoticeFunction_ButtonRedIsShow() then 
         UI_GetUIImageView(rightbutton, 1):setVisible(true)
      else 
         UI_GetUIImageView(rightbutton, 1):setVisible(false)
      end 
      for i = 1,3 do 
         UI_GetUIImageView(UI_GetUIButton(rightlayout, tag[i]), 1):setVisible(false)
      end 
   else 
      UI_GetUIImageView(rightbutton, 1):setVisible(false)
      for i = 1,3 do 
         if NoticeFunction_CheckRedIsShow(i) then 
            UI_GetUIImageView(UI_GetUIButton(rightlayout, tag[i]), 1):setVisible(true)
         else 
            UI_GetUIImageView(UI_GetUIButton(rightlayout, tag[i]), 1):setVisible(false)
         end 
      end 
   end 
end

function NoticeFunction_ButtonRedIsShow()
   if NoticeFunction_GetNoticeFlag(GameServer_pb.en_NoticeGuid_Formation) == 1
      or NoticeFunction_GetNoticeFlag(GameServer_pb.en_NoticeGuid_TASK) == 1
      or NoticeFunction_GetNoticeFlag(GameServer_pb.en_NoticeGuid_Friend) == 1 --then 
      or GetLocalPlayer():CheckHeroTallentCanUpOrAct()  then 
      
      return true 
   end 
   return false
end 

--改变阵形之后检测是否有改变
function NoticeFunction_UpdateAfterFormasting()
   Log("xxxxxxxxxxxxxxxxxxxxxxxxxxxxx")
   NoticeFunction_UpdateRightButton()
end 

function NoticeFunction_GetNoticeFlag(index)
   local flag = GetLocalPlayer():GetInt(EPlayer_FunctionNotice)
   Log("fasfasfasfafa=====asdasfaf=="..flag)
    --这里暂时只有4个
    Log("index=========fffff====="..index)
    Log("flag===========sssss====="..Math:GetAndFlagByPosition(flag,index))
    return Math:GetAndFlagByPosition(flag,index)
end 
function NoticeFunction_CheckRedIsShow(checkTag)
   --这里目前check 4个
   --  1. 布阵 2.任务 3.好友 4.商城 5.英雄 6.邮件 7.竞技场 8.领地武魂 9.领地日志 10.领地铁匠铺 11. 主线任务 
--   if checkTag == 1 then 
--      if GetLocalPlayer():CheckHeroTallentCanUpOrAct() then 
--         return true
--      end
--      return false
   if checkTag == 1 then 
      if NoticeFunction_GetNoticeFlag(GameServer_pb.en_NoticeGuid_Formation) == 1 then 
         return true
      end 
      return false      
   elseif checkTag == 2 then
      if NoticeFunction_GetNoticeFlag(GameServer_pb.en_NoticeGuid_TASK) == 1 then 
         return true 
      end  
      return false
   elseif checkTag == 3 then 
      if NoticeFunction_GetNoticeFlag(GameServer_pb.en_NoticeGuid_Friend) == 1 then 
         if GetLocalPlayer():GetInt(EPlayer_Lvl) >= 15 then 
            return true 
         else
            return false 
         end 
      end 
      return false
   elseif checkTag == 4 then 
      if NoticeFunction_GetNoticeFlag(GameServer_pb.en_NoticeGuid_SHOP) == 1 then 
         return true 
      end 
      return false
   elseif checkTag == 5 then 
      if GetLocalPlayer():CheckHeroTallentCanUpOrAct() or GetLocalPlayer():GetEquipBags():PlayerCanEquip() then 
         return true
      end
      return false
   elseif checkTag == 6 then 
      if NoticeFunction_GetNoticeFlag(GameServer_pb.en_NoticeGuid_HaveMail) == 1 then 
         return true 
      end 
      return false
   elseif checkTag == 7 then 
      if NoticeFunction_GetNoticeFlag(GameServer_pb.en_NoticeGuid_ArenaHaveNewLog) == 1 then 
         return true 
      end 
      return false
   elseif checkTag == 8 then 
      if NoticeFunction_GetNoticeFlag(GameServer_pb.en_NoticeGuid_ManorHaveWuHun) == 1 then 
         return true 
      end 
      return false
   elseif checkTag == 9 then 
      if NoticeFunction_GetNoticeFlag(GameServer_pb.en_NoticeGuid_ManorHaveNewLog) == 1 then 
         return true 
      end 
      return false
   elseif checkTag == 10 then 
      if NoticeFunction_GetNoticeFlag(GameServer_pb.en_NoticeGuid_ManorHaveItem) == 1 then 
         return true 
      end 
      return false
   elseif checkTag == 11 then 
      if NoticeFunction_GetNoticeFlag(GameServer_pb.en_NoticeGuid_TASK_Line) == 1 then 
         return true
      end 
      return false
    elseif checkTag == 14 then 
      if NoticeFunction_GetNoticeFlag(GameServer_pb.en_NoticeGuid_FirstPayMent) == 1 then 
         return true
      end 
      return false
   elseif checkTag == 15 then 
      if NoticeFunction_GetNoticeFlag(GameServer_pb.en_NoticeGuid_CheckIn) == 1 then 
         return true
      end 
      return false
   elseif checkTag == 16 then 
      if NoticeFunction_GetNoticeFlag(GameServer_pb.en_NoticeGuid_Legion_Worship) == 1 then 
         return true
      end 
      return false
   elseif checkTag == 17 then 
      if NoticeFunction_GetNoticeFlag(GameServer_pb.en_NoticeGuid_Legion_Salary) == 1 then 
         return true
      end 
      return false
   elseif checkTag == 18 then 
      if NoticeFunction_GetNoticeFlag(GameServer_pb.en_NoticeGuid_Legion_HaveRequest) == 1 then 
         return true
      end 
      return false
   end
   return false
end  

function NoticeFunction_UpdateTopMainUiButton()
   if g_mainUiTopWidget then 
       if NoticeFunction_ButtonRedIsShow() then 
          UI_GetUIImageView(UI_GetUIButton(g_mainUiTopWidget, 10), 1):setVisible(true)
       else 
          UI_GetUIImageView(UI_GetUIButton(g_mainUiTopWidget, 10), 1):setVisible(false)
       end
   end 
end 

--商城的弱引导
function NoticeFunction_UpdateShopButton()
   local widget = UI_GetBaseWidgetByName("MainUi") 
   if widget then 
      if NoticeFunction_CheckRedIsShow(4) then
         UI_GetUIImageView(UI_GetUIImageView(UI_GetUIScrollView(widget, 1), 200), 1):setVisible(true) 
      else 
         UI_GetUIImageView(UI_GetUIImageView(UI_GetUIScrollView(widget, 1), 200), 1):setVisible(false)
      end 
   end 
end 

--邮箱的弱引导
function NoticeFunction_UpdateMailButton()
   local widget = UI_GetBaseWidgetByName("MainUi")
   if widget then 
      if NoticeFunction_CheckRedIsShow(6) then 
         Log("true")
         UI_GetUIImageView(UI_GetUIImageView(UI_GetUIScrollView(widget, 1), 201), 1):setVisible(true) 
      else 
         Log("false")
         UI_GetUIImageView(UI_GetUIImageView(UI_GetUIScrollView(widget, 1), 201), 1):setVisible(false)
      end  
   end 
end 

--竞技场弱引导
function NoticeFunction_UpdateArenaButton()
   local widget = UI_GetBaseWidgetByName("MainUi")
   if widget then 
      if NoticeFunction_CheckRedIsShow(7) then 
         UI_GetUIImageView(UI_GetUIImageView(UI_GetUIScrollView(widget, 1), 202), 1):setVisible(true) 
      else 
         UI_GetUIImageView(UI_GetUIImageView(UI_GetUIScrollView(widget, 1), 202), 1):setVisible(false)
      end  
   end 
   local arenaWiget = UI_GetBaseWidgetByName("Arena")
   if arenaWiget then 
      if NoticeFunction_CheckRedIsShow(7) then 
         UI_GetUIImageView(UI_GetUIButton(arenaWiget,4647976),1):setVisible(true) 
      else 
         UI_GetUIImageView(UI_GetUIButton(arenaWiget,4647976),1):setVisible(false)
      end  
   end 
end 
--签到弱引导
function NoticeFunction_UpdateCheckIn()
   local widget = UI_GetBaseWidgetByName("MainUi")
   if widget then 
      if NoticeFunction_CheckRedIsShow(15) then 
         UI_GetUIImageView(UI_GetUIButton(widget:getChildByTag(1401), 1002), 1002):setVisible(true) 
      else 
         UI_GetUIImageView(UI_GetUIButton(widget:getChildByTag(1401), 1002), 1002):setVisible(false) 
      end  
   end 
end
--领地弱引导
function NoticeFunction_UpdateTerritoryButton()
   local widget = UI_GetBaseWidgetByName("MainUi")
   if widget then 
      if NoticeFunction_CheckRedIsShow(8) or NoticeFunction_CheckRedIsShow(9) or NoticeFunction_CheckRedIsShow(10) then 
         UI_GetUIImageView(UI_GetUIImageView(UI_GetUIScrollView(widget, 1), 203), 1):setVisible(true) 
      else 
         UI_GetUIImageView(UI_GetUIImageView(UI_GetUIScrollView(widget, 1), 203), 1):setVisible(false)
      end  
   end
   local territoryWidget = UI_GetBaseWidgetByName("MyTerritory")
   if territoryWidget then 
--      --武魂店
--      local bgLayout = UI_GetUILayout(territoryWidget,1)
--      if NoticeFunction_CheckRedIsShow(8) then 
--         UI_GetUIImageView(UI_GetUIImageView(territoryWidget,200),1):setVisible(true)
--      else
--         UI_GetUIImageView(UI_GetUIImageView(territoryWidget,200),1):setVisible(false) 
--      end 

      --日志
      if NoticeFunction_CheckRedIsShow(9) then 
         UI_GetUIImageView(UI_GetUIButton(territoryWidget,90),1):setVisible(true)
      else 
         UI_GetUIImageView(UI_GetUIButton(territoryWidget,90),1):setVisible(false)
      end 

--      --铁匠铺
--      if NoticeFunction_CheckRedIsShow(10) then 
--         UI_GetUIImageView(UI_GetUIImageView(territoryWidget,201),1):setVisible(true)
--      else
--         UI_GetUIImageView(UI_GetUIImageView(territoryWidget,201),1):setVisible(false) 
--      end 
   end 
end 

--英雄弱引导
function NoticeFunction_UpdateHeroButton()
   local widget = UI_GetBaseWidgetByName("MainUi") 
   if widget then 
       local rightButton = UI_GetUIButton(widget, 111)
       local rightLayout = UI_GetUIImageView(UI_GetUILayout(widget, 110), 1)
       NoticeFunction_RightButton_ShowOrFadeRedNodeByHeroButton(rightButton,rightLayout,12)
   end
   local otherWidget = UI_GetBaseWidgetByName("MainRight") 
   if otherWidget then 
      local rightButton = UI_GetUIButton(otherWidget, 2)
      local rightLayout = UI_GetUIImageView(UI_GetUILayout(otherWidget, 1), 1)
      NoticeFunction_RightButton_ShowOrFadeRedNodeByHeroButton(rightButton,rightLayout,1)
   end 
end 

function NoticeFunction_RightButton_ShowOrFadeRedNodeByHeroButton(rightbutton,rightlayout,tag)  
   if rightbutton:isBright() == false then 
      if NoticeFunction_ButtonRedIsShow() then 
         UI_GetUIImageView(rightbutton, 1):setVisible(true)
      else 
         UI_GetUIImageView(rightbutton, 1):setVisible(false)
      end 
      UI_GetUIImageView(UI_GetUIButton(rightlayout, tag), 1):setVisible(false)
   else 
      UI_GetUIImageView(rightbutton, 1):setVisible(false)
      if NoticeFunction_CheckRedIsShow(5) then 
         UI_GetUIImageView(UI_GetUIButton(rightlayout, tag), 1):setVisible(true)
      else 
         UI_GetUIImageView(UI_GetUIButton(rightlayout, tag), 1):setVisible(false)
      end 
   end
end 

--主线任务支线任务弱引导
function NoticeFunction_UpdateTaskTwoButton()
   local widget = UI_GetBaseWidgetByName("MainUi")
   if widget then 
      if NoticeFunction_CheckRedIsShow(11) then 
         UI_GetUIImageView(UI_GetUIButton(widget,260), 1):setVisible(true) 
      else 
         UI_GetUIImageView(UI_GetUIButton(widget,260), 1):setVisible(false)
      end  
   end 
end 
--首充礼包弱引导
function NoticeFunction_UpdateFirstPayButton()
   local widget = UI_GetBaseWidgetByName("MainUi")
   if widget then 
      if NoticeFunction_CheckRedIsShow(14) then 
         UI_GetUIImageView(UI_GetUIButton(widget:getChildByTag(1401),1001), 1002):setVisible(true) 
      else 
         UI_GetUIImageView(UI_GetUIButton(widget:getChildByTag(1401),1001), 1002):setVisible(false)
      end  
   end 
end 
--膜拜大神弱引导
function NoticeFunction_UpdateLegionWorship()
   local widget = UI_GetBaseWidgetByName("LegionDetails")
   if widget then 
        local listView = UI_GetUIListView(widget:getChildByTag(2), 4)
        local ImageView = tolua.cast(listView:getItem(0),"ImageView")
      if NoticeFunction_CheckRedIsShow(16) then 
         UI_GetUIImageView(ImageView, 1122):setVisible(true)
      else 
         UI_GetUIImageView(ImageView, 1122):setVisible(false)
      end  
   end 
end 
--军团俸禄弱引导
function NoticeFunction_UpdateLegionSalayr()
   local widget = UI_GetBaseWidgetByName("LegionDetails")
   if widget then 
        local listView = UI_GetUIListView(widget:getChildByTag(2), 4)
        local ImageView = tolua.cast(listView:getItem(1),"ImageView")
      if NoticeFunction_CheckRedIsShow(17) then 
         UI_GetUIImageView(ImageView, 1122):setVisible(true) 
      else 
         UI_GetUIImageView(ImageView, 1122):setVisible(false)
      end  
   end 
end 
--军团管理-有军团请求弱引导
function NoticeFunction_UpdateLegionHaveRequest()
   local widget = UI_GetBaseWidgetByName("LegionDetails")
   if widget then 
      if NoticeFunction_CheckRedIsShow(18) then 
         UI_GetUIImageView(widget:getChildByTag(4):getChildByTag(1), 1122):setVisible(true) 
      else 
         UI_GetUIImageView(widget:getChildByTag(4):getChildByTag(1), 1122):setVisible(false)
      end  
   end 
   local widget1 = UI_GetBaseWidgetByName("LegionManage")
   if widget1 then 
      if NoticeFunction_CheckRedIsShow(18) then 
         UI_GetUIImageView(widget1:getChildByTag(1):getChildByTag(1), 1122):setVisible(true) 
      else 
         UI_GetUIImageView(widget1:getChildByTag(1):getChildByTag(1), 1122):setVisible(false)
      end  
   end
end 
function NoticeFunction_UpdateMainUIButton()
    NoticeFunction_UpdateMailButton()
    NoticeFunction_UpdateArenaButton()
    NoticeFunction_UpdateTerritoryButton()
    NoticeFunction_UpdateTaskTwoButton()
    NoticeFunction_UpdateShopButton()
end 
--军团
function NoticeFunction_UpdateLegionButton()
    NoticeFunction_UpdateLegionWorship()
    NoticeFunction_UpdateLegionSalayr()
    NoticeFunction_UpdateLegionHaveRequest()
end
--判断是否有英雄的天赋可以激活或者是升级



--购买体力提示
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_NOTICE_CLIENT_PUSHVIP_SCENE, "TiliGouMaiVipTip_SC" )

function TiliGouMaiVipTip_SC(pkg)
   local function lookVipInfo()
      UI_CloseCurBaseWidget(EUICloseAction_FadeOut, 0.25)
      VIPInfo()
   end 
   Messagebox_Create({info = FormatString("Tili_BuyTipOne"), msgType = EMessageType_LeftRight, leftFun = lookVipInfo,leftText = FormatString("Tili_BuyTipTwo")})  
end 
