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

}

REFL_BEGIN_CLASS(AttributeTest::Test)
	REFL_ATTRIBUTE(attr1, int)
	REFL_ATTRIBUTE(attr2, const int)
	REFL_STATIC_ATTRIBUTE(attr3, int)
	REFL_STATIC_ATTRIBUTE(attr4, const int)
	REFL_DEFAULT_CONSTRUCTOR()
REFL_END_CLASS

using namespace AttributeTest;

void AttributeTestSuite::testVanillaAttribute()
{
	AttributeImpl<int Test::*> aImpl("attr1", &Test::attr1, "int");
	Attribute attr1(&aImpl);

	VariantValue v1 = Test();
	VariantValue r1 = attr1.get(v1);

	TS_ASSERT(r1.isA<int>());
	TS_ASSERT_EQUALS(r1.value<int>(), 101);

	attr1.set(v1, 201);
	r1 = attr1.get(v1);
	TS_ASSERT(r1.isA<int>());
	TS_ASSERT_EQUALS(r1.value<int>(), 201);

	TS_ASSERT_THROWS(attr1.get(), std::runtime_error);
	TS_ASSERT_THROWS(attr1.set(201), std::runtime_error);

	const VariantValue v2 = Test();
	VariantValue r2 = attr1.get(v2);

	TS_ASSERT(r2.isA<int>());
	TS_ASSERT_EQUALS(r2.value<int>(), 101);

	TS_ASSERT_THROWS(attr1.set(v2, 201), std::runtime_error);
}


void AttributeTestSuite::testConstAttribute()
{
	AttributeImpl<const int Test::*> aImpl("attr2", &Test::attr2, "const int");
	Attribute attr2(&aImpl);

	VariantValue v1 = Test();
	VariantValue r1 = attr2.get(v1);

	TS_ASSERT(r1.isA<int>());
	TS_ASSERT_EQUALS(r1.value<int>(), 102);
	TS_ASSERT_THROWS(attr2.set(v1, 202), std::runtime_error);

	const VariantValue v2 = Test();
	VariantValue r2 = attr2.get(v2);
	TS_ASSERT(r2.isA<int>());
	TS_ASSERT_EQUALS(r2.value<int>(), 102);

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

	if (luaL_loadfile(L, "attribute_test.lua") || lua_pcall(L,0,0,0)) {
		luaL_error(L, "cannot run config file: %s\n", lua_tostring(L, -1));
	}

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
