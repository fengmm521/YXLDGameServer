
-- 日志
function Log( fileNameOrMsg, msg )
-- if getChannelID() == "PC" then 
	if( msg == nil )then
		CCLuaLog( fileNameOrMsg );
		LogFile( fileNameOrMsg );
	else
		CCLuaLog( msg );
		LogFile( fileNameOrMsg, msg );
	end
-- else 
--     return 
-- end 
end

function Util_GetFunction( funcName )
	local func = _G[funcName]
	if( func == nil )then
		return nil
	end
	if( type(func) ~= "function" )then
		return nil
	end
	return func
end

function TO_CCPOINT( fmPoint )
	return CCPoint:new_local( fmPoint.x, fmPoint.y );
end

function TO_FMPOINT( ccPoint )
	return FmPoint:new_local( ccPoint.x, ccPoint.y );
end

function Stream_WriteVector(stream, args)
	local argSize = args:size();

	stream:Write_uint(argSize);
	for i=0, argSize-1 do
		args[i]:Serialize( stream )
	end
end

function Stream_ReadVector(stream)
	local args = vector_Var_:new_local();
	
	local argsSize = stream:Read_uint();
	for i=0, argsSize-1 do
		local arg = Var:new_local()
		arg:Deserialize( stream )
		args:push_back( arg )
	end
	
	return args;
end


function Packet_Cmd(iCmd)
	local msg = RouterServer_pb.CSMessage();
	msg.iCmd = iCmd;
	Log("%%%%%%%"..iCmd)
	local data = msg:SerializeToString();
	ClientSink:GetInstance():sendStringPacket(data, string.len(data));
end

function Packet_Full(iCmd, sBody)
	local msg = RouterServer_pb.CSMessage();
	msg.iCmd = iCmd;
	if (sBody) then
		msg.strMsgBody = sBody:SerializeToString();
	end
	local data = msg:SerializeToString();
	Log("%%%%%%%"..iCmd)
	Log("^^^^^^^^"..tostring(sBody))
	ClientSink:GetInstance():sendStringPacket(data, string.len(data));
end

-- 获取游戏中的资源
function GetGameData( storageID, dataID, structName )
	local data = CDataManager:GetInstance():GetGameDataUnKnow( storageID, dataID );
	if( data == nil )then
		return nil;
	end
	return tolua.cast( data, structName );
end


LoadFile( "Util/TypeUtility.lua" );
LoadFile( "Util/StringUtility.lua" );
LoadFile( "Util/TableUtility.lua" );
--LoadFile( "Util/Global.lua" );
