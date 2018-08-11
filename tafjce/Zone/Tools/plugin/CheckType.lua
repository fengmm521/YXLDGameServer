--------------------------------------------------------
-- 功能: 通用数据格式检查
-- 时间: 2012-06-07
--------------------------------------------------------

local g_tyErrmsgHead = "]数据类型检测失败："
local g_typeDefine = {}
g_typeDefine["int"]= 1;
g_typeDefine["string"]= 1;
g_typeDefine["float"]= 1;

-- 允许不填写，则为默认数据
local function chceckSingleType(data, tyName)
	if tyName == "int" and tonumber(data) == nil and string.len(data)~=0 then
		return false
	end
	
	if tyName == "float" and tonumber(data) == nil and string.len(data)~=0 then
		return false;
	end
	
	if data == nil then
		return false;
	end
	
	return true
end

local function csvTypeCheckImp(tbName, csvTable)
	if csvTable == nil then
		return -1, "没有找到:"..tbName
	end

	local filedList = csvTable[1];
	local tpList = csvTable[2];
	
	-- 是否有标题和类型定义
	if filedList == nil or tpList == nil then
		return -1, "["..tbName..g_tyErrmsgHead.."标题为空，或者类型定义为空"
	end
	
	-- 标题和类型定义数量是否一致
	if table.getn(filedList) ~= table.getn(tpList) then
		return -1, "["..tbName..g_tyErrmsgHead.."标题有"..table.getn(filedList).."列，类型定义只有"..table.getn(tpList).."列"
	end
	
	-- 类型定义是否合法
	for i = 1, table.getn(tpList) do
		local tyName = tpList[i]
		if g_typeDefine[tyName] ~= 1 then
			return -1, "["..tbName..g_tyErrmsgHead.."["..filedList[i].."]类型定义非法:"..tostring(tyName)
		end
	end
	
	-- 检测每一行的数据是否合法
	for i = 3, table.getn(csvTable) do
		local rowData = csvTable[i]
		
		-- 先验证列数目是否正确
		if table.getn(rowData) ~= table.getn(tpList) then
			return -1, "["..tbName..g_tyErrmsgHead.."第"..i.."行数据只有"..table.getn(rowData).."列，实际需要:"..table.getn(tpList).."列"
		end
		
		for field = 1, table.getn(tpList) do
			result = chceckSingleType(rowData[field], tpList[field]);
			if result == false then
				return -1, "["..tbName..g_tyErrmsgHead.."第"..i.."行"..field.."列数据类型错误:"..tostring(rowData[field]).."类型:"..tpList[field]
			end
		end
	end
	
	return 1, "OK"
end

function makeTypeCheck(tableName)
	return function()
		local tbName = tableName;
		logCheckMsg("开始基本类型检查:"..tableName);
		local csvTable = getCsvTable(tableName);
		local  result, errmsg = csvTypeCheckImp(tableName, csvTable)
		if result >= 0 then
			logCheckMsg("类型检查成功:"..tableName);
		end
		return result, errmsg
	end
end

function CheckType_AddCheck(tableName, args)
	addCheckItem(makeTypeCheck(tableName) )
end

