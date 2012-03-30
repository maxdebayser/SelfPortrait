#include "attribute_test.h"
#include "attribute.h"
#include "reflection.h"

#include <iostream>
#include <string>
using namespace std;


namespace {

	class Test {
	public:

		int attr1;
		const int attr2;
		static int attr3;
		static const int attr4;

		int& attr5;

		Test() : attr1(101), attr2(102), attr5(attr1) {}

	};

	int Test::attr3 = 103;
	const int Test::attr4 = 104;

}

void AttributeTestSuite::testVanillaAttribute()
{
	auto attr1 = make_attribute("attr1", &Test::attr1);

	VariantValue v1 = Test();
	VariantValue r1 = attr1.get(v1);

	TS_ASSERT(r1.isA<int>());
	TS_ASSERT_EQUALS(r1.value<int>(), 101);

	attr1.set(v1, 201);
	r1 = attr1.get(v1);
	TS_ASSERT(r1.isA<int>());
	TS_ASSERT_EQUALS(r1.value<int>(), 201);

	TS_ASSERT_THROWS(attr1.get(), std::runtime_error);
	TS_ASSERT_THROWS(attr1.set(201), std::runtime_error);

	const VariantValue v2 = Test();
	VariantValue r2 = attr1.get(v2);

	TS_ASSERT(r2.isA<int>());
	TS_ASSERT_EQUALS(r2.value<int>(), 101);

	TS_ASSERT_THROWS(attr1.set(v2, 201), std::runtime_error);
}


void AttributeTestSuite::testConstAttribute()
{
	auto attr2 = make_attribute("attr2", &Test::attr2);

	VariantValue v1 = Test();
	VariantValue r1 = attr2.get(v1);

	TS_ASSERT(r1.isA<int>());
	TS_ASSERT_EQUALS(r1.value<int>(), 102);
	TS_ASSERT_THROWS(attr2.set(v1, 202), std::runtime_error);

	const VariantValue v2 = Test();
	VariantValue r2 = attr2.get(v2);
	TS_ASSERT(r2.isA<int>());
	TS_ASSERT_EQUALS(r2.value<int>(), 102);

	TS_ASSERT_THROWS(attr2.set(v2, 202), std::runtime_error);
}


void AttributeTestSuite::testStaticAttribute()
{
	auto attr3 = make_static_attribute<Test>("attr3", &Test::attr3);

	VariantValue r1 = attr3.get();

	TS_ASSERT(r1.isA<int>());
	TS_ASSERT_EQUALS(r1.value<int>(), 103);

	attr3.set(201);
	r1 = attr3.get();
	TS_ASSERT(r1.isA<int>());
	TS_ASSERT_EQUALS(r1.value<int>(), 201);


	auto attr4 = make_static_attribute<Test>("attr4", &Test::attr4);
	VariantValue r2 = attr4.get();

	TS_ASSERT(r2.isA<int>());
	TS_ASSERT_EQUALS(r2.value<int>(), 104);

	TS_ASSERT_THROWS(attr4.set(202), std::runtime_error);
}

