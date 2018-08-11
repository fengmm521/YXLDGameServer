--
--                       _oo0oo_
--                      o8888888o
--                      88" . "88
--                      (| O=O |)
--                      0\  ^  /0
--                    ___/`---'\___
--                  .' \\|     |-- '.
--                 / \\|||  :  |||-- \
--                / _||||| -:- |||||- \
--               |   | \\\  -  --/ |   |
--               | \_|  ''\---/''  |_/ |
--               \  .-\__  '-'  ___/-. /
--             ___'. .'  /--.--\  `. .'___
--          ."" '<  `.___\_<|>_/___.' >' "".
--         | | :  `- \`.;`\ _ /`;.`/ - ` : | |
--         \  \ `_.   \_ __\ /__ _/   .-` /  /
--     =====`-.____`.___ \_____/___.-`___.-'=====
--                       `=---='
--
--
--     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
--
--               佛祖保佑         永无BUG
--

--[[" 
     初始化邮箱系统
"]]




function MailBoxSystem_ShowMailBoxSystem(widget,info)
   local widget = UI_CreateBaseWidgetByFileName("Mailbox.json")
   --return 
   UI_GetUIButton(widget, 1):addTouchEventListener(UI_ClickCloseCurBaseWidget)
   MailBoxSystem_InitValue()
   if #info.szMailIndexList ~= 0 then 
      UI_GetUIImageView(widget,8):setVisible(false)
   else 
      UI_GetUIImageView(widget,8):setVisible(true)
   end 
   --加入邮件列表组件
   MailBoxSystem_MailList(widget,info)
   UI_GetUILayout(widget, 2):addTouchEventListener(PublicCallBackWithNothingToDo)
end 

--为了效率  这里用变量
if g_MailBoxSystem_ChooseIndex == nil then 
   g_MailBoxSystem_ChooseIndex = 0
end 
--
if g_MailBoxSystem_HaveReward == nil then 
   g_MailBoxSystem_HaveReward = false 
end

if g_MailBoxSystem_InfoList == nil then 
   g_MailBoxSystem_InfoList = {}
end 

function MailBoxSystem_InitValue()
   g_MailBoxSystem_ChooseIndex = 0
   g_MailBoxSystem_HaveReward = false 
end

function  MailBoxSystem_RemoveInfotable(strUUID)
   for i = 1,#g_MailBoxSystem_InfoList do 
      if strUUID == g_MailBoxSystem_InfoList[i].strUUID then 
         table.remove(g_MailBoxSystem_InfoList,i)
         break
      end 
   end 
end 
 
function MailBoxSystem_GetMailBoxSystemListIndex(isGet)
   local widget = UI_GetBaseWidgetByName("Mailbox")
   local listview = UI_GetUIListView(widget, 3)
   if g_MailBoxSystem_HaveReward == false then 
      local layout = listview:getItem(g_MailBoxSystem_ChooseIndex)
      UI_GetUIImageView(layout, 2):loadTexture("MailBox/mailbox_maillist_letter_open_icon.png")
   else
      if isGet == true then 
         listview:removeItem(g_MailBoxSystem_ChooseIndex)
         MailBoxSystem_RemoveInfotable(g_MailBoxSystem_InfoList[g_MailBoxSystem_ChooseIndex + 1].strUUID)
      end 
   end 
   if #g_MailBoxSystem_InfoList ~= 0 then 
      UI_GetUIImageView(widget,8):setVisible(false)
   else 
      UI_GetUIImageView(widget,8):setVisible(true)
   end 
end 



--[[" 
    邮箱系统网络消息
"]]
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_QUERY_MAILLIST, "MailBoxSystem_QueryMailList" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_QUERY_MAILDESC, "MailBoxSystem_QueryOneMail" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_GET_MAILAWARD, "MailBoxSystem_GetmailAward" )

function MailBoxSystem_QueryMailList(pkg)
   EndWaiting()
   local info = GameServer_pb.CMD_QUERY_MAILLIST_SC()
   info:ParseFromString(pkg)
   Log("Mail===="..tostring(info))
   
   local widget = UI_GetBaseWidgetByName("Mailbox")
   if widget then
      if #info.szMailIndexList ~= 0 then 
         UI_GetUIImageView(widget,8):setVisible(false)
      else 
         UI_GetUIImageView(widget,8):setVisible(true)
      end 
      MailBoxSystem_MailList(widget,info)
   else 
      g_MailBoxSystem_InfoList = {} 
      g_MailBoxSystem_InfoList = info.szMailIndexList   
      MailBoxSystem_ShowMailBoxSystem(widget,info)
   end 
end 

function MailBoxSystem_QueryOneMail(pkg)
   EndWaiting()
   local info = GameServer_pb.CMD_QUERY_MAILDESC_SC()
   info:ParseFromString(pkg)
   Log("Info===="..tostring(info))
   MailBoxSystem_ShowCurMailInfo(info)
end 

function MailBoxSystem_GetmailAward(pkg)
   EndWaiting()
   local info = GameServer_pb.CMD_GET_MAILAWARD_SC()
   info:ParseFromString(pkg)
   UI_CloseCurBaseWidget(EUICloseAction_None)
   --Packet_Cmd(GameServer_pb.CMD_QUERY_MAILLIST)
   --ShowWaiting()
   MailBoxSystem_GetMailBoxSystemListIndex(true)
end 