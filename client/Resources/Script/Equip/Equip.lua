--[[function Equip_Create()
	local widget = UI_CreateBaseWidgetByFileName("Equip.json");
	if (widget == nil)then
		Log("Equip_Create error");
		return;
	end
		
	--if (g_selectHero == nil)then
	g_selectHero = GetEntityById(GetLocalPlayer():GetHeroList()[0], "Hero");
	if (g_selectHero:GetExtraBool("IsQueryEquip") == false) then
		local tab = GameServer_pb.CMD_QUERY_HEROEQUIP_CS();
		tab.dwHeroObjectID = g_selectHero:GetUint(EHero_ID);
		Packet_Full(GameServer_pb.CMD_QUERY_HEROEQUIP, tab);
	end
	
	g_selectNode = nil;
	
	local bagList = vector_Equip__:new_local()
	GetLocalPlayer():GetEquipBags():GetEquipList(bagList);
	
	local function ClickAutoSell(sender,eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			EquipSell_Create();
		end
	end
	UI_GetUIButton(widget, 5):addTouchEventListener(ClickAutoSell);
	
	local function ClickUpStep(sender,eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			EquipUpStage_Create();
		end
	end
	UI_GetUIButton(widget, 6):addTouchEventListener(ClickUpStep);
	
	local closeButton = UI_GetUIButton(widget, 4);
	closeButton:addTouchEventListener(UI_ClickCloseCurBaseWidget);
	
	Equip_Refresh(widget);
	
	UIMgr:GetInstance():PlayOpenAction(widget, EUIOpenAction_MoveIn_Right, 0.5);
end
]]
function Equip_Wear(equip)
	local tab = GameServer_pb.CMD_EQUIP_MOVE_CS();

	tab.dwSrcObjectID = GetLocalPlayer():GetInt(EPlayer_ObjId);
	tab.dwDstObjectID = g_selectHero:GetUint(EHero_ID);	
	
	tab.iSrcPos = equip:GetInt(EEquip_Pos);	
	tab.iDstPos = -1;	
	Packet_Full(GameServer_pb.CMD_EQUIP_MOVE, tab);
	ShowWaiting();
end
--[[
function Equip_Refresh(widget)
	EndWaiting();
	
	local tiliButton = UI_GetUIButton(widget, 23);
	tiliButton:addTouchEventListener(ClickBuyTili);
	UI_SetLabelText(widget, 20, UI_GetMoneyStr(GetLocalPlayer():GetUint(EPlayer_Gold)));
	UI_SetLabelText(widget, 21, UI_GetMoneyStr(GetLocalPlayer():GetUint(EPlayer_Silver)));
	UI_SetLabelText(widget, 22, ""..GetLocalPlayer():GetUint(EPlayer_Tili).."/"..GetLocalPlayer():GetUint(EPlayer_TiliMax));

	local heroLayout = UI_GetUILayout(widget, 2);

	local heroList = GetLocalPlayer():GetHeroList();
	local wearList = g_selectHero:GetEquipBags():GetEquipList();
	local bagList = vector_Equip__:new_local()
	GetLocalPlayer():GetEquipBags():GetEquipList(bagList);
	local equipBagInconIndex = -1;
	local equiplWearIconIndex = -1;
	
	local function ClickHeroIcon(sender,eventType)
		local sender = tolua.cast(sender, "ImageView");		
		if (eventType == TOUCH_EVENT_ENDED) then
			local hero = GetEntityById(heroList[sender:getTag()], "Hero");
			if (hero ~= g_selectHero)then
				Log("hero ="..hero:GetEntityName())
				g_selectHero = hero
				
				g_lightFrame:removeFromParent()		
				sender:addChild(g_lightFrame, 10, 10000)

				if (g_selectHero:GetExtraBool("IsQueryEquip")) then
					Equip_Refresh(widget)
				else
					local tab = GameServer_pb.CMD_QUERY_HEROEQUIP_CS();
					tab.dwHeroObjectID = g_selectHero:GetUint(EHero_ID);
					Packet_Full(GameServer_pb.CMD_QUERY_HEROEQUIP, tab);
					ShowWaiting();
				end
			end
		end
	end
	
	local function ClickWearIcon(sender,eventType) 
		local sender = tolua.cast(sender, "ImageView");		
		local tag = sender:getTag() - 1;
		
		if (eventType == TOUCH_EVENT_BEGAN) then
			
		elseif (eventType == TOUCH_EVENT_ENDED) then
			
				if (wearList[tag])then
					Equip_Info(wearList[tag], GameServer_pb.en_EquipContainer_HeroWear)
				else
					EquipSelect_Create(tag, Equip_Wear)
				end

			equipBagInconIndex = -1;
			equiplWearIconIndex = -1;
		end
	end 
	
	local function ClickBagIcon(sender,eventType) 
		local sender = tolua.cast(sender, "ImageView");		
		local tag = sender:getTag() - 1;
		if (eventType == TOUCH_EVENT_ENDED) then
			
				if (bagList[tag])then
					Equip_Info(bagList[tag], GameServer_pb.en_EquipContainer_ActorBag)
				end
			equipBagInconIndex = -1;
			equiplWearIconIndex = -1;
		end
	end 
	
	local listView = UI_GetUIListView(widget, 1);
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
				g_lightFrame:removeFromParent()		
				icon:addChild(g_lightFrame, 10, 10000)
			end

			listView:pushBackCustomItem(layout);
		end
	end
	
	for i=0, wearList:size()-1 do
		local button = UI_GetUIImageView(heroLayout, i+1)
		button:setTouchEnabled(true);
		button:removeChildByTag(1);

		if wearList[i] ~= nil then	
			local node = UI_ItemIconFrame(UI_GetItemIcon(wearList[i]:GetData().m_icon, wearList[i]:GetData().m_step, true), wearList[i]:GetData().m_quality)
			--node:setPosition(ccp(40, 40))	
			button:addChild(node, 1, 1);
		else
			local frame = ImageView:create();
			frame:loadTexture("Common/Icon_Bg_001.png")
			button:addChild(frame, 1, 1);
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
	
	UI_GetUILabelBMFont(heroLayout, 7):setText(g_selectHero:GetUint(EHero_FightValue))
	
	local bagListView = UI_GetUIListView(widget:getChildByTag(100), 1);
	bagListView:setItemModel(tempLayout);
	local index = 0;
	bagListView:removeAllItems();
	for i=0, bagList:size()-1 do
		local j = i % 4;
		if (j == 0)then
			layout = UI_GetCloneLayout(tempLayout);
			bagListView:pushBackCustomItem(layout);
		end
		local img = ImageView:create();
		img:setTouchEnabled(true);
		img:loadTexture("Common/Icon_Bg_base.png")
		img:setPosition(ccp(j*100 + 50, 50));
		layout:addChild(img, i, i+1);
		img:setName("Img_"..(i+1))
		if bagList[index] ~= nil then
			local node = UI_ItemIconFrame(UI_GetItemIcon(bagList[index]:GetData().m_icon, bagList[index]:GetData().m_step, true), bagList[index]:GetData().m_quality)
			img:addChild(node);
		else
			local frame = ImageView:create();
			frame:loadTexture("Common/Icon_Bg_001.png")
			img:addChild(frame);
		end
			
		img:addTouchEventListener(ClickBagIcon)
		
		index = index + 1;
	end	
	
	UI_SetLabelText(heroLayout:getChildByTag(11), 1, g_selectHero:GetInt(EHero_HP));
	UI_SetLabelText(heroLayout:getChildByTag(12), 1, g_selectHero:GetInt(EHero_Att));
	UI_SetLabelText(heroLayout:getChildByTag(13), 1, g_selectHero:GetInt(EHero_Armor));
	UI_SetLabelText(heroLayout:getChildByTag(14), 1, g_selectHero:GetInt(EHero_SunderArmor));
end
]]

if (g_bagType == nil ) then
	g_bagType = GameServer_pb.en_EquipContainer_ActorBag
end

function Equip_Info(equip, bagType)
	local full = 1;
	if (bagType == nil)then
		full = 0
	else
		g_bagType = bagType;
	end
	EndWaiting();
	local widget = UI_CreateBaseWidgetByFileName("EquipInfo.json");
	if (widget == nil)then
		Log("equip_Info error");
		return;
	end
	local infoLayout = UI_GetUIImageView(widget, 2)
	local data = equip:GetData();
	
	UI_GetUILabel(infoLayout, 8):setText(""..data.m_lvlLimit)
	UI_GetUILabel(infoLayout, 9):setText(""..equip:GetInt(EEquip_SellMoney))
	
	--UI_GetUIImageView(widget, 11):setVisible(data.m_sellType == 2);
	--UI_GetUIImageView(widget, 12):setVisible(data.m_sellType == 1);
	
	local nameLab = CompLabel:GetCompLabel(FormatString("EquipName", UI_GetStringColorNew(data.m_quality), data.m_name), 200, kCCTextAlignmentLeft);
	UI_GetUILabel(infoLayout, 5):addChild(nameLab);
	
	local maker = CompStringMaker:new_local();
	for i=EEquip_HP, EEquip_SunderArmor do
		if (equip:GetInt(i) > 0)then
			maker:AppendString(Equip_GetAttName(i).." +"..GetAttShowValue(i,equip:GetInt(i)), CompTextArgs:new_local(0,255,0));
			if (i ~= EEquip_SunderArmor)then
				maker:AppendChangeLine();
			end
		end
	end
	
	local attLab = CompLabel:GetCompLabel(maker:GetString(), 260, kCCTextAlignmentLeft);
	UI_GetUILabel(infoLayout, 6):addChild(attLab);
	
	--[[local maker1 = CompStringMaker:new_local();
	for i=EEquip_HP, EEquip_SunderArmor do
		if (equip:GetExtraInt(Equip_GetAttString(i)) > 0)then
			maker1:AppendString(Equip_GetAttName(i).." +"..equip:GetExtraInt(Equip_GetAttString(i)), CompTextArgs:new_local(255,193,0));
			if (i ~= EEquip_SunderArmor)then
				maker1:AppendChangeLine();
			end
		end
	end
	
	local attLab1 = CompLabel:GetCompLabel(maker1:GetString(), 260, kCCTextAlignmentLeft);
	UI_GetUILabel(widget, 7):addChild(attLab1);]]
	
	local function ClickLoad(sender,eventType) 
		if (eventType == TOUCH_EVENT_ENDED) then
			local tab = GameServer_pb.CMD_EQUIP_MOVE_CS();
			if (bagType == GameServer_pb.en_EquipContainer_HeroWear) then
				tab.dwSrcObjectID = g_selectHero:GetUint(EHero_ID);	
				tab.dwDstObjectID = GetLocalPlayer():GetInt(EPlayer_ObjId);
			else
				tab.dwSrcObjectID = GetLocalPlayer():GetInt(EPlayer_ObjId);
				tab.dwDstObjectID = g_selectHero:GetUint(EHero_ID);	
			end
			
			tab.iSrcPos = equip:GetInt(EEquip_Pos);	
			tab.iDstPos = -1;	
			Packet_Full(GameServer_pb.CMD_EQUIP_MOVE, tab);
			ShowWaiting();
			
			UI_CloseCurBaseWidget();
		end
	end 
	
	UI_GetUIImageView(infoLayout, 100):addTouchEventListener(UI_ClickCloseCurBaseWidget);
	
	UI_GetUIImageView(infoLayout, 20):addChild(UI_ItemIconFrame(UI_GetItemIcon(data.m_icon), data.m_quality))
	
	widget:addTouchEventListener(UI_ClickCloseCurBaseWidget);
	
	if (full == 1)then
		local loadButton = UI_GetUIButton(infoLayout, 2);
		loadButton:addTouchEventListener(ClickLoad);
		
		if (bagType == GameServer_pb.en_EquipContainer_HeroWear)then
			loadButton:setTitleText(FormatString("xiexia"))
		else
			loadButton:setTitleText(FormatString("zhuangbei"))
		end
		local function SellEquip()
			local tab = GameServer_pb.CMD_EQUIP_SELLBYPOS_CS();
				
			tab.iEquipPos = equip:GetInt(EEquip_Pos);	
			Packet_Full(GameServer_pb.CMD_EQUIP_SELLBYPOS, tab);
			ShowWaiting();
			
			UI_CloseCurBaseWidget();
		end
		
		local function ClickSell(sender,eventType) 
			if (eventType == TOUCH_EVENT_ENDED) then
				Messagebox_Create({info = FormatString("SellEquipNotice", FormatString("EquipName", UI_GetStringColorNew(data.m_quality), data.m_name)),msgType = EMessageType_LeftRight,leftFun = SellEquip})
			end
		end 
		
		UI_GetUIButton(infoLayout, 3):addTouchEventListener(ClickSell);
		UI_GetUIButton(infoLayout, 3):setTouchEnabled(data.m_sellType ~= 0 and bagType ~= GameServer_pb.en_EquipContainer_HeroWear);
		
		local function ClickQianghua(sender,eventType) 
			if (eventType == TOUCH_EVENT_ENDED) then
				g_selectEquip = equip;
				UI_CloseCurBaseWidget();
				
				local tab = GameServer_pb.CMD_EQUIP_QUERYENHANCE_CS();
				if (bagType == GameServer_pb.en_EquipContainer_HeroWear) then
					tab.dwHeroObjectID = g_selectHero:GetUint(EHero_ID);	
				else
					tab.dwHeroObjectID = GetLocalPlayer():GetInt(EPlayer_ObjId);
				end
				
				tab.iEquipPos = equip:GetInt(EEquip_Pos);	
				Packet_Full(GameServer_pb.CMD_EQUIP_QUERYENHANCE, tab);
				ShowWaiting();
				--EquipUp_Create();
			end
		end 
		
		UI_GetUIButton(infoLayout, 4):addTouchEventListener(ClickQianghua);
	else
		UI_GetUIButton(infoLayout, 2):setVisible(false);
		UI_GetUIButton(infoLayout, 3):setVisible(false);
		UI_GetUIButton(infoLayout, 4):setVisible(false);
	end
	
	local suitId = equip:GetInt(EEquip_SuitID);
	local suitData = GetGameData(DataFileEquipSuit, suitId, "stEquipSuitData");
	local suitLayout = UI_GetUIImageView(widget, 3);
	if (suitData) then
		UI_GetUILabel(suitLayout, 100):setText(suitData.m_name);
		local activeCount = 0
		if (bagType == GameServer_pb.en_EquipContainer_HeroWear) then
			local heroSuitState = GetHeroEquipSuitStates(g_selectHero:GetInt(EHero_ID));
			
			if (heroSuitState[suitId]) then
				activeCount = heroSuitState[suitId]
			end
		end
		
		for i=101, 103 do
			local label = UI_GetUILabel(suitLayout, i);
			local labelTitle = UI_GetUILabel(suitLayout, i + 10);
			if (activeCount > 0) then
				label:setColor(ccc3(255, 255, 255))
				labelTitle:setColor(ccc3(255, 255, 255))
				activeCount = activeCount -1;
			else
				label:setColor(ccc3(125, 125, 125))
				labelTitle:setColor(ccc3(125, 125, 125))
			end
			
			if (i == 101)then
				label:setText(suitData.m_att1)
			elseif (i == 102) then
				label:setText(suitData.m_att2)
			else
				label:setText(suitData.m_att3)
			end
		end
		
		local equipList = UI_Split(suitData.m_equipList, "#");
		
		for i=1, #equipList do
			local equipData = GetGameData(DataFileEquip, TypeConvert:ToInt(equipList[i]), "stEquipData")
			
			local nameLab = CompLabel:GetCompLabel(FormatString("EquipName", UI_GetStringColorNew(equipData.m_quality), equipData.m_name), 200, kCCTextAlignmentLeft);
			nameLab:setPosition(ccp(-nameLab:getSize().width/2, nameLab:getSize().height/2))
			UI_GetUILabel(suitLayout, i+10):addChild(nameLab);
			
			UI_GetUIImageView(suitLayout, i):addChild(UI_ItemIconFrame(UI_GetItemIcon(equipData.m_icon), equipData.m_quality))
		end
	else
		infoLayout:setPosition(ccp(480, 320))
		suitLayout:setVisible(false);
	end
	
	UIMgr:GetInstance():PlayOpenAction(widget, EUIOpenAction_Enlarge, 0.4, 0);
end

function EquipSell_Create()
	local widget = UI_CreateBaseWidgetByFileName("EquipSell.json");
	if (widget == nil)then
		Log("equip_Info error");
		return;
	end

	local closeButton = UI_GetUIButton(widget, 1);
	closeButton:addTouchEventListener(UI_ClickCloseCurBaseWidget);
	
	widget:addTouchEventListener(UI_ClickCloseCurBaseWidget);
	
	UI_GetUICheckBox(widget, 2):setSelectedState(true)
	
	local bag = GetLocalPlayer():GetEquipBags();
	local function SellMoney_Refresh()
		if (UI_GetUICheckBox(widget, 2):getSelectedState())then
			UI_GetUILabel(widget, 4):setText(""..bag:GetSellMoneyByQuality(2))
		else
			UI_GetUILabel(widget, 4):setText(""..bag:GetSellMoneyByQuality(3))
		end
	end
	
	SellMoney_Refresh();
	
	local function ClickSell(sender,eventType) 
		if (eventType == TOUCH_EVENT_ENDED) then
			local tab = GameServer_pb.CMD_EQUIP_SELLBYQUALITY_CS();
			tab.bOnlyGreen = UI_GetUICheckBox(widget, 2):getSelectedState();	
			Packet_Full(GameServer_pb.CMD_EQUIP_SELLBYQUALITY, tab);
			ShowWaiting();
		end
	end 
	
	UI_GetUIButton(widget, 5):addTouchEventListener(ClickSell);
	
	local function ClickCheckBox1(sender,eventType)
        if eventType == CHECKBOX_STATE_EVENT_SELECTED then
			UI_GetUICheckBox(widget, 3):setSelectedState(false)
			SellMoney_Refresh()
        elseif eventType == CHECKBOX_STATE_EVENT_UNSELECTED then
            UI_GetUICheckBox(widget, 3):setSelectedState(true)	
			SellMoney_Refresh()
        end
	end
	
	local function ClickCheckBox2(sender,eventType)
        if eventType == CHECKBOX_STATE_EVENT_SELECTED then
			UI_GetUICheckBox(widget, 2):setSelectedState(false)
			SellMoney_Refresh()
        elseif eventType == CHECKBOX_STATE_EVENT_UNSELECTED then
			UI_GetUICheckBox(widget, 2):setSelectedState(true)
			SellMoney_Refresh()
        end
	end
	
	UI_GetUICheckBox(widget, 2):addEventListenerCheckBox(ClickCheckBox1);
	UI_GetUICheckBox(widget, 3):addEventListenerCheckBox(ClickCheckBox2);
	
	UIMgr:GetInstance():PlayOpenAction(widget, EUIOpenAction_Enlarge, 0.4, 0);
end

function EquipSelect_Create(wearPos, callBack)
	local widget = UI_CreateBaseWidgetByFileName("EquipSelect.json");
	if (widget == nil)then
		Log("EquipSelect_Create error");
		return;
	end

	local closeButton = UI_GetUIButton(widget, 1);
	closeButton:addTouchEventListener(UI_ClickCloseCurBaseWidget);
	widget:addTouchEventListener(UI_ClickCloseCurBaseWidget);
	
	local equipList = vector_Equip__:new_local()
	local bag = GetLocalPlayer():GetEquipBags();
	bag:GetEquipListByWearPos(equipList, wearPos);
	
	local function ClickIcon(sender,eventType)
		local sender = tolua.cast(sender, "Button");		
		if (eventType == TOUCH_EVENT_ENDED) then
			UI_CloseCurBaseWidget();	
			if (callBack)then
				callBack(equipList[sender:getTag()]);
			end
		end
	end
	
	local tempLayout = UI_GetUILayout(widget, 100);

	local listView = UI_GetUIListView(widget, 2);
	listView:setItemModel(tempLayout);
	for i=0, equipList:size()-1 do
		local layout = UI_GetCloneLayout(tempLayout);
		layout:setVisible(true);
		local img = UI_GetUIImageView(layout, 10001);
		local node = UI_ItemIconFrame(UI_GetItemIcon(equipList[i]:GetData().m_icon),equipList[i]:GetData().m_quality)
		img:addChild(node);
		
		local nameLab = CompLabel:GetCompLabel(FormatString("EquipName", UI_GetStringColorNew(equipList[i]:GetData().m_quality), equipList[i]:GetData().m_name), 200, kCCTextAlignmentLeft);
		UI_GetUILabel(layout, 10002):addChild(nameLab);
		
		local maker = CompStringMaker:new_local();
		for j=EEquip_HP, EEquip_SunderArmor do
			if (equipList[i]:GetInt(j) > 0)then
				maker:AppendString(Equip_GetAttName(j).." +"..GetAttShowValue(j,equipList[i]:GetInt(j)), CompTextArgs:new_local(0,255,0));
				if (j ~= EEquip_SunderArmor)then
					maker:AppendChangeLine();
				end
			end
		end
	
		local attLab = CompLabel:GetCompLabel(maker:GetString(), 200, kCCTextAlignmentLeft);
		UI_GetUILabel(layout, 10004):addChild(attLab);
		
		local button = UI_GetUIButton(layout, 10003)
		button:setTag(i)
		button:addTouchEventListener(ClickIcon);

		listView:pushBackCustomItem(layout);
	end	
	
	UIMgr:GetInstance():PlayOpenAction(widget, EUIOpenAction_Enlarge, 0.4, 0);
end

function Equip_GetAttName(attId)
	if (attId == EEquip_HP) then
		return FormatString("en_LifeAtt_MaxHP");
	elseif (attId == EEquip_Att) then
		return FormatString("en_LifeAtt_Att");
	elseif (attId == EEquip_Doge) then
		return FormatString("en_LifeAtt_Doge");
	elseif (attId == EEquip_Hit) then
		return FormatString("en_LifeAtt_Hit");
	elseif (attId == EEquip_AntiKnock) then
		return FormatString("en_LifeAtt_AntiKnock");
	elseif (attId == EEquip_Knock) then
		return FormatString("en_LifeAtt_Knock");
	elseif (attId == EEquip_Block) then
		return FormatString("en_LifeAtt_Block");
	elseif (attId == EEquip_Wreck) then
		return FormatString("en_LifeAtt_Wreck");
	elseif (attId == EEquip_Armor) then
		return FormatString("en_LifeAtt_Armor");
	elseif (attId == EEquip_SunderArmor) then
		return FormatString("en_LifeAtt_Sunder");
	elseif (attId == EEquip_InitAnger) then
		return FormatString("en_LifeAtt_InitAnger");
	elseif (attId == EEquip_SkillDamage) then
		return FormatString("en_LifeAtt_SkillDamage");
	elseif (attId == en_LifeAtt_SkillDef) then
		return FormatString("en_LifeAtt_SkillDef");
	end
end

function Equip_GetAttString(attId)
	if (attId == EEquip_HP) then
		return "MaxHP";
	elseif (attId == EEquip_Att) then
		return "Att";
	elseif (attId == EEquip_Doge) then
		return "Doge";
	elseif (attId == EEquip_Hit) then
		return "Hit";
	elseif (attId == EEquip_AntiKnock) then
		return "AntiKnock";
	elseif (attId == EEquip_Knock) then
		return "Knock";
	elseif (attId == EEquip_Block) then
		return "Block";
	elseif (attId == EEquip_Wreck) then
		return "Wreck";
	elseif (attId == EEquip_Armor) then
		return "Armor";
	elseif (attId == EEquip_SunderArmor) then
		return "Sunder";
	elseif (attId == EEquip_InitAnger) then
		return "InitAnger";
	elseif (attId == EEquip_SkillDamage) then
		return "SkillDamage";
	end
end

if (g_selectEquip == nil)then
	g_selectEquip = nil;
end

if (g_lightFrame == nil) then
	g_lightFrame = ImageView:create();
	g_lightFrame:loadTexture("Common/light.png")
	g_lightFrame:retain();
end
--[[
if (g_lightFrame1 == nil) then
	g_lightFrame1 = ImageView:create();
	g_lightFrame1:loadTexture("hero/light.png")
	g_lightFrame1:retain();
end

if (g_lightFrame2 == nil) then
	g_lightFrame2 = ImageView:create();
	g_lightFrame2:loadTexture("hero/light.png")
	g_lightFrame2:retain();
end

function EquipUpStage_Create()
	local widget = UI_CreateBaseWidgetByFileName("EquipUpStage.json");
	if (widget == nil)then
		Log("EquipUpStage_Create error");
		return;
	end
	
	if (g_selectHero == nil)then
		g_selectHero = GetEntityById(GetLocalPlayer():GetHeroList()[0], "Hero");
	end
	g_selectNode = nil;
	
	local bagList = vector_Equip__:new_local()
	GetLocalPlayer():GetEquipBags():GetEquipList(bagList);
	
	local closeButton = UI_GetUIButton(widget, 4);
	closeButton:addTouchEventListener(UI_ClickCloseCurBaseWidgetWithNoAction);
	
	local function ClickEquipBag(sender,eventType) 
		if (eventType == TOUCH_EVENT_ENDED) then
			EquipUpStage_ChgLayout(widget, 3);
		end
	end 
	
	UI_GetUIButton(widget:getChildByTag(2), 15):addTouchEventListener(ClickEquipBag);

	local tempLayout = Layout:create();
	tempLayout:setSize(CCSizeMake(400, 100));
	
	UI_GetUIImageView(widget, 11):setTouchEnabled(true);
	UI_GetUIImageView(widget, 12):setTouchEnabled(true);
	UI_GetUIImageView(widget, 13):setTouchEnabled(true);
	UI_GetUIImageView(widget, 14):setTouchEnabled(true);
	
	local zhaugnbei = GetUIArmature("Effzhaugnbei")
	zhaugnbei:getAnimation():playWithIndex(0)
	zhaugnbei:setPosition(ccp(480, 320));
	widget:addNode(zhaugnbei, 1, 10001);
	
	EquipUpStage_ChgLayout(widget, 2);
	
	EquipUpStage_SetEquip(widget);
	EquipUpStage_Refresh(widget);
end

function EquipUpStage_Refresh(widget)
	EndWaiting();
	
	local tiliButton = UI_GetUIButton(widget, 23);
	tiliButton:addTouchEventListener(ClickBuyTili);
	UI_SetLabelText(widget, 20, UI_GetMoneyStr(GetLocalPlayer():GetUint(EPlayer_Gold)));
	UI_SetLabelText(widget, 21, UI_GetMoneyStr(GetLocalPlayer():GetUint(EPlayer_Silver)));
	UI_SetLabelText(widget, 22, ""..GetLocalPlayer():GetUint(EPlayer_Tili).."/"..GetLocalPlayer():GetUint(EPlayer_TiliMax));
	
	local heroLayout = UI_GetUILayout(widget, 2);

	local heroList = GetLocalPlayer():GetHeroList();
	local wearList = g_selectHero:GetEquipBags():GetEquipList();
	local bagList = vector_Equip__:new_local()
	GetLocalPlayer():GetEquipBags():GetEquipList(bagList);
	
	local function ClickHeroIcon(sender,eventType)
		local sender = tolua.cast(sender, "ImageView");		
		if (eventType == TOUCH_EVENT_ENDED) then
			local hero = GetEntityById(heroList[sender:getTag()], "Hero");
			EquipUpStage_ChgLayout(widget, 2);
			if (hero ~= g_selectHero)then
				Log("hero ="..hero:GetEntityName())
				g_selectHero = hero
				
				g_lightFrame1:removeFromParent()		
				sender:addChild(g_lightFrame1, 10, 10000)
				
				if (g_selectHero:GetExtraBool("IsQueryEquip")) then
					UIMgr:GetInstance():RefreshBaseWidgetByName("Equip")
					EquipUpStage_Refresh(widget)
				else
					local tab = GameServer_pb.CMD_QUERY_HEROEQUIP_CS();
					tab.dwHeroObjectID = g_selectHero:GetUint(EHero_ID);
					Packet_Full(GameServer_pb.CMD_QUERY_HEROEQUIP, tab);
					ShowWaiting();
				end
			end
		end
	end
	
	local function ClickWearIcon(sender,eventType) 
		local sender = tolua.cast(sender, "ImageView");		
		local tag = sender:getTag() - 1;
		
		if (eventType == TOUCH_EVENT_ENDED) then
			if (wearList[tag])then
				if (g_isGuide) then
					Guide_GoNext();
				end
				EquipUpStage_SetEquip(widget, wearList[tag])--Equip_Info(wearList[tag], GameServer_pb.en_EquipContainer_HeroWear)
				g_lightFrame2:removeFromParent()		
				sender:addChild(g_lightFrame2, 10, 10000)
			else
				EquipSelect_Create(tag, Equip_Wear)
			end
		end
	end 
	
	local function ClickBagIcon(sender,eventType) 
		local sender = tolua.cast(sender, "ImageView");		
		local tag = sender:getTag() - 1;
		if (eventType == TOUCH_EVENT_ENDED) then
			if (bagList[tag])then
				--Equip_Info(bagList[tag], GameServer_pb.en_EquipContainer_ActorBag)
				EquipUpStage_SetEquip(widget, bagList[tag])
				g_lightFrame2:removeFromParent()		
				sender:addChild(g_lightFrame2, 10, 10000)
			end
		end
	end 
	
	local listView = UI_GetUIListView(widget, 1);
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
				g_lightFrame1:removeFromParent()		
				icon:addChild(g_lightFrame1, 10, 10000)
			end

			listView:pushBackCustomItem(layout);
		end
	end
	
	for i=0, wearList:size()-1 do
		local button = UI_GetUIImageView(heroLayout, i+1)
		button:setTouchEnabled(true);
		button:removeChildByTag(1);
		button:removeAllNodes();
		if wearList[i] ~= nil then	
			local node = UI_ItemIconFrame(UI_GetItemIcon(wearList[i]:GetData().m_icon, wearList[i]:GetData().m_step, true), wearList[i]:GetData().m_quality)
			--node:setPosition(ccp(40, 40))	
			button:addChild(node, 1, 1);
			
			if (g_selectEquip == wearList[i])then
				g_lightFrame2:removeFromParent()		
				button:addChild(g_lightFrame2, 10, 10000)
			end
		else
			local frame = ImageView:create();
			frame:loadTexture("Common/Icon_Bg_001.png")
			button:addChild(frame);
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
	
	UI_GetUILabelBMFont(heroLayout, 7):setText(g_selectHero:GetUint(EHero_FightValue))
	
	local bagListView = UI_GetUIListView(widget:getChildByTag(3), 1);
	bagListView:setItemModel(tempLayout);
	local index = 0;
	bagListView:removeAllItems();
	
	for i=0, bagList:size()-1 do
		local j = i % 4;
		if (j == 0)then
			layout = UI_GetCloneLayout(tempLayout);
			bagListView:pushBackCustomItem(layout);
		end
		local img = ImageView:create();
		img:setTouchEnabled(true);
		img:loadTexture("Common/Icon_Bg_base.png")
		img:setPosition(ccp(j*100 + 50, 50));
		layout:addChild(img, i, i+1);
		
		if bagList[index] ~= nil then
			local node = UI_ItemIconFrame(UI_GetItemIcon(bagList[index]:GetData().m_icon, bagList[index]:GetData().m_step, true), bagList[index]:GetData().m_quality)
			img:addChild(node);
			
			if (g_selectEquip == bagList[index])then
				g_lightFrame2:removeFromParent()		
				img:addChild(g_lightFrame2, 10, 10000)
			end
		else
			local frame = ImageView:create();
			frame:loadTexture("Common/Icon_Bg_001.png")
			img:addChild(frame);
		end
			
		img:addTouchEventListener(ClickBagIcon)
		
		index = index + 1;
	end	
	
	UI_SetLabelText(heroLayout:getChildByTag(11), 1, g_selectHero:GetInt(EHero_HP));
	UI_SetLabelText(heroLayout:getChildByTag(12), 1, g_selectHero:GetInt(EHero_Att));
	UI_SetLabelText(heroLayout:getChildByTag(13), 1, g_selectHero:GetInt(EHero_Armor));
	UI_SetLabelText(heroLayout:getChildByTag(14), 1, g_selectHero:GetInt(EHero_SunderArmor));
end

function EquipUpStage_SetEquip(widget, equip)
	if (equip ~= nil)then
		g_selectEquip = equip;
	end
	
	UI_GetUIImageView(widget, 11):removeAllChildren();
	UI_GetUIImageView(widget, 12):removeAllChildren();
	UI_GetUIImageView(widget, 13):removeAllChildren();
	UI_GetUIImageView(widget, 14):removeChildByTag(1);
	
	if (g_selectEquip == nil)then
		return;
	end
	
	local bag = GetLocalPlayer():GetEquipBags()
	
	if (bag:EquipIsInBag(g_selectEquip)) then
		EquipUpStage_ChgLayout(widget, 3);
	else
		EquipUpStage_ChgLayout(widget, 2);
	end
	
	local data1 = g_selectEquip:GetData();
	local combineData = GetGameData(DataFileEquipCombine, data1.m_ID, "stEquipCombineData")
	if (combineData == nil)then
		return;
	end
	local data2 = GetGameData(DataFileEquip, combineData.m_materialID, "stEquipData")
	local data3 = GetGameData(DataFileEquip, combineData.m_createID, "stEquipData")
	local itemIcon1 = UI_GetItemIcon(data1.m_icon);
	itemIcon1:setScale(0.7)
	UI_GetUIImageView(widget, 11):addChild(itemIcon1)
	local nameLab1 = CompLabel:GetCompLabel(FormatString("EquipName", UI_GetStringColorNew(data1.m_quality), data1.m_name), 200, kCCTextAlignmentLeft);
	nameLab1:setPosition(ccp(-nameLab1:getSize().width/2, 60))
	UI_GetUIImageView(widget, 11):addChild(nameLab1)
	
	local count = bag:GetEquipCount(data2.m_ID, g_selectEquip);
	
	local itemIcon2 = UI_GetItemIcon(data2.m_icon)
	if (count < 1)then
		itemIcon2:addColorGray();
	end
	UI_GetUIImageView(widget, 12):addChild(itemIcon2)
	itemIcon2:setScale(0.7)
	local nameLab2 = CompLabel:GetCompLabel(FormatString("EquipName", UI_GetStringColorNew(data2.m_quality), data2.m_name), 200, kCCTextAlignmentLeft);
	nameLab2:setPosition(ccp(-nameLab2:getSize().width/2, 60))
	UI_GetUIImageView(widget, 12):addChild(nameLab2)
	local itemIcon2 = UI_GetItemIcon(data2.m_icon)
	
	local itemIcon3 = UI_GetItemIcon(data2.m_icon)
	if (count < 2)then
		itemIcon3:addColorGray();
	end
	itemIcon3:setScale(0.7)
	UI_GetUIImageView(widget, 13):addChild(itemIcon3)
	local nameLab3 = CompLabel:GetCompLabel(FormatString("EquipName", UI_GetStringColorNew(data2.m_quality), data2.m_name), 200, kCCTextAlignmentLeft);
	nameLab3:setPosition(ccp(-nameLab3:getSize().width/2, 60))
	UI_GetUIImageView(widget, 13):addChild(nameLab3)
	
	local itemIcon4 = UI_GetItemIcon(data3.m_icon);
	itemIcon4:addColorGray();
	itemIcon4:setScale(0.7)
	UI_GetUIImageView(widget, 14):addChild(itemIcon4, 1, 1)
	local nameLab4 = CompLabel:GetCompLabel(FormatString("EquipName", UI_GetStringColorNew(data3.m_quality), data3.m_name), 200, kCCTextAlignmentLeft);
	nameLab4:setPosition(ccp(-nameLab4:getSize().width/2, 60))
	itemIcon4:addChild(nameLab4, 2, 2)
	
	local function ZhuanbeiAnimEnd(armature, movementType, movementID)
		if (movementType == LOOP_COMPLETE)then
			if (movementID == "1")then
				armature:getAnimation():playWithIndex(2)
			elseif (movementID == "2")then
				armature:getAnimation():playWithIndex(3)
			elseif (movementID == "3")then
				armature:getAnimation():playWithIndex(4)		
			elseif (movementID == "4")then
				local tab = GameServer_pb.CMD_EQUIP_COMBINE_CS();
				if (GetLocalPlayer():GetEquipBags():EquipIsInBag(g_selectEquip))then
					tab.dwSrcObjectID = GetLocalPlayer():GetInt(EPlayer_ObjId);
				else
					tab.dwSrcObjectID = g_selectHero:GetInt(EHero_ID);
				end
				tab.iSrcEquipPos = g_selectEquip:GetUint(EEquip_Pos);
				
				Packet_Full(GameServer_pb.CMD_EQUIP_COMBINE, tab);
				armature:getAnimation():playWithIndex(0)
				ShowWaiting();
			end
		end
	end
	local function Armature_Event(bone, evt, originFrameIndex, currentFrameIndex)
		if (evt == "fade") then
			UI_GetUIImageView(widget:getChildByTag(14), 1):removeColorGray();
			local action = CCFadeIn:create(0.5)
			UI_GetUIImageView(widget:getChildByTag(14), 1):runAction(action);
		end
	end
	local zhaugnbei = tolua.cast( widget:getNodeByTag(10001), "CCArmature" )
	zhaugnbei:getAnimation():setMovementEventCallFunc(ZhuanbeiAnimEnd);
	zhaugnbei:getAnimation():setFrameEventCallFunc(Armature_Event);
	local function ClickIcon(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			sender = tolua.cast(sender, "ImageView");	
			local tag = sender:getTag();
			if (g_selectEquip == nil)then
				return;
			end
			
			if (tag == 11) then
				Equip_Info(g_selectEquip)
			elseif (tag == 14) then
				Log("data3.m_ID"..data3.m_ID)
				local tab = GameServer_pb.CMD_QUERY_EQUIPDESC_BYID_CS();
				tab.iEquipID = data3.m_ID;
				Packet_Full(GameServer_pb.CMD_QUERY_EQUIPDESC_BYID, tab);
				ShowWaiting();
			else
				local tab = GameServer_pb.CMD_QUERY_EQUIPSOURCE_CS();
				tab.iEquipID = data2.m_ID;
				Packet_Full(GameServer_pb.CMD_QUERY_EQUIPSOURCE, tab);
				ShowWaiting();
			end
		end
	end
	
	local function ClickCombine(sender,eventType) 
		if (eventType == TOUCH_EVENT_ENDED) then
			if (g_isGuide) then
				Guide_GoNext();
			end
		
			if (g_selectEquip == nil)then
				Messagebox_Create({info = FormatString("EquipCombineNotEquip"), msgType = EMessageType_None})
			elseif (count >= 2) then
				zhaugnbei:getAnimation():playWithIndex(1);
				
				local action1 = CCFadeOut:create(0.3)
				UI_GetUIImageView(widget, 11):runAction(action1);
				local action2 = CCFadeOut:create(0.3)
				UI_GetUIImageView(widget, 12):runAction(action2);
				local action3 = CCFadeOut:create(0.3)
				UI_GetUIImageView(widget, 13):runAction(action3);

				local action = CCFadeOut:create(0.3)
				UI_GetUIImageView(widget:getChildByTag(14), 1):runAction(action);
				
				ShowWaiting(15, false);
			else
				Messagebox_Create({info = FormatString("EquipCombineNotEnough"), msgType = EMessageType_None})
			end
		end
	end 
	
	UI_GetUIButton(widget, 16):addTouchEventListener(ClickCombine);
	
	UI_GetUIImageView(widget, 11):addTouchEventListener(ClickIcon)
	UI_GetUIImageView(widget, 12):addTouchEventListener(ClickIcon)
	UI_GetUIImageView(widget, 13):addTouchEventListener(ClickIcon)
	UI_GetUIImageView(widget, 14):addTouchEventListener(ClickIcon)
end
]]

function EquipGoDungeon_Create(tmp)
	local data = GetGameData(DataFileEquip, tmp.iEquipID, "stEquipData")
	local widget = UI_CreateBaseWidgetByFileName("EquipGoDungeon.json");
	widget:addTouchEventListener(UI_ClickCloseCurBaseWidget);
	UI_GetUIButton(widget, 1):addTouchEventListener(UI_ClickCloseCurBaseWidget);
	UI_GetUILabel(widget, 2):setText("");
	local nameLab = CompLabel:GetCompLabel(FormatString("EquipName", UI_GetStringColorNew(data.m_quality), data.m_name), 200, kCCTextAlignmentLeft);
	UI_GetUILabel(widget, 2):addChild(nameLab)
	
	local function ClickDungeon(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			sender = tolua.cast(sender, "Layout");
			local tag = sender:getTag();
			if (tmp.szEquipSource[tag].bOpened) then
				UI_CloseAllBaseWidget()
				g_selectSectionID = tmp.szEquipSource[tag].iSectionID;
				g_selectiSceneID = tmp.szEquipSource[tag].iSceneID;
				local tab = GameServer_pb.Cmd_Cs_QueryDSectionDetail();
				tab.iSectionID = g_selectSectionID;
				Packet_Full(GameServer_pb.CMD_QUERY_DSECTIONDETAIL, tab);
				ShowWaiting()
			end	
		end
	end
	local listView = UI_GetUIListView(widget, 3);	
	local tempLayout = Layout:create();
	tempLayout:setSize(CCSizeMake(400, 100))
	for i=1, #tmp.szEquipSource do
		local sceneSectionData = GetGameData(DataFileSceneSection, tmp.szEquipSource[i].iSectionID, "stSceneSectionData");
		local sceneData = GetGameData(DataFileScene, tmp.szEquipSource[i].iSceneID, "stSceneData");
		local layout = UI_GetCloneLayout(tempLayout)
		local layout1 = UI_GetCloneLayout(tempLayout)
		layout1:setTouchEnabled(true)
		layout1:addTouchEventListener(ClickDungeon);
		layout:addChild(layout1, i, i);
		local iconFrame = UI_IconFrame(UI_GetHeroIcon(sceneData.m_icon))
		iconFrame:setPosition(ccp(50, 50))
		layout1:addChild(iconFrame);
		
		local label = nil
		if (tmp.szEquipSource[i].bOpened) then
			label = CompLabel:GetDefaultLabWithSize(FormatString("EquipDungeon", sceneSectionData.m_name, sceneData.m_name), 24);
		else
			label = CompLabel:GetDefaultLabWithSize(FormatString("EquipDungeonNotOpen", sceneSectionData.m_name, sceneData.m_name), 24);
		end
		label:setAnchorPoint(ccp(0, 0.5));
		label:setPosition(ccp(100, 50))
		layout1:addChild(label);
		
		listView:pushBackCustomItem(layout);
	end
end
--[[
function EquipUpStage_ChgLayout(widget, index)
	UI_GetUILayout(widget, 2):setVisible(index == 2)
	UI_GetUILayout(widget, 3):setVisible(index == 3)
end

function EQUIP_COMBINE(pkg)
	EndWaiting();
	local widget = UI_GetBaseWidgetByName("EquipUpStage")
	if (widget)then
		g_selectEquip = nil;
		EquipUpStage_SetEquip(widget);
		--UIMgr:GetInstance():GetBaseWidgetByName("EquipUpStage")
		--UI_CloseCurBaseWidget(EUICloseAction_None);
	end
end
]]
function QUERY_EQUIPSOURCE(pkg)
	EndWaiting();
	local tmp = GameServer_pb.CMD_QUERY_EQUIPSOURCE_SC();
	tmp:ParseFromString(pkg)
	
	EquipGoDungeon_Create(tmp)
end

function EQUIP_SUITSTATE_UPDATE(pkg)
	local tmp = GameServer_pb.CMD_EQUIP_SUITSTATE_UPDATE_SC();
	tmp:ParseFromString(pkg)
	Log("*********"..tostring(tmp))
	
	for i=1, #tmp.szsuitState do
		Equip_SetSuitState(tmp.dwObjectID, tmp.szsuitState[i].iSuitID, tmp.szsuitState[i].iActiveCount)
	end
end

function Equip_SetSuitState(objId, suitId, activeCount)
	local suitStateTab = GetHeroEquipSuitStates(objId);
	suitStateTab[suitId] = activeCount;
end

if g_heroEquipSuitStatesTab == nil then
	g_heroEquipSuitStatesTab = {}
end

function GetHeroEquipSuitStates(objId)
	if (g_heroEquipSuitStatesTab[objId] == nil)then
		g_heroEquipSuitStatesTab[objId] = {};
	end 
	return g_heroEquipSuitStatesTab[objId];
end

function EquipUp_Create(tmp)
	EndWaiting();
	local widget = UI_GetBaseWidgetByName("EquipUp");
	local data = g_selectEquip:GetData();
	if widget == nil then
		widget = UI_CreateBaseWidgetByFileName("EquipUp.json");
		if (widget == nil)then
			Log("EquipUp_Create error");
			return;
		end
		UI_GetUIImageView(widget, 8):addChild(UI_GetItemIcon(data.m_icon))
		
		widget:addTouchEventListener(UI_ClickCloseCurBaseWidget);
		UI_GetUIButton(widget, 1):addTouchEventListener(UI_ClickCloseCurBaseWidget);

		local nameLab = CompLabel:GetCompLabel(FormatString("EquipName", UI_GetStringColorNew(data.m_quality), data.m_name), 200, kCCTextAlignmentLeft);
		nameLab:setPosition(ccp(nameLab:getSize().width/2, 0))
		UI_GetUILabel(widget, 2):setText("")
		UI_GetUILabel(widget, 2):addChild(nameLab);
		
		UIMgr:GetInstance():PlayOpenAction(widget, EUIOpenAction_Enlarge, 0.4);
	end
	
	UI_GetUILabel(widget, 3):setText(tmp.iCurLevel);
	UI_GetUILabel(widget, 4):setText(tmp.iNextLevel);
	UI_GetUILabel(widget, 5):setText(tmp.iCostSilver);
	
	UI_GetUILabel(widget, 11):setText(GetAttName(tmp.szCurPropList[1].iLifeAttID).."：");
	UI_GetUILabel(widget, 12):setText(GetAttShowValue(tmp.szCurPropList[1].iLifeAttID,tmp.szCurPropList[1].iValue));
	UI_GetUILabel(widget, 13):setText(GetAttShowValue(tmp.szCurPropList[1].iLifeAttID,tmp.szNextPropList[1].iValue));
	
	--[[for i=1, 2 do
		if (tmp.szCurPropList[i] ~= nil)then
			Log("tmp.szCurPropList[i].iLifeAttID"..tmp.szCurPropList[i].iLifeAttID)
			UI_GetUILabel(widget, i*10 + 1):setText(GetAttName(tmp.szCurPropList[i].iLifeAttID).."：");
			UI_GetUILabel(widget, i*10 + 2):setText(""..tmp.szCurPropList[i].iValue);
			UI_GetUILabel(widget, i*10 + 3):setText(""..tmp.szNextPropList[i].iValue);
		else
			UI_GetUILabel(widget, i*10 + 1):setText("");
			UI_GetUILabel(widget, i*10 + 2):setText("");
			UI_GetUILabel(widget, i*10 + 3):setText("");
		end
	end]]
	
	local function ClickAutoQianghua(sender,eventType) 
		if (eventType == TOUCH_EVENT_ENDED) then
			
		end
	end 
	
	local function ClickQianghua(sender,eventType) 
		if (eventType == TOUCH_EVENT_ENDED) then
			local tab = GameServer_pb.CMD_EQUIP_ENHANCE_CS();
			if (g_bagType == GameServer_pb.en_EquipContainer_HeroWear) then
				tab.dwHeroObjectID = g_selectHero:GetUint(EHero_ID);	
			else
				tab.dwHeroObjectID = GetLocalPlayer():GetInt(EPlayer_ObjId);
			end
			tab.iEquipPos = g_selectEquip:GetInt(EEquip_Pos);	
			Packet_Full(GameServer_pb.CMD_EQUIP_ENHANCE, tab);
			ShowWaiting();
		end
	end 
	UI_GetUIButton(widget, 7):addTouchEventListener(ClickQianghua);
end

function EQUIP_QUERYENHANCE(pkg)
	local tmp = GameServer_pb.CMD_EQUIP_QUERYENHANCE_SC();
	tmp:ParseFromString(pkg)
	Log("*********"..tostring(tmp))

	EquipUp_Create(tmp)
end

function EQUIP_ENHANCE(pkg)
	local tmp = GameServer_pb.CMD_EQUIP_ENHANCE_SC();
	tmp:ParseFromString(pkg)
	
	Log("*********"..tostring(tmp))
	if (tmp.bSuccess == false)then
	
	else
		local tab = GameServer_pb.CMD_EQUIP_QUERYENHANCE_CS();
		if (g_bagType == GameServer_pb.en_EquipContainer_HeroWear) then
			tab.dwHeroObjectID = g_selectHero:GetUint(EHero_ID);	
		else
			tab.dwHeroObjectID = GetLocalPlayer():GetInt(EPlayer_ObjId);
		end
		
		tab.iEquipPos = g_selectEquip:GetInt(EEquip_Pos);	
		Packet_Full(GameServer_pb.CMD_EQUIP_QUERYENHANCE, tab);
		ShowWaiting();
	end
end

function EquipChip_Create()
	local widget = UI_CreateBaseWidgetByFileName("EquipChip.json");
	UI_GetUIButton(widget, 1):addTouchEventListener(UI_ClickCloseCurBaseWidget);
	
	EquipChip_Refresh(widget);
	
	UIMgr:GetInstance():PlayOpenAction(widget, EUIOpenAction_MoveIn_Right, 0.4, 0);
	g_selectEquipChipIndex = 0;
end

if (g_selectEquipChipIndex == nil)then
	g_selectEquipChipIndex = 0;
end

function EquipChip_Refresh(widget)
	EndWaiting();
	
	local bagList = GetLocalPlayer():GetPlayerMaterail():GetBagList();
	
	if (bagList:size() == 0)then
		UI_CloseCurBaseWidget();
		return;
	end
	
	if (g_selectEquipChipIndex >= bagList:size())then
		g_selectEquipChipIndex = 0;
	end
	
	local function SetInfo()
		local equipChip = bagList[g_selectEquipChipIndex];
		local data = equipChip:getItemData();
		
		
		UI_GetUIImageView(widget, 2):removeAllChildren()
		UI_GetUIImageView(widget, 2):addChild(UI_ItemIconFrame(UI_GetItemIcon(data.m_icon), data.m_quality));
		
		local nameLab = CompLabel:GetCompLabel(FormatString("EquipName", UI_GetStringColorNew(data.m_quality), data.m_name), 200, kCCTextAlignmentLeft);
		UI_GetUILabel(widget, 3):setText("")
		UI_GetUILabel(widget, 3):removeAllChildren()
		UI_GetUILabel(widget, 3):addChild(nameLab);
		
		UI_GetUILabel(widget, 4):setText(FormatString("EquipChipCount", equipChip:GetInt(EBagItem_Count)));
		
		local descLabel = CompLabel:GetCompLabel(data.m_desc, 320, kCCTextAlignmentLeft);
		UI_GetUILabel(widget, 5):setText("")
		UI_GetUILabel(widget, 5):removeAllChildren()
		UI_GetUILabel(widget, 5):addChild(descLabel);
		
		UI_GetUILabel(widget, 6):setText(""..equipChip:GetInt(EBagItem_Count).."/"..equipChip:GetInt(EBagItem_CombineNeedCount))
		UI_GetUILabel(widget, 8):setText(""..data.m_sellMoney)
	end
	
	local function ClickIcon(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			sender = tolua.cast(sender, "ImageView");
			local tag = sender:getTag();
			g_selectEquipChipIndex = tag;
			
			g_lightFrame:removeFromParent()		
			sender:addChild(g_lightFrame, 10, 10000)
			
			SetInfo();
		end
	end
	
	local bagListView = UI_GetUIListView(widget, 11);
	local tempLayout = Layout:create();
	tempLayout:setSize(CCSizeMake(400, 100));
	bagListView:removeAllItems();
	local index = 0
	for i=0, bagList:size()-1 do
		local j = i % 5;
		if (j == 0)then
			layout = UI_GetCloneLayout(tempLayout);
			bagListView:pushBackCustomItem(layout);
		end
		local data = bagList[index]:getItemData()
		local img = ImageView:create();
		img:setTouchEnabled(true);
		img:loadTexture("Common/Icon_Bg_base.png")
		img:setScale(0.75)
		img:setPosition(ccp(j*80 + 40, 40));
		layout:addChild(img, i, i);
		img:setName("Img_"..(i+1))
		if bagList[index] ~= nil then
			local node = UI_ItemIconFrame(UI_GetItemIcon(data.m_icon), data.m_quality)
			img:addChild(node);
		else
			local frame = ImageView:create();
			frame:loadTexture("Common/Icon_Bg_011.png")
			img:addChild(frame);
		end
			
		img:addTouchEventListener(ClickIcon)
		
		if (i == g_selectEquipChipIndex) then
			g_lightFrame:removeFromParent()		
			img:addChild(g_lightFrame, 10, 10000)
		end
		
		index = index + 1;
	end	
	
	SetInfo()
	
	local function ClickCombine(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			local equipChip = bagList[g_selectEquipChipIndex];
			
			local tab = GameServer_pb.CMD_EQUIP_CHIPCOMBINE_CS();
			tab.iEquipChipID = equipChip:getItemData().m_ID;	
			Packet_Full(GameServer_pb.CMD_EQUIP_CHIPCOMBINE, tab);
			ShowWaiting();
		end
	end
	
	UI_GetUIButton(widget, 7):addTouchEventListener(ClickCombine);
	
	local function ClickDrop(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			local tab = GameServer_pb.CMD_QUERY_EQUIPSOURCE_CS();
			tab.iEquipID = data.m_ID;
			Packet_Full(GameServer_pb.CMD_QUERY_EQUIPSOURCE, tab);
			ShowWaiting();
		end
	end
	
	UI_GetUIButton(widget, 9):addTouchEventListener(ClickDrop);
	
	local function ClickSell(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			EquipChipSell_Create();
		end
	end
	
	UI_GetUIButton(widget, 10):addTouchEventListener(ClickSell);
end

function EquipChipSell_Create()
	local widget = UI_CreateBaseWidgetByFileName("EquipChipSell.json");
	widget:addTouchEventListener(UI_ClickCloseCurBaseWidget);
	UI_GetUIButton(widget, 1):addTouchEventListener(UI_ClickCloseCurBaseWidget);
	
	local sellCount = 1;
	
	local bagList = GetLocalPlayer():GetPlayerMaterail():GetBagList();
	
	if (bagList:size() == 0)then
		UI_CloseCurBaseWidget();
		return;
	end
	local infoLayout = UI_GetUIImageView(widget, 2);
	local equipChip = bagList[g_selectEquipChipIndex]
	local data = equipChip:getItemData();

	UI_GetUIImageView(infoLayout, 1):addChild(UI_ItemIconFrame(UI_GetItemIcon(data.m_icon), data.m_quality));
		
	local nameLab = CompLabel:GetCompLabel(FormatString("EquipName", UI_GetStringColorNew(data.m_quality), data.m_name), 200, kCCTextAlignmentLeft);
	UI_GetUILabel(infoLayout, 2):setText("")
	UI_GetUILabel(infoLayout, 2):addChild(nameLab);
		
	UI_GetUILabel(infoLayout, 3):setText(FormatString("EquipChipCount", equipChip:GetInt(EBagItem_Count)));
	UI_GetUILabel(infoLayout, 4):setText(""..data.m_sellMoney);
		
	local function Refresh()
		UI_GetUILabel(infoLayout, 6):setText(""..sellCount.."/"..equipChip:GetInt(EBagItem_Count))
		UI_GetUILabel(infoLayout, 9):setText(""..(sellCount*data.m_sellMoney))
	end
	
	Refresh()
	
	local function ClickDec(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			if (sellCount > 1)then
				sellCount = sellCount - 1;
				Refresh()
			end
		end
	end
	
	UI_GetUIButton(infoLayout, 5):addTouchEventListener(ClickDec);
	
	local function ClickAdd(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			if (sellCount < equipChip:GetInt(EBagItem_Count))then
				sellCount = sellCount + 1;
				Refresh()
			end
		end
	end
	
	UI_GetUIButton(infoLayout, 7):addTouchEventListener(ClickAdd);
	
	local function ClickMax(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			sellCount = equipChip:GetInt(EBagItem_Count)
			Refresh()
		end
	end
	
	UI_GetUIButton(infoLayout, 8):addTouchEventListener(ClickMax);
	
	local function ClickSell(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			local tab = GameServer_pb.CMD_EQUIP_CHIPSELL_CS();
			tab.iEquipCipID = data.m_ID;
			tab.iCount = sellCount;
			Packet_Full(GameServer_pb.CMD_EQUIP_CHIPSELL, tab);
			ShowWaiting();
			
			UI_CloseCurBaseWidget();
		end
	end

	UI_GetUIButton(infoLayout, 10):addTouchEventListener(ClickSell);
end

ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_EQUIP_ENHANCE, "EQUIP_ENHANCE" );
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_EQUIP_QUERYENHANCE, "EQUIP_QUERYENHANCE" );
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_EQUIP_SUITSTATE_UPDATE, "EQUIP_SUITSTATE_UPDATE" );
--ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_EQUIP_COMBINE, "EQUIP_COMBINE" );
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_QUERY_EQUIPSOURCE, "QUERY_EQUIPSOURCE" );
