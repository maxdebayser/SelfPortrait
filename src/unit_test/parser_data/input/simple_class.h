#ifndef SIMPLE_CLASS_H
#define SIMPLE_CLASS_H

/* This is not actually a source file. Its an input file
 * for the parsers' unit test
 *
 * This is a very simple test without other includes.
 */

struct POCO {
	int a;
	char b;
};

// TODO: test private destructor


class Foo {
private:
	int attr1;
	double attr2;
protected:
	char attr3;
public:
	float attr4;
	const int attr5;
	static int att6;
	static const int att7;
	int& attr8;
	const int& attr9;
	const int* attr10;

	Foo();

	Foo(int a1);

	Foo(int a1, double* a2);

	Foo(const Foo& a1);

	Foo& operator=(const Foo& a1);

	Foo(Foo&& a1);

	Foo& operator=(Foo&& a1);

private:
	Foo(char a1);

protected:
	Foo(long a1);

public:

	int method1();

	int method1() const;

	int method1(int a1);

	int method2() const;

	int method3() const volatile;

	void method4(double a1, char& a2);

	static int method5(const int& i);

};


namespace NS {

	class Foo{
	public:
		Foo(int);
	};

	namespace NestedNS {
		class Foo{
		public:
			Foo();
			int method(double a1);
		};
	}
}

#endif
