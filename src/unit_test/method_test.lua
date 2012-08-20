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

    local v1 = constr:call();

    TS_ASSERT(v1)

    TS_ASSERT[[ method1:call(v1, 3) == 6 ]]


    local method5 = methods["method5(int)"]
    TS_ASSERT(method5)

    TS_ASSERT[[ method5:call(3) == 18 ]]

    return true
end
