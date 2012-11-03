require "libluaselfportrait"
require "cxxtest"

function testClass()
    local Test1Class = Class.lookup("ClassTest::Test1")

    TS_ASSERT (Test1Class)
    TS_ASSERT [[Test1Class:simpleName() == "Test1"]]
    TS_ASSERT [[Test1Class:fullyQualifiedName() == "ClassTest::Test1"]]

	TS_ASSERT [[ Test1Class == Test1Class ]]

	local second = Class.lookup("ClassTest::Test1")
	TS_ASSERT [[ Test1Class == second ]]
	TS_ASSERT [[ Test1Class ~= 1 ]]

    local Superclasses = Test1Class:superclasses()
	if (#Superclasses ~= 2) then return false end

    local sups = {}
	for _, v in ipairs(Superclasses) do sups[v:simpleName()] = v end

    TS_ASSERT [=[sups["TestBase1"]]=]
    TS_ASSERT [=[sups["TestBase2"]]=]

    local Base1 = sups["TestBase1"]
	TS_ASSERT [[ Test1Class ~= Base1 ]]
    local Base1Cons = Base1:constructors()

    TS_ASSERT [[Base1Cons and #Base1Cons == 1]]

    local Base2 = sups["TestBase2"]
    local Base2Cons = Base2:constructors()

    TS_ASSERT [[Base2Cons and #Base2Cons == 1]]

    local base2Constructor = Base2Cons[1]

    TS_ASSERT [[base2Constructor:numberOfArguments() == 0]]

    local b2Inst = base2Constructor:call()

    TS_ASSERT [[b2Inst:isValid()]]
    TS_ASSERT [[#Base2:attributes() == 0]]

    local base2Methods = Base2:methods()

    TS_ASSERT [[#base2Methods == 1]]

    local base2Method1 = base2Methods[1]

    TS_ASSERT [[base2Method1:name() == "base2Method1"]]
    TS_ASSERT [[base2Method1:isConst()]]
    TS_ASSERT [[not base2Method1:isVolatile()]]
    TS_ASSERT [[not base2Method1:isStatic()]]
    TS_ASSERT [[base2Method1:numberOfArguments() == 0]]
    TS_ASSERT [[base2Method1:call(b2Inst) == 6]]

    local testConstructors = Test1Class:constructors()
    local testAttributes   = Test1Class:attributes()

    TS_ASSERT [[#testAttributes == 2]]
    local attr = testAttributes[1]

    TS_ASSERT [[attr:typeSpelling() == "int"]]

    local defaultConstr
    local intConstr

    for _, v in ipairs(testConstructors) do
        if v:numberOfArguments() == 0 then defaultConstr = v end
        if v:numberOfArguments() == 1 and v:argumentSpellings()[1] == "int" then intConstr = v end
    end

    TS_ASSERT [[defaultConstr:isDefaultConstructor()]]
    TS_ASSERT [[defaultConstr:numberOfArguments() == 0]]
    TS_ASSERT [[intConstr:numberOfArguments() == 1]]

    local testInst1 = defaultConstr:call()
    TS_ASSERT [[testInst1:isValid()]]
    TS_ASSERT [[attr:get(testInst1) == 3]]

    local testInst2 = intConstr:call(77);

    TS_ASSERT [[testInst2:isValid()]]
    TS_ASSERT [[attr:get(testInst2) == 77]]

    local methods = Test1Class:methods();

    TS_ASSERT [[#methods == 7]]

    local base1Method1
    local method1
    local method2
    local staticMethod

    for _, m in ipairs(methods) do
        if m:name() == "base1Method1" then base1Method1 = m end
        if m:name() == "base2Method1" then base2Method1 = m end
        if m:name() == "method1"      then method1      = m end
        if m:name() == "method2"      then method2      = m end
        if m:name() == "staticMethod" then staticMethod = m end
    end

    TS_ASSERT [[base1Method1:name() == "base1Method1"]]
    TS_ASSERT [[not base1Method1:isConst()]]
    TS_ASSERT [[not base1Method1:isStatic()]]
    TS_ASSERT [[not base1Method1:isVolatile()]]
    TS_ASSERT [[base1Method1:returnSpelling() == "int"]]
    TS_ASSERT [[base1Method1:numberOfArguments() == 0]]
    TS_ASSERT [[base1Method1:call(testInst2) == 5]]

    TS_ASSERT [[base2Method1:name() == "base2Method1"]]
    TS_ASSERT [[base2Method1:isConst()]]
    TS_ASSERT [[not base2Method1:isStatic()]]
    TS_ASSERT [[not base2Method1:isVolatile()]]
    TS_ASSERT [[base2Method1:returnSpelling() == "int"]]
    TS_ASSERT [[base2Method1:numberOfArguments() == 0]]
    TS_ASSERT [[base2Method1:call(testInst2) == 6]]

    TS_ASSERT [[method2:name() == "method2"]]
    TS_ASSERT [[not method2:isConst()]]
    TS_ASSERT [[not method2:isStatic()]]
    TS_ASSERT [[not method2:isVolatile()]]
    TS_ASSERT [[method2:returnSpelling() == "double"]]
    TS_ASSERT [[method2:numberOfArguments() == 1]]
    TS_ASSERT [[method2:argumentSpellings()[1] == "double"]]
    TS_ASSERT [[method2:call(testInst2, 2) == 6.28]]

    TS_ASSERT [[staticMethod:name() == "staticMethod"]]
    TS_ASSERT [[not staticMethod:isConst()]]
    TS_ASSERT [[staticMethod:isStatic()]]
    TS_ASSERT [[not staticMethod:isVolatile()]]
    TS_ASSERT [[staticMethod:returnSpelling() == "double"]]
    TS_ASSERT [[staticMethod:numberOfArguments() == 0]]
    TS_ASSERT [[staticMethod:call() == 3.14]]

    local searched = Test1Class:findSuperClass(function(c) return c:simpleName() == "TestBase1" end)
    TS_ASSERT(searched)
    TS_ASSERT[[searched:simpleName() == "TestBase1"]]

    return false
end
