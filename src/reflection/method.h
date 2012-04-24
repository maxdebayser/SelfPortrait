#ifndef METHOD_H
#define METHOD_H

#include "typelist.h"
#include "variant.h"
#include "reflection.h"
#include "str_utils.h"
#include "call_utils.h"

#include <vector>
#include <tuple>
#include <algorithm>




namespace {

template<class _Method>
struct method_type;

template<class _Clazz, class _Result, class... Args>
struct method_type<_Result(_Clazz::*)(Args...)> {
	
	typedef _Result (_Clazz::*ptr_to_method)(Args...);
	
	enum { is_const = false };
	enum { is_volatile = false };
	
	typedef _Clazz Clazz;
	typedef _Result Result;
	typedef TypeList<Args...> Arguments;
	typedef _Clazz& ClazzRef;

	template<class Ind, class RType>
	struct call_helper;

	template< ::std::size_t... I, template< ::std::size_t...> class Ind, class RType>
	struct call_helper<Ind<I...>, RType> {

		static_assert(size<Arguments>() == sizeof...(I), "number of arguments and number of indices don't match");

		static VariantValue call(ClazzRef object, ptr_to_method ptr, const ::std::vector<VariantValue>& args) {
			return ((object.*ptr)(args[I].moveValueThrow<typename type_at<Arguments, I>::type>()...));
		}
	};

	template< ::std::size_t... I, template< ::std::size_t...> class Ind>
	struct call_helper<Ind<I...>, void> {
		static_assert(size<Arguments>() == sizeof...(I), "number of arguments and number of indices don't match");
		static VariantValue call(ClazzRef object, ptr_to_method ptr, const ::std::vector<VariantValue>& args) {
			(object.*ptr)(args[I].moveValueThrow<typename type_at<Arguments, I>::type>()...);
			return VariantValue();
		}
	};

	template<_Result(_Clazz::*ptr)(Args...)>
	static VariantValue bindcall(const volatile VariantValue& object, const ::std::vector<VariantValue>& args)  {
		Clazz& ref = verifyObject<Clazz>(object);
		return call_helper<typename make_indices<sizeof...(Args)>::type, Result>::call(ref, ptr, args);
	}

};


template<class _Clazz, class _Result, class... Args>
struct method_type<_Result(_Clazz::*)(Args...) const> {
	
	typedef _Result (_Clazz::*ptr_to_method)(Args...) const;
	
	enum { is_const = true };
	enum { is_volatile = false };

	typedef _Clazz Clazz;
	typedef _Result Result;
	typedef TypeList<Args...> Arguments;
	typedef const _Clazz& ClazzRef;


	template<class Ind, class RType>
	struct call_helper;

	template< ::std::size_t... I, template< ::std::size_t...> class Ind, class RType>
	struct call_helper<Ind<I...>, RType> {

		static_assert(size<Arguments>() == sizeof...(I), "number of arguments and number of indices don't match");

		static VariantValue call(ClazzRef object, ptr_to_method ptr, const ::std::vector<VariantValue>& args) {
			return ((object.*ptr)(args[I].moveValueThrow<typename type_at<Arguments, I>::type>()...));
		}
	};

	template< ::std::size_t... I, template< ::std::size_t...> class Ind>
	struct call_helper<Ind<I...>, void> {
		static_assert(size<Arguments>() == sizeof...(I), "number of arguments and number of indices don't match");
		static VariantValue call(ClazzRef object, ptr_to_method ptr, const ::std::vector<VariantValue>& args) {
			(object.*ptr)(args[I].moveValueThrow<typename type_at<Arguments, I>::type>()...);
			return VariantValue();
		}
	};

	template<_Result(_Clazz::*ptr)(Args...) const>
	static VariantValue bindcall(const volatile VariantValue& object, const ::std::vector<VariantValue>& args)  {
		Clazz& ref = verifyObject<Clazz>(object);
		return call_helper<typename make_indices<sizeof...(Args)>::type, Result>::call(ref, ptr, args);
	}
};

template<class _Clazz, class _Result, class... Args>
struct method_type<_Result(_Clazz::*)(Args...) volatile> {
	
	typedef _Result (_Clazz::*ptr_to_method)(Args...) volatile;
	
	enum { is_const = false };
	enum { is_volatile = true };

	typedef _Clazz Clazz;
	typedef _Result Result;
	typedef TypeList<Args...> Arguments;
	typedef volatile _Clazz& ClazzRef;


	template<class Ind, class RType>
	struct call_helper;

	template< ::std::size_t... I, template< ::std::size_t...> class Ind, class RType>
	struct call_helper<Ind<I...>, RType> {

		static_assert(size<Arguments>() == sizeof...(I), "number of arguments and number of indices don't match");

		static VariantValue call(ClazzRef object, ptr_to_method ptr, const ::std::vector<VariantValue>& args) {
			return (object.*ptr)(args[I].moveValueThrow<typename type_at<Arguments, I>::type>()...);
		}
	};

	template< ::std::size_t... I, template< ::std::size_t...> class Ind>
	struct call_helper<Ind<I...>, void> {
		static_assert(size<Arguments>() == sizeof...(I), "number of arguments and number of indices don't match");
		static VariantValue call(ClazzRef object, ptr_to_method ptr, const ::std::vector<VariantValue>& args) {
			(object.*ptr)(args[I].moveValueThrow<typename type_at<Arguments, I>::type>()...);
			return VariantValue();
		}
	};

	template<_Result(_Clazz::*ptr)(Args...) volatile>
	static VariantValue bindcall(const volatile VariantValue& object, const ::std::vector<VariantValue>& args)  {
		Clazz& ref = verifyObject<Clazz>(object);
		return call_helper<typename make_indices<sizeof...(Args)>::type, Result>::call(ref, ptr, args);
	}
};

template<class _Clazz, class _Result, class... Args>
struct method_type<_Result(_Clazz::*)(Args...) const volatile> {

	typedef _Result (_Clazz::*ptr_to_method)(Args...) const volatile;

	enum { is_const = true };
	enum { is_volatile = true };

	typedef _Clazz Clazz;
	typedef _Result Result;
	typedef TypeList<Args...> Arguments;
	typedef const volatile _Clazz& ClazzRef;


	template<class Ind, class RType>
	struct call_helper;

	template< ::std::size_t... I, template< ::std::size_t...> class Ind, class RType>
	struct call_helper<Ind<I...>, RType> {

		static_assert(size<Arguments>() == sizeof...(I), "number of arguments and number of indices don't match");

		static VariantValue call(ClazzRef object, ptr_to_method ptr, const ::std::vector<VariantValue>& args) {
			return (object.*ptr)(args[I].moveValueThrow<typename type_at<Arguments, I>::type>()...);
		}
	};

	template< ::std::size_t... I, template< ::std::size_t...> class Ind>
	struct call_helper<Ind<I...>, void> {
		static_assert(size<Arguments>() == sizeof...(I), "number of arguments and number of indices don't match");
		static VariantValue call(ClazzRef object, ptr_to_method ptr, const ::std::vector<VariantValue>& args) {
			(object.*ptr)(args[I].moveValueThrow<typename type_at<Arguments, I>::type>()...);
			return VariantValue();
		}
	};

	template<_Result(_Clazz::*ptr)(Args...) const volatile>
	static VariantValue bindcall(const volatile VariantValue& object, const ::std::vector<VariantValue>& args)  {
		Clazz& ref = verifyObject<Clazz>(object);
		return call_helper<typename make_indices<sizeof...(Args)>::type, Result>::call(ref, ptr, args);
	}
};


template<class _Result, class... Args>
struct method_type<_Result(*)(Args...)> {

	typedef _Result (*ptr_to_method)(Args...);

	enum { is_const = false };
	enum { is_volatile = false };
	typedef _Result Result;
	typedef TypeList<Args...> Arguments;

	template<class R, class Ind>
	struct call_helper;

	template<class R, ::std::size_t... I, template< ::std::size_t...> class Ind>
	struct call_helper<R, Ind<I...>> {
		static VariantValue call(ptr_to_method ptr, const ::std::vector<VariantValue>& args) {
			return ptr(args[I].moveValueThrow<typename type_at<Arguments, I>::type>()...);
		}
	};

	template< ::std::size_t... I, template< ::std::size_t...> class Ind>
	struct call_helper<void, Ind<I...>> {
		static VariantValue call(ptr_to_method ptr, const ::std::vector<VariantValue>& args) {
			ptr(args[I].moveValueThrow<typename type_at<Arguments, I>::type>()...);
			return VariantValue();
		}
	};

	template <_Result(*ptr)(Args...)>
	static VariantValue bindcall(const volatile VariantValue&, const ::std::vector<VariantValue>& args)  {
		return call_helper<Result, typename make_indices<sizeof...(Args)>::type>::call(ptr, args);
	}
};

}




class MethodImpl: public Annotated {
public:

	MethodImpl(
			boundmethod m,
			const char* name,
			const char* returnSpelling,
			int numArguments,
			const char* argSpellings,
			bool isConst,
			bool isVolatile,
			bool isStatic
#ifndef NO_RTTI
			, const ::std::type_info& returnType
			, ::std::vector<const ::std::type_info*> argumentTypes
#endif
			);

	const char* name() const;
	::std::size_t numberOfArguments() const;
	::std::vector< ::std::string> argumentSpellings() const;

	::std::string returnTypeSpelling() const;

	bool isConst() const;
	bool isVolatile() const;
	bool isStatic() const;

#ifndef NO_RTTI
	const ::std::type_info& returnType() const;
	::std::vector<const ::std::type_info*> argumentTypes() const;
#endif


	VariantValue call(const ::std::vector<VariantValue>& args) const;
	VariantValue call(VariantValue& object, const ::std::vector<VariantValue>& args) const;
	VariantValue call(const VariantValue& object, const ::std::vector<VariantValue>& args) const;
	VariantValue call(volatile VariantValue& object, const ::std::vector<VariantValue>& args) const;
	VariantValue call(const volatile VariantValue& object, const ::std::vector<VariantValue>& args) const;
	
	MethodImpl(const MethodImpl&) = delete;
	MethodImpl(MethodImpl&&) = delete;
	MethodImpl& operator=(const MethodImpl&) = delete;
	MethodImpl& operator=(MethodImpl&&) = delete;

private:

	boundmethod m_method;
	const char* m_name;
	const char* m_returnSpelling;
	const char* m_argSpellings;
	const unsigned int m_numArgs : 5; // up to 32 parameters are supported
	const unsigned int m_isConst : 1;
	const unsigned int m_isVolatile : 1;
	const unsigned int m_isStatic : 1;
#ifndef NO_RTTI
	const ::std::type_info& m_returnType;
	::std::vector<const ::std::type_info*> m_argumentTypes;
#endif
};


template<class _Method>
Method make_method(boundmethod m, const char* name, const char* rString, const char* argString) {
	static MethodImpl impl(
				  m,
				  name,
				  rString,
				  typelist_size<typename method_type<_Method>::Arguments>::value,
				  argString,
				  method_type<_Method>::is_const,
				  method_type<_Method>::is_volatile,
				  false
	#ifndef NO_RTTI
				  , typeid(typename method_type<_Method>::Result)
				  , get_typeinfo<typename method_type<_Method>::Arguments>()
	#endif
				  );

	return Method(&impl);
}


template<class C, class _Method>
Method make_static_method(boundmethod m, const char* name, const char* rString, const char* argString)
{

	static MethodImpl impl(
				m,
				name,
				rString,
				typelist_size<typename method_type<_Method>::Arguments>::value,
				argString,
				false,
				false,
				true
  #ifndef NO_RTTI
				, typeid(typename method_type<_Method>::Result)
				, get_typeinfo<typename method_type<_Method>::Arguments>()
  #endif
				);
	return Method(&impl);
}



#endif /* METHOD_H */
