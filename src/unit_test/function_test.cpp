#include "function_test.h"
#include "function.h"
#include "reflection.h"
#include "reflection_impl.h"

#include <iostream>
#include <string>
using namespace std;


namespace Test {

	double globalFunction(double arg1, double arg2) {
		return arg1 + arg2;
	}

	int globalFunction(int arg1, int arg2) {
		return arg1 + arg2;
	}

}

FUNCTION(Test::globalFunction, double, double, double)
FUNCTION(Test::globalFunction, int, int, int)


void FunctionTestSuite::testFunction()
{
	std::list<Function> functions = Function::findFunctions("Test::globalFunction");

	Function overload1;
	Function overload2;

	for (const Function& f: functions) {

		if (f.returnType() == typeid(double)) {
			overload1 = f;
		} else if (f.returnType() == typeid(int)) {
			overload2 = f;
		}

	}

	TS_ASSERT_EQUALS(overload1.name(), "Test::globalFunction" );

	TS_ASSERT( overload1.returnType() == typeid(double) );
	TS_ASSERT_EQUALS( overload1.numberOfArguments(), 2);

	VariantValue v1 = overload1.call(2.1,3.2);
	TS_ASSERT(v1.isValid());
	TS_ASSERT(v1.isA<double>());
	TS_ASSERT_DELTA(v1.value<double>(), 5.3, 0.0001);


	TS_ASSERT_EQUALS(overload2.name(), "Test::globalFunction" );
	TS_ASSERT( overload2.returnType() == typeid(int) );

	VariantValue v2 = overload2.call(2.1, 3.2);
	TS_ASSERT(v2.isValid());
	TS_ASSERT(v2.isA<int>());
	TS_ASSERT_EQUALS(v2.value<int>(), 5);

}
