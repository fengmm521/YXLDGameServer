function FightSoul_Create()
	local widget = UI_CreateBaseWidgetByFileName("FightSoul.json");
	if (widget == nil)then
		Log("FightSoul_Create error");
		return;
	end
		
	--if (g_selectHero == nil)then
	g_selectHero = GetEntityById(GetLocalPlayer():GetHeroList()[0], "Hero");
	if (g_selectHero:GetExtraBool("IsQueryFightSoul") == false) then
		local tab = GameServer_pb.Cmd_Cs_QueryHeroFightSoul();
		tab.dwHeroObjectID = g_selectHero:GetUint(EHero_ID);
		Packet_Full(GameServer_pb.CMD_QUERY_HEROFIGHTSOUL, tab);
	end
	
	g_selectNode = nil;
	
	local function ClickExchange(sender,eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			FightSoulExchange_Create()
		end
	end
	UI_GetUIButton(widget:getChildByTag(1), 9):addTouchEventListener(ClickExchange);
	local function ClickAdvanceMode(sender,eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			Packet_Cmd(GameServer_pb.CMD_FIGHTSOUL_ADVANCEMODE)
			ShowWaiting();
		end
	end
	UI_GetUIButton(widget:getChildByTag(1), 6):addTouchEventListener(ClickAdvanceMode);
	
	local function ClickAutoCombine(sender,eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			Log("ClickAutoCombine")
			Packet_Cmd(GameServer_pb.CMD_FIGHTSOUL_AUTOCOMBINE);
			ShowWaiting();
		end
	end
	UI_GetUIButton(widget:getChildByTag(1), 100):addTouchEventListener(ClickAutoCombine);
	
	local function ClickTab(sender,eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			local sender = tolua.cast(sender, "CCNode");

			FightSoul_Chg(widget, sender:getTag());
			
			if (g_isGuide) then
				Guide_GoNext();
			end
		end
	end
	UI_GetUIButton(widget, 11):addTouchEventListener(ClickTab);
	
	--[[local function ClickHeroFightSoul(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			g_SelectIndex = 0
            UI_CloseAllBaseWidget()
		
			local tab = GameServer_pb.Cmd_Cs_GetHeroDesc();
		    local heroEntity = GetEntityById(GetLocalPlayer():GetHeroList()[0], "Hero");
			tab.dwObjectID = heroEntity:GetUint(EHero_ID);
			Packet_Full(GameServer_pb.CMD_GET_HERODESC, tab);	
			
			local tab = GameServer_pb.Cmd_Cs_QueryHeroFightSoul();
			tab.dwHeroObjectID = heroEntity:GetUint(EHero_ID);
			Packet_Full(GameServer_pb.CMD_QUERY_HEROFIGHTSOUL, tab);
			ShowWaiting();
		end
	end]]
	UI_GetUIButton(widget, 12):addTouchEventListener(ClickTab);
	
	local closeButton = UI_GetUIButton(widget, 4);
	closeButton:addTouchEventListener(UI_ClickCloseAllBaseWidget);
	
	local heroList = GetLocalPlayer():GetHeroList(true);
	
	FightSoul_Chg(widget, 11);
end

if (g_selectNode == nil) then
	g_selectNode = nil;
end

function FightSoul_Chg(widget, tag)
	UI_GetUIButton(widget, 11):setBright(tag == 12)
	UI_GetUIButton(widget, 12):setBright(tag == 11)
	UI_GetUILayout(widget, 1):setVisible(tag == 11)
	UI_GetUILayout(widget, 2):setVisible(tag == 12)
	
	FightSoul_Refresh(widget);
end

function FightSoul_Refresh(widget)
	EndWaiting();
	g_isRevPractice = true;
	
	local tiliButton = UI_GetUIButton(widget, 23);
	tiliButton:addTouchEventListener(ClickBuyTili);
	UI_SetLabelText(widget, 20, UI_GetMoneyStr(GetLocalPlayer():GetUint(EPlayer_Gold)));
	UI_SetLabelText(widget, 21, UI_GetMoneyStr(GetLocalPlayer():GetUint(EPlayer_Silver)));
	UI_SetLabelText(widget, 22, ""..GetLocalPlayer():GetUint(EPlayer_Tili).."/"..GetLocalPlayer():GetUint(EPlayer_TiliMax));
	
	local fightLayout = UI_GetUILayout(widget, 1);
	local heroLayout = UI_GetUILayout(widget, 2);

	local heroList = GetLocalPlayer():GetHeroList(false);
	local wearList = g_selectHero:GetFightSoulBags():GetFightSoulList();
	local bagList = GetLocalPlayer():GetFightSoulBags():GetFightSoulList();
	local fightSoulBagIconIndex = -1;
	local fightSoulWearIconIndex = -1;
	
	local function ClickHeroIcon(sender,eventType)
		local sender = tolua.cast(sender, "ImageView");		
		if (eventType == TOUCH_EVENT_ENDED) then
			local hero = GetEntityById(heroList[sender:getTag()], "Hero");
			if (hero ~= g_selectHero)then
				Log("hero ="..hero:GetEntityName())
				g_selectHero = hero
				if (g_selectNode)then
					g_selectNode:removeChildByTag(10000)
				end
				local frame = ImageView:create();
				frame:loadTexture("hero/light.png")
				sender:addChild(frame, 10, 10000)
				g_selectNode = sender;
				if (g_selectHero:GetExtraBool("IsQueryFightSoul")) then
					FightSoul_Refresh(widget)
				else
					local tab = GameServer_pb.Cmd_Cs_QueryHeroFightSoul();
					tab.dwHeroObjectID = g_selectHero:GetUint(EHero_ID);
					Packet_Full(GameServer_pb.CMD_QUERY_HEROFIGHTSOUL, tab);
					ShowWaiting();
				end
			end
		end
	end
	
	local function ClickWearIcon(sender,eventType) 
		local sender = tolua.cast(sender, "Button");		
		local tag = sender:getTag() - 1;
		
		if (g_selectHero:GetInt(EHero_Lvl) <= g_fightSoulLimitLvl[tag+1])then
			return;
		end
		if (eventType == TOUCH_EVENT_BEGAN) then
			if (wearList[tag])then
				fightSoulWearIconIndex = tag;
				fightSoulBagIconIndex = -1;
				local node = FightSoul_GetNode(wearList[tag]:GetData().m_quality)
				node:setScale(1.1*MainScene:GetInstance().m_minScale);

				local uiLayer = tolua.cast(widget:getParent():getParent(), "TouchGroup")
				uiLayer:SetDragNode(node);
			end
		elseif (eventType == TOUCH_EVENT_ENDED) then
			if (fightSoulWearIconIndex ~= -1 and fightSoulWearIconIndex ~= tag)then		
				
			elseif (fightSoulBagIconIndex ~= -1)then
				if (bagList[fightSoulBagIconIndex])then
					local tab = GameServer_pb.Cmd_Cs_MoveFightSoul();
					tab.dwHeroObjectID = g_selectHero:GetUint(EHero_ID);
					tab.iSrcConType = GameServer_pb.en_FSConType_Bag;	
					tab.iDstConType = GameServer_pb.en_FSConType_Wear;	
					tab.iSrcPos = fightSoulBagIconIndex;	
					tab.iDstPos = tag;	
					Packet_Full(GameServer_pb.CMD_MOVE_FIGHTSOUL, tab);
					ShowWaiting();
				end
			else
				if (wearList[tag])then
					FightSoul_Info(wearList[tag], GameServer_pb.en_FSConType_Wear, g_selectHero:GetUint(EHero_ID), tag)
				end
			end
			fightSoulBagIconIndex = -1;
			fightSoulWearIconIndex = -1;
		end
	end 
	
	local function ClickBagIcon(sender,eventType) 
		local sender = tolua.cast(sender, "ImageView");		
		local tag = sender:getTag() - 1;
		if (eventType == TOUCH_EVENT_BEGAN) then
			if (bagList[tag])then
				local node = FightSoul_GetNode(bagList[tag]:GetData().m_quality)
				node:setScale(1.1*MainScene:GetInstance().m_minScale);

				local uiLayer = tolua.cast(widget:getParent():getParent(), "TouchGroup")
				uiLayer:SetDragNode(node);	
				
				fightSoulWearIconIndex = -1;
				fightSoulBagIconIndex = tag;
			end
		elseif (eventType == TOUCH_EVENT_ENDED) then
			if (fightSoulBagIconIndex ~= -1 and fightSoulBagIconIndex ~= tag)then		
				if (bagList[fightSoulBagIconIndex])then
					local tab = GameServer_pb.Cmd_Cs_MoveFightSoul();
					tab.dwHeroObjectID = 0;
					tab.iSrcConType = GameServer_pb.en_FSConType_Bag;	
					tab.iDstConType = GameServer_pb.en_FSConType_Bag;	
					tab.iSrcPos = fightSoulBagIconIndex;	
					tab.iDstPos = tag;	
					Packet_Full(GameServer_pb.CMD_MOVE_FIGHTSOUL, tab);
					ShowWaiting();
				end
			elseif (fightSoulWearIconIndex ~= -1)then
				if (wearList[fightSoulWearIconIndex])then
					local tab = GameServer_pb.Cmd_Cs_MoveFightSoul();
					tab.dwHeroObjectID = g_selectHero:GetUint(EHero_ID);
					tab.iSrcConType = GameServer_pb.en_FSConType_Wear;	
					tab.iDstConType = GameServer_pb.en_FSConType_Bag;	
					tab.iSrcPos = fightSoulWearIconIndex;	
					tab.iDstPos = tag;	
					Packet_Full(GameServer_pb.CMD_MOVE_FIGHTSOUL, tab);
					ShowWaiting();
				end
			else
				if (bagList[tag])then
					FightSoul_Info(bagList[tag], GameServer_pb.en_FSConType_Bag, 0, tag)
				end
			end
			fightSoulBagIconIndex = -1;
			fightSoulWearIconIndex = -1;
		end
	end 
	
	local listView = UI_GetUIListView(heroLayout, 11);
	--listView:addEventListenerListView(ClickHeroIcon);
	listView:removeAllItems();
	
	local tempLayout = Layout:create();
    --tempLayout:setTouchEnabled(true);
	tempLayout:setSize(CCSizeMake(100, 100));
	listView:setItemModel(tempLayout);
	
	for i=0, heroList:size()-1 do
		local heroEntity = GetEntityById(heroList[i], "Hero");
		if heroEntity == nil then
			Log("heroEntity = nil")
		else
			local data = heroEntity:GetHeroData(); 
			GetGameData(DataFileHeroBorn, heroEntity:GetUint(EHero_HeroId), "stHeroBornData");
			local layout = UI_GetCloneLayout(tempLayout);
			
			local icon = UI_IconFrame(UI_GetHeroIconByHero(heroEntity), heroEntity:GetInt(EHero_LvlStep));
			icon:setPosition(ccp(50, 50))
			icon:setTouchEnabled(true);
			icon:addTouchEventListener(ClickHeroIcon);
			layout:addChild(icon, i, i);
			
			if (GetLocalPlayer():IsInFormation(heroEntity))then
				local formationIcon = ImageView:create();
				formationIcon:loadTexture("buzhen/bg_1.png")
				formationIcon:setPosition(ccp(icon:getSize().width/2 - 17, icon:getSize().height/2 - 17));
				icon:addChild(formationIcon);
			end
			
			if (g_selectHero == heroEntity)then
				local frame = ImageView:create();
				frame:loadTexture("hero/light.png")
				icon:addChild(frame, 10, 10000)
				g_selectNode = icon;
			end

			listView:pushBackCustomItem(layout);
		end
	end
	
	local layout = nil;
	for i=0, wearList:size()-1 do
		local button = UI_GetUIButton(heroLayout, i+1)
		button:removeAllChildren();
		button:removeAllNodes();
		if wearList[i] ~= nil then	
			local node = FightSoul_GetNodeWithName(wearList[i])
			--node:setPosition(ccp(40, 40))	
			button:addChild(node);
		end
		if (g_selectHero:GetInt(EHero_Lvl) > g_fightSoulLimitLvl[i+1]) then
			button:setBright(true) 
		else
			local lvlLabel = CompLabel:GetDefaultLabScript(FormatString("Arena_Level")..g_fightSoulLimitLvl[i+1] + 1, 20);
			lvlLabel:setPosition(ccp(0, -40));
			lvlLabel:setColor(ccc3(255, 0, 0));
			button:addChild(lvlLabel)
			button:setBright(false) 
		end
		button:addTouchEventListener(ClickWearIcon)
	end
	
	local data = g_selectHero:GetHeroData(); 
	local armature = GetArmature(data.m_animName)	
	--armature:setPosition(ccp(60, 20));
	local array = CCArray:create()
	for j=1, 3 do
		array:addObject(CCInteger:create(0));
	end
	array:addObject(CCInteger:create(2));

	armature:getAnimation():playWithIndexArray(array)
	UI_GetUIImageView(heroLayout, 10):removeAllNodes()
	UI_GetUIImageView(heroLayout, 10):addNode(armature)
	
	local nameLab = CompLabel:GetCompLabel(GetHeroName(data.m_name, g_selectHero:GetInt(EHero_LvlStep)), 192, kCCTextAlignmentLeft)
    nameLab:setAnchorPoint(ccp(0.5,0.5))
	UI_GetUIImageView(heroLayout, 9):removeAllChildren()
    UI_GetUIImageView(heroLayout, 9):addChild(nameLab)
	
	local function ClickPractice(sender,eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			Log("ClickPractice")
			g_isRevPractice = false;
			Packet_Cmd(GameServer_pb.CMD_FIGHTSOUL_PRACTICE);
			
			if (g_isGuide) then
				Guide_GoNext();
			end
		end
	end
	
	for i=1, 5 do
		local button = UI_GetUIButton(fightLayout, i)
		if (g_fightSoulPracticeStatus.iGrade == i)then
			button:setTouchEnabled(true)
			button:setBright(true) 
			--if (g_selectNode)then
				--g_selectNode:removeNodeByTag(9999);
			--end
			if (g_fightSoulPracticeStatus.iMode == GameServer_pb.en_practice_AdvanceMode) then
				--EffectMgr:AddButtonPromotToNode( button, 10, 9999);
				--g_selectNode = button
				UI_GetUIButton(fightLayout, 6):setVisible(false);
			else
				UI_GetUIButton(fightLayout, 6):setVisible(true);
			end
		else
			button:setTouchEnabled(false)
			button:setBright(false) 
		end
		button:addTouchEventListener(ClickPractice)
	end
	UI_GetUIButton(fightLayout, 7):addTouchEventListener(ClickPractice);
	local index = 1
	for i=0, bagList:size()-1 do
		local img = UI_GetUIImageView(widget:getChildByTag(100), i+1)
		img:removeAllChildren();
		img:removeAllNodes();
		if bagList[i] ~= nil then
			local node = FightSoul_GetNodeWithName(bagList[i], true)
			--node:setPosition(ccp(40, 40))
			img:addChild(node);
			
			if (bagList[i]:GetExtraBool("New") == true)then
				bagList[i]:SetExtraBool("New", false)
				UIMgr:GetInstance():PlayOpenAction(node, EUIOpenAction_FadeIn, index*0.3)
				index = index + 1;
			end		
		end
		img:addTouchEventListener(ClickBagIcon)
	end	
	
	UI_SetLabelText(fightLayout, 11, UI_GetMoneyStr(GetLocalPlayer():GetUint(EPlayer_Silver)));
	UI_SetLabelText(fightLayout, 10, UI_GetMoneyStr(GetLocalPlayer():GetUint(EPlayer_Gold)));
	UI_SetLabelText(fightLayout, 12, UI_GetMoneyStr(GetLocalPlayer():GetInt(EPlayer_FSChipCount)));
	UI_SetLabelText(fightLayout, 13, UI_GetMoneyStr(g_fightSoulCostMoney[g_fightSoulPracticeStatus.iGrade]));
	
	local function ClickAutoPractice(sender,eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			--[[if (g_autoPractice)then
				--g_autoPractice = false;
				--GetGlobalEntity():GetScheduler():UnRegisterScript( "FightSoul_AutoPractice" )
			else
				g_autoPractice = true;
				GetGlobalEntity():GetScheduler():RegisterInfiniteScript( "FightSoul_AutoPractice", 200 )
				UI_GetUIButton(fightLayout, 8):setButtonEnabled(false)
			end]]
			Packet_Cmd(GameServer_pb.CMD_FIGHTSOUL_AUTOPRACTICE)
			ShowWaiting(15, false);
		end
	end
	
	UI_GetUIButton(fightLayout, 8):setButtonEnabled(g_autoPractice == false)
	UI_GetUIButton(fightLayout, 8):addTouchEventListener(ClickAutoPractice);
end

if (g_autoPractice == nil) then
	g_autoPractice = false;
end

if (g_isRevPractice == nil)then
	g_isRevPractice = false;
end

if (g_selectHero == nil) then
	g_selectHero = nil;
end

function FightSoulExchange_Create()
	local widget = UI_CreateBaseWidgetByFileName("FightSoulExchange.json");
	if (widget == nil)then
		Log("FightSoulExchange_Create error");
		return;
	end
	
	UI_GetUIButton(widget, 1):addTouchEventListener(UI_ClickCloseCurBaseWidget);
	
	local listView = UI_GetUIListView(widget, 2);

	local exchangeList = CDataManager:GetInstance():GetGameDataKeyList(DataFileFightSoulExchange);
	
	local function ClickExchange(sender,eventType)
		local sender = tolua.cast(sender, "CCNode");		
		local tag = sender:getTag();
		if (eventType == TOUCH_EVENT_ENDED) then
			local tab = GameServer_pb.Cmd_Cs_FightSoulExchange();
		
			tab.iExchangeID = exchangeList[tag];
			Log("tab.iExchangeID"..tab.iExchangeID)
			Packet_Full(GameServer_pb.CMD_FIGHTSOUL_EXCHANGE, tab);
			ShowWaiting();
		end
	end
	
	for i=0, exchangeList:size()-1 do
		local exchangeData = GetGameData(DataFileFightSoulExchange, exchangeList[i], "stFightSoulExchangeData");
		local fightSoulData = GetGameData(DataFileFightSoul, exchangeData.m_fightSoulID, "stFightSoulData");
		local layout = UI_GetCloneImageView(UI_GetUIImageView(widget, 4));

		local nameLab = CompLabel:GetCompLabel(FormatString("FightSoulName_1", UI_GetStringColorNew(fightSoulData.m_quality), fightSoulData.m_name), 140, kCCTextAlignmentLeft);
		nameLab:setPosition(ccp(-nameLab:getSize().width/2,49));
		layout:addChild(nameLab);
		
		local node = FightSoul_GetNode(fightSoulData.m_quality);
		UI_GetUIImageView(layout, 10000):addChild(node);
		
		UI_GetUILabel(layout, 10002):setText(""..exchangeData.m_count);
	
		local descLab = CompLabel:GetCompLabel(FormatString("FightSoulDesc", exchangeData.m_desc), 140);
		--stuntNameLab:setText(FormatString("stunt", data.m_stuntName));
		descLab:setPosition(ccp(-70, -35));
		layout:addChild(descLab);
		
		local button = UI_GetUIButton(layout, 10001);
		button:addTouchEventListener(ClickExchange)
		button:removeFromParent();
		layout:addChild(button, i, i);
		listView:pushBackCustomItem(layout);
	end
	
	
	UI_GetUIImageView(widget, 4):setVisible(false);
end

function FightSoul_Info(fightSoul, bagType, objId, pos)
	EndWaiting();
	local widget = UI_CreateBaseWidgetByFileName("FightSoulInfo.json");
	if (widget == nil)then
		Log("FightSoul_Info error");
		return;
	end
	
	local data = fightSoul:GetData();
	local layout = UI_GetUILayout(widget, 1);

	local nameLab = CompLabel:GetCompLabel(FormatString("FightSoulName", UI_GetStringColorNew(data.m_quality), data.m_name, fightSoul:GetInt(EFightSoul_Lvl)), 260, kCCTextAlignmentLeft);
	--nameLab:setPosition(ccp(20,280));
	--nameLab:setAnchorPoint(0, 0.5)
	UI_GetUILabel(widget, 3):addChild(nameLab);
	
	UI_GetUILabel(widget, 4):setText(FormatString("FightSoulExp", fightSoul:GetInt(EFightSoul_Exp), fightSoul:GetInt(EFightSoul_NextLvExp)))

	local maker = CompStringMaker:new_local();
	if (fightSoul:GetData().m_ID == 42)then
		maker:AppendString(FormatString("FightSoulInfo", fightSoul:GetExtraInt("DevourExp")), CompTextArgs:new_local(255,193,0));
	else
		for i=EFightSoul_HP, EFightSoul_SunderArmor do
			if (fightSoul:GetInt(i) > 0)then
				maker:AppendString(FightSoul_GetAttName(i).." +"..GetAttShowValue(i,fightSoul:GetInt(i)), CompTextArgs:new_local(255,193,0));
				if (i ~= EFightSoul_SunderArmor)then
					maker:AppendChangeLine();
				end
			end
		end
	end
	
	local attLab = CompLabel:GetCompLabel(maker:GetString(), 260, kCCTextAlignmentLeft);
	--attLab:setPosition(ccp(20,200));
	--attLab:setAnchorPoint(0, 0.5)
	UI_GetUILabel(widget, 5):addChild(attLab);
	
	
	local function ClickLock(sender,eventType) 
		if (eventType == TOUCH_EVENT_ENDED) then
			local tab = GameServer_pb.Cmd_Cs_FightSoulLock();
			tab.iConType = bagType;
			tab.iPos = pos;	
			tab.dwHeroObjectID = objId;
			if (fightSoul:GetBool(EFightSoul_Lock))then
				tab.bLock = false;
			else
				tab.bLock = true;
			end
			Packet_Full(GameServer_pb.CMD_FIGHTSOUL_LOCK, tab);
			ShowWaiting();
		end
	end 
	
	local lockButton = UI_GetUIButton(widget, 6);
	lockButton:addTouchEventListener(ClickLock);
	
	if (fightSoul:GetBool(EFightSoul_Lock))then
		lockButton:setTitleText(FormatString("jiesuo"))
	else
		lockButton:setTitleText(FormatString("suoding"))
	end
	
	lockButton:setVisible(bagType == GameServer_pb.en_FSConType_Bag);
	
	g_selectFightSoul = fightSoul;
	local closeButton = UI_GetUIButton(widget, 1);
	closeButton:addTouchEventListener(UI_ClickCloseCurBaseWidget);
	
	UI_GetUIImageView(widget, 2):addChild(FightSoul_GetNode(data.m_quality))
	
	widget:addTouchEventListener(UI_ClickCloseCurBaseWidget);
end

if (g_selectFightSoul == nil)then
	g_selectFightSoul = nil
end

function FightSoul_GetAttName(attId)
	if (attId == EFightSoul_HP) then
		return FormatString("en_LifeAtt_MaxHP");
	elseif (attId == EFightSoul_Att) then
		return FormatString("en_LifeAtt_Att");
	elseif (attId == EFightSoul_Doge) then
		return FormatString("en_LifeAtt_Doge");
	elseif (attId == EFightSoul_Hit) then
		return FormatString("en_LifeAtt_Hit");
	elseif (attId == EFightSoul_AntiKnock) then
		return FormatString("en_LifeAtt_AntiKnock");
	elseif (attId == EFightSoul_Knock) then
		return FormatString("en_LifeAtt_Knock");
	elseif (attId == EFightSoul_Block) then
		return FormatString("en_LifeAtt_Block");
	elseif (attId == EFightSoul_Wreck) then
		return FormatString("en_LifeAtt_Wreck");
	elseif (attId == EFightSoul_Armor) then
		return FormatString("en_LifeAtt_Armor");
	elseif (attId == EFightSoul_SunderArmor) then
		return FormatString("en_LifeAtt_Sunder");
	elseif (attId == EFightSoul_InitAnger) then
		return FormatString("en_LifeAtt_InitAnger");
	end
end

function FightSoulInfo_Refresh(widget)
	local lockButton = UI_GetUIButton(widget, 6);
	
	if (g_selectFightSoul:GetBool(EFightSoul_Lock))then
		lockButton:setTitleText(FormatString("jiesuo"))
	else
		lockButton:setTitleText(FormatString("suoding"))
	end
end

function FightSoul_GetNode(quality)
	local layout = Layout:create()
	layout:setAnchorPoint(ccp(0.5, 0.5));
	local wuhun = GetUIArmature("Effwuhun_"..(quality - 1))
	wuhun:getAnimation():playWithIndex(0)
	layout:addNode(wuhun);
	return layout;	
end

function FightSoul_GetNodeWithName(fightSoul, isNeedLockIcon)
	local data = fightSoul:GetData();
	
	local layout = Layout:create()
	layout:setAnchorPoint(ccp(0.5, 0.5));
	local wuhun = GetUIArmature("Effwuhun_"..(data.m_quality - 1))
	wuhun:getAnimation():playWithIndex(0)
	layout:addNode(wuhun);
	
	if (isNeedLockIcon == true and fightSoul:GetBool(EFightSoul_Lock))then
		local img = ImageView:create()
		img:setPosition(ccp(25, 25))
		img:loadTexture("wuhun/ico_2.png");
		layout:addChild(img);
	end
	local nameLab = CompLabel:GetCompLabel(FormatString("FightSoulName_2", UI_GetStringColorNew(data.m_quality), data.m_name, fightSoul:GetInt(EFightSoul_Lvl)), 80, kCCTextAlignmentLeft);
	nameLab:setPosition(ccp(-40,-25));
	layout:addChild(nameLab)
	
	return layout;	
end

if (g_fightSoulPracticeStatus == nil)then
	g_fightSoulPracticeStatus = {}
end

function GetFightSoulBag(pkg)
	local tmp = GameServer_pb.Cmd_Sc_FightSoulBag();
	tmp:ParseFromString(pkg)
	
	Log(tostring(tmp))
	g_fightSoulPracticeStatus = tmp.status;
end

function GetFightSoulPractice(pkg)
	local tmp = GameServer_pb.Cmd_Sc_FightSoulPractice();
	tmp:ParseFromString(pkg)
	
	g_fightSoulPracticeStatus = tmp.status;
	
	UI_RefreshBaseWidgetByName("FightSoul");
end

function FightSoul_AutoPractice()
	if (g_autoPractice--[[ and g_fightSoulPracticeStatus.iGrade ~= GameServer_pb.en_practice_Grade5]])then
		if (g_isRevPractice) then
			g_isRevPractice = false;
			Packet_Cmd(GameServer_pb.CMD_FIGHTSOUL_PRACTICE);
		end
	else
		g_autoPractice = false;
		GetGlobalEntity():GetScheduler():UnRegisterScript( "FightSoul_AutoPractice" )
	end
	
	UI_RefreshBaseWidgetByName("FightSoul");
end

function QUERY_FIGHTSOUL(pkg)
	FightSoul_Create();
end

function FightSoul_PracticeFail(pkg)
	g_autoPractice = false;
	UI_RefreshBaseWidgetByName("FightSoul");
end

ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_QUERY_FIGHTSOUL, "QUERY_FIGHTSOUL" );
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_FIGHTSOUL_PRACTICEFAIL, "FightSoul_PracticeFail" );
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_FIGHTSOUL_BAG, "GetFightSoulBag" );
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_FIGHTSOUL_PRACTICE, "GetFightSoulPractice" );