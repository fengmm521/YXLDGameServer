--获取通用物品品质对应的颜色
function UI_GetColor(pingjie)
	local data = GetGameData(DataFileQualityProp, pingjie, "stQualityPropData")
	return Util_GetColor(data.m_color)
end

function Util_GetColor(quality)
	local colortable = {
	[1] = { color = ccc3(255,255,255)},
	[2] = { color = ccc3(16,255,0)} ,
	[3] = { color = ccc3(0,186,255)},
	[4] = { color = ccc3(234,128,252)},
	[5] = { color = ccc3(255,152,0)},
	[6] = { color = ccc3(255,234,0)},
	[7] = { color = ccc3(229,28,35)},
	}
	if (quality > 0 and quality <= 7) then
		return colortable[quality].color
	end
	return colortable[1].color
end

function UI_GetStringColor(pingjie)
    Log("pingjie==="..pingjie)
	local data = GetGameData(DataFileQualityProp, pingjie, "stQualityPropData")
	local color = Util_GetColor(data.m_color)
	return color.r.." "..color.g.." "..color.b
end

function UI_GetStringColorNew(pingjie)
   local color = Util_GetColor(pingjie)
   return color
end 
function GetAttShowValue(attId,value)
    if (attId == GameServer_pb.en_LifeAtt_Doge) or
        (attId == GameServer_pb.en_LifeAtt_Hit) or
        (attId == GameServer_pb.en_LifeAtt_AntiKnock) or
        (attId == GameServer_pb.en_LifeAtt_Knock) or
        (attId == GameServer_pb.en_LifeAtt_Block) or
        (attId == GameServer_pb.en_LifeAtt_Wreck) or
        (attId == GameServer_pb.en_LifeAtt_Armor) or
        (attId == GameServer_pb.en_LifeAtt_Sunder) then--千分比
		return ""..(value/10).."%";
    elseif(attId == GameServer_pb.en_LifeAtt_MaxHP) or
            (attId == GameServer_pb.en_LifeAtt_Att) or
            (attId == GameServer_pb.en_LifeAtt_KnockDamage) or
            (attId == GameServer_pb.en_LifeAtt_KnockXiXue) or
            (attId == GameServer_pb.en_LifeAtt_ConAttackRate) then --万分比
            if value%10>5 then
                value = value+10-value%10
            else
                value = value - value%10
            end
            return ""..(value/100).."%"
    end
    return ""..value
end
function GetAttNameAndValue(attId,value,Str)
    if Str == nil then
        Str="+"
    end
    local str = "";
	if (attId == GameServer_pb.en_LifeAtt_Silver) then
		str = FormatString("en_LifeAtt_Silver");
	elseif (attId == GameServer_pb.en_LifeAtt_Gold) then
		str = FormatString("en_LifeAtt_Gold");
	elseif (attId == GameServer_pb.en_LifeAtt_Exp) then
		str = FormatString("en_LifeAtt_Exp");
	elseif (attId == GameServer_pb.en_LifeAtt_HeroExp) then
		str = FormatString("en_LifeAtt_HeroExp");
	elseif (attId == GameServer_pb.en_LifeAtt_MaxHP) then
		str = FormatString("en_LifeAtt_MaxHP");
	elseif (attId == GameServer_pb.en_LifeAtt_Att) then
		str = FormatString("en_LifeAtt_Att");
	elseif (attId == GameServer_pb.en_LifeAtt_Doge) then--
		str = FormatString("en_LifeAtt_Doge");
	elseif (attId == GameServer_pb.en_LifeAtt_Hit) then
		str = FormatString("en_LifeAtt_Hit");
	elseif (attId == GameServer_pb.en_LifeAtt_AntiKnock) then
		str = FormatString("en_LifeAtt_AntiKnock");
	elseif (attId == GameServer_pb.en_LifeAtt_Knock) then
		str = FormatString("en_LifeAtt_Knock");
	elseif (attId == GameServer_pb.en_LifeAtt_Block) then
		str = FormatString("en_LifeAtt_Block");
	elseif (attId == GameServer_pb.en_LifeAtt_Wreck) then
		str = FormatString("en_LifeAtt_Wreck");
	elseif (attId == GameServer_pb.en_LifeAtt_Armor) then
		str = FormatString("en_LifeAtt_Armor");
	elseif (attId == GameServer_pb.en_LifeAtt_Sunder) then
		str = FormatString("en_LifeAtt_Sunder");
	elseif (attId == GameServer_pb.en_LifeAtt_InitAnger) then
		str = FormatString("en_LifeAtt_InitAnger");
	elseif (attId == GameServer_pb.en_LifeAtt_Honor) then
		str = FormatString("en_LifeAtt_Honor");
	elseif (attId == GameServer_pb.en_LifeAtt_SkillDamage) then
		str = FormatString("en_LifeAtt_SkillDamage");
	elseif (attId == GameServer_pb.en_LifeAtt_SkillDef) then
		str = FormatString("en_LifeAtt_SkillDef");
    elseif (attId == GameServer_pb.en_LifeAtt_Def) then 
        str = FormatString("en_LifeAtt_Def")
	elseif (attId == GameServer_pb.en_LifeAtt_KnockDamage) then 
        str = FormatString("en_LifeAtt_KnockDamage")
    elseif (attId == GameServer_pb.en_LifeAtt_KnockXiXue) then 
        str = FormatString("en_LifeAtt_KnockXiXue")
    elseif (attId == GameServer_pb.en_LifeAtt_ConAttackRate) then 
        str = FormatString("en_LifeAtt_ConAttackRate")
	end
    print("attId==============="..attId)
	if (attId == GameServer_pb.en_LifeAtt_Doge) or
        (attId == GameServer_pb.en_LifeAtt_Hit) or
        (attId == GameServer_pb.en_LifeAtt_AntiKnock) or
        (attId == GameServer_pb.en_LifeAtt_Knock) or
        (attId == GameServer_pb.en_LifeAtt_Block) or
        (attId == GameServer_pb.en_LifeAtt_Wreck) or
        (attId == GameServer_pb.en_LifeAtt_Armor) or
        (attId == GameServer_pb.en_LifeAtt_Sunder) then--千分比
		return str..Str..(value/10).."%";
    elseif (attId == GameServer_pb.en_LifeAtt_KnockDamage) or
            --(attId == GameServer_pb.en_LifeAtt_MaxHP) or
            --(attId == GameServer_pb.en_LifeAtt_Att) or
            (attId == GameServer_pb.en_LifeAtt_KnockDamage) or
            (attId == GameServer_pb.en_LifeAtt_KnockXiXue) or
            (attId == GameServer_pb.en_LifeAtt_ConAttackRate) then --万分比
            if value%10>5 then
                value = value+10-value%10
            else
                value = value - value%10
            end
            return str..Str..(value/100).."%"
    end
    return str..Str..value
end
function GetAttName(attId)
    Log("attId====="..attId)
	if (attId == GameServer_pb.en_LifeAtt_Silver) then
		return FormatString("en_LifeAtt_Silver");
	elseif (attId == GameServer_pb.en_LifeAtt_Gold) then
		return FormatString("en_LifeAtt_Gold");
	elseif (attId == GameServer_pb.en_LifeAtt_Exp) then
		return FormatString("en_LifeAtt_Exp");
	elseif (attId == GameServer_pb.en_LifeAtt_HeroExp) then
		return FormatString("en_LifeAtt_HeroExp");
	elseif (attId == GameServer_pb.en_LifeAtt_MaxHP) then
		return FormatString("en_LifeAtt_MaxHP");
	elseif (attId == GameServer_pb.en_LifeAtt_Att) then
		return FormatString("en_LifeAtt_Att");
	elseif (attId == GameServer_pb.en_LifeAtt_Doge) then--
		return FormatString("en_LifeAtt_Doge");
	elseif (attId == GameServer_pb.en_LifeAtt_Hit) then
		return FormatString("en_LifeAtt_Hit");
	elseif (attId == GameServer_pb.en_LifeAtt_AntiKnock) then
		return FormatString("en_LifeAtt_AntiKnock");
	elseif (attId == GameServer_pb.en_LifeAtt_Knock) then
		return FormatString("en_LifeAtt_Knock");
	elseif (attId == GameServer_pb.en_LifeAtt_Block) then
		return FormatString("en_LifeAtt_Block");
	elseif (attId == GameServer_pb.en_LifeAtt_Wreck) then
		return FormatString("en_LifeAtt_Wreck");
	elseif (attId == GameServer_pb.en_LifeAtt_Armor) then
		return FormatString("en_LifeAtt_Armor");
	elseif (attId == GameServer_pb.en_LifeAtt_Sunder) then
		return FormatString("en_LifeAtt_Sunder");
	elseif (attId == GameServer_pb.en_LifeAtt_InitAnger) then
		return FormatString("en_LifeAtt_InitAnger");
	elseif (attId == GameServer_pb.en_LifeAtt_Honor) then
		return FormatString("en_LifeAtt_Honor");
	elseif (attId == GameServer_pb.en_LifeAtt_SkillDamage) then
		return FormatString("en_LifeAtt_SkillDamage");
	elseif (attId == GameServer_pb.en_LifeAtt_SkillDef) then
		return FormatString("en_LifeAtt_SkillDef");
    elseif (attId == GameServer_pb.en_LifeAtt_Def) then 
        return FormatString("en_LifeAtt_Def")
	elseif (attId == GameServer_pb.en_LifeAtt_KnockDamage) then 
        return FormatString("en_LifeAtt_KnockDamage")
    elseif (attId == GameServer_pb.en_LifeAtt_KnockXiXue) then 
        return FormatString("en_LifeAtt_KnockXiXue")
    elseif (attId == GameServer_pb.en_LifeAtt_ConAttackRate) then 
        return FormatString("en_LifeAtt_ConAttackRate")
	end
	
	return "";
end

--返回增加的lifeatt数量
function GetAddLiftAttValueAndName(attId,value)
	-- body
    local str = "";
    local curValue = 0;
	if (attId == GameServer_pb.en_LifeAtt_Silver) then
		if value > GetLocalPlayer():GetInt(EPlayer_Silver) then
			curValue = value - GetLocalPlayer():GetInt(EPlayer_Silver)
		   str = FormatString("PromptBox_Sliver",curValue);
	    end
	elseif (attId == GameServer_pb.en_LifeAtt_Gold) then
		
		if value > GetLocalPlayer():GetInt(EPlayer_Gold) then
		    curValue = value - GetLocalPlayer():GetInt(EPlayer_Gold)
			str = FormatString("PromptBox_Gold",curValue);
	    end
	elseif (attId == GameServer_pb.en_LifeAtt_HeroExp) then
		
		if value > GetLocalPlayer():GetInt(EPlayer_HeroExp) then
			curValue = value - GetLocalPlayer():GetInt(EPlayer_HeroExp)
			str = FormatString("PromptBox_HeroExp",curValue);
	    end
	elseif (attId == GameServer_pb.en_LifeAtt_Honor) then
		if value > GetLocalPlayer():GetInt(EPlayer_Honor) then
			curValue = value - GetLocalPlayer():GetInt(EPlayer_Honor)
			str = FormatString("PromptBox_Honor",curValue);
	    end		
	elseif (attId == GameServer_pb.en_LifeAtt_PhyStrength)then		
		if value > GetLocalPlayer():GetInt(EPlayer_Tili) then
			curValue = value - GetLocalPlayer():GetInt(EPlayer_Tili)
		    str = FormatString("PromptBox_Tili",curValue);
	    end    		
	end
	return str;
end

function GetAttIcon(attId, small)
	--local frame = ImageView:create()
	--frame:loadTexture("LifeAttIcon/neikuang.png")
	local icon = ImageView:create()
	icon:loadTexture("LifeAttIcon/"..attId..".png")
	local waiFrame = ImageView:create()
	waiFrame:loadTexture("LifeAttIcon/waikuang.png")
	
	--frame:addChild(icon);
	icon:addChild(waiFrame);
	return icon;	
end

function GetAttIconNoFrame(attId)
	-- body
	local icon = ImageView:create()
	icon:loadTexture("LifeAttIcon/s_"..attId..".png")
	return icon
end

function GetArmature(animName)
	CCArmatureDataManager:sharedArmatureDataManager():removeArmatureFileInfo("AnimExport/"..animName.."/"..animName..".ExportJson");
	CCArmatureDataManager:sharedArmatureDataManager():addArmatureFileInfo("AnimExport/"..animName.."/"..animName..".ExportJson");
	local ret = CCArmature:create(animName);
	CCArmatureDataManager:sharedArmatureDataManager():removeArmatureFileInfo("AnimExport/"..animName.."/"..animName..".ExportJson");
	
	return ret;
end

function GetEffArmature(animName)
	CCArmatureDataManager:sharedArmatureDataManager():removeArmatureFileInfo("Effect/"..animName.."/"..animName..".ExportJson");
	CCArmatureDataManager:sharedArmatureDataManager():addArmatureFileInfo("Effect/"..animName.."/"..animName..".ExportJson");
	local ret = CCArmature:create(animName);
	CCArmatureDataManager:sharedArmatureDataManager():removeArmatureFileInfo("Effect/"..animName.."/"..animName..".ExportJson");
	
	return ret;
end

function GetUIArmature(animName)
	CCArmatureDataManager:sharedArmatureDataManager():removeArmatureFileInfo("UIEffect/"..animName.."/"..animName..".ExportJson");
	CCArmatureDataManager:sharedArmatureDataManager():addArmatureFileInfo("UIEffect/"..animName.."/"..animName..".ExportJson");
	local ret = CCArmature:create(animName);
	
	CCArmatureDataManager:sharedArmatureDataManager():removeArmatureFileInfo("UIEffect/"..animName.."/"..animName..".ExportJson");
	
	return ret;
end

function ArmatureEnd(armature, movementType, movementID)
	if (movementType == LOOP_COMPLETE)then
		armature:removeFromParentAndCleanup(true);
	end
end

function UI_Split(szFullString, szSeparator)
	local nFindStartIndex = 1
	local nSplitIndex = 1
	local nSplitArray = {}
	while true do
		local nFindLastIndex = string.find(szFullString, szSeparator, nFindStartIndex)
		if not nFindLastIndex then
			nSplitArray[nSplitIndex] = string.sub(szFullString, nFindStartIndex, string.len(szFullString))
			break
		end
		nSplitArray[nSplitIndex] = string.sub(szFullString, nFindStartIndex, nFindLastIndex - 1)

		nFindStartIndex = nFindLastIndex + string.len(szSeparator)
		nSplitIndex = nSplitIndex + 1
	end
	return nSplitArray
end

function IsItem(id)
    if id > 200000 then 
       return false
    else 
       return true;
    end

end

function IsEquip(id)
	return false;
end	