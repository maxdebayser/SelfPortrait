#ifndef TEMPLATES_H
#define TEMPLATES_H

/* This is not actually a source file. Its an input file
 * for the parsers' unit test
 *
 * This is a very simple test without other includes.
 */

template<class T>
struct Foo {
	T t;
	int count;
};

struct Bar {
public:
	// a member always forces a specialization
	Foo<double> d;
};

// Argument passing only introduces a specialization if the instantiateTemplates switch is used
void function1(Foo<char>* f);
void function2(Foo<int> f);
void function3(Foo<long>& f);

// Returning passing only introduces a specialization if the instantiateTemplates switch is used

Foo<char*> function4();
Foo<int*> function5();
Foo<long*> function6();

// we can also force an instantiation

template class Foo<double*>;

template<class T>
void tfuntion(const T& t) { }

template void tfuntion<float>(const float& t);

#endif
