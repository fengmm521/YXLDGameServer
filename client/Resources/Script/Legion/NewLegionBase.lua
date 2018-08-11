--                            _ooOoo_  
--                           o8888888o  
--                           88" . "88  
--                           (| -_- |)  
--                            O\ = /O  
--                        ____/`---'\____  
--                      .   ' \\| |// `.  
--                       / \\||| : |||// \  
--                     / _||||| -:- |||||- \  
--                       | | \\\ - /// | |  
--                     | \_| ''\---/'' | |  
--                      \ .-\__ `-` ___/-. /  
--                   ___`. .' /--.--\ `. . __  
--                ."" '< `.___\_<|>_/___.' >'"".  
--               | | : `- \`.;`\ _ /`;.`/ - ` : | |  
--                 \ \ `-. \_ __\ /__ _/ .-` / /  
--         ======`-.____`-.___\_____/___.-`____.-'======  
--                            `=---='  
--  
--         .............................................  
--                  佛祖镇楼                  BUG辟易 

--begin yjj 20140926
--[[" 
      军团建筑重写 Begin
"]]
--玩家的职位 
if g_NewLegionBase_PlayerPosition == nil then 
   g_NewLegionBase_PlayerPosition = 0
end 

--图标ID
if g_NewLegionBase_ImageIcon == nil then 
   g_NewLegionBase_ImageIcon = 0
end 

--军团名字 
if g_NewLegionBase_LegionName == nil then 
   g_NewLegionBase_LegionName = ""
end 

----军团宣言 
if g_NewLegionBase_LegionBroadcat == nil then 
   g_NewLegionBase_LegionBroadcat = ""
end 
if g_NewLegionBase_LeionAnnouce == nil then 
   g_NewLegionBase_LeionAnnouce = ""
end 

function NewLegionBase_InitValue(info)
   g_NewLegionBase_PlayerPosition = info.baseInfo.iOccupation
   g_NewLegionBase_ImageIcon = info.baseInfo.iLegionIconId
   g_NewLegionBase_LegionName = info.baseInfo.strLegionName
   g_NewLegionBase_LegionBroadcat = info.baseInfo.strBroadcat
   g_NewLegionBase_LeionAnnouce = info.baseInfo.strAnnouce
end 

function NewLegionBase_Refresh(widget,info)
   NewLegionBase_InitValue(info)

   NewLegionBase_ShowCurFunctionByPosition(widget,info)
   NewLegionBase_ShowMyLegionBaseInfo(widget,info)
   NewLegionBase_ShowMyLegionBuildingFunction(widget)
   NewLegionBase_ShowLegionData(widget)
end

function NewLegionBase_Refresh_CS()
   Packet_Cmd(GameServer_pb.CMD_CLICK_LEGION)
   ShowWaiting()
end

function NewLegionBase_ShowMyLegionBaseLayout(info)
   local widget = UI_CreateBaseWidgetByFileName("LegionDetails.json",EUIOpenAction_FadeIn,0.5)

   UI_GetUIButton(widget, 1):addTouchEventListener(UI_ClickCloseAllBaseWidget) 
   --init 
   NewLegionBase_InitValue(info)

   NewLegionBase_ShowCurFunctionByPosition(widget,info)
   NewLegionBase_ShowMyLegionBaseInfo(widget,info)
   NewLegionBase_ShowMyLegionBuildingFunction(widget)
   NewLegionBase_ShowLegionData(widget)
end

function NewLegionBase_ShowLegionData(widget)
   local button = UI_GetUIButton(UI_GetUILayout(UI_GetUILayout(widget,2),1),27)
   local function LookLegionBaseData(sender,eventType)
      if eventType == TOUCH_EVENT_BEGAN then 
         NewLegionBase_LegionBaseLayout()
      end 
   end 
   button:addTouchEventListener(LookLegionBaseData)
end

function NewLegionBase_LegionBaseLayout()
--   local widget = GetWidgetFromFile("LegionBaseData.json")
--   local layout = UI_GetCloneImageView(UI_GetUIImageView(widget,1))
--   return layout
     local widget = UI_CreateBaseWidgetByFileName("LegionBaseData.json")
     
end


function NewLegionBase_ShowMyLegionBaseInfo(widget,info)
   local parentlayout = UI_GetUILayout(widget, 2)
   local baseinfolayout = UI_GetUILayout(parentlayout, 1)

   --legin name 
   UI_SetLabelText(baseinfolayout, 2, info.baseInfo.strLegionName)
   --number 
   UI_SetLabelText(baseinfolayout, 3, "( "..info.baseInfo.iMemCount.."/"..info.baseInfo.iMemLimit.." )")
   --rank 
   UI_SetLabelText(baseinfolayout, 4, FormatString("LegionList_Rank",info.baseInfo.iRank))
   --leader name 
   UI_SetLabelText(baseinfolayout, 5, FormatString("LegionList_HeadName",info.baseInfo.strLeaderName))
   --lvl 
   UI_SetLabelText(baseinfolayout, 6, info.baseInfo.iLevel)
   --legion exp loadbar
   UI_GetUILoadingBar(baseinfolayout, 7):setPercent(info.baseInfo.iExp / info.baseInfo.iLevelUpExp * 100)
   --exp 
   UI_SetLabelText(baseinfolayout, 9, info.baseInfo.iExp.."/"..info.baseInfo.iLevelUpExp)
   --image 
   local imagestr = NewLegion_GetImageView(info.baseInfo.iLegionIconId)
   UI_GetUIImageView(baseinfolayout,8):loadTexture(imagestr)
   --announcement 
   NewLegionBase_ShowMyLegionAnnouceMent(widget,info.baseInfo.strBroadcat)
   --zijing
   UI_SetLabelText(baseinfolayout,26,info.baseInfo.iLegionMoney)
end 

function NewLegionBase_ShowMyLegionAnnouceMent(widget,announcementStr)
   local parentlayout = UI_GetUILayout(widget, 2)
   local baseinfolayout = UI_GetUILayout(parentlayout, 1)
   local announcementlayout = UI_GetUILayout(baseinfolayout, 1)
   UI_SetLabelText(announcementlayout, 1, announcementStr)
end 

function NewLegionBase_ClickLegionFunction(tag)
   -- 1. 膜拜大神 2.领取俸禄 3.军团商店 4.领地争夺 5. 军团技能
    if tag == 1 then 
       LegionWorship_createMine()
    elseif tag == 2 then 
       LegionSalary_Create()
    elseif tag == 3 then 
       LegionShopLayout_Create()
    elseif tag == 4 then 
       createPromptBoxlayout(FormatString("FunctionNotOpen"))
    elseif tag == 5 then
       createPromptBoxlayout(FormatString("FunctionNotOpen"))
    elseif tag == 6 then
       createPromptBoxlayout(FormatString("FunctionNotOpen"))
    end 
end

function NewLegionBase_ShowMyLegionBuildingFunction(widget,info)
   local parentlayout = UI_GetUILayout(widget, 2)
   local buildinglayout = UI_GetUILayout(parentlayout, 2)
   local listview = UI_GetUIListView(parentlayout,4)
   listview:removeAllItems()
   local width = listview:getContentSize().width
   local height = listview:getContentSize().height
   local layout = nil 
   local funTag = {1,2,3,4,5,6}
   local function touchLegionFunc(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then 
         local sender = tolua.cast(sender,"ImageView")
         local tag = listview:getIndex(sender) + 1
         Log("tag========="..tag)
         NewLegionBase_ClickLegionFunction(tag)
      end 
   end
   for i = 1,#funTag do 
      Log("i======"..i)
      layout = UI_GetCloneImageView(UI_GetUIImageView(buildinglayout,funTag[i]))
      layout:setVisible(true)
      layout:setTouchEnabled(true)
      layout:addTouchEventListener(touchLegionFunc)
      listview:pushBackCustomItem(layout)
   end 
end 

function NewLegionBase_ShowCurFunctionByPosition(widget)
   local parentlayout = UI_GetUILayout(widget, 2)
   local function clickLookupLegionNumberFunc(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then 
         LegionMemberList_Create()
      end 
   end 
   UI_GetUIButton(parentlayout, 3):addTouchEventListener(clickLookupLegionNumberFunc)

   if g_NewLegionBase_PlayerPosition == GameServer_pb.en_LegionOccupation_Leader then
       UI_GetUILayout(widget, 4):setVisible(true)
       UI_GetUILayout(widget, 3):setVisible(false)
   elseif g_NewLegionBase_PlayerPosition == GameServer_pb.en_LegionOccupation_ViceLeader then
       UI_GetUILayout(widget, 4):setVisible(true)
       UI_GetUILayout(widget, 3):setVisible(true) 
   elseif g_NewLegionBase_PlayerPosition == GameServer_pb.en_LegionOccupation_Member then 
       UI_GetUILayout(widget, 4):setVisible(false)
       UI_GetUILayout(widget, 3):setVisible(true)
   elseif g_NewLegionBase_PlayerPosition == GameServer_pb.en_LegionOccupation_Newer then 
       UI_GetUILayout(widget, 4):setVisible(false)
       UI_GetUILayout(widget, 3):setVisible(true)
   end 
   local function legionManageFunc(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then 
--         if g_NewLegionBase_PlayerPosition == GameServer_pb.en_LegionOccupation_Leader then

--         else 

--         end 
          NewLegionBase_ManageLayout(g_NewLegionBase_PlayerPosition)
      end 
   end 
   UI_GetUIButton(UI_GetUILayout(widget, 4), 1):addTouchEventListener(legionManageFunc)
   local function legionExitFunc(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then 
         
      end 
   end 
   UI_GetUIButton(UI_GetUILayout(widget, 3), 1):addTouchEventListener(legionExitFunc)

   local function BesureleaveLegion()
    	-- body
    	Log("LeaveLegion")
        Packet_Cmd(GameServer_pb.CMD_LEAVE_LEGION)    
        ShowWaiting()
    end
    local function Leavelegion(sender,eventType)
    	-- body
     	if eventType == TOUCH_EVENT_ENDED then
     	   local stringname  = FormatString("LegionInfo_Leave",g_NewLegionBase_LegionName)
    	   Messagebox_Create({info = stringname, msgType = EMessageType_LeftRight, leftFun = BesureleaveLegion})
        end 
    end
    UI_GetUIButton(UI_GetUILayout(widget,3),1):addTouchEventListener(Leavelegion)

   -- local function xuanshanglingFunc(sender,eventType)
   --   if eventType == TOUCH_EVENT_ENDED then 
   --     createPromptBoxlayout(FormatString("FunctionNotOpen"))
   --   end 
   --end
   --UI_GetUIButton(parentlayout,5):addTouchEventListener(xuanshanglingFunc)

   local function LegionBuildingFunc(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then 
         LegionBuildingLayout_Create()
      end 
   end
   UI_GetUIButton(parentlayout,6):addTouchEventListener(LegionBuildingFunc)
end 

--[[" 
      军团建筑重写 Begin
"]]

--[[" 
      军团管理 Begin
"]]

function NewLegionBase_LegionManageButton(isLeader)
   local widget = UI_CreateBaseWidgetByFileName("NewLegionManage.json", EUIOpenAction_FadeIn,0.5)
   if isLeader then 
      UI_GetUILayout(widget, 1):setVisible(true)
      UI_GetUILayout(widget, 2):setVisible(false)
   else 
      UI_GetUILayout(widget, 1):setVisible(false)
      UI_GetUILayout(widget, 2):setVisible(true)
   end 

   --applay
   local function applayFunc(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then 
         
      end 
   end
   UI_GetUIButton(UI_GetUILayout(widget, 1), 1):addTouchEventListener(applayFunc)
   UI_GetUIButton(UI_GetUILayout(widget, 2), 1):addTouchEventListener(applayFunc)

   --revamp announcement 
   local function revampAnnouncement(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then
       
      end 
   end 
   UI_GetUIButton(UI_GetUILayout(widget, 1), 2):addTouchEventListener(revampAnnouncement)
   UI_GetUIButton(UI_GetUILayout(widget, 2), 2):addTouchEventListener(revampAnnouncement)

   --dissolveLegion
   local function disolveLegionFunc(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then 
         
      end 
   end 
   UI_GetUIButton(UI_GetUILayout(widget, 1), 3):addTouchEventListener(disolveLegionFunc)

   --exit legion 
   local function exitLegionFunc(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then 
         
      end 
   end 
   UI_GetUIButton(UI_GetUILayout(widget, 2), 3):addTouchEventListener(exitLegionFunc)
end 

--[[" 
      军团管理 End
"]]

--[[" 
      军团申请处理 Begin
"]]



--[[" 
      军团申请处理 End
"]]


--[[" 
      军团成员列表 Begin
"]]

function NewLegionNumberList_Show(info)
   local widget = UI_CreateBaseWidgetByFileName("NewLegionNumberList.json", EUIOpenAction_FadeIn,0.5)
   UI_GetUIButton(widget, 1):addTouchEventListener(UI_ClickCloseCurBaseWidget)

   NewLegionNumberList_NumberListShow(widget,info)
end 

function NewLegionNumberList_NumberListShow(widget,info)
   local parentlayout = UI_GetUILayout(widget, 3)
   local numberlist = UI_GetUIListView(parentlayout, 1)
   numberlist:removeAllItems()
   local width = numberlist:getContentSize().width
   local height = numberlist:getContentSize().height

   local layout = nil 
   local function createNumberTemp(index)
      local imageparent = UI_GetUIImageView(layout, 1)
      --name 
      UI_SetLabelText(imageparent, 1, info.szMemberList[index].strMemName)
      --position
      local positionname = nil 
      if info.szMemberList[index].iOccupation == GameServer_pb.en_LegionOccupation_Leader then
            --帮主
         positionname = FormatString("LegionInfo_Leader")
      elseif info.szMemberList[index].iOccupation == GameServer_pb.en_LegionOccupation_ViceLeader then
            --副帮主
         positionname = FormatString("LegionInfo_DeputyHead")
      elseif info.szMemberList[index].iOccupation == GameServer_pb.en_LegionOccupation_Member then
            --团员
         positionname = FormatString("LegionInfo_Member")
      elseif info.szMemberList[index].iOccupation == GameServer_pb.en_LegionOccupation_Newer  then
            --见习
         positionname = FormatString("LegionInfo_Novitiate")
      end
      UI_SetLabelText(imageparent, 2, positionname)

      --contribution rank
      UI_SetLabelText(imageparent, 3, info.szMemberList[index].iContributeRank)

      --rank
      UI_SetLabelText(imageparent, 4, info.szMemberList[index].iArenaRank)

      --Daily contribution
      UI_SetLabelText(imageparent, 5, info.szMemberList[index].iDayContribute)

      --total contribution
      UI_SetLabelText(imageparent,6,info.szMemberList[index].iTotalContribute)
      --lvel 
      UI_GetUILabelBMFont(imageparent, 7):setText(info.szMemberList[index].iLevel)
      --image 

   end 
   for i = 1,#info.szMemberList do 
      layout = UI_GetCloneLayout(UI_GetUILayout(widget, 2))
      layout:setVisible(true)
      createNumberTemp(i)
      numberlist:pushBackCustomItem(layout)
   end 
end 

--[[" 
      军团成员列表 End
"]]

--[[" 
      修改军团设置 Begin
"]]

--军团管理
function NewLegionBase_ManageLayout(playerPosition)
   local widget = UI_CreateBaseWidgetByFileName("LegionManage.json")
   UI_GetUILayout(widget, 4):addTouchEventListener(PublicCallBackWithNothingToDo)
   --是否有入团请求
    NoticeFunction_UpdateLegionHaveRequest()
   --UI_GetUILayout()
   local parentImage = UI_GetUIImageView(widget,1)
   if playerPosition == GameServer_pb.en_LegionOccupation_Leader then 
      UI_GetUIButton(parentImage,3):setTouchEnabled(true)
   else 
      UI_GetUIButton(parentImage,3):setTouchEnabled(false)
   end 
   
   --申请处理
   local function  Application(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then 
         UI_CloseCurBaseWidget(EUICloseAction_None)
         Packet_Cmd(GameServer_pb.CMD_QUERY_LEGION_APP)
         ShowWaiting()
      end 
   end
   UI_GetUIButton(parentImage,1):addTouchEventListener(Application)

   --军团设置
   local function LegionSetting(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then 
         UI_CloseCurBaseWidget(EUICloseAction_None)
         NewLegionSetting_Layout()
      end 
   end
   UI_GetUIButton(parentImage,2):addTouchEventListener(LegionSetting)

   --解散军团
   local function BesureDisbandedlegion()
    	-- body
    	Log("disbandedlegion")
        Packet_Cmd(GameServer_pb.CMD_DISMISS_LEGION)
        ShowWaiting()
    end
   local function LegionDissolve(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then 
         UI_CloseCurBaseWidget(EUICloseAction_None)
         local stringname  = FormatString("LegionInfo_Disbanded",g_NewLegionBase_LegionName)
    	 Messagebox_Create({info = FormatString("LegionInfo_BesureDisbanded"),title = stringname, msgType = EMessageType_LeftRight, leftFun = BesureDisbandedlegion}) 
      end 
   end
   UI_GetUIButton(parentImage,3):addTouchEventListener(LegionDissolve)
   UI_GetUIButton(widget,2):addTouchEventListener(UI_ClickCloseCurBaseWidget)
end

function NewLegionBase_RevampAnnoucement()
   -- body
	local widget = UI_CreateBaseWidgetByFileName("LegionSetup.json")

	--return 
	UI_GetUIButton(widget,1):addTouchEventListener(UI_ClickCloseCurBaseWidget)
    UI_GetUILayout(widget,50):addTouchEventListener(PublicCallBackWithNothingToDo)
    --create Bt
    local function ClickCreateLegion(sender,eventType)
    	-- body
    	Log("create")
        if eventType == TOUCH_EVENT_ENDED then
        	local strAnnouce = UI_GetUITextField(widget,3)
        	local strAnnouceString = strAnnouce:getStringValue()

        	local strBroadcast = UI_GetUITextField(widget,2)
        	local strBroadcastString = strBroadcast:getStringValue()

        	local tab = GameServer_pb.CMD_MODIFY_LEGION_SETTING_CS()
        	tab.strAnnouce = strAnnouceString
        	tab.strBroadcast = strBroadcastString
        	Packet_Full(GameServer_pb.CMD_MODIFY_LEGION_SETTING, tab)
        	ShowWaiting()
        end
    end
    local createBt = UI_GetUIButton(widget,4)
    createBt:addTouchEventListener(ClickCreateLegion)
end 

--[[" 
      修改军团设置 End 
"]]


--[[" 
      修改设置 begin 
"]]

function NewLegionSetting_Layout()
   Log("xxxxxxxxxxxxxxx")
   local widget = UI_CreateBaseWidgetByFileName("LegionSetup.json")
   Log("xxxxxxxxxxxxxxx")
	--return 
	local returnBt = UI_GetUIButton(widget,1)
    returnBt:addTouchEventListener(UI_ClickCloseCurBaseWidget)
    UI_GetUILayout(widget,50):addTouchEventListener(PublicCallBackWithNothingToDo)
    --图标
    local str = NewLegion_GetImageView(g_NewLegionBase_ImageIcon)
    UI_GetUIImageView(widget,5):loadTexture(str)
    local function ChangeLegionIcon(sender,eventType)
       if eventType == TOUCH_EVENT_ENDED then 
          LegionCreateNewLegion_ImageList(NewLegionSetting_ChangeImage)
       end 
    end
    UI_GetUIButton(widget,6):addTouchEventListener(ChangeLegionIcon)

    --初始化
    UI_GetUITextField(widget,2):setText(g_NewLegionBase_LeionAnnouce)
    UI_GetUITextField(widget,3):setText(g_NewLegionBase_LegionBroadcat)

    local function BeSureChange(sender,eventType)
       if eventType == TOUCH_EVENT_ENDED then 
          local strAnnouce = UI_GetUITextField(widget,2)
          local strAnnouceString = nil
            if strAnnouce:getStringValue() == "" then 
               strAnnouceString = g_NewLegionBase_LeionAnnouce
            else 
               strAnnouceString = strAnnouce:getStringValue()
               g_NewLegionBase_LeionAnnouce = strAnnouceString
            end 
        	

        	local strBroadcast = UI_GetUITextField(widget,3)
            local strBroadcastString = nil 
            --if strBroadcast:getStringValue() == "" then 
            --   strBroadcastString = g_NewLegionBase_LegionBroadcat
            --else 
              strBroadcastString = strBroadcast:getStringValue()
              g_NewLegionBase_LegionBroadcat = strBroadcastString
            --end 
        

        	local tab = GameServer_pb.CMD_MODIFY_LEGION_SETTING_CS()
        	tab.strAnnouce = strAnnouceString
        	tab.strBroadcast = strBroadcastString
            tab.iLegionIconId = g_NewLegionBase_ImageIcon
        	Packet_Full(GameServer_pb.CMD_MODIFY_LEGION_SETTING, tab)
        	ShowWaiting()
       end 
    end
    UI_GetUIButton(widget,4):addTouchEventListener(BeSureChange)
end

function NewLegionSetting_ChangeImage(id)
   local widget = UI_GetBaseWidgetByName("LegionSetup")
   if widget == nil then
        return 
   end
   g_NewLegionBase_ImageIcon = id
   Log("-------------------------id========="..id)
   local str = NewLegion_GetImageView(g_NewLegionBase_ImageIcon)
   UI_GetUIImageView(widget,5):loadTexture(str)
end

--[[" 
      修改设置 end 
"]]

