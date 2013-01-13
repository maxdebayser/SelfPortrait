#include "test_functions.h"
#include "reflection_impl.h"

static long global_counter = 0;

namespace test_functions {

	long getCounter()
	{
		return global_counter;
	}

	void resetCounter()
	{
		global_counter = 0;
	}

	void noargs()
	{
		++global_counter;
	}

	void intarg1(int)
	{
		++global_counter;
	}
	void intarg2(int, int)
	{
		++global_counter;
	}
	void intarg3(int, int, int)
	{
		++global_counter;
	}
	void intarg4(int, int, int, int)
	{
		++global_counter;
	}
	void intarg5(int, int, int, int, int)
	{
		++global_counter;
	}
	void intarg6(int, int, int, int, int, int)
	{
		++global_counter;
	}
	void intarg7(int, int, int, int, int, int, int)
	{
		++global_counter;
	}
	void intarg8(int, int, int, int, int, int, int, int)
	{
		++global_counter;
	}
	void intarg9(int, int, int, int, int, int, int, int, int)
	{
		++global_counter;
	}

	void structArgCpy1(TestStruct)
	{
		++global_counter;
	}
	void structArgCpy2(TestStruct, TestStruct)
	{
		++global_counter;
	}
	void structArgCpy3(TestStruct, TestStruct, TestStruct)
	{
		++global_counter;
	}
	void structArgCpy4(TestStruct, TestStruct, TestStruct, TestStruct)
	{
		++global_counter;
	}
	void structArgCpy5(TestStruct, TestStruct, TestStruct, TestStruct, TestStruct)
	{
		++global_counter;
	}
	void structArgCpy6(TestStruct, TestStruct, TestStruct, TestStruct, TestStruct, TestStruct)
	{
		++global_counter;
	}
	void structArgCpy7(TestStruct, TestStruct, TestStruct, TestStruct, TestStruct, TestStruct, TestStruct)
	{
		++global_counter;
	}
	void structArgCpy8(TestStruct, TestStruct, TestStruct, TestStruct, TestStruct, TestStruct, TestStruct, TestStruct)
	{
		++global_counter;
	}
	void structArgCpy9(TestStruct, TestStruct, TestStruct, TestStruct, TestStruct, TestStruct, TestStruct, TestStruct, TestStruct)
	{
		++global_counter;
	}

	void structArgRef1(const TestStruct&)
	{
		++global_counter;
	}
	void structArgRef2(const TestStruct&, const TestStruct&)
	{
		++global_counter;
	}
	void structArgRef3(const TestStruct&, const TestStruct&, const TestStruct&)
	{
		++global_counter;
	}
	void structArgRef4(const TestStruct&, const TestStruct&, const TestStruct&, const TestStruct&)
	{
		++global_counter;
	}
	void structArgRef5(const TestStruct&, const TestStruct&, const TestStruct&, const TestStruct&, const TestStruct&)
	{
		++global_counter;
	}
	void structArgRef6(const TestStruct&, const TestStruct&, const TestStruct&, const TestStruct&, const TestStruct&, const TestStruct&)
	{
		++global_counter;
	}
	void structArgRef7(const TestStruct&, const TestStruct&, const TestStruct&, const TestStruct&, const TestStruct&, const TestStruct&, const TestStruct&)
	{
		++global_counter;
	}
	void structArgRef8(const TestStruct&, const TestStruct&, const TestStruct&, const TestStruct&, const TestStruct&, const TestStruct&, const TestStruct&, const TestStruct&)
	{
		++global_counter;
	}
	void structArgRef9(const TestStruct&, const TestStruct&, const TestStruct&, const TestStruct&, const TestStruct&, const TestStruct&, const TestStruct&, const TestStruct&, const TestStruct&)
	{
		++global_counter;
	}

	void polyArg1(const Base&)
	{
		++global_counter;
	}
	void polyArg2(const Base&, const Base&)
	{
		++global_counter;
	}
	void polyArg3(const Base&, const Base&, const Base&)
	{
		++global_counter;
	}
	void polyArg4(const Base&, const Base&, const Base&, const Base&)
	{
		++global_counter;
	}
	void polyArg5(const Base&, const Base&, const Base&, const Base&, const Base&)
	{
		++global_counter;
	}
	void polyArg6(const Base&, const Base&, const Base&, const Base&, const Base&, const Base&)
	{
		++global_counter;
	}
	void polyArg7(const Base&, const Base&, const Base&, const Base&, const Base&, const Base&, const Base&)
	{
		++global_counter;
	}
	void polyArg8(const Base&, const Base&, const Base&, const Base&, const Base&, const Base&, const Base&, const Base&)
	{
		++global_counter;
	}
	void polyArg9(const Base&, const Base&, const Base&, const Base&, const Base&, const Base&, const Base&, const Base&, const Base&)
	{
		++global_counter;
	}

}

REFL_FUNCTION(test_functions::noargs, void)

REFL_FUNCTION(test_functions::intarg1, void, int)

REFL_FUNCTION(test_functions::intarg2, void, int, int)

REFL_FUNCTION(test_functions::intarg3, void, int, int, int)

REFL_FUNCTION(test_functions::intarg4, void, int, int, int, int)

REFL_FUNCTION(test_functions::intarg5, void, int, int, int, int, int)

REFL_FUNCTION(test_functions::intarg6, void, int, int, int, int, int, int)

REFL_FUNCTION(test_functions::intarg7, void, int, int, int, int, int, int, int)

REFL_FUNCTION(test_functions::intarg8, void, int, int, int, int, int, int, int, int)

REFL_FUNCTION(test_functions::intarg9, void, int, int, int, int, int, int, int, int, int)

REFL_FUNCTION(test_functions::structArgCpy1, void, test_functions::TestStruct)

REFL_FUNCTION(test_functions::structArgCpy2, void, test_functions::TestStruct, test_functions::TestStruct)

REFL_FUNCTION(test_functions::structArgCpy3, void, test_functions::TestStruct, test_functions::TestStruct, test_functions::TestStruct)

REFL_FUNCTION(test_functions::structArgCpy4, void, test_functions::TestStruct, test_functions::TestStruct, test_functions::TestStruct, test_functions::TestStruct)

REFL_FUNCTION(test_functions::structArgCpy5, void, test_functions::TestStruct, test_functions::TestStruct, test_functions::TestStruct, test_functions::TestStruct, test_functions::TestStruct)

REFL_FUNCTION(test_functions::structArgCpy6, void, test_functions::TestStruct, test_functions::TestStruct, test_functions::TestStruct, test_functions::TestStruct, test_functions::TestStruct, test_functions::TestStruct)

REFL_FUNCTION(test_functions::structArgCpy7, void, test_functions::TestStruct, test_functions::TestStruct, test_functions::TestStruct, test_functions::TestStruct, test_functions::TestStruct, test_functions::TestStruct, test_functions::TestStruct)

REFL_FUNCTION(test_functions::structArgCpy8, void, test_functions::TestStruct, test_functions::TestStruct, test_functions::TestStruct, test_functions::TestStruct, test_functions::TestStruct, test_functions::TestStruct, test_functions::TestStruct, test_functions::TestStruct)

REFL_FUNCTION(test_functions::structArgCpy9, void, test_functions::TestStruct, test_functions::TestStruct, test_functions::TestStruct, test_functions::TestStruct, test_functions::TestStruct, test_functions::TestStruct, test_functions::TestStruct, test_functions::TestStruct, test_functions::TestStruct)

REFL_FUNCTION(test_functions::structArgRef1, void, const test_functions::TestStruct &)

REFL_FUNCTION(test_functions::structArgRef2, void, const test_functions::TestStruct &, const test_functions::TestStruct &)

REFL_FUNCTION(test_functions::structArgRef3, void, const test_functions::TestStruct &, const test_functions::TestStruct &, const test_functions::TestStruct &)

REFL_FUNCTION(test_functions::structArgRef4, void, const test_functions::TestStruct &, const test_functions::TestStruct &, const test_functions::TestStruct &, const test_functions::TestStruct &)

REFL_FUNCTION(test_functions::structArgRef5, void, const test_functions::TestStruct &, const test_functions::TestStruct &, const test_functions::TestStruct &, const test_functions::TestStruct &, const test_functions::TestStruct &)

REFL_FUNCTION(test_functions::structArgRef6, void, const test_functions::TestStruct &, const test_functions::TestStruct &, const test_functions::TestStruct &, const test_functions::TestStruct &, const test_functions::TestStruct &, const test_functions::TestStruct &)

REFL_FUNCTION(test_functions::structArgRef7, void, const test_functions::TestStruct &, const test_functions::TestStruct &, const test_functions::TestStruct &, const test_functions::TestStruct &, const test_functions::TestStruct &, const test_functions::TestStruct &, const test_functions::TestStruct &)

REFL_FUNCTION(test_functions::structArgRef8, void, const test_functions::TestStruct &, const test_functions::TestStruct &, const test_functions::TestStruct &, const test_functions::TestStruct &, const test_functions::TestStruct &, const test_functions::TestStruct &, const test_functions::TestStruct &, const test_functions::TestStruct &)

REFL_FUNCTION(test_functions::structArgRef9, void, const test_functions::TestStruct &, const test_functions::TestStruct &, const test_functions::TestStruct &, const test_functions::TestStruct &, const test_functions::TestStruct &, const test_functions::TestStruct &, const test_functions::TestStruct &, const test_functions::TestStruct &, const test_functions::TestStruct &)

REFL_BEGIN_CLASS(test_functions::TestStruct)
REFL_DEFAULT_CONSTRUCTOR()
REFL_ATTRIBUTE(elem1, int)
REFL_ATTRIBUTE(elem2, int)
REFL_ATTRIBUTE(elem3, int)
REFL_ATTRIBUTE(elem4, int)
REFL_END_CLASS

REFL_FUNCTION(test_functions::polyArg1, void, const test_functions::Base &)

REFL_FUNCTION(test_functions::polyArg2, void, const test_functions::Base &, const test_functions::Base &)

REFL_FUNCTION(test_functions::polyArg3, void, const test_functions::Base &, const test_functions::Base &, const test_functions::Base &)

REFL_FUNCTION(test_functions::polyArg4, void, const test_functions::Base &, const test_functions::Base &, const test_functions::Base &, const test_functions::Base &)

REFL_FUNCTION(test_functions::polyArg5, void, const test_functions::Base &, const test_functions::Base &, const test_functions::Base &, const test_functions::Base &, const test_functions::Base &)

REFL_FUNCTION(test_functions::polyArg6, void, const test_functions::Base &, const test_functions::Base &, const test_functions::Base &, const test_functions::Base &, const test_functions::Base &, const test_functions::Base &)

REFL_FUNCTION(test_functions::polyArg7, void, const test_functions::Base &, const test_functions::Base &, const test_functions::Base &, const test_functions::Base &, const test_functions::Base &, const test_functions::Base &, const test_functions::Base &)

REFL_FUNCTION(test_functions::polyArg8, void, const test_functions::Base &, const test_functions::Base &, const test_functions::Base &, const test_functions::Base &, const test_functions::Base &, const test_functions::Base &, const test_functions::Base &, const test_functions::Base &)

REFL_FUNCTION(test_functions::polyArg9, void, const test_functions::Base &, const test_functions::Base &, const test_functions::Base &, const test_functions::Base &, const test_functions::Base &, const test_functions::Base &, const test_functions::Base &, const test_functions::Base &, const test_functions::Base &)


REFL_BEGIN_STUB(test_functions::Base, test_functions__BaseStub)
REFL_END_STUB

REFL_BEGIN_CLASS(test_functions::Base)
REFL_STUB(test_functions__BaseStub)
REFL_DEFAULT_CONSTRUCTOR()
REFL_METHOD(operator=, test_functions::Base &, const test_functions::Base &)
REFL_END_CLASS

REFL_BEGIN_CLASS(test_functions::Derived)
REFL_SUPER_CLASS(test_functions::Base)
REFL_DEFAULT_CONSTRUCTOR()
REFL_METHOD(foo, void)
REFL_METHOD(operator=, test_functions::Derived &, const test_functions::Derived &)
REFL_END_CLASS

