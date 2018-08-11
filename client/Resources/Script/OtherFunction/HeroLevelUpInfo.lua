local l_playerlevelupInfo = nil 
--开关 作为判断当前场景
local l_playerlevelState = false 
local widget = nil 
function HeroLevelUpInfoShow()
	Log("HeroLevelUpInfoShow")
	-- body
	CreateGameEffectMusic(g_effectMusicFileTable["level"])
	widget = GUIReader:shareReader():widgetFromJsonFile("PlayerLevelup.json")
    UI_GetUILayout(widget, 4457205):addTouchEventListener(PublicCallBackWithNothingToDo)
	InitPlayerLevelUpUI(widget)
	local imageLayout = UI_GetUIImageView(widget,1)
	imageLayout:setScale(0.01)
	local action = CCEaseBackOut:create(CCScaleTo:create(0.3,1));
	local delayTiem = CCDelayTime:create(0.4)
	local actionArryOne = CCArray:create()
	actionArryOne:addObject(delayTiem)
	actionArryOne:addObject(action)
	local seqactionOne = CCSequence:create(actionArryOne)
    imageLayout:runAction(seqactionOne)

    local delay = CCDelayTime:create(0.6)
    UI_GetUIImageView(widget,2):setScale(0.01)
    local action1 = CCEaseBackOut:create(CCScaleTo:create(0.3,1))
    local func = CCCallFunc:create(afterPlayerLevelUpAction)
    local actionArry = CCArray:create()
    actionArry:addObject(delay)
    actionArry:addObject(action1)
    actionArry:addObject(func)
    local seqaction = CCSequence:create(actionArry)
    UI_GetUIImageView(widget,2):runAction(seqaction)


	--return bt
	local function closePlayerlevelupdate(sender,eventType)
		-- body
		if eventType == TOUCH_EVENT_ENDED then
			l_playerlevelupInfo = nil 
			MainScene:GetInstance():removeChildByTag(EMSTag_LevelUp,true)
			if g_lastGradeOpenFlag ~= GetLocalPlayer():GetInt(EPlayer_FunctionMask) then
				GradeOpenShowInfo()
			end
		end
	end
	UI_GetUIButton(imageLayout,1):addTouchEventListener(closePlayerlevelupdate)
  
    --条件
    if l_playerlevelupInfo ~= nil then 
    	levelUpShowInfo(imageLayout)
    end   
    MainScene:GetInstance():AddChild(widget, EMSTag_LevelUp, EMSTag_LevelUp , true);
end

function InitPlayerLevelUpUI(widget)
	-- body
	local imagelayout = UI_GetUIImageView(widget,1)
	--UI_GetUIButton(imagelayout,1):setOpacity(100)
	UI_GetUIButton(imagelayout,1):setTouchEnabled(false)
	for i = 8 , 10 do
		UI_GetUIImageView(imagelayout,i):setOpacity(0)
	end
	for i = 2 , 7 do 
	    UI_GetUILabel(imagelayout,i):setOpacity(0)
    end
end

local function ButtonSetEnabel()
		-- body
	local imagelayout = UI_GetUIImageView(widget,1)
	UI_GetUIButton(imagelayout,1):setTouchEnabled(true)	
end

function ReturnDelayAndFadeInAction(offsetTime,isCallFunc)
	-- body
    local delayTime = CCDelayTime:create(offsetTime)
    local actionArry = CCArray:create()
    actionArry:addObject(delayTime) 
    local actionFadeIn = CCFadeIn:create(0.1)
    actionArry:addObject(actionFadeIn)
    if isCallFunc == true then
        local callfunc = CCCallFunc:create(ButtonSetEnabel)
        actionArry:addObject(callfunc)
    end
    local seqaction = CCSequence:create(actionArry)
    return seqaction
end

function afterPlayerLevelUpAction()
   local imagelayout = UI_GetUIImageView(widget,1)
   local offsetTime = 0 
   local action1 = ReturnDelayAndFadeInAction(0,false)
   UI_GetUILabel(imagelayout,5):runAction(action1)
   offsetTime = offsetTime + 0.1
   local action2 = ReturnDelayAndFadeInAction(offsetTime,false)
   UI_GetUIImageView(imagelayout,8):runAction(action2)

   offsetTime = offsetTime + 0.1 
   local action3 = ReturnDelayAndFadeInAction(offsetTime,false)
   UI_GetUILabel(imagelayout,2):runAction(action3)

   offsetTime = offsetTime + 0.1 
   local action4 = ReturnDelayAndFadeInAction(offsetTime,false)
   UI_GetUILabel(imagelayout,6):runAction(action4)
   offsetTime = offsetTime + 0.1
   local action5 = ReturnDelayAndFadeInAction(offsetTime,false)
   UI_GetUIImageView(imagelayout,9):runAction(action5)

   offsetTime = offsetTime + 0.1 
   local action6 = ReturnDelayAndFadeInAction(offsetTime,false)
   UI_GetUILabel(imagelayout,3):runAction(action6)

   offsetTime = offsetTime + 0.1 
   local action7 = ReturnDelayAndFadeInAction(offsetTime,false)
   UI_GetUILabel(imagelayout,7):runAction(action7)
   offsetTime = offsetTime + 0.1
   local action8 = ReturnDelayAndFadeInAction(offsetTime,false)
   UI_GetUIImageView(imagelayout,10):runAction(action8)

   offsetTime = offsetTime + 0.1 
   local action9 = ReturnDelayAndFadeInAction(offsetTime,false)
   UI_GetUILabel(imagelayout,4):runAction(action9)

   offsetTime = offsetTime + 0.1
   local action10 = ReturnDelayAndFadeInAction(offsetTime,true)
   UI_GetUIButton(imagelayout,1):runAction(action10)
end

function InitPlayerlevelupInfo(playerlevelupinfo)
	-- body
	if l_playerlevelupInfo ~= nil then
		l_playerlevelupInfo = nil
	end
    l_playerlevelupInfo = playerlevelupinfo
end

function levelUpShowInfo(layoutParent)
	-- body
	UI_SetLabelText(layoutParent,5,FormatString("HeroLevelUp_lvl",l_playerlevelupInfo.iPreLevel))
	UI_SetLabelText(layoutParent,6,FormatString("HeroLevelUp_Tili",l_playerlevelupInfo.iPrePhyStrength))
	UI_SetLabelText(layoutParent,7,FormatString("HeroLevelUp_herolimit",l_playerlevelupInfo.iPreFormationHeroLimit))
    UI_SetLabelText(layoutParent,2,l_playerlevelupInfo.iLevel)
    UI_SetLabelText(layoutParent,3,l_playerlevelupInfo.iCurPhyStrength)
    UI_SetLabelText(layoutParent,4,l_playerlevelupInfo.iCurFormationHeroLimit)
    for i = 2,4 do 
       UI_GetUILabel(layoutParent,i):enableStroke()
    end 
end

function setpalyerlevelstate(state)
	-- body
	l_playerlevelState = state
end

function returnPlayerlevelupdateinfo()
	-- body
	return l_playerlevelupInfo
end
