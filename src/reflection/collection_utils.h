/*
** SelfPortrait API
** See Copyright Notice in reflection.h
*/
#ifndef COLL_UTILS_H
#define COLL_UTILS_H

#include "variant.h"
#include <vector>

inline void emplace(std::vector<VariantValue>& v )
{
	// nothing to do
}

template<class T, class... U>
inline void emplace(std::vector<VariantValue>& v, T&& t, U&&... u )
{
	v.emplace_back(t);
	emplace(v, u...);
}


#endif /* COLL_UTILS_H */
