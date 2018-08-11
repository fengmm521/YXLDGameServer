
function readFileData(fileName)
	local f = io.open(fileName, "rb");
	if f == nil then
		return "";
	end
	local tmpData = f:read("*all");
	f:close();
	
	return tmpData
end

function checkFileSame(leftFile, rightFile)
	local leftData = readFileData(leftFile)
	local rightData = readFileData(rightFile);
	
	if leftData == rightData then
		return true;
	end
	
	return false;
end

function mkProtobuf(fileName)
	print("protofile:"..fileName)
	
	local filePureName = nil
	local fileExt = nil
	
	local function parseFileName(a, b)
		filePureName = a
		fileExt = b
	end
	string.gsub(fileName, "(%w+)\.(%w+)", parseFileName)
	
	if filePureName == nil or fileExt == nil then
		print("fileName error:"..fileName)
		os.exit(-3);
		return;
	end
	
	if 0 ~= os.execute("rm -rf tmppb") then
		os.exit(-3);
		return;
	end
	
	if 0 ~= os.execute("mkdir tmppb") then
		os.exit(-3);
		return;
	end
	
	if 0 ~= os.execute("protoc --cpp_out=tmppb "..fileName) then
		os.exit(-3);
		return;
	end
	
	-- 比较新老文件
	local createHeadFile = filePureName..".pb.h";
	local createCppFile = filePureName..".pb.cc"

	local oldHeadF = io.open(createHeadFile, "rb");
	local oldCppF = io.open(createCppFile, "rb");
	
	-- 如果其中一个旧文件不存在，复制新文件
	if oldHeadF == nil or oldCppF == nil then
		print("not exist")
		if 0 ~= os.execute("cp tmppb/"..createHeadFile.." ./") then
			os.exit(-3);
			return
		end
		if 0 ~= os.execute("cp tmppb/"..createCppFile.." ./") then
			os.exit(-3);
			return
		end
		
		if oldHeadF ~= nil then
			io.close(oldHeadF);
		end
		
		if oldCppF ~= nil then
			io.close(oldCppF);
		end
		return;
	end
	
	-- 读取新老文件，比较
	local headEqual = checkFileSame("./"..createHeadFile, "tmppb/"..createHeadFile);
	local ccEqual = checkFileSame("./"..createCppFile, "tmppb/"..createCppFile);
	if headEqual ~= true or ccEqual ~= true then
		if headEqual ~= true then
			print(" head not equal")
		end
		
		if ccEqual ~= true then
			print(" cc not equal")
		end
		
		if 0 ~= os.execute("cp tmppb/"..createHeadFile.." ./") then
			os.exit(-3);
			return
		end
		if 0 ~= os.execute("cp tmppb/"..createCppFile.." ./") then
			os.exit(-3);
			return
		end
		
		if oldHeadF ~= nil then
			io.close(oldHeadF);
		end
		
		if oldCppF ~= nil then
			io.close(oldCppF);
		end
		return;
	end
end

if #arg ~= 1 then
	print("useage:[protofile]")
	os.exit(-2);
	return;
end

mkProtobuf(arg[1])
