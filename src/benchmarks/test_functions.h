#ifndef TEST_FUNCTIONS_H
#define TEST_FUNCTIONS_H

namespace test_functions {

	long getCounter();
	void resetCounter();

	void noargs();

	void intarg1(int);
	void intarg2(int, int);
	void intarg3(int, int, int);
	void intarg4(int, int, int, int);
	void intarg5(int, int, int, int, int);
	void intarg6(int, int, int, int, int, int);
	void intarg7(int, int, int, int, int, int, int);
	void intarg8(int, int, int, int, int, int, int, int);
	void intarg9(int, int, int, int, int, int, int, int, int);


	struct TestStruct {
		int elem1;
		int elem2;
		int elem3;
		int elem4;
	};

	void structArgCpy1(TestStruct);
	void structArgCpy2(TestStruct, TestStruct);
	void structArgCpy3(TestStruct, TestStruct, TestStruct);
	void structArgCpy4(TestStruct, TestStruct, TestStruct, TestStruct);
	void structArgCpy5(TestStruct, TestStruct, TestStruct, TestStruct, TestStruct);
	void structArgCpy6(TestStruct, TestStruct, TestStruct, TestStruct, TestStruct, TestStruct);
	void structArgCpy7(TestStruct, TestStruct, TestStruct, TestStruct, TestStruct, TestStruct, TestStruct);
	void structArgCpy8(TestStruct, TestStruct, TestStruct, TestStruct, TestStruct, TestStruct, TestStruct, TestStruct);
	void structArgCpy9(TestStruct, TestStruct, TestStruct, TestStruct, TestStruct, TestStruct, TestStruct, TestStruct, TestStruct);


	void structArgRef1(const TestStruct&);
	void structArgRef2(const TestStruct&, const TestStruct&);
	void structArgRef3(const TestStruct&, const TestStruct&, const TestStruct&);
	void structArgRef4(const TestStruct&, const TestStruct&, const TestStruct&, const TestStruct&);
	void structArgRef5(const TestStruct&, const TestStruct&, const TestStruct&, const TestStruct&, const TestStruct&);
	void structArgRef6(const TestStruct&, const TestStruct&, const TestStruct&, const TestStruct&, const TestStruct&, const TestStruct&);
	void structArgRef7(const TestStruct&, const TestStruct&, const TestStruct&, const TestStruct&, const TestStruct&, const TestStruct&, const TestStruct&);
	void structArgRef8(const TestStruct&, const TestStruct&, const TestStruct&, const TestStruct&, const TestStruct&, const TestStruct&, const TestStruct&, const TestStruct&);
	void structArgRef9(const TestStruct&, const TestStruct&, const TestStruct&, const TestStruct&, const TestStruct&, const TestStruct&, const TestStruct&, const TestStruct&, const TestStruct&);

	struct Base {
		virtual ~Base() {}
	};

	struct Derived: public Base {
		virtual void foo() {}
	};

	void polyArg1(const Base&);
	void polyArg2(const Base&, const Base&);
	void polyArg3(const Base&, const Base&, const Base&);
	void polyArg4(const Base&, const Base&, const Base&, const Base&);
	void polyArg5(const Base&, const Base&, const Base&, const Base&, const Base&);
	void polyArg6(const Base&, const Base&, const Base&, const Base&, const Base&, const Base&);
	void polyArg7(const Base&, const Base&, const Base&, const Base&, const Base&, const Base&, const Base&);
	void polyArg8(const Base&, const Base&, const Base&, const Base&, const Base&, const Base&, const Base&, const Base&);
	void polyArg9(const Base&, const Base&, const Base&, const Base&, const Base&, const Base&, const Base&, const Base&, const Base&);
}


#endif /* TEST_FUNCTIONS_H */
