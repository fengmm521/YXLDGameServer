if (g_selectSectionIndex == nil)then
	g_selectSectionIndex = -1;
end

if (g_selectiSceneID == nil)then
	g_selectiSceneID = -1;
end

g_dungeNotifyChallenge = false;



-- 注册全局Update
GetGlobalEntity():GetScheduler():RegisterInfiniteScript( "Dungeon_NotifyUpdate", 300) 


local function Dungeon_NotifyCheck()
	local widget = UI_GetBaseWidgetByName("Dungeon");
	if widget == nil or UIMgr:GetInstance():IsCurBaseWidget(widget)==false then
		CloseDungeonChallengeNotify();
		return;
	end
	
	-- 显示吧
	local playerLvl =1
	if GetLocalPlayer() then
		playerLvl = GetLocalPlayer():GetInt(EPlayer_Lvl);
	end
	
	if playerLvl < 5 then
		if g_isGuide== false and g_dungeNotifyChallenge == false and UI_GetUIButton(widget, 4):isTouchEnabled() then
			g_dungeNotifyChallenge = true;
			local worldPosition  = UI_GetUIButton(widget, 4):getWorldPosition()
			g_arrowLayout:setVisible(true);
			g_arrowLayout:setPosition(worldPosition);
		end
	end
end

function Dungeon_NotifyUpdate()
	-- 如果FB界面不不是当前界面，隐藏，否则按照正常逻辑处理
	Dungeon_NotifyCheck();
end




function CloseDungeonChallengeNotify()
	-- 有引导在不隐藏
	if g_dungeNotifyChallenge  then
		g_dungeNotifyChallenge = false;
		
		if g_isGuide == false then
			g_arrowLayout:setVisible(false);
		end
	end
end

function Dungeon_ListCreate(playAction)
	g_IsEquipFuben = false;
	local widget = UI_CreateBaseWidgetByFileName("DungeonList.json");
	if (widget == nil)then
		Log("Dungeon_ListCreate error");
		return;
	end
	
	local closeButton = UI_GetUIButton(widget, 1);
	closeButton:addTouchEventListener(UI_ClickCloseCurBaseWidgetWithNoAction);
	UI_GetUILayout(widget, 4755900):addTouchEventListener(PublicCallBackWithNothingToDo)
	--GetNoticeFlag()
	local sceneDataList = CDataManager:GetInstance():GetGameDataKeyList(DataFileSceneSection);
	
	local function ClickSceneSection(sender,eventType) 
		if  (eventType == TOUCH_EVENT_ENDED) then
			local img = tolua.cast( sender, "Button" );
			tag = img:getTag() - 1;
			if (Dungeon_GetPrerfectPass(sceneDataList[tag]) >= 0)then
				local tab = GameServer_pb.Cmd_Cs_QueryDSectionDetail();
				tab.iSectionID = sceneDataList[tag];
				if (g_selectSectionID ~= sceneDataList[tag])then
					g_selectSectionIndex = -1;
				end
				g_selectSectionID = sceneDataList[tag];
				
				Packet_Full(GameServer_pb.CMD_QUERY_DSECTIONDETAIL, tab);
				
				ShowWaiting();
			else
				createPromptBoxlayout(FormatString("DungeonNotOpen"))
			end
		end
	end 
	local size = nil;
	local scrollView = UI_GetUIScrollView(widget, 2);
	local openIndex = 0;
	for i=0, 32 do
		print("(i+1)========="..(i+1))
		local data = GetGameData(DataFileSceneSection, sceneDataList[i], "stSceneSectionData");
		local layout = UI_GetUIButton(scrollView, i + 1);
		local dungeonInfo = Dungeon_GetDungeonInfo(sceneDataList[i])
		layout:removeAllNodes();
		if (layout:getChildByTag(11) == nil)then
			for i=1, 3 do
				local starImg = ImageView:create()
				starImg:loadTexture("Dungeon/Bg_star_01.png")
				starImg:setPosition(ccp((i-2)*30, math.abs(i-2)*5-40))
				layout:addChild(starImg, 10, 10 + i)
			end
		end
		
		if (Dungeon_GetPrerfectPass(sceneDataList[i]) >= 0)then
			--[[if (Dungeon_GetPrerfectPass(sceneDataList[i]) > 0)then
				local shuiYin = ImageView:create();
				shuiYin:loadTexture("zhuxianfuben/ico_01.png");
				shuiYin:setPosition(ccp(0,0));
				img:addChild(shuiYin);
			end
			]]
			openIndex = i;
			layout:removeColorGrayWithButton();
		else
			--[[local shuiYin = ImageView:create();
			shuiYin:loadTexture("zhuxianfuben/ico_02.png");
			shuiYin:setPosition(ccp(0,0));
			img:addChild(shuiYin);]]
			layout:addColorGrayWithButton();
		end
		
		for i=1, 3 do
			local starImg = UI_GetUIImageView(layout, 10 + i);
			starImg:setVisible(dungeonInfo ~= nil)
			if (dungeonInfo)then
				if (dungeonInfo.iStar >= i) then
					starImg:loadTexture("Dungeon/Bg_star_02.png")
				else
					starImg:loadTexture("Dungeon/Bg_star_01.png")
				end
			end	
		end

		UI_GetUILabel(layout, 1):setText(data.m_name)
		
		--img:setPosition(ccp(100, 200))
		--img:setName("zhangjie_"..(i+1))
		layout:addTouchEventListener(ClickSceneSection);
		
		if (i == sceneDataList:size()-1) then
			layout = CCSizeMake(layout:getPositionX(), 300)
		end
		
		--[[local titleLab = CompLabel:GetDefaultLabWithSize(data.m_name, EFontSize_Big);
		--titleLab:setText(data.m_name);
		titleLab:setColor(ccc3(156, 89, 90));
		titleLab:setPosition(ccp(80,240));
		layout:addChild(titleLab, 1);]]
		
		--listView:pushBackCustomItem(layout);
	end
	--listView:refreshView();
	if (openIndex >= 0)then
		local layout = UI_GetUIButton(scrollView, openIndex + 1);
		local armature = GetUIArmature("zhangjie")
		armature:getAnimation():playWithIndex(0)
		armature:setScale(0.6)
		layout:addNode(armature, 100);
		
		size = scrollView:getInnerContainerSize();
		local percent = (layout:getPositionX() - 400) / (size.width - 400) * 100
		if (percent < 0)then
			percent = 0
		end
		scrollView:jumpToPercentHorizontal(percent)
	end

	--UI_SetLabelText(widget, 9, FormatString("HeroFightValue", GetPlayerFightValue()));
	--UI_SetLabelText(widget, 8, GetLocalPlayer():GetEntityName());
	
	--[[local tiliButton = UI_GetUIButton(widget, 3);
	tiliButton:addTouchEventListener(ClickBuyTili);
	
	function ClickFormation(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			Formation_Create();
		end
	end
	
	local formationButton = UI_GetUIButton(widget, 5);
	formationButton:addTouchEventListener(ClickFormation);

	local function HeroList(sender,eventType) 
		if (eventType == TOUCH_EVENT_ENDED) then
			--HeroList_Create();
			local heroList = GetLocalPlayer():GetHeroList()
			g_SelectIndex = 0
			local tab = GameServer_pb.Cmd_Cs_GetHeroDesc()
		    local heroEntity = GetEntityById(heroList[0], "Hero")
			tab.dwObjectID = heroEntity:GetUint(EHero_ID)
			Packet_Full(GameServer_pb.CMD_GET_HERODESC, tab)
			ShowWaiting()
		end
	end 
	local heroListButton = UI_GetUIButton(widget, 102);
	heroListButton:addTouchEventListener(HeroList);
	--if (playAction == true) then
	--	UIMgr:GetInstance():PlayOpenAction(widget, EUIOpenAction_Reel, 0.4, 0);
	--end
	--bar:setPercent(ToInt(tili*100/tiliMax));
	
	--local heroEntity = GetEntityById(GetLocalPlayer():GetHeroList()[0], "Hero");
	--local data = heroEntity:GetHeroData();
			
	--local icon = UI_GetHeroIcon(data.m_icon);
	--UI_GetUIImageView(widget, 11):addChild(icon);


	--任务
	local function ClickTaskFunc(sender,eventType)
		-- body
		if eventType == TOUCH_EVENT_ENDED then
		   --UI_CloseAllBaseWidget()
		   Packet_Cmd(GameServer_pb.CMD_TASK_QUERY_TASKINFO)
           ShowWaiting()
		end
	end
	UI_GetUIButton(widget,11):addTouchEventListener(ClickTaskFunc)
	--]]
	DungeonList_Refresh(widget);
	
	--UIMgr:GetInstance():PlayOpenAction(widget, EUIOpenAction_MoveIn_Right, 0.4, 0);
end

function DungeonList_Refresh(widget)
	--[[local tili = GetLocalPlayer():GetUint(EPlayer_Tili);
	local tiliMax = GetLocalPlayer():GetUint(EPlayer_TiliMax);
	
	UI_SetLabelText(widget, 10, ""..tili.."/"..tiliMax);
	UI_SetLabelText(widget, 1, UI_GetMoneyStr(GetLocalPlayer():GetUint(EPlayer_Silver)));
	UI_SetLabelText(widget, 2, UI_GetMoneyStr(GetLocalPlayer():GetUint(EPlayer_Gold)));]]
end

if (g_selectSectionID == nil)then
	g_selectSectionID = 0;
end


function Dungeon_Create()
	EndWaiting();
	Log("Dungeon_Create")
	local widget = UI_GetBaseWidgetByName("Dungeon");
	if (widget == nil)then
		widget = UI_CreateBaseWidgetByFileName("Dungeon.json");
        Refresh_SlectIndex()
	end
	UI_GetUILayout(widget, 4755899):addTouchEventListener(PublicCallBackWithNothingToDo)
	local function ClickClose(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			UI_CloseCurBaseWidget()
            if g_HeroSystem_IsHeroSystem then
               local heroEntity = HeroListSecond_GetHeroEntityByIndex()
               HeroCurInfoSecond_CreateHeroCurLayout(heroEntity) 
			else 
               Dungeon_ListCreate();
            end 
			
			CloseDungeonChallengeNotify();
            Refresh_SlectIndex()
		end
	end
	
	local closeButton = UI_GetUIButton(widget, 1);
	closeButton:addTouchEventListener(ClickClose);
	
	--UI_GetUIButton(widget, 200):addTouchEventListener(UI_ClickCloseCurBaseWidgetWithNoAction);
	
	local function CloseDungeonWidgetNotify(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			CloseDungeonChallengeNotify();
			UI_ClickCloseCurBaseWidgetWithNoAction(sender, eventType);
            if g_HeroSystem_IsHeroSystem then 
               g_HeroSystem_IsHeroSystem = false
            end 
            Refresh_SlectIndex()
		end
	end
	
	UI_GetUIButton(widget, 200):addTouchEventListener(CloseDungeonWidgetNotify);
	
	local dungeonInfo = Dungeon_GetDungeonInfo(g_selectSectionID)
	--GetNoticeFlag()
	
	local function ClickTiaozhan(sender,eventType)
		if eventType == TOUCH_EVENT_ENDED then
		   local tab = GameServer_pb.Cmd_Cs_DungeonFight();
			tab.iSceneID = g_curDungeonInfo[g_selectSectionIndex].iSceneID;
			g_openUIType = EUIOpenType_Dugeon;
			Packet_Full(GameServer_pb.CMD_DUNGEON_FIGHT, tab);
			ShowWaiting();
            --在战斗开始前保存主公的等级和经验，在结算的时候使用
            g_Fight_lv = GetLocalPlayer():GetInt(EPlayer_Lvl)          
            g_Fight_expNow = GetLocalPlayer():GetInt(EPlayer_Exp)
            g_Fight_expMax = GetLocalPlayer():GetInt(EPlayer_MaxExp)
			ShowWaiting();
			CloseDungeonChallengeNotify();
		end
	end
	UI_GetUIButton(widget, 4):addTouchEventListener(ClickTiaozhan)
	
	local function ClickSaodang(sender,eventType)
		if eventType == TOUCH_EVENT_ENDED then
			local tab = GameServer_pb.CMD_DUNGEON_SAODANG_CS();
			tab.iSceneID = g_curDungeonInfo[g_selectSectionIndex].iSceneID;
			Packet_Full(GameServer_pb.CMD_DUNGEON_SAODANG, tab);
			ShowWaiting();
			CloseDungeonChallengeNotify();
		end
	end
	UI_GetUIButton(widget, 3):addTouchEventListener(ClickSaodang)
	
	local index = Dungeon_Refresh(widget)
	
	UI_GetUIListView(widget, 11):jumpContainerToSelectedIndex(index - 2);
	
	--UIMgr:GetInstance():PlayOpenAction(widget, EUIOpenAction_MoveIn_Right, 0.4, 0);
    --左右更换章节
    local function change(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            local tag = tolua.cast(sender,"Button"):getTag()
            local nextID = g_selectSectionID
            if tag == 1124781087 then
                nextID = nextID-1
            elseif tag == 1124781089 then
                nextID = nextID+1
            end
            if nextID<g_dungeonList[1].iSectionID then
                return 
            else

            end  
            print("g_selectSectionID============================"..g_selectSectionID)
            if Dungeon_GetPrerfectPass(nextID) >= 0 then

                local tab = GameServer_pb.Cmd_Cs_QueryDSectionDetail();
                g_selectSectionID = nextID
		        tab.iSectionID = g_selectSectionID;
				
		        Packet_Full(GameServer_pb.CMD_QUERY_DSECTIONDETAIL, tab);
				
		        ShowWaiting();
            else
                createPromptBoxlayout(FormatString("DungeonNotOpen"))
            end
        end
    end
    UI_GetUILayout(widget, 1124781087):addTouchEventListener(change)
    UI_GetUILayout(widget, 1124781089):addTouchEventListener(change)
    UI_GetFadeInAndOutAction(UI_GetUIButton(widget, 4681161))
    UI_GetUIButton(widget, 4681161):setVisible(true)
    UI_GetFadeInAndOutAction(UI_GetUIButton(widget, 4681159))
    UI_GetUIButton(widget, 4681159):setVisible(true)
    UI_GetUILayout(widget,1124781087):setTouchEnabled(true)
    UI_GetUILayout(widget,1124781089):setTouchEnabled(true)	
end
--物品的提示消息----------------------------------------------------------
function AddItemTip(id,down)
        
        local tipWidget = UI_CreateBaseWidgetByFileName("UIExport/DemoLogin/ItemTip.json")
        tipWidget:addTouchEventListener(UI_ClickCloseCurBaseWidgetWithNoAction)

        local layout = UI_GetUILayout(tipWidget, 4674185)
        local itemData = GetGameData(DataFileItem, id, "stItemData")

        if down == nil then
            down =0
        end
        local image = UI_GetUIImageView(tipWidget, 4521681)
        image:setPosition(ccp(image:getPositionX(),image:getPositionY()-down))

                --icon
        UI_GetUIImageView(layout, 4521683):addChild(UI_ItemIconFrame(UI_GetItemIcon(itemData.m_icon), itemData.m_quality))
                --name
        UI_GetUILabel(layout, 4521687):setText( itemData.m_name )
        UI_GetUILabel(layout, 4521687):setColor( Util_GetColor(itemData.m_quality) )
        if #itemData.m_name>18 then
            UI_GetUILabel(layout, 4521687):setFontSize(24)
        elseif #itemData.m_name>21 then
            UI_GetUILabel(layout, 4521687):setFontSize(20)
        elseif #itemData.m_name>24 then
            UI_GetUILabel(layout, 4521687):setFontSize(18)
        else
            UI_GetUILabel(layout, 4521687):setFontSize(24)
        end
            --price
        UI_GetUILabel(layout, 4521693):setText( itemData.m_sellMoney )
        UI_GetUILabel(layout, 4521693):setVisible( itemData.m_sellMoney > 0 )
        UI_GetUILabel(layout, 4521689):setVisible( itemData.m_sellMoney > 0 )

        local listView = UI_GetUIListView(image, 4674188)
        listView:removeAllItems()
        layout:removeFromParent()
        listView:pushBackCustomItem(layout)

        if (itemData.m_ID>=4001 and itemData.m_ID<=4123) then --喜好品和装备等有属性的道具
            --属性
            local lab = UI_GetUILabel(tipWidget, 4674190)
            lab:removeFromParent()
            lab:setText(itemData.m_attinfo)

            print("lab:getContentSize().height=====1111============"..lab:getContentSize().height)
            
            --CompLabel:SetFontSize(10)
            local desclab = CompLabel:GetDefaultCompLabel(itemData.m_desc,270)
            desclab:setColor(ccc3(255,203,31))

            listView:pushBackCustomItem(lab)
            listView:pushBackCustomItem(desclab)

            local height = layout:getSize().height+lab:getSize().height+desclab:getSize().height+20
            image:setSize(CCSizeMake(300 , height))
            --坐标修正
            down = (350 - height)/2
            image:setPosition(ccp(image:getPositionX(),image:getPositionY()-down))
        else --没有属性的道具
             --描述
            local infolab = CompLabel:GetDefaultCompLabel(itemData.m_desc,270)
            listView:pushBackCustomItem(infolab)

            local height = layout:getSize().height+infolab:getSize().height+20
            image:setSize(CCSizeMake(300 , height))
            --坐标修正
            down = (350 - height)/2
            image:setPosition(ccp(image:getPositionX(),image:getPositionY()-down))
        end

        --image:setPosition(ccp(260+tag*100,300))
end
function Dungeon_Refresh(widget)
    EndWaiting()
    if g_IsEquipFuben then 
       return 
    end 
    print("g_selectSectionIDRefrehs=================="..g_selectSectionID)
	local jumpIndex = 1;
	local dungeonInfo = Dungeon_GetDungeonInfo(g_selectSectionID)
	--GetNoticeFlag()
	
	local sceneSectionData = GetGameData(DataFileSceneSection, g_selectSectionID, "stSceneSectionData");
	UI_GetUILabel(widget, 6):setText(sceneSectionData.m_nameFirst.." "..sceneSectionData.m_name);
	local infoLayout = UI_GetUIImageView(widget, 5);
	local sdBtn = UI_GetUIButton(widget, 3)
	--UI_GetUILabel(sdBtn, 1):setVisible(g_dungeonInfo.bSaoDangHaveLimit);
	local function LeftTimeEnd(sender, eventType)
		local tab = GameServer_pb.Cmd_Cs_QueryDSectionDetail();
		tab.iSectionID = g_selectSectionID;
				
		Packet_Full(GameServer_pb.CMD_QUERY_DSECTIONDETAIL, tab);
	end
	if (g_dungeonInfo.bSaoDangHaveLimit)then
		if (g_dungeonInfo.bShowSaoDangCD)then
			UI_GetUILabel(sdBtn, 1):setTimer(g_dungeonInfo.iSaoDangCDSecond, TimeFormatType_HourMinSec)
			UI_GetUILabel(sdBtn, 1):addEventListenerTimer(LeftTimeEnd);
		else
			UI_GetUILabel(sdBtn, 1):stopTimer()
			UI_GetUILabel(sdBtn, 1):setText(""..g_dungeonInfo.iLeftSaoDangCount.."/"..g_dungeonInfo.iSaoDangLimitCount)
		end
    else 
       UI_GetUILabel(sdBtn, 1):setText(FormatString("DungeonLimit"))
	end
	local function Refresh(tag)
		Log("tag"..tag)
        tag = tag+1
        repeat 
            tag = tag -1 
        until tag <= #g_curDungeonInfo
        if tag <= 0 then
            tag =1
        end
		local data = GetGameData(DataFileScene, g_curDungeonInfo[tag].iSceneID, "stSceneData");
		local dungeonInfo = g_curDungeonInfo[tag];
		UI_SetLabelText(infoLayout, 3, ""..g_dungeonInfo.iCostPhystrength);
		UI_SetLabelText(infoLayout, 2, "");
		local infoLabel = CompLabel:GetDefaultCompLabel(data.m_desc, 520);
		UI_GetUILabel(infoLayout, 2):removeAllChildren();
		UI_GetUILabel(infoLayout, 2):addChild(infoLabel);
		
		if (dungeonInfo.bHasPassLimit)then
			UI_SetLabelText(infoLayout, 4, ""..dungeonInfo.iLeftCount.."/"..dungeonInfo.iTotalCount);
		else
			UI_SetLabelText(infoLayout, 4, FormatString("DungeonLimit"));
		end
		UI_SetLabelText(infoLayout, 1, data.m_name);
		
		sdBtn:setTouchEnabled(dungeonInfo.bIsOpend)
		UI_GetUIButton(widget, 4):setTouchEnabled(dungeonInfo.bIsOpend)
		
		if dungeonInfo.bIsOpend == false  then
			CloseDungeonChallengeNotify();
		end
        
		local itemList = UI_Split(data.m_drop, "#");

        local function AddItemTipcall(sender,eventType)   
            if eventType == TOUCH_EVENT_ENDED then
                local tag = tolua.cast(sender,"ImageView"):getTag() - 1200
		        AddItemTip(TypeConvert:ToInt(itemList[tag]))
            end
        end
        -------------------------------------------------------------------------
		
		for i=1, 5 do
			UI_GetUIImageView(infoLayout, 10 + i):removeAllChildren()
			if (i > #itemList)then
				UI_GetUIImageView(infoLayout, 10 + i):setVisible(false);
			else
				UI_GetUIImageView(infoLayout, 10 + i):setVisible(true);
				local id =  TypeConvert:ToInt(itemList[i])
				if (id >= 30000)then
					local equipData = GetGameData(DataFileEquip, TypeConvert:ToInt(itemList[i]), "stEquipData")
					UI_GetUIImageView(infoLayout, 10 + i):addChild(UI_ItemIconFrame(UI_GetItemIcon(equipData.m_icon), equipData.m_quality))
				else
					Log("itemList[i]"..itemList[i])
					local itemData = GetGameData(DataFileItem, TypeConvert:ToInt(itemList[i]), "stItemData")
                    local icon = UI_ItemIconFrame(UI_GetItemIcon(itemData.m_icon), itemData.m_quality)
                    icon:setScale(80 / 92)
					UI_GetUIImageView(infoLayout, 10 + i):addChild(icon,2,i+1200)

                    icon:setTouchEnabled(true)
                    icon:addTouchEventListener(AddItemTipcall)
				end
			end
		end
		
		--[[if (g_selectSectionIndex > tag)then
			--UIMgr:GetInstance():PlayOpenAction(infoLayout, EUIOpenAction_MoveIn_Top, 0.6, 0)
			UIMgr:GetInstance():PlayOpenAction(sdBtn, EUIOpenAction_Move_Right_Left, 0.6, 0)
			UIMgr:GetInstance():PlayOpenAction(UI_GetUIButton(widget, 4), EUIOpenAction_Move_Right_Left, 0.6, 0)
		elseif (g_selectSectionIndex < tag)then
			--UIMgr:GetInstance():PlayOpenAction(infoLayout, EUIOpenAction_MoveIn_Bottom, 0.6, 0)
			UIMgr:GetInstance():PlayOpenAction(sdBtn, EUIOpenAction_Move_Right_Left, 0.6, 0)
			UIMgr:GetInstance():PlayOpenAction(UI_GetUIButton(widget, 4), EUIOpenAction_Move_Right_Left, 0.6, 0)
		end]]
		
		g_selectSectionIndex = tag;
	end
	
	local lastMoveLayout = nil;
	
	local function MoveAction(moveLayout)
		if (lastMoveLayout)then
			lastMoveLayout:setPosition(ccp(0, 0))
			lastMoveLayout:stopAllActions()
		end
	
		local moveAction = CCEaseBounceOut:create(CCEaseSineOut:create(CCMoveBy:create(0.8, ccp(118, 0))));
		--local moveReverse = CCEaseBounceOut:create(CCEaseSineOut:create(CCMoveBy:create(0.8, ccp(-118, 0))));
		local arryaction = CCArray:create()
		arryaction:addObject(CCDelayTime:create(0.1))
		arryaction:addObject(moveAction)
		--arryaction:addObject(CCDelayTime:create(0.6))
		--arryaction:addObject(moveReverse)
		local seqaction = CCSequence:create(arryaction)
		--local reaction = CCRepeatForever:create(seqaction)
		moveLayout:runAction(seqaction);
		
		lastMoveLayout = moveLayout;
	end

	local function ClickScene(sender,eventType) 
		if  (eventType == TOUCH_EVENT_ENDED) then
			local img = tolua.cast( sender, "Layout" );
			tag = img:getTag();
			if (g_selectSectionIndex ~= tag)then
				local moveLayout = UI_GetUILayout(img:getChildByTag(5), 1);
                --如果此关卡未开启
                if g_curDungeonInfo[tag].bIsOpend == false then
                    UI_GetUIImageView(moveLayout, 4555596):loadTexture("UIExport/DemoLogin/Dungeon/Bg_dungeonList_NotOpen.png")
                else
                    UI_GetUIImageView(moveLayout, 4555596):loadTexture("UIExport/DemoLogin/Dungeon/Bg_dungeonList_Select.png")
                end

				MoveAction(moveLayout)
				Refresh(tag)
			end
		end
	end 
	local listView = UI_GetUIListView(widget, 11);
	
	--local sceneDataList = CDataManager:GetInstance():GetGameDataKeyList(DataFileSceneSection);
	local tempLayout = Layout:create();
    --tempLayout:setTouchEnabled(true);
	tempLayout:setSize(CCSizeMake(220, 160));
	
	local cloneLayout = UI_GetUILayout(widget, 100);
	cloneLayout:setPosition(ccp(0, 0))
	
	local isNewOpen = false;
	listView:removeAllItems();
	listView:setItemModel(tempLayout);
	
	local isPass = true;
	if (g_selectSectionIndex == -1)then 
		for i=1, #g_curDungeonInfo do
			if (g_curDungeonInfo[i].bIsOpend == false) then	
				g_selectSectionIndex = i - 1;	
				break
			end
		end
		
		if (g_selectSectionIndex == -1)then
			g_selectSectionIndex = #g_curDungeonInfo
		end
	end
	
	
	for i=1, #g_curDungeonInfo do
		local layout = UI_GetCloneLayout(tempLayout);
		local img = UI_GetCloneLayout(cloneLayout);
		img:setVisible(true);
		img:addTouchEventListener(ClickScene);
		
		local data = GetGameData(DataFileScene, g_curDungeonInfo[i].iSceneID, "stSceneData");
		
		UI_GetUILabel(img, 6):setText(data.m_name)
		
		if (i == #g_curDungeonInfo) then
			UI_GetUIImageView(img, 1):loadTexture("Dungeon/Bnt_dungeonList_001.png");
		else
			UI_GetUIImageView(img, 1):loadTexture("Dungeon/Bnt_dungeonList_002.png");
		end
		
		local path = ""
		if data.m_icon >= 10 then
			path = "Dungeon/Bg_dungeonList_0"..data.m_icon..".png"
		else
			path = "Dungeon/Bg_dungeonList_00"..data.m_icon..".png"
		end

		UI_GetUIImageView(img:getChildByTag(5):getChildByTag(1), 1):loadTexture(path)

		if (g_curDungeonInfo[i].bIsOpend) then	
			img:setName("guanqia_"..i)
			
			local star = g_curDungeonInfo[i].iStarLevel
			if (star == 0)then
				isPass = false;
			end
			for j=1, 3 do
				local starSp = UI_GetUIImageView(img, j + 1);
				if (j <= star)then
					starSp:loadTexture("Dungeon/Bg_star_02.png");
				else
					starSp:loadTexture("Dungeon/Bg_star_01.png");
				end
			end
			
			if (g_curDungeonInfo[i].bNewOpen)then
				g_selectSectionIndex = i;
				g_curDungeonInfo[i].bNewOpen = false;
			end
			
			if (g_selectiSceneID == data.m_ID)then
				g_selectSectionIndex = i;
			end
		else	
			isPass = false;
			for j=1, 5 do
				if (j == 5)then
					UI_GetUIImageView(img:getChildByTag(5):getChildByTag(1), 1):addColorGray();
				else
					UI_GetUIImageView(img, j):addColorGray();
				end
			end	
		end
		
		if (i == g_selectSectionIndex)then
			local moveLayout = UI_GetUILayout(img:getChildByTag(5), 1);

            if g_curDungeonInfo[i].bIsOpend == false then
                UI_GetUIImageView(moveLayout, 4555596):loadTexture("UIExport/DemoLogin/Dungeon/Bg_dungeonList_NotOpen.png")
            else
                UI_GetUIImageView(moveLayout, 4555596):loadTexture("UIExport/DemoLogin/Dungeon/Bg_dungeonList_Select.png")
            end

			MoveAction(moveLayout)
		end
		
		layout:addChild(img, 0, i);
		listView:pushBackCustomItem(layout);
	end
	
	g_selectiSceneID = -1;
	
	Refresh(g_selectSectionIndex)
	
	jumpIndex = g_selectSectionIndex
	
	--UI_GetUIButton(widget, 6):setVisible(dungeonInfo.bGeted == false);
	
	local function ClickGodBox(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED)then
			if (dungeonInfo.bGeted == false)then
				Dungeon_GoldBox()
			end
		end
	end
	local godButton = UI_GetUIButton(widget, 101):clone();
	godButton:setVisible(true);
	godButton:addTouchEventListener(ClickGodBox);
	godButton:setPosition(ccp(110, 80))
	--godButton:setTouchEnabled(false)
	UI_GetUIImageView(godButton, 1):setVisible(dungeonInfo.bGeted);	
	local layout1 = UI_GetCloneLayout(tempLayout);
	layout1:addChild(godButton)
	listView:pushBackCustomItem(layout1);
	
	if (isPass)then
		local function ClickNextDungeon(sender, eventType)
			if (eventType == TOUCH_EVENT_ENDED)then
				local tab = GameServer_pb.Cmd_Cs_QueryDSectionDetail();
				g_selectSectionID = g_dungeonList[#g_dungeonList].iSectionID;
				Log("g_selectSectionID"..g_selectSectionID);
				tab.iSectionID = g_selectSectionID;
				g_selectSectionIndex = -1;
				Packet_Full(GameServer_pb.CMD_QUERY_DSECTIONDETAIL, tab);		
				ShowWaiting();
				
				UI_CloseCurBaseWidget(EUICloseAction_None);
			end
		end
		local nextButton = UI_GetCloneButton(UI_GetUIButton(widget, 102));
		nextButton:setVisible(true)
		nextButton:setPosition(ccp(110, 80))
		nextButton:addTouchEventListener(ClickNextDungeon);
		
		local layout = UI_GetCloneLayout(tempLayout);
		layout:addChild(nextButton)
		listView:pushBackCustomItem(layout);
		jumpIndex = jumpIndex + 1;
	end
	
	listView:refreshView();
	
	if (dungeonInfo.iIsPerfectPass > 0 and dungeonInfo.bGeted == false) then
		Dungeon_GoldBox()
		g_newPassSectionID = -1;
	--elseif (g_newPassSectionID == g_selectSectionID)then
	--	NextDungeon_Create();
	--	g_newPassSectionID = -1;
	end
	
	return jumpIndex;
end

function Refresh_SlectIndex()
   for i = 1, #g_curDungeonInfo do 
      if g_curDungeonInfo[i].bIsOpend == false then 
         g_selectSectionIndex = i - 1
         break
      end 
   end 
end

function Dungeon_GetPrerfectPass(id)
	for i=1,#g_dungeonList do
		if (g_dungeonList[i].iSectionID == id) then
			return g_dungeonList[i].iIsPerfectPass;
		end
	end
	
	return -1;
end

function Dungeon_GetDungeonInfo(id)
	for i=1,#g_dungeonList do
		if (g_dungeonList[i].iSectionID == id) then
			return g_dungeonList[i];
		end
	end
	
	return nil;
end

function OnDungeonList(pkg)
	local tmp = GameServer_pb.Cmd_Sc_DungeonSectionList();
	tmp:ParseFromString(pkg)
	Log("OnDungeonList"..tostring(tmp))
	for i=1, #tmp.szDungeonSection do
		g_dungeonList[#g_dungeonList + 1] = tmp.szDungeonSection[i];
	end
	
	g_selectSectionID = g_dungeonList[#g_dungeonList].iSectionID;
	for i=1,#g_dungeonList do
		if (g_dungeonList[i].iIsPerfectPass > 0 and g_dungeonList[i].bGeted == false)then
			g_selectSectionID = g_dungeonList[i].iSectionID;
		end
	end
end
if g_dungeonList == nil then
	g_dungeonList = {};
end

if g_curDungeonInfo == nil then
	g_curDungeonInfo = {};
end

function UpdateDungeonList(pkg)
	local tmp = GameServer_pb.Cmd_Sc_SectionUpdate();
	tmp:ParseFromString(pkg)
	
	Log("UpdateDungeonList"..tostring(tmp))
	for i=1, #g_dungeonList do
		if (g_dungeonList[i].iSectionID == tmp.updateInfo.iSectionID) then
			g_dungeonList[i] = tmp.updateInfo;
			return;
		end
	end
	g_dungeonList[#g_dungeonList+1] = tmp.updateInfo;
	
	UI_RefreshBaseWidgetByName("Dungeon");
end

if (g_dungeonInfo == nil)then
	g_dungeonInfo = {}
end

function OnQueryDungeonInfo(pkg)
	local tmp = GameServer_pb.Cmd_Sc_QueryDSectionDetail();
	tmp:ParseFromString(pkg)
	Log("OnQueryDungeonInfo"..tostring(tmp))
	for i=1, #tmp.szDungeonScene do
		--Log("tmp.szDungeonSection[i]..iSceneID ="..tmp.szDungeonScene[i].iSceneID)
	end
	
	g_dungeonInfo = tmp;
	g_curDungeonInfo = tmp.szDungeonScene;
	
	Log("OnQueryDungeonInfo---------------"..(tmp.iSectionID));
	g_IsEquipFuben = tmp.iSectionID >= 20000;

	if (UI_GetBaseWidgetByName("DungeonList"))then
		UI_CloseCurBaseWidget();
	end
	if g_IsEquipFuben then 
       --装备副本 
       if UI_GetBaseWidgetByName("Dungeon") then 
          EquipFuben_FubenlayoutRefresh(UI_GetBaseWidgetByName("Dungeon"))
       else 
          EquipFuben_InitFubenLayout()
       end 
    else 
       if (UI_GetBaseWidgetByName("Dungeon"))then
		  Dungeon_Refresh(UI_GetBaseWidgetByName("Dungeon"));
	   else
          if (g_curGuideStep ~= 1 or g_curGuideId==15)then
			    Dungeon_Create();
		  end
       end 
    end 
end

if (g_fightReslut == nil) then
	g_fightReslut = {};
end

function Fight_Win()
    Log("Win")
	GetCurScene():Resume(1000);
	--升级info
    if g_EquipCard_IsOpen == false then 
        if returnPlayerlevelupdateinfo() ~=nil then
           HeroLevelUpInfoShow()
        end
    else 
       Log("open")
       EquipCardTrun_InitLayout()
    end 
	local widget = UI_CreateBaseWidgetByFileName("FightWin.json");
	if (widget == nil)then
		Log("Fight_Win error");
		return;
	end
    print("====================================="..tostring(g_fightReslut))
	local awardPropList = g_fightReslut.awardResult.szAwardPropList;
	local awardItemList = g_fightReslut.awardResult.szAwardItemList;
	local awardHeroList = g_fightReslut.awardResult.szAwardHeroList
	
    --初始化收益---------------------------------------
    local lvLabel = UI_GetUILabel(widget, 4633463)
    local loadingBar = UI_GetUILoadingBar(widget, 4633466)
    if g_Fight_expNow ~= nil then--是副本
        --设置等级
        lvLabel:setText(g_Fight_lv)
        --经验
        loadingBar:setPercent(g_Fight_expNow*100/g_Fight_expMax)
    else--不是副本
        --设置等级
        lvLabel:setText(GetLocalPlayer():GetInt(EPlayer_Lvl))
        --经验
        loadingBar:setPercent(GetLocalPlayer():GetInt(EPlayer_Exp)*100/GetLocalPlayer():GetInt(EPlayer_MaxExp))
    end
    -- --获得的经验
    -- UI_GetUILabel(widget, 4633472):setText("+0")
    -- --获得的铜币
    -- UI_GetUILabel(widget, 4633482):setText("0")

    ----------------------------------------------------
    --lifeAtt类收益
    Log("#awardPropList===="..#awardPropList)
    if #awardPropList == 0 then 
       loadingBar:setPercent((GetLocalPlayer():GetInt(EPlayer_Exp) / GetLocalPlayer():GetInt(EPlayer_MaxExp)) * 100)
       local expnum = 0
       local expaddnum = 0
       
       UI_GetUILabelBMFont(widget, 1100):setText("+"..expnum)
	   UI_GetUILabelBMFont(widget, 1102):setText("+"..expaddnum)
	   
       UI_GetUILayout(widget,1107):setVisible(false)
    end 
    local playerVip = GetLocalPlayer():GetInt(EPlayer_VipLevel)
    UI_GetUILabelBMFont(widget, 1103):setText("VIP"..playerVip)
    for i=1, #awardPropList do  
        print("-------------")     
        if awardPropList[i].iLifeAttID == 3 and g_Fight_lv<120 then --主公经验
            local time = 2.0
            if g_Fight_expNow+awardPropList[i].iValue >= g_Fight_expMax then--如果升级了
                local function Add()
                    UI_GetUIImageView(widget, 4633487):setVisible(true)
                    lvLabel:setText(g_Fight_lv+1)
                end
                time = (g_Fight_expMax-g_Fight_expNow)/awardPropList[i].iValue*time
                
                percent = 101
                if g_Fight_expNow+awardPropList[i].iValue > g_Fight_expMax then--升级过后还有经验
                    percent = (g_Fight_expNow+awardPropList[i].iValue-g_Fight_expMax)*100/GetLocalPlayer():GetInt(EPlayer_MaxExp)+100
                end
                loadingBar:setPercentSlowly(percent,2.0)

                local actionArry = CCArray:create()
                local delayTime = CCDelayTime:create(time)
                actionArry:addObject(delayTime)
                local callback = CCCallFuncN:create(Add)
                actionArry:addObject(callback)

                local seqaction = CCSequence:create(actionArry)
                UI_GetUIImageView(widget, 4633487):runAction(seqaction)
            else
                loadingBar:setPercentSlowly((g_Fight_expNow+awardPropList[i].iValue)*100/g_Fight_expMax,10)
            end       
            --获得的经验
            local expnum = awardPropList[i].iValue - g_fightReslut.iExtraExp 
            local expaddnum = g_fightReslut.iExtraExp
            local playerVip = GetLocalPlayer():GetInt(EPlayer_VipLevel)
            UI_GetUILabelBMFont(widget, 1100):setText("+"..expnum)
			UI_GetUILabelBMFont(widget, 1102):setText("+"..expaddnum)
			--UI_GetUILabelBMFont(widget, 1103):setText("VIP"..playerVip)

        elseif awardPropList[i].iLifeAttID == 1 then --铜钱
            UI_GetUILabelBMFont(UI_GetUILayout(widget,1107), 1104):setText(awardPropList[i].iValue)
        elseif awardPropList[i].iLifeAttID == 4 then --修为
            local xiuwei = UI_GetUILayout(widget, 1108)
            xiuwei:setVisible(true)
            UI_GetUILabelBMFont(xiuwei, 1105):setText(awardPropList[i].iValue)
        elseif awardPropList[i].iLifeAttID == 20 then --荣誉
            local rongyu = UI_GetUILayout(widget, 1109)
            rongyu:setVisible(true)
            UI_GetUILabelBMFont(rongyu, 1106):setText(awardPropList[i].iValue)
        end
	end
    --英雄和道具的渐变动作
    local function getAction(callBack)
        local actionArry = CCArray:create() 
        local fadeIn = CCFadeIn:create(0.05)
        actionArry:addObject(fadeIn)
        local callback = CCCallFuncN:create(callBack)
        actionArry:addObject(callback)
        local seqaction = CCSequence:create(actionArry)
        return seqaction
    end
    --显示获得的道具
    local ItemNum = 1
    local function showGetItem()
        if ItemNum<=#awardItemList then
		--UI_GetUIImageView(widget, 10):setVisible(true);
            if awardItemList[ItemNum].iItemID ~= -1 then 
		        local img = UI_GetUIImageView(widget, ItemNum+10);
		        img:setVisible(true);
		        local data = nil
		        Log("awardItemList[ItemNum].iItemID"..awardItemList[ItemNum].iItemID)
                local icon = nil 
		        if (IsItem(awardItemList[ItemNum].iItemID))then
			        data = GetGameData(DataFileItem, awardItemList[ItemNum].iItemID, "stItemData");
                    icon = UI_ItemIconFrame(UI_GetItemIcon(data.m_icon, awardItemList[ItemNum].iCount), data.m_quality);
		        else
			        data = GetGameData(DataFileEquip, awardItemList[ItemNum].iItemID, "stEquipData");
                    icon = UI_ItemIconFrame(UI_GetEquipIcon(data.m_icon), data.m_quality)
		        end
		    
		        img:addChild(icon)

                local function AddItemTipcall(sender,eventType)
                    if eventType == TOUCH_EVENT_ENDED then
                       if (IsItem(awardItemList[ItemNum].iItemID))then
                           AddItemTip(awardItemList[ItemNum].iItemID,100)
                       else 
                           AddEquipTip(awardItemList[ItemNum].iItemID,100,awardItemList[ItemNum].dynamicEquipProp)
                       end 
                    end
                end
            
                icon:setTouchEnabled(true)
                icon:addTouchEventListener(AddItemTipcall)    
                ItemNum = ItemNum +1  
                img:runAction(getAction(showGetItem))
            else       
                ItemNum = ItemNum +1
                showGetItem()
            end
	    end
    end
    --显示获得的武魂
    local HeroSoulNum= 1
    local function showGetHeroSoul()
        if  HeroSoulNum <= #awardHeroList then
		    --UI_GetUIImageView(widget, 10):setVisible(true);
		    local img = UI_GetUIImageView(widget, i+10);
		    img:setVisible(true);
		    local data = GetGameData(DataFileHeroBorn, awardHeroList[i].iHeroID, "stHeroBornData");
		
		    local icon = UI_IconFrame(UI_GetHeroIcon(data.m_icon, awardHeroList[i].iCount), awardHeroList[i].iLevelStep, awardHeroList[i].iQuality);
		    img:addChild(icon)
            HeroSoulNum = HeroSoulNum+1
            img:runAction(getAction(showGetHeroSoul))
	    else
            showGetItem()
        end
    end
    --显示出战英雄
    local HeroListID = 1
    local formationherolist = vector_uint_:new_local()
    GetLocalPlayer():GetFormationHeroList(formationherolist)
    local function showHeroGet()    
        local szHeroAward = g_fightReslut.szHeroAward;
	    local layout = UI_GetUIImageView(widget, 20 + HeroListID);
        if layout and HeroListID <= formationherolist:size() then
	        layout:setVisible(true)
        end
        --如果有发送队伍信息
	    if HeroListID<=#szHeroAward then
		    local heroEntity = GetLocalPlayer():GetHeroByObjId(szHeroAward[HeroListID].dwHeroObjectID);
		    local data = heroEntity:GetHeroData();
		    local img = UI_IconFrame(UI_GetHeroIcon(data.m_icon, heroEntity:GetInt(EHero_Lvl)), heroEntity:GetInt(EHero_LvlStep), heroEntity:GetInt(EHero_Quality));
		    layout:addChild(img);
		
		    UI_GetUILoadingBar(layout, 1):setPercent(szHeroAward[HeroListID].iHeroExpPercent)
            UI_GetUILabelBMFont(layout, 120):setText("+"..g_fightReslut.iHeroGetExp)
		
		    local function AnimEnd(armature, movementType, movementID)
			    if (movementType == LOOP_COMPLETE)then
				    if (movementID == "0")then
					    armature:getAnimation():playWithIndex(1)
				    end
			    end
		    end
		
		    if (szHeroAward[HeroListID].bLevelUp)then
			    local armature = GetUIArmature("Effshegnji")
			    armature:getAnimation():playWithIndex(0)
			    armature:getAnimation():setMovementEventCallFunc(AnimEnd);
			    layout:addNode(armature);
		    end
            HeroListID = HeroListID+1
            layout:runAction(getAction(showHeroGet))
	    elseif (#szHeroAward == 0)then
	    --如果没有发送队伍信息
		    local inForMationList = vector_uint_:new_local()
		    GetLocalPlayer():GetFormationHeroList(inForMationList)
		    if HeroListID<= inForMationList:size() then 
			    local heroEntity = GetEntityById(inForMationList[HeroListID-1],"Hero")
			    if heroEntity then
				    local data = heroEntity:GetHeroData();
				    local img = UI_IconFrame(UI_GetHeroIcon(data.m_icon, heroEntity:GetInt(EHero_Lvl)), heroEntity:GetInt(EHero_LvlStep), heroEntity:GetInt(EHero_Quality));
				    layout:addChild(img);

				    local curExp = heroEntity:GetIntByName("HeroExp")
				    local herolevel = heroEntity:GetUint(EHero_Lvl)
				    local expdata = GetGameData(DataFileHeroLevelExp,herolevel,"stHeroLevelExpData")
				    local needExp = expdata.m_needxiuwei
	
				    UI_GetUILoadingBar(layout, 1):setPercent(ToInt(curExp*100/needExp))
                    UI_GetUILabelBMFont(layout, 120):setText("+0")
			    end
		    end
            HeroListID = HeroListID+1
            if HeroListID<= inForMationList:size() then  
               layout:runAction(getAction(showHeroGet))
            end 
	    else
            showGetHeroSoul()
        end
    end

    --结算界面进入
    local actionArry = CCArray:create() 
    local fadeIn = CCFadeIn:create(0.5)
    actionArry:addObject(fadeIn)
    local callback = CCCallFuncN:create(showHeroGet)
    actionArry:addObject(callback)
    local seqaction = CCSequence:create(actionArry)
    widget:runAction(seqaction)
	
	function ClickReplay(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
            InitFightRoundNumber()
			FightMgr:GetInstance():Replay();
		end
	end
	
	local replayButton = UI_GetUIButton(widget, 2);
	replayButton:addTouchEventListener(ClickReplay);
	
	function ClickLeave(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			FightMgr:GetInstance():GetMainState():ResetState(EFightState_End, 1);
		end
	end
	
	local leaveButton = UI_GetUIButton(widget, 3);
	leaveButton:addTouchEventListener(ClickLeave);
	--UI_GetUIImageView(widget, 20):setVisible(true);
end

function Fight_RemoveFightWinAnim()
	g_winAnimLayout:removeFromParent();
end

if (g_fightQuit == nil) then
	g_fightQuit = false;
end

function Fight_Lose()
	GetCurScene():Resume(1000);
	--升级info
    if returnPlayerlevelupdateinfo() ~=nil then
       HeroLevelUpInfoShow()
    end

	EndWaiting();
	local widget = UI_CreateBaseWidgetByFileName("FightLose.json", EUIOpenAction_MoveIn_Top, 1.5);
	if (widget == nil)then
		Log("Fight_Win error");
		return;
	end
	
	g_battleAutoFight = false;
	
	local function ClickFormation(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			SceneMgr:GetInstance():SwitchToMainUi();
			g_openUIType = EUIOpenType_Formation;
		end
	end
	
	UI_GetUIImageView(widget, 11):addTouchEventListener(ClickFormation);
	
	local function ClickHeroInfo(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			SceneMgr:GetInstance():SwitchToMainUi();
			g_openUIType = EUIOpenType_Hero;
		end
	end
	
	UI_GetUIImageView(widget, 12):addTouchEventListener(ClickHeroInfo);
	
	
	local function ClickGodAnim(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			SceneMgr:GetInstance():SwitchToMainUi();
			g_openUIType = EUIOpenType_GodAnim;
		end
	end
	
	UI_GetUIImageView(widget, 13):addTouchEventListener(ClickGodAnim);
	
	
	function ClickFightSoul(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			SceneMgr:GetInstance():SwitchToMainUi();
			g_openUIType = EUIOpenType_FightSoul;
		end
	end
	
	--[[UI_GetUIImageView(widget, 14):addTouchEventListener(ClickFightSoul);
	if (Math:HasFlag(mask, math.pow(2, GameServer_pb.en_Function_FightSoul)) == false)then
		UI_GetUIButton(widget, 4):setVisible(false);
		UI_GetUIImageView(widget, 14):setVisible(false);
	end]]
	
	function ClickReplay(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
            InitFightRoundNumber()
			FightMgr:GetInstance():Replay();
		end
	end
	
	local replayButton = UI_GetUIButton(widget, 5);
	replayButton:addTouchEventListener(ClickReplay);
	replayButton:setVisible(g_fightQuit == false);
	g_fightQuit = false;
	
	function ClickLeave(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			FightMgr:GetInstance():GetMainState():ResetState(EFightState_End, 1);
		end
	end
	
	local leaveButton = UI_GetUIButton(widget, 6);
	leaveButton:addTouchEventListener(ClickLeave);
	
	local layout = UI_GetUILayout(widget, 100);
	
	local maker = CompStringMaker:new_local();
	local awardPropList = g_fightReslut.awardResult.szAwardPropList;
	
	for i=1, #awardPropList do
		--Log("awardPropList[i].iLifeAttID"..awardPropList[i].iLifeAttID)
		maker:AppendSprite( "LifeAttIcon/s_"..awardPropList[i].iLifeAttID..".png", 0 , 1);
		maker:AppendString("  "..GetAttNameAndValue(awardPropList[i].iLifeAttID,awardPropList[i].iValue), CompTextArgs:new_local(255,255,255));

		maker:AppendString("   ", CompTextArgs:new_local(255,255,255));
	end
	local awarPropLab = CompLabel:GetDefaultCompLabel(maker:GetString(), 800);
	awarPropLab:setPosition(ccp(-awarPropLab:getSize().width/2, 50));
	layout:addChild(awarPropLab);

    function autoExit()
        FightMgr:GetInstance():GetMainState():ResetState(EFightState_End, 1);
    end
    if (g_openUIType == EUIOpenType_WorldBoss) then
        local actionArry = CCArray:create()
        local delayTime = CCDelayTime:create(5)
        actionArry:addObject(delayTime)
        local callback = CCCallFuncN:create(autoExit)
        actionArry:addObject(callback)

        local seqaction = CCSequence:create(actionArry)
        widget:runAction(seqaction)
    end
end

function Dungeon_GoldBox()
	local widget = UI_CreateBaseWidgetByFileName("GoldBox.json", EUIOpenAction_Enlarge);
	if (widget == nil)then
		Log("Dungeon_GoldBox error");
		return;
	end
    --UI_GetUILayout(widget, 4499630):addTouchEventListener(PublicCallBackWithNothingToDo)
	--UI_GetUIButton(widget, 5):addTouchEventListener(UI_ClickCloseCurBaseWidget);

	local dungeonInfo = Dungeon_GetDungeonInfo(g_selectSectionID)
	local function ClickGet(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			local tab = GameServer_pb.Cmd_Cs_GetSectionAward();
			tab.iSectionID = g_selectSectionID;
			Packet_Full(GameServer_pb.CMD_GET_SECTION_AWARD, tab);
			ShowWaiting();
		end
	end
	UI_GetUIButton(widget, 6):addTouchEventListener(ClickGet);
	
	local name = ""
	local icon = nil;
	
	for i=1, #dungeonInfo.szPerfectIDList do		
		local img = UI_GetUILayout(widget, i);
		img:setVisible(true);
		local awardData = GetGameData(DataFilePerfectPassAward, dungeonInfo.szPerfectIDList[i], "stPerfectPassAwardData");
		
		if (awardData.m_type == GameServer_pb.en_ShopType_LifeAttType)then
			icon = GetAttIcon(awardData.m_goodID);
			name = ""..UI_GetMoneyStr(awardData.m_count)..GetAttName(awardData.m_goodID);
		elseif (awardData.m_type == GameServer_pb.en_Shop_GoodType_Hero)then
			local data = GetGameData(DataFileHeroBorn, awardData.m_goodID, "stHeroBornData");
			name = GetHeroName(data.m_name, awardData.m_stepLvl);
			icon = UI_IconFrame(UI_GetHeroIcon(data.m_icon), awardData.m_stepLvl)
		elseif (awardData.m_type == GameServer_pb.en_Shop_GoodType_GodAnimal)then
			local data = GetGameData(DataFileGodAniaml, awardData.m_goodID, "stGodAnimalData");
			name = GetHeroName(data.m_name, awardData.m_stepLvl);
			icon = UI_IconFrame(UI_GetGodAnimalIcon(data.m_icon), awardData.m_stepLvl)
		elseif (awardData.m_type == GameServer_pb.en_Shop_GoodType_Item)then
			local data = nil;
			if (IsItem(awardData.m_goodID))then
				data = GetGameData(DataFileItem, awardData.m_goodID, "stItemData");
				name = FormatString("ItemName", UI_GetStringColorNew(data.m_quality), data.m_name);
				icon = UI_ItemIconFrame(UI_GetItemIcon(data.m_icon), data.m_quality)
			else
				data = GetGameData(DataFileEquip, awardData.m_goodID, "stEquipData");
				name = FormatString("EquipName", UI_GetStringColorNew(data.m_quality), data.m_name);
				icon = UI_ItemIconFrame(UI_GetItemIcon(data.m_icon), data.m_quality)
			end	
		end
		if (icon) then
			img:addChild(icon);
		end
		
		if (name ~= "")then
			local nameLabel = CompLabel:GetDefaultCompLabel(name, 150);
			nameLabel:setPosition(ccp(-nameLabel:getContentSize().width / 2, -50));
			img:addChild(nameLabel);
		end
	end
end

function OpenFightResultUi()
	UIMgr:GetInstance():CloseAllBaseWidget(true);

	EndWaiting();
	CCDirector:sharedDirector():setScaleTime(1.0)
	
	if (g_fightReslut == nil or g_fightReslut.iIsSuccess == nil)then
		FightMgr:GetInstance():GetMainState():ResetState(EFightState_End, 1);
		return;
	end
	Log("g_fightReslut.iIsSuccess ="..g_fightReslut.iIsSuccess)

    if g_Arena_isCheckRecord == true then--jjc的查看功能结算走另一条道路
        g_Arena_isCheckRecord = false
        Fight_Win_ArenaCheckRecord()
    else
	    if (g_fightReslut.iIsSuccess ~= 0)then
		    Fight_WinAnim()
	    else
		    Fight_Lose()
	    end 
    end
end

function Fight_WinAnimEnd()
	g_colorLayer:removeFromParent()  
    Fight_Win();
end

if (g_winAnimLayout == nil)then
	g_winAnimLayout = nil;
end
function Fight_WinAnim()
    local star = g_fightReslut.iStar;
	if (star == 0)then
		star = 3;
	end

    local function Shengli_End(armature, movementType, movementID)
	    if (movementType == LOOP_COMPLETE)then
		    armature:getAnimation():playWithIndex(6-2*star+1);
	    end
    end

	g_winAnimLayout = Layout:create();
	local shengli = GetUIArmature("Effshengli")
	shengli:getAnimation():playWithIndex(6-2*star)
	shengli:getAnimation():setMovementEventCallFunc(Shengli_End);
	shengli:setPosition(ccp( 480, 360 ))
	

	
	--local index = (star - 1) * 2	

	--function Xingxing_End(armature, movementType, movementID)
	--	if (movementType == LOOP_COMPLETE)then
	--		armature:getAnimation():playWithIndex(index + 1);
	--	end
	--end
	--local xingxing = GetUIArmature("Effxingxing")
	--xingxing:getAnimation():playWithIndex(index)
	--xingxing:setPosition(ccp( 0, -80 ))
	--xingxing:getAnimation():setMovementEventCallFunc(Xingxing_End);
	--shengli:addChild(xingxing);
	
	local delayAction = CCDelayTime:create(2);
	local moveAction = CCMoveBy:create(0.3, ccp(-67,165));
	local endCall = CCCallFunc:create(Fight_WinAnimEnd);
	local actionArry = CCArray:create()
	actionArry:addObject(delayAction)
	actionArry:addObject(moveAction)
	actionArry:addObject(endCall)
	
	shengli:runAction(CCSequence:create(actionArry));
	
	g_winAnimLayout:addNode(g_colorLayer, 0);
	g_winAnimLayout:addNode(shengli, 1);
	MainScene:GetInstance():AddChild(g_winAnimLayout, EMSTag_DungeonEffect, EMSTag_DungeonEffect, false);
end

function OnFightResult(pkg)
	CloseFightActorSkillNotify();
	g_fightReslut = GameServer_pb.Cmd_Sc_CommFightResult();
	g_fightReslut:ParseFromString(pkg)
	Log("OnFightResult"..tostring(g_fightReslut))
	if g_fightReslut:HasField("bCanFP") then
       g_EquipCard_IsOpen = true 
    end 
	if(FightMgr:GetInstance():GetMainState():GetState() == EFightState_Pause)then
		Fight_ResultSound();
		FightMgr:GetInstance():GetMainState():ResetState(EFightState_UI, 1);
	end
end

function Fight_ResultSound()
	GetAudioMgr():stopBackgroundMusic()
	if FightMgr:GetInstance().m_isCg == false then
		if (g_fightReslut.iIsSuccess ~= 0)then
		    CreateGameEffectMusic(g_effectMusicFileTable["win"])	
		else
			CreateGameEffectMusic(g_effectMusicFileTable["lose"])	
		end
	end
end

function Fight_Init()
	g_fightReslut = {};
end

function GET_SECTION_AWARD(pkg)
	EndWaiting();
	local tmp = GameServer_pb.CMD_GET_SECTION_AWARD_SC();
	tmp:ParseFromString(pkg)
	
	for i=1, #g_dungeonList do
		if (g_dungeonList[i].iSectionID == tmp.iSectionID) then
			g_dungeonList[i].bGeted = true;
			
			if (UI_GetBaseWidgetByName("GoldBox")) then
				UI_CloseCurBaseWidget();
			end
			--UI_CloseCurBaseWidget();
			UI_RefreshBaseWidgetByName("Dungeon")
			--NextDungeon_Create();
			return;
		end
	end
end

function NextDungeon_Create()
	local widget = UI_CreateBaseWidgetByFileName("NextDungeon.json");
	if (widget == nil)then
		Log("NextDungeon_Create error");
		return;
	end
	
	UI_GetUIButton(widget, 1):addTouchEventListener(UI_ClickCloseCurBaseWidget);
	
	local function GoNext(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			UI_CloseCurBaseWidget();
			UI_CloseCurBaseWidget();
			Dungeon_ListCreate();
		end
	end
	UI_GetUIButton(widget, 2):addTouchEventListener(GoNext);
end

if (g_newPassSectionID == nil)then
	g_newPassSectionID = -1;
end

function NEWPASS_SECTION(pkg)
	local tmp = GameServer_pb.CMD_NEWPASS_SECTION_SC();
	tmp:ParseFromString(pkg)
	
	g_newPassSectionID = tmp.iPassedSectionID;
end

function DUNGEON_SAODANG(pkg)
	local tmp = GameServer_pb.CMD_DUNGEON_SAODANG_SC();
	tmp:ParseFromString(pkg)
	Log("DUNGEON_SAODANG"..tostring(tmp))
	
	Saodang_Create(tmp)
	
	local tab = GameServer_pb.Cmd_Cs_QueryDSectionDetail();
	tab.iSectionID = g_selectSectionID;
			
	Packet_Full(GameServer_pb.CMD_QUERY_DSECTIONDETAIL, tab);
end


function Saodang_Create(tmp)
	--升级info
    if returnPlayerlevelupdateinfo() ~=nil then
       HeroLevelUpInfoShow()
    end

	local widget = UI_CreateBaseWidgetByFileName("Saodang.json");
	EndWaiting();
	if (widget == nil)then
		Log("Fight_Win error");
		return;
	end

	--local layout = UI_GetUILayout(widget, 1);
	
	local maker = CompStringMaker:new_local();
	local awardPropList = tmp.awardInfo.szAwardPropList;
	local awardItemList = tmp.awardInfo.szAwardItemList;
	local awardHeroList = tmp.awardInfo.szAwardHeroList

    print("award================================"..tostring(tmp))

	--for i=1,#awardPropList  do
	--	--Log("awardPropList[i].iLifeAttID"..awardPropList[i].iLifeAttID)
	--	maker:AppendSprite( "LifeAttIcon/s_"..awardPropList[i].iLifeAttID..".png", 0 , 1);
	--	maker:AppendString("  +"..awardPropList[i].iValue, CompTextArgs:new_local(255,255,255));

	--	if (i%3 == 0)then
	--		maker:AppendChangeLine();
	--	else
	--		maker:AppendString("   ", CompTextArgs:new_local(255,255,255));
	--	end
	--end
	--local awarPropLab = CompLabel:GetDefaultCompLabel(maker:GetString(), 480);
	--awarPropLab:setPosition(ccp(-awarPropLab:getSize().width/2, 50));
	--layout:addChild(awarPropLab);
	for i=1,#awardPropList  do
        local image_ = UI_GetUIImageView(widget, 1101+i*100)
        local text_ = UI_GetUILabel(widget, 1102+i*100)

        image_:setVisible(true)
        text_:setVisible(true)

        image_:loadTexture("LifeAttIcon/s_"..awardPropList[i].iLifeAttID..".png")
        if awardPropList[i].iLifeAttID == GameServer_pb.en_LifeAtt_Exp then 
            local expnum = awardPropList[i].iValue - tmp.iExtraExp 
            local expaddnum = tmp.iExtraExp
            local playerVip = GetLocalPlayer():GetInt(EPlayer_VipLevel)
            local expstr = "+"..expnum.."+"..expaddnum.."(".."VIP"..playerVip..FormatString("Fight_addExpByVip")..")"
            text_:setText(expstr)
        else 
           text_:setText("+"..awardPropList[i].iValue)  
        end 
        

        --text_:enableStroke()
    end


    if #awardItemList==0 then
        UI_GetUILabel(widget, 4656247):setVisible(true)
    end

	for i=1, #awardHeroList do
		local img = UI_GetUIImageView(widget, i+10);
		img:setVisible(true);
		local data = GetGameData(DataFileHeroBorn, awardHeroList[i].iHeroID, "stHeroBornData");
		
		local icon = UI_IconFrame(UI_GetHeroIcon(data.m_icon, awardHeroList[i].iCount), awardHeroList[i].iLevelStep, awardHeroList[i].iQuality);
		img:addChild(icon)
	end
	
	for i=#awardHeroList + 1, #awardHeroList + #awardItemList do
		local img = UI_GetUIImageView(widget, i+10);
		img:setVisible(true);
		local data = nil
        local icon = nil 
		if (IsItem(awardItemList[i - #awardHeroList].iItemID))then
            local function showItemData(sender,eventType)
                if eventType == TOUCH_EVENT_ENDED then
                    AddItemTip(awardItemList[i - #awardHeroList].iItemID,-150)
                end
            end
			data = GetGameData(DataFileItem, awardItemList[i - #awardHeroList].iItemID, "stItemData");
            icon = UI_ItemIconFrame(UI_GetItemIcon(data.m_icon, awardItemList[i - #awardHeroList].iCount), data.m_quality);
		    icon:setTouchEnabled(true)
            icon:addTouchEventListener(showItemData)
        else
            local function showEquipData(sender,eventType)
                if eventType == TOUCH_EVENT_ENDED then
                    AddEquipTip(awardItemList[i - #awardHeroList].iItemID,-150,awardItemList[i - #awardHeroList].dynamicEquipProp)
                end
            end
			data = GetGameData(DataFileEquip, awardItemList[i - #awardHeroList].iItemID, "stEquipData");
            icon = UI_ItemIconFrame(UI_GetEquipIcon(data.m_icon), data.m_quality)
            icon:setTouchEnabled(true)
            icon:addTouchEventListener(showEquipData)
		end
		img:addChild(icon)
	end

	--UI_GetUIButton(widget, 2):addTouchEventListener(UI_ClickCloseCurBaseWidget);
	

	local szHeroAward = tmp.szHeroAward;
	for i=1, #szHeroAward do
		local heroEntity = GetLocalPlayer():GetHeroByObjId(szHeroAward[i].dwHeroObjectID);
		local layout = UI_GetUIImageView(widget, 20 + i);
		local data = heroEntity:GetHeroData();
		layout:setVisible(true)
		local img = UI_IconFrame(UI_GetHeroIcon(data.m_icon, heroEntity:GetInt(EHero_Lvl)), heroEntity:GetInt(EHero_LvlStep), heroEntity:GetInt(EHero_Quality));
		layout:addChild(img);
		
		UI_GetUILabel(layout, 1):setText("+"..tmp.iHeroGetExp)
		
		local function AnimEnd(armature, movementType, movementID)
			if (movementType == LOOP_COMPLETE)then
				if (movementID == "0")then
					armature:getAnimation():playWithIndex(1)
				end
			end
		end
		
		if (szHeroAward[i].bLevelUp)then
			local armature = GetUIArmature("Effshegnji")
			armature:getAnimation():playWithIndex(0)
			armature:getAnimation():setMovementEventCallFunc(AnimEnd);
			layout:addNode(armature)
		end
	end
	
	
	-- 如果有翻牌
	if tmp:HasField("bCanFP") then
		g_EquipCard_IsOpen = true
		EquipCardTrun_InitLayout();
	else
		g_EquipCard_IsOpen = false;
	end
	
end

--竞技场查看记录的战报
function Fight_Win_ArenaCheckRecord()
    local fightMember =  FightMgr:GetInstance():getFightMember()
    
    local widget = UI_CreateBaseWidgetByFileName("ArenaCheckRecord.json")
    UI_GetUILayout(widget, 4663605):setTouchEnabled(false)
    --再看一遍
    local function ClickReplay_(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			FightMgr:GetInstance():Replay();
            g_Arena_isCheckRecord = true
		end
	end
	UI_GetUIButton(widget, 4663662):addTouchEventListener(ClickReplay_);
    --退出播放
    local function ClickLeave_(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			FightMgr:GetInstance():GetMainState():ResetState(EFightState_End, 1);
		end
	end
	UI_GetUIButton(widget, 4663654):addTouchEventListener(ClickLeave_);
    
    local liftObjects,rightObjects={},{}

    for i=0,17,1 do
        if fightMember.Objects[i] ~= nil then
            if fightMember.Objects[i].location < 9 then 
                liftObjects[#liftObjects+1] = fightMember.Objects[i]
            else
                rightObjects[#rightObjects+1] = fightMember.Objects[i]
            end
        end
    end

    --攻击方-----------------------------------------------------
    local recordLiftImage = UI_GetUIImageView(widget, 4663599)
    Fight_Win_SetData(recordLiftImage,fightMember.leftPlayer,liftObjects)
    --防守方-------------------------------------------------------
    local recordRightImage = UI_GetUIImageView(widget, 4663602)
    Fight_Win_SetData(recordRightImage,fightMember.rightPlayer,rightObjects)

    if fightMember.iIsAttackerWin ~= 0 then
        UI_GetUIImageView(recordLiftImage, 4663665):loadTexture("UIExport/DemoLogin/zhandou/Icon_07.png")
        UI_GetUIImageView(recordRightImage, 4663693):loadTexture("UIExport/DemoLogin/zhandou/Icon_05.png")
    else
        UI_GetUIImageView(recordLiftImage, 4663665):loadTexture("UIExport/DemoLogin/zhandou/Icon_05.png")
        UI_GetUIImageView(recordRightImage, 4663693):loadTexture("UIExport/DemoLogin/zhandou/Icon_07.png")
    end

end
-- 设置竞技场查看记录的战报的数据
function Fight_Win_SetData(image,player,Objects) 
    --头像
    local head = UI_GetUILayout(image, 1101)
    local headImg = CCSprite:create("Icon/HeroIcon/x"..player.iHeadID..".png")
    head:removeNodeByTag(2);
    local headIcon = UIMgr:GetInstance():createMaskedSprite(headImg,"zhujiemian/bg_4.png")
    head:addNode(headIcon,0,2)
    --等级
    UI_GetUILabel(image, 1102):setText(player.iLevel)
    --名字
    UI_GetUILabel(image, 1103):setText(player.strActorName)
    --英雄
    for i=1,#Objects,1 do
        if Objects[i]~=nil and Objects[i].location ~= -1 then
            local location
            if Objects[i].location>=9 then
                location = UI_GetUILayout(image, 1200+Objects[i].location-9)
            else
                location = UI_GetUILayout(image, 1200+Objects[i].location)
            end
            print("Objects[i].location============"..Objects[i].location)
            local icon =  UI_IconFrame(UI_GetHeroIcon(Objects[i].iBaseID, Objects[i].iLevel), Objects[i].iLevelStep, Objects[i].iQuality);
            icon:setPosition(ccp(44,44))
            location:addChild(icon)
        end
    end
end
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_DUNGEON_SAODANG, "DUNGEON_SAODANG" );
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_GET_SECTION_AWARD, "GET_SECTION_AWARD" );
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_COMM_FIGHTRESULT, "OnFightResult" );
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_DUNGEON_SECTIONLIST, "OnDungeonList" );
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_SECTION_UPDATE, "UpdateDungeonList" );
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_QUERY_DSECTIONDETAIL, "OnQueryDungeonInfo" );
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_NEWPASS_SECTION, "NEWPASS_SECTION" );
