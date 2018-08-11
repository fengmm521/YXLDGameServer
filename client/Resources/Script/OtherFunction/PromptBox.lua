--上浮提示框的开关 默认为true

if g_prompboxFloatSwitch == nil then
	g_prompboxFloatSwitch = true
end

local l_PromptBoxLabTagtab = {} 

local l_PromptBoxindex = 0 

local l_PromptBoxLabTag = 0 

g_PromptBoxLabLayout = nil 

function PromptBox_FadeOutAction()
	-- body
	if g_PromptBoxLabLayout ~= nil then
		local offsetTime = 1
		for i = 1,#l_PromptBoxLabTagtab do
			local actionArry = CCArray:create()
			local delayTime = CCDelayTime:create(offsetTime * (i - 1))
			actionArry:addObject(delayTime)
			local fadeoutAction = CCFadeOut:create(0.25)
			actionArry:addObject(fadeoutAction)
			if i == #l_PromptBoxLabTagtab then
				local promptbox_faction = CCCallFunc:create(PrompBox_RemoveAll)
				actionArry:addObject(promptbox_faction)
			end
	        local seqAction = CCSequence:create(actionArry)
	        g_PromptBoxLabLayout:getChildByTag(l_PromptBoxLabTagtab[i]):runAction(seqAction)
		end
    end

    l_PromptBoxindex = 0
	l_PromptBoxLabTag = 0
	l_PromptBoxLabTagtab = {}
	g_PromptBoxLabLayout = nil 
end

function PromptBox_FloatAction(promtBoxString, pos,isOther)
	-- body

    l_PromptBoxindex = l_PromptBoxindex + 1
    l_PromptBoxLabTag = l_PromptBoxLabTag + 1
    local promtboxstringlab = nil 
    if isOther then 
      promtboxstringlab = createPromptBoxFrameBg(promtBoxString, pos)--Label:create()
    else 
      promtboxstringlab = createPromptBoxFrameBgOther(promtBoxString, pos)
    end 
    -- GetGlobalEntity():GetScheduler():UnRegisterScript("MarqueeMove")
    -- promtboxstringlab:setText(promtBoxString)
    -- promtboxstringlab:setFontSize(24)
    if l_PromptBoxindex >= 4 then
       table.remove(l_PromptBoxLabTagtab,1)
       if l_PromptBoxindex % 3 == 0 then
       	  g_PromptBoxLabLayout:removeChildByTag(3,true)
       else
       	  local tempIndex = l_PromptBoxindex % 3
       	  g_PromptBoxLabLayout:removeChildByTag(tempIndex,true)
       	  l_PromptBoxLabTag = tempIndex
       end    
    end
    table.insert(l_PromptBoxLabTagtab,l_PromptBoxLabTag)
	if (pos == nil)then
		promtboxstringlab:setPosition(ccp(0,-l_PromptBoxindex * 45))
	else
		promtboxstringlab:setPosition(ccp(pos.x,pos.y-l_PromptBoxindex * 45))
	end
    
    Log("xxxxxxxxxxxxx....========"..l_PromptBoxLabTag)
    g_PromptBoxLabLayout:addChild(promtboxstringlab,l_PromptBoxLabTag,l_PromptBoxLabTag)
    local promptboxX,promptboxY = g_PromptBoxLabLayout:getPosition()
    local FloatAction = CCMoveBy:create(0.5, ccp(0,45))
    g_PromptBoxLabLayout:runAction(FloatAction)
    GetGlobalEntity():GetScheduler():RegisterInfiniteScript( "PromptBox_FadeOutAction", 1.5 * 1000)
end

function createPromptBoxFrameBgOther(promtBoxString,pos)
   GetGlobalEntity():GetScheduler():UnRegisterScript("PromptBox_FadeOutAction")
	--promtboxstringlab:setText(promtBoxString)
    --promtboxstringlab:setFontSize(24)

	local promtboxstringlab = CompLabel:GetDefaultCompLabel(promtBoxString, 400);
	promtboxstringlab:setAnchorPoint(ccp(0.5,0.5));
    promtboxstringlab:setColor(ccc3(0,255,0))
    return promtboxstringlab 
end 

function createPromptBoxFrameBg(promtBoxString, pos)
	-- body
	--local promtboxstringlab = Label:create()
    GetGlobalEntity():GetScheduler():UnRegisterScript("PromptBox_FadeOutAction")
	--promtboxstringlab:setText(promtBoxString)
    --promtboxstringlab:setFontSize(24)

	local promtboxstringlab = CompLabel:GetDefaultCompLabel(promtBoxString, 400);
	promtboxstringlab:setAnchorPoint(ccp(0.5,0.5));
	
    local bg = ImageView:create()
    bg:loadTexture("Common/Bg_Common_005.png",UI_TEX_TYPE_LOCAL)
	
    if promtboxstringlab:getContentSize().width > bg:getContentSize().width then
	    bg:setScale9Enabled(true)
	    bg:setSize(CCSizeMake(promtboxstringlab:getContentSize().width + 20,promtboxstringlab:getContentSize().height + 20))
    end
    bg:addChild(promtboxstringlab)
    return bg 
end

function createPromptBoxlayout(promtBoxString, pos)
	-- body
	if g_PromptBoxLabLayout == nil then
	   g_PromptBoxLabLayout = Layout:create()
	   g_PromptBoxLabLayout:setPosition(ccp(480, 320))
       MainScene:GetInstance():AddChild(g_PromptBoxLabLayout, EMSTag_PromptBox, EMSTag_PromptBox, false) 
	end
	
	if (pos == nil)then
		pos = ccp(0, 0)
	else
		pos = ccp(pos.x - 480, pos.y - 320)
	end
	
	PromptBox_FloatAction(promtBoxString, pos,true)
end

function createPromptBoxByHeroFavorite(promtBoxString,pos)
   if g_PromptBoxLabLayout == nil then
	   g_PromptBoxLabLayout = Layout:create()
	   g_PromptBoxLabLayout:setPosition(ccp(480, 320))
       MainScene:GetInstance():AddChild(g_PromptBoxLabLayout, EMSTag_PromptBox, EMSTag_PromptBox, false) 
	end

    if (pos == nil)then
		pos = ccp(0, 0)
	else
		pos = ccp(pos.x - 480, pos.y - 320)
	end 
    PromptBox_FloatAction(promtBoxString, pos,false)
end 

--item单独处理
function createPromptBoxlayoutbyItem(value)
	-- body
	if g_prompboxFloatSwitch == true then
		if g_PromptBoxLabLayout == nil then
		   g_PromptBoxLabLayout = Layout:create()
		   g_PromptBoxLabLayout:setPosition(ccp(960 / 2,640 / 2 + 50))
	       MainScene:GetInstance():AddChild(g_PromptBoxLabLayout, EMSTag_PromptBox, EMSTag_PromptBox,false) 
		end
		Log("value==="..value[0]:Int())
		Log("value==="..value[1]:Int())

		local Itemdata = GetGameData(DataFileItem,value[0]:Int(),"stItemData")
	    local str = FormatString("PromptBox_Item",Itemdata.m_name,value[1]:Int())
	    PromptBox_FloatAction(str,nil,true)
    end
end

function PrompBox_RemoveAll()
	-- body
    --Log("wolegequkf odasfjkaf lkf af")
	--g_PromptBoxLabLayout:removeAllChildren()
	MainScene:GetInstance():removeChildByTag(EMSTag_PromptBox)
end

local luatostring = tostring 
function promptBoxFloatSwitch(pkg)
	-- body
	local info = GameServer_pb.CMD_ATT_COMEUP_SC() 
	info:ParseFromString(pkg)
	Log("xxx===..."..luatostring(info))
	g_prompboxFloatSwitch = info.bIsComeUp 
end

function HaveNewHero(pkg)
   local info = GameServer_pb.CMD_COMMON_GETHERO_SC()
   info:ParseFromString(pkg)
   local data = GetGameData(DataFileHeroBorn, info.iHeroBaseId, "stHeroBornData")
   Log("xxxxx===..."..tostring(info))
   if (g_randomType == 1)then
	  pos = ccp(240, 320)
   else
	  pos = ccp(720, 320)
   end
   local str1 = GetHeroNameByString(data.m_name,info.iHeroNewStep)
   local str2 = GetHeroNameByString(data.m_name,info.iHeroOldStep)
   local namestr = FormatString("HeroShop_HeroSoulInfo",str1,str2,info.iGetHeroSoul)
   Log("str1==="..tostring(str1))
   Log("str2==="..tostring(str2))
   Log("bnamestring==="..tostring(namestr))
   createPromptBoxlayout(namestr)
end 

ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_ATT_COMEUP, "promptBoxFloatSwitch" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_COMMON_GETHERO, "HaveNewHero" )