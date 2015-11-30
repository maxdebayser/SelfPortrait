/*
** SelfPortrait API
** See Copyright Notice in reflection.h
*/
#include "constructor_test.h"
#include "constructor.h"
#include "reflection.h"
#include "reflection_impl.h"

#include "test_utils.h"
#include "str_utils.h"

#include "boost/date_time/posix_time/posix_time.hpp"

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

	class Test2 {};

    class Test3 {
    public:
        Test3(const Test3&) = delete;
        Test3(Test3&& t) {
            this->attr1 = t.attr1;
        }
        Test3(int arg1)
            : attr1(arg1)
        {}
        int attr1;
    };


    class Test4 {
    public:
        boost::gregorian::date d;

        Test4(int year, int month, int day) : d(year, month, day) {}
        Test4(boost::gregorian::date _d) : d(_d) {}
    };

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

    struct TestConsParamPassing1 {
        int id;
        TestConsParamPassing1(CopyCount c) {
            c.changeId(c.id()+1);
            id = c.id();
        }
    };
    struct TestConsParamPassing2 {
        int id;
        TestConsParamPassing2(CopyCount& c) {
            c.changeId(c.id()+1);
            id = c.id();
        }
    };struct TestConsParamPassing3 {
        int id;
        TestConsParamPassing3(const CopyCount& c) : id(c.id()) {}
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


REFL_BEGIN_CLASS(ConstructorTest::Test2)
REFL_END_CLASS

REFL_BEGIN_CLASS(ConstructorTest::Test3)
REFL_CONSTRUCTOR(ConstructorTest::Test3&&)
REFL_CONSTRUCTOR(int)
REFL_ATTRIBUTE(attr1, int)
REFL_END_CLASS

REFL_BEGIN_CLASS(ConstructorTest::Test4)
REFL_CONSTRUCTOR(boost::gregorian::date)
REFL_CONSTRUCTOR(int, int, int)
REFL_ATTRIBUTE(d, boost::gregorian::date)
REFL_END_CLASS


REFL_BEGIN_CLASS(ConstructorTest::CopyCount)
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

REFL_BEGIN_CLASS(ConstructorTest::TestConsParamPassing1)
    REFL_ATTRIBUTE(id, int)
    REFL_CONSTRUCTOR(ConstructorTest::CopyCount)
REFL_END_CLASS


REFL_BEGIN_CLASS(ConstructorTest::TestConsParamPassing2)
    REFL_ATTRIBUTE(id, int)
    REFL_CONSTRUCTOR(ConstructorTest::CopyCount&)
REFL_END_CLASS

REFL_BEGIN_CLASS(ConstructorTest::TestConsParamPassing3)
    REFL_ATTRIBUTE(id, int)
    REFL_CONSTRUCTOR(const ConstructorTest::CopyCount&)
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

    VariantValue v4 = copyConstructor.call(v3.convertTo<Test&>());
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
    LuaUtils::addTestFunctionsAndPaths(&*L);

    const int errIndex = LuaUtils::pushTraceBack(L);
    if (luaL_loadfile(L, strconv::fmt_str("%1/constructor_test.lua", srcpath()).c_str()) || lua_pcall(L,0,0,errIndex)) {
		luaL_error(L, "cannot run config file: %s\n", lua_tostring(L, -1));
	}
    LuaUtils::removeTraceBack(L, errIndex);
	LuaUtils::callFunc<bool>(L, "testConstructor");
}


void ConstructorTestSuite::testConstructorHash()
{
	using namespace std;
	unordered_map<Constructor, int> cmap;

	Class test = Class::lookup("ConstructorTest::Test");
	auto it = test.constructors().begin();
	Constructor c1 = *it++;
	Constructor c2 = *it++;
	Constructor c3 = *it++;

	cmap[c1] = 1;
	cmap[c2] = 2;
	cmap[c3] = 3;

	TS_ASSERT_EQUALS(cmap[c1], 1);
	TS_ASSERT_EQUALS(cmap[c2], 2);
	TS_ASSERT_EQUALS(cmap[c3], 3);
}


void ConstructorTestSuite::testClassRef()
{
	Class test = Class::lookup("ConstructorTest::Test");
	Class test2 = Class::lookup("ConstructorTest::Test2");
	auto it = test.constructors().begin();
	Constructor c1 = *it++;
	Constructor c2 = *it++;
	Constructor c3 = *it++;

	TS_ASSERT_EQUALS(c1.getClass(), test);
	TS_ASSERT_EQUALS(c2.getClass(), test);
	TS_ASSERT_EQUALS(c3.getClass(), test);

	TS_ASSERT_EQUALS(c1.getClass(), c2.getClass());

	TS_ASSERT_DIFFERS(c1.getClass(), test2);
}


void ConstructorTestSuite::testCaseGregorian()
{
    Class test4 = Class::lookup("ConstructorTest::Test4");

    boost::gregorian::date d(2005, 1, 15);

    auto it = test4.constructors().begin();
    Constructor c1 = *it++;
    Constructor c2 = *it++;

    TS_ASSERT_EQUALS(c1.getClass(), test4);
    TS_ASSERT_EQUALS(c2.getClass(), test4);

    TS_ASSERT_EQUALS(c1.numberOfArguments(), 1);
    TS_ASSERT_EQUALS(c2.numberOfArguments(), 3);

    Attribute attr = test4.attributes().front();

    VariantValue b1 = c1.call(d);
    boost::gregorian::date d1 = attr.get(b1).convertTo<boost::gregorian::date>();
    TS_ASSERT_EQUALS(d, d1);

    VariantValue b2 = c2.call(2005,1,15);

    boost::gregorian::date d2 = attr.get(b2).convertTo<boost::gregorian::date>();
    TS_ASSERT_EQUALS(d, d2);

    VariantValue d3 = attr.get(b1);
    VariantValue b3 = c1.callArgArray({d3});

    boost::gregorian::date d4 = attr.get(b3).convertTo<boost::gregorian::date>();
    TS_ASSERT_EQUALS(d, d4);
}


void ConstructorTestSuite::testParametersByValue()
{
    Class test = Class::lookup("ConstructorTest::TestConsParamPassing1");

    Attribute attr = test.attributes().front();
    Constructor cons = test.constructors().front();

    CopyCount::resetAll();

    CopyCount c(77);

    TestConsParamPassing1 inst(c);

    TS_ASSERT_EQUALS(inst.id, 78);
    TS_ASSERT_EQUALS(c.id(), 77);

    int directCallCopies = CopyCount::numberOfCopies();
    int directCallMoves = CopyCount::numberOfMoves();
    TS_ASSERT_EQUALS(directCallCopies, 1);
    TS_ASSERT_EQUALS(directCallMoves,  0);

    CopyCount::resetAll();

    VariantValue vinst = cons.call(c);
    VariantValue v = attr.get(vinst);

    TS_ASSERT(v.isValid());
    TS_ASSERT_EQUALS(v.value<int>(), 78);
    TS_ASSERT_EQUALS(c.id(), 77);
    int reflectionCopies = CopyCount::numberOfCopies();
    int reflectionMoves = CopyCount::numberOfMoves();

    TS_ASSERT_EQUALS(reflectionCopies, 1);
    TS_ASSERT_EQUALS(reflectionMoves,  1);
    TS_ASSERT(!c.hasBeenMoved());
}


void ConstructorTestSuite::testParametersByReference()
{
    Class test = Class::lookup("ConstructorTest::TestConsParamPassing2");

    Attribute attr = test.attributes().front();
    Constructor cons = test.constructors().front();

    CopyCount::resetAll();

    CopyCount c(77);

    TestConsParamPassing2 inst(c);

    TS_ASSERT_EQUALS(inst.id, 78);
    TS_ASSERT_EQUALS(c.id(), 78);

    int directCallCopies = CopyCount::numberOfCopies();
    int directCallMoves = CopyCount::numberOfMoves();
    TS_ASSERT_EQUALS(directCallCopies, 0);
    TS_ASSERT_EQUALS(directCallMoves,  0);

    CopyCount::resetAll();

    VariantValue vinst = cons.call(c);
    VariantValue v = attr.get(vinst);

    TS_ASSERT(v.isValid());
    TS_ASSERT_EQUALS(v.value<int>(), 79);
    TS_ASSERT_EQUALS(c.id(), 79);
    int reflectionCopies = CopyCount::numberOfCopies();
    int reflectionMoves = CopyCount::numberOfMoves();

    TS_ASSERT_EQUALS(reflectionCopies, 0);
    TS_ASSERT_EQUALS(reflectionMoves,  0);
    TS_ASSERT(!c.hasBeenMoved());
}


void ConstructorTestSuite::testParametersByConstReference()
{
    Class test = Class::lookup("ConstructorTest::TestConsParamPassing3");

    Attribute attr = test.attributes().front();
    Constructor cons = test.constructors().front();

    CopyCount::resetAll();

    CopyCount c(77);

    TestConsParamPassing3 inst(c);

    TS_ASSERT_EQUALS(inst.id, 77);
    TS_ASSERT_EQUALS(c.id(), 77);

    int directCallCopies = CopyCount::numberOfCopies();
    int directCallMoves = CopyCount::numberOfMoves();
    TS_ASSERT_EQUALS(directCallCopies, 0);
    TS_ASSERT_EQUALS(directCallMoves,  0);

    CopyCount::resetAll();

    VariantValue vinst = cons.call(c);
    VariantValue v = attr.get(vinst);

    TS_ASSERT(v.isValid());
    TS_ASSERT_EQUALS(v.value<int>(), 77);
    TS_ASSERT_EQUALS(c.id(), 77);
    int reflectionCopies = CopyCount::numberOfCopies();
    int reflectionMoves = CopyCount::numberOfMoves();

    TS_ASSERT_EQUALS(reflectionCopies, 0);
    TS_ASSERT_EQUALS(reflectionMoves,  0);
    TS_ASSERT(!c.hasBeenMoved());
}
