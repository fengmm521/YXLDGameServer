function CreateRole_GetRoleResult(pkg)
	local tmp = GameServer_pb.Cmd_Sc_GetRole();
	tmp:ParseFromString(pkg)
	
	if (tmp.iResult == GameServer_pb.en_GetRole_OK)then	
		for i=1, #tmp.roleBaseData.szPropList do	
			local attId = tmp.roleBaseData.szPropList[i].iLifeAttID;
			local value = tmp.roleBaseData.szPropList[i].iValue;
			
			SetLifeAtt(GetLocalPlayer(), attId, value)				
			UI_RefreshBaseWidgetByName("MainUi");
		end
	elseif (tmp.iResult == GameServer_pb.en_GetRole_NoRole)then
		--CreateRole_Create();
	elseif (tmp.iResult == GameServer_pb.en_GetRole_Error)then
		--Login_BackToMaiMenu();
	end
end

--[[function CreateRole_CheckName(pkg)
	local tmp = GameServer_pb.Cmd_Sc_CheckActorName();
	local isRight = tmp:ParseFromString(pkg)
	
	if (tmp.iIsOk == 0)then	
		Log("name error");
	else
		UI_CloseCurBaseWidget();
		CreateRole_SelectHero();
	end
end]]

function CreateRole_GetRandomName()
	local randomName = "";
	local nameList = CDataManager:GetInstance():GetGameDataKeyList(DataFileCreateName);
	local maxValue = nameList:size() - 1;
	
	Log("maxValue ="..maxValue)
	
	local randomIndex = 0;
	local data = nil;
	
	for i=1, 50 do
		randomIndex = Random:RandomInt(0, maxValue);
		data = GetGameData(DataFileCreateName, nameList[randomIndex], "stCreateNameData");
		if (data.m_name1 ~= "") then
			randomName = randomName..data.m_name1;
			break;
		end
	end
	
	for i=1, 50 do
		randomIndex = Random:RandomInt(0, maxValue);
		data = GetGameData(DataFileCreateName, nameList[randomIndex], "stCreateNameData");
		if (data.m_name2 ~= "") then
			randomName = randomName..data.m_name2;
			break;
		end
	end
	
	for i=1, 50 do
		randomIndex = Random:RandomInt(0, maxValue);
		data = GetGameData(DataFileCreateName, nameList[randomIndex], "stCreateNameData");
		if (data.m_name3 ~= "") then
			randomName = randomName..data.m_name3;
			break;
		end
	end
	
	Log("randomName ="..randomName)
	return randomName;
end

function CreateRole_Create()
	local widget = UI_GetBaseWidgetByName("CreateRole");
	
	if (widget == nil)then
		widget = UI_CreateBaseWidgetByFileName("CreateRole.json");
	end
	if (widget == nil)then
		Log("CreateRole_Create error");
		return;
	end
	
	local randomName = CreateRole_GetRandomName();
	
	local textField = UI_GetUITextField(widget, 49);
	textField:setText(randomName)
	
	local function Name_Random(sender,eventType) 
		if (eventType == TOUCH_EVENT_ENDED) then
			textField:setText(CreateRole_GetRandomName())
		end
	end 
	local nameRandomButton = UI_GetUIButton(widget, 52);
	nameRandomButton:addTouchEventListener(Name_Random);
	
	local function ClickComfire(sender,eventType) 
		if (eventType == TOUCH_EVENT_ENDED) then
			local registerTextField = UI_GetUITextField(widget, 49);
			local registerStr = registerTextField:getStringValue();
			if (registerStr ~= "") then
				g_roleName = registerStr;
				
				local tab = GameServer_pb.Cmd_Cs_NewRole();
				tab.strRoleName = g_roleName;
	
				Packet_Full(GameServer_pb.CMD_NEW_ROLE, tab);
			else
                createPromptBoxlayout(FormatString("NameNull"))
            end
		end
	end 
	local confireButton = UI_GetUIButton(widget, 53);
	confireButton:addTouchEventListener(ClickComfire);

    local function startEnter(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then   
            UI_GetUITextField(widget, 49):attachWithIME()
        end
    end
    local enterName = UI_GetUIImageView(widget, 48)
    enterName:setTouchEnabled(true)
    enterName:addTouchEventListener(startEnter)

end

if (g_selectIndex == nil)then
	g_selectIndex = -1;
end

if (g_roleName == nil)then
	g_roleName = "";
end

--[[function CreateRole_SelectHero()
	g_selectIndex = -1;

	local widget = UI_CreateBaseWidgetByFileName("SelectHero.json");
	if (widget == nil)then
		Log("CreateRole_SelectHero error");
		return;
	end
	
	local listView = UI_GetUIListView(widget, 56);
	local selectNode = nil;
	
	local function ClickHero(sender,eventType) 
		if (eventType == TOUCH_EVENT_ENDED) then
			local layout = tolua.cast( sender, "Layout" );
			g_selectIndex = listView:getIndex(layout);
			Log("g_selectIndex= "..g_selectIndex);
			
			if (selectNode)then
				selectNode:removeNodeByTag(9999);
			end
			EffectMgr:AddButtonPromotToNode( layout, 10, 9999);
			selectNode = layout
		end
	end 
	local listView = UI_GetUIListView(widget, 56);
	
	local heroList = CDataManager:GetInstance():GetGameDataKeyList(DataFileHeroCreate);
	local tempLayout = Layout:create();
    tempLayout:setTouchEnabled(true);
	tempLayout:setSize(CCSizeMake(180, 400));
	--tempLayout:setBackGroundColorType(LAYOUT_COLOR_SOLID);
	--tempLayout:setBackGroundColor(ccc3(128, 128, 128));
	--tempLayout:setBackGroundColorOpacity(0);
	
	listView:setItemModel(tempLayout);
	for i=0, heroList:size()-1 do
		local data = GetGameData(DataFileHeroCreate, heroList[i], "stHeroCreateData");
		local layout = UI_GetCloneLayout(tempLayout);
	
		local titleLab = CompLabel:GetDefaultLabWithSize(data.m_name, EFontSize_Big);
		--titleLab:setText(data.m_name);
		titleLab:setColor(ccc3(156, 89, 90));
		titleLab:setPosition(ccp(90,350));
		
		layout:addChild(titleLab);
		local img = UI_GetHeroImg(data.m_img1, data.m_img2);
		img:setPosition(ccp(90, 200))
		layout:addChild(img);
		local featureLab = CompLabel:GetDefaultLab(FormatString("feature", data.m_feature));
		--featureLab:setText(FormatString("feature", data.m_feature));
		featureLab:setPosition(ccp(90, 80));
		
		layout:addChild(featureLab);
		local stuntNameLab = CompLabel:GetDefaultLab(FormatString("stunt", data.m_stuntName));
		--stuntNameLab:setText(FormatString("stunt", data.m_stuntName));
		stuntNameLab:setPosition(ccp(90, 50));
		
		layout:addChild(stuntNameLab);
		layout:addTouchEventListener(ClickHero)
		listView:pushBackCustomItem(layout);
	end
	
	local function ClickComfire(sender,eventType) 
		if (eventType == TOUCH_EVENT_ENDED) then
			if (g_selectIndex >= 0 and g_roleName ~= "") then
				local tab = GameServer_pb.Cmd_Cs_NewRole();
				tab.strRoleName = g_roleName;
				tab.iSelectHeroID = heroList[g_selectIndex];	
				Packet_Full(GameServer_pb.CMD_NEW_ROLE, tab);
			end
		end
	end 
	local confireButton = UI_GetUIButton(widget, 57);
	confireButton:addTouchEventListener(ClickComfire);
end
]]
function CreateRole_NewRoleResult(pkg)
	local tmp = GameServer_pb.Cmd_Sc_NewRole();
	tmp:ParseFromString(pkg)
	
	if (tmp.iResult == GameServer_pb.en_NewRole_OK)then	
		Log("new role suc")
		--Packet_Cmd(GameServer_pb.CMD_GETROLE);
		UI_CloseCurBaseWidget();
		CreateRole_Fifth()
		TBTSDK:GetInstance().isCreateNewRole = true;
	elseif (tmp.iResult == GameServer_pb.en_NewRole_DunplicateName) then
		CreateRole_Create();
	else
		Log("new role error")
		Login_BackToMaiMenu();
	end
end

g_cgIndex = 1;
g_animIndex = 0;
g_kaichangAnim=nil;

function Kaichang_AnimEnd(armature, movementType, movementID)
	if (movementType == LOOP_COMPLETE)then
		if (g_animIndex == 0 or g_animIndex == 2 or g_animIndex == 3)then
			g_animIndex = g_animIndex + 1;
			g_kaichangAnim:getAnimation():playWithIndex(g_animIndex)
			
			if (g_animIndex == 1 or g_animIndex == 4)then
				local widget = UI_GetBaseWidgetByName("Cg");
				if (widget)then
					UI_GetUIImageView(widget, 3):setVisible(true)
				end
			end
		elseif (g_animIndex == 5)then
			UI_CloseCurBaseWidget(EUICloseAction_FadeOut);
			CreateRole_Second();
		end
	end
end

function CreateRole_First()
	g_cgIndex = 1;
	
	local widget = UI_CreateBaseWidgetByFileName("Cg.json");
	local bg = UI_GetUIImageView(widget, 1);
	--bg:loadTexture(g_firstImg[g_cgIndex]);
	
	--[[g_kaichangAnim = GetUIArmature("Kaichang")
	g_kaichangAnim:getAnimation():playWithIndex(g_animIndex)
	g_kaichangAnim:getAnimation():setMovementEventCallFunc(Kaichang_AnimEnd);
	g_kaichangAnim:setPosition(ccp(480, 320));
	widget:addNode(g_kaichangAnim);]]
	bg:setColor(ccc3(0, 0, 0));
	bg:setTouchEnabled(true);
	--widget:removeChildByTag(1000)
	local info = CompLabel:GetDefaultCompLabel(FormatString("CommonDelayLabelCg", g_firstStr[g_cgIndex]), 760);
	info:setPosition(ccp(100, 380));
	widget:addChild(info, 2, 1000);
	--UI_GetUIImageView(widget, 3):setVisible(false)
	local function ClickScrene(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
            local function nextInfo()           
			    g_cgIndex = g_cgIndex + 1
			    widget:removeChildByTag(1000)
			    if (g_cgIndex > #g_firstStr)then
				    CreateRole_Second();
				    return;
			    end
                widget:removeChildByTag(1000)
			    info = CompLabel:GetDefaultCompLabel(FormatString("CommonDelayLabelCg", g_firstStr[g_cgIndex]), 760);
			    info:setPosition(ccp(100, 380));
			    widget:addChild(info, 2, 1000);
            end 
            if info ~= nil then
                local isShowAll = CompLabel:IsShowAll(info)
                if isShowAll == true then
                    nextInfo()
                else
                    widget:removeChildByTag(1000)
                    info = CompLabel:GetDefaultCompLabel(FormatString("CommonDelayLabelCg1", g_firstStr[g_cgIndex]), 760);
			        info:setPosition(ccp(100, 380));
			        widget:addChild(info, 2, 1000);
                    info = nil
                end
            else
                nextInfo()
            end
		end
	end
	
	bg:addTouchEventListener(ClickScrene)
end

function CreateRole_Second()
	UI_CloseCurBaseWidget(EUICloseAction_None);
	FightMgr:GetInstance():GetMainState():ResetState(EFightState_Init, 300);
end

function CreateRole_Third()
	if table.getn(g_thirdStr) == 0 then
		CreateRole_Fourth();
		return;
	end

	g_cgIndex = 1
	local widget = UI_CreateBaseWidgetByFileName("Cg.json");
	
	local bg = UI_GetUIImageView(widget, 1);
	bg:setColor(ccc3(0, 0, 0));
	bg:setTouchEnabled(true);
	
	local info = CompLabel:GetDefaultCompLabel(FormatString("CommonDelayLabelCg", g_thirdStr[g_cgIndex]), 760);
	info:setPosition(ccp(100, 380));
	widget:addChild(info, 2, 1000);
	
	local function ClickScrene(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			g_cgIndex = g_cgIndex + 1
			widget:removeChildByTag(1000)
			if (g_cgIndex > #g_thirdStr)then
				CreateRole_Fourth();
				return;
			end

			local info = CompLabel:GetDefaultCompLabel(FormatString("CommonDelayLabelCg", g_thirdStr[g_cgIndex]), 760);
			info:setPosition(ccp(100, 380));
			widget:addChild(info, 2, 1000)
		end
	end
	
	bg:addTouchEventListener(ClickScrene)
end

function CreateRole_AddTouchEvent()
	local widget = UI_GetBaseWidgetByName("Cg");
	local bg = UI_GetUIImageView(widget, 1);
	bg:setVisible(true);
	widget:removeNodeByTag(100);
	widget:removeNodeByTag(99);
	bg:setTouchEnabled(true);
	
	local img = UI_GetUIImageView(widget, 2);
	img:setVisible(true);
	local function ClickScrene(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			if (g_cgIndex < #g_thirdImg)then
				g_cgIndex = g_cgIndex + 1
				bg:loadTexture(g_thirdImg[g_cgIndex])
			else
				img:setVisible(false);
				CreateRole_Fourth();
			end
		end
	end
	
	bg:addTouchEventListener(ClickScrene)
end

function CreateRole_Fourth()
	CreateRole_Create()
end

function CreateRole_Fifth()
	Packet_Cmd(GameServer_pb.CMD_GETROLE);
	ShowWaiting();
	--[[
	g_cgIndex = 1
	local widget = UI_GetBaseWidgetByName("Cg");
	local bg = UI_GetUIImageView(widget, 1);
	bg:loadTexture(g_fifthImg[g_cgIndex]);
	bg:setTouchEnabled(true);
	local function ClickScrene(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			if (g_cgIndex < #g_fifthImg)then
				g_cgIndex = g_cgIndex + 1
				bg:loadTexture(g_fifthImg[g_cgIndex])
			else
				Packet_Cmd(GameServer_pb.CMD_GETROLE);
				ShowWaiting();
			end
		end
	end
	
	bg:addTouchEventListener(ClickScrene)]]
end

function CreateRole_Start()
	Log("CreateRole_Start")
	Packet_Cmd(GameServer_pb.CMD_QUERY_FIRSTFIGHT);
end

ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_GETROLE, "CreateRole_GetRoleResult" );
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_CHECK_ACTORNAME, "CreateRole_CheckName" );
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_NEW_ROLE, "CreateRole_NewRoleResult" );

















