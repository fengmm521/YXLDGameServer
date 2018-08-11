
--[[" 
      herocurinfo界面第二版 Begin
"]]

--记录喜好品位置
if g_heroClickfavoriteIndex == nil then 
   g_heroClickfavoriteIndex = 0
end 

--英雄升级 点击间隔时间
if g_HeroSystem_HeroUpdateIntervalTime == nil then  
   g_HeroSystem_HeroUpdateIntervalTime = 0
end

--英雄界面
if g_HeroSystem_IsHeroSystem == nil then 
   g_HeroSystem_IsHeroSystem = false
end 

function HeroCurInfoSecond_InitValue()
   g_heroClickfavoriteIndex = 0
   g_heroCurInfoLayoutState = 1
   g_HeroSystem_HeroUpdateIntervalTime = 0
   g_HeroSystem_IsHeroSystem = true
   g_heroCurInfo_TallentId_Select = 0
end 

--当前layout
--1.喜好品 2.升级 3.详细属性 4.技能 5.装备
if g_heroCurInfoLayoutState == nil then 
   g_heroCurInfoLayoutState = 1
end 

if g_heroCurInfo_TallentId_Select == nil then 
   g_heroCurInfo_TallentId_Select = 0
end 

function HeroCurInfoSecond_CreateHeroCurLayout(heroEntity)
   local widget = UI_CreateBaseWidgetByFileName("HeroSystemSecond.json",EUIOpenAction_FadeIn,0.5)
   local function HeroSystem_ClickClose(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then 
         UI_CloseCurBaseWidget(EUICloseAction_FadeOut, 0.25)
         g_HeroSystem_IsHeroSystem = false
      end 
   end 
   UI_GetUIButton(widget, 1):addTouchEventListener(HeroSystem_ClickClose)

   local function HeroSystem_ClickReturn(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then 
         UI_CloseCurBaseWidget(EUICloseAction_FadeOut, 0.25)
         HeroListSecond_CreateHeroListLayout()
         g_HeroSystem_IsHeroSystem = false
      end 
   end 
   UI_GetUIButton(widget, 2):addTouchEventListener(HeroSystem_ClickReturn)
   HeroCurInfoSecond_InitValue()

   UI_GetUILayout(widget, 4):setPosition(ccp(UI_GetUILayout(widget, 9):getPosition()))
   UI_GetUILayout(widget, 4):setVisible(true)
   HeroCurInfoSecond_HeroBaseInfoShow(widget,heroEntity)
   HeroCurInfoSecond_HeroUpdateStar(widget,heroEntity)
   HeroCurInfoSecond_HeroLeftAndRightButtonFunc(widget)
   HeroCurInfoSecond_UpdateButton(widget)
end 

function HeroCurInfoSecond_HeroLeftAndRightButtonFunc(widget)
   local leftButtonImage = UI_GetUIImageView(widget,12)
   local rightButtonImage = UI_GetUIImageView(widget,11)
   UI_GetFadeInAndOutAction(leftButtonImage)
   UI_GetFadeInAndOutAction(rightButtonImage)

   --left
   local function ClickLeftImageFunc(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then 
         HeroCurInfoSecond_HeroLeftImageFunc()
      end 
   end 
   UI_GetUILayout(widget,14):setTouchEnabled(true)
   UI_GetUILayout(widget,14):addTouchEventListener(ClickLeftImageFunc)

   --right 
   local function ClickRightImageFunc(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then
		 if g_isGuide then
			Guide_GoNext()
		 end
	  
         heroCurInfoSecond_HeroRightImageFunc()
      end 
   end 
   UI_GetUILayout(widget,13):setTouchEnabled(true)
   UI_GetUILayout(widget,13):addTouchEventListener(ClickRightImageFunc)
end 

function HeroCurInfoSecond_HeroLeftImageFunc()
   --local herolist = GetLocalPlayer():GetHeroList(false)
   --if g_herolistClickIndex == 0 then 
      --g_herolistClickIndex = herolist:size() - 1
   --else  
      --g_herolistClickIndex = g_herolistClickIndex - 1
   --end 
   --local heroEntity = 
   local cansummondHerolist = vector_uint_:new_local()
   GetLocalPlayer():GetCanSummonHeroList(cansummondHerolist)
   local cansummondHeroSize = cansummondHerolist:size()
   local playerherolist = vector_uint_:new_local()
   GetLocalPlayer():GetCanSummonHeroAndHeroList(playerherolist)

   if g_herolistClickIndex == cansummondHeroSize then 
      g_herolistClickIndex = playerherolist:size() - 1
   else 
      g_herolistClickIndex = g_herolistClickIndex - 1
   end
   local heroEntity = HeroListSecond_GetHeroEntityByIndex()
   HeroSystem_heroViewScroll_ChangeView(heroEntity)

end 

function heroCurInfoSecond_HeroRightImageFunc()
   local cansummondHerolist = vector_uint_:new_local()
   GetLocalPlayer():GetCanSummonHeroList(cansummondHerolist)
   local cansummondHeroSize = cansummondHerolist:size()
   local playerherolist = vector_uint_:new_local()
   GetLocalPlayer():GetCanSummonHeroAndHeroList(playerherolist)

   if g_herolistClickIndex == playerherolist:size() - 1 then 
      g_herolistClickIndex = cansummondHeroSize
   else 
      g_herolistClickIndex = g_herolistClickIndex + 1
   end
   local heroEntity = HeroListSecond_GetHeroEntityByIndex()
   HeroSystem_heroViewScroll_ChangeView(heroEntity)
end

function HeroSystem_heroViewScroll_ChangeView(heroEntity)
   local widget = UI_GetBaseWidgetByName("HeroSystemSecond")
   if UI_GetUILayout(widget,4):isVisible() then 
      --喜好品
      --HeroCurInfoSecond_ShowCurLayout(widget,heroEntity) 
      HeroCurInfoSecond_HeroBaseInfoShow(widget,heroEntity)
      HeroCurInfoSecond_HeroUpdateStar(widget,heroEntity)
      HeroCurInfoSecond_HeroLeftAndRightButtonFunc(widget)
   elseif UI_GetUILayout(widget,5):isVisible() then
      --升级
      local tab = GameServer_pb.Cmd_Cs_QueryHeroLevelInfo()
	  tab.dwHeroObjectID = heroEntity:GetUint(EHero_ID)
	  Packet_Full(GameServer_pb.CMD_QUERY_HERO_LEVELINFO,tab)
      ShowWaiting()
   elseif UI_GetUILayout(widget,6):isVisible() then 
      --属性
      local tab = GameServer_pb.Cmd_Cs_GetHeroDesc()
      tab.dwObjectID = heroEntity:GetUint(EHero_ID)
	  Packet_Full(GameServer_pb.CMD_GET_HERODESC, tab)
	  ShowWaiting()
   elseif UI_GetUILayout(widget,7):isVisible() then 
      --技能
      HeroCurInfoSecond_HeroTallentSkillShow(widget,heroEntity)
      HeroCurInfoSecond_HeroBaseInfoShow(widget,heroEntity)
      HeroCurInfoSecond_HeroUpdateStar(widget,heroEntity)
      HeroCurInfoSecond_HeroLeftAndRightButtonFunc(widget)
   elseif UI_GetUILayout(widget,8):isVisible() then 
      --装备
      NewHeroEquip_QuerHeroEquip(heroEntity) 
      HeroCurInfoSecond_HeroBaseInfoShow(widget,heroEntity)
      HeroCurInfoSecond_HeroUpdateStar(widget,heroEntity)
      HeroCurInfoSecond_HeroLeftAndRightButtonFunc(widget)
   end 
end  

function HeroCurInfoSecond_HeroUpdateStar(widget,heroEntity)
   local heroId = heroEntity:GetUint(EHero_HeroId)
   local herodata = GetGameData(DataFileHeroBorn,heroId,"stHeroBornData")
   local itemId = herodata.m_heroSoulItemId
   local playerhaveNumber = GetLocalPlayer():GetPlayerBag():GetItemCountByItemId(itemId)
   local pingzi = heroEntity:GetUint(EHero_LvlStep)
   local qualitydata = GetGameData(DataFileHeroLevelStepGrow, pingzi, "stLevelStepGrowData")
   Log("pingzi===="..pingzi)
   Log("costnumber==="..qualitydata.m_CostNumber)
   local updateNeedNumber = qualitydata.m_CostNumber
   local function BesureUpdateStar()
      local tab = GameServer_pb.CMD_HERO_LEVELSTEPUP_CS()
      tab.dwHostHeroID = heroEntity:GetUint(EHero_ID)
      Packet_Full(GameServer_pb.CMD_HERO_LEVELSTEPUP,tab)
      ShowWaiting()
   end 
   local function ClickUpdateStar(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then 
         if playerhaveNumber < updateNeedNumber then 
             createPromptBoxlayout(FormatString("HeroInfoSecond_HaveNoEnoughSoul"))
         else    
             Messagebox_Create({info = FormatString("HeroInfoSecond_UpdateStarCost",qualitydata.m_needSliver),title = FormatString("ScatteredPower_messagetitle"), msgType = EMessageType_LeftRight, leftFun = BesureUpdateStar})  
         end 
      end 
   end 
   local layout = UI_GetUILayout(widget,3)
   local updatestarButton = UI_GetUIButton(layout,6) 
   updatestarButton:addTouchEventListener(ClickUpdateStar)
   UI_GetUILabel(layout,9):enableStroke()
   if heroEntity:GetUint(EHero_LvlStep) == 4 then 
      UI_GetUILabel(layout,9):setText(FormatString("HeroInfoSecond_StarIsMax"))
      updatestarButton:setVisible(false)
   else 
      UI_GetUILabel(layout,9):setText(playerhaveNumber.."/"..updateNeedNumber)
      updatestarButton:setVisible(true)
   end
   if playerhaveNumber >= updateNeedNumber then 
      UI_GetUILoadingBar(layout,8):setPercent(100)    
      if updatestarButton:getNodeByTag(1000) then 
         updatestarButton:removeNodeByTag(1000)
      end 
      local animaaction = GetUIArmature("anniutishi")
      updatestarButton:addNode(animaaction,1000,1000)
      animaaction:getAnimation():playWithIndex(0)
      animaaction:setPosition(ccp(0,5))
   else
      if updatestarButton:getNodeByTag(1000) then 
         updatestarButton:removeNodeByTag(1000)
      end 
      UI_GetUILoadingBar(layout,8):setPercent(playerhaveNumber / updateNeedNumber * 100) 
   end
   
   --魂魄来源
   local function ClickLookHeroSoulResouceFunc(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then
          --HeroCurInfoSecond_LookHeroSoulSource(heroEntity)
          local tab = GameServer_pb.CMD_QUERY_EQUIPSOURCE_CS()
          tab.iEquipID = itemId
          Packet_Full(GameServer_pb.CMD_QUERY_EQUIPSOURCE,tab)
          ShowWaiting()
      end 
   end 
   UI_GetUIButton(layout,7):addTouchEventListener(ClickLookHeroSoulResouceFunc)  
end 

function HeroSoulSour_GetHeroSoulInit(info)
   HeroCurInfoSecond_LookHeroSoulSource(info)
end 

function HeroCurInfoSecond_CheckSourceIndex(number)
   --1.商城招募 2.商城—荣誉兑换 3.武魂殿 4.公会商店兑换 5.关卡
   if number == 1001 then 
      return 1
   elseif number == 1002 then 
      return 2
   elseif number == 1003 then 
      return 3
   elseif number == 1004 then 
      return 4
   else  
      return 5
   end 
end 

function HeroCurInfoSecond_GetSourceIconAndString(number)
   local icon = nil 
   local src = ""
   if number == 1001 then 
      icon = "zhujiemian/shangcheng_1.png"
	  src = FormatString("HeroSoulSouce_Shop")
   elseif number == 1002 then 
      icon = "zhujiemian/jinjichang_1.png"
	  src = FormatString("HeroSoulSouce_shopchange") 
   elseif number == 1003 then 
      icon = "zhujiemian/lingdi_1.png"
	  src = FormatString("HeroSoulSouce_lingdi")
   elseif number == 1004 then 
      return 4
   else 
      Log("lua error=== cehua biao cuo l ")   
   end
   return icon,src
end 

function HeroCurInfoSecond_LookHeroSoulSource(info)
   local widget = UI_CreateBaseWidgetByFileName("HeroSoulSource.json", EUIOpenAction_FadeIn,0.5)
   local data = GetGameData(DataFileItem,info.iEquipID,"stItemData")
   --image
   UI_GetHeroSoulIcon(data.m_icon,UI_GetUIImageView(widget, 10),1)

   UI_SetLabelText(widget, 4, data.m_name)
   
   --list
   local sourlist = UI_GetUIListView(widget,11)
   local sourlistwidth = sourlist:getContentSize().width 
   local sourlistheight = sourlist:getContentSize().height
   local layout = nil 
   local curlayout = nil 
   local src = vector_int_:new_local()
   GetLocalPlayer():GetHeroQualityFavoriteItemId(info.strSpecialSource,src)

   local function clickSoulNotFuben(index,layout)
      Log("xxxxxxxxxxxxxxxxxxxxxxxxxxxxssss=="..index)
      local function ClickLayoutFunc(sender,eventType)
         Log("xxxxxxx=="..index)
         if eventType == TOUCH_EVENT_ENDED then 
            playUiEffect()
            if index == 1001 then 
               UI_CloseAllBaseWidget()
               Shop_Create()
            elseif index == 1002 then 
               if GetLocalPlayer():GetInt(EPlayer_Lvl) < 9 then 
                  createPromptBoxlayout(FormatString("HeroSoulSouce_shopchangeNotOpen"))
               else 
                  UI_CloseAllBaseWidget()
                  Shop_CreateExchange()
               end 
            elseif index == 1003 then
               Packet_Cmd(GameServer_pb.CMD_MANOR_QUERY);
               ShowWaiting(); 
            elseif index == 1004 then 
            end 
         end 
      end 
      layout:setTouchEnabled(true)
      layout:addTouchEventListener(ClickLayoutFunc)
   end 

   local function clickHeroHeroFavoriteSource(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then
         local sender = tolua.cast(sender,"Layout")
         local index = sourlist:getIndex(sender)
         Log("index ==="..index)
         if info.szEquipSource[index + 1].bOpened then
            UI_CloseAllBaseWidget()
            g_selectSectionID = info.szEquipSource[index + 1].iSectionID
		    g_selectiSceneID = info.szEquipSource[index + 1].iSceneID
			local tab = GameServer_pb.Cmd_Cs_QueryDSectionDetail()
			tab.iSectionID = g_selectSectionID
			Packet_Full(GameServer_pb.CMD_QUERY_DSECTIONDETAIL, tab)
			ShowWaiting()
         else 
            createPromptBoxlayout(FormatString("NewHeroSystem_HeroTallentItemSourceTwo")) 
         end 
      end 
   end

   for i = 1 ,#info.szEquipSource do 
      curlayout = UI_GetCloneLayout(UI_GetUILayout(widget,2))
      curlayout:setVisible(true)
	  local SectionID = info.szEquipSource[i].iSectionID
	  local SceneID = info.szEquipSource[i].iSceneID
	  Log("SceneID==="..SceneID)
      Log("SectionID==="..SectionID)
      local sceneSectionData = GetGameData(DataFileSceneSection, SectionID, "stSceneSectionData")
      local sceneData = GetGameData(DataFileScene,SceneID,"stSceneData")
      --image 
      local image = UI_GetUIImageView(curlayout,1)
      --local height = layout:getContentSize().height
      if sceneData.m_icon >= 10 then 
         image:loadTexture("Dungeon/Bnt_dungeon_0"..sceneData.m_icon..".png")
      else 
         image:loadTexture("Dungeon/Bnt_dungeon_00"..sceneData.m_icon..".png")
      end 
      --name 
      UI_SetLabelText(curlayout, 2, sceneSectionData.m_nameFirst.." "..sceneSectionData.m_name )

      --关卡
      local isOpened = info.szEquipSource[i].bOpened
      local guanqianame = sceneData.m_name
      if isOpened == false then 
         guanqianame = guanqianame..FormatString("NewHeroSystem_HeroTallentItemSource")
      end 
      UI_SetLabelText(curlayout, 3,guanqianame)
      if isOpened == false then
        --UI_GetUILabel(layout, 2):setColor(ccc3(255,0,0)) 
        UI_GetUILabel(curlayout, 3):setColor(ccc3(255,0,0)) 
      end 
      curlayout:addTouchEventListener(clickHeroHeroFavoriteSource)
      sourlist:pushBackCustomItem(curlayout)
   end 
   for i = 0,src:size()- 1 do 
--      if HeroCurInfoSecond_CheckSourceIndex(src[i]) ~= 5 then
         layout = UI_GetCloneLayout(UI_GetUILayout(widget,1))
         local icon,srcstring = HeroCurInfoSecond_GetSourceIconAndString(src[i])
         UI_GetUIImageView(layout,2):loadTexture(icon)	 
		 UI_SetLabelText(layout,1,srcstring)
         layout:setVisible(true)
         clickSoulNotFuben(src[i],layout) 
         sourlist:pushBackCustomItem(layout)
   end 
--   for i = 1, #info

--   --click
--   --商店
--   local function clickShop(sender,eventType)
--      if eventType == TOUCH_EVENT_ENDED then
--          UI_CloseAllBaseWidget()
--          Shop_Create()
--      end 
--   end 
--   UI_GetUILayout(widget,1):setTouchEnabled(true)
--   UI_GetUILayout(widget,1):addTouchEventListener(clickShop)

--   --荣誉
--   local function clickRongyuShop(sender,eventType)
--      if eventType == TOUCH_EVENT_ENDED then
--          UI_CloseAllBaseWidget()
--          Shop_Create()
--          Shop_getDataExchagne()
--      end 
--   end 
--   UI_GetUILayout(widget,2):setTouchEnabled(true)
--   UI_GetUILayout(widget,2):addTouchEventListener(clickRongyuShop)

--   --领地
--   local function clickLingdi(sender,eventType)
--      if eventType == TOUCH_EVENT_ENDED then
--         Packet_Cmd(GameServer_pb.CMD_MANOR_QUERY);
--	     ShowWaiting();
--      end 
--   end 
--   UI_GetUILayout(widget,3):setTouchEnabled(true)
--   UI_GetUILayout(widget,3):addTouchEventListener(clickLingdi)
end 

function HeroCurInfoSecond_HeroBaseInfoShow(widget,heroEntity)
   HeroCurInfoSecond_HeroAnimaAction(widget,heroEntity)
   HeroCurInfoSecond_HeroNameInfo(widget,heroEntity)
   HeroCurInfoSecond_HeroCurType(widget,heroEntity)
   HeroCurInfoSecond_HeroFighting(widget,heroEntity)
   HeroCurInfoSecond_HeroSoulInfo(widget,heroEntity)
   HeroCurInfoSecond_HeroFavoriteInitShow(widget,heroEntity)
   HeroCurInfoSecond_HeroPanalButton(widget)
end 

function HeroCurInfoSecond_HeroInfoAnimEnd(armature, movementType, movementID) 
   if (movementType == LOOP_COMPLETE)then
       if (movementID == "0")then
           local rand = Random:RandomInt(1, 100)
           if ( rand < 20)then
               armature:getAnimation():playWithIndex(2)
           elseif (rand < 40 and rand >= 20) then 
               armature:getAnimation():playWithIndex(7)
           end
       elseif (movementID == "2") or (movementID == "7") then
            armature:getAnimation():playWithIndex(0)
       end
   end
end 

function HeroCurInfoSecond_HeroAnimaAction(widget,heroEntity)
   local data = GetGameData(DataFileHeroBorn,heroEntity:GetUint(EHero_HeroId),"stHeroBornData")
   local heroinfolayout = UI_GetUILayout(widget, 3)
   local heroAnimallayout = UI_GetUILayout(heroinfolayout, 1) 
   heroAnimallayout:removeAllNodes()
   local heroAnimalName = data.m_animName
   local heroanimatrue = GetArmature(heroAnimalName)
   heroAnimallayout:addNode(heroanimatrue)
   heroanimatrue:setPosition(ccp(heroAnimallayout:getContentSize().width / 2, 20))
   --heroanimatrue:setScale(0.8)
   heroanimatrue:getAnimation():playWithIndex(0)
   heroanimatrue:getAnimation():setMovementEventCallFunc(HeroCurInfoSecond_HeroInfoAnimEnd)
   HeroCurInfoSecond_HeroPanalButton(widget,heroEntity)
end 

function HeroCurInfoSecond_StarViewShow(layout,tagTable,levelstep)
   for i = 1,#tagTable do 
      if i <= levelstep then 
         UI_GetUIImageView(layout, i):setVisible(true)
      else 
         UI_GetUIImageView(layout, i):setVisible(false)
      end 
   end 
end 

function HeroCurInfoSecond_HeroNameInfo(widget,heroEntity)
   local data = GetGameData(DataFileHeroBorn,heroEntity:GetUint(EHero_HeroId),"stHeroBornData")
   local heroinfolayout = UI_GetUILayout(widget, 3)
   
   --name
   local namelayout = UI_GetUILayout(heroinfolayout, 10)
   namelayout:removeAllChildren()
   local namelab = GetHeroNameByLabel(data.m_name,heroEntity:GetUint(EHero_Quality),24)
   --namelab:setAnchorPoint(ccp(0.0,0.0))
   namelab:setPosition(ccp(namelayout:getContentSize().width / 2,namelayout:getContentSize().height / 2))
   namelayout:addChild(namelab)

   --levelsetp
   local startag = {1,2,3,4,5}
   local levelstep = heroEntity:GetUint(EHero_LvlStep) + 1
   HeroCurInfoSecond_StarViewShow(UI_GetUILayout(heroinfolayout, 12),startag,levelstep)

   --
   UI_GetHeroLevelStepIcon(heroinfolayout,11,heroEntity:GetUint(EHero_Quality))
   --lvl
   UI_SetLabelText(heroinfolayout:getChildByTag(14), 13, heroEntity:GetUint(EHero_Lvl))

end 

function HeroCurInfoSecond_HeroCurType(widget,heroEntity)
   local data = GetGameData(DataFileHeroBorn,heroEntity:GetUint(EHero_HeroId),"stHeroBornData")
   local heroinfolayout = UI_GetUILayout(widget, 3)
   local herocurTypelayout = UI_GetUILayout(heroinfolayout, 14)
   
   UI_GetHeroTypeIconByTag(herocurTypelayout,1,data.m_heroType)
   UI_SetLabelText(herocurTypelayout, 2, UI_GetHeroTypeName(data.m_heroType))
   UI_GetUILoadingBar(herocurTypelayout, 3):setPercent(data.m_attckability)
   UI_GetUILoadingBar(herocurTypelayout, 4):setPercent(data.m_defenseability)
   UI_GetUILoadingBar(herocurTypelayout, 5):setPercent(data.m_lifeability)
   
   UI_SetLabelText(herocurTypelayout, 6, FormatString("HeroType_Position",data.m_heroPositioning))
   UI_SetLabelText(herocurTypelayout, 7, FormatString("HeroType_Features",data.m_herotrait))

end 

function HeroCurInfoSecond_HeroFighting(widget,heroEntity)
   local data = GetGameData(DataFileHeroBorn,heroEntity:GetUint(EHero_HeroId),"stHeroBornData")
   local heroinfolayout = UI_GetUILayout(widget, 3)
   local fightinglayout = UI_GetUILayout(heroinfolayout, 14)  
   UI_GetUILabel(fightinglayout, 20):setText(heroEntity:GetUint(EHero_FightValue))
   --UI_GetUILabel(fightinglayout, 20):enableStroke()
   --UI_GetUILabel(fightinglayout, 4747036):enableStroke()
end 

function HeroCurInfoSecond_HeroSoulInfo(widget,heroEntity)
   --local data = GetGameData(DataFileHeroBorn,heroEntity:GetUint(EHero_HeroId),"stHeroBornData")
   --local herosoulId = data.m_heroSoulItemId
   --local heroinfolayout = UI_GetUILayout(widget, 3)
   --local needNumber = data.m_heroNeedSoulNumber
   --local playerhaveNumber = GetLocalPlayer():GetPlayerBag():GetItemCountByItemId(herosoulId)
   --UI_GetUILoadingBar(heroinfolayout, 8):setPercent(playerhaveNumber / needNumber * 100)
   --UI_SetLabelText(heroinfolayout, 9, playerhaveNumber.."/"..needNumber)
end 

function HeroCurInfoSecond_IsCanSummond(favoriteId)
   if GetLocalPlayer():GetPlayerBag():GetItemCountByItemId(favoriteId) > 0 then 
      return false
   end
   return true 
end 

function LookHeroFavoriteInfo(layout,state,favoriteId)
   local function LookFavoriteInfo(sender,eventType)
       if eventType == TOUCH_EVENT_ENDED then
          local sender = tolua.cast(sender,"ImageView")
          local tag = sender:getTag()
          Log("tag===="..tag) 
          g_heroClickfavoriteIndex = tag - 1
          HeroFavorite_InitLayout(favoriteId,state)
       end 
   end 
   layout:setTouchEnabled(true)
   layout:addTouchEventListener(LookFavoriteInfo)
end 

function HeroCurInfoSecond_HeroFavoriteInitShow(widget,heroEntity)
   local favoritelayout = UI_GetUILayout(UI_GetUILayout(widget, 4), 1)
   local favoriteTag = {1,2,3,4,5,6}
   local favoritelist = heroEntity:GetHeroFavorite():GetHeroFavoriteState()
   local allfavoritelist = heroEntity:GetHeroFavorite():GetHeroFavoriteId()
  
   --1.没有 2.已经装备 3.背包有 4.可以合成 5.等级不够
   for i = 0, favoritelist:size() - 1 do
      local parentImage = UI_GetUIImageView(favoritelayout, favoriteTag[i + 1])
      local canimage = UI_GetUIImageView(parentImage, 3)
      local cannotimage = UI_GetUIImageView(parentImage, 4)
      local strlab = UI_GetUILabel(parentImage, 5)
      local favoriteState;
      local itemId = allfavoritelist[i]
      UI_GetUIImageView(parentImage, 1):setColor(ccc3(255,255,255)) 
     -- parentImage:setColor(ccc3(255,255,255))
      local data = GetGameData(DataFileItem,itemId,"stItemData")
      UI_GetUIImageView(parentImage, 1):loadTexture("Icon/Item/"..data.m_icon..".png")
      if favoritelist[i] ~= HaveEquipFavorite then
         UI_GetUIImageView(parentImage,2):loadTexture("Common/Icon_Bg_011.png")
         UI_GetUIImageView(parentImage, 1):setColor(ccc3(125,125,125)) 
         --parentImage:setColor(ccc3(125,125,125)) 
         --UI_IconFrameWithSkillOrItem(parentImage,2,nil,dat)
         Log("ssssssssssssssssssssss======"..favoritelist[i])     
      else 
         --frame
         UI_IconFrameWithSkillOrItem(parentImage,2,nil,data.m_quality)
      end 
      if favoritelist[i] == NoHaveHeroFavorite then 
         canimage:setVisible(false)
         cannotimage:setVisible(false)
         strlab:setVisible(true)
         strlab:setText(FormatString("HeroFavorite_Nothave"))
         strlab:setColor(ccc3(255,230,40))
         favoriteState = 1
      elseif favoritelist[i] == HaveEquipFavorite then 
         canimage:setVisible(false)
         cannotimage:setVisible(false)
         strlab:setVisible(false)
         favoriteState = 2
      elseif favoritelist[i] == CanEquipFavorite then 
         canimage:setVisible(true)
         cannotimage:setVisible(false)
         strlab:setVisible(true)
         if HeroCurInfoSecond_IsCanSummond(itemId) then 
            strlab:setText(FormatString("HeroFavorite_canSum"))
            strlab:setColor(ccc3(0,255,0))
            favoriteState = 4
         else 
            strlab:setText(FormatString("HeroFavorite_canEq"))
            strlab:setColor(ccc3(0,255,0))
            favoriteState = 3
         end 
      elseif favoritelist[i] == CannotEquipFavorite then 
         canimage:setVisible(false)
         cannotimage:setVisible(true)
         strlab:setVisible(true)
         strlab:setText(FormatString("HeroFavorite_cannot"))
         strlab:setColor(ccc3(255,230,40))
         favoriteState = 5
      end
      LookHeroFavoriteInfo(parentImage,favoriteState,itemId)
   end 

   --判断是否可以进阶
   local function ClickHeroAdvanced(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then 
         if GetLocalPlayer():HeroHaveAllEquipFavorite(heroEntity:GetUint(EHero_ID)) then 
            local tab = GameServer_pb.CMD_HERO_QUALITY_UP_CS()
            tab.dwHeroObjectID = heroEntity:GetUint(EHero_ID)
            Packet_Full(GameServer_pb.CMD_HERO_QUALITY_UP,tab)
            ShowWaiting()
         else 
            createPromptBoxlayout(FormatString("HeroAdvanced_Nothave"))
         end 
         --HeroCurInfoSecond_FavoriteUpdateAction(widget)
      end 
   end 
   local updateButton = UI_GetUIButton(favoritelayout, 7)
        
   if GetLocalPlayer():HeroHaveAllEquipFavorite(heroEntity:GetUint(EHero_ID)) then 
       if updateButton:getNodeByTag(1000) then 
          updateButton:removeNodeByTag(1000)
       end 
        local animaaction = GetUIArmature("anniutishi")
        updateButton:addNode(animaaction,1000,1000)
        animaaction:getAnimation():playWithIndex(0)
        animaaction:setPosition(ccp(0,5))
   else
       if updateButton:getNodeByTag(1000) then 
          updateButton:removeNodeByTag(1000)
       end 
   end 
   UI_GetUIButton(favoritelayout, 7):addTouchEventListener(ClickHeroAdvanced)
end 

function HeroCurInfoSecond_FavoriteUpdateAction(widget)
   local favoritelayout = UI_GetUILayout(UI_GetUILayout(widget, 4), 1)
   local favoriteTag = {1,2,3,4,5,6}
   local cloneLayout = {}
   local actionTable = {}
   local actionTwoTable = {}
   local offsetTime = 0
   local layoutPosX,layoutPosY = UI_GetUILayout(UI_GetUILayout(widget,9),1):getPosition()
   local function iscallback(sender)
      local sender = tolua.cast(sender,"Layout")
      sender:removeFromParent()
   end 
   for i = 1,#favoriteTag do 
       local layout = UI_GetCloneLayout(UI_GetUILayout(favoritelayout,favoriteTag[i]))
       local posX,PosY = UI_GetUILayout(favoritelayout,favoriteTag[i]):getPosition()
       table.insert(cloneLayout,layout)
       local action = CCMoveTo:create(0.5,ccp(layoutPosX + 100,layoutPosY + 50))
       offsetTime = offsetTime + 0.5
       local delayTime = CCDelayTime:create(offsetTime)
       local actionArry = CCArray:create()
      -- actionArry:addObject(delayTime) 
       actionArry:addObject(action)
       local seqaction = CCSequence:create(actionArry)
       table.insert(actionTable,seqaction)
       widget:addChild(layout,1000,1000)
       layout:setPosition(ccp(posX,PosY))

       offsetTimeTwo = offsetTime
       local actionArrayTwo = CCArray:create()
       local delayTimeTwo = CCDelayTime:create(offsetTimeTwo)
      -- actionArrayTwo:addObject(delayTimeTwo)
       local actionTwo = CCScaleTo:create(0.5,0.5)
       actionArrayTwo:addObject(actionTwo)
       local fadeout = CCFadeOut:create(0.25)
       actionArrayTwo:addObject(fadeout)
       local callback = CCCallFuncN:create(iscallback)
       actionArrayTwo:addObject(callback)
       local seqactionTwo = CCSequence:create(actionArrayTwo)
       table.insert(actionTwoTable,seqactionTwo)
   end
   for i = 1,#cloneLayout do 
       cloneLayout[i]:runAction(actionTable[i])
       cloneLayout[i]:runAction(actionTwoTable[i])
   end  
end 

function HeroCurInfoSecond_ShowCurLayout(widget)
   local layoutTag = {4,5,6,7,8}
   
   for i = 1,#layoutTag do 
      if UI_GetUILayout(widget, layoutTag[i]):isVisible() then 
         local positionX,positionY = UI_GetUILayout(widget, 10):getPosition()
         HeroSystem_MoveAndFadeOut(UI_GetUILayout(widget, layoutTag[i]),positionX,positionY,0.25)
      end
      if i == g_heroCurInfoLayoutState then
         UI_GetUILayout(widget, layoutTag[i]):setVisible(true)
         UI_GetUILayout(widget, layoutTag[i]):setOpacity(0)
         local positionX,positionY = UI_GetUILayout(widget, 9):getPosition()
         HeroSystem_MoveAndFadeIn(UI_GetUILayout(widget, layoutTag[i]),positionX,positionY,0.25)
      end  
   end
   HeroCurInfoSecond_UpdateButton(widget)
   --HeroSystem_UpdateButtonChange()
   --for i = 1,#layoutTag do 
   --    local isShow = nil 
   --    if i == g_HeroSystem_curlayoutstate then 
   --       isShow = true 
   --    else 
   --       isShow = false
   --    end
   --    UI_GetUILayout(widget, layoutTag[i]):setVisible(isShow)
   --end
end 

function HeroCurInfoSecond_HeroPanalButton(widget,heroEntity)
   local heroinfolayout = UI_GetUILayout(widget,3)
   local buttonTag = {4,5,16,3,2} 
   local function Hero_PannalButtonFunction(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then 
          local sender = tolua.cast(sender,"Button")
          local selectIndex = sender:getTag()
          Log("selectIndex====="..g_herolistClickIndex)
          Log("g_heroCurInfoLayoutState=="..g_heroCurInfoLayoutState)
          local slectheroEntity = HeroListSecond_GetHeroEntityByIndex()
          --g_heroCurInfoLayoutState 1.喜好品 2.升级 3.详细属性 4.技能 5.装备
          if selectIndex == buttonTag[1] then
             if g_heroCurInfoLayoutState == 1 then 
                return 
             end 
             g_heroCurInfoLayoutState = 1
             HeroCurInfoSecond_ShowCurLayout(widget,slectheroEntity) 
          elseif selectIndex == buttonTag[2] then
             if g_heroCurInfoLayoutState == 2 then 
                return 
             end 
             local tab = GameServer_pb.Cmd_Cs_QueryHeroLevelInfo()
	         tab.dwHeroObjectID = slectheroEntity:GetUint(EHero_ID)
	         Packet_Full(GameServer_pb.CMD_QUERY_HERO_LEVELINFO,tab)
             ShowWaiting()
			if (g_isGuide) then
				Guide_GoNext();
			end
             --g_heroCurInfoLayoutState = 2
          elseif selectIndex == buttonTag[3] then 
             if g_heroCurInfoLayoutState == 3 then 
                return 
             end 
             --g_heroCurInfoLayoutState = 3
             local tab = GameServer_pb.Cmd_Cs_GetHeroDesc()
             tab.dwObjectID = slectheroEntity:GetUint(EHero_ID)
	         Packet_Full(GameServer_pb.CMD_GET_HERODESC, tab)
	         ShowWaiting()
          elseif selectIndex == buttonTag[4] then 
             if g_heroCurInfoLayoutState == 4 then 
                return 
             end 
             g_heroCurInfoLayoutState = 4
             HeroCurInfoSecond_HeroTallentSkillShow(widget,slectheroEntity)
             HeroCurInfoSecond_ShowCurLayout(widget,slectheroEntity) 
			 if (g_isGuide) then
				Guide_GoNext();
			end
          elseif selectIndex == buttonTag[5] then 
             if g_heroCurInfoLayoutState == 5 then 
                return 
             end 
             --createPromptBoxlayout(FormatString("HeroInfoSecond_EquipmentNotHave"))
                
             --NewHeroEquip_ShowLayout(widget,slectheroEntity)
             if HeroEquipIsOpen() == false then 
                createPromptBoxlayout(FormatString("HeroEquip_OpenLimit"))
                return 
             end 
             NewHeroEquip_QuerHeroEquip(slectheroEntity) 
			 if (g_isGuide) then
				Guide_GoNext();
			 end

          end
          --HeroCurInfoSecond_ShowCurLayout(widget,heroEntity)   
       end 
   end 
   for i = 1, #buttonTag do 
      UI_GetUIButton(heroinfolayout, buttonTag[i]):addTouchEventListener(Hero_PannalButtonFunction)
   end
end 

function HeroCurInfoSecond_UpdateButton(widget)
   local foriteButton = nil 
   local updateButton = nil
   local infoButton = nil 
   local skillButton = nil 
   local equipButton = nil 
   local butonTag = {4,5,16,3,2} 
   --g_heroCurInfoLayoutState 1.喜好品 2.升级 3.详细属性 4.技能 5.装备
   if g_heroCurInfoLayoutState == 1 then 
      foriteButton = false
      updateButton = true 
      infoButton = true 
      skillButton = true
      equipButton = true
   elseif g_heroCurInfoLayoutState == 2 then
      foriteButton = true 
      updateButton = false 
      infoButton = true 
      skillButton = true
      equipButton = true
   elseif g_heroCurInfoLayoutState == 3 then 
      foriteButton = true 
      updateButton = true
      infoButton = false
      skillButton = true
      equipButton = true
   elseif g_heroCurInfoLayoutState == 4 then 
      foriteButton = true 
      updateButton = true 
      infoButton = true 
      skillButton = false
      equipButton = true
   elseif g_heroCurInfoLayoutState == 5 then 
      foriteButton = true 
      updateButton = true 
      infoButton = true 
      skillButton = true
      equipButton = false
   end 
   local isTouchTable = {foriteButton,updateButton,infoButton,skillButton,equipButton}
   for i = 1,#butonTag do 
      UI_GetUIButton(UI_GetUILayout(widget, 3), butonTag[i]):setButtonEnabled(isTouchTable[i])
   end 
   --UI_GetUIButton(UI_GetUILayout(widget, 2), 9):setButtonEnabled(upbuttonis)
   --UI_GetUIButton(UI_GetUILayout(widget, 2), 10):setButtonEnabled(descbuttonis)
end 

--[[" 
      herocurinfo界面第二版 End
"]]



--[[" 
      英雄喜好品 Begin
"]]

--记录记录点击的喜好品ID
if g_heroFavorite_MaterialIdTable == nil then 
   g_heroFavorite_MaterialIdTable = {}
end 

function HeroFavorite_IsBaseFavorite(favoriteId)
   local data = GetGameData(DataFileFavorite,favoriteId,"stFavoriteData")
   if data == nil then 
      return true 
   end 
   return false
end 

function HeroFavorite_InitValue()
   g_heroFavorite_MaterialIdTable = {}
end 

--1.没有 2.已经装备 3.背包有 4.可以合成 5.等级不够
function HeroFavorite_InitLayout(favoriteId,favoriteState)
   local widget = UI_CreateBaseWidgetByFileName("HeroSystemLoveThingOne.json",EUIOpenAction_FadeIn,0.5)
   local function HeroSystem_ClickReturn(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then 
         UI_CloseCurBaseWidget(EUICloseAction_FadeOut, 0.25)
      end 
   end 
   HeroFavorite_InitValue()
   table.insert(g_heroFavorite_MaterialIdTable,favoriteId)
   Log("favoriteState==="..favoriteState)
   UI_GetUILayout(widget, 6):addTouchEventListener(HeroSystem_ClickReturn)
   HeroFavorite_BaseInfoShow(widget,favoriteId)

   UI_GetUILabel(UI_GetUILayout(widget, 1), 7):enableStroke()
   UI_GetUILabel(UI_GetUILayout(widget, 1), 5):enableStroke()
   if favoriteState == 1 then 
      HeroFavorite_NotHaveEquip(widget,favoriteId)
   elseif favoriteState == 2 then
      HeroFavorite_HaveEquip(widget,favoriteId)
   elseif favoriteState == 3 then
      HeroFavorite_BagHaveFavoriet(widget,favoriteId)
   elseif favoriteState == 4 then 
      HeroFavorite_CanSummond(widget,favoriteId)
   elseif favoriteState == 5 then 
      HeroFavorite_LevelIsLimit(widget,favoriteId)
   end 
end

--baseinfo 
function HeroFavorite_BaseInfoShow(widget,favoriteId)
   local layout = UI_GetUILayout(widget, 1)
   local data = GetGameData(DataFileItem,favoriteId,"stItemData")
   --frame
   local parentImage = UI_GetUIImageView(layout, 1)
   UI_IconFrameWithSkillOrItem(parentImage,3,nil,data.m_quality)
   --icon
   UI_GetUIImageView(parentImage, 2):loadTexture("Icon/Item/"..data.m_icon..".png")
   --name
   UI_SetLabelText(layout, 10, data.m_name)
   --number
   local number = GetLocalPlayer():GetPlayerBag():GetItemCountByItemId(favoriteId)
   UI_SetLabelText(layout,2,number)
   UI_GetUILabel(layout,2):enableStroke()
   --position
   local posX,posY = UI_GetUILabel(layout, 2):getPosition()
   UI_GetUILabel(layout, 3):setPosition(ccp(posX + UI_GetUILabel(layout, 2):getContentSize().width + 5,posY))
   --miaoshu
   UI_SetLabelText(layout, 4, data.m_attinfo)
   --描述
   local infolablayout = UI_GetUILayout(layout,20)
   local lablayoutWidth = infolablayout:getContentSize().width 
   local infolab = CompLabel:GetCompLabel(FormatString("HeroInfoSecond_favoriteCurInfo",data.m_desc),lablayoutWidth,kCCTextAlignmentLeft)
   infolablayout:addChild(infolab)
   local infolabHeight = infolab:getSize().height
   infolab:setPosition(ccp(0,infolabHeight + 10))
end  

--没有
function HeroFavorite_NotHaveEquip(widget,favoriteId)
   local layout = UI_GetUILayout(widget, 1)
   UI_GetUIButton(layout, 6):setVisible(false)
   UI_GetUIButton(layout, 9):setVisible(false)
   UI_GetUILabel(layout, 5):setVisible(false)
   UI_GetUILabel(layout, 7):setVisible(true)
   local itemdata = GetGameData(DataFileItem,favoriteId,"stItemData")
    if GetLocalPlayer():GetUint(EPlayer_Lvl) < itemdata.m_lvLimits then 
       UI_SetLabelText(layout,7,FormatString("HeroFavorite_cannotEq",itemdata.m_lvLimits))
       UI_GetUILabel(layout, 7):setColor(ccc3(255,0,0))
    else 
       UI_SetLabelText(layout,7,FormatString("HeroFavorite_cannotEq",itemdata.m_lvLimits))
       UI_GetUILabel(layout, 7):setColor(ccc3(0,255,0))
    end 
   local function ClickSource(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then 
         --HeroFavorite_MoveAction(widget,favoriteId)
         local tab = GameServer_pb.CMD_QUERY_EQUIPSOURCE_CS()
         tab.iEquipID = favoriteId
         Packet_Full(GameServer_pb.CMD_QUERY_EQUIPSOURCE,tab)
         ShowWaiting()
         UI_GetUIButton(layout, 12):setVisible(false)
         UI_GetUIButton(layout,6):setVisible(true)
         UI_GetUIButton(layout,6):setTouchEnabled(false)
      end 
   end 

   local function ClickSummond(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then 
         HeroFavorite_MoveAction(widget,favoriteId)
         UI_GetUIButton(layout,8):setVisible(false)
         UI_GetUIButton(layout,6):setVisible(true)
         UI_GetUIButton(layout,6):setTouchEnabled(false)
      end 
   end 
   --判断是否基本喜好品
   if HeroFavorite_IsBaseFavorite(favoriteId) then 
      UI_GetUIButton(layout, 12):setVisible(true)
      UI_GetUIButton(layout, 8):setVisible(false)
      UI_GetUIButton(layout, 12):addTouchEventListener(ClickSource)
   else 
      UI_GetUIButton(layout, 12):setVisible(false)
      UI_GetUIButton(layout, 8):setVisible(true)
      UI_GetUIButton(layout, 8):addTouchEventListener(ClickSummond)
   end 
end 

--已经装备
function HeroFavorite_HaveEquip(widget,favoriteId)
    local layout = UI_GetUILayout(widget, 1)
    UI_GetUIButton(layout, 6):setVisible(false)
    UI_GetUIButton(layout, 9):setVisible(true)
    UI_GetUILabel(layout, 5):setVisible(false)
    UI_GetUILabel(layout, 7):setVisible(false)
    UI_GetUIButton(layout, 12):setVisible(false)
    UI_GetUIButton(layout, 8):setVisible(false)
    local function CloseCurBaseLayout(sender,eventType)
       if eventType == TOUCH_EVENT_ENDED then 
          UI_CloseCurBaseWidget(EUICloseAction_FadeOut, 0.25)
       end 
    end 
    UI_GetUIButton(layout, 9):addTouchEventListener(CloseCurBaseLayout)
end 

--背包有
function HeroFavorite_BagHaveFavoriet(widget,favoriteId)
    local layout = UI_GetUILayout(widget, 1)
    UI_GetUIButton(layout, 6):setVisible(true)
    UI_GetUIButton(layout, 9):setVisible(false)
    UI_GetUILabel(layout, 5):setVisible(true)
    UI_GetUILabel(layout, 7):setVisible(false)
    UI_GetUIButton(layout, 12):setVisible(false)
    UI_GetUIButton(layout, 8):setVisible(false)

    local function EquipHeroFavorite(sender,eventType)
       if eventType == TOUCH_EVENT_ENDED then 
          local heroEntity = HeroListSecond_GetHeroEntityByIndex()
          local tab = GameServer_pb.CMD_HERO_EQUIP_FAVORITE_CS()
          tab.dwHeroObjectID = heroEntity:GetUint(EHero_ID)
          tab.iItemID = favoriteId
          tab.iPos = g_heroClickfavoriteIndex
          Packet_Full(GameServer_pb.CMD_HERO_EQUIP_FAVORITE, tab)
          ShowWaiting()
       end 
    end 
    UI_GetUIButton(layout, 6):addTouchEventListener(EquipHeroFavorite)
end 

--可以合成
function HeroFavorite_CanSummond(widget,favoriteId)
    local layout = UI_GetUILayout(widget, 1)
    UI_GetUIButton(layout, 6):setVisible(false)
    UI_GetUIButton(layout, 9):setVisible(false)
    
    UI_GetUIButton(layout, 12):setVisible(false)
    UI_GetUIButton(layout, 8):setVisible(true)
    local function ClickSummond(sender,eventType)
       if eventType == TOUCH_EVENT_ENDED then 
          HeroFavorite_MoveAction(widget,favoriteId)
          UI_GetUIButton(layout,8):setVisible(false)
          UI_GetUIButton(layout,6):setVisible(true)
          UI_GetUIButton(layout,6):setTouchEnabled(false)
       end 
    end 
    local itemdata = GetGameData(DataFileItem,favoriteId,"stItemData")
    if GetLocalPlayer():GetUint(EPlayer_Lvl) < itemdata.m_lvLimits then 
       UI_SetLabelText(layout,7,FormatString("HeroFavorite_cannotEq",itemdata.m_lvLimits))
       UI_GetUILabel(layout, 7):setColor(ccc3(255,0,0))
       UI_GetUILabel(layout, 5):setVisible(false)
       UI_GetUILabel(layout, 7):setVisible(true)
    else 
       UI_SetLabelText(layout,7,FormatString("HeroFavorite_cannotEq",itemdata.m_lvLimits))
       UI_GetUILabel(layout, 7):setColor(ccc3(0,255,0))
       UI_GetUILabel(layout, 5):setVisible(true)
       UI_GetUILabel(layout, 7):setVisible(false)
    end 
    UI_GetUIButton(layout, 8):addTouchEventListener(ClickSummond)
end 

--等级不够
function HeroFavorite_LevelIsLimit(widget,favoriteId)
    local layout = UI_GetUILayout(widget, 1)
    UI_GetUIButton(layout, 9):setVisible(false)
    UI_GetUILabel(layout, 5):setVisible(false)
    UI_GetUIButton(layout, 12):setVisible(false)
    UI_GetUILabel(layout, 7):setVisible(true)
    local itemdata = GetGameData(DataFileItem,favoriteId,"stItemData")
    UI_SetLabelText(layout,7,FormatString("HeroFavorite_cannotEq",itemdata.m_lvLimits))
    if GetLocalPlayer():GetPlayerBag():GetItemCountByItemId(favoriteId) > 0 then
       UI_GetUIButton(layout, 8):setVisible(false)
       UI_GetUIButton(layout, 6):setVisible(true)
       if itemdata.m_lvLimits > GetLocalPlayer():GetUint(EPlayer_Lvl) then 
          UI_GetUIButton(layout,6):setTouchEnabled(false)
       else 
          UI_GetUIButton(layout,6):setTouchEnabled(true)
       end 
    else
       UI_GetUIButton(layout, 8):setVisible(true)
       UI_GetUIButton(layout, 6):setVisible(false)
    end  
    local function EquipHeroFavorite(sender,eventType)
       if eventType == TOUCH_EVENT_ENDED then 
          local heroEntity = HeroListSecond_GetHeroEntityByIndex()
          local tab = GameServer_pb.CMD_HERO_EQUIP_FAVORITE_CS()
          tab.dwHeroObjectID = heroEntity:GetUint(EHero_ID)
          tab.iItemID = favoriteId
          tab.iPos = g_heroClickfavoriteIndex
          Packet_Full(GameServer_pb.CMD_HERO_EQUIP_FAVORITE, tab)
          ShowWaiting()
       end 
    end 
    local function ClickSummond(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then 
         HeroFavorite_MoveAction(widget,favoriteId)
         UI_GetUIButton(layout,8):setVisible(false)
         UI_GetUIButton(layout,6):setVisible(true)
         UI_GetUIButton(layout,6):setTouchEnabled(false)
      end 
    end 
    UI_GetUIButton(layout, 8):addTouchEventListener(ClickSummond)
    UI_GetUIButton(layout, 6):addTouchEventListener(EquipHeroFavorite)
end 

--action
function HeroFavorite_MoveAction(widget,favoriteId)
   local oneposX,oneposY = UI_GetUILayout(widget, 4):getPosition()
   local twoposX,twoposY = UI_GetUILayout(widget, 5):getPosition()
   local moveOne = CCMoveTo:create(0.25,ccp(oneposX,oneposY))
   local moveTwo = CCMoveTo:create(0.25,ccp(twoposX,twoposY))
   UI_GetUILayout(widget, 1):runAction(moveOne)
   UI_GetUILayout(widget, 2):runAction(moveTwo)
   UI_GetUILayout(widget, 2):setVisible(true)

   local data = GetGameData(DataFileFavorite,favoriteId,"stFavoriteData")
   if data ~= nil then 
       Log("size==="..#g_heroFavorite_MaterialIdTable)
       HeroFavorite_ShowMaterialLayoutInit(widget,data.m_favoriteCount,favoriteId)
   else 
       --获得途径
       HeroFavorite_ShowMaterialLayoutInit(widget,0,favoriteId)
   end   
end 

--喜好品详情

function HeroFavorite_FadeMaterialLayoutAction(layout,time)
   layout:setOpacity(0)
   local fadein = CCFadeIn:create(time)
   layout:runAction(fadein)
end 
function HeroFavorite_ShowMaterialLayoutInit(widget,materialkind,favoriteId)
   -- 1,2,3,4,0
   local tag = {2,3,4,5,6}
   local parent = UI_GetUILayout(widget,2)
   local showIndex = 0
   if materialkind == 1 then
      showIndex = 2
   elseif materialkind == 2 then
      showIndex = 3 
   elseif materialkind == 3 then
      showIndex = 4
   elseif materialkind == 4 then
      showIndex = 5
   elseif materialkind == 0 then 
      showIndex = 6
   end 
   for i = 1,#tag do 
      if tag[i] == showIndex then 
         UI_GetUILayout(parent,tag[i]):setVisible(true)
         HeroFavorite_FadeMaterialLayoutAction(UI_GetUILayout(parent,tag[i]),0.25)
      else
         Log("tag[i]===="..tag[i]) 
         UI_GetUILayout(parent,tag[i]):setVisible(false)
      end 
   end 
   if materialkind ~= 0 then 
      HeroFavorite_ShowMaterialCurInfo(widget,favoriteId,materialkind)
   end 
end 

function HeroFavorite_InitSoulLayout(widget)
   local parentlayout = UI_GetUILayout(widget,2)
   local tag = {2,3,4,5,6}
   for i = 1,#tag do 
      local layout = UI_GetUILayout(parentlayout,tag[i])
      if UI_GetUILayout(layout,1):getChildByTag(10000) then 
         UI_GetUILayout(layout,1):removeChildByTag(10000)
      end 
   end 
end 

function HeroFavorite_ShowMaterialCurInfo(widget,favoriteId,materialkind)
   local parentlayout = UI_GetUILayout(widget,2)
   local layout = nil 
   Log("materialkind===="..materialkind)
   HeroFavorite_InitSoulLayout(widget)
   if materialkind == 0 then 
      layout = UI_GetUILayout(parentlayout,6)
   elseif materialkind == 1 then
      layout = UI_GetUILayout(parentlayout,2)
   elseif materialkind == 2 then 
      layout = UI_GetUILayout(parentlayout,3)
   elseif materialkind == 3 then 
      layout = UI_GetUILayout(parentlayout,4)
   elseif materialkind == 4 then 
      layout = UI_GetUILayout(parentlayout,5)
   end  
   if materialkind ~= 0 then 
       HeroFavorite_CommonCurInfo(layout,favoriteId)
       HeroFavorite_ShowMaterialIconList(widget)    
   end 
end
 
function HeroFavorite_CommonCurInfo(layout,favoriteId)
   Log("favoriteId==="..favoriteId)
   local data = GetGameData(DataFileItem,favoriteId,"stItemData")
   --name
   UI_SetLabelText(layout,3,data.m_name)
   UI_GetUILabel(layout,3):enableStroke()
   --icon
   Log("data.m_icon===="..data.m_icon)
   UI_GetUIImageView(UI_GetUILayout(layout,2), 1):loadTexture("Icon/Item/"..data.m_icon..".png")

   --frame 
   UI_IconFrameWithSkillOrItem(UI_GetUILayout(layout,2),2,nil,data.m_quality)

   --cost

   local data = GetGameData(DataFileFavorite,favoriteId,"stFavoriteData")
   local materialKind = data.m_favoriteCount
   local materiallayout = UI_GetUILayout(UI_GetUILayout(layout,2),1)
   local favoritelist = vector_int_:new_local()
   local favoriteMateriallist = vector_int_:new_local()
   GetLocalPlayer():GetHeroFavoriteSummondItemId(favoriteId,favoritelist,favoriteMateriallist)
   local function createClickTempLayout(layout,index)
      local function clickMaterialFunc(sender,eventType)
         if eventType == TOUCH_EVENT_ENDED then 
            local sender = tolua.cast(sender,"Layout")
            local tag = sender:getTag()
            Log("tag==="..tag)
            table.insert(g_heroFavorite_MaterialIdTable,favoritelist[tag - 1])
            local widget = UI_GetBaseWidgetByName("HeroSystemLoveThingOne")
            if widget then 
               --HeroFavorite_ShowMaterialIconList(widget)
               local data = GetGameData(DataFileFavorite,favoritelist[tag - 1],"stFavoriteData")
               if data == nil then 
                  local tab = GameServer_pb.CMD_QUERY_EQUIPSOURCE_CS()
                  tab.iEquipID = favoritelist[tag - 1]
                  Packet_Full(GameServer_pb.CMD_QUERY_EQUIPSOURCE,tab)
                  ShowWaiting()
               else 
                  HeroFavorite_ShowMaterialLayoutInit(widget,data.m_favoriteCount,favoritelist[tag - 1])
               end 
            end 
         end 
      end 
      layout:setTouchEnabled(true)
      layout:addTouchEventListener(clickMaterialFunc)
   end 
   if materialKind ~= 0 then 
       local function findNum(materialId,tag)
            local haveNumber = GetLocalPlayer():GetPlayerBag():GetItemCountByItemId(materialId)
            for i=1 , tag-1 , 1 do
                if materialId == favoritelist[i-1] then
                    haveNumber = haveNumber - favoriteMateriallist[i - 1]
                end
            end
            if haveNumber <= 0 then
                return 0
            else 
                return GetLocalPlayer():GetPlayerBag():GetItemCountByItemId(materialId)
            end
       end
       for i = 1,materialKind do
          local templayout = UI_GetUILayout(UI_GetUILayout(layout,1),i)
          local materialId = favoritelist[i - 1]
          local favoriteMaterialNeed = favoriteMateriallist[i - 1]
          Log("materialId====="..materialId)
          local tempdata = GetGameData(DataFileItem,materialId,"stItemData")
          --icon
          UI_GetUIImageView(templayout,1):loadTexture("Icon/Item/"..tempdata.m_icon..".png")
          --frame
          UI_IconFrameWithSkillOrItem(templayout,2,nil,tempdata.m_quality)

          --number
          local num = findNum(materialId,i)
          UI_SetLabelText(templayout,3,num.."/"..favoriteMaterialNeed)
          UI_GetUILabel(templayout, 3):setColor(ccc3(250,250,250))
          if num < favoriteMaterialNeed then
                UI_GetUILabel(templayout, 3):setColor(ccc3(200,0,0))
          end
          createClickTempLayout(templayout,i)
       end 
   end 

   --合成
   local function ClickSummondFunc(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then 
         if GetLocalPlayer():HeroFavoriteCanSummondByFavoriteId(favoriteId) then 
            local tab = GameServer_pb.CMD_HERO_COMBINE_FAVORITE_CS()
            tab.iItemID = favoriteId
            Packet_Full(GameServer_pb.CMD_HERO_COMBINE_FAVORITE,tab)
            ShowWaiting()
         else 
            local favoritebaseId = GetLocalPlayer():HeroFavoriteNotHave_ByCanSummondByFavoriteId(favoriteId)   
            Log("favoritebaseId======"..favoritebaseId) 
            if favoritebaseId ~= 0 then 
               local label = HeroCurInfoSecond_ShowWhoCancommit()
               local templayout = UI_GetUILayout(UI_GetUILayout(layout,1),favoritebaseId)
               local posx,posy = templayout:getPosition()
               local width = templayout:getContentSize().width
               local height = templayout:getContentSize().height
               if UI_GetUILayout(layout,1):getChildByTag(10000) then 
                  UI_GetUILayout(layout,1):removeChildByTag(10000)
               end 
               UI_GetUILayout(layout,1):addChild(label,10000,10000)
               label:setPosition(ccp(posx + width / 2 - 5,posy + height + 15))
            else 
               createPromptBoxlayout(FormatString("HeroFavorite_CannotSummond"))
            end     
            --
         end 
      end 
   end 
   UI_GetUIButton(layout,6):addTouchEventListener(ClickSummondFunc)

   --cost
   UI_GetUILabel(layout,4):setText(data.m_cost)
end

function HeroCurInfoSecond_ShowWhoCancommit_Remove(sender)
   sender:removeFromParent()
end 

function HeroCurInfoSecond_ShowWhoCancommit()
   local image = ImageView:create()
   image:loadTexture("tips/bg_03.png")
   local labe = Label:create()
   labe:setText(FormatString("HeroFavorite_firstSummond"))
   labe:setFontSize(18)
   UI_SetLabelFontName(1,labe)
   labe:setPosition(ccp(0,5))
   image:addChild(labe)
   local arryaction = CCArray:create()
   local delaytime = CCDelayTime:create(1)
   arryaction:addObject(delaytime)
   local fadeout = CCFadeOut:create(0.25)
   arryaction:addObject(fadeout)
   local callfuncone = CCCallFuncN:create(HeroCurInfoSecond_ShowWhoCancommit_Remove)
   arryaction:addObject(callfuncone) 
   local seqaction = CCSequence:create(arryaction)
   image:runAction(seqaction)
   return image
end  

--table的copy函数
function CopyTable(st)
    -- body
    local tab = {}
    for k,v in pairs(st or {}) do
        if type(v) ~= "table" then
            tab[k] = v
        else
            tab[k] = CopyTable(v)
        end
    end
    return tab
end

function HeroFavorite_ChangeMaterialIdTable(index)
   local temptable = CopyTable(g_heroFavorite_MaterialIdTable)
   g_heroFavorite_MaterialIdTable = {}
   for i = 1, index do 
      table.insert(g_heroFavorite_MaterialIdTable,temptable[i])
   end 
end 

function HeroFavorite_ShowMaterialIconList(widget)
   Log("first first first")
   local layout = UI_GetUILayout(widget,2)
   local list = UI_GetUIListView(layout,1)
   list:removeAllItems()
   local curlayout = nil 
   Log("size===="..#g_heroFavorite_MaterialIdTable)
   local modellayout = Layout:create()
   modellayout:setSize(CCSizeMake(104,80))
   list:setItemModel(modellayout)
   local function ClickHeroFavoriteListFunc(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then 
         local sender = tolua.cast(sender,"Layout")
         local index = list:getIndex(sender)
         Log("index ==="..index)
         --table的改变
         HeroFavorite_ChangeMaterialIdTable(index + 1)
         local data = GetGameData(DataFileFavorite,g_heroFavorite_MaterialIdTable[index + 1],"stFavoriteData")
         if data == nil then 
            local tab = GameServer_pb.CMD_QUERY_EQUIPSOURCE_CS()
            tab.iEquipID = g_heroFavorite_MaterialIdTable[index + 1]
            Packet_Full(GameServer_pb.CMD_QUERY_EQUIPSOURCE,tab)
            ShowWaiting()
         else 
            HeroFavorite_ShowMaterialLayoutInit(widget,data.m_favoriteCount,g_heroFavorite_MaterialIdTable[index + 1])
            HeroFavorite_ShowMaterialIconList(widget)
         end 
      end 
   end 
   for i = 1,#g_heroFavorite_MaterialIdTable do 
      curlayout = UI_GetCloneLayout(UI_GetUILayout(widget,3))
      curlayout:setVisible(true)
      ----icon
      local data = GetGameData(DataFileItem,g_heroFavorite_MaterialIdTable[i],"stItemData")
      UI_GetUIImageView(curlayout, 1):loadTexture("Icon/Item/"..data.m_icon..".png")
      --frame
      UI_IconFrameWithSkillOrItem(curlayout,2,nil,data.m_quality)
      list:pushBackCustomItem(curlayout)
      curlayout:setTouchEnabled(true)
      curlayout:addTouchEventListener(ClickHeroFavoriteListFunc)
      --外框
      if i == #g_heroFavorite_MaterialIdTable then 
         UI_GetUIImageView(curlayout,4):setVisible(true)
         UI_GetUIImageView(curlayout,3):setVisible(false)
      else 
         UI_GetUIImageView(curlayout,4):setVisible(false)
         UI_GetUIImageView(curlayout,3):setVisible(true)
      end 
   end
   list:refreshView()
   list:jumpContainerToSelectedIndex(#g_heroFavorite_MaterialIdTable + 1) 
end 

function HeroFavorite_MaterialListShow(layout,SectionID,SceneID,isOpened)
   local sceneSectionData = GetGameData(DataFileSceneSection, SectionID, "stSceneSectionData")
   local sceneData = GetGameData(DataFileScene,SceneID,"stSceneData")
   Log("SectionID==="..SectionID)
   Log("SceneID==="..SceneID)
   --image 
   local image = UI_GetUIImageView(layout,1)
   --local height = layout:getContentSize().height
   if sceneData.m_icon >= 10 then 
      image:loadTexture("Dungeon/Bnt_dungeon_0"..sceneData.m_icon..".png")
   else 
      image:loadTexture("Dungeon/Bnt_dungeon_00"..sceneData.m_icon..".png")
   end 

   --local scale = height / image:getContentSize().height
   --image:setScale(scale)

   --name 
   UI_SetLabelText(layout, 2, sceneSectionData.m_nameFirst.." "..sceneSectionData.m_name )

   --关卡
   local guanqianame = sceneData.m_name
   if isOpened == false then 
      guanqianame = guanqianame..FormatString("NewHeroSystem_HeroTallentItemSource")
   end 
   UI_SetLabelText(layout, 3,guanqianame)
   if isOpened == false then
      --UI_GetUILabel(layout, 2):setColor(ccc3(255,0,0)) 
      UI_GetUILabel(layout, 3):setColor(ccc3(255,0,0)) 
   end 
end 

function HeroFavorite_GetFavoriteSourInfo(widget,info)
   local layout = UI_GetUILayout(UI_GetUILayout(widget,2),6)
   --name
   local itemdata = GetGameData(DataFileItem,info.iEquipID,"stItemData")
   UI_SetLabelText(layout,3,itemdata.m_name)
   local sourcelist = UI_GetUIListView(layout,1)
   sourcelist:removeAllItems()
   local curlayout = nil 
   local function clickHeroHeroFavoriteSource(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then
         playUiEffect()
         local sender = tolua.cast(sender,"Layout")
         local index = sourcelist:getIndex(sender)
         Log("index ==="..index)
         if info.szEquipSource[index + 1].bOpened then
            UI_CloseAllBaseWidget()
            g_selectSectionID = info.szEquipSource[index + 1].iSectionID
		    g_selectiSceneID = info.szEquipSource[index + 1].iSceneID
			local tab = GameServer_pb.Cmd_Cs_QueryDSectionDetail()
			tab.iSectionID = g_selectSectionID
			Packet_Full(GameServer_pb.CMD_QUERY_DSECTIONDETAIL, tab)
			ShowWaiting()
         else 
            createPromptBoxlayout(FormatString("NewHeroSystem_HeroTallentItemSourceTwo")) 
         end 
      end 
   end
   for i = 1,#info.szEquipSource do 
      curlayout = UI_GetCloneLayout(UI_GetUILayout(widget,7))
      curlayout:setVisible(true)
      HeroFavorite_MaterialListShow(curlayout,info.szEquipSource[i].iSectionID,info.szEquipSource[i].iSceneID,info.szEquipSource[i].bOpened)
      sourcelist:pushBackCustomItem(curlayout)
      curlayout:setTouchEnabled(true)
      curlayout:addTouchEventListener(clickHeroHeroFavoriteSource)
   end 
   HeroFavorite_MoveAction(widget,info.iEquipID)
   HeroFavorite_ShowMaterialIconList(widget)

   local function clickReturn(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then 
         UI_CloseCurBaseWidget(EUICloseAction_FadeOut, 0.25)
      end 
   end 
   UI_GetUIButton(layout,2):addTouchEventListener(clickReturn)
end 
--[[" 
      英雄喜好品 End
"]]

--[[" 
      更新
"]]
function HeroSecond_UpdateAfterEquipHeroFavorite(widget,info)
   --判断下把
   local heroEntity = GetLocalPlayer():GetHeroByObjId(info.dwHeroObjectID)
   --if HeroListSecond_GetHeroEntityByIndex() == heroEntity then 
    print("this is test=================================")
      --更新当前
      HeroSecond_AfterEquipFavoriteShowAction(info)
      HeroCurInfoSecond_HeroFighting(widget,heroEntity)
      HeroCurInfoSecond_HeroFavoriteInitShow(widget,heroEntity)
   --end 
end 

function HeroSecond_AfterEquipFavoriteShowAction(info)
   local itemId = info.iItemID
   local data = GetGameData(DataFileItem,itemId,"stItemData")
   local favoriteNature = vector_string_:new_local()
   GetLocalPlayer():GetHeroFavoriteNatureByItemId(favoriteNature,itemId)
   Log("favoriteNature==="..favoriteNature:size())
   local widget = UI_GetBaseWidgetByName("HeroSystemSecond")
   local posx,posy = UI_GetUILayout(widget,9):getPosition()
   local posonex,posoney = UI_GetUILayout(UI_GetUILayout(widget,9),1):getPosition()
   posx = posx + posonex
   posy = posy + posoney
   OtherPromptBox_Init()
   for i = 0, favoriteNature:size() - 1 do 
      --createPromptBoxlayout(favoriteNature[i])
      --createPromptBoxByHeroFavorite(favoriteNature[i],ccp(posx + 50,posy + 150))
      OtherPromptBox_InitValue(favoriteNature[i])
   end 
   OtherPromptBox_StarPromptAction(ccp(posx + 50,posy))
end 

function HeroSecond_AfterAdvancedSuccess(widget,info)
   local heroEntity = GetLocalPlayer():GetHeroByObjId(info.dwHeroObjectID)
   if HeroListSecond_GetHeroEntityByIndex() == heroEntity then
      HeroSecond_AfterAdvanceSuccessShowAction(info)
      HeroCurInfoSecond_FavoriteUpdateAction(widget)
      HeroCurInfoSecond_HeroFighting(widget,heroEntity)
      HeroCurInfoSecond_HeroFavoriteInitShow(widget,heroEntity)
      HeroCurInfoSecond_HeroNameInfo(widget,heroEntity)
      HeroCurInfoSecond_HeroAdvancedEffectShow(widget)
   end 
end

local function ArmatureEnd(armature, movementType, movementID)
   if (movementType == LOOP_COMPLETE)then
		armature:removeFromParentAndCleanup(true)
	end
end 

function HeroCurInfoSecond_HeroAdvancedEffectShow(widget)
   local shengxingArmature = GetUIArmature("shengxing")
   shengxingArmature:getAnimation():playWithIndex(0)
   shengxingArmature:getAnimation():setMovementEventCallFunc(ArmatureEnd)
   local heroinfolayout = UI_GetUILayout(widget, 3)
   local heroAnimallayout = UI_GetUILayout(heroinfolayout, 1) 
   shengxingArmature:setPosition(ccp(50, 50))
   heroAnimallayout:addNode(shengxingArmature, 99999)
end 

function HeroSecond_AfterAdvanceSuccessShowAction(info)
   local widget = UI_GetBaseWidgetByName("HeroSystemSecond")
   local posx,posy = UI_GetUILayout(widget,9):getPosition()
   local posonex,posoney = UI_GetUILayout(UI_GetUILayout(widget,9),1):getPosition()
   posx = posx + posonex
   posy = posy + posoney
   OtherPromptBox_Init()
   for i = 1,#info.szPropAddValueList do 
      local name = GetAttName(info.szPropAddValueList[i].iLifeAttID)..": "
      local name = name.."+ "..info.szPropAddValueList[i].iValue
      --createPromptBoxlayout(name)
      --createPromptBoxByHeroFavorite(name,ccp(posx + 50,posy + 150))
      OtherPromptBox_InitValue(name)
   end 
   OtherPromptBox_StarPromptAction(ccp(posx + 50,posy))
end  

function HeroFavorite_AfterSummondUpdate(widget,info) 
   local favoriteId = info.iItemID
   if #g_heroFavorite_MaterialIdTable ~= 1 then 
      table.remove(g_heroFavorite_MaterialIdTable)
      HeroFavorite_ShowMaterialIconList(widget)
      Log("size==="..#g_heroFavorite_MaterialIdTable)
      local size = #g_heroFavorite_MaterialIdTable
      local data = GetGameData(DataFileFavorite,g_heroFavorite_MaterialIdTable[size],"stFavoriteData")
      HeroFavorite_ShowMaterialLayoutInit(widget,data.m_favoriteCount,g_heroFavorite_MaterialIdTable[size])
   else 
      local data = GetGameData(DataFileFavorite,favoriteId,"stFavoriteData")
      local layout = UI_GetUILayout(widget, 1)
      --number
      local number = GetLocalPlayer():GetPlayerBag():GetItemCountByItemId(favoriteId)
      UI_SetLabelText(layout,2,number)
      --position
      local posX,posY = UI_GetUILabel(layout, 2):getPosition()
      UI_GetUILabel(layout, 3):setPosition(ccp(posX + UI_GetUILabel(layout, 2):getContentSize().width + 5,posY))

      --if GetLocalPlayer():HeroFavoriteCanSummondByFavoriteId(favoriteId) then 
      if GetLocalPlayer():GetPlayerBag():GetItemCountByItemId(g_heroFavorite_MaterialIdTable[1]) > 0 then 
         local itemData = GetGameData(DataFileItem,g_heroFavorite_MaterialIdTable[1],"stItemData")
         if itemData.m_lvLimits <= GetLocalPlayer():GetUint(EPlayer_Lvl) then 
            local layout = UI_GetUILayout(widget,1)
            UI_GetUIButton(layout,6):setTouchEnabled(true)
            local function EquipHeroFavorite(sender,eventType)
               if eventType == TOUCH_EVENT_ENDED then 
                  local heroEntity = HeroListSecond_GetHeroEntityByIndex()
                  local tab = GameServer_pb.CMD_HERO_EQUIP_FAVORITE_CS()
                  tab.dwHeroObjectID = heroEntity:GetUint(EHero_ID)
                  tab.iItemID = g_heroFavorite_MaterialIdTable[1]
                  tab.iPos = g_heroClickfavoriteIndex
                  Packet_Full(GameServer_pb.CMD_HERO_EQUIP_FAVORITE, tab)
                  ShowWaiting()
               end 
            end 
            UI_GetUIButton(layout,6):addTouchEventListener(EquipHeroFavorite)
         end 
      end 
      HeroFavorite_ShowMaterialLayoutInit(widget,data.m_favoriteCount,favoriteId)
   end
   --更新下层
   local infoWidget = UI_GetBaseWidgetByName("HeroSystemSecond")
   if infoWidget then 
      local heroEntity = HeroListSecond_GetHeroEntityByIndex()
      HeroCurInfoSecond_HeroFavoriteInitShow(infoWidget,heroEntity)
   end  
end 

--升级特效
function HeroLevelUp_AnimEnd(armature, movementType, movementID)
    -- body
    if (movementType == LOOP_COMPLETE)then
        local index = 0
        local widget = UI_GetBaseWidgetByName("HeroSystemSecond") 
        local heroUpgradeParent = UI_GetUILayout(UI_GetUILayout(widget, 4) , 1)
        if movementID == "0" then
            index = 0
        elseif movementID == "1" then 
            index = 1
        end
        if widget:getNodeByTag(6000 + index) then
           widget:removeNodeByTag(6000 + index)
        end
    end
end

function HeroSystem_ShowXiuweiDianjiAnimature(widget,index)
    -- body
    --local heroUpgradeParent = UI_GetUILayout(UI_GetUILayout(widget, 4) , 1)
    local animaaction = GetUIArmature("Effxiuweidianji")
    widget:addNode(animaaction,50000,6000 + index)
    animaaction:setPosition(ccp(480,320))
    animaaction:getAnimation():playWithIndex(index)
    animaaction:getAnimation():setMovementEventCallFunc(HeroLevelUp_AnimEnd)
end

function HeroCurInfoSecond_AfterLookUpHeroUpdateInfo(widget,info)
   local heroEntity = GetLocalPlayer():GetHeroByObjId(info.levelInfo.dwHeroObjectID)
   --HeroCurInfoSecond_ShowCurLayout(widget,heroEntity)
   HeroCurInfoSecond_HeroFighting(widget,heroEntity)
   local heroinfolayout = UI_GetUILayout(widget, 3)
   UI_SetLabelText(heroinfolayout, 13, heroEntity:GetUint(EHero_Lvl)) 

   local updatelayout = UI_GetUILayout(widget,5)
   HeroCurInfoSecond_HeroUpdateView_XiuWeiEffect(updatelayout)
   HeroCurInfoSecond_HeroUpdateView_BaseInfo(updatelayout,heroEntity,info,false)
   
   --升级
   local function ClickHeroUpdateFunc(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then 
         local curtime = SysUtil:GetMSTime()
         if curtime - g_HeroSystem_HeroUpdateIntervalTime < 300 then
             return 
         else
             g_HeroSystem_HeroUpdateIntervalTime = SysUtil:GetMSTime()
         end
	     local tab = GameServer_pb.Cmd_Cs_HeroLevelUp();
	     tab.dwHeroObjectID = heroEntity:GetUint(EHero_ID)     
	     Packet_Full(GameServer_pb.CMD_HERO_LEVELUP,tab)
		 
		if (g_isGuide) then
			Guide_GoNext();
		end
      end 
   end 
   UI_GetUIButton(updatelayout,2):addTouchEventListener(ClickHeroUpdateFunc)

   --散功
   local function BesureScatteredPower()
      Log("BesureScatteredPower")
      local tab = GameServer_pb.CMD_HERO_SANGONG_CS()
      tab.dwObjectID = heroEntity:GetUint(EHero_ID)
      Packet_Full(GameServer_pb.CMD_HERO_SANGONG,tab)
      ShowWaiting()
   end 
   local function ClickHeroSangongFunc(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then 
         if eventType == TOUCH_EVENT_ENDED then 
             local herodata = GetGameData(DataFileHeroBorn,heroEntity:GetUint(EHero_HeroId),"stHeroBornData")
             local heroQuality = heroEntity:GetUint(EHero_Quality)
             if heroEntity:GetUint(EHero_Lvl) == 1 and heroEntity:GetUint(EHero_HeroExp) == 0 then
                Messagebox_Create({info = FormatString("ScatteredPower_messagenotinfo"),title = FormatString("ScatteredPower_messagetitle"),msgType = EMessageType_Middle})
             else
                --Messagebox_Create({info = FormatString("ScatteredPower_messageinfo",GetHeroNewName(herodata.m_name,heroQuality)),title = FormatString("ScatteredPower_messagetitle"), msgType = EMessageType_LeftRight, leftFun = BesureScatteredPower})    
                --HeroSangGongFunction(heroEntity)
                local tab = GameServer_pb.CMD_QUERY_SANGONG_CS()
                tab.dwHeroObjectID = heroEntity:GetUint(EHero_ID)
                Packet_Full(GameServer_pb.CMD_QUERY_SANGONG,tab)
                ShowWaiting()
             end 
         end 
      end 
   end
   UI_GetUIButton(updatelayout,3):addTouchEventListener(ClickHeroSangongFunc) 
end 

function HeroSangGongFunction(info)
   local heroEntity = GetLocalPlayer():GetHeroByObjId(info.dwHeroObjectID)
   local widget = UI_CreateBaseWidgetByFileName("SangongTip.json",EUIOpenAction_Enlarge,0.25)
   local herodata = GetGameData(DataFileHeroBorn,heroEntity:GetUint(EHero_HeroId),"stHeroBornData")
   local function HeroSystem_ClickReturn(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then 
         UI_CloseCurBaseWidget(EUICloseAction_FadeOut, 0.25)
      end 
   end 
   UI_GetUIButton(widget,1):addTouchEventListener(HeroSystem_ClickReturn)
   UI_GetUIButton(widget,6):addTouchEventListener(HeroSystem_ClickReturn)
   UI_GetUILayout(widget, 4637059):addTouchEventListener(PublicCallBackWithNothingToDo)

   local heroQuality = heroEntity:GetUint(EHero_Quality)
   local strlab = CompLabel:GetCompLabel(FormatString("ScatteredPower_messageinfo",GetHeroNewName(herodata.m_name,heroQuality)),380,kCCTextAlignmentLeft)
   local layout = UI_GetUILayout(widget,2)
   strlab:setPosition(ccp(10,0))
   layout:addChild(strlab)

   local cost = info.iSilver
   UI_SetLabelText(widget,3,cost)

   --queding
   local function BesureScatteredPower(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then 
          Log("BesureScatteredPower")
          UI_CloseCurBaseWidget(EUICloseAction_FadeOut, 0.25)
          local tab = GameServer_pb.CMD_HERO_SANGONG_CS()
          tab.dwObjectID = heroEntity:GetUint(EHero_ID)
          Packet_Full(GameServer_pb.CMD_HERO_SANGONG,tab)
          ShowWaiting()
      end 
   end
   UI_GetUIButton(widget,5):addTouchEventListener(BesureScatteredPower)
end 

function HeroCurInfoSecond_AfterHeroUpdateView(widget,info)
   local heroEntity = GetLocalPlayer():GetHeroByObjId(info.levelInfo.dwHeroObjectID)
   local updatelayout = UI_GetUILayout(widget,5)
   HeroCurInfoSecond_HeroUpdateView_BaseInfo(updatelayout,heroEntity,info,true)
   HeroCurInfoSecond_HeroFighting(widget,heroEntity)
   print("level-----------------------------------------------------------------")
   local heroinfolayout = UI_GetUILayout(widget, 3)
   UI_SetLabelText(heroinfolayout:getChildByTag(14), 13, heroEntity:GetUint(EHero_Lvl))
end 

function HeroCurInfoSecond_HeroUpdateView_XiuWeiEffect(updatelayout)
   local heroUpdateEffectlayout = UI_GetUILayout(updatelayout, 1)
   heroUpdateEffectlayout:removeAllNodes()
   if heroUpdateEffectlayout:getNodeByTag(4000) == nil then
      local animaaction = GetUIArmature("Effxiuwei")
      heroUpdateEffectlayout:addNode(animaaction,100,4000)
      animaaction:setPosition(ccp(heroUpdateEffectlayout:getContentSize().width / 2 ,20))
      animaaction:getAnimation():playWithIndex(0)
   end
end 

function HeroCurInfoSecond_HeroUpdateView_BaseInfo(updatelayout,heroEntity,info,isAfterUpdate)
   --剩余修为  
   local leftExp = info.levelInfo.iActorTkHeroExp
   UI_SetLabelText(updatelayout,4,leftExp)

   local curExp = info.levelInfo.iCurHeroExp
   Log("curExp==="..curExp)
   local needExp = info.levelInfo.iNeedHeroExp
   UI_SetLabelText(updatelayout, 6, curExp.."/"..needExp)
   UI_GetUILabel(updatelayout,6):enableStroke()
   if isAfterUpdate then 
      if curExp == 0 then
         UI_GetUILoadingBar(updatelayout,5):setEnableToZero()
		 UI_GetUILoadingBar(updatelayout,5):setPercentSlowly(100,0.25) 
      else
		 UI_GetUILoadingBar(updatelayout,5):setPercentSlowly(curExp / needExp * 100, 0.25)
      end	
   else 
      UI_GetUILoadingBar(updatelayout, 5):setPercent(curExp / needExp * 100)
   end 

   --curinfo
   HeroCurInfoSecond_HeroUpdateView_NowInfo(updatelayout,heroEntity)
   HeroCurInfoSecond_HeroUpdateView_NextInfo(updatelayout,info,heroEntity)
end

function HeroCurInfoSecond_HeroUpdateView_NowInfo(updatelayout,heroEntity)
   --当前信息
   local curinfotag = {7,8,9,10}
   UI_SetLabelText(updatelayout,curinfotag[1],FormatString("HeroUpgrade_LvL",heroEntity:GetUint(EHero_Lvl)))
   UI_SetLabelText(updatelayout,curinfotag[2],FormatString("HeroUpgrade_Life",heroEntity:GetUint(EHero_HP)))
   UI_SetLabelText(updatelayout,curinfotag[3],FormatString("HeroUpgrade_Attact",heroEntity:GetUint(EHero_Att))) 
   UI_SetLabelText(updatelayout,curinfotag[4],FormatString("HeroUpgrade_Def",heroEntity:GetUint(EHero_Def))) 
end 


--英雄属性动态文字
function HeroSystem_FadeLab(value)
    -- body
    local lab = CompLabel:GetCompLabel(FormatString("HeroAscendingOrder_Label",value),192,kCCTextAlignmentLeft)
    UI_GetFadeInAndOutAction(lab)
    return lab 
end

--返回最大的数
function GetleftMaxNumber(valuetable)
    -- body
    local maxNumber = 0
    for i = 1, #valuetable do 
       if maxNumber < valuetable[i] then 
          maxNumber = valuetable[i]
       end
    end
    return maxNumber
end

function HeroCurInfoSecond_HeroUpdateView_NextInfo(updatelayout,info,heroEntity)
    local curinfotag = {7,8,9,10}
    local nextinfotag = {11,12,13,14}
    
    local nextLvlayout = UI_GetUILayout(updatelayout,nextinfotag[1])
    local nextMapHplayout = UI_GetUILayout(updatelayout,nextinfotag[2])
    local nextAttlayout = UI_GetUILayout(updatelayout,nextinfotag[3])
    local nextDeflayout = UI_GetUILayout(updatelayout,nextinfotag[4])
    --下级信息
    --NextLv
    local nextlvlab = HeroSystem_FadeLab(info.levelInfo.iNextLv - heroEntity:GetUint(EHero_Lvl))
    nextLvlayout:removeAllChildren()
    nextlvlab:setAnchorPoint(ccp(0,0.5))
    local nexlvlarrowimg = UI_GetAddImage()
    nexlvlarrowimg:setAnchorPoint(ccp(0,0.5))
    nextLvlayout:addChild(nextlvlab)
    nextLvlayout:addChild(nexlvlarrowimg)
    nextlvlab:setPosition(ccp(nexlvlarrowimg:getContentSize().width + 5,0))

    --nextMaxHp
    local nextMaxHp = HeroSystem_FadeLab(info.levelInfo.iNextMaxHP - heroEntity:GetUint(EHero_HP))
    nextMapHplayout:removeAllChildren()
    nextMaxHp:setAnchorPoint(ccp(0,0.5))
    local nextMaxHparrowimg = UI_GetAddImage()
    nextMaxHparrowimg:setAnchorPoint(ccp(0,0.5))
    nextMapHplayout:addChild(nextMaxHp)
    nextMapHplayout:addChild(nextMaxHparrowimg)
    nextMaxHp:setPosition(ccp(nextMaxHparrowimg:getContentSize().width + 5 ,0))

    --nextAtt
    local nextAttlab = HeroSystem_FadeLab(info.levelInfo.iNextAtt - heroEntity:GetUint(EHero_Att))
    nextAttlayout:removeAllChildren()
    nextAttlab:setAnchorPoint(ccp(0,0.5))
    local nextAttarrowimg = UI_GetAddImage()
    nextAttarrowimg:setAnchorPoint(ccp(0,0.5))
    nextAttlayout:addChild(nextAttlab)
    nextAttlayout:addChild(nextAttarrowimg)
    nextAttlab:setPosition(ccp(nextAttarrowimg:getContentSize().width + 5 ,0))

    --nextDef
    local netDeflab = HeroSystem_FadeLab(info.levelInfo.iNextDef - heroEntity:GetUint(EHero_Def))
    nextDeflayout:removeAllChildren()
    netDeflab:setAnchorPoint(ccp(0,0.5))
    local nexDefarrowimg = UI_GetAddImage()
    nexDefarrowimg:setAnchorPoint(ccp(0,0.5))
    nextDeflayout:addChild(netDeflab)
    nextDeflayout:addChild(nexDefarrowimg)
    netDeflab:setPosition(ccp(nexDefarrowimg:getContentSize().width + 5 ,0))

    --计算箭头和后面值的位置
    local labwidthtable = {UI_GetUILayout(updatelayout,curinfotag[1]):getContentSize().width,
                           UI_GetUILayout(updatelayout,curinfotag[2]):getContentSize().width,
                           UI_GetUILayout(updatelayout,curinfotag[3]):getContentSize().width,
                           UI_GetUILayout(updatelayout,curinfotag[4]):getContentSize().width}
    local Maxlabwidth = GetleftMaxNumber(labwidthtable) + 5
    local lvlPositionX,lvlPositionY = UI_GetUILayout(updatelayout,curinfotag[1]):getPosition()
    local lifePositionX,lifePositionY = UI_GetUILayout(updatelayout,curinfotag[2]):getPosition()
    local attPositionX,attPositionY = UI_GetUILayout(updatelayout,curinfotag[3]):getPosition()
    local defPositionX,defPositionY = UI_GetUILayout(updatelayout,curinfotag[4]):getPosition()
    
    --设置位置  这样好看些 
    nextLvlayout:setPosition(ccp(lvlPositionX + Maxlabwidth,lvlPositionY))
    nextMapHplayout:setPosition(ccp(lifePositionX + Maxlabwidth,lifePositionY))
    nextAttlayout:setPosition(ccp(attPositionX + Maxlabwidth,attPositionY))
    nextDeflayout:setPosition(ccp(defPositionX + Maxlabwidth,defPositionY))
end 


--详细属性
function HeroSystem_MoreDetailsLabOp(labelone,labeltwo)
   local positionX,positionY = labelone:getPosition()
   local width = labelone:getContentSize().width
   labeltwo:setPosition(ccp(positionX + width + 5,positionY))
end

function HeroCurInfoSecond_HeroCurBaseInfo(widget,info)
   local heroEntity = GetLocalPlayer():GetHeroByObjId(info.dwObjectID)
   local herocurlayout = UI_GetUILayout(widget,6)
   local scollView = UI_GetUIScrollView(herocurlayout,1)
   --列传
   local data = heroEntity:GetHeroData()
   local liezhuanlayout = UI_GetUILayout(scollView, 2)
   UI_SetLabelText(liezhuanlayout, 1, data.m_desc)

   --属性
   --生命成长
   local desclayout = UI_GetUILayout(scollView, 1)
   UI_SetLabelText(desclayout, 1, info.iMaxHPGrow / 10000)
   --攻击成长
   UI_SetLabelText(desclayout, 2, info.iAttGrow / 10000)
   --防御成长
   UI_SetLabelText(desclayout, 3, info.iDefGrow / 10000)

   local lifeatttable = {5,6,37,8,7,10,9,12,11,14,13,15}
   local basedescTag = {4,5,6,7,8,9,10,11,12,13,14,15}
   local adddescTag = {16,17,18,19,20,21,22,23,24,25,26,27}
   local function gettag(lifeattID)
      for i = 1 ,#lifeatttable do
          if lifeattID == lifeatttable[i] then 
             return i
          end 
      end
      return nil
   end 
   for i = 1,#info.szPropList do
       local tag = gettag(info.szPropList[i].iLifeAttID)
       if tag == nil then
          Log("this is wrong")
          break;          
       end   
       local name = GetAttName(info.szPropList[i].iLifeAttID)..": "
       local addnumber = nil 
      
       if info.szPropList[i].iLifeAttID == GameServer_pb.en_LifeAtt_MaxHP or 
       info.szPropList[i].iLifeAttID == GameServer_pb.en_LifeAtt_Att or 
       info.szPropList[i].iLifeAttID == GameServer_pb.en_LifeAtt_Def or 
       info.szPropList[i].iLifeAttID == GameServer_pb.en_LifeAtt_InitAnger then 
          name = name..info.szPropList[i].iValue
          if info.szPropAddValueList[i].iValue ~= 0 then 
             addnumber = "+"..info.szPropAddValueList[i].iValue
          end 
       else 
          --显示百分比
          name = name..(info.szPropList[i].iValue / 10).."%"
          if info.szPropAddValueList[i].iValue ~= 0 then  
             addnumber = "+"..(info.szPropAddValueList[i].iValue / 10).."%"
          end 
       end 
       UI_SetLabelText(desclayout,basedescTag[tag] , name)
       if addnumber ~= nil then 
          UI_GetUILabel(desclayout, adddescTag[tag]):setVisible(true)
          UI_SetLabelText(desclayout,adddescTag[tag] , addnumber)
          HeroSystem_MoreDetailsLabOp(UI_GetUILabel(desclayout, basedescTag[tag]),UI_GetUILabel(desclayout, adddescTag[tag]))
       else 
          UI_GetUILabel(desclayout, adddescTag[tag]):setVisible(false)
       end 
    end
end 

--当前天赋技能是否达到最高等级
function HeroCurInfoSecond_HeroTallentLvlIsMax(herotallentId)
   local data = GetGameData(DataFileHeroTallentBorn,herotallentId + 1,"stHeroTallentBornData")
   if data == nil then 
      return true
   else 
      return false
   end  
end 

function HeroCurInfoSecond_GetTallentPos(tallentId,heroEntity)
   local tallentskilllist = heroEntity:GetHeroInnateskill():GetInnateskillData() 
   local pos = 1
   for i = 0,tallentskilllist:size() - 1 do 
      if tallentskilllist[i].m_TallentID == tallentId then 
         return (pos + i)
      end 
   end 
   return pos
end 

function HeroCurInfoSecond_HeroTallentSkillShow(widget,heroEntity)
   local tallentskilllist = heroEntity:GetHeroInnateskill():GetInnateskillData()
   local herotallentlayout = UI_GetUILayout(widget, 7)
   local heroTallentTag = {1,2,3,4}
   Log("tallentskilllist:size()=="..tallentskilllist:size())
   local function ClcikUpdateHeroTallenSkill(layout,tag,data)
      local function clickHeroTallenSkillUporAct(sender,eventType)
          if eventType == TOUCH_EVENT_ENDED then 
			if (g_isGuide) then
				Guide_GoNext();
			end
             if GetLocalPlayer():GetInt(EPlayer_Lvl) >= data.m_playerlimit then 
                 local tallentId = (data.m_ID - data.m_level) / 1000
                 Log("tallentId==="..tallentId)
                 local tab = GameServer_pb.CMD_HERO_TALLENT_LEVELUP_CS()
                 tab.dwHeroObjectID = heroEntity:GetUint(EHero_ID)
                 tab.iTallentID = tallentId
                 g_heroCurInfo_TallentId_Select = tallentId
                 Packet_Full(GameServer_pb.CMD_HERO_TALLENT_LEVELUP, tab)
                 ShowWaiting()
             else 
                 createPromptBoxlayout(FormatString("NewHeroSystem_HeroTallentActiveByPlayerLvlTwo",data.m_playerlimit))
             end 
          end 
      end 
      UI_GetUIButton(layout,tag):addTouchEventListener(clickHeroTallenSkillUporAct)
   end 
   local function clickHeroTallentSkilllayout(sender,eventType)
       if eventType == TOUCH_EVENT_BEGAN then 
          local sender = tolua.cast(sender,"Layout")
          local tag = sender:getTag()
          Log("tag===="..tag)
          local tallentId = tallentskilllist[tag - 1].m_TallentID * 1000 + tallentskilllist[tag - 1].m_iLevel
          if tallentskilllist[tag - 1].m_iLevel == 0 then 
             HeroCurInfoSecond_LookHeroTallenSkillCurInfo(widget,tallentId,false,tag)
          else 
             HeroCurInfoSecond_LookHeroTallenSkillCurInfo(widget,tallentId,true,tag)
          end
       elseif eventType == TOUCH_EVENT_ENDED or eventType == TOUCH_EVENT_CANCELED then --eventType == TOUCH_EVENT_MOVED or  eventType == TOUCH_EVENT_CANCELED then  
          --UI_GetUIImageView(widget,16):setVisible(false)
          if widget:getChildByTag(999) then 
             widget:removeChildByTag(999)
          end 
       end 
   end 
   for i = 0, tallentskilllist:size() - 1 do
      local parentlayout = UI_GetUILayout(herotallentlayout, heroTallentTag[i + 1])
      local imageparent = UI_GetUIImageView(parentlayout,1)
      local tallenId = tallentskilllist[i].m_TallentID * 1000 + tallentskilllist[i].m_iLevel
      Log("tallenId==="..tallenId)
      local data = GetGameData(DataFileHeroTallentBorn,tallenId,"stHeroTallentBornData") 
      --图标
      UI_GetUIImageView(imageparent, 1):setColor(ccc3(255,255,255))
      UI_GetHeroTallenSkillIconAndFrame(imageparent,1,2,data.m_icon,data.m_quality)  
      local isActivity = false  
      if tallentskilllist[i].m_iLevel == 0 then 
      --为0的时候是未激活
         --UI_GetUIImageView(imageparent, 1):addColorGray()
         UI_GetUIImageView(imageparent, 1):setColor(ccc3(125,125,125))  
      end
      --name
      UI_SetLabelText(parentlayout,2,data.m_name)
      --是否激活
      if tallentskilllist[i].m_iLevel == 0 then 
         UI_GetUILayout(parentlayout,3):setVisible(false)
         UI_GetUILabel(parentlayout,4):setVisible(true)
         UI_GetHeroLevelStepLab(parentlayout,4,data.m_levelstep)
      else 
         UI_GetUILabel(parentlayout,4):setVisible(false)
         local infolayout = UI_GetUILayout(parentlayout,3)
         infolayout:setVisible(true)
         UI_SetLabelText(infolayout,3,"Lv:"..tallentskilllist[i].m_iLevel)
         UI_SetLabelText(infolayout,1,data.m_sliver)
         if data.m_sliver > GetLocalPlayer():GetInt(EPlayer_Silver) then 
            UI_GetUILabel(infolayout,1):setColor(ccc3(255,0,0))
         else 
            UI_GetUILabel(infolayout,1):setColor(ccc3(255,255,255))
         end 
         ClcikUpdateHeroTallenSkill(infolayout,2,data)
      end 
      
      parentlayout:setTouchEnabled(true)
      parentlayout:addTouchEventListener(clickHeroTallentSkilllayout)
   end 
end 


function HeroCurInfoSecond_HeroTallentUpdate(widget,heroEntity)
   HeroCurInfoSecond_HeroTallentSkillShow(widget,heroEntity)
   HeroCurInfoSecond_HeroFighting(widget,heroEntity)
end

function HeroCurInfoSecond_LookHeroTallenSkillCurInfo(widget,skillId,isAc,tag)
   local skilldata = GetGameData(DataFileHeroTallentBorn,skillId,"stHeroTallentBornData")
   local imageView = ImageView:create()--UI_GetUIImageView(widget,16)
   --imageView:removeAllChildren()
   imageView:loadTexture("Common/shop_talk_bg.png")
   imageView:setAnchorPoint(ccp(0,0.5))
   local curInfolabel = Label:create()
   local width = 250 
   local height = imageView:getSize().height
   imageView:setScale9Enabled(true)
   imageView:setSize(CCSizeMake(width,height))
   if isAc then 
      local labelone = CompLabel:GetCompLabel(FormatString("HeroInfoSecond_Heroinfo",skilldata.m_Info), width - 10, kCCTextAlignmentLeft)
      imageView:addChild(labelone)
      local labeloneHeight = labelone:getContentSize().height 
      local labeltwo = CompLabel:GetCompLabel(FormatString("HeroInfoSecond_CurSkillInfo1",skilldata.m_curskillinfo), width - 10, kCCTextAlignmentLeft)
      imageView:addChild(labeltwo)
      local labeltwoHeight = labeltwo:getContentSize().height
      local labeltree = CompLabel:GetCompLabel(FormatString("HeroInfoSecond_NextSkillInfo1",skilldata.m_nextskillinfo), width - 10, kCCTextAlignmentLeft)
      imageView:addChild(labeltree)
      local labeltreeHeight = labeltree:getContentSize().height
      local tempheight = labeloneHeight + labeltwoHeight + labeltreeHeight
      if tempheight <= height then 
         tempheight = height 
      else
         imageView:setSize(CCSizeMake(width,tempheight + 10)) 
      end 
      labelone:setPosition(ccp(5,tempheight / 2 - 5))
      labeltwo:setPosition(ccp(5,tempheight / 2 - labeloneHeight - 5))
      labeltree:setPosition(ccp(5,tempheight / 2 - labeloneHeight - labeltwoHeight - 5))
   else 
      local labelone = CompLabel:GetCompLabel(FormatString("HeroInfoSecond_Heroinfo",skilldata.m_Info), width - 10, kCCTextAlignmentLeft)
      local labeloneheight = labelone:getContentSize().height
      imageView:addChild(labelone)
      local tempheight = labeloneheight 
      if labeloneheight <= height then 
         tempheight = height
         labelone:setPosition(ccp(5,labeloneheight / 2))
      else
         imageView:setSize(CCSizeMake(width,tempheight + 10)) 
         labelone:setPosition(ccp(5,tempheight / 2 - 5))
      end      
   end 
     local layout = UI_GetUILayout(UI_GetUILayout(widget,7),tag)
     local layoutWidth = layout:getContentSize().width
--   Log("height==="..height)
--   Log("strlabHeight==="..strlabHeight)
   local layoutHeight = layout:getContentSize().height
   local layoutPosX,layoutPosY = layout:getPosition()
   imageView:setPosition(ccp(layoutPosX + layoutWidth,layoutPosY + layoutHeight / 2))
   widget:addChild(imageView,999,999)
end 


function HeroSystem_getString(src,tb)
    -- body
    Log("src==="..tostring(src))
    local function spilit(txt)
        -- body
        Log("txt========="..txt)
        table.insert(tb,txt)
    end
    string.gsub(src.."#","(.-)#",spilit)
end

function HeroSystem_AfterTallentSkillChange(widget,heroEntity,vars)
    Log("xxxxxxxxxxxxxxxxxxxxxx")
    if widget then 
        if g_heroCurInfo_TallentId_Select == vars[0]:Int() then 
        --HeroSystem_HeroTallentSkillShow(widget,heroEntity)
        local tallentID = vars[0]:Int() * 1000 + vars[1]:Int()
        Log("tallentID====="..vars[0]:Int())
        local pos = HeroCurInfoSecond_GetTallentPos(vars[0]:Int(),heroEntity)
        Log("pos============="..pos)
        local skilldata = GetGameData(DataFileHeroTallentBorn,tallentID,"stHeroTallentBornData")  
        local src = skilldata.m_skillInfoShow
        local tb = {}
        HeroSystem_getString(src,tb)
        local herotallentlayout = UI_GetUILayout(widget, 7)
        local heroTallentTag = {1,2,3,4}
        local skilllayout = UI_GetUILayout(herotallentlayout,heroTallentTag[pos])
        local actiontab = {}
        local infotab = {}
        local offtime = 0.35
        local templayout = Layout:create()
        for i = 1,#tb do 
           local actionArry = CCArray:create()
           local delaytime = CCDelayTime:create(offtime * i)
           actionArry:addObject(delaytime)
           local callfuncone = CCCallFuncN:create(OtherPromptBox_Visible)
           actionArry:addObject(callfuncone)
           local actionone = CCMoveBy:create(offtime * i,ccp(0,30))
           actionArry:addObject(actionone)
           local callfunc = CCCallFuncN:create(OtherPromptBox_removeAction)
           actionArry:addObject(callfunc)
           local seq = CCSequence:create(actionArry)
           table.insert(actiontab,seq)

           local label = Label:create()
           UI_SetLabelFontName(1,label)
           label:setText(tb[i])
           label:setPosition(ccp(0,-10))
           UI_SetLabelFontName(1,label)
           label:enableStroke()
           label:setColor(ccc3(250,128,10))
           label:setFontSize(20)
           label:setVisible(false)
           table.insert(infotab,label)
           templayout:addChild(label)
        end
        for i = 1,#tb do 
           infotab[i]:runAction(actiontab[i])
        end 
        local skillimage = UI_GetUIImageView(UI_GetUIImageView(skilllayout,1),2)
--        if skillimage:getChildren() then 
--           skillimage :removeAllChildren()
--        end 
         skillimage:addChild(templayout)
         templayout:setPosition(ccp(0,skillimage:getContentSize().height / 2 - 10))
--        OtherPromptBox_Init()
--        for i = 1,#tb do 
--           OtherPromptBox_InitValue(tb[i])
--        end 
--        OtherPromptBox_StarPromptAction()
   end 
   end 
end 

--英雄展示
function HeroCurInfoSecond_ShowUpdateStarLayout(info)
   local widget = UI_CreateBaseWidgetByFileName("HeroAscendingShow.json",EUIOpenAction_Enlarge,0.25)
   local heroEntity = GetLocalPlayer():GetHeroByObjId(info.dwHeroObjectID)
   --oldStateimage 
   local data = GetGameData(DataFileHeroBorn,heroEntity:GetUint(EHero_HeroId),"stHeroBornData")
   UI_GetUIImageView(UI_GetUILayout(widget, 2),1):addChild(UI_IconFrame(UI_GetHeroIcon(data.m_icon,-1),info.oldState.iLevelStep,heroEntity:GetUint(EHero_Quality)))
   --UI_GetHeroIncoByClone(data.m_icon,UI_GetUILayout(widget, 2),2)
   --UI_IconFrameByClone(UI_GetUILayout(widget, 2),1,info.oldState.iLevelStep + 1)
   --UI_IconFrameByPingjie(UI_GetUILayout(widget, 2),1,heroEntity:GetUint(EHero_Quality))
   --local oldstartagTab = {1,2,3,4,5}
   --setHeroStarImageByClone(UI_GetUILayout(UI_GetUILayout(widget, 2), 3),oldstartagTab,info.oldState.iLevelStep + 1)

   --newstateimage
   UI_GetUIImageView(UI_GetUILayout(widget, 3),1):addChild(UI_IconFrame(UI_GetHeroIcon(data.m_icon,-1),info.newState.iLevelStep,heroEntity:GetUint(EHero_Quality)))
   --UI_GetHeroIncoByClone(data.m_icon,UI_GetUILayout(widget, 3),2)
   --UI_IconFrameByClone(UI_GetUILayout(widget, 3),1,info.newState.iLevelStep + 1)
   --UI_IconFrameByPingjie(UI_GetUILayout(widget, 3),1,heroEntity:GetUint(EHero_Quality))
   --local newstartagTab = {1,2,3,4,5}
   --setHeroStarImageByClone(UI_GetUILayout(UI_GetUILayout(widget, 3), 3),newstartagTab,info.newState.iLevelStep + 1)

   --生命成长
   UI_SetLabelText(UI_GetUILayout(widget, 4), 1, (info.oldState.iHPGrow / 10000))  
   UI_SetLabelText(UI_GetUILayout(widget, 4), 2, (info.newState.iHPGrow - info.oldState.iHPGrow) / 10000)
   UI_SetLabelText(UI_GetUILayout(widget, 4), 3, FormatString("HeroSystem_AscendingLifeShow",info.iHPAddValue))
   local lifepositionX,lifepositionY = UI_GetUILabel(UI_GetUILayout(widget, 4), 1):getPosition()
   local lifewidth = UI_GetUILabel(UI_GetUILayout(widget, 4), 1):getContentSize().width 
 --  UI_GetUILabel(UI_GetUILayout(widget, 4), 2):setPosition(ccp(lifepositionX + lifewidth + 5,lifepositionY))
  -- UI_GetUILabel(UI_GetUILayout(widget, 4), 3):setPosition(ccp(lifepositionX + lifewidth + 5 + UI_GetUILabel(UI_GetUILayout(widget, 4), 2):getContentSize().width + 5,lifepositionY))

   --攻击成长
   UI_SetLabelText(UI_GetUILayout(widget, 5), 1, (info.oldState.iAttGrow / 10000))
   UI_SetLabelText(UI_GetUILayout(widget, 5), 2, (info.newState.iAttGrow - info.oldState.iAttGrow) / 10000)
   UI_SetLabelText(UI_GetUILayout(widget, 5), 3, FormatString("HeroSystem_AscendingAttShow",info.iAttAddValue))
   local attpositionX,attpositionY = UI_GetUILabel(UI_GetUILayout(widget, 5), 1):getPosition()
   local attwidth = UI_GetUILabel(UI_GetUILayout(widget, 5), 1):getContentSize().width 
  -- UI_GetUILabel(UI_GetUILayout(widget, 5), 2):setPosition(ccp(attpositionX + attwidth + 5,attpositionY))
  -- UI_GetUILabel(UI_GetUILayout(widget, 5), 3):setPosition(ccp(attpositionX + attwidth + 5 + UI_GetUILabel(UI_GetUILayout(widget, 5), 2):getContentSize().width + 5,attpositionY))

   --防御成长
   UI_SetLabelText(UI_GetUILayout(widget, 8), 1 , (info.oldState.iDefGrow / 10000))
   UI_SetLabelText(UI_GetUILayout(widget, 8), 2, (info.newState.iDefGrow - info.oldState.iDefGrow) / 10000)
   UI_SetLabelText(UI_GetUILayout(widget, 8), 3, FormatString("HeroSystem_AscendingDefShow",info.iDefAddValue))
   local defpositionX,defpositionY = UI_GetUILabel(UI_GetUILayout(widget, 8), 1):getPosition()
   local defwidth = UI_GetUILabel(UI_GetUILayout(widget, 8), 1):getContentSize().width 
   --UI_GetUILabel(UI_GetUILayout(widget, 8), 2):setPosition(ccp(defpositionX + defwidth + 5,defpositionY))
   --UI_GetUILabel(UI_GetUILayout(widget, 8), 3):setPosition(ccp(defpositionX + defwidth + 5 + UI_GetUILabel(UI_GetUILayout(widget, 8), 2):getContentSize().width + 5,defpositionY))
   local godanimalanimatrue = GetUIArmature("chouherotishi")
   godanimalanimatrue:setPosition(ccp(0,0))
   godanimalanimatrue:getAnimation():playWithIndex(0) 
   UI_GetUIImageView(widget,4519205):addNode(godanimalanimatrue) 
end 

--[[" 
      网络消息
"]]

ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_HERO_EQUIP_FAVORITE, "HeroFavorite_EquipHeroFavorite" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_HERO_QUALITY_UP, "HeroFavorite_HeroAdvanced_SC" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_QUERY_EQUIPSOURCE, "HeroFavorite_LookFavoriteSource_SC" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_HERO_COMBINE_FAVORITE, "HeroFavorite_SummondFavorite_SC" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_QUERY_HERO_LEVELINFO, "HeroCurInfoSecond_UpgradeInfo_SC" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_HERO_LEVELUP, "HeroCurInfoSecond_AfterHeroUpgradeView_SC" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_HERO_SANGONG, "HeroSystem_AfterSangongView_SC" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_QUERY_SANGONG, "HeroSystem_AfterLookUpSangongView_SC" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_GET_HERODESC, "HeroSystem_HeroDescView_SC" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_HERO_TALLENT_LEVELUP, "HeroSystem_AfterHeroTallentLevelUp_SC" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_HERO_LEVELSTEPUP, "HeroSystem_HeroAscending_AfterAscending_SC" )



function HeroFavorite_EquipHeroFavorite(pkg)
   EndWaiting()
   local info = GameServer_pb.CMD_HERO_EQUIP_FAVORITE_SC()
   info:ParseFromString(pkg)
   Log("info===="..tostring(info))
   UI_CloseCurBaseWidget(EUICloseAction_FadeOut, 0.25)
   local widget = UI_GetBaseWidgetByName("HeroSystemSecond")
   GetLocalPlayer():EquipHeroFavoriteByHeroObject(info.iItemID,info.dwHeroObjectID,info.iPos)
   GetLocalPlayer():PlayerHeroFavoriteCheck()
   if widget then 
      HeroSecond_UpdateAfterEquipHeroFavorite(widget,info)
      CreateGameEffectMusic("equip.wav")
   end 
end

function HeroFavorite_HeroAdvanced_SC(pkg) 
   EndWaiting()
   local info = GameServer_pb.CMD_HERO_QUALITY_UP_SC()
   info:ParseFromString(pkg)
   Log("info===="..tostring(info))
   local widget = UI_GetBaseWidgetByName("HeroSystemSecond")
   GetLocalPlayer():UpdateHeroFavoriteByHeroObjectId(info.dwHeroObjectID)
   --GetLocalPlayer():PlayerHeroFavoriteCheck()
   if widget then  
      HeroSecond_AfterAdvancedSuccess(widget,info)
      CreateGameEffectMusic("herolevelstep.wav")
   end 
end 

function HeroFavorite_LookFavoriteSource_SC(pkg)
   EndWaiting()
   local info = GameServer_pb.CMD_QUERY_EQUIPSOURCE_SC()
   info:ParseFromString(pkg)
   Log("info===="..tostring(info))
   local widget = UI_GetBaseWidgetByName("HeroSystemLoveThingOne")
   if widget then
      HeroFavorite_GetFavoriteSourInfo(widget,info) 
   else 
      --武魂来源
      HeroSoulSour_GetHeroSoulInit(info)
   end 
end 

function HeroFavorite_SummondFavorite_SC(pkg)
   EndWaiting()
   local info = GameServer_pb.CMD_HERO_COMBINE_FAVORITE_SC()
   info:ParseFromString(pkg)
   Log("info====="..tostring(info))
   local widget = UI_GetBaseWidgetByName("HeroSystemLoveThingOne")
   if widget then
      HeroFavorite_AfterSummondUpdate(widget,info) 
   end 
end 

function HeroCurInfoSecond_UpgradeInfo_SC(pkg)
   EndWaiting()
   Log("aaaaaaaaaaaaaaa")
   local info = GameServer_pb.Cmd_Sc_QueryHeroLevelInfo()
   info:ParseFromString(pkg)
   Log("info ==="..tostring(info))
   local widget = UI_GetBaseWidgetByName("HeroSystemSecond")
   g_heroCurInfoLayoutState = 2
   local heroEntity = GetLocalPlayer():GetHeroByObjId(info.levelInfo.dwHeroObjectID)
   if widget then
      if UI_GetUILayout(widget,5):isVisible() then 
         HeroCurInfoSecond_AfterLookUpHeroUpdateInfo(widget,info)
         HeroCurInfoSecond_HeroBaseInfoShow(widget,heroEntity)
         HeroCurInfoSecond_HeroUpdateStar(widget,heroEntity)
         HeroCurInfoSecond_HeroLeftAndRightButtonFunc(widget)
      else 
         HeroCurInfoSecond_ShowCurLayout(widget,heroEntity)
         HeroCurInfoSecond_AfterLookUpHeroUpdateInfo(widget,info)
      end 
   end 
end 

function HeroCurInfoSecond_AfterHeroUpgradeView_SC(pkg)
   Log("xxxxxxxxxxxxxxxxx")
   local info = GameServer_pb.Cmd_Sc_HeroLevelUp()
   info:ParseFromString(pkg)
   local widget = UI_GetBaseWidgetByName("HeroSystemSecond")
   if widget then
      HeroSystem_ShowXiuweiDianjiAnimature(widget,0)
      HeroCurInfoSecond_AfterHeroUpdateView(widget,info)
   end 
end 

function HeroSystem_AfterSangongView_SC(pkg)
   EndWaiting()
   local info = GameServer_pb.CMD_HERO_SANGONG_SC()
   info:ParseFromString(pkg)
   local widget = UI_GetBaseWidgetByName("HeroSystemSecond")
   if widget then 
      HeroSystem_ShowXiuweiDianjiAnimature(widget,1)
      local tab = GameServer_pb.Cmd_Cs_QueryHeroLevelInfo();
      tab.dwHeroObjectID = info.dwObjectID
      Packet_Full(GameServer_pb.CMD_QUERY_HERO_LEVELINFO,tab)
      ShowWaiting()
   end  
end 

function HeroSystem_HeroDescView_SC(pkg)
   EndWaiting()
   local widget = UI_GetBaseWidgetByName("HeroSystemSecond")
   local info = GameServer_pb.Cmd_Sc_GetHeroDesc()
   info:ParseFromString(pkg)
   Log("info===="..tostring(info))
   local heroEntity = GetLocalPlayer():GetHeroByObjId(info.dwObjectID)
   g_heroCurInfoLayoutState = 3
   if widget then 
      if UI_GetUILayout(widget, 6):isVisible() == false then
         HeroCurInfoSecond_ShowCurLayout(widget,heroEntity)
      else 
         HeroCurInfoSecond_HeroBaseInfoShow(widget,heroEntity)
         HeroCurInfoSecond_HeroUpdateStar(widget,heroEntity)
         HeroCurInfoSecond_HeroLeftAndRightButtonFunc(widget)
      end 
      HeroCurInfoSecond_HeroCurBaseInfo(widget,info)
   end 
end 

function HeroSystem_AfterHeroTallentLevelUp_SC(pkg)
   EndWaiting()
end

function HeroSystem_HeroAscending_AfterAscending_SC(pkg)
   EndWaiting()
   local info = GameServer_pb.CMD_HERO_LEVELSTEPUP_SC()
   info:ParseFromString(pkg)
   Log("info===="..tostring(info))
   local widget = UI_GetBaseWidgetByName("HeroSystemSecond")
   if widget then 
      local heroEntity = GetLocalPlayer():GetHeroByObjId(info.dwHeroObjectID)
      HeroCurInfoSecond_ShowUpdateStarLayout(info)
      HeroCurInfoSecond_HeroUpdateStar(widget,heroEntity)
      HeroCurInfoSecond_HeroNameInfo(widget,heroEntity)
      HeroCurInfoSecond_HeroFighting(widget,heroEntity)
      CreateGameEffectMusic("herolevelstep.wav")
   end 
end  

function HeroSystem_AfterLookUpSangongView_SC(pkg)
   EndWaiting()
   local info = GameServer_pb.CMD_QUERY_SANGONG_SC()
   info:ParseFromString(pkg)
   Log("info===="..tostring(pkg))
   HeroSangGongFunction(info)
end 