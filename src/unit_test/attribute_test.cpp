/*
** SelfPortrait API
** See Copyright Notice in reflection.h
*/
#include "attribute_test.h"
#include "attribute.h"
#include "reflection.h"

#include "reflection_impl.h"

#include "test_utils.h"
#include "str_utils.h"

#include <lua.hpp>

#include <iostream>
#include <string>
using namespace std;


namespace AttributeTest {

	class Test {
	public:

		int attr1;
		const int attr2;
		static int attr3;
		static const int attr4;

		int& attr5;

		Test() : attr1(101), attr2(102), attr5(attr1) {}

	};

	int Test::attr3 = 103;
	const int Test::attr4 = 104;

	class Test2 {};

    class Test3 {
    public:
        std::unique_ptr<int> attr1;
    };

}

REFL_BEGIN_CLASS(AttributeTest::Test)
	REFL_ATTRIBUTE(attr1, int)
	REFL_ATTRIBUTE(attr2, const int)
	REFL_STATIC_ATTRIBUTE(attr3, int)
	REFL_STATIC_ATTRIBUTE(attr4, const int)
	REFL_DEFAULT_CONSTRUCTOR()
REFL_END_CLASS


REFL_BEGIN_CLASS(AttributeTest::Test2)
REFL_END_CLASS


REFL_BEGIN_CLASS(AttributeTest::Test3)
REFL_ATTRIBUTE(attr1, std::unique_ptr<int>)
REFL_END_CLASS

using namespace AttributeTest;

void AttributeTestSuite::testVanillaAttribute()
{
	AttributeImpl<int Test::*> aImpl("attr1", &Test::attr1, "int");
	Attribute attr1(&aImpl);

    VariantValue v1 = Test();
    VariantValue r1 = attr1.get(v1);

    // TODO: check if it is possible to implement a conversion to int (calling the copy constructor)
    TS_ASSERT(r1.isA<const int&>());
    TS_ASSERT_EQUALS(r1.value<const int&>(), 101);

	attr1.set(v1, 201);
	r1 = attr1.get(v1);
    TS_ASSERT(r1.isA<const int&>());
    TS_ASSERT_EQUALS(r1.value<const int&>(), 201);

	TS_ASSERT_THROWS(attr1.get(), std::runtime_error);
	TS_ASSERT_THROWS(attr1.set(201), std::runtime_error);

	const VariantValue v2 = Test();
	VariantValue r2 = attr1.get(v2);

    TS_ASSERT(r2.isA<const int&>());
    TS_ASSERT_EQUALS(r2.value<const int&>(), 101);

	TS_ASSERT_THROWS(attr1.set(v2, 201), std::runtime_error);
}


void AttributeTestSuite::testConstAttribute()
{
	AttributeImpl<const int Test::*> aImpl("attr2", &Test::attr2, "const int");
	Attribute attr2(&aImpl);

	VariantValue v1 = Test();
	VariantValue r1 = attr2.get(v1);

    TS_ASSERT(r1.isA<const int&>());
    TS_ASSERT_EQUALS(r1.value<const int&>(), 102);
	TS_ASSERT_THROWS(attr2.set(v1, 202), std::runtime_error);

	const VariantValue v2 = Test();
	VariantValue r2 = attr2.get(v2);
    TS_ASSERT(r2.isA<const int&>());
    TS_ASSERT_EQUALS(r2.value<const int&>(), 102);

	TS_ASSERT_THROWS(attr2.set(v2, 202), std::runtime_error);
}


void AttributeTestSuite::testStaticAttribute()
{
	StaticAttributeImpl<Test, int*> aImpl3("attr3", &Test::attr3, "int");
	Attribute attr3(&aImpl3);

	VariantValue r1 = attr3.get();

	TS_ASSERT(r1.isA<int>());
	TS_ASSERT_EQUALS(r1.value<int>(), 103);

	attr3.set(201);
	r1 = attr3.get();
	TS_ASSERT(r1.isA<int>());
	TS_ASSERT_EQUALS(r1.value<int>(), 201);

	// reset
	Test::attr3 = 103;

	StaticAttributeImpl<Test, const int*> aImpl4("attr4", &Test::attr4, "const int");
	Attribute attr4(&aImpl4);

	VariantValue r2 = attr4.get();

	TS_ASSERT(r2.isA<int>());
	TS_ASSERT_EQUALS(r2.value<int>(), 104);

	TS_ASSERT_THROWS(attr4.set(202), std::runtime_error);
}



void AttributeTestSuite::testLuaAPI()
{
	LuaUtils::LuaStateHolder L;
    LuaUtils::addTestFunctionsAndPaths(&*L);

    const int errIndex = LuaUtils::pushTraceBack(L);
    if (luaL_loadfile(L, strconv::fmt_str("%1/attribute_test.lua", srcpath()).c_str()) || lua_pcall(L,0,0,errIndex)) {
		luaL_error(L, "cannot run config file: %s\n", lua_tostring(L, -1));
	}
    LuaUtils::removeTraceBack(L, errIndex);
	LuaUtils::callFunc<bool>(L, "testAttribute");
}


void AttributeTestSuite::testHash()
{
	using namespace std;
	unordered_map<Attribute, int> amap;

	Class test = Class::lookup("AttributeTest::Test");
	auto it = test.attributes().begin();
	Attribute a1 = *it++;
	Attribute a2 = *it++;
	Attribute a3 = *it++;

	amap[a1] = 1;
	amap[a2] = 2;
	amap[a3] = 3;

	TS_ASSERT_EQUALS(amap[a1], 1);
	TS_ASSERT_EQUALS(amap[a2], 2);
	TS_ASSERT_EQUALS(amap[a3], 3);
}


void AttributeTestSuite::testClassRef()
{
	Class test = Class::lookup("AttributeTest::Test");
	Class test2 = Class::lookup("AttributeTest::Test2");
	auto it = test.attributes().begin();
	Attribute a1 = *it++;
	Attribute a2 = *it++;
	Attribute a3 = *it++;

	TS_ASSERT_EQUALS(a1.getClass(), test);
	TS_ASSERT_EQUALS(a2.getClass(), test);
	TS_ASSERT_EQUALS(a3.getClass(), test);

	TS_ASSERT_EQUALS(a1.getClass(), a2.getClass());

	TS_ASSERT_DIFFERS(a1.getClass(), test2);
}

void AttributeTestSuite::testNonAssignableAttribute()
{
    static_assert(std::is_copy_assignable<std::unique_ptr<int>>::value == false, "unique_ptr should not be assignable");
}
