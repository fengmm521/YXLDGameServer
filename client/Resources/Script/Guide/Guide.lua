--当前的引导ID
if (g_curGuideId == nil)then
	g_curGuideId = 0
end
--当前引导走到第几步
if (g_curGuideStep == nil)then
	g_curGuideStep = 0;
end
--引导的那个手
if (g_arrowLayout == nil)then
	g_arrowLayout = nil;
end

--是否正在引导中
if (g_isGuide == nil)then
	g_isGuide = false;
end
--后面的引导
if (g_nextGuide == nil)then
	g_nextGuide = {};
end

if (g_clipGuide == nil)then
	g_clipGuide = nil;
end

g_orgClickConPos = nil;
g_orgClickArrowConPos = nil;

g_testGuide = 0;
g_curLayerTag = nil

function Guide_Init()
	local function ClickScene(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
				Guide_GoNext();
		end
	end
	g_guideWidget = GUIReader:shareReader():widgetFromJsonFile("Guide.json");
	g_guideWidget:retain();
	--[[local colorLayer = CCLayerColor:create(ccc4(0,0,0,100))
	colorLayer:setAnchorPoint(ccp(0.5, 0.5));
	colorLayer:setPosition(ccp(-960, -640));
	colorLayer:setScale(MainScene:GetInstance().m_maxScale/MainScene:GetInstance().m_minScale);
	colorLayer:setContentSize(CCSizeMake(1920, 1280));

	local clipImg = CCSprite:create("01.png");
	clipImg:setScale(0.4)
	g_clipGuide = CCClippingNode:create(clipImg);
	g_clipGuide:setAnchorPoint(ccp(0.5, 0.5));
	g_clipGuide:setAlphaThreshold(0.0005);
	g_clipGuide:setInverted(true);
	g_clipGuide:addChild(colorLayer);
	g_clipGuide:setScale(MainScene:GetInstance().m_minScale);
	g_clipGuide:setVisible(false);
	--g_guideWidget:addNode(g_clipGuide);
	MainScene:GetInstance():addChild(g_clipGuide, EMSTag_GuideLayer, EMSTag_GuideLayer);]]
	
	g_guideWidget:addTouchEventListener(ClickScene);
	g_arrowLayout = Widget:create();
	
	g_arrowLayout:setVisible(false);
	--g_arrowLayout:setAnchorPoint(ccp(0.5, 0.5));
	local arrowNode = CCSprite:create("xinshouyindao/ico.png");
	arrowNode:setPosition(ccp(30, -30))
	g_arrowLayout:addNode(arrowNode, 2, 3);
	arrowNode:setVisible(false)
	
	local arrowArmature = GetUIArmature("Effmaobi")
	arrowArmature:getAnimation():playWithIndex(0)
	g_arrowLayout:addNode(arrowArmature, 1, 2);
	g_arrowLayout:setScale(MainScene:GetInstance().m_minScale);
	--g_arrowLayout:addChild(g_clipGuide);
	
	--g_guideWidget:addChild(g_arrowLayout, 9999, 9999);
	g_arrowLayout:setName("FFFFF");
	MainScene:GetInstance():addChild(g_arrowLayout, EMSTag_Guide, EMSTag_Guide);
	
	
	local guideImg = UI_GetUIImageView(g_guideWidget, 1);
	local clickConWidget = UI_GetUIImageView(guideImg, 4532482);
	local clickArrowWidget = UI_GetUIImageView(guideImg, 4532483);
	
	local tmpX, tmpY = clickConWidget:getPosition();
	g_orgClickConPos = ccp(tmpX, tmpY)
	
	tmpX, tmpY = clickArrowWidget:getPosition();
	g_orgClickArrowConPos = ccp(tmpX, tmpY)
	
	--g_arrowLayout:setPosition(ccp(960, 640));
	--g_arrowLayout:setVisible(true)
end

if (g_guideWidget == nil)then
	Guide_Init()
end

function Guide_CheckIsHave()
	if (g_curGuideId ~= 100 and g_curGuideStep == 1)then
		SceneMgr:GetInstance().m_nextState = EGameState_MainUi;
	end
end
--引导开始
function Guide_Start(id)
	
	Guide_End()
	
	if g_testGuide ~= 0 and id ~= g_testGuide and id ~= 100 then
		local tab = GameServer_pb.CMD_GUIDE_FINISH_CS();
		tab.iGuideID = id;
		Packet_Full(GameServer_pb.CMD_GUIDE_FINISH, tab);
		return;
	end
	
	Log("Guide_Start id="..id)
	if (g_GuideDataTable[id] == nil)then
		return;
	end
	
	
	
	--HideNoticeFlag();
	g_curGuideId = id;
	g_curGuideStep = 1;
	
	Guide_StepStart();
end
--引导结束
function Guide_End()
	Log("Guide_End")
	g_isGuide = false;
	g_curGuideId = 0;
	g_curGuideStep = 0;
	MainScene:GetInstance():removeChildByTag(EMSTag_Guide_Notice);
	g_arrowLayout:setVisible(false);
	--g_clipGuide:setVisible(false);

	ShowNoticeFlag();
end
--获取当前引导步骤的页面 UILayer
--[[function Guide_GetUILayer()
	local widget = UI_GetBaseWidgetByName(g_GuideDataTable[g_curGuideId][g_curGuideStep].baseWidgetName)
	if (widget)then	
		--Log("baseWidgetName nil---"..g_GuideDataTable[g_curGuideId][g_curGuideStep].baseWidgetName)
		return UIMgr:GetInstance():GetUILayarByWidget(widget);
	end

	return nil;
end
--]]

function Guide_GetCurUILayer()
	if (UI_IsCurBaseWidgetByName(g_GuideDataTable[g_curGuideId][g_curGuideStep].baseWidgetName))then	
		return UIMgr:GetInstance():GetUILayarByWidget(GetCurWidget());
	end
	
	if g_GuideDataTable[g_curGuideId][g_curGuideStep].isMainUiTopWidget then
		return  UIMgr:GetInstance():GetUILayarByWidget(g_mainUiTopWidget);
	end
	
	if g_GuideDataTable[g_curGuideId][g_curGuideStep].UseMessageBox then
		return UIMgr:GetInstance():getMessageBoxLayer();
	end
	
	--Log("baseWidgetName nil---"..g_GuideDataTable[g_curGuideId][g_curGuideStep].baseWidgetName)
	return nil;
end

local function getOffset()
	local frameSize = CCEGLView:sharedOpenGLView():getFrameSize();
	local uiSize = CCSizeMake(MainScene:GetInstance().m_minScale*960, MainScene:GetInstance().m_minScale*640);
	
	local offsetX = (frameSize.width - uiSize.width)/2;
	local offsetY = (frameSize.height - uiSize.height)/2;
	
	return offsetX, offsetY
end

--引导某一步开始
function Guide_StepStart()


	--Log("Guide_StepStart")
	
	
	
	if (g_GuideDataTable[g_curGuideId][g_curGuideStep] == nil)then
		Log("g_GuideDataTable[g_curGuideId][g_curGuideStep] nil")
		Guide_End()
		return
	end	

	
	SceneMgr_Guide();
	
	local guideData = g_GuideDataTable[g_curGuideId][g_curGuideStep]
	if (guideData.sendServer)then
		local tab = GameServer_pb.CMD_GUIDE_FINISH_CS();
		tab.iGuideID = g_curGuideId;
		Packet_Full(GameServer_pb.CMD_GUIDE_FINISH, tab);
		Log("sendID ="..g_curGuideId)
		guideData.sendServer = false;
	end
	
	
	
	local uiLayer = Guide_GetCurUILayer();
	if (uiLayer == nil)then
		return;
	end
	
	
	
	local guideNode = nil;
	if (guideData.widgetName)then
		guideNode = uiLayer:getWidgetByName(guideData.widgetName);
		if (guideNode == nil)then
			Log("guideData.widgetName nil ="..guideData.widgetName)
			return;
		end
	end
	
	g_isGuide = true;
	uiLayer:setIsGuide(true);
	g_curLayerTag = uiLayer:getTag();
	uiLayer:cleanGuideWidget();
	if (guideData.widgetName)then
		uiLayer:addGuideWidgetName(guideData.widgetName);
		
		-- 有时候显示的按钮和Event按钮不同
		if guideData.eventWidgetName ~= nil then
			uiLayer:addGuideWidgetName(guideData.eventWidgetName);
		end
				
		local position = guideNode:getWorldPosition();
		Log("Pos----Name:"..guideData.widgetName.."   "..position.x..", "..position.y);
		if (guideData.widgetPos)then
			local offsetX, offsetY = getOffset();
			position = ccp(guideData.widgetPos.x * MainScene:GetInstance().m_minScale + offsetX, guideData.widgetPos.y*MainScene:GetInstance().m_minScale + offsetY);
		end
		
		if guideData.needCenter == true and guideNode ~= nil then
			local gwSize = guideNode:getContentSize();
			Log("NeedCenter---"..gwSize.width..","..gwSize.height);
			position.x = position.x + gwSize.width/2;
			position.y = position.y + gwSize.height/2;
		end
		
		g_arrowLayout:setPosition(position);
		
		
		local function SetPos()
			g_arrowLayout:setPosition(position);
		end

		g_arrowLayout:setVisible(true);
		g_arrowLayout:removeChildByTag(1);
		g_arrowLayout:stopAllActions();
		
		-- 最后处理特殊引导
		if guideData.moveTargetWidget then
			uiLayer:addGuideWidgetName(guideData.moveTargetWidget);
			
			if (g_curGuideId == 3) then
				--local hero = GetEntityById(GetLocalPlayer():GetHeroList()[1], "Hero");
				--local heroNode = UI_GetHeroIcon(hero:GetHeroData().m_icon);
				local cloneNode = tolua.cast(guideNode:getParent(),"Widget"):clone();
				cloneNode:setPosition(ccp(0, 0))
				g_arrowLayout:addChild(cloneNode, 1, 1);
				
				local contentSize = cloneNode:getContentSize();
				
				local anthor = cloneNode:getAnchorPoint();
				local offsetCCP = ccp(-contentSize.width * 0.5, -contentSize.height * 0.5);
				cloneNode:setPosition(offsetCCP);
			end
			
			
			local targetNode = uiLayer:getWidgetByName(guideData.moveTargetWidget);
			
			local targetWorldPos = targetNode:getWorldPosition();
			
			-- 播放手势动画	
			Log("from------- "..position.x.." "..position.y.." ->"..targetWorldPos.x.." "..targetWorldPos.y);
			
			local moveAction = CCMoveTo:create(1.5, targetWorldPos);
			local endCall = CCCallFunc:create(SetPos);
			
			local actionArry = CCArray:create()
			actionArry:addObject(moveAction)
			actionArry:addObject(endCall)
			g_arrowLayout:runAction(CCRepeatForever:create(CCSequence:create(actionArry)));
			--g_arrowLayout:runAction(moveAction);
			
			-- 隐藏特效
			g_arrowLayout:getNodeByTag(2):setVisible(false);
			g_arrowLayout:getNodeByTag(3):setVisible(true);
		else
			g_arrowLayout:getNodeByTag(2):setVisible(true);
			g_arrowLayout:getNodeByTag(3):setVisible(false);
		end
		
	else
		--g_clipGuide:setVisible(false);
	end
	
	-- 安全起见，先删除吧
	MainScene:GetInstance():removeChildByTag(EMSTag_Guide_Notice);
	
	-- 坑，这里还不够，原因是这段代码可能是在处理Touch的逻辑里，虽然remove了,但是还没来得及将parent置为NULL。
	g_guideWidget:setParent(nil)
	MainScene:GetInstance():AddChild(g_guideWidget, EMSTag_Guide_Notice, EMSTag_Guide_Notice, guideData.widgetName == nil);
	
	local guideImg = UI_GetUIImageView(g_guideWidget, 1);
	guideImg:setVisible(false);
	UI_GetUIImageView(g_guideWidget, 2):setVisible(false);
	UI_GetUIImageView(g_guideWidget, 3):setVisible(false);
	--g_guideWidget:removeChildByTag(10);
	guideImg:removeChildByTag(10);
	
	local clickConWidget = UI_GetUIImageView(guideImg, 4532482);
	local clickArrowWidget = UI_GetUIImageView(guideImg, 4532483);
	clickConWidget:setVisible(false);
	clickArrowWidget:setVisible(false);
	
	if (guideData.textPanel)then
		--if (guideData.textPanel.up == nil)then
			local guideInfo = CompLabel:GetDefaultCompLabel(guideData.textPanel.text, 410);
			guideImg:addChild(guideInfo, 10, 10);
			guideImg:setVisible(true);
			
			guideInfo:setAnchorPoint(ccp(0.5, 0.5));
			
			guideImg:setPosition(ccp(guideData.textPanel.pos.x, guideData.textPanel.pos.y))
			
			if guideData.widgetName == nil then
				clickConWidget:setVisible(true);
			else
				clickArrowWidget:setVisible(true);
			end
			
			if (guideData.textPanel.left == false) then
				
				guideInfo:setPosition(ccp(0, -110) );
				--guideInfo:setScaleX(-1);
				--guideInfo:setPosition(ccp(guideData.textPanel.pos.x - 10, guideData.textPanel.pos.y + guideInfo:getContentSize().height/2 - 20));
				guideInfo:setScaleX(-1);
				guideImg:setScaleX(-1);
				
				
				clickConWidget:setScaleX(-1)
				if g_orgClickConPos ~= nil then
					clickConWidget:setPosition(ccp(- g_orgClickConPos.x, g_orgClickConPos.y)  );
				end
				
				clickArrowWidget:setScaleX(-1)
				clickArrowWidget:setPosition(ccp(- g_orgClickArrowConPos.x, g_orgClickArrowConPos.y))
			else
				guideInfo:setPosition(ccp(0, -110) );
				--guideInfo:setPosition(ccp(guideData.textPanel.pos.x - 10, guideData.textPanel.pos.y + guideInfo:getContentSize().height/2 - 20));
				guideImg:setScaleX(1);
				clickConWidget:setScaleX(1)
				clickConWidget:setPosition(g_orgClickConPos);
				
				clickArrowWidget:setScaleX(1)
				clickArrowWidget:setPosition(g_orgClickArrowConPos)
			end
		--[[else
			local guideInfo = CompLabel:GetDefaultCompLabel(guideData.textPanel.text, 200);
			g_guideWidget:addChild(guideInfo, 1, 10);
			if (guideData.textPanel.up)then
				UI_GetUIImageView(g_guideWidget, 2):setVisible(true);
				UI_GetUIImageView(g_guideWidget, 2):setPosition(ccp(guideData.textPanel.pos.x, guideData.textPanel.pos.y))
				guideInfo:setPosition(ccp(guideData.textPanel.pos.x - 100, guideData.textPanel.pos.y + guideInfo:getContentSize().height/2 - 90));
			else
				UI_GetUIImageView(g_guideWidget, 3):setVisible(true);
				UI_GetUIImageView(g_guideWidget, 3):setPosition(ccp(guideData.textPanel.pos.x, guideData.textPanel.pos.y))
				guideInfo:setPosition(ccp(guideData.textPanel.pos.x - 100, guideData.textPanel.pos.y + guideInfo:getContentSize().height/2 + 90));
			end
			--]]
		--end
		--UI_GetUIImageView(guideImg, 1):setVisible(guideData.textPanel.left == false);
		--UI_GetUIImageView(guideImg, 2):setVisible(guideData.textPanel.left);
	end
end

function Guide_StepEnd()
	Log("Guide_StepEnd")
	g_isGuide = false;
	local guideData = g_GuideDataTable[g_curGuideId][g_curGuideStep]
	MainScene:GetInstance():removeChildByTag(EMSTag_Guide_Notice);
	g_guideWidget:removeFromParent();
	g_arrowLayout:setVisible(false);
	g_arrowLayout:getNodeByTag(3):setVisible(false);
	
	--g_clipGuide:setVisible(false);
	
	if g_curLayerTag ~= nil then
		local uiLayer = tolua.cast(MainScene:GetInstance():getChildByTag(g_curLayerTag), "TouchGroup")  --Guide_GetCurUILayer();
		if (uiLayer)then
			uiLayer:setIsGuide(false);
			uiLayer:cleanGuideWidget();
		end
	end
	
	g_curGuideStep = g_curGuideStep + 1;
	Log("g_curGuideStep--------"..g_curGuideStep)
	if (g_curGuideStep > #g_GuideDataTable[g_curGuideId])then
		Guide_End();
	else
		Guide_StepStart();
	end
end
--引导update
function Guide_Update()
	
	if (g_curGuideId == 0)then
		if (#g_nextGuide > 0)then
			local useGuideID = g_nextGuide[1];
			table.remove(g_nextGuide, 1);
			Guide_Start(useGuideID);
		end
		
		return;
	end
	
	if (g_GuideDataTable[g_curGuideId] == nil)then
		Guide_End()
		return;
	end
	
	if (g_GuideDataTable[g_curGuideId][g_curGuideStep] == nil)then
		Guide_End()
		return;
	end
	
	Guide_Check();
end
--检查位置和是否结束
function Guide_Check()
	
	if (g_isGuide == false)then
		Guide_StepStart()
		return
	end
	
	
	
	local guideData = g_GuideDataTable[g_curGuideId][g_curGuideStep]
	local uiLayer = Guide_GetCurUILayer();
	if (uiLayer and g_GuideDataTable[g_curGuideId][g_curGuideStep].widgetName and guideData.moveTargetWidget == nil)then
		local guideNode = uiLayer:getWidgetByName(g_GuideDataTable[g_curGuideId][g_curGuideStep].widgetName);
		local position = guideNode:getWorldPosition()
		if (guideData.widgetPos)then
			local offsetX, offsetY = getOffset();
			position = ccp(guideData.widgetPos.x * MainScene:GetInstance().m_minScale + offsetX, guideData.widgetPos.y*MainScene:GetInstance().m_minScale + offsetY);
			--position = ccp(guideData.widgetPos.x*MainScene:GetInstance().m_minScale, guideData.widgetPos.y*MainScene:GetInstance().m_minScale);
		end
		
		if guideData.needCenter == true and guideNode ~= nil then
			local gwSize = guideNode:getContentSize();
			Log("NeedCenter---"..gwSize.width..","..gwSize.height);
			position.x = position.x + gwSize.width/2;
			position.y = position.y + gwSize.height/2;
		end
		
		g_arrowLayout:setPosition(position);
		--g_clipGuide:setPosition(guideNode:getWorldPosition());
	end
	
	local goToNext = false;
	local endCondition = guideData.endCondition

	if (endCondition.Type == "UiOpen") then
		local nextWidget = UI_GetBaseWidgetByName(endCondition.args[1]);
		if (nextWidget)then
			goToNext = true
		end
	elseif (endCondition.Type == "UiClose") then
		local nextWidget = UI_GetBaseWidgetByName(endCondition.args[1]);
		if (nextWidget == nil)then
			goToNext = true
		end
	elseif (endCondition.Type == "Formation") then
		if (GetLocalPlayer():GetFormationList()[endCondition.args[1]] ~= 0)then
			goToNext = true
		end
	elseif (endCondition.Type == "HeroFightSoulBag") then
		local hero = GetEntityById(GetLocalPlayer():GetHeroList()[0], "Hero");
		local wearList = hero:GetFightSoulBags():GetFightSoulList();
		
		if ( wearList:size() > 0 and wearList[0])then
			goToNext = true
		end
	end
	
	if (goToNext == true)then
		Guide_GoNext();
	end
end
--结束当前某一步
function Guide_GoNext()
	Log("goNext------------");
	local endCondition = g_GuideDataTable[g_curGuideId][g_curGuideStep].endCondition
	if (endCondition.sendServer)then
		local tab = GameServer_pb.CMD_GUIDE_FINISH_CS();
		tab.iGuideID = g_curGuideId;
		Packet_Full(GameServer_pb.CMD_GUIDE_FINISH, tab);
	end
	Guide_StepEnd()
end
--服务器下发某一个引导开启
function NOTIFY_GUIDE(pkg)
	EndWaiting();
	local tmp = GameServer_pb.CMD_NOTIFY_GUIDE_SC();
	tmp:ParseFromString(pkg)
	
	Log("!!!!!"..tostring(tmp))
	g_nextGuide[#g_nextGuide + 1] = tmp.iGuideID
end

g_firstFightGuided = false;
function Guide_AddHp()
	g_firstFightGuided = true;
	Guide_Start(100);
end


function RoleLoginFinish()
	if g_testGuide > 0 then
		sendGMMsg("setlifeatt#1#100000000");
		sendGMMsg("setlifeatt#2#100000000");
		sendGMMsg("addItem#13014#1000")
		sendGMMsg("addexp#10000")

		--Guide_Start(g_testGuide);
	end
end



--ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_NOTIFY_GUIDE, "NOTIFY_GUIDE" );
