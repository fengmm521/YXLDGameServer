--------------------------------------------------------
-- 功能: ID唯一性检查插件
-- 时间: 2012-06-07
--------------------------------------------------------

local function doUniqueCheck(tableName, csvTable, args)
	local fieldName = args["field"];
	local fieldList = csvTable[1];
	local fieldIndex = nil
	for i = 1, table.getn(fieldList) do
		if fieldList[i] == fieldName then
			fieldIndex = i
			break
		end
	end
	
	if fieldIndex == nil then
		return -1, "Unique检查失败:数据档["..tableName.."]没有该列:"..fieldName
	end
	
	local tmpTb = {}
	for i = 3, table.getn(csvTable) do
		local value = csvTable[i][fieldIndex];
		if tmpTb[value]  ~= nil then
			return -1, "Unique检查失败:数据档["..tableName.."]["..fieldName.."]第"..i.."行和第"..tmpTb[value].."行数据相同:"..value
		end
		tmpTb[value] = i
	end
	
	return 1, "OK"
end


local function makeUniqueCheck(tableName, args)
	return function()
		local fieldName = args["field"];
		logCheckMsg("开始数值唯一(Unique)性检查:["..tableName.."]["..fieldName.."]");
		local csvTable = getCsvTable(tableName);
		
		return doUniqueCheck(tableName, csvTable, args)
	end
end

function Unique_AddCheck(tableName, args)
	addCheckItem(makeUniqueCheck(tableName, args) )
end