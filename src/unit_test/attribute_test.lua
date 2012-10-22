package.cpath = package.cpath..";../lua_module/?.so"
require "libluacppreflect"
require "cxxtest"

function testAttribute()

    local TestClass = Class.lookup("AttributeTest::Test")

    TS_ASSERT (TestClass)
    TS_ASSERT [[TestClass:simpleName() == "Test"]]
    TS_ASSERT [[TestClass:fullyQualifiedName() == "AttributeTest::Test"]]

    local testConstructors = TestClass:constructors()

    TS_ASSERT [[ #testConstructors > 0 ]]

    local cons = testConstructors[1]

    TS_ASSERT (cons)


    local attrList = TestClass:attributes()
    TS_ASSERT [[ #attrList >= 4 ]]

    local attrs = {}

    for _,v in ipairs (TestClass:attributes()) do
        attrs[v:name()] = v
    end

    local v1 = cons:call()

    TS_ASSERT (v1)

    local attr1 = attrs["attr1"]

    TS_ASSERT(attr1)
	TS_ASSERT[[ attr1:getClass() == TestClass ]]
    TS_ASSERT[[ attr1:typeSpelling() == 'int' ]]
    TS_ASSERT(not attr1:isConst())
    TS_ASSERT(not attr1:isStatic())

    TS_ASSERT[[ attr1:get(v1) == 101 ]]

    TS_ASSERT[[ attr1:get(v1) ~= 201 ]]
    attr1:set(v1, 201)
    TS_ASSERT[[ attr1:get(v1) == 201 ]]


    if pcall( function() attr1:get() end ) then
       TS_FAIL("expected exception when getting non-static attribute without object")
    end

    if pcall( function() attr1:set(201) end ) then
       TS_FAIL("expected exception when setting non-static attribute without object")
    end

    local attr2 = attrs["attr2"]

    TS_ASSERT(attr2)
    TS_ASSERT[[ attr2:typeSpelling() == 'const int' ]]

    TS_ASSERT(attr2:isConst())
    TS_ASSERT(not attr2:isStatic())

    TS_ASSERT[[ attr2:get(v1) == 102 ]]

    if pcall( function() attr2:set(v1, 201) end ) then
       TS_FAIL("expected exception when setting const attribute")
    end

    local attr3 = attrs["attr3"]
    TS_ASSERT(attr3)
    TS_ASSERT[[ attr3:typeSpelling() == 'int' ]]
    TS_ASSERT(not attr3:isConst())
    TS_ASSERT(attr3:isStatic())

    TS_ASSERT[[ attr3:get() == 103 ]]
    attr3:set(303)
    TS_ASSERT[[ attr3:get() == 303 ]]
    -- reset
    attr3:set(103)
    TS_ASSERT[[ attr3:get() == 103 ]]

    local attr4 = attrs["attr4"]
    TS_ASSERT(attr4)
    TS_ASSERT[[ attr4:typeSpelling() == 'const int' ]]
    TS_ASSERT(attr4:isConst())
    TS_ASSERT(attr4:isStatic())

    TS_ASSERT[[ attr4:get() == 104 ]]

    if pcall( function() attr4:set(204) end ) then
       TS_FAIL("expected exception when setting const static attribute")
    end


    local searched = TestClass:findAttribute(function(a) return a:name() == "attr1" end)
    TS_ASSERT(searched)
    TS_ASSERT[[searched:name() == "attr1"]]
	TS_ASSERT[[ searched == attr1 ]]
	TS_ASSERT[[ searched ~= attr2 ]]

    local constAttributes = TestClass:findAllAttributes(function(a) return a:isConst() end)
    TS_ASSERT(constAttributes)
    TS_ASSERT[[#constAttributes == 2]]

    return true
end
