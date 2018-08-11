
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
