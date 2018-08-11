

-- 功能: 解析超链接信息
function parseCallInfo(callString)
	local tmpCallInfo = {
		funname = "",
		paramTb = {}
	}
	function saveFunName(n, body)
		if string.len(tmpCallInfo.funname) == 0 then
			print("fun:"..n)
			tmpCallInfo.funname = n
		end
		
		return body
	end
	
	function saveParam(n)
		print("param:"..n)
		table.insert(tmpCallInfo.paramTb, n);
	end
	
	print("callString:"..callString)
	callString = string.gsub(callString, "(%d+)#?(.+)", saveFunName)
	print("leftCallString:"..callString)
	string.gsub(callString, "(.-)[#|;]", saveParam)
	return tmpCallInfo;
end

-- 通用call,仅作默认函数
function commonCall(callInfo)
		print("functionName:"..callInfo.funname)
		for i=1, table.getn(callInfo.paramTb) do
			print("Param"..i..":"..tostring(callInfo.paramTb[i]))
		end
end


----超链接调用表
local g_callTable = {}
function InitHyperLinkHandler()

	g_callTable[1] = testHyperLink
	g_callTable[2] = confirmCombineFightSoul
	g_callTable[3] = FSEnableAdvance
	g_callTable[4] = FSAutoCombine
	g_callTable[5] = DomainAdvanceLove
	--g_callTable[6] = DomainPlayMaid
	g_callTable[7] = addPlayerTowerResetCount
	g_callTable[8] = luaBuyArenaCount
	g_callTable[9] = confirmBuyPhyStrength
	g_callTable[10] = confirmGoldAddSilverResSpeed
	g_callTable[11] = confirmGoldAddHeroExpResSpeed 
	g_callTable[12] = confirmGoldAddWuHunDianLevelUpSpeed
	g_callTable[13] = confirmGoldAddTieJiangPuLevelUpSpeed
	g_callTable[14] = confirmGoldRefreshHonorConvet
	g_callTable[15] = confirmGoldResetCD
	g_callTable[16] = confirmManorResHarvest
	g_callTable[17] = confirmManorNoCanLootActorReturn
	g_callTable[18] = confirmContributeShopRefresh
	g_callTable[19] = confirmLeaveLegion
	g_callTable[20] = confirmNormalShopRefresh
	--[[g_callTable[1]  = GetEquipDescByContainerPos;
	g_callTable[2]  = HyperLinkWarp2Dungeon;--仅占用命令字
	g_callTable[3]  = HyperLinkWarp2NPC;--仅占用命令字
	g_callTable[4]  = GetDetailsByObjectID;
	g_callTable[5]  = HyperLinkWarp2Arena;--仅占用命令字
	g_callTable[6]  = HyperLinkGetRoleInfo;--通过世界ID,Account,rolePos查看其他角色详情
	g_callTable[7]  = HyperLinkWarp2Refine;--仅占用命令字
	g_callTable[8]  = HyperLinkWarp2Forge;--仅占用命令字
	g_callTable[9]  = HyperLinkWarp2YanWu;--仅占用命令字
	g_callTable[10] = HyperLinkGetRoleSimpleInfo;--通过世界ID,Account,rolePos查看其他角色简单信息
	g_callTable[11] = HyperLinkGetOnlineRoleInfoByEntityID;--通过在线的角色名查看其他角色详情
	g_callTable[12] = HyperLinkGetOnlineRoleSimInfoByEntityID;--通过在线的角色名查看其他角色简单信息
	g_callTable[13] = HyperLinkGetPetInfo;--通过世界ID,Account,rolePos,宠物baseid查看其他角色详情
	g_callTable[14] = HyperLinkGetOnlinePetInfoByEntityID;--通过在线的角色名查看其他角色的宠物详情
	g_callTable[15] = HyperLinkWarp2WorldChat;--仅占用命令字
	g_callTable[16] = HyperLinkWarp2NearbyFrinedRecommond;--仅占用命令字
	g_callTable[17] = HyperLinkWarp2MagicGhostOpenFunc;--仅占用命令字
	g_callTable[18] = HyperLinkWarp2ItemTraceDungeon;--仅占用命令字材料追踪到关卡选择界面	
	g_callTable[19] = HyperLinkWarp2Bag;--仅占用命令字
	g_callTable[20] = HyperLinkWarp2ShouHun;--仅占用命令字
	
	g_callTable[21] = HyperLinkGetByNameRoleSimpleInfo;--通过角色名获取其他角色简单数据	
	g_callTable[22] = HyperLinkGetByNameRoleInfo;--通过角色名获取其他角色详细数据
	g_callTable[23] = HyperLinkGetByNamePetInfo;--通过角色名及宠物位置获取其他角色的一只宠物的详情
	
	g_callTable[24] = HyperLinkDrillingGoBack; -- 仅占位通知客户端跳转回到当前殿并与之战斗
	g_callTable[25] = HyperLinkDrillingRefresh;-- 仅占位，客户端自行实现点击刷新按钮
	g_callTable[26] = HyperLinkChanllengMonster; -- 挑战演武堂怪物
	g_callTable[27] = HyperLinkHoldEmpty; -- 占领空关卡
	g_callTable[28] = HyperLinkChallengeHold; -- PVP占领挑战--]]
	
	
	--这里的命令字大小必须<100,>=100为客户端自行使用
end

function doHyperLinkCall(callInfo)	
	local iFunName = tonumber(callInfo.funname)
	local fun = g_callTable[iFunName];
	if nil == fun then
			LogMsg("no function "..tostring(callInfo.funname) )
			print("no function "..tostring(callInfo.funname) )
			fun = commonCall
	end
	
	fun(callInfo);
end


function HyperLinkError(msg)
	LogMsg("LuaScriptError", msg);
end

function HyperLinkTrace(msg)
	LogMsg("LuaScriptError", msg);
end

function OnHyperLinkMessage(player, args)
	local command = args.context:getString("msg", "");
	command=command..";"
	local resultCallInfo = parseCallInfo(command)
	if resultCallInfo == nil then
		ErrorMsg("Parse Command:["..command.."]Fail")
		return
	end
		
	table.insert(resultCallInfo.paramTb, 1, player)
	doHyperLinkCall(resultCallInfo)
end
