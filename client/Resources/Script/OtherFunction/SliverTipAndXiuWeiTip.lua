--金钱不足tip

--跳转标记
if g_SliverTip_ClickShouhuo == nil then 
   g_SliverTip_ClickShouhuo = false
end 

if g_SliverTip_ClickNueduo == nil then 
   g_SliverTip_ClickNueduo = false
end 

function SliverTip_InitValue()
   g_SliverTip_ClickShouhuo = false 
   g_SliverTip_ClickNueduo = false
end 

function SliverTip_CloseTishi()
   g_SliverTip_ClickShouhuo = false 
   g_SliverTip_ClickNueduo = false
end 

function SliverTip_ShowSliverLayout()
   local widget = UI_CreateBaseWidgetByFileName("SliverTip.json",EUIOpenAction_FadeIn,0.5)
   SliverTip_InitValue()

   --领地开启等级 
   local lingdiLevelStep = 6
   local isOpen = false 
   if lingdiLevelStep <= GetLocalPlayer():GetInt(EPlayer_Lvl) then 
      isOpen = true 
   else 
      isOpen = false 
   end 

   local yaoqianshulayout = UI_GetUILayout(widget,1)
   local shouhuolayout = UI_GetUILayout(widget,2)
   local nueduolayout = UI_GetUILayout(widget,3)
   if isOpen then
       UI_GetUILabel(shouhuolayout,1):setColor(ccc3(255,255,255))
       UI_GetUILabel(shouhuolayout,2):setColor(ccc3(255,255,255))
       UI_GetUILabel(nueduolayout,1):setColor(ccc3(255,255,255))
       UI_GetUILabel(nueduolayout,2):setColor(ccc3(255,255,255))
   else 
       UI_GetUILabel(shouhuolayout,1):setColor(ccc3(255,0,0))
       UI_GetUILabel(shouhuolayout,2):setColor(ccc3(255,0,0))
       UI_GetUILabel(nueduolayout,1):setColor(ccc3(255,0,0))
       UI_GetUILabel(nueduolayout,2):setColor(ccc3(255,0,0))
   end

   local function getyaoqianshufunction(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then 
         playUiEffect()
         UI_CloseAllBaseWidget()
         CashCow_Create()
      end 
   end 
   yaoqianshulayout:setTouchEnabled(true) 
   yaoqianshulayout:addTouchEventListener(getyaoqianshufunction)

   local function getshouhuolayoutfunction(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then 
         playUiEffect()
         g_SliverTip_ClickShouhuo = true 
         local territorywidget = UI_GetBaseWidgetByName("MyTerritory") 
         if territorywidget then 
            UI_CloseCurBaseWidget()
            ShowTerrioryChange(territorywidget)
         else 
            Packet_Cmd(GameServer_pb.CMD_MANOR_QUERY);
	        ShowWaiting();
         end 
      end 
   end 
   shouhuolayout:setTouchEnabled(true)
   shouhuolayout:addTouchEventListener(getshouhuolayoutfunction)

   local function getnueduolayoutfunction(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then 
         playUiEffect()
         g_SliverTip_ClickNueduo = true
         local territorywidget = UI_GetBaseWidgetByName("MyTerritory") 
         if territorywidget then 
            UI_CloseCurBaseWidget()
            ShowTerrioryChange(territorywidget)
         else 
            Packet_Cmd(GameServer_pb.CMD_MANOR_QUERY);
	        ShowWaiting();
         end 
      end 
   end 
   nueduolayout:setTouchEnabled(true)
   nueduolayout:addTouchEventListener(getnueduolayoutfunction)
end 

function ShowTerrioryChange(widget)
   if UI_GetUIButton(widget,94):getChildByTag(1000) then 
       UI_GetUIButton(widget,94):removeChildByTag(1000)
    end 
    if UI_GetUIButton(widget,93):getChildByTag(1000) then
       UI_GetUIButton(widget,93):removeChildByTag(1000) 
    end 
    if g_SliverTip_ClickShouhuo then 
       Log("arrowwidth")
       local arrow = UI_GetArrow()
       UI_GetUIButton(widget,94):addChild(arrow,1000,1000)
       local arrowwidth = arrow:getSize().width 
       arrow:setPosition(ccp(-arrowwidth,0))
    end 
    if g_SliverTip_ClickNueduo then 
       local arrow = UI_GetArrow()
       UI_GetUIButton(widget,93):addChild(arrow,1000,1000)
       local arrowwidth = arrow:getSize().width 
       arrow:setPosition(ccp(-arrowwidth,0))
    end 
end 


--修为不足tip
function XiuWeiTip_ShowXiuWeiLayout()
   local widget = UI_CreateBaseWidgetByFileName("XiuWeiTip.json",EUIOpenAction_FadeIn,0.5)
   SliverTip_InitValue()

   --领地开启等级 
   local lingdiLevelStep = 6
   local isOpen = false 
   if lingdiLevelStep <= GetLocalPlayer():GetInt(EPlayer_Lvl) then 
      isOpen = true 
   else 
      isOpen = false 
   end 

   local yaoqianshulayout = UI_GetUILayout(widget,1)
   local shouhuolayout = UI_GetUILayout(widget,2)
   local nueduolayout = UI_GetUILayout(widget,3)
   if isOpen then
       UI_GetUILabel(shouhuolayout,1):setColor(ccc3(255,255,255))
       UI_GetUILabel(shouhuolayout,2):setColor(ccc3(255,255,255))
       UI_GetUILabel(nueduolayout,1):setColor(ccc3(255,255,255))
       UI_GetUILabel(nueduolayout,2):setColor(ccc3(255,255,255))
   else 
       UI_GetUILabel(shouhuolayout,1):setColor(ccc3(255,0,0))
       UI_GetUILabel(shouhuolayout,2):setColor(ccc3(255,0,0))
       UI_GetUILabel(nueduolayout,1):setColor(ccc3(255,0,0))
       UI_GetUILabel(nueduolayout,2):setColor(ccc3(255,0,0))
   end

   local function getyaoqianshufunction(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then 
         --CashCow_Create()
         playUiEffect()
         UI_CloseAllBaseWidget()
         Dungeon_ListCreate(true);
      end 
   end 
   
   yaoqianshulayout:setTouchEnabled(true) 
   yaoqianshulayout:addTouchEventListener(getyaoqianshufunction)

   local function getshouhuolayoutfunction(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then
         playUiEffect() 
         g_SliverTip_ClickShouhuo = true 
         local territorywidget = UI_GetBaseWidgetByName("MyTerritory") 
         if territorywidget then 
            UI_CloseCurBaseWidget()
            ShowTerrioryChange(territorywidget)
         else 
            Packet_Cmd(GameServer_pb.CMD_MANOR_QUERY);
	        ShowWaiting();
         end 
      end 
   end 
   shouhuolayout:setTouchEnabled(true)
   shouhuolayout:addTouchEventListener(getshouhuolayoutfunction)

   local function getnueduolayoutfunction(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then 
         playUiEffect()
         g_SliverTip_ClickNueduo = true
         local territorywidget = UI_GetBaseWidgetByName("MyTerritory") 
         if territorywidget then 
            UI_CloseCurBaseWidget()
            ShowTerrioryChange(territorywidget)
         else 
            Packet_Cmd(GameServer_pb.CMD_MANOR_QUERY);
	        ShowWaiting();
         end 
      end 
   end 
   nueduolayout:setTouchEnabled(true)
   nueduolayout:addTouchEventListener(getnueduolayoutfunction)
end 

ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_NEED_SILVER, "SliverTip_ShowSliver_SC" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_NEED_HEROEXP, "XiuweiTip_ShowXiuwei_SC" )

function SliverTip_ShowSliver_SC(pkg)
   EndWaiting()
   Log("xxxxxx")
   SliverTip_ShowSliverLayout()
end 

function XiuweiTip_ShowXiuwei_SC(pkg)
   EndWaiting()
   Log("xxxxxx")
   XiuWeiTip_ShowXiuWeiLayout()
end 