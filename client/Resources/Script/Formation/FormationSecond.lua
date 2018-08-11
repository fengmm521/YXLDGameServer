--网络消息：发送上阵信息
function Formation_SendMessage()
	if (g_FormationOrNineSky == "Formation")then
		local tab = GameServer_pb.Cmd_Cs_FormationChg();
        for i=1,#g_InTeamId,1 do
            if g_InTeamId[i] ~=0 then
		        
                local tmp = GameServer_pb.FormationItemData();
                tmp.dwObjectID = g_InTeamId[i]
                tmp.iPos = i-1
                table.insert(tab.szFormationList, tmp);
            end
        end	
		Packet_Full(GameServer_pb.CMD_FORMATION_CHG, tab);
		ShowWaiting();

	elseif (g_FormationOrNineSky == "NineSky")then
		local tab = GameServer_pb.CMD_DREAMLAND_FORMATION_CHG_CS();
        for i=1,#g_InTeamId,1 do
            if g_InTeamId[i] ~=0 then
		        
                local tmp = GameServer_pb.DreamLandFormation();
                tmp.dwObjcetId = g_InTeamId[i]
                tmp.iPos = i-1
                table.insert(tab.szFormationList, tmp);

            end
        end	
        Packet_Full(GameServer_pb.CMD_DREAMLAND_FORMATION_CHG, tab);
	    ShowWaiting();
		
	end
end
--出战角色的ID
g_InTeamId={}
--保存最开始的队伍
g_OldInTeamId={}
function Formation_Create()
    g_FormationOrNineSky = "Formation"
	local widget = UI_CreateBaseWidgetByFileName("FormationSecond.json");
	if (widget == nil)then
		Log("Formation_Create error");
		return;
	end

    UI_GetUIImageView(widget, 4724164):setTouchEnabled(false)

    local function close(send,eventType)
        --在英雄位置移动期间，退出无法响应
        if eventType == TOUCH_EVENT_ENDED and g_Formation_heroLocation == -1 then
            for i=1 , #g_InTeamId ,1 do
                print(g_InTeamId[i].."-------------"..g_OldInTeamId[i])
                if g_InTeamId[i] ~= g_OldInTeamId[i] then
                    --如果队伍被修改过 
                    Formation_Exit()
                    return
                end
            end
            Formation_Close(sender, eventType)
        else
            g_Formation_heroLocation = -1
            g_Formation_isMoved= false
        end
    end

	UI_GetUIButton(widget, 4695721):addTouchEventListener(close);
    
    local formationList = GetLocalPlayer():GetFormationList();
	--显示当前队伍
	Formation_Show(widget,formationList,0,formationList:size()-1)
    --初始化武将列表
    Formation_HeroList()
    --添加主公技能
    local rightImage = UI_GetUIImageView(widget, 4484301)
    for i=1, 3,1 do  
        Formation_addSkill(widget,i)
    end
    --保存阵型
    local function saveTeam(sender,eventType)
        --在英雄位置移动期间，保存无法响应
        if eventType == TOUCH_EVENT_ENDED and g_Formation_heroLocation == -1 then
            Formation_SendMessage()
			
			if g_isGuide then
				Guide_GoNext();
			end
			
        else
            g_Formation_heroLocation = -1
            g_Formation_isMoved= false
        end
    end
    UI_GetUIButton(widget, 4691907):addTouchEventListener(saveTeam)

    Formation_RefreshData()

    UI_GetUILabel(widget, 4751562):enableStroke()
end
--添加主公技能
function Formation_addSkill(widget,id)
    --添加技能说明
    local function showInfo(send,eventType)
        --在英雄位置移动期间，技能无法响应
        if eventType == TOUCH_EVENT_ENDED and g_Formation_heroLocation == -1 then
            SkillTipForActor(tolua.cast(send,"ImageView"):getTag())
        else
            g_Formation_heroLocation = -1
            g_Formation_isMoved= false
        end
    end
    local SkillData = GetGameData(DataFileActorSkill, id, "stActorSkillData")
    local skillIcon = UI_ItemIconFrame(UI_GetSkillIcon(SkillData.m_icon), 7)

    UI_GetUIImageView(widget, 11000+id):removeAllChildren()

    UI_GetUIImageView(widget, 11000+id):addChild(skillIcon,10001,10001)
    
    skillIcon:setTag( id )
    skillIcon:setTouchEnabled(true)
    skillIcon:addTouchEventListener(showInfo)

    local label = UI_GetUILabel(widget, 12000+id)
    local playerLvl = GetLocalPlayer():GetInt(EPlayer_Lvl)

    label:enableStroke()
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

--显示当前队伍
function Formation_Show(widget,list,startIndex,endIndex)

    local function removeHeroFromTeam(sender , eventType)
        if eventType == TOUCH_EVENT_ENDED then
            Formation_removeHeroFromTeam(tolua.cast(sender,"ImageView"):getTag())
        end
    end
    --上阵人数
    g_num=0
    local index = 1
    for i=startIndex, endIndex do
        g_InTeamId[index] = 0
        g_OldInTeamId[index] = 0
		if index%3 ~= 0 then          
            Formation_addOneHero(index-1,nil,nil)
            local location = UI_GetUIImageView(widget, 10000+i)

			local heroEntity = GetLocalPlayer():GetHeroByObjId(list[i]);
			if heroEntity ~= nil then
                local data = heroEntity:GetHeroData();

                g_num = g_num+1

                local icon  = Formation_GetHeroIcon(data,heroEntity)

                Formation_addOneHero(index-1,icon,heroEntity)
                g_OldInTeamId[index] = heroEntity:GetInt(EHero_ID)
            end 
        end
        index = index+1
	end
end
--将英雄从队伍中移除
function Formation_removeHeroFromTeam(id)
	
	if g_isGuide and g_curGuideId == 3 then
		return;
	end
	
	print(id)
    local widget = UI_GetBaseWidgetByName("FormationSecond")
    local location = UI_GetUIImageView(widget, 10000+id-2000)
    location:removeChildByTag(id-2000+1340)
    g_InTeamId[id-2000+1] = 0
    Formation_HeroListRefresh()
    g_num = g_num-1
    Formation_RefreshData()
end
--显示英雄列表
function Formation_HeroList()
    local function addHeroToTeam(sender , eventType)
        --在英雄位置移动期间，英雄列表无法响应
        if eventType == TOUCH_EVENT_ENDED and g_Formation_heroLocation == -1 then
            local tag = tolua.cast(sender,"ImageView"):getTag()-1900
            print(tag)
            local HeroList = GetLocalPlayer():GetHeroList(true);
            local heroID = HeroList[g_Formation_HeroListIndex[tag]]

            local location = Formation_IsInTeam(heroID)
            if location ~= false then
                
                Formation_removeHeroFromTeam(location+2000)
            else
                Formation_AddHeroToTeam(tag)
            end
			
			if g_isGuide then
				Guide_GoNext();
			end
			
        else
            g_Formation_heroLocation = -1
            g_Formation_isMoved= false
        end
		
    end
    local widget = UI_GetBaseWidgetByName("FormationSecond")
    local listView = UI_GetUIListView(widget, 4691744)
    listView:removeAllItems();

    local HeroList = GetLocalPlayer():GetHeroList(true);
    --排序后的英雄所在的索引位置
    g_Formation_HeroListIndex = Formation_SortHeroList(HeroList)
    --所有英雄在英雄列表中的图标
    g_Formation_HeroIcon={}

    local line = nil
    for i=1,HeroList:size(),1 do
        if (i-1)%3 == 0 then
            line = UI_GetUILayout(widget, 4695724):clone()
            listView:pushBackCustomItem(line)
        end
        local oneHero = UI_GetUILayout(widget, 4691749):clone()

        local heroID = HeroList[g_Formation_HeroListIndex[i]]

        UI_GetUIImageView(oneHero, 4691753):setVisible(false)
        if Formation_IsInTeam(heroID) ~= false then
            UI_GetUIImageView(oneHero, 4691757):setVisible(true)
        else
            UI_GetUIImageView(oneHero, 4691757):setVisible(false)
        end

        local heroEntity = GetEntityById(heroID,"Hero")
        local data = heroEntity:GetHeroData();
        --九天幻境要显示血量
        local icon = Formation_GetHeroIcon(data,heroEntity)
        if g_FormationOrNineSky == "NineSky" 
            and NineSky_IsHeroLoseHP(heroEntity:GetInt(EHero_ID)) == 0 then
                UI_GetUIImageView(oneHero, 4691753):setVisible(true)
                UI_GetUIImageView(oneHero, 4691753):setTouchEnabled(true)
        end
        UI_GetUIImageView(oneHero, 4691751):addChild(icon)
        line:addChild(oneHero)
        oneHero:setPosition(ccp(line:getSize().width/3*((i-1)%3),0))
        
        icon:setTouchEnabled(true)
        icon:setTag(1900+i)
        icon:addTouchEventListener(addHeroToTeam)
		icon:setName("FormationHero_"..(i-1) );
        
        g_Formation_HeroIcon[i] = oneHero
    end
end
--添加英雄到队伍
function Formation_AddHeroToTeam(id)
    if g_num+1>GetLocalPlayer():GetInt(EPlayer_FormationLimit) then
            createPromptBoxlayout(FormatString("NoMoreLoaction"))
        return
    end
    local HeroList = GetLocalPlayer():GetHeroList(true);
    local heroID = HeroList[g_Formation_HeroListIndex[id]]
    local heroEntity = GetEntityById(heroID,"Hero")
    local data = heroEntity:GetHeroData();
    --九天幻境要显示血量
    local icon = Formation_GetHeroIcon(data,heroEntity)
    for i=1,9,1 do
        --找到可添加的位置
        if g_InTeamId[i] == 0 and i%3 ~=0 then
            Formation_addOneHero(i-1,icon,heroEntity)

            Formation_HeroListRefresh();
            g_num = g_num+1
            Formation_RefreshData()
            return 
        end 
    end
end
--是否进行英雄移动操作
g_Formation_isMoved = false
--被移动英雄的起始位置
g_Formation_heroLocation = -1
--将单个英雄添加到指定位置
function Formation_addOneHero(i,icon,heroEntity)
    local widget = UI_GetBaseWidgetByName("FormationSecond")

    local moveIcon = nil  
    local location = UI_GetUIImageView(widget, 10000+i)

    local localX,localY
    local function removeHeroFromTeam(sender , eventType)
        local tag = tolua.cast(sender,"ImageView"):getTag()
        if eventType == TOUCH_EVENT_BEGAN then
            if location:getChildByTag(1340+i) then
                local data = heroEntity:GetHeroData();
                
                moveIcon = Formation_GetHeroIcon(data,heroEntity)

                local uiLayer = tolua.cast(widget:getParent():getParent(), "TouchGroup")
				uiLayer:SetDragNode(moveIcon);	

                g_Formation_heroLocation = tag-2000

                localX = 0 
                localY = 0
                --localX = moveIcon:getWorldPosition().x
                --localY = moveIcon:getWorldPosition().y
                --print("localX == "..localX )
                --print("localY == "..localY )
                --print("============================================== ")
            end
        elseif eventType == TOUCH_EVENT_MOVED then
            
            if g_Formation_isMoved == false then
                if localX == 0 and localY==0 then
                    localX = moveIcon:getWorldPosition().x
                    localY = moveIcon:getWorldPosition().y   
                    
                end

                local tempX = moveIcon:getWorldPosition().x
                local tempY = moveIcon:getWorldPosition().y

                print("localX - tempX == "..localX - tempX )
                print("localY - tempY == "..localY - tempY )
                print("============================================== ")

                if (localX - tempX > 5 or localX - tempX < -5 )
                    or (localY - tempY > 5 or localY - tempY < -5 ) then
                        g_Formation_isMoved = true
                        print("ismove-----------------------------------------------------")
                end
            end
        elseif eventType == TOUCH_EVENT_CANCELED then  
           
        elseif eventType == TOUCH_EVENT_ENDED then
            --如果没有进行移动操作，则表示是下阵操作
            if location:getChildByTag(1340+i) and g_Formation_isMoved == false then
                Formation_removeHeroFromTeam(tag)
            elseif g_Formation_heroLocation ~= -1 then
                --如果起始位置和借宿位置相同，则不进行移动
                if g_Formation_heroLocation == tag-2000 then 
                else       
                
                --移动英雄位置
                    Formation_ChangeHeroLocation(g_Formation_heroLocation,tag-2000)
					
					if g_isGuide and g_curGuideId == 3 then
						Guide_GoNext();
					end
					
                end
            end
            g_Formation_isMoved = false
            g_Formation_heroLocation = -1
        end
    end
    print("i=============================="..i)
    if location:getChildByTag(1340+i) then
        location:removeChildByTag(1340+i)
    end
    if icon ~= nil then
        
        location:addChild(icon)
        icon:setPosition(ccp(44,44))
        icon:setTag(1340+i)
    end
    UI_GetUIImageView(location, 2000+i):addTouchEventListener(removeHeroFromTeam)
    if heroEntity ~= nil then
        g_InTeamId[i+1] = heroEntity:GetInt(EHero_ID)
    else
        g_InTeamId[i+1] = 0
    end
end
--交换英雄位置
function Formation_ChangeHeroLocation(oldLocation,newLocation)
    local temp1 = g_InTeamId[oldLocation+1]
    local temp2 = g_InTeamId[newLocation+1]
    print("temp1==========================="..temp1)
    print("temp2==========================="..temp2)
    local HeroList = GetLocalPlayer():GetHeroList(true);
    --交换头像
    local heroEntity = GetLocalPlayer():GetHeroByObjId(temp1)
    local data = heroEntity:GetHeroData();
    local icon = Formation_GetHeroIcon(data,heroEntity)
    Formation_addOneHero(newLocation,icon,heroEntity)
    if temp2 ~= 0 then
        local heroEntity = GetLocalPlayer():GetHeroByObjId(temp2)
        local data = heroEntity:GetHeroData();
        local icon = Formation_GetHeroIcon(data,heroEntity)
        Formation_addOneHero(oldLocation,icon,heroEntity)
    else
        Formation_addOneHero(oldLocation,nil,nil)
    end
end
--刷新英雄的上阵状态
function Formation_HeroListRefresh()    
    local HeroList = GetLocalPlayer():GetHeroList(true);
    for i=1 , #g_Formation_HeroListIndex,1 do
        if Formation_IsInTeam(HeroList[g_Formation_HeroListIndex[i]]) then
            UI_GetUIImageView(g_Formation_HeroIcon[i], 4691757):setVisible(true)
        else
            UI_GetUIImageView(g_Formation_HeroIcon[i], 4691757):setVisible(false)
        end
    end
end
--刷新战斗力和上阵人数
function Formation_RefreshData()
    local widget = UI_GetBaseWidgetByName("FormationSecond")
    --九天幻境不显示出战人数
    if g_FormationOrNineSky ~= "NineSky" then
        --出战人数
        local aa = FormatString("FormationNumber",g_num, GetLocalPlayer():GetInt(EPlayer_FormationLimit))
        UI_GetUILabel(widget, 4691904):setText(aa)
    end
    --战斗力
    local fightValue = 0
    for i=1 , #g_InTeamId , 1 do
        if g_InTeamId[i] ~= 0 then
            local heroEntity = GetLocalPlayer():GetHeroByObjId(g_InTeamId[i])
            fightValue = fightValue + heroEntity:GetInt(EHero_FightValue)
        end
    end
    UI_GetUILabel(widget, 4691776):setText(fightValue)
    UI_GetUILabel(widget, 4691776):enableStroke()
end
--为英雄列表排序
function Formation_SortHeroList(herolist)
    local heroList={}
    local heroListTemp={}
    --拷贝数据
    for i=1,herolist:size(),1 do 
        heroListTemp[i] = i
    end
    --排序
    for i=1 ,#heroListTemp,1 do
        local index = 0

        repeat
            index = index +1
        until heroListTemp[index] ~= nil

        local heroEntity = GetEntityById(herolist[index-1],"Hero")

        for j = index+1,#heroListTemp,1 do
            if heroListTemp[j] ~= nil then
                --等级优先度最高
                local heroEntityNext = GetEntityById(herolist[j-1],"Hero")

                if heroEntityNext:GetInt(EHero_Lvl) >  heroEntity:GetInt(EHero_Lvl) then
                    heroEntity = heroEntityNext
                    index=j
                elseif heroEntityNext:GetInt(EHero_Lvl) ==  heroEntity:GetInt(EHero_Lvl)
                    and heroEntityNext:GetInt(EHero_Quality) >  heroEntity:GetInt(EHero_Quality) then
                    --品阶优先度第二
                        heroEntity = heroEntityNext
                        index=j
                elseif heroEntityNext:GetInt(EHero_Lvl) ==  heroEntity:GetInt(EHero_Lvl)
                    and heroEntityNext:GetInt(EHero_Quality) ==  heroEntity:GetInt(EHero_Quality)
                    and heroEntityNext:GetInt(EHero_LvlStep) >  heroEntity:GetInt(EHero_LvlStep) then
                    --星级优先度第三
                        heroEntity = heroEntityNext
                        index=j
                elseif heroEntityNext:GetInt(EHero_Lvl) ==  heroEntity:GetInt(EHero_Lvl)
                    and heroEntityNext:GetInt(EHero_LvlStep) ==  heroEntity:GetInt(EHero_LvlStep)
                    and heroEntityNext:GetInt(EHero_Quality) ==  heroEntity:GetInt(EHero_Quality)
                    and heroEntityNext:GetHeroData().m_ID < heroEntity:GetHeroData().m_ID  then
                    --ID优先度最低
                        heroEntity = heroEntityNext
                        index=j
                else

                end
            end
        end
        heroList[i] = index-1
        heroListTemp[index] = nil
    end
    return heroList
end
--英雄是否上阵
function Formation_IsInTeam(objectID)
    local formationList = GetLocalPlayer():GetFormationList();
   
    for i=1, #g_InTeamId,1 do
        local temp = GetEntityById(objectID,"Hero")
        if temp:GetInt(EHero_ID) == g_InTeamId[i] then
            return i-1
        end
    end
    return false
end
--退出
function Formation_Exit()
    local tab = {}
    tab.info = FormatString("Exit")
    tab.msgType = EMessageType_LeftRight
    tab.leftFun = Formation_Close
    Messagebox_Create(tab)
end
function Formation_Close(sender,eventType)
    local widget= UI_GetBaseWidgetByName("FormationSecond") 
    UI_ClickCloseCurBaseWidgetWithNoAction(UI_GetUIButton(widget, 4691907),TOUCH_EVENT_ENDED)
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
function FormationSecond_Refresh(widget)
    EndWaiting()
    UI_ClickCloseCurBaseWidgetWithNoAction(UI_GetUIButton(widget, 4691907),TOUCH_EVENT_ENDED)
end

function Formation_GetHeroIcon(data,heroEntity)
    --九天幻境要显示血量
    local icon = nil
    if g_FormationOrNineSky == "NineSky" then
        icon = UI_IconFrame(UI_GetHeroIcon(data.m_ID, heroEntity:GetInt(EHero_Lvl),nil,NineSky_IsHeroLoseHP(heroEntity:GetInt(EHero_ID))),
                heroEntity:GetInt(EHero_LvlStep), heroEntity:GetInt(EHero_Quality));
    else 
        icon = UI_IconFrame(UI_GetHeroIcon(data.m_ID, heroEntity:GetInt(EHero_Lvl)), heroEntity:GetInt(EHero_LvlStep), heroEntity:GetInt(EHero_Quality));
    end
    return icon
end

-----------------------------九天幻境使用--------------------------------------
--显示当前队伍
function NineSkyFormation_Show(widget,Objects)
    local list={}
    for i=1,9,1 do
        list[i] = 0
    end
    for i=1,#Objects,1 do
        list[Objects[i].iPos+1] = Objects[i].dwObjcetId
    end
    Formation_Show(widget,list,1,#list)
end