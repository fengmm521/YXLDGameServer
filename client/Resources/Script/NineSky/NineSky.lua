function NineSky_Create(tmp)
	local widget = UI_GetBaseWidgetByName("NineSky");
	if (widget == nil)then
		widget = UI_CreateBaseWidgetByFileName("NineSky.json");
	end
	if (widget == nil)then
		Log("NineSky_Create error");
		return;
	end
    UI_GetUILayout(widget, 4768631):addTouchEventListener(PublicCallBackWithNothingToDo)
	UI_GetUIButton(widget, 1):addTouchEventListener(UI_ClickCloseCurBaseWidgetWithNoAction);
	
	UI_GetUILabel(widget, 3):setText(""..tmp.iResetTimes)
	
	local function Reset()
		Packet_Cmd(GameServer_pb.CMD_DREAMLAND_RESET);
		ShowWaiting();
	end
	
	local function ClickReset(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then			
			if (tmp.iResetTimes > 0)then
				Messagebox_Create({info = FormatString("NineSkyResetNotice"),msgType = EMessageType_LeftRight,leftFun = Reset})
			else
                createPromptBoxlayout(FormatString("NineSkyResetNoCount"))
				--Messagebox_Create({info = FormatString("NineSkyResetNoCount"), msgType = EMessageType_None})
			end
		end
	end
	
	UI_GetUIButton(widget, 4):addTouchEventListener(ClickReset);
	
	local function ClickRule(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then			
			local ruleWidget = UI_CreateBaseWidgetByFileName("NineSkyRule.json", EUIOpenAction_Enlarge)
			UI_GetUIButton(ruleWidget, 1):addTouchEventListener(UI_ClickCloseCurBaseWidget);
            local listView = UI_GetUIListView(ruleWidget, 4714917)
            for i=1,6,1 do 
                local infolab = CompLabel:GetDefaultCompLabel(FormatString("NineSkyRule_"..i),listView:getSize().width)
                --infolab:setColor(ccc3(164,240,25))
                local tempLayout = UI_GetUILayout(ruleWidget, 1122):clone()
                listView:pushBackCustomItem(infolab)
                listView:pushBackCustomItem(tempLayout)
            end
		end
	end
	
	UI_GetUIButton(widget, 5):addTouchEventListener(ClickRule);
	local cloneLayout = UI_GetUILayout(widget, 100);
	cloneLayout:setPosition(ccp(0, 0));
	
	local tmpLayout = Layout:create();
	tmpLayout:setSize(CCSizeMake(200, 340));
	
	local function ClickNineSky(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED)then
			local sender = tolua.cast(sender, "Widget");
			local tag = sender:getTag();
			local tab = GameServer_pb.CMD_DREAMLAND_QUERY_SCENEINFO_CS();
			g_nineSkyID = tag;
			tab.iSectionID = tag;
			Packet_Full(GameServer_pb.CMD_DREAMLAND_QUERY_SCENEINFO, tab);
			ShowWaiting();
		end
	end
	
	local listView = UI_GetUIListView(widget, 2)
	listView:removeAllItems();
	
	for i=1, 9 do
		local layout = tmpLayout:clone();
		local img = cloneLayout:clone();
		img:setVisible(true)

		UI_GetUIImageView(img, 1):loadTexture("NineSky/NineSky_name_0"..i..".png");
		UI_GetUIImageView(img, 2):loadTexture("Icon/heroImage/500"..i..".png");
		
		if (i <= tmp.iOPenCount)then
			if (tmp.unLockSectionID == -1)then
				UI_GetUIImageView(img, 3):loadTexture("NineSky/NineSky_bg_05.png");
			elseif (tmp.unLockSectionID == i)then
				UI_GetUIImageView(img, 3):loadTexture("NineSky/NineSky_bg_02.png");
			else		
				UI_GetUIImageView(img, 3):loadTexture("NineSky/NineSky_bg_03.png");
			end
		else
			UI_GetUIImageView(img, 2):setColor(ccc3(125, 125, 125))
		end
		
		img:addTouchEventListener(ClickNineSky);
		
		layout:addChild(img, i, i);
		listView:pushBackCustomItem(layout);
	end
	
	cloneLayout:setVisible(false);
end

function DREAMLAND_OPEN_REQSECTON(pkg)
	EndWaiting();
	local tmp = GameServer_pb.CMD_DREAMLAND_OPEN_REQSECTON_SC();
	tmp:ParseFromString(pkg)

	Log("*********"..tostring(tmp))
	NineSky_Create(tmp)
end

function NineSkyMap_Create(tmp)
    print("NineSkyMap_Create-----------------")
    if g_IsRefreshMap == false then
        g_IsRefreshMap= true
        return
    end
	local widget = UI_GetBaseWidgetByName("NineSkyMap");
	if (widget == nil)then
		widget = UI_CreateBaseWidgetByFileName("NineSkyMap.json");
	end
	
	if widget == nil then
		Log("NineSkyMap_Create error")
		return;
	end
	UI_GetUILayout(widget, 4768632):setTouchEnabled(false)
    local function close(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            UI_ClickCloseAllBaseWidget(sender,eventType)
            UI_RefreshMainUiTop()       
        end
    end

	local data = GetGameData(DataFileGodAniaml, 5000 + g_nineSkyID, "stGodAnimalData")
	local nineData = GetGameData(DataFileDreamLandSection, g_nineSkyID, "stDreamLandSectionData")
	UI_GetUIButton(widget, 1):addTouchEventListener(UI_ClickCloseCurBaseWidgetWithNoAction);
	UI_GetUIButton(widget, 100):addTouchEventListener(close);
	UI_GetUILabel(widget, 3):setText(nineData.m_name)
	local scrollView = UI_GetUIScrollView(widget, 2);
	
	local function ClickFormation(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			local sender = tolua.cast(sender, "Widget");
			local tag = (sender:getTag() + 1)/2;
			g_nineSkySceneID = tmp.szSceneDetail[tag].iSceneId;
			g_nineSkyIndexID = tag;
			local tab = GameServer_pb.CMD_DREAMLAND_QUERY_SCENEFORMATION_CS();
			tab.iSectionID = g_nineSkyID;
			tab.iSceneId = g_nineSkySceneID;
			Packet_Full(GameServer_pb.CMD_DREAMLAND_QUERY_SCENEFORMATION, tab);
			ShowWaiting();
		end
	end

	--发送消息
    local tab = nil
    local function SendMessage(armature, movementType, movementID)
    
        print("QQQQQQQQQQQQQQQQQQQQQQQQQQ------"..movementType.."   "..movementID)
        if (movementType == LOOP_COMPLETE and movementID == "1")then
		    armature:removeFromParentAndCleanup(true) 
            NineSkyAward_Create()
            NineSkyMap_Create(g_NineSky_Info)  
	    end
        
        

    end
	local function ClickBox(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			local sender = tolua.cast(sender, "Widget");
			local tag = sender:getTag()/2;
			
            --未开启关卡的宝箱无法查看
            if tag<#tmp.szSceneDetail then
                return 
            end

			g_nineSkySceneID = tmp.szSceneDetail[tag].iSceneId;
			g_nineSkyIndexID = tag;
			
			if (tmp.szSceneDetail[tag] and tmp.szSceneDetail[tag].bHaveGetReward == false)then
                --开启
				if (tmp.szSceneDetail[tag].iSceneState == GameServer_pb.en_State_FINISH) then
					tab = GameServer_pb.CMD_DREAMLAND_REQUEST_OPENBOX_CS();
					tab.iSectionID = g_nineSkyID;
					tab.iSceneId = g_nineSkySceneID;

                    
		            local boxBnt = UI_GetUILayout(scrollView, tag*2);
                    boxBnt:removeAllNodes()
                    boxBnt:removeAllChildren();
                    boxBnt:stopAllActions()
                    local layout = Layout:create();
                    local godanimalanimatrue = GetUIArmature("baoxiang01")
                    godanimalanimatrue:setPosition(ccp(40,10))
                    godanimalanimatrue:getAnimation():playWithIndex(1) 
                    godanimalanimatrue:getAnimation():setMovementEventCallFunc(SendMessage) 
                    boxBnt:addNode(godanimalanimatrue)

                    g_IsShowBox = false
                    g_IsRefreshMap = false

                    Packet_Full(GameServer_pb.CMD_DREAMLAND_REQUEST_OPENBOX, tab);
                    ShowWaiting();

                    boxBnt:setTouchEnabled(false)

					return;
				end
			end
			tab = GameServer_pb.CMD_DREAMLAND_QUERY_BOX_CS();
			tab.iSectionID = g_nineSkyID;
			tab.iSceneId = g_nineSkySceneID;
			Packet_Full(GameServer_pb.CMD_DREAMLAND_QUERY_BOX, tab);
            ShowWaiting();

		end
	end
	local size = nil
	for i=1, 16 do
		local sceneBnt = UI_GetUIButton(scrollView, i*2 - 1);
		local boxBnt = UI_GetUILayout(scrollView, i*2);
		sceneBnt:addTouchEventListener(ClickFormation)
		sceneBnt:removeAllNodes();  
		sceneBnt:removeAllChildren();
		boxBnt:addTouchEventListener(ClickBox)
		boxBnt:stopAllActions();
		--宝箱动画
		if (i <= #tmp.szSceneDetail) then
            local imageOpenOrClose = ImageView:create()
            imageOpenOrClose:setPosition(ccp(44,34))
            boxBnt:addChild(imageOpenOrClose)
   --         local godanimalanimatrue = GetUIArmature("baoxiang01")
   --         godanimalanimatrue:setPosition(ccp(40,30))
   --         boxBnt:addNode(godanimalanimatrue,10,10)

			--if (tmp.szSceneDetail[i].bHaveGetReward)then
			--	godanimalanimatrue:getAnimation():playWithIndex(2) 
			--else       
   --             godanimalanimatrue:getAnimation():playWithIndex(0) 
   --         end
            if (tmp.szSceneDetail[i].bHaveGetReward)then
				imageOpenOrClose:loadTexture("NineSky/NineSky_02.png") 
			else       
                imageOpenOrClose:loadTexture("NineSky/NineSky_01.png") 
            end
			
			if (i== #tmp.szSceneDetail and tmp.szSceneDetail[i].bHaveGetReward == false)then
				if (tmp.szSceneDetail[i].iSceneState == GameServer_pb.en_State_FINISH) then
					local arryaction = CCArray:create()
					arryaction:addObject(CCDelayTime:create(0.6))
					arryaction:addObject(CCScaleTo:create(0.1, 1.2))
					arryaction:addObject(CCScaleTo:create(0.1, 0.9))
					arryaction:addObject(CCScaleTo:create(0.1, 1.1))
					arryaction:addObject(CCScaleTo:create(0.1, 1.0))
					local seqaction = CCSequence:create(arryaction)
					local reaction = CCRepeatForever:create(seqaction)
					boxBnt:runAction(reaction)
				else
					local armature = GetUIArmature("zhangjie")
					armature:getAnimation():playWithIndex(0)
					armature:setScale(0.6)
					sceneBnt:addNode(armature, 100);
                    imageOpenOrClose:loadTexture("NineSky/NineSky_01.png") 
				end
			end
			
			sceneBnt:setTouchEnabled(true);
		else
            local imageOpenOrClose=ImageView:create()
            imageOpenOrClose:loadTexture("NineSky/NineSky_01.png") 
            imageOpenOrClose:setPosition(ccp(44,34))
            boxBnt:addChild(imageOpenOrClose)
			--local godanimalanimatrue = GetUIArmature("baoxiang01")
   --         godanimalanimatrue:setPosition(ccp(40,10))
   --         boxBnt:addNode(godanimalanimatrue,10,10)
            sceneBnt:setTouchEnabled(false)
		end
		
		if (i == 16)then
			local armature = GetArmature(data.m_animName)	
			armature:getAnimation():playWithIndex(0)
			armature:setScale(0.8)
			sceneBnt:addNode(armature);
			
			size = CCSizeMake(boxBnt:getPositionX(), 300)
		end
		
		
	end
	
    if g_NineSky_isGetAward ~= true then
        NineSky_ResetScrollViewLocation(tmp,size)
    else
        g_NineSky_isGetAward = false
    end
end
function NineSky_ResetScrollViewLocation(tmp,size)
    local widget = UI_GetBaseWidgetByName("NineSkyMap");
	if (widget == nil)then
		return 
	end
    local scrollView = UI_GetUIScrollView(widget, 2);

	local layout = UI_GetUIButton(scrollView, #tmp.szSceneDetail*2);

	local percent = (layout:getPositionX() - 400) / (size.width - 400)* 100

	if (percent < 0)then
		percent = 0
	end
	scrollView:jumpToPercentHorizontal(percent)
end
if (g_nineSkyID == nil)then
	g_nineSkyID = 1;
end

if (g_nineSkySceneID == nil)then
	g_nineSkySceneID = 1;
end

if (g_nineSkyIndexID == nil)then
	g_nineSkyIndexID = 1;
end

function DREAMLAND_QUERY_SCENEINFO(pkg)
	EndWaiting();
	local tmp = GameServer_pb.CMD_DREAMLAND_QUERY_SCENEINFO_SC();
	tmp:ParseFromString(pkg)

    Log("*********"..tostring(tmp))
	
    g_NineSky_Info = tmp

	NineSkyMap_Create(tmp)
end
function DREAMLAND_QUERY_BOX(pkg)
	EndWaiting();
	local tmp = GameServer_pb.CMD_DREAMLAND_QUERY_BOX_SC();
	tmp:ParseFromString(pkg)
	Log("*********"..tostring(tmp))
	print("---------------1--------------")
	NineSkyBox_Create(tmp)
end
function NineSkyBox_Create(tmp)
	local widget = UI_CreateBaseWidgetByFileName("NineSkyBox.json")
	if widget == nil then
		Log("NineSkyBox_Create error")
		return;
	end
	
	UI_GetUIButton(widget, 10):addTouchEventListener(UI_ClickCloseCurBaseWidget);	
	
	local awardPropList = tmp.awardResoult.szAwardPropList;
	local awardItemList = tmp.awardResoult.szAwardItemList;
	local awardHeroList = tmp.awardResoult.szAwardHeroList;
	
	for i=1, #awardPropList do
		if (awardPropList[i].iLifeAttID == GameServer_pb.en_LifeAtt_Silver)then
			UI_GetUILabel(widget, 11):setText(""..awardPropList[i].iValue);
		elseif (awardPropList[i].iLifeAttID == GameServer_pb.en_LifeAtt_HeroExp)then
			UI_GetUILabel(widget, 12):setText(""..awardPropList[i].iValue);
		end
	end
end


function DREAMLAND_REQUEST_OPENBOX(pkg)
	EndWaiting();
	local tmp = GameServer_pb.CMD_DREAMLAND_REQUEST_OPENBOX_SC();
	tmp:ParseFromString(pkg)
	Log("*********"..tostring(tmp))
	
    g_NineSky_Box = tmp

	print("---------------2--------------")
	NineSkyAward_Create()
end
function NineSkyAward_Create()
    if g_IsShowBox == false then
        g_IsShowBox = true
        return;
    end
    --local begin = os.time()
	local widget = UI_CreateBaseWidgetByFileName("NineSkyAward.json")
    --print("time =========="..(os.time() - begin))

	if widget == nil then
		Log("NineSkyAward_Create error")
		return;
	end
    g_NineSky_isGetAward = true

	UI_GetUILayout(widget, 4499630):addTouchEventListener(PublicCallBackWithNothingToDo)
	UI_GetUIButton(widget, 4695816):addTouchEventListener(UI_ClickCloseCurBaseWidget);	
	local awardPropList = g_NineSky_Box.awardResoult.szAwardPropList;
	local awardItemList = g_NineSky_Box.awardResoult.szAwardItemList;
	local awardHeroList = g_NineSky_Box.awardResoult.szAwardHeroList;
    local layout = nil

    --print("#awardPropList+#awardItemList+#awardHeroList ======"..#awardPropList+#awardItemList+#awardHeroList )
    if #awardPropList+#awardItemList+#awardHeroList == 4 then
        layout = UI_GetUILayout(widget, 3334)
        layout:setVisible(true)
        UI_GetUILayout(widget, 3333):setVisible(false)
    elseif #awardPropList+#awardItemList+#awardHeroList == 3 then 
        layout = UI_GetUILayout(widget, 3333)
        layout:setVisible(true)
        UI_GetUILayout(widget, 3334):setVisible(false)
    else
        print("awardnum is not 3 or 4")
        return
    end

	for i=1, #awardPropList do
		if (awardPropList[i].iLifeAttID == GameServer_pb.en_LifeAtt_Silver)then
			UI_GetUILabel(layout, 4695808):setText(""..awardPropList[i].iValue);
		elseif (awardPropList[i].iLifeAttID == GameServer_pb.en_LifeAtt_HeroExp)then
			UI_GetUILabel(layout, 4695810):setText(""..awardPropList[i].iValue);
		end
	end
	UI_GetUILayout(layout, 12001):setVisible(false)
    if UI_GetUILayout(layout, 12002) then
        UI_GetUILayout(layout, 12002):setVisible(false)
    end
    local awardNum = 0
	for i=1, #awardHeroList do
        awardNum = awardNum+1
		local layout = UI_GetUILayout(layout, 12000+awardNum);
		layout:setVisible(true); 
        
        local data = nil
        local icon = nil
        if awardHeroList[i].bHaveChgSoul==true then
            data = GetGameData(DataFileItem, awardHeroList[i].iSoulId, "stItemData");
            icon = UI_ItemIconFrame(UI_GetItemIcon(data.m_icon, 0), data.m_quality)
        else
            data = GetGameData(DataFileHeroBorn, awardHeroList[i].iHeroID, "stHeroBornData");
            icon = UI_IconFrame(UI_GetHeroIcon(data.m_icon), awardHeroList[i].iLevelStep);
        end

		UI_GetUIImageView(layout, 111):addChild(icon)
        UI_GetUILabel(layout, 222):setText("X"..awardHeroList[i].iCount)  

        if awardHeroList[i].bHaveChgSoul==true then
            Shop_CallHero(awardHeroList[i].iHeroID,awardHeroList[i].iCount,false,nil,awardHeroList[i].bHaveChgSoul) 
        else
            Shop_CallHero(awardHeroList[i].iHeroID,awardHeroList[i].iCount,false,nil)
        end
	end
	
	for i=#awardHeroList + 1, #awardHeroList + #awardItemList do
		awardNum = awardNum+1
		local layout = UI_GetUILayout(layout, 12000+awardNum);
		layout:setVisible(true);
		local data = nil
		Log("awardItemList[i - #awardHeroList].iItemID"..awardItemList[i - #awardHeroList].iItemID)
		if (IsItem(awardItemList[i - #awardHeroList].iItemID))then
			data = GetGameData(DataFileItem, awardItemList[i - #awardHeroList].iItemID, "stItemData");
		else
			data = GetGameData(DataFileEquip, awardItemList[i - #awardHeroList].iItemID, "stEquipData");
		end
		local icon = UI_ItemIconFrame(UI_GetItemIcon(data.m_icon, 0), data.m_quality);
		UI_GetUIImageView(layout, 111):addChild(icon)    
        UI_GetUILabel(layout, 222):setText("X"..awardItemList[i - #awardHeroList].iCount) 
	end
    --print("alltime =========="..(os.time() - begin))
end

------------------------------------------------------------------------------------------------
function DREAMLAND_QUERY_SCENEFORMATION(pkg)
	EndWaiting();
	local tmp = GameServer_pb.CMD_DREAMLAND_QUERY_SCENEFORMATION_SC();
	tmp:ParseFromString(pkg)
	Log("*********"..tostring(tmp))
	
	print("---------------3--------------")
	NineSkyEnemy_Create(tmp)
end
--敌方布阵
function NineSkyEnemy_Create(tmp)
	NineSkyOhtersLineUp_Create(tmp)
end

function DREAMLAND_GETACTOR_FORMATION_INFO(pkg)
	EndWaiting();
	local tmp = GameServer_pb.CMD_DREAMLAND_GETACTOR_FORMATION_INFO_SC();
	tmp:ParseFromString(pkg)
	Log("*********"..tostring(tmp))

    NineSkyFormation_Create(tmp);

end
--我方布阵
function NineSkyFormation_Create(temp)
    g_FormationOrNineSky = "NineSky"
	local widget = UI_CreateBaseWidgetByFileName("FormationSecond.json");
	if (widget == nil)then
		Log("Formation_Create error");
		return;
	end
    UI_GetUIImageView(widget, 4724164):setTouchEnabled(false)
    local function close(send,eventType)
        --在英雄位置移动期间，退出无法响应
        if eventType == TOUCH_EVENT_ENDED and g_Formation_heroLocation == -1 then
            Formation_Close(sender, eventType)
        else
            g_Formation_heroLocation = -1
            g_Formation_isMoved= false
        end
    end

	UI_GetUIButton(widget, 4695721):addTouchEventListener(close);
    
    local Objects = temp.szDreamLandFormation
    g_NineSky_HaveBattleHeroInfo = temp.szHaveBattleHeroInfo
	--显示当前队伍
	NineSkyFormation_Show(widget,Objects)
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
            local isSend = false
            for i=1,#g_InTeamId,1 do
                if g_InTeamId[i] ~=0 then
		            isSend = true;
                end	
            end
            if isSend == false then
                createPromptBoxlayout(FormatString("NineSky_NoHero"))
                return ;
            end
            Formation_SendMessage()
            local tab = GameServer_pb.CMD_DREAMLAND_REQUEST_FIGHT_CS();
			tab.iSectionID = g_nineSkyID;
			tab.iSceneId = g_nineSkySceneID;
			Packet_Full(GameServer_pb.CMD_DREAMLAND_REQUEST_FIGHT, tab);
			g_openUIType = EUIOpenType_NineSky
            UI_ClickCloseCurBaseWidgetWithNoAction(sender, eventType)
			ShowWaiting()

        else
            g_Formation_heroLocation = -1
            g_Formation_isMoved= false
        end
    end
    UI_GetUIButton(widget, 4691907):addTouchEventListener(saveTeam)
    UI_GetUIButton(widget, 4691907):setTitleText(FormatString("NineSkyFightStart"))

    Formation_RefreshData()
    UI_GetUILabel(widget, 4691904):setText(FormatString("LastTimes",temp.remaindTime,10))
    UI_GetUILabel(widget, 4751562):enableStroke()
end
--查询英雄是否死亡或者掉血
function NineSky_IsHeroLoseHP(dwObjcetId)
    for i=1,#g_NineSky_HaveBattleHeroInfo,1 do
        if g_NineSky_HaveBattleHeroInfo[i].dwObjcetId == dwObjcetId then
            return g_NineSky_HaveBattleHeroInfo[i].iBlootPresent/100
        end
    end
    return 100
end
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_DREAMLAND_GETACTOR_FORMATION_INFO, "DREAMLAND_GETACTOR_FORMATION_INFO" );
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_DREAMLAND_QUERY_SCENEFORMATION, "DREAMLAND_QUERY_SCENEFORMATION" );
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_DREAMLAND_REQUEST_OPENBOX, "DREAMLAND_REQUEST_OPENBOX" );
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_DREAMLAND_QUERY_BOX, "DREAMLAND_QUERY_BOX" );
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_DREAMLAND_QUERY_SCENEINFO, "DREAMLAND_QUERY_SCENEINFO" );
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_DREAMLAND_OPEN_REQSECTON, "DREAMLAND_OPEN_REQSECTON" );


