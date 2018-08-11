--跑马灯功能

--缓存服务器发来的数据
local l_MarqueeInfoBySever = {}

local marqueeLab = nil 
local l_templayout = nil 

local stringtest = nil 
--跑马灯 lua 匹配模式

local function ReplaceName(src)
  -- body
  return src
end

local function changeHeroName(src)
  -- body
  local tb = {}
  local function spilit(txt)
    -- body
    table.insert(tb,txt)
  end
  string.gsub(src,"(%w+)#?",spilit)
  Log("tb[1]====="..tb[1].."tb[2]========"..tb[2])
  local herodata = GetGameData(DataFileHeroBorn, tb[1], "stHeroBornData")
  return GetHeroName(herodata.m_name, tb[2])
end

local function changeGodAnimalName(src)
  -- body
  local tb = {}
  local function spilit(txt)
    -- body
    table.insert(tb,txt)
  end
  string.gsub(src,"(%w+)#?",spilit)
  local godAnimaldata = GetGameData(DataFileGodAniaml, tb[1], "stGodAnimalData")
  return GetHeroName(godAnimaldata.m_name,tb[2])
end

local function changeFightSoulName(src)
  -- body
  local tb = {}
  local function spilit(txt)
    -- body
    table.insert(tb,txt)
  end
  string.gsub(src,"(%w+)#?",spilit)
  local fightsouldata = GetGameData(DataFileFightSoul, tb[1], "stFightSoulData")
  return FormatString("FightSoulName_3", UI_GetStringColorNew(fightsouldata.m_quality), fightsouldata.m_name)
end

local function changeNormalHero(src)
   -- body
  local tb = {}
  local function spilit(txt)
    -- body
    table.insert(tb,txt)
  end
  string.gsub(src,"(%w+)#?",spilit)
  Log("tb[1]====="..tb[1].."tb[2]========"..tb[2])
  local herodata = GetGameData(DataFileHeroBorn, tb[1], "stHeroBornData")
  return FormatString("FightSoulName_0",herodata.m_name)
end

local function changeNormalGodAnimal(src)
  -- body
  local tb = {}
  local function spilit(txt)
    -- body
    table.insert(tb,txt)
  end
  string.gsub(src,"(%w+)#?",spilit)
  local godAnimaldata = GetGameData(DataFileGodAniaml, tb[1], "stGodAnimalData")
  return FormatString("FightSoulName_0",godAnimaldata.m_name)
end

local function changeMonsterName(src)
   local tb = {}
   local function spilit(txt)
      table.insert(tb,txt)
   end 
   string.gsub(src,"(%w+)#?",spilit)
   local monsterdata = GetGameData(DataFileMonster, tb[1], "stMonsterData")
   return FormatString("FightSoulName_0",monsterdata.m_name)
end 

--函数tab
local l_luaMarqueeFunctiontab = {}
l_luaMarqueeFunctiontab["replace"] = ReplaceName
l_luaMarqueeFunctiontab["changeHero"] = changeHeroName
l_luaMarqueeFunctiontab["changeGodAnimal"] = changeGodAnimalName
l_luaMarqueeFunctiontab["changeFightSoul"] = changeFightSoulName 
l_luaMarqueeFunctiontab["changeNormalHero"] = changeNormalHero
l_luaMarqueeFunctiontab["changeNormalGodAnimal"] = changeNormalGodAnimal
l_luaMarqueeFunctiontab["changeMonsterName"] = changeMonsterName

local function MarqueeConversionProcess(testTab)
  -- body
  local i = 1
  return function(functionname)
    -- body
    Log("functionname====="..functionname.."i===="..i)
    local V = l_luaMarqueeFunctiontab[functionname](testTab[i])
    i = i + 1
    return V
  end
end
--local testTab = {"xxx","1001#20","1001#25"}
local function MarqueeInitInfo(testTab,id)
  -- body
    local tempString = FormatString("Marquee_"..id)
    stringtest = string.gsub(tempString,"{(%w+)}",MarqueeConversionProcess(testTab))
    Log("stringtest======"..tostring(stringtest))
    return stringtest
end

local l_marqueeWidget = nil 
local function InitMarqueeWidget()
  -- body
    l_marqueeWidget = GUIReader:shareReader():widgetFromJsonFile("Marquee.json")
    l_templayout = UI_GetUILayout(l_marqueeWidget,1)
    MainScene:GetInstance():AddChild(l_marqueeWidget, EMSTag_Marquee, EMSTag_Marquee,false)
end
function MarqueeFunction(Id,Info)
	-- body
  --InitMarqueeWidget()
	  --local widget = GUIReader:shareReader():widgetFromJsonFile("Marquee.json")
    --test 数据
    --l_templayout = UI_GetUILayout(widget,1)
    --l_MarqueeInfoBySever = {}
    --local testTab = {"xxx","1001#20","1001#25"}
    --table.insert(l_MarqueeInfoBySever,MarqueeInitInfo(testTab,1))
   --  local testTwoTab = {"xxxx","5001#10","5001#30"}
   -- -- table.insert(l_MarqueeInfoBySever,MarqueeInitInfo(testTwoTab,2))
   --  local testThreeTab = {"xxx","13"}
    -- table.insert(l_MarqueeInfoBySever,MarqueeInitInfo(Info,Id))

    if #l_MarqueeInfoBySever ~= 0 then
    	CreateMarquee(l_MarqueeInfoBySever[1])
    end
    -- local action = CCMoveTo:create(5,ccp(-testlab:getContentSize().width,15))
    -- testlab:runAction(action)
	 -- MainScene:GetInstance():AddChild(widget, EMSTag_Marquee, EMSTag_Marquee,false);
end

function CreateMarquee(marqueestring)
	-- body
    local layoutwidth = l_templayout:getContentSize().width 
    local layoutheight = l_templayout:getContentSize().height 
    --marqueeLab = CompLabel:GetDefaultCompLabel(marqueestring,layoutwidth)
    marqueeLab = CompLabel:GetCompLabel(marqueestring, 9999, kCCTextAlignmentLeft)
    marqueeLab:setAnchorPoint(ccp(0,0.5))
    marqueeLab:setPosition(ccp(layoutwidth,layoutheight / 2))
    l_templayout:addChild(marqueeLab)
    if #l_MarqueeInfoBySever ~= 0 then
    	table.remove(l_MarqueeInfoBySever,1)
    	GetGlobalEntity():GetScheduler():RegisterInfiniteScript( "MarqueeMove", 0.01)
    end

end
function MarqueeMove()
	-- body
	local marqueestringwidth = marqueeLab:getContentSize().width 
	local testlabPositionX = marqueeLab:getPosition()
    if testlabPositionX < -marqueestringwidth then
       GetGlobalEntity():GetScheduler():UnRegisterScript("MarqueeMove")
       l_templayout:removeAllChildren()
       if #l_MarqueeInfoBySever ~= 0 then
       	  CreateMarquee(l_MarqueeInfoBySever[1])
       else
       	  MainScene:GetInstance():removeChildByTag(EMSTag_Marquee)
          -- l_marqueeWidget = nil 
       end
    else
    	marqueeLab:setPosition(ccp(testlabPositionX - 3,15))
    end 
end

function GetMarqueeInfo(pkg)
  -- body
  local info = GameServer_pb.CMD_MARQUEE_SC()
  info:ParseFromString(pkg)
  Log("MarqueeInfo================"..tostring(info))
  table.insert(l_MarqueeInfoBySever,MarqueeInitInfo(info.szStrMessage,info.marqueeID))
  if MainScene:GetInstance():getChildByTag(EMSTag_Marquee) == nil then
     InitMarqueeWidget()
     MarqueeFunction(info.marqueeID,info.szStrMessage)
  end
end

ScriptSys:GetInstance():RegisterScriptFunc( GameServer_pb.CMD_MARQUEE, "GetMarqueeInfo" );