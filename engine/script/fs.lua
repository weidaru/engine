local m = {}

function m.get_os() 
	if package.config:sub(1,1)=="/" then
		return "unix"
	elseif package.config:sub(1,1)=="\\" then
		return "windows"
	end
end

local function exec_dir_command_windows(dir, patterns)
	local buffer = {}
	table.insert(buffer, "dir ")
	for _,v in ipairs(patterns) do
		table.insert(buffer, dir)
		table.insert(buffer, "\\")
		table.insert(buffer, v)
		table.insert(buffer, " ")
	end
	table.insert(buffer, "/s/b/a-s")

	return io.popen(table.concat(buffer)) 
end

local function exec_dir_command_unix(dir, patterns)
	local buffer = {}
	table.insert(buffer, "find ")
	table.insert(buffer, dir)
	for _,v in ipairs(patterns) do
		table.insert(buffer, string.format([[-name "%s" ]], v))
		table.insert(buffer, string.format("-or "))
	end
	table.remove(buffer)

	return io.popen(table.concat(buffer))
end

local function exec_dir_command(dir, patterns)
	if m.get_os()=="windows" then
		return exec_dir_command_windows(dir, patterns)
	elseif m.get_os()=="unix" then
		return exec_dir_command_unix(dir, patterns)
	else
		return nil 
	end
end

function m.scan_dir(dir, patterns, func) 
	local f=exec_dir_command(dir, patterns)
	
	assert(f, "Cannot list related files")
	
	for line in f:lines() do
		func(line)
	end
end

function m.get_last_entry(path)
	local sep = package.config:sub(1,1)
	for i=path:len(),1,-1 do
		if path:sub(i,i) == sep then
			return path:sub(i+1)
		end
	end
	return nil
end

return m















