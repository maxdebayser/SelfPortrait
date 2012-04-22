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

	static VariantValue call(ptr_to_function ptr, const ::std::vector<VariantValue>& args) {
		return call_helper<Result, typename make_indices<sizeof...(Args)>::type>::call(ptr, args);
	}
};

}

class AbstractFunctionImpl: public Annotated {
public:

	AbstractFunctionImpl(
			const char* name
			, const char* returnSpelling
			, int numArgs
			, const char* argSpellings
#ifndef NO_RTTI
			, const ::std::type_info& returnType
			, ::std::vector<const ::std::type_info*> argumentTypes
#endif
			)
		: m_name(name)
		, m_returnSpelling(returnSpelling)
		, m_numArgs(numArgs)
		, m_argSpellings(argSpellings)
#ifndef NO_RTTI
		, m_returnType(returnType)
		, m_argumentTypes(argumentTypes)
#endif
	{}

	virtual ~AbstractFunctionImpl() {}
	
	::std::string name() const { return m_name; }
	::std::size_t numberOfArguments() const { return m_numArgs; }
	::std::string returnTypeSpelling() const { return normalizedTypeName(m_returnSpelling); }
	::std::vector< ::std::string> argumentSpellings() const { return splitArgs(m_argSpellings); }
#ifndef NO_RTTI
	const ::std::type_info& returnType() const { return m_returnType; }
	::std::vector<const ::std::type_info*> argumentTypes() const { return m_argumentTypes; }
#endif

	virtual VariantValue call(const ::std::vector<VariantValue>& args) const = 0;
	
	AbstractFunctionImpl(const AbstractFunctionImpl&) = delete;
	AbstractFunctionImpl(AbstractFunctionImpl&&) = delete;
	AbstractFunctionImpl& operator=(const AbstractFunctionImpl&) = delete;
	AbstractFunctionImpl& operator=(AbstractFunctionImpl&&) = delete;

private:
	const char* m_name;
	const char* m_returnSpelling;
	unsigned int m_numArgs : 5;
	const char* m_argSpellings;

#ifndef NO_RTTI
	const ::std::type_info& m_returnType;
	::std::vector<const ::std::type_info*> m_argumentTypes;
#endif
};


template<class Func>
class FunctionImpl: public AbstractFunctionImpl {
public:
	typedef function_type<Func> FDescr;
	typedef typename FDescr::ptr_to_function ptr_to_function;
	typedef typename FDescr::Arguments Arguments;
	typedef typename FDescr::Result Result;
	
	FunctionImpl(
			const char* name
			, ptr_to_function ptr
			, const char* returnSpelling
			, const char* argSpellings
			)
		: AbstractFunctionImpl(name, returnSpelling, typelist_size<Arguments>::value, argSpellings
#ifndef NO_RTTI
			  , typeid(Result)
			  , get_typeinfo<typename FDescr::Arguments>()
#endif
			  )
		, m_ptr(ptr) {}
	

	
	virtual VariantValue call(const ::std::vector<VariantValue>& args) const {
		return FDescr::call(m_ptr, args);
	}
	
private:
	::std::string m_name;
	ptr_to_function m_ptr;
	::std::string m_returnSpelling;
	::std::vector< ::std::string> m_argSpellings;
};




template<class FuncPtr>
Function make_function(const char* name, FuncPtr ptr, const char* resultString, const char* argString)
{
	static FunctionImpl<FuncPtr> impl(name, ptr, resultString, argString);
	return &impl;
}



#endif /* FUNCTION_H */
