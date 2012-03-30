#ifndef ATTRIBUTE_TEST_H
#define ATTRIBUTE_TEST_H

#include <cxxtest/TestSuite.h>

class AttributeTestSuite : public CxxTest::TestSuite
{
public:
	
	// test methods must begin with "test", otherwise cxxtestgen ignores them

	void testVanillaAttribute();
	void testConstAttribute();
	void testStaticAttribute();
};


#endif /* ATTRIBUTE_TEST_H */
