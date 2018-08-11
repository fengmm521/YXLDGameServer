--[[
table常用函数
]]--

-- 默认排序函数
function DefaultCompFunc( a, b )
	return a.Key > b.Key;
end

-- 对非数组table进行排序
function SortTable( t, compFunc )
	if( compFunc == nil )then
		compFunc = DefaultCompFunc;
	end
	local sortTable = {}
	for k,v in pairs( t ) do
		sortTable[#sortTable+1] = { Key=k, Value=v };
	end
	table.sort( sortTable, compFunc );
	return sortTable;
end

--打印table里的数据
function PrintTable( t, space )
	if( space == nil ) then space = "" end
	if( t == nil )then
		Log( space.."nil" )
		return
	end
	if( type(t) ~= "table" )then
			Log( space.."NOT a table" )
		return
	end
	Log( space.."{" )
	for k,v in pairs(t) do
		if( type(v) ~= "table" )then
			if( type(v) == "string" )then
				if( type(k) == "number" )then
					Log( space.."["..k.."]='"..tostring(v).."'" )
				else
					Log( space..k.."='"..tostring(v).."'" )
				end
			else
				if( type(k) == "number" )then
					Log( space.."["..k.."]="..tostring(v) )
				else
					Log( space..k.."="..tostring(v) )
				end
			end
		else
			if( type(k) == "number" )then
				Log( space.."["..k.."]=" )
			else
				Log( space..k.."=" )
			end
			spaceNext = space.."  "
			PrintTable( v, spaceNext )
		end
	end
	Log( space.."}" )
end

-- 打印实体的表数据
function PrintEntityTable( entity, tableName )
	local rowCount = entity:TableGetRows( tableName );
	local columnCount = entity:TableGetColumns( tableName );
	Log( "rowCount="..rowCount.." columnCount="..columnCount );
	for i=0,rowCount-1 do
		local rowString = "";
		for c=0,columnCount-1 do
			local row = entity:TableGetCell( tableName, i, c);
			rowString = rowString..tostring(row)..",";
		end
		Log( rowString );
	end
end

function PrintPlayerTable( entityId, tableName )
	local entity = Entity( entityId, 4 );
	PrintEntityTable( entity, tableName );
end

function PrintTableFile( fileName, t, space )
	if( space == nil ) then space = "" end
	if( t == nil )then
		Log( space.."nil" )
		return
	end
	if( type(t) ~= "table" )then
			Log( space.."NOT a table" )
		return
	end
	LogFile( fileName, space.."{" )
	for k,v in pairs(t) do
		if( type(v) ~= "table" )then
			if( type(v) == "string" )then
				if( type(k) == "number" )then
					LogFile( fileName, space.."["..k.."]='"..tostring(v).."'" )
				else
					LogFile( fileName, space..k.."='"..tostring(v).."'" )
				end
			else
				if( type(k) == "number" )then
					LogFile( fileName, space.."["..k.."]="..tostring(v) )
				else
					LogFile( fileName, space..k.."="..tostring(v) )
				end
			end
		else
			if( type(k) == "number" )then
				LogFile( fileName, space.."["..k.."]=" )
			else
				LogFile( fileName, space..k.."=" )
			end
			spaceNext = space.."  "
			PrintTableFile( fileName, v, spaceNext )
		end
	end
	LogFile( fileName, space.."}" )
end
