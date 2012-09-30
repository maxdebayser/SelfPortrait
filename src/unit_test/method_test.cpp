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
