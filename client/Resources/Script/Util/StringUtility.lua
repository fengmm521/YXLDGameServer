--[[
字符串常用函数
]]--

-- 分解字符串,以空格分开,返回一个字符串表
-- 参数:以splitChar分隔开的字符串
function SplitString( str, splitChar )
	local strTab = {}
	local len = string.len( str )
	local tmpStr = "";
	if( splitChar == nil )then
		splitChar = " ";
	end
	local space = string.byte( splitChar );
	--Log( "space = "..space ); --32
	for i=1,len do
		if( string.byte(str,i) == space )then
			--Log( "tmpStr = "..tmpStr );
			if( string.len( tmpStr ) > 0 )then
				strTab[#strTab+1] = tmpStr;
			end
			tmpStr = "";
		else
			tmpStr = tmpStr..string.char(string.byte(str,i));
		end
	end
	if( string.len( tmpStr ) > 0 )then
		strTab[#strTab+1] = tmpStr;
		--Log( "tmpStr = "..tmpStr );
	end
	return strTab;
end

-- 格式化字符串
function FormatString( key, arg1, arg2, arg3, arg4, arg5, arg6 )
	if( arg1 == nil )then
		return StringMgr:GetInstance():FormatString( key )
	end
	local argTable = {}
	if( arg1 )then
		argTable[#argTable+1] = arg1
		if( arg2 )then
			argTable[#argTable+1] = arg2
			if( arg3 )then
				argTable[#argTable+1] = arg3
				if( arg4 )then
					argTable[#argTable+1] = arg4
					if( arg5 )then
						argTable[#argTable+1] = arg5
						if( arg6 )then
							argTable[#argTable+1] = arg6
						end
					end
				end
			end
		end
	end
	local args = vector_Var_:new_local();
	for i,arg in ipairs(argTable) do
		local newArg = Var:new_local()
		if( type(arg) == "number" )then
			newArg:InitFromString( ETypeId_double, tostring(arg) )
		elseif( type(arg) == "string" )then
			newArg:InitFromString( ETypeId_string, arg )
		end
		args:push_back( newArg )
	end
	return StringMgr:GetInstance():FormatStringArgs( key, args )
end

-- 检查电话号码是否合法
function String_CheckPhoneNo( str )
	if( string.find( str, "1%d%d%d%d%d%d%d%d%d%d" ) )then
		return true
	end
	return false
end

-- 检查邮箱是否合法
function String_CheckEmail( str )
	if( string.find( str, "%w+@%w+[.]%w+" ) )then
		return true
	end
	return false
end

-- 检查身份证是否合法
function String_CheckIdCard( str )
	local len = string.len( str )
	if( len == 15 )then
		-- 15位全是数字
		if( string.find( str, "%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d" ) )then
			return true
		end
	elseif( len == 18 )then
		-- 18位的最后一位可能是X
		if( string.find( str, "%d%d%d%d%d%d19%d%d%d%d%d%d%d%d%d%w" ) )then
			return true
		end
		if( string.find( str, "%d%d%d%d%d%d20%d%d%d%d%d%d%d%d%d%w" ) )then
			return true
		end
	end
	return false
end

-- 检查用户名是否合法(3-14位英文或数字)
function String_CheckUserName( str )
	local len = string.len( str )
	if( len < 3 or len > 14 )then
		return false
	end
	if( string.find( str, "%W" ) )then
		return false
	end
	-- 防止玩家起"youke"开头的账号
	if( string.find( str, "youke%d+" ) )then
		return false
	end
	return true
end

-- 检查密码是否合法(6-14位英文或数字)
function String_CheckUserPwd( str )
	local len = string.len( str )
	if( len < 6 or len > 14 )then
		return false
	end
	if( string.find( str, "%W" ) )then
		return false
	end
	return true
end
