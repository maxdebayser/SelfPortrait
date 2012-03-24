#include<iostream>
#include <cxxabi.h>
#include <vector>

#include "typelist.h"
#include "class.h"
#include "variant.h"
#include "method.h"
#include "constructor.h"
#include "reflection.h"
#include "attribute.h"
using namespace std;





struct lalala {	
	
	lalala() : attr1(3), attr2(3.14) {}
	
	lalala(const lalala& rhs) : attr1(rhs.attr1), attr2(rhs.attr2) {}
	lalala& operator=(const lalala& rhs) { attr1 = rhs.attr1; return *this; }
	
	operator ::std::string() const {
			return "eu sou lalala" ;
	}
	
	int attr1;
	const float attr2;
	
	int method1(float f, double d) { return f*d; } 
	int method2(float f, double d) const { return f+d; }
	int method3(float f, double d) volatile { return f-d; }
	double method4(float f, double d) const volatile { return f/d; }
	
};

struct Test {
	
	int vanillaMethod(int, double);
	int constMethod(int, double) const;
	int volatileMethod(int, double) volatile;
	int constVolatileMethod(int, double) const volatile;
};

template<class T>
struct ClassBuilder {
	ClassBuilder() {
		ClassImpl<T>* instance = ClassImpl<T>::instance();
		
		
		
	}
};

int main()
{;
	
 	cout << "TypeList size = " << size<TypeList<int, void, double>>() << ", is int:" << same_type<int, type_at<TypeList<int, void>, 0>::type>() << ", contains int " << contains<TypeList<int, double>, int>() << endl;
	cout << same_type< TypeList<void>, tail_at<TypeList<double, int, void>, 0>::type   >() << endl;
	cout << same_type< TypeList<void, void>, prepend<void,TypeList<void>>::type   >() << endl;
	
	cout << same_type< TypeList<int, double, char, void>, concat<TypeList<int, double>,TypeList<char,void>>::type   >() << endl;
	
	cout << "indices " << same_type< Indices<0,1,2>, make_indices<3>::type>() << endl;
	cout << "indices " << same_type< Indices<>, make_indices<0>::type>() << endl;
	
	cout << same_type< TypeList<int, double,float>   , sublist_up_to<TypeList<int, double, float>, 2>::type >() << endl;
	
	cout << same_type< TypeList<int, float>   , selection<TypeList<int, double, float>, Indices<0,2>>::type >() << endl;
	
	cout << same_type< TypeList<double, float>   , erase_at<TypeList<int, double, float>, 0>::type >() << endl;
	
	//static_assert(same_type< TypeList<int, float>   , selection<TypeList<int, double, float>, Indices<0,1>>::type >(), "Erro");
	
	VariantValue v;
	cout << "v.isValid() = " << v.isValid() << endl;

	v = 3;
	
	cout << "alignof = " << v.alignOf() << endl;
	
	cout << "v.isValid() = " << v.isValid() << endl;
	cout << "v.isA<int>() = " << v.isA<int>() << endl;
	cout << "v.isA<float>() = " << v.isA<float>() << endl;
	cout << "v.value<int>() = " << v.convertTo<int>() << endl;
	cout << "is printable: " << convertible_to_string<int>() << endl;
	cout << "is readable: " << convertible_from_string<int>() << endl;
	cout << "convertible: " << ::std::is_convertible<const char *, ::std::string>::value << endl;
	
	string s = v.convertTo<string>();
	cout << "s = " << s << endl;
	
	double d = v.convertTo<double>();
	cout << "d = " << d << endl;
	
	int i = fromString<int>("34");
	cout << "i = " << i << endl;
	
	lalala l;
	cout << toString(l) << endl;
	
	bool success = true;
	l = v.convertTo<lalala>(&success);
	cout << "success = " << success << endl;
	
	//cout << sizeof( cout << (*((lalala*)(0)) )) << endl;
	
	cout << "is volatile " <<  method_type<decltype(&Test::constVolatileMethod)>::is_volatile << endl;
	
	std::vector<const std::type_info*> tv = get_typeinfo<TypeList<int, double>>();
	cout << "list size  = " << tv.size() << endl;
	cout << "first is int = " << (*tv[0] == typeid(int)) << endl;
	cout << "second is double = " << (*tv[1] == typeid(double)) << endl;
	
	
	auto method1 = make_method("method1", &lalala::method1);
	auto method2 = make_method("method2", &lalala::method2);
	auto method3 = make_method("method3", &lalala::method3);
	auto method4 = make_method("method4", &lalala::method4);
	auto attr1 = make_attribute("attr1", &lalala::attr1);
	auto attr2 = make_attribute("attr2", &lalala::attr2);
	auto constructor = make_constructor<lalala>();
	//MethodImpl<decltype(&lalala::method1)> method1("methdo1", &lalala::method1);
	
	//Object obj(new ObjectImpl<lalala>(new lalala()));
	Object obj = constructor.call();
	
	
	try {
	cout << "result1 = " << method1.call(obj, 3.0f, 4.0 ).value<int>() << endl;
	} catch(const ::std::exception& ex) { cout << "catch " << ex.what() << endl; }
	try {
	cout << "result2 = " << method2.call(obj, 3.0f, 4.0 ).value<int>() << endl;
	} catch(const ::std::exception& ex) { cout << "catch " << ex.what() << endl; }
	try {
	cout << "result3 = " << method3.call(obj, 3.0f, 4.0 ).value<int>() << endl;
	} catch(const ::std::exception& ex) { cout << "catch " << ex.what() << endl; }
	try {
	cout << "result4 = " << method4.call(obj, 3.0f, 4.0 ).value<double>() << endl;
	} catch(const ::std::exception& ex) { cout << "catch " << ex.what() << endl; }
	
	try {
		cout << "attr1 = " << attr1.get(obj).value<int>() << endl;
		attr1.set(obj, 5);
		cout << "attr1 = " << attr1.get(obj).value<int>() << endl;
	} catch(const ::std::exception& ex) { cout << "catch " << ex.what() << endl; }
	
	
		try {
		cout << "attr2 = " << attr2.get(obj).value<float>() << endl;
		attr2.set(obj, 5.4);
		cout << "attr2 = " << attr2.get(obj).value<float>() << endl;
	} catch(const ::std::exception& ex) { cout << "catch " << ex.what() << endl; }
	
	return 0;
}