/*
** SelfPortrait API
** See Copyright Notice in reflection.h
*/
#ifndef TYPELIST_H
#define TYPELIST_H

#ifndef NO_RTTI
#include <typeinfo>
#endif
#include <typeindex>
#include <initializer_list>
#include <vector>
#include "typeutils.h"

namespace {

template<typename... T>
struct TypeList;


// base for the polymorphic typelist
template<>
struct TypeList<>
{
};

template<typename H, typename... T>
struct TypeList<H, T...>: public TypeList<T...>
{
	typedef H Head;
	typedef TypeList<T...> Tail;
};

template<std::size_t...>
struct Indices;

template<std::size_t N, class I>
struct make_indices_impl;

template<std::size_t N, template<std::size_t...> class I, std::size_t... indices>
struct make_indices_impl<N, I<indices...>> {
	typedef typename make_indices_impl<N-1, I<N-1, indices...>>::type type;
};

template<template<std::size_t...> class I, std::size_t... indices>
struct make_indices_impl<0, I<indices...>> {
	typedef Indices<indices...> type ;
};

template<std::size_t N>
struct make_indices {
	typedef typename make_indices_impl<N, Indices<>>::type type;
};

#ifndef NO_RTTI
template<class TL>
struct get_typeinfo_impl;

template<typename... T, template<typename...> class TL>
struct get_typeinfo_impl<TL<T...>> {
	//typedef ::std::initializer_list<const ::std::type_info*> tlist; // I wanted to return initializer_list, but had strange memory corruption issues	
	typedef ::std::vector<const ::std::type_info*> tlist;
	static tlist list() {
		return { (&typeid(T))... };
	}
};

template<class TL>
static typename get_typeinfo_impl<TL>::tlist get_typeinfo() {
		return get_typeinfo_impl<TL>::list();
}
#endif

template<typename NewHead, class TL>
struct prepend;

template<typename NewHead, typename... T, template<typename...> class TL>
struct prepend<NewHead, TL<T...>> {
		typedef TypeList<NewHead, T...> type;
};

template<typename TL1, class TL2>
struct concat;

template<typename... T1, template<typename...> class TL1, typename... T2, template<typename...> class TL2>
struct concat<TL1<T1...>, TL2<T2...>> {
		typedef TypeList<T1..., T2...> type;
};


template<class TL>
struct typelist_size;

template<typename... T, template<typename...> class TL>
struct typelist_size<TL<T...>> {
	enum { value = sizeof...(T) };
};

template<class TL>
constexpr std::size_t size()
{
	return typelist_size<TL>::value;
}


template<class TL, int N>
struct sublist_at {
	static_assert(N >= 0, "There are no types at negative indices");
	static_assert(N <= size<TL>(), "Index greater than type list size");
	typedef typename sublist_at<typename TL::Tail, N-1>::type type;
};

template<class TL>
struct sublist_at<TL,0> {
	typedef TL type;
};

template<class TL, int I>
struct type_at {
	typedef typename sublist_at<TL, I>::type::Head type;
};

template<class TL, int I>
struct tail_at {
	typedef typename sublist_at<TL, I>::type::Tail type;
};


template<class TL, class Is>
struct selection;

template<typename... T, std::size_t... I>
struct selection<TypeList<T...>, Indices<I...>> {
	typedef TypeList< typename type_at<TypeList<T...>,I>::type...> type;
};

template<class TL, std::size_t N>
struct sublist_up_to;

template<std::size_t N, typename... T>
struct sublist_up_to<TypeList<T...>, N> {
	 typedef typename selection<TypeList<T...>, typename make_indices<N+1>::type>::type type;
};


template<class TL, int I>
struct erase_at {
	typedef typename concat<typename sublist_up_to<TL, I-1>::type, typename tail_at<TL, I>::type>::type type;
};

template<class TL>
struct erase_at<TL,0> {
	typedef typename TL::Tail type;
};


template<class TL, int I>
constexpr const std::type_info& typeid_at()
{
	return typeid(typename type_at<TL,I>::type);
}

struct success_t
{
		enum { value = true };
};

template<class TL, class T>
struct search_type
{
		enum { value = Select<std::is_same<typename TL::Head, T>::value, success_t, search_type<typename TL::Tail, T>>::type::value };
};

template<class T>
struct search_type<TypeList<>, T>
{
		enum { value = false };
};


template<class TL, class T>
constexpr bool contains()
{
		return search_type<TL,T>::value;
}

}

#endif
