--这是一个全局的选中英雄的计数，因为英雄界面涉及到很多的点击英雄的交互
if g_SelectIndex == nil then
   g_SelectIndex = 0
end

--UI的变动 英雄列表和 英雄转换在一个界面上面 用标签页进行切换
--修改 by yjj 2014 5 15 
--herolist费了  2014 5 29
function HeroList_Create()
	local widget = UI_CreateBaseWidgetByFileName("HeroList.json");
	if (widget == nil)then
		Log("CreateRole_SelectHero error");
		return;
	end
    --return
    local closeButton = UI_GetUIButton(widget,1)
	closeButton:addTouchEventListener(UI_ClickCloseCurBaseWidget)

	--tab_herolist
	local function ClickHeroListInfoFunc(sender,eventType)
		-- body
		if eventType == TOUCH_EVENT_ENDED then
			HerolistInfo(widget)
		end
	end
	UI_GetUIButton(widget,2):addTouchEventListener(ClickHeroListInfoFunc)

	--tab_heroconvert
	local function ClickHeroConvertInfoFunc(sender,eventType)
		-- body
		if eventType == TOUCH_EVENT_ENDED then
			HeroConVertInfo(0,widget)
		end
	end
	UI_GetUIButton(widget,3):addTouchEventListener(ClickHeroConvertInfoFunc)

    HerolistInfo(widget)

    -- local image = UI_GetUIImageView(widget,66481)
    -- local function runmove(sender,eventType)
    -- 	-- body
    -- 	if eventType == TOUCH_EVENT_ENDED then
    -- 	   local actionone = CCSkewTo:create(1, 3, 0)
    --        local actionTwo = CCSkewTo:create(1, -3, 0)
    --        local actionobject = CCArray:create()
    --        actionobject:addObject(actionone)
    --        actionobject:addObject(actionTwo)
    --        local tempseqaction = CCSequence:create(actionobject)
    --        local a = CCRepeat:create(tempseqaction,10)
    -- 	   image:runAction(a)
    --     end
    -- end
    -- image:setTouchEnabled(true)
    -- image:addTouchEventListener(runmove)
end

function HerolistInfo(widget)
	-- body
    --显示herolist界面 隐藏heroconvert界面
    UI_GetUILayout(widget,4):setVisible(true)
    UI_GetUILayout(widget,6):setVisible(false)

    --按钮状态
    UI_GetUIButton(widget,2):setButtonEnabled(false)
    UI_GetUIButton(widget,3):setButtonEnabled(true)

    local herolistparent = UI_GetUILayout(widget,4)

	local isMove = false
	local heroList = GetLocalPlayer():GetHeroList(false);
	local listView = UI_GetUIListView(herolistparent, 2);
	local function ClickHero(sender,eventType)
		if  (eventType == TOUCH_EVENT_ENDED) then
		     local list = tolua.cast( sender, "Layout");
		     local selectIndex = listView:getIndex(list)
		     Log("selectIndex = "..selectIndex);
             g_SelectIndex = selectIndex
             UI_CloseAllBaseWidget()
		     local tab = GameServer_pb.Cmd_Cs_GetHeroDesc();
		     local heroEntity = GetEntityById(heroList[selectIndex], "Hero");
			 tab.dwObjectID = heroEntity:GetUint(EHero_ID);
			 Packet_Full(GameServer_pb.CMD_GET_HERODESC, tab);
			 ShowWaiting();		
		end
	end 
	listView:removeAllItems()
    local curlayout = nil 
    local function createHeroListInfo(index)
    	-- body
    	--name
    	local heroEntity = GetEntityById(heroList[index], "Hero");
    	local data = heroEntity:GetHeroData();
    	local heroLvlStep = heroEntity:GetUint(EHero_LvlStep);
    	Log("GetHeroName========"..tostring(GetHeroName(data.m_name, heroLvlStep)))
    	local nameLab = CompLabel:GetCompLabel(GetHeroName(data.m_name, heroLvlStep), 192, kCCTextAlignmentLeft)
    	--由于名字为富文本 所以这里只获取坐标
    	local nameX,nameY = UI_GetUILabel(curlayout,8):getPosition()
    	nameLab:setPosition(ccp(nameX,nameY)) 
    	nameLab:setAnchorPoint(ccp(0.5,0.5))
    	curlayout:addChild(nameLab)

    	--为富文本 所以这里只获取坐标
    	local lvlX,lvlY = UI_GetUILabel(curlayout,2):getPosition()
    	local lvlLab = CompLabel:GetCompLabel(FormatString("HeroList_Level", heroEntity:GetUint(EHero_Lvl)), 192, kCCTextAlignmentLeft);
    	lvlLab:setPosition(ccp(lvlX,lvlY))
    	lvlLab:setAnchorPoint(ccp(0.5,0.5))
    	curlayout:addChild(lvlLab)

    	--星星
    	local starLvl = GetHeroStarLvl(heroLvlStep)
    	local startagTab = {3,4,5,6,7}
    	setHeroStarImageByClone(curlayout,startagTab,starLvl)
        --heroimag
        local img = UI_GetHeroImg(data.m_img1, data.m_img2);
        UI_GetHeroImgByClone(data.m_img1, data.m_img2,curlayout,1)

        if (GetLocalPlayer():IsInFormation(heroEntity))then
        	UI_GetUIImageView(curlayout,9):setVisible(true)
	   end
    end

	for i = 0, heroList:size()-1 do
			curlayout = UI_GetCloneLayout(UI_GetUILayout(widget,5));
            curlayout:setVisible(true)
			createHeroListInfo(i)
			curlayout:addTouchEventListener(ClickHero)
		    curlayout:setTouchEnabled(true)
		    listView:pushBackCustomItem(curlayout);
	end					
	UI_SetLabelText(herolistparent,1,heroList:size())
end

function setHeroStarImageByClone(curlayout,startagTab,starLvl)
	-- body
	--方便UI 苦了程序( // - - ||)
	for i = 1,#startagTab do
		UI_GetUIImageView(curlayout,startagTab[i]):setVisible(false)
	end
    if starLvl == 1 then
		UI_GetUIImageView(curlayout,startagTab[3]):setVisible(true)
	elseif starLvl == 2 then
		UI_GetUIImageView(curlayout,startagTab[2]):setVisible(true)
		UI_GetUIImageView(curlayout,startagTab[4]):setVisible(true)
		local starOneX,starOneY = UI_GetUIImageView(curlayout,startagTab[2]):getPosition()
		local starWidth = UI_GetUIImageView(curlayout,startagTab[2]):getContentSize().width / 2 - 2
		UI_GetUIImageView(curlayout,startagTab[2]):setPosition(ccp(starOneX + starWidth,starOneY))
		local starTwoX,starTwoY = UI_GetUIImageView(curlayout,startagTab[4]):getPosition()
		UI_GetUIImageView(curlayout,startagTab[4]):setPosition(ccp(starTwoX - starWidth,starTwoY))
	elseif starLvl == 3 then
		UI_GetUIImageView(curlayout,startagTab[2]):setVisible(true)
		UI_GetUIImageView(curlayout,startagTab[3]):setVisible(true)
		UI_GetUIImageView(curlayout,startagTab[4]):setVisible(true)
	elseif starLvl == 4 then
		UI_GetUIImageView(curlayout,startagTab[1]):setVisible(true)
		UI_GetUIImageView(curlayout,startagTab[2]):setVisible(true)
		UI_GetUIImageView(curlayout,startagTab[4]):setVisible(true)
        UI_GetUIImageView(curlayout,startagTab[5]):setVisible(true)
        local starWidth = UI_GetUIImageView(curlayout,startagTab[2]):getContentSize().width / 2 - 2
        local starOneX,starOneY = UI_GetUIImageView(curlayout,startagTab[1]):getPosition()
		local starTwoX = UI_GetUIImageView(curlayout,startagTab[2]):getPosition()
		local starThreeX = UI_GetUIImageView(curlayout,startagTab[4]):getPosition()
		local starFourX = UI_GetUIImageView(curlayout,startagTab[5]):getPosition()
        UI_GetUIImageView(curlayout,startagTab[1]):setPosition(ccp(starOneX + starWidth,starOneY))
        UI_GetUIImageView(curlayout,startagTab[2]):setPosition(ccp(starTwoX + starWidth,starOneY))
        UI_GetUIImageView(curlayout,startagTab[4]):setPosition(ccp(starThreeX - starWidth,starOneY))
        UI_GetUIImageView(curlayout,startagTab[5]):setPosition(ccp(starFourX - starWidth,starOneY))
	elseif starLvl == 5 then
		UI_GetUIImageView(curlayout,startagTab[1]):setVisible(true)
		UI_GetUIImageView(curlayout,startagTab[2]):setVisible(true)
		UI_GetUIImageView(curlayout,startagTab[3]):setVisible(true)
		UI_GetUIImageView(curlayout,startagTab[4]):setVisible(true)
		UI_GetUIImageView(curlayout,startagTab[5]):setVisible(true)
    end

end


function UpdateHeroListPos()
	local widget = UI_GetBaseWidgetByName("HeroList");
	if (widget == nil) then
		local listView = UI_GetUIListView(widget, 72);
		local curPos = listView:getInnerContainer().getPosition();
		local curIndex = ToInt(curPos/192);	
	end
end

function GetHeroPinjie(heroLvlStep)
	local data = GetGameData(DataFileHeroLevelStepGrow, heroLvlStep, "stLevelStepGrowData")
	return data.m_step;
end

function GetHeroStarLvl(heroLvlStep)
	local data = GetGameData(DataFileHeroLevelStepGrow, heroLvlStep, "stLevelStepGrowData")
	return data.m_quality - 1;
end

function GetHeroPingzi(quality)
	local data = GetGameData(DataFileQualityProp, quality, "stQualityPropData")
	return data.m_color;
end
function GetHeroJieDuan(quality)
	local data = GetGameData(DataFileQualityProp, quality, "stQualityPropData")
	return data.m_Pingjie;
end
function GetHeroName(name, heroLvlStep, isSmall, quality)
	local data = GetGameData(DataFileHeroLevelStepGrow, heroLvlStep, "stLevelStepGrowData")
	if (data.m_step == 0)then
		if (isSmall)then
			--return GetStarName(heroLvlStep, isSmall)..FormatString("HeroNameSmall", UI_GetStringColor(GetHeroPingzi(heroLvlStep)), name);
            return FormatString("HeroNameSmall", UI_GetStringColor(data.m_quality), name)
		else
			--return GetStarName(heroLvlStep, isSmall)..FormatString("HeroName", UI_GetStringColor(GetHeroPingzi(heroLvlStep)), name);
            return FormatString("HeroName", UI_GetStringColor(data.m_quality), name)
		end
	else
		if (isSmall)then
			--return GetStarName(heroLvlStep, isSmall)..FormatString("HeroNameSmall_1", UI_GetStringColor(GetHeroPingzi(heroLvlStep)), name, GetHeroPinjie(heroLvlStep));
            return FormatString("HeroNameSmall_1", UI_GetStringColor(data.m_quality), name, data.m_step)
		else
			--return GetStarName(heroLvlStep, isSmall)..FormatString("HeroName_1", UI_GetStringColor(GetHeroPingzi(heroLvlStep)), name, GetHeroPinjie(heroLvlStep));
            return FormatString("HeroName_1", UI_GetStringColor(data.m_quality), name, data.m_step)
		end
	end
end

function GetHeroNewName(name,quality)
   Log("quality==="..quality)
   local data = GetGameData(DataFileQualityProp, quality, "stQualityPropData")
   if data.m_Pingjie == 0 then 
      return FormatString("HeroName", UI_GetStringColor(quality), name)
   else
      return FormatString("HeroName_1", UI_GetStringColor(quality), name, data.m_Pingjie)
   end 
end 

function GetHeroNameByLabel(name,heroPingzi,Fontsize)
   local data = GetGameData(DataFileQualityProp, heroPingzi, "stQualityPropData")
   local labe = Label:create()
   local str = nil 
   if data.m_Pingjie == 0 then 
      str = name
   else
      str = name.."+"..data.m_Pingjie
   end 
   labe:setText(str)
   labe:setFontSize(Fontsize)
   UI_SetLabelFontName(1,labe)
   labe:setColor(UI_GetStringColorNew(data.m_color))
   --labe:enableShadow()
   labe:enableStroke()
   return labe
end

function UI_SetLabelFontName(index,label)
   local fontname = ""
   if index == 1 then 
      fontname = "Fonts/FZZhunYuan-M02S.ttf"
   else 
      fontname = "Fonts/FZZhunYuan-M02S.ttf"
   end 
   label:setFontName(fontname)
end 

function GetHeroQualityIcon(heroPingzi,layout,tagtable)
   local data = GetGameData(DataFileQualityProp, heroPingzi, "stQualityPropData")
   local icon = nil 
   local pingjie = data.m_Pingjie
   Log("pingjie==="..pingjie)
   for i = 1,#tagtable do 
      UI_GetUILayout(layout,tagtable[i]):setVisible(false)
   end 
   if pingjie == 0 then 
   else
      UI_GetUILayout(layout,tagtable[pingjie]):setVisible(true)
      for i = 1, pingjie do 
         UI_GetUIImageView(UI_GetUILayout(layout,tagtable[pingjie]),i):loadTexture("Common/Icon_class_0"..(data.m_color - 1)..".png")
      end 
   end 
end 

function GetHeroNameByString(name,heroLvlStep)
   local data = GetGameData(DataFileHeroLevelStepGrow, heroLvlStep, "stLevelStepGrowData")
   if data.m_step % 10 == 0 then 
      return FormatString("HeroName", UI_GetStringColor(data.m_quality), name)
   else
      return FormatString("HeroName_1", UI_GetStringColor(data.m_quality), name, data.m_step)
   end
end 

function GetItemNameByLabel(name,quality,Fontsize)
   local labe = Label:create()
   labe:setText(name)
   labe:setFontSize(Fontsize)
   labe:setColor(UI_GetStringColorNew(quality))
   return labe
end  

function GetHeroLevelStepAndImage(heroLvlStep)
   local layout = Layout:create()
   local iconstr = ""
   if GetHeroStarLvl(heroLvlStep) == 1 then
		iconstr = "Common/Fort_Rank_01.png"
	elseif GetHeroStarLvl(heroLvlStep) == 2 then
		iconstr = "Common/Fort_Rank_02.png"
	elseif GetHeroStarLvl(heroLvlStep) == 3 then
		iconstr = "Common/Fort_Rank_03.png"
	elseif GetHeroStarLvl(heroLvlStep) == 4 then
		iconstr = "Common/Fort_Rank_04.png"
    elseif GetHeroStarLvl(heroLvlStep) == 5 then
		iconstr = "Common/Fort_Rank_05.png"
	end
    local image = ImageView:create()
    image:loadTexture(iconstr)
    layout:addChild(image)
    image:setAnchorPoint(ccp(0,0))
    image:setPosition(ccp(0,0))
	
	local data = GetGameData(DataFileHeroLevelStepGrow, heroLvlStep, "stLevelStepGrowData")
	
    if (data.m_step % 10 == 0) then
        
    else 
       local str = FormatString("HeroSystem_LevelStep", UI_GetStringColor(data.m_quality),data.m_step)
       local strlab = CompLabel:GetCompLabel(str,192,kCCTextAlignmentLeft)
       layout:addChild(strlab)
       strlab:setPosition(ccp(image:getContentSize().width - 10 ,image:getContentSize().height / 2 - 10))
       strlab:setAnchorPoint(ccp(0,0))
    end 
    return layout 
end

function GetHeroStarNameAndStep(heroLvlStep, isSmall)
	local data = GetGameData(DataFileHeroLevelStepGrow, heroLvlStep, "stLevelStepGrowData")
	if (data.m_step == 0)then
		if (isSmall)then
			return GetStarNameByHeroAscending(heroLvlStep, isSmall)..FormatString("StarNameAndStepSmall", UI_GetStringColor(data.m_quality));
		else
			return GetStarNameByHeroAscending(heroLvlStep, isSmall)..FormatString("StarNameAndStepSmall", UI_GetStringColor(data.m_quality));
		end
	else
		if (isSmall)then
			return GetStarNameByHeroAscending(heroLvlStep, isSmall)..FormatString("StarNameAndStepSmall_1", UI_GetStringColor(data.m_quality),data.m_step);
		else
			return GetStarNameByHeroAscending(heroLvlStep, isSmall)..FormatString("StarNameAndStepSmall_1", UI_GetStringColor(data.m_quality),data.m_step);
		end	
	end
end

function GetStarName(heroLvlStep, isSmall)
	-- body
	local type = nil
	if GetHeroStarLvl(heroLvlStep) == 2 then
		type = FormatString("StarClassSpirit")
	elseif GetHeroStarLvl(heroLvlStep) == 3 then
		type = FormatString("StarClassImmortal")
	elseif GetHeroStarLvl(heroLvlStep) == 4 then
		type = FormatString("StarClassGod")
	elseif GetHeroStarLvl(heroLvlStep) == 5 then
		type = FormatString("StarClassSaint")
	end
	local data = GetGameData(DataFileHeroLevelStepGrow, heroLvlStep, "stLevelStepGrowData")
	if (type)then
		if (isSmall)then
			return FormatString("StarNameSmall",UI_GetStringColor(data.m_quality),type)
		else
			return FormatString("StarName",UI_GetStringColor(data.m_quality),type)
		end
	end
	
	return ""
end
--单独给英雄升阶要求的 这是策划的要求
function GetStarNameByHeroAscending(heroLvlStep, isSmall)
	-- body
	local type = nil
	if GetHeroStarLvl(heroLvlStep) == 2 then
		type = FormatString("StarClassSpirit")
	elseif GetHeroStarLvl(heroLvlStep) == 3 then
		type = FormatString("StarClassImmortal")
	elseif GetHeroStarLvl(heroLvlStep) == 4 then
		type = FormatString("StarClassGod")
	elseif GetHeroStarLvl(heroLvlStep) == 5 then
		type = FormatString("StarClassSaint")
	elseif GetHeroStarLvl(heroLvlStep) == 1 or GetHeroStarLvl(heroLvlStep) == 0 then 
	    type = FormatString("StarClassMortal")
	end
	local data = GetGameData(DataFileHeroLevelStepGrow, heroLvlStep, "stLevelStepGrowData")
	if (type)then
		if (isSmall)then
			return FormatString("StarNameSmall",UI_GetStringColor(data.m_quality),type)
		else
			return FormatString("StarName",UI_GetStringColor(data.m_quality),type)
		end
	end
	
	return ""
end


function GetPlayerFightValue()
	local fightValue = 0;
	local formationList = GetLocalPlayer():GetFormationList();
	
	for i=0, formationList:size()-1 do
		local heroEntity = GetLocalPlayer():GetHeroByObjId(formationList[i]);
		if (heroEntity) then
			fightValue = fightValue + heroEntity:GetUint(EHero_FightValue);
		end
	end
	return fightValue;
end

function GetEntityByObjId(objId)
	if (GetLocalPlayer():GetUint(EPlayer_ObjId) == objId)then
		return GetLocalPlayer();
	else
		return GetLocalPlayer():GetHeroByObjId(objId);
	end
end

function OnAttChg(pkg)
	local tmp = GameServer_pb.Cmd_Sc_AttChg();
	tmp:ParseFromString(pkg)
	Log("tmp======="..tostring(tmp))
	for i=1, #tmp.szChgLifeAtt do	
		local attId = tmp.szChgLifeAtt[i].iLifeAttID;
		local value = tmp.szChgLifeAtt[i].iValue;
		Log("attId ="..attId)
		Log("value ="..value)
		local entity = GetEntityByObjId(tmp.dwObjectID);
		if (entity) then
		    if g_prompboxFloatSwitch == true then 				
				if (entity == GetLocalPlayer())then
                    if SceneMgr:GetInstance():GetGameState() ~= EGameState_Fighting then 		
					    if GetAddLiftAttValueAndName(attId,value) ~= "" then
					       createPromptBoxlayout(GetAddLiftAttValueAndName(attId,value))
					    end
                    end 
					UI_RefreshBaseWidgetByName("MainUi");		
					UI_RefreshBaseWidgetByName("Territory");
					UI_RefreshBaseWidgetByName("MyTerritory");
				end
		    end
				SetLifeAtt(entity, attId, value)
			if (entity == GetLocalPlayer())then		
				UI_RefreshBaseWidgetByName("MainUi");
				UI_RefreshBaseWidgetByName("Territory");
				UI_RefreshBaseWidgetByName("MyTerritory");
			end
		end
	end
end

function SetLifeAttFromC(entity, var2, var3)
	SetLifeAtt(entity, var2:Int(), var3:Int());
end

function SetLifeAtt(entity, attId, value)
	if (attId == GameServer_pb.en_LifeAtt_Silver) then
		entity:SetIntByName("Silver", value);
	elseif (attId == GameServer_pb.en_LifeAtt_Gold) then
		entity:SetIntByName("Gold", value);
	elseif (attId == GameServer_pb.en_LifeAtt_Exp) then
		entity:SetIntByName("Exp", value);
	elseif (attId == GameServer_pb.en_LifeAtt_HeroExp) then
		entity:SetIntByName("HeroExp", value);
	elseif (attId == GameServer_pb.en_LifeAtt_MaxHP) then
		entity:SetIntByName("HP", value);
	elseif (attId == GameServer_pb.en_LifeAtt_Att) then
		entity:SetIntByName("Att", value);
	elseif (attId == GameServer_pb.en_LifeAtt_Doge) then
		entity:SetIntByName("Doge", value);
	elseif (attId == GameServer_pb.en_LifeAtt_Hit) then
		entity:SetIntByName("Hit", value);
	elseif (attId == GameServer_pb.en_LifeAtt_AntiKnock) then
		entity:SetIntByName("AntiKnock", value);
	elseif (attId == GameServer_pb.en_LifeAtt_Knock) then
		entity:SetIntByName("Knock", value);
	elseif (attId == GameServer_pb.en_LifeAtt_Block) then
		entity:SetIntByName("Block", value);
	elseif (attId == GameServer_pb.en_LifeAtt_Wreck) then
		entity:SetIntByName("Wreck", value);
	elseif (attId == GameServer_pb.en_LifeAtt_Armor) then
		entity:SetIntByName("Armor", value);
	elseif (attId == GameServer_pb.en_LifeAtt_Sunder) then
		entity:SetIntByName("SunderArmor", value);
	elseif (attId == GameServer_pb.en_LifeAtt_InitAnger) then
		entity:SetIntByName("InitAnger", value);
	elseif (attId == GameServer_pb.en_LifeAtt_FSChipCount) then
		entity:SetIntByName("FSChipCount", value);
	elseif (attId == GameServer_pb.en_LifeAtt_HeroConvertCount) then
		entity:SetIntByName("HeroConvertCount", value);
	elseif (attId == GameServer_pb.en_LifeAtt_PhyStrength)then
		entity:SetIntByName("Tili", value);
		UI_RefreshBaseWidgetByName("DungeonList");	
		UI_RefreshBaseWidgetByName("Dungeon");	
	elseif (attId == GameServer_pb.en_LifeAtt_Honor) then
		entity:SetIntByName("Honor", value);
	elseif (attId == GameServer_pb.en_LifeAtt_PhyStrengthLimit) then
		entity:SetIntByName("TiliMax", value);
	elseif (attId == GameServer_pb.en_LifeAtt_VIPLevel) then
		entity:SetIntByName("VIP", value);
	elseif (attId == GameServer_pb.en_LifeAtt_FunctionMask) then
		entity:SetIntByName("FunctionMask", value);
	elseif (attId == GameServer_pb.en_LifeAtt_FightValue) then
		entity:SetIntByName("FightValue", value);
	elseif (attId == GameServer_pb.en_LifeAtt_HasFightSoul) then
		entity:SetIntByName("HasFightSoul",value)
	elseif (attId == GameServer_pb.en_LifeAtt_Notice) then
		entity:SetIntByName("FunctionNotice",value)
		--GetNoticeFlag()
		Log("FunctionNotice=="..value)
		NoticeFunction_UpdateRightButton()
		NoticeFunction_UpdateTopMainUiButton()
		NoticeFunction_UpdateMainUIButton()
		NoticeFunction_UpdateFirstPayButton()
		NoticeFunction_UpdateCheckIn()
        NoticeFunction_UpdateLegionButton()
	elseif (attId == GameServer_pb.en_LifeAtt_SkillDamage) then
		entity:SetIntByName("SkillDamage",value)
	elseif (attId == GameServer_pb.en_LifeAtt_Price) then 
		entity:SetIntByName("Price",value)
	elseif (attId == GameServer_pb.en_LifeAtt_HasEquip) then 
		entity:SetIntByName("HasEquip",value)
    elseif (attId == GameServer_pb.en_LifeAtt_actorhead) then 
        Log("value"..value)
		entity:SetIntByName("HeadId",value)
    elseif (attId == GameServer_pb.en_LifeAtt_Quality) then 
        entity:SetIntByName("Quality",value)
        Log("HeroQuality===="..value)
	elseif (attId == GameServer_pb.en_LifeAtt_actorVigor) then 
		entity:SetIntByName("Vigor",value)
    elseif (attId == GameServer_pb.en_LifeAtt_Def) then 
        entity:SetIntByName("Def",value)
    elseif (attId == GameServer_pb.en_LifeAtt_LeftPhyStrength) then 
        entity:SetIntByName("LeftPhyStrength",value)
    elseif (attId == GameServer_pb.en_LifeAtt_VIPEXP) then 
        local payWidget = UI_GetBaseWidgetByName("RechargeSystem")
        if payWidget then 
           RechargeSystem_VipInfo(payWidget)
           RechargeSystem_ShowLayout()
        end 
    elseif (attId == GameServer_pb.en_LifeAtt_KnockDamage) then 
        entity:SetIntByName("KnockDamage",value)
    elseif (attId == GameServer_pb.en_LifeAtt_KnockXiXue) then 
        entity:SetIntByName("KnockXiXue",value)
    elseif (attId == GameServer_pb.en_LifeAtt_ConAttackRate) then 
        entity:SetIntByName("ConAttackRate",value)
	end
	
--	local widget = UI_GetBaseWidgetByName("HeroSystemSecond")
--	if widget then
--		local heroID = entity:GetUint(EHero_HeroId)
--		if heroID > 0 then
--			HeroCurInfoSecond_HeroBaseInfoShow(widget, entity);
--		end
--	end
end

function SetExtraLifeAttFromC(entity, var2, var3)
	SetExtraLifeAtt(entity, var2:Int(), var3:Int());
end

function SetExtraLifeAtt(entity, attId, value)
	if (attId == GameServer_pb.en_LifeAtt_Silver) then
		entity:SetExtraInt("Silver", value);
	elseif (attId == GameServer_pb.en_LifeAtt_Gold) then
		entity:SetExtraInt("Gold", value);
	elseif (attId == GameServer_pb.en_LifeAtt_Exp) then
		entity:SetExtraInt("Exp", value);
	elseif (attId == GameServer_pb.en_LifeAtt_HeroExp) then
		entity:SetExtraInt("HeroExp", value);
	elseif (attId == GameServer_pb.en_LifeAtt_MaxHP) then
		entity:SetExtraInt("HP", value);
	elseif (attId == GameServer_pb.en_LifeAtt_Att) then
		entity:SetExtraInt("Att", value);
	elseif (attId == GameServer_pb.en_LifeAtt_Doge) then
		entity:SetExtraInt("Doge", value);
	elseif (attId == GameServer_pb.en_LifeAtt_Hit) then
		entity:SetExtraInt("Hit", value);
	elseif (attId == GameServer_pb.en_LifeAtt_AntiKnock) then
		entity:SetExtraInt("AntiKnock", value);
	elseif (attId == GameServer_pb.en_LifeAtt_Knock) then
		entity:SetExtraInt("Knock", value);
	elseif (attId == GameServer_pb.en_LifeAtt_Block) then
		entity:SetExtraInt("Block", value);
	elseif (attId == GameServer_pb.en_LifeAtt_Wreck) then
		entity:SetExtraInt("Wreck", value);
	elseif (attId == GameServer_pb.en_LifeAtt_Armor) then
		entity:SetExtraInt("Armor", value);
	elseif (attId == GameServer_pb.en_LifeAtt_Sunder) then
		entity:SetExtraInt("SunderArmor", value);
	elseif (attId == GameServer_pb.en_LifeAtt_InitAnger) then
		entity:SetExtraInt("InitAnger", value);
	elseif (attId == GameServer_pb.en_LifeAtt_FSChipCount) then
		entity:SetExtraInt("FSChipCount", value);
	elseif (attId == GameServer_pb.en_LifeAtt_HeroConvertCount) then
		entity:SetExtraInt("HeroConvertCount", value);
	elseif (attId == GameServer_pb.en_LifeAtt_PhyStrength)then
		entity:SetExtraInt("Tili", value);
	elseif (attId == GameServer_pb.en_LifeAtt_Honor) then
		entity:SetExtraInt("Honor", value);
	elseif (attId == GameServer_pb.en_LifeAtt_PhyStrengthLimit) then
		entity:SetExtraInt("TiliMax", value);
	elseif (attId == GameServer_pb.en_LifeAtt_VIPLevel) then
		entity:SetExtraInt("VIP", value);
	elseif (attId == GameServer_pb.en_LifeAtt_FunctionMask) then
		entity:SetExtraInt("FunctionMask", value);
	elseif (attId == GameServer_pb.en_LifeAtt_FightValue) then
		entity:SetExtraInt("FightValue", value);
	elseif (attId == GameServer_pb.en_LifeAtt_HasFightSoul) then
		entity:SetExtraInt("HasFightSoul",value)
	elseif (attId == GameServer_pb.en_LifeAtt_Notice) then
		entity:SetExtraInt("FunctionNotice",value)
		--GetNoticeFlag()
	elseif (attId == GameServer_pb.en_LifeAtt_SkillDamage) then
		entity:SetExtraInt("SkillDamage",value)
    elseif (attId == GameServer_pb.en_LifeAtt_Quality) then 
        entity:SetExtraInt("HeroQuality",value)
	end
end

ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_ATT_CHG, "OnAttChg" );