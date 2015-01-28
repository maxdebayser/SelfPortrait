/*
** SelfPortrait API
** See Copyright Notice in reflection.h
*/
#include "proxy_test.h"

#include "reflection_impl.h"

#include "test_utils.h"

namespace ProxyTest {

	class Test {
	public:
		virtual ~Test() {}

		virtual int method1(int, int) = 0;

	};


	class Client {
	private:
		Test* m_test = nullptr;

	public:
		void setTest(Test* t) { m_test = t; }

		int doSomething(int i) {
			return m_test->method1(i, 2);
		}

	};

}


REFL_BEGIN_STUB(ProxyTest::Test, TestStub)
REFL_STUB_METHOD(ProxyTest::Test, method1, int, int, int)
REFL_END_STUB

REFL_BEGIN_CLASS(ProxyTest::Test)
	REFL_DEFAULT_CONSTRUCTOR()
	REFL_METHOD(method1, int, int, int)
	REFL_STUB(TestStub)
REFL_END_CLASS

REFL_BEGIN_CLASS(ProxyTest::Client)
	REFL_DEFAULT_CONSTRUCTOR()
	REFL_METHOD(setTest, void, ProxyTest::Test*)
	REFL_METHOD(doSomething, int, int)
REFL_END_CLASS


void ProxyTestSuite::testProxy()
{
	Class test = Class::lookup("ProxyTest::Test");

	TS_ASSERT(test.isValid());
	TS_ASSERT(test.isInterface());

	Method m = test.findMethod([](const Method& m){ return m.name() == "method1"; });

	VariantValue handle;
	{ // inner scope
		Proxy proxy(test);
		Proxy::IFaceList ifaces = proxy.interfaces();

		TS_ASSERT_EQUALS(ifaces.size(), 1)
		TS_ASSERT_EQUALS(ifaces.front(), test)

		proxy.addImplementation(m, [](const std::vector<VariantValue>& args) -> VariantValue {
			TS_ASSERT(args.size() == 2);
			TS_ASSERT(args[0].isA<int>());
			TS_ASSERT(args[1].isA<int>());
			int first = args[0].value<int>();
			int second = args[1].value<int>();
			return VariantValue(first*second);
		});

		TS_ASSERT(proxy.hasImplementation(m));

		handle = proxy.reference(test);
	}

	TS_ASSERT(handle.isValid());

	TS_ASSERT(handle.isA<ProxyTest::Test>());
	TS_ASSERT(handle.isA<ProxyTest::Test&>());

	auto& stub = handle.convertTo<ProxyTest::Test&>();

	int result = stub.method1(3,5);
	TS_ASSERT_EQUALS(15, result);
}


void ProxyTestSuite::testClient()
{
	Class test = Class::lookup("ProxyTest::Test");

	TS_ASSERT(test.isValid());
	TS_ASSERT(test.isInterface());

	Method m = test.findMethod([](const Method& m){ return m.name() == "method1"; });

	Proxy proxy(test);

	Proxy::IFaceList ifaces = proxy.interfaces();

	TS_ASSERT_EQUALS(ifaces.size(), 1)
	TS_ASSERT_EQUALS(ifaces.front(), test)

	proxy.addImplementation(m, [](const std::vector<VariantValue>& args) -> VariantValue {
		TS_ASSERT(args.size() == 2);
		TS_ASSERT(args[0].isA<int>());
		TS_ASSERT(args[1].isA<int>());
		int first = args[0].value<int>();
		double second = args[1].value<int>();
		return VariantValue(first*second);
	});

	TS_ASSERT(proxy.hasImplementation(m));

	VariantValue handle = proxy.reference(test);

	TS_ASSERT(handle.isValid());

	TS_ASSERT(handle.isA<ProxyTest::Test>());
	TS_ASSERT(handle.isA<ProxyTest::Test&>());

	Class client = Class::lookup("ProxyTest::Client");

	TS_ASSERT(client.isValid());

	Constructor cons = client.findConstructor([](const Constructor& c){ return c.isDefaultConstructor(); });
	Method setter = client.findMethod([](const Method& m) { return m.name() == "setTest"; });
	Method doSomething = client.findMethod([](const Method& m) { return m.name() == "doSomething"; });

	TS_ASSERT(cons.isValid());
	TS_ASSERT(setter.isValid());
	TS_ASSERT(doSomething.isValid());

	VariantValue cinst = cons.call();

	TS_ASSERT(cinst.isValid());
	TS_ASSERT(cinst.isA<ProxyTest::Client>());


	std::vector<VariantValue> vargs;
	vargs.emplace_back(std::move(handle));

	setter.callArgArray(cinst, vargs);
	//setter.call(cinst, handle); // TODO: pass variant directly

	VariantValue result = doSomething.call(cinst, 3);
	TS_ASSERT(result.isA<int>());
	TS_ASSERT(result.value<int>() == 6);
}


void ProxyTestSuite::testLuaAPI()
{
	LuaUtils::LuaStateHolder L;

    if (luaL_loadfile(L, strconv::fmt_str("%1/proxy_test.lua", srcpath()).c_str()) || lua_pcall(L,0,0,0)) {
		luaL_error(L, "cannot run config file: %s\n", lua_tostring(L, -1));
	}
	LuaUtils::callFunc<bool>(L, "testProxy");
}
