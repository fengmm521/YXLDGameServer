function NewHeroList_HeroImageInfo(layout,heroEntity)
   --iamge 
   local data = GetGameData(DataFileHeroBorn,heroEntity:GetUint(EHero_HeroId),"stHeroBornData")
   UI_GetHeroIncoByClone(data.m_icon,layout,2)

   --frame
   local heroLvLStep = heroEntity:GetUint(EHero_LvlStep)
   UI_IconFrameByClone(layout,1,heroLvLStep)
end 


--获得天赋skill
function UI_GetHeroTallenSkillIconAndFrame(layout,Icontag,Frametag,icon,levelstep)
   UI_GetUIImageView(layout,Icontag):loadTexture("Icon/Skill/"..icon..".png")
   UI_IconFrameWithSkillOrItem(layout,Frametag,levelstep)
end 


function HeroSystem_ReturnDelayAndFadeInAction(offsetTime)
   local delayTime = CCDelayTime:create(offsetTime)
   local actionArry = CCArray:create()
   actionArry:addObject(delayTime) 
   local actionFadeIn = CCFadeIn:create(0.1)
   actionArry:addObject(actionFadeIn)
   local seqaction = CCSequence:create(actionArry)
   return seqaction
end 

function HeroSystem_ReturnDelayAndFadeInAction(offsetTime,iscallback)
   local delayTime = CCDelayTime:create(offsetTime)
   local actionArry = CCArray:create()
   actionArry:addObject(delayTime) 
   local actionFadeIn = CCFadeIn:create(0.1)
   actionArry:addObject(actionFadeIn)
   if iscallback ~= nil then 
      local callback = CCCallFuncN:create(iscallback)
      actionArry:addObject(callback)
   end 
   local seqaction = CCSequence:create(actionArry)
   return seqaction
end 
function HeroSystem_HeroAscendingActionShow(layoutTag,starOffsetTime,iscallback)
   local offsetTime = starOffsetTime
   local action = {} 
   for i = 1,#layoutTag do 
      action[i] = nil 
      if i == #layoutTag then
         if iscallback ~= nil then 
            action[i] = HeroSystem_ReturnDelayAndFadeInAction(offsetTime,iscallback)
         else 
            action[i] = HeroSystem_ReturnDelayAndFadeInAction(offsetTime)
         end 
      else  
         action[i] = HeroSystem_ReturnDelayAndFadeInAction(offsetTime)
      end 
      layoutTag[i]:runAction(action[i])
      offsetTime = offsetTime + 0.1
   end   
end

function HeroSystem_HeroOnlyOneLayoutActionShow_FadeIn(layout,time,iscallback) 
   local action = HeroSystem_ReturnDelayAndFadeInAction(time,iscallback)
   layout:runAction(action)
end

function HeroSystem_HeroDelayAndCallbackAction(layout,offsetTime,iscallback) 
   local delayTime = CCDelayTime:create(offsetTime)
   local actionArry = CCArray:create()
   actionArry:addObject(delayTime) 
   if iscallback ~= nil then 
      local callback = CCCallFuncN:create(iscallback)
      actionArry:addObject(callback)
   end 
   local seqaction = CCSequence:create(actionArry)
   layout:runAction(seqaction)
end 

function HeroSystem_MoveAndFadeIn(layout,moveX,moveY,time)
   local arry = CCArray:create()
   layout:setVisible(true)
   local moveaction = CCMoveTo:create(time,ccp(moveX,moveY))
   arry:addObject(moveaction)
   local fadein = CCFadeIn:create(time)
   arry:addObject(fadein)
   local swap = CCSpawn:create(arry)
   layout:runAction(swap)
end 

local function afterFade(sender)
   local sender = tolua.cast(sender,"Layout")
   sender:setVisible(false)
end 
function HeroSystem_MoveAndFadeOut(layout,moveX,moveY,time)
   local arry = CCArray:create()
   layout:setVisible(true)
   local moveaction = CCMoveTo:create(time,ccp(moveX,moveY))
   arry:addObject(moveaction)
  -- local fadein = CCFadeOut:create(time)
   --arry:addObject(fadein)
   local afterfade = CCCallFuncN:create(afterFade)
   arry:addObject(afterfade)
   local swap = CCSequence:create(arry)
   layout:runAction(swap)
end 

function HeroSystem_UpArrowLabFadeAndOut(layout,str,fontSize)
   local arrowImag = ImageView:create()
   arrowImag:loadTexture("Common/Arrow_03.png")
   arrowImag:setAnchorPoint(ccp(0,0))
   layout:addChild(arrowImag)
   arrowImag:setScale(0.4)
   local lab = Label:create()
   lab:setText(str)
   lab:setFontSize(fontSize)
   lab:setAnchorPoint(ccp(0,0))
   lab:setColor(ccc3(0,255,0))
   lab:setPosition(ccp(arrowImag:getContentSize().width * 0.4 + 10,0))
   layout:addChild(lab)
   UI_GetFadeInAndOutAction(layout)
end 

function HeroSystem_ScaleAction(layout,time)
   local arry = CCArray:create()
   local actionLarger = CCEaseBackOut:create(CCScaleTo:create(time,1.5))
   local actionNormal = CCScaleTo:create(time,1.0)
   arry:addObject(actionLarger)
   arry:addObject(actionNormal)
   local seq = CCSequence:create(arry)
   layout:runAction(seq)
end 
--[["
    英雄网络消息 Begin    
"]]

ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_HEROSYSTEM_SELLHERO, "NewHeroList_AfterSellHeroSoul" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_HERO_CONVERT, "NewHeroList_AfterHeroSoulConvert" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_HERO_CONVERT_CONFIRM, "NewHeroList_BeSureHeroSoulConvert" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_QUERY_HERO_LEVELINFO, "HeroSystem_UpgradeScInfo" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_HERO_LEVELUP, "HeroSystem_AfterHeroUpgradeView" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_HERO_SANGONG, "HeroSystem_AfterSangongView" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_HERO_LEVELSTEPUP, "HeroSystem_HeroAscending_AfterAscending" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_GET_HERODESC, "HeroSystem_HeroDescView" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_HERO_TALLENT_LEVELUP, "HeroSystem_AfterHeroTallentLevelUp" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_QUERY_EQUIPSOURCE, "HeroSystem_AfterQuerItemWhere" )
function NewHeroList_AfterSellHeroSoul(pkg)
   EndWaiting()
   UI_CloseCurBaseWidget()
   local widget = UI_GetBaseWidgetByName("NewHeroList")
   if widget then 
      NewHeroList_UpdateHeroSoulInfoAterSell(widget)
   end 
end 

function NewHeroList_AfterHeroSoulConvert(pkg)
   EndWaiting()
   local info = GameServer_pb.CMD_HERO_CONVERT_SC()
   info:ParseFromString(pkg)
   Log("heroSoulConvert====="..tostring(info))
   local widget = UI_GetBaseWidgetByName("HeroSoulConver")
   g_NewHeroList_HaveConvertHeroSoul = true 
   if widget then 
      if g_NewHeroList_IsAutoHeroConver == true then 
         NewHeroList_AutoHeroSoulConver_Update(widget,g_NewHeroList_ChooseConvertHeroSoul,info.iNewHeroID)
      else 
         NewHeroList_HeroSoulConver_InfoShow(widget,g_NewHeroList_ChooseConvertHeroSoul,info.iNewHeroID)
      end 
   end 
end 

function NewHeroList_BeSureHeroSoulConvert(pkg)
   EndWaiting()
   UI_CloseCurBaseWidget()
   local widget = UI_GetBaseWidgetByName("NewHeroList")
   if widget then 
      NewHeroList_UpdateHeroSoulInfoAterConvert(widget)
   end 
end 

function HeroSystem_UpgradeScInfo(pkg)
   EndWaiting()
   local info = GameServer_pb.Cmd_Sc_QueryHeroLevelInfo()
   info:ParseFromString(pkg)
   local heroEntity = GetLocalPlayer():GetHeroByObjId(info.levelInfo.dwHeroObjectID)
   if UI_GetBaseWidgetByName("NewHeroSystem") then 
      Log("refresh")
      local widget = UI_GetBaseWidgetByName("NewHeroSystem") 
      if UI_GetUILayout(widget, 4):isVisible() == false then 
         HeroSystem_ShowCurlayoutWithAction()
      end  
      HeroSystem_UpdateHeroAfterLevelUp(widget,heroEntity,info.levelInfo,false)
   else 
      Log("create")
      HeroSystem_InitLayout(heroEntity,info.levelInfo)
   end 
end 

function HeroSystem_AfterHeroUpgradeView(pkg)
   local info = GameServer_pb.Cmd_Sc_HeroLevelUp()
   info:ParseFromString(pkg)
   local widget = UI_GetBaseWidgetByName("NewHeroSystem")
   local heroEntity = GetLocalPlayer():GetHeroByObjId(info.levelInfo.dwHeroObjectID) 
   if widget then  
      HeroSystem_ShowXiuweiDianjiAnimature(widget,0)
      HeroSystem_UpdateHeroAfterLevelUp(widget,heroEntity,true)
   end 
end

function HeroSystem_AfterSangongView(pkg)
   EndWaiting()
   local info = GameServer_pb.CMD_HERO_SANGONG_SC()
   info:ParseFromString(pkg)
   local widget = UI_GetBaseWidgetByName("NewHeroSystem")
   local heroEntity = GetLocalPlayer():GetHeroByObjId(info.dwObjectID)
   if widget then 
      --local tab = GameServer_pb.Cmd_Cs_QueryHeroLevelInfo();
      --tab.dwHeroObjectID = info.dwObjectID
      --Packet_Full(GameServer_pb.CMD_QUERY_HERO_LEVELINFO,tab)
      --ShowWaiting()
      HeroSystem_ShowXiuweiDianjiAnimature(widget,1)
      HeroSystem_UpdateHeroAfterLevelUp(widget,heroEntity,false)
   end  
end 

function HeroSystem_HeroAscending_AfterAscending(pkg)
   EndWaiting()
   local info = GameServer_pb.CMD_HERO_LEVELSTEPUP_SC()
   info:ParseFromString(pkg)
   Log("info==="..tostring(info))
   --这里还是要处理数据
   --根据左边的界面刷新界面
   local widget = UI_GetBaseWidgetByName("NewHeroSystem")
   local heroEntity = GetLocalPlayer():GetHeroByObjId(info.dwHeroObjectID)
   HeroSystem_AfterAscedingRightShow(widget,heroEntity,true)
   if widget then
      --if UI_GetUILayout(widget, 4):isVisible() then 
      --   --升级界面
      --   --请求
      --   local tab = GameServer_pb.Cmd_Cs_QueryHeroLevelInfo();
      --   tab.dwHeroObjectID = info.dwHeroObjectID
      --   Packet_Full(GameServer_pb.CMD_QUERY_HERO_LEVELINFO,tab)
      --   ShowWaiting()
      --end
      if UI_GetUILayout(widget, 5):isVisible() then 
         --属性界面
         local tab = GameServer_pb.Cmd_Cs_GetHeroDesc()
         tab.dwObjectID = heroEntity:GetUint(EHero_ID)
	     Packet_Full(GameServer_pb.CMD_GET_HERODESC, tab)
	     ShowWaiting()
      end 
      --天赋技能界面走天赋技能变化的协议
   end 
   HeroSysetm_AfterHeroAscedingShow(info) 
end 

function HeroSystem_HeroDescView(pkg)
   EndWaiting()
   local widget = UI_GetBaseWidgetByName("NewHeroSystem")
   local info = GameServer_pb.Cmd_Sc_GetHeroDesc()
   info:ParseFromString(pkg)
   Log("info===="..tostring(info))
   local heroEntity = GetLocalPlayer():GetHeroByObjId(info.dwObjectID)
   if widget then
      if UI_GetUILayout(widget, 5):isVisible() == false then
         HeroSystem_ShowCurlayoutWithAction()
      end  
      HeroSystem_MoreDetailsShow(widget,heroEntity,info)
   end   
end 

function HeroSystem_AfterHeroTallentLevelUp(pkg)
   EndWaiting()
end 

function HeroSystem_AfterQuerItemWhere(pkg)
   EndWaiting()
   local info = GameServer_pb.CMD_QUERY_EQUIPSOURCE_SC()
   info:ParseFromString(pkg)
   Log("info....==="..tostring(info))
   HeroSystem_HeroTallentSkillItemWhere(info)
end 
--[["
    英雄网络消息 End   
"]]