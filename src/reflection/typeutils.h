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

#endif