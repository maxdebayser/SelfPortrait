/*
** SelfPortrait API
** See Copyright Notice in reflection.h
*/
#include "variant_test.h"

#include <iostream>
#include <string>
#include <boost/date_time/gregorian/gregorian.hpp>
#include "lua_utils.h"
#include "test_utils.h"

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
    TS_ASSERT(v3.isA<char[]>());

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

    int pointee = 666;

    VariantValue v4(&pointee);
    ok = false;
    int* ptr2 = v4.convertTo<int*>(&ok);
    TS_ASSERT(v4.mayConvertTo<int*>());
    TS_ASSERT(ok);
    TS_ASSERT_EQUALS(*ptr2, 666);
    ok = false;
    int& ref2 = v4.convertTo<int&>(&ok);
    TS_ASSERT(v4.mayConvertTo<int&>());
    TS_ASSERT(ok);
    TS_ASSERT_EQUALS(ref2, 666);

    ok = false;
    int copy1 = v4.convertTo<int>(&ok);
    TS_ASSERT(v4.mayConvertTo<int>());
    TS_ASSERT(ok);
    TS_ASSERT_EQUALS(copy1, 666);

    int cpointee = 777;

    VariantValue v5(&cpointee);
    ok = false;
    TS_ASSERT(v5.mayConvertTo<const int*>());
    const int* cptr2 = v5.convertTo<const int*>(&ok);
    TS_ASSERT(ok);
    TS_ASSERT_EQUALS(*cptr2, 777);
    ok = false;
    TS_ASSERT(v5.mayConvertTo<const int&>());
    const int& cref2 = v5.convertTo<const int&>(&ok);
    TS_ASSERT(ok);
    TS_ASSERT_EQUALS(cref2, 777);
    ok = false;
    TS_ASSERT(v5.mayConvertTo<const int>());
    const int copy2 = v5.convertTo<const int>(&ok);
    TS_ASSERT(ok);
    TS_ASSERT_EQUALS(copy2, 777);

}

struct NonCopyable {

    NonCopyable(int i) : m_i(i) {}

	NonCopyable(const NonCopyable&) = delete;
	NonCopyable(NonCopyable&&) = delete;
	NonCopyable& operator=(const NonCopyable&) = delete;
	NonCopyable& operator=(NonCopyable&&) = delete;

	int m_i;
};

namespace my {
template<typename T>
T& value_ref();


template<class T, class = decltype(value_ref<T>() = value_ref<T>() )>
std::true_type  supports_assignment(const T&);
std::false_type supports_assignment(...);

template<class T> using assigneable = decltype(supports_assignment(value_ref<T>()));

template<class S, class T>
struct pair {
    S first;
    T second;

    pair&
    operator=(const pair& __p)
    {
  first = __p.first;
  second = __p.second;
  return *this;
    }
};

}

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


    VariantValue v4;
    typedef std::pair<const std::string, int> problematic_pair;
    v4.construct<problematic_pair>();

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

namespace {
enum class Units {
    INV_VOLUME = 1,
    VOLUME = 2,
    DISTANCE = 3
};

}

void VariantTestSuite::testEnum()
{
    VariantValue v1;
    v1.construct<int>(1);
    bool success = false;
    Units u1 = v1.moveValue<Units>(&success);
    TS_ASSERT(success);
    TS_ASSERT_EQUALS(u1, Units::INV_VOLUME);

    VariantValue v2;
    v2.construct<Units>(Units::VOLUME);
    success = false;
    int u2 = v2.moveValue<int>(&success);
    TS_ASSERT(success);
    TS_ASSERT_EQUALS(u2, 2);

    VariantValue v3;
    v3.construct<Units>(Units::DISTANCE);
    success = false;
    double u3 = v3.moveValue<double>(&success);
    TS_ASSERT(success);
    TS_ASSERT_EQUALS(u3, 3);
}


void VariantTestSuite::testPrintable()
{
    stringstream ss;
    boost::gregorian::days d(666);
    ss << d;
    TS_ASSERT_EQUALS(ss.str(), "666");
    VariantValue v;
    v.construct<boost::gregorian::days>(667);
    TS_ASSERT_EQUALS(v.convertToThrow<std::string>(), "667");
}

void VariantTestSuite::testLuaAPI()
{
    LuaUtils::LuaStateHolder L;
    LuaUtils::addTestFunctionsAndPaths(&*L);

    const int errIndex = LuaUtils::pushTraceBack(L);
    if (luaL_loadfile(L, strconv::fmt_str("%1/variant_test.lua", srcpath()).c_str()) || lua_pcall(L,0,0,errIndex)) {
        luaL_error(L, "cannot run config file: %s\n", lua_tostring(L, -1));
    }
    LuaUtils::removeTraceBack(L, errIndex);
    LuaUtils::callFunc<bool>(L, "testVariant");
}


void VariantTestSuite::testAssignement()
{
    VariantValue v1(33);
    bool ok = false;

    int val1 = v1.convertTo<int>(&ok);
    TS_ASSERT(ok);
    TS_ASSERT_EQUALS(val1, 33);

    v1.assign(35);

    ok = false;
    int val2 = v1.convertTo<int>(&ok);
    TS_ASSERT(ok);
    TS_ASSERT_EQUALS(val2, 35);

    VariantValue v2;
    v2.construct<int&>(val2);

    v2.assign(45);

    TS_ASSERT_EQUALS(val2, 45);

}

struct AlignTest {
    long double l;
};

void VariantTestSuite::testAlignemnt()
{
    {
        VariantValue v;
        v.construct<char>(0);
        TS_ASSERT_EQUALS(alignof(char), v.alignOf());
    }
    {
        VariantValue v;
        v.construct<short>(0);
        TS_ASSERT_EQUALS(alignof(short), v.alignOf());
    }
    {
        VariantValue v;
        v.construct<int>(0);
        TS_ASSERT_EQUALS(alignof(int), v.alignOf());
    }
    {
        VariantValue v;
        v.construct<long int>(0);
        TS_ASSERT_EQUALS(alignof(long int), v.alignOf());
    }
    {
        VariantValue v;
        v.construct<long long int>(0);
        TS_ASSERT_EQUALS(alignof(long long int), v.alignOf());
    }
    {
        VariantValue v;
        v.construct<double>(0);
        TS_ASSERT_EQUALS(alignof(double), v.alignOf());
    }
    {
        VariantValue v;
        v.construct<long double>(0);
        TS_ASSERT_EQUALS(alignof(long double), v.alignOf());
    }
    {
        VariantValue v;
        v.construct<int*>(nullptr);
        TS_ASSERT_EQUALS(alignof(int*), v.alignOf());
    }
    {
        VariantValue v;
        v.construct<AlignTest>();
        TS_ASSERT_EQUALS(alignof(AlignTest), v.alignOf());
    }
}
