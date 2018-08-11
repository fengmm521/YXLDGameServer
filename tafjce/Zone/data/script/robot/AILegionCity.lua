





local g_createdLegion = {}


function createLegionCityTest(clientPlayer, cmd)
	local paramListTb = {};
	local function parseParam(param)
		table.insert(paramListTb, param)
	end
	
	string.gsub(cmd, "(%w+)#?", parseParam);
	local strLegionName;
	local bCreateLegion = false;	
	if #paramListTb == 1 then
		strLegionName = paramListTb[1];
	end
	if #paramListTb == 2 then
		bCreateLegion = tonumber(paramListTb[2]) == 1
	end

	return function()
		WaitMessage(clientPlayer, GameServer_pb.CMD_ROLE_FIN, 10)
		if bCreateLegion == true then
			local reqBody = GameServer_pb.CMD_CREATE_LEGION_CS();
			reqBody.strLegionName = strLegionName;
			sendMsgHelp(clientPlayer, GameServer_pb.CMD_CREATE_LEGION, reqBody);
			
			local function procsLegionBase(msg)
				local body = GameServer_pb.CMD_QUERY_LEGION_BASE_SC();
				body:ParseFromString(msg.strMsgBody)\
				g_createdLegion[body.baseInfo.strLegionName] = body.baseInfo.dwLegionObjectID;
				logMsg("Create LegionSuccess|"..body.baseInfo.strLegionName);
			end
			addMsgProc(clientPlayer, GameServer_pb.CMD_QUERY_LEGION_BASE, procsLegionBase);
			WaitMessage(clientPlayer, GameServer_pb.CMD_QUERY_LEGION_BASE, 10)
		end
		
		local legionObjectID = g_createdLegion[strLegionName];
		if legionObjectID == nil then
			return;
		end
		
		
	end
end





function LegionCityTest(clientPlayer, args)
	local objID = clientPlayer:getAccount();
	local cmd = args.context:getString("cmd", "")
	if g_croutineTb[objID] == nil then
		g_croutineTb[objID] = coroutine.create(createLegionCityTest(clientPlayer, cmd));
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