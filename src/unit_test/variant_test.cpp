/*
** SelfPortrait API
** See Copyright Notice in reflection.h
*/
#include "variant_test.h"

#include <iostream>
#include <string>

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
	TS_ASSERT(v3.isA<const char*>());
	TS_ASSERT_EQUALS(v3.value<const char*>(), "hello");
	TS_ASSERT(v3.isA<decltype("hello")>());
	TS_ASSERT(v3.isA<const char[]>());
	TS_ASSERT(v3.isA<const char(&)[]>());
	TS_ASSERT(!v3.isA<char[]>());

	VariantValue v4(3);
	TS_ASSERT_EQUALS(v1,v4);

	VariantValue v5 = v4;
	TS_ASSERT_EQUALS(v5,v4);
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

	VariantValue v3(3);

	TS_ASSERT(v3.isA<int>());
	TS_ASSERT_EQUALS(v3.value<int>(), 3);

	bool ok = false;

	int* ptr = v3.convertTo<int*>(&ok);
	TS_ASSERT(ok);
	TS_ASSERT_EQUALS(*ptr, 3);
	*ptr = 4;
	TS_ASSERT_EQUALS(*ptr, 4);
	TS_ASSERT_EQUALS(v3.value<int>(), 4);

	const int* cptr = v3.convertTo<const int*>(&ok);
	TS_ASSERT(ok);
	TS_ASSERT_EQUALS(*cptr, 4);
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
    static_assert(comparable<NonCopyable>::value == false, "There is no == operator defined for this class");
	VariantValue v1;
	v1.construct<NonCopyable>(2);

	NonCopyable& ref = v1.convertTo<NonCopyable&>();

	TS_ASSERT_EQUALS(ref.m_i, 2);
	TS_ASSERT_THROWS(VariantValue v2(v1), std::runtime_error);

	VariantValue v3;
	v3.construct<NonCopyable>(3);
	TS_ASSERT_THROWS(bool b = (v1 == v3), std::runtime_error);

    VariantValue v2;
    v2.construct<std::unique_ptr<int>>();

}

namespace {
	class Base {
	public:

		Base(double n) : d(n) {}
		~Base() {}

		virtual int method2() const = 0;
		virtual double method1() const {
			return d;
		}

		void setD(double e) { d = e; }

		Base(const Base&) = delete;
		Base& operator=(const Base&) = delete;
		Base(Base&&) = delete;
		Base& operator=(Base&&) = delete;
	private:
		double d;
	};

	class Derived: public Base {
	public:
		Derived() : Base(3.14), i(9) {}

		int method2() const {
			return i;
		}
	private:
		int i;
	};
}

void VariantTestSuite::testBaseConversion()
{
	VariantValue v1;
	v1.construct<Derived>();

	bool success = false;
	Derived& dref = v1.convertTo<Derived&>(&success);

	TS_ASSERT(success);

	TS_ASSERT_EQUALS(dref.method2(), 9);
	TS_ASSERT_EQUALS(dref.method1(), 3.14);

	success = false;
	Base& bref = v1.convertTo<Base&>(&success);
	TS_ASSERT(success);

	TS_ASSERT_EQUALS(bref.method2(), 9);
	TS_ASSERT_EQUALS(bref.method1(), 3.14);

	bref.setD(5.3);
	TS_ASSERT_EQUALS(dref.method1(), 5.3);
	TS_ASSERT_EQUALS(bref.method1(), 5.3);
}

