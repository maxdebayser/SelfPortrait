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


    class TestVoid {
    public:
        virtual ~TestVoid() {}

        virtual void method1(int, int) = 0;

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


    class CopyCount {
    public:
        CopyCount(int id) : m_id(id), m_moved(false) {}
        CopyCount() : CopyCount(0) {}

        CopyCount(const CopyCount& rhs) : CopyCount(rhs.m_id) {
            ++s_numCopies;
        }

        CopyCount(CopyCount&& rhs) : CopyCount(rhs.m_id) {
            ++s_numMoves;
            rhs.m_moved = true;
        }

        CopyCount& operator=(const CopyCount& rhs) {
            m_id = rhs.m_id;
            ++s_numCopies;
            return *this;
        }

        CopyCount& operator=(CopyCount&& rhs) {
            m_id = rhs.m_id;
            ++s_numMoves;
            rhs.m_moved = true;
            return *this;
        }

        int id() const { return m_id; }

        void changeId(int newId) {
            m_id = newId;
        }

        static void resetCopyCount() {
            s_numCopies = 0;
        }
        static int numberOfCopies() {
            return s_numCopies;
        }
        static void resetMoveCount() {
            s_numMoves = 0;
        }
        static int numberOfMoves() {
            return s_numMoves;
        }
        static void resetAll() {
            resetCopyCount();
            resetMoveCount();
        }

        bool hasBeenMoved() {
            return m_moved;
        }

    private:
        static int s_numCopies;
        static int s_numMoves;
        int m_id;
        bool m_moved;
    };

    int CopyCount::s_numCopies = 0;
    int CopyCount::s_numMoves = 0;


    struct TestParamPassing {

        virtual CopyCount returnObjectByValue() = 0;

        virtual CopyCount& returnObjectByReference() = 0;

        virtual const CopyCount& returnObjectByConstReference() = 0;

        virtual int paramByValue(CopyCount c) = 0;

        virtual int paramByReference(CopyCount& c) = 0;

        virtual int paramByConstReference(const CopyCount& c) = 0;

        virtual ~TestParamPassing() {}
    };

    struct TestParamPassingImpl: public TestParamPassing {

        CopyCount returnObjectByValue() override {
            return CopyCount(33);
        }

        CopyCount& returnObjectByReference() override {
            static CopyCount instance(777);
            return instance;
        }

        const CopyCount& returnObjectByConstReference() override {
            static CopyCount instance(888);
            return instance;
        }

        int paramByValue(CopyCount c) override {
            return c.id();
        }

        int paramByReference(CopyCount& c) override {
            c.changeId(c.id()+1);
            return c.id();
        }

        int paramByConstReference(const CopyCount& c) override {
            return c.id();
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

REFL_BEGIN_STUB(ProxyTest::TestVoid, TestVoidStub)
REFL_STUB_METHOD(ProxyTest::TestVoid, method1, void, int, int)
REFL_END_STUB

REFL_BEGIN_CLASS(ProxyTest::TestVoid)
    REFL_DEFAULT_CONSTRUCTOR()
    REFL_METHOD(method1, void, int, int)
    REFL_STUB(TestVoidStub)
REFL_END_CLASS

REFL_BEGIN_CLASS(ProxyTest::Client)
	REFL_DEFAULT_CONSTRUCTOR()
	REFL_METHOD(setTest, void, ProxyTest::Test*)
	REFL_METHOD(doSomething, int, int)
REFL_END_CLASS

REFL_BEGIN_CLASS(ProxyTest::CopyCount)
    REFL_CONSTRUCTOR(int)
    REFL_CONST_METHOD(id, int)
    REFL_METHOD(changeId, void, int)
    REFL_METHOD(hasBeenMoved, bool)
    REFL_STATIC_METHOD(resetCopyCount, void)
    REFL_STATIC_METHOD(numberOfCopies, int)
    REFL_STATIC_METHOD(resetMoveCount, void)
    REFL_STATIC_METHOD(numberOfMoves, int)
    REFL_STATIC_METHOD(resetAll, void)
REFL_END_CLASS


REFL_BEGIN_STUB(ProxyTest::TestParamPassing, TestParamPassingStub)
REFL_STUB_METHOD(ProxyTest::TestParamPassing, returnObjectByValue, ProxyTest::CopyCount)
REFL_STUB_METHOD(ProxyTest::TestParamPassing, returnObjectByReference, ProxyTest::CopyCount&)
REFL_STUB_METHOD(ProxyTest::TestParamPassing, returnObjectByConstReference, const ProxyTest::CopyCount&)
REFL_STUB_METHOD(ProxyTest::TestParamPassing, paramByValue, int, ProxyTest::CopyCount)
REFL_STUB_METHOD(ProxyTest::TestParamPassing, paramByReference, int, ProxyTest::CopyCount&)
REFL_STUB_METHOD(ProxyTest::TestParamPassing, paramByConstReference, int, const ProxyTest::CopyCount&)
REFL_END_STUB

REFL_BEGIN_CLASS(ProxyTest::TestParamPassing)
REFL_STUB(TestParamPassingStub)
REFL_METHOD(returnObjectByValue, ProxyTest::CopyCount)
REFL_METHOD(returnObjectByReference, ProxyTest::CopyCount&)
REFL_METHOD(returnObjectByConstReference, const ProxyTest::CopyCount&)
REFL_METHOD(paramByValue, int, ProxyTest::CopyCount)
REFL_METHOD(paramByReference, int, ProxyTest::CopyCount&)
REFL_METHOD(paramByConstReference, int, const ProxyTest::CopyCount&)
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

        proxy.addImplementation(m, [](const ArgArray& args) -> VariantValue {
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


void ProxyTestSuite::testVoidProxy()
{
    Class test = Class::lookup("ProxyTest::TestVoid");

    TS_ASSERT(test.isValid());
    TS_ASSERT(test.isInterface());

    Method m = test.findMethod([](const Method& m){ return m.name() == "method1"; });

    int result = 987;

    VariantValue handle;
    { // inner scope
        Proxy proxy(test);
        Proxy::IFaceList ifaces = proxy.interfaces();

        TS_ASSERT_EQUALS(ifaces.size(), 1)
        TS_ASSERT_EQUALS(ifaces.front(), test)

        proxy.addImplementation(m, [&](const ArgArray& args) -> VariantValue {
            TS_ASSERT(args.size() == 2);
            TS_ASSERT(args[0].isA<int>());
            TS_ASSERT(args[1].isA<int>());
            int first = args[0].value<int>();
            int second = args[1].value<int>();
            result = first*second;
            return VariantValue();
        });

        TS_ASSERT(proxy.hasImplementation(m));

        handle = proxy.reference(test);
    }

    TS_ASSERT(handle.isValid());

    TS_ASSERT(handle.isA<ProxyTest::TestVoid>());
    TS_ASSERT(handle.isA<ProxyTest::TestVoid&>());

    auto& stub = handle.convertTo<ProxyTest::TestVoid&>();

    stub.method1(3,6);
    //TS_ASSERT_EQUALS(18, result);
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

    proxy.addImplementation(m, [](const ArgArray& args) -> VariantValue {
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


    ArgArray vargs;
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
    LuaUtils::addTestFunctionsAndPaths(&*L);

    const int errIndex = LuaUtils::pushTraceBack(L);
    if (luaL_loadfile(L, strconv::fmt_str("%1/proxy_test.lua", srcpath()).c_str()) || lua_pcall(L,0,0,errIndex)) {
		luaL_error(L, "cannot run config file: %s\n", lua_tostring(L, -1));
	}
    LuaUtils::removeTraceBack(L, errIndex);
	LuaUtils::callFunc<bool>(L, "testProxy");
}

void ProxyTestSuite::testReturnByValue()
{
    using namespace ProxyTest;
    Class test = Class::lookup("ProxyTest::TestParamPassing");

    TS_ASSERT(test.isValid());
    TS_ASSERT(test.isInterface());

    Method m = test.findMethod([](const Method& m){ return m.name() == "returnObjectByValue"; });

    VariantValue handle;
    { // inner scope
        Proxy proxy(test);
        Proxy::IFaceList ifaces = proxy.interfaces();

        TS_ASSERT_EQUALS(ifaces.size(), 1)
        TS_ASSERT_EQUALS(ifaces.front(), test)

        proxy.addImplementation(m, [](const ArgArray& args) -> VariantValue {
            TS_ASSERT(args.size() == 0);
            VariantValue v;
            v.construct<CopyCount>(33);
            return v;
            //return CopyCount(33); // This produce an extra copy
        });

        TS_ASSERT(proxy.hasImplementation(m));

        handle = proxy.reference(test);
    }

    TS_ASSERT(handle.isValid());

    auto& stub = handle.convertTo<ProxyTest::TestParamPassing&>();

    ProxyTest::TestParamPassingImpl inst;

    CopyCount::resetAll();

    CopyCount c = inst.returnObjectByValue();

    TS_ASSERT_EQUALS(c.id(), 33);

    int directCallCopies = CopyCount::numberOfCopies();
    CopyCount::resetAll();

    CopyCount ref = stub.returnObjectByValue();

    int reflectionCopies = CopyCount::numberOfCopies();

    TS_ASSERT_EQUALS(ref.id(),33);

    TS_ASSERT_EQUALS(directCallCopies, reflectionCopies);

}


void ProxyTestSuite::testReturnByReference()
{

    using namespace ProxyTest;
    Class test = Class::lookup("ProxyTest::TestParamPassing");

    TS_ASSERT(test.isValid());
    TS_ASSERT(test.isInterface());

    Method m = test.findMethod([](const Method& m){ return m.name() == "returnObjectByReference"; });

    VariantValue handle;
    { // inner scope
        Proxy proxy(test);
        Proxy::IFaceList ifaces = proxy.interfaces();

        TS_ASSERT_EQUALS(ifaces.size(), 1)
        TS_ASSERT_EQUALS(ifaces.front(), test)

        proxy.addImplementation(m, [](const ArgArray& args) -> VariantValue {
            TS_ASSERT(args.size() == 0);
            static CopyCount instance(777);
            VariantValue v;
            v.construct<CopyCount&>(instance);
            return v;
            //return CopyCount(33); // This produce an extra copy
        });

        TS_ASSERT(proxy.hasImplementation(m));

        handle = proxy.reference(test);
    }

    TS_ASSERT(handle.isValid());

    auto& stub = handle.convertTo<ProxyTest::TestParamPassing&>();

    ProxyTest::TestParamPassingImpl inst;

    CopyCount::resetAll();
    CopyCount& ref = stub.returnObjectByReference();

    TS_ASSERT_EQUALS(CopyCount::numberOfCopies(), 0);
    TS_ASSERT(!inst.returnObjectByReference().hasBeenMoved())
}


void ProxyTestSuite::testReturnByConstReference()
{

    using namespace ProxyTest;
    Class test = Class::lookup("ProxyTest::TestParamPassing");

    TS_ASSERT(test.isValid());
    TS_ASSERT(test.isInterface());

    Method m = test.findMethod([](const Method& m){ return m.name() == "returnObjectByConstReference"; });

    VariantValue handle;
    { // inner scope
        Proxy proxy(test);
        Proxy::IFaceList ifaces = proxy.interfaces();

        TS_ASSERT_EQUALS(ifaces.size(), 1)
        TS_ASSERT_EQUALS(ifaces.front(), test)

        proxy.addImplementation(m, [](const ArgArray& args) -> VariantValue {
            TS_ASSERT(args.size() == 0);
            static CopyCount instance(888);
            VariantValue v;
            v.construct<CopyCount&>(instance);
            return v;
            //return CopyCount(33); // This produce an extra copy
        });

        TS_ASSERT(proxy.hasImplementation(m));

        handle = proxy.reference(test);
    }

    TS_ASSERT(handle.isValid());

    auto& stub = handle.convertTo<ProxyTest::TestParamPassing&>();

    ProxyTest::TestParamPassingImpl inst;

    CopyCount::resetAll();


    const CopyCount& cref = stub.returnObjectByConstReference();

    TS_ASSERT_EQUALS(CopyCount::numberOfCopies(), 0);

    TS_ASSERT_EQUALS(cref.id(), 888);
}


void ProxyTestSuite::testParametersByValue()
{
    using namespace ProxyTest;
    Class test = Class::lookup("ProxyTest::TestParamPassing");

    TS_ASSERT(test.isValid());
    TS_ASSERT(test.isInterface());

    Method m = test.findMethod([](const Method& m){ return m.name() == "paramByValue"; });

    VariantValue handle;
    { // inner scope
        Proxy proxy(test);
        Proxy::IFaceList ifaces = proxy.interfaces();

        TS_ASSERT_EQUALS(ifaces.size(), 1)
        TS_ASSERT_EQUALS(ifaces.front(), test)

        proxy.addImplementation(m, [](const ArgArray& args) -> VariantValue {
            TS_ASSERT(args.size() == 1);
            return args[0].convertToThrow<CopyCount&>().id();
        });

        TS_ASSERT(proxy.hasImplementation(m));

        handle = proxy.reference(test);
    }
    TS_ASSERT(handle.isValid());

    auto& stub = handle.convertTo<ProxyTest::TestParamPassing&>();
    ProxyTest::TestParamPassingImpl inst;

    CopyCount::resetAll();

    CopyCount c(77);

    TS_ASSERT_EQUALS(inst.paramByValue(c), 77);

    int directCallCopies = CopyCount::numberOfCopies();
    int directCallMoves = CopyCount::numberOfMoves();
    TS_ASSERT_EQUALS(directCallCopies, 1);
    TS_ASSERT_EQUALS(directCallMoves,  0);

    CopyCount::resetAll();
    int ret = stub.paramByValue(c);
    TS_ASSERT_EQUALS(ret, 77);
    int reflectionCopies = CopyCount::numberOfCopies();
    int reflectionMoves = CopyCount::numberOfMoves();

    TS_ASSERT_EQUALS(reflectionCopies, 1);
    TS_ASSERT_EQUALS(reflectionMoves,  0);
    TS_ASSERT(!c.hasBeenMoved());
}


void ProxyTestSuite::testParametersByReference()
{

    using namespace ProxyTest;
    Class test = Class::lookup("ProxyTest::TestParamPassing");

    TS_ASSERT(test.isValid());
    TS_ASSERT(test.isInterface());

    Method m = test.findMethod([](const Method& m){ return m.name() == "paramByReference"; });

    VariantValue handle;
    { // inner scope
        Proxy proxy(test);
        Proxy::IFaceList ifaces = proxy.interfaces();

        TS_ASSERT_EQUALS(ifaces.size(), 1)
        TS_ASSERT_EQUALS(ifaces.front(), test)

        proxy.addImplementation(m, [](const ArgArray& args) -> VariantValue {
            TS_ASSERT(args.size() == 1);
            CopyCount& c = args[0].convertToThrow<CopyCount&>();
            c.changeId(c.id()+1);
            return c.id();
        });

        TS_ASSERT(proxy.hasImplementation(m));

        handle = proxy.reference(test);
    }
    TS_ASSERT(handle.isValid());

    auto& stub = handle.convertTo<ProxyTest::TestParamPassing&>();
    ProxyTest::TestParamPassingImpl inst;

    CopyCount::resetAll();

    CopyCount c(44);

    TS_ASSERT_EQUALS(inst.paramByReference(c), 45);
    TS_ASSERT_EQUALS(c.id(), 45);
    TS_ASSERT_EQUALS(CopyCount::numberOfCopies(), 0);

    int r = stub.paramByReference(c);

    TS_ASSERT_EQUALS(CopyCount::numberOfCopies(), 0);
    TS_ASSERT_EQUALS(r, 46);
    TS_ASSERT_EQUALS(c.id(), 46);
}

void ProxyTestSuite::testParametersByConstReference()
{
    using namespace ProxyTest;
    Class test = Class::lookup("ProxyTest::TestParamPassing");

    TS_ASSERT(test.isValid());
    TS_ASSERT(test.isInterface());

    Method m = test.findMethod([](const Method& m){ return m.name() == "paramByConstReference"; });

    VariantValue handle;
    { // inner scope
        Proxy proxy(test);
        Proxy::IFaceList ifaces = proxy.interfaces();

        TS_ASSERT_EQUALS(ifaces.size(), 1)
        TS_ASSERT_EQUALS(ifaces.front(), test)

        proxy.addImplementation(m, [](const ArgArray& args) -> VariantValue {
            TS_ASSERT(args.size() == 1);
            const CopyCount& c = args[0].convertToThrow<const CopyCount&>();
            return c.id();
        });

        TS_ASSERT(proxy.hasImplementation(m));

        handle = proxy.reference(test);
    }
    TS_ASSERT(handle.isValid());

    auto& stub = handle.convertTo<ProxyTest::TestParamPassing&>();
    ProxyTest::TestParamPassingImpl inst;

    CopyCount::resetAll();

    CopyCount c(44);

    TS_ASSERT_EQUALS(inst.paramByConstReference(c), 44);
    TS_ASSERT_EQUALS(c.id(), 44);
    TS_ASSERT_EQUALS(CopyCount::numberOfCopies(), 0);

    cout << "checkpoint 1" << endl;
    int r = stub.paramByConstReference(c);
    cout << "checkpoint 2" << endl;

    TS_ASSERT_EQUALS(CopyCount::numberOfCopies(), 0);
    TS_ASSERT_EQUALS(r, 44);
    TS_ASSERT_EQUALS(c.id(), 44);
}
