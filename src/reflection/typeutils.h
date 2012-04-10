#ifndef TYPE_UTILS_H
#define TYPE_UTILS_H

#include <type_traits>


template<bool B, typename U, typename V>
struct Select {
	typedef V type;
};

template<typename U, typename V>
struct Select<true, U, V>
{
	typedef U type;
};


namespace comparable_impl {

	typedef char no;
	typedef char yes[2];

	template<class T>
	no operator==( T const&, T const& );

	yes& test_eq( bool );
	no test_eq( no );

	template<typename T>
	struct test {
		static T const& t1;
		static T const& t2;
		static bool const value = sizeof( test_eq(t1 == t2) ) == sizeof( yes );
	};

}

template<typename T>
struct comparable {
	enum { value = comparable_impl::test<T>::value };
};

template<class T>
struct strip_reference {
	typedef T type;
	typedef T* ptr_type;
};

template<class T>
struct strip_reference<T&> {
	typedef T type;
	typedef T* ptr_type;
};


template<class T>
struct strip_reference<const T&> {
	typedef T type;
	typedef const T* ptr_type;

};

template<class T>
struct strip_reference<volatile T&> {
	typedef T type;
	typedef volatile T* ptr_type;
};


template<class T>
struct strip_reference<const volatile T&> {
	typedef T type;
	typedef const volatile T* ptr_type;
};

// the /dev/null of arguments
template<class... T>
inline void sink(T&&... t) {}

#endif
