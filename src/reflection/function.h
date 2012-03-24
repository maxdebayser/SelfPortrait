#ifndef FUNCTION_H
#define FUNCTION_H

#include <array>
#include <stdexcept>
#include <string>
#include <tuple>
#include <typeinfo>

#include "typelist.h"
#include "variant.h"
#include "str_conversion.h"

template<class _Function>
struct function_type;

template<class _Result, class... Args>
struct function_type<_Result(*)(Args...)> {
	
	typedef _Result (*ptr_to_function)(Args...);

	typedef _Result Result;
	typedef TypeList<Args...> Arguments;
	
	template< ::size_t N>
	struct call_verifier {
		::std::array<bool,N> success;
		call_verifier(::std::size_t actual) {
			if (actual < N) {
				throw ::std::runtime_error("function called with insufficient number of arguments");
			}
			success.fill(false);
		}
		void assert_conversion_succeded() const {
			for (::std::size_t i = 0; i < success.size(); ++i) {
				if (success[i] == false) {
					throw ::std::runtime_error(::fmt_str("function called with incompatible argument at position %1", i) );
				}
			}
		}
	};
		
	template<class R, class Ind>
	struct call_helper;	
	
	template<class R, ::std::size_t... I, template< ::std::size_t...> class Ind>
	struct call_helper<R, Ind<I...>> {
		static VariantValue call(ptr_to_function ptr, const ::std::vector<VariantValue>& args) {
			call_verifier<sizeof...(I)> ver(args.size());
			auto t = ::std::make_tuple(args[I].convertTo<typename type_at<Arguments, I>::type>(&ver.success[I])...);
			ver.assert_conversion_succeded();
			return ptr(::std::get<I>(t)...);
		}
	};
	
	template< ::std::size_t... I, template< ::std::size_t...> class Ind>
	struct call_helper<void, Ind<I...>> {
		static VariantValue call(ptr_to_function ptr, const ::std::vector<VariantValue>& args) {
			call_verifier<sizeof...(I)> ver(args.size());
			auto t = ::std::make_tuple(args[I].convertTo<typename type_at<Arguments, I>::type>(&ver.success[I])...);
			ver.assert_conversion_succeded();
			ptr(::std::get<I>(t)...);
			return VariantValue();
		}
	};
	
	static VariantValue call(ptr_to_function ptr, const ::std::vector<VariantValue>& args) {
		return call_helper<Result, typename make_indices<sizeof...(Args)>::type>::call(ptr, args);
	}
};

class AbstractFunctionImpl {
public:
	AbstractFunctionImpl() {}
	virtual ~AbstractFunctionImpl() {}
	
	virtual const ::std::string& name() const = 0;
	virtual ::std::size_t numberOfArguments() const = 0;
	virtual ::std::vector<const ::std::type_info*> argumentTypes() const = 0;
	virtual VariantValue call(const ::std::vector<VariantValue>& args) const = 0;
	
	AbstractFunctionImpl(const AbstractFunctionImpl&) = delete;
	AbstractFunctionImpl(AbstractFunctionImpl&&) = delete;
	AbstractFunctionImpl& operator=(const AbstractFunctionImpl&) = delete;
	AbstractFunctionImpl& operator=(AbstractFunctionImpl&&) = delete;
};

template<class Func>
class FunctionImpl: public AbstractFunctionImpl {
public:
	typedef function_type<Func> FDescr;
	typedef typename FDescr::ptr_to_function ptr_to_function;
	typedef typename FDescr::Arguments Arguments;
	
	FunctionImpl(::std::string name, ptr_to_function ptr) : m_name(name), m_ptr(ptr) {}
	
	virtual const ::std::string& name() const { return m_name; }
	virtual ::std::size_t numberOfArguments() const { return typelist_size<Arguments>::size; }
	virtual ::std::vector<const ::std::type_info*> argumentTypes() const { return get_typeinfo<typename FDescr::Arguments>(); }
	
	virtual VariantValue call(const ::std::vector<VariantValue>& args) const {
		return FDescr::call(m_ptr, args);
	}
	
private:
	::std::string m_name;
	ptr_to_function m_ptr;
};


class Function {
public:
	
	Function(const Function& rhs) : m_impl(rhs.m_impl) {}
	Function(Function&& rhs) : m_impl(rhs.m_impl) {}
	Function& operator=(const Function& rhs) { m_impl = rhs.m_impl; return *this; }
	Function& operator=(Function&& rhs) { m_impl = rhs.m_impl; return *this; }
	
	const ::std::string& name() const { return m_impl->name(); }
	::std::size_t numberOfArguments() const { return m_impl->numberOfArguments(); }
	::std::vector<const ::std::type_info*> argumentTypes() const { return m_impl->argumentTypes(); }
	
	template<class... Args>
	VariantValue call(const Args&... args) const {
		::std::vector<VariantValue> vargs{ VariantValue(args)... };
		return m_impl->call(vargs);
	}
	
private:
	Function(AbstractFunctionImpl* impl) : m_impl(impl) {}
	AbstractFunctionImpl* m_impl;
	
	template<class FuncPtr>
	friend Function make_function(const ::std::string& name, FuncPtr ptr);
};

template<class FuncPtr>
Function make_function(const ::std::string& name, FuncPtr ptr)
{
	static FunctionImpl<FuncPtr> impl(name, ptr);
	return &impl;
}



#endif /* FUNCTION_H */
