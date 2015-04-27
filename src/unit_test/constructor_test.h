/*
** SelfPortrait API
** See Copyright Notice in reflection.h
*/
#ifndef CONSTRUCTOR_TEST_H
#define CONSTRUCTOR_TEST_H

#include <cxxtest/TestSuite.h>

class ConstructorTestSuite : public CxxTest::TestSuite
{
public:
	
	// test methods must begin with "test", otherwise cxxtestgen ignores them

	void testConstruction();
	void testLuaAPI();
	void testConstructorHash();
	void testClassRef();

    void testCaseGregorian();
};


#endif /* CONSTRUCTOR_TEST_H */
