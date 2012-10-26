#include "reflection_impl.h"
#include "../input/templates.h"

REFL_BEGIN_CLASS(Foo<double>)
REFL_ATTRIBUTE(t, double)
REFL_ATTRIBUTE(count, int)
REFL_END_CLASS

REFL_BEGIN_CLASS(Foo<char>)
REFL_ATTRIBUTE(t, char)
REFL_ATTRIBUTE(count, int)
REFL_END_CLASS

REFL_BEGIN_CLASS(Foo<int>)
REFL_ATTRIBUTE(t, int)
REFL_ATTRIBUTE(count, int)
REFL_END_CLASS

REFL_BEGIN_CLASS(Foo<long>)
REFL_ATTRIBUTE(t, long)
REFL_ATTRIBUTE(count, int)
REFL_END_CLASS

REFL_BEGIN_CLASS(Foo<char *>)
REFL_ATTRIBUTE(t, char *)
REFL_ATTRIBUTE(count, int)
REFL_END_CLASS

REFL_BEGIN_CLASS(Foo<int *>)
REFL_ATTRIBUTE(t, int *)
REFL_ATTRIBUTE(count, int)
REFL_END_CLASS

REFL_BEGIN_CLASS(Foo<long *>)
REFL_ATTRIBUTE(t, long *)
REFL_ATTRIBUTE(count, int)
REFL_END_CLASS

REFL_BEGIN_CLASS(Foo<double *>)
REFL_ATTRIBUTE(t, double *)
REFL_ATTRIBUTE(count, int)
REFL_END_CLASS

REFL_BEGIN_CLASS(Bar)
REFL_ATTRIBUTE(d, Foo<double>)
REFL_END_CLASS

REFL_FUNCTION(function1, void, Foo<char> *)

REFL_FUNCTION(function2, void, Foo<int>)

REFL_FUNCTION(function3, void, Foo<long> &)

REFL_FUNCTION(function4, Foo<char *>)

REFL_FUNCTION(function5, Foo<int *>)

REFL_FUNCTION(function6, Foo<long *>)

REFL_FUNCTION(tfuntion<float>, void, const float &)

