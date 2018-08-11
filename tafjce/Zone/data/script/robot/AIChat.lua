

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


function createChatTest(clientPlayer)
	return function()
		print("开始接收");
		addMsgProc(clientPlayer, GameServer_pb.CMD_CHANNEL_CHAT, mkShowMsg("CMD_CHANNEL_CHAT_SC") )
		WaitTime(10000)
	end
end


function ChatTest(clientPlayer, args)
	--local cmd = args.context:getString("cmd", "")
	local objID = clientPlayer:getAccount();
	if g_croutineTb[objID] == nil then
		g_croutineTb[objID] = coroutine.create(createChatTest(clientPlayer));
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