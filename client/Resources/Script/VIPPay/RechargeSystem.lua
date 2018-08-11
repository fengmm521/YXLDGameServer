--
--                       _oo0oo_
--                      o8888888o
--                      88" . "88
--                      (| O=O |)
--                      0\  -  /0
--                    ___/`---'\___
--                  .' \\|     |-- '.
--                 / \\|||  :  |||-- \
--                / _||||| -:- |||||- \
--               |   | \\\  -  --/ |   |
--               | \_|  ''\---/''  |_/ |
--               \  .-\__  '-'  ___/-. /
--             ___'. .'  /--.--\  `. .'___
--          ."" '<  `.___\_<|>_/___.' >' "".
--         | | :  `- \`.;`\ _ /`;.`/ - ` : | |
--         \  \ `_.   \_ __\ /__ _/   .-` /  /
--     =====`-.____`.___ \_____/___.-`___.-'=====
--                       `=---='
--
--
--     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
--
--               ∑◊Ê±£””         ”¿ŒﬁBUG
--

if g_ClickIndex == nil then 
   g_ClickIndex = 0
end 

if g_JSFUCK_PricePro == nil then 
   g_JSFUCK_PricePro = ""
end 
function RechargeSystem_InitValue()
  -- body
   g_ClickIndex = 0
   g_JSFUCK_PricePro = ""
end


local l_money = nil;	-- purchasing money, xionghai add, 

---÷ß∏∂ΩÁ√Ê
function RechargeSystem_ShowLayout()  
    Packet_Cmd(GameServer_pb.CMD_PAYMENT_QUERY);
    ShowWaiting()
    --RechargeSystem_InitData()
end

function ReChargeSystem_JSPayLayout()
    if getChannelID() == "JSFUCK" then 
       local userID = TBTSDK:GetInstance().s_channelSdkInfo.UserId
       local money = RechargeSystem_RechargeGetMoney(g_ClickIndex) / 10
       TBTSDK:GetInstance():payTotal(g_JSFUCK_PricePro,userID,money)
    end 
end 

function RechargeSystem_InitData(pkg) 
    EndWaiting()
    local info = GameServer_pb.CMD_PAYMENT_QUERY_SC()
    info:ParseFromString(pkg)
    Log("info==========="..tostring(info))
 
    local widget = UI_GetBaseWidgetByName("RechargeSystem")
    if widget == nil then
        widget = UI_CreateBaseWidgetByFileName("RechargeSystem.json")
    end

   UI_GetUILayout(widget, 4777090):addTouchEventListener(PublicCallBackWithNothingToDo)
   RechargeSystem_InitValue()
   UI_GetUIButton(widget,1):addTouchEventListener(UI_ClickCloseCurBaseWidget)
   RechargeSystem_VipInfo(widget)
   RechargeSystem_RechargeView(widget,info)
end

function RechargeSystem_VipInfo(widget)
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
    print("curVipExp==========="..curVipExp)
    print("nextLevelUpExp==========="..nextLevelUpExp)
	UI_GetUILoadingBar(widget,3):setPercent(curVipExp / nextLevelUpExp * 100)

	--label jingdu
	UI_SetLabelText(widget,4,curVipExp.."/"..nextLevelUpExp)

	--pay info
	local paylayout = UI_GetUILayout(widget,6)
    UI_SetLabelText(paylayout,1,(nextLevelUpExp - curVipExp))
    
    local payLabePosx,payLabePosY = UI_GetUILabel(paylayout,1):getPosition()
    local width = UI_GetUILabel(paylayout,1):getContentSize().width
    UI_GetUIImageView(paylayout,2):setPosition(ccp(payLabePosx + width + 5,payLabePosY))

    --next vipsetp
    local VipInfo = CDataManager:GetInstance():GetGameDataKeyList(DataFileVip)
    local MaxVip = VipInfo:size() - 1
    Log("VipMax==="..VipInfo:size())
    if IsMaxVipLvl() == false then
       --UI_GetUILabelBMFont(paylayout,9):setText(GetLocalPlayer():GetInt(EPlayer_VipLevel) + 1)
       UI_GetUIImageView(paylayout, 3):loadTexture(MainUi_VIPPath(GetLocalPlayer():GetInt(EPlayer_VipLevel)+1))
        if (UI_GetUIButton(paylayout, 3):getNodeByTag(1) == nil) then
	        local armature = GetUIArmature("Effvip")
	        armature:getAnimation():playWithIndex(0)
	        UI_GetUIButton(paylayout,3):addNode(armature, 1, 1);
        end
    else
       paylayout:setVisible(false)
       UI_GetUILayout(widget,7):setVisible(true)
    end

    --vip look
    local function LookVipInfo(sender,eventType)
       if eventType == TOUCH_EVENT_ENDED then
          UI_CloseCurBaseWidget(EUICloseAction_None)
          VIPInfo()
       end 
    end
    UI_GetUIButton(widget,5):addTouchEventListener(LookVipInfo)
end

function RechargeSystem_RechargeGetMoney(tag)
    local payData = GetGameData(DataFilePayment, tag, "stPaymentData");
	Log("payData.m_PayID"..payData.m_PayID)
    return payData.m_PayID*10
end

function RechargeSystem_RechargeGetYuanbao(tag)
   local payData = GetGameData(DataFilePayment, tag, "stPaymentData");
   return payData.m_yuanbao
end
function RechargeSystem_RechargeGetPayID(tag)
    local payData = GetGameData(DataFilePayment, tag, "stPaymentData");
    return payData.m_PayID
end
function RechargeSystem_RechargeGetDes(tag,kind)
    local payData = GetGameData(DataFilePayment, tag, "stPaymentData");
    if kind ==1 then
        return payData.m_des1
    else 
        return payData.m_des2
    end
end
function RechargeSystem_RechargeIsRecommend(tag)
    local payData = GetGameData(DataFilePayment, tag, "stPaymentData");
    if payData.m_Recommend == 1 then
        return true
    else
        return false
    end
end


--- IOS related code
--- define product id table, the key is tag, the value is product id
local ios_productTable = {
    [1]    = "yueka",
    [2]    = "yuanbao6480",
    [3]    = "yuanbao3280",
    [4]    = "yuanbao1980",
    [5]    = "yuanbao980",
    [6]    = "yuanbao300",
    [7]    = "yuanbao1"
};


local function __iosGetProductID(_tag)
    local productID = ios_productTable[_tag];
    return productID;
end

local IOS_IAP_EVENT_ERROR   = 0;
local IOS_IAP_EVENT_PAIED   = 1;
local IOS_IAP_EVENT_END     = 2;


function RechargeSystem_IOS_ErrorCallback(_vars)
    Log(">>>>> ERROR");
    EndWaiting();

    -- get params
    local transID = _vars[0]:Int();
    local errorCode = _vars[1]:Int();
    local errorMsg  = _vars[2]:ToString();

    local message = "TID:"..transID.." errorCode:"..errorCode.." msg:"..errorMsg;
    Log(message);
    --createPromptBoxlayout(message);
end

function RechargeSystem_IOS_PaySuccessCallback(_vars)
    Log(">>>>> PAIED");
    EndWaiting();

    -- get params
    local transID = _vars[0]:Int();
    local receipt = _vars[1]:ToString();
    local message = "TID:"..transID.." receipt:"..receipt;
	Log(message);
	
	-- send request to gs
	local tab = GameServer_pb.CMD_IOS_BUY_GOODS_CS();
	tab.iPrice = l_money;
	tab.receipt = receipt;
	Packet_Full(GameServer_pb.CMD_IOS_BUY_GOODS, tab);
end


function RechargeSystem_IOS_EndCallback(_vars)
    Log(">>>>> END");
    EndWaiting();

    -- get params
    local transID = _vars[0]:Int();
    local message = "TID:"..transID;
    Log(message);
    --createPromptBoxlayout(message);
end

IOSSDK:registerIAPCallback(IOS_IAP_EVENT_ERROR, "RechargeSystem_IOS_ErrorCallback");
IOSSDK:registerIAPCallback(IOS_IAP_EVENT_PAIED, "RechargeSystem_IOS_PaySuccessCallback");
IOSSDK:registerIAPCallback(IOS_IAP_EVENT_END,   "RechargeSystem_IOS_EndCallback");
--- IOS related code end


function RechargeSystem_RechargeView(widget,info)
   local layout = UI_GetUILayout(widget,16)
   local listView = UI_GetUIListView(layout,1)
   listView:removeAllItems()
   local listwidth = listView:getContentSize().width 
   local curlayout = nil 
   local templayout = nil 
   local tagtable = {14,8,9,10,11,12,13}
   Log("WorldId===="..g_RecordUserData.ServerId)
   local function ClickTemplayoutRechageSystem(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then 
        local sender = tolua.cast(sender,"Layout")
        local tag = sender:getTag()
        local money = RechargeSystem_RechargeGetMoney(tag);
		l_money = money;
        Log("tag ==="..tag);
        
        local osID = getOSID();
        Log(">>>>>>> osID:"..osID);
        if osID == "IOS" then
            local productID = __iosGetProductID(tag);
            if productID ~= nil then
                Log("purchase product:"..productID.." x 1");
                ShowWaiting();
                IOSSDK:purchseProductRequest(productID, 1);
            end
        else
            if getChannelID() == "TBT" then
                --money = money / 10
                --TBTSDK:GetInstance():TBUnipayForCoinWithOrder("1234567890aaaa",money,"0_9848654")
                g_ClickIndex = tag
                Packet_Cmd(GameServer_pb.CMD_TBT_GETUUID)
                ShowWaiting()
            elseif getChannelID() == "QQYYB" then
                startSaveMoney(money)
            elseif getChannelID() == "UC" then
                g_ClickIndex = tag
                Packet_Cmd(GameServer_pb.CMD_TBT_GETUUID)
                ShowWaiting()
            elseif getChannelID() == "JSFUCK" then
                g_ClickIndex = tag
                Packet_Cmd(GameServer_pb.CMD_TBT_GETUUID)
                ShowWaiting()
            elseif getChannelID() == "LJ" then
                g_ClickIndex = tag
                RechargeSystem_LjChargeFunction()
            elseif getChannelID() == "VIVO" then
                local tab = GameServer_pb.CMD_VIVO_BUY_GOOD_CS();
                tab.iPrice = money
                Packet_Full(GameServer_pb.CMD_VIVO_BUY_GOOD, tab);

                ShowWaiting()
            end
        end
      end 
   end
   local function ClickTemplayoutRechageSystemAndMonthCard(sender,eventType)
       if eventType == TOUCH_EVENT_ENDED then  
          local sender = tolua.cast(sender,"Layout")
          local tag = sender:getTag()
          local money = RechargeSystem_RechargeGetMoney(tag);
		  l_money = money;
          Log("tag ==="..tag)
          Log("Money =="..money)
		  g_YueKaMoney = money
          g_ClickIndex = tag
          Packet_Cmd(GameServer_pb.CMD_CANBUY_MONTHCARD)
       end 
   end
   for i = 1,#tagtable do 
      local row = i % 2 
      if row == 1 then 
         curlayout = nil 
         curlayout = Layout:create()
         curlayout:setSize(CCSizeMake(listwidth,UI_GetUILayout(widget, 8):getContentSize().height))        
      end 
      templayout = UI_GetCloneLayout(UI_GetUILayout(widget,tagtable[i]))
      templayout:setVisible(true)
      curlayout:addChild(templayout,i,i)
      templayout:setPosition(ccp((1 - row) * listView:getContentSize().width / 2,0 ))
      if row == 0 or i == #tagtable then 
         listView:pushBackCustomItem(curlayout)
      end
      if tagtable[i] ~= 14 then 
            templayout:setTouchEnabled(true)
            templayout:addTouchEventListener(ClickTemplayoutRechageSystem)
            --价值RMB
            local R_M_B = RechargeSystem_RechargeGetMoney(i)
            --价值元宝
            UI_GetUILabel(templayout, 1001):setText(FormatString("Pay_GetGold",R_M_B))
            --价值RMB
            UI_GetUILabel(templayout, 1002):setText(FormatString("Pay_RMB",R_M_B/10))
            --双倍
            if RechargeSystem_FindIsDouble(RechargeSystem_RechargeGetPayID(i),info) then
                --描述
                UI_GetUILabel(templayout, 1003):setText(RechargeSystem_RechargeGetDes(i,1))
                UI_GetUIImageView(templayout, 4001):setVisible(true)
            elseif RechargeSystem_RechargeIsRecommend(i) then -- 推荐
                --描述
                UI_GetUILabel(templayout, 1003):setText(RechargeSystem_RechargeGetDes(i,2))
                UI_GetUIImageView(templayout, 4002):setVisible(true)
            else --一般
                --描述
                UI_GetUILabel(templayout, 1003):setText(RechargeSystem_RechargeGetDes(i,2))
            end
      else 
           --价值元宝
           local R_M_B = RechargeSystem_RechargeGetMoney(i)
           UI_GetUILabel(templayout, 1001):setText(FormatString("Pay_GetGoldAndYueka",R_M_B + 20))
           --价值RMB
           UI_GetUILabel(templayout, 1002):setText(FormatString("Pay_RMB",R_M_B/10))
           if info.iMonthCardRemaind == 0 then 
              UI_GetUILabel(templayout, 1003):setText(RechargeSystem_RechargeGetDes(i,1))
           else 
              UI_GetUILabel(templayout, 1003):setText(RechargeSystem_RechargeGetDes(i,2)..FormatString("Pay_GetGoldAndYuekaLeft",info.iMonthCardRemaind))
           end 
           templayout:setTouchEnabled(true)
           templayout:addTouchEventListener(ClickTemplayoutRechageSystemAndMonthCard)
      end 
   end 
end

function  RechargeSystem_TbtChargeFunction(info)
  -- body
   if g_ClickIndex ~= 0 then 
      local money = RechargeSystem_RechargeGetMoney(g_ClickIndex)
      money = money / 10
      Log("money===="..money)
      local pro = g_RecordUserData.ServerId
      local userId = TBTSDK:GetInstance():TBUserID()
      local protr = pro.."_"..userId
      Log("userId===="..protr)
      TBTSDK:GetInstance():TBUnipayForCoinWithOrder(info.strUUId,money,protr)
   end 
end

function RechargeSystem_UCChargeFunction(info)
   if g_ClickIndex ~= 0 then 
      local userId = TBTSDK:GetInstance():getSdkUid()
	  Log("payUserId ==="..userId)
	  local money = RechargeSystem_RechargeGetMoney(g_ClickIndex) / 10
	  local pro = g_RecordUserData.ServerId
	  local url = "http://115.159.2.206:9003"
	  --local url = "http://114.215.181.2:9003"
	  cpaySdk("123", g_ReChargeSystem_Info[g_ClickIndex], money, info.strUUId, userId, url,pro);
   end 
end 

function RechargeSystem_JSFuckCHargeFunction(info)
   if g_ClickIndex ~= 0 then
      local userId = TBTSDK:GetInstance().s_channelSdkInfo.UserId
	  Log("payUserId ==="..userId)
	  local money = RechargeSystem_RechargeGetMoney(g_ClickIndex) / 10
	  local pro = g_RecordUserData.ServerId.."_"..userId
      g_JSFUCK_PricePro = pro 
	  --local url = "http://115.159.2.206:9008"
	  local url = "http://114.215.181.2:9008"
	  cpaySdk("123", g_ReChargeSystem_Info[g_ClickIndex], money, info.strUUId, userId, url,pro);
   end
end 

function RechargeSystem_LjChargeFunction()
   if g_ClickIndex ~= 0 then  
      local url = "http://115.159.2.206:9005"
      local userId = g_RecordUserData.ServerId.."_"..TBTSDK:GetInstance().s_ljsdkLoginInfo.userId
      local pro = g_RecordUserData.ServerId
      local money = RechargeSystem_RechargeGetMoney(g_ClickIndex) * 10
      local count = RechargeSystem_RechargeGetMoney(g_ClickIndex)
      TBTSDK:GetInstance():LjSDKPay(money,count,g_ReChargeSystem_Info[g_ClickIndex],userId,url)
   end 
end
---------
function RechargeSystem_FindIsDouble(id,info)
    if info.iId == nil or info.iId == {} then
        return true
    end
    for i=1,#info.iId,1 do
        if info.iId[i] == id then
            return false
        end
    end
    return true
end
--
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_TBT_GETUUID, "GetTBTPro" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_PAYMENT_QUERY, "RechargeSystem_InitData" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_CANBUY_MONTHCARD, "RechargeSystem_CanBuyYueCard" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_VIVO_BUY_GOOD, "CMD_VIVO_BUY_GOOD" )
function GetTBTPro(pkg)
    EndWaiting()
    local info = GameServer_pb.CMD_TBT_GETUUID_SC()
    info:ParseFromString(pkg)
    Log("info==========="..tostring(info))
	if getChannelID() == "TBT" then 
       RechargeSystem_TbtChargeFunction(info)
	elseif getChannelID() == "UC" then 
	   RechargeSystem_UCChargeFunction(info)
    elseif getChannelID() == "JSFUCK" then 
       RechargeSystem_JSFuckCHargeFunction(info)
	end 
end

function RechargeSystem_CanBuyYueCard(pkg)
    local money = RechargeSystem_RechargeGetMoney(g_ClickIndex)
    Log("tag ==="..g_ClickIndex)
	
	local osID = getOSID();
        Log(">>>>>>> osID:"..osID);
        if osID == "IOS" then
            local productID = __iosGetProductID(g_ClickIndex);
            if productID ~= nil then
                Log("purchase product:"..productID.." x 1");
                ShowWaiting();
                IOSSDK:purchseProductRequest(productID, 1);
            end
        else
			if getChannelID() == "TBT" then
			   Packet_Cmd(GameServer_pb.CMD_TBT_GETUUID)
			   ShowWaiting()
			elseif getChannelID() == "QQYYB" then
			   startSaveMoney(money)
			elseif getChannelID() == "UC" then 
			   Packet_Cmd(GameServer_pb.CMD_TBT_GETUUID)
			   ShowWaiting()
			elseif getChannelID() == "LJ" then 
			   RechargeSystem_LjChargeFunction()
			elseif getChannelID() == "JSFUCK" then 
			   Packet_Cmd(GameServer_pb.CMD_TBT_GETUUID)
			   ShowWaiting()
			elseif getChannelID() == "VIVO" then
				local tab = GameServer_pb.CMD_VIVO_BUY_GOOD_CS();
				tab.iPrice = g_YueKaMoney
				Packet_Full(GameServer_pb.CMD_VIVO_BUY_GOOD, tab);

				ShowWaiting()	
			end
	end
end

function CMD_VIVO_BUY_GOOD(pkg)
    EndWaiting()
    local info = GameServer_pb.CMD_VIVO_BUY_GOOD_SC()
    info:ParseFromString(pkg)
    Log("info==========="..tostring(info))
	
	local productName = FormatString("en_LifeAtt_Gold")
	local productDes = FormatString("Pay_GetGold_",info.iPrice/100)
	local blance = FormatString("Pay_GetGold_",info.iPrice/100)
	local party = GetLocalPlayer():GetString(EPlayer_LegionName)
	if party == "" or party == nil then
		party="----"
	end
	if info.iPrice/100 == 28 then
		productDes = FormatString("Pay_GetGoldYueKa_",info.iPrice/100)
		blance = FormatString("Pay_GetGoldYueKa_",info.iPrice/100)
	end
	TBTSDK:GetInstance():VIVOSDKPayDataToJava(info.orderNumber,info.accessKey,productName,productDes,info.iPrice,"",
		blance,"vip"..GetLocalPlayer():GetInt(EPlayer_VipLevel),GetLocalPlayer():GetInt(EPlayer_Lvl),party,
		GetLocalPlayer():GetEntityName(),GetLocalPlayer():GetEntityName(),g_RecordUserData.ServerName,
		"",false)
end