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
      军团网络消息 Begin
"]]

ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_QUERY_LEGIONLIST, "NewLegionList_OpenHeroList_SC" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_QUERY_OTHERLEGION, "NewLegionList_LookLegionInfo_SC" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_APPJOIN_LEGION, "NewLegionList_ApplicationSuccess_SC" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_CANCEL_LEGIONAPP, "NewLegionList_CancelApplicationSuccess_SC" )

ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_QUERY_LEGION_BASE, "NewLegionBase_ShowLegionBase_SC" )

ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_QUERY_LEGION_MEMBERLIST, "NewLegionNUmberList_LegionNumbersListInfo_SC" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_MODIFY_LEGION_SETTING, "NewLegionBase_SuccessfullymodifiedAnn_SC" )

ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_QUERY_OTHERLEGION, "NewLegionList_legionlistinfo_SC" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_QUERY_LEGION_APP, "NewLegionList_LegionapplicationInfo" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_DEAL_LEGIONAPP, "NewLegionList_DeallegionApp" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_DEAL_LEGIONAPP_ONEKEY, "NewLegionList_IgnoreAll" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_DISMISS_LEGION, "NewLegion_MisslegionSuccess" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_LEAVE_LEGION, "NewLegion_LeavelegionSuccess" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_NOTICE_LEGOIN_MEMBER_OUTLEGION, "NewLegion_Leavelegion_SC" )

function NewLegionList_OpenHeroList_SC(pkg)
   EndWaiting()
   local info = GameServer_pb.CMD_QUERY_LEGIONLIST_SC()
   info:ParseFromString(pkg)
   Log("NewLegionList_OpenHeroList==="..tostring(info))
   local widget = UI_GetBaseWidgetByName("Legion")
   if widget then 
      --fresh   
      g_NewLegionList_GetLegionListSeverIsReturn = true
      NewLegionList_UpdateLegionList(widget,info)
   else 
      --create 
      NewLegionList_ShowHeroList(info)
   end 
end 

function NewLegionList_LookLegionInfo_SC(pkg)
   EndWaiting()
   local info = GameServer_pb.CMD_QUERY_OTHERLEGION_SC()
   info:ParseFromString(pkg)
   NewLegionInfo_LookOtherLegionInfo(info)
end

function NewLegionList_ApplicationSuccess_SC(pkg)
   EndWaiting()
   local info = GameServer_pb.CMD_APPJOIN_LEGION_SC()
   info:ParseFromString(pkg)
   NewLegionList_UpdateApplicationBt(info,false)
end  

function NewLegionList_CancelApplicationSuccess_SC(pkg)
   EndWaiting()
   local info = GameServer_pb.CMD_CANCEL_LEGIONAPP_SC()
   info:ParseFromString(pkg)
   NewLegionList_UpdateApplicationBt(info,true)
end 

function NewLegionBase_ShowLegionBase_SC(pkg)
   EndWaiting()
   local info = GameServer_pb.CMD_QUERY_LEGION_BASE_SC()
   info:ParseFromString(pkg)
   Log("LegionBaseInfo==============="..tostring(info))
   local widget = UI_GetBaseWidgetByName("LegionDetails")
   if widget then
      --fresh
      NewLegionBase_Refresh(widget,info)
   else
      --create 
      if UI_GetBaseWidgetByName("CreatingLegionBase") then 
         UI_CloseCurBaseWidget(EUICloseAction_FadeOut,0.25)
      end 
      NewLegionBase_ShowMyLegionBaseLayout(info)    
         NoticeFunction_UpdateLegionButton()
   end 
end 

function NewLegionNUmberList_LegionNumbersListInfo_SC(pkg)
   EndWaiting()
   local info = GameServer_pb.CMD_QUERY_LEGION_MEMBERLIST_SC()
   info:ParseFromString(pkg)
   local widget = UI_GetBaseWidgetByName("NewLegionNumberList")
   if widget then 
      --fresh
       
   else 
      --create 
      NewLegionNumberList_Show(info)
   end 
end 

function NewLegionBase_SuccessfullymodifiedAnn_SC(pkg)
   EndWaiting()
   if UI_HasBaseWidget("LegionSetup") then
      UI_CloseCurBaseWidget()
   end
   Packet_Cmd(GameServer_pb.CMD_CLICK_LEGION)
   ShowWaiting()
end 


function NewLegionList_legionlistinfo_SC(pkg)
   EndWaiting()
   local info = GameServer_pb.CMD_QUERY_OTHERLEGION_SC()
   info:ParseFromString(pkg)
   NewLegionList_InfoViewlayout(info)
end

function NewLegionList_LegionapplicationInfo(pkg)
   EndWaiting()
   local info = GameServer_pb.CMD_QUERY_LEGION_APP_SC()
   info:ParseFromString(pkg)
   Log("xxxxx ===="..tostring(info))
   local widget = UI_GetBaseWidgetByName("Legionapplicationlist")
   if widget then 
      --fresh
      NewLegionApplicationList_ShowApplicationList(info,widget)
   else 
      --create
      NewLegionApplicationList_LayoutView(info)
   end 
end

function NewLegionList_DeallegionApp(pkg)
   EndWaiting()
   local info = GameServer_pb.CMD_DEAL_LEGIONAPP_SC()
   info:ParseFromString(pkg)
   Log("DeallegionApp=======return   OK///////")
   Packet_Cmd(GameServer_pb.CMD_QUERY_LEGION_APP)
   ShowWaiting()
end

function NewLegionList_IgnoreAll(pkg)
   EndWaiting()
   Packet_Cmd(GameServer_pb.CMD_QUERY_LEGION_APP)
   ShowWaiting()
end

function NewLegion_MisslegionSuccess(pkg)
   EndWaiting()
   --UI_CloseAllBaseWidget()
end

function NewLegion_LeavelegionSuccess(pkg)
   EndWaiting()
   UI_CloseAllBaseWidget() 
end

function NewLegion_Leavelegion_SC(pkg)
   local info = GameServer_pb.CMD_NOTICE_LEGOIN_MEMBER_OUTLEGION_SC()
   info:ParseFromString(pkg)
   if UI_GetBaseWidgetByName("LegionDetails") then 
      Log("aaaaaaaaaaaaaaaaaaaaaxxxxxxxxxxxxxxxxxxxxxx===="..info.iNoticeType)
      local function besure()
            Log("left")
            UI_CloseAllBaseWidget()
      end
      local str = nil 
      if info.iNoticeType == 1 then 
         str = FormatString("legion_Qui")
      else 
         str = FormatString("legion_Jiesan")
      end 
      Messagebox_Create({info = str, msgType = EMessageType_Middle, leftFun = besure})
   end 
end

--[[" 
      军团网络消息 End
"]]