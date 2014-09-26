/*
** SelfPortrait API
** See Copyright Notice in reflection.h
*/
#ifndef CONSTRUCTOR_H
#define CONSTRUCTOR_H

#include "typelist.h"
#include "variant.h"
#include <tuple>
#include <array>
#include "reflection.h"
#include "str_utils.h"
#include "call_utils.h"

typedef VariantValue (*boundcons)(const ::std::vector<VariantValue>& args);
#include <iostream>
using namespace std;
namespace {

template<class _Clazz, class... Args>
struct constructor_type {

	typedef _Clazz Clazz;
	typedef TypeList<Args...> Arguments;

	template<bool Abstract, class Ind>
	struct call_helper;

	template<class Ind>
	struct call_helper<true, Ind> {
		static VariantValue call(const ::std::vector<VariantValue>& args) {
			throw ::std::runtime_error("Class declares pure virtual members or has a private destructor");
		}
	};

	template< ::std::size_t... I, template< ::std::size_t...> class Ind>
	struct call_helper<false, Ind<I...>> {
		static VariantValue call(const ::std::vector<VariantValue>& args) {
			verify_call<Arguments, I...>(args);
			VariantValue ret;
			ret.construct<Clazz>(args[I].moveValue<typename type_at<Arguments, I>::type>()...);
			return ret;
		}
	};

	static VariantValue bindcall(const ::std::vector<VariantValue>& args) {
		return call_helper< ::std::is_abstract<Clazz>::value || !::std::is_destructible<Clazz>::value, typename make_indices<sizeof...(Args)>::type>::call(args);
	}
};

}

class ConstructorImpl: public Annotated {
public:	

	ConstructorImpl(
			boundcons c
			, int numArgs
			, const char* argSpellings
#ifndef NO_RTTI
			, ::std::vector<const ::std::type_info*> argumentTypes
#endif
			);

	::std::size_t numberOfArguments() const;

	::std::vector< ::std::string> argumentSpellings() const;

	VariantValue call(const ::std::vector<VariantValue>& args) const;


#ifndef NO_RTTI
	::std::vector<const ::std::type_info*> argumentTypes() const;
#endif
private:
	const char* m_argSpellings;
	unsigned int m_numArgs;
#ifndef NO_RTTI
	::std::vector<const ::std::type_info*> m_argumentTypes;
#endif
	boundcons m_c;
};

namespace {

template<class Clazz, class... Args>
Constructor make_constructor(const char* argString) {
	typedef TypeList<Args...> Arguments;
	static ConstructorImpl impl(
				&constructor_type<Clazz, Args...>::bindcall
				, sizeof...(Args)
				, argString
#ifndef NO_RTTI
				, get_typeinfo<Arguments>()
#endif
				);

	return Constructor(&impl);
}

}

#endif /* CONSTRUCTOR_H */
