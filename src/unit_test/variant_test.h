#ifndef VARIANT_TEST_H
#define VARIANT_TEST_H

#include "variant.h"
#include <cxxtest/TestSuite.h>

class VariantTestSuite : public CxxTest::TestSuite
{
public:
	
	void testInvalid();
	void testValue();
	void testConversions();	
};


#endif /* VARIANT_TEST_H */