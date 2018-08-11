--退出帐号
function ExitGameCleanData()
  if MainScene:GetInstance():getChildByTag(EMSTag_Ide_Notice) then 
    	MainScene:GetInstance():removeChildByTag(EMSTag_Ide_Notice)
    end 
    if MainScene:GetInstance():getChildByTag(EMSTag_Ide_Notice + 100) then 
       MainScene:GetInstance():removeChildByTag(EMSTag_Ide_Notice + 100)
    end
    g_arrowLayout:setVisible(false)
   if g_NoticeArrowClone ~= nil then
      g_NoticeArrowClone:setVisible(false)
   end
   if g_NoticeGuideClone ~= nil then
      g_NoticeGuideClone:setVisible(false)
   end
   Reload()
   --SceneMgr:GetInstance():ShowMainMenuSceneUi()
    --切换时清除所有聊天记录
    for i=1, 4 do
	    g_chatChannel[i] = {};
    end
    g_RecordUserData.UserName = ""
    g_login = false;
    --Socket_Connect()

    Login_BackToMaiMenu()
end 

--切换账号
local function ChangeAccountNumber(widget)
	-- body
	local function ChangeAccountNumberFunction(sender,eventType)
		-- body
		if eventType == TOUCH_EVENT_ENDED then
            Reload()
            SceneMgr:GetInstance():ShowMainMenuSceneUi()
            --切换时清除所有聊天记录
            for i=1, 4 do
		        g_chatChannel[i] = {};
	        end
		end
	end
    UI_GetUIButton(widget,3):addTouchEventListener(ChangeAccountNumberFunction)
end

function Mute(widget)
	-- body
	local function MuteFunction(sender,eventType)
		-- body
		if eventType == TOUCH_EVENT_ENDED then
			if GetAudioMgr():isEnabled() == false then
                UI_GetUIButton(widget, 46):setBright(true)
				GetAudioMgr():setEnabled(true)  
                --SimpleAudioEngine:sharedEngine():setEnableEffect(true) 
                SimpleAudioEngine:sharedEngine():setEffectsVolume(1.0)       
			else
                UI_GetUIButton(widget, 46):setBright(false)
				GetAudioMgr():setEnabled(false)
               -- SimpleAudioEngine:sharedEngine():setEnableEffect(false) 
                SimpleAudioEngine:sharedEngine():setEffectsVolume(0)
			end
		end
	end
    UI_GetUIButton(widget,46):addTouchEventListener(MuteFunction)
end

local function Announcement(widget)
	-- body
	local function AnnouncementFunction(sender,eventType)
		-- body
		if eventType == TOUCH_EVENT_ENDED then
			UI_CloseCurBaseWidget(EUICloseAction_None)
           AnnouceMentInfo()
		end
	end
	UI_GetUIButton(widget,5):addTouchEventListener(AnnouncementFunction)
end

--兑换码
function EnterActivityID(widget)
-- body
	local function EnterActivityId(sender,eventType)
		-- body
		if eventType == TOUCH_EVENT_ENDED then
			ActivityID_Create()
		end
	end
	UI_GetUIButton(widget,1006):addTouchEventListener(EnterActivityId)
end
function SystemSettings()
	-- body
	local widget = UI_CreateBaseWidgetByFileName("SystemSettings.json")

    UI_GetUILayout(widget, 4616720):addTouchEventListener(PublicCallBackWithNothingToDo)
	--return 
	UI_GetUIButton(widget,1):addTouchEventListener(UI_ClickCloseCurBaseWidget)


	--切换帐号功能
	ChangeAccountNumber(widget)

	--静音
	Mute(widget)

	--最新公告
	Announcement(widget)

    --最新公告
	EnterActivityID(widget)

    --初始化静音按钮的状态
    if GetAudioMgr():isEnabled() then 
        UI_GetUIButton(widget, 4):setBright(true)
    else 
        UI_GetUIButton(widget, 4):setBright(false)
    end
end



--bgmusic 
function CreateGameBgMusic(file)
	-- body
	-- SimpleAudioEngine:sharedEngine():playBackgroundMusic("Data/Sound/"..file,true)
	-- --AudioEngine:playBackgroundMusic("Data/Sound/"..file,true)
	--local a = CCComAudio:create()
	GetAudioMgr():setBgstrFilePath("Data/Sound/"..file)
    GetAudioMgr():playBackgroundMusic("Data/Sound/"..file,true)
    GetAudioMgr():setBackgroundMusicVolume(0.8)
end

function CreateGameEffectMusic(file)
	-- body
	GetAudioMgr():playEffect("Data/Sound/"..file,false)
end



function InitGameBgMusicFile()
	-- body
	for key,value in pairs(g_bgMusicFileTable) do
		Log("value==="..value)
        GetAudioMgr():preloadBackgroundMusic("Data/Sound/"..value)
	end
end

function InitGameEffectMusicFile()
	-- body
	for key,value in pairs(g_effectMusicFileTable) do
		Log("value==="..value)
        GetAudioMgr():preloadEffect("Data/Sound/"..value)
	end
end


--fighttingbg music
function PlayFightingBg()
	-- body
	if GetAudioMgr():isEnabled() == true then
		Log("FightMgr:GetInstance().m_fightType==="..FightMgr:GetInstance().m_fightType)
		-- if FightMgr:GetInstance().m_fightType == GameServer_pb.en_FightType_Scene then 
		-- 	--fuben music
  --   	   CreateGameBgMusic(g_bgMusicFileTable["fuben"])
    	if FightMgr:GetInstance().m_fightType == GameServer_pb.en_FightType_Arena
    	   or FightMgr:GetInstance().m_fightType == GameServer_pb.en_FightType_CampBattleFactory
    	   or FightMgr:GetInstance().m_fightType == GameServer_pb.en_FightType_LegionCity
    	   or FightMgr:GetInstance().m_fightType == GameServer_pb.en_FightType_DomainLoot then
    		--竞技场
    		--阵营战
    		--领地争夺
    		--军团
    		Log("pvp")
    	   CreateGameBgMusic(g_bgMusicFileTable["pvp"])
    	else
    	   CreateGameBgMusic(g_bgMusicFileTable["fuben"])
    	end
    end
end

--点击音效
function playUiEffect()
	-- body
	GetAudioMgr():playEffect("Data/Sound/"..g_effectMusicFileTable["ui"],false)
end



--AnnouceMent 

local l_CSVnumber = nil 

function AnnouceMentInfo()
	-- body
	local widget = UI_CreateBaseWidgetByFileName("Announcement.json")
    UI_GetUILayout(widget, 4755896):addTouchEventListener(PublicCallBackWithNothingToDo)
	--return 
	UI_GetUIButton(widget,2):addTouchEventListener(UI_ClickCloseCurBaseWidget)


	--info
	-- for i = 0,
	--local infolayout = UI_GetUILayout()
	local titlelist = UI_GetUIListView(widget,3)
	titlelist:removeAllChildren()
	local curlayout = nil 
	local i = 0;
	local function ClickAnnouncementFunc(sender,eventType)
		-- body
		if eventType == TOUCH_EVENT_ENDED then
		   local sender = tolua.cast(sender,"Button")
		   local tag = titlelist:getIndex(sender:getParent())
		   Log("tag=========="..tag)
		   AnnounceMentINfo_Refresh(widget,tag)
		end
	end
	while GetGameData(DataFileAnnnouceMent,i, "stAnnounceMentData") ~= nil do
        local annoucementdata = GetGameData(DataFileAnnnouceMent,i,"stAnnounceMentData")

        curlayout = UI_GetCloneLayout(UI_GetUILayout(widget,4))
        curlayout:setVisible(true)
        UI_GetUIButton(curlayout,1):setTitleText(annoucementdata.m_title)
        UI_GetUIButton(curlayout,1):setTitleFontSize(24)
        titlelist:pushBackCustomItem(curlayout)
        UI_GetUIButton(curlayout,1):addTouchEventListener(ClickAnnouncementFunc)
        i = i+1
	end
    l_CSVnumber = i
    AnnounceMentINfo_Refresh(widget,0)
end

--在csv中以*来代替\n那么在csv中的*是用来换行的(工具的原因)
local function AnnouceMentInfo_Change(res)
	-- body
	local infostring = string.gsub(res,"*","\\n")
	return FormatString("System_Announcement",infostring)
end

function AnnounceMentINfo_Refresh(widget,index)
	-- body
	local annoucementdata = GetGameData(DataFileAnnnouceMent,index,"stAnnounceMentData")
        
    local rigthImage = UI_GetUIImageView(widget, 1001)
    --公告名称
    UI_GetUILabel(rigthImage, 2001):setText(annoucementdata.m_name)
    --活动时间
    UI_GetUILabel(rigthImage, 2002):setText(annoucementdata.m_time)

	local listView = UI_GetUIListView(widget, 4768633)
    listView:removeAllItems()
    --活动名称
 --   local nameLayout = UI_GetUILayout(widget, 4768634):clone()
 --   UI_GetUILayout(widget, 4768634):setVisible(false)
 --   nameLayout:setVisible(true)
 --   UI_GetUILabel(nameLayout, 1122):setText(annoucementdata.m_name)
 --   listView:pushBackCustomItem(nameLayout)
	----时间
 --   local timeLayout = UI_GetUILayout(widget, 4768642):clone()
 --   UI_GetUILayout(widget, 4768642):setVisible(false)
 --   timeLayout:setVisible(true)
 --   UI_GetUILabel(timeLayout, 2):setText(annoucementdata.m_time)
	--listView:pushBackCustomItem(timeLayout)
    --"活动内容"
    local ContentTextLayout = UI_GetUILayout(widget, 4768657):clone()
    UI_GetUILayout(widget, 4768657):setVisible(false)
    ContentTextLayout:setVisible(true)
    listView:pushBackCustomItem(ContentTextLayout)

    --颜色
    local colorLabel = UI_GetUILabel(widget:getChildByTag(4768649), 1)
    
	local infodesc = CompLabel:GetDefaultCompLabel(GetCompLabelStrByLabel(colorLabel,annoucementdata.m_info,30),listView:getSize().width)
    listView:pushBackCustomItem(infodesc)
    
    local titlelist = UI_GetUIListView(widget,3)
    for i = 0,l_CSVnumber - 1 do
    	if i == index then
    		UI_GetUIButton(titlelist:getItem(i),1):setButtonEnabled(false)
    	else
    		UI_GetUIButton(titlelist:getItem(i),1):setButtonEnabled(true)
    	end
    end
end

--使用json中的文本框的字体颜色和大小，将一个已知文本替换成新的字符串使其在CompLabel中创建出相同的样式
function GetCompLabelStrByLabel(label,str,height)
    local color = label:getColor()
    local size = label:getFontSize()
    str = string.gsub(str,"*","\\n")
    local info 
    if height == nil then
        info = "<lb sz=\""..size.."\" cl=\""..color.r.." "..color.g.." "..color.b.."\" c=\""..str.."\" />"
    else
        info = "<s><clh h=\""..height.."\"/><lb sz=\""..size.."\" cl=\""..color.r.." "..color.g.." "..color.b.."\" c=\""..str.."\" /></s>"
    end
    return info
end

function PublicCallBackWithNothingToDo(sender,eventType)

end