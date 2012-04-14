#include "class_test.h"
#include "class.h"
#include "reflection_impl.h"

#include <iostream>
#include <string>
#include <algorithm>
using namespace std;


namespace Test {

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
		std::string method1() const{
			return "this is a test";
		}

		virtual double method2(double arg) {
			return arg*3.14;
		}

		int attribute1;

		static double staticMethod() {
			return 3.14;
		}

		Test1() : attribute1(3) {}

		Test1(int arg1) : attribute1(arg1) {}

		Test1(const Test1& rhs) : attribute1(rhs.attribute1) {}

		Test1& operator=(const Test1& rhs) { attribute1 = rhs.attribute1; return *this; }
	};


}

BEGIN_CLASS(Test::TestBase1)
	METHOD(method2, double, double)
	METHOD(base1Method1, int)
	DEFAULT_CONSTRUCTOR()
END_CLASS

BEGIN_CLASS(Test::TestBase2)
	CONST_METHOD(base2Method1, int)
	DEFAULT_CONSTRUCTOR()
END_CLASS

BEGIN_CLASS(Test::Test1)
	SUPER_CLASS(class Test::TestBase1)
	SUPER_CLASS(Test::TestBase2)
	CONST_METHOD(method1, std::string)
	METHOD(method2, double, double)
	STATIC_METHOD(staticMethod, double)
	ATTRIBUTE(attribute1, int)
	DEFAULT_CONSTRUCTOR()
	CONSTRUCTOR(int)
	CONSTRUCTOR(const Test::Test1&)
	METHOD(operator=, Test::Test1&, const Test::Test1&)
END_CLASS




using namespace Test;

void ClassTestSuite::testClass()
{
	//Class test = ClassOf<Test1>();

	Class test = Class::forName("Test::Test1");

	TS_ASSERT_EQUALS(test.simpleName(), "Test1");

	Class::ClassList superClasses = test.superclasses();

	TS_ASSERT_EQUALS(superClasses.size(), 2);

	Class base1;
	Class base2;

	TS_ASSERT_THROWS(base1.simpleName(), std::runtime_error); // uninitialized handle

	for(const Class& c: superClasses) {
		if (c.fullyQualifiedName() == "Test::TestBase1") {
			base1 = c;
		} else if (c.fullyQualifiedName() == "Test::TestBase2") {
			base2 = c;
		}
	}

	TS_ASSERT_EQUALS(base1.fullyQualifiedName(), "Test::TestBase1");
	TS_ASSERT_EQUALS(base1.simpleName(), "TestBase1");
	TS_ASSERT_EQUALS(base2.fullyQualifiedName(), "Test::TestBase2");
	TS_ASSERT_EQUALS(base2.simpleName(), "TestBase2");

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

	TS_ASSERT_EQUALS(attributes.size(), 1);
	Attribute attr = attributes.front();

	TS_ASSERT(attr.type() == typeid(int));

	TS_ASSERT_EQUALS(testConstructors.size(), 3);

	Constructor defaultConstr;
	Constructor intConstr;

	TS_ASSERT_THROWS(defaultConstr.argumentTypes(), std::runtime_error);

	for (const Constructor& c: testConstructors) {
		if (c.numberOfArguments() == 0) {
			defaultConstr = c;
		} else if (c.numberOfArguments() == 1  && *c.argumentTypes()[0] == typeid(int)){
			intConstr = c;
		}
	}

	TS_ASSERT_EQUALS(defaultConstr.numberOfArguments(), 0);
	TS_ASSERT_EQUALS(intConstr.numberOfArguments(), 1);

	VariantValue testInst1 = defaultConstr.call();

	TS_ASSERT(testInst1.isA<Test1>());

	TS_ASSERT_EQUALS(attr.get(testInst1).value<int>(), 3);


	VariantValue testInst2 = intConstr.call(77);
	TS_ASSERT(testInst2.isA<Test1>());
	TS_ASSERT_EQUALS(attr.get(testInst2).value<int>(), 77);

	Class::MethodList methods = test.methods();

	TS_ASSERT_EQUALS(methods.size(), 7);

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
	TS_ASSERT(base1Method1.returnType() == typeid(int));
	TS_ASSERT_EQUALS(base1Method1.numberOfArguments(), 0);
	TS_ASSERT_EQUALS(base1Method1.call(testInst2).value<int>(), 5);

	TS_ASSERT_EQUALS(base2Method1.name(), "base2Method1");
	TS_ASSERT(base2Method1.isConst());
	TS_ASSERT(!base2Method1.isStatic());
	TS_ASSERT(!base2Method1.isVolatile());
	TS_ASSERT(base2Method1.returnType() == typeid(int));
	TS_ASSERT_EQUALS(base2Method1.numberOfArguments(), 0);
	TS_ASSERT_EQUALS(base2Method1.call(testInst2).value<int>(), 6);

	TS_ASSERT_EQUALS(method2.name(), "method2");
	TS_ASSERT(!method2.isConst());
	TS_ASSERT(!method2.isStatic());
	TS_ASSERT(!method2.isVolatile());
	TS_ASSERT(method2.returnType() == typeid(double));
	TS_ASSERT_EQUALS(method2.numberOfArguments(), 1);
	TS_ASSERT(*method2.argumentTypes()[0] == typeid(double));
	TS_ASSERT_EQUALS(method2.call(testInst2, 2).value<double>(), 6.28);

	TS_ASSERT_EQUALS(staticMethod.name(), "staticMethod");
	TS_ASSERT(!staticMethod.isConst());
	TS_ASSERT(staticMethod.isStatic());
	TS_ASSERT(!staticMethod.isVolatile());
	TS_ASSERT(staticMethod.returnType() == typeid(double));
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

BEGIN_CLASS(OTest)
	METHOD(overload, int, int)
	METHOD(overload, double, double)
	METHOD(constOverload, int&)
	CONST_METHOD(constOverload, const int&)
	STATIC_METHOD(staticOverload, double, double)
	STATIC_METHOD(staticOverload, int, int)
END_CLASS

void ClassTestSuite::testOverload()
{



}
