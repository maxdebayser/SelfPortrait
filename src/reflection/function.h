#ifndef FUNCTION_H
#define FUNCTION_H

#include <array>
#include <stdexcept>
#include <string>
#include <tuple>
#ifndef NO_RTTI
#include <typeinfo>
#endif
#include <utility>

#include "reflection.h"
#include "typelist.h"
#include "variant.h"
#include "str_conversion.h"
#include "str_utils.h"
#include "call_utils.h"

namespace {

template<class _Function>
struct function_type;

template<class _Result, class... Args>
struct function_type<_Result(*)(Args...)> {
	
	typedef _Result (*ptr_to_function)(Args...);

	typedef _Result Result;
	typedef TypeList<Args...> Arguments;

	template<class R, class Ind>
	struct call_helper;	

	template<class R, ::std::size_t... I, template< ::std::size_t...> class Ind>
	struct call_helper<R, Ind<I...>> {
		static VariantValue call(ptr_to_function ptr, const ::std::vector<VariantValue>& args) {
			VariantValue ret;
			verify_call<Arguments, I...>(args);
			ret.construct<R>(ptr(args[I].moveValue<typename type_at<Arguments, I>::type>()...));
			return ret;
		}
	};
	
	template< ::std::size_t... I, template< ::std::size_t...> class Ind>
	struct call_helper<void, Ind<I...>> {
		static VariantValue call(ptr_to_function ptr, const ::std::vector<VariantValue>& args) {
			verify_call<Arguments, I...>(args);
			ptr(args[I].moveValue<typename type_at<Arguments, I>::type>()...);
			return VariantValue();
		}
	};

	template <_Result(*ptr)(Args...)>
	static VariantValue bindcall(const ::std::vector<VariantValue>& args) {
		return call_helper<Result, typename make_indices<sizeof...(Args)>::type>::call(ptr, args);
	}
};

}


class FunctionImpl: public Annotated {
public:

	FunctionImpl(
			boundfunction f
			, const char* name
			, const char* returnSpelling
			, int numArgs
			, const char* argSpellings
#ifndef NO_RTTI
			, const ::std::type_info& returnType
			, ::std::vector<const ::std::type_info*> argumentTypes
#endif
			);

	~FunctionImpl();
	
	::std::string name() const;
	::std::size_t numberOfArguments() const;
	::std::string returnTypeSpelling() const;
	::std::vector< ::std::string> argumentSpellings() const;
#ifndef NO_RTTI
	const ::std::type_info& returnType() const;
	::std::vector<const ::std::type_info*> argumentTypes() const;
#endif

	VariantValue call(const ::std::vector<VariantValue>& args) const;
	
	FunctionImpl(const FunctionImpl&) = delete;
	FunctionImpl(FunctionImpl&&) = delete;
	FunctionImpl& operator=(const FunctionImpl&) = delete;
	FunctionImpl& operator=(FunctionImpl&&) = delete;

private:
	const char* m_name;
	const char* m_returnSpelling;
	unsigned int m_numArgs;
	const char* m_argSpellings;

#ifndef NO_RTTI
	const ::std::type_info& m_returnType;
	::std::vector<const ::std::type_info*> m_argumentTypes;
#endif
	boundfunction m_f;
};



template<class FuncPtr>
Function make_function(boundfunction f, const char* name, const char* resultString, const char* argString)
{
	typedef function_type<FuncPtr> FDescr;
	typedef typename FDescr::Arguments Arguments;
	typedef typename FDescr::Result Result;

	static FunctionImpl impl(
				f
				, name
				, resultString
				, typelist_size<Arguments>::value
				, argString
#ifndef NO_RTTI
				, typeid(Result)
				, get_typeinfo<Arguments>()
#endif
				);
	return &impl;
}



#endif /* FUNCTION_H */
