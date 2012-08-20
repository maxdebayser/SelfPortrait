#include "constructor_test.h"
#include "constructor.h"
#include "reflection.h"
#include "reflection_impl.h"

#include "test_utils.h"
#include "str_utils.h"

#include <lua.hpp>

#include <iostream>
#include <string>
using namespace std;

namespace ConstructorTest {

	class Test {
	public:
		Test()
			: attr1(101)
			, attr2(102)
		{}

		Test(int arg1)
			: attr1(arg1)
			, attr2(103)
		{}

		Test(int arg1, int arg2)
			: attr1(arg1)
			, attr2(arg2)
		{}

		Test(const Test& t)
			: attr1(t.attr1)
			, attr2(t.attr2)
		{}

		int attr1;
		int attr2;
	};
}

REFL_BEGIN_CLASS(ConstructorTest::Test)
	REFL_ATTRIBUTE(attr1, int)
	REFL_ATTRIBUTE(attr2, int)
	REFL_DEFAULT_CONSTRUCTOR()
	REFL_CONSTRUCTOR(int)
	REFL_CONSTRUCTOR(int, int)
	REFL_CONSTRUCTOR(const ConstructorTest::Test&)
REFL_END_CLASS

using namespace ConstructorTest;

void ConstructorTestSuite::testConstruction()
{
	auto defaultConstructor = make_constructor<Test>("");
	auto constructor1Args = make_constructor<Test, int>("int");
	auto constructor2Args = make_constructor<Test, int, int>("int,int");
	auto copyConstructor = make_constructor<Test, const Test&>("const Test&");

	bool success = false;

	VariantValue v1 = defaultConstructor.call();

	TS_ASSERT(v1.isValid());
	WITH_RTTI(TS_ASSERT(v1.typeId() == typeid(Test)));

	Test& t1 = v1.convertTo<Test&>(&success);
	TS_ASSERT(success);
	TS_ASSERT_EQUALS(t1.attr1, 101);
	TS_ASSERT_EQUALS(t1.attr2, 102);



	VariantValue v2 = constructor1Args.call(13);
	TS_ASSERT_THROWS(constructor1Args.call(), std::runtime_error);
	TS_ASSERT(v2.isValid());
	WITH_RTTI(TS_ASSERT(v2.typeId() == typeid(Test)));

	Test& t2 = v2.convertTo<Test&>(&success);
	TS_ASSERT(success);
	TS_ASSERT_EQUALS(t2.attr1, 13);
	TS_ASSERT_EQUALS(t2.attr2, 103);


	VariantValue v3 = constructor2Args.call(13,17);
	TS_ASSERT(v3.isValid());
	WITH_RTTI(TS_ASSERT(v3.typeId() == typeid(Test)));

	Test& t3 = v3.convertTo<Test&>(&success);
	TS_ASSERT(success);
	TS_ASSERT_EQUALS(t3.attr1, 13);
	TS_ASSERT_EQUALS(t3.attr2, 17);

	VariantValue v4 = copyConstructor.call(v3);
	TS_ASSERT(v4.isValid());
	WITH_RTTI(TS_ASSERT(v4.typeId() == typeid(Test)));

	Test& t4 = v4.convertTo<Test&>(&success);
	TS_ASSERT(success);
	TS_ASSERT_EQUALS(t4.attr1, 13);
	TS_ASSERT_EQUALS(t4.attr2, 17);
}


void ConstructorTestSuite::testLuaAPI()
{
	LuaUtils::LuaStateHolder L;

	if (luaL_loadfile(L, "constructor_test.lua") || lua_pcall(L,0,0,0)) {
		luaL_error(L, "cannot run config file: %s\n", lua_tostring(L, -1));
	}
	LuaUtils::callFunc<bool>(L, "testConstructor");
}
