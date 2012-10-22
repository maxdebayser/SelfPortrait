package.cpath = package.cpath..";../lua_module/?.so"
require "libluacppreflect"
require "cxxtest"

function testMethod()

    local TestClass = Class.lookup("MethodTest::Test1")
    TS_ASSERT(TestClass)


    local constr = TestClass:constructors()[1]

    TS_ASSERT[[ constr:numberOfArguments() == 0 ]]

    local methods = {}
    for _, v in ipairs(TestClass:methods()) do
        methods[v:name()..'('..table.concat(v:argumentSpellings(), ',')..')'] = v
    end

    local method1 = methods["method1(int)"]
    TS_ASSERT(method1)
    TS_ASSERT [[method1:fullName() == "int MethodTest::Test1::method1(int)"]]

    local v1 = constr:call();

    TS_ASSERT(v1)

    TS_ASSERT[[ method1:call(v1, 3) == 6 ]]


    local method5 = methods["method5(int)"]
    TS_ASSERT(method5)

    TS_ASSERT[[ method5:call(3) == 18 ]]


    local m1 = TestClass:findMethod(function(m) return m:name() == "method1" end)
    local m2 = TestClass:findMethod(function(m) return m:name() == "method2" end)
    local m3 = TestClass:findMethod(function(m) return m:name() == "method3" end)
    local m41 = TestClass:findMethod(function(m) return m:name() == "method4" and m:numberOfArguments() == 1 end)
    local m42 = TestClass:findMethod(function(m) return m:name() == "method4" and m:numberOfArguments() == 2 end);
    local m5 = TestClass:findMethod(function(m) return m:name() == "method5" end)
    local m99 = TestClass:findMethod(function(m) return m:name() == "method99" end)

    TS_ASSERT(m1)
    TS_ASSERT(m2)
    TS_ASSERT(m3)
    TS_ASSERT(m41)
    TS_ASSERT(m42)
    TS_ASSERT(m5)
    TS_ASSERT(not m99)

	TS_ASSERT[[ m5 == method5 ]]
	TS_ASSERT[[ m3 ~= method5 ]]
	TS_ASSERT[[ m5 ~= m41 ]]

	TS_ASSERT[[m1:getClass() == TestClass]]
	TS_ASSERT[[m2:getClass() == TestClass]]
	TS_ASSERT[[m3:getClass() == TestClass]]
	TS_ASSERT[[m41:getClass() == TestClass]]
	TS_ASSERT[[m42:getClass() == TestClass]]
	TS_ASSERT[[m5:getClass() == TestClass]]

    TS_ASSERT[[m1:fullName() == "int MethodTest::Test1::method1(int)"]]
    TS_ASSERT[[m2:fullName() == "int MethodTest::Test1::method2(int) const"]]
    TS_ASSERT[[m3:fullName() == "int MethodTest::Test1::method3(int) volatile"]]
    TS_ASSERT[[m41:fullName() == "int MethodTest::Test1::method4(int) const volatile"]]
    TS_ASSERT[[m42:fullName() == "int MethodTest::Test1::method4(int, int) const volatile"]]
    TS_ASSERT[[m5:fullName() == "static int MethodTest::Test1::method5(int)"]]

    return true
end
