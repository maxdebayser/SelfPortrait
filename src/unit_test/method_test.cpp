#include "method_test.h"
#include "method.h"
#include "reflection.h"

#include <iostream>
#include <string>
using namespace std;

namespace {
	class Test1 {
	public:
		int method1(int arg) { return arg*2; }
		int method2(int arg) const { return arg*3; }
		int method3(int arg) volatile { return arg*4; }
		int method4(int arg) const volatile { return arg*5; }
	};

}


void MethodTestSuite::testNonCVMethod() {

	auto method = make_method("method1", &Test1::method1);

	VariantValue v1 = Test1();

	VariantValue r1 = method.call(v1, 3);

	TS_ASSERT(r1.isA<int>());
	TS_ASSERT_EQUALS(r1.value<int>(), 6);

	const VariantValue v2 = Test1();
	TS_ASSERT_THROWS(method.call(v2, 3), std::runtime_error);

	volatile VariantValue v3 = Test1();
	TS_ASSERT_THROWS(method.call(v3, 3), std::runtime_error);

	const volatile VariantValue v4 = Test1();
	TS_ASSERT_THROWS(method.call(v4, 3), std::runtime_error);
}


void MethodTestSuite::testCMethod()
{

	auto method = make_method("method2", &Test1::method2);

	VariantValue v1 = Test1();
	VariantValue r1 = method.call(v1, 3);

	TS_ASSERT(r1.isA<int>());
	TS_ASSERT_EQUALS(r1.value<int>(), 9);


	const VariantValue v2 = Test1();
	VariantValue r2 = method.call(v2, 3);

	TS_ASSERT(r2.isA<int>());
	TS_ASSERT_EQUALS(r2.value<int>(), 9);

	volatile VariantValue v3 = Test1();
	TS_ASSERT_THROWS(method.call(v3, 3), std::runtime_error);

	const volatile VariantValue v4 = Test1();
	TS_ASSERT_THROWS(method.call(v4, 3), std::runtime_error);
}


void MethodTestSuite::testVMethod()
{
	auto method = make_method("method3", &Test1::method3);

	VariantValue v1 = Test1();

	VariantValue r = method.call(v1, 3);

	TS_ASSERT(r.isA<int>());
	TS_ASSERT_EQUALS(r.value<int>(), 12);

	const VariantValue v2 = Test1();
	TS_ASSERT_THROWS(method.call(v2, 3), std::runtime_error);


	volatile VariantValue v3 = Test1();

	VariantValue r3 = method.call(v3, 3);

	TS_ASSERT(r3.isA<int>());
	TS_ASSERT_EQUALS(r3.value<int>(), 12);

	const volatile VariantValue v4 = Test1();
	TS_ASSERT_THROWS(method.call(v4, 3), std::runtime_error);
}


void MethodTestSuite::testCVMethod()
{
	auto method = make_method("method4", &Test1::method4);

	VariantValue v1 = Test1();

	VariantValue r = method.call(v1, 3);

	TS_ASSERT(r.isA<int>());
	TS_ASSERT_EQUALS(r.value<int>(), 15);

	const VariantValue v2 = Test1();

	VariantValue r2 = method.call(v2, 3);

	TS_ASSERT(r2.isA<int>());
	TS_ASSERT_EQUALS(r2.value<int>(), 15);


	volatile VariantValue v3 = Test1();

	VariantValue r3 = method.call(v3, 3);

	TS_ASSERT(r3.isA<int>());
	TS_ASSERT_EQUALS(r3.value<int>(), 15);


	const volatile VariantValue v4 = Test1();

	VariantValue r4 = method.call(v4, 3);

	TS_ASSERT(r4.isA<int>());
	TS_ASSERT_EQUALS(r4.value<int>(), 15);
}

