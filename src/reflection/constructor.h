#ifndef CONSTRUCTOR_H
#define CONSTRUCTOR_H

#include "typelist.h"
#include "variant.h"
#include <tuple>
#include <array>
#include "reflection.h"
#include "str_utils.h"


class AbstractConstructorImpl: public Annotated {
public:	
	virtual ::std::size_t numberOfArguments() const = 0;
	
	virtual ::std::vector<const ::std::type_info*> argumentTypes() const = 0;

	virtual const ::std::vector< ::std::string>& argumentSpellings() const = 0;
	
	virtual VariantValue call(const ::std::vector<VariantValue>& args) const = 0;
};

template<class _Clazz, class... Args>
class ConstructorImpl: public AbstractConstructorImpl {
public:
	typedef _Clazz Clazz;
	typedef TypeList<Args...> Arguments;
	
	virtual ::std::size_t numberOfArguments() const { return size<Arguments>(); }
	
	virtual ::std::vector<const ::std::type_info*> argumentTypes() const { return get_typeinfo<Arguments>(); }
	virtual const ::std::vector< ::std::string>& argumentSpellings() const { return m_argSpellings; }

	ConstructorImpl(::std::vector< ::std::string>&& argSpellings) : m_argSpellings(argSpellings) {}
	
	
	template<bool Abstract, class Ind>
	struct call_helper;

	template<class Ind>
	struct call_helper<true, Ind> {
		VariantValue call(const ::std::vector<VariantValue>& args) {
			throw ::std::runtime_error("Class declares pure virtual members");
		}
	};
	
	template<::std::size_t... I, template< ::std::size_t...> class Ind>
	struct call_helper<false, Ind<I...>> {

		::std::array<bool,sizeof...(I)> success;
				
		static_assert(size<Arguments>() == sizeof...(I), "number of arguments and number of indices don't match");
		
		VariantValue call(const ::std::vector<VariantValue>& args) {
			success.fill(false);
			if (static_cast<int>(args.size()) < static_cast<int>(sizeof...(I))) {
				throw ::std::runtime_error("constructor called with insufficient number of arguments");
			}
			
			sink(args[I].moveValue<Args>(&success[I])...);
			
			for (::std::size_t i = 0; i < success.size(); ++i) {
				if (success[i] == false) {
					throw ::std::runtime_error(::fmt_str("constructor called with incompatible argument at position %1", i) );
				}
			}
			VariantValue ret;
			ret.construct<Clazz>(args[I].moveValue<typename type_at<Arguments, I>::type>()...);
			return ret;
		}
	};
	
	virtual VariantValue call(const ::std::vector<VariantValue>& args) const {
		call_helper< ::std::is_abstract<Clazz>::value, typename make_indices<sizeof...(Args)>::type> helper;
		return helper.call(args);
	}
private:
	::std::vector< ::std::string> m_argSpellings;
};




template<class Clazz, class... Args>
Constructor make_constructor(const char* argString) {
	static ConstructorImpl<Clazz, Args...> impl(splitArgs(argString));
	return Constructor(&impl);
}

#endif /* CONSTRUCTOR_H */
