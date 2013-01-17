#include "test_functions.h"
#include "reflection.h"

#include <time.h>
#include <stdlib.h>
#include <iostream>
using namespace std;

static const int times = 100000000;

void noargtest()
{
	std::list<Function> functions = Function::findFunctions("test_functions::noargs");

	if (functions.size() != 1) {
		std::cerr << "wrong number of functions found" << std::endl;
		exit(1);
	}

	Function noargRefl = functions.front();

	test_functions::resetCounter();


	clock_t start = clock();

	for (int i = 0; i < times; ++i) {
		test_functions::noargs();
	}

	clock_t final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "direct 0 args = " << (final - start) << std::endl;

	test_functions::resetCounter();

	start = clock();

	for (int i = 0; i < times; ++i) {
		noargRefl.call();
	}

	final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "reflective 0 args = " << (final - start) << std::endl;

}


void arg1test()
{
	std::list<Function> functions = Function::findFunctions("test_functions::intarg1");

	if (functions.size() != 1) {
		std::cerr << "wrong number of functions found" << std::endl;
		exit(1);
	}

	Function reflFunc = functions.front();

	test_functions::resetCounter();

	clock_t start = clock();

	for (int i = 0; i < times; ++i) {
		test_functions::intarg1(0);
	}

	clock_t final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "direct 1 args = " << (final - start) << std::endl;

	test_functions::resetCounter();

	start = clock();

	std::vector<VariantValue> args = { 0 };

	for (int i = 0; i < times; ++i) {
		//reflFunc.call(0);
		reflFunc.callArgArray(args);
	}

	final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "reflective 1 args = " << (final - start) << std::endl;

}


void arg2test()
{
	std::list<Function> functions = Function::findFunctions("test_functions::intarg2");

	if (functions.size() != 1) {
		std::cerr << "wrong number of functions found" << std::endl;
		exit(1);
	}

	Function reflFunc = functions.front();

	test_functions::resetCounter();

	clock_t start = clock();

	for (int i = 0; i < times; ++i) {
		test_functions::intarg2(0, 1);
	}

	clock_t final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "direct 2 args = " << (final - start) << std::endl;

	test_functions::resetCounter();

	start = clock();

	std::vector<VariantValue> args = { 0, 1 };

	for (int i = 0; i < times; ++i) {
		reflFunc.callArgArray(args);
	}

	final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "reflective 2 args = " << (final - start) << std::endl;

}

void arg3test()
{
	std::list<Function> functions = Function::findFunctions("test_functions::intarg3");

	if (functions.size() != 1) {
		std::cerr << "wrong number of functions found" << std::endl;
		exit(1);
	}

	Function reflFunc = functions.front();

	test_functions::resetCounter();

	clock_t start = clock();

	for (int i = 0; i < times; ++i) {
		test_functions::intarg3(0, 1, 2);
	}

	clock_t final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "direct 3 args = " << (final - start) << std::endl;

	test_functions::resetCounter();

	start = clock();

	std::vector<VariantValue> args = { 0, 1, 2 };

	for (int i = 0; i < times; ++i) {
		reflFunc.callArgArray(args);
	}

	final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "reflective 3 args = " << (final - start) << std::endl;

}

void arg4test()
{
	std::list<Function> functions = Function::findFunctions("test_functions::intarg4");

	if (functions.size() != 1) {
		std::cerr << "wrong number of functions found" << std::endl;
		exit(1);
	}

	Function reflFunc = functions.front();

	test_functions::resetCounter();

	clock_t start = clock();

	for (int i = 0; i < times; ++i) {
		test_functions::intarg4(0, 1, 2, 3);
	}

	clock_t final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "direct 4 args = " << (final - start) << std::endl;

	test_functions::resetCounter();

	start = clock();

	std::vector<VariantValue> args = { 0, 1, 2, 3 };

	for (int i = 0; i < times; ++i) {
		reflFunc.callArgArray(args);
	}

	final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "reflective 4 args = " << (final - start) << std::endl;

}

void arg5test()
{
	std::list<Function> functions = Function::findFunctions("test_functions::intarg5");

	if (functions.size() != 1) {
		std::cerr << "wrong number of functions found" << std::endl;
		exit(1);
	}

	Function reflFunc = functions.front();

	test_functions::resetCounter();

	clock_t start = clock();

	for (int i = 0; i < times; ++i) {
		test_functions::intarg5(0, 1, 2, 3, 4);
	}

	clock_t final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "direct 5 args = " << (final - start) << std::endl;

	test_functions::resetCounter();

	start = clock();

	std::vector<VariantValue> args = { 0, 1, 2, 3, 4 };

	for (int i = 0; i < times; ++i) {
		reflFunc.callArgArray(args);
	}

	final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "reflective 5 args = " << (final - start) << std::endl;

}

void arg6test()
{
	std::list<Function> functions = Function::findFunctions("test_functions::intarg6");

	if (functions.size() != 1) {
		std::cerr << "wrong number of functions found" << std::endl;
		exit(1);
	}

	Function reflFunc = functions.front();

	test_functions::resetCounter();

	clock_t start = clock();

	for (int i = 0; i < times; ++i) {
		test_functions::intarg6(0, 1, 2, 3, 4, 5);
	}

	clock_t final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "direct 6 args = " << (final - start) << std::endl;

	test_functions::resetCounter();

	start = clock();

	std::vector<VariantValue> args = { 0, 1, 2, 3, 4, 5 };

	for (int i = 0; i < times; ++i) {
		reflFunc.callArgArray(args);
	}

	final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "reflective 6 args = " << (final - start) << std::endl;

}

void arg7test()
{
	std::list<Function> functions = Function::findFunctions("test_functions::intarg7");

	if (functions.size() != 1) {
		std::cerr << "wrong number of functions found" << std::endl;
		exit(1);
	}

	Function reflFunc = functions.front();

	test_functions::resetCounter();

	clock_t start = clock();

	for (int i = 0; i < times; ++i) {
		test_functions::intarg7(0, 1, 2, 3, 4, 5, 6);
	}

	clock_t final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "direct 7 args = " << (final - start) << std::endl;

	test_functions::resetCounter();

	start = clock();

	std::vector<VariantValue> args = { 0, 1, 2, 3, 4, 5, 6 };

	for (int i = 0; i < times; ++i) {
		reflFunc.callArgArray(args);
	}

	final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "reflective 7 args = " << (final - start) << std::endl;

}

void arg8test()
{
	std::list<Function> functions = Function::findFunctions("test_functions::intarg8");

	if (functions.size() != 1) {
		std::cerr << "wrong number of functions found" << std::endl;
		exit(1);
	}

	Function reflFunc = functions.front();

	test_functions::resetCounter();

	clock_t start = clock();

	for (int i = 0; i < times; ++i) {
		test_functions::intarg8(0, 1, 2, 3, 4, 5, 6, 7);
	}

	clock_t final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "direct 8 args = " << (final - start) << std::endl;

	test_functions::resetCounter();

	start = clock();

	std::vector<VariantValue> args = { 0, 1, 2, 3, 4, 5, 6, 7 };

	for (int i = 0; i < times; ++i) {
		reflFunc.callArgArray(args);
	}

	final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "reflective 8 args = " << (final - start) << std::endl;

}

void arg9test()
{
	std::list<Function> functions = Function::findFunctions("test_functions::intarg9");

	if (functions.size() != 1) {
		std::cerr << "wrong number of functions found" << std::endl;
		exit(1);
	}

	Function reflFunc = functions.front();

	test_functions::resetCounter();

	clock_t start = clock();

	for (int i = 0; i < times; ++i) {
		test_functions::intarg9(0, 1, 2, 3, 4, 5, 6, 7, 8);
	}

	clock_t final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "direct 9 args = " << (final - start) << std::endl;

	test_functions::resetCounter();

	start = clock();

	std::vector<VariantValue> args = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };

	for (int i = 0; i < times; ++i) {
		reflFunc.callArgArray(args);
	}

	final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "reflective 9 args = " << (final - start) << std::endl;

}


void structArgCpy1Test()
{
	using namespace test_functions;
	std::list<Function> functions = Function::findFunctions("test_functions::structArgCpy1");

	if (functions.size() != 1) {
		std::cerr << "wrong number of functions found" << std::endl;
		exit(1);
	}

	Function noargRefl = functions.front();

	test_functions::resetCounter();


	clock_t start = clock();

	TestStruct t1;
	for (int i = 0; i < times; ++i) {
		test_functions::structArgCpy1(t1);
	}

	clock_t final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "direct 1 struct arg = " << (final - start) << std::endl;

	test_functions::resetCounter();

	start = clock();

	std::vector<VariantValue> args = { TestStruct() };

	for (int i = 0; i < times; ++i) {
		noargRefl.callArgArray(args);
	}

	final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "reflective 1 struct arg = " << (final - start) << std::endl;

}

void structArgCpy2Test()
{
	using namespace test_functions;
	std::list<Function> functions = Function::findFunctions("test_functions::structArgCpy2");

	if (functions.size() != 1) {
		std::cerr << "wrong number of functions found" << std::endl;
		exit(1);
	}

	Function noargRefl = functions.front();

	test_functions::resetCounter();
	clock_t start = clock();

	TestStruct t1, t2;
	for (int i = 0; i < times; ++i) {
		test_functions::structArgCpy2(t1, t2);
	}

	clock_t final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "direct 2 struct arg = " << (final - start) << std::endl;

	test_functions::resetCounter();

	start = clock();

	std::vector<VariantValue> args = { TestStruct(), TestStruct() };

	for (int i = 0; i < times; ++i) {
		noargRefl.callArgArray(args);
	}

	final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "reflective 2 struct arg = " << (final - start) << std::endl;

}

void structArgCpy3Test()
{
	using namespace test_functions;
	std::list<Function> functions = Function::findFunctions("test_functions::structArgCpy3");

	if (functions.size() != 1) {
		std::cerr << "wrong number of functions found" << std::endl;
		exit(1);
	}

	Function noargRefl = functions.front();

	test_functions::resetCounter();
	clock_t start = clock();

	TestStruct t1, t2, t3;
	for (int i = 0; i < times; ++i) {
		test_functions::structArgCpy3(t1, t2, t3);
	}

	clock_t final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "direct 3 struct arg = " << (final - start) << std::endl;

	test_functions::resetCounter();

	start = clock();

	std::vector<VariantValue> args = { TestStruct(), TestStruct(), TestStruct() };

	for (int i = 0; i < times; ++i) {
		noargRefl.callArgArray(args);
	}

	final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "reflective 3 struct arg = " << (final - start) << std::endl;

}

void structArgCpy4Test()
{
	using namespace test_functions;
	std::list<Function> functions = Function::findFunctions("test_functions::structArgCpy4");

	if (functions.size() != 1) {
		std::cerr << "wrong number of functions found" << std::endl;
		exit(1);
	}

	Function noargRefl = functions.front();

	test_functions::resetCounter();
	clock_t start = clock();

	TestStruct t1, t2, t3, t4;
	for (int i = 0; i < times; ++i) {
		test_functions::structArgCpy4(t1, t2, t3, t4);
	}

	clock_t final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "direct 4 struct arg = " << (final - start) << std::endl;

	test_functions::resetCounter();

	start = clock();

	std::vector<VariantValue> args = { TestStruct(), TestStruct(), TestStruct(), TestStruct() };

	for (int i = 0; i < times; ++i) {
		noargRefl.callArgArray(args);
	}

	final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "reflective 4 struct arg = " << (final - start) << std::endl;

}

void structArgCpy5Test()
{
	using namespace test_functions;
	std::list<Function> functions = Function::findFunctions("test_functions::structArgCpy5");

	if (functions.size() != 1) {
		std::cerr << "wrong number of functions found" << std::endl;
		exit(1);
	}

	Function noargRefl = functions.front();

	test_functions::resetCounter();
	clock_t start = clock();

	TestStruct t1, t2, t3, t4, t5;
	for (int i = 0; i < times; ++i) {
		test_functions::structArgCpy5(t1, t2, t3, t4, t5);
	}

	clock_t final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "direct 5 struct arg = " << (final - start) << std::endl;

	test_functions::resetCounter();

	start = clock();

	std::vector<VariantValue> args = { TestStruct(), TestStruct(), TestStruct(), TestStruct(), TestStruct() };

	for (int i = 0; i < times; ++i) {
		noargRefl.callArgArray(args);
	}

	final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "reflective 5 struct arg = " << (final - start) << std::endl;

}

void structArgCpy6Test()
{
	using namespace test_functions;
	std::list<Function> functions = Function::findFunctions("test_functions::structArgCpy6");

	if (functions.size() != 1) {
		std::cerr << "wrong number of functions found" << std::endl;
		exit(1);
	}

	Function noargRefl = functions.front();

	test_functions::resetCounter();
	clock_t start = clock();

	TestStruct t1, t2, t3, t4, t5, t6;
	for (int i = 0; i < times; ++i) {
		test_functions::structArgCpy6(t1, t2, t3, t4, t5, t6);
	}

	clock_t final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "direct 6 struct arg = " << (final - start) << std::endl;

	test_functions::resetCounter();

	start = clock();

	std::vector<VariantValue> args = { TestStruct(), TestStruct(), TestStruct(), TestStruct(), TestStruct(), TestStruct() };

	for (int i = 0; i < times; ++i) {
		noargRefl.callArgArray(args);
	}

	final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "reflective 6 struct arg = " << (final - start) << std::endl;

}

void structArgCpy7Test()
{
	using namespace test_functions;
	std::list<Function> functions = Function::findFunctions("test_functions::structArgCpy7");

	if (functions.size() != 1) {
		std::cerr << "wrong number of functions found" << std::endl;
		exit(1);
	}

	Function noargRefl = functions.front();

	test_functions::resetCounter();
	clock_t start = clock();

	TestStruct t1, t2, t3, t4, t5, t6, t7;
	for (int i = 0; i < times; ++i) {
		test_functions::structArgCpy7(t1, t2, t3, t4, t5, t6, t7);
	}

	clock_t final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "direct 7 struct arg = " << (final - start) << std::endl;

	test_functions::resetCounter();

	start = clock();

	std::vector<VariantValue> args = { TestStruct(), TestStruct(), TestStruct(), TestStruct(), TestStruct(), TestStruct(), TestStruct() };

	for (int i = 0; i < times; ++i) {
		noargRefl.callArgArray(args);
	}

	final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "reflective 7 struct arg = " << (final - start) << std::endl;

}


void structArgCpy8Test()
{
	using namespace test_functions;
	std::list<Function> functions = Function::findFunctions("test_functions::structArgCpy8");

	if (functions.size() != 1) {
		std::cerr << "wrong number of functions found" << std::endl;
		exit(1);
	}

	Function noargRefl = functions.front();

	test_functions::resetCounter();
	clock_t start = clock();

	TestStruct t1, t2, t3, t4, t5, t6, t7, t8;
	for (int i = 0; i < times; ++i) {
		test_functions::structArgCpy8(t1, t2, t3, t4, t5, t6, t7, t8);
	}

	clock_t final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "direct 8 struct arg = " << (final - start) << std::endl;

	test_functions::resetCounter();

	start = clock();

	std::vector<VariantValue> args = { TestStruct(), TestStruct(), TestStruct(), TestStruct(), TestStruct(), TestStruct(), TestStruct(), TestStruct() };

	for (int i = 0; i < times; ++i) {
		noargRefl.callArgArray(args);
	}

	final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "reflective 8 struct arg = " << (final - start) << std::endl;

}


void structArgCpy9Test()
{
	using namespace test_functions;
	std::list<Function> functions = Function::findFunctions("test_functions::structArgCpy9");

	if (functions.size() != 1) {
		std::cerr << "wrong number of functions found" << std::endl;
		exit(1);
	}

	Function noargRefl = functions.front();

	test_functions::resetCounter();
	clock_t start = clock();

	TestStruct t1, t2, t3, t4, t5, t6, t7, t8, t9;
	for (int i = 0; i < times; ++i) {
		test_functions::structArgCpy9(t1, t2, t3, t4, t5, t6, t7, t8, t9);
	}

	clock_t final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "direct 9 struct arg = " << (final - start) << std::endl;

	test_functions::resetCounter();

	start = clock();

	std::vector<VariantValue> args = { TestStruct(), TestStruct(), TestStruct(), TestStruct(), TestStruct(), TestStruct(), TestStruct(), TestStruct(), TestStruct() };

	for (int i = 0; i < times; ++i) {
		noargRefl.callArgArray(args);
	}

	final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "reflective 9 struct arg = " << (final - start) << std::endl;

}

void structArgRef1Test()
{
	using namespace test_functions;
	std::list<Function> functions = Function::findFunctions("test_functions::structArgRef1");

	if (functions.size() != 1) {
		std::cerr << "wrong number of functions found" << std::endl;
		exit(1);
	}

	Function noargRefl = functions.front();

	test_functions::resetCounter();


	clock_t start = clock();

	TestStruct t1;
	for (int i = 0; i < times; ++i) {
		test_functions::structArgRef1(t1);
	}

	clock_t final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "direct 1 struct arg = " << (final - start) << std::endl;

	test_functions::resetCounter();

	start = clock();

	std::vector<VariantValue> args = { TestStruct() };

	for (int i = 0; i < times; ++i) {
		noargRefl.callArgArray(args);
	}

	final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "reflective 1 struct arg = " << (final - start) << std::endl;

}

void structArgRef2Test()
{
	using namespace test_functions;
	std::list<Function> functions = Function::findFunctions("test_functions::structArgRef2");

	if (functions.size() != 1) {
		std::cerr << "wrong number of functions found" << std::endl;
		exit(1);
	}

	Function noargRefl = functions.front();

	test_functions::resetCounter();
	clock_t start = clock();

	TestStruct t1, t2;
	for (int i = 0; i < times; ++i) {
		test_functions::structArgRef2(t1, t2);
	}

	clock_t final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "direct 2 struct arg = " << (final - start) << std::endl;

	test_functions::resetCounter();

	start = clock();

	std::vector<VariantValue> args = { TestStruct(), TestStruct() };

	for (int i = 0; i < times; ++i) {
		noargRefl.callArgArray(args);
	}

	final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "reflective 2 struct arg = " << (final - start) << std::endl;

}

void structArgRef3Test()
{
	using namespace test_functions;
	std::list<Function> functions = Function::findFunctions("test_functions::structArgRef3");

	if (functions.size() != 1) {
		std::cerr << "wrong number of functions found" << std::endl;
		exit(1);
	}

	Function noargRefl = functions.front();

	test_functions::resetCounter();
	clock_t start = clock();

	TestStruct t1, t2, t3;
	for (int i = 0; i < times; ++i) {
		test_functions::structArgRef3(t1, t2, t3);
	}

	clock_t final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "direct 3 struct arg = " << (final - start) << std::endl;

	test_functions::resetCounter();

	start = clock();

	std::vector<VariantValue> args = { TestStruct(), TestStruct(), TestStruct() };

	for (int i = 0; i < times; ++i) {
		noargRefl.callArgArray(args);
	}

	final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "reflective 3 struct arg = " << (final - start) << std::endl;

}

void structArgRef4Test()
{
	using namespace test_functions;
	std::list<Function> functions = Function::findFunctions("test_functions::structArgRef4");

	if (functions.size() != 1) {
		std::cerr << "wrong number of functions found" << std::endl;
		exit(1);
	}

	Function noargRefl = functions.front();

	test_functions::resetCounter();
	clock_t start = clock();

	TestStruct t1, t2, t3, t4;
	for (int i = 0; i < times; ++i) {
		test_functions::structArgRef4(t1, t2, t3, t4);
	}

	clock_t final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "direct 4 struct arg = " << (final - start) << std::endl;

	test_functions::resetCounter();

	start = clock();

	std::vector<VariantValue> args = { TestStruct(), TestStruct(), TestStruct(), TestStruct() };

	for (int i = 0; i < times; ++i) {
		noargRefl.callArgArray(args);
	}

	final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "reflective 4 struct arg = " << (final - start) << std::endl;

}

void structArgRef5Test()
{
	using namespace test_functions;
	std::list<Function> functions = Function::findFunctions("test_functions::structArgRef5");

	if (functions.size() != 1) {
		std::cerr << "wrong number of functions found" << std::endl;
		exit(1);
	}

	Function noargRefl = functions.front();

	test_functions::resetCounter();
	clock_t start = clock();

	TestStruct t1, t2, t3, t4, t5;
	for (int i = 0; i < times; ++i) {
		test_functions::structArgRef5(t1, t2, t3, t4, t5);
	}

	clock_t final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "direct 5 struct arg = " << (final - start) << std::endl;

	test_functions::resetCounter();

	start = clock();

	std::vector<VariantValue> args = { TestStruct(), TestStruct(), TestStruct(), TestStruct(), TestStruct() };

	for (int i = 0; i < times; ++i) {
		noargRefl.callArgArray(args);
	}

	final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "reflective 5 struct arg = " << (final - start) << std::endl;

}

void structArgRef6Test()
{
	using namespace test_functions;
	std::list<Function> functions = Function::findFunctions("test_functions::structArgRef6");

	if (functions.size() != 1) {
		std::cerr << "wrong number of functions found" << std::endl;
		exit(1);
	}

	Function noargRefl = functions.front();

	test_functions::resetCounter();
	clock_t start = clock();

	TestStruct t1, t2, t3, t4, t5, t6;
	for (int i = 0; i < times; ++i) {
		test_functions::structArgRef6(t1, t2, t3, t4, t5, t6);
	}

	clock_t final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "direct 6 struct arg = " << (final - start) << std::endl;

	test_functions::resetCounter();

	start = clock();

	std::vector<VariantValue> args = { TestStruct(), TestStruct(), TestStruct(), TestStruct(), TestStruct(), TestStruct() };

	for (int i = 0; i < times; ++i) {
		noargRefl.callArgArray(args);
	}

	final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "reflective 6 struct arg = " << (final - start) << std::endl;

}

void structArgRef7Test()
{
	using namespace test_functions;
	std::list<Function> functions = Function::findFunctions("test_functions::structArgRef7");

	if (functions.size() != 1) {
		std::cerr << "wrong number of functions found" << std::endl;
		exit(1);
	}

	Function noargRefl = functions.front();

	test_functions::resetCounter();
	clock_t start = clock();

	TestStruct t1, t2, t3, t4, t5, t6, t7;
	for (int i = 0; i < times; ++i) {
		test_functions::structArgRef7(t1, t2, t3, t4, t5, t6, t7);
	}

	clock_t final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "direct 7 struct arg = " << (final - start) << std::endl;

	test_functions::resetCounter();

	start = clock();

	std::vector<VariantValue> args = { TestStruct(), TestStruct(), TestStruct(), TestStruct(), TestStruct(), TestStruct(), TestStruct() };

	for (int i = 0; i < times; ++i) {
		noargRefl.callArgArray(args);
	}

	final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "reflective 7 struct arg = " << (final - start) << std::endl;

}


void structArgRef8Test()
{
	using namespace test_functions;
	std::list<Function> functions = Function::findFunctions("test_functions::structArgRef8");

	if (functions.size() != 1) {
		std::cerr << "wrong number of functions found" << std::endl;
		exit(1);
	}

	Function noargRefl = functions.front();

	test_functions::resetCounter();
	clock_t start = clock();

	TestStruct t1, t2, t3, t4, t5, t6, t7, t8;
	for (int i = 0; i < times; ++i) {
		test_functions::structArgRef8(t1, t2, t3, t4, t5, t6, t7, t8);
	}

	clock_t final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "direct 8 struct arg = " << (final - start) << std::endl;

	test_functions::resetCounter();

	start = clock();

	std::vector<VariantValue> args = { TestStruct(), TestStruct(), TestStruct(), TestStruct(), TestStruct(), TestStruct(), TestStruct(), TestStruct() };

	for (int i = 0; i < times; ++i) {
		noargRefl.callArgArray(args);
	}

	final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "reflective 8 struct arg = " << (final - start) << std::endl;

}


void structArgRef9Test()
{
	using namespace test_functions;
	std::list<Function> functions = Function::findFunctions("test_functions::structArgRef9");

	if (functions.size() != 1) {
		std::cerr << "wrong number of functions found" << std::endl;
		exit(1);
	}

	Function noargRefl = functions.front();

	test_functions::resetCounter();
	clock_t start = clock();

	TestStruct t1, t2, t3, t4, t5, t6, t7, t8, t9;
	for (int i = 0; i < times; ++i) {
		test_functions::structArgRef9(t1, t2, t3, t4, t5, t6, t7, t8, t9);
	}

	clock_t final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "direct 9 struct arg = " << (final - start) << std::endl;

	test_functions::resetCounter();

	start = clock();

	std::vector<VariantValue> args = { TestStruct(), TestStruct(), TestStruct(), TestStruct(), TestStruct(), TestStruct(), TestStruct(), TestStruct(), TestStruct() };

	for (int i = 0; i < times; ++i) {
		noargRefl.callArgArray(args);
	}

	final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "reflective 9 struct arg = " << (final - start) << std::endl;

}


void arg1Conversiontest()
{
	std::list<Function> functions = Function::findFunctions("test_functions::intarg1");

	if (functions.size() != 1) {
		std::cerr << "wrong number of functions found" << std::endl;
		exit(1);
	}

	Function reflFunc = functions.front();

	test_functions::resetCounter();

	clock_t start = clock();

	double val = 99.45;

	for (int i = 0; i < times; ++i) {
		test_functions::intarg1(val);
	}

	clock_t final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "direct 1 args = " << (final - start) << std::endl;

	test_functions::resetCounter();

	start = clock();

	std::vector<VariantValue> args = { val };

	for (int i = 0; i < times; ++i) {
		reflFunc.callArgArray(args);
	}

	final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "reflective 1 args = " << (final - start) << std::endl;

}


void polyArgRef1Test()
{
	using namespace test_functions;
	std::list<Function> functions = Function::findFunctions("test_functions::polyArg1");

	if (functions.size() != 1) {
		std::cerr << "wrong number of functions found" << std::endl;
		exit(1);
	}

	Function noargRefl = functions.front();

	test_functions::resetCounter();


	clock_t start = clock();

	Derived t1;
	for (int i = 0; i < times; ++i) {
		test_functions::polyArg1(t1);
	}

	clock_t final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "direct 1 poly arg = " << (final - start) << std::endl;

	test_functions::resetCounter();

	start = clock();

	std::vector<VariantValue> args = { Derived() };

	for (int i = 0; i < times; ++i) {
		noargRefl.callArgArray(args);
	}

	final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "reflective 1 poly arg = " << (final - start) << std::endl;
}

void polyArgRef2Test()
{
	using namespace test_functions;
	std::list<Function> functions = Function::findFunctions("test_functions::polyArg2");

	if (functions.size() != 1) {
		std::cerr << "wrong number of functions found" << std::endl;
		exit(1);
	}

	Function noargRefl = functions.front();

	test_functions::resetCounter();


	clock_t start = clock();

	Derived t1, t2;
	for (int i = 0; i < times; ++i) {
		test_functions::polyArg2(t1, t2);
	}

	clock_t final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "direct 2 poly arg = " << (final - start) << std::endl;

	test_functions::resetCounter();

	start = clock();

	std::vector<VariantValue> args = { Derived(), Derived() };

	for (int i = 0; i < times; ++i) {
		noargRefl.callArgArray(args);
	}

	final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "reflective 2 poly arg = " << (final - start) << std::endl;
}

void polyArgRef3Test()
{
	using namespace test_functions;
	std::list<Function> functions = Function::findFunctions("test_functions::polyArg3");

	if (functions.size() != 1) {
		std::cerr << "wrong number of functions found" << std::endl;
		exit(1);
	}

	Function noargRefl = functions.front();

	test_functions::resetCounter();


	clock_t start = clock();

	Derived t1, t2, t3;
	for (int i = 0; i < times; ++i) {
		test_functions::polyArg3(t1, t2, t3);
	}

	clock_t final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "direct 3 poly arg = " << (final - start) << std::endl;

	test_functions::resetCounter();

	start = clock();

	std::vector<VariantValue> args = { Derived(), Derived(), Derived() };

	for (int i = 0; i < times; ++i) {
		noargRefl.callArgArray(args);
	}

	final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "reflective 3 poly arg = " << (final - start) << std::endl;
}

void polyArgRef4Test()
{
	using namespace test_functions;
	std::list<Function> functions = Function::findFunctions("test_functions::polyArg4");

	if (functions.size() != 1) {
		std::cerr << "wrong number of functions found" << std::endl;
		exit(1);
	}

	Function noargRefl = functions.front();

	test_functions::resetCounter();


	clock_t start = clock();

	Derived t1, t2, t3, t4;
	for (int i = 0; i < times; ++i) {
		test_functions::polyArg4(t1, t2, t3, t4);
	}

	clock_t final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "direct 4 poly arg = " << (final - start) << std::endl;

	test_functions::resetCounter();

	start = clock();

	std::vector<VariantValue> args = { Derived(), Derived(), Derived(), Derived() };

	for (int i = 0; i < times; ++i) {
		noargRefl.callArgArray(args);
	}

	final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "reflective 4 poly arg = " << (final - start) << std::endl;
}

void polyArgRef5Test()
{
	using namespace test_functions;
	std::list<Function> functions = Function::findFunctions("test_functions::polyArg5");

	if (functions.size() != 1) {
		std::cerr << "wrong number of functions found" << std::endl;
		exit(1);
	}

	Function noargRefl = functions.front();

	test_functions::resetCounter();


	clock_t start = clock();

	Derived t1, t2, t3, t4, t5;
	for (int i = 0; i < times; ++i) {
		test_functions::polyArg5(t1, t2, t3, t4, t5);
	}

	clock_t final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "direct 5 poly arg = " << (final - start) << std::endl;

	test_functions::resetCounter();

	start = clock();

	std::vector<VariantValue> args = { Derived(), Derived(), Derived(), Derived(), Derived() };

	for (int i = 0; i < times; ++i) {
		noargRefl.callArgArray(args);
	}

	final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "reflective 5 poly arg = " << (final - start) << std::endl;
}

void polyArgRef6Test()
{
	using namespace test_functions;
	std::list<Function> functions = Function::findFunctions("test_functions::polyArg6");

	if (functions.size() != 1) {
		std::cerr << "wrong number of functions found" << std::endl;
		exit(1);
	}

	Function noargRefl = functions.front();

	test_functions::resetCounter();


	clock_t start = clock();

	Derived t1, t2, t3, t4, t5, t6;
	for (int i = 0; i < times; ++i) {
		test_functions::polyArg6(t1, t2, t3, t4, t5, t6);
	}

	clock_t final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "direct 6 poly arg = " << (final - start) << std::endl;

	test_functions::resetCounter();

	start = clock();

	std::vector<VariantValue> args = { Derived(), Derived(), Derived(), Derived(), Derived(), Derived() };

	for (int i = 0; i < times; ++i) {
		noargRefl.callArgArray(args);
	}

	final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "reflective 6 poly arg = " << (final - start) << std::endl;
}

void polyArgRef7Test()
{
	using namespace test_functions;
	std::list<Function> functions = Function::findFunctions("test_functions::polyArg7");

	if (functions.size() != 1) {
		std::cerr << "wrong number of functions found" << std::endl;
		exit(1);
	}

	Function noargRefl = functions.front();

	test_functions::resetCounter();


	clock_t start = clock();

	Derived t1, t2, t3, t4, t5, t6, t7;
	for (int i = 0; i < times; ++i) {
		test_functions::polyArg7(t1, t2, t3, t4, t5, t6, t7);
	}

	clock_t final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "direct 7 poly arg = " << (final - start) << std::endl;

	test_functions::resetCounter();

	start = clock();

	std::vector<VariantValue> args = { Derived(), Derived(), Derived(), Derived(), Derived(), Derived(), Derived() };

	for (int i = 0; i < times; ++i) {
		noargRefl.callArgArray(args);
	}

	final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "reflective 7 poly arg = " << (final - start) << std::endl;
}

void polyArgRef8Test()
{
	using namespace test_functions;
	std::list<Function> functions = Function::findFunctions("test_functions::polyArg8");

	if (functions.size() != 1) {
		std::cerr << "wrong number of functions found" << std::endl;
		exit(1);
	}

	Function noargRefl = functions.front();

	test_functions::resetCounter();


	clock_t start = clock();

	Derived t1, t2, t3, t4, t5, t6, t7, t8;
	for (int i = 0; i < times; ++i) {
		test_functions::polyArg8(t1, t2, t3, t4, t5, t6, t7, t8);
	}

	clock_t final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "direct 8 poly arg = " << (final - start) << std::endl;

	test_functions::resetCounter();

	start = clock();

	std::vector<VariantValue> args = { Derived(), Derived(), Derived(), Derived(), Derived(), Derived(), Derived(), Derived() };

	for (int i = 0; i < times; ++i) {
		noargRefl.callArgArray(args);
	}

	final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "reflective 8 poly arg = " << (final - start) << std::endl;
}

void polyArgRef9Test()
{
	using namespace test_functions;
	std::list<Function> functions = Function::findFunctions("test_functions::polyArg9");

	if (functions.size() != 1) {
		std::cerr << "wrong number of functions found" << std::endl;
		exit(1);
	}

	Function noargRefl = functions.front();

	test_functions::resetCounter();


	clock_t start = clock();

	Derived t1, t2, t3, t4, t5, t6, t7, t8, t9;
	for (int i = 0; i < times; ++i) {
		test_functions::polyArg9(t1, t2, t3, t4, t5, t6, t7, t8, t9);
	}

	clock_t final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "direct 9 poly arg = " << (final - start) << std::endl;

	test_functions::resetCounter();

	start = clock();

	std::vector<VariantValue> args = { Derived(), Derived(), Derived(), Derived(), Derived(), Derived(), Derived(), Derived(), Derived() };

	for (int i = 0; i < times; ++i) {
		noargRefl.callArgArray(args);
	}

	final = clock();

	if (test_functions::getCounter() != times) {
		std::cerr << "wrong counter" << std::endl;
		exit(1);
	}

	std::cout << "reflective 9 poly arg = " << (final - start) << std::endl;
}


int main()
{

	std::cout << "0 args function call:" << std::endl;

	noargtest();

	std::cout << "1 args function call:" << std::endl;

	arg1test();

	std::cout << "2 args function call:" << std::endl;

	arg2test();

	std::cout << "3 args function call:" << std::endl;

	arg3test();

	std::cout << "4 args function call:" << std::endl;
	arg4test();

	std::cout << "5 args function call:" << std::endl;
	arg5test();

	std::cout << "6 args function call:" << std::endl;
	arg6test();

	std::cout << "7 args function call:" << std::endl;
	arg7test();

	std::cout << "8 args function call:" << std::endl;
	arg8test();

	std::cout << "9 args function call:" << std::endl;
	arg9test();

	std::cout << "1 args function call with conversion:" << std::endl;

	arg1test();

	std::cout << "1 struct args by copy function call:" << std::endl;
	structArgCpy1Test();

	std::cout << "2 struct args by copy function call:" << std::endl;
	structArgCpy2Test();

	std::cout << "3 struct args by copy function call:" << std::endl;
	structArgCpy3Test();

	std::cout << "4 struct args by copy function call:" << std::endl;
	structArgCpy4Test();

	std::cout << "5 struct args by copy function call:" << std::endl;
	structArgCpy5Test();

	std::cout << "6 struct args by copy function call:" << std::endl;
	structArgCpy6Test();

	std::cout << "7 struct args by copy function call:" << std::endl;
	structArgCpy7Test();

	std::cout << "8 struct args by copy function call:" << std::endl;
	structArgCpy8Test();

	std::cout << "9 struct args by copy function call:" << std::endl;
	structArgCpy9Test();

	std::cout << "1 struct args by ref function call:" << std::endl;
	structArgRef1Test();

	std::cout << "2 struct args by ref function call:" << std::endl;
	structArgRef2Test();

	std::cout << "3 struct args by ref function call:" << std::endl;
	structArgRef3Test();

	std::cout << "4 struct args by ref function call:" << std::endl;
	structArgRef4Test();

	std::cout << "5 struct args by ref function call:" << std::endl;
	structArgRef5Test();

	std::cout << "6 struct args by ref function call:" << std::endl;
	structArgRef6Test();

	std::cout << "7 struct args by ref function call:" << std::endl;
	structArgRef7Test();

	std::cout << "8 struct args by ref function call:" << std::endl;
	structArgRef8Test();

	std::cout << "9 struct args by ref function call:" << std::endl;
	structArgRef9Test();

	std::cout << "1 poly args by ref function call:" << std::endl;
	polyArgRef1Test();

	std::cout << "2 poly args by ref function call:" << std::endl;
	polyArgRef2Test();

	std::cout << "3 poly args by ref function call:" << std::endl;
	polyArgRef3Test();

	std::cout << "4 poly args by ref function call:" << std::endl;
	polyArgRef4Test();

	std::cout << "5 poly args by ref function call:" << std::endl;
	polyArgRef5Test();

	std::cout << "6 poly args by ref function call:" << std::endl;
	polyArgRef6Test();

	std::cout << "7 poly args by ref function call:" << std::endl;
	polyArgRef7Test();

	std::cout << "8 poly args by ref function call:" << std::endl;
	polyArgRef8Test();

	std::cout << "9 poly args by ref function call:" << std::endl;
	polyArgRef9Test();

	return 0;
}
