
function processPropDef(input, output)
	local f, errmsg = io.lines(input)
	if f == nil then
		print("open "..input.." fail:"..errmsg)
		return false
	end

	local headString="key,value,type\r\n"
	headString = headString.."string,int,string\r\n"
	
	local outString = ""
	
	local lastValue = 0;
	while true do
		local lineData = f();
		if lineData == nil then
			break;
		end
		
		-- 去掉空格
		lineData = string.gsub(lineData, " ", "")
		lineData = string.gsub(lineData, "\t", "")
		
		-- 是否注释行
		local tmp, pos = string.gsub(lineData, "^//", "");
		
		
		local keyStr = ""
		local keyValue = 0
		local keyType = "int"
				
		local function createParamFun(descIndex, valueIndex)
			local paramList = {}
			local function pushValue(data)
				--print("push:"..data)
				table.insert(paramList, data);
			end
			
			return function(data)
				data = data..","
				--print("content:"..data)
				paramList = {}
				string.gsub(data, "(.-),", pushValue)
				keyStr = paramList[descIndex]
				keyStr = string.gsub(keyStr, "\"", "")
				keyType = paramList[2];
				if nil == valueIndex then
					keyValue = lastValue
					lastValue = lastValue + 1
				else
					keyValue = paramList[valueIndex];
					lastValue = tonumber(keyValue)
					lastValue = lastValue + 1
				end
				--print(keyStr, keyValue)
			end
		end
		
		if pos == 0 then
			
			string.gsub(lineData, "^PROP_SIMPLEDEFIMP[\(](.+)[\)]", createParamFun(3))
			string.gsub(lineData, "^PROP_DEFIMP[\(](.+)[\)]", createParamFun(4, 3))
			if string.len(keyStr) > 0 then
				outString =outString..keyStr..","..keyValue..","..keyType.."\r\n"
			end
		end
	end
	
	if string.len(outString) == 0 then
		return
	end
	
	local wf, errmsg = io.open(output, "wb")
	if wf == nil then
		print("open "..output.." fail:"..errmsg)
		return false;
	end
	
	wf:write(headString..outString);
	wf:flush()
	wf:close()
end



function makeProperty(input, outDir, output)
	processPropDef(input, outDir.."/"..output);
end
if table.getn(arg) ~= 3 then
	print("useage:[input] [outdir] [output]")
	os.exit(-2);
	return;
end
print("parse:"..arg[1].." to:"..arg[2].."/"..arg[3])
makeProperty(arg[1], arg[2], arg[3])
