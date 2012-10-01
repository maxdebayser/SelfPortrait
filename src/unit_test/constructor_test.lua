package.cpath = package.cpath..";../lua_module/?.so"
require "libluacppreflect"
require "cxxtest"

function testConstructor()

    local TestClass = Class.lookup("ConstructorTest::Test")

    TS_ASSERT (TestClass)
    TS_ASSERT [[TestClass:simpleName() == "Test"]]
    TS_ASSERT [[TestClass:fullyQualifiedName() == "ConstructorTest::Test"]]

    local constructors = TestClass:constructors()

    TS_ASSERT (constructors)
    TS_ASSERT[[ #constructors >= 4 ]]

    local cons = {}
    for k,v in ipairs(constructors) do
        cons[table.concat(v:argumentSpellings(), ",")] = v
    end

    local attrs = {}
    for k,v in ipairs (TestClass:attributes()) do
        attrs[v:name()] = v
    end

    local default = cons[""]
    TS_ASSERT (default)
    TS_ASSERT[[ default:numberOfArguments() == 0 ]]

    local attr1 = attrs["attr1"]
    TS_ASSERT(attr1)
    local attr2 = attrs["attr2"]
    TS_ASSERT(attr2)

    local v1 = default:call()

    TS_ASSERT[[ attr1:get(v1) == 101 ]]
    TS_ASSERT[[ attr2:get(v1) == 102 ]]

    local cons1Arg = cons["int"]
    TS_ASSERT(cons1Arg)
    TS_ASSERT[[ cons1Arg:numberOfArguments() == 1 ]]

    local v2 = cons1Arg:call(33)

    TS_ASSERT(v2)
    TS_ASSERT[[ attr1:get(v2) == 33]]
    TS_ASSERT[[ attr2:get(v2) == 103 ]]

    local cons2Arg = cons["int,int"]
    TS_ASSERT(cons2Arg)
    TS_ASSERT[[ cons2Arg:numberOfArguments() == 2 ]]

    local v3 = cons2Arg:call(44, 88)

    TS_ASSERT(v3)
    TS_ASSERT[[ attr1:get(v3) == 44 ]]
    TS_ASSERT[[ attr2:get(v3) == 88 ]]

    local copyCons = cons["const ConstructorTest::Test &"]
    TS_ASSERT(copyCons)
    TS_ASSERT[[ copyCons:numberOfArguments() == 1 ]]

    local v4 = copyCons:call(v3)
    TS_ASSERT(v4)
    TS_ASSERT[[ attr1:get(v4) == 44 ]]
    TS_ASSERT[[ attr2:get(v4) == 88 ]]

    local searched = TestClass:findConstructor(function(c) return c:numberOfArguments() == 1 end)
    TS_ASSERT(searched)

    return true
end
