local l_worldbaseInfo = nil 

--记录开始之前的消息
local l_beforworldBossStartInfo = nil 

local l_IsWorldBossIsStart = nil 

--记录开始中的消息
local l_inWorldBossInfo = nil 

--记录伤害排名信息
local l_worldBossHurtRank = nil 

--世界boss结算信息
local l_worldBossSettleMentInfo = nil 

--记录玩家选择的状态。这里涉及到战斗之后的跳转
g_worldBossAutoFightCheckBoxState = nil

--记录玩家鼓舞次数
l_worldBossCheerCount = nil 

function WorldBossBaseInfo()
	-- body
	local widget = UI_CreateBaseWidgetByFileName("WorldBoss.json")
    
    UI_GetUILayout(widget, 4760312):addTouchEventListener(PublicCallBackWithNothingToDo)
	--return bt
	-- local function ClickReturnFunc(sender,eventType)
	-- 	-- body
	-- 	if eventType == TOUCH_EVENT_ENDED then
	-- 		UI_CloseCurBaseWidget()
	-- 	end
	-- end
	local returnBt = UI_GetUIButton(widget,13)
    returnBt:addTouchEventListener(UI_ClickCloseCurBaseWidget)

    local function BesureBesurInpiration()
    	-- body
		Packet_Cmd(GameServer_pb.CMD_CHEER_WORLDBOSS)
        ShowWaiting()
    end

    --鼓舞功能
    local function InspirationFunc(sender,eventType)
	-- body
		if eventType == TOUCH_EVENT_ENDED then
			if l_IsWorldBossIsStart == false then
				Messagebox_Create({title = FormatString("LegionBoss_Tip"),info = FormatString("WorldBoss_InspirationTip"),msgType = EMessageType_Middle})
			else
				if l_worldBossCheerCount >= g_WorldBossInspirationMaxNum then					
	                Messagebox_Create({title = FormatString("LegionBoss_Tip"),info = FormatString("LegionBoss_OverInspirationTip"),msgType = EMessageType_Middle})
				else
	                --UI_SetLabelText(wiget,4,FormatString("LegionBoss_InspirationAtt"),l_bossFightInfo.fightInfo.iCheeredCount * g_LegionBossInspirationAtt)
		            Messagebox_Create({info = FormatString("LegionBoss_InspirationAddAttTip"),title = FormatString("LegionBoss_Tip"), msgType = EMessageType_LeftRight, leftFun = BesureBesurInpiration})       
				end
			end
		end
    end
	local inspirationBt = UI_GetUIButton(widget,5)
	inspirationBt:addTouchEventListener(InspirationFunc)

	--参战按钮
	local function StartWorldBossFunc(sender,eventType)
		-- body
		if eventType == TOUCH_EVENT_ENDED then
			g_openUIType = EUIOpenType_WorldBoss
			Packet_Cmd(GameServer_pb.CMD_FIGHT_WORLDBOSS) 
		end
	end
	UI_GetUIButton(widget,6):addTouchEventListener(StartWorldBossFunc)

	--复活按钮
	local hurtRankListLayout = UI_GetUILayout(widget,3)
	local function WorldBossPlayerRelive(sender,eventType)
		-- body
		if eventType == TOUCH_EVENT_ENDED then
		    Packet_Cmd(GameServer_pb.CMD_WORLDBOSS_RELIVE) 
            ShowWaiting()
		end
	end
	UI_GetUIButton(UI_GetUILayout(hurtRankListLayout,14),1):addTouchEventListener(WorldBossPlayerRelive)

	if l_worldBossSettleMentInfo ~= nil then
		WorldBossSettlementInfo()
	end
    if g_worldBossAutoFightCheckBoxState ~= nil then
	   UI_GetUICheckBox(hurtRankListLayout,6):setSelectedState(g_worldBossAutoFightCheckBoxState)
	else
	   UI_GetUICheckBox(hurtRankListLayout,6):setSelectedState(true)
	end
    local function clickAutoFightFunc(sender,eventType)
    	-- body
    	if eventType == CHECKBOX_STATE_EVENT_SELECTED then
    	   g_worldBossAutoFightCheckBoxState = true
    	   Log("xuanzhong")
    	elseif  eventType == CHECKBOX_STATE_EVENT_UNSELECTED then
    		Log("meiyouxuanzhong")
    		g_worldBossAutoFightCheckBoxState = false
    	end
    end
    UI_GetUICheckBox(hurtRankListLayout,6):addEventListenerCheckBox(clickAutoFightFunc)


    --Boss等级 + Boss名字 + boss形象
    --lvl 
    UI_SetLabelText(widget,11,l_worldbaseInfo.iBossLevel)

    --name
    UI_SetLabelText(widget,10,g_WorldBossName[l_worldbaseInfo.iShowMonsterID])

    --image
    UI_GetUIImageView(UI_GetUIImageView(widget,12),12):loadTexture("ImgUi/WorldBossImage/"..g_WorldBossImage[l_worldbaseInfo.iShowMonsterID])


    if l_IsWorldBossIsStart == false then
       l_beforworldBossStartInfo = l_worldbaseInfo.preStartInfo
       beforeWorldBossStart(widget)
    else
        l_inWorldBossInfo = l_worldbaseInfo.fightInfo
        l_worldBossHurtRank = l_worldbaseInfo.fightInfo.szRankList
       InWorldBossInfo(widget)
    end
end

function beforeWorldBossStart(widget)
	-- body
	--世界boss战斗开始之前
    
    --隐藏排行榜
    local hurtRankListLayout = UI_GetUILayout(widget,3)
    hurtRankListLayout:setVisible(false)
    
    local function KillCD(sender,eventType)
    	-- body
		Packet_Cmd(GameServer_pb.CMD_QUERY_WORLDBOSS)
        ShowWaiting()	   	
    end
    --开启倒计时
    local SingUpTimelab = UI_GetUILabel(widget,7)
    SingUpTimelab:setTimer(l_beforworldBossStartInfo.iWaitSecond + 1, TimeFormatType_HourMinSec)
    SingUpTimelab:addEventListenerTimer(KillCD)

    --bossHp
    local BossMaxHp = l_beforworldBossStartInfo.iBossMaxHP
    local BossCurHp = l_beforworldBossStartInfo.iBossHP
    WorldBossInfo_Refresh(widget,BossMaxHp,BossCurHp)
    
    --
    --UI_GetUILabel(widget,4):setVisible(false)
    UI_GetUILayout(widget,4):setVisible(false)
    UI_GetUIButton(widget,6):setVisible(false)
    UI_GetUILayout(UI_GetUILayout(widget,3),14):setVisible(false)
end

function WorldBossInfo_Refresh(widget,MaxHp,CurHp)
	-- body
	local HpLoadingbar = UI_GetUILoadingBar(widget,1)
	HpLoadingbar:setPercent(CurHp / MaxHp *100)
	UI_SetLabelText(widget,2,CurHp.."/"..MaxHp)
end

function WorldBossAutoFight(widget)
 	-- body
 	local hurtRankListLayout = UI_GetUILayout(widget,3)
 	if UI_GetUICheckBox(hurtRankListLayout,6):getSelectedState() == true then
 		Log("zidongzhandoukaishil")
 		g_openUIType = EUIOpenType_WorldBoss
		Packet_Cmd(GameServer_pb.CMD_FIGHT_WORLDBOSS) 
 	end
 end 

local curautofightingbtPositionX,curautofightingbtPositionY = nil,nil
local curautofightinglabPositionX,curautofightinglabPositionY = nil,nil
local autofightingbtPositionX,autofightingbtPositionY = nil,nil 
local autofightinglabPositionX,autofightinglabPositionY = nil,nil 
function InWorldBossInfo(widget)
	-- body
	--隐藏倒计时
	--boss没有时限

    --bossHp
    local BossMaxHp = l_inWorldBossInfo.iBossMaxHP
    local BossCurHp = l_inWorldBossInfo.iBossHP
    WorldBossInfo_Refresh(widget,BossMaxHp,BossCurHp)

	UI_GetUILabel(widget,7):setVisible(false)
    UI_GetUILabel(widget,8):setVisible(false)
    UI_GetUIImageView(widget,15):setVisible(false)
	--显示排行榜信息
	local hurtRankListLayout = UI_GetUILayout(widget,3)
    hurtRankListLayout:setVisible(true)

    UI_GetUILabel(widget,4):setVisible(true)
    UI_GetUIButton(widget,6):setVisible(true)

    --保存当前自动战斗的位置
    curautofightingbtPositionX,curautofightingbtPositionY = UI_GetUICheckBox(hurtRankListLayout,6):getPosition()
    curautofightinglabPositionX,curautofightinglabPositionY = UI_GetUILabel(hurtRankListLayout,7):getPosition()
    --移动过后的位置
    autofightingbtPositionX,autofightingbtPositionY = UI_GetUICheckBox(hurtRankListLayout,16):getPosition()
    autofightinglabPositionX,autofightinglabPositionY = UI_GetUILabel(hurtRankListLayout,15):getPosition()

    --鼓舞
    cheerWorldBossNumberIcon(l_inWorldBossInfo.iCheeredCount * g_WorldBosssInspirationAtt)
    local function ResurrectionCD(sender,eventType)
    	-- body
    	Log("zi")
    	UI_GetUIButton(widget,6):setVisible(true)
        UI_GetUILayout(hurtRankListLayout,14):setVisible(false)
        UI_GetUICheckBox(hurtRankListLayout,6):setPosition(ccp(curautofightingbtPositionX,curautofightingbtPositionY))
        UI_GetUILabel(hurtRankListLayout,7):setPosition(ccp(curautofightinglabPositionX,curautofightinglabPositionY))
    	WorldBossAutoFight(widget)
    end
    --显示复活或者参战
    if l_inWorldBossInfo.iReliveCD == 0 then
    	Log("life")
    	UI_GetUIButton(widget,6):setVisible(true)
        UI_GetUILayout(hurtRankListLayout,14):setVisible(false)
        UI_GetUICheckBox(hurtRankListLayout,6):setPosition(ccp(curautofightingbtPositionX,curautofightingbtPositionY))
        UI_GetUILabel(hurtRankListLayout,7):setPosition(ccp(curautofightinglabPositionX,curautofightinglabPositionY))
    else
    	Log("die")
        UI_GetUILabel(UI_GetUILayout(hurtRankListLayout,14),2):setVisible(true)
    	UI_GetUIButton(widget,6):setVisible(false)
        UI_GetUILayout(hurtRankListLayout,14):setVisible(true)
        UI_GetUICheckBox(hurtRankListLayout,6):setPosition(ccp(autofightingbtPositionX,autofightingbtPositionY))
        UI_GetUILabel(hurtRankListLayout,7):setPosition(ccp(autofightinglabPositionX,autofightinglabPositionY))
        UI_GetUILabel(UI_GetUILayout(hurtRankListLayout,14),2):setTimer(l_inWorldBossInfo.iReliveCD,TimeFormatType_Auto)
        UI_GetUILabel(UI_GetUILayout(hurtRankListLayout,14),2):addEventListenerTimer(ResurrectionCD)
        UI_SetLabelText(UI_GetUILayout(hurtRankListLayout,14),3,g_WorldBossResurrectionCost)
    end

     --我的伤害
     UI_SetLabelText(hurtRankListLayout,3,l_inWorldBossInfo.iSelfDamage)
    
     --铜币
     UI_SetLabelText(hurtRankListLayout,11,l_inWorldBossInfo.iSumSilver)

     --荣誉
     UI_SetLabelText(hurtRankListLayout,5,l_inWorldBossInfo.iSumHonor)
 --    --位置
 --    local x,y = UI_GetUILabel(hurtRankListLayout,3):getPosition()
 --    local sliverlabwidth = UI_GetUILabel(hurtRankListLayout,3):getContentSize().width 
 --    UI_GetUIImageView(hurtRankListLayout,4):setPosition(ccpAdd(ccp(x,y),ccp(sliverlabwidth + 20,0)))
 --    local imageWidth = UI_GetUIImageView(hurtRankListLayout,4):getContentSize().width 
 --    UI_GetUILabel(hurtRankListLayout,5):setPosition(ccpAdd(ccp(x,y),ccp(sliverlabwidth + imageWidth + 20,0)))

     WorldBossHurtRank(widget)
end

function WorldBossHurtRank(widget)
	-- body
	local hurtRankListLayout = UI_GetUILayout(widget,3)
	local hurtRankList = UI_GetUIListView(hurtRankListLayout,1)
	local width = hurtRankList:getContentSize().width
	local templayout = Layout:create()
	templayout:setSize(CCSizeMake(width,30))
    hurtRankList:removeAllItems()
    hurtRankList:setItemModel(templayout)
    
    local namePositionX = UI_GetUILabel(hurtRankListLayout,8):getPosition()
    local hurtValuePositionX = UI_GetUILabel(hurtRankListLayout,9):getPosition()
    local hurtValuePercentPositionX = UI_GetUILabel(hurtRankListLayout,10):getPosition()

    --boss战 前三名的颜色
    local colorString = {ccc3(240,0,255),ccc3(0,198,255),ccc3(12,255,0)}
    local layout = nil 
    local function createWorldBossHurtRank(index)
    	-- body
        local parentImage = UI_GetUIImageView(layout, 1)
    	--hurtvaluepercentage
        local hurpecent = l_worldBossHurtRank[index].iDamageValue / l_inWorldBossInfo.iBossMaxHP
        local temphurtpercent = string.format("%6.2f",hurpecent*100)
        UI_SetLabelText(parentImage, 1, l_worldBossHurtRank[index].strName)
        UI_SetLabelText(parentImage, 2, l_worldBossHurtRank[index].iDamageValue)
        UI_SetLabelText(parentImage, 3, ""..temphurtpercent.."%")
        local labTag = {1,2,3}
        if index <= 3 then
           for i = 1,#labTag do 
              UI_GetUILabel(parentImage,labTag[i] ):setColor(colorString[index])
           end 
        end
    end
    for i = 1, #l_worldBossHurtRank do
    	layout = UI_GetCloneLayout(UI_GetUILayout(widget, 100))
        layout:setVisible(true)
        layout:setTouchEnabled(false)
    	--local node = 
        createWorldBossHurtRank(i)
    	--layout:addChild(node)
    	hurtRankList:pushBackCustomItem(layout)
    end
end

function WorldBossSettlementInfo()
	-- body
	local widget = UI_CreateBaseWidgetByFileName("WorldBossSettlement.json")
	local layoutparent = UI_GetUILayout(widget,1)

	if l_worldBossSettleMentInfo.iRank == 0 then
		UI_SetLabelText(layoutparent,1,FormatString("LegionBoss_NotOnTheRank"))
	else
		UI_SetLabelText(layoutparent,1,FormatString("LegionBoss_OnTheRank",l_worldBossSettleMentInfo.iRank))
	end

	UI_SetLabelText(layoutparent,2,FormatString("LegionBoss_MyHurt",l_worldBossSettleMentInfo.iDamage))
    UI_SetLabelText(layoutparent,3,l_worldBossSettleMentInfo.iAwardSilver)
--    local x,y = UI_GetUILabel(layoutparent,3):getPosition()
--    local width = UI_GetUILabel(layoutparent,3):getContentSize().width
--    UI_GetUIImageView(layoutparent,4):setPosition(ccpAdd(ccp(x,y),ccp(width + 20,0)))
    UI_SetLabelText(layoutparent,5,l_worldBossSettleMentInfo.iAwardHonor)
--    local imageWidth = UI_GetUIImageView(layoutparent,4):getContentSize().width 
--    UI_GetUILabel(layoutparent,5):setPosition(ccpAdd(ccp(x,y),ccp(width + imageWidth + 20,0)))
        local function beSureBtFunc(sender,eventType)
       	-- body
       	    if eventType == TOUCH_EVENT_ENDED then
       	       l_worldBossSettleMentInfo = nil
               UI_CloseCurBaseWidget()
       	    end
       end
    UI_GetUIButton(layoutparent,6):addTouchEventListener(beSureBtFunc)

    if UI_GetUILayout(widget, 1124) then
        UI_GetUILayout(widget, 1124):addTouchEventListener(PublicCallBackWithNothingToDo)
    end
end

function cheerWorldBossNumberIcon(value)
    -- body
    local img = nil 
    if value == 0 then
        img =  "font_01.png"
    elseif value == 20 then
        img = "font_02.png"
    elseif value == 40 then
        img = "font_03.png"
    elseif value == 60 then
        img = "font_04.png"
    elseif value == 80 then
        img = "font_05.png"
    elseif value == 100 then
        img = "font_06.png"
    end
    UI_GetUIImageView(UI_GetUILayout(UI_GetBaseWidgetByName("WorldBoss"),4),1):loadTexture("ImgUi/WorldBossCheerNumber/"..img)
end

local custonToString = tostring
function queryWorldBossInfo(pkg)
	-- body
	EndWaiting()
    l_worldbaseInfo = GameServer_pb.CMD_QUERY_WORLDBOSS_SC()
    l_worldbaseInfo:ParseFromString(pkg)
    Log("xxxxx ===="..custonToString(l_worldbaseInfo))
    l_IsWorldBossIsStart = l_worldbaseInfo.bStarted
    local widget = UI_GetBaseWidgetByName("WorldBoss")
    if widget then
    	Log("fresh")
        l_inWorldBossInfo = l_worldbaseInfo.fightInfo
        l_worldBossCheerCount = l_worldbaseInfo.fightInfo.iCheeredCount
        l_worldBossHurtRank = l_worldbaseInfo.fightInfo.szRankList
    	InWorldBossInfo(widget)
    else
    	Log("Create")
    	l_inWorldBossInfo = l_worldbaseInfo.fightInfo
        l_worldBossCheerCount = l_worldbaseInfo.fightInfo.iCheeredCount
    	l_worldBossHurtRank = l_worldbaseInfo.fightInfo.szRankList
        WorldBossBaseInfo()
    end
end

function cheerWorldBossInfo(pkg)
	-- body
	EndWaiting()
    local widget = UI_GetBaseWidgetByName("WorldBoss")
    if widget then
    	local info = GameServer_pb.CMD_CHEER_WORLDBOSS_SC()
    	info:ParseFromString(pkg)
        Log("xxxxx ===="..custonToString(info))
        l_worldBossCheerCount = info.iCheerCount
    	--刷新战斗加成
    	-- UI_SetLabelText(GetCurWidget(),4,FormatString("LegionBoss_InspirationAtt",info.iCheerCount * g_WorldBosssInspirationAtt))
        cheerWorldBossNumberIcon(info.iCheerCount * g_WorldBosssInspirationAtt)
    end
end

function updateWorldBossInfo(pkg)
	-- body
    local widget = UI_GetBaseWidgetByName("WorldBoss")
    if widget then
        local info = GameServer_pb.CMD_UPDATE_WORLDBOSS_SC()
        info:ParseFromString(pkg)
        Log("xxxxx ===="..custonToString(info))
        l_worldBossHurtRank = info.updateInfo.szRankList
        WorldBossHurtRank(widget)
        WorldBossInfo_Refresh(widget,info.updateInfo.iBossMaxHP,info.updateInfo.iBossHP)
    end
end

function reliveWorldBossInfo()
	-- body
	EndWaiting()
    Log("xxxxxxxxxxxxxxxsadasdasdas")
	local widget = UI_GetBaseWidgetByName("WorldBoss")
	local hurtRankListLayout = UI_GetUILayout(widget,3)
    UI_GetUIButton(widget,6):setVisible(true)
    UI_GetUILayout(hurtRankListLayout,14):setVisible(false)
    UI_GetUICheckBox(hurtRankListLayout,6):setPosition(ccp(curautofightingbtPositionX,curautofightingbtPositionY))
    UI_GetUILabel(hurtRankListLayout,7):setPosition(ccp(curautofightinglabPositionX,curautofightinglabPositionY))
    UI_GetUILabel(UI_GetUILayout(hurtRankListLayout,14),2):stopTimer()
    UI_GetUILabel(UI_GetUILayout(hurtRankListLayout,14),2):setVisible(false)
end

function worldBossEnd(pkg)
	-- body
    l_worldBossSettleMentInfo = GameServer_pb.CMD_WORLDBOSS_END_SC()
    l_worldBossSettleMentInfo:ParseFromString(pkg)
    if UI_GetBaseWidgetByName("WorldBoss") then 
       if UI_GetBaseWidgetByName("WorldBossSettlement") == nil then 
          WorldBossSettlementInfo()
       end 
    end 
    Log("l_worldBossSettleMentInfo========="..tostring(l_worldBossSettleMentInfo))
end
ScriptSys:GetInstance():RegisterScriptFunc(GameServer_pb.CMD_QUERY_WORLDBOSS, "queryWorldBossInfo")
ScriptSys:GetInstance():RegisterScriptFunc(GameServer_pb.CMD_CHEER_WORLDBOSS, "cheerWorldBossInfo")
ScriptSys:GetInstance():RegisterScriptFunc(GameServer_pb.CMD_UPDATE_WORLDBOSS, "updateWorldBossInfo")
ScriptSys:GetInstance():RegisterScriptFunc(GameServer_pb.CMD_WORLDBOSS_RELIVE, "reliveWorldBossInfo")
ScriptSys:GetInstance():RegisterScriptFunc(GameServer_pb.CMD_WORLDBOSS_END, "worldBossEnd")