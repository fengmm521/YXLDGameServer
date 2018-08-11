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
     初始化神兽系统
"]]


--数据

--记录当前所在的layout 1.培养 2.进阶 3.传承 4.魂魄
if g_GodAnimalSystem_Curlayout == nil then
   g_GodAnimalSystem_Curlayout = 1
end 

--当前选取神兽的索引
if g_GodAnimalSystem_CurChooseIndex == nil then 
   g_GodAnimalSystem_CurChooseIndex = 0
end 

--记录总共的index
if g_GodAnimalSystem_CurTotolChooseIndex == nil then 
   g_GodAnimalSystem_CurTotolChooseIndex = 0
end

----记录未获得神兽的Index
--if g_GodAnimalSystem_CurNotHaveChooseIndex == nil then 
--   g_GodAnimalSystem_CurNotHaveChooseIndex = 0
--end 

--记录神兽培养之前的等级
if g_GodAnimalSystem_lastlevel == nil then
   g_GodAnimalSystem_lastlevel = 0
end 

--判断当前index是否在已获得列表中
function GodAnimalSystem_CurIndexIsHavelist()
   local haveanimallist = GetLocalPlayer():GetGodAnimalList()
   if g_GodAnimalSystem_CurChooseIndex < haveanimallist:size() - 1 then
      return true 
   else 
      return false  
   end 
end 

--判断总体index 是否在已获得列表中
function GodAnimalSystem_totalIndexIsHavelist()
   local haveanimallist = GetLocalPlayer():GetGodAnimalList() 
   if g_GodAnimalSystem_CurTotolChooseIndex <= haveanimallist:size() - 1 then
      return true 
   else
      return false   
   end 
end 

--记录当前当前继承属性的坐标
if g_GodAnimalSystem_leftPosX == nil then 
   g_GodAnimalSystem_leftPosX = 0
end 

if g_GodAnimalSystem_leftPosY == nil then 
   g_GodAnimalSystem_leftPosY = 0
end 

--初始化数据
function GodAnimalSystem_InitValue()
   g_GodAnimalSystem_Curlayout = 1
   g_GodAnimalSystem_CurChooseIndex = 0
   g_GodAnimalSystem_CurTotolChooseIndex = 0
   g_GodAnimalSystem_lastlevel = 0
   --g_GodAnimalSystem_CurNotHaveChooseIndex = 0 
  -- GodAnimalSystem_InitAnimalIdList()
end 

--获得 ObjectId
function GodAnimalSystem_GetGodAnimalObjectIdByIndex(index)
	-- body
	local temp = GetLocalPlayer():GetGodAnimalList()
	return temp[index]:GetInt(EGodAnimal_ObjId) 
end

--获得Entity
function GodAnimalSystem_GetGodAnimalItem(dwobjectID)
	local temp = GetLocalPlayer():GetGodAnimalList()
	for i=0,temp:size()-1 do
	   if( temp[i]:GetInt(EGodAnimal_ObjId) == dwobjectID)then
		  return GetLocalPlayer():GetGodAnimalList()[i]
	   end
	end
	return nil
end

--获得Godanimaldata
function GodAnimalSystem_GetGodAnimalData(dwobjectID)
   return GetLocalPlayer():GetGodAnimalByDwObject(dwobjectID):getGodAnimalData()
end 

--没有获得的神兽的data
function GodAnimalSystem_GetNotGodAnimalData(dwobjectID)
   return GetGameData(DataFileGodAniaml,dwobjectID,"stGodAnimalData")
end 

--获得神兽ID
function GodAnimalSystem_GetAnimalId(index)
   local godanimallist = CDataManager:GetInstance():GetGameDataKeyList(DataFileGodAniaml)
   return godanimallist[index]
end 

--获得已经拥有的神兽个数
function GodAnimalSystem_GetHaveAnimalNumber()
   return GetLocalPlayer():GetGodAnimalList():size()
end 

--获得总共的神兽个数
function GodAnimalSystem_GetTotalNumber()
   return g_GodAnimalSystem_AnimalIdlist:size()
end 

--神兽列表
if g_GodAnimalSystem_AnimalIdlist == nil then 
   g_GodAnimalSystem_AnimalIdlist = vector_int_:new_local()
   --GetLocalPlayer():sortGodAnimalWithId(g_GodAnimalSystem_AnimalIdlist) 
   --GetLocalPlayer():sortGodAnimalWithId(g_GodAnimalSystem_AnimalIdlist)
end 
function GodAnimalSystem_InitAnimalIdList()
    --g_GodAnimalSystem_AnimalIdlist = vector_int_:new_local()
    GetLocalPlayer():sortGodAnimalWithId(g_GodAnimalSystem_AnimalIdlist)
end 

--skillTip
function GodAnimalSystem_SkillTipView(layout,skillId)
	-- body
    local function GodAnimalSkillTipFunc(sender,eventType)
    	-- body
    	if eventType == TOUCH_EVENT_ENDED then
           SkillTipView(skillId)
    	end
    end	
    layout:setTouchEnabled(true)
    layout:addTouchEventListener(GodAnimalSkillTipFunc)
end

--查询神兽信息CS
function GodAnimalSystem_QueryGodAnimalDetai(index)
  -- g_GodAnimalSystem_AnimalIdlist = vector_int_:new_local()
   --GetLocalPlayer():sortGodAnimalWithId(g_GodAnimalSystem_AnimalIdlist) 
   local animalEntity = GetLocalPlayer():GetGodAnimalByAnimalId(g_GodAnimalSystem_AnimalIdlist[index])
   if animalEntity ~= nil then
      local tab = GameServer_pb.Cmd_Cs_QueryGodAnimalDetai()
      tab.dwGodAnimalObjectID =  animalEntity:GetInt(EGodAnimal_ObjId) 
      Packet_Full(GameServer_pb.CMD_GODANIMAL_DETAIL, tab)
      ShowWaiting()
   else 
      local tab = GameServer_pb.CMD_GODANIMAL_DETAIL_BYID_CS()
      local data = GetGameData(DataFileGodAniaml,g_GodAnimalSystem_AnimalIdlist[index],"stGodAnimalData")
      tab.iGodAnimalID = data.m_ID
      Packet_Full(GameServer_pb.CMD_GODANIMAL_DETAIL_BYID,tab)
      ShowWaiting()
   end    
end 

--进阶查询
function GodAnimalSystem_QueryGodAnmialAscendingInfo(index)
   local animalEntity = GetLocalPlayer():GetGodAnimalByAnimalId(g_GodAnimalSystem_AnimalIdlist[index])
   if animalEntity ~= nil then
      local godanimalObjectId = animalEntity:GetInt(EGodAnimal_ObjId)
      local tab = GameServer_pb.Cmd_Cs_QueryGodAnimalLevelStep()
      tab.dwObjectID = godanimalObjectId 
      Packet_Full(GameServer_pb.CMD_GODANIMAL_QUERYLEVELSTEP, tab)
      ShowWaiting()     
   end 
end 

--神兽系统
function GodAnimalSystem_InitLayout(info) 
   local widget = UI_CreateBaseWidgetByFileName("GodAnimalSystem.json")

   --return 
   UI_GetUIButton(widget, 1):addTouchEventListener(UI_ClickCloseCurBaseWidget)
   --初始化数据
   GodAnimalSystem_InitValue()

   --加入神兽形象组件
   local animaldata = GodAnimalSystem_GetNotGodAnimalData(g_GodAnimalSystem_AnimalIdlist[0])
   GodAnimalSystem_FormInfoView(widget,animaldata,info,info.iLevel)

   --加入获取其他神兽组件
   GodAnimalSystem_GetNewAnimalFormInfo(widget)

   --加入更新左右键组件
   GodAnimalSystem_UpdateGodAnimalInfo_LeftandRightButton(widget)

   --加入神兽升级属性
   GodAnimalSystem_GodAnimalNature(widget,info)

   --加入神兽培养或激活组件
   GodAnimalSystem_UpdateOrActivity(widget,info)

   --加入出战组件
   GodAnimalSystem_Played(widget)

   --加入当前界面组件
   GodAnimalSystem_ShowCurlayout()

   --加入标签页功能
   GodAnimalSystem_LabelPageButton(widget)

   if g_GodAnimalSystem_leftPosX == 0 or g_GodAnimalSystem_leftPosY == 0 then 
      local layout = UI_GetUILayout(UI_GetUILayout(widget, 6),4)
      local infolayout = UI_GetUILayout(UI_GetUILayout(layout, 3),4)
      local godanimalinfolayout = UI_GetUILayout(infolayout,3)
      g_GodAnimalSystem_leftPosX ,g_GodAnimalSystem_leftPosY = godanimalinfolayout:getPosition()
   end 
end 


--网络消息
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_GODANIMAL_DETAIL, "GodAnimalSystem_ShowGodAnimalDetai" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_GODANIMAL_DETAIL_BYID, "GodAnimalSystem_ShowNotHaveGodAnimalDetai" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_GODANIMAL_TRAIN, "GodAnimalSystem_TrainSuccess" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_GODANIMAL_ANIMALSOUL_ACTIVE, "GodAnimalSystem_ActionactivitySuccess" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_GODANIMAL_ACTIVE, "GodAnimalSystem_GetGodAnimalPlayed" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_GODANIMAL_QUERYLEVELSTEP, "GodAnimalSystem_QueryStepLevelUpInfo" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_GODANIMAL_ANIMALSOUL_QUERY, "GodAnimalSystem_QuerySoulInfo" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_GODANIMAL_ANIMALSOUL_SELL, "GodAnimalSystem_AfterSoulSell" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_GODANIMAL_QUERYINHERIT, "GodAnimalSystem_InheritanceInfo" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_GODANIMAL_EXEINHERIT, "GodAnimalSystem_ExeGodAnimalInheritReturn" )

function GodAnimalSystem_ShowGodAnimalDetai(pkg)
   EndWaiting()
   local info = GameServer_pb.Cmd_Sc_QueryGodAnimalDetai()
   info:ParseFromString(pkg)
   Log("info===="..tostring(info))
   if UI_GetBaseWidgetByName("GodAnimalSystem") then
      local widget = UI_GetBaseWidgetByName("GodAnimalSystem")
      if UI_GetUILayout(UI_GetUILayout(widget, 6),2):isVisible() == false then 
          GodAnimalSystem_ShowCurlayout()
      end 
      local godanimaData = GodAnimalSystem_GetGodAnimalData(info.detail.dwObjectID)
      GodAnimalSystem_FormInfoView(widget,godanimaData,info.detail,info.detail.iLevel)   
      GodAnimalSystem_UpdateGodAnimalInfo_LeftandRightButton(widget)
      --加入神兽升级属性
      GodAnimalSystem_GodAnimalNature(widget,info.detail)
      GodAnimalSystem_UpdateOrActivity(widget,info.detail) 
   else 
      Log("Create GodAnimalSystem")
      GodAnimalSystem_InitLayout(info.detail)
   end 
end 

function GodAnimalSystem_ShowNotHaveGodAnimalDetai(pkg)
   EndWaiting()
   local info = GameServer_pb.CMD_GODANIMAL_DETAIL_BYID_SC()
   info:ParseFromString(pkg)
   Log("Heroxxxxxx==="..tostring(info))
   local widget = UI_GetBaseWidgetByName("GodAnimalSystem")
   if widget then
      if UI_GetUILayout(UI_GetUILayout(widget, 6),2):isVisible() == false then 
         GodAnimalSystem_ShowCurlayout()
      end
      local animaldata = GodAnimalSystem_GetNotGodAnimalData(info.iGodAnimalID)
      GodAnimalSystem_FormInfoView(widget,animaldata,info.detail,info.detail.iLevel)
      GodAnimalSystem_UpdateGodAnimalInfo_LeftandRightButton(widget)
      --加入神兽升级属性
      GodAnimalSystem_GodAnimalNature(widget,info.detail)

      GodAnimalSystem_UpdateOrActivity(widget,info.detail)
   end 
end 

function GodAnimalSystem_TrainSuccess(pkg)
   EndWaiting()
   local info = GameServer_pb.Cmd_Sc_GoldAnimalTrain()
   info:ParseFromString(pkg)
   local widget = UI_GetBaseWidgetByName("GodAnimalSystem")
   if widget then 
      Log("exp==========="..info.iGetExp)
      --更新loadingbar
      GodAnimalSystem_UpdateLoadingbar(widget,info.detail,true)
      --培养数字 action
      local levelup = info.detail.iLevel - g_GodAnimalSystem_lastlevel
      GodAnimalSystem_UpdateNumberAction(widget,info.bKnock,levelup,info.iGetExp)
      GodAnimalSystem_UpdateGodAnimal_AfterTrain(widget,info.detail)
   end 
end 
function GodAnimalSystem_ActionactivitySuccess(pkg)
   EndWaiting()
   local widget = UI_GetBaseWidgetByName("GodAnimalSystem")
   if widget then
      g_GodAnimalSystem_CurTotolChooseIndex = 0
      g_GodAnimalSystem_CurChooseIndex = 0
      GodAnimalSystem_InitAnimalIdList()
      
      if UI_GetUILayout(UI_GetUILayout(widget, 6),2):isVisible() then
         GodAnimalSystem_QueryGodAnimalDetai(0)
      elseif UI_GetUILayout(UI_GetUILayout(widget, 6),5):isVisible() then 
         Packet_Cmd(GameServer_pb.CMD_GODANIMAL_ANIMALSOUL_QUERY)
         ShowWaiting()
      end 
   end 
end 

function GodAnimalSystem_GetGodAnimalPlayed(pkg)
   EndWaiting()
   local info = GameServer_pb.Cmd_Sc_GoldAnimalActive()
   info:ParseFromString(pkg)
   local widget = UI_GetBaseWidgetByName("GodAnimalSystem")
   if widget then 
      GetLocalPlayer():SetGodAnimalActive(info.dwObjectID)
      g_GodAnimalSystem_CurTotolChooseIndex = 0
      g_GodAnimalSystem_CurChooseIndex = 0
      GodAnimalSystem_InitAnimalIdList()
      GodAnimalSystem_QueryGodAnimalDetai(0)
   end 
end 

function GodAnimalSystem_QueryStepLevelUpInfo(pkg) 
   EndWaiting()
   local widget = UI_GetBaseWidgetByName("GodAnimalSystem")
   if widget then 
      local info = GameServer_pb.Cmd_Sc_QueryGodAnimalLevelStep()
	  info:ParseFromString(pkg)   
      if UI_GetUILayout(UI_GetUILayout(widget, 6),3):isVisible() == false then 
         GodAnimalSystem_ShowCurlayout()        
      end 
      local animalEntity = GetLocalPlayer():GetGodAnimalByDwObject(info.dwObjectID)
      local animaldata = animalEntity:getGodAnimalData()
      GodAnimalSystem_FormInfoView(widget,animaldata,info.curInfo,animalEntity:GetInt(EGodAnimal_Level))
      
      Log("info======="..tostring(info))  
      GodAnimalSystem_AscendingSkillAndInfo(widget,info)
      GodAnimalSystem_UpdateGodAnimalInfo_LeftandRightButton(widget)
   end 
end 

function GodAnimalSystem_QuerySoulInfo(pkg)
   EndWaiting()
   local widget = UI_GetBaseWidgetByName("GodAnimalSystem")
   if widget then 
      if UI_GetUILayout(UI_GetUILayout(widget, 6),5):isVisible() == false then 
         GodAnimalSystem_ShowCurlayout()
      end 
      local info = GameServer_pb.CMD_GODANIMAL_ANIMALSOUL_QUERY_SC()
      info:ParseFromString(pkg)
      GodAnimalSystem_SoulShow(widget,info)
   end 
end 

function GodAnimalSystem_AfterSoulSell(pkg)
   EndWaiting()
   Packet_Cmd(GameServer_pb.CMD_GODANIMAL_ANIMALSOUL_QUERY)
   ShowWaiting()
end 

function GodAnimalSystem_InheritanceInfo(pkg)
   EndWaiting()
   local widget = UI_GetBaseWidgetByName("GodAnimalSystem")
   if widget then
      if UI_GetUILayout(UI_GetUILayout(widget, 6),4):isVisible() == false then
         GodAnimalSystem_ShowCurlayout()
      else 
        --这里就暂时先这样写 神兽不多 刷新list也不会出先卡的现象 
      end 
      local info = GameServer_pb.Cmd_Sc_QueryGodAnimalInherit()
      info:ParseFromString(pkg)
      Log("info ====="..tostring(info)) 
      --添加继承神兽组件
      GodAnimalSystem_Inheritance_LeftList(widget,info)
      --添加传承神兽组件
      GodAnimalSystem_Inheritance_RightList(widget,info)
   end 
end

function GodAnimalSystem_ExeGodAnimalInheritReturn(pkg)
   EndWaiting()
   local widget = UI_GetBaseWidgetByName("GodAnimalSystem")
   if widget then
      local info = GameServer_pb.Cmd_Sc_ExeGodAnimalInherit()
      info:ParseFromString(pkg)
      local tab = GameServer_pb.Cmd_Cs_QueryGodAnimalInherit()
      local passGodVec = GetLocalPlayer():GetCanInheritGodAnimal( GetLocalPlayer():GetGodAnimalByDwObject(info.acceptDetail.dwObjectID))
      if passGodVec:size() ~= 0 then
      local passgodAnimalObjId = passGodVec[0]:GetInt(EGodAnimal_ObjId)
            tab.dwPasserObjectID = passgodAnimalObjId
      end
      local godanimalObjectId = GetLocalPlayer():GetGodAnimalByDwObject(info.acceptDetail.dwObjectID):GetInt(EGodAnimal_ObjId)
      tab.dwAccepObjectID = godanimalObjectId
      Packet_Full( GameServer_pb.CMD_GODANIMAL_QUERYINHERIT, tab )
      ShowWaiting()
   end 
end