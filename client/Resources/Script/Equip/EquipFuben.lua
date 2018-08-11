--                            _ooOoo_  
--                           o8888888o  
--                           88" . "88  
--                           (| -_- |)  
--                            O\ = /O  
--                        ____/`---'\____  
--                      .   ' \\| |// `.  
--                       / \\||| : |||// \  
--                     / _||||| -:- |||||- \  
--                       | | \\\ - /// | |  
--                     | \_| ''\---/'' | |  
--                      \ .-\__ `-` ___/-. /  
--                   ___`. .' /--.--\ `. . __  
--                ."" '< `.___\_<|>_/___.' >'"".  
--               | | : `- \`.;`\ _ /`;.`/ - ` : | |  
--                 \ \ `-. \_ __\ /__ _/ .-` / /  
--         ======`-.____`-.___\_____/___.-`____.-'======  
--                            `=---='  
--  
--         .............................................  
--                  佛祖镇楼                  BUG辟易  

--[[" 
      装备副本 Begin
"]]
if g_EquipFubenTable == nil then 
   g_EquipFubenTable = {20001,20002,20003,20004,20005,20006}
end

if g_EquipFuben_SelectIndex == nil then 
   g_EquipFuben_SelectIndex = 0
end 

if g_IsEquipFuben == nil then 
   g_IsEquipFuben = false 
end 

function ReturnSceneSection(Id)
   return GetGameData(DataFileSceneSection, Id, "stSceneSectionData")
end 
function EquipFuben_InitValue()
   g_EquipFuben_SelectIndex = 0
   g_IsEquipFuben = true 
end

function EquipFuben_InitLayout()
   local widget = UI_CreateBaseWidgetByFileName("EquipFuben.json")
   EquipFuben_InitValue()
   EquipFunben_ShowLayout(widget)
   UI_GetUIButton(widget,5):addTouchEventListener(UI_ClickCloseCurBaseWidgetWithNoAction)
   UI_GetUILayout(widget,2):addTouchEventListener(PublicCallBackWithNothingToDo)
end


function EquipFunben_ShowLayout(widget)
   local parentlayout = UI_GetUILayout(widget,1)
   local tag = {1,2,3,4,5,6}
   local function ClickEquipFubenFunc(layout,index)
      local function Click(sender,eventType)
         if eventType == TOUCH_EVENT_ENDED then 
            Log("index======"..index)
            UI_CloseCurBaseWidget()
            g_EquipFuben_SelectIndex = g_EquipFubenTable[index]
            local tab = GameServer_pb.Cmd_Cs_QueryDSectionDetail()
            tab.iSectionID = g_EquipFubenTable[index]
            Packet_Full(GameServer_pb.CMD_QUERY_DSECTIONDETAIL, tab)
            ShowWaiting()
         end 
      end
      layout:setTouchEnabled(true)
      layout:addTouchEventListener(Click)
   end
   local function createFubenInfo(layout,index)
      local data = ReturnSceneSection(g_EquipFubenTable[index])
      UI_SetLabelText(layout,1,data.m_name)
   end
   for i = 1,#tag do 
      createFubenInfo(UI_GetUILayout(parentlayout,tag[i]),i)
      ClickEquipFubenFunc(UI_GetUILayout(parentlayout,tag[i]),i)
   end 
end


function EquipFuben_InitFubenLayout()
   EndWaiting()
   local widget = UI_GetBaseWidgetByName("Dungeon");
	if (widget == nil)then
		widget = UI_CreateBaseWidgetByFileName("Dungeon.json");
        Refresh_SlectIndex()
	end
	UI_GetUILayout(widget, 4755899):addTouchEventListener(PublicCallBackWithNothingToDo)
	local function ClickClose(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
		    UI_ClickCloseCurBaseWidgetWithNoAction(sender, eventType)
			--CloseDungeonChallengeNotify();
            EquipFuben_InitLayout()
		end
	end
	
	local closeButton = UI_GetUIButton(widget, 1);
	closeButton:addTouchEventListener(ClickClose);
	
	--UI_GetUIButton(widget, 200):addTouchEventListener(UI_ClickCloseCurBaseWidgetWithNoAction);
	
	local function CloseDungeonWidgetNotify(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then
			UI_ClickCloseCurBaseWidgetWithNoAction(sender, eventType)
            --UI_CloseAllBaseWidget()
            UI_CloseCurBaseWidget(EUICloseAction_None)
		end
	end
	
	UI_GetUIButton(widget, 200):addTouchEventListener(CloseDungeonWidgetNotify)
	
	local function ClickTiaozhan(sender,eventType)
		if eventType == TOUCH_EVENT_ENDED then
		    local tab = GameServer_pb.Cmd_Cs_DungeonFight();
			tab.iSceneID = g_curDungeonInfo[g_selectSectionIndex].iSceneID;
			g_openUIType = EUIOpenType_EquipFuben;
			Packet_Full(GameServer_pb.CMD_DUNGEON_FIGHT, tab);
			ShowWaiting();
            --在战斗开始前保存主公的等级和经验，在结算的时候使用
            g_Fight_lv = GetLocalPlayer():GetInt(EPlayer_Lvl)          
            g_Fight_expNow = GetLocalPlayer():GetInt(EPlayer_Exp)
            g_Fight_expMax = GetLocalPlayer():GetInt(EPlayer_MaxExp)
			ShowWaiting();
--			CloseDungeonChallengeNotify();
		end
	end
	UI_GetUIButton(widget, 4):addTouchEventListener(ClickTiaozhan)
	
	local function ClickSaodang(sender,eventType)
		if eventType == TOUCH_EVENT_ENDED then
			local tab = GameServer_pb.CMD_DUNGEON_SAODANG_CS();
			tab.iSceneID = g_curDungeonInfo[g_selectSectionIndex].iSceneID;
			Packet_Full(GameServer_pb.CMD_DUNGEON_SAODANG, tab);
			ShowWaiting();
			CloseDungeonChallengeNotify();
            g_selectSectionID = g_EquipFuben_SelectIndex
		end
	end
	UI_GetUIButton(widget, 3):addTouchEventListener(ClickSaodang)
	
	local index = EquipFuben_FubenlayoutRefresh(widget)
	
	UI_GetUIListView(widget, 11):jumpContainerToSelectedIndex(index - 2);
	
    UI_GetUIButton(widget, 4681159):setVisible(false)
    UI_GetUIButton(widget, 4681161):setVisible(false)
    --UI_GetUIButton(widget, 1124781087):setTouchEnabled(false)
    UI_GetUILayout(widget,1124781087):setTouchEnabled(false)
    UI_GetUILayout(widget,1124781089):setTouchEnabled(false)
end

function EquipFuben_FubenlayoutRefresh(widget)
    EndWaiting()
	local jumpIndex = 1	
	local sceneSectionData = ReturnSceneSection(g_EquipFuben_SelectIndex)
	UI_GetUILabel(widget, 6):setText(sceneSectionData.m_nameFirst.." "..sceneSectionData.m_name);
	local infoLayout = UI_GetUIImageView(widget, 5);
	local sdBtn = UI_GetUIButton(widget, 3)
	--UI_GetUILabel(sdBtn, 1):setVisible(g_dungeonInfo.bSaoDangHaveLimit);
	local function LeftTimeEnd(sender, eventType)
		local tab = GameServer_pb.Cmd_Cs_QueryDSectionDetail();
		tab.iSectionID = g_selectSectionID;
				
		Packet_Full(GameServer_pb.CMD_QUERY_DSECTIONDETAIL, tab);
	end
	if (g_dungeonInfo.bSaoDangHaveLimit)then
		if (g_dungeonInfo.bShowSaoDangCD)then
			UI_GetUILabel(sdBtn, 1):setTimer(g_dungeonInfo.iSaoDangCDSecond, TimeFormatType_HourMinSec)
			UI_GetUILabel(sdBtn, 1):addEventListenerTimer(LeftTimeEnd);
		else
			UI_GetUILabel(sdBtn, 1):stopTimer()
			UI_GetUILabel(sdBtn, 1):setText(""..g_dungeonInfo.iLeftSaoDangCount.."/"..g_dungeonInfo.iSaoDangLimitCount)
		end
    else 
       UI_GetUILabel(sdBtn, 1):setText(FormatString("DungeonLimit"))
	end
	local function Refresh(tag)
		Log("tag"..tag)
        tag = tag+1
        repeat 
            tag = tag -1 
        until tag <= #g_curDungeonInfo
        if tag <= 0 then
            tag =1
        end
		local data = GetGameData(DataFileScene, g_curDungeonInfo[tag].iSceneID, "stSceneData");
		local dungeonInfo = g_curDungeonInfo[tag];
		UI_SetLabelText(infoLayout, 3, ""..g_dungeonInfo.iCostPhystrength);
		UI_SetLabelText(infoLayout, 2, "");
		local infoLabel = CompLabel:GetDefaultCompLabel(data.m_desc, 520);
		UI_GetUILabel(infoLayout, 2):removeAllChildren();
		UI_GetUILabel(infoLayout, 2):addChild(infoLabel);

		if (dungeonInfo.bHasPassLimit)then
			UI_SetLabelText(infoLayout, 4, ""..dungeonInfo.iLeftCount.."/"..dungeonInfo.iTotalCount);
		else
			UI_SetLabelText(infoLayout, 4, FormatString("DungeonLimit"));
		end
		UI_SetLabelText(infoLayout, 1, data.m_name);

		sdBtn:setTouchEnabled(dungeonInfo.bIsOpend)
		UI_GetUIButton(widget, 4):setTouchEnabled(dungeonInfo.bIsOpend)

		if dungeonInfo.bIsOpend == false  then
			CloseDungeonChallengeNotify();
		end

		local itemList = UI_Split(data.m_drop, "#");

        local function AddItemTipcall(sender,eventType)   
            if eventType == TOUCH_EVENT_ENDED then
                local tag = tolua.cast(sender,"ImageView"):getTag() - 1200
		        AddEquipTip(TypeConvert:ToInt(itemList[tag]))
            end
        end
        -------------------------------------------------------------------------

		for i=1, 5 do
			UI_GetUIImageView(infoLayout, 10 + i):removeAllChildren()
			if (i > #itemList)then
				UI_GetUIImageView(infoLayout, 10 + i):setVisible(false);
			else
				UI_GetUIImageView(infoLayout, 10 + i):setVisible(true);
				local id =  TypeConvert:ToInt(itemList[i])
                Log("id====="..id)
				local equipData = GetGameData(DataFileEquip, TypeConvert:ToInt(itemList[i]), "stEquipData")
                local icon = UI_ItemIconFrame(UI_GetEquipIcon(equipData.m_icon), equipData.m_quality)
				UI_GetUIImageView(infoLayout, 10 + i):addChild(icon,2,i+1200)
                icon:setTouchEnabled(true)
                icon:setScale(80/92)
                icon:addTouchEventListener(AddItemTipcall)
			end
		end
		g_selectSectionIndex = tag;
	end

	local lastMoveLayout = nil;

	local function MoveAction(moveLayout)
		if (lastMoveLayout)then
			lastMoveLayout:setPosition(ccp(0, 0))
			lastMoveLayout:stopAllActions()
		end

		local moveAction = CCEaseBounceOut:create(CCEaseSineOut:create(CCMoveBy:create(0.8, ccp(118, 0))));
		--local moveReverse = CCEaseBounceOut:create(CCEaseSineOut:create(CCMoveBy:create(0.8, ccp(-118, 0))));
		local arryaction = CCArray:create()
		arryaction:addObject(CCDelayTime:create(0.1))
		arryaction:addObject(moveAction)
		--arryaction:addObject(CCDelayTime:create(0.6))
		--arryaction:addObject(moveReverse)
		local seqaction = CCSequence:create(arryaction)
		--local reaction = CCRepeatForever:create(seqaction)
		moveLayout:runAction(seqaction);

		lastMoveLayout = moveLayout;
	end
    local sceneDataList = CDataManager:GetInstance():GetGameDataKeyList(DataFileSceneSection)
	local function ClickScene(sender,eventType) 
		if  (eventType == TOUCH_EVENT_ENDED) then
			local img = tolua.cast( sender, "Layout" );
			tag = img:getTag();
			if (g_selectSectionIndex ~= tag)then
				local moveLayout = UI_GetUILayout(img:getChildByTag(5), 1);
                --如果此关卡未开启
                if g_curDungeonInfo[tag].bIsOpend == false then
                    UI_GetUIImageView(moveLayout, 4555596):loadTexture("UIExport/DemoLogin/Dungeon/Bg_dungeonList_NotOpen.png")
                else
                    UI_GetUIImageView(moveLayout, 4555596):loadTexture("UIExport/DemoLogin/Dungeon/Bg_dungeonList_Select.png")
                end

				MoveAction(moveLayout)
				Refresh(tag)
			end
		end
	end 
	local listView = UI_GetUIListView(widget, 11);

	--local sceneDataList = CDataManager:GetInstance():GetGameDataKeyList(DataFileSceneSection);
	local tempLayout = Layout:create();
    --tempLayout:setTouchEnabled(true);
	tempLayout:setSize(CCSizeMake(220, 160));

	local cloneLayout = UI_GetUILayout(widget, 100);
	cloneLayout:setPosition(ccp(0, 0))

	local isNewOpen = false;
	listView:removeAllItems();
	listView:setItemModel(tempLayout);

	local isPass = true;
	if (g_selectSectionIndex == -1)then 
		for i=1, #g_curDungeonInfo do
			if (g_curDungeonInfo[i].bIsOpend == false) then	
				g_selectSectionIndex = i - 1;	
				break
			end
		end

		if (g_selectSectionIndex == -1)then
			g_selectSectionIndex = #g_curDungeonInfo
		end
	end


	for i=1, #g_curDungeonInfo do
		local layout = UI_GetCloneLayout(tempLayout);
		local img = UI_GetCloneLayout(cloneLayout);
		img:setVisible(true);
		img:addTouchEventListener(ClickScene);

		local data = GetGameData(DataFileScene, g_curDungeonInfo[i].iSceneID, "stSceneData");

		UI_GetUILabel(img, 6):setText(data.m_name)

		if (i == #g_curDungeonInfo) then
			UI_GetUIImageView(img, 1):loadTexture("Dungeon/Bnt_dungeonList_001.png");
		else
			UI_GetUIImageView(img, 1):loadTexture("Dungeon/Bnt_dungeonList_002.png");
		end

		local path = ""
		if data.m_icon >= 10 then
			path = "Dungeon/Bg_dungeonList_0"..data.m_icon..".png"
		else
			path = "Dungeon/Bg_dungeonList_00"..data.m_icon..".png"
		end

		UI_GetUIImageView(img:getChildByTag(5):getChildByTag(1), 1):loadTexture(path)

		if (g_curDungeonInfo[i].bIsOpend) then	
			img:setName("guanqia_"..i)

			local star = g_curDungeonInfo[i].iStarLevel
			if (star == 0)then
				isPass = false;
			end
			for j=1, 3 do
				local starSp = UI_GetUIImageView(img, j + 1);
				if (j <= star)then
					starSp:loadTexture("Dungeon/Bg_star_02.png");
				else
					starSp:loadTexture("Dungeon/Bg_star_01.png");
				end
			end

			if (g_curDungeonInfo[i].bNewOpen)then
				g_selectSectionIndex = i;
				g_curDungeonInfo[i].bNewOpen = false;
			end

			if (g_selectiSceneID == data.m_ID)then
				g_selectSectionIndex = i;
			end
		else	
			isPass = false;
			for j=1, 5 do
				if (j == 5)then
					UI_GetUIImageView(img:getChildByTag(5):getChildByTag(1), 1):addColorGray();
				else
					UI_GetUIImageView(img, j):addColorGray();
				end
			end	
		end

		if (i == g_selectSectionIndex)then
			local moveLayout = UI_GetUILayout(img:getChildByTag(5), 1);

            if g_curDungeonInfo[i].bIsOpend == false then
                UI_GetUIImageView(moveLayout, 4555596):loadTexture("UIExport/DemoLogin/Dungeon/Bg_dungeonList_NotOpen.png")
            else
                UI_GetUIImageView(moveLayout, 4555596):loadTexture("UIExport/DemoLogin/Dungeon/Bg_dungeonList_Select.png")
            end

			MoveAction(moveLayout)
		end

		layout:addChild(img, 0, i);
		listView:pushBackCustomItem(layout);
	end

	g_selectiSceneID = -1;

	Refresh(g_selectSectionIndex)

	jumpIndex = g_selectSectionIndex

	listView:refreshView();
	return jumpIndex;
end


function AddEquipTip(id,down,dynamicItem,tipWidget)
    if tipWidget == nil then
        tipWidget = UI_CreateBaseWidgetByFileName("UIExport/DemoLogin/ItemTip.json")
        tipWidget:addTouchEventListener(UI_ClickCloseCurBaseWidgetWithNoAction)
    else   
        local function exit(sender,eventType)
            if eventType == TOUCH_EVENT_ENDED then
                sender:removeFromParent()
            end
        end
        tipWidget:addTouchEventListener(exit)
    end

    local layout = UI_GetUILayout(tipWidget, 4674185)
    Log("id====="..id)
    local equipData = GetGameData(DataFileEquip, id, "stEquipData")

    if down == nil then
        down =0
    end
    local image = UI_GetUIImageView(tipWidget, 4521681)
    image:setPosition(ccp(image:getPositionX(),image:getPositionY()-down))

    --icon
    UI_GetUIImageView(layout, 4521683):addChild(UI_ItemIconFrame(UI_GetEquipIcon(equipData.m_icon), equipData.m_quality))
    --name
    UI_GetUILabel(layout, 4521687):setText( equipData.m_name )
    UI_GetUILabel(layout, 4521687):setColor( Util_GetColor(equipData.m_quality) )
    if #equipData.m_name>18 then
        UI_GetUILabel(layout, 4521687):setFontSize(24)
    elseif #equipData.m_name>21 then
        UI_GetUILabel(layout, 4521687):setFontSize(20)
    elseif #equipData.m_name>24 then
        UI_GetUILabel(layout, 4521687):setFontSize(18)
    else
        UI_GetUILabel(layout, 4521687):setFontSize(24)
    end

    UI_GetUILabel(layout, 4521693):setVisible(false)
    UI_GetUILabel(layout, 4521689):setVisible(false)

    local data = UI_GetUILayout(layout, 4101)
    data:setVisible(true)
        --price
    UI_GetUILabel(data, 1003):setText( equipData.m_sellCount )
    UI_GetUILabel(data, 1001):setText( FormatString("BagLayer_limit_"..equipData.m_zhiyeLimit) )
    UI_GetUILabel(data, 1002):setText( equipData.m_lvlLimit )

    local listView = UI_GetUIListView(image, 4674188)
    listView:removeAllItems()
    layout:removeFromParent()
    listView:pushBackCustomItem(layout)
    local lab = UI_GetUILabel(tipWidget, 4674190)
    lab:removeFromParent()
    lab:setText(equipData.m_attinfo)

    local addlab = nil
    if dynamicItem == nil then
        addlab = CompLabel:GetDefaultCompLabel(FormatString("EquipFuben_AddInfo"),280)
    else
        local str = GetAttNameAndValue(dynamicItem.iLifeAttID,dynamicItem.iValue)
        addlab = CompLabel:GetDefaultCompLabel(str,280)
    end
    addlab:setColor(ccc3(170,45,170))
    print("lab:getContentSize().height================="..lab:getContentSize().height)
            
    --CompLabel:SetFontSize(10)
    local desclab = CompLabel:GetDefaultCompLabel(equipData.m_desc,280)
    desclab:setColor(ccc3(255,203,31))

    listView:pushBackCustomItem(lab)
    listView:pushBackCustomItem(addlab)
    listView:pushBackCustomItem(desclab)


    local height = layout:getSize().height+lab:getSize().height+desclab:getSize().height+20 + addlab:getSize().height
    image:setSize(CCSizeMake(300 , height))
    --坐标修正
    down = (350 - height)/2
    image:setPosition(ccp(image:getPositionX(),image:getPositionY()-down))
end
--[[" 
      装备副本 End
"]]


ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_EQUIPDUNGEON_QUERY_EQUIPINFO, "EquipFuben_TrunCard_SC" )
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_EQUIPDUNGEON_FP, "TrunCard_SC" )


function EquipFuben_TrunCard_SC(pkg)
    EndWaiting()
    local info = GameServer_pb.CMD_EQUIPDUNGEON_QUERY_EQUIPINFO_SC();
	info:ParseFromString(pkg)
    Log("info====="..tostring(info))
    EquipFuben_LookCanGetEquip(info)
end



--触发变量
if g_EquipCard_IsOpen == nil then 
   g_EquipCard_IsOpen = false
end 

--widget
if g_EquipWidget == nil then 
   g_EquipWidget = 0
end 

--记录当前的状态
local l_isOpen = {}

g_curCostInfo = nil;
function TrunCard_SC(pkg)
   EndWaiting()
   local info = GameServer_pb.CMD_EQUIPDUNGEON_FP_SC();
   info:ParseFromString(pkg)
   Log("infohahahaha====="..tostring(info))
   if g_EquipWidget ~= 0 then 
       --EquipCardTrun_CreateCardInfo(UI_GetUILayout(widget,))
       if #l_isOpen ~= 0 then 
          l_isOpen[info.iPos] = true
       end 
       local tag = {4,5,6,7}
       --EquipCardTrun_InitCardLayout(UI_GetUILayout(g_EquipWidget,2),info)
       --UI_GetUILayout(layout,tag[info.iPos]):setVisible(false)
       CardTrunAction(UI_GetUILayout(g_EquipWidget,2),info)
       EquipFuben_CostInfo(UI_GetUILayout(g_EquipWidget,2),info)
	   g_curCostInfo = info;
   end 
end

function EquipCardTrun_InitValue(widget)
   l_isOpen = {false,false,false,false}
   g_EquipWidget = widget 
end

function EquipCardTrun_IsOpen()
   local isOpen = false 
   for i = 1 , #l_isOpen do 
       if l_isOpen[i] == true then 
          isOpen = true 
          break 
       end 
   end 
   return isOpen
end


function CardTrunAction(layout,info)
   local tag = {4,5,6,7}
   local posx,posy = UI_GetUILayout(layout,tag[info.iPos]):getPosition()
   UI_GetUILayout(layout,tag[info.iPos]):setTouchEnabled(false)
   UI_GetUILayout(layout,tag[info.iPos]):setAnchorPoint(ccp(0.5,0.5))
   local layoutwidth = UI_GetUILayout(layout,tag[info.iPos]):getContentSize().width
   local layoutheight = UI_GetUILayout(layout,tag[info.iPos]):getContentSize().height
   UI_GetUILayout(layout,tag[info.iPos]):setPosition(ccp(posx + layoutwidth / 2,posy + layoutheight / 2))
   local function afteraction()
       local templayout = EquipCardTrun_CreateCardTemp(layout,true)
       templayout:setVisible(true)
       templayout:setPosition(ccp(0,0))
       templayout:setTouchEnabled(false)
       UI_GetUILayout(layout,tag[info.iPos]):removeAllChildren()
       UI_GetUILayout(layout,tag[info.iPos]):setTouchEnabled(false)
       EquipCardTrun_CreateCardInfo(templayout,info)

       local notopendactionArry = CCArray:create()
       local notopendaction = CCOrbitCamera:create(0.3, 1, 0, 90, -90, 0, 0)
       --local showcontinufun = CCCallFunc:create()
       notopendactionArry:addObject(notopendaction)
       --notopendactionArry:addObject(showcontinufun)
       local tempseqaction = CCSequence:create(notopendactionArry)
       UI_GetUILayout(layout,tag[info.iPos]):addChild(templayout) 
       UI_GetUILayout(layout,tag[info.iPos]):runAction(tempseqaction)
   end
   local notopendactionArryone = CCArray:create()
   local notopendactionone = CCOrbitCamera:create(0.3, 1, 0, 0, -90, 0, 0)
   local showcontinufunone = CCCallFunc:create(afteraction)
   notopendactionArryone:addObject(notopendactionone)
   notopendactionArryone:addObject(showcontinufunone)
   local tempseqactionone = CCSequence:create(notopendactionArryone) 
   UI_GetUILayout(layout,tag[info.iPos]):runAction(tempseqactionone)
end

function EquipCardTrun_InitLayout()
   local widget = GUIReader:shareReader():widgetFromJsonFile("EquipCardTurn.json")
   EquipCardTrun_InitValue(widget)
   EquipCardTrun_ShowCurLayout(widget)
   MainScene:GetInstance():AddChild(widget, EMSTag_EquipCard, EMSTag_EquipCard , true)
   local function clickClose(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then 
		if  (EquipCardTrun_IsOpen() == false) or (g_curCostInfo ~= nil and g_curCostInfo.iNextCostGold == 0) then
			createPromptBoxlayout(FormatString("EquipFuben_TrunCard"))
			return;
        else 
            g_curCostInfo = nil 
		end
			
	  
         if EquipCardTrun_IsOpen() then 
             g_EquipWidget = 0
             MainScene:GetInstance():removeChildByTag(EMSTag_EquipCard)
             if returnPlayerlevelupdateinfo() ~=nil then
                HeroLevelUpInfoShow()
             end
             g_EquipCard_IsOpen = false
         end 
      end 
   end
   UI_GetUIButton(widget,3):addTouchEventListener(clickClose)
end

function EquipCardTrun_ShowCurLayout(widget)
   if g_fightReslut == nil then 
      return 
   end 

   local parentlayout = UI_GetUILayout(widget,2)
   --查看
   local function LookFunc(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then
         Packet_Cmd(GameServer_pb.CMD_EQUIPDUNGEON_QUERY_EQUIPINFO) 
         ShowWaiting() 
      end 
   end
   UI_GetUIButton(parentlayout,1):addTouchEventListener(LookFunc)


   EquipCardTrun_InitCardLayout(parentlayout)
   EquipCardTrun_ClickCard(parentlayout)
   EquipFuben_CostInfo(parentlayout)
end

function EquipCardTrun_InitCardLayout(layout,info)
    local tag = {4,5,6,7}
    if info == nil then 
       --全部没有翻开的时候
       for i = 1,#tag do  
          local templayout = EquipCardTrun_CreateCardTemp(layout,false)
          templayout:setVisible(true)
          templayout:setPosition(ccp(0,0))
          templayout:setTouchEnabled(false)
          UI_GetUILayout(layout,tag[i]):addChild(templayout)
       end 
    else 
       Log("xxxxxxxxxxxxxxxxxx")
       for i = 1,#tag do 
           if i == info.iPos then 
              local templayout = EquipCardTrun_CreateCardTemp(layout,true)
              templayout:setVisible(true)
              templayout:setPosition(ccp(0,0))
              templayout:setTouchEnabled(false)
              UI_GetUILayout(layout,tag[i]):removeAllChildren()
              UI_GetUILayout(layout,tag[i]):addChild(templayout)
              UI_GetUILayout(layout,tag[i]):setTouchEnabled(false)
              EquipCardTrun_CreateCardInfo(templayout,info)
           end 
       end 
    end 
end

function EquipCardTrun_ClickCard(layout)
   local tag = {4,5,6,7}
   local function ClickFunc(templayout,index)
      local function click(sender,eventType)
         if eventType == TOUCH_EVENT_ENDED then 
            Log("i====="..index)
            if #l_isOpen == 0 then 
               return 
            end 
            if l_isOpen[i] then 
               Log("open")
            else 
               Log("not open")
               local function ToContinue()
                  EquipCardTrun_TrunCard(templayout,index)
               end
               if g_curCostInfo ~= nil then 
                   if g_curCostInfo.iNextCostGold ~= 0 then 
                      local tab = {}
                      tab.info = FormatString("EquipFuben_TurnCardCost",g_curCostInfo.iNextCostGold)
                      tab.msgType = EMessageType_LeftRight
                      tab.leftFun = ToContinue
                      Messagebox_Create(tab)
                   else 
                      EquipCardTrun_TrunCard(templayout,index)
                   end 
               else 
                  EquipCardTrun_TrunCard(templayout,index)
               end 
            end 
         end 
      end
      templayout:setTouchEnabled(true)
      templayout:addTouchEventListener(click)
   end
   for i = 1,#tag do 
      ClickFunc(UI_GetUILayout(layout,tag[i]),i)
   end   
end

function EquipCardTrun_TrunCard(templayout,index)
   local tab = GameServer_pb.CMD_EQUIPDUNGEON_FP_CS()
   tab.iPos = index
   Packet_Full(GameServer_pb.CMD_EQUIPDUNGEON_FP,tab)
   ShowWaiting()
end

function EquipCardTrun_CreateCardTemp(layout,isCardBack)
   if isCardBack then 
      return UI_GetCloneLayout(UI_GetUILayout(layout,9))
   else 
      return UI_GetCloneLayout(UI_GetUILayout(layout,8))
   end 
end

function EquipTrun_Wuyu(str)
   local infostring = string.gsub(str,"\n","\n".."   ")
   return infostring
end

function EquipCardTrun_CreateCardInfo(layout,info)
   local equipId = info.equipInfo.iItemID
   local equipData = GetGameData(DataFileEquip, equipId, "stEquipData")
   --name 
   UI_SetLabelText(layout,1,equipData.m_name)
   --lvl
   UI_SetLabelText(layout,3,FormatString("Hero_lvl",equipData.m_lvlLimit))

   --icon
   --UI_GetHeroEquipIcon(equipData.m_icon,UI_GetUIImageView(layout,2),1)
   --frame
   local icon = UI_ItemIconFrame(UI_GetEquipIcon(equipData.m_icon), equipData.m_quality)
   icon:setScale(0.8)
   UI_GetUIListView(UI_GetUIImageView(layout,2),2):addChild(icon)
   --UI_IconFrameWithSkillOrItem(UI_GetUIImageView(layout,2),2,equipData.m_quality)
   
   --str
   --info
   local infolayout = UI_GetUILayout(layout,4)
   local layoutwidth = infolayout:getContentSize().width
   local layoutheight = infolayout:getContentSize().height
   local infostrlab = Label:create()
   infostrlab:setText(FormatString("HeroEquip_BaseInfo").."\n".."   "..EquipTrun_Wuyu(equipData.m_attinfo))
   UI_SetLabelFontName(1,infostrlab)
   infostrlab:setFontSize(20)
   infolayout:addChild(infostrlab)
   infostrlab:setAnchorPoint(ccp(0,1))
   infostrlab:setPosition(ccp(0,layoutheight))
   local infostrlabHeight = infostrlab:getContentSize().height
   local equipe = GetLocalPlayer():GetEquipBags():getEquipByObjectId(info.equipInfo.dwObjectID)
   local addabstri = equipe:getAddAttribute()
   if addabstri.lifeattId ~= 0 then 
      local addabstristr = FormatString("HeroEquip_AddInfo").."\n".."   "..GetAttNameAndValue(addabstri.lifeattId,addabstri.lifeattValue)
      local addabstrilab = Label:create()
      addabstrilab:setText(addabstristr)
      addabstrilab:setFontSize(20)
      addabstrilab:setAnchorPoint(ccp(0,1))
      addabstrilab:setPosition(ccp(0,layoutheight - infostrlabHeight - 10))
      infolayout:addChild(addabstrilab)
      addabstrilab:setColor(ccc3(167,44,176))
      UI_SetLabelFontName(1,addabstrilab)
   end 

   --sellCoun
   UI_SetLabelText(layout,5,equipData.m_sellCount)
end

function EquipFuben_CostInfo(layout,info)
   if info == nil then 
      UI_GetUILayout(layout,2):setVisible(true)
      UI_GetUILayout(layout,3):setVisible(false)
   else 
      UI_GetUILayout(layout,2):setVisible(false)
      UI_GetUILayout(layout,3):setVisible(true)
      UI_SetLabelText(UI_GetUILayout(layout,3),1,info.iNextCostGold)
   end 
end


function EquipFuben_LookCanGetEquip(info)
   local widget = GUIReader:shareReader():widgetFromJsonFile("EquipCardCanGet.json")
   local parentlayout = UI_GetUILayout(widget,1)
   local tag = {1,2,3,4}
   local function createinfo(imagelayout,index)
      local equipData = GetGameData(DataFileEquip, info.szEquipIDList[index], "stEquipData")
      --icon
      --UI_GetHeroEquipIcon(equipData.m_icon,imagelayout,1)
      --frame
      UI_GetUIImageView(imagelayout, 2):addChild(UI_ItemIconFrame(UI_GetEquipIcon(equipData.m_icon), equipData.m_quality))
      --UI_IconFrameByClone(imagelayout,2,equipData.m_quality)
   end
   local function clickInfo(imagelayout,index)
      local function clickfunc(sender,eventType)
         if eventType == TOUCH_EVENT_ENDED then 
            Log("xxxxxx==="..index)
            local tip = GUIReader:shareReader():widgetFromJsonFile("ItemTip.json")
            tip:setZOrder(100)
            widget:addChild(tip)
            AddEquipTip(info.szEquipIDList[index],nil,nil,tip)
            --AddEquipTip_GetWidget(UI_GetUILayout(widget,3),info.szEquipIDList[index])
         end 
      end
      imagelayout:setTouchEnabled(true)
      imagelayout:addTouchEventListener(clickfunc)
   end
   for i = 1,#tag do 
      local tempimage = UI_GetUIImageView(parentlayout,tag[i])
      createinfo(tempimage,i)
      clickInfo(tempimage,i)
   end 
   MainScene:GetInstance():AddChild(widget,EMSTag_EquipCardLook,EMSTag_EquipCardLook,true)


   local function closefunc(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then 
         MainScene:GetInstance():removeChildByTag(EMSTag_EquipCardLook,true)
      end 
   end
   UI_GetUILayout(widget,2):setTouchEnabled(true)
   UI_GetUILayout(widget,2):addTouchEventListener(closefunc)
end

function AddEquipTip_GetWidget(layout,id)
    layout:setVisible(true)
   local equipData = GetGameData(DataFileEquip, id, "stEquipData")
   --name 
   UI_SetLabelText(layout,1,equipData.m_name)
   --lvl
   UI_SetLabelText(layout,3,FormatString("Hero_lvl",equipData.m_lvlLimit))
   --icon
   UI_GetHeroEquipIcon(equipData.m_icon,UI_GetUIImageView(layout,2),1)
   --frame
   UI_IconFrameWithSkillOrItem(UI_GetUIImageView(layout,2),2,equipData.m_quality)
   
   --str
   --info
   local infolayout = UI_GetUILayout(layout,4)
   infolayout:removeAllChildren()
   local layoutwidth = infolayout:getContentSize().width
   local layoutheight = infolayout:getContentSize().height
   local infostrlab = Label:create()
   infostrlab:setText(equipData.m_attinfo)
   UI_SetLabelFontName(1,infostrlab)
   infostrlab:setFontSize(20)
   infolayout:addChild(infostrlab)
   infostrlab:setAnchorPoint(ccp(0,1))
   infostrlab:setPosition(ccp(0,layoutheight))
   local infostrlabHeight = infostrlab:getContentSize().height
   local addabstristr = FormatString("EquipFuben_AddInfo")

   local addabstrilab = Label:create()
   addabstrilab:setText(addabstristr)
   addabstrilab:setFontSize(20)
   addabstrilab:setAnchorPoint(ccp(0,1))
   addabstrilab:setPosition(ccp(0,layoutheight - infostrlabHeight - 10))
   infolayout:addChild(addabstrilab)
   addabstrilab:setColor(ccc3(52,255,255))
   UI_SetLabelFontName(1,addabstrilab)
end