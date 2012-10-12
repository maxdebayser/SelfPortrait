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

	if (luaL_loadfile(L, "method_test.lua") || lua_pcall(L,0,0,0)) {
		luaL_error(L, "cannot run config file: %s\n", lua_tostring(L, -1));
	}
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
