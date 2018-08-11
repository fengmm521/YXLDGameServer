--------------------------
--
------------------------------

local function mkShowMsg(cmdTxt)
	local code = "return function(msg)\
			local body = GameServer_pb."..cmdTxt.."();\
			body:ParseFromString(msg.strMsgBody)\
			print(tostring(body) )\
		end";
		
	local f, err = loadstring(code);
	if f == nil then
		print("XXX|"..err)
		print("Error:"..err);
	end
	
	print("function|"..tostring(f) )
	return f();
end


local function createArenaTest(clientPlayer)
	return function()
		WaitMessage(clientPlayer, GameServer_pb.CMD_ROLE_FIN, 10)
		print("角色登录成功，准备查询竞技场")
		sendMsgHelp(clientPlayer, GameServer_pb.CMD_OPEN_ARENA);
		
		local szChallengeList = {};
		local function openArenaProcess(msg)
			local body = GameServer_pb.CMD_OPEN_ARENA_SC();
			body:ParseFromString(msg.strMsgBody);
			for i = 1, #(body.challengeInfo.szChallengeList) do
				szChallengeList[i] = body.challengeInfo.szChallengeList[i].iRank;
				print("ChallengeRank|"..szChallengeList[i])
			end
		end
		
		addMsgProc(clientPlayer, GameServer_pb.CMD_OPEN_ARENA, openArenaProcess)
		--addMsgProc(clientPlayer, GameServer_pb.CMD_OPEN_ARENA, mkShowMsg("CMD_OPEN_ARENA_SC") )
		WaitMessage(clientPlayer, GameServer_pb.CMD_OPEN_ARENA, 10)
		
		
		--[[sendMsgHelp(clientPlayer, GameServer_pb.CMD_QUERY_ARENA_LOG);
		addMsgProc(clientPlayer, GameServer_pb.CMD_QUERY_ARENA_LOG, mkShowMsg("CMD_QUERY_ARENA_LOG_SC") )
		--]]
		--CMD_QUERY_ARENA_LOG_SC
		
		-- 查询
		--[[print("查询获奖记录")
		sendMsgHelp(clientPlayer, GameServer_pb.CMD_QUERY_ARENA_AWARD);
		addMsgProc(clientPlayer, GameServer_pb.CMD_QUERY_ARENA_AWARD, mkShowMsg("CMD_QUERY_ARENA_AWARD_SC") )
		WaitMessage(clientPlayer, GameServer_pb.CMD_QUERY_ARENA_AWARD, 10)
		--]]
		
		-- 挑战吧
		print("准备挑战")
		print("tbSize|"..tostring(table.getn(szChallengeList) ) )
		if table.getn(szChallengeList) == 0 then
			WaitTime(10000)
		end
		
		print("挑战")
		local rand = math.random(1, #szChallengeList)
		local body = GameServer_pb.CMD_ARENA_FIGHT_CS()
		body.iRank = szChallengeList[rand];
		sendMsgHelp(clientPlayer, GameServer_pb.CMD_ARENA_FIGHT, body);
		addMsgProc(clientPlayer, GameServer_pb.CMD_COMM_FIGHTRESULT, mkShowMsg("Cmd_Sc_CommFightResult") )
		WaitMessage(clientPlayer, GameServer_pb.CMD_COMM_FIGHTRESULT, 10)
		
	end
end

function ArenaTest(clientPlayer, args)
	--local cmd = args.context:getString("cmd", "")
	local objID = clientPlayer:getAccount();
	if g_croutineTb[objID] == nil then
		g_croutineTb[objID] = coroutine.create(createArenaTest(clientPlayer));
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