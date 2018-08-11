----------------------发送数据--------------------------------------
--获取指定仙宠的详细数据
function GodAnimalSystem_GetAnimalData(EGodAnimal_ObjId)
    local tab = GameServer_pb.Cmd_Cs_QueryGodAnimalDetai();
	tab.dwGodAnimalObjectID = EGodAnimal_ObjId
	Packet_Full(GameServer_pb.CMD_GODANIMAL_DETAIL, tab);

    ShowWaiting()
end
--获取指定未获得的仙兽的详细数据
function GodAnimalSystem_GetAnimalDataNG(id)
    local tab = GameServer_pb.CMD_GODANIMAL_DETAIL_BYID_CS()
    tab.iGodAnimalID = id
    Packet_Full(GameServer_pb.CMD_GODANIMAL_DETAIL_BYID,tab)

    ShowWaiting()
end
--召唤
function GodAnimalSystem_CallAnimal(id)
   local tab = GameServer_pb.CMD_GODANIMAL_ANIMALSOUL_ACTIVE_CS()
   tab.iAnimalSoulID = id
   g_GodAnimalSystem_callID = id
   Packet_Full(GameServer_pb.CMD_GODANIMAL_ANIMALSOUL_ACTIVE,tab)
end
--传承
function GodAnimalSystem_inheritance(acceptPreDwObjectID,passerDwObjectID)
print("passerDwObjectID== "..passerDwObjectID)
print("acceptPreDwObjectID== "..acceptPreDwObjectID)
    local tab = GameServer_pb.Cmd_Cs_ExeGodAnimalInherit()
    tab.dwPasserObjectID = passerDwObjectID
    tab.dwAccepObjectID = acceptPreDwObjectID
    Packet_Full( GameServer_pb.CMD_GODANIMAL_EXEINHERIT, tab)
    ShowWaiting()
end
--查询传承之后的数据
function GodAnimalSystem_inheritanceData(id,id2) 
    local tab = GameServer_pb.Cmd_Cs_QueryGodAnimalInherit()
    tab.dwAccepObjectID = id
    if(id2 ~= -1)then
        tab.dwPasserObjectID = id2
    end
    Packet_Full( GameServer_pb.CMD_GODANIMAL_QUERYINHERIT, tab )
end
--培养
function GodAnimalSystem_TrainMsg(id)
    local tab = GameServer_pb.Cmd_Cs_GodAnimalTrain()
  	tab.dwObjectID = id
  	Packet_Full(GameServer_pb.CMD_GODANIMAL_TRAIN, tab)
  	ShowWaiting()
end
--升阶
function GodAnimalSystem_GetAnimalStepLevelUp(EGodAnimal_ObjId)
    local tab = GameServer_pb.Cmd_Cs_ExeLevelStepUp()
	tab.dwObjectID = EGodAnimal_ObjId
	Packet_Full(GameServer_pb.CMD_GODANIMAL_EXELEVELSTEPUP, tab)
	ShowWaiting()
end
--升阶查询
function GodAnimalSystem_GetAnimalStepLevelUpInfo(EGodAnimal_ObjId)
    local tab = GameServer_pb.Cmd_Cs_QueryGodAnimalLevelStep()
    tab.dwObjectID = EGodAnimal_ObjId 
    Packet_Full(GameServer_pb.CMD_GODANIMAL_QUERYLEVELSTEP, tab)
    ShowWaiting()  
end
--出战
function GodAnimalSystem_GetAnimalActive(EGodAnimal_ObjId)
    local tab = GameServer_pb.Cmd_Cs_GoldAnimalActive()
	tab.dwObjectID = EGodAnimal_ObjId
	Packet_Full(GameServer_pb.CMD_GODANIMAL_ACTIVE, tab)
	ShowWaiting() 
end
--------------------------------------------------------------------------
--获得等阶
function GodAnimalSystem_getStep(steplevel)
    if steplevel<3 then
        return 1
    elseif steplevel<7 then
        return 2
    elseif steplevel<13 then
        return 3
    elseif steplevel<20 then
        return 4
    else
        return 5
    end
end
--获得等阶的增加“+”
function GodAnimalSystem_getStepNum(steplevel)
    if steplevel<3 then
        return steplevel
    elseif steplevel<7 then
        return steplevel-3
    elseif steplevel<13 then
        return steplevel-7
    elseif steplevel<20 then
        return steplevel-13
    else
        return 0
    end
end
function GodAnimalSystem_SetSelected(tag)
    local widget = UI_GetBaseWidgetByName("GodAnimalSystem")
    widgetLeft = UI_GetUIImageView(widget, 4524935)
    local showData = UI_GetUIImageView(widgetLeft, 4525191)
    --传承
    UI_GetUIButton(showData, 4525100):setButtonEnabled(tag~=4525100)
    --培养
    UI_GetUIButton(showData, 4525111):setButtonEnabled(tag~=4525111)
end
--------------------------------------------------
--创建仙兽界面时游戏发送数据请求
function GodAnimalSystem_create()
    local i=0
    local animalList = GetLocalPlayer():GetGodAnimalList()
    while i<animalList:size() do
        if animalList[i]:GetBool(EGodAnimal_IsActive) then   
            --请求第一个仙宠的数据
            GodAnimalSystem_GetAnimalData(animalList[i]:GetInt(EGodAnimal_ObjId))
            --第一个仙宠的升阶数据预览
            GodAnimalSystem_GetAnimalStepLevelUpInfo(animalList[i]:GetInt(EGodAnimal_ObjId))
            return
        end
        i=i+1
    end
    --如果没有出战的神兽
    --请求第一个仙宠的数据
    GodAnimalSystem_GetAnimalData(animalList[0]:GetInt(EGodAnimal_ObjId))
    --第一个仙宠的升阶数据预览
    GodAnimalSystem_GetAnimalStepLevelUpInfo(animalList[0]:GetInt(EGodAnimal_ObjId))
end
--创建界面
function GodAniamalSystem_createWidget()
    local widget = UI_CreateBaseWidgetByFileName("UIExport/DemoLogin/GodAnimalSystem.json")
    local function closeFunck(sender,eventType)
       if eventType == TOUCH_EVENT_ENDED then 
          UI_CloseCurBaseWidget(EUICloseAction_FadeOut,0.35)
       end 
    end 
    UI_GetUIButton(widget, 4525445):addTouchEventListener(closeFunck);

    g_isRefreshListView = false
    --更新列表
    GodAnimalSystem_ShowListView(widget)
    GodAnimalSystem_refreshListView(widget)
    --将第一个仙兽设为高亮--待修改
    GodAnimalSystem_refreshListViewHL(0,1,widget)

    return widget
end
--更新仙宠列表
function GodAnimalSystem_refreshListView(widget,animalIDList)
    if animalIDList==nil then
        animalIDList = vector_int_:new_local() 
        GetLocalPlayer():sortGodAnimalWithId(animalIDList)
        print("=============")
    end

    local leftImage = UI_GetUIImageView(widget, 4524938)
    local listView = UI_GetUIListView( leftImage , 4525076)
    listView:removeAllItems()

    local isAddNoCall=false

    --更换显示的仙宠-已获得
    local function changeAnimal(sender,eventType)
        if(eventType == TOUCH_EVENT_ENDED) then
            local index = tolua.cast(sender,"ImageView"):getTag()-1400
            local animaldwID = GetLocalPlayer():GetGodAnimalByAnimalId(animalIDList[index]):GetInt(EGodAnimal_ObjId)
            GodAnimalSystem_GetAnimalData(animaldwID)
            GodAnimalSystem_GetAnimalStepLevelUpInfo(animaldwID)
            g_GodAnimalSystem_lastlevel = GetLocalPlayer():GetGodAnimalByAnimalId(animalIDList[index]):GetInt(EGodAnimal_Level)

            GodAnimalSystem_refreshListViewHL(index,animalIDList:size(),widget)
        end
    end
    --更换显示的仙宠-未获得
    local function NGchangeAnimal(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            local index = tolua.cast(sender,"ImageView"):getTag()-1400
            GodAnimalSystem_GetAnimalDataNG(animalIDList[index-1])
            GodAnimalSystem_refreshListViewHL(index,animalIDList:size(),widget)
        end
    end
    --已拥有的仙宠
    for i=0,animalIDList:size()-1,1 do

        local GodAnimal = GetLocalPlayer():GetGodAnimalByAnimalId(animalIDList[i])
        if GodAnimal then
            local GAwidget = UI_GetUIImageView(widget, 111001):clone()
            listView:pushBackCustomItem(GAwidget)
            local GodAnimalData = GodAnimal:getGodAnimalData()
            --name
            local name = GodAnimalData.m_name
        
            local step = GodAnimal:GetInt(EGodAnimal_LevelStep)
            if GodAnimalSystem_getStepNum(step) >0 then
                name = name.."+"..GodAnimalSystem_getStepNum(step)
            end
            UI_GetUILabel(GAwidget, 4525073):setText(name)
            --level
            UI_GetUILabel(GAwidget, 4524975):setText(GodAnimal:GetInt(EGodAnimal_Level))
            --凡、仙......
            local step_ = GodAnimalSystem_getStep(step)
            UI_GetUIImageView(GAwidget, 4525071):loadTexture(GodAnimalSystem_getPath("Step",step_))
            --頭像
            UI_GetUIImageView(GAwidget, 4525066):loadTexture(GodAnimalSystem_getPath("iconHero", GodAnimalData.m_ID))
            --头像外框
            UI_GetUIImageView(GAwidget, 4525821):loadTexture(GodAnimalSystem_getPath("animalHeadCommon", step_))
            --是否出战
            if( GodAnimal:GetBool(EGodAnimal_IsActive) ) then 
                UI_GetUIImageView(GAwidget, 4524976):setVisible(true)
            else
                UI_GetUIImageView(GAwidget, 4524976):setVisible(false)
            end
            --更换显示的仙宠
            GAwidget:addTouchEventListener(changeAnimal)
            GAwidget:setTag(i+1400)
        else
            if isAddNoCall == false then
                local NoCall = UI_GetUILayout(widget, 4528178):clone()
                listView:pushBackCustomItem(NoCall)
                isAddNoCall = true
            end
            local NGAwidget = UI_GetUIImageView(widget, 4525386):clone()
            listView:pushBackCustomItem(NGAwidget)   
            local NGGodAnimalData = GetGameData(DataFileGodAniaml,animalIDList[i],"stGodAnimalData")
            --name
            UI_GetUILabel(NGAwidget, 4525393):setText(NGGodAnimalData.m_name)
            --頭像
            UI_GetUIImageView(NGAwidget, 4525391):loadTexture(GodAnimalSystem_getPath("iconHero", NGGodAnimalData.m_ID))
            --灰色
            UI_GetUIImageView(NGAwidget, 4525391):addColorGray()
            --拥有碎片
            local haveNum = GetLocalPlayer():GetPlayerBag():GetItemCountByItemId(NGGodAnimalData.m_changeItemId)
            local allNum = NGGodAnimalData.m_changeNumber

            local percent = haveNum/allNum*100 > 100 and 100
            percent = percent or haveNum/allNum*100

            UI_GetUILoadingBar(NGAwidget, 4525422):setPercent(percent)
            UI_GetUILabel(NGAwidget, 4549318):setText(haveNum.."/"..allNum)
            --更换显示的仙宠
            NGAwidget:addTouchEventListener(NGchangeAnimal)
            NGAwidget:setTag((i+1)+1400)
        end
    end
end
--仙宠列表标记被选中的仙兽
function GodAnimalSystem_refreshListViewHL(chooseTag,allNum,widget)
    local leftImage = UI_GetUIImageView(widget, 4524938)
    local listView = UI_GetUIListView( leftImage , 4525076)

    local arra = listView:getItems()
    print("ListView Conut"..arra:count() )

    for i=0,allNum,1 do
        local GodAnimalWidget = listView:getItem(i)
        local image = GodAnimalWidget:getChildByTag(1122)
        if( image ) then
            if i~=chooseTag then
                image:setVisible(false)
            else
                image:setVisible(true)
            end
        end
    end 
end
--动画等待
function GodAnimalSystem_AnimEnd(armature, movementType, movementID)
  if (movementType == LOOP_COMPLETE)then
    if (movementID == "0")then
      if (Random:RandomInt(1, 100) < 15)then
        armature:getAnimation():playWithIndex(1)
      end
    elseif (movementID == "1")then
      armature:getAnimation():playWithIndex(0)
    end
  end
end
--添加动画
function GodAnimalSystem_AddAnimation(widget,animationName)
    if widget:getChildByTag(199) then
        widget:removeChildByTag(199)
    end
    local animalPic = UI_GetUIImageView(widget, 4524948)

    local layout = Layout:create();
    local godanimalanimatrue = GetArmature(animationName)
    godanimalanimatrue:setPosition(ccp(animalPic:getPositionX(),animalPic:getPositionY()))
    godanimalanimatrue:getAnimation():playWithIndex(0) 
    godanimalanimatrue:getAnimation():setMovementEventCallFunc(GodAnimalSystem_AnimEnd) 
    godanimalanimatrue:setAnchorPoint(ccp(0.42,0.42))
    layout:addNode(godanimalanimatrue)
    widget:addChild(layout,5,199)
    animalPic:setVisible(false)
end
--更新右方的公共部分数据
function GodAnimalSystem_setPublicData(widget,detail)
   --animation 
    local rightImage = UI_GetUIImageView(widget, 4524935)
    --仙宠的本地数据
    local animalLocal = GetGameData(DataFileGodAniaml,detail.iBaseID,"stGodAnimalData")
    --动画
    GodAnimalSystem_AddAnimation(widget,animalLocal.m_animName)
    -------------------------------------------------------------------
    --显示技能详情
    local function Skillinfo(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then    
            print("tag=="..tolua.cast(sender,"ImageView"):getTag())
            SkillTipForGodAnimal(tolua.cast(sender,"ImageView"):getTag())
        end 
    end
    --技能

    local ActiveSkillData = GetGameData(DataFileSkill,detail.iActiveSkillID,"stSkillData")
    local StageSkillData = GetGameData(DataFileSkill,detail.iStageSkillID,"stSkillData")
    print("ActiveSkillData.m_quality=="..ActiveSkillData.m_quality)
    local Active = UI_IconSkillFrame(UI_GetSkillIcon(ActiveSkillData.m_icon), ActiveSkillData.m_quality)
    local Stage = UI_IconSkillFrame(UI_GetSkillIcon(StageSkillData.m_icon), StageSkillData.m_quality)

    UI_GetUIImageView(rightImage, 4525092):removeAllChildren()
    UI_GetUIImageView(rightImage, 4525095):removeAllChildren()
    UI_GetUIImageView(rightImage, 4525092):addChild(Active,1,1)
    UI_GetUIImageView(rightImage, 4525095):addChild(Stage,1,1)

    Active:setTouchEnabled(true)
    Stage:setTouchEnabled(true)

    Active:setTag(detail.iActiveSkillID)
    Stage:setTag(detail.iStageSkillID)
    
    Active:addTouchEventListener(Skillinfo)
    Stage:addTouchEventListener(Skillinfo)
    --技能外框
    local step = GodAnimalSystem_getStep(detail.iLevelStep)
    local stepNum = GodAnimalSystem_getStepNum(detail.iLevelStep)


    --名字
    UI_GetUILabel(rightImage, 4525138):setText(animalLocal.m_name)
    --阶
    local stepLabel = UI_GetUILabel(rightImage, 4525198)
    stepLabel:setVisible( stepNum > 0 )
    stepLabel:setText("+"..stepNum)
    stepLabel:setColor(UI_GetColor(step+1))
    --阶的图片
     UI_GetUIImageView(rightImage, 4525088):loadTexture(GodAnimalSystem_getPath("Step",step))
end
--获取路径
function GodAnimalSystem_getPath(kind,id)
    if( kind =="Step"  )then
        return "Common/Fort_Rank_0"..id..".png"
    elseif kind == "iconHero" then
        return "Icon/HeroIcon/"..id..".png"
    elseif kind == "skill" then
        return "Icon/Skill/"..id..".png"
    elseif kind == "skillCommon" then
        return "Common/Icon_Bg_0"..(id+21)..".png"
    elseif kind == "animalHeadCommon" then
        return "Common/Icon_Bg_00"..(id+1)..".png"
    elseif kind == "heroType" then
        return "Common/Icon_001_0"..(id)..".png"
    end

end
--当宠物存在时，显示详细信息，并提供操作
function GodAnimalSystem_ShowData(widget,detail)
    widgetLeft = UI_GetUIImageView(widget, 4524935)
    --屏蔽召唤界面---------------------------------------------------------
    local callLayout = UI_GetUIButton(widgetLeft, 4525207)
    callLayout:setVisible(false)
    callLayout:setTouchEnabled(false)
    UI_GetUIButton(callLayout, 4535116):setTouchEnabled(false)
    -----------------------------------------------------------------------
    --按钮回调--------------------------------------
    --出战
    local function fight(sender,eventType)
        if( eventType == TOUCH_EVENT_ENDED ) then
            GodAnimalSystem_GetAnimalActive(detail.dwObjectID)
        end      
    end
    
     --传承
    local function inheritance(sender,eventType)
        if( eventType == TOUCH_EVENT_ENDED ) then
            GodAnimalSystem_ShowInheritance(widget)
            GodAnimalSystem_Inherit(widget, detail)
            GodAnimalSystem_SetSelected(4525100)
        end
    end
     --培养
    local function train(sender,eventType)
        if( eventType == TOUCH_EVENT_ENDED ) then
            GodAnimalSystem_ShowTrain(widget)
            GodAnimalSystem_GetAnimalData(detail.dwObjectID)
			if (g_isGuide) then
				Guide_GoNext();
			end
            GodAnimalSystem_SetSelected(4525111)
        end
    end
    ------------------------------------------------
    --设置
    local showData = UI_GetUIImageView(widgetLeft, 4525191)
    
    showData:setVisible(true)
    UI_GetUIButton(showData, 4525109):setTouchEnabled(true)
    UI_GetUIButton(showData, 4525110):setTouchEnabled(true) 
    UI_GetUIButton(showData, 4525100):setTouchEnabled(true)   
    UI_GetUIButton(showData, 4525111):setTouchEnabled(true)
    UI_GetUIButton(showData, 4525098):setTouchEnabled(true)
    --出战
    UI_GetUIButton(showData, 4525109):addTouchEventListener(fight)
    --传承
    UI_GetUIButton(showData, 4525100):addTouchEventListener(inheritance)
    --培养
    UI_GetUIButton(showData, 4525111):addTouchEventListener(train)
    --等级
    UI_GetUILabel(showData, 4525140):setText(detail.iLevel)
    --战斗力
    UI_GetUILabel(showData, 4525181):setText(detail.iFightValue)
    --生命
    UI_GetUILabel(showData, 4525182):setText(detail.szPropList[1].iValue)
    --等级
    UI_GetUILabel(showData, 4525183):setText(detail.szPropList[2].iValue)
end
--由于升阶需要额外发送消息，因此升阶的数据初始化讲和GodAnimalSystem_ShowData分开
function GodAnimalSystem_SetStepLevelUpInfo(info)
    local widget = UI_GetBaseWidgetByName("GodAnimalSystem")
    widgetLeft = UI_GetUIImageView(widget, 4524935)
    local showData = UI_GetUIImageView(widgetLeft, 4525191)

    
    --拥有魂魄数
    local haveNum = -1
    if info.bTouchMaxLv ~= true then
        haveNum = GetLocalPlayer():GetPlayerBag():GetItemCountByItemId(info.szConsumeItem[1].iItemID)
    end
    --进阶
    local function StepLevelUp()
        --if( eventType == Touch_EVENT_BEGAN ) then
            GodAnimalSystem_GetAnimalStepLevelUp(info.dwObjectID)
        --end
        --print("----")
    end
    --进阶提示框
    local function stepLevelUp(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            if info.bTouchMaxLv then
                StepLevelUp()
            elseif(haveNum>=info.szConsumeItem[1].iNeedCount) then
                local tab = {}
                tab.info = FormatString("StepUpExpense1")..info.iLevelStepSilverCost..FormatString("StepUpExpense2")
                tab.msgType = EMessageType_LeftRight
                tab.leftFun = StepLevelUp
                Messagebox_Create(tab)
            else
                createPromptBoxlayout(FormatString("NotEnoughSoulStepUp"))

            end
        end
    end
    --进阶
    UI_GetUIButton(showData, 4525110):addTouchEventListener(stepLevelUp)
    --修改升阶消耗 
    if info.bTouchMaxLv then
        UI_GetUILabel(showData, 4525144):setText(FormatString("NewHeroSystem_HeroMaxAscending"))
        UI_GetUILoadingBar(showData, 4525173):setPercent(100)
        --屏蔽获取方式
        UI_GetUIButton(showData, 4525098):setTouchEnabled(false)
        UI_GetUIButton(showData, 4525098):setVisible(false)
    else
        --设置进阶条-数值
        UI_GetUILabel(showData, 4525144):setText(haveNum.."/"..info.szConsumeItem[1].iNeedCount)
        percent = haveNum*100/info.szConsumeItem[1].iNeedCount
        local temp = percent>100 and 100
        percent = temp or percent
        --设置进阶条-进度条
        UI_GetUILoadingBar(showData, 4525173):setPercent(percent)
        --获得碎片出处
        local function GetFragment(sender,eventType)
            if( eventType == TOUCH_EVENT_ENDED ) then
                GodAnimalSystem_GetFragment(info.szConsumeItem[1].iItemID,info.szConsumeItem[1].iNeedCount)
            end
        end
        --"+"获取出处
        UI_GetUIButton(showData, 4525098):setTouchEnabled(true)
        UI_GetUIButton(showData, 4525098):setVisible(true)
        UI_GetUIButton(showData, 4525098):addTouchEventListener(GetFragment)
    end
end
--当宠物不存在时，显示召唤界面
function GodAnimalSystem_ShowDataNG(widget,detail)
    widgetRight = UI_GetUIImageView(widget, 4524935)
    --屏蔽显示详细信息界面的按钮------------------------
    local showData = UI_GetUIImageView(widgetRight, 4525191)

    showData:setVisible(false)
    UI_GetUIButton(showData, 4525109):setTouchEnabled(false)
    UI_GetUIButton(showData, 4525110):setTouchEnabled(false) 
    UI_GetUIButton(showData, 4525100):setTouchEnabled(false)   
    UI_GetUIButton(showData, 4525111):setTouchEnabled(false)
    UI_GetUIButton(showData, 4525098):setTouchEnabled(false)
    -------------------------------------------------
    --激活召唤界面------------------------------------
    local callLayout = UI_GetUIButton(widgetRight, 4525207)
    callLayout:setVisible(true)
    callLayout:setTouchEnabled(true)
    UI_GetUIButton(callLayout, 4535116):setTouchEnabled(true)
    -------------------------------------------------
    local NGGodAnimalData = GetGameData(DataFileGodAniaml,detail.iBaseID,"stGodAnimalData")
    local haveNum = GetLocalPlayer():GetPlayerBag():GetItemCountByItemId(NGGodAnimalData.m_changeItemId)
    local allNum = NGGodAnimalData.m_changeNumber
    local itemData = GetGameData(DataFileItem,NGGodAnimalData.m_changeItemId,"stItemData")
    --“确认召唤”按钮响应
    local function callAnimalDetermine(sender,eventType)
        if eventType == Touch_EVENT_BEGAN then
            GodAnimalSystem_CallAnimal(detail.iBaseID)
        end
    end
    --"召唤"按钮响应
    local function callAnimal(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            if haveNum>= allNum then
                local tab = {}
                tab.info = FormatString("CallExpense1")..NGGodAnimalData.m_cost..FormatString("CallExpense2")--待修改
                tab.msgType = EMessageType_LeftRight
                tab.leftFun = callAnimalDetermine
                Messagebox_Create(tab)
            else
                createPromptBoxlayout(FormatString("NotEnoughSoul"))
            end
        end
    end
    --灵魂图标
    UI_GetUIImageView(callLayout, 4525828):removeAllChildren()
    UI_GetUIImageView(callLayout, 4525828):addChild(UI_ItemIconFrame(UI_GetItemIcon(NGGodAnimalData.m_changeItemId, -1),itemData.m_quality) )
    UI_GetUILabel(callLayout, 4525224):setText(NGGodAnimalData.m_name)
    UI_GetUILabel(callLayout, 4525226):setText(haveNum.."/"..allNum)

    callLayout:addTouchEventListener(callAnimal)
    
    --获得碎片出处
    local function GetFragment(sender,eventType)
        if( eventType == TOUCH_EVENT_ENDED ) then
            GodAnimalSystem_GetFragment(NGGodAnimalData.m_changeItemId,allNum)
         end
    end
    UI_GetUIButton(callLayout, 4535116):addTouchEventListener(GetFragment)
end
------------------------------------------------------------------------------------
--传承
function GodAnimalSystem_Inherit(widget, detail)
    local leftImage = UI_GetUIImageView(widget, 4524938)
    local inheritance = UI_GetUIImageView(leftImage, 4525237)   
 
    --散功英雄ID
    local passerDwObjectID=-1
    --初始化预览信息
    local StepLevelUpInfo = UI_GetUIImageView(inheritance, 4525255)
    StepLevelUpInfo:setVisible(false)
    ----等级
    --UI_GetUILabel(inheritance, 4525312):setText(999)
    ----生命
    --UI_GetUILabel(inheritance, 4525347):setText(999999)
    ----攻击
    --UI_GetUILabel(inheritance, 4525348):setText(999999) 
    --将散功仙兽图标隐藏
    UI_GetUIImageView(inheritance, 4526264):setVisible(false)
    --铜币消耗
    UI_GetUILabel(inheritance, 4525354):setText(0)
    --传承方的“战”
    UI_GetUIImageView(inheritance, 4525270):setVisible(false)
    --设置散功英雄--------------------------------------------------------
    local function GodAnimalSystem_SetInheritHero(id)
        local widget = UI_GetBaseWidgetByName("GodAnimalSystem");
        local leftImage = UI_GetUIImageView(widget, 4524938)
        local inheritance = UI_GetUIImageView(leftImage, 4525237)
        local GodAnimal = GetLocalPlayer():GetGodAnimalList()[id]
        --凡、仙......
        local step =  GodAnimal:GetInt(EGodAnimal_LevelStep)
        local step_ = (step - step%10)/10+1
        --頭像
        UI_GetUIImageView(inheritance, 4526264):setVisible(true)
        UI_GetUIImageView(inheritance, 4526264):loadTexture(GodAnimalSystem_getPath("iconHero", GodAnimal:getGodAnimalData().m_ID))
        --头像外框
        UI_GetUIImageView(inheritance, 4526268):loadTexture(GodAnimalSystem_getPath("animalHeadCommon", step_))
        --保存散功ID
        passerDwObjectID=GodAnimal:GetInt(EGodAnimal_ObjId)

        --是否出战
        if( GetLocalPlayer():GetGodAnimalByDwObject(passerDwObjectID):GetBool(EGodAnimal_IsActive) ) then 
            UI_GetUIImageView(inheritance, 4525270):setVisible(true)
        else
            UI_GetUIImageView(inheritance, 4525270):setVisible(false)
        end    
        
        GodAnimalSystem_inheritanceData(detail.dwObjectID,passerDwObjectID)  
    end
    -------------------------------------------------------------------
    ----散功英雄列表---------------------------------------------------------
    local function GodAnimalSystem_InheritHero(detail)
        local widget = UI_CreateBaseWidgetByFileName("UIExport/DemoLogin/inheritanceHero.json")
        local listView = UI_GetUIListView(widget, 4526304)       
        --退出
        UI_GetUIButton(widget, 4526306):addTouchEventListener(UI_ClickCloseCurBaseWidget)
        --添加可用于散功的宠物
        local GodAnimalList = GetLocalPlayer():GetGodAnimalList()

        --选择散功英雄
        local function choosInheritHero(sender,eventType)
            if eventType == TOUCH_EVENT_ENDED then
                local id = tolua.cast(sender,"ImageView"):getTag()-1400
                GodAnimalSystem_SetInheritHero(id)
                UI_ClickCloseCurBaseWidget(sender,TOUCH_EVENT_ENDED)
            end
        end
        --传承
        local function inheritance(sender,eventType)
            if eventType == TOUCH_EVENT_ENDED then
                local id = tolua.cast(sender,"ImageView"):getTag()-1400
                GodAnimalSystem_SetInheritHero(id)
                UI_ClickCloseCurBaseWidget(sender,TOUCH_EVENT_ENDED)
            end
        end
        for i=0,GodAnimalList:size()-1,1 do
            local GodAnimalData = GodAnimalList[i]:getGodAnimalData()
            --等级要高于自己
            if GodAnimalList[i]:GetInt(EGodAnimal_Level) > detail.iLevel then
                local GAwidget = UI_GetUIImageView(widget, 4526318):clone()
                listView:pushBackCustomItem(GAwidget)

                --name
                local name = GodAnimalData.m_name
        
                local step = GodAnimalList[i]:GetInt(EGodAnimal_LevelStep)
                if step%10 >0 then
                    name = name.."+"..step%10
                end
                UI_GetUILabel(GAwidget, 4526327):setText(name)
                --level
                UI_GetUILabel(GAwidget, 4526322):setText(GodAnimalList[i]:GetInt(EGodAnimal_Level))
                --凡、仙......
                local step_ = (step - step%10)/10+1
                UI_GetUIImageView(GAwidget, 4526326):loadTexture(GodAnimalSystem_getPath("Step",step_))
                --頭像
                UI_GetUIImageView(GAwidget, 4526325):loadTexture(GodAnimalSystem_getPath("iconHero", GodAnimalData.m_ID))
                --头像外框
                UI_GetUIImageView(GAwidget, 4526324):loadTexture(GodAnimalSystem_getPath("animalHeadCommon", step_))
                --是否出战
                if( GodAnimalList[i]:GetBool(EGodAnimal_IsActive) ) then 
                    UI_GetUIImageView(GAwidget, 4526319):setVisible(true)
                else
                    UI_GetUIImageView(GAwidget, 4526319):setVisible(false)
                end  
                --更换显示的仙宠
                GAwidget:addTouchEventListener(choosInheritHero)
                GAwidget:setTag(i+1400)
            end
        end
        UI_GetUIImageView(widget, 4526318):setVisible(true)
    end
    ------------------------------------------------------------------------
    --返回
    local function cancel(sender,eventType )
        if eventType == TOUCH_EVENT_ENDED then
            GodAnimalSystem_ShowListView(widget)
            GodAnimalSystem_refreshListView(widget)
            GodAnimalSystem_SetSelected(-1)
        end
    end
    --传承
    local function determine(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            if passerDwObjectID == -1 then
                    createPromptBoxlayout(FormatString("NoInheritHero"))
                return
            end 
        end
    end
    --选择散功宠物
    local function choose(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then  
            GodAnimalSystem_InheritHero(detail)
        end
    end
    --传承
    UI_GetUIButton(inheritance, 4525372):addTouchEventListener(determine)
    --返回
    UI_GetUIButton(inheritance, 4525374):addTouchEventListener(cancel)
    --等级
    UI_GetUILabel(inheritance, 4525307):setText(detail.iLevel)
    --生命
    UI_GetUILabel(inheritance, 4525309):setText(detail.szPropList[1].iValue)
    --攻击
    UI_GetUILabel(inheritance, 4525311):setText(detail.szPropList[2].iValue)
    UI_GetHeroIcon(detail.iBaseID, count, heroType)
    --继承宠物
    UI_GetUIImageView(inheritance, 4525240):loadTexture(GodAnimalSystem_getPath("iconHero",detail.iBaseID))
    --边框
    local step_ = (detail.iLevelStep - detail.iLevelStep%10)/10+1
    UI_GetUIImageView(inheritance, 4526265):loadTexture(GodAnimalSystem_getPath("animalHeadCommon",step_))
    --重置散功英雄
    UI_GetUIImageView(inheritance, 4526264):loadTexture("UIExport/DemoLogin/Common/Icon_Bg_base.png")
    --重置散功英雄边框
    UI_GetUIImageView(inheritance, 4526268):loadTexture(GodAnimalSystem_getPath("animalHeadCommon",1))
    --是否出战
    if( GetLocalPlayer():GetGodAnimalByDwObject(detail.dwObjectID):GetBool(EGodAnimal_IsActive) ) then 
        UI_GetUIImageView(inheritance, 4525272):setVisible(true)
    else
        UI_GetUIImageView(inheritance, 4525272):setVisible(false)
    end
    --选择散功宠物
    inheritance:addTouchEventListener(choose)
end
--显示传承的预览信息
function GodAnimalSystem_InheritPrediction(widget,info)
    local detail = info.acceptPostDetail
    local leftImage = UI_GetUIImageView(widget, 4524938)
    local inheritance = UI_GetUIImageView(leftImage, 4525237) 
    local StepLevelUpInfo = UI_GetUIImageView(inheritance, 4525255)
    StepLevelUpInfo:setVisible(true) 
    --等级
    UI_GetUILabel(StepLevelUpInfo, 4525312):setText(detail.iLevel)
    --生命
    UI_GetUILabel(StepLevelUpInfo, 4525347):setText(detail.szPropList[1].iValue)
    --攻击
    UI_GetUILabel(StepLevelUpInfo, 4525348):setText(detail.szPropList[2].iValue)
    --铜币消耗
    UI_GetUILabel(inheritance, 4525354):setText(info.iInheritSilverCost)
     --传承
    local function Inheritance()     
        GodAnimalSystem_inheritance(detail.dwObjectID,info.passerDetail.dwObjectID)
        GodAnimalSystem_GetAnimalData(detail.dwObjectID)  
    end
    --传承提示框
    local function determine(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            local tab = {}
            tab.info = FormatString("InheritanceExpense1")..info.iInheritSilverCost..FormatString("InheritanceExpense2")
            tab.msgType = EMessageType_LeftRight
            tab.leftFun = Inheritance
            Messagebox_Create(tab)
        end
    end
    --传承
    UI_GetUIButton(inheritance, 4525372):addTouchEventListener(determine)
end
------------------------------------------------------------------------------------------
--培养
function GodAnimalSystem_Train(widget, detail)
    local leftImage = UI_GetUIImageView(widget, 4524938)
    --local inheritance = UI_GetUIImageView(leftImage, 4525237)   
    local train = UI_GetUIImageView(leftImage, 4526346)
    --返回
     local function cancel(sender,eventType )
        if eventType == TOUCH_EVENT_ENDED then
            GodAnimalSystem_ShowListView(widget)
            GodAnimalSystem_refreshListView(widget)
            GodAnimalSystem_SetSelected(-1)
        end
    end
    --培养
    local function Train(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
			if (g_isGuide) then
				Guide_GoNext();
			end
            GodAnimalSystem_TrainMsg(detail.dwObjectID)
        end
    end
    --返回
    UI_GetUIButton(train, 4526357):addTouchEventListener(cancel)
    --培养
    UI_GetUIButton(train, 4526354):addTouchEventListener(Train)
    --经验条
    UI_GetUILoadingBar(train, 4526351):setPercent(detail.iCurExp/detail.iNeedExp*100)
end
--进阶信息显示
function GodAnimalSystem_StepLevelUp(info)
    local widget = UI_CreateBaseWidgetByFileName("UIExport/DemoLogin/GodAnimalStepLevelUP.json")
    --UI_GetUIButton(widget, 4526751):addTouchEventListener(UI_ClickCloseCurBaseWidget)
    local GodAnimalData = GetLocalPlayer():GetGodAnimalByDwObject(info.dwNewGodAnimalObjectId):getGodAnimalData()
    -----------------------原有---------------------------------------------
    --原有：头像
    UI_GetUIImageView(widget, 4526765):loadTexture(GodAnimalSystem_getPath("iconHero", GodAnimalData.m_ID))
    local step_ = GodAnimalSystem_getStep(info.oldState.iGodAnimalLevelStep)
    --原有：仙、凡
    UI_GetUIImageView(widget, 4526774):loadTexture(GodAnimalSystem_getPath("Step",step_))
    --原有：头像外框
    UI_GetUIImageView(widget, 4526766):loadTexture(GodAnimalSystem_getPath("animalHeadCommon", step_))
    --名字
    local name = GodAnimalData.m_name
    if GodAnimalSystem_getStepNum(info.oldState.iGodAnimalLevelStep)>0 then
        name = name.."+"..GodAnimalSystem_getStepNum(info.oldState.iGodAnimalLevelStep)
    end
    UI_GetUILabel(widget, 4526781):setText(name)
    --------------------------------------------------------------------------
    ------------------------当前----------------------------------------------
    --现在：头像
    UI_GetUIImageView(widget, 4526771):loadTexture(GodAnimalSystem_getPath("iconHero", GodAnimalData.m_ID))
    local step_ = GodAnimalSystem_getStep(info.newState.iGodAnimalLevelStep)
    --现在：仙、凡
    UI_GetUIImageView(widget, 4526777):loadTexture(GodAnimalSystem_getPath("Step",step_))
    --现在：头像外框
    UI_GetUIImageView(widget, 4526772):loadTexture(GodAnimalSystem_getPath("animalHeadCommon", step_))
    --名字
    local name = GodAnimalData.m_name
    if (GodAnimalSystem_getStepNum(info.newState.iGodAnimalLevelStep)>0) then
        name = name.."+"..GodAnimalSystem_getStepNum(info.newState.iGodAnimalLevelStep)
    end
    UI_GetUILabel(widget, 4526788):setText(name)
    --------------------------------------------------------------------------
    --生命
    UI_GetUILabel(widget, 4526804):setText(info.oldState.iHpGrow/10000)
    UI_GetUILabel(widget, 4526810):setText((info.newState.iHpGrow-info.oldState.iHpGrow)/10000)
    UI_GetUILabel(widget, 4526816):setText("("..FormatString("en_LifeAtt_MaxHP").."+"..info.iHPAddValue..")")
    --攻击
    UI_GetUILabel(widget, 4526800):setText(info.oldState.iAttGrow/10000)
    UI_GetUILabel(widget, 4526812):setText((info.newState.iAttGrow-info.oldState.iAttGrow)/10000)
    UI_GetUILabel(widget, 4526813):setText("("..FormatString("en_LifeAtt_Att").."+"..info.iAttAddValue..")")

end
--获得碎片出处
function GodAnimalSystem_GetFragment(iItemID,need)
    local widget = UI_CreateBaseWidgetByFileName("UIExport/DemoLogin/GodAnimalFragment.json")
    UI_GetUIButton(widget, 4528164):addTouchEventListener(UI_ClickCloseCurBaseWidget)
   print("iItemID=="..iItemID)
    --获取需求的碎片的基本信息
    local itemData = GetGameData(DataFileItem,iItemID,"stItemData")
    --图标
    local icon = UI_ItemIconFrame(UI_GetItemIcon(iItemID, -1), itemData.m_quality)
    UI_GetUIImageView(widget, 4528132):addChild(icon)
    --名字
    UI_GetUILabel(widget, 4528135):setText(itemData.m_name)
    --拥有魂魄数
    local haveNum = GetLocalPlayer():GetPlayerBag():GetItemCountByItemId(iItemID)
    --显示数量和需求
    UI_GetUILabel(widget, 4528137):setText( "("..haveNum.."/"..need..")" )
    --九天幻境
    local function GetMethodsOne(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            Packet_Cmd(GameServer_pb.CMD_DREAMLAND_OPEN_REQSECTON);
			ShowWaiting()
        end
    end
    --商城
    local function GetMethodsTwo(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            UI_CloseCurBaseWidget()
            Shop_Create()
        end
    end
    --九天幻境
    UI_GetUIButton(widget, 4528153):addTouchEventListener(GetMethodsOne)
    --商城
    UI_GetUIButton(widget, 4528150):addTouchEventListener(GetMethodsTwo)
end
----------------------------------------------------------------------------------------
--激活培养界面并屏蔽宠物列表和传承界面
function GodAnimalSystem_ShowTrain(widget)
    local leftImage = UI_GetUIImageView(widget, 4524938)
    local inheritance = UI_GetUIImageView(leftImage, 4525237)   
    local train = UI_GetUIImageView(leftImage, 4526346)
    local listView = UI_GetUIListView( leftImage , 4525076)
    listView:removeAllItems()
    --恢复“仙宠”列表的按键响应
    listView:setVisible(false)
    listView:setTouchEnabled(false)
    --屏蔽"传承"界面的“传承”和“取消”按钮  
    --确定
    UI_GetUIButton(inheritance, 4525372):setTouchEnabled(false)
    --取消
    UI_GetUIButton(inheritance, 4525374):setTouchEnabled(false)
    --散功宠物
    inheritance:setTouchEnabled(false)
    inheritance:setVisible(false)  
    --屏蔽“培养”界面
    train:setVisible(true)
    UI_GetUIButton(train, 4526354):setTouchEnabled(true)
    UI_GetUIButton(train, 4526357):setTouchEnabled(true)
end
--激活传承界面并屏蔽宠物列表和培养界面
function GodAnimalSystem_ShowInheritance(widget)

    local leftImage = UI_GetUIImageView(widget, 4524938)
    local inheritance = UI_GetUIImageView(leftImage, 4525237)   
    local train = UI_GetUIImageView(leftImage, 4526346)
    local listView = UI_GetUIListView( leftImage , 4525076)
    --恢复“仙宠”列表的按键响应
    listView:setVisible(false)
    listView:setTouchEnabled(false)
    --屏蔽"传承"界面的“传承”和“取消”按钮  
    --确定
    UI_GetUIButton(inheritance, 4525372):setTouchEnabled(true)
    --取消
    UI_GetUIButton(inheritance, 4525374):setTouchEnabled(true)
    --散功宠物
    inheritance:setTouchEnabled(true)
    inheritance:setVisible(true)  
    --屏蔽“培养”界面
    train:setVisible(false)
    UI_GetUIButton(train, 4526354):setTouchEnabled(false)
    UI_GetUIButton(train, 4526357):setTouchEnabled(false)
    --将散功仙兽图标隐藏
    UI_GetUIImageView(inheritance, 4526264):setVisible(false)
    listView:removeAllItems()
    print("-----------------------------GodAnimalSystem_ShowInheritance-------------------------")
end
--激活仙宠列表并屏蔽传承界面和培养界面
function GodAnimalSystem_ShowListView(widget)

    local leftImage = UI_GetUIImageView(widget, 4524938)
    local inheritance = UI_GetUIImageView(leftImage, 4525237)
    local train = UI_GetUIImageView(leftImage, 4526346)
    local listView = UI_GetUIListView( leftImage , 4525076)
    listView:removeAllItems()
    listView:setVisible(true)
    --恢复“仙宠”列表的按键响应
    listView:setTouchEnabled(true)
    --屏蔽“培养”界面
    train:setVisible(false)
    UI_GetUIButton(train, 4526354):setTouchEnabled(false)
    UI_GetUIButton(train, 4526357):setTouchEnabled(false)
    --屏蔽"传承"界面的“传承”和“取消”按钮  
    --确定
    UI_GetUIButton(inheritance, 4525372):setTouchEnabled(false)
    --取消
    UI_GetUIButton(inheritance, 4525374):setTouchEnabled(false)
    --散功宠物
    inheritance:setTouchEnabled(false)
    inheritance:setVisible(false)
end
--------------------------------------------------------------------------------------------------------------------------
--网络消息;获取神兽详细信息
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_GODANIMAL_DETAIL, "GodAnimalSystem_ShowGodAnimalDetai" )
--网络消息;获取未获得的神兽详细信息
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_GODANIMAL_DETAIL_BYID, "GodAnimalSystem_ShowNotHaveGodAnimalDetai" )
--网络消息;召唤成功
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_GODANIMAL_ANIMALSOUL_ACTIVE, "GodAnimalSystem_ActionactivitySuccess" )
--网络消息：传承
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_GODANIMAL_EXEINHERIT, "GodAnimalSystem_ExeGodAnimalInheritReturn" )
--网络消息：查询传承后的数据
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_GODANIMAL_QUERYINHERIT, "GodAnimalSystem_InheritanceInfo" )
--网络消息：培养
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_GODANIMAL_TRAIN, "GodAnimalSystem_TrainSuccess" )
--网络消息：升阶查询
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_GODANIMAL_QUERYLEVELSTEP, "GodAnimalSystem_QueryStepLevelUpInfo" )
--网络消息：升阶
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_GODANIMAL_EXELEVELSTEPUP, "GodAnimalSystem_QueryStepLevelUp" )
--网络消息：出战
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_GODANIMAL_ACTIVE, "GodAnimalSystem_GetGodAnimalPlayed" )
---------------------------------------------------------------------------------------------------------------------------
--获取神兽详细信息
function GodAnimalSystem_ShowGodAnimalDetai(pkg)
    EndWaiting()

    local info = GameServer_pb.Cmd_Sc_QueryGodAnimalDetai()
    info:ParseFromString(pkg)
    Log("info===="..tostring(info))
    local widget = UI_GetBaseWidgetByName("GodAnimalSystem")

    if widget == nil then
        widget = GodAniamalSystem_createWidget()

    end

    --右边数据中的上方公共部分
    GodAnimalSystem_setPublicData(widget,info.detail)
    --当宠物存在时，显示详细信息，并提供操作
    GodAnimalSystem_ShowData(widget,info.detail)
    --刷新经验显示
    GodAnimalSystem_Train(widget, info.detail)
    --更新传承界面
    GodAnimalSystem_Inherit(widget,info.detail)
    --刷新宠物列表
   if g_isRefreshListView then
        GodAnimalSystem_refreshListView(widget)
        g_isRefreshListView = false
    end
   
   g_GodAnimalSystem_lastlevel = info.detail.iLevel
end
--获取未获得的神兽详细信息
function GodAnimalSystem_ShowNotHaveGodAnimalDetai(pkg)
   EndWaiting()
   local info = GameServer_pb.CMD_GODANIMAL_DETAIL_BYID_SC()
   info:ParseFromString(pkg)
   Log("Heroxxxxxx==="..tostring(info))
   local widget = UI_GetBaseWidgetByName("GodAnimalSystem")
   --右边数据中的上方公共部分
   GodAnimalSystem_setPublicData(widget,info.detail)
   --召唤界面
   GodAnimalSystem_ShowDataNG(widget,info.detail)
end 
--召唤成功
function GodAnimalSystem_ActionactivitySuccess(pkg)
   EndWaiting()
   local widget = UI_GetBaseWidgetByName("GodAnimalSystem")
   --更新宠物列表
   local animalIDList = vector_int_:new_local() 
   GetLocalPlayer():sortGodAnimalWithId(animalIDList)

   GodAnimalSystem_refreshListView(widget,animalIDList)

   for i=0,animalIDList:size()-1,1 do
        local animal = GetLocalPlayer():GetGodAnimalByAnimalId(animalIDList[i])
        if animal:getGodAnimalData().m_ID ==g_GodAnimalSystem_callID then
            print("i==============="..i)
            GodAnimalSystem_refreshListViewHL(i,animalIDList:size(),widget)
            --显示召唤仙宠的数据
            local ID = animal:GetInt(EGodAnimal_ObjId)
            GodAnimalSystem_GetAnimalStepLevelUpInfo(ID)
            GodAnimalSystem_GetAnimalData(ID)

                        --请求第一个仙宠的数据
            --GodAnimalSystem_GetAnimalData(animalList[i]:GetInt(EGodAnimal_ObjId))
            --第一个仙宠的升阶数据预览
            --GodAnimalSystem_GetAnimalStepLevelUpInfo(animalList[i]:GetInt(EGodAnimal_ObjId))
            return
        end
    end
end 
--传承
function GodAnimalSystem_ExeGodAnimalInheritReturn(pkg)
   EndWaiting()
end
--查询传承后的数据
function GodAnimalSystem_InheritanceInfo(pkg)
   EndWaiting()
   local widget = UI_GetBaseWidgetByName("GodAnimalSystem")
   local info = GameServer_pb.Cmd_Sc_QueryGodAnimalInherit()
   info:ParseFromString(pkg)
   Log("______info ====="..tostring(info)) 
   --显示传承的预览信息
   GodAnimalSystem_InheritPrediction(widget,info)
   
end
--培养
function GodAnimalSystem_TrainSuccess(pkg)
    EndWaiting()
    local info = GameServer_pb.Cmd_Sc_GoldAnimalTrain()
    info:ParseFromString(pkg)
    local widget = UI_GetBaseWidgetByName("GodAnimalSystem")
    if widget then 
        Log("train info==========="..tostring(info))

        local leftImage = UI_GetUIImageView(widget, 4524938)  
        local train = UI_GetUIImageView(leftImage, 4526346)

        
        local uplevel = info.detail.iLevel - g_GodAnimalSystem_lastlevel
        --培养-经验条增加
        GodAnimalSystem_UpdateLoadingbar(train,info.detail,uplevel)
        --培养-等级和经验
        GodAnimalSystem_UpdateNumberAction(train,info,uplevel)
        if uplevel>0 then        
            --更新宠物数据
            GodAnimalSystem_ShowData(widget,info.detail)
        end
        g_GodAnimalSystem_lastlevel = info.detail.iLevel
    end 
end 
--升阶查询
function GodAnimalSystem_QueryStepLevelUpInfo(pkg) 
    EndWaiting()
    local info = GameServer_pb.Cmd_Sc_QueryGodAnimalLevelStep()
    info:ParseFromString(pkg)  
    print("____stepLevel===="..tostring(info))
    
    GodAnimalSystem_SetStepLevelUpInfo(info) 
end 
function GodAnimalSystem_QueryStepLevelUp(pkg)
    EndWaiting()

    local info = GameServer_pb.CMD_GODANIMAL_EXELEVELSTEPUP_SC()
    info:ParseFromString(pkg)  
    print("+++++++stepLevel===="..tostring(info)) 

    --更新宠物列表
    local widget = UI_GetBaseWidgetByName("GodAnimalSystem")  
    GodAnimalSystem_refreshListView(widget) 
    --查询当前数据  
    g_isRefreshListView = true
    GodAnimalSystem_GetAnimalData(info.dwNewGodAnimalObjectId)
    
    GodAnimalSystem_StepLevelUp(info)
end
--出战
function GodAnimalSystem_GetGodAnimalPlayed(pkg)
    EndWaiting()
    local info = GameServer_pb.Cmd_Sc_GoldAnimalActive()
    info:ParseFromString(pkg)
    local widget = UI_GetBaseWidgetByName("GodAnimalSystem")
    print("GodAnimalSystem_GetGodAnimalPlayed = "..tostring(info))
    --设置出战
    GetLocalPlayer():SetGodAnimalActive(info.dwObjectID)
    --更新宠物列表
    GodAnimalSystem_refreshListView(widget)
    GodAnimalSystem_refreshListViewHL(0,1,widget)
end 
---------------------------------网络消息所产生的相关动画等------------------------------------
--培养-经验条增加
function GodAnimalSystem_UpdateLoadingbar(widget,detail,uplevel) 
    local upExppercent = detail.iCurExp / detail.iNeedExp
    UI_GetUILoadingBar(widget, 4526351):setPercentSlowly(upExppercent * 100 + uplevel * 100,1.0)
end
--培养-等级和经验
function GodAnimalSystem_UpdateNumberAction(widget,info,uplevel)
   --升级 action
	local animaltionlayoutPositionX , animaltionlayoutPositionY = widget:getPosition()

	local bezierconfig = ccBezierConfig:new_local()
	bezierconfig.controlPoint_1 = ccp(animaltionlayoutPositionX,animaltionlayoutPositionY + 120)
	bezierconfig.controlPoint_2 = ccp(animaltionlayoutPositionX - 100,animaltionlayoutPositionY + 120)
	bezierconfig.endPosition = ccp(animaltionlayoutPositionX - 120,animaltionlayoutPositionY + 50 )
	local bezierAction = CCBezierTo:create(1.0,bezierconfig)

	local fadeOut = CCFadeOut:create(0.5)
	local actionArray = CCArray:create()
	local  function rmoveSelf(sender)
	    local sender = tolua.cast(sender,"Label")
		sender:removeFromParent()
	end
	local callback = CCCallFuncN:create(rmoveSelf)
	actionArray:addObject(bezierAction)
	actionArray:addObject(fadeOut)
	actionArray:addObject(callback)
	local  action = CCSequence:create(actionArray)

	if uplevel ~= 0 then
        local levelUPLabel = CompLabel:GetDefaultCompLabel(FormatString("GadAnimal_LevelUP",uplevel),192)
		levelUPLabel:setPosition( ccp(animaltionlayoutPositionX,animaltionlayoutPositionY))
		levelUPLabel:setAnchorPoint(ccp(0.5,0.5))
		widget:addChild(levelUPLabel,1000,1000)
		levelUPLabel:runAction(action)
	end

	----经验action
    local bezierconfig = ccBezierConfig:new_local()
	bezierconfig.controlPoint_1 = ccp(animaltionlayoutPositionX,animaltionlayoutPositionY + 120)
	bezierconfig.controlPoint_2 = ccp(animaltionlayoutPositionX + 100,animaltionlayoutPositionY + 120)
	bezierconfig.endPosition = ccp(animaltionlayoutPositionX + 120,animaltionlayoutPositionY + 50 )
	local bezierAction = CCBezierTo:create(1.0,bezierconfig)

	local fadeOut = CCFadeOut:create(0.5)
	local actionArray = CCArray:create()
	local  function rmoveSelf(sender)
	    local sender = tolua.cast(sender,"Label")
		sender:removeFromParent()
	end
	local callback = CCCallFuncN:create(rmoveSelf)
	actionArray:addObject(bezierAction)
	actionArray:addObject(fadeOut)
	actionArray:addObject(callback)
	local  action = CCSequence:create(actionArray)
    
    local expLabel = LabelBMFont:create()
    expLabel:setFntFile("Fonts/chengse.fnt")
    Log("expUp===="..info.iGetExp)
    if bKnock then
      local str = FormatString("GadAnimal_Knock",info.iGetExp)
      expLabel:setText(str)
    	--expLabel = CompLabel:GetDefaultCompLabel(FormatString("GadAnimal_Knock",iGetExp),192);
    else
      expLabel:setText("+ "..info.iGetExp)
    	--expLabel = CompLabel:GetDefaultCompLabel(FormatString("GadAnimal_LifeAtt_Exp",iGetExp),192);
    end
    expLabel:runAction(action)
    
    expLabel:setPosition( ccp(animaltionlayoutPositionX,animaltionlayoutPositionY))
	expLabel:setAnchorPoint(ccp(0.5,0.5))
	widget:addChild(expLabel,500,500)
end 
