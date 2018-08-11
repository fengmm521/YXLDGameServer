------------------
--- 阵形测试
------------------

local s_formationData = {}
local s_heroTb = {}

local function parseFormation(strAccount, msg)
	
	local scFormation = GameServer_pb.Cmd_Sc_FormationData()
	scFormation:ParseFromString(msg.strMsgBody)
	
	local formationList = scFormation.szFormationList
	for i = 1, table.getn(formationList) do
		local item = formationList[i];
		s_formationData[item.iPos] = item.dwObjectID;
		logMsg(strAccount.."|Formation|"..item.iPos.."|"..item.dwObjectID);
	end
end

local function parseHeroData(strAccount, msg)
	local scHero = GameServer_pb.Cmd_Sc_HeroList();
	scHero:ParseFromString(msg.strMsgBody)
	print(tostring(scHero) )
	for i=1, #scHero.szHeroList do
		local item = scHero.szHeroList[i]
		s_heroTb[item.dwObjectID] = item
		logMsg(strAccount.."|hero|"..item.dwObjectID)
	end
end

local function parseHeroDesc(strAccount, msg)
	local scHero = GameServer_pb.Cmd_Sc_GetHeroDesc();
	print("desc|"..string.len(msg.strMsgBody) )
	scHero:ParseFromString(msg.strMsgBody)
	print("desc|"..tostring(scHero) )
end

local function processMsg(strAccount, msgTb)
	for i=1, #msgTb do
		local msg = msgTb[i];
		if msg.iCmd == GameServer_pb.CMD_FORMATION_DATA then
			parseFormation(strAccount, msg)
		elseif msg.iCmd == GameServer_pb.CMD_HERO_LIST then
			parseHeroData(strAccount, msg);
		elseif msg.iCmd == GameServer_pb.CMD_GET_HERODESC then
			parseHeroDesc(strAccount, msg);
		end
	end
end 

local function getHeroDesc(player, objectID)
	local reqBody = GameServer_pb.Cmd_Cs_GetHeroDesc();
	reqBody.dwObjectID = objectID
	sendMsgHelp(player, GameServer_pb.CMD_GET_HERODESC, reqBody);
end


function createFormationTest(player)
	return function()
		local strAccount = player:getAccount();
		while true do
			-- wait formation data or hero data
			local result, msgID, msgData = WaitMultMessage(player, 10, GameServer_pb.CMD_FORMATION_DATA, GameServer_pb.CMD_HERO_LIST);
			if result == false then
				logMsg(strAccount.."|formationTest Fail|no formation or hero data")
				break;
			end
			logMsg(strAccount.."|"..msgID)
			processMsg(strAccount, msgData)
			
			result, msgID, msgData = WaitMultMessage(player, 10, GameServer_pb.CMD_FORMATION_DATA, GameServer_pb.CMD_HERO_LIST);
			if result == false then
				logMsg(strAccount.."|formationTest Fail|no formation or hero data")
				break;
			end
			logMsg(strAccount.."|"..msgID)
			processMsg(strAccount, msgData)
			-- wait fin
			result, msgData = WaitMessage(player, GameServer_pb.CMD_ROLE_FIN, 10);
			if result == false then
				logMsg(strAccount.."|recv fin fail")
				break;
			end 
			
			-- 添加新英雄
			player:sendGmMsg("addHero#2")
			result, data = WaitMessage(player, GameServer_pb.CMD_HERO_UPDATE, 10)
			if result == false then
				logMsg(strAccount.."|recv hero add fail")
				break;
			end
			local newMsg = data[1];
			local scHeroUpdate = GameServer_pb.Cmd_Sc_HeroUpdate();
			scHeroUpdate:ParseFromString(newMsg.strMsgBody)
			local newHeroData = scHeroUpdate.heroData;
			
			s_heroTb[newHeroData.dwObjectID] = newHeroData;
			logMsg(strAccount.."|recv new hero|"..newHeroData.dwObjectID)
			
			getHeroDesc(player, newHeroData.dwObjectID)
			result, dataList = WaitMessage(player, GameServer_pb.CMD_GET_HERODESC, 10)
			processMsg(strAccount, dataList);
			
			
			-- 上阵
			local chgReq = GameServer_pb.Cmd_Cs_FormationChg();
			for i = 0, 8 do
				if s_formationData[i] == nil then
					chgReq.dwObjectID = newHeroData.dwObjectID;
					chgReq.iTargetPos = i;
					break;
				end
			end
			sendMsgHelp(player, GameServer_pb.CMD_FORMATION_CHG, chgReq);
			
			
			
			logMsg(strAccount.."|get formationData success");
			break;
		end
	end
end

function FormationTest(clientPlayer, args)
	local objID = clientPlayer:getAccount();
	if g_croutineTb[objID] == nil then
		g_croutineTb[objID] = coroutine.create(createFormationTest(clientPlayer));
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