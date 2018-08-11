function processErrorCodeDef(input, output)
	local f, errmsg = io.lines(input)
	if f == nil then
		print("open "..input.." fail:"..errmsg)
		return false
	end

	local headString="协议枚举名,错误值,错误描述\r\n"
	headString = headString.."string,int,utf8string\r\n"

	local outString = ""
	local lastValue = 0;
	local rowcount =0;
	while true do
		local lineData = f();
		if lineData == nil then
			break;
		end
		
		-- 去掉空格 TAB 和注释
		lineData = string.gsub(lineData, " ", "")
		lineData = string.gsub(lineData, "\t", "")	
		lineData = string.gsub(lineData, "=", ",");
		lineData = string.gsub(lineData, "//", "");
		local validRow = string.find(lineData, "ERROR_");
		if( nil ~= validRow ) then
			outString =outString..lineData.."\n";
		end;
	end
	--print(headString..outString);
	if string.len(outString) == 0 then
		return
	end
	
	local wf, errmsg = io.open(output, "wb+")
	if wf == nil then
		print("open "..output.." fail:"..errmsg)
		return false;
	end
	--print(headString..outString);
	wf:write(headString..outString);
	wf:flush()
	wf:close()
end



function makeErrorCode(input, outDir, output)
	processErrorCodeDef(input, outDir.."/"..output);
end
if table.getn(arg) ~= 3 then
	print("useage:[input] [outdir] [output]")
	os.exit(-2);
	return;
end
print("parse:"..arg[1].." to:"..arg[2].."/"..arg[3])
makeErrorCode(arg[1], arg[2], arg[3])
