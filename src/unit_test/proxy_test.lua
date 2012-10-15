package.cpath = package.cpath..";../lua_module/?.so"
require "libluacppreflect"
require "cxxtest"

function testProxy()

    local test = Class.lookup("ProxyTest::Test")

    TS_ASSERT(test)
    TS_ASSERT(test:isInterface());

	local m1 = test:findMethod(function(m) return m:name() == "method1" end)

	TS_ASSERT(m1)

	local proxy = Proxy.create(test)

	TS_ASSERT(proxy)

	local ifaces = proxy:interfaces()

	TS_ASSERT[[ #ifaces == 1]]
	-- TODO: equality in lua

	proxy:addImplementation(m1, function(varg1, varg2)
		TS_ASSERT(varg1)
		TS_ASSERT(varg1:isIntegral())
		TS_ASSERT(varg2)
		TS_ASSERT(varg2:isIntegral())
		return varg1:tonumber()*varg2:tonumber()
	end)

	TS_ASSERT(proxy:hasImplementation(m1))

	local handle = proxy:reference(test)

	TS_ASSERT(handle)

	TS_ASSERT[[ m1:call(handle, 3, 5) == 15 ]]

	local client = Class.lookup("ProxyTest::Client")

	TS_ASSERT(client)

	local cons = client:findConstructor(function(c) return c:isDefaultConstructor() end)
	local setter = client:findMethod(function(m) return m:name() == "setTest" end)
	local doSomething = client:findMethod(function(m) return m:name() == "doSomething" end)

	TS_ASSERT(cons)
	TS_ASSERT(setter)
	TS_ASSERT(doSomething)

	local cinst = cons:call()
	TS_ASSERT(cinst)

	setter:call(cinst, handle)

	TS_ASSERT[[ doSomething:call(cinst, 3) == 6 ]]

    return true
end
