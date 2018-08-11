function Arena_Create()
    local widget = UI_GetBaseWidgetByName("Arena")
	if (widget == nil)then
	    widget = UI_CreateBaseWidgetByFileName("Arena.json");
	end
	NoticeFunction_UpdateArenaButton()
	UI_GetUIButton(widget, 1):addTouchEventListener(UI_ClickCloseCurBaseWidgetWithNoAction);
    UI_GetUILayout(widget, 4764242):addTouchEventListener(PublicCallBackWithNothingToDo)
	--兑换
	function ClickArenaAward(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then			
			Shop_CreateExchange()
		end
	end
	UI_GetUIButton(widget, 5):addTouchEventListener(ClickArenaAward);
	--规则说明
	function ClickArenaRank(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then			
			Arena_Rule()
		end
	end
	UI_GetUIButton(widget, 6):addTouchEventListener(ClickArenaRank);
	--战报
	function ClickArenaLog(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then			
			Packet_Cmd(GameServer_pb.CMD_QUERY_ARENA_LOG);
			ShowWaiting();
		end
	end
	UI_GetUIButton(widget, 4647976):addTouchEventListener(ClickArenaLog);
    --排行
    local function 	getRanking(sender, eventType)
        if eventType == TOUCH_EVENT_ENDED then
            Packet_Cmd(GameServer_pb.CMD_QUERY_ARENA_RANK);
			ShowWaiting();
        end
    end
    UI_GetUIButton(widget, 4647975):addTouchEventListener(getRanking)
    
    local allData = #g_arenaInfo.challengeInfo.szChallengeList
	--获取队伍信息
	local function ClickHeroIcon(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then		
		    local tag = tolua.cast(sender,"ImageView"):getTag()
			Log("tag====="..g_arenaInfo.challengeInfo.szChallengeList[allData + 11001 - tag ].strName)
			local tab = GameServer_pb.CMD_USENAME_FINED_ACTOR_INFO_CS()
			tab.strActorName = g_arenaInfo.challengeInfo.szChallengeList[allData + 11001 - tag].strName
			Packet_Full(GameServer_pb.CMD_USENAME_FINED_ACTOR_INFO,tab)
		    ShowWaiting()
		end
	end
	--挑战的回调
	local function ClickFight(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then			
			local sender = tolua.cast(sender, "CCNode");		
			local tag = sender:getTag();
			
			local arenaPlayer = g_arenaInfo.challengeInfo.szChallengeList[allData +1 -tag];
			local tab = GameServer_pb.CMD_ARENA_FIGHT_CS();
			tab.iRank = arenaPlayer.iRank;
			Packet_Full(GameServer_pb.CMD_ARENA_FIGHT, tab);
			
			g_openUIType = EUIOpenType_Arena;
			ShowWaiting();
		end
	end
	
    UI_GetUIImageView(widget, 4702499):setVisible(true)
    local isHaveEnemy = false
	for i=1, 4, 1 do
		local arenaPlayer = g_arenaInfo.challengeInfo.szChallengeList[allData+1-i];
		local layout = UI_GetUIImageView(widget, 100 + i);	
        
        layout:setVisible(false);

        local iconBg = UI_GetUIImageView(layout, i + 1000);
        iconBg:removeNodeByTag(2);
		if (arenaPlayer)then
            isHaveEnemy = true
			layout:setVisible(true);
			local data = nil;
			if (arenaPlayer.bMonster)then
				data = GetGameData(DataFileMonster, arenaPlayer.iShowID, "stMonsterData")
			else
				data = GetGameData(DataFileHeroBorn, arenaPlayer.iShowID, "stHeroBornData")
			end
            --对方头像

            local headImg = CCSprite:create("Icon/HeroIcon/"..data.m_icon..".png")
            local headIcon = UIMgr:GetInstance():createMaskedSprite(headImg,"zhujiemian/bg_4.png")
            iconBg:addNode(headIcon,0,2)

					
			UI_GetUILabel(layout, 10003):setText(arenaPlayer.strName);
			UI_GetUILabel(layout, 10002):setText(arenaPlayer.iLevel);
			UI_GetUILabel(layout, 10004):setText(FormatString("Arena_RankStr", arenaPlayer.iRank));
			UI_GetUILabel(layout, 10005):setText(FormatString("HeroFightValue", arenaPlayer.iFightValue));
			
            --挑战按钮
			local button = UI_GetUIButton(layout,i);
			button:setTouchEnabled(arenaPlayer.strName ~= GetLocalPlayer():GetEntityName());

			button:addTouchEventListener(ClickFight);
            --查看信息
            UI_GetUILayout(layout, i+11000):addTouchEventListener(ClickHeroIcon)
		end
	end
	if isHaveEnemy then
        UI_GetUIImageView(widget, 4702499):setVisible(false)
    end
	Arena_Refresh(widget)
	
	--UIMgr:GetInstance():PlayOpenAction(widget, EUIOpenAction_Enlarge, 0.4, 0);
    UI_GetUILabel(widget, 4751556):enableStroke()
    UI_GetUILabel(widget, 4751551):enableStroke()
    UI_GetUILabel(widget, 4647986):enableStroke()
    UI_GetUILabel(widget:getChildByTag(4647977), 4647999):enableStroke()
    UI_GetUILabel(widget:getChildByTag(4648002), 4648010):enableStroke()
    
    UI_GetUILabel(widget, 3):enableStroke()
    UI_GetUILabel(widget, 4):enableStroke()
end

function Arena_Refresh(widget)
	if (widget == nil)then
		Log("Arena_Refresh error");
		return;
	end
	UI_GetUIButton(widget, 5):removeAllNodes();
	--if (g_arenaInfo.bCanGetAward == true) then
	--	local armature = GetUIArmature("Effrenwutisi")
	--	armature:getAnimation():playWithIndex(0)
	--	UI_GetUIButton(widget, 5):addNode(armature);
	--end
	
	if (g_arenaInfo.iSelfRank == 0) then
		UI_GetUILabel(widget, 3):setText(FormatString("Arena_NoCurRank"));
	else
		UI_GetUILabel(widget, 3):setText(""..g_arenaInfo.iSelfRank);
	end
	
	UI_GetUILabel(widget, 4):setText(""..g_arenaInfo.iFightValue);
	UI_GetUILabel(widget, 4647986):setText(g_arenaInfo.appendInfo.iCurRemaindTimes.."/5");
    UI_GetUILabel(widget, 4647986):setVisible(true)

    if g_arenaInfo.appendInfo.iCurRemaindTimes > 0 then
        --UI_GetUILabel(widget, 4647986):setColor(ccc3(217,130,67))
        UI_GetUILabel(widget, 8):setText(FormatString("Arena2"))
    else
        UI_GetUILabel(widget, 4647986):setVisible(false)
        UI_GetUILabel(widget, 8):setText(FormatString("Arena1"))
    end
	
    -------------------------购买挑战次数、重置冷却时间、换一批---------------------
    --购买挑战次数
    local function buyTimes(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            Packet_Cmd(GameServer_pb.CMD_ADD_ARENACHALLENGE);
	        ShowWaiting();
        end
    end
    --重置冷却时间
    local function resetTime(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            Packet_Cmd(GameServer_pb.CMD_ARENA_RESET);
	        ShowWaiting();
        end
    end
    --换一批
    local function changeEnemy(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            Packet_Cmd(GameServer_pb.CMD_ARENA_CHANGE_CHALLENGELIST);
	        ShowWaiting();
        end
    end
    --冷却时间到
    local function timeEnd(sender,eventType)
        UI_GetUILayout(widget, 4647977):setVisible(false)
    end

    local button = UI_GetUIButton(widget, 7)
    local time = UI_GetUILayout(widget, 4647977)
    local buy = UI_GetUILayout(widget, 4648002)

    time:setVisible(false)
    buy:setVisible(false)
    if g_arenaInfo.appendInfo.state==2 then
        --挑战倒计时
        time:setVisible(true)
        UI_GetUILabel(time, 4647999):setTimer(g_arenaInfo.appendInfo.iRemaindSecond)
        UI_GetUILabel(time, 4647999):addEventListenerTimer(timeEnd)
        
        button:addTouchEventListener(resetTime)
        button:setTitleText(FormatString("ResetTime"))
    elseif g_arenaInfo.appendInfo.state==3 then
        --购买次数
        buy:setVisible(true)
        UI_GetUILabel(buy, 4648010):setText(g_arenaInfo.appendInfo.iCost)

        button:addTouchEventListener(buyTimes)
        button:setTitleText(FormatString("BuyFightTimes"))
    elseif g_arenaInfo.appendInfo.state==1 then
        --换一批
        button:addTouchEventListener(changeEnemy)
        button:setTitleText(FormatString("ChangeEnemy"))
        g_Arena_Change = true
    end     
    ------------------------------------------END-----------------------------------------
end

function ArenaAward_Create()
	local widget = UI_GetBaseWidgetByName("ArenaAward")
	if (widget == nil)then
		widget = UI_CreateBaseWidgetByFileName("ArenaAward.json");
	end
	if (widget == nil)then
		Log("ArenaAward_Create error");
		return;
	end
	
	UI_GetUIButton(widget, 1):addTouchEventListener(UI_ClickCloseCurBaseWidget);
	UI_GetUILayout(widget, 4508841):addTouchEventListener(PublicCallBackWithNothingToDo)
	local function ClickGetAward(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then			
			local sender = tolua.cast(sender, "CCNode");		
			local tag = sender:getTag();

			local tab = GameServer_pb.CMD_GET_ARENA_AWARD_CS();
			tab.strUUID = g_arenaAwardList[tag].strUUID;
			Packet_Full(GameServer_pb.CMD_GET_ARENA_AWARD, tab);
			
			ShowWaiting();
		end
	end
	
	local function LeftTimeEnd(sender, eventType)
		Packet_Cmd(GameServer_pb.CMD_QUERY_ARENA_AWARD);
	end
	local tmpLayout = UI_GetUIImageView(widget, 100);
	local listView = UI_GetUIListView(widget, 2);
	listView:removeAllItems();

	for i=1, #g_arenaAwardList do
		local arenaAwardItem = g_arenaAwardList[i]
		local layout = UI_GetCloneImageView(tmpLayout);
		layout:setVisible(true);
		UI_GetUILabel(layout, 10001):setText(FormatString("Arena_AwardDate", os.date("%m", arenaAwardItem.dwTime), os.date("%d", arenaAwardItem.dwTime), arenaAwardItem.iRank));
		UI_GetUILabel(layout, 10002):setText(FormatString("Arena_AwardInfo", arenaAwardItem.iHonorAward, arenaAwardItem.iSilverAward));
		UI_GetUIButton(layout, 10003):setVisible(arenaAwardItem.bCanGetNow)
		
		if (arenaAwardItem.bCanGetNow) then
			local button = UI_GetUIButton(layout, 10003);
			button:removeFromParent();
			button:addTouchEventListener(ClickGetAward)
			layout:addChild(button, i, i);
		else
			local leftTime = UI_GetUILabel(layout, 10004);
			leftTime:setTimer(arenaAwardItem.iLeftSecond, TimeFormatType_HourMinSec)
			leftTime:addEventListenerTimer(LeftTimeEnd);
		end
		
		UI_GetUILabel(layout, 10004):setVisible(arenaAwardItem.bCanGetNow == false);
		listView:pushBackCustomItem(layout);
	end
	tmpLayout:setVisible(false);
end

function ArenaLog_Create(szLogItemList)
	local widget = UI_CreateBaseWidgetByFileName("ArenaLog.json");

	if (widget == nil)then
		Log("ArenaLog_Create error");
		return;
	end
	
	UI_GetUIButton(widget, 1):addTouchEventListener(UI_ClickCloseCurBaseWidgetWithNoAction);
	UI_GetUILayout(widget, 4508837):addTouchEventListener(PublicCallBackWithNothingToDo)
	local function ClickFightInfo(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then			
			local sender = tolua.cast(sender, "CCNode");		
			local tag = sender:getTag();
			
			local tab = GameServer_pb.CMD_COMM_QUERYBATTLE_CS();
			tab.strFightKey = szLogItemList[tag].strUUID;
			Packet_Full(GameServer_pb.CMD_COMM_QUERYBATTLE, tab);
			
			g_openUIType = EUIOpenType_Arena;
            --竞技场查看战报           
            g_Arena_isCheckRecord = true
			ShowWaiting();
		end
	end
	
	local tmpLayout = UI_GetUIImageView(widget, 100);
    tmpLayout:setVisible(true)
	local listView = UI_GetUIListView(widget, 2);
	for i=1, #szLogItemList do
		local arenaLogItem = szLogItemList[i];
		local layout = UI_GetCloneLayout(tmpLayout);

		local icon = UI_GetUIImageView(layout, 10003);
		local infoLabel = nil;
		if (GetLocalPlayer():GetEntityName() == arenaLogItem.strAttName) then
			if (arenaLogItem.bAttackerWin) then
				if (arenaLogItem.iRank == 0) then
					infoLabel = CompLabel:GetDefaultCompLabel(FormatString("Arena_Win3", arenaLogItem.strTarget), 500);
				else
					infoLabel = CompLabel:GetDefaultCompLabel(FormatString("Arena_Win1", arenaLogItem.strTarget, arenaLogItem.iRank), 500);
				end
			else
				icon:loadTexture("jingjichang/ico_2_01.png");
				infoLabel = CompLabel:GetDefaultCompLabel(FormatString("Arena_Lose1", arenaLogItem.strTarget), 500);
			end
		else
			if (arenaLogItem.bAttackerWin == false) then
				infoLabel = CompLabel:GetDefaultCompLabel(FormatString("Arena_Win2", arenaLogItem.strAttName), 500);
			else
				icon:loadTexture("jingjichang/ico_2_01.png");
				if (arenaLogItem.iRank == 0) then
					infoLabel = CompLabel:GetDefaultCompLabel(FormatString("Arena_Lose3", arenaLogItem.strAttName), 500);
				else
					infoLabel = CompLabel:GetDefaultCompLabel(FormatString("Arena_Lose2", arenaLogItem.strAttName, arenaLogItem.iRank), 500);
				end
				
			end
		end

		UI_GetUILabel(layout, 10001):addChild(infoLabel);
		
		local button = UI_GetUIButton(layout, 10002);
		button:removeFromParent();
		button:addTouchEventListener(ClickFightInfo)
		layout:addChild(button, i, i);
		
		listView:pushBackCustomItem(layout);
	end
	tmpLayout:setVisible(false);
end

function Arena_RefreshData()
    if UI_GetBaseWidgetByName("Arena") then
        Packet_Cmd(GameServer_pb.CMD_OPEN_ARENA);
        ShowWaiting();
    end
end

function OPEN_ARENA(pkg)
	EndWaiting();
	local tmp = GameServer_pb.CMD_OPEN_ARENA_SC();
	tmp:ParseFromString(pkg)
	Log("*********"..tostring(tmp))
	g_arenaInfo = tmp;

    local widget = UI_GetBaseWidgetByName("Arena")
    if widget == nil or g_Arena_Change == true then
        g_Arena_Change = false
	    Arena_Create();
    else
        Arena_Refresh(widget)
    end
end

function ADD_ARENACHALLENGE(pkg)
	EndWaiting();
	local tmp = GameServer_pb.CMD_ADD_ARENACHALLENGE_SC();
	tmp:ParseFromString(pkg)

	g_arenaInfo.iLeftChallengeCount = tmp.iLeftChallengeCount;
	local widget = UI_GetBaseWidgetByName("Arena")
	if (widget) then		
	    UI_GetUILabel(widget, 4647986):setText(g_arenaInfo.iLeftChallengeCount.."/5");
        UI_GetUILayout(widget, 4648002):setVisible(false)
        UI_GetUILayout(widget, 4647977):setVisible(false)

        local button = UI_GetUIButton(widget, 7)
        local function changeEnemy(sender,eventType)
            if eventType == TOUCH_EVENT_ENDED then
                Packet_Cmd(GameServer_pb.CMD_ARENA_CHANGE_CHALLENGELIST);
	            ShowWaiting();
            end
        end
        button:addTouchEventListener(changeEnemy)
        button:setTitleText(FormatString("ChangeEnemy"))
	end
end

if (g_arenaInfo == nil) then
	g_arenaInfo = {};
end

if (g_arenaAwardList == nil) then
	g_arenaAwardList = {};
end

function QUERY_ARENA_AWARD(pkg)
	EndWaiting();
	
	local tmp = GameServer_pb.CMD_QUERY_ARENA_AWARD_SC();
	tmp:ParseFromString(pkg)
	
	Log("QUERY_ARENA_AWARD"..tostring(tmp))
	g_arenaAwardList = tmp.szArenaAwardList;
	ArenaAward_Create();
end

function GET_ARENA_AWARD(pkg)
	EndWaiting();
	local tmp = GameServer_pb.CMD_GET_ARENA_AWARD_SC();
	tmp:ParseFromString(pkg)
	Log("GET_ARENA_AWARD"..tostring(tmp))
	for i=1, #g_arenaAwardList do
		if (g_arenaAwardList[i].strUUID == tmp.strUUID)then
			table.remove(g_arenaAwardList, i);
			break;
		end
	end
	
	ArenaAward_Create();
	g_arenaInfo.bCanGetAward = tmp.bCanGetAward;
	UI_RefreshBaseWidgetByName("Arena");
end

function QUERY_ARENA_LOG(pkg)
	EndWaiting();
	local tmp = GameServer_pb.CMD_QUERY_ARENA_LOG_SC();
	tmp:ParseFromString(pkg)
	
	ArenaLog_Create(tmp.szLogItemList);
end

--规则说明
function Arena_Rule()
    local widget = UI_CreateBaseWidgetByFileName("ArenaRule.json")
    local listview = UI_GetUIListView(widget, 4650961)
    listview:removeAllItems()

    --排名说明
    local layoutInfo = UI_GetUILayout(widget, 10001):clone()
    layoutInfo:setVisible(true)
    listview:pushBackCustomItem(layoutInfo)

    --当前排名的奖励
    local layoutAward = UI_GetUILayout(widget, 10002):clone()
    layoutAward:setVisible(true)
    listview:pushBackCustomItem(layoutAward)

    Arena_GetAward(g_arenaInfo.iSelfRank)

    if g_Arena_award[1] == nil then
        UI_GetUILabel(layoutInfo, 4650966):setText(FormatString("NotRankingArea"))
        layoutAward:setVisible(false)
    else 
        if g_Arena_award[2] == g_Arena_award[3] then
            UI_GetUILabel(layoutInfo, 4650966):setText(FormatString("RankingAreaAward",FormatString("Ranking",g_Arena_award[2])))
        else
            UI_GetUILabel(layoutInfo, 4650966):setText(FormatString("RankingAreaAward",FormatString("RankingArea",g_Arena_award[2],g_Arena_award[3])))
        end
        --元宝
        UI_GetUILabel(layoutAward, 1201):setText(g_Arena_award[1].m_wing)
        --铜币
        UI_GetUILabel(layoutAward, 1202):setText(g_Arena_award[1].m_Copper)
        --荣誉
        UI_GetUILabel(layoutAward, 4650988):setText(g_Arena_award[1].m_honor)
    end

    --规则说明
    local layoutRule = UI_GetUILayout(widget, 10003):clone()
    layoutRule:setVisible(true)
    listview:pushBackCustomItem(layoutRule)

    local rule = ""

    for i=1,7,1 do
        rule = rule..FormatString("ArenaRule_"..i).."\n"
    end
    local label = UI_GetUILabel(layoutRule, 4651002)
    label:setText(rule)
    --竞技场奖励说明
    local layoutAwardInfo = UI_GetUILayout(widget, 10004):clone()
    layoutAwardInfo:setVisible(true)
    listview:pushBackCustomItem(layoutAwardInfo)
    --前十名奖励
    for i=1,10,1 do
        local layout = UI_GetUILayout(widget, 10005):clone()
        layout:setVisible(true)
        listview:pushBackCustomItem(layout)
        --名次
        UI_GetUILabel(layout, 4651014):setText(FormatString("Ranking",i))
        local award = GetGameData(DataFileArenaAward, i, "stArenaAwardData")
        --宝箱数量
        --UI_GetUILabel(layout, 4651018):setText(award.)
        --元宝数量
        UI_GetUILabel(layout, 4651022):setText(award.m_wing)
        --铜币数量
        UI_GetUILabel(layout, 4651020):setText(award.m_Copper)
        --荣誉数量
        UI_GetUILabel(layout, 4651036):setText(award.m_honor)
    end
end
--获取对应名次的奖励
function Arena_GetAward(ranking)
    local award = nil
    local rankingMin = 0
    local rankingMax = 0
    for i=1,100000,1 do
        award = GetGameData(DataFileArenaAward, i, "stArenaAwardData")
        if award ~= nil then
            local rankingArea = award.m_Ranking
            local location = string.find(rankingArea, '#')
            print(location)
            if location ~= nil  then
            print(string.sub(rankingArea, 1, location-1))
            print(string.sub(rankingArea, location+1, #rankingArea))
                rankingMin = tonumber(string.sub(rankingArea, 1, location-1))
                rankingMax = tonumber(string.sub(rankingArea, location+1, #rankingArea))
            else
                rankingMin = tonumber(rankingArea)
                rankingMax = tonumber(rankingArea)
            end
        end
        if (award == nil or (rankingMin<=ranking and  ranking<=rankingMax)) then
            g_Arena_award = {}
            g_Arena_award[1] = award
            g_Arena_award[2] = rankingMin
            g_Arena_award[3] = rankingMax

            return 
        end
    end  
end
--获取排行榜
function Arena_ShowRanking(pkg)
	EndWaiting();
	local tmp = GameServer_pb.CMD_QUERY_ARENA_RANK_SC();
	tmp:ParseFromString(pkg)
	Log("Ranking==="..tostring(tmp))
    local widget = UI_CreateBaseWidgetByFileName("ArenaRankList.json")
    UI_GetUILayout(widget, 4508835):addTouchEventListener(PublicCallBackWithNothingToDo)
    local listView = UI_GetUIListView(widget, 3)
    listView:removeAllItems()

    UI_GetUIButton(widget,1):addTouchEventListener(UI_ClickCloseCurBaseWidgetWithNoAction)

    local function checkInfo(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then
            local tag = tolua.cast(sender,"ImageView"):getTag()-1250
            local tab = GameServer_pb.CMD_USENAME_FINED_ACTOR_INFO_CS()
			tab.strActorName = tmp.szRankList[tag].strName
			Packet_Full(GameServer_pb.CMD_USENAME_FINED_ACTOR_INFO,tab)
		    ShowWaiting()
        end
    end

    for i=1,50,1 do
        local oneLayout = UI_GetUILayout(widget, 4710709):clone()
        local oneLine = UI_GetUIImageView(oneLayout, 100)
        oneLayout:setVisible(true)
        --排名
        if i<4 then
            UI_GetUIImageView(oneLine, 1):loadTexture("UIExport/DemoLogin/jingjichang/font_7_0"..i..".png")
            UI_GetUIImageView(oneLine, 1):setVisible(true)
            UI_GetUILabelBMFont(oneLine, 5):setVisible(false)
        else
            UI_GetUILabelBMFont(oneLine, 5):setText(i)
            UI_GetUILabelBMFont(oneLine, 5):setVisible(true)
            UI_GetUIImageView(oneLine, 1):setVisible(false)
        end
        --名字
        UI_GetUILabel(oneLine, 2):setText(tmp.szRankList[i].strName)
        --等级
        UI_GetUILabel(oneLine, 3):setText("Lv."..tmp.szRankList[i].iLevel)
        --战斗力
        UI_GetUILabel(oneLine, 4):setText(FormatString("CombatEffectiveness",tmp.szRankList[i].iFightValue))
        listView:pushBackCustomItem(oneLayout)

        oneLayout:setTouchEnabled(true)
        oneLayout:addTouchEventListener(checkInfo)
        oneLayout:setTag(1250+i)

        if tmp.szRankList[i].strName == GetLocalPlayer():GetEntityName() then
            oneLine:loadTexture("Common/Bg_Common_15.png")
        end
    end
end
--重置冷却时间
function Arena_Reset()
	local widget = UI_GetBaseWidgetByName("Arena")
	if (widget) then		
        UI_GetUILayout(widget, 4648002):setVisible(false)
        UI_GetUILayout(widget, 4647977):setVisible(false)

        local button = UI_GetUIButton(widget, 7)
        local function changeEnemy(sender,eventType)
            if eventType == TOUCH_EVENT_ENDED then
                Packet_Cmd(GameServer_pb.CMD_ARENA_CHANGE_CHALLENGELIST);
	            ShowWaiting();
            end
        end
        button:addTouchEventListener(changeEnemy)
        button:setTitleText(FormatString("ChangeEnemy"))
    end
end
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_OPEN_ARENA, "OPEN_ARENA" );
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_QUERY_ARENA_AWARD, "QUERY_ARENA_AWARD" );
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_GET_ARENA_AWARD, "GET_ARENA_AWARD" );
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_QUERY_ARENA_LOG, "QUERY_ARENA_LOG" );

ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_ADD_ARENACHALLENGE, "ADD_ARENACHALLENGE" );
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_ARENA_RESET, "Arena_Reset" );
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_ARENA_CHANGE_CHALLENGELIST, "OPEN_ARENA" );
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_QUERY_ARENA_RANK, "Arena_ShowRanking" );
