/*
** SelfPortrait API
** See Copyright Notice in reflection.h
*/
#ifndef TYPE_UTILS_H
#define TYPE_UTILS_H

#include <type_traits>
#include <vector>
#include <deque>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <utility>
#include <list>
#include <string>
#include <array>
#include <tuple>

template<bool B, typename U, typename V>
struct Select {
	typedef V type;
};

template<typename U, typename V>
struct Select<true, U, V>
{
	typedef U type;
};


template<class... T>
struct static_and;

template<>
struct static_and<std::true_type> {
    typedef std::true_type type;
};

template<>
struct static_and<std::false_type> {
    typedef std::false_type type;
};

template<class H, class... T>
struct static_and<H,T...> {
    static_assert(std::is_same<H, std::true_type>::value || std::is_same<H, std::false_type>::value, "");
    typedef typename Select<std::is_same<H, std::true_type>::value, typename static_and<T...>::type, std::false_type>::type type;
};


template<class T>
struct equal_test_helper {
    template<class U, class = decltype(std::declval<U>() == std::declval<U>() )>
    static std::true_type  supports_equal_test(const U&);

    static std::false_type supports_equal_test(...);
};

// disable all templates
template<template <typename...> class templClass, typename... T>
struct equal_test_helper<templClass<T...>> {
    template <class U>
    static std::false_type supports_equal_test(const U&);
};

// enable basic_string if value_type is comparable
template<class T, class Traits, class Alloc>
struct equal_test_helper<std::basic_string<T, Traits, Alloc>> {
    static decltype(equal_test_helper<T>::supports_equal_test(std::declval<T>()))  supports_equal_test(const std::basic_string<T, Traits, Alloc>&);
};


// enable vector if value_type is comparable
template<class T, class Alloc>
struct equal_test_helper<std::vector<T, Alloc>> {
    static decltype(equal_test_helper<T>::supports_equal_test(std::declval<T>()))  supports_equal_test(const std::vector<T, Alloc>&);
};

// enable list if value_type is comparable
template<class T, class Alloc>
struct equal_test_helper<std::list<T, Alloc>> {
    static decltype(equal_test_helper<T>::supports_equal_test(std::declval<T>()))  supports_equal_test(const std::list<T, Alloc>&);
};

// enable array if value_type is comparable
template<class T, std::size_t N>
struct equal_test_helper<std::array<T, N>> {
    static decltype(equal_test_helper<T>::supports_equal_test(std::declval<T>()))  supports_equal_test(const std::array<T, N>&);
};

// enable set if value_type is comparable
template<class T, class Comp, class Alloc>
struct equal_test_helper<std::set<T, Comp, Alloc>> {
    static decltype(equal_test_helper<T>::supports_equal_test(std::declval<T>()))  supports_equal_test(const std::set<T, Comp, Alloc>&);
};


// enable set if value_type is comparable
template<class T, class Comp, class Alloc>
struct equal_test_helper<std::multiset<T, Comp, Alloc>> {
    static decltype(equal_test_helper<T>::supports_equal_test(std::declval<T>()))  supports_equal_test(const std::multiset<T, Comp, Alloc>&);
};


// enable set if value_type is comparable
template<class T, class Hash, class Pred, class Alloc>
struct equal_test_helper<std::unordered_set<T, Hash, Pred, Alloc>> {
    static decltype(equal_test_helper<T>::supports_equal_test(std::declval<T>()))  supports_equal_test(const std::unordered_set<T, Hash, Pred, Alloc>&);
};


// enable set if value_type is comparable
template<class T, class Hash, class Pred, class Alloc>
struct equal_test_helper<std::unordered_multiset<T, Hash, Pred, Alloc>> {
    static decltype(equal_test_helper<T>::supports_equal_test(std::declval<T>()))  supports_equal_test(const std::unordered_multiset<T, Hash, Pred, Alloc>&);
};

// enable pair if value_type is comparable
template<class T, class S>
struct equal_test_helper<std::pair<T, S>> {
    static typename static_and<decltype(equal_test_helper<T>::supports_equal_test(std::declval<T>())), decltype(equal_test_helper<S>::supports_equal_test(std::declval<S>()))>::type supports_equal_test(const std::pair<T, S>&);
};

// enable map if value_type is comparable
template<class T, class S, class Comp, class Alloc>
struct equal_test_helper<std::map<T, S, Comp, Alloc>> {
    static typename static_and<decltype(equal_test_helper<T>::supports_equal_test(std::declval<T>())), decltype(equal_test_helper<S>::supports_equal_test(std::declval<S>()))>::type supports_equal_test(const std::map<T, S, Comp, Alloc>&);
};

// enable map if value_type is comparable
template<class T, class S, class Comp, class Alloc>
struct equal_test_helper<std::multimap<T, S, Comp, Alloc>> {
    static typename static_and<decltype(equal_test_helper<T>::supports_equal_test(std::declval<T>())), decltype(equal_test_helper<S>::supports_equal_test(std::declval<S>()))>::type supports_equal_test(const std::multimap<T, S, Comp, Alloc>&);
};

// enable map if value_type is comparable
template<class T, class S, class Hash, class Pred, class Alloc>
struct equal_test_helper<std::unordered_map<T, S, Hash, Pred, Alloc>> {
    static typename static_and<decltype(equal_test_helper<T>::supports_equal_test(std::declval<T>())), decltype(equal_test_helper<S>::supports_equal_test(std::declval<S>()))>::type supports_equal_test(const std::unordered_map<T, S, Hash, Pred, Alloc>&);
};


// enable map if value_type is comparable
template<class T, class S, class Hash, class Pred, class Alloc>
struct equal_test_helper<std::unordered_multimap<T, S, Hash, Pred, Alloc>> {
    static typename static_and<decltype(equal_test_helper<T>::supports_equal_test(std::declval<T>())), decltype(equal_test_helper<S>::supports_equal_test(std::declval<S>()))>::type supports_equal_test(const std::unordered_multimap<T, S, Hash, Pred, Alloc>&);
};

// enable pair if value_type is comparable
template<class... T>
struct equal_test_helper<std::tuple<T...>> {
    static typename static_and<decltype(equal_test_helper<T>::supports_equal_test(std::declval<T>()))...>::type supports_equal_test(const std::tuple<T...>&);
};

template<class T> using comparable = decltype(equal_test_helper<T>::supports_equal_test(std::declval<T>()));


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
