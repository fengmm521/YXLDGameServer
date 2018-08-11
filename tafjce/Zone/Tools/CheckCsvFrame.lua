--------------------------------------------------------
-- 功能: 数据档检测框架
-- 时间: 2012-06-07
--------------------------------------------------------

dofile("csv.lua")
dofile("xml.lua")
if table.getn(arg) ~= 1 then
	print("useage:[dataDir]")
	return;
end

-- csv数据档目录
local g_dataDir = arg[1]; 

-- 测试用例
local g_checkItem = {}

-- csv解析原始数据表,主要是用来加速各个测试用例的执行，避免重复解析
local g_csvTable = {}
local g_errlog = "checkCsv.log";
local g_errFile = io.open(g_errlog, "wb");

function getCsvTable(tableName)
	if g_csvTable[tableName] == nil then
		g_csvTable[tableName] = parseCsv(g_dataDir.."/"..tableName);
	end
	
	return g_csvTable[tableName]
end

function addCheckItem(checkClosure)
	table.insert(g_checkItem, checkClosure);
end

function logCheckMsg(data)
	g_errFile:write(data.."\n")
	g_errFile:flush();
	print(data)
end

-- 运行测试用例
function doCheck()
	for i =1, table.getn(g_checkItem) do
		local f = g_checkItem[i]
		local result, errmsg = f()
		if result < 0 then
			logCheckMsg(errmsg)
			g_errFile:flush();
			g_errFile:close();
			os.exit(-1);
		end
	end
	
	logCheckMsg("当你看到这一句，说明所有测试通过了！");
end

-- 加载测试用例配置XML
function loadDataCheckConf(fileName)
	print("-------------")
	local confTable = parseXml(fileName);
	if confTable == nil then
		logCheckMsg("未找到数据档检测配置文件:"..tostring(fileName) );
		os.exit(-1);
	end
	
	if confTable.n ~= 2 then
		logCheckMsg(fileName.."格式错误");
		os.exit(-1);
	end
	
	confTable = confTable[2]
	if confTable.label ~= "tablecheck" then
		logCheckMsg("需要配置tablecheck");
		os.exit(-1);
	end
	
	if confTable[1] == nil then
		logCheckMsg("没有找到测试功能插件");
		os.exit(-1);
	end
	
	-- 加载插件模块
	local pluginTb = confTable[1];
	for i = 1, pluginTb.n do
		local pluginName = pluginTb[i].args["name"];
		local pluginFile = "plugin/"..pluginName..".lua"
		logCheckMsg("开始加载插件:"..pluginFile)
		dofile(pluginFile)
		logCheckMsg("结束加载插件:"..pluginFile)
	end
	
	-- 加载测试表
	if confTable[2].label ~= "tables"  then
		logCheckMsg("没有配置要检测的数据表")
		return;
	end
	
	-- 加载具体测试表
	local checkTbs = confTable[2];
	print("lable:"..checkTbs.label)
	for i = 1, checkTbs.n do
		local checkItemTable = checkTbs[i];
		loadSingTableCheck(checkItemTable);
	end
end

function loadSingTableCheck(confTable)
	local tableName = confTable.args["name"];
	logCheckMsg("加载["..tableName.."]检测项，共有"..(confTable.n).."项检测")
	for i = 1, confTable.n do
		local singleCheck = confTable[i];
		local checkType = singleCheck.args["type"];
		logCheckMsg("准备添加数据表:"..tableName.."测试项["..checkType.."]");
		g_cmdArgs = singleCheck.args
		local cmdString = checkType.."_AddCheck(\""..tableName.."\",g_cmdArgs)"
		local result, errmsg = pcall(loadstring(cmdString) )
		if result ~= true then
			logCheckMsg("添加表测试失败:["..tableName.."]["..checkType.."]errmsg:"..errmsg);
		end
	end
end

-- 插件加载,默认提供类型检查
--dofile("CheckType.lua")
--dofile("CsvCheckValueLimit.lua")
--loading 策划/测试配置的测试配置
--addCheckItem(makeTypeCheck("test.csv") )

loadDataCheckConf("DataCheck.xml")
doCheck();

