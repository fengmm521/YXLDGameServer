--网络消息：发送上阵信息
function Formation_SendMessage(objID,location)
	if (UI_IsCurBaseWidgetByName("Formation"))then
		local tab = GameServer_pb.Cmd_Cs_FormationChg();
		tab.dwObjectID = objID
		tab.iTargetPos = location;	
		Packet_Full(GameServer_pb.CMD_FORMATION_CHG, tab);
		ShowWaiting();
	elseif (UI_IsCurBaseWidgetByName("NineSkyFormation"))then
		local tab = GameServer_pb.CMD_DREAMLAND_FORMATION_CHG_CS();
		tab.dwObjectID = objID;
		tab.iTargetPos = g_Formation_LocationHero;	
		Packet_Full(GameServer_pb.CMD_DREAMLAND_FORMATION_CHG, tab);
		ShowWaiting();
	end
 
end
function Formation_Create()
	local widget = UI_CreateBaseWidgetByFileName("Formation.json");
	if (widget == nil)then
		Log("Formation_Create error");
		return;
	end
	UI_GetUIButton(widget, 10):addTouchEventListener(UI_ClickCloseCurBaseWidgetWithNoAction);
    local leftImage = UI_GetUIImageView(widget, 4525686)
    leftImage:setVisible(false)
    
    --被选中的英雄位置
    g_Formation_LocationHero = nil
    
    --是否显示第一个英雄的数据
    g_showFirstHero = true
	--刷新战斗组队界面
	Formation_Refresh(widget)
    --添加主公技能
    local rightImage = UI_GetUIImageView(widget, 4484301)
    for i=1, 3,1 do  
        Formation_addSkill(rightImage,i)
    end


end
function Formation_addSkill(widget,id)
    --添加技能说明
    local function showInfo(send,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            SkillTipForActor(tolua.cast(send,"ImageView"):getTag())
        end
    end
    local SkillData = GetGameData(DataFileActorSkill, id, "stActorSkillData")
    local skillIcon = UI_ItemIconFrame(UI_GetSkillIcon(SkillData.m_icon), 7)

    UI_GetUIImageView(widget, 100+id):removeAllChildren()

    UI_GetUIImageView(widget, 100+id):addChild(skillIcon,10001,10001)
    
    skillIcon:setTag( id )
    skillIcon:setTouchEnabled(true)
    skillIcon:addTouchEventListener(showInfo)

    local label = UI_GetUILabel(widget, 110+id)
    local playerLvl = GetLocalPlayer():GetInt(EPlayer_Lvl)

    if playerLvl >= SkillData.m_level then
        label:setText(SkillData.m_name)
        label:setColor(ccc3(255,255,230))
    else
        --灰色
        skillIcon:getChildByTag(1):addColorGray()
        --skillIcon:getChildByTag(1):getChildByTag(3):addColorGray()
        label:setText(SkillData.m_level..FormatString("LevelRequirements"))    
        label:setColor(ccc3(145,35,35))
    end
end

--添加动画
function Formation_AddAnimation(widget,animationName,tag)
    if tag ==199 then --199表示这是一个被指定了tag指动画，这个动画在武魂殿被使用且widget还存在别的孩子
        if widget:getChildByTag(tag) then
            widget:removeChildByTag(199)
        end
    else 
        widget:removeAllChildren()
    end
    local animalPic = widget;
    local layout = Layout:create();
    local godanimalanimatrue = GetArmature(animationName)
    godanimalanimatrue:setPosition(ccp(animalPic:getPositionX(),animalPic:getPositionY()))
    godanimalanimatrue:getAnimation():playWithIndex(0) 
    godanimalanimatrue:getAnimation():setMovementEventCallFunc(GodAnimalSystem_AnimEnd) 
    godanimalanimatrue:setPosition(ccp(0,0))
    layout:setAnchorPoint(ccp(0.5,0))
    layout:addNode(godanimalanimatrue)
    widget:addChild(layout,5,tag)
end
--刷新战斗组队界面
function Formation_Refresh(widget)
	EndWaiting();
	local formationList = GetLocalPlayer():GetFormationList();

    local rightImage = UI_GetUIImageView(widget, 4484301)
    --战斗力
    UI_GetUILabelBMFont(rightImage, 4539340):setText(GetLocalPlayer():GetInt(EPlayer_FightValue))
    --是否可以上阵
    local isAddHero = false
    --上阵人数
    g_num=0
    --被选中用于拖动的英雄的objID
    local HeroObjID = -1
    --被选中英雄的原本位置
    local HeroLocation = -1
    --获取上阵英雄信息或者增加新的英雄
    local function GetHeroInfoOrAddHero(sender,eventType)
        local image = tolua.cast(sender,"ImageView")
        if eventType == TOUCH_EVENT_BEGAN then
            if image:getChildren():count() >0 then
                local child = image:getChildren():objectAtIndex(0) 
                local heroEntity = GetLocalPlayer():GetHeroByObjId(child:getTag());
                --显示被选中的英雄数据
                Formation_ShowInfo(widget,child:getTag())
                liftImage(true)
                --移动

                local img = GetArmature(heroEntity:GetHeroData().m_animName);
				img:setScale(1.1*MainScene:GetInstance().m_minScale);

				local uiLayer = tolua.cast(widget:getParent():getParent(), "TouchGroup")
				uiLayer:SetDragNode(img);	

				HeroObjID = child:getTag();
            end
            HeroLocation = tolua.cast(sender,"ImageView"):getTag()-10
        elseif eventType == TOUCH_EVENT_ENDED then
            if HeroObjID ~= -1 then--如果是英雄被拖动状态
                if tolua.cast(sender,"ImageView"):getTag()-10 ~= HeroLocation then--如果发生移动
                    local heroEntity = GetLocalPlayer():GetHeroByObjId(HeroObjID);
				    if (heroEntity)then
                        Formation_SendMessage(HeroObjID,tolua.cast(sender,"ImageView"):getTag()-10)
				    end
                end       
                g_Formation_LocationHero = tolua.cast(sender,"ImageView"):getTag()-10
            else--如果是点击状态
                if tolua.cast(sender,"ImageView"):getTag()-10 == HeroLocation then--如果没有发生移动
                    if isAddHero then--如果上阵人数未满
                        Formation_HeroList(tolua.cast(sender,"ImageView"):getTag()-10)
                    end
                end
            end
            HeroObjID = -1
        end       
    end

	for i=0, formationList:size()-1 do
		if (i+1)%3 ~= 0 then
            local location = UI_GetUIImageView(rightImage, 10+i)
            location:addTouchEventListener(GetHeroInfoOrAddHero)
            location:removeAllChildren()

			local heroEntity = GetLocalPlayer():GetHeroByObjId(formationList[i]);
			if heroEntity ~= nil then
                local data = heroEntity:GetHeroData();
			    print("data.m_animName===="..data.m_animName)
				Formation_AddAnimation(location,data.m_animName,formationList[i])

                g_num = g_num+1
                --默认显示第一个英雄的信息
                if g_num == 1 and g_showFirstHero==true then
                    Formation_ShowInfo(widget,formationList[i])
                    liftImage(true)
                    g_showFirstHero = false
                    --默认的更换位置未默认显示英雄的位置
                    g_Formation_LocationHero = i
                end
                print("i===="..i)
                local layout = UI_GetUILayout(rightImage, 4550155)
                UI_GetUILabel(layout, (100+i)):setText("")
           else--如果此位置没有英雄，则显示“点击上阵”
                local layout = UI_GetUILayout(rightImage, 4550155)
                UI_GetUILabel(layout, (100+i)):setText(FormatString("AddHero"))
			end
		end
	end
    --上阵人数
    local str = g_num.."/"..GetLocalPlayer():GetInt(EPlayer_FormationLimit)
    --是否可以上阵
    isAddHero = (g_num~=GetLocalPlayer():GetInt(EPlayer_FormationLimit))
    --是否显示上阵提示
    UI_GetUILayout(rightImage, 4550155):setVisible(g_num<GetLocalPlayer():GetInt(EPlayer_FormationLimit))
    UI_GetUILabel(rightImage, 4539336):setText(str)
end
--显示被选中的角色的信息
function Formation_ShowInfo(widget,objID)
    local heroList = GetLocalPlayer():GetHeroList();
	local formationList = GetLocalPlayer():GetFormationList();

    local heroEntity = nil
    for i=0,formationList:size()-1,1 do
        if formationList[i] == objID then
            heroEntity = GetLocalPlayer():GetHeroByObjId(formationList[i]);
            break;
        end
    end
    if heroEntity ~= nil then
        local HeroData = heroEntity:GetHeroData()
        local leftImage = UI_GetUIImageView(widget, 4525686)

        --凡，仙......
        local heroQuality = GetGameData(DataFileQualityProp, heroEntity:GetInt(EHero_Quality), "stQualityPropData")
        print("heroEntity:GetInt(EHero_Quality)============================"..heroEntity:GetInt(EHero_Quality))
        UI_GetUIImageView(leftImage, 4539385):loadTexture(GodAnimalSystem_getPath("Step",heroQuality.m_color-1))
        --+1/2/3/4......
        local qualityNum = heroQuality.m_Pingjie
        --名字----------------------
        local Name = UI_GetUILabel(leftImage, 4542292)

        local name = HeroData.m_name
        if qualityNum>0 then
            name = name.."+"..qualityNum
        end

        if( Name == nil )then
            print("not find nameLabel")
        else

            Name:setText(name)
            Name:setColor(UI_GetColor(heroEntity:GetInt(EHero_Quality)))
        end
        --动画
        Formation_AddAnimation(UI_GetUILayout(leftImage, 4542328),HeroData.m_animName,objID)
        ---------------------------
        --等级
        UI_GetUILabel(leftImage, 4542311):setText(heroEntity:GetInt(EHero_Lvl))
        --战力
        UI_GetUILabel(leftImage, 4542317):setText(heroEntity:GetInt(EHero_FightValue))
        --生命
        UI_GetUILabel(leftImage, 4542318):setText(heroEntity:GetInt(EHero_HP))
        --攻击
        UI_GetUILabel(leftImage, 4542319):setText(heroEntity:GetInt(EHero_Att))
        --绝技说明
        local skill = GetGameData(DataFileSkill, HeroData.m_skillId, "stSkillData")
        UI_GetUILabel(leftImage, 4542298):setText(skill.m_name..":".. skill.m_desc)
        --类型
        UI_GetUIImageView(leftImage, 4542332):loadTexture(GodAnimalSystem_getPath("heroType",HeroData.m_heroType))
        --更换----------------
        local function change(sender,eventType)
            if eventType == TOUCH_EVENT_ENDED then      
                Formation_HeroList(objID)
            end
        end
        UI_GetUIButton(leftImage, 4539377):addTouchEventListener(change)
        ----------------------
        --下阵----------------
        local function remove(sender,eventType)
            if eventType == TOUCH_EVENT_ENDED then
                Formation_SendMessage(objID,-1)
                print(objID)
                leftImage:setVisible(false)
            end
        end
        UI_GetUIButton(leftImage, 4539379):addTouchEventListener(remove)    
    end
end
--
function Formation_createPlayerHeroTempLayout(id,curlayout)
      local heroObjectId = id
      local heroEntity = GetEntityById(heroObjectId,"Hero")
      Log("EHero_HeroId =="..heroEntity:GetUint(EHero_HeroId))
      local data = GetGameData(DataFileHeroBorn,heroEntity:GetUint(EHero_HeroId),"stHeroBornData")
      --icon 
      UI_GetHeroIncoByClone(data.m_icon,curlayout,1)
      --frame
      UI_IconFrameByPingjie(curlayout,2,heroEntity:GetUint(EHero_Quality))
      --pingjie
      UI_GetHeroLevelStepIcon(curlayout,3,heroEntity:GetUint(EHero_Quality))
      --name
      local namelayout = UI_GetUILayout(curlayout, 4)
      namelayout:removeAllChildren()
      local namelab = GetHeroNameByLabel(data.m_name,heroEntity:GetUint(EHero_Quality),24)
      namelab:setAnchorPoint(ccp(0.0,0.0))
      namelayout:addChild(namelab)
      --leixing
      UI_GetHeroTypeIconByTag(curlayout,11,data.m_heroType)
      --lvl
      UI_SetLabelText(curlayout, 12, heroEntity:GetUint(EHero_Lvl))
      --star 
      local startagTab = {1,2,3,4,5}
      setHeroStarImageByClone(UI_GetUILayout(curlayout, 15),startagTab,heroEntity:GetUint(EHero_LvlStep) + 1)
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
            isRed = true 
         end 
      end 
      if isRed == true then 
         UI_GetUIImageView(curlayout, 14):setVisible(true)
      else 
         UI_GetUIImageView(curlayout, 14):setVisible(false)
      end 
   end 
--英雄列表
function Formation_HeroList(objID)
    local widget = UI_CreateBaseWidgetByFileName("HeroListSecond.json")
    UI_GetUIButton(widget, 1):addTouchEventListener(UI_ClickCloseCurBaseWidgetWithNoAction)
    
    local HeroList = GetLocalPlayer():GetHeroList(true);

    local listView = UI_GetUIListView(UI_GetUILayout(widget, 2), 1)
    listView:setBounceEnabled(true)
    --隐掉英雄和武魂
    UI_GetUIButton(widget, 3):setVisible(false)
    UI_GetUIButton(widget, 4):setVisible(false)
    UI_GetUIButton(widget, 3):setTouchEnabled(false)
    UI_GetUIButton(widget, 4):setTouchEnabled(false)

    --初始化英雄数据--------------------------------------------------------------------------------------
    local HeroIndex = 0
    local j=0
    if objID == nil then
        objID=0
    end
    local layout = nil
    local function InitHeroData()
        local function addHero(sender,eventType)--上阵
            if eventType==TOUCH_EVENT_ENDED then
                UI_CloseCurBaseWidget(EUICloseAction_None)
                if(objID<10) then--当objID小于10的时候，他其实是按在空白位置的位置标记
                    Formation_SendMessage(tolua.cast(sender,"ImageView"):getTag(),objID)
                else
                    Formation_SendMessage(tolua.cast(sender,"ImageView"):getTag(),g_Formation_LocationHero)
                end
                g_showFirstHero = true
            end
        end
        local heroID = HeroList[HeroIndex]
        HeroIndex = HeroIndex+1
        local heroEntity = GetEntityById(heroID,"Hero")
        local newHeroBox = UI_GetUILayout(widget, 3):clone()
			
        if( j==0 )then
            layout =Layout:create()
            layout:setSize(CCSizeMake(690,120));
            listView:pushBackCustomItem(layout)
			
            j=2
        end
        if objID == heroEntity:GetInt(EHero_ID) then
            print("equal")
            if (HeroIndex<HeroList:size()) then
				InitHeroData();
            end
        else
            j = j-1
            newHeroBox:setVisible(true)
            newHeroBox:setPosition(ccp( 345*((1-j)%2),0 ))
            layout:addChild(newHeroBox)
            --设置数据
            Formation_createPlayerHeroTempLayout(heroID,newHeroBox)
            newHeroBox:setTouchEnabled(true)
			newHeroBox:addTouchEventListener(addHero)
            newHeroBox:setTag(heroEntity:GetInt(EHero_ID))
			newHeroBox:setName("hero_"..HeroIndex)
            --英雄属性窗口的动作--------------
            local actionArry = CCArray:create()
            --如果添加了两个英雄
            if j==0 then
                local delayTime = CCDelayTime:create(0.1)
                actionArry:addObject(delayTime)
            else
                local delayTime = CCDelayTime:create(0.05)
                actionArry:addObject(delayTime)
            end
            --如果后有没有添加完的英雄
            if (HeroIndex<HeroList:size()) then
                local callback = CCCallFuncN:create(InitHeroData)
                actionArry:addObject(callback)
            end
            local seqaction = CCSequence:create(actionArry)
            newHeroBox:runAction(seqaction)
			
            -----------------------------------
            --容器的动作
            local actionArry_layout = CCArray:create()
            if j==1 or HeroIndex>HeroList:size() then
                local actionFadeIn = CCFadeIn:create(0.2)
                actionArry_layout:addObject(actionFadeIn)
                local seqaction_layout = CCSequence:create(actionArry_layout)
                layout:runAction(seqaction_layout)
            end
        end       
    end
    ----------------------------------------------------------------------------------------------------
    --添加可出战的英雄
    if (HeroList:size()-g_num ==0 ) then--无额外英雄
        local label = Label:create()
        label:setText(FormatString("NotMoreHero"))
        label:setFontSize(24)
        label:setPosition(ccp(0,0))
        UI_GetUIImageView(UI_GetUILayout(widget, 2), 4612416):addChild(label,10,10)
    else--有额外的英雄    
        InitHeroData()
    end
end

function liftImage(isVisible)
    local widget = UI_GetBaseWidgetByName("Formation")

    local leftImage = UI_GetUIImageView(widget, 4525686)

    local change = UI_GetUIButton(leftImage, 4539377)

    local remove = UI_GetUIButton(leftImage, 4539379)

    leftImage:setVisible(isVisible)
    change:setTouchEnabled(isVisible)
    remove:setTouchEnabled(isVisible)
end