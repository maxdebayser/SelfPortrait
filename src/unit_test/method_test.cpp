/*
** SelfPortrait API
** See Copyright Notice in reflection.h
*/
#include "method_test.h"
#include "method.h"
#include "reflection.h"
#include "reflection_impl.h"

#include "test_utils.h"
#include "str_utils.h"

#include <lua.hpp>

#include <iostream>
#include <string>
using namespace std;

namespace MethodTest {
	class Test1 {
	public:
		int method1(int arg) { return arg*2; }
		int method2(int arg) const { return arg*3; }
		int method3(int arg) volatile { return arg*4; }
		int method4(int arg) const volatile { return arg*5; }
		int method4(int arg1, int arg2) const volatile { return arg1*5 + arg2; }
		static int method5(int arg) { return arg*6; }
	};

	class Test2 {};


	class Base {
	public:
		virtual ~Base() {}
		virtual int method1() { return 555; }
		int method2() { return 666; }
	};

	class Derived: public Base {
	public:
		virtual ~Derived() {}
		virtual int method1() { return 556; }
		int method2() { return 667; }
	};

    class Logger {
    public:
        Logger(int) {}
        Logger(const Logger&) = delete;
        Logger(Logger&&) = delete;
    };

    class LoggerFactory {
    public:
        static Logger& getLogger(const std::string& name, std::string outputName = "stderr", bool colored = true) { static Logger myLogger(3); return myLogger; }
        static void getLogger2(const std::string& name, std::string outputName = "stderr", bool colored = true) {  }
        static void getLogger3(const std::string& name) { cout << "Name = " << name << endl; }
        static void getLogger4(std::string name) { cout << "Name = " << name << endl;  }
        static void getLogger5(bool name) {  }
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

        CopyCount returnObjectByValue() {
            return CopyCount(33);
        }

        CopyCount& returnObjectByReference() {
            static CopyCount instance(777);
            return instance;
        }

        const CopyCount& returnObjectByConstReference() {
            static CopyCount instance(888);
            return instance;
        }


        int paramByValue(CopyCount c) {
            return c.id();
        }

        int paramByReference(CopyCount& c) {
            c.changeId(c.id()+1);
            return c.id();
        }

        int paramByConstReference(const CopyCount& c) {
            return c.id();
        }
    };
}


REFL_BEGIN_CLASS(MethodTest::Test1)
	REFL_DEFAULT_CONSTRUCTOR()
	REFL_METHOD(method1, int, int)
	REFL_CONST_METHOD(method2, int, int)
	REFL_VOLATILE_METHOD(method3, int, int)
	REFL_CONST_VOLATILE_METHOD(method4, int, int)
	REFL_CONST_VOLATILE_METHOD(method4, int, int, int)
	REFL_STATIC_METHOD(method5, int, int)
REFL_END_CLASS


REFL_BEGIN_CLASS(MethodTest::Test2)
REFL_END_CLASS


REFL_BEGIN_CLASS(MethodTest::Base)
	REFL_DEFAULT_CONSTRUCTOR()
	REFL_METHOD(method1, int)
	REFL_METHOD(method2, int)
REFL_END_CLASS

REFL_BEGIN_CLASS(MethodTest::Derived)
	REFL_SUPER_CLASS(MethodTest::Base)
	REFL_DEFAULT_CONSTRUCTOR()
	REFL_METHOD(method1, int)
	REFL_METHOD(method2, int)
REFL_END_CLASS

REFL_BEGIN_CLASS(MethodTest::LoggerFactory)
REFL_STATIC_METHOD(getLogger, MethodTest::Logger&, const std::basic_string<char>&, std::basic_string<char>, bool)
REFL_STATIC_METHOD(getLogger2, void, const std::basic_string<char>&, std::basic_string<char>, bool)
REFL_STATIC_METHOD(getLogger3, void, const std::basic_string<char>&)
REFL_STATIC_METHOD(getLogger4, void, std::basic_string<char>)
REFL_STATIC_METHOD(getLogger5, void, bool)
REFL_END_CLASS


REFL_BEGIN_CLASS(MethodTest::CopyCount)
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


using namespace MethodTest;

void MethodTestSuite::testNonCVMethod() {

	auto method = make_method<int(Test1::*)(int)>(&method_type<int(Test1::*)(int)>::bindcall<&Test1::method1>, "method1", "int", "int");

	TS_ASSERT_THROWS(method.call(3), std::runtime_error);


	VariantValue v1 = Test1();

	VariantValue r1 = method.call(v1, 3);

	TS_ASSERT(r1.isA<int>());
	TS_ASSERT_EQUALS(r1.value<int>(), 6);

	const VariantValue v2 = Test1();
	TS_ASSERT_THROWS(method.call(v2, 3), std::runtime_error);

	Test1 t; // declare a local variable because
	// 1 - gcc think volatile VariantValue v3(Test()); is a function declaration
	// 2 - gcc uses the copy constructor in volatile VariantValue v3 = Test();
	//     and in this case it complains rightfully that it can't construct
	//     a VariantValue from a volatile Variant temporary


	volatile VariantValue v3(t);
	TS_ASSERT_THROWS(method.call(v3, 3), std::runtime_error);

	const volatile VariantValue v4(t);
	TS_ASSERT_THROWS(method.call(v4, 3), std::runtime_error);


    VariantValue v5(&t);

    VariantValue r2 = method.call(v5, 3);

    TS_ASSERT(r2.isA<int>());
    TS_ASSERT_EQUALS(r2.value<int>(), 6);
}


void MethodTestSuite::testCMethod()
{

	auto method = make_method<int(Test1::*)(int) const>(&method_type<int(Test1::*)(int) const>::bindcall<&Test1::method2>, "method2", "int", "int");

	VariantValue v1 = Test1();
	VariantValue r1 = method.call(v1, 3);

	TS_ASSERT(r1.isA<int>());
	TS_ASSERT_EQUALS(r1.value<int>(), 9);


	const VariantValue v2 = Test1();
	VariantValue r2 = method.call(v2, 3);

	TS_ASSERT(r2.isA<int>());
	TS_ASSERT_EQUALS(r2.value<int>(), 9);

	Test1 t;

	volatile VariantValue v3(t);
	TS_ASSERT_THROWS(method.call(v3, 3), std::runtime_error);

	const volatile VariantValue v4(t);
	TS_ASSERT_THROWS(method.call(v4, 3), std::runtime_error);
}


void MethodTestSuite::testVMethod()
{
	auto method = make_method<int(Test1::*)(int) volatile>(&method_type<int(Test1::*)(int) volatile>::bindcall<&Test1::method3>, "method3", "int", "int");

	VariantValue v1 = Test1();

	VariantValue r = method.call(v1, 3);

	TS_ASSERT(r.isA<int>());
	TS_ASSERT_EQUALS(r.value<int>(), 12);

	const VariantValue v2 = Test1();
	TS_ASSERT_THROWS(method.call(v2, 3), std::runtime_error);

	Test1 t;

	volatile VariantValue v3(t);

	VariantValue r3 = method.call(v3, 3);

	TS_ASSERT(r3.isA<int>());
	TS_ASSERT_EQUALS(r3.value<int>(), 12);

	const volatile VariantValue v4(t);
	TS_ASSERT_THROWS(method.call(v4, 3), std::runtime_error);
}


void MethodTestSuite::testCVMethod()
{
	auto method = make_method<int(Test1::*)(int) const volatile>(&method_type<int(Test1::*)(int) const volatile>::bindcall<&Test1::method4>, "method4", "int", "int");

	VariantValue v1 = Test1();

	VariantValue r = method.call(v1, 3);

	TS_ASSERT(r.isA<int>());
	TS_ASSERT_EQUALS(r.value<int>(), 15);

	const VariantValue v2 = Test1();

	VariantValue r2 = method.call(v2, 3);

	TS_ASSERT(r2.isA<int>());
	TS_ASSERT_EQUALS(r2.value<int>(), 15);

	Test1 t;

	volatile VariantValue v3(t);

	VariantValue r3 = method.call(v3, 3);

	TS_ASSERT(r3.isA<int>());
	TS_ASSERT_EQUALS(r3.value<int>(), 15);


	const volatile VariantValue v4(t);

	VariantValue r4 = method.call(v4, 3);

	TS_ASSERT(r4.isA<int>());
	TS_ASSERT_EQUALS(r4.value<int>(), 15);
}


void MethodTestSuite::testStaticMethod()
{
	auto method = make_static_method<Test1, int (*)(int)>(&method_type<int (*)(int)>::bindcall<&Test1::method5>, "method5", "int", "int");
	VariantValue r = method.call(3);
	TS_ASSERT(r.isA<int>());
	TS_ASSERT_EQUALS(r.value<int>(), 18);
}


void MethodTestSuite::testLuaAPI()
{
	LuaUtils::LuaStateHolder L;
    LuaUtils::addTestFunctionsAndPaths(&*L);

    const int errIndex = LuaUtils::pushTraceBack(L);
    if (luaL_loadfile(L, strconv::fmt_str("%1/method_test.lua", srcpath()).c_str()) || lua_pcall(L,0,0,errIndex)) {
		luaL_error(L, "cannot run config file: %s\n", lua_tostring(L, -1));
	}
    LuaUtils::removeTraceBack(L, errIndex);
	LuaUtils::callFunc<bool>(L, "testMethod");
}


void MethodTestSuite::testMethodHash()
{
	using namespace std;
	unordered_map<Method, int> mmap;

	Class test = Class::lookup("MethodTest::Test1");
	auto it = test.methods().begin();
	Method m1 = *it++;
	Method m2 = *it++;
	Method m3 = *it++;

	mmap[m1] = 1;
	mmap[m2] = 2;
	mmap[m3] = 3;

	TS_ASSERT_EQUALS(mmap[m1], 1);
	TS_ASSERT_EQUALS(mmap[m2], 2);
	TS_ASSERT_EQUALS(mmap[m3], 3);
}

void MethodTestSuite::testClassRef()
{

	Class test = Class::lookup("MethodTest::Test1");
	Class test2 = Class::lookup("MethodTest::Test2");
	auto it = test.methods().begin();
	Method m1 = *it++;
	Method m2 = *it++;
	Method m3 = *it++;

	TS_ASSERT_EQUALS(m1.getClass(), test);
	TS_ASSERT_EQUALS(m2.getClass(), test);
	TS_ASSERT_EQUALS(m3.getClass(), test);

	TS_ASSERT_EQUALS(m1.getClass(), m2.getClass());

	TS_ASSERT_DIFFERS(m1.getClass(), test2);
}

void MethodTestSuite::testFullName()
{
	Class test = Class::lookup("MethodTest::Test1");

	Method m1 = test.findMethod([](const Method& m){ return m.name() == "method1";});
	Method m2 = test.findMethod([](const Method& m){ return m.name() == "method2";});
	Method m3 = test.findMethod([](const Method& m){ return m.name() == "method3";});
	Method m41 = test.findMethod([](const Method& m){ return m.name() == "method4" && m.numberOfArguments() == 1;});
	Method m42 = test.findMethod([](const Method& m){ return m.name() == "method4" && m.numberOfArguments() == 2;});
	Method m5 = test.findMethod([](const Method& m){ return m.name() == "method5";});

	TS_ASSERT(m1.isValid());
	TS_ASSERT(m2.isValid());
	TS_ASSERT(m3.isValid());
	TS_ASSERT(m41.isValid());
	TS_ASSERT(m42.isValid());
	TS_ASSERT(m5.isValid());

	TS_ASSERT(overloads(m41, m42));
	TS_ASSERT(overloads(m42, m41));
	TS_ASSERT(!overrides(m41, m42));
	TS_ASSERT(!overloads(m1, m2));

	TS_ASSERT_EQUALS(m1.fullName(), "int MethodTest::Test1::method1(int)");
	TS_ASSERT_EQUALS(m2.fullName(), "int MethodTest::Test1::method2(int) const");
	TS_ASSERT_EQUALS(m3.fullName(), "int MethodTest::Test1::method3(int) volatile");
	TS_ASSERT_EQUALS(m41.fullName(), "int MethodTest::Test1::method4(int) const volatile");
	TS_ASSERT_EQUALS(m42.fullName(), "int MethodTest::Test1::method4(int, int) const volatile");
	TS_ASSERT_EQUALS(m5.fullName(), "static int MethodTest::Test1::method5(int)");
}


void MethodTestSuite::testMethodOverriding()
{
	Class base = Class::lookup("MethodTest::Base");
	Class derived = Class::lookup("MethodTest::Derived");

	TS_ASSERT(base.isValid());
	TS_ASSERT(derived.isValid());

	Method bm1 = derived.findMethod([&](const Method& m) { return m.name() == "method1" && m.getClass() == base; });
	Method bm2 = derived.findMethod([&](const Method& m) { return m.name() == "method2" && m.getClass() == base; });


	Method dm1 = derived.findMethod([&](const Method& m) { return m.name() == "method1" && m.getClass() == derived; });
	Method dm2 = derived.findMethod([&](const Method& m) { return m.name() == "method2" && m.getClass() == derived; });

	TS_ASSERT(bm1.isValid());
	TS_ASSERT(bm2.isValid());
	TS_ASSERT(dm1.isValid());
	TS_ASSERT(dm2.isValid());

	TS_ASSERT(overrides(dm1,bm1));
	TS_ASSERT(overrides(dm2,bm2));

	TS_ASSERT(!overrides(dm2,dm1));
	TS_ASSERT(!overloads(dm2,dm1));

	TS_ASSERT_DIFFERS(bm1, dm1);
	TS_ASSERT_DIFFERS(bm2, dm2);

	Constructor bc = base.findConstructor([](const Constructor& c){ return c.isDefaultConstructor(); });
	Constructor dc = derived.findConstructor([](const Constructor& c){ return c.isDefaultConstructor(); });

	TS_ASSERT(bc.isValid());
	TS_ASSERT(dc.isValid());


	VariantValue binst = bc.call();
	VariantValue dinst = dc.call();

	TS_ASSERT(binst.isValid());
	TS_ASSERT(dinst.isValid());

	VariantValue r1 = bm1.call(binst);
	TS_ASSERT(r1.isA<int>());
	TS_ASSERT_EQUALS(r1.value<int>(), 555);

	VariantValue r2 = bm2.call(binst);
	TS_ASSERT(r2.isA<int>());
	TS_ASSERT_EQUALS(r2.value<int>(), 666);

	VariantValue r3 = bm1.call(dinst);
	TS_ASSERT(r3.isA<int>());
	TS_ASSERT_EQUALS(r3.value<int>(), 556);

	VariantValue r4 = bm2.call(dinst);
	TS_ASSERT(r4.isA<int>());
	TS_ASSERT_EQUALS(r4.value<int>(), 666);

	VariantValue r5 = dm1.call(dinst);
	TS_ASSERT(r5.isA<int>());
	TS_ASSERT_EQUALS(r5.value<int>(), 556);

	VariantValue r6 = dm2.call(dinst);
	TS_ASSERT(r6.isA<int>());
	TS_ASSERT_EQUALS(r6.value<int>(), 667);
}

void MethodTestSuite::testLoggerCase()
{
    Class test = Class::lookup("MethodTest::LoggerFactory");

    Method m1 = test.findMethod([](const Method& m){ return m.name() == "getLogger";});
    Method m2 = test.findMethod([](const Method& m){ return m.name() == "getLogger2";});
    Method m3 = test.findMethod([](const Method& m){ return m.name() == "getLogger3";});
    Method m4 = test.findMethod([](const Method& m){ return m.name() == "getLogger4";});
    Method m5 = test.findMethod([](const Method& m){ return m.name() == "getLogger5";});

    VariantValue v;
    v.construct<const char*>("hello");
    bool success = false;
    v.convertTo<const std::string&>(&success);
    TS_ASSERT(success);

    /*VariantValue v2;
    v2.construct<float>(3.5);
    success = false;
    v2.convertTo<const int&>(&success);
    TS_ASSERT(success);*/

    TS_ASSERT(m1.isValid());
    TS_ASSERT(m2.isValid());
    TS_ASSERT(m3.isValid());
    TS_ASSERT(m4.isValid());
    TS_ASSERT(m5.isValid());
    m3.call("hello");
    m4.call("hello");
    m5.call(true);
    m2.call("myLogger", "stderr", true);
    m1.call("myLogger", "stderr", true);

    VariantValue str1;
    str1.construct<const char*>("Roger the Shrubber");
    cout << str1.convertTo<std::string>() << endl;
    cout << str1.convertTo<const std::string&>() << endl;
    VariantValue obj;

    m3.callArgArray(obj, {str1});
    m4.callArgArray(obj, {str1});
}


void MethodTestSuite::testReturnByValue()
{
    auto m = make_method<CopyCount(MethodTest::TestParamPassing::*)()>(&method_type<CopyCount(MethodTest::TestParamPassing::*)()>::bindcall<&MethodTest::TestParamPassing::returnObjectByValue>, NULL, NULL, NULL);
    TestParamPassing inst;
    VariantValue vinst;
    vinst.construct<TestParamPassing>();

    CopyCount::resetAll();

    CopyCount c = inst.returnObjectByValue();

    TS_ASSERT_EQUALS(c.id(), 33);

    int directCallCopies = CopyCount::numberOfCopies();
    CopyCount::resetAll();

    VariantValue v = m.call(vinst);

    int reflectionCopies = CopyCount::numberOfCopies();

    TS_ASSERT(v.isValid());

    CopyCount& ref = v.convertTo<CopyCount&>();

    TS_ASSERT_EQUALS(ref.id(),33);

    TS_ASSERT_EQUALS(directCallCopies, reflectionCopies);

}

void MethodTestSuite::testReturnByReference()
{

    auto m = make_method<CopyCount& (MethodTest::TestParamPassing::*)()>(&method_type<CopyCount&(MethodTest::TestParamPassing::*)()>::bindcall<&MethodTest::TestParamPassing::returnObjectByReference>, NULL, NULL, NULL);
    TestParamPassing inst;
    VariantValue vinst;
    vinst.construct<TestParamPassing>();

    CopyCount::resetAll();
    VariantValue v = m.call(vinst);
    CopyCount& ref = v.convertTo<CopyCount&>();

    TS_ASSERT_EQUALS(ref.id(), 777);
    ref.changeId(888);
    TS_ASSERT_EQUALS(inst.returnObjectByReference().id(), 888);
    TS_ASSERT_EQUALS(CopyCount::numberOfCopies(), 0);
    TS_ASSERT(!inst.returnObjectByReference().hasBeenMoved())
}


void MethodTestSuite::testReturnByConstReference()
{

    auto m = make_method<const CopyCount& (MethodTest::TestParamPassing::*)()>(&method_type<const CopyCount&(MethodTest::TestParamPassing::*)()>::bindcall<&MethodTest::TestParamPassing::returnObjectByConstReference>, NULL, NULL, NULL);
    TestParamPassing inst;
    VariantValue vinst;
    vinst.construct<TestParamPassing>();

    CopyCount::resetAll();
    VariantValue v = m.call(vinst);
    TS_ASSERT(v.isValid());
    bool ok = false;
    v.convertTo<CopyCount&>(&ok);
    TS_ASSERT(!ok);

    const CopyCount& cref = v.convertTo<const CopyCount&>(&ok);
    TS_ASSERT(ok);

    TS_ASSERT_EQUALS(CopyCount::numberOfCopies(), 0);

    TS_ASSERT_EQUALS(cref.id(), 888);
    TS_ASSERT_EQUALS(&cref, &inst.returnObjectByConstReference());
}


void MethodTestSuite::testParametersByValue()
{
    auto m = make_method<int(MethodTest::TestParamPassing::*)(CopyCount)>(&method_type<int(MethodTest::TestParamPassing::*)(CopyCount)>::bindcall<&MethodTest::TestParamPassing::paramByValue>, NULL, NULL, NULL);
    TestParamPassing inst;
    VariantValue vinst;
    vinst.construct<TestParamPassing>();

    CopyCount::resetAll();

    CopyCount c(77);

    TS_ASSERT_EQUALS(inst.paramByValue(c), 77);

    int directCallCopies = CopyCount::numberOfCopies();
    int directCallMoves = CopyCount::numberOfMoves();
    TS_ASSERT_EQUALS(directCallCopies, 1);
    TS_ASSERT_EQUALS(directCallMoves,  0);

    CopyCount::resetAll();

    VariantValue v = m.call(vinst, c);

    TS_ASSERT(v.isValid());
    TS_ASSERT_EQUALS(v.value<int>(), 77);
    int reflectionCopies = CopyCount::numberOfCopies();
    int reflectionMoves = CopyCount::numberOfMoves();

    TS_ASSERT_EQUALS(reflectionCopies, 1);
    TS_ASSERT_EQUALS(reflectionMoves,  0);
    TS_ASSERT(!c.hasBeenMoved());
}


void MethodTestSuite::testParametersByReference()
{

    auto m = make_method<int(MethodTest::TestParamPassing::*)(CopyCount&)>(&method_type<int(MethodTest::TestParamPassing::*)(CopyCount&)>::bindcall<&MethodTest::TestParamPassing::paramByReference>, NULL, NULL, NULL);
    TestParamPassing inst;
    VariantValue vinst;
    vinst.construct<TestParamPassing>();

    CopyCount::resetAll();

    CopyCount c(44);

    TS_ASSERT_EQUALS(inst.paramByReference(c), 45);
    TS_ASSERT_EQUALS(c.id(), 45);
    TS_ASSERT_EQUALS(CopyCount::numberOfCopies(), 0);

    VariantValue r = m.call(vinst, c);

    TS_ASSERT_EQUALS(CopyCount::numberOfCopies(), 0);
    TS_ASSERT(r.isValid());
    TS_ASSERT_EQUALS(r.value<int>(), 46);
    TS_ASSERT_EQUALS(c.id(), 46);
}


void MethodTestSuite::testParametersByConstReference()
{
    auto m = make_method<int(MethodTest::TestParamPassing::*)(const CopyCount&)>(&method_type<int(MethodTest::TestParamPassing::*)(const CopyCount&)>::bindcall<&MethodTest::TestParamPassing::paramByConstReference>, NULL, NULL, NULL);
    TestParamPassing inst;
    VariantValue vinst;
    vinst.construct<TestParamPassing>();

    CopyCount::resetAll();

    CopyCount c(44);

    TS_ASSERT_EQUALS(inst.paramByConstReference(c), 44);
    TS_ASSERT_EQUALS(c.id(), 44);
    TS_ASSERT_EQUALS(CopyCount::numberOfCopies(), 0);

    VariantValue r = m.call(vinst, c);

    TS_ASSERT_EQUALS(CopyCount::numberOfCopies(), 0);
    TS_ASSERT(r.isValid());
    TS_ASSERT_EQUALS(r.value<int>(), 44);
    TS_ASSERT_EQUALS(c.id(), 44);
}
