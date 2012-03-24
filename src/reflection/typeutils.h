#ifndef TYPE_UTILS_H
#define TYPE_UTILS_H

#include <type_traits>

template<class U, class V>
constexpr bool same_type()
{
	return ::std::is_same<U,V>::value;
}

template<bool B, typename U, typename V>
struct Select {
	typedef V result;
};

template<typename U, typename V>
struct Select<true, U, V>
{
	typedef U result;
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
#endif
