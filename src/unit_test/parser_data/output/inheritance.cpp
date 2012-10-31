#include "reflection_impl.h"
#include "../input/inheritance.h"

REFL_BEGIN_CLASS(Base1)
REFL_METHOD(method1, void)
REFL_END_CLASS

REFL_BEGIN_CLASS(PBase1)
REFL_METHOD(method1, void)
REFL_METHOD(method2, int)
REFL_METHOD(operator=, PBase1 &, const PBase1 &)
REFL_METHOD(operator=, PBase1 &, PBase1 &&)
REFL_END_CLASS

REFL_BEGIN_CLASS(Base2)
REFL_CONSTRUCTOR(int)
REFL_METHOD(method2, void)
REFL_METHOD(method3, void)
REFL_METHOD(operator=, Base2 &, const Base2 &)
REFL_METHOD(operator=, Base2 &, Base2 &&)
REFL_END_CLASS

REFL_BEGIN_CLASS(Derived1)
REFL_SUPER_CLASS(Base1)
REFL_METHOD(method1, void)
REFL_METHOD(method2, void, int)
REFL_END_CLASS

REFL_BEGIN_CLASS(Derived2)
REFL_SUPER_CLASS(Base1)
REFL_SUPER_CLASS(Base2)
REFL_METHOD(method1, void)
REFL_METHOD(method2, void, int)
REFL_END_CLASS

REFL_BEGIN_CLASS(Derived3)
REFL_SUPER_CLASS(PBase1)
REFL_METHOD(method1, void)
REFL_METHOD(method2, int)
REFL_METHOD(method2, int, int)
REFL_METHOD(operator=, Derived3 &, const Derived3 &)
REFL_METHOD(operator=, Derived3 &, Derived3 &&)
REFL_END_CLASS

REFL_BEGIN_CLASS(Derived4)
REFL_SUPER_CLASS(PBase1)
REFL_SUPER_CLASS(Base2)
REFL_METHOD(method1, void)
REFL_METHOD(method2, int, int)
REFL_METHOD(operator=, Derived4 &, const Derived4 &)
REFL_METHOD(operator=, Derived4 &, Derived4 &&)
REFL_END_CLASS

REFL_BEGIN_CLASS(Derived5)
REFL_SUPER_CLASS(PBase1)
REFL_METHOD(method3, int, int, double)
REFL_METHOD(operator=, Derived5 &, const Derived5 &)
REFL_METHOD(operator=, Derived5 &, Derived5 &&)
REFL_END_CLASS

REFL_BEGIN_CLASS(Derived6)
REFL_SUPER_CLASS(Derived3)
REFL_SUPER_CLASS(Derived5)
REFL_METHOD(method3, int, int, double)
REFL_METHOD(operator=, Derived6 &, const Derived6 &)
REFL_METHOD(operator=, Derived6 &, Derived6 &&)
REFL_END_CLASS

