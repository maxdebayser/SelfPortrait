#ifndef UTILITIES_TEST_H
#define UTILITIES_TEST_H

#include <cxxtest/TestSuite.h>

class UtilitiesTestSuite : public CxxTest::TestSuite
{
public:
	
	// test methods must begin with "test", otherwise cxxtestgen ignores them

	void testTypelist();
	void testConversionsToStr();
	void testConversionsFromStr();
};


#endif /* UTILITIES_TEST_H */
