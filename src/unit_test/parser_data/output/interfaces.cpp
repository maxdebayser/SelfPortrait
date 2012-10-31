#include "reflection_impl.h"
#include "../input/interfaces.h"

REFL_BEGIN_STUB(Interface1, Interface1Stub)
REFL_STUB_METHOD(method1, int)
REFL_STUB_METHOD(method1, int, int)
REFL_STUB_CONST_METHOD(method1, int, int)
REFL_END_STUB

REFL_BEGIN_CLASS(Interface1)
REFL_STUB(Interface1Stub)
REFL_DEFAULT_CONSTRUCTOR()
REFL_METHOD(method1, int)
REFL_METHOD(method1, int, int)
REFL_CONST_METHOD(method1, int, int)
REFL_METHOD(operator=, Interface1 &, const Interface1 &)
REFL_END_CLASS

REFL_BEGIN_STUB(Interface2, Interface2Stub)
REFL_STUB_METHOD(method1, int)
REFL_END_STUB

REFL_BEGIN_CLASS(Interface2)
REFL_STUB(Interface2Stub)
REFL_DEFAULT_CONSTRUCTOR()
REFL_CONSTRUCTOR(int)
REFL_METHOD(method1, int)
REFL_METHOD(operator=, Interface2 &, const Interface2 &)
REFL_END_CLASS

REFL_BEGIN_STUB(Interface3, Interface3Stub)
REFL_END_STUB

REFL_BEGIN_CLASS(Interface3)
REFL_STUB(Interface3Stub)
REFL_SUPER_CLASS(Interface1)
REFL_SUPER_CLASS(Interface2)
REFL_DEFAULT_CONSTRUCTOR()
REFL_METHOD(operator=, Interface3 &, const Interface3 &)
REFL_END_CLASS

REFL_BEGIN_CLASS(NotAnInterface1)
REFL_DEFAULT_CONSTRUCTOR()
REFL_METHOD(method1, int)
REFL_METHOD(operator=, NotAnInterface1 &, const NotAnInterface1 &)
REFL_END_CLASS

REFL_BEGIN_CLASS(NotAnInterface2)
REFL_DEFAULT_CONSTRUCTOR()
REFL_METHOD(method1, int)
REFL_METHOD(operator=, NotAnInterface2 &, const NotAnInterface2 &)
REFL_END_CLASS

REFL_BEGIN_CLASS(NotAnInterface3)
REFL_DEFAULT_CONSTRUCTOR()
REFL_METHOD(method2, int)
REFL_METHOD(operator=, NotAnInterface3 &, const NotAnInterface3 &)
REFL_END_CLASS

REFL_BEGIN_CLASS(NotAnInterface4)
REFL_DEFAULT_CONSTRUCTOR()
REFL_ATTRIBUTE(attr2, int)
REFL_METHOD(method2, int)
REFL_METHOD(operator=, NotAnInterface4 &, const NotAnInterface4 &)
REFL_END_CLASS

REFL_BEGIN_CLASS(NotAnInterface5)
REFL_DEFAULT_CONSTRUCTOR()
REFL_METHOD(method2, int)
REFL_METHOD(operator=, NotAnInterface5 &, const NotAnInterface5 &)
REFL_END_CLASS

REFL_BEGIN_CLASS(NotAnInterface5::Inner)
REFL_DEFAULT_CONSTRUCTOR()
REFL_ATTRIBUTE(i1, int)
REFL_END_CLASS

REFL_BEGIN_STUB(NotAnInterface6, NotAnInterface6Stub)
REFL_STUB_METHOD(method2, int)
REFL_END_STUB

REFL_BEGIN_CLASS(NotAnInterface6)
REFL_STUB(NotAnInterface6Stub)
REFL_METHOD(method2, int)
REFL_METHOD(operator=, NotAnInterface6 &, const NotAnInterface6 &)
REFL_END_CLASS

REFL_BEGIN_CLASS(NotAnInterface7)
REFL_SUPER_CLASS(Interface1)
REFL_SUPER_CLASS(NotAnInterface1)
REFL_DEFAULT_CONSTRUCTOR()
REFL_METHOD(operator=, NotAnInterface7 &, const NotAnInterface7 &)
REFL_END_CLASS

REFL_BEGIN_CLASS(NotAnInterface8)
REFL_DEFAULT_CONSTRUCTOR()
REFL_METHOD(method1, int)
REFL_METHOD(method1, int, int)
REFL_CONST_METHOD(method1, int, int)
REFL_METHOD(operator=, NotAnInterface8 &, const NotAnInterface8 &)
REFL_END_CLASS

REFL_BEGIN_CLASS(NotAnInterface9)
REFL_SUPER_CLASS(Interface1)
REFL_DEFAULT_CONSTRUCTOR()
REFL_METHOD(operator=, NotAnInterface9 &, const NotAnInterface9 &)
REFL_END_CLASS

REFL_BEGIN_STUB(NS::Interface1, NS__Interface1Stub)
REFL_STUB_METHOD(method1, int)
REFL_STUB_METHOD(method1, int, int)
REFL_STUB_CONST_METHOD(method1, int, int)
REFL_END_STUB

REFL_BEGIN_CLASS(NS::Interface1)
REFL_STUB(NS__Interface1Stub)
REFL_DEFAULT_CONSTRUCTOR()
REFL_METHOD(method1, int)
REFL_METHOD(method1, int, int)
REFL_CONST_METHOD(method1, int, int)
REFL_METHOD(operator=, NS::Interface1 &, const NS::Interface1 &)
REFL_END_CLASS

REFL_BEGIN_STUB(NS::Impl1, NS__Impl1Stub)
REFL_END_STUB

REFL_BEGIN_CLASS(NS::Impl1)
REFL_STUB(NS__Impl1Stub)
REFL_SUPER_CLASS(Interface2)
REFL_DEFAULT_CONSTRUCTOR()
REFL_METHOD(operator=, NS::Impl1 &, const NS::Impl1 &)
REFL_END_CLASS

REFL_BEGIN_STUB(NS::Impl2, NS__Impl2Stub)
REFL_END_STUB

REFL_BEGIN_CLASS(NS::Impl2)
REFL_STUB(NS__Impl2Stub)
REFL_SUPER_CLASS(NS::Interface1)
REFL_DEFAULT_CONSTRUCTOR()
REFL_METHOD(operator=, NS::Impl2 &, const NS::Impl2 &)
REFL_END_CLASS

REFL_BEGIN_STUB(Impl3, Impl3Stub)
REFL_END_STUB

REFL_BEGIN_CLASS(Impl3)
REFL_STUB(Impl3Stub)
REFL_SUPER_CLASS(Interface2)
REFL_DEFAULT_CONSTRUCTOR()
REFL_METHOD(operator=, Impl3 &, const Impl3 &)
REFL_END_CLASS

REFL_BEGIN_STUB(Impl4, Impl4Stub)
REFL_END_STUB

REFL_BEGIN_CLASS(Impl4)
REFL_STUB(Impl4Stub)
REFL_SUPER_CLASS(NS::Interface1)
REFL_DEFAULT_CONSTRUCTOR()
REFL_METHOD(operator=, Impl4 &, const Impl4 &)
REFL_END_CLASS

