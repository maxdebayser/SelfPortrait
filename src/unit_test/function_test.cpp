#include "function_test.h"
#include "function.h"
#include "reflection.h"
#include "reflection_impl.h"

#include "test_utils.h"
#include "str_utils.h"

#include <lua.hpp>

#include <iostream>
#include <string>
using namespace std;


namespace FunctionTest {

	double globalFunction(double arg1, double arg2) {
		return arg1 + arg2;
	}

	int globalFunction(int arg1, int arg2) {
		return arg1 + arg2;
	}

}

REFL_FUNCTION(FunctionTest::globalFunction, double, double, double)
REFL_FUNCTION(FunctionTest::globalFunction, int, int, int)


void FunctionTestSuite::testFunction()
{
	std::list<Function> functions = Function::findFunctions("FunctionTest::globalFunction");

	Function overload1;
	Function overload2;

	for (const Function& f: functions) {

		if (f.returnSpelling() == "double") {
			overload1 = f;
		} else if (f.returnSpelling() == "int") {
			overload2 = f;
		}
	}

	TS_ASSERT_EQUALS(overload1.name(), "FunctionTest::globalFunction" );

	TS_ASSERT( overload1.returnSpelling() == "double");
	WITH_RTTI(TS_ASSERT( overload1.returnType() == typeid(double) ));
	TS_ASSERT_EQUALS( overload1.numberOfArguments(), 2);

	VariantValue v1 = overload1.call(2.1,3.2);
	TS_ASSERT(v1.isValid());
	TS_ASSERT(v1.isA<double>());
	TS_ASSERT_DELTA(v1.value<double>(), 5.3, 0.0001);


	TS_ASSERT_EQUALS(overload2.name(), "FunctionTest::globalFunction" );
	TS_ASSERT( overload2.returnSpelling() == "int" );
	WITH_RTTI(TS_ASSERT( overload2.returnType() == typeid(int) ));

	VariantValue v2 = overload2.call(2.1, 3.2);
	TS_ASSERT(v2.isValid());
	TS_ASSERT(v2.isA<int>());
	TS_ASSERT_EQUALS(v2.value<int>(), 5);
}

namespace FunctionTest {

	class CopyCount {
	public:
		CopyCount(int id) : m_id(id), m_moved(false) {}
		CopyCount() : CopyCount(0) {}

		CopyCount(const CopyCount& rhs) : CopyCount(rhs.m_id) {
			++s_numCopies;
		}

		CopyCount(CopyCount&& rhs) : CopyCount(rhs.m_id) {
			++s_numMoves;
			rhs.m_moved = true;
		}

		CopyCount& operator=(const CopyCount& rhs) {
			m_id = rhs.m_id;
			++s_numCopies;
			return *this;
		}

		CopyCount& operator=(CopyCount&& rhs) {
			m_id = rhs.m_id;
			++s_numMoves;
			rhs.m_moved = true;
			return *this;
		}

		int id() const { return m_id; }

		void changeId(int newId) {
			m_id = newId;
		}

		static void resetCopyCount() {
			s_numCopies = 0;
		}
		static int numberOfCopies() {
			return s_numCopies;
		}
		static void resetMoveCount() {
			s_numMoves = 0;
		}
		static int numberOfMoves() {
			return s_numMoves;
		}
		static void resetAll() {
			resetCopyCount();
			resetMoveCount();
		}

		bool hasBeenMoved() {
			return m_moved;
		}

	private:
		static int s_numCopies;
		static int s_numMoves;
		int m_id;
		bool m_moved;
	};

	int CopyCount::s_numCopies = 0;
	int CopyCount::s_numMoves = 0;

	CopyCount returnObjectByValue() {
		return CopyCount(33);
	}

	CopyCount& returnObjectByReference() {
		static CopyCount instance(777);
		return instance;
	}

	const CopyCount& returnObjectByConstReference() {
		static CopyCount instance(888);
		return instance;
	}


	int paramByValue(CopyCount c) {
		return c.id();
	}

	int paramByReference(CopyCount& c) {
		c.changeId(c.id()+1);
		return c.id();
	}

	int paramByConstReference(const CopyCount& c) {
		return c.id();
	}
}


REFL_BEGIN_CLASS(FunctionTest::CopyCount)
	REFL_CONSTRUCTOR(int)
	REFL_CONST_METHOD(id, int)
	REFL_METHOD(changeId, void, int)
	REFL_METHOD(hasBeenMoved, bool)
	REFL_STATIC_METHOD(resetCopyCount, void)
	REFL_STATIC_METHOD(numberOfCopies, int)
	REFL_STATIC_METHOD(resetMoveCount, void)
	REFL_STATIC_METHOD(numberOfMoves, int)
	REFL_STATIC_METHOD(resetAll, void)
REFL_END_CLASS

REFL_FUNCTION(FunctionTest::returnObjectByValue, FunctionTest::CopyCount)
REFL_FUNCTION(FunctionTest::returnObjectByReference, FunctionTest::CopyCount&)
REFL_FUNCTION(FunctionTest::returnObjectByConstReference, const FunctionTest::CopyCount&)
REFL_FUNCTION(FunctionTest::paramByValue, int, FunctionTest::CopyCount)
REFL_FUNCTION(FunctionTest::paramByReference, int, FunctionTest::CopyCount&)
REFL_FUNCTION(FunctionTest::paramByConstReference, int, const FunctionTest::CopyCount&)

using namespace FunctionTest;

void FunctionTestSuite::testReturnByValue()
{
	auto f = make_function<CopyCount (*)()>(&function_type<CopyCount (*)()>::bindcall<&returnObjectByValue>, "returnObjectByValue", "CopyCount", "");

	CopyCount::resetAll();

	CopyCount c = returnObjectByValue();

	TS_ASSERT_EQUALS(c.id(), 33);

	int directCallCopies = CopyCount::numberOfCopies();
	CopyCount::resetAll();

	VariantValue v = f.call();

	int reflectionCopies = CopyCount::numberOfCopies();

	TS_ASSERT(v.isValid());

	CopyCount& ref = v.convertTo<CopyCount&>();

	TS_ASSERT_EQUALS(ref.id(),33);

	TS_ASSERT_EQUALS(directCallCopies, reflectionCopies);
}

void FunctionTestSuite::testReturnByReference()
{
	auto f = make_function<CopyCount& (*)()>(&function_type<CopyCount& (*)()>::bindcall<&returnObjectByReference>, "returnObjectByReference", "CopyCount &", "");
	CopyCount::resetAll();
	VariantValue v = f.call();
	CopyCount& ref = v.convertTo<CopyCount&>();

	TS_ASSERT_EQUALS(ref.id(), 777);
	ref.changeId(888);
	TS_ASSERT_EQUALS(returnObjectByReference().id(), 888);
	TS_ASSERT_EQUALS(CopyCount::numberOfCopies(), 0);
	TS_ASSERT(!returnObjectByReference().hasBeenMoved())
}

void FunctionTestSuite::testReturnByConstReference()
{
	auto f = make_function<const CopyCount& (*)()>(&function_type<const CopyCount& (*)()>::bindcall<&returnObjectByConstReference>, "returnObjectByConstReference", "const CopyCount &", "");
	CopyCount::resetAll();
	VariantValue v = f.call();
	TS_ASSERT(v.isValid());
	bool ok = false;
	CopyCount& ref = v.convertTo<CopyCount&>(&ok);
	TS_ASSERT(!ok);

	const CopyCount& cref = v.convertTo<const CopyCount&>(&ok);
	TS_ASSERT(ok);

	TS_ASSERT_EQUALS(CopyCount::numberOfCopies(), 0);

	TS_ASSERT_EQUALS(cref.id(), 888);
	TS_ASSERT_EQUALS(&cref, &returnObjectByConstReference());
}

void FunctionTestSuite::testParametersByValue()
{
	auto f = make_function<int (*)(CopyCount)>(&function_type<int (*)(CopyCount)>::bindcall<&paramByValue>, "paramByValue", "int", "CopyCount");
	CopyCount::resetAll();

	CopyCount c(77);

	TS_ASSERT_EQUALS(paramByValue(c), 77);

	int directCallCopies = CopyCount::numberOfCopies();
	int directCallMoves = CopyCount::numberOfMoves();
	TS_ASSERT_EQUALS(directCallCopies, 1);
	TS_ASSERT_EQUALS(directCallMoves,  0);

	CopyCount::resetAll();

	VariantValue v = f.call(c);

	TS_ASSERT(v.isValid());
	TS_ASSERT_EQUALS(v.value<int>(), 77);
	int reflectionCopies = CopyCount::numberOfCopies();
	int reflectionMoves = CopyCount::numberOfMoves();

	TS_ASSERT_EQUALS(reflectionCopies, 1);
	TS_ASSERT_EQUALS(reflectionMoves,  2);
	TS_ASSERT(!c.hasBeenMoved());
}


void FunctionTestSuite::testParametersByReference()
{
	auto f = make_function<int (*)(CopyCount&)>(&function_type<int (*)(CopyCount&)>::bindcall<&paramByReference>, "paramByReference", "int", "CopyCount&");

	CopyCount::resetAll();

	CopyCount c(44);

	TS_ASSERT_EQUALS(paramByReference(c), 45);
	TS_ASSERT_EQUALS(c.id(), 45);
	TS_ASSERT_EQUALS(CopyCount::numberOfCopies(), 0);

	VariantValue param;
	param.construct<CopyCount&>(c);

	VariantValue r = f.call(param);

	TS_ASSERT_EQUALS(CopyCount::numberOfCopies(), 0);
	TS_ASSERT(r.isValid());
	TS_ASSERT_EQUALS(r.value<int>(), 46);
	TS_ASSERT_EQUALS(c.id(), 46);
}


void FunctionTestSuite::testParametersByConstReference()
{
	auto f = make_function<int (*)(const CopyCount&)>(&function_type<int (*)(const CopyCount&)>::bindcall<&paramByConstReference>, "paramByConstReference", "int", "const CopyCount&");

	CopyCount::resetAll();

	CopyCount c(44);

	TS_ASSERT_EQUALS(paramByConstReference(c), 44);
	TS_ASSERT_EQUALS(c.id(), 44);
	TS_ASSERT_EQUALS(CopyCount::numberOfCopies(), 0);

	VariantValue param;
	param.construct<CopyCount&>(c);

	VariantValue r = f.call(param);

	TS_ASSERT_EQUALS(CopyCount::numberOfCopies(), 0);
	TS_ASSERT(r.isValid());
	TS_ASSERT_EQUALS(r.value<int>(), 44);
	TS_ASSERT_EQUALS(c.id(), 44);
}



void FunctionTestSuite::testLuaAPI()
{
	LuaUtils::LuaStateHolder L;

	if (luaL_loadfile(L, fmt_str("%1/function_test.lua", srcpath()).c_str()) || lua_pcall(L,0,0,0)) {
		luaL_error(L, "cannot run config file: %s\n", lua_tostring(L, -1));
	}
	LuaUtils::callFunc<bool>(L, "testFunction");
}

void FunctionTestSuite::testLuaReturnByValue()
{
	LuaUtils::LuaStateHolder L;

	if (luaL_loadfile(L, fmt_str("%1/function_test.lua", srcpath()).c_str()) || lua_pcall(L,0,0,0)) {
		luaL_error(L, "cannot run config file: %s\n", lua_tostring(L, -1));
	}
	LuaUtils::callFunc<bool>(L, "testReturnByValue");
}

void FunctionTestSuite::testLuaReturnByReference()
{
	LuaUtils::LuaStateHolder L;

	if (luaL_loadfile(L, fmt_str("%1/function_test.lua", srcpath()).c_str()) || lua_pcall(L,0,0,0)) {
		luaL_error(L, "cannot run config file: %s\n", lua_tostring(L, -1));
	}
	LuaUtils::callFunc<bool>(L, "testReturnByReference");
}

void FunctionTestSuite::testLuaReturnByConstReference()
{
	LuaUtils::LuaStateHolder L;

	if (luaL_loadfile(L, fmt_str("%1/function_test.lua", srcpath()).c_str()) || lua_pcall(L,0,0,0)) {
		luaL_error(L, "cannot run config file: %s\n", lua_tostring(L, -1));
	}
	LuaUtils::callFunc<bool>(L, "testReturnByConstReference");
}

void FunctionTestSuite::testLuaParameterByValue()
{
	LuaUtils::LuaStateHolder L;

	if (luaL_loadfile(L, fmt_str("%1/function_test.lua", srcpath()).c_str()) || lua_pcall(L,0,0,0)) {
		luaL_error(L, "cannot run config file: %s\n", lua_tostring(L, -1));
	}
	LuaUtils::callFunc<bool>(L, "testParameterByValue");
}

void FunctionTestSuite::testLuaParameterByReference()
{
	LuaUtils::LuaStateHolder L;

	if (luaL_loadfile(L, fmt_str("%1/function_test.lua", srcpath()).c_str()) || lua_pcall(L,0,0,0)) {
		luaL_error(L, "cannot run config file: %s\n", lua_tostring(L, -1));
	}
	LuaUtils::callFunc<bool>(L, "testParameterByReference");
}

void FunctionTestSuite::testLuaParameterByConstReference()
{
	LuaUtils::LuaStateHolder L;

	if (luaL_loadfile(L, fmt_str("%1/function_test.lua", srcpath()).c_str()) || lua_pcall(L,0,0,0)) {
		luaL_error(L, "cannot run config file: %s\n", lua_tostring(L, -1));
	}
	LuaUtils::callFunc<bool>(L, "testParameterByConstReference");
}


void FunctionTestSuite::testFunctionHash()
{
	using namespace std;
	unordered_map<Function, int> fmap;

	std::list<Function> functions = Function::findFunctions("FunctionTest::globalFunction");
	auto it = functions.begin();

	Function f1 = *it++;
	Function f2 = *it++;

	fmap[f1] = 1;
	fmap[f2] = 2;

	TS_ASSERT_EQUALS(fmap[f1], 1);
	TS_ASSERT_EQUALS(fmap[f2], 2);
}
