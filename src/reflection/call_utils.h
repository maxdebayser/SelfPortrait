/*
** SelfPortrait API
** See Copyright Notice in reflection.h
*/
#ifndef CALL_UTILS_H
#define CALL_UTILS_H

#include "typelist.h"
#include "typeutils.h"


#include <array>

namespace {
	template< ::size_t N>
	struct call_verifier {
		::std::array<bool,N> success;
		call_verifier(::std::size_t actual) {
			if (static_cast<int>(actual) < static_cast<int>(N)) {
				throw ::std::runtime_error("function or constructor called with insufficient number of arguments");
			}
			success.fill(false);
		}
		void assert_conversion_succeded() const {
			for (::std::size_t i = 0; i < success.size(); ++i) {
				if (success[i] == false) {
                    throw ::std::runtime_error(::strconv::fmt_str("function or constructor called with incompatible argument at position %1", i) );
				}
			}
		}
	};

	template<class Arguments, std::size_t... I >
	void verify_call(const ::std::vector<VariantValue>& args) {
		call_verifier<sizeof...(I)> ver(args.size());
		sink(args[I].moveValue<typename type_at<Arguments, I>::type>(&ver.success[I])...);
		ver.assert_conversion_succeded();
	}

	template<class Clazz>
	Clazz& verifyObject(const volatile VariantValue& object, bool methodIsConst) {
		bool success = false;
		const Clazz& ref = object.convertTo<const Clazz&>(&success);

		if (!success) {
			throw ::std::runtime_error("method called with wrong type of object");
		}
		if (!methodIsConst) {
			object.convertTo<Clazz&>(&success);
			if (!success) {
				throw ::std::runtime_error("non-const method called with const object");
			}
		}
		return const_cast<Clazz&>(ref);
	}
}

#endif /* CALL_UTILS_H*/
