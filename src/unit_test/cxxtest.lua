--
-- SelfPortrait API
-- See Copyright Notice in lua_utils.h
--
require "debug"

function TS_ASSERT(code)

    if (type(code) ~= "string") then
        if not code then
            local info = debug.getinfo(2, "Sl");
            TS_FAIL(info.short_src, info.currentline, "Failed assertion")
        end
        return
    end

    local info = debug.getinfo(2, "Sluf");

    local calling_globals = {}
    if _VERSION == "Lua 5.2" then
        --calling_globals = debug.getlocal(2, i)
        for i=1,math.huge do
            local name, value = debug.getupvalue(info.func, i)
            if not name or name == "_ENV" then
                calling_globals = value or {}
                break
            end
        end
    else
        calling_globals = getfenv(2)
    end

    local calling_locals  = {}
    for i=1,math.huge do
        local name, value = debug.getlocal(2, i)
        if not name then break end
        calling_locals[name] = value
    end



    local up = 1
    while up <= info.nups do
        local name, value = debug.getupvalue(info.func, up)
        calling_locals[name] = value
        up = up + 1
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

    local func, err
    local chunk = "return "..code
    if _VERSION == "Lua 5.2" then
        func, err = load(chunk, chunk, "t", env)
    else
        func, err = loadstring(chunk)
        setfenv(func, env)
    end
    if not func then
        local info = debug.getinfo(2, "Sl");
        local msg = {"Uncompilable test expression at ", info.short_src, ":", info.currentline, ":" , tostring(err) }
        print(tostring(table.concat))
        error(table.concat(msg))
    end


    if not func() then
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

