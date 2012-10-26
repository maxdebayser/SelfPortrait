#include "reflection_impl.h"
#include "../input/functions.h"

REFL_FUNCTION(function1, void)

REFL_FUNCTION(function2, int, double)

REFL_FUNCTION(function3, int, double, float *)

REFL_FUNCTION(function4, char *, double, float *, const long &)

REFL_FUNCTION(function4, char *, double, float *, const long &)

REFL_FUNCTION(function5, int, int, char **)

REFL_FUNCTION(NS::function2, int, double)

REFL_FUNCTION(NS::NestedNS::function2, int, double)


