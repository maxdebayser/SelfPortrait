/*
** SelfPortrait API
** See Copyright Notice in reflection.h
*/
#ifndef SMALL_ARRAY_TEST_H
#define SMALL_ARRAY_TEST_H

#include <cxxtest/TestSuite.h>

class SmallArrayTestSuite : public CxxTest::TestSuite
{
public:

    // test methods must begin with "test", otherwise cxxtestgen ignores them

    void testConstruction();
    void testEmplace();
    void testInitializerList();
    void testIterators();
};


#endif /* SMALL_ARRAY_TEST_H */
