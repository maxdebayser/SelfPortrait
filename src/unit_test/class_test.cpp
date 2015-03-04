/*
** SelfPortrait API
** See Copyright Notice in reflection.h
*/
#include "class_test.h"
#include "class.h"
#include "reflection_impl.h"
#include "test_utils.h"
#include "str_utils.h"

#include <lua.hpp>

#include <iostream>
#include <string>
#include <algorithm>

using namespace std;


namespace ClassTest {

	class TestBase1 {
	public:

		int base1Method1() { return 5; }
		virtual double method2(double arg) = 0;

	};

	class TestBase2 {
	public:

		int base2Method1() const { return 6; }

	};


	class Test1: public TestBase1, public TestBase2 {
	public:
		std::string method1() const {
			return "this is a test";
		}

		virtual double method2(double arg) {
			return arg*3.14;
		}

		int attribute1;

		const double attribute2;

		static double staticMethod() {
			return 3.14;
		}

		Test1() : attribute1(3), attribute2(3.14) {}

		Test1(int arg1) : attribute1(arg1), attribute2(3.14) {}

		Test1(const Test1& rhs) : attribute1(rhs.attribute1), attribute2(rhs.attribute2) {}

		Test1& operator=(const Test1& rhs) { attribute1 = rhs.attribute1; return *this; }
		Test1& operator=(Test1&& rhs) { attribute1 = rhs.attribute1; return *this; }

		TestBase1& returnsRefToAbstract() { return *this; }
	};

	class Test2 {
	private:
		~Test2() {}
	public:
		void method1() {}
	};
}

REFL_BEGIN_CLASS(ClassTest::TestBase1)
	REFL_METHOD(method2, double, double)
	REFL_METHOD(base1Method1, int)
	REFL_DEFAULT_CONSTRUCTOR()
REFL_END_CLASS

REFL_BEGIN_CLASS(ClassTest::TestBase2)
	REFL_CONST_METHOD(base2Method1, int)
	REFL_DEFAULT_CONSTRUCTOR()
REFL_END_CLASS

REFL_BEGIN_CLASS(ClassTest::Test1)
	REFL_SUPER_CLASS(ClassTest::TestBase1)
	REFL_SUPER_CLASS(ClassTest::TestBase2)
	REFL_CONST_METHOD(method1, std::string)
	REFL_METHOD(method2, double, double)
	REFL_STATIC_METHOD(staticMethod, double)
	REFL_ATTRIBUTE(attribute1, int)
	REFL_ATTRIBUTE(attribute2, double)
	REFL_DEFAULT_CONSTRUCTOR()
	REFL_CONSTRUCTOR(int)
	REFL_CONSTRUCTOR(const ClassTest::Test1&)
	REFL_METHOD(operator=, ClassTest::Test1&, const ClassTest::Test1&)
	REFL_METHOD(operator=, ClassTest::Test1&, ClassTest::Test1&&)
	REFL_METHOD(returnsRefToAbstract, ClassTest::TestBase1&)
REFL_END_CLASS

REFL_BEGIN_CLASS(ClassTest::Test2)
	REFL_METHOD(method1, void)
	REFL_DEFAULT_CONSTRUCTOR()
REFL_END_CLASS


using namespace ClassTest;

void ClassTestSuite::testClass()
{
	//Class test = ClassOf<Test1>();

	Class test = Class::lookup("ClassTest::Test1");

	TS_ASSERT_EQUALS(test.simpleName(), "Test1");

#ifndef NO_RTTI
	TS_ASSERT(test.describes<ClassTest::Test1>())

	Class test2 = Class::lookup(typeid(ClassTest::Test1));
	TS_ASSERT_EQUALS(test2.simpleName(), "Test1");
	TS_ASSERT_EQUALS(test, test2);
#endif

	Class::ClassList superClasses = test.superclasses();

	TS_ASSERT_EQUALS(superClasses.size(), 2);

	Class base1;
	Class base2;

	TS_ASSERT_THROWS(base1.simpleName(), std::runtime_error); // uninitialized handle

	for(const Class& c: superClasses) {
		if (c.fullyQualifiedName() == "ClassTest::TestBase1") {
			base1 = c;
		} else if (c.fullyQualifiedName() == "ClassTest::TestBase2") {
			base2 = c;
		}
	}

	TS_ASSERT_EQUALS(base1.fullyQualifiedName(), "ClassTest::TestBase1");
	TS_ASSERT_EQUALS(base1.simpleName(), "TestBase1");
	TS_ASSERT_EQUALS(base2.fullyQualifiedName(), "ClassTest::TestBase2");
	TS_ASSERT_EQUALS(base2.simpleName(), "TestBase2");

	TS_ASSERT(inheritanceRelation(test, base1));
	TS_ASSERT(inherits(test, base1));
	TS_ASSERT(inheritedBy(base1, test));

	TS_ASSERT(inheritanceRelation(test, base2));
	TS_ASSERT(inherits(test, base2));
	TS_ASSERT(inheritedBy(base2, test));

	TS_ASSERT(!inheritanceRelation(base1, base2));

	Class::ConstructorList base1Constructors = base1.constructors();
	TS_ASSERT_EQUALS(base1.constructors().size(), 1);

	Constructor base1Constructor = base1Constructors.front();

	TS_ASSERT_EQUALS(base1Constructor.numberOfArguments(), 0);
	TS_ASSERT_THROWS(base1Constructor.call(), std::runtime_error); // abstract class


	Class::ConstructorList base2Constructors = base2.constructors();
	TS_ASSERT_EQUALS(base2.constructors().size(), 1);

	Constructor base2Constructor = base2Constructors.front();

	TS_ASSERT_EQUALS(base2Constructor.numberOfArguments(), 0);

	VariantValue b2Inst = base2Constructor.call();
	TS_ASSERT(b2Inst.isValid());

	TS_ASSERT_EQUALS(base2.attributes().size(), 0);
	Class::MethodList base2Methods = base2.methods();
	TS_ASSERT_EQUALS(base2Methods.size(), 1);

	Method base2Method1 = base2Methods.front();

	TS_ASSERT_EQUALS(base2Method1.name(), "base2Method1");
	TS_ASSERT(base2Method1.isConst());
	TS_ASSERT(!base2Method1.isVolatile());
	TS_ASSERT(!base2Method1.isStatic());
	TS_ASSERT_EQUALS(base2Method1.numberOfArguments(), 0);
	TS_ASSERT_EQUALS(base2Method1.call(b2Inst).value<int>(), 6);


	Class::ConstructorList testConstructors = test.constructors();
	Class::AttributeList   attributes       = test.attributes();

	TS_ASSERT_EQUALS(attributes.size(), 2);
	Attribute attr = attributes.front();

	WITH_RTTI(TS_ASSERT(attr.type() == typeid(int)));

	TS_ASSERT_EQUALS(testConstructors.size(), 3);

	Constructor defaultConstr;
	Constructor intConstr;

	TS_ASSERT_THROWS(defaultConstr.argumentSpellings(), std::runtime_error);

	for (const Constructor& c: testConstructors) {
		if (c.numberOfArguments() == 0) {
			defaultConstr = c;
		} else if (c.numberOfArguments() == 1  && c.argumentSpellings()[0] == "int"){
			intConstr = c;
		}
	}


	TS_ASSERT_EQUALS(defaultConstr.numberOfArguments(), 0);
	TS_ASSERT_EQUALS(intConstr.numberOfArguments(), 1);

	VariantValue testInst1 = defaultConstr.call();

	TS_ASSERT(testInst1.isA<Test1>());


#ifndef NO_RTTI
	Class test3 = Class::lookup(testInst1.typeId());
	TS_ASSERT_EQUALS(test3.simpleName(), "Test1");
	TS_ASSERT_EQUALS(test, test3);
#endif

    TS_ASSERT_EQUALS(attr.get(testInst1).value<const int&>(), 3);


	VariantValue testInst2 = intConstr.call(77);

	TS_ASSERT(testInst2.isA<Test1>());
    TS_ASSERT_EQUALS(attr.get(testInst2).value<const int&>(), 77);

	Class::MethodList methods = test.methods();

	TS_ASSERT_EQUALS(methods.size(), 9);

	Method base1Method1;
	Method method1;
	Method method2;
	Method staticMethod;


	for (const Method& m: methods) {
		if (m.name() == "base1Method1") {
			base1Method1 = m;
		} else if (m.name() == "base2Method1") {
			base2Method1 = m;
		} else if (m.name() == "method1") {
			method1 = m;
		} else if (m.name() == "method2") {
			method2 = m;
		} else if (m.name() == "staticMethod") {
			staticMethod = m;
		}
	}

	TS_ASSERT_EQUALS(base1Method1.name(), "base1Method1");
	TS_ASSERT(!base1Method1.isConst());
	TS_ASSERT(!base1Method1.isStatic());
	TS_ASSERT(!base1Method1.isVolatile());
	WITH_RTTI(TS_ASSERT(base1Method1.returnType() == typeid(int)));
	TS_ASSERT_EQUALS(base1Method1.numberOfArguments(), 0);
	TS_ASSERT_EQUALS(base1Method1.call(testInst2).value<int>(), 5);

	TS_ASSERT_EQUALS(base2Method1.name(), "base2Method1");
	TS_ASSERT(base2Method1.isConst());
	TS_ASSERT(!base2Method1.isStatic());
	TS_ASSERT(!base2Method1.isVolatile());
	WITH_RTTI(TS_ASSERT(base2Method1.returnType() == typeid(int)));
	TS_ASSERT_EQUALS(base2Method1.numberOfArguments(), 0);
	TS_ASSERT_EQUALS(base2Method1.call(testInst2).value<int>(), 6);

	TS_ASSERT_EQUALS(method2.name(), "method2");
	TS_ASSERT(!method2.isConst());
	TS_ASSERT(!method2.isStatic());
	TS_ASSERT(!method2.isVolatile());
	WITH_RTTI(TS_ASSERT(method2.returnType() == typeid(double)));
	TS_ASSERT_EQUALS(method2.numberOfArguments(), 1);
	WITH_RTTI(TS_ASSERT(*method2.argumentTypes()[0] == typeid(double)));
	TS_ASSERT_EQUALS(method2.call(testInst2, 2).value<double>(), 6.28);

	TS_ASSERT_EQUALS(staticMethod.name(), "staticMethod");
	TS_ASSERT(!staticMethod.isConst());
	TS_ASSERT(staticMethod.isStatic());
	TS_ASSERT(!staticMethod.isVolatile());
	WITH_RTTI(TS_ASSERT(staticMethod.returnType() == typeid(double)));
	TS_ASSERT_EQUALS(staticMethod.numberOfArguments(), 0);
	TS_ASSERT_EQUALS(staticMethod.call().value<double>(), 3.14);
}


namespace {

	class OTest {
	public:

		int overload(int i) { return i; }

		double overload(double i) { return i; }

		int& constOverload() { return m_attr1; }
		const int& constOverload() const { return m_attr1; }

		static int staticOverload(int i) { return i+1; }
		static double staticOverload(double i) { return i+1.0; }

		int m_attr1 = 3;

	};

}

REFL_BEGIN_CLASS(OTest)
	REFL_METHOD(overload, int, int)
	REFL_METHOD(overload, double, double)
	REFL_METHOD(constOverload, int&)
	REFL_CONST_METHOD(constOverload, const int&)
	REFL_STATIC_METHOD(staticOverload, double, double)
	REFL_STATIC_METHOD(staticOverload, int, int)
REFL_END_CLASS


void ClassTestSuite::testOverload()
{

}


void ClassTestSuite::testLuaAPI()
{
	LuaUtils::LuaStateHolder L;
    LuaUtils::addTestFunctionsAndPaths(&*L);

    if (luaL_loadfile(L, strconv::fmt_str("%1/class_test.lua", srcpath()).c_str()) || lua_pcall(L,0,0,0)) {
		luaL_error(L, "cannot run config file: %s\n", lua_tostring(L, -1));
	}

	int i = 3;
	int b = 4;
	//TS_ASSERT_EQUALS(i, b);
	LuaUtils::callFunc<bool>(L, "testClass");
}


void ClassTestSuite::testClassHash()
{
	using namespace std;

	unordered_map<Class, int> cmap;

	Class test = Class::lookup("ClassTest::Test1");
	Class testBase1 = Class::lookup("ClassTest::TestBase1");
	Class testBase2 = Class::lookup("ClassTest::TestBase2");

	cmap[test] = 1;
	cmap[testBase1] = 2;
	cmap[testBase2] = 3;

	TS_ASSERT_EQUALS(cmap[test], 1);
	TS_ASSERT_EQUALS(cmap[testBase1], 2);
	TS_ASSERT_EQUALS(cmap[testBase2], 3);

}


void ClassTestSuite::testMethodSearch()
{
	Class test = Class::lookup("ClassTest::Test1");

	auto retDouble = test.findAllMethods([&](const Method& m) {
		return m.returnSpelling() == "double";
	});

	TS_ASSERT_EQUALS(retDouble.size(), 3);

	auto retDoubleStrict = test.findAllMethods([&](const Method& m) {
		return m.returnSpelling() == "double" && m.getClass() == test; //filter out inherited methods
	});

	TS_ASSERT_EQUALS(retDoubleStrict.size(), 2);

	auto constMethods = test.findAllMethods([&](const Method& m) {
		return m.isConst();
	});

	TS_ASSERT_EQUALS(constMethods.size(), 2);

	Method m = test.findMethod([&](const Method& m) {
			   return m.name() == "method1";
		   });

	TS_ASSERT(m.isValid());
	TS_ASSERT(m.isConst());
	TS_ASSERT_EQUALS(m.returnSpelling(), "std::string");


	Method m30 = test.findMethod([&](const Method& m) {
			   return m.name() == "method30";
		   });

	TS_ASSERT(!m30.isValid());
	TS_ASSERT_THROWS_ANYTHING(m30.isConst());
}



void ClassTestSuite::testAttributeSearch()
{
	Class test = Class::lookup("ClassTest::Test1");

	auto attrs = test.findAllAttributes([&](const Attribute& a) {
		return a.isConst();
	});

	TS_ASSERT_EQUALS(attrs.size(), 1);
	TS_ASSERT(attrs.front().isConst());
	TS_ASSERT_EQUALS(attrs.front().name(), "attribute2");

	Attribute a1 = test.findAttribute([](const Attribute& a){
		return a.name() == "attribute1";
	});
	TS_ASSERT(a1.isValid());
	TS_ASSERT(!a1.isConst());
	TS_ASSERT_EQUALS(a1.typeSpelling(), "int");

	Attribute a3 = test.findAttribute([](const Attribute& a){
		return a.name() == "attribute3";
	});
	TS_ASSERT(!a3.isValid());
	TS_ASSERT_THROWS_ANYTHING(a3.isConst());
}


void ClassTestSuite::testConstructorSearch()
{
	Class test = Class::lookup("ClassTest::Test1");

	Constructor defaultCons = test.findConstructor([](const Constructor& c) {
		return c.numberOfArguments() == 0;
	});

	TS_ASSERT(defaultCons.isValid());

	Constructor copyCons = test.findConstructor([](const Constructor& c) {
		return c.numberOfArguments() == 1 && c.argumentSpellings()[0] == "const ClassTest::Test1 &";
	});
	TS_ASSERT(copyCons.isValid());
}


void ClassTestSuite::testSuperClassSearch()
{
	Class test = Class::lookup("ClassTest::Test1");
	Class base1 = Class::lookup("ClassTest::TestBase1");

	Class ret = test.findSuperClass([](const Class& c) {
		return c.fullyQualifiedName() == "ClassTest::TestBase1";
	});

	TS_ASSERT(ret.isValid());
	TS_ASSERT_EQUALS(base1, ret);
}


void ClassTestSuite::testPrivateDestructor()
{
	Class test = Class::lookup("ClassTest::Test2");

	TS_ASSERT(test.isValid());

	Constructor c = test.findConstructor([](const Constructor& c) {
		return c.isDefaultConstructor();
	});



	TS_ASSERT_THROWS_ANYTHING(c.call());
}
