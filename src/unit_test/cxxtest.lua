function TS_ASSERT(code)

    if (type(code) ~= "string") then
        if not code then
            local info = debug.getinfo(2, "Sl");
            local msg = {"Failed assertion"}
            TS_FAIL(info.short_src, info.currentline, table.concat(msg))
        end
        return
    end

    local func = assert(loadstring("return "..code))

    local calling_globals = getfenv(2)
    local calling_locals  = {}
    local i = 1
    while true do
        local name, value = debug.getlocal(2, i)
        if not name then break end
        calling_locals[name] = value
        i = i + 1
    end

    local notpresent = {}
    setmetatable(calling_locals, { __index=function() return notpresent end})

    local referenced = {}


    local env = {}

    setmetatable(env, {__index=
        function(table, name)
            local value = calling_locals[name]
            if value == notpresent then value = calling_globals[name] end
            referenced[name] = value
            return value
        end
    })

    setfenv(func, env)
    if not func() then
       local info = debug.getinfo(2, "Sl");
       local msg = {"Failed assertion: ", code}

       local first = true
       for k, v in pairs(referenced) do
           if first then
               msg[#msg+1]= ". Variables: "
           else
               msg[#msg+1] = ", "
           end
           msg[#msg+1]= tostring(k).." = "..tostring(v)
           first = false
       end
       TS_FAIL(info.short_src, info.currentline, table.concat(msg))
    end
end

