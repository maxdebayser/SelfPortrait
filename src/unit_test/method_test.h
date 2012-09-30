#ifndef METHOD_TEST_H
#define METHOD_TEST_H

#include <cxxtest/TestSuite.h>

class MethodTestSuite : public CxxTest::TestSuite
{
public:
	
	// test methods must begin with "test", otherwise cxxtestgen ignores them

	void testNonCVMethod();
	void testCMethod();
	void testVMethod();
	void testCVMethod();
	void testStaticMethod();
	void testLuaAPI();
	void testMethodHash();
	void testClassRef();
};


#endif /* METHOD_TEST_H */
