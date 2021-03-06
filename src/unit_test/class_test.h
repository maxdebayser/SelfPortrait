/*
** SelfPortrait API
** See Copyright Notice in reflection.h
*/
#ifndef CLASS_TEST_H
#define CLASS_TEST_H

#include <cxxtest/TestSuite.h>

class ClassTestSuite : public CxxTest::TestSuite
{
public:
	
	// test methods must begin with "test", otherwise cxxtestgen ignores them
	void testClass();
	void testOverload();
	void testLuaAPI();
	void testClassHash();
	void testMethodSearch();
	void testAttributeSearch();
	void testConstructorSearch();
	void testSuperClassSearch();
	void testPrivateDestructor();
};


#endif /* CLASS_TEST_H */
