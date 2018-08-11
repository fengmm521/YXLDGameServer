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
		logMsg("Error:"..err);
	end
	
	print("function|"..tostring(f) )
	return f();
end


local function createWorldBossTest(clientPlayer)
	return function()
		WaitMessage(clientPlayer, GameServer_pb.CMD_ROLE_FIN, 10)
		WaitTime(100)
		
		print("角色登录成功，准备查询世界BOSS")
		sendMsgHelp(clientPlayer, GameServer_pb.CMD_QUERY_WORLDBOSS);
		
		addMsgProc(clientPlayer, GameServer_pb.CMD_QUERY_WORLDBOSS, mkShowMsg("CMD_QUERY_WORLDBOSS_SC") )
		WaitMessage(clientPlayer, GameServer_pb.CMD_QUERY_WORLDBOSS, 10)
		
		-- 战斗
		while true do
			sendMsgHelp(clientPlayer, GameServer_pb.CMD_FIGHT_WORLDBOSS);
			WaitTime(20)
		end
		
		--[[
		print("查询获奖记录")
		sendMsgHelp(clientPlayer, GameServer_pb.CMD_QUERY_ARENA_AWARD);
		addMsgProc(GameServer_pb.CMD_QUERY_ARENA_AWARD, mkShowMsg("CMD_QUERY_ARENA_AWARD_SC") )
		WaitMessage(clientPlayer, GameServer_pb.CMD_QUERY_ARENA_AWARD, 10)
		
		-- 挑战吧
		print("挑战")
		local body = GameServer_pb.CMD_ARENA_FIGHT_CS()
		body.iRank = 2;
		sendMsgHelp(clientPlayer, GameServer_pb.CMD_ARENA_FIGHT, body);
		addMsgProc(GameServer_pb.CMD_COMM_FIGHTRESULT, mkShowMsg("Cmd_Sc_CommFightResult") )
		WaitMessage(clientPlayer, GameServer_pb.CMD_COMM_FIGHTRESULT, 10)
		--]]
	end
end

function WorldBossTest(clientPlayer, args)
	--local cmd = args.context:getString("cmd", "")
	local objID = clientPlayer:getAccount();
	if g_croutineTb[objID] == nil then
		g_croutineTb[objID] = coroutine.create(createWorldBossTest(clientPlayer));
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