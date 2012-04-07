#ifndef METHOD_H
#define METHOD_H

#include "typelist.h"
#include "variant.h"
#include "function.h"
#include "reflection.h"

#include <vector>
#include <tuple>
#include <array>
#include <algorithm>

#include <iostream>
using namespace std;

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

	template< ::size_t N>
	struct call_verifier {
		::std::array<bool,N> success;
		call_verifier(::std::size_t actual) {
			if (static_cast<int>(actual) < static_cast<int>(N)) {
				throw ::std::runtime_error("method called with insufficient number of arguments");
			}
			success.fill(false);
		}
		void assert_conversion_succeded() const {
			for (::std::size_t i = 0; i < success.size(); ++i) {
				if (success[i] == false) {
					throw ::std::runtime_error(::fmt_str("method called with incompatible argument at position %1", i) );
				}
			}
		}
	};
	
	template<class Ind, class RType, bool CRestriction, bool VRestriction>
	struct call_helper;
	
	template< ::std::size_t... I, template< ::std::size_t...> class Ind, class RType>
	struct call_helper<Ind<I...>, RType, false, false> {

		static_assert(size<Arguments>() == sizeof...(I), "number of arguments and number of indices don't match");
		
		static VariantValue call(ClazzRef object, ptr_to_method ptr, const ::std::vector<VariantValue>& args) {
			call_verifier<sizeof...(I)> ver(args.size());
			auto t = ::std::make_tuple(args[I].convertTo<typename type_at<Arguments, I>::type>(&ver.success[I])...);
			ver.assert_conversion_succeded();
			return (object.*ptr)(::std::get<I>(t)...);
		}
	};
	
	template< ::std::size_t... I, template< ::std::size_t...> class Ind>
	struct call_helper<Ind<I...>, void, false, false> {
		static_assert(size<Arguments>() == sizeof...(I), "number of arguments and number of indices don't match");
		static VariantValue call(ClazzRef object, ptr_to_method ptr, const ::std::vector<VariantValue>& args) {
			call_verifier<sizeof...(I)> ver(args.size());
			auto t = ::std::make_tuple(args[I].convertTo<typename type_at<Arguments, I>::type>(&ver.success[I])...);
			ver.assert_conversion_succeded();
			(object.*ptr)(::std::get<I>(t)...);
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
			throw ::std::runtime_error("Called non-const, non-voltile method of const volatile object");
		}
	};
	
	static VariantValue call(Clazz& object, ptr_to_method ptr, const ::std::vector<VariantValue>& args) {
		// plain reference, no restriction
		return call_helper<typename make_indices<size<Arguments>()>::type, Result, false, false>::call(object, ptr, args);
	}
	
	static VariantValue call(const Clazz& object, ptr_to_method ptr, const ::std::vector<VariantValue>& args) {
		// const reference, restrict if method is not const
		return call_helper<typename make_indices<size<Arguments>()>::type, Result, !Base::is_const, false>::call(object, ptr, args);
	}
	
	static VariantValue call(volatile Clazz& object, ptr_to_method ptr, const ::std::vector<VariantValue>& args) {
		// const reference, restrict if method is not const
		return call_helper<typename make_indices<size<Arguments>()>::type, Result, false, !Base::is_volatile>::call(object, ptr, args);
	}
	
	static VariantValue call(const volatile Clazz& object, ptr_to_method ptr, const ::std::vector<VariantValue>& args) {
		// const reference, restrict if method is not const
		return call_helper<typename make_indices<size<Arguments>()>::type, Result, !Base::is_const, !Base::is_volatile>::call(object, ptr, args);
	}
};


class AbstractMethodImpl: public Annotated {
public:
	AbstractMethodImpl() {}
	virtual ~AbstractMethodImpl() {}
	virtual const ::std::string& name() const = 0;
	virtual ::std::size_t numberOfArguments() const = 0;
	virtual ::std::vector<const ::std::type_info*> argumentTypes() const = 0;
	virtual const ::std::type_info& returnType() const = 0;
	virtual bool isConst() const = 0;
	virtual bool isVolatile() const = 0;
	virtual bool isStatic() const = 0;	
	virtual VariantValue call(const ::std::vector<VariantValue>& args) const = 0;
	virtual VariantValue call(VariantValue& object, const ::std::vector<VariantValue>& args) const = 0;
	virtual VariantValue call(const VariantValue& object, const ::std::vector<VariantValue>& args) const = 0;
	virtual VariantValue call(volatile VariantValue& object, const ::std::vector<VariantValue>& args) const = 0;
	virtual VariantValue call(const volatile VariantValue& object, const ::std::vector<VariantValue>& args) const = 0;
	
	AbstractMethodImpl(const AbstractMethodImpl&) = delete;
	AbstractMethodImpl(AbstractMethodImpl&&) = delete;
	AbstractMethodImpl& operator=(const AbstractMethodImpl&) = delete;
	AbstractMethodImpl& operator=(AbstractMethodImpl&&) = delete;
};

template<class _Method>
class MethodImpl: public AbstractMethodImpl {
public:
	typedef method_type<_Method> MDescr;
	typedef typename MDescr::Clazz Clazz;
	typedef typename MDescr::ClazzRef ClazzRef;
	typedef typename MDescr::ptr_to_method ptr_to_method;
	typedef typename MDescr::Result Result;
	
	constexpr MethodImpl(const ::std::string& name, ptr_to_method ptr) : m_name(name), m_ptr(ptr) {}
	
	virtual const ::std::string& name() const { return m_name; }
	
	virtual ::std::size_t numberOfArguments() const { return size<typename MDescr::Arguments>(); }
	
	virtual ::std::vector<const ::std::type_info*> argumentTypes() const { return get_typeinfo<typename MDescr::Arguments>(); }

	virtual const ::std::type_info& returnType() const { return typeid(Result); }
	
	virtual bool isConst() const { return MDescr::is_const; }
	
	virtual bool isVolatile() const { return MDescr::is_volatile; }
	
	virtual bool isStatic() const { return false; }

	virtual VariantValue call(const ::std::vector<VariantValue>& args) const {
		throw ::std::runtime_error("cannnot call non-static method withtout object");
	}
	
	virtual VariantValue call(VariantValue& object, const ::std::vector<VariantValue>& args) const {
		bool success = false;
		Clazz& ref = object.convertTo<Clazz&>(&success);
		
		if (!success) {
			throw ::std::runtime_error("method called with wrong type of object");
		}
		return MDescr::call(ref, m_ptr, args);
	}
		
	virtual VariantValue call(const VariantValue& object, const ::std::vector<VariantValue>& args) const {
		bool success = false;
		const Clazz& ref = object.convertTo<const Clazz&>(&success);

		if (!success) {
			throw ::std::runtime_error("method called with wrong type of object");
		}
		return MDescr::call(ref, m_ptr, args);
	}
	

	virtual VariantValue call(volatile VariantValue& object, const ::std::vector<VariantValue>& args) const {
		bool success = false;
		volatile Clazz& ref = object.convertTo<volatile Clazz&>(&success);

		if (!success) {
			throw ::std::runtime_error("method called with wrong type of object");
		}
		return MDescr::call(ref, m_ptr, args);
	}
	
	virtual VariantValue call(const volatile VariantValue& object, const ::std::vector<VariantValue>& args) const {
		bool success = false;
		const volatile Clazz& ref = object.convertTo<const volatile Clazz&>(&success);

		if (!success) {
			throw ::std::runtime_error("method called with wrong type of object");
		}
		return MDescr::call(ref, m_ptr, args);
	}
	
private:		
	
	::std::string m_name;
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
	
	constexpr StaticMethodImpl(const ::std::string& name, ptr_to_method ptr) : m_name(name), m_ptr(ptr) {}
	
	virtual const ::std::string& name() const { return m_name; }
	
	virtual ::std::size_t numberOfArguments() const { return size<typename MDescr::Arguments>(); }

	virtual const ::std::type_info& returnType() const { return typeid(Result); }
	
	virtual ::std::vector<const ::std::type_info*> argumentTypes() const { return get_typeinfo<typename MDescr::Arguments>(); }
	
	virtual bool isConst() const { return false; }
	
	virtual bool isVolatile() const { return false; }
	
	virtual bool isStatic() const { return true; }

	virtual VariantValue call(const ::std::vector<VariantValue>& args) const {
		return MDescr::call(m_ptr, args);
	}
	
	virtual VariantValue call(VariantValue&, const ::std::vector<VariantValue>& args) const {
		return MDescr::call(m_ptr, args);
	}
		
	virtual VariantValue call(const VariantValue&, const ::std::vector<VariantValue>& args) const {
		return MDescr::call(m_ptr, args);
	}	

	virtual VariantValue call(volatile VariantValue&, const ::std::vector<VariantValue>& args) const {
		return MDescr::call(m_ptr, args);
	}
	
	virtual VariantValue call(const volatile VariantValue&, const ::std::vector<VariantValue>& args) const {
		return MDescr::call(m_ptr, args);
	}	
private:		
	
	::std::string m_name;
	ptr_to_method m_ptr;
};




template<class _Method>
Method make_method(const ::std::string& name, _Method ptr) {
	static MethodImpl<_Method> impl(name, ptr);
	return Method(&impl);
}


template<class C, class _Method>
Method make_static_method(const ::std::string& name, _Method ptr) {
	static StaticMethodImpl<C, _Method> impl(name, ptr);
	return Method(&impl);
}

#endif /* METHOD_H */
