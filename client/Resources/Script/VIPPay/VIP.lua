local l_VipInfoViewPage_Index = nil 
function VIPInfo()
	-- body
	local widget = UI_CreateBaseWidgetByFileName("VipSystem.json")
	UI_GetUIButton(widget,1):addTouchEventListener(UI_ClickCloseCurBaseWidget)
    UI_GetUILayout(widget, 4773696):addTouchEventListener(PublicCallBackWithNothingToDo)
	PlayerVIP_Refresh(widget)

    UI_GetUILabel(widget:getChildByTag(11), 5):enableStroke()
    UI_GetUILabel(widget, 4):enableStroke()
end

--在csv中以*来代替\n那么在csv中的*是用来换行的(工具的原因)
local function VipLvlPrivilegeInfo_Change(res)
	-- body
	local infostring = string.gsub(res,"*","\n")
	return infostring
end

function IsMaxVipLvl()
	-- body
	local VipInfo = CDataManager:GetInstance():GetGameDataKeyList(DataFileVip)
    local MaxVip = VipInfo:size() - 1
    if GetLocalPlayer():GetInt(EPlayer_VipLevel) < MaxVip then
       return false
    else
       return true
    end
end

function PlayerVIP_Refresh(widget)
	-- body
	--player vip 
	--UI_GetUILabelBMFont(widget,2):setText(GetLocalPlayer():GetInt(EPlayer_VipLevel))
    UI_GetUIImageView(widget, 2):loadTexture(MainUi_VIPPath(GetLocalPlayer():GetInt(EPlayer_VipLevel)))
    if (UI_GetUIButton(widget, 2):getNodeByTag(1) == nil) then
		local armature = GetUIArmature("Effvip")
		armature:getAnimation():playWithIndex(0)
		UI_GetUIButton(widget,2):addNode(armature, 1, 1);
	end
    
	--loadbar
	local curVipExp = GetLocalPlayer():GetInt(EPlayer_VipExp)
	local nextLevelUpExp = GetLocalPlayer():GetInt(EPlayer_VipLevelUpExp)
	UI_GetUILoadingBar(widget,3):setPercent(curVipExp / nextLevelUpExp * 100)

	--label jingdu
	UI_SetLabelText(widget,4,curVipExp.."/"..nextLevelUpExp)

	--pay info
	local paylayout = UI_GetUILayout(widget,11)
    UI_SetLabelText(paylayout,5,(nextLevelUpExp - curVipExp))
    
    local payLabePosx,payLabePosY = UI_GetUILabel(paylayout,5):getPosition()
    local width = UI_GetUILabel(paylayout,5):getContentSize().width
    UI_GetUIImageView(paylayout,6):setPosition(ccp(payLabePosx + width + 5,payLabePosY))

    --next vipsetp
    local VipInfo = CDataManager:GetInstance():GetGameDataKeyList(DataFileVip)
    local MaxVip = VipInfo:size() - 1
    Log("VipMax==="..VipInfo:size())
    if IsMaxVipLvl() == false then
       --UI_GetUILabelBMFont(paylayout,9):setText(GetLocalPlayer():GetInt(EPlayer_VipLevel) + 1)
       UI_GetUIImageView(paylayout, 9):loadTexture(MainUi_VIPPath(GetLocalPlayer():GetInt(EPlayer_VipLevel)+1))
       if (UI_GetUIButton(paylayout, 9):getNodeByTag(1) == nil) then
		    local armature = GetUIArmature("Effvip")
		    armature:getAnimation():playWithIndex(0)
		    UI_GetUIButton(paylayout,9):addNode(armature, 1, 1);
	    end

    else
       paylayout:setVisible(false)
       UI_GetUILayout(widget,12):setVisible(true)
    end

    --充值
    local function PayFunc(sender,eventType)
    	-- body
    	if eventType == TOUCH_EVENT_ENDED then
    		UI_CloseCurBaseWidget(EUICloseAction_None)
            RechargeSystem_ShowLayout()
    	end
    end
    UI_GetUIButton(widget,7):addTouchEventListener(PayFunc)

    --pageView
    local pageview = UI_GetUIPageView(widget,8)
    pageview:removeAllPages()
    local curlayout = nil 
    for i = 0, MaxVip do 
        curlayout = UI_GetCloneLayout(UI_GetUILayout(widget,10))
        curlayout:setVisible(true)
        --nextvip lvl
        --UI_GetUILabelBMFont(curlayout,1):setText(i)
        UI_GetUIImageView(curlayout, 1):loadTexture(MainUi_VIPPath(i))
        if (UI_GetUIButton(curlayout, 1):getNodeByTag(1) == nil) then
		    local armature = GetUIArmature("Effvip")
		    armature:getAnimation():playWithIndex(0)
		    UI_GetUIButton(curlayout,1):addNode(armature, 1, 1);
	    end
        --UI_GetUILabel(curlayout, 11):setText(i)
        VipInfodata = GetGameData(DataFileVip,i,"stVipData")
        --desc
        --UI_SetLabelText(UI_GetUILayout(curlayout,4),1,VipLvlPrivilegeInfo_Change(VipInfodata.m_LvlPrivilege))
        if i == GetLocalPlayer():GetInt(EPlayer_VipLevel) or i == GetLocalPlayer():GetInt(EPlayer_VipLevel) + 1 or i == GetLocalPlayer():GetInt(EPlayer_VipLevel) + 2 then
            UI_SetLabelText(UI_GetUILayout(curlayout,4),1,VipLvlPrivilegeInfo_Change(VipInfodata.m_LvlPrivilege))
        end
        pageview:addPage(curlayout)
        --lefebutton
        local function leftFunc(sender,eventType)
    	-- body
    	   if eventType == TOUCH_EVENT_ENDED then
    	      pageview:scrollToPage(pageview:getCurPageIndex() - 1)
    	   end
        end
        UI_GetUIButton(curlayout,2):addTouchEventListener(leftFunc)

        --rightButton
        local function rightFunc(sender,eventType)
        	-- body
        	if eventType == TOUCH_EVENT_ENDED then
        	   pageview:scrollToPage(pageview:getCurPageIndex() + 1)
        	end
        end
        UI_GetUIButton(curlayout,3):addTouchEventListener(rightFunc)

        if i == 1 then 
           UI_GetUIButton(curlayout,2):setVisible(false)
        elseif i == MaxVip then
           UI_GetUIButton(curlayout,3):setVisible(false)
        end
    end 
    if IsMaxVipLvl() == false then
       pageview:scrollToPage(GetLocalPlayer():GetInt(EPlayer_VipLevel) + 1)
    else
       pageview:scrollToPage(GetLocalPlayer():GetInt(EPlayer_VipLevel))
    end

    local function refreshPageViewFunc(sender,eventType)
        -- body
        if eventType == PAGEVIEW_EVENT_TURNING then
            local sender = tolua.cast(sender,"PageView")
            local curindex = sender:getCurPageIndex()
            Log("curindex===="..curindex)
            if curindex - 1 >= 0 then
               local pagelayout = sender:getPage(curindex - 1)
               --注意这里是动态加载，在UI中的Label必须不要赋值
               if UI_GetUILabel(UI_GetUILayout(pagelayout,4),1):getStringValue() == "" then
                   VipInfodata = GetGameData(DataFileVip,curindex - 1,"stVipData")
                   UI_SetLabelText(UI_GetUILayout(pagelayout,4),1,VipLvlPrivilegeInfo_Change(VipInfodata.m_LvlPrivilege))
               end
            end
            if curindex + 1 <= MaxVip then
               local pagelayout = sender:getPage(curindex + 1)
                if UI_GetUILabel(UI_GetUILayout(pagelayout,4),1):getStringValue() == "" then
                   VipInfodata = GetGameData(DataFileVip,curindex + 1,"stVipData")
                   UI_SetLabelText(UI_GetUILayout(pagelayout,4),1,VipLvlPrivilegeInfo_Change(VipInfodata.m_LvlPrivilege))
               end
            end
        end
    end
    pageview:addEventListenerPageView(refreshPageViewFunc)

    UI_GetUILayout(widget,10):setVisible(false)
end
function UpdataPlayerVipLevelUp(pkg)
	-- body
	local info = GameServer_pb.CMD_ACTOR_VIPLEVELUP_SC()
	info:ParseFromString(pkg)
	Log("UpdataPlayerVipLevelUp==="..tostring(info))
	GetLocalPlayer():SetInt(EPlayer_VipLevel,info.iVipLevel)
	GetLocalPlayer():SetInt(EPlayer_VipExp,info.iVipExp)
	GetLocalPlayer():SetInt(EPlayer_VipLevelUpExp,info.iVipLevelUpExp)
	UI_RefreshBaseWidgetByName("MainUi")
end
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_ACTOR_VIPLEVELUP, "UpdataPlayerVipLevelUp" );