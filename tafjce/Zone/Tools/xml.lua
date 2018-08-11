function parseargs (s)
  local arg = {}
  string.gsub(s, "(%w+)=([\"'])(.-)%2", function (w, _, a)
    arg[w] = a
    --print(w, a)
  end)
  return arg
end

function tinsert(a, b)
	table.insert(a, b);
	a.n = a.n+1
end



function collect (s)
  --œ»¥¶¿Ì◊¢ Õ
  s = string.gsub(s, "<!%-%-.-%-%->", "")
  
  local stack = {n=0}
  local top = {n=0}
  tinsert(stack, top)
  local ni,c,label,args, empty
  local i, j = 1, 1
  while 1 do
  
    ni,j,c,label,args, empty = string.find(s, "<(%/?)([%w:]+)(.-)(%/?)>", i)
    if not ni then break end
    local text = string.sub(s, i, ni-1)
    if not string.find(text, "^%s*$") then
      tinsert(top, text)
    end
    if empty == "/" then  -- empty element tag
      tinsert(top, {n=0, label=label, args=parseargs(args), empty=1})
    elseif c == "" then   -- start tag
      top = {n=0, label=label, args=parseargs(args)}
      tinsert(stack, top)   -- new level
    else  -- end tag
      local toclose = table.remove(stack)  -- remove top
      stack.n = stack.n - 1
      top = stack[stack.n]
      if stack.n < 1 then
        print("nothing to close with "..label)
      end
      if toclose.label ~= label then
        print("trying to close "..toclose.label.." with "..label)
      end
      tinsert(top, toclose)
    end 
    i = j+1
  end
  local text = string.sub(s, i)
  if not string.find(text, "^%s*$") then
    tinsert(stack[stack.n], text)
  end
  if stack.n > 1 then
    print("unclosed "..stack[stack.n].label)
  end
  return stack[1]
end

function parseXml(fileName)
	local fHandle = io.open(fileName, "rb");
	local fileData = fHandle:read("*all");
	fHandle:close()

	local xmlTb = collect(fileData);
	return xmlTb
end
