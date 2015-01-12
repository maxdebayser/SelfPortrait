require "libluaselfportrait"
require "cxxtest"

function testFunction()

    local globalFunctions = Function.lookup("FunctionTest::globalFunction")
    TS_ASSERT(globalFunctions)
    TS_ASSERT[[type(globalFunctions) == "table"]]
    TS_ASSERT[[#globalFunctions == 2]]

    local funcs = {}
    for _,v in ipairs(globalFunctions) do
        funcs[table.concat(v:argumentSpellings(), ",")] = v
    end

    local func1 = funcs["double,double"]

    TS_ASSERT(func1)
    TS_ASSERT[[ func1:numberOfArguments() == 2 ]]
    TS_ASSERT[[ func1:call(3, 5) == 8 ]]
    TS_ASSERT[[ func1:call(2, 4) == 6 ]]


    local func2 = funcs["int,int"]
    TS_ASSERT(func2)
    TS_ASSERT[[ func2:numberOfArguments() == 2 ]]
    TS_ASSERT[[ func2:call(3.3, 5.5) == 8 ]]
    TS_ASSERT[[ func2:call(2, 4) == 6 ]]


    return true
end

local CopyCount = Class.lookup("FunctionTest::CopyCount")

local constr = CopyCount:constructors()[1]

local methods = {}
for _,v in ipairs(CopyCount:methods()) do
    methods[v:name()] = v
end


function testReturnByValue()

    --TODO: this test is broken because internally the module now uses const char* instead of std::string to pass strings

    methods["resetAll"]:call()

    local c = Function.lookup("FunctionTest::returnObjectByValue"):call()

    TS_ASSERT(c)
    TS_ASSERT[[ methods["id"]:call(c) == 33 ]]

    local copies = methods["numberOfCopies"]:call()
    TS_ASSERT[[ copies == 0 ]]

    return true
end

function testReturnByReference()

    methods["resetAll"]:call()

    local c = Function.lookup("FunctionTest::returnObjectByReference"):call()

    TS_ASSERT(c)
    methods["changeId"]:call(c, 45)
    TS_ASSERT[[ methods["id"]:call(c) == 45 ]]

    methods["changeId"]:call(c, 35)
    TS_ASSERT[[ methods["id"]:call(c) == 35 ]]

    local c2 = Function.lookup("FunctionTest::returnObjectByReference"):call()
    TS_ASSERT[[ methods["id"]:call(c2) == 35 ]]

    local copies = methods["numberOfCopies"]:call()
    TS_ASSERT[[ copies == 0 ]]

    return true
end

function testReturnByConstReference()

    methods["resetAll"]:call()

    local c = Function.lookup("FunctionTest::returnObjectByConstReference"):call()

    TS_ASSERT(c)
    TS_ASSERT[[ methods["id"]:call(c) == 888 ]]
    if pcall( function() methods["changeId"]:call(c, 999) end) then
        TS_FAIL("expected exception when changing const object")
    end

    local copies = methods["numberOfCopies"]:call()
    TS_ASSERT[[ copies == 0 ]]
    return true
end

function testParameterByValue()

    methods["resetAll"]:call()

    local c = constr:call(99)
    TS_ASSERT(c)
    TS_ASSERT[[ methods["id"]:call(c) == 99 ]]

    local id = Function.lookup("FunctionTest::paramByValue"):call(c)
    TS_ASSERT[[ id == 99 ]]

    local copies = methods["numberOfCopies"]:call()
    local moves = methods["numberOfMoves"]:call()

    TS_ASSERT[[ copies == 0 ]] --estranho
    TS_ASSERT[[ moves == 2 ]]

    return true
end

function testParameterByReference()

    methods["resetAll"]:call()

    local c = constr:call(99)
    TS_ASSERT(c)
    TS_ASSERT[[ methods["id"]:call(c) == 99 ]]

    local id = Function.lookup("FunctionTest::paramByReference"):call(c)
    TS_ASSERT[[ id == 100 ]]
    TS_ASSERT[[ methods["id"]:call(c) == 100 ]]

    local copies = methods["numberOfCopies"]:call()
    local moves = methods["numberOfMoves"]:call()

    TS_ASSERT[[ copies == 0 ]] --estranho
    TS_ASSERT[[ moves == 0 ]]
    return true
end

function testParameterByConstReference()

    methods["resetAll"]:call()

    local c = constr:call(99)
    TS_ASSERT(c)
    TS_ASSERT[[ methods["id"]:call(c) == 99 ]]

    local id = Function.lookup("FunctionTest::paramByConstReference"):call(c)
    TS_ASSERT[[ id == 99 ]]

    local copies = methods["numberOfCopies"]:call()
    local moves = methods["numberOfMoves"]:call()

    TS_ASSERT[[ copies == 0 ]] --estranho
    TS_ASSERT[[ moves == 0 ]]

    return true
end

