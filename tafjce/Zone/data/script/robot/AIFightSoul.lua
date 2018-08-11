-----------------
-- 武魂测试
-----------------------



local function showMsg(msgID, msgList)
	for i = 1, #msgList do
		local tmpMsg = msgList[i]
		local body = nil
		if msgID == GameServer_pb.CMD_CHG_FSCONTAINER then
			body = GameServer_pb.Cmd_Sc_ChgFsContainer();
		elseif msgID == GameServer_pb.CMD_FIGHTSOUL_PRACTICE then
			body = GameServer_pb.Cmd_Sc_FightSoulPractice();
		elseif msgID == GameServer_pb.CMD_FIGHTSOUL_BAG then
			body = GameServer_pb.Cmd_Sc_FightSoulBag();
		elseif msgID == GameServer_pb.CMD_ATT_CHG then
			body = GameServer_pb.Cmd_Sc_AttChg();
		elseif msgID == GameServer_pb.CMD_HERO_LIST then
			body = GameServer_pb.Cmd_Sc_HeroList();
		end
		body:ParseFromString(tmpMsg.strMsgBody)
		logMsg("msgID|"..msgID);
		logMsg(tostring(body) );
	end
end


local function waitAndShowFightSoulMsg(clientPlayer, second, ...)
	local result, msgID, msgData = WaitMultMessage(clientPlayer, 10, ...);
	if result == false then
		print("wait fail")
		return;
	end
	showMsg(msgID, msgData);
end

local g_heroTb ={}
local g_fightSoulBg = {}

local function procBagMsg(strAccount)
	return function(msg)
		local msgID = msg.iCmd;
		local tmpTb = {}
		table.insert(tmpTb, msg)
		showMsg(msgID, tmpTb)
		
		local body = GameServer_pb.Cmd_Sc_FightSoulBag();
		body:ParseFromString(msg.strMsgBody)
		g_fightSoulBg[strAccount] = body.szFightSoulList
		logMsg("procBagMsg-------"..table.getn(body.szFightSoulList));
		--[[for i = 1, table.getn(body.szFightSoulList) do
			local tmpItem = body.szFightSoulList[i];
			g_fightSoulBg[strAccount][tmpItem.iPos] = tmpItem
		end--]]
	end
end

local function procHeroMsg(strAccount)
	return function(msg)
		local msgID = msg.iCmd;
		local tmpTb = {}
		table.insert(tmpTb, msg)
		showMsg(msgID, tmpTb)
		
		local body = GameServer_pb.Cmd_Sc_HeroList();
		body:ParseFromString(msg.strMsgBody)
		g_heroTb[strAccount] = body.szHeroList
	end
end

local function procCommMsg(strAccount)
	return function(msg)
		local msgID = msg.iCmd;
		local tmpTb = {}
		table.insert(tmpTb, msg)
		showMsg(msgID, tmpTb)
	end
end

local function createFightSoulTest(clientPlayer)
	return function()
		print("-----------fightsoul");
		addMsgProc(clientPlayer, GameServer_pb.CMD_FIGHTSOUL_BAG, procBagMsg(clientPlayer:getAccount() ) );
		addMsgProc(clientPlayer, GameServer_pb.CMD_HERO_LIST, procHeroMsg(clientPlayer:getAccount() ) );
		addMsgProc(clientPlayer, GameServer_pb.CMD_CHG_FSCONTAINER, procCommMsg(clientPlayer:getAccount() ) );
		addMsgProc(clientPlayer, GameServer_pb.CMD_ATT_CHG, procCommMsg(clientPlayer:getAccount() ) );
		WaitMultMessage(clientPlayer, 10, GameServer_pb.CMD_FIGHTSOUL_BAG, GameServer_pb.CMD_HERO_LIST);
		WaitMultMessage(clientPlayer, 10, GameServer_pb.CMD_FIGHTSOUL_BAG, GameServer_pb.CMD_HERO_LIST);
		
		-- 穿装备
		local strAccount = clientPlayer:getAccount()
		local fightSoulBg = g_fightSoulBg[strAccount]
		
		logMsg("ready move------------------"..table.getn(fightSoulBg) )
		for i = 1, table.getn(fightSoulBg) do
			logMsg("check pos|"..i)
			if fightSoulBg[i] ~= nil then
				local mvBody = GameServer_pb.Cmd_Cs_MoveFightSoul()
				mvBody.iSrcConType = GameServer_pb.en_FSConType_Bag
				mvBody.iDstConType = GameServer_pb.en_FSConType_Wear
				mvBody.iSrcPos = i;
				mvBody.iDstPos = 0;
				mvBody.dwHeroObjectID = g_heroTb[strAccount][1].dwObjectID;
				local csMsg = GameServer_pb.CSMessage();
				csMsg.iCmd = GameServer_pb.CMD_MOVE_FIGHTSOUL;
				csMsg.strMsgBody = mvBody:SerializeToString()
				local data = csMsg:SerializeToString();
				clientPlayer:sendBuff(data, string.len(data) )
				break;
			end
		end
		
		--[[WaitTime(1);
		for i = 1, 5 do
			clientPlayer:sendGmMsg("practice");
		end--]]
		
		
		
		WaitTime(1000)
		
		--[[
		clientPlayer:sendGmMsg("setlifeatt#"..GameServer_pb.en_LifeAtt_Silver.."#10000");
		
		-- 先合并一次		
		clientPlayer:sendGmMsg("autocombine");
		waitAndShowFightSoulMsg(clientPlayer, 10, GameServer_pb.CMD_CHG_FSCONTAINER, GameServer_pb.CMD_FIGHTSOUL_PRACTICE);
		
		for i = 1, 5 do
			clientPlayer:sendGmMsg("practice");
			waitAndShowFightSoulMsg(clientPlayer, 10, GameServer_pb.CMD_CHG_FSCONTAINER, GameServer_pb.CMD_FIGHTSOUL_PRACTICE);
			waitAndShowFightSoulMsg(clientPlayer, 10, GameServer_pb.CMD_CHG_FSCONTAINER, GameServer_pb.CMD_FIGHTSOUL_PRACTICE);
		end
		logMsg("enable advance mode----------------");
		clientPlayer:sendGmMsg("enableadvance");
		-- 开启高级模式
		for i = 1, 2 do
			clientPlayer:sendGmMsg("practice");
			waitAndShowFightSoulMsg(clientPlayer, 10, GameServer_pb.CMD_CHG_FSCONTAINER, GameServer_pb.CMD_FIGHTSOUL_PRACTICE);
			waitAndShowFightSoulMsg(clientPlayer, 10, GameServer_pb.CMD_CHG_FSCONTAINER, GameServer_pb.CMD_FIGHTSOUL_PRACTICE);
		end
		-- 再合并
		clientPlayer:sendGmMsg("autocombine");
		waitAndShowFightSoulMsg(clientPlayer, 10, GameServer_pb.CMD_CHG_FSCONTAINER, GameServer_pb.CMD_FIGHTSOUL_PRACTICE);
		waitAndShowFightSoulMsg(clientPlayer, 10, GameServer_pb.CMD_CHG_FSCONTAINER, GameServer_pb.CMD_FIGHTSOUL_PRACTICE);
		
		-- 兑换
		clientPlayer:sendGmMsg("setlifeatt#"..GameServer_pb.en_LifeAtt_FSChipCount.."#100");
		waitAndShowFightSoulMsg(clientPlayer, 10, GameServer_pb.CMD_CHG_FSCONTAINER, GameServer_pb.CMD_ATT_CHG);
		clientPlayer:sendGmMsg("fightsoulexchange#1");
		waitAndShowFightSoulMsg(clientPlayer, 10, GameServer_pb.CMD_CHG_FSCONTAINER, GameServer_pb.CMD_ATT_CHG);
		waitAndShowFightSoulMsg(clientPlayer, 10, GameServer_pb.CMD_CHG_FSCONTAINER, GameServer_pb.CMD_ATT_CHG);
		--]]
	end
end


function FightSoulTest(clientPlayer, args)
	local objID = clientPlayer:getAccount();
	if g_croutineTb[objID] == nil then
		g_croutineTb[objID] = coroutine.create(createFightSoulTest(clientPlayer));
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