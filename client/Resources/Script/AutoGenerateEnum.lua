--[[
	在此配置c++头文件
]]--

local cleintPath = "../Classes/";

local fileList = 
{
	cleintPath.."Enum/FmComponentEnum.h",
	cleintPath.."Enum/FmNumricEnum.h",
	cleintPath.."Enum/FmEntityRenderEnum.h",
	cleintPath.."Enum/FmCommonEnum.h",
}



--[[
	根据c++的枚举文件自动生成lua文件
]]--
function EnumCppToLua( cppFile, outputPath )
	local luaStrings = "";
	local lastEnumValue = -1;
	local findEnum = false;
	local findLeftBracket = false;
	for line in io.lines( cppFile ) do
		--print( line );
		line = RemoveComment( line );	-- 删除注释
		line = StringUtil:Trim( line );	-- 删除头尾的空格
		--print( line );
		-- 先找到Enum关键字
		if( findEnum == false )then
			local beginIdx,endIdx = string.find( line, "enum" );
			if( beginIdx ~= nil and beginIdx == 1 )then
				findEnum = true;
				-- 再找左括号
				beginIdx,endIdx = string.find( line, "{" );
				if( beginIdx ~= nil )then
					findLeftBracket = true;
				end
			end
		else
			-- 再找左括号
			if( findLeftBracket == false )then
				local beginIdx,endIdx = string.find( line, "{" );
				if( beginIdx ~= nil )then
					findLeftBracket = true;
				end
			else
				-- 再找右括号
				local beginIdx,endIdx = string.find( line, "}" );
				if( beginIdx ~= nil )then
					-- 枚举结束
					lastEnumValue = -1;
					findEnum = false;
					findLeftBracket = false;
					luaStrings = luaStrings .. "\n";
				else
					-- 这一行是枚举值
					beginIdx,endIdx = string.find( line, "," );
					-- 逗号不是必须的
					if( beginIdx == nil )then
						beginIdx = string.len( line ) + 1;
					end
					if( beginIdx ~= nil )then
						line = string.sub( line, 1, beginIdx-1 );
						beginIdx,endIdx = string.find( line, "=" );
						local enumName = "";
						local enumValue = 0;
						-- 有具体赋值的
						if( beginIdx ~= nil )then
							enumName = string.sub( line, 1, beginIdx-1 );
							enumName = StringUtil:Trim( enumName );
							if( string.len(enumName) > 0 )then
								local enumValueString = string.sub( line, endIdx+1 );
								enumValueString = StringUtil:Trim( enumValueString );
								enumValue = tonumber( enumValueString );
								lastEnumValue = enumValue;
							end
						-- 递增数值的
						else
							enumName = StringUtil:Trim( line );
							if( string.len(enumName) > 0 )then
								enumValue = lastEnumValue + 1;
								lastEnumValue = enumValue;
							end
						end
						if( string.len(enumName) > 0 )then
							if( enumValue == nil )then
								print("enumValue == nil enumName="..enumName);
							end
							luaStrings = luaStrings .. enumName .. "=" .. enumValue .. "\n";
						end
						--print( string.format( "%s = %d", enumName, enumValue or -100 ) );
					end
				end
			end
		end
	end
	
	local fullName,baseName,pathName = StringUtil:SplitFilename( cppFile, nil, nil );
	baseName = string.sub( baseName, 1, string.len(baseName)-2 ); -- 去除.h
	local luaFileName = baseName..".lua";
	if( outputPath ~= nil )then
		luaFileName = outputPath.."/"..luaFileName;
	end
	local f = io.open( luaFileName, "w+" );
	if( f ~= nil )then
		f:write( luaStrings );
		f:flush();
		f:close();
	else
		print( string.format("open file error:%s", luaFileName) );
	end
end

-- 去除注释
function RemoveComment( line )
	local beginIdx,endIdx = string.find( line, "//" );
	if( beginIdx ~= nil )then
		return string.sub( line, 1, beginIdx-1 );
	end
	return line;
end




local luaStrings = "";

for index,cppFile in pairs( fileList ) do
	-- cppFile = cppFile;
	local fullName,baseName,pathName = StringUtil:SplitFilename( cppFile, nil, nil );
	baseName = string.sub( baseName, 1, string.len(baseName)-2 ); -- 去除.h
	local luaFileName = baseName..".lua";
	EnumCppToLua( cppFile );
	
	luaStrings = luaStrings.."LoadFile('Enum/"..luaFileName.."')\n";
	
	os.execute( string.format("copy .\\%s .\\..\\Resources\\Script\\Enum\\", luaFileName) );
	os.execute( string.format("del .\\%s", luaFileName) );
end

local f = io.open( "EnumAutoGenerate.lua", "w+" );
if( f ~= nil )then
	f:write( luaStrings );
	f:flush();
	f:close();
else
	print( string.format("open file error:%s", "EnumAutoGenerate.lua") );
end

os.execute( string.format("copy .\\%s .\\..\\Resources\\Script\\Enum\\", "EnumAutoGenerate.lua" ) );
os.execute( string.format("del .\\%s", "EnumAutoGenerate.lua") );
