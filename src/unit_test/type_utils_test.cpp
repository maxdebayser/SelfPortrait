#include "type_utils_test.h"

#include "typeutils.h"
#include "str_conversion.h"
#include <memory>
#include <iostream>
#include <vector>
namespace {

    class NonComparable {

    };

    class Comparable {

    };
    bool operator==(const Comparable&, const Comparable&) {
        return true;
    }

    template<class T, class B = bool>
    class comparable_template {
    public:
        T t;
    };
    template<class T>
    bool operator==(const comparable_template<T>& t1, const comparable_template<T>& t2) {
        return  t1.t == t2.t;
    }
}


void TypeUtilsTestSuite::testMeta()
{
    static_assert(comparable<int>::value == true, "");
    static_assert(comparable<std::string>::value == true, "");

    static_assert(comparable<Comparable>::value == true, "");
    static_assert(comparable<NonComparable>::value == false, "");

    static_assert(comparable<comparable_template<Comparable>>::value == false, "");
    static_assert(comparable<comparable_template<NonComparable>>::value == false, "");

    static_assert(comparable<std::vector<Comparable>>::value == true, "");
    static_assert(comparable<std::vector<NonComparable>>::value == false, "");

    // test recursion, fuck yeah!
    static_assert(comparable<std::vector<std::vector<Comparable>>>::value == true, "");
    static_assert(comparable<std::vector<std::vector<NonComparable>>>::value == false, "");

    static_assert(comparable<std::pair<Comparable, Comparable>>::value == true, "");
    static_assert(comparable<std::pair<NonComparable, NonComparable>>::value == false, "");

    static_assert( std::is_same<std::false_type, static_and<std::false_type>::type>::value, "");
    static_assert( std::is_same<std::true_type, static_and<std::true_type>::type>::value, "");

    static_assert( std::is_same<std::true_type,  static_and<std::true_type,  std::true_type>::type>::value,  "");
    static_assert( std::is_same<std::false_type, static_and<std::true_type,  std::false_type>::type>::value, "");
    static_assert( std::is_same<std::false_type, static_and<std::true_type,  std::false_type>::type>::value, "");
    static_assert( std::is_same<std::false_type, static_and<std::false_type, std::false_type>::type>::value, "");

    static_assert(comparable<std::pair<Comparable, NonComparable>>::value == false, "");
    static_assert(comparable<std::pair<NonComparable, Comparable>>::value == false, "");

    static_assert(comparable<std::tuple<Comparable, Comparable>>::value == true, "");
    static_assert(comparable<std::tuple<NonComparable, NonComparable>>::value == false, "");
    static_assert(comparable<std::tuple<Comparable, NonComparable>>::value == false, "");
    static_assert(comparable<std::tuple<NonComparable, Comparable>>::value == false, "");

    static_assert(strconv::convertible_to_string<int>() == true, "");
    static_assert(strconv::convertible_to_string<std::string>() == true, "");
    static_assert(strconv::convertible_to_string<const char*>() == true, "");


    static_assert(strconv::convertible_to_string<std::unique_ptr<int>>() == false, "");
    static_assert(strconv::convertible_to_string<Comparable>() == false, "");


    static_assert(strconv::convertible_from_string<int>() == true, "");
    static_assert(strconv::convertible_from_string<std::string>() == true, "");
    static_assert(strconv::convertible_from_string<char*>() == true, "");

    static_assert(strconv::convertible_from_string<std::unique_ptr<int>>() == false, "");
    static_assert(strconv::convertible_from_string<Comparable>() == false, "");
}
