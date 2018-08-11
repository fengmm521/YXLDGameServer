--记录之前的值
if g_lastGradeOpenFlag == nil then
	g_lastGradeOpenFlag = 0
end

--装备的判断
function HeroEquipIsOpen()
--   local GradeOpenFlag = Math:XORFlag(GetLocalPlayer():GetInt(EPlayer_FunctionMask), g_lastGradeOpenFlag)   
--   local curOpenFlag = Math:GetXORFlag(GradeOpenFlag)\
   local flag = GetLocalPlayer():GetInt(EPlayer_FunctionMask)
   local curOpenFlag = Math:GetAndFlagByPosition(flag,GameServer_pb.en_Function_EquipDungeon)
   Log("curOpenFlag====="..curOpenFlag)
   if curOpenFlag == 1 then
      return true  
   else 
      return false
   end 
end

function GradeOpenShowInfo()
	-- body
	Log("hero falge==="..GetLocalPlayer():GetInt(EPlayer_FunctionMask))
	Log("lastfla============"..g_lastGradeOpenFlag)

	local GradeOpenFlag = Math:XORFlag(GetLocalPlayer():GetInt(EPlayer_FunctionMask), g_lastGradeOpenFlag)   
    local curOpenFlag = Math:GetXORFlag(GradeOpenFlag)
	Log("a=========="..GradeOpenFlag)
	Log("a=========="..curOpenFlag)

    local widget = GUIReader:shareReader():widgetFromJsonFile("GradeOpen.json")
    local layoutparent = UI_GetUIImageView(widget,1)
    layoutparent:setScale(0.01)
    local action = CCEaseBackOut:create(CCScaleTo:create(0.3,1));
    layoutparent:runAction(action)

    local delay = CCDelayTime:create(0.1)
    UI_GetUIImageView(widget,2):setScale(0.01)
    local action1 = CCEaseBackOut:create(CCScaleTo:create(0.3,1))
    --local func = CCCallFunc:create(afterPlayerLevelUpAction)
    local actionArry = CCArray:create()
    actionArry:addObject(delay)
    actionArry:addObject(action1)
    --actionArry:addObject(func)
    local seqaction = CCSequence:create(actionArry)
    UI_GetUIImageView(widget,2):runAction(seqaction)

    local GradeOpenData = GetGameData(DataFileOpenFunction, curOpenFlag - 1, "stOpenFunctionData")
    --local GradeOpenData = GetGameData(DataFileOpenFunction, 0, "stOpenFunctionData")

    local function ReturnMainUi(sender,eventType)
    	-- body
    	if eventType == TOUCH_EVENT_ENDED then
    		--UI_CloseAllBaseWidget()
            MainScene:GetInstance():removeChildByTag(EMSTag_GradeOpen,true)
            if GradeOpenData.m_yindao == 1 then
        		g_openUIType = EUIOpenType_None 
        		SceneMgr:GetInstance():SwitchToMainUi(false)
            end
    	end
    end
    if GradeOpenData.m_yindao == 1 then
        UI_GetUIButton(layoutparent,3):setTitleText(FormatString("OpenGrade_BeGo"))
        UI_GetUIButton(layoutparent,3):setTitleFontSize(24)
    else
        UI_GetUIButton(layoutparent,3):setTitleText(FormatString("OpenGrade_BeSure"))
        UI_GetUIButton(layoutparent,3):setTitleFontSize(24)
    end
    UI_GetUIButton(layoutparent,3):addTouchEventListener(ReturnMainUi)
    --icon
    UI_GetUIImageView(layoutparent,1):loadTexture("zhujiemian/"..GradeOpenData.m_icon,UI_TEX_TYPE_LOCAL)

    --描述
    local scrollview = UI_GetUIScrollView(layoutparent,2)
    scrollviewwidth = scrollview:getContentSize().width   
    local desclab = CompLabel:GetDefaultCompLabel(FormatString("OpenGrade_info",GradeOpenData.m_info),scrollviewwidth)
    desclab:setAnchorPoint(ccp(0,0))

    scrollview:removeAllChildren()
    local height = desclab:getContentSize().height 
    if height < scrollview:getSize().height then
    	height = scrollview:getSize().height 
    end

    scrollview:setInnerContainerSize(CCSizeMake(scrollviewwidth,height))
    desclab:setPosition(ccp(0,height - desclab:getContentSize().height))
	scrollview:addChild(desclab)

    MainScene:GetInstance():AddChild(widget, EMSTag_GradeOpen, EMSTag_GradeOpen , true)
end