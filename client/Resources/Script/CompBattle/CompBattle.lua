--创建阵营战界面
function CompBattle_Create()
	local widget = UI_CreateBaseWidgetByFileName("CompBattle.json");
	if (widget == nil)then
		Log("CompBattle_Create error");
		return;
	end
	
	--g_battleLog = {};
	UI_GetUILayout(widget, 4760313):addTouchEventListener(PublicCallBackWithNothingToDo)
	local infoLayout = UI_GetUILayout(widget, 102);
	--关闭
	local function ClickClose(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then			
			Packet_Cmd(GameServer_pb.CMD_CAMPBATTLE_CLOSE);
			g_battleAutoFight = false;
            g_CompBattle_CheckBox = false
			UI_ClickCloseCurBaseWidgetWithNoAction(sender,eventType);
		end
	end
	UI_GetUIButton(widget, 1):addTouchEventListener(ClickClose);
	--排队
	local function ClickBattle(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then			
			Packet_Cmd(GameServer_pb.CMD_CAMPBATTLE_JOIN);
			ShowWaiting()
		end
	end	
	UI_GetUIButton(infoLayout, 21):addTouchEventListener(ClickBattle);
	--取消排队
	local function ClickBattleCancle(sender, eventType)
		if (eventType == TOUCH_EVENT_ENDED) then			
			Packet_Cmd(GameServer_pb.CMD_CAMPBATTLE_JOIN_CANCLE);
			g_battleAutoFight = false;
			ShowWaiting()
		end
	end	
	UI_GetUIButton(infoLayout, 22):addTouchEventListener(ClickBattleCancle);
	
	UI_GetUICheckBox(infoLayout, 9):addEventListenerCheckBox(CompBattle_RefreshLog);

    if g_CompBattle_CheckBox then
        UI_GetUICheckBox(infoLayout, 9):setSelectedState(true)
    end

	--自己的阵营战信息
	local selfListView = UI_GetUIListView(widget, 10);
	for i=1, #g_battleSelfLog do
		selfListView:pushBackCustomItem(GetLogLayout(g_battleSelfLog[i]));
		selfListView:refreshView();
		selfListView:scrollToBottom(1, false);
	end
	--所有人的阵营战信息
	local allListView = UI_GetUIListView(widget, 3);
	for i=1, #g_battleLog do
		allListView:pushBackCustomItem(GetLogLayout(g_battleLog[i]));
		allListView:refreshView();
		allListView:scrollToBottom(1, false);
	end
	
	if (g_battleAutoFight)then
		Packet_Cmd(GameServer_pb.CMD_CAMPBATTLE_JOIN);
		ShowWaiting();
	end
	
	CompBattle_Refresh(widget);
end
--刷新数据
function CompBattle_Refresh(widget)
	CompBattle_RefreshLog()
	CompBattle_RefreshRankList()
	CompBattle_RefreshDetail()
	CompBattle_RefreshTime()
end
--刷新战斗信息
function CompBattle_RefreshLog()
	local widget = UI_GetBaseWidgetByName("CompBattle");
	if (widget)then
		local infoLayout = UI_GetUILayout(widget, 102);
		local listView = UI_GetUIListView(widget, 3);
		local checkBox = UI_GetUICheckBox(infoLayout, 9);
		local selfListView = UI_GetUIListView(widget, 10);
		
		if (checkBox:getSelectedState())then
			listView:setVisible(false);
			selfListView:setVisible(true);

            g_CompBattle_CheckBox=true
		else
			listView:setVisible(true);
			selfListView:setVisible(false);

            g_CompBattle_CheckBox = false
		end
	end
end
--刷新排行榜
function CompBattle_RefreshRankList()
	local widget = UI_GetBaseWidgetByName("CompBattle");
	if (widget)then
		--[[local leftListView = UI_GetUIListView(widget, 2);
		local rightListView = UI_GetUIListView(widget, 4);
		leftListView:removeAllItems();
		rightListView:removeAllItems();]]
		local leftLayout = UI_GetUILayout(widget, 100);
		local rightLayout = UI_GetUILayout(widget, 101);
		
		for i=1, 4 do
			UI_GetUIImageView(leftLayout, i):setVisible(false);
			UI_GetUIImageView(rightLayout, i):setVisible(false);
		end
		for i=1, #g_battleLeftList do
            if i>=5 then 

            else
			    local layout = UI_GetUIImageView(leftLayout, i);
			    layout:setVisible(true);

			    local data = GetGameData(DataFileHeroBorn, g_battleLeftList[i].iMainHeroId, "stHeroBornData")

                local widgetScene = UI_GetBaseWidgetByName("MainUi")
                local headImgScene = CCSprite:create("Icon/HeroIcon/x"..data.m_icon..".png")
                local headIconScene = UIMgr:GetInstance():createMaskedSprite(headImgScene,"zhujiemian/bg_4.png")
	      --      UI_GetUIImageView(widgetScene, 2):removeNodeByTag(2);
	      --      UI_GetUIImageView(widgetScene, 2):addNode(headIconScene, 0, 2);

			    --local icon = UI_GetHeroIcon(data.m_icon, i);
			    UI_GetUILayout(layout, 1):removeAllChildren();
                headIconScene:setScale(UI_GetUILayout(layout, 1234):getScaleX())
			    UI_GetUILayout(layout, 1):addNode(headIconScene, 0, 2);

			    UI_GetUILabel(layout, 2):setText(g_battleLeftList[i].strActorName);
			
			    UI_GetUILabel(layout, 3):setText(g_battleLeftList[i].iActorHonor);
            end
		end
		
		for i=1, #g_battleRightList do
            if i>=5 then

            else 
			    local layout = UI_GetUIImageView(rightLayout, i);
			    layout:setVisible(true);
			    --layout:loadTexture("zhenyingzhan/bg_2_0"..i..".png")

			    local data = GetGameData(DataFileHeroBorn, g_battleRightList[i].iMainHeroId, "stHeroBornData")
			    local widgetScene = UI_GetBaseWidgetByName("MainUi")
                local headImgScene = CCSprite:create("Icon/HeroIcon/x"..data.m_icon..".png")
                local headIconScene = UIMgr:GetInstance():createMaskedSprite(headImgScene,"zhujiemian/bg_4.png")
	      --      UI_GetUIImageView(widgetScene, 2):removeNodeByTag(2);
	      --      UI_GetUIImageView(widgetScene, 2):addNode(headIconScene, 0, 2);

			    --local icon = UI_GetHeroIcon(data.m_icon, i);
			    UI_GetUILayout(layout, 1):removeAllChildren();
                headIconScene:setScale(UI_GetUILayout(layout, 1234):getScaleX())
			    UI_GetUILayout(layout, 1):addNode(headIconScene, 0, 2);

			    UI_GetUILabel(layout, 2):setText(g_battleRightList[i].strActorName);
			
			    UI_GetUILabel(layout, 3):setText(g_battleRightList[i].iActorHonor);
            end
		end
	end
end
--刷新玩家自己的阵营战数据
function CompBattle_RefreshDetail()
	local widget = UI_GetBaseWidgetByName("CompBattle");
	if (widget)then
		local infoLayout = UI_GetUILayout(widget, 102);
		UI_GetUILabel(infoLayout, 11):setText(FormatString("Battle_BestLiansha", g_actorDetail.iTopDoubleHitTimes));
		UI_GetUILabel(infoLayout, 12):setText(FormatString("Battle_CurLiansha", g_actorDetail.iCurDoubleHitTimes));
		UI_GetUILabel(infoLayout, 13):setText(FormatString("Battle_WinCount", g_actorDetail.iSumWinTimes));
		UI_GetUILabel(infoLayout, 14):setText(FormatString("Battle_LoseCount", g_actorDetail.iSumFailedTimes));
		UI_GetUILabel(infoLayout, 15):setText(FormatString("Battle_Honor", g_actorDetail.iSumHonor));
		UI_GetUILabel(infoLayout, 16):setText(FormatString("Battle_Tongbi", g_actorDetail.iSumSilverCount));
	end
end
--刷新开始或者结束时间
function CompBattle_RefreshTime()
	local widget = UI_GetBaseWidgetByName("CompBattle");
	if (widget)then
		--local infoLayout = UI_GetUILayout(widget, 102);
		if (g_battleTime.bIsStarted)then
			UI_GetUILabel(widget, 8):setVisible(true);
			UI_GetUILabel(widget, 7):setVisible(false);
		else
			UI_GetUILabel(widget, 7):setVisible(true);
			UI_GetUILabel(widget, 8):setVisible(false);
		end
		
		UI_GetUILabel(widget, 6):setTimer(g_battleTime.iRemaindSecond,TimeFormatType_HourMinSec);
	end
end
--请求打开阵营战界面的服务器回包
function CAMPBATTLE_OPEN(pkg)
	EndWaiting();
	local tmp = GameServer_pb.CMD_CAMPBATTLE_OPEN_SC();
	tmp:ParseFromString(pkg)
	Log("*********"..tostring(tmp))
	
	g_battleLeftList = tmp.szLeftRankList;
	g_battleRightList = tmp.szRightRankList;
	g_battleTime = tmp.remaindTime;
	g_actorDetail = tmp.playerDetail;
	
	CompBattle_Create();
end
--所有人的阵营战信息
if (g_battleLog == nil) then
	g_battleLog = {}
end
--自己的阵营战信息
if (g_battleSelfLog == nil) then
	g_battleSelfLog = {}
end
--或者单条战斗的信息layout
function GetLogLayout(campBattleReport)
	local layout = Layout:create();
	local winName = ""
	local loseName = ""
	
	if (campBattleReportiWinerCampId == 1)then
		winName = FormatString("Battle_Left", campBattleReport.strWinActorName)
		loseName = FormatString("Battle_Right", campBattleReport.strFailedActorName)
	else
		winName = FormatString("Battle_Right", campBattleReport.strWinActorName)
		loseName = FormatString("Battle_Left", campBattleReport.strFailedActorName)
	end
	
	local info = "";
	if (campBattleReport.iBattleResoultType == GameServer_pb.en_CampBattleResoultType_DoubleHit)then
		info = FormatString("Battle_Info3", winName, loseName, campBattleReport.iCurDoubleHitTimes)
	elseif (campBattleReport.iBattleResoultType == GameServer_pb.en_CampBattleResoultType_Win)then
		info = FormatString("Battle_Info1", winName, loseName)
	elseif (campBattleReport.iBattleResoultType == GameServer_pb.en_CampBattleResoultType_WinEnd)then
		info = FormatString("Battle_Info2", winName, loseName, campBattleReport.iCurDoubleHitTimes)
	end
	
	local award = ""
	local tongi = FormatString("Battle_Count", campBattleReport.iRewardSilver);
	local honor = FormatString("Battle_Count", campBattleReport.iRewardHonor);
	
	if (campBattleReport.iRewardHonor == 0) then
		if (campBattleReport.iRewardSilver == 0)then
			award = FormatString("Battle_Award4");
		else	
			award = FormatString("Battle_Award3", tongi);
		end
	else
		if (campBattleReport.iRewardSilver == 0)then
			award = FormatString("Battle_Award2", honor);
		else	
			award = FormatString("Battle_Award1", tongi, honor);
		end
	end
	
	local label = CompLabel:GetDefaultCompLabel(info..award, 300);
	label:setPosition(ccp(5, label:getSize().height + 7));
	layout:addChild(label);

	layout:setSize(CCSizeMake(320, label:getSize().height + 14));
	
	return layout;
end
--服务器推送的战斗信息
function CAMPBATTLE_BATTLE_LOG(pkg)
	EndWaiting();
	local tmp = GameServer_pb.CMD_CAMPBATTLE_BATTLE_LOG_SC();
	tmp:ParseFromString(pkg)
	Log("CAMPBATTLE_BATTLE_LOG"..tostring(tmp))
	
	local widget = UI_GetBaseWidgetByName("CompBattle");
	for i=1, #tmp.szBattleReport do
		if (#g_battleLog >= 50)then
			table.remove(g_battleLog, 1);
		end
		g_battleLog[#g_battleLog + 1] = tmp.szBattleReport[i];
		if (widget)then
			local listView = UI_GetUIListView(widget, 3);
			listView:pushBackCustomItem(GetLogLayout(g_battleLog[#g_battleLog]));
			listView:refreshView();
			listView:scrollToBottom(1, false);
		end
		
		if (tmp.szBattleReport[i].strWinActorName == GetLocalPlayer():GetEntityName() or tmp.szBattleReport[i].strFailedActorName == GetLocalPlayer():GetEntityName()) then
			if (#g_battleSelfLog >= 50)then
				table.remove(g_battleSelfLog, 1);
			end
			g_battleSelfLog[#g_battleSelfLog + 1] = tmp.szBattleReport[i];
			
			if (widget) then
				local infoLayout = UI_GetUILayout(widget, 102);
				local selfListView = UI_GetUIListView(widget, 10);
				selfListView:pushBackCustomItem(GetLogLayout(g_battleSelfLog[#g_battleSelfLog]));
				selfListView:refreshView();
				selfListView:scrollToBottom(1, false);
			end
		end
	end
end

if (g_battleLeftList == nil) then
	g_battleLeftList = {}
end

if (g_battleRightList == nil) then
	g_battleRightList = {}
end
--服务器推送的排行榜信息
function CAMPBATTLE_REFREASH_RANKLIST(pkg)
	EndWaiting();
	local tmp = GameServer_pb.CMD_CAMPBATTLE_REFREASH_RANKLIST_SC();
	tmp:ParseFromString(pkg)
	Log("CAMPBATTLE_REFREASH_RANKLIST"..tostring(tmp))
	g_battleLeftList = tmp.szLeftRankList;
	g_battleRightList = tmp.szRightRankList;
	
	CompBattle_RefreshRankList();
end

if (g_actorDetail == nil) then
	g_actorDetail = {}
end
--玩家自己的战斗信息
function CAMPBATTLE_REFREASH_ACTORDETAIL(pkg)
	EndWaiting();
	local tmp = GameServer_pb.CMD_CAMPBATTLE_REFREASH_ACTORDETAIL_SC();
	tmp:ParseFromString(pkg)
	Log("CAMPBATTLE_REFREASH_ACTORDETAIL---"..tostring(tmp))
	
	g_actorDetail = tmp.playerDetail;
	
	CompBattle_RefreshDetail();
end

if (g_battleTime == nil) then
	g_battleTime = {}
end
--服务器发送的时间信息
function CAMPBATTLE_REFREASH_TIME(pkg)
	EndWaiting();
	local tmp = GameServer_pb.CMD_CAMPBATTLE_REFREASH_TIME_SC();
	tmp:ParseFromString(pkg)
	Log("*********"..tostring(tmp))
	
	g_battleTime = tmp.remaindTime
	
	CompBattle_RefreshTime()
end
--排队的服务器回包
function CAMPBATTLE_JOIN(pkg)
	Log("CAMPBATTLE_JOIN")
	EndWaiting();
	local widget = UI_GetBaseWidgetByName("CompBattle");
	if (widget)then
		local infoLayout = UI_GetUILayout(widget, 102);
		UI_GetUIButton(infoLayout, 22):setVisible(true);
		UI_GetUIButton(infoLayout, 21):setVisible(false);
		
		g_battleAutoFight = true;
		g_openUIType = EUIOpenType_CompBattle
	end
end
--取消排队的服务器回包
function CAMPBATTLE_JOIN_CANCLE(pkg)
	EndWaiting();
	local widget = UI_GetBaseWidgetByName("CompBattle");
	if (widget)then
		local infoLayout = UI_GetUILayout(widget, 102);
		UI_GetUIButton(infoLayout, 22):setVisible(false);
		UI_GetUIButton(infoLayout, 21):setVisible(true);
	end
	
	g_battleAutoFight = false;
	g_openUIType = EUIOpenType_None
end

if (g_battleReult == nil) then
	g_battleReult = {}
end

g_battleAutoFight=false;
--阵营战结束
function CMAPBATTLE_SYSTEM_CLOSE(pkg)
	EndWaiting();
	local tmp = GameServer_pb.CMD_CMAPBATTLE_SYSTEM_CLOSE_SC();
	tmp:ParseFromString(pkg)
	Log("*********"..tostring(tmp))
	g_battleReult = tmp;
	
	if (SceneMgr:GetInstance():GetGameState() ~= EGameState_MainUi) then
		g_openUIType = EUIOpenType_CompBattleResult;	
	else
		CompBattle_CreateResult();
	end
end

function CompBattle_CreateResult()
	local tab = {};
	tab.info = FormatString("Battle_Result", g_battleReult.iTopDoubleHitCount, g_battleReult.iWinTimes, g_battleReult.iFailedTimes, g_battleReult.iRewardHonor, g_battleReult.iRewardSilver);
	tab.leftFun = CompBattle_ClikComfire;
	tab.msgType = EMessageType_Middle;
	
	Messagebox_Create(tab)
end

function CompBattle_ClikComfire()
	if (UI_IsCurBaseWidgetByName("CompBattle"))then
		UI_CloseCurBaseWidget();
	end
end

ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_CAMPBATTLE_JOIN, "CAMPBATTLE_JOIN" );
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_CAMPBATTLE_JOIN_CANCLE, "CAMPBATTLE_JOIN_CANCLE" );
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_CAMPBATTLE_OPEN, "CAMPBATTLE_OPEN" );
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_CAMPBATTLE_BATTLE_LOG, "CAMPBATTLE_BATTLE_LOG" );
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_CAMPBATTLE_REFREASH_RANKLIST, "CAMPBATTLE_REFREASH_RANKLIST" );
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_CAMPBATTLE_REFREASH_ACTORDETAIL, "CAMPBATTLE_REFREASH_ACTORDETAIL" );
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_CAMPBATTLE_REFREASH_TIME, "CAMPBATTLE_REFREASH_TIME" );
ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_CMAPBATTLE_SYSTEM_CLOSE, "CMAPBATTLE_SYSTEM_CLOSE" );
