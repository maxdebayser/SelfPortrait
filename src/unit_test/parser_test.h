/*
** SelfPortrait API
** See Copyright Notice in reflection.h
*/
#ifndef PARSER_TEST_H
#define PARSER_TEST_H

#include <cxxtest/TestSuite.h>

#include <string>

/* parser regression tests
 *
 * This test runs the parser on a number of predefined
 * header files and compares it with the expected output.
 *
 */


class ParserTestSuite : public CxxTest::TestSuite
{
public:

	// test methods must begin with "test", otherwise cxxtestgen ignores them

	void testFunctions();
	void testSimpleClass();
	void testInheritance();
	void testInterface();
	void testTemplates();
};


#endif /* PARSER_TEST_H */
