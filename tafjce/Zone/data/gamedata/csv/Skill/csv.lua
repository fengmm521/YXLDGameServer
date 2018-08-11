
function doParseCsv (s, eComment)
  s = s        -- ending comma
  
  -- 先去掉注释
  if eComment == true then
	while true do
		if string.sub(s, 1, 2) == "##" then
			local nextPos = string.find(s, '\n');
			if nextPos == nil then
				return {};
			else
				s = string.sub(s, nextPos+1)
			end
		else
			break;
		end
	end
  end
  local lineTb = {}
  local t = {}        -- table to collect fields
  local fieldstart = 1
  repeat
	-- next field is quoted? (start with `"'?)
	if string.find(s, '^"', fieldstart) then
	  local a, c
	  local i  = fieldstart
	  repeat
		-- find closing quote
		a, i, c = string.find(s, '"("?)', i+1)
	  until c ~= '"'    -- quote not followed by quote?
	  if not i then error('unmatched "') end
	  local f = string.sub(s, fieldstart+1, i-1)
	  table.insert(t, (string.gsub(f, '""', '"')))
	  
	  local nextLinePos = string.find(s, '\n', i) + 1
	  fieldstart = string.find(s, ',', i) + 1
	  if nextLinePos < fieldstart then
		table.insert(lineTb, t);
		t = {}
		fieldstart = nextLinePos;
		--print("line:"..table.getn(lineTb) )
	  end
	else                -- unquoted; find next comma
	  local nextLinePos = string.find(s, '\n', fieldstart)
	  local nexti = string.find(s, ',', fieldstart)
	  if nexti == nil or  nextLinePos < nexti then
		chgLine = true;
		nexti = nextLinePos
		table.insert(t, string.sub(s, fieldstart, nexti-2))
		table.insert(lineTb, t);
		t = {}
		--print("line:"..table.getn(lineTb) )
	  else
		table.insert(t, string.sub(s, fieldstart, nexti-1))
	  end
	  fieldstart = nexti + 1
	end
  until fieldstart > string.len(s)
  return lineTb
end

function parseCsv(fileName)
	local fHandle = io.open(fileName, "rb");
	local fileData = fHandle:read("*all");
	fHandle:close()
	
	if string.sub(fileData, string.len(fileData), string.len(fileData) ) ~= "\n" then
		fileData = fileData.."\r\n"
	end
	
	local csvTb = doParseCsv(fileData, true);

	return csvTb
end
function check()
	--要查询的表的路径
	local csvTb = parseCsv("../../../Design/Numeric/Skill/SkillClientEffect.csv")
	--查询结果保存路径
	g_check_checkEnd = assert(io.open("../../../Design/Numeric/Skill/checkEnd.txt", 'w'))
	--所有动画所在的总目录
	local path = "../../../Art/Effect/"
	--子弹特效
	g_check_error = {}
	findData(csvTb,9,path)
	local path = "../../../Art/Effect/"
	--受击特效
	findData(csvTb,12,path)
	--特效与攻击类型
	checkBulletKindAndBulletEffect(csvTb,9,8)
	message("==============END===============",false)

	for i=1,#g_check_error,1 do
		g_check_checkEnd:write(g_check_error[i].."\n")
		print(g_check_error[i])
	end

	g_check_checkEnd:close()
end

function findData(csvTb,line,path)
	for i=3,#csvTb,1 do
		--9代表要查询的动画在csv中的第几数列
		if csvTb[i][line] ~= nil and csvTb[i][line] ~= "" then
			g_fHandle = io.open(path..csvTb[i][line].."/"..csvTb[i][line]..".ExportJson", "rb");
			g_lineData = nil
			if g_fHandle == nil then
				message(csvTb[i][line].."	 can not find",true)
			else
				if findLine("name","Can not find index \"name\" in "..csvTb[i][line]) then
					local animateName = "\""..csvTb[i][line].."\""
					local findName = string.find(g_lineData,animateName)
					if findName == nil then
						message(csvTb[i][line].."	 is name error",true)
					else
						if findLine("animation_data",csvTb[i][line].."	 can not find animation that is named 0") then

							if findLine("name",csvTb[i][line].."	 can not find animation that is named 0") then
								local animateName = "\""..csvTb[i][line].."\""
								local findName = string.find(g_lineData,animateName)
								if findName == nil then
									message(csvTb[i][line].."	 is name error than animation is named 0",true)
								else
									if findLine("name",csvTb[i][line].."	 can not find animation that is named 0") then
										local animateName = "0"
										local findName = string.find(g_lineData,animateName)
										if findName == nil then
											message(csvTb[i][line].."	 is name error that animation should be named 0",true)
										else
											message(csvTb[i][line].."	 is name OK",false)
										end
									end
								end
							end
						end
					end
				end
			end
		end
	end					
end
function findLine(str,msg)
	g_lineData = nil
	repeat
		g_lineData = g_fHandle:read("*line")
	until g_lineData==nil or string.find(g_lineData,str)
	if g_lineData ~= nil then
		return true
	else
		message(msg,true)
		return false
	end
end

function checkBulletKindAndBulletEffect(csvTb,lineEffect,lineKind,path)
	for i=3,#csvTb,1 do
		if csvTb[i][lineEffect] == nil or csvTb[i][lineEffect]=="" then
			if csvTb[i][lineKind] ~= "0" then
				message("The Effect is nil and the bulletKind is not 0 in line "..i+1,true)
			else 
				message("The Effect and bulletKind is OK in line "..i+1,false)
			end
		else
			if csvTb[i][lineKind] == "1" or csvTb[i][lineKind] == "2" then
				message("The Effect and bulletKind is OK in line "..i+1,false)
			else
				message("The Effect is \""..csvTb[i][lineEffect].."\" and the bulletKind is not 1 and 2 in line "..i+1,true)
			end
		end
	end
end
function message( msg , isError)
	g_check_checkEnd:write(msg.."\n")
	print(msg)
	if isError == true then
		g_check_error[#g_check_error+1] = msg
	end
end
--parseCsv()

check();