local l_taskInfo = nil 

--这里是csv表中的奖励字段
local l_rewardtype = {"LifeAtt","Item","Hero"}

--任务中作为跳转的地方
function TaskGotoPurpose(purpose,Relevance2)
	-- body
    if purpose == "Dungeon" then
       if Relevance2 == 0 then 
          
       else 
          local id = TypeConvert:ToInt(Relevance2)
          g_selectSectionID = id
       end 
        --Dungeon_ListCreate(true);
        g_selectSectionIndex = -1
        UI_CloseAllBaseWidget()
		local tab = GameServer_pb.Cmd_Cs_QueryDSectionDetail()
		tab.iSectionID = g_selectSectionID
		Packet_Full(GameServer_pb.CMD_QUERY_DSECTIONDETAIL, tab)
		ShowWaiting()
    elseif purpose == "CompBattle" then
       Packet_Cmd(GameServer_pb.CMD_CAMPBATTLE_OPEN)
       ShowWaiting()
    elseif purpose == "WorldBoss" then
    	Packet_Cmd(GameServer_pb.CMD_QUERY_WORLDBOSS)
        ShowWaiting()
    elseif purpose == "Shop" then
    	Shop_Create()
	elseif purpose == "Hero" then
	    HeroListSecond_CreateHeroListLayout()
	elseif purpose == "GodAnimal" then
		if( GetLocalPlayer():GetGodAnimalList():size()>0)then
		   GodAnimalSystem_create()
		end
	elseif purpose == "Babel" then
	    Packet_Cmd(GameServer_pb.CMD_QUERY_CLIMBTOWERDETAIL)
    elseif purpose == "FightSoul" then
    	Packet_Cmd(GameServer_pb.CMD_QUERY_FIGHTSOUL);
		ShowWaiting();
    elseif purpose == "TerritoryHarry" then
        --领地掠夺
        Packet_Cmd(GameServer_pb.CMD_MANOR_QUERY);
	    ShowWaiting();
        startTasklead()
    elseif purpose == "Territory" then
        Packet_Cmd(GameServer_pb.CMD_MANOR_QUERY);
	    ShowWaiting();
    elseif purpose == "MainUI" then
        UI_CloseAllBaseWidget()
    elseif purpose == "dreamland" then 
        Packet_Cmd(GameServer_pb.CMD_DREAMLAND_OPEN_REQSECTON);
		ShowWaiting()
    elseif purpose == "gold" then
        UI_CloseAllBaseWidget() 
        CashCow_Create()
    elseif purpose == "friend" then 
        UI_CloseAllSpecial();
    	Packet_Cmd(GameServer_pb.CMD_FRIENDSYSTEM_QUERY)
        ShowWaiting()
    elseif purpose == "fight" then 
        Packet_Cmd(GameServer_pb.CMD_OPEN_ARENA);
	    ShowWaiting();
    end
end

function InitTaskLayout()
	-- body
	local widget = UI_CreateBaseWidgetByFileName("TaskSystem.json")

	--return 
	UI_GetUIButton(UI_GetUILayout(widget,1),1):addTouchEventListener(UI_ClickCloseCurBaseWidgetWithNoAction)

	ShowTaskInfo(widget)
end

function ShowTaskInfo(widget)
	-- body
	local parentlayout = UI_GetUILayout(widget,1)
    
    local tasklist = UI_GetUIListView(parentlayout,2)
    tasklist:removeAllItems()
    local curlayout = nil
    if l_taskInfo.type == GameServer_pb.enTaskType_DayHave then 
       Log("xxxxxxxxxxxxx")
--       UI_SetLabelText(UI_GetUIImageView(parentlayout,10),1,FormatString("TaskSystem_Richang"))
       UI_GetUIImageView(UI_GetUIImageView(parentlayout,10),1):setVisible(false)
       UI_GetUIImageView(UI_GetUIImageView(parentlayout,10),2):setVisible(true)
    else
    Log("xxxxxxxxxxxxx")
       UI_GetUIImageView(UI_GetUIImageView(parentlayout,10),1):setVisible(true)
       UI_GetUIImageView(UI_GetUIImageView(parentlayout,10),2):setVisible(false)
    end 
    local function createtaskInfo(index)
    	-- body
    	local data = GetGameData(DataFileTask,l_taskInfo.szTaskDetail[index].iTaskId,"stTaskData")
    	--miaoshu
    	UI_SetLabelText(curlayout,2,data.m_info)
    	--图标
    	--1.主线 2.悬赏 3.活动任务
--    	if l_taskInfo.szTaskDetail[index].iTaskType == 1 then
--            UI_GetUIImageView(curlayout,1):loadTexture("renwu/renwu_005.png")
--    	elseif l_taskInfo.szTaskDetail[index].iTaskType == 2 then
--            UI_GetUIImageView(curlayout,1):loadTexture("renwu/renwu_007.png")
--    	elseif l_taskInfo.szTaskDetail[index].iTaskType == 3 then
--            UI_GetUIImageView(curlayout,1):loadTexture("renwu/renwu_006.png")
--        end
        UI_GetUIImageView(curlayout,1):loadTexture("renwu/"..data.m_Icon..".png")
        --name
        UI_SetLabelText(curlayout,9,data.m_name)
        --奖励
        local function getreward(src,tb)
        	-- body
        	-- tb = {}
        	Log("src==="..tostring(src))
        	local function spilit(txt)
        		-- body
        		table.insert(tb,txt)
        	end
        	string.gsub(src,"(%w+)#?",spilit)
        end
        local rewardtable = {}
        if data.m_rewardTypeone ~= "" then
           local tb = {}
           getreward(data.m_rewardTypeone,tb)
           table.insert(rewardtable,tb)
        end

        if data.m_rewardTypetwo ~= "" then
        	local tb = {}
        	getreward(data.m_rewardTypetwo,tb)
        	table.insert(rewardtable,tb)
        end

        if data.m_rewardTypetree ~= "" then
        	local tb = {}
        	getreward(data.m_rewardTypetree,tb)
        	table.insert(rewardtable,tb)
        end
        local rewardlayout = UI_GetUILayout(curlayout,4)
        local positionX,positionY,offestY,offestX = 0,0,0,0
        for i = 1,#rewardtable do
            --
            -- rewardlayout:addChild(reward[i])
            if rewardtable[i][1] == l_rewardtype[1] then
            	local lifeattIcon = GetAttIconNoFrame(rewardtable[i][2])
            	lifeattIcon:setAnchorPoint(ccp(0,0))
            	lifeattIcon:setScale(0.8)
            	lifeattIcon:setPosition(ccp(offestX,-5))
            	rewardlayout:addChild(lifeattIcon)
            	local label = Label:create()
            	label:setAnchorPoint(ccp(0,0))
            	positionX,positionY = lifeattIcon:getPosition()
            	positionX = positionX + lifeattIcon:getContentSize().width * 0.8
                offestX =  positionX + lifeattIcon:getContentSize().width + 20
                offestY = positionY - lifeattIcon:getContentSize().height * 0.8
            	-- label:setText("x"..rewardtable[i][3])
            	-- label:setFontSize(15)
            	-- label:setPosition(ccp(positionX,positionY))
            	-- rewardlayout:addChild(label)
            	--positionX = label:getPosition() + label:getContentSize().width
            elseif rewardtable[i][1] == l_rewardtype[2] then
    	        local Itemdata = GetGameData(DataFileItem,rewardtable[i][2],"stItemData")
    	        local imageName = "Icon/Item/"..Itemdata.m_icon..".png"
                local itemImage = UI_GetCloneLayout(UI_GetUILayout(parentlayout,20))
                itemImage:setVisible(true)
                UI_GetUIImageView(itemImage,1):loadTexture(imageName)
--    	        itemImage:loadTexture(imageName)
--    	        itemImage:setAnchorPoint(ccp(0,0))
     	        itemImage:setPosition(ccp(offestX,-5))
--    	        itemImage:setScale(0.4)
    	        rewardlayout:addChild(itemImage)
    	        positionX,positionY = itemImage:getPosition()
    	        positionX = positionX + itemImage:getContentSize().width
                offestX = positionX + itemImage:getContentSize().width
    	        offestY = positionY - itemImage:getContentSize().height * 0.35 + 5
                -- local label = Label:create()
    	        -- label:setAnchorPoint(ccp(0,0))
    	        -- label:setText("x"..rewardtable[i][3])
            	-- label:setFontSize(15)
            	-- label:setPosition(ccp(positionX,positionY))
            	-- rewardlayout:addChild(label)
            	-- positionX = label:getPosition() + label:getContentSize().width
            elseif rewardtable[i][1] == l_rewardtype[3] then
            	local herodata = GetGameData(DataFileHeroBorn, rewardtable[i][2] , "stHeroBornData")
                local heroIcon = ImageView:create() 
                Log("xxxx=="..herodata.m_icon)
                heroIcon:loadTexture("Icon/HeroIcon/"..herodata.m_icon..".png")
                heroIcon:setScale(0.5)
                heroIcon:setAnchorPoint(ccp(0,0))
                rewardlayout:addChild(heroIcon)
                heroIcon:setPosition(ccp(offestX,0))
                positionX,positionY = heroIcon:getPosition()
    	        positionX = positionX + heroIcon:getContentSize().width * 0.5  + 20
                offestX = positionX + heroIcon:getContentSize().width*0.35
    	        offestY = positionY - heroIcon:getContentSize().height * 0.5 + 5
                -- local label = Label:create()
    	        -- label:setAnchorPoint(ccp(0,0))
    	        -- label:setText("x"..rewardtable[i][3])
            	-- label:setFontSize(15)
            	-- label:setPosition(ccp(positionX,positionY))
            	-- rewardlayout:addChild(label)
            	-- positionX = label:getPosition() + label:getContentSize().width
            end
              local label = Label:create()
    	      label:setAnchorPoint(ccp(0,0))
    	      label:setText("x"..rewardtable[i][3])
              label:setFontSize(20)
              label:setPosition(ccp(positionX,positionY + 5))
              rewardlayout:addChild(label)
              positionX = label:getPosition() + label:getContentSize().width
              Log("positionX==="..positionX)
              offestX = positionX + 10
        end


        --进度
        UI_SetLabelText(curlayout,5,l_taskInfo.szTaskDetail[index].iTaskOver.. "/".. l_taskInfo.szTaskDetail[index].iTaskTotal)

        -- button
        local function ClickbuttonFunc(sender,eventType)
        	-- body
        	if eventType == TOUCH_EVENT_ENDED then
        		local sender = tolua.cast(sender,"Button")
        		local tag = tasklist:getIndex(sender:getParent()) 
				
        		-- Log("tag====="..tag)
        		if GameServer_pb.en_Task_OPen == l_taskInfo.szTaskDetail[index].iTaskState then
        		   Log("go")
                   UI_CloseCurBaseWidget(EUICloseAction_None)
                   TaskGotoPurpose(data.m_Relevance,data.m_Relevance2)
        		elseif GameServer_pb.en_Task_Finish == l_taskInfo.szTaskDetail[index].iTaskState then
        		   Log("get")
                   local tab = GameServer_pb.CMD_TASK_GET_REWARD_CS()
                   tab.iTaskId = l_taskInfo.szTaskDetail[tag + 1].iTaskId 
                   tab.type = l_taskInfo.type
                   Packet_Full(GameServer_pb.CMD_TASK_GET_REWARD,tab)
                   ShowWaiting()
        		end
				
				if g_isGuide then
					Guide_GoNext();
				end
				
        	end
        end
        UI_GetUIButton(curlayout,6):setTitleFontSize(24)
        if GameServer_pb.en_Task_OPen == l_taskInfo.szTaskDetail[index].iTaskState then
        	UI_GetUIButton(curlayout,6):setTitleText(FormatString("Task_willgo"))
        elseif GameServer_pb.en_Task_Finish == l_taskInfo.szTaskDetail[index].iTaskState then 
        	UI_GetUIButton(curlayout,6):setTitleText(FormatString("Task_getreward"))
        end
		UI_GetUIButton(curlayout,6):setName("Btn_func_"..index)
        UI_GetUIButton(curlayout,6):addTouchEventListener(ClickbuttonFunc)
    end
    local width = tasklist:getContentSize().width 
    local templayout = Layout:create()
    templayout:setSize(CCSizeMake(width / 3, tasklist:getContentSize().height))
    tasklist:setItemModel(templayout)
    tasklist:setGravity(LISTVIEW_GRAVITY_BOTTOM)

	for i = 1,#l_taskInfo.szTaskDetail do
       curlayout = UI_GetCloneLayout(UI_GetUILayout(parentlayout,3))
       curlayout:setVisible(true)
       createtaskInfo(i)
       tasklist:pushBackCustomItem(curlayout)
	end

    if (TaskTili_ReturnType() == 1 or TaskTili_ReturnType() == 2) and l_taskInfo.type == GameServer_pb.enTaskType_DayHave then 
       if l_taskInfo.bVigorHaveGet == false then 
           if GetLocalPlayer():IsInCurTime(g_TaskTiliTimeAm_TimeBegin,g_TaskTiliTimeAm_TimeEnd) or GetLocalPlayer():IsInCurTime(g_TaskTiliTimePm_TimeBegin,g_TaskTiliTimePm_TimeEnd) then 
              tasklist:insertCustomItem(Task_Tili(widget),0)
           else
              tasklist:pushBackCustomItem(Task_Tili(widget))
           end
       else 
           --tasklist:pushBackCustomItem(Task_Tili(widget))
       end 
    end
    if l_taskInfo.type == GameServer_pb.enTaskType_DayHave then  
        if (l_taskInfo.monthcardInfo.iRemaindTimes == 0) or (l_taskInfo.monthcardInfo.bTodayHaveGet)  then 
           tasklist:pushBackCustomItem(Task_MonthCard(widget))
        else
           tasklist:insertCustomItem(Task_MonthCard(widget),0) 
        end 
    end 
end

function TaskTili_ReturnType()
   local time = GetLocalPlayer():getCurrentTime()
   if time >= g_TaskTiliTimeAm_ShowTime and time <= g_TaskTiliTimeAm_TimeEnd then 
      return 1
   elseif time >= g_TaskTiliTimePm_ShowTime and time <= g_TaskTiliTimePm_TimeEnd then 
      return 2
   else
      return 3
   end  
   return 0
end

function Task_Tili(widget,tag)
   local layout = UI_GetCloneLayout(UI_GetUILayout(UI_GetUILayout(widget,1),30))
   layout:setVisible(true)
   local typeindex = TaskTili_ReturnType()
   local titlestr = g_TaskTili_Info[typeindex][1]
   local infostr = g_TaskTili_Info[typeindex][2] 
   local awardstr = g_TaskTili_Info[typeindex][3]
   --title
   UI_SetLabelText(layout,3,titlestr)
   --info
   UI_SetLabelText(layout,1,infostr)
   --award
   UI_SetLabelText(layout,2,awardstr)

   --uibutton
   local function ClickGet(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then 
         Packet_Cmd(GameServer_pb.CMD_TASK_GET_VIGOR)
         ShowWaiting()
      end 
   end
   UI_GetUIButton(layout,4):addTouchEventListener(ClickGet)

    if GetLocalPlayer():IsInCurTime(g_TaskTiliTimeAm_TimeBegin,g_TaskTiliTimeAm_TimeEnd) or GetLocalPlayer():IsInCurTime(g_TaskTiliTimePm_TimeBegin,g_TaskTiliTimePm_TimeEnd) then
    --if tag == 0 then
		UI_GetUIButton(layout,4):setEnabled(true)
   else 
		UI_GetUIButton(layout,4):setEnabled(false)
   end 
   return layout
end

function Task_MonthCard(widget)
   local layout = UI_GetCloneLayout(UI_GetUILayout(UI_GetUILayout(widget,1),50))
   layout:setVisible(true)
   local function clickBuyMonthCard(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then 
         UI_CloseCurBaseWidget()
         RechargeSystem_ShowLayout()
      end 
   end 
   local function clickGetMonthCard(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then  
         Packet_Cmd(GameServer_pb.CMD_GET_MONTHCARD_REWARD)
         ShowWaiting()
      end 
   end
   if l_taskInfo.monthcardInfo.iRemaindTimes ~= 0 then 
      if l_taskInfo.monthcardInfo.bTodayHaveGet then 
         UI_GetUILabel(layout,1):setText(FormatString("Pay_MonthCardHaveGet",l_taskInfo.monthcardInfo.iRemaindTimes))
         UI_GetUIButton(layout,2):setVisible(false)
      else
         UI_GetUIButton(layout,2):setVisible(true)
         UI_GetUIButton(layout,2):addTouchEventListener(clickGetMonthCard)
         UI_GetUILabel(layout,1):setText(FormatString("Pay_MonthCardCanGet"))
      end 
   else 
      UI_GetUILabel(layout,1):setText(FormatString("Pay_MonthCardNotBuy"))
      UI_GetUIButton(layout,2):setTitleText(FormatString("Pay_MonthCardToBuy"))
      UI_GetUIButton(layout,2):addTouchEventListener(clickBuyMonthCard)
   end 
   return layout
end 

function paixu()
    -- body
    local templab = {}
    templab = l_taskInfo.szTaskDetail
    local number = 1
    for i = 1, #l_taskInfo.szTaskDetail do
        if GameServer_pb.en_Task_Finish == l_taskInfo.szTaskDetail[i].iTaskState then 
            Log("l_taskInfo.szTaskDetail[i]=="..l_taskInfo.szTaskDetail[i].iTaskId)
            local tabletemp = nil
            tabletemp = l_taskInfo.szTaskDetail[i]
            table.remove(templab,i)
            Log("number==="..number)
            table.insert(templab,number,tabletemp)
            number = number + 1
        end
    end
    for i = 1 , #templab do
        Log("afasfasfa==="..tostring(templab[i]))
        l_taskInfo.szTaskDetail[i] = templab[i]
    end
end


function LookTaskInfo(pkg)
	-- body
	Log("Recv LookTaskInfo---------------------------");
	EndWaiting()
	l_taskInfo = GameServer_pb.CMD_TASK_QUERY_TASKINFO_SC()
	l_taskInfo:ParseFromString(pkg)
    paixu()
	Log("Begin LookTaskInfo---====="..tostring(l_taskInfo))
    local widget = UI_GetBaseWidgetByName("TaskSystem")
	if widget then
	   Log("Refresh LookTaskInfo--------------------")
	   ShowTaskInfo(widget)
	   
	    if (g_isGuide and g_curGuideId == 12 and g_curGuideStep == 2) then
			Guide_GoNext();
	    end
        if returnPlayerlevelupdateinfo() ~=nil then
		   Log("HeroLevelUpInfoShow-------------------------------------------");
           HeroLevelUpInfoShow()
        end
    else
		
		 Log("create---------------------------")
		 
		 
		 if  returnPlayerlevelupdateinfo() ~=nil then
			Log("Fuck!!!! HeroLevelUpInfoShow-------------------------------------------");
			HeroLevelUpInfoShow()
			
		 else
			InitTaskLayout()
		 end	   
    end
end

function HaveNewTask(pkg)
    -- body
    --createPromptBoxlayout("有新任务开启")
    Log("xxxxx")
    if SceneMgr:GetInstance():GetGameState() ~= EGameState_LoadScene and SceneMgr:GetInstance():GetGameState() ~= EGameState_Fighting then
        CreateHaveNewTask()
    else
       g_havenewtaskIsShow = true
    end
end

--有新任务提示 挡loading 和战斗界面
if g_havenewtaskIsShow == nil then
    g_havenewtaskIsShow = false
end
local function HaveNewTask_AnimEnd(armature, movementType, movementID)
    -- body
    if (movementType == LOOP_COMPLETE)then
        g_havenewtaskIsShow = false
       MainScene:GetInstance():removeChildByTag(EMSTag_HaveNewTask)
    end
end
function CreateHaveNewTask()
    -- body
    if MainScene:GetInstance():getChildByTag(EMSTag_HaveNewTask) ~= nil then
        return 
    end
    if SceneMgr:GetInstance():GetGameState() == EGameState_LoadScene or SceneMgr:GetInstance():GetGameState() == EGameState_Fighting then
       g_havenewtaskIsShow = true
    end
    local haveNewtaskAnimation = GetUIArmature("Effxinrenwu")
    local layout = Layout:create()
    layout:setPosition(ccp(480,320))
    layout:addNode(haveNewtaskAnimation)
    haveNewtaskAnimation:getAnimation():playWithIndex(0)
    haveNewtaskAnimation:getAnimation():setMovementEventCallFunc(HaveNewTask_AnimEnd)
    MainScene:GetInstance():AddChild(layout, EMSTag_HaveNewTask, EMSTag_HaveNewTask,false)
end

ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_TASK_QUERY_TASKINFO, "LookTaskInfo" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_TASKSYSTEM_NEWTASKOPEN, "HaveNewTask" )

--有任务完成

function createAccomplishTask()
    -- body
    if g_noticeFlagIsShow == false then
        return 
    end
    if SceneMgr:GetInstance():GetGameState() == EGameState_LoadScene or SceneMgr:GetInstance():GetGameState() == EGameState_Fighting then
       g_AccomplishTaskCanShow = true
    end
    if UI_HasBaseWidget("DungeonList") or UI_HasBaseWidget("Dungeon") or UI_HasBaseWidget("TaskSystem") or UI_HasBaseWidget("Shop") then
       return 
    end
    local widget = GUIReader:shareReader():widgetFromJsonFile("AccomplishTask.json")
    --return button 
    local function CloseAccomplishTaskLayoutFunc(sender,eventType)
        -- body
        if eventType == TOUCH_EVENT_ENDED then
            g_AccomplishTaskCanShow = false
            MainScene:GetInstance():removeChildByTag(EMSTag_AccomplishTask,true)
        end
    end
    UI_GetUIButton(widget,1):addTouchEventListener(CloseAccomplishTaskLayoutFunc)

    --lingjiang
    local function RewardAccomplishTaskFunc(sender,eventType)
        -- body
        if eventType == TOUCH_EVENT_ENDED then
            g_AccomplishTaskCanShow = false
            MainScene:GetInstance():removeChildByTag(EMSTag_AccomplishTask,true)
            Packet_Cmd(GameServer_pb.CMD_TASK_QUERY_TASKINFO)
           ShowWaiting()
        end
    end
    UI_GetUIButton(widget,2):addTouchEventListener(RewardAccomplishTaskFunc)

    MainScene:GetInstance():AddChild(widget, EMSTag_AccomplishTask, EMSTag_AccomplishTask , true);
end

--任务完成提示框 挡战斗 loading界面变量
if g_AccomplishTaskCanShow == nil then
    g_AccomplishTaskCanShow = false
end

function TaskHasAccomplish(pkg)
    -- body
    Log("shenm qingkuang")
    if g_openUIType == EUIOpenType_Dugeon or g_curGuideId then
       return
    end
    if SceneMgr:GetInstance():GetGameState() ~= EGameState_LoadScene and SceneMgr:GetInstance():GetGameState() ~= EGameState_Fighting then
        createAccomplishTask()
    else
        g_AccomplishTaskCanShow = true
    end
end

ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_TASKSYSTEM_NEWTASKFINISH, "TaskHasAccomplish" )
