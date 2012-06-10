package.cpath = package.cpath..";../lua_module/?.so"
require "libluacppreflect"
require "cxxtest"

function testClass()
    local Test1Class = Class.lookup("Test::Test1")

    TS_ASSERT [[Test1Class]]
    TS_ASSERT [[Test1Class:simpleName() == "Test1"]]
    TS_ASSERT [[Test1Class:fullyQualifiedName() == "Test::Test1"]]

    local Superclasses = Test1Class:superclasses()
	if (#Superclasses ~= 2) then return false end

    local sups = {}
	for _, v in ipairs(Superclasses) do sups[v:simpleName()] = v end

    TS_ASSERT [=[sups["TestBase1"]]=]
    TS_ASSERT [=[sups["TestBase2"]]=]

    local Base1 = sups["TestBase1"]
    local Base1Cons = Base1:constructors()

    TS_ASSERT [[Base1Cons and #Base1Cons == 1]]

    local Base2Cons = sups["TestBase2"]:constructors()

    TS_ASSERT [[Base2Cons and #Base2Cons == 1]]

    local Base2Constructor = Base2Cons[1]

    return false
end
