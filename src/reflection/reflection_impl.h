/*
** SelfPortrait API
** See Copyright Notice in reflection.h
*/
#ifndef REFLECTION_IMPL_H
#define REFLECTION_IMPL_H

#include "attribute.h"
#include "class.h"
#include "constructor.h"
#include "function.h"
#include "method.h"
#include "proxy.h"
#include "reflection.h"
#include <unordered_map>
#include <list>

#ifndef NO_RTTI
#include <typeindex>
#endif

#define TOKENPASTE(x, y) x ## y
#define TOKENPASTE2(x, y) TOKENPASTE(x, y)
#define UNIQUE TOKENPASTE2(Unique_, __LINE__)
#define COMMA ,

class ClassRegistry {
public:
	void registerClass(const Class& c);

	const Class forName(const ::std::string& name) const;

#ifndef NO_RTTI
	const Class forTypeId(const ::std::type_info& id) const;
#endif

	static ClassRegistry& instance();

private:
	ClassRegistry() {}

	::std::unordered_map< ::std::string, Class > m_registryByName;
#ifndef NO_RTTI
	::std::unordered_map< ::std::type_index, Class > m_registryByTypeId;
#endif
};

namespace {

	template<class Clazz>
	struct ClassRegHelper {
		ClassRegHelper( const char* name ) {
			ClassRegistry::instance().registerClass(ClassOf<Clazz>());
		}
	};

}

#ifndef NO_RTTI

#define REFL_BEGIN_CLASS(CLASS_NAME) \
	static ClassRegHelper<CLASS_NAME> UNIQUE(#CLASS_NAME); \
template<> ClassImpl* ClassImpl::inst<CLASS_NAME>() {\
	typedef CLASS_NAME ThisClass;\
	static ClassImpl instance;\
	if (instance.open()) {\
	instance.setTypeInfo(typeid(ThisClass)); \
		instance.setFullyQualifiedName(#CLASS_NAME);

#else

#define REFL_BEGIN_CLASS(CLASS_NAME) \
	static ClassRegHelper<CLASS_NAME> UNIQUE(#CLASS_NAME); \
template<> ClassImpl* ClassImpl::inst<CLASS_NAME>() {\
	typedef CLASS_NAME ThisClass;\
	static ClassImpl instance;\
	if (instance.open()) {\
		instance.setFullyQualifiedName(#CLASS_NAME);

#endif

#define REFL_END_CLASS \
	instance.close();\
}\
return &instance;\
}

template<class B, class D, bool P>
struct cast_helper {
    static VariantValue cast(const VariantValue&) {
        return VariantValue();
    }
};

template<class B, class D>
struct cast_helper<B,D,true> {
    static VariantValue cast(const VariantValue& b) {
        VariantValue ret;
        auto br = b.convertToThrow<D*>();
        auto dr = dynamic_cast<B*>(br);
        if (dr) {
            ret.construct<B&>(*dr);
        }
        return ret;
    }
};

#define REFL_SUPER_CLASS(CLASS_NAME) \
    instance.registerSuperClass(#CLASS_NAME, cast_helper<ThisClass, CLASS_NAME, std::is_polymorphic<CLASS_NAME>::value>::cast);


#ifndef NO_RTTI

#define REFL_METHOD(METHOD_NAME, RESULT, ...) \
{\
static MethodImpl impl(\
			  &method_type<RESULT(ThisClass::*)(__VA_ARGS__)>::bindcall<&ThisClass::METHOD_NAME>,\
			  #METHOD_NAME,\
			  #RESULT,\
			  typelist_size<typename method_type<RESULT(ThisClass::*)(__VA_ARGS__)>::Arguments>::value,\
			  #__VA_ARGS__,\
			  method_type<RESULT(ThisClass::*)(__VA_ARGS__)>::is_const,\
			  method_type<RESULT(ThisClass::*)(__VA_ARGS__)>::is_volatile,\
			  false\
			  , typeid(typename method_type<RESULT(ThisClass::*)(__VA_ARGS__)>::Result)\
			  , get_typeinfo<typename method_type<RESULT(ThisClass::*)(__VA_ARGS__)>::Arguments>()\
			  );\
instance.registerMethod(Method(&impl));\
}

#define REFL_CONST_METHOD(METHOD_NAME, RESULT, ...) \
{\
static MethodImpl impl(\
			  &method_type<RESULT(ThisClass::*)(__VA_ARGS__) const>::bindcall<&ThisClass::METHOD_NAME>,\
			  #METHOD_NAME,\
			  #RESULT,\
			  typelist_size<typename method_type<RESULT(ThisClass::*)(__VA_ARGS__) const>::Arguments>::value,\
			  #__VA_ARGS__,\
			  method_type<RESULT(ThisClass::*)(__VA_ARGS__) const>::is_const,\
			  method_type<RESULT(ThisClass::*)(__VA_ARGS__) const>::is_volatile,\
			  false\
			  , typeid(typename method_type<RESULT(ThisClass::*)(__VA_ARGS__) const>::Result)\
			  , get_typeinfo<typename method_type<RESULT(ThisClass::*)(__VA_ARGS__) const>::Arguments>()\
			  );\
instance.registerMethod(Method(&impl));\
}

#define REFL_VOLATILE_METHOD(METHOD_NAME, RESULT, ...) \
{\
static MethodImpl impl(\
			  &method_type<RESULT(ThisClass::*)(__VA_ARGS__) volatile>::bindcall<&ThisClass::METHOD_NAME>,\
			  #METHOD_NAME,\
			  #RESULT,\
			  typelist_size<typename method_type<RESULT(ThisClass::*)(__VA_ARGS__) volatile>::Arguments>::value,\
			  #__VA_ARGS__,\
			  method_type<RESULT(ThisClass::*)(__VA_ARGS__) volatile>::is_const,\
			  method_type<RESULT(ThisClass::*)(__VA_ARGS__) volatile>::is_volatile,\
			  false\
			  , typeid(typename method_type<RESULT(ThisClass::*)(__VA_ARGS__) volatile>::Result)\
			  , get_typeinfo<typename method_type<RESULT(ThisClass::*)(__VA_ARGS__) volatile>::Arguments>()\
			  );\
instance.registerMethod(Method(&impl));\
}

#define REFL_CONST_VOLATILE_METHOD(METHOD_NAME, RESULT, ...) \
{\
static MethodImpl impl(\
			  &method_type<RESULT(ThisClass::*)(__VA_ARGS__) const volatile>::bindcall<&ThisClass::METHOD_NAME>,\
			  #METHOD_NAME,\
			  #RESULT,\
			  typelist_size<typename method_type<RESULT(ThisClass::*)(__VA_ARGS__) const volatile>::Arguments>::value,\
			  #__VA_ARGS__,\
			  method_type<RESULT(ThisClass::*)(__VA_ARGS__) const volatile>::is_const,\
			  method_type<RESULT(ThisClass::*)(__VA_ARGS__) const volatile>::is_volatile,\
			  false\
			  , typeid(typename method_type<RESULT(ThisClass::*)(__VA_ARGS__) const volatile>::Result)\
			  , get_typeinfo<typename method_type<RESULT(ThisClass::*)(__VA_ARGS__) const volatile>::Arguments>()\
			  );\
instance.registerMethod(Method(&impl));\
}

#define REFL_STATIC_METHOD(METHOD_NAME, RESULT, ...) \
{\
static MethodImpl impl(\
			&method_type<RESULT(*)(__VA_ARGS__)>::bindcall<&ThisClass::METHOD_NAME>,\
			#METHOD_NAME,\
			#RESULT,\
			typelist_size<typename method_type<RESULT(*)(__VA_ARGS__)>::Arguments>::value,\
			#__VA_ARGS__,\
			false,\
			false,\
			true\
			, typeid(typename method_type<RESULT(*)(__VA_ARGS__)>::Result)\
			, get_typeinfo<typename method_type<RESULT(*)(__VA_ARGS__)>::Arguments>()\
			);\
instance.registerMethod(Method(&impl));\
}

#else


#define REFL_METHOD(METHOD_NAME, RESULT, ...) \
{\
static MethodImpl impl(\
			  &method_type<RESULT(ThisClass::*)(__VA_ARGS__)>::bindcall<&ThisClass::METHOD_NAME>,\
			  #METHOD_NAME,\
			  #RESULT,\
			  typelist_size<typename method_type<RESULT(ThisClass::*)(__VA_ARGS__)>::Arguments>::value,\
			  #__VA_ARGS__,\
			  method_type<RESULT(ThisClass::*)(__VA_ARGS__)>::is_const,\
			  method_type<RESULT(ThisClass::*)(__VA_ARGS__)>::is_volatile,\
			  false\
			  );\
instance.registerMethod(Method(&impl));\
}

#define REFL_CONST_METHOD(METHOD_NAME, RESULT, ...) \
{\
static MethodImpl impl(\
			  &method_type<RESULT(ThisClass::*)(__VA_ARGS__) const>::bindcall<&ThisClass::METHOD_NAME>,\
			  #METHOD_NAME,\
			  #RESULT,\
			  typelist_size<typename method_type<RESULT(ThisClass::*)(__VA_ARGS__) const>::Arguments>::value,\
			  #__VA_ARGS__,\
			  method_type<RESULT(ThisClass::*)(__VA_ARGS__) const>::is_const,\
			  method_type<RESULT(ThisClass::*)(__VA_ARGS__) const>::is_volatile,\
			  false\
			  );\
instance.registerMethod(Method(&impl));\
}

#define REFL_VOLATILE_METHOD(METHOD_NAME, RESULT, ...) \
{\
static MethodImpl impl(\
			  &method_type<RESULT(ThisClass::*)(__VA_ARGS__) volatile>::bindcall<&ThisClass::METHOD_NAME>,\
			  #METHOD_NAME,\
			  #RESULT,\
			  typelist_size<typename method_type<RESULT(ThisClass::*)(__VA_ARGS__) volatile>::Arguments>::value,\
			  #__VA_ARGS__,\
			  method_type<RESULT(ThisClass::*)(__VA_ARGS__) volatile>::is_const,\
			  method_type<RESULT(ThisClass::*)(__VA_ARGS__) volatile>::is_volatile,\
			  false\
			  );\
instance.registerMethod(Method(&impl));\
}

#define REFL_CONST_VOLATILE_METHOD(METHOD_NAME, RESULT, ...) \
{\
static MethodImpl impl(\
			  &method_type<RESULT(ThisClass::*)(__VA_ARGS__) const volatile>::bindcall<&ThisClass::METHOD_NAME>,\
			  #METHOD_NAME,\
			  #RESULT,\
			  typelist_size<typename method_type<RESULT(ThisClass::*)(__VA_ARGS__) const volatile>::Arguments>::value,\
			  #__VA_ARGS__,\
			  method_type<RESULT(ThisClass::*)(__VA_ARGS__) const volatile>::is_const,\
			  method_type<RESULT(ThisClass::*)(__VA_ARGS__) const volatile>::is_volatile,\
			  false\
			  );\
instance.registerMethod(Method(&impl));\
}

#define REFL_STATIC_METHOD(METHOD_NAME, RESULT, ...) \
{\
static MethodImpl impl(\
			&method_type<RESULT(*)(__VA_ARGS__)>::bindcall<&ThisClass::METHOD_NAME>,\
			#METHOD_NAME,\
			#RESULT,\
			typelist_size<typename method_type<RESULT(*)(__VA_ARGS__)>::Arguments>::value,\
			#__VA_ARGS__,\
			false,\
			false,\
			true\
			);\
instance.registerMethod(Method(&impl));\
}
#endif

#define REFL_ATTRIBUTE(ATTRIBUTE_NAME, TYPE_SPELLING) \
	static AttributeImpl<decltype(&ThisClass::ATTRIBUTE_NAME)> impl##ATTRIBUTE_NAME(#ATTRIBUTE_NAME, &ThisClass::ATTRIBUTE_NAME, #TYPE_SPELLING);\
	instance.registerAttribute(&impl##ATTRIBUTE_NAME);

#define REFL_STATIC_ATTRIBUTE(ATTRIBUTE_NAME, TYPE_SPELLING) \
	static StaticAttributeImpl<ThisClass, decltype(&ThisClass::ATTRIBUTE_NAME)> impl##ATTRIBUTE_NAME(#ATTRIBUTE_NAME, &ThisClass::ATTRIBUTE_NAME, #TYPE_SPELLING);\
	instance.registerAttribute(&impl##ATTRIBUTE_NAME);

#define REFL_DEFAULT_CONSTRUCTOR(...) \
	instance.registerConstructor(make_constructor<ThisClass>(""));

#define REFL_CONSTRUCTOR(...) \
	instance.registerConstructor(make_constructor<ThisClass, __VA_ARGS__>(#__VA_ARGS__));


#define REFL_STUB(STUBCLASS) \
	instance.registerInterface(STUBCLASS::create);


class FunctionRegistry {
public:

	const ::std::list<Function>& findFunction(const ::std::string& name) const;
	void registerFunction(const ::std::string& name, const Function& func);

	static FunctionRegistry& instance();

private:
	FunctionRegistry() {}

	::std::map< ::std::string, ::std::list<Function> > m_registry;
	const ::std::list<Function> emptyList;
};



template<class FuncType>
struct FuncRegHelper {
	FuncRegHelper( boundfunction bf, const char* name, const char* rString, const char* args ) {
		FunctionRegistry::instance().registerFunction(name, make_function<FuncType>(bf, name, rString, args));
	}
};

/*
#define REFL_FUNCTION(NAME, RESULT, ...) \
	static FuncRegHelper<RESULT (*)(__VA_ARGS__)> UNIQUE(#NAME, &NAME, #RESULT, #__VA_ARGS__);
*/
#define REFL_FUNCTION(NAME, RESULT, ...) \
	static FuncRegHelper<RESULT (*)(__VA_ARGS__)> UNIQUE(&function_type<RESULT (*)(__VA_ARGS__)>::bindcall<&NAME>, #NAME, #RESULT, #__VA_ARGS__);


/* macro wizardry reference:
 *
 * http://gustedt.wordpress.com/2010/06/03/default-arguments-for-c99/
 * http://gustedt.wordpress.com/2010/06/08/detect-empty-macro-arguments/
 *
 */

#define _ARG100(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83, _84, _85, _86, _87, _88, _89, _90, _91, _92, _93, _94, _95, _96, _97, _98, _99,  ...) _99
#define HAS_COMMA(...) _ARG100(__VA_ARGS__, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0)
#define NARG100(...) _ARG100(__VA_ARGS__, 99, 98, 97, 96, 95, 94, 93, 92, 91, 90, 89, 88, 87, 86, 85, 84, 83, 82, 81, 80, 79, 78, 77, 76, 75, 74, 73, 72, 71, 70, 69, 68, 67, 66, 65, 64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

#define _TRIGGER_PARENTHESIS_(...) ,

#define NARG_01(...) 0
#define NARG_00(...) NARG100(__VA_ARGS__)
#define NARG_11(...) NARG100(__VA_ARGS__)

#define __NARG(N1, N2, ...) NARG_##N1##N2(__VA_ARGS__)
#define _NARG(N1, N2, ...) __NARG(N1, N2,__VA_ARGS__)
#define NARG(...) _NARG(HAS_COMMA(__VA_ARGS__), HAS_COMMA(_TRIGGER_PARENTHESIS_ __VA_ARGS__ ()),__VA_ARGS__)

#define ARGNAME0(x)
#define ARGNAME1(x) x##1
#define ARGNAME2(x) x##2, ARGNAME1(x)
#define ARGNAME3(x) x##3, ARGNAME2(x)
#define ARGNAME4(x) x##4, ARGNAME3(x)
#define ARGNAME5(x) x##5, ARGNAME4(x)
#define ARGNAME6(x) x##6, ARGNAME5(x)
#define ARGNAME7(x) x##7, ARGNAME6(x)
#define ARGNAME8(x) x##8, ARGNAME7(x)
#define ARGNAME9(x) x##9, ARGNAME8(x)
#define ARGNAME10(x) x##10, ARGNAME9(x)
#define ARGNAME11(x) x##11, ARGNAME10(x)
#define ARGNAME12(x) x##12, ARGNAME11(x)
#define ARGNAME13(x) x##13, ARGNAME12(x)
#define ARGNAME14(x) x##14, ARGNAME13(x)
#define ARGNAME15(x) x##15, ARGNAME14(x)
#define ARGNAME16(x) x##16, ARGNAME15(x)
#define ARGNAME17(x) x##17, ARGNAME16(x)
#define ARGNAME18(x) x##18, ARGNAME17(x)
#define ARGNAME19(x) x##19, ARGNAME18(x)
#define ARGNAME20(x) x##20, ARGNAME19(x)
#define ARGNAME21(x) x##21, ARGNAME20(x)
#define ARGNAME22(x) x##22, ARGNAME21(x)
#define ARGNAME23(x) x##23, ARGNAME22(x)
#define ARGNAME24(x) x##24, ARGNAME23(x)
#define ARGNAME25(x) x##25, ARGNAME24(x)
#define ARGNAME26(x) x##26, ARGNAME25(x)
#define ARGNAME27(x) x##27, ARGNAME26(x)
#define ARGNAME28(x) x##28, ARGNAME27(x)
#define ARGNAME29(x) x##29, ARGNAME28(x)
#define ARGNAME30(x) x##30, ARGNAME29(x)
#define ARGNAME31(x) x##31, ARGNAME30(x)
#define ARGNAME32(x) x##32, ARGNAME31(x)
#define ARGNAME33(x) x##33, ARGNAME32(x)
#define ARGNAME34(x) x##34, ARGNAME33(x)
#define ARGNAME35(x) x##35, ARGNAME34(x)
#define ARGNAME36(x) x##36, ARGNAME35(x)
#define ARGNAME37(x) x##37, ARGNAME36(x)
#define ARGNAME38(x) x##38, ARGNAME37(x)
#define ARGNAME39(x) x##39, ARGNAME38(x)
#define ARGNAME40(x) x##40, ARGNAME39(x)
#define ARGNAME41(x) x##41, ARGNAME40(x)
#define ARGNAME42(x) x##42, ARGNAME41(x)
#define ARGNAME43(x) x##43, ARGNAME42(x)
#define ARGNAME44(x) x##44, ARGNAME43(x)
#define ARGNAME45(x) x##45, ARGNAME44(x)
#define ARGNAME46(x) x##46, ARGNAME45(x)
#define ARGNAME47(x) x##47, ARGNAME46(x)
#define ARGNAME48(x) x##48, ARGNAME47(x)
#define ARGNAME49(x) x##49, ARGNAME48(x)
#define ARGNAME50(x) x##50, ARGNAME49(x)
#define ARGNAME51(x) x##51, ARGNAME50(x)
#define ARGNAME52(x) x##52, ARGNAME51(x)
#define ARGNAME53(x) x##53, ARGNAME52(x)
#define ARGNAME54(x) x##54, ARGNAME53(x)
#define ARGNAME55(x) x##55, ARGNAME54(x)
#define ARGNAME56(x) x##56, ARGNAME55(x)
#define ARGNAME57(x) x##57, ARGNAME56(x)
#define ARGNAME58(x) x##58, ARGNAME57(x)
#define ARGNAME59(x) x##59, ARGNAME58(x)
#define ARGNAME60(x) x##60, ARGNAME59(x)
#define ARGNAME61(x) x##61, ARGNAME60(x)
#define ARGNAME62(x) x##62, ARGNAME61(x)
#define ARGNAME63(x) x##63, ARGNAME62(x)
#define ARGNAME64(x) x##64, ARGNAME63(x)
#define ARGNAME65(x) x##65, ARGNAME64(x)
#define ARGNAME66(x) x##66, ARGNAME65(x)
#define ARGNAME67(x) x##67, ARGNAME66(x)
#define ARGNAME68(x) x##68, ARGNAME67(x)
#define ARGNAME69(x) x##69, ARGNAME68(x)
#define ARGNAME70(x) x##70, ARGNAME69(x)
#define ARGNAME71(x) x##71, ARGNAME70(x)
#define ARGNAME72(x) x##72, ARGNAME71(x)
#define ARGNAME73(x) x##73, ARGNAME72(x)
#define ARGNAME74(x) x##74, ARGNAME73(x)
#define ARGNAME75(x) x##75, ARGNAME74(x)
#define ARGNAME76(x) x##76, ARGNAME75(x)
#define ARGNAME77(x) x##77, ARGNAME76(x)
#define ARGNAME78(x) x##78, ARGNAME77(x)
#define ARGNAME79(x) x##79, ARGNAME78(x)
#define ARGNAME80(x) x##80, ARGNAME79(x)
#define ARGNAME81(x) x##81, ARGNAME80(x)
#define ARGNAME82(x) x##82, ARGNAME81(x)
#define ARGNAME83(x) x##83, ARGNAME82(x)
#define ARGNAME84(x) x##84, ARGNAME83(x)
#define ARGNAME85(x) x##85, ARGNAME84(x)
#define ARGNAME86(x) x##86, ARGNAME85(x)
#define ARGNAME87(x) x##87, ARGNAME86(x)
#define ARGNAME88(x) x##88, ARGNAME87(x)
#define ARGNAME89(x) x##89, ARGNAME88(x)
#define ARGNAME90(x) x##90, ARGNAME89(x)
#define ARGNAME91(x) x##91, ARGNAME90(x)
#define ARGNAME92(x) x##92, ARGNAME91(x)
#define ARGNAME93(x) x##93, ARGNAME92(x)
#define ARGNAME94(x) x##94, ARGNAME93(x)
#define ARGNAME95(x) x##95, ARGNAME94(x)
#define ARGNAME96(x) x##96, ARGNAME95(x)
#define ARGNAME97(x) x##97, ARGNAME96(x)
#define ARGNAME98(x) x##98, ARGNAME97(x)
#define ARGNAME99(x) x##99, ARGNAME98(x)
#define ARGNAME100(x) x##100, ARGNAME99(x)

#define ARGNAMEN(x, i) ARGNAME##i(x)


#define _ARGNAME(x, n) ARGNAMEN(x, n)
#define ARGNAME(...) _ARGNAME(a, NARG(__VA_ARGS__))


#define TYPE_ARGNAME_0(...)
#define TYPE_ARGNAME_1(type) type a1
#define TYPE_ARGNAME_2(type, ...) type a2, TYPE_ARGNAME_1(__VA_ARGS__)
#define TYPE_ARGNAME_3(type, ...) type a3, TYPE_ARGNAME_2(__VA_ARGS__)
#define TYPE_ARGNAME_4(type, ...) type a4, TYPE_ARGNAME_3(__VA_ARGS__)
#define TYPE_ARGNAME_5(type, ...) type a5, TYPE_ARGNAME_4(__VA_ARGS__)
#define TYPE_ARGNAME_6(type, ...) type a6, TYPE_ARGNAME_5(__VA_ARGS__)
#define TYPE_ARGNAME_7(type, ...) type a7, TYPE_ARGNAME_6(__VA_ARGS__)
#define TYPE_ARGNAME_8(type, ...) type a8, TYPE_ARGNAME_7(__VA_ARGS__)
#define TYPE_ARGNAME_9(type, ...) type a9, TYPE_ARGNAME_8(__VA_ARGS__)
#define TYPE_ARGNAME_10(type, ...) type a10, TYPE_ARGNAME_9(__VA_ARGS__)
#define TYPE_ARGNAME_11(type, ...) type a11, TYPE_ARGNAME_10(__VA_ARGS__)
#define TYPE_ARGNAME_12(type, ...) type a12, TYPE_ARGNAME_11(__VA_ARGS__)
#define TYPE_ARGNAME_13(type, ...) type a13, TYPE_ARGNAME_12(__VA_ARGS__)
#define TYPE_ARGNAME_14(type, ...) type a14, TYPE_ARGNAME_13(__VA_ARGS__)
#define TYPE_ARGNAME_15(type, ...) type a15, TYPE_ARGNAME_14(__VA_ARGS__)
#define TYPE_ARGNAME_16(type, ...) type a16, TYPE_ARGNAME_15(__VA_ARGS__)
#define TYPE_ARGNAME_17(type, ...) type a17, TYPE_ARGNAME_16(__VA_ARGS__)
#define TYPE_ARGNAME_18(type, ...) type a18, TYPE_ARGNAME_17(__VA_ARGS__)
#define TYPE_ARGNAME_19(type, ...) type a19, TYPE_ARGNAME_18(__VA_ARGS__)
#define TYPE_ARGNAME_20(type, ...) type a20, TYPE_ARGNAME_19(__VA_ARGS__)
#define TYPE_ARGNAME_21(type, ...) type a21, TYPE_ARGNAME_20(__VA_ARGS__)
#define TYPE_ARGNAME_22(type, ...) type a22, TYPE_ARGNAME_21(__VA_ARGS__)
#define TYPE_ARGNAME_23(type, ...) type a23, TYPE_ARGNAME_22(__VA_ARGS__)
#define TYPE_ARGNAME_24(type, ...) type a24, TYPE_ARGNAME_23(__VA_ARGS__)
#define TYPE_ARGNAME_25(type, ...) type a25, TYPE_ARGNAME_24(__VA_ARGS__)
#define TYPE_ARGNAME_26(type, ...) type a26, TYPE_ARGNAME_25(__VA_ARGS__)
#define TYPE_ARGNAME_27(type, ...) type a27, TYPE_ARGNAME_26(__VA_ARGS__)
#define TYPE_ARGNAME_28(type, ...) type a28, TYPE_ARGNAME_27(__VA_ARGS__)
#define TYPE_ARGNAME_29(type, ...) type a29, TYPE_ARGNAME_28(__VA_ARGS__)
#define TYPE_ARGNAME_30(type, ...) type a30, TYPE_ARGNAME_29(__VA_ARGS__)
#define TYPE_ARGNAME_31(type, ...) type a31, TYPE_ARGNAME_30(__VA_ARGS__)
#define TYPE_ARGNAME_32(type, ...) type a32, TYPE_ARGNAME_31(__VA_ARGS__)
#define TYPE_ARGNAME_33(type, ...) type a33, TYPE_ARGNAME_32(__VA_ARGS__)
#define TYPE_ARGNAME_34(type, ...) type a34, TYPE_ARGNAME_33(__VA_ARGS__)
#define TYPE_ARGNAME_35(type, ...) type a35, TYPE_ARGNAME_34(__VA_ARGS__)
#define TYPE_ARGNAME_36(type, ...) type a36, TYPE_ARGNAME_35(__VA_ARGS__)
#define TYPE_ARGNAME_37(type, ...) type a37, TYPE_ARGNAME_36(__VA_ARGS__)
#define TYPE_ARGNAME_38(type, ...) type a38, TYPE_ARGNAME_37(__VA_ARGS__)
#define TYPE_ARGNAME_39(type, ...) type a39, TYPE_ARGNAME_38(__VA_ARGS__)
#define TYPE_ARGNAME_40(type, ...) type a40, TYPE_ARGNAME_39(__VA_ARGS__)
#define TYPE_ARGNAME_41(type, ...) type a41, TYPE_ARGNAME_40(__VA_ARGS__)
#define TYPE_ARGNAME_42(type, ...) type a42, TYPE_ARGNAME_41(__VA_ARGS__)
#define TYPE_ARGNAME_43(type, ...) type a43, TYPE_ARGNAME_42(__VA_ARGS__)
#define TYPE_ARGNAME_44(type, ...) type a44, TYPE_ARGNAME_43(__VA_ARGS__)
#define TYPE_ARGNAME_45(type, ...) type a45, TYPE_ARGNAME_44(__VA_ARGS__)
#define TYPE_ARGNAME_46(type, ...) type a46, TYPE_ARGNAME_45(__VA_ARGS__)
#define TYPE_ARGNAME_47(type, ...) type a47, TYPE_ARGNAME_46(__VA_ARGS__)
#define TYPE_ARGNAME_48(type, ...) type a48, TYPE_ARGNAME_47(__VA_ARGS__)
#define TYPE_ARGNAME_49(type, ...) type a49, TYPE_ARGNAME_48(__VA_ARGS__)
#define TYPE_ARGNAME_50(type, ...) type a50, TYPE_ARGNAME_49(__VA_ARGS__)
#define TYPE_ARGNAME_51(type, ...) type a51, TYPE_ARGNAME_50(__VA_ARGS__)
#define TYPE_ARGNAME_52(type, ...) type a52, TYPE_ARGNAME_51(__VA_ARGS__)
#define TYPE_ARGNAME_53(type, ...) type a53, TYPE_ARGNAME_52(__VA_ARGS__)
#define TYPE_ARGNAME_54(type, ...) type a54, TYPE_ARGNAME_53(__VA_ARGS__)
#define TYPE_ARGNAME_55(type, ...) type a55, TYPE_ARGNAME_54(__VA_ARGS__)
#define TYPE_ARGNAME_56(type, ...) type a56, TYPE_ARGNAME_55(__VA_ARGS__)
#define TYPE_ARGNAME_57(type, ...) type a57, TYPE_ARGNAME_56(__VA_ARGS__)
#define TYPE_ARGNAME_58(type, ...) type a58, TYPE_ARGNAME_57(__VA_ARGS__)
#define TYPE_ARGNAME_59(type, ...) type a59, TYPE_ARGNAME_58(__VA_ARGS__)
#define TYPE_ARGNAME_60(type, ...) type a60, TYPE_ARGNAME_59(__VA_ARGS__)
#define TYPE_ARGNAME_61(type, ...) type a61, TYPE_ARGNAME_60(__VA_ARGS__)
#define TYPE_ARGNAME_62(type, ...) type a62, TYPE_ARGNAME_61(__VA_ARGS__)
#define TYPE_ARGNAME_63(type, ...) type a63, TYPE_ARGNAME_62(__VA_ARGS__)
#define TYPE_ARGNAME_64(type, ...) type a64, TYPE_ARGNAME_63(__VA_ARGS__)
#define TYPE_ARGNAME_65(type, ...) type a65, TYPE_ARGNAME_64(__VA_ARGS__)
#define TYPE_ARGNAME_66(type, ...) type a66, TYPE_ARGNAME_65(__VA_ARGS__)
#define TYPE_ARGNAME_67(type, ...) type a67, TYPE_ARGNAME_66(__VA_ARGS__)
#define TYPE_ARGNAME_68(type, ...) type a68, TYPE_ARGNAME_67(__VA_ARGS__)
#define TYPE_ARGNAME_69(type, ...) type a69, TYPE_ARGNAME_68(__VA_ARGS__)
#define TYPE_ARGNAME_70(type, ...) type a70, TYPE_ARGNAME_69(__VA_ARGS__)
#define TYPE_ARGNAME_71(type, ...) type a71, TYPE_ARGNAME_70(__VA_ARGS__)
#define TYPE_ARGNAME_72(type, ...) type a72, TYPE_ARGNAME_71(__VA_ARGS__)
#define TYPE_ARGNAME_73(type, ...) type a73, TYPE_ARGNAME_72(__VA_ARGS__)
#define TYPE_ARGNAME_74(type, ...) type a74, TYPE_ARGNAME_73(__VA_ARGS__)
#define TYPE_ARGNAME_75(type, ...) type a75, TYPE_ARGNAME_74(__VA_ARGS__)
#define TYPE_ARGNAME_76(type, ...) type a76, TYPE_ARGNAME_75(__VA_ARGS__)
#define TYPE_ARGNAME_77(type, ...) type a77, TYPE_ARGNAME_76(__VA_ARGS__)
#define TYPE_ARGNAME_78(type, ...) type a78, TYPE_ARGNAME_77(__VA_ARGS__)
#define TYPE_ARGNAME_79(type, ...) type a79, TYPE_ARGNAME_78(__VA_ARGS__)
#define TYPE_ARGNAME_80(type, ...) type a80, TYPE_ARGNAME_79(__VA_ARGS__)
#define TYPE_ARGNAME_81(type, ...) type a81, TYPE_ARGNAME_80(__VA_ARGS__)
#define TYPE_ARGNAME_82(type, ...) type a82, TYPE_ARGNAME_81(__VA_ARGS__)
#define TYPE_ARGNAME_83(type, ...) type a83, TYPE_ARGNAME_82(__VA_ARGS__)
#define TYPE_ARGNAME_84(type, ...) type a84, TYPE_ARGNAME_83(__VA_ARGS__)
#define TYPE_ARGNAME_85(type, ...) type a85, TYPE_ARGNAME_84(__VA_ARGS__)
#define TYPE_ARGNAME_86(type, ...) type a86, TYPE_ARGNAME_85(__VA_ARGS__)
#define TYPE_ARGNAME_87(type, ...) type a87, TYPE_ARGNAME_86(__VA_ARGS__)
#define TYPE_ARGNAME_88(type, ...) type a88, TYPE_ARGNAME_87(__VA_ARGS__)
#define TYPE_ARGNAME_89(type, ...) type a89, TYPE_ARGNAME_88(__VA_ARGS__)
#define TYPE_ARGNAME_90(type, ...) type a90, TYPE_ARGNAME_89(__VA_ARGS__)
#define TYPE_ARGNAME_91(type, ...) type a91, TYPE_ARGNAME_90(__VA_ARGS__)
#define TYPE_ARGNAME_92(type, ...) type a92, TYPE_ARGNAME_91(__VA_ARGS__)
#define TYPE_ARGNAME_93(type, ...) type a93, TYPE_ARGNAME_92(__VA_ARGS__)
#define TYPE_ARGNAME_94(type, ...) type a94, TYPE_ARGNAME_93(__VA_ARGS__)
#define TYPE_ARGNAME_95(type, ...) type a95, TYPE_ARGNAME_94(__VA_ARGS__)
#define TYPE_ARGNAME_96(type, ...) type a96, TYPE_ARGNAME_95(__VA_ARGS__)
#define TYPE_ARGNAME_97(type, ...) type a97, TYPE_ARGNAME_96(__VA_ARGS__)
#define TYPE_ARGNAME_98(type, ...) type a98, TYPE_ARGNAME_97(__VA_ARGS__)
#define TYPE_ARGNAME_99(type, ...) type a99, TYPE_ARGNAME_98(__VA_ARGS__)
#define TYPE_ARGNAME_100(type, ...) type a100, TYPE_ARGNAME_99(__VA_ARGS__)


#define __TYPE_ARGNAME(N, ...) TYPE_ARGNAME_##N (__VA_ARGS__)
#define _TYPE_ARGNAME(N, ...) __TYPE_ARGNAME(N, __VA_ARGS__)
#define TYPE_ARGNAME(...) _TYPE_ARGNAME(NARG(__VA_ARGS__), __VA_ARGS__)

#define REFL_BEGIN_STUB(CLASS, STUBCLASSNAME) \
	namespace {\
		class STUBCLASSNAME: public CLASS {\
			std::shared_ptr<ProxyImpl> impl;\
		public:\
			STUBCLASSNAME(std::shared_ptr<ProxyImpl>& pi) : impl(pi){}\
			typedef CLASS ThisClass;\
			static VariantValue create(std::shared_ptr<ProxyImpl>& pImpl) { VariantValue ret; ret.construct<STUBCLASSNAME>(pImpl); return std::move(ret); }

#define CHECK_N(x, n, ...) n
#define CHECK(...) CHECK_N(__VA_ARGS__, 0,)

#define IIF(c) TOKENPASTE(IIF_, c)
#define IIF_0(t, f) f
#define IIF_1(t, f) t

#define IF_VOID2(ARG) IF_VOID_##ARG
#define IF_VOID(ARG) IIF(CHECK(IF_VOID2(ARG)))
#define IF_VOID_void foo, 1,

#define NOTHING
#define EMPTY100(...) _ARG100(__VA_ARGS__, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0)

#define EMPTY_01(...) 0
#define EMPTY_00(...) EMPTY100(__VA_ARGS__)
#define EMPTY_11(...) EMPTY100(__VA_ARGS__)

#define __EMPTY(N1, N2, ...) EMPTY_##N1##N2(__VA_ARGS__)
#define _EMPTY(N1, N2, ...) __EMPTY(N1, N2,__VA_ARGS__)
#define EMPTY(...) _EMPTY(HAS_COMMA(__VA_ARGS__), HAS_COMMA(_TRIGGER_PARENTHESIS_ __VA_ARGS__ ()),__VA_ARGS__)

#define PRODUCE_COMMA(...) IIF(EMPTY(__VA_ARGS__))(COMMA, ) __VA_ARGS__


#define REFL_STUB_METHOD(CLASS, METHOD_NAME, RESULT, ...) \
	RESULT METHOD_NAME ( TYPE_ARGNAME(__VA_ARGS__) ) override {\
        IF_VOID(RESULT)(NOTHING, return) impl->call(reinterpret_cast<size_t>(&method_type<RESULT(CLASS::*)(__VA_ARGS__)>::bindcall<&CLASS::METHOD_NAME>) PRODUCE_COMMA(ARGNAME(__VA_ARGS__))) \
            IF_VOID(RESULT)(NOTHING, .moveValueThrow<RESULT>());\
	}

#define REFL_STUB_CONST_METHOD(CLASS, METHOD_NAME, RESULT, ...) \
	RESULT METHOD_NAME ( TYPE_ARGNAME(__VA_ARGS__) ) const override {\
        IF_VOID(RESULT)(NOTHING, return) impl->call(reinterpret_cast<size_t>(&method_type<RESULT(CLASS::*)(__VA_ARGS__) const>::bindcall<&CLASS::METHOD_NAME>) PRODUCE_COMMA(ARGNAME(__VA_ARGS__))) \
            IF_VOID(RESULT)(NOTHING, .moveValueThrow<RESULT>());\
	}

#define REFL_STUB_VOLATILE_METHOD(CLASS, METHOD_NAME, RESULT, ...) \
	RESULT METHOD_NAME ( TYPE_ARGNAME(__VA_ARGS__) ) volatile override {\
        IF_VOID(RESULT)(NOTHING, return) impl->call(reinterpret_cast<size_t>(&method_type<RESULT(CLASS::*)(__VA_ARGS__) volatile>::bindcall<&CLASS::METHOD_NAME>) PRODUCE_COMMA(ARGNAME(__VA_ARGS__))) \
            IF_VOID(RESULT)(NOTHING, .moveValueThrow<RESULT>());\
	}


#define REFL_STUB_CONST_VOLATILE_METHOD(CLASS, METHOD_NAME, RESULT, ...) \
	RESULT METHOD_NAME ( TYPE_ARGNAME(__VA_ARGS__) ) const volatile override {\
        IF_VOID(RESULT)(NOTHING, return) impl->call(reinterpret_cast<size_t>(&method_type<RESULT(CLASS::*)(__VA_ARGS__) const volatile>::bindcall<&ThisClass::METHOD_NAME>) PRODUCE_COMMA(ARGNAME(__VA_ARGS__))) \
            IF_VOID(RESULT)(NOTHING, .moveValueThrow<RESULT>());\
	}


#define REFL_END_STUB };}

#endif /* REFLECTION_IMPL_H */

