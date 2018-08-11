--------------------------------------------------------
-- 功能: 数值区间检查插件
-- 时间: 2012-06-07
--------------------------------------------------------
local function doValueLimitCheck(tableName, csvTable, args)
	local fieldName = args["field"];
	local minV = tonumber(args["min"]);
	local maxV = tonumber(args["max"]);
	local fieldList = csvTable[1];
	local fieldIndex = nil
	for i = 1, table.getn(fieldList) do
		if fieldList[i] == fieldName then
			fieldIndex = i
			break
		end
	end
	
	if fieldIndex == nil then
		return -1, "ValueLimit检查失败:数据档["..tableName.."]没有该列:"..fieldName
	end
	
	local fieldType = csvTable[2][fieldIndex];
	if fieldType ~= "int" and fieldType ~= "float" then
		return -1, "ValueLimit检查失败:["..tableName.."]的列["..fieldName.."]类型不是数值型，不能检测"
	end
	
	for i = 3, table.getn(csvTable) do
		local valueString = csvTable[i][fieldIndex];
		local value = 0;
		if string.len(valueString) > 0 then
			value = tonumber(valueString);
		end
		
		if value == nil or value > maxV or value < minV then
			return -1, "ValueLimit检查失败:数据档:["..tableName.."]["..fieldName.."]数值检查错误,内容:["..valueString.."]要求数值范围["..minV..", "..maxV.."]"
		end
	end
	
	return 1, "OK"
end


local function makeValueLimitCheck(tableName, args)
	return function()
		local tbName = tableName;
		local fieldName = args["field"];
		logCheckMsg("开始数值范围检查:["..tableName.."]["..fieldName.."]");
		local csvTable = getCsvTable(tableName);
		return doValueLimitCheck(tbName, csvTable, args)
	end
end

function ValueLimit_AddCheck(tableName, args)
	addCheckItem(makeValueLimitCheck(tableName, args) )
end