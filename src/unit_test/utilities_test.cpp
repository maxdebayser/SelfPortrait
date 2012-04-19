#include "utilities_test.h"
#include "typelist.h"
#include "str_conversion.h"
#include "reflection.h"


#include <type_traits>
#include <iostream>
#include <string>
using namespace std;


void UtilitiesTestSuite::testTypelist()
{
	// Compile-time tests
	typedef TypeList<int, double, char> TestTL;

	static_assert( typelist_size<TypeList<>>::value == 0, "wrong typelist size");
	static_assert( typelist_size<TestTL>::value == 3, "wrong typelist size");

	static_assert( contains<TestTL, int>(), "type element not found");
	static_assert( contains<TestTL, double>(), "type element not found");
	static_assert( contains<TestTL, char>(), "type element not found");

	static_assert( is_same< typename prepend<float, TestTL>::type, TypeList<float, int, double, char>  >::value, "prepend failed");
	//static_assert( is_same< typename append<float, TestTL>::type, TypeList<int, double, char, float>  >::value, "append failed");

	static_assert( is_same< typename sublist_at<TestTL, 0>::type, TypeList<int, double, char> >::value, "sublist_at failed");
	static_assert( is_same< typename sublist_at<TestTL, 1>::type, TypeList<double, char> >::value, "sublist_at failed");
	static_assert( is_same< typename sublist_at<TestTL, 2>::type, TypeList<char> >::value, "sublist_at failed");

	static_assert( is_same< typename sublist_up_to<TestTL, 0>::type, TypeList<int> >::value, "sublist_up_to failed");
	static_assert( is_same< typename sublist_up_to<TestTL, 1>::type, TypeList<int, double> >::value, "sublist_up_to failed");
	static_assert( is_same< typename sublist_up_to<TestTL, 2>::type, TypeList<int, double, char> >::value, "sublist_up_to failed");

	static_assert( is_same< typename type_at<TestTL, 0>::type, int >::value, "type_at failed");
	static_assert( is_same< typename type_at<TestTL, 1>::type, double >::value, "type_at failed");
	static_assert( is_same< typename type_at<TestTL, 2>::type, char >::value, "type_at failed");

	static_assert( is_same< typename tail_at<TestTL, 0>::type, TypeList<double, char> >::value, "tail_at failed");
	static_assert( is_same< typename tail_at<TestTL, 1>::type, TypeList<char> >::value, "tail_at failed");
	static_assert( is_same< typename tail_at<TestTL, 2>::type, TypeList<> >::value, "tail_at failed");

	static_assert( is_same< typename selection<TestTL, Indices<0,2> >::type, TypeList<int, char> >::value, "select failed");
	static_assert( is_same< typename selection<TestTL, Indices<0> >::type, TypeList<int> >::value, "select failed");
	static_assert( is_same< typename selection<TestTL, Indices<1,2> >::type, TypeList<double, char> >::value, "select failed");

	static_assert( is_same< typename concat<TypeList<>, TypeList<> >::type, TypeList<> >::value, "concat failed");
	static_assert( is_same< typename concat<TypeList<int>, TypeList<> >::type, TypeList<int> >::value, "concat failed");
	static_assert( is_same< typename concat<TypeList<>, TypeList<int> >::type, TypeList<int> >::value, "concat failed");
	static_assert( is_same< typename concat<TypeList<int>, TypeList<float> >::type, TypeList<int, float> >::value, "concat failed");
	static_assert( is_same< typename concat<TypeList<int, char>, TypeList<float> >::type, TypeList<int, char, float> >::value, "concat failed");

	static_assert( is_same< typename erase_at<TestTL, 0 >::type, TypeList<double, char> >::value, "erase_at failed");
	static_assert( is_same< typename erase_at<TestTL, 1 >::type, TypeList<int, char> >::value, "erase_at failed");
	static_assert( is_same< typename erase_at<TestTL, 2 >::type, TypeList<int, double> >::value, "erase_at failed");

	static_assert( is_same< typename make_indices<0>::type, Indices<> >::value, "make_indices failed");
	static_assert( is_same< typename make_indices<1>::type, Indices<0> >::value, "make_indices failed");
	static_assert( is_same< typename make_indices<2>::type, Indices<0,1> >::value, "make_indices failed");
	static_assert( is_same< typename make_indices<3>::type, Indices<0,1,2> >::value, "make_indices failed");

}

namespace {

	struct ConvertibleTo {
		operator std::string() const {
			return "this is a late parrot";
		}
	};

	struct Printable {
	};

	std::ostream& operator<<(std::ostream& stream, const Printable& p) {
		return stream << "this parrot is no more";
	}

	struct ConvertibleToAndPrintable {
		operator std::string() const {
			return "whatever1";
		}
	};

	std::ostream& operator<<(std::ostream& stream, const ConvertibleToAndPrintable& p) {
		return stream << "whatever1";
	}


	struct NoConversions {};


	struct ConvertibleFrom {
		ConvertibleFrom(const std::string& str) : attr1(str) {}
		std::string attr1;
	};

	struct Parseable {
		Parseable() {}
		std::string attr1;
	};

	std::istream& operator>>(std::istream& stream, Parseable& p) {
		return stream >> p.attr1;
	}

}


void UtilitiesTestSuite::testConversionsToStr()
{
	bool success = false;
	ConvertibleTo t1;
	std::string r1 = toString(t1, &success);
	TS_ASSERT(success);
	TS_ASSERT_EQUALS(r1, "this is a late parrot");

	// Disabled until I solve the problem of ambiguities introduced
	// by object which are not printable but are convertible to scalar types
	//Printable t2;
	//std::string r2 = toString(t2, &success);
	//TS_ASSERT(success);
	//TS_ASSERT_EQUALS(r2, "this parrot is no more");


	ConvertibleToAndPrintable t3;
	std::string r3 = toString(t3, &success);
	TS_ASSERT(success);
	TS_ASSERT_EQUALS(r3, "whatever1");

	std::string r4 = toString(3, &success);
	TS_ASSERT(success);
	TS_ASSERT_EQUALS(r4, "3");


	NoConversions t5;
	std::string r5 = toString(t5, &success);
	TS_ASSERT(!success);
}


void UtilitiesTestSuite::testConversionsFromStr()
{
	bool success = false;

	int i = fromString<int>("3", &success);
	TS_ASSERT(success);
	TS_ASSERT_EQUALS(i, 3);

	ConvertibleFrom t1 = fromString<ConvertibleFrom>("it's gone to meet its maker", &success);
	TS_ASSERT(success);
	TS_ASSERT_EQUALS(t1.attr1, "it's gone to meet its maker");

	Parseable t2 = fromString<Parseable>("deceased", &success);
	TS_ASSERT(success);
	TS_ASSERT_EQUALS(t2.attr1, "deceased");

	NoConversions t3 = fromString<NoConversions>("whatever", &success);
	TS_ASSERT(!success);
}
