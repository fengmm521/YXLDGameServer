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

--[[" 
      军团申请列表 Begin
"]]

function NewLegionApplicationList_LayoutView(info)
   local widget = UI_CreateBaseWidgetByFileName("Legionapplicationlist.json",EUIOpenAction_FadeIn,0.5)
   local function ClickClose(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then 
         UI_CloseCurBaseWidget(EUICloseAction_FadeOut, 0.25)
         NewLegionBase_Refresh_CS()
      end 
   end
   UI_GetUIButton(UI_GetUILayout(widget,1), 1):addTouchEventListener(ClickClose)

   NewLegionApplicationList_ShowApplicationList(info,widget)
end

function NewLegionApplicationList_ShowApplicationList(info,widget)
   local listview = UI_GetUIListView(UI_GetUILayout(widget,1),3)
   listview:removeAllItems()
   local listWidth = listview:getContentSize().width 
   local listHeight = listview:getContentSize().height

   local layout = nil 
   local function createApplicationLayout(layout,i)
      --name 
      UI_SetLabelText(layout,3,info.szAppList[i].strAppName)
      --lvl
      UI_SetLabelText(layout,4,"lv."..info.szAppList[i].iLevel)
      --竞技场排名
      UI_SetLabelText(layout,5,FormatString("Legion_AreaRank",info.szAppList[i].iArenaRank))

      --image 
      local data = GetGameData(DataFileHeroBorn, info.szAppList[i].iActorHeadIcon, "stHeroBornData")
      local friendImg = CCSprite:create("Icon/HeroIcon/x"..data.m_icon..".png")
      local imag = UIMgr:GetInstance():createMaskedSprite(friendImg,"zhujiemian/bg_4.png")
      UI_GetUIImageView(layout, 2):addNode(imag)
      --同意
      local function AgreeApplication(sender,eventType)
            -- body
            if eventType == TOUCH_EVENT_ENDED then
                local clickIndex = listview:getIndex(sender:getParent())
                local tab = GameServer_pb.CMD_DEAL_LEGIONAPP_CS()
                tab.strAppName = info.szAppList[i].strAppName
                tab.bAccept = true
                Packet_Full(GameServer_pb.CMD_DEAL_LEGIONAPP, tab)
                ShowWaiting()  
            end
      end
      UI_GetUIButton(layout,6):addTouchEventListener(AgreeApplication)

      --忽略
      local function legionIgnore(sender,eventType)
            -- body
            if eventType == TOUCH_EVENT_ENDED then
                local clickIndex = listview:getIndex(sender:getParent())
                local tab = GameServer_pb.CMD_DEAL_LEGIONAPP_CS()
                tab.strAppName = info.szAppList[i].strAppName
                tab.bAccept = false
                Packet_Full(GameServer_pb.CMD_DEAL_LEGIONAPP, tab)
                ShowWaiting()
            end
        end
        UI_GetUIButton(layout,7):addTouchEventListener(legionIgnore)
   end
   for i = 1,#info.szAppList do 
      local layout = UI_GetCloneImageView(UI_GetUIImageView(widget,2))
      layout:setVisible(true)
      createApplicationLayout(layout,i)
      listview:pushBackCustomItem(layout)
   end 
   if #info.szAppList ~= 0 then
        UI_GetUILabel(widget, 1240):setVisible(false)
   else
        UI_GetUILabel(widget, 1240):setVisible(true)
   end
   --一键忽略
   local function IgnoreAll(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then 
         Packet_Cmd(GameServer_pb.CMD_DEAL_LEGIONAPP_ONEKEY)
         ShowWaiting()
      end 
   end
   UI_GetUIButton(UI_GetUILayout(widget,1),2):addTouchEventListener(IgnoreAll)
end
--[[" 
      军团申请列表 End
"]]