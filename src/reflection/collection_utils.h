/*
** SelfPortrait API
** See Copyright Notice in reflection.h
*/
#ifndef COLL_UTILS_H
#define COLL_UTILS_H

#include "selfportrait_config.h"

inline void emplace(ArgArray& v )
{
	// nothing to do
}

template<class T, class... U>
inline void emplace(ArgArray& v, T&& t, U&&... u )
{
    v.emplace_back(t);
	emplace(v, u...);
}

inline void variant_construct(ArgArray& v )
{
    // nothing to do
}


template<class T, class... U>
inline void variant_construct(ArgArray& v, T&& t, U&&... u )
{
    v.emplace_back();
    v.back().construct<T>(t);
    variant_construct(v, u...);
}


#endif /* COLL_UTILS_H */
