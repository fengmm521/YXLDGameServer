
--[[" 
      herolist界面第二版 Begin
"]]

--选中的herolistindex
if g_herolistClickIndex == nil then
   g_herolistClickIndex = 0
end 

function HeroListSecond_GetHeroEntityByIndex()
   local playerherolist = vector_uint_:new_local()
   GetLocalPlayer():GetCanSummonHeroAndHeroList(playerherolist)
   Log("xxxxxxxxxxxsssssssssswwweeeeeee ==="..playerherolist[g_herolistClickIndex])
   local heroEntity = GetEntityById(playerherolist[g_herolistClickIndex],"Hero")
   return heroEntity
end 

function HeroListSecond_InitValue()
   g_herolistClickIndex = 0
end 
function HeroListSecond_CreateHeroListLayout()
   local widget = UI_CreateBaseWidgetByFileName("HeroListSecond.json")
   local function HeroSystem_ClickReturn(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then 
         UI_CloseCurBaseWidget(EUICloseAction_FadeOut,0.25)
      end 
   end 
   UI_GetUIButton(widget, 1):addTouchEventListener(HeroSystem_ClickReturn)

   --屏蔽背景BG的按键响应
   UI_GetUILayout(widget, 4742867):addTouchEventListener(PublicCallBackWithNothingToDo)
   GetLocalPlayer():GetHeroList(true)
   HeroListSecond_InitValue()
   HeroListSecond_InitHeroListShow(widget)
end

function HeroListSecond_InitHeroListShow(widget)
   local herolist = UI_GetUIListView(UI_GetUILayout(widget, 2),1)
   herolist:removeAllItems()
   local listwidth = herolist:getContentSize().width
   local listheight = herolist:getContentSize().height 
   local layout = nil
   local curlayout = nil 
   local cansummondHerolist = vector_uint_:new_local()
   GetLocalPlayer():GetCanSummonHeroList(cansummondHerolist)
   local cansummondHeroSize = cansummondHerolist:size()
   local playerherolist = vector_uint_:new_local()
   GetLocalPlayer():GetCanSummonHeroAndHeroList(playerherolist)

   local function createCanSummomdHeroTempLayout(index)
       local heroId = playerherolist[index]
       local data = GetGameData(DataFileHeroBorn, heroId, "stHeroBornData")
       --icon
	   print("data.m_icon========="..data.m_icon)
       UI_GetHeroIncoByClone(data.m_icon,curlayout,1)
       --pingjie
      -- UI_GetHeroLevelStepIcon(curlayout,3,data.m_heroInitQuality)
       --frame
       UI_IconFrameByPingjie(curlayout,2,data.m_heroInitQuality)
       --name
       local namelayout = UI_GetUILayout(curlayout, 4)
       namelayout:removeAllChildren()
       local namelab = GetHeroNameByLabel(data.m_name,data.m_heroInitQuality,24)
       namelab:setAnchorPoint(ccp(0.5,0.0))
       namelayout:addChild(namelab)
       --leixing
	   UI_GetUIImageView(curlayout,5):loadTexture("Common/Icon_001A_0"..data.m_heroType..".png")
       --UI_GetHeroTypeIconByTag(curlayout,5,data.m_heroType)
       --lvl
       --UI_SetLabelText(curlayout, 6, 1)
       --star 
      -- local startagTab = {1,2,3,4,5}
      -- setHeroStarImageByClone(UI_GetUILayout(curlayout, 9),startagTab,data.m_heroInitStar + 1)

       --loadingbar
       UI_GetUILoadingBar(curlayout, 11):setPercent(100)
       UI_SetLabelText(curlayout, 12, FormatString("HeroListSecond_CanSummond"))
       if curlayout:getNodeByTag(1000) then 
          curlayout:removeNodeByTag(1000)
       else 
          local animaaction = GetUIArmature("jihuotishi")
          curlayout:addNode(animaaction,1000,1000)
          animaaction:getAnimation():playWithIndex(0)
          animaaction:setPosition(ccp(curlayout:getContentSize().width / 2 ,curlayout:getContentSize().height / 2 - 10))
       end 
   end 

   local function createPlayerHeroTempLayout(index)
      local heroObjectId = playerherolist[index]
      local heroEntity = GetEntityById(heroObjectId,"Hero")
      Log("EHero_HeroId =="..heroEntity:GetUint(EHero_HeroId))
      local data = GetGameData(DataFileHeroBorn,heroEntity:GetUint(EHero_HeroId),"stHeroBornData")
      --icon 
      --UI_GetHeroIncoByClone(data.m_icon,curlayout,1)
      --
	  UI_GetUIImageView(curlayout,2):addChild(UI_IconFrame(UI_GetHeroIcon(data.m_icon,heroEntity:GetUint(EHero_Lvl)),heroEntity:GetUint(EHero_LvlStep),heroEntity:GetUint(EHero_Quality)))
      --UI_IconFrameByPingjie(curlayout,2,heroEntity:GetUint(EHero_Quality))
      --pingjie
      --UI_GetHeroLevelStepIcon(curlayout,3,heroEntity:GetUint(EHero_Quality))

      --点点
      --local tagtable = {20,21,22}
      --GetHeroQualityIcon(heroEntity:GetUint(EHero_Quality),curlayout,tagtable)

      --name
      local namelayout = UI_GetUILayout(curlayout, 4)
      namelayout:removeAllChildren()
      local namelab = GetHeroNameByLabel(data.m_name,heroEntity:GetUint(EHero_Quality),24)
      namelab:setAnchorPoint(ccp(0.5,0.0))
      namelayout:addChild(namelab)
      --leixing
	  UI_GetUIImageView(curlayout,11):loadTexture("Common/Icon_001A_0"..data.m_heroType..".png")
      --UI_GetHeroTypeIconByTag(curlayout,11,data.m_heroType)
      --lvl
      UI_SetLabelText(curlayout, 12, heroEntity:GetUint(EHero_Lvl))
      --star 
      --local startagTab = {1,2,3,4,5}
      --setHeroStarImageByClone(UI_GetUILayout(curlayout, 15),startagTab,heroEntity:GetUint(EHero_LvlStep) + 1)
      if GetLocalPlayer():IsInFormation(heroEntity) then
		 UI_GetUIImageView(curlayout, 13):setVisible(true)
	  end
      --favorite
      local isRed = false 
      local favoritelist = heroEntity:GetHeroFavorite():GetHeroFavoriteState()
      local allfavoritelist = heroEntity:GetHeroFavorite():GetHeroFavoriteId()
      local favoriteTag = {5,6,7,8,9,10}
      for i = 0,favoritelist:size() - 1 do 
         local parentImage = UI_GetUIImageView(curlayout, favoriteTag[i + 1])
         local image = UI_GetUIImageView(parentImage, 1)
         if favoritelist[i] == NoHaveHeroFavorite then 
            image:setVisible(false)
         elseif favoritelist[i] == HaveEquipFavorite then 
            local itemId = allfavoritelist[i]
            local data = GetGameData(DataFileItem,itemId,"stItemData")
            image:loadTexture("Icon/Item/"..data.m_icon..".png")
         elseif favoritelist[i] == CanEquipFavorite then 
            image:loadTexture("HeroSystem/Bg_014.png")
            isRed = true 
         elseif favoritelist[i] == CannotEquipFavorite then 
            image:loadTexture("HeroSystem/Bg_013.png")
         end 
      end 
--      local equipetag = {1,2,3,4,5,6}
--      for i = 1 , #equipetag do 
--          if GetLocalPlayer():GetEquipBags():HeroHasCanEquip(equipetag[i] - 1,data.m_heroType) then 
--             isRed = true 
--             break
--          end 
--      end 
      if isRed == true then 
         UI_GetUIImageView(curlayout, 14):setVisible(true)
      else 
         UI_GetUIImageView(curlayout, 14):setVisible(false)
      end 
   end 
   local function HeroCurInfoView(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then 
         playUiEffect()
         local sender = tolua.cast(sender,"Layout")
         local tag = sender:getTag()
         Log("tag ==="..tag)
         g_herolistClickIndex = tag
         local heroEntity = GetEntityById(playerherolist[tag],"Hero")
         UI_CloseCurBaseWidget(EUICloseAction_None)
         HeroCurInfoSecond_CreateHeroCurLayout(heroEntity)
      end 
   end 

   local function summondHeroFunc(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then 
         playUiEffect()
         local sender = tolua.cast(sender,"Layout")
         local tag = sender:getTag()
         Log("tag ==="..tag)
         local heroId = playerherolist[tag]
         Log("heroID =="..heroId)
         local herodata = GetGameData(DataFileHeroBorn, heroId, "stHeroBornData")
         local function BesureSummondHero()       
            local tab = GameServer_pb.CMD_HERO_CALLHERO_CS()
            tab.iHeorID = heroId
            Packet_Full(GameServer_pb.CMD_HERO_CALLHERO,tab)
			
			if g_isGuide then
				Guide_GoNext();
			end
			
            ShowWaiting()
         end  
		 
		 if g_isGuide then
			Guide_GoNext();
		 end
		 
         Messagebox_Create({info = FormatString("HeroSecondList_HeroSummondCostSliver",herodata.m_heroSummondCost),title = FormatString("HeroSecondList_BeSummondHero"), msgType = EMessageType_LeftRight, leftFun = BesureSummondHero})                 
      end 
   end 
   for i = 0, playerherolist:size() - 1 do 
      local row = i % 2
      if row == 0 then 
         layout = nil 
         layout = Layout:create()
         layout:setSize(CCSizeMake(listwidth,UI_GetUILayout(widget, 3):getContentSize().height))
      end
      Log("cansummondHeroSize==="..cansummondHeroSize)
      if i < cansummondHeroSize then  
         curlayout = UI_GetCloneLayout(UI_GetUILayout(widget,4))
         curlayout:setVisible(true)
         createCanSummomdHeroTempLayout(i)
         curlayout:setTouchEnabled(true)
         curlayout:addTouchEventListener(summondHeroFunc)
      else 
         curlayout = UI_GetCloneLayout(UI_GetUILayout(widget,3))
         curlayout:setVisible(true)
         createPlayerHeroTempLayout(i)
         curlayout:setTouchEnabled(true)
		 curlayout:setName("hero_"..i)
         curlayout:addTouchEventListener(HeroCurInfoView)      
      end   
      curlayout:setPosition(ccp(row * herolist:getContentSize().width / 2,0 ))
      layout:addChild(curlayout,i,i)
      if row == 1 or i == playerherolist:size() - 1 then 
         herolist:pushBackCustomItem(layout)
      end
   end
   local templayout = UI_GetCloneLayout(UI_GetUILayout(widget, 5))
   templayout:setVisible(true)
   herolist:pushBackCustomItem(templayout)

   --cannot summond hero
   local star = playerherolist:size()
   local cannotSummondHerolist = vector_uint_:new_local()
   GetLocalPlayer():GetNotSummonHeroList(cannotSummondHerolist)
   Log("herolistsize ==="..cannotSummondHerolist:size())
   local function createCannotSummondHeroTemp(index)
       local heroId = cannotSummondHerolist[index]
       Log("heroId===="..heroId)
       local data = GetGameData(DataFileHeroBorn, heroId, "stHeroBornData")
       --icon
       UI_GetHeroIncoByClone(data.m_icon,curlayout,1)
       --pingjie
       ----UI_GetHeroLevelStepIcon(curlayout,3,data.m_heroInitQuality)
       --frame
       UI_IconFrameByPingjie(curlayout,2,data.m_heroInitQuality)
       --name
       local namelayout = UI_GetUILayout(curlayout, 4)
       namelayout:removeAllChildren()
       Log("m_heroInitQuality==="..data.m_heroInitQuality)
       local namelab = GetHeroNameByLabel(data.m_name,data.m_heroInitQuality,24)
       namelab:setAnchorPoint(ccp(0.5,0.0))
       namelayout:addChild(namelab)
       --leixing
		UI_GetUIImageView(curlayout,5):loadTexture("Common/Icon_001A_0"..data.m_heroType..".png")
       --UI_GetHeroTypeIconByTag(curlayout,5,data.m_heroType)
       --loadingbar
       local neednumber = data.m_heroNeedSoulNumber
       local playerhaveNumber = GetLocalPlayer():GetPlayerBag():GetItemCountByItemId(data.m_heroSoulItemId)
       UI_GetUILoadingBar(curlayout, 11):setPercent(playerhaveNumber / neednumber * 100)
       UI_SetLabelText(curlayout, 12, playerhaveNumber.."/"..neednumber)
   end 

   local function HeroCurInfoSecond_LookHeroSoulSource(data)
        local widget = UI_CreateBaseWidgetByFileName("HeroSoulSource.json", EUIOpenAction_FadeIn,0.5)
        --local data = GetGameData(DataFileHeroBorn,heroEntity:GetUint(EHero_HeroId),"stHeroBornData")
        --image
        UI_GetHeroIncoByClone(data.m_icon,UI_GetUIImageView(widget, 10),1)

        UI_SetLabelText(widget, 4, FormatString("NewHeroList_HeroSoulName",data.m_name))

        --click
        --商店
        local function clickShop(sender,eventType)
            if eventType == TOUCH_EVENT_ENDED then
                UI_CloseAllBaseWidget()
                Shop_Create()
            end 
        end 
        UI_GetUILayout(widget,1):setTouchEnabled(true)
        UI_GetUILayout(widget,1):addTouchEventListener(clickShop)

        --荣誉
        local function clickRongyuShop(sender,eventType)
            if eventType == TOUCH_EVENT_ENDED then
                UI_CloseAllBaseWidget()
                Shop_Create()
                Shop_getDataExchagne()
            end 
        end 
        UI_GetUILayout(widget,2):setTouchEnabled(true)
        UI_GetUILayout(widget,2):addTouchEventListener(clickRongyuShop)

        --领地
        local function clickLingdi(sender,eventType)
            if eventType == TOUCH_EVENT_ENDED then
                Packet_Cmd(GameServer_pb.CMD_MANOR_QUERY);
	            ShowWaiting();
            end 
        end 
        UI_GetUILayout(widget,3):setTouchEnabled(true)
        UI_GetUILayout(widget,3):addTouchEventListener(clickLingdi)
   end 

   local function clickLookHeroSoulSource(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then 
         playUiEffect()
         local sender = tolua.cast(sender,"Layout")
         local tag = sender:getTag() - star 
         local heroId = cannotSummondHerolist[tag]
         Log("heroID =="..heroId)
         local herodata = GetGameData(DataFileHeroBorn, heroId, "stHeroBornData")
         --HeroCurInfoSecond_LookHeroSoulSource(herodata)
         local tab = GameServer_pb.CMD_QUERY_EQUIPSOURCE_CS()
         tab.iEquipID = herodata.m_heroSoulItemId
         Packet_Full(GameServer_pb.CMD_QUERY_EQUIPSOURCE,tab)
         ShowWaiting()
      end 
   end 
   for i = 0,cannotSummondHerolist:size() - 1 do 
      Log("i====="..i)
      local row = i % 2
      if row == 0 then 
         layout = nil 
         layout = Layout:create()
         layout:setSize(CCSizeMake(listwidth,UI_GetUILayout(widget, 3):getContentSize().height))
      end 
      curlayout = UI_GetCloneLayout(UI_GetUILayout(widget, 4))
      curlayout:setVisible(true)
      curlayout:setPosition(ccp(row * herolist:getContentSize().width / 2,0))
      curlayout:setTouchEnabled(true)
      curlayout:addTouchEventListener(clickLookHeroSoulSource)
      layout:addChild(curlayout,i + star,i + star)
      createCannotSummondHeroTemp(i)
      --curlayout:setColor(ccc3(125,125,125))
      UI_GetUIImageView(curlayout,1):setColor(ccc3(125,125,125))
      if row == 1 or i == cannotSummondHerolist:size() - 1 then 
         herolist:pushBackCustomItem(layout)
      end 
   end 
end 

--[[" 
      herolist界面第二版 End
"]]


function HeroListSecond_SummondHero(widget,info)
   HeroListSecond_InitHeroListShow(widget)
end 




ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_HERO_CALLHERO, "HeroListSecond_SummondHero_SC" )

function HeroListSecond_SummondHero_SC(pkg)
   EndWaiting()
   local info = GameServer_pb.CMD_HERO_CALLHERO_SC()
   info:ParseFromString(pkg)
   Log("info ==="..tostring(info))
   local widget = UI_GetBaseWidgetByName("HeroListSecond")
   if widget then 
      HeroListSecond_SummondHero(widget,info)
   end 
   Shop_CallHero(info.iHeroID,0,false)
end 