#ifndef VARIANT_H
#define VARIANT_H

#include <memory>
#include <stdexcept>


#ifndef NO_RTTI
#include <typeinfo>
#endif

#include <type_traits>
#include <string>
#include <utility>

#include "str_conversion.h"
#include "typeutils.h"

namespace {

namespace number_conversion {
	
	typedef long long int dst_int_t; // largest integer type
	typedef long double dst_float_t; // larget floating point type
	
	template<class T, bool ArithType>
	struct convert {
		static dst_int_t convertToInteger(const T&, bool* success) {
			if (success != nullptr) *success = false;
			return 0;
		}
		static dst_float_t convertToFloat(const T&, bool* success) {
			if (success != nullptr) *success = false;
			return 0.0;
		}
	};
	
	template<class T>
	struct convert<T, true> {
		static dst_int_t convertToInteger(T t, bool* success) {
			if (success != nullptr) *success = true;
			return static_cast<dst_int_t>(t);
		}
		static dst_float_t convertToFloat(T t, bool* success) {
			if (success != nullptr) *success = true;
			return static_cast<dst_float_t>(t);
		}
	};
	
	template<>
	struct convert< ::std::string, true> {
		static dst_int_t convertToInteger(const ::std::string& str, bool* success) {
			return ::fromString<dst_int_t>(str, success);
		}
		static dst_float_t convertToFloat(const ::std::string& str, bool* success) {
			return ::fromString<dst_float_t>(str, success);
		}
	};
	
	// If it were a char* instead of a const char*, chances would be higher that it's
	// not a 0-terminated string. Even with const char* this danger exists, but it's
	// usualy used for string literals
	template<>
	struct convert<const char*, true> {
		static dst_int_t convertToInteger(const ::std::string& str, bool* success) {
			return ::fromString<dst_int_t>(str, success);
		}
		static dst_float_t convertToFloat(const ::std::string& str, bool* success) {
			return ::fromString<dst_float_t>(str, success);
		}
	};
}


template<class T>
constexpr bool convertibleToInteger() {
		return ::std::is_arithmetic<T>::value || ::std::is_convertible<T, ::number_conversion::dst_int_t>::value || ::std::is_same<T, ::std::string>::value || ::std::is_same<T, const char *>::value;
}

template<class T>
constexpr bool convertibleToFloatingPoint() {
		return ::std::is_arithmetic<T>::value || ::std::is_convertible<T, ::number_conversion::dst_float_t>::value || ::std::is_same<T, ::std::string>::value || ::std::is_same<T, const char *>::value;;
}

template<class T>
constexpr number_conversion::dst_int_t convertToInteger(const T& t, bool* success = nullptr) {
	return number_conversion::convert<T, convertibleToInteger<T>()>::convertToInteger(t, success);
}

template<class T>
constexpr number_conversion::dst_float_t convertToFloatingPoint(const T& t, bool* success = nullptr) {
	return number_conversion::convert<T, convertibleToFloatingPoint<T>()>::convertToFloat(t, success);
}

}

union number_return {
	number_conversion::dst_int_t i;
	number_conversion::dst_float_t f;
};

enum class NumberType {
	INTEGER,
	FLOATING
};

class IValueHolder {
public:

	constexpr IValueHolder(
			const void * ptr,
#ifndef NO_RTTI
			const ::std::type_info& typeId,
#endif
			std::size_t sizeOf,
			std::size_t alignOf,
			bool isPod,
			bool isIntegral,
			bool isFloatingPoint,
			bool isPointer,
			bool isStdString)
		: m_ptrToValue(ptr)
#ifndef NO_RTTI
		, m_typeId(typeId)
#endif
		, m_sizeOf(sizeOf)
		, m_alignOf(alignOf)
		, m_isPod(isPod)
		, m_isIntegral(isIntegral)
		, m_isFloatingPoint(isFloatingPoint)
		, m_isPointer(isPointer)
		, m_isStdString(isStdString)
	{}

	virtual IValueHolder* clone() const = 0;
	
	virtual bool equals(const IValueHolder* rhs) const = 0;

	void * ptrToValue() { return const_cast<void*>(m_ptrToValue); }


	const void * ptrToValue() const {
		return m_ptrToValue;
	}
	
#ifndef NO_RTTI
	const ::std::type_info& typeId() const { return m_typeId; }
#endif
	
	::std::size_t sizeOf() const { return m_sizeOf; }
	
	::std::size_t alignOf() const { return m_alignOf; }
	
	bool isPOD() const { return m_isPod; }
	
	bool isIntegral() const { return m_isIntegral; }
	
	bool isFloatingPoint() const { return m_isFloatingPoint; }
	
	bool isPointer() const { return m_isPointer; }
	
	bool isStdString() const { return m_isStdString; }

	virtual void throwCast() const = 0;
	
	virtual ::std::string convertToString() const = 0;

	virtual number_return convertToNumber(NumberType t) const = 0;

	virtual ~IValueHolder() noexcept {}
	IValueHolder() = default;
	IValueHolder(const IValueHolder&) = delete;
	IValueHolder& operator=(const IValueHolder&) = delete;

private:
	const void * m_ptrToValue;

#ifndef NO_RTTI
	const ::std::type_info& m_typeId;
#endif

	const std::size_t m_sizeOf;

	const std::size_t m_alignOf;

	const unsigned int m_isPod : 1;
	const unsigned int m_isIntegral : 1;
	const unsigned int m_isFloatingPoint : 1;
	const unsigned int m_isPointer : 1;
	const unsigned int m_isStdString : 1;
};

namespace {

template <class T>
class ValueHolder: public IValueHolder {

	template<class Dummy,bool OK>
	struct CloneHelper {
		static IValueHolder* clone(const ValueHolder* holder) {
			return new ValueHolder(holder->m_value);
		}
	};

	template<class Dummy>
	struct CloneHelper<Dummy, false> {
		static IValueHolder* clone(const ValueHolder*) {
			throw std::runtime_error("type has no copy constructor");
		}
	};

	typedef CloneHelper<T, ::std::is_constructible<T, T>::value> Cloner;


	template<class U,bool OK>
	struct CompareHelper {
		static bool equal(const U& u1, const U& u2) {
			return u1 == u2;
		}
	};

	template<class U>
	struct CompareHelper<U, false> {
		static bool equal(const U& u1, const U& u2) {
			throw std::runtime_error("no equality operator exists for type");
		}
	};

	typedef CompareHelper<T, comparable<T>::value> Compare;


public:
	typedef T ValueType;

	template<class... Args>
	ValueHolder(Args&&... args)
		: IValueHolder(reinterpret_cast<const void*>(&m_value),
#ifndef NO_RTTI
					   typeid(ValueType),
#endif
					   sizeof(ValueType),
					   alignof(ValueType),
					   ::std::is_pod<ValueType>::value,
					   ::std::is_integral<ValueType>::value,
					   ::std::is_floating_point<ValueType>::value,
					   ::std::is_pointer<ValueType>::value,
					   ::std::is_same< ::std::string, ValueType>::value),
		m_value( ::std::forward<Args>(args)...)
	{
	}

	~ValueHolder() noexcept{
		//m_value.~ValueType();
	}

	ValueHolder(const ValueHolder&) = delete;
	ValueHolder& operator=(ValueHolder) = delete;

	IValueHolder* clone() const override {
		return Cloner::clone(this);
	}

	virtual bool equals(const IValueHolder* rhs) const override {
		if (rhs != nullptr) {
			try {
				rhs->throwCast();
			} catch (ValueType* ptr) {
				return Compare::equal(m_value, *ptr);
			} catch (...) {}
		}
		return false;
	}

	virtual ::std::string convertToString() const override {
			return ::std::move(toString(m_value));
	}

	virtual number_return convertToNumber(NumberType t) const {
		number_return r;
		if (t == NumberType::INTEGER) {
			r.i = ::convertToInteger(m_value);
		} else {
			r.f = ::convertToFloatingPoint(m_value);
		}
		return r;
	}

	virtual void throwCast() const {
		throw const_cast<ValueType*>(&m_value);
	}

private:
	ValueType m_value;
};


template <class T>
class ValueHolder<T&>: public IValueHolder {
public:
	typedef T& RefType;
	typedef typename normalize_type<T&>::type ValueType;
private:

	template<class Dummy,bool OK>
	struct CloneHelper {
		static IValueHolder* clone(const ValueHolder* holder) {
			return new ValueHolder(holder->m_value);
		}
	};

	template<class Dummy>
	struct CloneHelper<Dummy, false> {
		static IValueHolder* clone(const ValueHolder*) {
			throw std::runtime_error("type has no copy constructor");
		}
	};

	typedef CloneHelper<ValueType, ::std::is_constructible<ValueType, ValueType>::value> Cloner;


	template<class U,bool OK>
	struct CompareHelper {
		static bool equal(const U& u1, const U& u2) {
			return u1 == u2;
		}
	};

	template<class U>
	struct CompareHelper<U, false> {
		static bool equal(const U& u1, const U& u2) {
			throw std::runtime_error("no equality operator exists for type");
		}
	};

	typedef CompareHelper<ValueType, comparable<ValueType>::value> Compare;


public:

	ValueHolder(RefType v)
		: IValueHolder(reinterpret_cast<const void*>(&v),
#ifndef NO_RTTI
					   typeid(ValueType),
#endif
					   sizeof(ValueType),
					   alignof(ValueType),
					   ::std::is_pod<ValueType>::value,
					   ::std::is_integral<ValueType>::value,
					   ::std::is_floating_point<ValueType>::value,
					   ::std::is_pointer<ValueType>::value,
					   ::std::is_same< ::std::string, ValueType>::value),
		  m_value(v) {}

	~ValueHolder() noexcept {
	}

	ValueHolder(const ValueHolder&) = delete;
	ValueHolder& operator=(ValueHolder) = delete;

	IValueHolder* clone() const override {
		return Cloner::clone(this);
	}

	virtual bool equals(const IValueHolder* rhs) const override {
		if (rhs != nullptr) {
			try {
				rhs->throwCast();
			} catch (const ValueType* ptr) {
				return Compare::equal(m_value, *ptr);
			} catch (...) {}
		}
		return false;
	}


	virtual ::std::string convertToString() const override {
			return ::std::move(toString(m_value));
	}

	virtual number_return convertToNumber(NumberType t) const {
		number_return r;
		if (t == NumberType::INTEGER) {
			r.i = ::convertToInteger(m_value);
		} else {
			r.f = ::convertToFloatingPoint(m_value);
		}
		return r;
	}

	virtual void throwCast() const {
		throw &m_value;
	}

private:
	RefType m_value;
};

}

// A variant value contains a value type by value
class VariantValue {
public:
	//! Creates an empty variant
	explicit VariantValue();

	template<class ValueType>
	VariantValue(const ValueType& t) : m_impl(new ValueHolder<ValueType>(t)) {}

	template<class ValueType>
	VariantValue(ValueType* t) : m_impl(new ValueHolder<ValueType*>(t)) {}


	template<class ValueType, class... Args>
	VariantValue& construct(Args&&... args) {
		m_impl = ::std::unique_ptr<ValueHolder<ValueType>>(new ValueHolder<ValueType>( ::std::forward<Args>(args)... ));
		return *this;
	}

	VariantValue(const VariantValue& rhs);
	
	VariantValue(VariantValue&& rhs);
	
	VariantValue& operator=(const VariantValue& rhs);
	
	VariantValue& operator=(VariantValue&& rhs);
	
	template<class ValueType>
	VariantValue& operator=(ValueType value) {
		m_impl.reset(new ValueHolder<ValueType>(value));
		return *this;
	}

	VariantValue createReference() const;
	
	template<class ValueType>
	bool isA() const {
		return (isValid() && (isAPriv<ValueType>() != nullptr));
	}

	template<class ValueType>
	bool isA() const volatile {
		return const_cast<const VariantValue*>(this)->template isA<ValueType>();
	}
	
private:

	template<class ValueType>
	typename normalize_type<ValueType>::ptr_type isAPriv() const {
		if (isValid()) {
			try {
				m_impl->throwCast();
			} catch(typename normalize_type<ValueType>::ptr_type ptr) {
				return ptr;
			} catch (...) {
				return nullptr;
			}
		}
		return nullptr;
	}

	template<class ValueType>
	struct pointerConversion {
		static ValueType value(const ::std::shared_ptr<IValueHolder>& holder, bool * success) {

			try {
				holder->throwCast();
			} catch (ValueType& ptr) {
				if (success != nullptr) *success = true;
				return ptr;
			} catch(...) { }
			if (success != nullptr) *success = false;
			return ValueType();
		}
		static ValueType value(const ::std::shared_ptr<IValueHolder>& holder) {
			try {
				holder->throwCast();
			} catch (ValueType& ptr) {
				return ptr;
			} catch(...) { }
			throw std::runtime_error("failed to convert variant to pointer");
		}
	};

	template<class ValueType>
	struct integralConversion {
		static ValueType value(const ::std::shared_ptr<IValueHolder>& holder, bool * success) {
			if (success != nullptr) *success = true;
			number_return r = holder->convertToNumber(NumberType::INTEGER);
			return static_cast<ValueType>(r.i);
		}
		static ValueType value(const ::std::shared_ptr<IValueHolder>& holder) {
			return value(holder, nullptr);
		}
	};

	template<class ValueType>
	struct floatConversion {
		static ValueType value(const ::std::shared_ptr<IValueHolder>& holder, bool * success) {
			if (success != nullptr) *success = true;
			number_return r = holder->convertToNumber(NumberType::FLOATING);
			return static_cast<ValueType>(r.f);
		}
		static ValueType value(const ::std::shared_ptr<IValueHolder>& holder) {
			return value(holder, nullptr);
		}
	};

	template<class ValueType>
	struct stringConversion {
		static ValueType value(const ::std::shared_ptr<IValueHolder>& holder, bool * success) {
			if (success != nullptr) *success = true;
			return holder->convertToString();
		}
		static ValueType value(const ::std::shared_ptr<IValueHolder>& holder) {
			return value(holder, nullptr);
		}
	};

	template<class ValueType>
	struct impossibleConversion {
		static ValueType& value(const ::std::shared_ptr<IValueHolder>& holder, bool * success) {
			if (success != nullptr) *success = false;
			ValueType* ptr = nullptr;
			return *ptr;
		}
		static ValueType value(const ::std::shared_ptr<IValueHolder>& holder) {
			throw std::runtime_error("failed conversion");
		}
	};

	template<class ValueType>
	struct impossibleConversion<ValueType&> {
		static ValueType& value(const ::std::shared_ptr<IValueHolder>& holder, bool * success) {
			if (success != nullptr) *success = false;
			ValueType* ptr = nullptr;
			return *ptr;
		}
		static ValueType& value(const ::std::shared_ptr<IValueHolder>& holder) {
			throw std::runtime_error("failed conversion");
		}
	};

	template<class ValueType>
	struct impossibleConversion<ValueType&&> {
		static ValueType& value(const ::std::shared_ptr<IValueHolder>& holder, bool * success) {
			if (success != nullptr) *success = false;
			ValueType* ptr = nullptr;
			return *ptr;
		}
		static ValueType&& value(const ::std::shared_ptr<IValueHolder>& holder) {
			throw std::runtime_error("failed conversion");
		}
	};


public:
	
	template<class ValueType>
	ValueType value() const {

		auto ptr = isAPriv< ValueType >();

		if (ptr == nullptr) {
			throw ::std::runtime_error("variant value is not of the requested type");
		}
		return *ptr;
	}
	
	template<class ValueType>
	ValueType convertTo(bool * success = nullptr) const {
		check_valid();

		auto ptr = isAPriv<ValueType>();
		
		if (ptr != nullptr) {
			if (success != nullptr) * success = true;
			return *ptr;
		}
		
		return Select< ::std::is_integral<ValueType>::value,
				integralConversion<ValueType>,
				typename Select< ::std::is_floating_point<ValueType>::value,
					floatConversion<ValueType>,
					typename Select< ::std::is_same<ValueType, ::std::string>::value,
						stringConversion<ValueType>,
						typename Select< ::std::is_pointer<ValueType>::value,
								pointerConversion<ValueType>,
								impossibleConversion<ValueType>>::type>::type>::type>::type::value(m_impl, success);
	}

	template<class ValueType>
	ValueType convertToThrow() const {
		check_valid();

		auto ptr = isAPriv<ValueType>();

		if (ptr != nullptr) {
			return *ptr;
		}

		return Select< ::std::is_integral<ValueType>::value,
				integralConversion<ValueType>,
				typename Select< ::std::is_floating_point<ValueType>::value,
					floatConversion<ValueType>,
					typename Select< ::std::is_same<ValueType, ::std::string>::value,
						stringConversion<ValueType>,
						typename Select< ::std::is_pointer<ValueType>::value,
								pointerConversion<ValueType>,
								impossibleConversion<ValueType>>::type>::type>::type>::type::value(m_impl);
	}

	template<class ValueType>
	ValueType moveValue(bool * success = nullptr) const {
		check_valid();

		auto ptr = isAPriv<ValueType>();

		if (ptr != nullptr) {
			if (success != nullptr) * success = true;
			return ::std::forward<ValueType>(*ptr);
		}

		return ::std::forward<ValueType>(Select< ::std::is_integral<ValueType>::value,
										 integralConversion<ValueType>,
										 typename Select< ::std::is_floating_point<ValueType>::value,
											 floatConversion<ValueType>,
											 typename Select< ::std::is_same<ValueType, ::std::string>::value,
												 stringConversion<ValueType>,
												 typename Select< ::std::is_pointer<ValueType>::value,
														 pointerConversion<ValueType>,
														 impossibleConversion<ValueType>>::type>::type>::type>::type::value(m_impl, success));
	}

	template<class ValueType>
	ValueType moveValueThrow() const {
		check_valid();

		auto ptr = isAPriv<ValueType>();

		if (ptr != nullptr) {
			return ::std::forward<ValueType>(*ptr);
		}

		return ::std::forward<ValueType>(Select< ::std::is_integral<ValueType>::value,
										 integralConversion<ValueType>,
										 typename Select< ::std::is_floating_point<ValueType>::value,
											 floatConversion<ValueType>,
											 typename Select< ::std::is_same<ValueType, ::std::string>::value,
												 stringConversion<ValueType>,
												 typename Select< ::std::is_pointer<ValueType>::value,
														 pointerConversion<ValueType>,
														 impossibleConversion<ValueType>>::type>::type>::type>::type::value(m_impl));
	}

	template<class ValueType>
	ValueType convertTo(bool * success = nullptr) const volatile {
		return const_cast<VariantValue&>(*this).convertTo<ValueType>(success);
	}
	
	// checks if a conversion is supported
	// Even if this method returns true, the conversion can fail
	// For example a string can be converted to int if it contains a number
	template<class ValueType>
	bool mayConvertTo() const {
		check_valid();
		
		if (isA<ValueType>() ||
			::std::is_integral<ValueType>::value ||
			::std::is_floating_point<ValueType>::value ||
			::std::is_same<ValueType, ::std::string>::value
		) {
			return true;		
		} else {
			return false;
		}
	}
	
	bool isValid() const;

#ifndef NO_RTTI
	const ::std::type_info& typeId() const;
#endif

	bool isStdString() const;

	bool isIntegral() const;
	
	bool isFloatingPoint() const;
	
	bool isArithmetical() const;
	
	// Is a Plain-Old-Datatype (can be memcpy'd)
	bool isPOD() const;
	
	::std::size_t sizeOf() const;
	
	::std::size_t alignOf() const;
	
	// If the type is a POD and you know what you're doing, you can memcpy it, but beware of the alignment
	const void * ptrToValue() const;
	
	
	
private:
	std::shared_ptr<IValueHolder> m_impl;
	
	void check_valid() const;
	
	friend bool operator==(const VariantValue& v1, const VariantValue& v2);
};

bool operator==(const VariantValue& v1, const VariantValue& v2);

bool operator!=(const VariantValue& v1, const VariantValue& v2);


#endif /* VARIANT_H */
