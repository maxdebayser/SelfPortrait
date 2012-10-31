#include "reflection_impl.h"
#include "../input/simple_class.h"

REFL_BEGIN_CLASS(POCO)
REFL_DEFAULT_CONSTRUCTOR()
REFL_ATTRIBUTE(a, int)
REFL_ATTRIBUTE(b, char)
REFL_END_CLASS

REFL_BEGIN_CLASS(Foo)
REFL_DEFAULT_CONSTRUCTOR()
REFL_CONSTRUCTOR(int)
REFL_CONSTRUCTOR(int, double *)
REFL_CONSTRUCTOR(const Foo &)
REFL_CONSTRUCTOR(Foo &&)
REFL_ATTRIBUTE(attr4, float)
REFL_ATTRIBUTE(attr5, const int)
REFL_ATTRIBUTE(attr8, int &)
REFL_ATTRIBUTE(attr9, const int &)
REFL_ATTRIBUTE(attr10, const int *)
REFL_METHOD(operator=, Foo &, const Foo &)
REFL_METHOD(operator=, Foo &, Foo &&)
REFL_METHOD(method1, int)
REFL_CONST_METHOD(method1, int)
REFL_METHOD(method1, int, int)
REFL_CONST_METHOD(method2, int)
REFL_CONST_VOLATILE_METHOD(method3, int)
REFL_METHOD(method4, void, double, char &)
REFL_STATIC_METHOD(method5, int, const int &)
REFL_END_CLASS

REFL_BEGIN_CLASS(NS::Foo)
REFL_CONSTRUCTOR(int)
REFL_END_CLASS

REFL_BEGIN_CLASS(NS::NestedNS::Foo)
REFL_DEFAULT_CONSTRUCTOR()
REFL_METHOD(method, int, double)
REFL_END_CLASS

