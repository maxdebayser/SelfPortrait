#include "SmallArray_test.h"

#include "SmallArray.h"

void SmallArrayTestSuite::testConstruction()
{
    SmallArray<int> s;
    TS_ASSERT_EQUALS(s.size(), 0);

    s.push_back(2);
    TS_ASSERT_EQUALS(s.size(), 1);
    TS_ASSERT_EQUALS(s[0], 2);

    s.push_back(3);
    TS_ASSERT_EQUALS(s.size(), 2);
    TS_ASSERT_EQUALS(s[1], 3);

    s.push_back(4);
    TS_ASSERT_EQUALS(s.size(), 3);
    TS_ASSERT_EQUALS(s[2], 4);

    s.push_back(5);
    TS_ASSERT_EQUALS(s.size(), 4);
    TS_ASSERT_EQUALS(s[3], 5);

    s.push_back(6);
    TS_ASSERT_EQUALS(s.size(), 5);
    TS_ASSERT_EQUALS(s[4], 6);

    s.push_back(7);
    TS_ASSERT_EQUALS(s.size(), 6);
    TS_ASSERT_EQUALS(s[5], 7);

    s.push_back(8);
    TS_ASSERT_EQUALS(s.size(), 7);
    TS_ASSERT_EQUALS(s[6], 8);
}

namespace {

struct Test {
    int val;

    Test(int i) : val(i) {}

    Test(const Test&) = delete;
    Test& operator=(const Test&) = delete;
    Test(Test&&) = delete;
    Test& operator=(Test&&) = delete;

};

}

void SmallArrayTestSuite::testEmplace()
{
    SmallArray<Test> s;
    TS_ASSERT_EQUALS(s.size(), 0);

    s.emplace_back(2);
    TS_ASSERT_EQUALS(s.size(), 1);
    TS_ASSERT_EQUALS(s[0].val, 2);

    s.emplace_back(3);
    TS_ASSERT_EQUALS(s.size(), 2);
    TS_ASSERT_EQUALS(s[1].val, 3);

    s.emplace_back(4);
    TS_ASSERT_EQUALS(s.size(), 3);
    TS_ASSERT_EQUALS(s[2].val, 4);

    s.emplace_back(5);
    TS_ASSERT_EQUALS(s.size(), 4);
    TS_ASSERT_EQUALS(s[3].val, 5);

    s.emplace_back(6);
    TS_ASSERT_EQUALS(s.size(), 5);
    TS_ASSERT_EQUALS(s[4].val, 6);

    s.emplace_back(7);
    TS_ASSERT_EQUALS(s.size(), 6);
    TS_ASSERT_EQUALS(s[5].val, 7);

    s.emplace_back(8);
    TS_ASSERT_EQUALS(s.size(), 7);
    TS_ASSERT_EQUALS(s[6].val, 8);
}


void SmallArrayTestSuite::testInitializerList()
{
    SmallArray<int> s = {2, 3, 4, 5, 6, 7, 8};

    TS_ASSERT_EQUALS(s[0], 2);
    TS_ASSERT_EQUALS(s[1], 3);
    TS_ASSERT_EQUALS(s[2], 4);
    TS_ASSERT_EQUALS(s[3], 5);
    TS_ASSERT_EQUALS(s[4], 6);
    TS_ASSERT_EQUALS(s[5], 7);
    TS_ASSERT_EQUALS(s[6], 8);
    TS_ASSERT_EQUALS(s.size(), 7);
}

void SmallArrayTestSuite::testIterators()
{
    SmallArray<int> s = {2, 3, 4, 5, 6, 7, 8};

    auto it = s.begin();
    auto end = s.end();
    TS_ASSERT(it != end);

    TS_ASSERT_EQUALS(*it++, 2);
    TS_ASSERT_EQUALS(*it++, 3);
    TS_ASSERT_EQUALS(*it++, 4);
    TS_ASSERT_EQUALS(*it++, 5);
    TS_ASSERT_EQUALS(*it++, 6);
    TS_ASSERT_EQUALS(*it++, 7);
    TS_ASSERT_EQUALS(*it++, 8);
    TS_ASSERT(it == end);

    it = s.begin();

    TS_ASSERT_EQUALS(*it, 2);
    TS_ASSERT_EQUALS(*++it, 3);
    TS_ASSERT_EQUALS(*++it, 4);
    TS_ASSERT_EQUALS(*++it, 5);
    TS_ASSERT_EQUALS(*++it, 6);
    TS_ASSERT_EQUALS(*++it, 7);
    TS_ASSERT_EQUALS(*++it, 8);
    ++it;
    TS_ASSERT(it == end);
}
