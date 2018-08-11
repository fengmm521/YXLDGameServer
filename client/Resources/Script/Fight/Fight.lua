if (g_scaleTime == nil)then
	g_scaleTime = 1
end

if (g_fightWave == nil) then
	g_fightWave = 1;
end

g_ActorSkillNotify = false; -- 战斗中是否已经提示过使用主公技能了,一场战斗只能一次
g_ActorNotifying = false;  -- 是否正在战斗中提示使用主公技能


local function getScaleTime()
	if 1 == g_scaleTime then
		return 1.3;
	elseif 2 == g_scaleTime then
		return 2;
	else
		return 1;
	end
end

g_actorSkillID = 0;

function CloseFightActorSkillNotify()
	if g_ActorNotifying then
		g_arrowLayout:setVisible(false);
		g_ActorNotifying = false;
	end
end

function Fight_Create(forceVisible)
	
    Log("cccccccssddd")
	local scene = GetCurScene();
	if (scene == nil)then
		return;
	end
	local widget = UI_CreateBaseWidgetByFileName("Fight.json");
	if (widget == nil)then
		Log("Fight_Create error");
		return;
	end
	widget:setVisible(forceVisible)
    ReFreshFightRound(widget)
	function ClickBg(sender, eventType)
		if (eventType == TOUCH_EVENT_BEGAN) then
			Fight_IsShowName(true)
		elseif (eventType == TOUCH_EVENT_ENDED or eventType == TOUCH_EVENT_CANCELED) then
			Fight_IsShowName(false)
		end
	end	
	local bg = UI_GetUILayout(widget, 100);
	bg:addTouchEventListener(ClickBg);
	
	function ClickHuifu(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			g_scaleTime = 1;
			CCDirector:sharedDirector():setScaleTime(getScaleTime(g_scaleTime))
			UI_GetUILayout(bg, 4):setVisible(false);
			UI_GetUILayout(bg, 3):setVisible(true);
		end
	end	
	local huifu = UI_GetUILayout(bg, 4);
	huifu:addTouchEventListener(ClickHuifu);
	
	function ClickJiasu(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			-- 小于15级不加速
			if GetLocalPlayer() == nil or GetLocalPlayer():GetInt(EPlayer_Lvl) < 15 then
				UI_ShowError(1, FormatString("SpeedUpLevelCondition") );
				return;
			end

		
			g_scaleTime = 2;
			CCDirector:sharedDirector():setScaleTime(getScaleTime(g_scaleTime))
			UI_GetUILayout(bg, 3):setVisible(false);
			UI_GetUILayout(bg, 4):setVisible(true);
		end
	end	
	local jiasu = UI_GetUILayout(bg, 3);
	jiasu:addTouchEventListener(ClickJiasu);
	
	UI_GetUILayout(bg, 4):setVisible(g_scaleTime==2);
	UI_GetUILayout(bg, 3):setVisible(g_scaleTime==1);
	
	function ClickQuit(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			if (FightMgr:GetInstance().m_canSkip == false)then
				--FightMgr:GetInstance():GetMainState():ResetState(EFightState_Pause, 0);
				Packet_Cmd(GameServer_pb.CMD_FIGHT_QUIT);
				ShowWaiting()
			end
		end
	end	
	UI_GetUIButton(bg, 2):addTouchEventListener(ClickQuit);
	UI_GetUIButton(bg, 2):setVisible((FightMgr:GetInstance().m_canSkip == false) and FightMgr:GetInstance().m_isCg == false and GameServer_pb.en_FightType_DreamLand ~= FightMgr:GetInstance().m_fightType)
    if g_openUIType == EUIOpenType_WorldBoss then 
       UI_GetUIButton(bg, 2):setVisible(false)
	end 
	function ClickSkip(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			if (FightMgr:GetInstance().m_canSkip)then
				FightMgr:GetInstance():FinishFight();
				CloseFightActorSkillNotify();
                UI_GetUIButton(bg, 1):setTouchEnabled(false)
			else		
				if (GameServer_pb.en_FightType_ClimbTower == FightMgr:GetInstance().m_fightType)then
					createPromptBoxlayout(FormatString("Notice_SkipBabel"));
				elseif (GameServer_pb.en_FightType_DreamLand == FightMgr:GetInstance().m_fightType) then
					createPromptBoxlayout(FormatString("Notice_SkipNineSky"));
				else
					createPromptBoxlayout(FormatString("Notice_Skip"));
				end
			end
		end
	end	
	UI_GetUIButton(bg, 1):addTouchEventListener(ClickSkip);
	UI_GetUIButton(bg, 1):setVisible(FightMgr:GetInstance().m_isCg == false)
    if g_openUIType == EUIOpenType_WorldBoss then 
       UI_GetUIButton(bg, 1):setVisible(false)
    end 
	UI_GetUILabelBMFont(widget:getChildByTag(102), 50):setVisible(false);
	UI_GetUIImageView(widget, 102):setVisible(false);
	
	if (FightMgr:GetInstance().m_isCanItem)then
		local heroList = vector_Hero__:new_local()
		scene:GetLeftHeroList(heroList)
		UI_GetUILabelBMFont(widget:getChildByTag(102), 50):setVisible(true);
		UI_GetUIImageView(widget, 102):setVisible(true);
	
		local function ClickHeroIcon(sender, eventType)
			if (eventType == TOUCH_EVENT_ENDED) then
				local sender = tolua.cast(sender, "ImageView");		
				local tag = sender:getTag();
				local data = GetGameData(DataFileActorSkill, tag-1000, "stActorSkillData")
				
				local playerLvl =1
				if GetLocalPlayer() then
					playerLvl = GetLocalPlayer():GetInt(EPlayer_Lvl);
				end
				if playerLvl < data.m_level then
					--local labelWidget = CompLabel:GetDefaultCompLabel(FormatString("ActorSkill"..(tag-1000).."NoOpen", data.m_level), 200);
					local errMsg = FormatString("ActorSkill"..(tag-1000).."NoOpen", data.m_level)
					UI_ShowError(GameServer_pb.en_ErrorCodeType_Float, errMsg);
					return;
				end
				
				
				if (sender:getNodeByTag(9999) ~= nil)then
					createPromptBoxlayout(FormatString("Notice_CD"));
				elseif data.m_cost > FightMgr:GetInstance().m_itemCount then
					createPromptBoxlayout(FormatString("Notice_NoHpItem"));
				elseif (scene:IsPause() == false)then
					if (FightMgr:GetInstance().m_isCg == false)then		
						local tab = GameServer_pb.CMD_FIGHT_USEACTORSKILL_CS();
						local indexSt = FightMgrIndex:new_local();
						FightMgr:GetInstance():GetFightMgrIndex(indexSt);
						--Log("indexSt.m_actionIndex"..indexSt.m_actionIndex)
						--Log("indexSt.m_resultIndex"..indexSt.m_resultIndex)
						if (indexSt.m_actionIndex < 0 or indexSt.m_resultIndex < 0)then
							Log("index is not right")
							return;
						end
						
						tab.iActionIndex = indexSt.m_actionIndex;
						tab.iResultSetIndex = indexSt.m_resultIndex;	
						tab.iActorSkillID = tag-1000
						if (indexSt.m_isFirst)then
							tab.bPost = false;
						else
							tab.bPost = true;
						end
						g_useItemRev=false;
						g_revAction=false;
						
						--tab.iTargetPos = heroList[tag]:GetInt(EHero_FormationPos)
						Packet_Full(GameServer_pb.CMD_FIGHT_USEACTORSKILL, tab);
						
						local dianji = GetUIArmature("Effdianji")
						dianji:getAnimation():playWithIndex(0)
						dianji:getAnimation():setMovementEventCallFunc(ArmatureEnd)
						sender:addNode(dianji, 100);
						
						local function CDEnd()
							local image2Widget = UI_GetUIImageView(widget, 102);
							for i=1, 3 do
								local layout = UI_GetUIImageView(image2Widget, 1000+i)
								layout:removeNodeByTag(9999)
							end
						end
						
						for i=1, 3 do
							local to = CCProgressFromTo:create(5, 100, 0);
							local progress = CCProgressTimer:create(CCSprite:create("Common/Mask_Icon_01.png"));
							progress:setType( kCCProgressTimerTypeRadial );
							progress:setReverseProgress(true);
		
							if (i==1)then
								local endCall = CCCallFunc:create(CDEnd)
								local actionArry = CCArray:create()
								actionArry:addObject(to)
								actionArry:addObject(endCall)
								progress:runAction(CCSequence:create(actionArry));
							else
								progress:runAction(to)	
							end
							
							local layout = UI_GetUIImageView(UI_GetUIImageView(widget, 102), 1000+i)								
							layout:addNode(progress, 10, 9999);
						end
						Fight_HpEffect(tag-1000)
						
						-- 点击后隐藏吧
						CloseFightActorSkillNotify();
					else
						if (g_isGuide) then
							Guide_GoNext();
							Fight_HpEffect(tag-1000)
						end
					end
				else

				end
			end
		end
		
		local playerLvl =1
		if GetLocalPlayer() then
			playerLvl = GetLocalPlayer():GetInt(EPlayer_Lvl);
		end
		local image2Widget = UI_GetUIImageView(widget, 102);
		for i=1, 3 do
			local layout = UI_GetUIImageView(image2Widget, 1000+i)
			layout:setVisible(true)
			local data = GetGameData(DataFileActorSkill, i, "stActorSkillData")
			Log("data.m_cost"..data.m_cost)
			
			UI_GetUILabelBMFont(layout, 2):setText(data.m_cost);
			local icon = UI_GetSkillIcon(data.m_icon)
			layout:addChild(icon)
			if (playerLvl >= data.m_level)then
				--UI_GetUIImageView(layout, 1):setColor(ccc3(255, 255, 255));
				layout:setTouchEnabled(true);		
			else
				--UI_GetUIImageView(layout, 1):setColor(ccc3(125, 125, 125));
				layout:setTouchEnabled(true);
				icon:addColorGray()
			end
			
			layout:addTouchEventListener(ClickHeroIcon);
		end
	end
	
	Fight_Refresh(widget)
end

function Fight_CheckActorSkill()
	local widget = UI_GetBaseWidgetByName("Fight")
	
	if widget == nil then
		return;
	end
	
	UI_GetUILabelBMFont(widget:getChildByTag(102), 50):setText(""..FightMgr:GetInstance().m_itemCount.."/12");
	local img = UI_GetUIImageView(widget, 102);
	for i=1, 12 do
		UI_GetUIImageView(img, i):setVisible(i<=FightMgr:GetInstance().m_itemCount)
	end
	
	local image2Widget = UI_GetUIImageView(widget, 102);
	for i=1, 3 do
		local layout = UI_GetUIImageView(image2Widget, 1000+i)
		local data = GetGameData(DataFileActorSkill, i, "stActorSkillData")
		local playerLvl =1
		if GetLocalPlayer() then
			playerLvl = GetLocalPlayer():GetInt(EPlayer_Lvl);
		end
		if (data.m_level <= playerLvl and layout:getNodeByTag(9999) == nil and layout:getNodeByTag(99) == nil and data.m_cost <= FightMgr:GetInstance().m_itemCount)then
			local armature = GetUIArmature("Efftisikuang")
			armature:getAnimation():playWithIndex(0)
			layout:addNode(armature, 99, 99);
		end
		
		if data.m_cost > FightMgr:GetInstance().m_itemCount then
			layout:removeNodeByTag(99);
		end
	end
	
	-- 提示性引导
	local playerLvl =1
	if GetLocalPlayer() then
		playerLvl = GetLocalPlayer():GetInt(EPlayer_Lvl);
	end
	
	local layout = UI_GetUIImageView(UI_GetUIImageView(widget, 102), 1001)
	if   playerLvl < 10 and  FightMgr:GetInstance().m_itemCount>=3   then
		
		if g_ActorNotifying or (g_isGuide== false and FightMgr:GetInstance().m_isCg == false and layout:isVisible() and g_ActorSkillNotify==false)   then
			local worldPosition = layout:getWorldPosition();
			g_arrowLayout:setVisible(true);
			
			-- 隐藏小手
			g_arrowLayout:getNodeByTag(3):setVisible(false);
			g_arrowLayout:setPosition(worldPosition);
			
			-- 如果还没标记，标记下
			if g_ActorSkillNotify == false then
				g_ActorSkillNotify = true;
			end
			
			-- 如果还没标记，标记下
			if g_ActorNotifying == false then
				g_ActorNotifying = true;
			end
		end
		
	end
end

function Fight_IsShowName(isShow)
	local heroList = vector_uint_:new_local()
	GetCurScene():GetObjectList(heroList)
	
	for i=0, heroList:size() - 1 do
		local heroEntity = GetEntityById(heroList[i], "Hero")
		if (heroEntity) then
			local entityNameUI = getUIControl(heroEntity:GetRender(), EEntityRenderTag_EntityName, "Label")
			entityNameUI:setVisible(isShow);
		end
	end
end

function Fight_AddBlackScreen()
	g_colorLayer:setOpacity(255)
	GetCurScene():GetEffectLayer():addChild(g_colorLayer, 9);
end

function Fight_RemoveBlackScreen()
	g_colorLayer:setOpacity(100)
	g_colorLayer:removeFromParentAndCleanup(true);
end

function Fight_MoveInSceneFirst()
	g_fightWave = 1;
	CCDirector:sharedDirector():setScaleTime(getScaleTime(g_scaleTime))
    InitFightRoundNumber()
	Fight_MoveInScene()
end

function Fight_MoveInScene()
    Log("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa")
	local heroList = vector_Hero__:new_local()
	GetCurScene():GetLeftHeroList(heroList)
	for i=0, heroList:size() - 1 do
		local heroEntity = heroList[i];
		if (heroEntity) then
			local render = heroEntity:GetRender()
			render:stopAllActions();
			render:setPosition(ccp(g_yujianPosLeft.x + heroList:size()*30 - i*70 - 20, g_yujianPosLeft.y))
			
			local actionArry = CCArray:create()
			actionArry:addObject(CCMoveTo:create(1.5, ccp(g_yujianPosMid.x + heroList:size()*30 - i*70 - 20, g_yujianPosMid.y)))
			actionArry:addObject(CCDelayTime:create(0.3))
			actionArry:addObject(CCMoveTo:create(0.1, ccp(g_yujianPosMid.x + heroList:size()*30 - i*70 - 20, g_yujianPosMid.y + 50)))
			actionArry:addObject(CCMoveTo:create(0.1, heroEntity:GetPosition()))
			render:runAction(CCSequence:create(actionArry));
		end
	end
	
	local godAnimal = GetCurScene():GetLeftGodAnimal()
	if (godAnimal)then
		local render = godAnimal:GetRender()
		render:setPosition(ccp(godAnimal:GetPosition().x - (g_yujianPosMid.x - g_yujianPosLeft.x), godAnimal:GetPosition().y))
		local actionArry = CCArray:create()
		actionArry:addObject(CCMoveTo:create(1.5, godAnimal:GetPosition()))
		render:runAction(CCSequence:create(actionArry));
	end
	
	local forceVisiable = false;
	local function Fight_MoveInSceneEnd()
		Log("Fight_MoveInSceneEnd--------------------------------------------------------");
		if FightMgr:GetInstance().m_isCg == false or g_firstFightGuided then
			Log("set FightHPLayout VIsible----------------------------------------------------------");
			Fight_SetHPLayoutVisible(true)
			forceVisiable = true;
            if UI_GetBaseWidgetByName("Fight") then 
               ReFreshFightRound(UI_GetBaseWidgetByName("Fight"))
            end 
		end
		
		g_yujianArmature:removeFromParent();
		if (g_fightWave == 1)then
			g_ActorSkillNotify = false;
			Fight_StartAnim(forceVisiable)
		else
			FightMgr:GetInstance():GetMainState():ResetState(EFightState_DecodeOneResultSet, 1);
		end
		
		Fight_CheckActorSkill();
	end
	
	g_yujianArmature:stopAllActions();
	g_yujianArmature:setOpacity(255)
	g_yujianArmature:setPosition(g_yujianPosLeft)
	local actionArry = CCArray:create()
	actionArry:addObject(CCMoveTo:create(1.5, g_yujianPosMid))
	actionArry:addObject(CCDelayTime:create(0.3))
	actionArry:addObject(CCFadeOut:create(0.2))
	actionArry:addObject(CCDelayTime:create(0.2))
	actionArry:addObject(CCCallFunc:create(Fight_MoveInSceneEnd))
	g_yujianArmature:runAction(CCSequence:create(actionArry));
	GetCurScene():GetObjectLayer():addChild(g_yujianArmature, 0);
end

function Fight_MoveToNextScene()
	
	-- 如果在首次战斗的引导过程中,直接结束引导~~
	if (FightMgr:GetInstance().m_isCg == true) and g_isGuide then
		Log("************Found Fix------");
		Guide_GoNext();
	end

	Fight_SetHPLayoutVisible(false)

	local heroList = vector_Hero__:new_local()
	GetCurScene():GetLeftHeroList(heroList)
	for i=0, heroList:size() - 1 do
		local heroEntity = heroList[i];
		if (heroEntity) then
			local render = heroEntity:GetRender()
			--render:setPosition(ccp(g_yujianPosLeft.x + 150 - i*80, g_yujianPosLeft.y))
			
			local actionArry = CCArray:create()
			actionArry:addObject(CCMoveTo:create(0.1, ccp(g_yujianPosMid.x + heroList:size()*30 - i*70 - 20, g_yujianPosMid.y + 50)))
			actionArry:addObject(CCMoveTo:create(0.1, ccp(g_yujianPosMid.x + heroList:size()*30 - i*70 - 20, g_yujianPosMid.y)))
			actionArry:addObject(CCDelayTime:create(0.3))
			actionArry:addObject(CCMoveTo:create(1.5, ccp(g_yujianPosRight.x + heroList:size()*30 - i*70 - 20, g_yujianPosRight.y)))
			--actionArry:addObject(CCMoveTo:create(0.1, heroEntity:GetPosition()))
			render:runAction(CCSequence:create(actionArry));
		end
	end
	
	local godAnimal = GetCurScene():GetLeftGodAnimal()
	if (godAnimal)then
		local render = godAnimal:GetRender()
		--render:setPosition(ccp(godAnimal:GetPosition().x - (g_yujianPosMid.x - g_yujianPosLeft.x), godAnimal:GetPosition().y))
		local actionArry = CCArray:create()
		actionArry:addObject(CCDelayTime:create(0.5))
		actionArry:addObject(CCMoveTo:create(1.5, ccp(godAnimal:GetPosition().x + (g_yujianPosRight.x - g_yujianPosMid.x), godAnimal:GetPosition().y)))
		render:runAction(CCSequence:create(actionArry));
	end
	
	g_yujianArmature:setOpacity(255)
	g_yujianArmature:setPosition(g_yujianPosMid)
	local actionArry = CCArray:create()
	actionArry:addObject(CCDelayTime:create(0.5))
	actionArry:addObject(CCMoveTo:create(1.5, g_yujianPosRight))
	actionArry:addObject(CCCallFunc:create(Fight_MoveOutSceneEnd))
	g_yujianArmature:runAction(CCSequence:create(actionArry));
	GetCurScene():GetObjectLayer():addChild(g_yujianArmature, 0);
end

function Fight_MoveOutSceneEnd()
	g_yujianArmature:removeFromParent();
	GetCurScene():ChangeMap(g_fightWave);
	FightMgr:GetInstance():GetMainState():ResetState(EFightState_ReplaceMember, 1);
	GetGlobalEntity():GetScheduler():RegisterScript( "Fight_MoveInScene", 500, 1 )
end



if (g_yujianArmature == nil)then
	g_yujianArmature = GetEffArmature("yujianfeixing01");
	g_yujianArmature:getAnimation():playWithIndex(0);
	g_yujianArmature:retain();
	
	g_yujianPosLeft = ccp(-300, 320)
	g_yujianPosMid = ccp(300, 320)
	g_yujianPosRight = ccp(1300, 320)
end

function Fight_StartAnim(forceVisible)
	local function Fight_StartAnimEnd(armature, movementType, movementID)
		if (movementType == LOOP_COMPLETE)then
			if (FightMgr:GetInstance():GetMainState():GetState() == EFightState_PlayStartAnim)then
				FightMgr:GetInstance():GetMainState():ResetState(EFightState_PlayStartAnim, 1);
			end
			armature:removeFromParentAndCleanup(true);
			--Fight_RemoveBlackScreen();
			if forceVisible == nil then
				Fight_Create(true);
			else
				Fight_Create(forceVisible);
			end
		end
	end


	CCDirector:sharedDirector():setScaleTime(getScaleTime(g_scaleTime))
	local kaichang = GetUIArmature("Effkaisi")
	kaichang:getAnimation():playWithIndex(0)
	kaichang:getAnimation():setMovementEventCallFunc(Fight_StartAnimEnd)
	kaichang:setPosition(ccp(480, 320))
	
	GetCurScene():GetEffectLayer():addChild(kaichang, 10);
	
	PlayFightingBg()
end

if (g_colorLayer == nil)then
	g_colorLayer = CCLayerColor:create(ccc4(0,0,0,122))
	g_colorLayer:setAnchorPoint(ccp(0.5, 0.5));
	g_colorLayer:setPosition(ccp(0, 0));
	g_colorLayer:setScale(MainScene:GetInstance().m_maxScale/MainScene:GetInstance().m_minScale);
	g_colorLayer:setContentSize(CCSizeMake(960, 640));
	g_colorLayer:retain();
end

if (g_heroSelect == nil)then
	g_heroSelect = nil
end

function Fight_HpEffect(skillId)
	Log("Fight_HpEffect skillId ="..skillId)
	g_actorSkillID = skillId;
	GetCurScene():Pause();
	g_actionEnd=false;
	--g_heroSelect = hero;

	--local render = hero:GetRender();
	--render:removeFromParentAndCleanup(true)
	--GetCurScene():GetObjectLayer():addChild(g_colorLayer, 99998);
	--render:setZOrder(99999);
	--GetCurScene():GetEffectLayer():addChild(render, 99999);
	local armature = nil;
	local hurtSound = nil;
	if (skillId == GameServer_pb.en_ActorSkill_Damage)then
		armature = GetUIArmature("jineng01");
		CreateGameEffectMusic("zhugongji_sxwj1.wav")
		hurtSound = "zhugongji_sxwj2.wav";
	elseif (skillId == GameServer_pb.en_ActorSkill_Heal)then
		armature = GetUIArmature("jineng03");
		CreateGameEffectMusic("zhugongji_kmfc1.wav")
		hurtSound = "zhugongji_kmfc2.wav";
	else
		armature = GetUIArmature("jineng02");
		CreateGameEffectMusic("zhugongji_lyhx1.wav")
		hurtSound = "zhugongji_lyhx2.wav";
	end
	
	local posList = GetCurScene():GetHeroPosList();
	if (skillId == GameServer_pb.en_ActorSkill_Heal)then
		--armature:setPosition(ccp(posList[4].x, posList[4].y + 80))
	else
		--armature:setPosition(ccp(posList[13].x, posList[13].y + 80))
	end
	
	local function Fight_ActionEnd(armature, movementType, movementID)
		if (movementType == LOOP_COMPLETE)then
			if (movementID == "0")then
				armature:getAnimation():playWithIndex(1)
				CreateGameEffectMusic(hurtSound);
			elseif (movementID == "1")then
				g_actionEnd = true;
				Fight_Resume();
				armature:removeFromParentAndCleanup(true);
			end
		end
	end
	
	
	armature:setPosition(ccp(480,320))
	armature:getAnimation():playWithIndex(0)
	armature:getAnimation():setMovementEventCallFunc(Fight_ActionEnd)
	GetCurScene():GetEffectLayer():addChild(armature, 9999);
	--render:GetLayer(ERenderLayerType_UI):addChild(jiaxue, 5);
end

function Fight_GetLayout(widget, hero)
	local heroList = vector_Hero__:new_local()
	GetCurScene():GetLeftHeroList(heroList)
	
	for i=0, heroList:size() - 1 do
		if (hero == heroList[i]) then
			return UI_GetUIImageView(widget, i + 1), i;
		end
	end
	
	return nil
end

function Fight_PlayReplaceMemAnim(pos)
	local diaoguai = GetUIArmature("Effdiaoguai")
	diaoguai:getAnimation():playWithIndex(0)
	diaoguai:getAnimation():setMovementEventCallFunc(ArmatureEnd)
	diaoguai:setPosition(pos)
	GetCurScene():GetEffectLayer():addChild(diaoguai, 1);
end

function Fight_Refresh(widget)
	--UI_GetUILabelBMFont(widget:getChildByTag(101), 1):setText(""..FightMgr:GetInstance().m_itemCount.."/12");
	--UI_GetUILabelBMFont(widget, 101):setScale(1.3);
	
	Fight_CheckActorSkill();
end

function Fight_ItemCountChg(hero, var)
	local widget = UI_GetBaseWidgetByName("Fight");
	
	if (widget)then
		--[[UI_GetUILabel(widget, 101):stopAllActions();
		UI_GetUILabel(widget, 101):setScale(1);
		local actionArry = CCArray:create()
		actionArry:addObject(CCScaleTo:create(1, 1.6))
		actionArry:addObject(CCCallFunc:create(ChgItemCount))
		actionArry:addObject(CCScaleTo:create(1, 1))
		UI_GetUILabel(widget, 101):runAction(CCSequence:create(actionArry));]]
		
		local function RemoveAnim(armature, movementType, movementID)
			if (movementType == LOOP_COMPLETE)then
				if (movementID == "1")then
					armature:getAnimation():playWithIndex(2);
				elseif (movementID == "2")then
					armature:removeFromParentAndCleanup(true);
				end
			end
		end

		if ( hero and UI_GetUILabel(widget, 102):isVisible())then
			local chgCount = var:Int();
			local offset = chgCount*5 + 5;
			local pos = hero:GetRender():GetCurPos();
			pos = ccp(pos.x, pos.y + 120)
			for i=1, chgCount do
				local changshenshi = GetUIArmature("Effchangshenshi")
				changshenshi:getAnimation():playWithIndex(1)
				changshenshi:setPosition(ccp(pos.x + i*10 - offset, pos.y));
				
				local time = changshenshi:getAnimation():getDurationTween() / 60;
				local x, y = UI_GetUILabel(widget, 102):getPosition();
				local randomValue = Random:RandomInt(1, 4);
				local x1 = (x - pos.x) * randomValue / 5;
				randomValue = Random:RandomInt(1, 4);
				local y1 = (y - pos.y) * randomValue / 5;
				local array = CCPointArray:create(20);
				array:addControlPoint(ccp(pos.x, pos.y));
				array:addControlPoint(ccp(pos.x + x1, pos.y + y1));
				array:addControlPoint(ccp(x, y));
				--CCCardinalSplineTo
				--    作用：创建一个样条曲线轨迹的动作
				--    参数1：完成轨迹所需的时间
				--    参数2：控制点的坐标数组
				--    拟合度  其值= 0 路径最柔和
				local cardinalSplineTo = CCCardinalSplineTo:create(time, array, 0);
				changshenshi:getAnimation():setMovementEventCallFunc(RemoveAnim)
				changshenshi:runAction(cardinalSplineTo);
				widget:addNode(changshenshi, 9999);
			end
		else
			--[[local function ChgItemCount()
				UI_GetUILabelBMFont(widget, 101):setText(""..FightMgr:GetInstance().m_itemCount);
			end
		
			local actionArry = CCArray:create()
			actionArry:addObject(CCScaleTo:create(0.5, 1.6))
			actionArry:addObject(CCCallFunc:create(ChgItemCount))
			actionArry:addObject(CCScaleTo:create(0.5, 1))
			UI_GetUILabelBMFont(widget, 101):runAction(CCSequence:create(actionArry));]]
		end
		
		UI_RefreshBaseWidgetByName("Fight")
	end
	
	Fight_CheckActorSkill();
end

function Fight_SendFinish()
	Packet_Cmd(GameServer_pb.CMD_FIGHT_PLAYFINISH);
	ShowWaiting();
end

g_useItemRev=false;
g_actionEnd=false;
g_revAction=false;
function FIGHT_USEACTORSKILL(pkg)
	local tmp = GameServer_pb.CMD_FIGHT_USEACTORSKILL_SC();
	tmp:ParseFromString(pkg)
	Log("FIGHT_USEITEM");
	FightMgr:GetInstance().m_isCanRevAcion = true;
	
	g_useItemRev = true;
end

function Fihgt_RevAction()
	if (g_useItemRev)then
		g_revAction = true;
		Fight_Resume();
	end
end

function Fight_Resume()
	if (g_useItemRev and g_revAction and g_actionEnd)then
		FightMgr:GetInstance():FightResume();
		g_useItemRev = false;
		
		--if (g_actionEnd)then
		g_actionEnd = false;
		GetCurScene():Resume(g_actorSkillID);
		--end
	end

	if(FightMgr:GetInstance().m_isCanRevAcion and g_actionEnd and FightMgr:GetInstance().m_isCg)then
		FightMgr:GetInstance():FightResume();
		
		g_actionEnd = false;
		GetCurScene():Resume(g_actorSkillID);
	end
end





function Fight_Dlg(var)
	local dlgId = var:Int();
	Log("dlgId"..dlgId)
	
	local dlgData = g_dialog[dlgId];
	if (dlgData)then
		Dialog_create(dlgData)
	else
		FightMgr:GetInstance():GetMainState():ResetState(EFightState_ReadNextAction, 1);
	end
	
	CloseFightActorSkillNotify();
end


function Dialog_create(dlgData)
	local widget = nil
	if (dlgData.left)then
		widget = UI_CreateBaseWidgetByFileName("Dialog_Left.json");
	else
		widget = UI_CreateBaseWidgetByFileName("Dialog_Right.json");
	end
	if (widget == nil)then
		Log("Dialog_create error");
		return;
	end
	
	local showEntityName = ""
	if (dlgData.name == "")then
		if (GetLocalPlayer()) then
			Log("TryGet Hero Name---------------")
			local hero = GetEntityById(GetLocalPlayer():GetHeroList()[0], "Hero");
			showEntityName = hero:GetEntityName();
			Log("TryGet Hero Name---------------:"..showEntityName)
			--UI_GetUILabel(widget, 2):setText(hero:GetEntityName());
		end
	else
		showEntityName = dlgData.name;
		--UI_GetUILabel(widget, 2):setText(dlgData.name);
	end	
	
	Log("----------SHowName:"..showEntityName)
	
	if (dlgData.imgId ~= "") then
		UI_GetUIImageView(widget, 1):loadTexture("Icon/Gaine/"..dlgData.imgId);
	else
		if (GetLocalPlayer()) then
			local hero = GetEntityById(GetLocalPlayer():GetHeroList()[0], "Hero");
			UI_GetUIImageView(widget, 1):loadTexture("Icon/Gaine/"..hero:GetHeroData().m_icon..".png");
		end
	end

	local strResultText = FormatString("CommonDelayLabel", showEntityName, dlgData.info);	
	Log("--------------:"..strResultText)
	local info = CompLabel:GetDefaultCompLabel(strResultText, 520);
	if (dlgData.left)then
		info:setPosition(ccp(-160, info:getSize().height/2));
	else
		info:setPosition(ccp(-360, info:getSize().height/2));
	end
	UI_GetUIImageView(widget, 100):addChild(info, 2, 1000);
	
	--Fight_SetHPLayoutVisible(false)
	local function ClickScene(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then  
            if info ~= nil then 
                local isShowAll = CompLabel:IsShowAll(info)
                if isShowAll == true then
			        FightMgr:GetInstance():GetMainState():ResetState(EFightState_ReadNextAction, 1);
                else
                    UI_GetUIImageView(widget, 100):removeChildByTag(1000)             
	                local strResultText1 = FormatString("CommonDelayLabel1", showEntityName, dlgData.info);
                    local info1 = CompLabel:GetDefaultCompLabel(strResultText1, 520);
	                if (dlgData.left)then
		                info1:setPosition(ccp(-160, info1:getSize().height/2));
	                else
		                info1:setPosition(ccp(-360, info1:getSize().height/2));
	                end 
                    UI_GetUIImageView(widget, 100):addChild(info1, 2, 1000);
                    info = nil
                end
            else   
			    FightMgr:GetInstance():GetMainState():ResetState(EFightState_ReadNextAction, 1);
            end
			--Fight_SetHPLayoutVisible(true)
		end
	end
	widget:addTouchEventListener(ClickScene);
	
end

function Fight_SetHPLayoutVisible(isVisible)
	Log("Fight_SetHPLayoutVisible:"..tostring(isVisible).."----------------------------------" );
	local widget = UI_GetBaseWidgetByName("Fight");
	if (widget)then
		widget:setVisible(isVisible);
		
		if isVisible == false then
			g_arrowLayout:setVisible(false);
		end
		
		--[[for i=1, 3 do
			local layout = UI_GetUIImageView(widget, i)
			layout:setVisible(isVisible);
		end
		UI_GetUIImageView(widget, 101):setVisible(isVisible)
		UI_GetUIImageView(widget, 102):setVisible(isVisible)]]
	end
end

function Fight_AddHp()
	Fight_SetHPLayoutVisible(true)
	
	Guide_AddHp();
end

function FIGHT_QUIT(pkg)
	g_fightQuit = true;
	FightMgr:GetInstance():GetMainState():ResetState(EFightState_Pause, 0);
	
	CloseFightActorSkillNotify();
end

function Fight_ShowWave(isLast)
	g_fightWave = g_fightWave + 1;
	--[[local path = "";
	if (isLast ~= 0)then
		path = "zhandou/font_004.png";
	else
		path = "zhandou/font_00"..g_fightWave..".png";
		g_fightWave = g_fightWave + 1;
	end
	
	
	local widget = UI_GetBaseWidgetByName("Fight");
	local img = ImageView:create();
	img:loadTexture(path);
	widget:addChild(img, 9999);
	
	local function EndAction()
		img:removeFromParentAndCleanup(true);
	end
	img:setScale(0.01);
	img:setPosition(ccp(680, 320))
	
	local actionArry = CCArray:create()
	actionArry:addObject(CCScaleTo:create(1, 1))
	actionArry:addObject(CCCallFunc:create(EndAction))
	img:runAction(CCSequence:create(actionArry));
	
	--img:runAction(CCScaleTo:create(1, 1));]]
end

ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_FIGHT_QUIT, "FIGHT_QUIT" );
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_FIGHT_USEACTORSKILL, "FIGHT_USEACTORSKILL" );



--获取当前的关卡的怪物波数 客户端行为 
function GetMonsterBySectionId()
   local sceneID = g_curDungeonInfo[g_selectSectionIndex].iSceneID
   local data = GetGameData(DataFileScene, sceneID, "stSceneData")
   local monsterstring = data.m_monster
   local function getMonster(src,tb)
        Log("src==="..tostring(src))
        local function spilit(txt)
        	-- body
        	table.insert(tb,txt)
        end
        string.gsub(src,"(%w+)#?",spilit)
    end
    local monstertab = {}
    getMonster(monsterstring,monstertab)
    Log("monsterNumber ====="..#monstertab)
    return (#monstertab)
end

--战斗回合
if g_FightRoundNumber == nil then 
   g_FightRoundNumber = 1
end

--战斗总回合数据
if g_FightRoundTotolNumber == nil then 
   g_FightRoundTotolNumber = 0
end 

function InitFightRoundNumber()
   g_FightRoundNumber = 1
   if g_openUIType ~= EUIOpenType_EquipFuben and g_openUIType ~= EUIOpenType_Dugeon then 
       g_FightRoundTotolNumber = 1
   else 
      g_FightRoundTotolNumber = GetMonsterBySectionId()
   end
   local widget = UI_GetBaseWidgetByName("Fight")
end

function ReFreshFightRound(widget)
   --战斗回合数
      --local monterNumber = GetMonsterBySectionId()
      if g_openUIType ~= EUIOpenType_EquipFuben and g_openUIType ~= EUIOpenType_Dugeon then 
         UI_GetUILabelBMFont(widget,150):setVisible(false)
      else 
         Log("ReFreshFightRound============"..g_FightRoundNumber)
         UI_GetUILabelBMFont(widget,150):setVisible(true)
         UI_GetUILabelBMFont(widget,150):setText(g_FightRoundNumber.."/"..g_FightRoundTotolNumber)
		-- UI_SetLabelText(widget,150,g_FightRoundNumber.."/"..g_FightRoundTotolNumber)
         g_FightRoundNumber = g_FightRoundNumber + 1
      end 
    --end	
end