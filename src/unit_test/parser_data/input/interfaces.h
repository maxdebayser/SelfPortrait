#ifndef INTERFACE_H
#define INTERFACE_H

/* This is not actually a source file. Its an input file
 * for the parsers' unit test
 *
 * This is a very simple test without other includes.
 */


/* We consider a class an interface if:
 * 1) It has no attributes
 * 2) It has only public pure virtual methods
 * 3) It has a public default constructor
 * 4) It has a public virtual destructor
 * 5) It inherits only from interfaces
 * 6) It has no inner classes
 *
 * Other constuctors are allowed.
 */

class Interface1 {
public:
	virtual ~Interface1();
	virtual int method1() = 0;
	virtual int method1(int a1) = 0;
	virtual int method1(int a1) const = 0;
};

class Interface2 {
public:
	virtual ~Interface2();
	virtual int method1() = 0;
	Interface2();
	Interface2(int a1);
};

class Interface3: public Interface1, public Interface2 {
public:
};

class NotAnInterface1 {
public:
	~NotAnInterface1(); // destructor is not virtual
	virtual int method1() = 0;
};

class NotAnInterface2 {
	virtual ~NotAnInterface2(); // destructor is not public
public:
	virtual int method1() = 0;
};

class NotAnInterface3 {
public:
	virtual ~NotAnInterface3();
	virtual int method2() = 0;
private: // has private methods
	virtual int method1() = 0;
};

class NotAnInterface4 {
public:
	virtual ~NotAnInterface4();
	virtual int method2() = 0;
// has attributes
	int attr2;
};


class NotAnInterface5 {
public:
	virtual ~NotAnInterface5();
	virtual int method2() = 0;
// has inner class definition
	struct Inner {
		int i1;
	};
};

class NotAnInterface6 {
public:
	virtual ~NotAnInterface6();
	virtual int method2() = 0;
private:
	NotAnInterface6(); // nas no public default constructor
};

class NotAnInterface7: public Interface1, public NotAnInterface1 {
public:
	// inherits from non-interface
};

class NotAnInterface8 {
public:
	virtual ~NotAnInterface8();
	int method1(); // has non-virtual method
	virtual int method1(int a1) = 0;
	virtual int method1(int a1) const = 0;
};

class NotAnInterface9: private Interface1 {
public:
	// inherits privately from interface
};

namespace NS {
	class Interface1 {
	public:
		virtual ~Interface1();
		virtual int method1() = 0;
		virtual int method1(int a1) = 0;
		virtual int method1(int a1) const = 0;
	};

	class Impl1: public Interface2 {

	};

	class Impl2: public Interface1 {

	};
}


class Impl3: public Interface2 {

};


class Impl4: public NS::Interface1 {

};

#endif
