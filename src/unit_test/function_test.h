/*
** SelfPortrait API
** See Copyright Notice in reflection.h
*/
#ifndef FUNCTION_TEST_H
#define FUNCTION_TEST_H

#include <cxxtest/TestSuite.h>

class FunctionTestSuite : public CxxTest::TestSuite
{
public:
	
	// test methods must begin with "test", otherwise cxxtestgen ignores them
	void testFunction();

	void testReturnByValue();
	void testReturnByReference();
	void testReturnByConstReference();
	void testParametersByValue();
	void testParametersByReference();
	void testParametersByConstReference();
	void testLuaAPI();
	void testLuaReturnByValue();
	void testLuaReturnByReference();
	void testLuaReturnByConstReference();
	void testLuaParameterByValue();
	void testLuaParameterByReference();
	void testLuaParameterByConstReference();
	void testFunctionHash();
};


#endif /* FUNCTION_TEST_H */
