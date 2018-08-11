
package.path = package.path .. ';'..getDataPath()..'./script/robot/pb/?.lua'
--package.cpath = package.cpath ..';'..getDataPath()..'./script/robot/pb/?.so'..";"..getDataPath()..'./script/robot/pb/?.dll'

local WorkDir=getDataPath();
dofile(WorkDir.."/script/robot/AIFormation.lua");
dofile(WorkDir.."/script/robot/AIFightSoul.lua");
dofile(WorkDir.."/script/robot/AIHero.lua");
dofile(WorkDir.."/script/robot/AIArena.lua");
dofile(WorkDir.."/script/robot/AIWorldBoss.lua");
dofile(WorkDir.."/script/robot/AIChat.lua");
--dofile(WorkDir.."/script/robot/AICampBattle.lua");

require "GameServer_pb"

g_waitMsgTb = {}
g_croutineTb = {}
g_msgProcTb = {}

function processMessage(icmd, account, strMsgData)
	--[[print("rcv msg|"..account.." data len:"..string.len(strMsgData))
	local tmp = GameServer_pb.SCMessage();
	tmp:ParseFromString(strMsgData)
	print("cmd:"..tmp.iCmd)
	--]]
	
	if g_msgProcTb[icmd] ~= nil then
		local tmp = GameServer_pb.SCMessage();
		tmp:ParseFromString(strMsgData)
		if g_msgProcTb[icmd] ~= nil and g_msgProcTb[icmd][account] ~= nil then
			g_msgProcTb[icmd][account](tmp)
		end
	end
	
	if g_waitMsgTb[icmd] ~= nil then
		local tmp = GameServer_pb.SCMessage();
		tmp:ParseFromString(strMsgData)
		if g_waitMsgTb[tmp.iCmd][account] ~= nil then
			local tmpData = g_waitMsgTb[tmp.iCmd][account];
			tmpData.isOk = true
			table.insert(tmpData.msgTb, tmp)
			print("######################");
		end
	end
end


function addMsgProc(player, msgID, procFun)
	if g_msgProcTb[msgID] == nil then
		g_msgProcTb[msgID] = {}
	end
	
	local strAccount = player:getAccount();
	g_msgProcTb[msgID][strAccount] = procFun;
	--table.insert(g_msgProcTb[msgID], procFun);
end

function WaitTime(second)
	local beginTime = getRunSecond();
	--print("beginTime:"..beginTime)
	while true do
		local curTime = getRunSecond();
		--print("curTime:"..curTime)
		if (curTime - beginTime) < second then
			coroutine.yield()
		else
			break;
		end
	end
end



function WaitMessage(player, msgID, second)
	local strAccount = player:getAccount();
	local beginTime = getRunSecond();
	local waitDesc = {}
	waitDesc.isOk = false;
	waitDesc.msgTb = {}
	
	local targetMsgTb = g_waitMsgTb[msgID];
	if targetMsgTb == nil then
		g_waitMsgTb[msgID] = {}
		targetMsgTb = g_waitMsgTb[msgID]
	end
	
	targetMsgTb[strAccount] = waitDesc;
	while true do
		if waitDesc.isOk == true then
			targetMsgTb[strAccount] = nil
			print("find--------------------")
			return true, waitDesc.msgTb
		end
		
		if second ~= nil then
			if getRunSecond() - beginTime > second then
				targetMsgTb[strAccount] = nil
				return false;
			end
		end
		coroutine.yield();
	end
end

local function clearMsgTb(strAccount, msgTb)
	for i = 1, table.getn(msgTb) do
		local msgID = msgTb[i];
		g_waitMsgTb[msgID][strAccount] = nil
	end
end

function WaitMultMessage(player, second, ...)
	local strAccount = player:getAccount();
	local beginTime = getRunSecond();
	
	for i = 1, table.getn(arg) do
		local msgID = arg[i];
		local waitDesc = {}
		waitDesc.isOk = false;
		waitDesc.msgTb = {}
		local targetMsgTb = g_waitMsgTb[msgID];
		if targetMsgTb == nil then
			g_waitMsgTb[msgID] = {}
			targetMsgTb = g_waitMsgTb[msgID]
		end
		targetMsgTb[strAccount] = waitDesc;
	end
	
	while true do
		for i = 1, table.getn(arg) do
			local msgID = arg[i];
			if g_waitMsgTb[msgID][strAccount].isOk == true then
				print("-----------find")
				local retMsgTb = g_waitMsgTb[msgID][strAccount].msgTb
				clearMsgTb(strAccount, arg);
				return true, msgID, retMsgTb
			end
		end
				
		if getRunSecond() - beginTime > second then
			clearMsgTb(strAccount, arg);
			return false;
		end
		coroutine.yield();
	end
end


function sendMsgHelp(player, msgID, bodyInfo)
	local csMsg = GameServer_pb.CSMessage();
	csMsg.iCmd = msgID;
	
	if bodyInfo ~= nil then
		csMsg.strMsgBody = bodyInfo:SerializeToString();
	end
	
	local data = csMsg:SerializeToString();
	player:sendBuff(data, string.len(data) )
end



















