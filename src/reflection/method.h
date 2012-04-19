#ifndef METHOD_H
#define METHOD_H

#include "typelist.h"
#include "variant.h"
#include "function.h"
#include "reflection.h"
#include "str_utils.h"
#include "call_utils.h"

#include <vector>
#include <tuple>
#include <array>
#include <algorithm>


namespace {

template<class _Method>
struct method_type_base;

template<class _Clazz, class _Result, class... Args>
struct method_type_base<_Result(_Clazz::*)(Args...)> {
	
	typedef _Result (_Clazz::*ptr_to_method)(Args...);
	
	enum { is_const = false };
	enum { is_volatile = false };
	
	typedef _Clazz Clazz;
	typedef _Result Result;
	typedef TypeList<Args...> Arguments;
	typedef _Clazz& ClazzRef;
};


template<class _Clazz, class _Result, class... Args>
struct method_type_base<_Result(_Clazz::*)(Args...) const> {
	
	typedef _Result (_Clazz::*ptr_to_method)(Args...) const;
	
	enum { is_const = true };
	enum { is_volatile = false };
	
	typedef _Clazz Clazz;
	typedef _Result Result;
	typedef TypeList<Args...> Arguments;
	typedef const _Clazz& ClazzRef;
};

template<class _Clazz, class _Result, class... Args>
struct method_type_base<_Result(_Clazz::*)(Args...) volatile> {
	
	typedef _Result (_Clazz::*ptr_to_method)(Args...) volatile;
	
	enum { is_const = false };
	enum { is_volatile = true };
	
	typedef _Clazz Clazz;
	typedef _Result Result;
	typedef TypeList<Args...> Arguments;
	typedef volatile _Clazz& ClazzRef;
};

template<class _Clazz, class _Result, class... Args>
struct method_type_base<_Result(_Clazz::*)(Args...) const volatile> {
	
	typedef _Result (_Clazz::*ptr_to_method)(Args...) const volatile;
	
	enum { is_const = true };
	enum { is_volatile = true };
	
	typedef _Clazz Clazz;
	typedef _Result Result;
	typedef TypeList<Args...> Arguments;
	typedef const volatile _Clazz& ClazzRef;
};


template<class Functor>
struct method_type: public method_type_base<Functor> {
		
	typedef method_type<Functor> Base;
	typedef typename Base::ptr_to_method ptr_to_method;
	typedef typename Base::Clazz Clazz;
	typedef typename Base::Result Result;
	typedef typename Base::Arguments Arguments;
	typedef typename Base::ClazzRef ClazzRef;	
	
	template<class Ind, class RType, bool CRestriction, bool VRestriction>
	struct call_helper;
	
	template< ::std::size_t... I, template< ::std::size_t...> class Ind, class RType>
	struct call_helper<Ind<I...>, RType, false, false> {

		static_assert(size<Arguments>() == sizeof...(I), "number of arguments and number of indices don't match");
		
		static VariantValue call(ClazzRef object, ptr_to_method ptr, const ::std::vector<VariantValue>& args) {
			verify_call<Arguments, I...>(args);
			return (object.*ptr)(args[I].moveValue<typename type_at<Arguments, I>::type>()...);
		}
	};
	
	template< ::std::size_t... I, template< ::std::size_t...> class Ind>
	struct call_helper<Ind<I...>, void, false, false> {
		static_assert(size<Arguments>() == sizeof...(I), "number of arguments and number of indices don't match");
		static VariantValue call(ClazzRef object, ptr_to_method ptr, const ::std::vector<VariantValue>& args) {
			verify_call<Arguments, I...>(args);
			(object.*ptr)(args[I].moveValue<typename type_at<Arguments, I>::type>()...);
			return VariantValue();
		}
	};

	template< ::std::size_t... I, template< ::std::size_t...> class Ind, class RType>
	struct call_helper<Ind<I...>, RType, true, false> {
		static VariantValue call(const volatile Clazz&, ptr_to_method, const ::std::vector<VariantValue>& args) {
			throw ::std::runtime_error("Called non-const method of const object");
		}
	};

	template< ::std::size_t... I, template< ::std::size_t...> class Ind, class RType>
	struct call_helper<Ind<I...>, RType, false, true> {
		static VariantValue call(const volatile Clazz&, ptr_to_method, const ::std::vector<VariantValue>& args) {
			throw ::std::runtime_error("Called non-volatile method of volatile object");
		}
	};
	
	template< ::std::size_t... I, template< ::std::size_t...> class Ind, class RType>
	struct call_helper<Ind<I...>, RType, true, true> {
		static VariantValue call(const volatile Clazz&, ptr_to_method, const ::std::vector<VariantValue>& args) {
			throw ::std::runtime_error("Called non-const, non-volatile method of const volatile object");
		}
	};
};

}

class AbstractMethodImpl: public Annotated {
public:

	AbstractMethodImpl(const char* name, const char* returnSpelling, int numArguments, const char* argSpellings, bool isConst, bool isVolatile, bool isStatic)
		: m_name(name)
		, m_returnSpelling(returnSpelling)
		, m_argSpellings(argSpellings)
		, m_numArgs(numArguments)
		, m_isConst(isConst)
		, m_isVolatile(isVolatile)
		, m_isStatic(isStatic)
	{}

	virtual ~AbstractMethodImpl() {}

	const char* name() const { return m_name; }
	::std::size_t numberOfArguments() const { return m_numArgs; }
	::std::vector< ::std::string> argumentSpellings() const { return splitArgs(m_argSpellings); }

	::std::string returnTypeSpelling() const { return normalizedTypeName(m_returnSpelling); }

	bool isConst() const { return m_isConst; }
	bool isVolatile() const { return m_isVolatile; }
	bool isStatic() const { return m_isStatic; }

#ifndef NO_RTTI
	virtual ::std::vector<const ::std::type_info*> argumentTypes() const = 0;
	virtual const ::std::type_info& returnType() const = 0;
#endif


	VariantValue call(const ::std::vector<VariantValue>& args) const {
		if (!m_isStatic) {
			throw ::std::runtime_error("cannnot call non-static method withtout object");
		}
		return this->call(VariantValue(), args);
	}

	VariantValue call(VariantValue& object, const ::std::vector<VariantValue>& args) const {
		return call(false, false, object, args);
	}
	VariantValue call(const VariantValue& object, const ::std::vector<VariantValue>& args) const {
		return call(true, false, object, args);
	}
	VariantValue call(volatile VariantValue& object, const ::std::vector<VariantValue>& args) const {
		return call(false, true, object, args);
	}
	VariantValue call(const volatile VariantValue& object, const ::std::vector<VariantValue>& args) const {
		return call(true, true, object, args);
	}
	
	AbstractMethodImpl(const AbstractMethodImpl&) = delete;
	AbstractMethodImpl(AbstractMethodImpl&&) = delete;
	AbstractMethodImpl& operator=(const AbstractMethodImpl&) = delete;
	AbstractMethodImpl& operator=(AbstractMethodImpl&&) = delete;

private:

	virtual VariantValue call(bool isConst, bool isVolatile, const volatile VariantValue& object, const ::std::vector<VariantValue>& args) const = 0;

	const char* m_name;
	const char* m_returnSpelling;
	const char* m_argSpellings;
	const unsigned int m_numArgs : 5; // up to 32 parameters are supported
	const unsigned int m_isConst : 1;
	const unsigned int m_isVolatile : 1;
	const unsigned int m_isStatic : 1;
};

namespace {

template<class _Method>
class MethodImpl: public AbstractMethodImpl {
public:
	typedef method_type<_Method> MDescr;
	typedef typename MDescr::Clazz Clazz;
	typedef typename MDescr::ClazzRef ClazzRef;
	typedef typename MDescr::ptr_to_method ptr_to_method;
	typedef typename MDescr::Result Result;
	
	 MethodImpl(const char* name, ptr_to_method ptr, const char* returnSpelling, const char* argSpellings)
		: AbstractMethodImpl(name, returnSpelling, size<typename MDescr::Arguments>(), argSpellings, MDescr::is_const, MDescr::is_volatile, false)
		, m_ptr(ptr)
	{ }
	
#ifndef NO_RTTI
	virtual ::std::vector<const ::std::type_info*> argumentTypes() const { return get_typeinfo<typename MDescr::Arguments>(); }

	virtual const ::std::type_info& returnType() const { return typeid(Result); }
#endif
	
	 virtual VariantValue call(const bool isConst, const bool isVolatile, const volatile VariantValue& object, const ::std::vector<VariantValue>& args) const {
		 if (!object.isA<Clazz>()) {
			 throw ::std::runtime_error("method called with wrong type of object");
		 }
		 if (!isConst && !isVolatile) {
			 Clazz& ref = object.convertTo<Clazz&>();
			 return MDescr::template call_helper<typename make_indices<size<typename MDescr::Arguments>()>::type, Result, false, false>::call(ref, m_ptr, args);
		 } else if (isConst && !isVolatile) {
			 const Clazz& ref = object.convertTo<const Clazz&>();
			 return MDescr::template call_helper<typename make_indices<size<typename MDescr::Arguments>()>::type, Result, !MDescr::is_const, false>::call(ref, m_ptr, args);
		 } else if (!isConst && isVolatile) {
			 volatile Clazz& ref = object.convertTo<volatile Clazz&>();
			 return MDescr::template call_helper<typename make_indices<size<typename MDescr::Arguments>()>::type, Result, false, !MDescr::is_volatile>::call(ref, m_ptr, args);
		 } else /*(isConst && isVolatile) */{
			 const volatile Clazz& ref = object.convertTo<const volatile Clazz&>();
			 return MDescr::template call_helper<typename make_indices<size<typename MDescr::Arguments>()>::type, Result, !MDescr::is_const, !MDescr::is_volatile>::call(ref, m_ptr, args);
		 }
	 }
private:
	ptr_to_method m_ptr;
};


template<class _Clazz, class _Method>
class StaticMethodImpl: public AbstractMethodImpl {
public:
	typedef function_type<_Method> MDescr;
	typedef _Clazz Clazz;
	typedef Clazz& ClazzRef;
	typedef typename MDescr::ptr_to_function ptr_to_method;
	typedef typename MDescr::Result Result;
	

	StaticMethodImpl(const char* name, ptr_to_method ptr, const char* returnSpelling, const char* argSpellings)
		: AbstractMethodImpl(name, ::std::move(returnSpelling), size<typename MDescr::Arguments>(), argSpellings, false, false, true)
		, m_ptr(ptr)
	{}

#ifndef NO_RTTI
	virtual const ::std::type_info& returnType() const { return typeid(Result); }
	
	virtual ::std::vector<const ::std::type_info*> argumentTypes() const { return get_typeinfo<typename MDescr::Arguments>(); }
#endif

	virtual VariantValue call(bool isConst, bool isVolatile, const volatile VariantValue& object, const ::std::vector<VariantValue>& args) const {
		return MDescr::call(m_ptr, args);
	}

private:
	ptr_to_method m_ptr;
};

}

template<class _Method>
Method make_method(const char* name, _Method ptr, const char* rString, const char* argString) {
	static MethodImpl<_Method> impl(name, ptr, rString, argString);
	return Method(&impl);
}


template<class C, class _Method>
Method make_static_method(const char* name, _Method ptr, const char* rString, const char* argString) {
	static StaticMethodImpl<C, _Method> impl(name, ptr, rString, argString);
	return Method(&impl);
}



#endif /* METHOD_H */
