/*
** SelfPortrait API
** See Copyright Notice in reflection.h
*/
#ifndef CALL_UTILS_H
#define CALL_UTILS_H

#include "typelist.h"
#include "typeutils.h"
#include "str_conversion.h"

#include <array>

namespace {

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
