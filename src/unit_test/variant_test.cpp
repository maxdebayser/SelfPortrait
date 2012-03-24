#include "variant_test.h"

#include <iostream>
#include <string>
using namespace std;

void VariantTestSuite::testInvalid() {
	VariantValue v;
	TS_ASSERT(!v.isValid());
	v = 3;
	TS_ASSERT(v.isValid());
}

void VariantTestSuite::testValue() {
	VariantValue v1(3);
	TS_ASSERT_EQUALS(v1.value<int>(), 3);
	
	VariantValue v2(3.3);
	TS_ASSERT_EQUALS(v2.value<double>(), 3.3);
	
	VariantValue v3("hello");
	TS_ASSERT_EQUALS(v3.value<const char*>(), "hello");
}

void VariantTestSuite::testConversions() {
	VariantValue v1(3.3);
	TS_ASSERT_EQUALS(v1.convertTo<int>(), 3);
	TS_ASSERT_EQUALS(v1.convertTo<char>(), 3);
	TS_ASSERT_EQUALS(v1.convertTo<string>(), "3.3");
			
	bool success = false;
	double& ref = v1.convertTo<double&>(&success);
	TS_ASSERT(success);
	if (success) {
		TS_ASSERT_EQUALS(ref, 3.3);
		ref = 4.5;
	}
	TS_ASSERT_EQUALS(v1.value<double>(), 4.5);
	
	
	success = false;
	const double& cref = v1.convertTo<const double&>(&success);
	TS_ASSERT(success);
	if (success) {
		TS_ASSERT_EQUALS(cref, 4.5);
	}
	
	VariantValue v2(string("1.23"));
	TS_ASSERT_EQUALS(v2.convertTo<int>(), 1);
	TS_ASSERT_EQUALS(v2.convertTo<char>(), 1);
	TS_ASSERT_DELTA(v2.convertTo<float>(), 1.23, 0.0001);	

}

struct NonCopyable {

	NonCopyable(int i) : m_i(i) {}

	NonCopyable(const NonCopyable&) = delete;
	NonCopyable(NonCopyable&&) = delete;
	NonCopyable& operator=(const NonCopyable&) = delete;
	NonCopyable& operator=(NonCopyable&&) = delete;

	int m_i;
};


void VariantTestSuite::testNonCopyable()
{
	VariantValue v1;
	v1.construct<NonCopyable>(2);

	NonCopyable& ref = v1.convertTo<NonCopyable&>();

	TS_ASSERT_EQUALS(ref.m_i, 2);
	TS_ASSERT_THROWS(VariantValue v2(v1), std::runtime_error);

	VariantValue v3;
	v3.construct<NonCopyable>(3);
	TS_ASSERT_THROWS(bool b = (v1 == v3), std::runtime_error);

}
