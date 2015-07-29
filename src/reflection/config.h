/*
** SelfPortrait API
** See Copyright Notice in reflection.h
*/
#ifndef CONFIG_H
#define CONFIG_H

#include "variant.h"
#include <vector>
#ifdef USE_STD_VECTOR
#include <vector>
typedef ::std::vector<VariantValue> ArgArray;
#else
#include "SmallArray.h"
typedef SmallArray<VariantValue> ArgArray;
#endif
#endif /* CONFIG_H*/
