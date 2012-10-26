#ifndef FUNCTIONS_H
#define FUNCTIONS_H

/* This is not actually a source file. Its an input file
 * for the parsers' unit test
 *
 * This is a very simple test without other includes.
 */

void function1();

int function2(double a1);

int function3(double a1, float* a2);

char* function4(double a1, float* a2, const long& a3);

// overload
char* function4(double a1, float* a2, const long& a3);

int function5(int argc, char* argv[]);

namespace NS {

	// test functions in a namespace
	int function2(double a1);

	namespace NestedNS {
		int function2(double a1);
	}
}

#endif
