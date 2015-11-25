/*
** SelfPortrait API
** See Copyright Notice in reflection.h
*/
#ifndef PROXY_TEST_H
#define PROXY_TEST_H

#include <cxxtest/TestSuite.h>

class ProxyTestSuite : public CxxTest::TestSuite
{
public:

	// test methods must begin with "test", otherwise cxxtestgen ignores them

	void testProxy();
    void testVoidProxy();
	void testClient();
	void testLuaAPI();
};


#endif /* PROXY_TEST_H */
