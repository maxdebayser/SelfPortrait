/*
** SelfPortrait API
** See Copyright Notice in reflection.h
*/
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

template<class T, class = decltype(std::declval<T>() == std::declval<T>() )>
std::true_type  supports_equal_test(const T&);
std::false_type supports_equal_test(...);

template<class T> using comparable = decltype(supports_equal_test(std::declval<T>()));


template<class T>
struct normalize_type {
	typedef T type;
	typedef T* ptr_type;
	enum { is_const = false };
};

template<class T>
struct normalize_type<const T> {
	typedef T type;
	typedef const T* ptr_type;
	enum { is_const = true };
};

template<class T>
struct normalize_type<T*> {
	typedef T type;
	typedef T** ptr_type;
	enum { is_const = false };
};

template<class T>
struct normalize_type<const T*> {
	typedef T type;
	typedef const T** ptr_type;
	enum { is_const = true };
};

template<class T>
struct normalize_type<T&> {
	typedef T type;
	typedef T* ptr_type;
	enum { is_const = false };
};


template<class T>
struct normalize_type<T&&> {
	typedef T type;
	typedef T* ptr_type;
	enum { is_const = false };
};


template<class T>
struct normalize_type<const T&> {
	typedef T type;
	typedef const T* ptr_type;
	enum { is_const = true };

};

template<class T>
struct normalize_type<volatile T&> {
	typedef T type;
	typedef volatile T* ptr_type;
	enum { is_const = false };
};


template<class T>
struct normalize_type<const volatile T&> {
	typedef T type;
	typedef const volatile T* ptr_type;
	enum { is_const = true };
};


template<class T>
struct normalize_type<T[]> {
	typedef T type;
	typedef T** ptr_type;
	enum { is_const = false };
};

template<class T>
struct normalize_type<const T[]> {
	typedef T type;
	typedef const T** ptr_type;
	enum { is_const = true };
};

template<class T>
struct normalize_type<volatile T[]> {
	typedef T type;
	typedef volatile T** ptr_type;
	enum { is_const = false };
};

template<class T>
struct normalize_type<const volatile T[]> {
	typedef T type;
	typedef const volatile T** ptr_type;
	enum { is_const = true };
};

template<class T, std::size_t N>
struct normalize_type<T[N]> {
	typedef T type;
	typedef T** ptr_type;
	enum { is_const = false };
};

template<class T, std::size_t N>
struct normalize_type<const T[N]> {
	typedef T type;
	typedef const T** ptr_type;
	enum { is_const = true };
};

template<class T, std::size_t N>
struct normalize_type<volatile T[N]> {
	typedef T type;
	typedef volatile T** ptr_type;
	enum { is_const = false };
};

template<class T, std::size_t N>
struct normalize_type<const volatile T[N]> {
	typedef T type;
	typedef const volatile T** ptr_type;
	enum { is_const = true };
};


template<class T>
struct normalize_type<T(&)[]> {
	typedef T type;
	typedef T** ptr_type;
	enum { is_const = false };
};

template<class T>
struct normalize_type<const T(&)[]> {
	typedef T type;
	typedef const T** ptr_type;
	enum { is_const = true };
};

template<class T>
struct normalize_type<volatile T(&)[]> {
	typedef T type;
	typedef volatile T** ptr_type;
	enum { is_const = false };
};

template<class T>
struct normalize_type<const volatile T(&)[]> {
	typedef T type;
	typedef const volatile T** ptr_type;
	enum { is_const = true };
};

template<class T, std::size_t N>
struct normalize_type<T(&)[N]> {
	typedef T type;
	typedef T** ptr_type;
	enum { is_const = false };
};

template<class T, std::size_t N>
struct normalize_type<const T(&)[N]> {
	typedef T type;
	typedef const T** ptr_type;
	enum { is_const = true };
};

template<class T, std::size_t N>
struct normalize_type<volatile T(&)[N]> {
	typedef T type;
	typedef volatile T** ptr_type;
	enum { is_const = false };
};

template<class T, std::size_t N>
struct normalize_type<const volatile T(&)[N]> {
	typedef T type;
	typedef const volatile T** ptr_type;
	enum { is_const = true };
};


// the /dev/null of arguments
template<class... T>
inline void sink(T&&... t) {}


template<class T>
struct is_pointer_to_const {
	enum { value = false };
};

template<class T>
struct is_pointer_to_const<const T*> {
	enum { value = true };
};

#endif
