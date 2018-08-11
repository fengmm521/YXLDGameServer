function Activity_Create() 
    Activity_init()
end

function Activity_init()
    local widget = UI_GetBaseWidgetByName("Activity")
    if widget == nil then
        widget = UI_CreateBaseWidgetByFileName("Activity.json")
    end
    UI_GetUILayout(widget, 1301):addTouchEventListener(PublicCallBackWithNothingToDo)
    UI_GetUIButton(widget, 1302):addTouchEventListener(UI_ClickCloseCurBaseWidgetWithNoAction)
    Activity_AddActivityButton()
    Activity_ResetButtonState(1)
    Activity_ResetActivityInfoState(1)
    Activity_InitActivityData(1)
    Activity_RefreshTopData()
	Log(">>>>>>>>>>>>>>>>>>>>>>>>>>> Activity_init()");
end
--添加活动列表
function Activity_AddActivityButton()
    local widget = UI_GetBaseWidgetByName("Activity")

    local function initButton(layout,index)
        local data = GetGameData(DataFileOperateActive, l_ActivityKindInfo.szOpenActive[index], "stOperateActiveData")
        --巨划算、超值
        UI_GetUIImageView(layout, 2004):loadTexture("huodong/mark_02.png")
        --活动图标
        UI_GetUIImageView(layout, 2002):loadTexture("huodong/"..data.m_activityIcon..".png")
        --活动名称
        UI_GetUILabel(layout, 2003):setText(data.m_activityName)
        --响应
        local button = UI_GetUIButton(layout, 2001)
        button:setTag(index)
        button:setName("button")
        button:addTouchEventListener(Activity_FindActivityInfo)
    end
    --放置活动按钮
    local listView = UI_GetUIListView(widget, 1004)
    g_ActivityButtons = {}
    listView:removeAllItems();
    for i=1,#l_ActivityKindInfo.szOpenActive do
        local buttouLayout = UI_GetUILayout(widget, 1006):clone()
        buttouLayout:setVisible(true)
        initButton(buttouLayout,i)
        listView:pushBackCustomItem(buttouLayout)
        g_ActivityButtons[i] = buttouLayout
    end
end
--查询活动数据
function Activity_FindActivityInfo(sender,eventType)
    if eventType == TOUCH_EVENT_ENDED then
        local button = tolua.cast(sender,"Button")
--        Activity_ResetButtonState(button:getTag())
--        Activity_ResetActivityInfoState(button:getTag())
--        Activity_InitActivityData(button:getTag())
         if button:getTag() == 1 then 
            --成长计划
            Packet_Cmd(GameServer_pb.CMD_QUERY_GROWUP_DETAIL)
            ShowWaiting()
         elseif button:getTag() == 2 then 
            --累计充值
            Packet_Cmd(GameServer_pb.CMD_QUERY_ACCOUNT_PAYMENT_DETAIL)
            ShowWaiting()
         end 
    end
end
--初始化活动信息
function Activity_InitActivityData(info)
    local widget = UI_GetBaseWidgetByName("Activity")
    local rightImage = UI_GetUIImageView(widget, 1005)
    
    --活动标题
    UI_GetUILabel(rightImage, 2001):setText("11-11-11-11")
    --活动时间
    UI_GetUILabel(rightImage, 2002):setText("2015-3-5~2015-4-5")
    if info == 1 then
        Activity_initDataType_1()
    elseif info == 2 then
        Activity_initDataType_2()
    elseif info == 3 then
        Activity_initDataType_3()
    end
end
--初始化成长计划
function Activity_initDataType_1()
    local widget = UI_GetBaseWidgetByName("Activity")
    local image = UI_GetUIImageView(widget, 1101)
    --活动中的任务
    local task = UI_GetUILayout(widget, 11011)
    --任务列表
    local listView = UI_GetUIListView(image, 2001)
    listView:removeAllItems();

    for i=1,5,1 do
        local newTask = task:clone()
        newTask:setVisible(true)
        Activity_initTaskData(newTask,i,data,Activity_GetTaskAward,Activity_ToDoTaskType_1)
        listView:pushBackCustomItem(newTask)
    end
end
--初始公告类活动
function Activity_initDataType_2()
    local widget = UI_GetBaseWidgetByName("Activity")
    local image = UI_GetUIImageView(widget, 1102)
    local baseLabel = UI_GetUILabel(image, 2002)
    baseLabel:setVisible(false)
    local listView = UI_GetUIListView(image, 2003)
    listView:removeAllItems()

    local str = GetCompLabelStrByLabel(baseLabel,"test test test test tetest test test test test test test test test test test test test test test test test test ")

    local messageLabel = CompLabel:GetDefaultCompLabel(str,listView:getSize().width)
    listView:pushBackCustomItem(messageLabel)
end
--初始化VIP等级任务
function Activity_initDataType_3()
    local widget = UI_GetBaseWidgetByName("Activity")
    local image = UI_GetUIImageView(widget, 1103)
    --活动中的任务
    local task = UI_GetUILayout(widget, 11011)
    local listView = UI_GetUIListView(image, 2001)
    listView:removeAllItems();

    for i=1,5,1 do
        local newTask = task:clone()
        newTask:setVisible(true)
        Activity_initTaskData(newTask,i,data,Activity_GetTaskAward,Activity_ToDoTaskType_1)
        listView:pushBackCustomItem(newTask)
    end
    --购买功能
    local function buy(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            local function BuyGrowthPlan(sender ,eventType)
                local function ToPay(sender,eventType)      
                    RechargeSystem_ShowLayout()
                end
                if GetLocalPlayer():GetInt(EPlayer_VipLevel) <3 then
                    local tab = {}
                    tab.info = FormatString("Activity_VIPNotEnough")
                    tab.msgType = EMessageType_LeftRight
                    tab.leftFun = ToPay
                    Messagebox_Create(tab)
                elseif GetLocalPlayer():GetInt(EPlayer_Gold) < 2000 then
                    local tab = {}
                    tab.info = FormatString("Activity_YuanbaoNotEnough")
                    tab.msgType = EMessageType_LeftRight
                    tab.leftFun = ToPay
                    Messagebox_Create(tab)
                else
                    print("BuyGrowthPlan")
                end
            end
            local tab = {}
            tab.info = FormatString("Activity_BuyGrowthPlan")
            tab.msgType = EMessageType_LeftRight
            tab.leftFun = BuyGrowthPlan
            Messagebox_Create(tab)
        end 
    end
    UI_GetUIButton(image, 2003):addTouchEventListener(buy)
    --购买的Vip等级限制
    UI_GetUIImageView(image, 2002):loadTexture("VIP/frot_VIP_004.png")
    if (UI_GetUIButton(image, 2002):getNodeByTag(1) == nil) then
		local armature = GetUIArmature("Effvip")
		armature:getAnimation():playWithIndex(0)
		UI_GetUIButton(image, 2002):addNode(armature, 1, 1);
	end
end
--初始化活动任务
function Activity_initTaskData(layout,taskID,taskDataFromServer,ComCallBack,NotComCallBack)
    UI_GetUILabel(layout, 2002):enableStroke()
    --任务要求
    UI_GetUILabel(layout, 2001):setText("1-1-1")
    --任务进度
    UI_GetUILabel(layout, 2002):setText("2-2-2")
    --奖励
    for i=1,6,1 do
        local award = UI_GetUIImageView(layout, 2100+i)
        local function showAwardInfo(sender,eventType)
            if eventType == TOUCH_EVENT_ENDED then
                local image = tolua.cast(sender,"ImageView")
                print(image:getWorldPosition().x.."----"..image:getWorldPosition().y)
                AddItemTip(4008,200-image:getWorldPosition().y)

            end
        end
        if i<=3 then
            award:setVisible(true)
            local icon = UI_ItemIconFrame(UI_GetItemIcon(4008, -1), 3)
            award:addChild(icon)
            icon:setTouchEnabled(true)
            icon:addTouchEventListener(showAwardInfo)
        else
            award:setVisible(false)
        end
    end

    --领取或者去完成
    local function callBack(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            print("TaskCallBack")
        end
    end 
    UI_GetUIButton(layout, 2003):addTouchEventListener(callBack)
end
--充值类任务
function Activity_ToDoTaskType_1(taskID)
    
end
--完成任务
function Activity_GetTaskAward(taskID)
    
end
--设置左边列表中的按钮状态
function Activity_ResetButtonState(id)
    local widget = UI_GetBaseWidgetByName("Activity")
    Log("id===="..id)
    for i=1,#g_ActivityButtons,1 do
        local button = tolua.cast(g_ActivityButtons[i]:getChildByName("button"),"Button")
        local tag = button:getTag()
        print(tag.."--------------------"..id)
        if tag == id then
            button:setButtonEnabled(false)
        else
            button:setButtonEnabled(true)
        end
    end
end
--设置右边活动显示处的状态，确定应该显示何种模板
function Activity_ResetActivityInfoState(id)
    local widget = UI_GetBaseWidgetByName("Activity")
    for i=1,3,1 do
        if id == i then
            UI_GetUIImageView(widget, 1100+i):setVisible(true)
        else
            UI_GetUIImageView(widget, 1100+i):setVisible(false)
        end
    end
end

function Activity_RefreshTopData()
    local widget = UI_GetBaseWidgetByName("Activity")
    local function dealWithData(num)
        if num>100000000 then
            return FormatString("TenThousand",num/10000) 
        end
        return num 
    end
    --铜币
    UI_GetUILabelBMFont(widget:getChildByTag(1001),2001):setText(dealWithData(GetLocalPlayer():GetInt(EPlayer_Silver)))
    --元宝
    UI_GetUILabelBMFont(widget:getChildByTag(1003),2001):setText(dealWithData(GetLocalPlayer():GetInt(EPlayer_Gold)))
    --体力
    UI_GetUILabelBMFont(widget:getChildByTag(1002),2001):setText(GetLocalPlayer():GetInt(EPlayer_Tili).."/"..GetLocalPlayer():GetInt(EPlayer_TiliMax))
end



-- 定义一些局部结构
local l_ActivityKindSize = 0
local l_ActivityOpenIndex = {};
if g_ActivityInfo == nil then
   g_ActivityInfo = {}
end  

if g_ActivityButtons == nil then 
   g_ActivityButtons = {}
end 


ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_QUERY_OPERATE_ACTIVE, "Activity_LayoutInfo" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_QUERY_GROWUP_DETAIL, "Activity_GrowupDetail" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_QUERY_ACCOUNT_PAYMENT_DETAIL, "Activity_PayMentDetail_SC" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_GET_ACCOUNT_PAYMENT_REWARD, "Activity_GetPayMentReward_SC" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_OPERATE_TELPHONE_STREAM, "Activity_Telephone_SC" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_QUERY_ACCOUNT_LOGIN_IN, "Activity_7Days_SC" );
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_GET_ACCOUNT_REWARD, "Activity_7DaysReward_SC");

-- 七日登录信息 服务器端响应
function Activity_7Days_SC(pkg)
   EndWaiting();

   local info = GameServer_pb.CMD_QUERY_ACCOUNT_LOGIN_IN_SC();
   info:ParseFromString(pkg);

   -- MOCK
   -- info.iAccLoginCount = 3;

   Log("Activity_7Days_SC START:-----------------");
   Log(tostring(info));
   Log("Activuty_7Days_SC END -------------------");

   local widget = UI_GetBaseWidgetByName("Activity") 
   if widget then 
      --refresh
      Activity_InitValue(info)
      Activity_ResetButtonState(4)
      Activity_7DaysDetailShow(widget);
   else 
      --create
      Activity_InitValue(info)
      Activity_LayoutCreate(true)
   end 
end

function Activity_Telephone_SC(pkg)
   EndWaiting()
   --UI_CloseCurBaseWidget(EUICloseAction_None)
   Packet_Cmd(GameServer_pb.CMD_QUERY_OPERATE_ACTIVE)
   ShowWaiting()
end

function Activity_LayoutInfo(pkg)
   EndWaiting()
   local info = GameServer_pb.CMD_QUERY_OPERATE_ACTIVE_SC()
   info:ParseFromString(pkg)
   local widget = UI_GetBaseWidgetByName("Activity") 
   Log("===============Activity_LayoutInfo Start====================");
   Log(tostring(info));
   Log("===============Activity_LayoutInfo End====================");
   l_ActivityKindSize = #info.szOpenActive;
   --Activity_Create(info) 
   local isLookGrowUpdate = false 
   for i = 1 , #info.szOpenActive do 
      l_ActivityOpenIndex[i] = info.szOpenActive[i];
      if info.szOpenActive[i] == 1 then 
	     isLookGrowUpdate = true
		 break
	  end 
   end 
   if isLookGrowUpdate then 
      if widget then
         Activity_ListViewInit(widget) 
         LookGrowUpDetail_CS()
      else 
         LookGrowUpDetail_CS()
      end 
   else 
      if widget then 
         Activity_ListViewInit(widget)
         LookPayMentDetail()
      else 
         LookPayMentDetail()
      end 
   end 
end

function Activity_GrowupDetail(pkg)
   EndWaiting()
   local info = GameServer_pb.CMD_QUERY_GROWUP_DETAIL_SC()
   info:ParseFromString(pkg)
   Log("Activity_GrowupDetail===="..tostring(info))
   local widget = UI_GetBaseWidgetByName("Activity") 
   if widget then 
      --refresh
      Log("refresh")
      Activity_InitValue(info)
      Activity_ResetButtonState(1)
      Activity_GrowUpDetailShow(widget)
   else 
      --create
      Activity_InitValue(info)
      Activity_LayoutCreate(true)
   end 
end

function Activity_PayMentDetail_SC(pkg)
   EndWaiting()
   local info = GameServer_pb.CMD_QUERY_ACCOUNT_PAYMENT_DETAIL_SC()
   info:ParseFromString(pkg)
   Log("Activity_PayMentDetail_SC====="..tostring(info))
   local widget = UI_GetBaseWidgetByName("Activity")
   if widget then 
	  Log("----> Set already existed");
      Activity_InitValue(info)
      Activity_PayMentDetailShowlayout(widget)
      Activity_ResetButtonState(2)
   else 
      --create 
	  Log("----> Create new activity.");
	  Activity_InitValue(info);
	  Log("----> Before layout create");
      Activity_LayoutCreate(false)
      Activity_ResetButtonState(2)
   end 
   Log(">>>>>>>>>>>>>>>>>>>>>>>>> KKND");
end

function Activity_GetPayMentReward_SC(pkg)
   EndWaiting()
   local info = GameServer_pb.CMD_GET_ACCOUNT_PAYMENT_REWARD_SC()
   info:ParseFromString(pkg)
   --Log("Activity_GetPayMentReward_SC====="..tostring(info))
   Activity_GetPayMentReward_ShowLayout(info)
   LookPayMentDetail()
end

function Activity_7DaysReward_SC(pkg)
   EndWaiting();
   local info = GameServer_pb.CMD_GET_ACCOUNT_PAYMENT_REWARD_SC();
   info:ParseFromString(pkg);
   Log("Activity_7DaysReward_SC====="..tostring(info));
   Activity_GetPayMentReward_ShowLayout(info);
   Look7DaysDeatil_CS();
end

function Activity_GetPayMentReward_ShowLayout(info)
   local awardPropList = info.reward.szAwardPropList
   local awardItemList = info.reward.szAwardItemList
   local awardHeroList = info.reward.szAwardHeroList
   --lifeatt
   --item
   --hero
   for i = 1, #awardHeroList do 
      Shop_CallHero(awardHeroList[i].iHeroID ,0,false)
   end 
end

function Activity_InitValue(info)
   g_ActivityInfo = info 
end

function Activity_LayoutCreate(isGrowUp)
   Log("Activity_LayoutCreate: before create widget.");
   local widget = UI_CreateBaseWidgetByFileName("Activity.json");
   Log("Activity_LayoutCreate: after create widget.");
   UI_GetUILayout(widget, 1301):addTouchEventListener(PublicCallBackWithNothingToDo)
   UI_GetUIButton(widget, 1302):addTouchEventListener(UI_ClickCloseCurBaseWidgetWithNoAction)
   --默认是1
   Activity_ListViewInit(widget)
   --Activity_ChangeButtonState(widget,1)
   if isGrowUp then 
		Activity_ResetButtonState(1)
		Activity_GrowUpDetailShow(widget)
   else 
        Activity_ResetButtonState(1)
		Activity_PayMentDetailShowlayout(widget)
   end 
end

function Activity_ListViewInit(widget) 
   Log("Activity_ListViewInit invoked.");
   local listview = UI_GetUIListView(widget,1004)
   listview:removeAllItems()
   local function initButton(layout,index)
      local data = GetGameData(DataFileOperateActive, index, "stOperateActiveData")
      Log("-------------------------------DATA START----------------------------------");
      Log("-- data.m_activityUpIcon:"..data.m_activityUpIcon);
      Log("-- data.m_activityName:"..data.m_activityName);
      Log("-------------------------------DATA END------------------------------------");
      --巨划算、超值
      if data.m_activityUpIcon == "" then 
         UI_GetUIImageView(layout, 2004):setVisible(false)
      else 
         UI_GetUIImageView(layout, 2004):setVisible(true)
         UI_GetUIImageView(layout, 2004):loadTexture("huodong/"..data.m_activityUpIcon..".png")
      end 
      --活动图标
      UI_GetUIImageView(layout, 2002):loadTexture("huodong/"..data.m_activityIcon..".png")
      --活动名称
      UI_GetUILabel(layout, 2003):setText(data.m_activityName)
      --响应
      local function clickActivityButton(sender,eventType)
         if eventType == TOUCH_EVENT_ENDED then 
             local sender = tolua.cast(sender,"Button")
             -- local tag = listview:getIndex(sender:getParent())
             -- Activity_LookOtherActivity(tag + 1)
             local tag = sender:getTag();
             Activity_LookOtherActivity(tag);
         end 
      end
      local button = UI_GetUIButton(layout, 2001)
      button:setTag(index)
      button:setName("button")
      button:addTouchEventListener(clickActivityButton)
    end
    --放置活动按钮
    Log("l_ActivityKindSize===="..l_ActivityKindSize)
    g_ActivityButtons = {}

    for i=1, #l_ActivityOpenIndex do
        local buttouLayout = UI_GetUILayout(widget, 1006):clone()
        buttouLayout:setVisible(true)
        initButton(buttouLayout,l_ActivityOpenIndex[i]);
        listview:pushBackCustomItem(buttouLayout)
        g_ActivityButtons[i] = buttouLayout
    end
    --[[
    for i=1, l_ActivityKindSize do
        local buttouLayout = UI_GetUILayout(widget, 1006):clone()
        buttouLayout:setVisible(true)
        initButton(buttouLayout,i)
        listview:pushBackCustomItem(buttouLayout)
        g_ActivityButtons[i] = buttouLayout
    end
    ]]
end

--查看活动
function Activity_LookOtherActivity(tag)
   if tag == 1 then 
      --成长计划
      LookGrowUpDetail_CS()
   elseif tag == 2 then 
      --累计充值
      LookPayMentDetail()
   elseif tag == 3 then 
      --liuliang 
      local widget = UI_GetBaseWidgetByName("Activity")
      if widget then 
         Activity_ResetButtonState(3)
         Activit_TelphoneStream_Show(widget)
      end 
   elseif tag == 4 then
      --七日登录
      Look7DaysDeatil_CS();
   end 
end

--liuliang 
function Activit_TelphoneStream_Show(widget)
   Activity_ActivityLayoutChange(widget,3)
   local imageparent = UI_GetUIImageView(widget,500)
   
   --tile
   local data = GetGameData(DataFileOperateActive, 3, "stOperateActiveData")
   UI_GetUILabel(UI_GetUIImageView(widget,1005),2001):setText(data.m_activityName)
   UI_GetUILabel(UI_GetUIImageView(widget,1005),2002):setText(data.m_activityDes)

   if data.m_activityType == 1 then 
      UI_GetUILabel(UI_GetUIImageView(widget,1005),2005):setVisible(true)
      Activit_InfoButtonFunc(widget,1)
   else 
      UI_GetUILabel(UI_GetUIImageView(widget,1005),2005):setVisible(false)
   end 

   --info
   UI_SetLabelText(imageparent,1,data.m_activityInfo)

   -- telep
   local telimage = UI_GetUIImageView(imageparent,2)
   local textfield = UI_GetUITextField(UI_GetUIImageView(telimage, 1), 1)
   textfield:setTouchEnabled(false)
   --textediet 
   local function enter(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            textfield:setTouchEnabled(true)
            textfield:setText("")
            textfield:attachWithIME()
        end
    end
    UI_GetUIImageView(telimage, 1):setTouchEnabled(true)
    UI_GetUIImageView(telimage, 1):addTouchEventListener(enter)

    --btn
    local function clicksend(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then 
           local telephonenumbe = textfield:getStringValue()
           Log("telephonenumbe===="..telephonenumbe)
           local iscurnumber = PhoneNumber_IsSureNumber(telephonenumbe)
           local function toGet()
              Log("telephonenumbe===="..telephonenumbe)
              local telnumber = PhoneNumber_GetFuckNumber(telephonenumbe)
              Log("telnumber===="..telnumber)
              local tab = GameServer_pb.CMD_OPERATE_TELPHONE_STREAM_CS()
              tab.strTelPhoneNum = telnumber
              Packet_Full(GameServer_pb.CMD_OPERATE_TELPHONE_STREAM,tab)
              ShowWaiting()
           end
           if iscurnumber then 
              if GetLocalPlayer():GetInt(EPlayer_Lvl) >= 35 then 
                  local tab = {}
                  tab.info = FormatString("getliuliangNumber",telephonenumbe)
                  tab.msgType = EMessageType_LeftRight
                  tab.leftFun = toGet
                  Messagebox_Create(tab)
              else 
                  createPromptBoxlayout(FormatString("NoCurTelepNumber_limit"))
              end 
           else
              createPromptBoxlayout(FormatString("NoCurTelepNumber"))
           end 
        end 
    end
    UI_GetUIButton(telimage,2):addTouchEventListener(clicksend)
end

--7日登录
function Activity_7DaysDetailShow(widget)
    -- g_ActivityInfo
    Activity_ActivityLayoutChange(widget, 4);

    -- 设置title和des
    local data = GetGameData(DataFileOperateActive, 4, "stOperateActiveData");
    UI_GetUILabel(UI_GetUIImageView(widget,1005),2001):setText(data.m_activityName);
    UI_GetUILabel(UI_GetUIImageView(widget,1005),2002):setText(data.m_activityDes);

    -- 详情
    if data.m_activityType == 1 then 
      UI_GetUILabel(UI_GetUIImageView(widget,1005),2005):setVisible(true);
      Activit_InfoButtonFunc(widget,4);
   else 
      UI_GetUILabel(UI_GetUIImageView(widget,1005),2005):setVisible(false);
   end 

   -- 清空列表
   local parentimage = UI_GetUIImageView(widget,1101)
   local listview = UI_GetUIListView(parentimage,2001)
   listview:removeAllItems()
   local layout = nil 

   local function initLayout(layout,index)
      local day = g_ActivityInfo.szAccount[index];
      local loginCount = g_ActivityInfo.iAccLoginCount;
      -- 显示title
      UI_GetUILabel(layout,2001):setText(FormatString("Activity_7Days",day));

      --reward
      local imagetag = {2101,2102,2103,2104,2105,2106}
      local awardData = GetGameData(DataFileAccumlateLogin, day,"stAccumlateLoginData")
      local awardPayMentTable = {awardData.m_RewardOne,awardData.m_RewardTwo,awardData.m_RewardTree,awardData.m_RewardFour}
      --Log("====================>RewardFour:"..awardData.m_RewardFour);
      local rewardsize = 4
      for i = 1,#imagetag do 
          local imageview = UI_GetUIImageView(layout,imagetag[i])
          if i <= rewardsize then 
             imageview:setVisible(true)
          else 
             imageview:setVisible(false)
          end 
      end
      local function showAwardInfo(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            local image = tolua.cast(sender,"ImageView")
            AddItemTip(tolua.cast(sender,"ImageView"):getTag(),200-image:getWorldPosition().y)
        end
      end
      for i = 1,#awardPayMentTable do
         local awardtable = Activity_getStrTwo(awardPayMentTable[i])
         local awardType = awardtable[1] 
         local awardId = TypeConvert:ToInt(awardtable[2])
         local awardNum = TypeConvert:ToInt(awardtable[3])

         if i == 4 then
         Log("awardType:"..awardType.." awardId:"..awardId.."awardNum:"..awardNum);
         end

         if awardType == "LifeAtt" then
            local icon = UI_ItemIconFrame(UI_GetLifeAtt(awardId, awardNum), 3)
            UI_GetUIImageView(layout,imagetag[i]):addChild(icon,1,awardId);
         elseif awardType == "Item" then 
            local Itemdata = GetGameData(DataFileItem,awardId,"stItemData")
            local icon = UI_ItemIconFrame(UI_GetItemIcon(Itemdata.m_icon, awardNum), 3)
            icon:setTouchEnabled(true)
            icon:addTouchEventListener(showAwardInfo)
            UI_GetUIImageView(layout,imagetag[i]):addChild(icon,1,awardId);
         elseif awardType == "Hero" then 
            local herodata = GetGameData(DataFileHeroBorn, awardId , "stHeroBornData");
            local icon = UI_IconFrame(UI_GetHeroIcon(awardId), herodata.m_heroInitStar);
            --local icon = UI_ItemIconFrame(UI_GetHeroIcon(herodata.m_icon,1), 2);
            UI_GetUIImageView(layout,imagetag[i]):addChild(icon,1);
         elseif awardType == "Equip" then
            local equipData = GetGameData(DataFileEquip, awardId, "stEquipData");
            local icon = UI_ItemIconFrame(UI_GetEquipIcon(equipData.m_icon), equipData.m_quality);
            UI_GetUIImageView(layout,imagetag[i]):addChild(icon,1,awardId);

            local function showData(sender,eventType)
                if eventType == TOUCH_EVENT_ENDED then
                    AddEquipTip(awardId,50)

                end
            end

            icon:setTouchEnabled(true)
            icon:addTouchEventListener(showData)
         end 
      end 

      --button
      local function clickGetReward(sender,eventType)
         if eventType == TOUCH_EVENT_ENDED then 
            local tab = GameServer_pb.CMD_GET_ACCOUNT_REWARD_CS();
            tab.iAccountTimes = day;
            Packet_Full(GameServer_pb.CMD_GET_ACCOUNT_REWARD,tab);
            ShowWaiting();
         end 
      end 
      local btn = UI_GetUIButton(layout,2002);
      if loginCount >= day then
        --btn:setButtonEnabled(true);
        btn:setTouchEnabled(true);
        btn:addTouchEventListener(clickGetReward);
      else
        btn:setTouchEnabled(false);
      end

      --[[
      UI_GetUILabel(layout,2001):setText(FormatString("Activity_Paymenttotal",g_ActivityInfo.szAccountList[index].iGoal))
      --lvl 
      UI_GetUILabel(layout,2002):setText(g_ActivityInfo.iActorAccPayment.."/"..g_ActivityInfo.szAccountList[index].iGoal)
      --reward
      local imagetag = {2101,2102,2103,2104,2105,2106}
      local awardData = GetGameData(DataFileAccumulatePayMent,g_ActivityInfo.szAccountList[index].iId,"stAccumulatePaymentData")
      local awardPayMentTable = {awardData.m_RewardOne,awardData.m_RewardTwo,awardData.m_RewardTree}
      local rewardsize = 3
      for i = 1,#imagetag do 
          local imageview = UI_GetUIImageView(layout,imagetag[i])
          if i <= rewardsize then 
             imageview:setVisible(true)
          else 
             imageview:setVisible(false)
          end 
      end
      local function showAwardInfo(sender,eventType)
            if eventType == TOUCH_EVENT_ENDED then
                local image = tolua.cast(sender,"ImageView")
                AddItemTip(tolua.cast(sender,"ImageView"):getTag(),200-image:getWorldPosition().y)
            end
        end
      for i = 1,#awardPayMentTable do 
         local awardtable = Activity_getStrTwo(awardPayMentTable[i])
         local awardType = awardtable[1] 
         local awardId = TypeConvert:ToInt(awardtable[2])
         local awardNum = TypeConvert:ToInt(awardtable[3])
         if awardType == "LifeAtt" then
            local icon = UI_ItemIconFrame(UI_GetLifeAtt(awardId, awardNum), 3)
            UI_GetUIImageView(layout,imagetag[i]):addChild(icon,1,awardId);
         elseif awardType == "Item" then 
            local Itemdata = GetGameData(DataFileItem,awardId,"stItemData")
            local icon = UI_ItemIconFrame(UI_GetItemIcon(Itemdata.m_icon, awardNum), 3)
            icon:setTouchEnabled(true)
            icon:addTouchEventListener(showAwardInfo)
            UI_GetUIImageView(layout,imagetag[i]):addChild(icon,1,awardId);
         elseif awardType == "Hero" then 
            local herodata = GetGameData(DataFileHeroBorn, awardId , "stHeroBornData")
            local icon = UI_ItemIconFrame(UI_GetHeroIcon(herodata.m_icon,1), 2)
            UI_GetUIImageView(layout,imagetag[i]):addChild(icon,1);
         end 
      end 
      --button
      local function clickGetReward(sender,eventType)
         if eventType == TOUCH_EVENT_ENDED then 
            local tab = GameServer_pb.CMD_GET_GROWUP_REWARD_CS()
            tab.iId = g_ActivityInfo.szAccountList[index].iId
            Packet_Full(GameServer_pb.CMD_GET_ACCOUNT_PAYMENT_REWARD,tab)
            ShowWaiting()
         end 
      end 
      if g_ActivityInfo.iActorAccPayment < g_ActivityInfo.szAccountList[index].iGoal then
         UI_GetUIButton(layout,2003):setTouchEnabled(false)
      else 
         UI_GetUIButton(layout,2003):setTouchEnabled(true)
         UI_GetUIButton(layout,2003):addTouchEventListener(clickGetReward)
      end 
      ]]
   end
   for i = 1,#g_ActivityInfo.szAccount do 
      layout = UI_GetCloneLayout(UI_GetUILayout(widget,11012))
      layout:setVisible(true);
      initLayout(layout,i);
      listview:pushBackCustomItem(layout);
   end 
end

--成长计划
function Activity_GrowUpDetailShow(widget)
   Activity_ActivityLayoutChange(widget,1)
   local imageparent = UI_GetUIImageView(widget,1103)

   --button 
   local function BuyGrowUp(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then
          local function BuyGrowthPlan(sender ,eventType)
                local function ToPay(sender,eventType)      
                    RechargeSystem_ShowLayout()
                end
                if GetLocalPlayer():GetInt(EPlayer_VipLevel) <3 then
                    local tab = {}
                    tab.info = FormatString("Activity_VIPNotEnough")
                    tab.msgType = EMessageType_LeftRight
                    tab.leftFun = ToPay
                    Messagebox_Create(tab)
                elseif GetLocalPlayer():GetInt(EPlayer_Gold) < 2000 then
                    local tab = {}
                    tab.info = FormatString("Activity_YuanbaoNotEnough")
                    tab.msgType = EMessageType_LeftRight
                    tab.leftFun = ToPay
                    Messagebox_Create(tab)
                else
                    Packet_Cmd(GameServer_pb.CMD_BUY_GROWUP_PLAN)
                    ShowWaiting()
                end
            end 
         local tab = {}
         tab.info = FormatString("Activity_BuyGrowthPlan")
         tab.msgType = EMessageType_LeftRight
         tab.leftFun = BuyGrowthPlan
         Messagebox_Create(tab)
      end 
   end

   --title 
   local data = GetGameData(DataFileOperateActive, 1, "stOperateActiveData")
   UI_GetUILabel(UI_GetUIImageView(widget,1005),2001):setText(data.m_activityName)
   UI_GetUILabel(UI_GetUIImageView(widget,1005),2002):setText(data.m_activityDes)

   if data.m_activityType == 1 then 
      UI_GetUILabel(UI_GetUIImageView(widget,1005),2005):setVisible(true)
      Activit_InfoButtonFunc(widget,1)
   else 
      UI_GetUILabel(UI_GetUIImageView(widget,1005),2005):setVisible(false)
   end 

   local button = UI_GetUIButton(imageparent,2003)
   if g_ActivityInfo.bIsBuy then 
      button:setTitleText(FormatString("Activity_GrowUpHaveBuy"))
      button:setTouchEnabled(false)
   else 
      button:setTitleText(FormatString("Activity_GrowUpHaveNotBuy"))
      button:setTouchEnabled(true)
      button:addTouchEventListener(BuyGrowUp)
   end 
   local listview = UI_GetUIListView(imageparent,2001)
   listview:removeAllItems()
   local layout = nil 
   local isBuy = g_ActivityInfo.bIsBuy
   local function initLayout(layout,index)
      --title 
      UI_GetUILabel(layout,2001):setText(FormatString("Activity_GrowUpLimit",g_ActivityInfo.szGrowUpDetail[index].iGoalLevel))
      --lvl 
      UI_GetUILabel(layout,2002):setText(GetLocalPlayer():GetInt(EPlayer_Lvl).."/"..g_ActivityInfo.szGrowUpDetail[index].iGoalLevel)
      --reward
      local imagetag = {2101,2102,2103,2104,2105,2106}
      for i = 1 , #imagetag do 
         local imageview = UI_GetUIImageView(layout,imagetag[i])
         if i == 1 then 
            --imageview:setVisible(true)
            --UI_GetUIImageView(imageview,2):loadTexture("Common/ico_jinbi_01.png")
            --UI_GetUILabel(imageview,1):setText(g_ActivityInfo.szGrowUpDetail[index].iAward)
           -- print("rewardindex==========="..rewardindex)
            local icon = UI_ItemIconFrame(UI_GetLifeAtt(2, g_ActivityInfo.szGrowUpDetail[index].iAward), 3)
            imageview:addChild(icon,1);
         else 
            imageview:setVisible(false)
         end 
      end 
      --button 
      local function clickGetReward(sender,eventType)
         if eventType == TOUCH_EVENT_ENDED then 
            local tab = GameServer_pb.CMD_GET_GROWUP_REWARD_CS()
            tab.iId = g_ActivityInfo.szGrowUpDetail[index].iId
            Packet_Full(GameServer_pb.CMD_GET_GROWUP_REWARD,tab)
            ShowWaiting()
         end 
      end 
      if isBuy == false or g_ActivityInfo.szGrowUpDetail[index].iGoalLevel > GetLocalPlayer():GetInt(EPlayer_Lvl) then 
         UI_GetUIButton(layout,2003):setTouchEnabled(false)
      else 
         UI_GetUIButton(layout,2003):setTouchEnabled(true)
         UI_GetUIButton(layout,2003):addTouchEventListener(clickGetReward)
      end 
   end
   for i = 1,#g_ActivityInfo.szGrowUpDetail do 
      layout = UI_GetCloneLayout(UI_GetUILayout(widget,11011))
      layout:setVisible(true)
      --Log("index===="..i)
      initLayout(layout,i)
      listview:pushBackCustomItem(layout)
   end 
end

function Activit_InfoButtonFunc(widget,tag)
   local function clicklookinfo(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then  
         Activity_ShowActivityInfoLayout(tag)
      end  
   end
   UI_GetUILabel(UI_GetUIImageView(widget,1005),2005):setTouchEnabled(true)
   UI_GetUILabel(UI_GetUIImageView(widget,1005),2005):addTouchEventListener(clicklookinfo)
   UI_GetUILabel(UI_GetUIImageView(widget,1005),2005):enableStroke()
end

function Activity_getStrTwo(src)
   local tab = {}
    local function spilit(txt)
        -- body
        table.insert(tab,txt)
    end
    string.gsub(src.."#","(.-)#",spilit)
    for i = 1,#tab do 
       --Log("src===="..tab[i])
    end 
    return tab 
end

function Activity_PayMentDetailShowlayout(widget)
   Activity_ActivityLayoutChange(widget,2)
   local parentimage = UI_GetUIImageView(widget,1101)
   --title 
   local data = GetGameData(DataFileOperateActive, 2, "stOperateActiveData")
   UI_GetUILabel(UI_GetUIImageView(widget,1005),2001):setText(data.m_activityName)
   UI_GetUILabel(UI_GetUIImageView(widget,1005),2002):setText(data.m_activityDes)

   if data.m_activityType == 1 then 
      UI_GetUILabel(UI_GetUIImageView(widget,1005),2005):setVisible(true)
      Activit_InfoButtonFunc(widget,2)
   else 
      UI_GetUILabel(UI_GetUIImageView(widget,1005),2005):setVisible(false)
   end 

   local listview = UI_GetUIListView(parentimage,2001)
   listview:removeAllItems()
   local layout = nil 
   local function initLayout(layout,index)
      UI_GetUILabel(layout,2001):setText(FormatString("Activity_Paymenttotal",g_ActivityInfo.szAccountList[index].iGoal))
      --lvl 
      UI_GetUILabel(layout,2002):setText(g_ActivityInfo.iActorAccPayment.."/"..g_ActivityInfo.szAccountList[index].iGoal)
      --reward
      local imagetag = {2101,2102,2103,2104,2105,2106}
      local awardData = GetGameData(DataFileAccumulatePayMent,g_ActivityInfo.szAccountList[index].iId,"stAccumulatePaymentData")
      local awardPayMentTable = {awardData.m_RewardOne,awardData.m_RewardTwo,awardData.m_RewardTree}
      local rewardsize = 3
      for i = 1,#imagetag do 
          local imageview = UI_GetUIImageView(layout,imagetag[i])
          if i <= rewardsize then 
             imageview:setVisible(true)
          else 
             imageview:setVisible(false)
          end 
      end
      local function showAwardInfo(sender,eventType)
            if eventType == TOUCH_EVENT_ENDED then
                local image = tolua.cast(sender,"ImageView")
                AddItemTip(tolua.cast(sender,"ImageView"):getTag(),200-image:getWorldPosition().y)
            end
        end
      for i = 1,#awardPayMentTable do 
         local awardtable = Activity_getStrTwo(awardPayMentTable[i])
         local awardType = awardtable[1] 
         local awardId = TypeConvert:ToInt(awardtable[2])
         local awardNum = TypeConvert:ToInt(awardtable[3])
         if awardType == "LifeAtt" then
            local icon = UI_ItemIconFrame(UI_GetLifeAtt(awardId, awardNum), 3)
            UI_GetUIImageView(layout,imagetag[i]):addChild(icon,1,awardId);
         elseif awardType == "Item" then 
            local Itemdata = GetGameData(DataFileItem,awardId,"stItemData")
            local icon = UI_ItemIconFrame(UI_GetItemIcon(Itemdata.m_icon, awardNum), 3)
            icon:setTouchEnabled(true)
            icon:addTouchEventListener(showAwardInfo)
            UI_GetUIImageView(layout,imagetag[i]):addChild(icon,1,awardId);
         elseif awardType == "Hero" then 
            local herodata = GetGameData(DataFileHeroBorn, awardId , "stHeroBornData")
            --local icon = UI_ItemIconFrame(UI_GetHeroIcon(herodata.m_icon,1), 2)
            local icon = UI_IconFrame(UI_GetHeroIcon(awardId), herodata.m_heroInitStar);
            UI_GetUIImageView(layout,imagetag[i]):addChild(icon,1);
         end 
      end 
      --button
      local function clickGetReward(sender,eventType)
         if eventType == TOUCH_EVENT_ENDED then 
            local tab = GameServer_pb.CMD_GET_GROWUP_REWARD_CS()
            tab.iId = g_ActivityInfo.szAccountList[index].iId
            Packet_Full(GameServer_pb.CMD_GET_ACCOUNT_PAYMENT_REWARD,tab)
            ShowWaiting()
         end 
      end 
      if g_ActivityInfo.iActorAccPayment < g_ActivityInfo.szAccountList[index].iGoal then
         UI_GetUIButton(layout,2003):setTouchEnabled(false)
      else 
         UI_GetUIButton(layout,2003):setTouchEnabled(true)
         UI_GetUIButton(layout,2003):addTouchEventListener(clickGetReward)
      end 
   end
   for i = 1,#g_ActivityInfo.szAccountList do 
      layout = UI_GetCloneLayout(UI_GetUILayout(widget,11011))
      layout:setVisible(true)
      initLayout(layout,i)
      listview:pushBackCustomItem(layout)
   end 
end

function ListView_UpdateRefresh(listView,layout)
   GetGlobalEntity():GetScheduler():RegisterInfiniteScript("ListView_PushBackLayout",0.1)
end 

function ListView_PushBackLayout()
   if index <= #g_ActivityInfo.szAccountList then 
      listview:pushBackCustomItem(layout)
   else 
      GetGlobalEntity():GetScheduler():UnRegisterScript("ListView_PushBackLayout")
   end  
end

function Activity_ActivityLayoutChange(widget,index)
   Log("============Activity_ActivityLayoutChange index:"..index);
   if index == 1 then
      --成长计划
      UI_GetUIImageView(widget,1101):setVisible(false)
      UI_GetUIImageView(widget,1102):setVisible(false)
      UI_GetUIImageView(widget,1103):setVisible(true)
      UI_GetUIImageView(widget,500):setVisible(false)
   elseif index == 2 then 
      --充值返利
      UI_GetUIImageView(widget,1101):setVisible(true)
      UI_GetUIImageView(widget,1102):setVisible(false)
      UI_GetUIImageView(widget,1103):setVisible(false) 
      UI_GetUIImageView(widget,500):setVisible(false)
   elseif index == 3 then 
      --liuliang 
      UI_GetUIImageView(widget,1101):setVisible(false)
      UI_GetUIImageView(widget,1102):setVisible(false)
      UI_GetUIImageView(widget,1103):setVisible(false) 
      UI_GetUIImageView(widget,500):setVisible(true)
    elseif index == 4 then
      --七日登录
      UI_GetUIImageView(widget,1101):setVisible(true);
      UI_GetUIImageView(widget,1102):setVisible(false);
      UI_GetUIImageView(widget,1103):setVisible(false);
      UI_GetUIImageView(widget,500):setVisible(false);
   end
end

function Activity_ChangeButtonState(widget,tag)
   local listview = UI_GetUIListView(widget,1004)
   --Log("l_ActivityKindSize==="..l_ActivityKindSize.."     "..tag)
   for i = 1,l_ActivityKindSize do 
      local buttonlayuout = tolua.cast(listview:getItem(tag - 1),"Layout")
      local button = UI_GetUIButton(buttonlayuout,2001)
      if i == tag then 
         button:setButtonEnabled(true)
      else 
         button:setButtonEnabled(false)
      end 
   end 
end

function LookGrowUpDetail_CS()
   Packet_Cmd(GameServer_pb.CMD_QUERY_GROWUP_DETAIL)
   ShowWaiting()
end 

function LookPayMentDetail()
   Packet_Cmd(GameServer_pb.CMD_QUERY_ACCOUNT_PAYMENT_DETAIL) 
   ShowWaiting()
end

-- 请求7日登录详细数据
function Look7DaysDeatil_CS()
   Packet_Cmd(GameServer_pb.CMD_QUERY_ACCOUNT_LOGIN_IN);
   ShowWaiting();
end

--详情
function Activity_ShowActivityInfoLayout(index)
   local data = GetGameData(DataFileOperateActive, index, "stOperateActiveData")
   local widget = UI_CreateBaseWidgetByFileName("ActivityInfo.json")
   local listview = UI_GetUIListView(widget,3)
   listview:removeAllItems()
   UI_GetUILayout(widget, 1):addTouchEventListener(UI_ClickCloseCurBaseWidgetWithNoAction)
   --title 
   UI_GetUILabel(widget,2):setText(data.m_activityName)

   local infotab = Activity_getStr(data.m_activityInfo)
   local listwidth = listview:getContentSize().width
   local height = 30
   local layout = nil 
   for i = 1,#infotab do 
      local layout = Layout:create()
      layout:setSize(CCSizeMake(listwidth,5))
      local lab = CompLabel:GetCompLabel(infotab[i], listwidth - 10, kCCTextAlignmentLeft)
      listview:pushBackCustomItem(lab)
      listview:pushBackCustomItem(layout)
   end 
end

function Activity_getStr(src)
   local tab = {}
    local function spilit(txt)
        -- body
        table.insert(tab,"  "..txt)
    end
    string.gsub(src.."#","(.-)#",spilit)
    for i = 1,#tab do 
       --Log("src===="..tab[i])
    end 
    return tab 
end


function PhoneNumber_IsSureNumber(number)
   local tempnumber = string.sub(number,1,3)
   Log("tempnumber===="..tempnumber)
   for i = 1, #g_PhoneNumber do 
       if tempnumber == g_PhoneNumber[i] then 
          if TBTSDK:GetInstance():PhoneNumberIsSure(number) then 
             return true
          else 
             return false
          end 
       end 
   end 
   return false
end

function PhoneNumber_GetFuckNumber(num)
   local telnumberone = string.sub(num,1,3)
   local telnumbertwo = string.sub(num,4)
   local telnumber = telnumberone.."_"..telnumbertwo
   return telnumber
end