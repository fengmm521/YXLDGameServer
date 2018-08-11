---------------------------
--- hero test
-------------------------

local g_heroTb = {}

local function procHeroMsg(strAccount)
	return function(msg)
		--[[local msgID = msg.iCmd;
		local tmpTb = {}
		table.insert(tmpTb, msg)
		showMsg(msgID, tmpTb)
		--]]
		local body = GameServer_pb.Cmd_Sc_HeroList();
		body:ParseFromString(msg.strMsgBody)
		g_heroTb[strAccount] = body.szHeroList
	end
end

local function procHeroDesc(strAccount)
	return function(msg)
		local body = GameServer_pb.Cmd_Sc_GetHeroDesc();
		body:ParseFromString(msg.strMsgBody)
		logMsg(tostring(body) );
	end
end

local function createHeroTest(clientPlayer, cmd)
	return function()
		--[[addMsgProc(GameServer_pb.CMD_GET_HERODESC, procHeroDesc(clientPlayer:getAccount() ) );
		addMsgProc(GameServer_pb.CMD_HERO_LIST, procHeroMsg(clientPlayer:getAccount() ) );
		
		WaitMessage(clientPlayer, GameServer_pb.CMD_HERO_LIST, 10)
		
		-- 查询
		local reqBody = GameServer_pb.Cmd_Cs_GetHeroDesc();
		reqBody.dwObjectID = g_heroTb[clientPlayer:getAccount()][1].dwObjectID;
		sendMsgHelp(clientPlayer, GameServer_pb.CMD_GET_HERODESC, reqBody)
		--]]
		
		local legionName = ""
		local function getLegionName(param)
			legionName = param
		end
		string.gsub(cmd, "#(%w+)", getLegionName)
		print("LegionName:"..legionName)
		-- 发送创建军团信息
		WaitMessage(clientPlayer, GameServer_pb.CMD_ROLE_FIN, 10)
		print("-----------");
		clientPlayer:sendGmMsg("setlifeatt#1#100000000");
		
		local reqBody = GameServer_pb.CMD_CREATE_LEGION_CS();
		reqBody.strLegionName = legionName
		sendMsgHelp(clientPlayer, GameServer_pb.CMD_CREATE_LEGION, reqBody)
		
		WaitTime(1)
		sendMsgHelp(clientPlayer, GameServer_pb.CMD_QUERY_LEGIONBATTLE)
		
		local function showLegionBattle(msg)
			local body = GameServer_pb.CMD_QUERY_LEGIONBATTLE_SC();
			body:ParseFromString(msg.strMsgBody)
			print(tostring(body) )
		end
		
		print("------------------------")
		clientPlayer:sendGmMsg("setlegionlevel#4");
		addMsgProc(clientPlayer, GameServer_pb.CMD_QUERY_LEGIONBATTLE, showLegionBattle);
		
		print("查询势力范围")
		sendMsgHelp(clientPlayer, GameServer_pb.CMD_QUERY_BATTLEOWNER)
		
		local function showCityOwner(msg)
			local body = GameServer_pb.CMD_QUERY_BATTLEOWNER_SC();
			body:ParseFromString(msg.strMsgBody)
			print(tostring(body) )
		end
		addMsgProc(clientPlayer, GameServer_pb.CMD_QUERY_BATTLEOWNER, showCityOwner);
		
		
		sendMsgHelp(clientPlayer, GameServer_pb.CMD_QUERY_LASTBATTLE_LIST)
		local function showLastBattle(msg)
			local body = GameServer_pb.CMD_QUERY_LASTBATTLE_LIST_SC();
			body:ParseFromString(msg.strMsgBody)
			print(tostring(body) )
		end
		addMsgProc(clientPlayer, GameServer_pb.CMD_QUERY_LASTBATTLE_LIST, showLastBattle);
		
		--WaitTime(10000);
		
		print("查询宣战:");
		WaitTime(1)
		sendMsgHelp(clientPlayer, GameServer_pb.CMD_QUERY_LEGIONBATTLE_REPORT)
		local function showQueryReport(msg)
			local body = GameServer_pb.CMD_QUERY_LEGIONBATTLE_REPORT_SC();
			body:ParseFromString(msg.strMsgBody)
			print(tostring(body) )
		end
		addMsgProc(clientPlayer, GameServer_pb.CMD_QUERY_LEGIONBATTLE_REPORT, showQueryReport);
		
		print("宣战");
		WaitTime(1)
		local reqBody = GameServer_pb.CMD_REPORT_LEGIONBATTLE_CS()
		reqBody.iCityID = 1;
		sendMsgHelp(clientPlayer, GameServer_pb.CMD_REPORT_LEGIONBATTLE, reqBody)
		
		local function showReportSc(msg)
			local body = GameServer_pb.CMD_REPORT_LEGIONBATTLE_SC();
			body:ParseFromString(msg.strMsgBody)
			print(tostring(body) )
		end
		addMsgProc(clientPlayer, GameServer_pb.CMD_REPORT_LEGIONBATTLE, showReportSc);
		
		print("开战");
		clientPlayer:sendGmMsg("startlegionfight");
		--[[print("参战")
		WaitTime(1)
		sendMsgHelp(clientPlayer, GameServer_pb.CMD_JOIN_LEGIONBATTLE)
		local function showJoinSc(msg)
			local body = GameServer_pb.CMD_JOIN_LEGIONBATTLE_SC();
			body:ParseFromString(msg.strMsgBody)
			print(tostring(body) )
		end
		addMsgProc(GameServer_pb.CMD_JOIN_LEGIONBATTLE, showJoinSc);
		--]]
		
		
		--sendMsgHelp(clientPlayer, GameServer_pb.CMD_QUERY_LEGIONBATTLE_REPORT)
		
		--[[WaitTime(1)
		sendMsgHelp(clientPlayer, GameServer_pb.CMD_CALL_LEGIONBOSS)
		
		while true do
			WaitTime(1)
			sendMsgHelp(clientPlayer, GameServer_pb.CMD_FIGHT_LEGIONBOSS)
		end--]]
		--[[local reqBody = GameServer_pb.CMD_QUERY_LEGIONLIST_CS();
		reqBody.iPageIndex = 0;
		sendMsgHelp(clientPlayer, GameServer_pb.CMD_QUERY_LEGIONLIST, reqBody)
		--]]
		-- 查询
		
	--[[
		addMsgProc(GameServer_pb.CMD_HERO_LIST, procHeroMsg(clientPlayer:getAccount() ) );
		WaitMessage(clientPlayer, GameServer_pb.CMD_ROLE_FIN, 10)
		
		local body = GameServer_pb.Cmd_Cs_HeroConvert();
		local strAccount = clientPlayer:getAccount()
		body.dwHeroObjectID = g_heroTb[strAccount][1].dwObjectID;
		sendMsgHelp(clientPlayer, GameServer_pb.CMD_HERO_CONVERT, body)
		
		local result, msgTb = WaitMessage(clientPlayer, GameServer_pb.CMD_HERO_UPDATE, 10)
		if result == false then
			return;
		end
		local msg = msgTb[1];
		local newBoby = GameServer_pb.Cmd_Sc_HeroUpdate();
		newBoby:ParseFromString(msg.strMsgBody)
		logMsg(tostring(newBoby) );
		--]]
	end
end


function HeroTest(clientPlayer, args)
	local cmd = args.context:getString("cmd", "")
	local objID = clientPlayer:getAccount();
	if g_croutineTb[objID] == nil then
		g_croutineTb[objID] = coroutine.create(createHeroTest(clientPlayer, cmd));
		coroutine.resume(g_croutineTb[objID]);
	else
		if coroutine.status(g_croutineTb[objID]) == "dead" then
			return;
		end	
	
		local result, errmsg = coroutine.resume(g_croutineTb[objID]);
		if false == result then
			logMsg("Script ResumeFail:"..errmsg)
		end
	end
end