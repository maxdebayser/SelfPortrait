#ifndef CONSTRUCTOR_H
#define CONSTRUCTOR_H

#include "typelist.h"
#include "variant.h"
#include <tuple>
#include <array>
#include "reflection.h"
#include "str_utils.h"
#include "call_utils.h"

class AbstractConstructorImpl: public Annotated {
public:	

	AbstractConstructorImpl(int numArgs, const char* argSpellings)
		: m_numArgs(numArgs)
		, m_argSpellings(argSpellings)
	{}

	::std::size_t numberOfArguments() const { return m_numArgs; }
	
#ifndef NO_RTTI
	virtual ::std::vector<const ::std::type_info*> argumentTypes() const = 0;
#endif

	::std::vector< ::std::string> argumentSpellings() const { return splitArgs(m_argSpellings); }
	
	virtual VariantValue call(const ::std::vector<VariantValue>& args) const = 0;


private:
	const char* m_argSpellings;
	unsigned int m_numArgs : 5;
};

namespace {

template<class _Clazz, class... Args>
class ConstructorImpl: public AbstractConstructorImpl {
public:
	typedef _Clazz Clazz;
	typedef TypeList<Args...> Arguments;

#ifndef NO_RTTI
	virtual ::std::vector<const ::std::type_info*> argumentTypes() const { return get_typeinfo<Arguments>(); }
#endif

	ConstructorImpl(const char* argSpellings)
		: AbstractConstructorImpl(size<Arguments>(), argSpellings) {}
	
	
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


		VariantValue call(const ::std::vector<VariantValue>& args) {
			verify_call<Arguments, I...>(args);
			VariantValue ret;
			ret.construct<Clazz>(args[I].moveValue<typename type_at<Arguments, I>::type>()...);
			return ret;
		}
	};
	
	virtual VariantValue call(const ::std::vector<VariantValue>& args) const {
		call_helper< ::std::is_abstract<Clazz>::value, typename make_indices<sizeof...(Args)>::type> helper;
		return helper.call(args);
	}
};

}

template<class Clazz, class... Args>
Constructor make_constructor(const char* argString) {
	static ConstructorImpl<Clazz, Args...> impl(argString);
	return Constructor(&impl);
}

#endif /* CONSTRUCTOR_H */
