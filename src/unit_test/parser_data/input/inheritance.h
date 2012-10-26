#ifndef INHERITANCE_H
#define INHERITANCE_H

/* This is not actually a source file. Its an input file
 * for the parsers' unit test
 *
 * This is a very simple test without other includes.
 */


class Base1 {
protected:
	Base1();
public:
	void method1();
};

class PBase1 {
protected:
	PBase1();
public:
	virtual void method1() = 0;
	int method2();
};

class Base2 {
public:
	Base2(int a1);

	void method2();
	void method3();
};

class Derived1: public Base1 {
public:
	void method1();
	void method2(int);
};

class Derived2: public Base1, public Base2 {
public:
	void method1();
	void method2(int);
};

class Derived3: public PBase1 {
public:
	void method1();
	int method2();
	int method2(int a1);
};

class Derived4: public PBase1, public Base2 {
public:
	void method1();
	int method2(int a1);
};

class Derived5: public PBase1 {
public:
	int method3(int a1, double a2);
private:
	int method4(int a1);
};

class Derived6: public Derived3, public Derived5 {
public:
	int method3(int a1, double a2);
};

#endif
