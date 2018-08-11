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
--Begin yjj 20140925
--[[" 
      军团列表重写 Begin
"]]

--动态加载军团列表，在滑动到底的时候与服务器通讯，这里为了防止多次加载
if g_NewLegionList_GetLegionListSeverIsReturn == nil then 
   g_NewLegionList_GetLegionListSeverIsReturn = true
end

--记录当前是否第一次进入列表
if g_NewLegionList_IsFistInLegionList == nil then 
   g_NewLegionList_IsFistInLegionList = true 
end

--记录当前列表中的所有军团的ObjectID
if g_NewLegionList_LegionObjectID == nil then 
   g_NewLegionList_LegionObjectID = {} 
end 

function NewLegionList_InitValue() 
   g_NewLegionList_GetLegionListSeverIsReturn = true
   g_NewLegionList_IsFistInLegionList = true 
   g_NewLegionList_LegionObjectID = {}
end  


function NewLegionList_ShowHeroList(info)
   local widget = UI_CreateBaseWidgetByFileName("Legion.json",EUIOpenAction_FadeIn,0.5)
   UI_GetUIButton(widget, 1):addTouchEventListener(UI_ClickCloseCurBaseWidget)

   NewLegionList_InitValue()
   --create legion 
   local function CreatelegionFunc(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then 
         LegionCreateNewLegion()
      end 
   end 
   UI_GetUIButton(widget, 3):addTouchEventListener(CreatelegionFunc)
   NewLegionList_ListInitShow(widget,info)
end 

function NewLegionList_createlayoutTemp(layout,legionInfo)
   table.insert(g_NewLegionList_LegionObjectID,legionInfo.dwLegionObjectID)
   --rank
   local imageparent = UI_GetUIImageView(layout,1)
   NewLegion_RankImageFont(UI_GetUIImageView(imageparent,7),UI_GetUILabelBMFont(imageparent, 6),legionInfo.iRank)
   --image 
   --
   local imagestr = NewLegion_GetImageView(legionInfo.iLegionIconId)
   UI_GetUIImageView(imageparent,5):loadTexture(imagestr)
   --legion name
   UI_SetLabelText(imageparent,1 ,legionInfo.strLegionName)
   --number 
   UI_SetLabelText(imageparent, 2, "( "..legionInfo.iMemCount.."/"..legionInfo.iMemLimit.." )")
   --legionleader name
   UI_SetLabelText(imageparent, 3, FormatString("LegionList_HeadName",legionInfo.strLeaderName))
   --legion level
   UI_SetLabelText(imageparent, 4, FormatString("LegionBuilding_LvL",legionInfo.iLevel))
   --buttonState
   if legionInfo.bApplyed then 
      UI_GetUIButton(layout, 2):setTitleText(FormatString("LegionList_AlreadyApplication"))
   else 
      UI_GetUIButton(layout, 2):setTitleText(FormatString("LegionList_Application"))
   end
   local function applicationFunc(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then
         if UI_GetUIButton(layout, 2):getTitleText() == FormatString("LegionList_AlreadyApplication") then 
            --取消
            Log("quxiao")
            local tab = GameServer_pb.CMD_CANCEL_LEGIONAPP_CS()
            tab.dwLegionObjectID = legionInfo.dwLegionObjectID
            Packet_Full(GameServer_pb.CMD_CANCEL_LEGIONAPP, tab)
            ShowWaiting()
         else 
            --申请
            Log("applay")
            local tab = GameServer_pb.CMD_APPJOIN_LEGION_CS()
            tab.dwLegionObjectID = legionInfo.dwLegionObjectID
            Packet_Full(GameServer_pb.CMD_APPJOIN_LEGION, tab)
            ShowWaiting()
         end 
      end 
   end 
   UI_GetUIButton(layout, 2):addTouchEventListener(applicationFunc)
end 


function NewLegionList_ListInitShow(widget,info)
   local legionlist = UI_GetUIListView(UI_GetUIImageView(widget, 2), 1)
   legionlist:removeAllItems()
   local width = legionlist:getContentSize().width 
   local height = legionlist:getContentSize().height 
   local templayout = Layout:create()
   templayout:setSize(CCSizeMake(UI_GetUILayout(widget, 7):getContentSize().width , UI_GetUILayout(widget, 7):getContentSize().height))
   legionlist:setItemModel(templayout)

   local layout = nil 
   local function lookLegionInfo(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then
         local sender = tolua.cast(sender,"Layout")
         local tag = legionlist:getIndex(sender)
         Log("tag==="..tag)
         local tab = GameServer_pb.CMD_QUERY_OTHERLEGION_CS()
         tab.dwLegionObjectID = info.szLegionList[tag + 1].dwLegionObjectID
         Packet_Full(GameServer_pb.CMD_QUERY_OTHERLEGION, tab)
         ShowWaiting()
      end 
   end 
   for i = 1,#info.szLegionList do 
      layout = UI_GetCloneLayout(UI_GetUILayout(widget, 7))
      layout:setVisible(true)
      layout:setTouchEnabled(true)
      layout:addTouchEventListener(lookLegionInfo)
      NewLegionList_createlayoutTemp(layout,info.szLegionList[i])
      legionlist:pushBackCustomItem(layout)
   end
   --动态加载
   NewLegionList_UpdateLegionListBySever(widget,info.iPageIndex,info.iTotalPages)
end 

--动态刷新list
function NewLegionList_UpdateLegionListBySever(widget,curpage,totalpage)
   local legionlist = UI_GetUIListView(UI_GetUIImageView(widget, 2), 1)
   local function getLeginListInfo(sender,eventType)
      if eventType == SCROLLVIEW_EVENT_SCROLL_TO_BOTTOM then 
         if g_NewLegionList_GetLegionListSeverIsReturn  then 
             if g_NewLegionList_IsFistInLegionList == false then                  
                if curpage + 1 < totalpage then
                   local tab = GameServer_pb.CMD_QUERY_LEGIONLIST_CS()
                   tab.iPageIndex = curpage + 1
                   Packet_Full(GameServer_pb.CMD_QUERY_LEGIONLIST, tab)
                   g_NewLegionList_GetLegionListSeverIsReturn = false 
                   ShowWaiting()
                end
             else 
                g_NewLegionList_IsFistInLegionList = false  
             end 
         end 
      end 
   end 
   legionlist:addEventListenerScrollView(getLeginListInfo)
end 

function NewLegionList_UpdateLegionList(widget,info)
   local legionlist = UI_GetUIListView(UI_GetUIImageView(widget, 2), 1)
   local layout = nil 
   local function lookLegionInfo(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then
         local sender = tolua.cast(sender,"Layout")
         local tag = legionlist:getIndex(sender)
         Log("tag==="..tag)
         local tab = GameServer_pb.CMD_QUERY_OTHERLEGION_CS()
         local index = (tag+1)%10
         if index == 0 then
            index = 10
         end
         tab.dwLegionObjectID = info.szLegionList[index].dwLegionObjectID
         Packet_Full(GameServer_pb.CMD_QUERY_OTHERLEGION, tab)
         ShowWaiting()
      end 
   end 
   for i = 1,#info.szLegionList do 
      layout = UI_GetCloneLayout(UI_GetUILayout(widget, 7))
      layout:setVisible(true)
      layout:setTouchEnabled(true)
      layout:addTouchEventListener(lookLegionInfo)
      NewLegionList_createlayoutTemp(layout,info.szLegionList[i])
      legionlist:pushBackCustomItem(layout)
   end 
   --动态加载
   NewLegionList_UpdateLegionListBySever(widget,info.iPageIndex,info.iTotalPages)
end 

--查找当前点击的军团的Object
function NewLegionList_FindApplicationIndexByObjectId(objectId)
   for i = 1 ,#g_NewLegionList_LegionObjectID do 
      if objectId == g_NewLegionList_LegionObjectID[i] then 
         return i 
      end 
   end 
   return nil 
end 

function NewLegionList_UpdateApplicationBt(info,isApplication)
   local tag = NewLegionList_FindApplicationIndexByObjectId(info.dwLegionObjectID)
   local widget = UI_GetBaseWidgetByName("Legion")
   if widget == nil then
      return   
   end 
   local legionlist = UI_GetUIListView(UI_GetUIImageView(widget, 2), 1) 
   if tag ~= nil then 
       local layout = legionlist:getItem(tag - 1)
       if isApplication then 
          UI_GetUIButton(layout, 2):setTitleText(FormatString("LegionList_Application"))
       else 
          UI_GetUIButton(layout, 2):setTitleText(FormatString("LegionList_AlreadyApplication")) 
       end 
   end 
end 

--[[" 
      军团列表重写 End
"]]


--[[" 
      军团创建重写 Begin
"]]

function NewCreateLegion_CreateLegionShow()
   local widget = UI_CreateBaseWidgetByFileName("CreatingLegionBase.json",EUIOpenAction_FadeIn,0.5)
   UI_GetUIButton(widget, 1):addTouchEventListener(UI_ClickCloseCurBaseWidget)
   --cost 
   UI_SetLabelText(UI_GetUIImageView(widget, 5),1,UI_GetMoneyStr(g_CreatingLegionSliver))
   --LegionName
   local legionName = UI_GetUITextField(UI_GetUIImageView(widget, 4),1)
   legionName:setMaxLength(UI_GetUIImageView(widget, 4):getContentSize().width)

   --createBt
   local function ClickCreateLegion(sender,eventType)
    	-- body
       if eventType == TOUCH_EVENT_ENDED then
           local LegionName = UI_GetUITextField(UI_GetUIImageView(widget, 4),1)
           local LegionNamString = LegionName:getStringValue()
           if LegionNamString ~= "" then
              local tab = GameServer_pb.CMD_CREATE_LEGION_CS()
              tab.strLegionName = LegionNamString
              Packet_Full(GameServer_pb.CMD_CREATE_LEGION, tab)
           else
           	   Log("message")
           	   Messagebox_Create({info = FormatString("LegionList_LegionNameTempty"), msgType = EMessageType_LeftRight})
           end
        end
    end
    UI_GetUIButton(widget,3):addTouchEventListener(ClickCreateLegion)
end 

--[[" 
      军团创建重写 End
"]]

--[[" 
      查看军团信息重写 Begin
"]]

function NewLegionInfo_LookOtherLegionInfo(info)
   local widget = UI_CreateBaseWidgetByFileName("LegionListInfoView.json", EUIOpenAction_FadeIn,0.5)
   UI_GetUIButton(widget, 2):addTouchEventListener(UI_ClickCloseCurBaseWidget)

   local infoparent = UI_GetUILayout(widget, 1)
   --legion name
   UI_SetLabelText(infoparent, 1, info.otherLegionInfo.strLegionName)
   --legion leader name 
   UI_SetLabelText(infoparent, 2, FormatString("LegionList_HeadName",info.otherLegionInfo.strLeaderName))
   --level 
   UI_SetLabelText(infoparent, 3, FormatString("LegionBuilding_LvL",info.otherLegionInfo.iLevel))
   --rank 
   UI_SetLabelText(infoparent, 4, FormatString("LegionList_Rank",info.otherLegionInfo.iRank))
   --number 
   UI_SetLabelText(infoparent, 5,  FormatString("LegionList_NumbersPercent",info.otherLegionInfo.iMemCount,info.otherLegionInfo.iMemLimit))
   --Declaration
   UI_SetLabelText(infoparent, 6, info.otherLegionInfo.info)
end 

--[[" 
      查看军团信息重写 End
"]]

--end 20140925


--[[" 
      军团列表查看 Begin
"]]

function NewLegionList_InfoViewlayout(info)
   local widget = UI_CreateBaseWidgetByFileName("LegionListInfoView.json")
   local returnBt = UI_GetUIButton(widget,2)
   returnBt:addTouchEventListener(UI_ClickCloseCurBaseWidget)

   NewLegionList_InfoViewlayout_CurInfo(widget,info)
end

function NewLegionList_InfoViewlayout_CurInfo(widget,info)
   local LegionListTipViewParent = UI_GetUILayout(widget,1)

	--LegionName
	local LegionName = info.otherLegionInfo.strLegionName
	UI_SetLabelText(LegionListTipViewParent,1,LegionName)
	--LegionHeadName
	local LegionHeadName = FormatString("LegionList_HeadName",info.otherLegionInfo.strLeaderName)
	UI_SetLabelText(LegionListTipViewParent,2,LegionHeadName)

	--LegionLvL
	local LegionLvL = FormatString("LegionList_LvL",info.otherLegionInfo.iLevel)
	UI_SetLabelText(LegionListTipViewParent,3,LegionLvL)

	--LegionRank
	local LegionRank = FormatString("LegionList_Rank",info.otherLegionInfo.iRank)
	UI_SetLabelText(LegionListTipViewParent,4,LegionRank)

	--LegionNumbersNum
	local LegionNumbersNum = FormatString("LegionList_NumbersPercent",info.otherLegionInfo.iMemCount,info.otherLegionInfo.iMemLimit)
	UI_SetLabelText(LegionListTipViewParent,5,LegionNumbersNum)

	--Declaration
	local scrollview = UI_GetUIScrollView(LegionListTipViewParent,6)
	scrollview:removeAllChildren()

	local width = scrollview:getSize().width

	local Declaration = CompLabel:GetDefaultCompLabel(info.otherLegionInfo.strAnnouce,width)
	Declaration:setAnchorPoint(ccp(0,0))
    
	local height = Declaration:getSize().height

	if height < scrollview:getSize().height then
		height = scrollview:getSize().height
	end

	scrollview:setInnerContainerSize(CCSizeMake(width,height))
	Declaration:setPosition(ccp(0,height - Declaration:getContentSize().height))
    -- - UI_GetUILabel(LegionListTipViewParent,7):getContentSize().height
	scrollview:addChild(Declaration)
end

--[[" 
      军团列表查看 End
"]]