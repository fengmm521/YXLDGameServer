--根据名字获取基础控件
function UI_GetBaseWidgetByName(widgetName)
	return UIMgr:GetInstance():GetBaseWidgetByName(widgetName);
end
--获取当前的基础控件
function GetCurWidget()
	return UIMgr:GetInstance():GetCurWidget();
end

function GetWidgetFromFile(filename)
    return UIMgr:GetInstance():getWidgetFromFile(filename)
end
--根据文件名创建一个基础控件
function UI_CreateBaseWidgetByFileName(widgetName, action, time)
	if (action == nil)then
		action = EUIOpenAction_None;
	end
	if (time == nil)then
		time = 0.25;
	end
	return UIMgr:GetInstance():CreateBaseWidgetByFileName(widgetName, action, time);
end

function UI_AddCloseEvent(widget)
	widget:addTouchEventListener(UI_ClickCloseCurBaseWidget);
end

function UI_RefreshCurBaseWidget()
	UIMgr:GetInstance():RefreshCurBaseWidget();
end

function UI_RefreshAllBaseWidget()
	UIMgr:GetInstance():RefreshAllBaseWidget();
end

function UI_RefreshBaseWidgetByName(name)
	UIMgr:GetInstance():RefreshBaseWidgetByName(name);
end

--关闭当前基础控件
function UI_CloseCurBaseWidget(action, time)
	if (action == nil)then
		action = EUICloseAction_Lessen
	end
	if (time == nil)then
		time = 0.3
	end
	UIMgr:GetInstance():CloseCurBaseWidget(action, time);
end
--关闭所有的基础控件
function UI_CloseAllBaseWidget()
	return UIMgr:GetInstance():CloseAllBaseWidget();
end
--是不是当前正在显示的基础控件
function UI_IsCurBaseWidgetByName(widgetName)
	return UIMgr:GetInstance():IsCurBaseWidgetByName(widgetName);
end
--是不是存在基础控件
function UI_HasBaseWidget(widgetName)
	return UIMgr:GetInstance():HasBaseWidget(widgetName);
end

function UI_GetUIButton(baseWidget, tag)
	return tolua.cast( baseWidget:getChildByTag(tag), "Button" )
end

function UI_GetUILabelBMFont(baseWidget, tag)
	return tolua.cast( baseWidget:getChildByTag(tag), "LabelBMFont" )
end

function UI_GetUILabel(baseWidget, tag)
	return tolua.cast( baseWidget:getChildByTag(tag), "Label" )
end

function UI_GetUICheckBox(baseWidget, tag)
	return tolua.cast( baseWidget:getChildByTag(tag), "CheckBox" )
end

function UI_GetUIImageView(baseWidget, tag)
	return tolua.cast( baseWidget:getChildByTag(tag), "ImageView" )
end

function UI_GetUILoadingBar(baseWidget, tag)
	return tolua.cast( baseWidget:getChildByTag(tag), "LoadingBar" )
end

function UI_GetUITextField(baseWidget, tag)
	return tolua.cast( baseWidget:getChildByTag(tag), "TextField" )
end

function UI_GetUIScrollView(baseWidget, tag)
	return tolua.cast( baseWidget:getChildByTag(tag), "ScrollView" )
end

function UI_GetUIListView(baseWidget, tag)
	return tolua.cast( baseWidget:getChildByTag(tag), "ListView" )
end

function UI_GetUILayout(baseWidget, tag)
	return tolua.cast( baseWidget:getChildByTag(tag), "Layout" )
end
function UI_GetUIPageView(baseWidget, tag)
	return tolua.cast( baseWidget:getChildByTag(tag), "PageView" )
end

function UI_GetUILabelBMFont(baseWidget, tag)
	return tolua.cast( baseWidget:getChildByTag(tag), "LabelBMFont" )
end

function UI_GetCloneLayout(layout)
	return tolua.cast( layout:clone(), "Layout" )
end

function UI_GetCloneButton(button)
	return tolua.cast( button:clone(), "Button" )
end

function UI_GetCloneImageView(imgView)
	return tolua.cast( imgView:clone(), "ImageView" )
end

function UI_GetCountIcon(count)
	local countNode = CompLabel:GetDefaultLabWithSize( ""..count, 18);
	return countNode;
end

function UI_IconSkillFrame(img, quality)
	local frame = ImageView:create();
	frame:loadTexture("Common/Icon_Bg_base.png")
	
	frame:addChild(img, 0, 1);
	
	local border = ImageView:create();

	border:loadTexture("Common/Icon_Bg_02"..quality..".png")

	img:addChild(border, 10, 3);
	
	return frame;
end

function UI_IconFrame(img, stepLvl, quality)
	local frame = ImageView:create();
	frame:loadTexture("Common/Icon_Bg_base.png")
	
	frame:addChild(img, 0, 1);
	
	Log("stepLvl ="..stepLvl)
	local border = ImageView:create();
	if (stepLvl >= 0)then
		local starLayout = UI_GetStarLayout(stepLvl+1);
		starLayout:setPosition(ccp(0, -40));
		starLayout:setScale(0.8);
		img:addChild(starLayout, 11, 4);
	end
	
	if (quality) then
		local pingzi = GetHeroPingzi(quality);
		border:loadTexture("Common/Icon_Bg_00"..pingzi..".png")

        local JieDuan = GetHeroJieDuan(quality)
        local lab1 = {[1]=0,[2]=14,[3]=-14}
        local lab2 = {[1]=6,[2]=-6}
        for i=1 , JieDuan,1 do
            local imag = ImageView:create()
            imag:loadTexture("Common/Icon_class_0"..(pingzi-1)..".png")
            if JieDuan%2==0 then
                imag:setPosition(
                    ccp(lab2[i%2+1]*ToInt((i+1)/2)
                        , frame:getSize().height/2-7) )
            else
                if i==1 then
                    imag:setPosition(
                        ccp(lab1[1]
                            , frame:getSize().height/2-7))  
                else
                    imag:setPosition(
                          ccp(lab1[(i)%2+2]*ToInt((i)/2)
                                , frame:getSize().height/2-7))
                end
            end
            frame:addChild(imag,100)
        end
	else
		border:loadTexture("Common/Icon_Bg_002.png")
	end
	
	img:addChild(border, 10, 3);
	
	return frame;
end

function UI_ItemIconFrame(img, quality)
	local frame = ImageView:create();
	frame:loadTexture("Common/Icon_Bg_base.png")
	
	frame:addChild(img, 0, 1);
	
	local border = ImageView:create();
	if (quality) then
		border:loadTexture("Common/Icon_Bg_01"..quality..".png")
	end
	
	frame:addChild(border, 10, 3);
	
	return frame;
end

function UI_IconFrameByClone(layout,tag,steplvl)
	local frame = UI_GetUIImageView(layout,tag)
	if (steplvl)then
		local pingzi = GetHeroPingzi(steplvl);
		UI_GetUIImageView(layout,tag):loadTexture("Common/Icon_Bg_00"..pingzi..".png")
	else
		UI_GetUIImageView(layout,tag):loadTexture("Common/Icon_Bg_002.png")
	end 
end

function UI_IconFrameByPingjie(layout,tag,pingjie)
   Log("pingjie====xxxxxxxxxxxxxx==="..pingjie)
   local data = GetGameData(DataFileQualityProp, pingjie, "stQualityPropData")
   local quality = data.m_color
   Log("quality=====xxxxxxxxxxxx==="..quality)
   local frame = UI_GetUIImageView(layout,tag)
	if (quality)then
		UI_GetUIImageView(layout,tag):loadTexture("Common/Icon_Bg_00"..quality..".png")
	else
		UI_GetUIImageView(layout,tag):loadTexture("Common/Icon_Bg_002.png")
	end 
end 


function UI_IconFrameWithSkillOrItem(layout,tag,steplvl,itempingzi)
	-- body
	local frame = UI_GetUIImageView(layout,tag)
	if (steplvl)then
		UI_GetUIImageView(layout,tag):loadTexture("Common/Icon_Bg_02"..steplvl..".png")
	end 
	if itempingzi then
		UI_GetUIImageView(layout,tag):loadTexture("Common/Icon_Bg_01"..itempingzi..".png")
	end
end

function UI_GetHeroImg(id1, id2)
	--CCSpriteFrameCache:sharedSpriteFrameCache():addSpriteFramesWithFile("Icon/HeroImage_"..id1..".plist");

	local img = ImageView:create();
	img:loadTexture("Icon/heroImage/"..id1..".png");

	return img;
end

function UI_GetHeroImgByClone(id1,id2,layout,tag)
    --CCSpriteFrameCache:sharedSpriteFrameCache():addSpriteFramesWithFile("Icon/HeroImage_"..id1..".plist");
	UI_GetUIImageView(layout,tag):loadTexture("Icon/heroImage/"..id1..".png");
end

function UI_GetGodAnimalImgByClone(id1,id2,layout,tag)
	UI_GetHeroImgByClone(id1,id2,layout,tag)
end

function UI_GetHeroIcon(id, count, heroType,HPPercent)
	local img = ImageView:create();
	img:loadTexture("Icon/HeroIcon/"..id..".png");
	
	if (count ~= nil and count > 0) then
		local countBg = ImageView:create();
		countBg:loadTexture("Common/Bg_Common_021.png")
		countBg:setPosition(ccp(img:getSize().width/2 - 15, -img:getSize().height/2 + 15));
		img:addChild(countBg, 1,11);
        countBg:setScale(0.8)
		local countLab = UI_GetCountIcon(count);
		countLab:setPosition(ccp(img:getSize().width/2 - 15, -img:getSize().height/2 + 15));
		img:addChild(countLab, 12);
	end
	
	if (heroType) then
		local typeIcon = ImageView:create();
		if (heroType == 1)then
			typeIcon:loadTexture("Formation/ico_01.png")
		else
			typeIcon:loadTexture("Formation/ico_02.png")
		end
		typeIcon:setPosition(ccp(-img:getSize().width/2 + 15, img:getSize().height/2 - 15));
		img:addChild(typeIcon, 13);
	end

    if HPPercent then
        if HPPercent<0 then
            HPPercent = 0
        elseif HPPercent>100 then
            HPPercent =100
        end
        local image = ImageView:create()
        image:loadTexture("Common/bar_tongyong_02.png")
        img:addChild(image,14)
        local loadingBar = LoadingBar:create()
        loadingBar:loadTexture("Common/bar_tongyong_01.png")
        loadingBar:setPercent(HPPercent)
        img:addChild(loadingBar,15)

        image:setPosition(ccp(0, img:getSize().height/2 - 7))
        loadingBar:setPosition(ccp(0, img:getSize().height/2 - 7))
    end

	return img;
end

function UI_GetHeroTypeIcon(layout,Type)
	-- body
	local typeImage = ImageView:create()
	if (Type == 1)then
			typeImage:loadTexture("Formation/ico_01.png")
	else
		typeImage:loadTexture("Formation/ico_02.png")
	end
	typeImage:setPosition(ccp(layout:getContentSize().width / 2 - 30, layout:getContentSize().height - 17))
	layout:addChild(typeImage,100)
end

function UI_GetHeroTypeIconByTag(layout,tag,Type)
   UI_GetUIImageView(layout, tag):loadTexture("Common/Icon_001_0"..Type..".png")
end

function UI_GetHeroIconByHero(hero)
	local data = hero:GetHeroData()
	local id = data.m_icon;
	local lvl = hero:GetInt(EHero_Lvl)
	local heroType = data.m_heroType;
	
	return UI_GetHeroIcon(id, lvl, heroType);
end

function UI_GetHeroIncoByClone(id,layout,tag)
	-- body
    Log("id===="..id)
    UI_GetUIImageView(layout,tag):loadTexture("Icon/HeroIcon/"..id..".png");
end
function UI_GetHeroSoulIcon(id,layout,tag)
   UI_GetUIImageView(layout,tag):loadTexture("Icon/Item/"..id..".png")
end 

function UI_GetHeroEquipIcon(id,layout,tag)
   UI_GetUIImageView(layout,tag):loadTexture("Icon/Equip/"..id..".png")
   Log("equipId =="..id)
end

function UI_GetSkillIcon(id)
	local img = ImageView:create();
	img:loadTexture("Icon/Skill/"..id..".png");

	return img;
end

function UI_GetItemIcon(id, count, needAdd)
	local img = ImageView:create();
    img:loadTexture("Icon/Item/"..id..".png");
    print("path=========".."Icon/Item/"..id..".png")
	if (count ~= nil and count > 0) then
		if (needAdd)then
			local countNode = CompLabel:GetDefaultLabWithSize( "+"..count, 18);
			countNode:setPosition(ccp(img:getSize().width/2 - 33, -img:getSize().height/2 + 30));
			countNode:setColor(ccc3(0, 255, 0))
			img:addChild(countNode,0,1);
		else
			--local countLab = UI_GetCountIcon(count);
            local countLab = LabelBMFont:create()
            countLab:setFntFile("zhandou/nuqi.fnt")
            countLab:setText(count)
            countLab:setAnchorPoint(ccp(1,0))
			countLab:setPosition(ccp(img:getSize().width/2 - 10, -img:getSize().height/2-2));
			img:addChild(countLab,1,1);
		end
	end
	return img;
end

function UI_GetLifeAtt(id, count)
	local img = ImageView:create();
    if id==1 then
        img:loadTexture("LifeAttIcon/1_1.png");
    elseif id==2 then
        img:loadTexture("LifeAttIcon/2_2.png");
    elseif id==4 then
        img:loadTexture("LifeAttIcon/4-4.png");
    elseif id==17 then
        img:loadTexture("LifeAttIcon/17-17.png");
    elseif id==20 then
        img:loadTexture("LifeAttIcon/20-20.png");
    end
	if (count ~= nil and count > 0) then
	    --local countLab = UI_GetCountIcon(count);
        local countLab = LabelBMFont:create()
        countLab:setFntFile("zhandou/nuqi.fnt")
        countLab:setText(count)
        countLab:setAnchorPoint(ccp(1,0))
	    countLab:setPosition(ccp(img:getSize().width/2 - 10, -img:getSize().height/2-2));
	    img:addChild(countLab,1,1);
	end
	return img;
end
function UI_GetEquipIcon(id)
	local img = ImageView:create();
	img:loadTexture("Icon/Equip/"..id..".png");
	return img;
end
function UI_GetArrow()
	-- body
	local arrowimg = ImageView:create()
	arrowimg:loadTexture("Common/Arrow_01.png")
	-- local action = CCBlink:create(99999, 99999)
	UI_GetFadeInAndOutAction(arrowimg)
    arrowimg:setScale(0.7)
    return arrowimg
end

function UI_GetAddImage()
   local addimage = ImageView:create()
   addimage:loadTexture("HeroSystem/Bg_005.png")
   UI_GetFadeInAndOutAction(addimage)
   return addimage
end 

function UI_GetFadeInAndOutAction(tempwidget)
	-- body
	local arryaction = CCArray:create()
	local fadeinaction = CCFadeIn:create(1)
	local fadeoutaction = CCFadeOut:create(1)
	arryaction:addObject(fadeinaction)
	arryaction:addObject(fadeoutaction)
	local seqaction = CCSequence:create(arryaction)
	local reaction = CCRepeatForever:create(seqaction)
    tempwidget:runAction(reaction)
end

function UI_GetHeroLevelStepIcon(widget,tag,pingzhi)
    local data = GetGameData(DataFileQualityProp, pingzhi, "stQualityPropData")
    local iconstr = ""
    if data.m_color == 2 then
		iconstr = "Common/Fort_Rank_01.png"
	elseif data.m_color == 3 then
		iconstr = "Common/Fort_Rank_02.png"
	elseif data.m_color == 4 then
		iconstr = "Common/Fort_Rank_03.png"
	elseif data.m_color == 5 then
		iconstr = "Common/Fort_Rank_04.png"
    elseif data.m_color == 6 then
		iconstr = "Common/Fort_Rank_05.png"
	end
    UI_GetUIImageView(widget, tag):loadTexture(iconstr)
end

function UI_GetHeroLevelStepLab(widget,tag,pingzhi)
   local data = GetGameData(DataFileQualityProp, pingzhi, "stQualityPropData")
   local str = ""
   Log("pingzhi==="..pingzhi)
   if data.m_color == 2 then 
      str = FormatString("StarClassMortal")
   elseif data.m_color == 3 then 
      str = FormatString("StarClassSpirit")
   elseif data.m_color == 4 then 
      str = FormatString("StarClassImmortal")
   elseif data.m_color == 5 then
      str = FormatString("StarClassGod")
   elseif data.m_color == 6 then 
      str = FormatString("StarClassSaint")
   end 
   UI_SetLabelText(widget,tag,FormatString("HeroInfoSecond_SkillLevelStep",str))
end 

function UI_GetHeroTypeName(herotype)
   local str = ""
   Log("heroType==="..herotype)
   if herotype == 1 then 
      str = FormatString("HeroType_shenjiang")
   elseif herotype == 2 then
      str = FormatString("HeroType_moushi")
   elseif herotype == 3 then
      str = FormatString("HeroType_cike")
   elseif herotype == 4 then
      str = FormatString("HeroType_haoxia")
   elseif herotype == 5 then
      str = FormatString("HeroType_huangzhe")
   end 
   return str 
end 

function UI_GetBMFontFightNumber(layout,tag,value)
	-- body
	Log("fightvalue=="..value)
	if value >= 1000000 then

		local value = math.floor(value / 10000)
		Log("fightvalue=="..value)
		UI_GetUILabelBMFont(layout, tag):setText(value.."万")
	else
		UI_GetUILabelBMFont(layout, tag):setText(""..value)
	end
end

function UI_MoveForeverAndBack(tempwidget)
	-- body
	local width = tempwidget:getContentSize().width 
	local Posx,PosY = tempwidget:getPosition()
	local move = CCMoveBy:create(0.5,ccp(20,0)) 
	local arryaction = CCArray:create()
	local moveReverse = CCMoveBy:create(0.5,ccp(-20,0)) 
	arryaction:addObject(move)
	arryaction:addObject(moveReverse)
	local seqaction = CCSequence:create(arryaction)
	local reaction = CCRepeatForever:create(seqaction)
	tempwidget:runAction(reaction)
end

function UI_GetGodAnimalIcon(id)
	return UI_GetHeroIcon(id);
end

function UI_GetGodAnimalImg(id1,id2)
	return UI_GetHeroImg(id1, id2);
end

function UI_GetStarLayout(count)
	return UIMgr:GetInstance():GetStarLayout(count);
end

function UI_SetLabelText(baseWidget, tag, str)
	local text = tolua.cast( baseWidget:getChildByTag(tag), "Label" )
	if (text)then
		text:setText(str);
	end 
end

function ShowWaiting(second, isVisible)
	if (second == nil)then
		second = 15;
	end
	
	MainScene:GetInstance():removeChildByTag(EMSTag_Waiting);
	
	local widget = Widget:create();
	if (isVisible ~= false)then
		local armature = GetUIArmature("Effjuhua");
		armature:getAnimation():playWithIndex(0)
		widget:addNode(armature)
	end
	widget:setPosition(ccp(480, 320));
	widget:setTouchEnabled(true);
	
	MainScene:GetInstance():AddChild(widget, EMSTag_Waiting, EMSTag_Waiting, true);
	
	
	widget:ignoreContentAdaptWithSize(false);
	widget:setSize(CCSizeMake(960, 640) );
	GetGlobalEntity():GetScheduler():RegisterScript( "EndWaiting", second*1000, 1 )
end

function EndWaiting()
	GetGlobalEntity():GetScheduler():UnRegisterScript( "EndWaiting")
	MainScene:GetInstance():removeChildByTag(EMSTag_Waiting);
end

function UI_ClickCloseCurBaseWidget(sender, eventType)
	if (eventType == TOUCH_EVENT_ENDED) then
		UI_CloseCurBaseWidget()
	end
end

function UI_CloseAllSpecial()
	UIMgr:GetInstance():CloseAllSpecial();
end

function UI_ClickCloseAllSpecial(sender, eventType)
	if (eventType == TOUCH_EVENT_ENDED) then
		UI_CloseAllSpecial()
	end
end

function UI_ClickCloseCurBaseWidgetWithNoAction(sender, eventType)
	-- body
	if (eventType == TOUCH_EVENT_ENDED) then
		UI_CloseCurBaseWidget(EUICloseAction_None)
	end
end

function UI_ClickCloseAllBaseWidget(sender, eventType)
	if (eventType == TOUCH_EVENT_ENDED) then
		UI_CloseAllBaseWidget();
	end
end

function UI_AddSimpleChat(widget, pLayer)	
	--g_simpleChatUi:setVisible(true);
	--g_simpleChatUi:removeFromParent();
	--widget:addChild(g_simpleChatUi, 9999999);
	
	--local listView = UI_GetUIListView(g_simpleChatUi:getChildByTag(1), 1);
	--listView:jumpToBottom();
end

function UI_HiddenSimpleChat()
	--g_simpleChatUi:setVisible(false);
end

function UI_ShowError(type, str)
	if type == GameServer_pb.en_ErrorCodeType_Normal then
	   Messagebox_Create({info = FormatString(str), msgType = EMessageType_Middle, deltaTime=3000000})
	elseif  GameServer_pb.en_ErrorCodeType_Float then
       createPromptBoxlayout(str)
    end
end

function On_ScError(pkg)
	EndWaiting();

	local tmp = GameServer_pb.Cmd_Sc_Error();
	tmp:ParseFromString(pkg)
	Log("erroType ====="..tostring(tmp))
	UI_ShowError(tmp.iType, tmp.strErrorMsg);
end

function UI_GetMoneyStr(number)
	local str = ""..number;
	if (number >= 10000000)then
		number = ToInt(number/10000);
		str = FormatString("TenThousand", number);
	end
	return str;
end

--[[function UI_AddCircleButtonEffect(button)
	local act = CCRepeatForever:create(CCRotateBy:create(0.5, 360));
	local img = ImageView:create();
	img:setPosition(ccp(0, 3))
	img:loadTexture("tongyong/light.png");
	img:runAction(act);
	button:removeChildByTag(999999)
	button:addChild(img, 999999, 999999)
end]]

function GetParticleEffect(layout,plist)
	-- body
    local particle = CCParticleSystemQuad:create(plist)
    particle:retain()
    local bacthnode = CCParticleBatchNode:createWithTexture(particle:getTexture())
    bacthnode:addChild(particle)
    layout:addNode(bacthnode)
    particle:release()
end

ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_ERROR, "On_ScError" );

-- --英雄list item 模板
-- if g_HeroListItemTemplate == nil then 
	-- g_HeroListItemTemplate = {}
	
	-- local widget = UIMgr:GetInstance():getCloneHeroListItemTemplate()
	-- for i=1, 33 do
		-- g_HeroListItemTemplate[i] = UI_GetCloneLayout(widget);
		-- g_HeroListItemTemplate[i]:setPosition(ccp(0, 0))
		-- g_HeroListItemTemplate[i]:retain()
	-- end
-- end
-- function UI_GetHeroListItemTemplate(heroEntity, i, isNineSky)
	-- g_HeroListItemTemplate[i]:removeFromParent();
	
	-- local layout = g_HeroListItemTemplate[i];
    -- --设置模板
    -- local data = GetGameData(DataFileHeroBorn,heroEntity:GetUint(EHero_HeroId),"stHeroBornData")
    -- --image 
    -- local imagelayout = UI_GetUILayout(layout, 1)
    -- UI_GetHeroIncoByClone(data.m_icon,imagelayout,1)
        
    -- --lvl
    -- UI_SetLabelText(layout,4,FormatString("HeroConver_lvl", heroEntity:GetUint(EHero_Lvl)))
    -- --英雄类型
    -- UI_GetHeroTypeIconByTag(imagelayout,3,data.m_heroType)

    -- local heroLvLStep = heroEntity:GetUint(EHero_LvlStep)

    -- --英雄等阶图标
    -- UI_GetHeroLevelStepIcon(layout,3,heroLvLStep)

    -- --外框
    -- UI_IconFrameByClone(imagelayout,2,heroLvLStep)

    -- --英雄名字
    -- local namelayout = UI_GetUILayout(layout, 2)
    -- local heroNameLab = CompLabel:GetCompLabel(GetHeroName(data.m_name,heroLvLStep),192,kCCTextAlignmentLeft)
    -- local nameWidth = namelayout:getContentSize().width 
    -- local nameHeight = namelayout:getContentSize().height 
    -- heroNameLab:setPosition(ccp(nameWidth / 2,nameHeight / 2))
    -- heroNameLab:setAnchorPoint(ccp(0.5,0.5))
    -- namelayout:removeAllChildren()
    -- namelayout:addChild(heroNameLab)
	
	-- UI_GetUIImageView(layout, 5):setVisible(false)
    -- --是否出战
	-- if (isNineSky)then
		-- if (NineSky_IsInFormation(heroEntity))then
			-- UI_GetUIImageView(layout, 5):setVisible(true)
		-- end
    -- elseif i <= 5 and GetLocalPlayer():IsInFormation(heroEntity) then
       -- UI_GetUIImageView(layout, 5):setVisible(true)
    -- end
	
    -- if layout:getNodeByTag(66) ~= nil then 
       -- layout:removeAllNodes()
    -- end 
	-- return layout;
-- end

if (g_mainUiTopBG == nil)then
	g_mainUiTopBG = ImageView:create();
	g_mainUiTopBG:loadTexture("Bg/pagebg.png")
	g_mainUiTopBG:setPosition(ccp(480, 320))
	local scale = MainScene:GetInstance().m_maxScale/MainScene:GetInstance().m_minScale;
	g_mainUiTopBG:setScale(scale);
	g_mainUiTopBG:retain();
end


function UI_AddMainUiTopToWidget(widget)
	g_mainUiTopWidget:removeFromParent();
	local uiLayer = tolua.cast(widget:getParent():getParent(), "TouchGroup")
	uiLayer:addWidget(g_mainUiTopWidget);
	--弱引导
	NoticeFunction_UpdateTopMainUiButton()
	if (widget:getName() ~= "MainUi")then
		UI_GetUIButton(g_mainUiTopWidget, 10):setVisible(true)
		g_mainUiTopBG:removeFromParent();
		g_mainUiTopBG:setOpacity(255)
		widget:addChild(g_mainUiTopBG, -999)
		g_mainUiTopWidget:setPosition(ccp(0, widget:getPositionY()))
	else
		g_mainUiTopWidget:setPosition(ccp(0, 0))
	end
    --九天幻境不显示右边的动作框
    if UI_GetBaseWidgetByName("NineSky") or UI_GetBaseWidgetByName("NineSkyMap.json") then
        UI_GetUIButton(g_mainUiTopWidget, 10):setVisible(false)
    end
end

function UI_AddMainUiTop(widget)
	UI_GetUIButton(g_mainUiTopWidget, 10):setVisible(false);
	
	local name = widget:getName();
	for i=1, #g_needMainUiTopWidget do
		if (name == g_needMainUiTopWidget[i])then
			UI_AddMainUiTopToWidget(widget)
			return;
		end
	end
    --九天幻境不显示右边的动作框
    if UI_GetBaseWidgetByName("NineSky") or UI_GetBaseWidgetByName("NineSkyMap.json") then
        UI_GetUIButton(g_mainUiTopWidget, 10):setVisible(false)
    end
end

function UI_RefreshMainUiTop()
	Log("UI_RefreshMainUiTop")
	--g_mainUiTopWidget:removeFromParent();
	UI_GetUIButton(g_mainUiTopWidget, 10):setVisible(false);
	
	local widgetList = UIMgr:GetInstance():GetWidgetList();
	for i=widgetList:size()-1, 0, -1 do
		local name = widgetList[i]:getName()
		for j=1, #g_needMainUiTopWidget do
			if (widgetList[i]:getIsNeedRemove() == false and name == g_needMainUiTopWidget[j])then
				UI_AddMainUiTopToWidget(widgetList[i])
				return;
			end
		end
	end
    --九天幻境不显示右边的动作框
    if UI_GetBaseWidgetByName("NineSky") or UI_GetBaseWidgetByName("NineSkyMap.json") then
        UI_GetUIButton(g_mainUiTopWidget, 10):setVisible(false)
    end
end