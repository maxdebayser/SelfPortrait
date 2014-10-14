#include "type_utils_test.h"

#include "typeutils.h"
#include "str_conversion.h"
#include <memory>
#include <iostream>
namespace {

    class NonComparable {

    };

    class Comparable {

    };
    bool operator==(const Comparable&, const Comparable&) {
        return true;
    }

}



void TypeUtilsTestSuite::testMeta()
{
    static_assert(comparable<Comparable>::value == true, "");
    static_assert(comparable<NonComparable>::value == false, "");

    static_assert(convertible_to_string<int>() == true, "");
    static_assert(convertible_to_string<std::string>() == true, "");
    static_assert(convertible_to_string<const char*>() == true, "");


    static_assert(convertible_to_string<std::unique_ptr<int>>() == false, "");
    static_assert(convertible_to_string<Comparable>() == false, "");


    static_assert(convertible_from_string<int>() == true, "");
    static_assert(convertible_from_string<std::string>() == true, "");
    static_assert(convertible_from_string<char*>() == true, "");

    static_assert(convertible_from_string<std::unique_ptr<int>>() == false, "");
    static_assert(convertible_from_string<Comparable>() == false, "");
}
