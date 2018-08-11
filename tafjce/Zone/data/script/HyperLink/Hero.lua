

function testHyperLink(callInfo)
	local player = callInfo.paramTb[1];
	local Account = player:getProperty(PROP_ACTOR_ACCOUNT, "");
	local handle = player:getHandle();
	
	print("-----"..Account.."|"..handle.."|"..callInfo.paramTb[2])
end

function confirmCombineFightSoul(callInfo)
	local player = callInfo.paramTb[1];
	local handle = player:getHandle();
	
	local srcConType = tonumber(callInfo.paramTb[2]);
	local srcPos = tonumber(callInfo.paramTb[3]);
	local dstConType = tonumber(callInfo.paramTb[4]);
	local dstPos = tonumber(callInfo.paramTb[5]);
	local heroObjectID = tonumber(callInfo.paramTb[6]);
	doFightSoulCombine(handle, srcConType, srcPos, dstConType, dstPos, heroObjectID)
end

function makeColorString(strMsg, color)
	if nil == color then
		color = "255 255 255"
	end
	return "<lb cl=\""..color.."\" c=\""..strMsg.."\"/>"
end

function sendFightSoulCmbConfirm(args)
	local entityID = args.context:getInt("entity");
	local srcConType = args.context:getInt("srcConType");
	local srcPos = args.context:getInt("srcPos");
	local dstConType = args.context:getInt("dstConType");
	local dstPos = args.context:getInt("dstPos");
	local heroID = args.context:getInt("heroObjectID");
	
	local srcName = args.context:getString("scrName", "");
	local dstName = args.context:getString("dstName", "");
	local getExp = args.context:getInt("exp");
	local curLevel = args.context:getInt("level");
	local newLevel = args.context:getInt("newlevel");
	
	local expStr = "获得"..getExp.."经验";
	if curLevel ~= newLevel then
		expStr = expStr.."提升至"..newLevel.."级"
	end
	
	local msg = ""..dstName..makeColorString("将吞噬")..srcName..makeColorString(expStr)
	
	local leftName = "取消"
	local righrName = "确认"
	
	local leftCmd = ""
	local righrCmd = "2#"..srcConType.."#"..srcPos.."#"..dstConType.."#"..dstPos.."#"..heroID;
	
	sendCommDlg(entityID, msg, righrName, righrCmd, leftName, leftCmd)
end

function FSEnableAdvance(callInfo)
	local player = callInfo.paramTb[1];
	local handle = player:getHandle();
	enableFSAdvance(handle)
end


function FSAutoCombine(callInfo)
	local player = callInfo.paramTb[1];
	local handle = player:getHandle();
	autoCombineFightSoul(handle)
end

function NotifyFSEnableAdvance(args)
	local entityID = args.context:getInt("entity");
	local costGold = args.context:getInt("costGold");
	local leftCount = args.context:getInt("leftCount");
	
	local msg = makeColorString("花费"..costGold.."元宝召唤得心应手状态?\\n")
	msg = msg..makeColorString("更易获得高品质武魂，额外获得武魂碎片\\n", "0, 255, 0")
	msg = msg..makeColorString("今日剩余召唤次数:"..leftCount)
	
	local leftName = "取消"
	local leftCmd = ""
	local rightName = "确认"
	local rightCmd = "3"
	sendCommDlg(entityID, msg, rightName, rightCmd, leftName, leftCmd)
end

function NotifyAutoCombine(args)
	local entityID = args.context:getInt("entity");
	local hostName = args.context:getString("hostName", "");
	local getExp = args.context:getInt("exp");
	
	local curLevel = args.context:getInt("level");
	local newLevel = args.context:getInt("newlevel");
	
	local descStr = "将吞噬背包中所有武魂获得"..getExp.."经验"
	if curLevel ~= newLevel then
		descStr = descStr.."提升至"..newLevel.."级"
	end
	
	local msg = hostName..makeColorString(descStr.."，是否进行合成？")
	local leftName = "取消"
	local leftCmd = ""
	local rightName = "确认"
	local rightCmd = "4"
	
	sendCommDlg(entityID, msg, rightName, rightCmd, leftName, leftCmd)
end

function DomainAdvanceLove(callInfo)
	local player = callInfo.paramTb[1];
	local handle = player:getHandle();
	
	loveMaid(handle, true)
end

--function ConfirmAdvanceLove(args)
	--local entity = args.context:getInt("entity");
	--local neeedGold = args.context:getInt("needGold");
	
	--local msg = "是否消耗"..neeedGold.."元宝高级宠幸？"
	--local leftName = "取消"
	--local leftCmd = ""
	--local rightName = "确认"
	--local rightCmd = "5"
	
	--sendCommDlg(entity, msg, rightName, rightCmd, leftName, leftCmd)
--end


--function DomainPlayMaid(callInfo)
	--local player = callInfo.paramTb[1];
	--local handle = player:getHandle();
	--local strAccount = callInfo.paramTb[2];
	--local worldID = tonumber(callInfo.paramTb[3])
	--playMaid(handle, true, strAccount, worldID)
--end

--function ConfirmAdvancePlay(args)
--	local entity = args.context:getInt("entity");
--	local targetAccount = args.context:getString("targetAccount", "");
--	local worldID = args.context:getInt("worldid");
--	local costGold = args.context:getInt("costGold");
	
--	local msg = "确认消耗"..costGold.."元宝高级调戏？"
--	local leftName = "取消"
	--local leftCmd = ""
	--local rightName = "确认"
	--local rightCmd = "6#"..targetAccount.."#"..worldID
	
--	sendCommDlg(entity, msg, rightName, rightCmd, leftName, leftCmd)
--end
function AddTowerRestCount(args)
	local entity = args.context:getInt("entity")
	local costGold = args.context:getInt("costGold")
	
	local msg = "花费"..costGold .."元宝增加一次重置次数？"
	local leftName = "取消"
	local leftCmd = "";
	local rightName = "确认"
	local rightCmd = "7"
	sendCommDlg(entity, msg, rightName, rightCmd, leftName, leftCmd)
end
function addPlayerTowerResetCount(callInfo)
	local player = callInfo.paramTb[1]
	local handle = player:getHandle()
	addTowerResetCount(handle,true)
end


function sendBuyChallengeConfirm(args)
	local entity = args.context:getInt("entity")
	local costGold = args.context:getInt("needgold")
	
	local msg = "花费"..costGold.."元宝购买5次挑战次数？"
	local leftName = "确定"
	local leftCmd = "8"
	local rightName = "取消"
	local rightCmd = ""
	
	sendCommDlg(entity, msg, leftName, leftCmd, rightName, rightCmd)
end

function luaBuyArenaCount(callInfo)
	local player = callInfo.paramTb[1]
	local handle = player:getHandle()
	
	buyArenaCount(handle);
end


function MakeSysInfo(args)
	local userSize = args.context:getInt("accountsize")
	local actorSize = args.context:getInt("playersize")
	local startTime = args.context:getInt("starttime")
	
	local msg = makeColorString("帐号数目:"..userSize.."\\n")
	msg = msg..makeColorString("玩家数目:"..actorSize.."\\n")
	msg = msg..makeColorString("服务器启动时间:"..os.date("%Y-%m-%d %H-%M-%S", startTime) )
	
	return msg
end


function NotiyBuyPhyStrength(args)
	local entity = args.context:getInt("entity")
	local costGold = args.context:getInt("cost")
	local getPhy = args.context:getInt("getphy");
	local haveByTimes = args.context:getInt("haveByTimes");
	local msg = "花费"..costGold.."元宝购买"..getPhy.."点体力是否继续？".."(今日已购买"..haveByTimes.."次)"
	
	local leftName = "确定"
	local leftCmd = "9"
	local rightName = "取消"
	local rightCmd = ""
	sendCommDlg(entity, msg, leftName, leftCmd, rightName, rightCmd)
end

function confirmBuyPhyStrength(callInfo)
	local player = callInfo.paramTb[1]
	local handle = player:getHandle()
	
	buyPhystrength(handle);
end

function ManorSilverGoldAddSpeed(args)
	local entity = args.context:getInt("entity")
	local costGold = args.context:getInt("cost")
	local name = args.context:getString("name","")
	local resId = args.context:getInt("resId")
	
	local msg = "花费"..costGold.."元宝立即完成"..name.."升级"
	
	local leftName = "确定"
	local leftCmd = "10#"..costGold.."#"..resId
	local rightName = "取消"
	local rightCmd = ""
	sendCommDlg(entity, msg, leftName, leftCmd, rightName, rightCmd)
end

function confirmGoldAddSilverResSpeed(callInfo)
	local player = callInfo.paramTb[1]
	local handle = player:getHandle()
	local cost = callInfo.paramTb[2]
	local resId = callInfo.paramTb[3]
	manorSilverResGoldAddSpeed(handle,cost,resId)
end

function manorHeroExpGoldAddSpeed(args)
	
	local entity = args.context:getInt("entity")
	local costGold = args.context:getInt("cost")
	local name = args.context:getString("name","");
	local resId = args.context:getInt("resId")
	
	local msg = "花费"..costGold.."元宝立即完成"..name.."升级"
	
	local  leftName = "确定"
	local leftCmd = "11#"..costGold.."#"..resId
	local rightName = "取消"
	local rightCmd = ""
	sendCommDlg(entity, msg, leftName, leftCmd, rightName, rightCmd)
end

function confirmGoldAddHeroExpResSpeed(callInfo)
	local player = callInfo.paramTb[1]
	local handle = player:getHandle()
	local cost = callInfo.paramTb[2]
	local resId = callInfo.paramTb[3]
	manorHeroExpResGoldAddSpeed(handle,cost,resId)
end

function manorWuHunDianGoldAddLevelUpSpeed(args)
	local entity = args.context:getInt("entity")
	local costGold = args.context:getInt("cost")
	
	local msg = "花费"..costGold.."元宝立即完成武魂殿的升级"
	
	local leftName = "确定"
	local leftCmd = "12#"..costGold
	local rightName = "取消"
	local rightCmd = ""
	sendCommDlg(entity, msg, leftName, leftCmd, rightName, rightCmd)
end

function confirmGoldAddWuHunDianLevelUpSpeed(callInfo)
	local player = callInfo.paramTb[1]
	local handle = player:getHandle()
	local costGold = callInfo.paramTb[2]
	manorGoldAddWuHunDianLevelUpSpeed(handle , costGold)
end

function manorTieJiangPuGoldAddLevelUpSpeed(args)
	local entity = args.context:getInt("entity")
	local costGold = args.context:getInt("cost")
	
	local msg = "花费"..costGold.."元宝立即完成铁匠铺的升级"
	
	local leftName = "确定"
	local leftCmd = "13#"..costGold
	local rightName ="取消" 
	local rightCmd = ""
	sendCommDlg(entity, msg, leftName, leftCmd, rightName, rightCmd)
end

function confirmGoldAddTieJiangPuLevelUpSpeed(callInfo)
	local player = callInfo.paramTb[1]
	local handle = player:getHandle()
	local costGold = callInfo.paramTb[2]
	manorGoldAddTieJiangPuLevelUpSpeed(handle, costGold)
end

function shopHonerConvertRefreshCost(args)
	print("--- in Hero.lua shopHonerConvertRefreshCost");
	local entity = args.context:getInt("entity")
	local costGold = args.context:getInt("cost")
	
	local msg = "是否花费"..costGold.."点荣誉刷新"
	
	local leftName = "确定"
	local leftCmd = "14"
	local rightName = "取消"
	local rightCmd = ""
	sendCommDlg(entity, msg, leftName, leftCmd, rightName, rightCmd)
end



function confirmGoldRefreshHonorConvet(callInfo)
	local player = callInfo.paramTb[1]
	local handle = player:getHandle()
	ShopsytemGoldRefreshHonorConvet(handle)
end

-- 普通商花费确认
function normalShopRefreshCost(args)
	print("--- in Hero.lua, normalShopRefreshCost");
	local entity = args.context:getInt("entity");
	local cost = args.context:getInt("cost");
	
	local msg = "是否花费"..cost.."点元宝刷新";
	local leftName = "确定";
	local leftCmd = "20";
	local rightName ="取消";
	local rightCmd = "";
	sendCommDlg(entity, msg, leftName, leftCmd, rightName, rightCmd);
end

-- 确认回调
function confirmNormalShopRefresh(callInfo)
	local player = callInfo.paramTb[1];
	local handle = player:getHandle();
	ShopsystemYuanbaoRefreshNormalShop(handle);
end

function arenaGlodResetCD(args)
	local entity = args.context:getInt("entity")
	local costGold = args.context:getInt("cost")
	
	local msg = "是否花费"..costGold.."元宝重置挑战CD"
	
	local leftName = "确定"
	local leftCmd = "15"
	local rightName = "取消"
	local rightCmd = ""
	sendCommDlg(entity, msg, leftName, leftCmd, rightName, rightCmd)
end

function confirmGoldResetCD(callInfo)
	local player = callInfo.paramTb[1]
	local handle = player:getHandle()
	confirmArenaGlodResetCD(handle)
end

function manorResHarvest(args)
	local entity = args.context:getInt("entity")
	local remaind = args.context:getInt("remaind")
	
	local msg = makeColorString("消耗 1 次收取次数收取当前产出的铜币与修为？\\n")
	msg = msg..makeColorString("当前剩余收取次数:"..remaind, "0 255 0")
	
	local leftName = "确定"
	local leftCmd = "16"
	local rightName = "取消"
	local rightCmd = ""
	sendCommDlg(entity, msg, leftName, leftCmd, rightName, rightCmd)
end

function confirmManorResHarvest(callInfo)
	local player = callInfo.paramTb[1]
	local handle = player:getHandle()
	manorResHarvestReturn(handle)
end

function manorNoCanLootActor(args)
	local entity = args.context:getInt("entity")
	
	local msg = "暂时没有找到可以掠夺的领地,重新搜索？"
	
	local leftName = "重搜"
	local leftCmd = "17"
	local rightName = "取消"
	local rightCmd = ""
	sendCommDlg(entity, msg, leftName, leftCmd, rightName, rightCmd)
end

function confirmManorNoCanLootActorReturn(callInfo)
	local player = callInfo.paramTb[1]
	local handle = player:getHandle()
	manorNoCanLootActorReturn(handle)
end

function contributeShopRefreshCost(args)
	local entity = args.context:getInt("entity")
	local costGold = args.context:getInt("cost")
	
	local msg = "是否花费"..costGold.."点贡献刷新"
	
	local leftName = "确定"
	local leftCmd = "18"
	local rightName = "取消"
	local rightCmd = ""
	sendCommDlg(entity, msg, leftName, leftCmd, rightName, rightCmd)
end

function confirmContributeShopRefresh(callInfo)
	local player = callInfo.paramTb[1]
	local handle = player:getHandle()
	contributeShopRefreshConfirm(handle)
end

function leaveLegionNotice(args)
	local entity = args.context:getInt("entity")
	local iRemaindTimes = args.context:getInt("remaindTimes")
	
	local msg = makeColorString("退出后48小时无法回到原军团\\n")
	msg = msg..makeColorString("本周剩余可加入军团次数："..iRemaindTimes, "255 0 0")
	
	local leftName = "确定"
	local leftCmd = "19"
	local rightName = "取消"
	local rightCmd = ""
	sendCommDlg(entity, msg, leftName, leftCmd, rightName, rightCmd)
end

function confirmLeaveLegion(callInfo)
	local player = callInfo.paramTb[1]
	local handle = player:getHandle()
	leaveLegionCallBack(handle)
end


