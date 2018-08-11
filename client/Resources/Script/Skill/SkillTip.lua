function SkillTipView(SkillId)
	-- body
    local widget = UI_CreateBaseWidgetByFileName("SkillTip.json")
    local closeBt = UI_GetUIButton(widget,1)
    closeBt:addTouchEventListener(UI_ClickCloseCurBaseWidget)

    --UI_GetUILayout(widget,6):addTouchEventListener(UI_ClickCloseCurBaseWidget)

    -- Log("SkillId.======"..SkillId)
    --Skill Icon
    local SkillData = GetGameData(DataFileSkill,SkillId,"stSkillData")
    UI_GetUIImageView(widget,2):loadTexture("Icon/Skill/"..SkillData.m_icon..".png")

    --Skill name 
    UI_SetLabelText(widget,3,SkillData.m_name)

    --Skill Type
    UI_SetLabelText(widget,4,FormatString("HeroSkillType",getHeroSkillTypeName(SkillData.m_type)))

    --frame 
    UI_IconFrameWithSkillOrItem(widget,7,SkillData.m_quality)
    --Skill desc
    UI_SetLabelText(widget, 5, SkillData.m_desc)   
end
function SkillTipForGodAnimal(skillId)
    	-- body
    local widget = UI_CreateBaseWidgetByFileName("SkillTip.json")

    UI_GetUILayout(widget,1):setTouchEnabled(false)
    UI_GetUILayout(widget,1):setVisible(false)

    --BG
    UI_GetUIImageView(widget, 4508833):setVisible(true)
    -- Log("SkillId.======"..SkillId)
    --Skill Icon
    local SkillData = GetGameData(DataFileSkill,skillId,"stSkillData")
    local skill = UI_GetUIImageView(widget,7)
    skill:addChild(UI_IconSkillFrame(UI_GetSkillIcon(SkillData.m_icon), SkillData.m_quality))
    --Skill name 
    UI_SetLabelText(widget,3,SkillData.m_name)
    --Skill Type
    UI_SetLabelText(widget,4,FormatString("HeroSkillType",getHeroSkillTypeName(SkillData.m_type)))
    --Skill desc
    local skillscroll = UI_GetUILabel(widget,5)
    skillscroll:setText(SkillData.m_desc)

end
--技能说明
function SkillTipForActor(skillId)
    local widget = UI_CreateBaseWidgetByFileName("SkillTip.json")
    UI_GetUILayout(widget,1):setTouchEnabled(false)
    UI_GetUILayout(widget,1):setVisible(false)

    --BG
    UI_GetUIImageView(widget, 4508833):setVisible(true)
    -- Log("SkillId.======"..SkillId)
    --Skill Icon
    local SkillData = GetGameData(DataFileActorSkill, skillId, "stActorSkillData")
    local skill = UI_GetUIImageView(widget,2555)
    skill:addChild(UI_ItemIconFrame(UI_GetSkillIcon(SkillData.m_icon), 7))
    --Skill name 
    UI_SetLabelText(widget,3,SkillData.m_name)
    --cost
    local layout = UI_GetUILayout(widget, 4695764)
    layout:setVisible(true)
    UI_SetLabelText(layout,4695773,SkillData.m_cost)
    --Skill desc
    local skillscroll = UI_GetUILabel(widget,5)
    skillscroll:setText( SkillData.m_desc)
end
function getHeroSkillTypeName(type)
	-- body
	if type == 0 then
		return FormatString("HeroSkillIsNormalSkill")
	elseif type == 1 then
		return FormatString("HeroSkillIsStunkSkill")
	elseif type == 2 then
		return FormatString("HeroSkillIsPassiveSkill")
    elseif type == 3 then
        return FormatString("HeroSkillIsFirst")
    end
end