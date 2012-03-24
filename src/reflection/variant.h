#ifndef VARIANT_H
#define VARIANT_H

#include <memory>
#include <stdexcept>
#include <type_traits>
#include <typeinfo>
#include <string>

#include "str_conversion.h"
#include "typeutils.h"


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

class IValueHolder {
public:
	virtual IValueHolder* clone() const = 0;
	
	virtual bool equals(const IValueHolder* rhs) const = 0;
		
	const virtual void * ptrToValue() const = 0;
	
	virtual void * ptrToValue() = 0;
	
	virtual const ::std::type_info& typeId() const = 0;
	
	virtual ::std::size_t sizeOf() const = 0;
	
	virtual ::std::size_t alignOf() const = 0;
	
	virtual bool isPOD() const = 0;
	
	virtual bool isIntegral() const = 0;
	
	virtual bool isFloatingPoint() const = 0;
	
	virtual bool isPointer() const = 0;
	
	virtual bool isStdString() const = 0;
	
	virtual ::std::string convertToString() const = 0;
	
	virtual number_conversion::dst_int_t convertToInteger() const = 0;
	
	virtual number_conversion::dst_float_t convertToFloatingPoint() const = 0;

	virtual ~IValueHolder() noexcept {}
	IValueHolder() = default;
	IValueHolder(const IValueHolder&) = delete;
	IValueHolder& operator=(const IValueHolder&) = delete;
};


template <class T>
class ValueHolder: public IValueHolder {
public:
	typedef T ValueType;
	
	constexpr ValueHolder(ValueType value) : m_value(value) {}
	
	ValueHolder(const ValueHolder&) = delete;
	ValueHolder& operator=(ValueHolder) = delete;
	
	IValueHolder* clone() const override {
			return new ValueHolder(m_value);
	}
	
	virtual bool equals(const IValueHolder* rhs) const override {
		auto that = dynamic_cast<const ValueHolder*>(rhs);
		if (that != nullptr) {
			return m_value == that->m_value;
		}
		return false;
	}
		
	const void * ptrToValue() const override {
		return reinterpret_cast<const void*>(&m_value);
	}
	
	void * ptrToValue() override {
		return reinterpret_cast<void*>(&m_value);
	}
	
	virtual const ::std::type_info& typeId() const override {
		return typeid(ValueType);
	}
	
	virtual ::std::size_t sizeOf() const override {
		return sizeof(ValueType);
	}
	
	virtual ::std::size_t alignOf() const override {
		return alignof(ValueType);
	}
	
	virtual bool isPOD() const {
		return ::std::is_pod<ValueType>::value;
	}
	
	virtual bool isIntegral() const override {
		return ::std::is_integral<ValueType>::value;
	}
	
	virtual bool isFloatingPoint() const override {
		return ::std::is_floating_point<ValueType>::value;
	}
	
	virtual bool isPointer() const override {
		return ::std::is_pointer<ValueType>::value;
	}
	
	virtual bool isStdString() const {
		return ::std::is_same< ::std::string, ValueType>::value;
	}
	
	virtual ::std::string convertToString() const override {
			return ::std::move(toString(m_value));
	}
	
	virtual number_conversion::dst_int_t convertToInteger() const override {
		return ::convertToInteger(m_value);
	}
	
	// convert to largest floating point type
	virtual number_conversion::dst_float_t convertToFloatingPoint() const override {
		return ::convertToFloatingPoint(m_value);
	}
		
private:
	ValueType m_value;
};

// A variant value contains a value type by value
class VariantValue {
public:
	//! Creates an empty variant
	VariantValue() : m_impl() {}
	
	template<class ValueType>
	VariantValue(ValueType t) :	m_impl(new ValueHolder<ValueType>(t)) {}
		
	VariantValue(const VariantValue& rhs) : m_impl(rhs.m_impl->clone()) {}
	
	VariantValue(VariantValue&& rhs) : m_impl(::std::move(rhs.m_impl)) {}
	
	VariantValue& operator=(const VariantValue& rhs) {
		m_impl.reset(rhs.m_impl->clone());
		return *this;
	}
	
	VariantValue& operator=(VariantValue&& rhs) {
		m_impl = ::std::move(rhs.m_impl);
		return *this;
	}
	
	template<class ValueType>
	VariantValue& operator=(ValueType value) {
		m_impl.reset(new ValueHolder<ValueType>(value));
		return *this;
	}
	
	template<class ValueType>
	bool isA() const {
		if (isValid()) {
			return (typeid(ValueType) == m_impl->typeId());
		}
		return false;
	}
	
private:
	template<class ValueType>
	struct directConversion {
		static ValueType value(const ::std::unique_ptr<IValueHolder>& holder, bool * success) {
			if (success != nullptr) *success = true;
			return *reinterpret_cast<const ValueType*>(holder->ptrToValue());
		}
	};
	
	template<class ValueType>
	struct directConversion<ValueType&> {
		static ValueType& value(const ::std::unique_ptr<IValueHolder>& holder, bool * success) {
			if (success != nullptr) *success = true;
			return *reinterpret_cast<ValueType*>(holder->ptrToValue());
		}
	};
	
	template<class ValueType>
	struct integralConversion {
		static ValueType value(const ::std::unique_ptr<IValueHolder>& holder, bool * success) {
			if (success != nullptr) *success = true;
			return static_cast<ValueType>(holder->convertToInteger());
		}
	};
		
	template<class ValueType>
	struct floatConversion {
		static ValueType value(const ::std::unique_ptr<IValueHolder>& holder, bool * success) {
			if (success != nullptr) *success = true;
			return static_cast<ValueType>(holder->convertToFloatingPoint());
		}
	};
		
	template<class ValueType>
	struct stringConversion {
		static ValueType value(const ::std::unique_ptr<IValueHolder>& holder, bool * success) {
			if (success != nullptr) *success = true;
			return holder->convertToString();
		}
	};
	
	template<class ValueType>
	struct impossibleConversion {
		static ValueType value(const ::std::unique_ptr<IValueHolder>& holder, bool * success) {
			if (success != nullptr) *success = false;
			return ValueType();
		}
	};
	
	template<class ValueType>
	struct impossibleConversion<ValueType&> {
		static ValueType& value(const ::std::unique_ptr<IValueHolder>& holder, bool * success) {
			if (success != nullptr) *success = false;
			ValueType* ptr = nullptr;
			return *ptr;
		}
	};
	
public:
	
	template<class ValueType>
	ValueType value() const {
		if (!isA<ValueType>()) {
			throw ::std::runtime_error("variant value is not of the requested type");
		}
		return directConversion<ValueType>::value(m_impl, nullptr);
	}
	
	template<class ValueType>
	ValueType convertTo(bool * success = nullptr) const {
		check_valid();
		
		if (isA<ValueType>()) {
			if (success != nullptr) * success = true;
			return value<ValueType>();		
		}
		
		return Select< ::std::is_integral<ValueType>::value,
				integralConversion<ValueType>,
				typename Select< ::std::is_floating_point<ValueType>::value,
					floatConversion<ValueType>,
					typename Select< ::std::is_same<ValueType, ::std::string>::value,
						stringConversion<ValueType>,
						impossibleConversion<ValueType>>::result>::result>::result::value(m_impl, success);
	}
	
	// checks if a conversion is supported
	// Even if this method return true, the conversion can fail
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
	
	bool isValid() const { return m_impl.get() != nullptr; }
	
	const ::std::type_info& typeId() const {
		check_valid();
		return m_impl->typeId();
	}
	
	// Is a Plain-Old-Datatype (can be memcpy'd)
	bool isStdString() const {
		check_valid();
		return m_impl->isStdString();
	}
	
	// Is a Plain-Old-Datatype (can be memcpy'd)
	bool isIntegral() const {
		check_valid();
		return m_impl->isIntegral();
	}
	
	bool isFloatingPoint() const {
		check_valid();
		return m_impl->isFloatingPoint();
	}
	
	bool isArithmetical() const {
		return isIntegral() || isFloatingPoint();
	}
	
	// Is a Plain-Old-Datatype (can be memcpy'd)
	bool isPOD() const {
		check_valid();
		return m_impl->isPOD();
	}
	
	::std::size_t sizeOf() const {
		check_valid();
		return m_impl->sizeOf();
	}
	
	::std::size_t alignOf() const {
		check_valid();
		return m_impl->alignOf();
	}
	
	// If the type is a POD and you know what you're doing, you can memcpy it, but beware of the alignment
	const void * ptrToValue() const {
		check_valid();
		return m_impl->ptrToValue();
	}
	
	
	
private:
	std::unique_ptr<IValueHolder> m_impl;
	
	void check_valid() const {
		if (!isValid()) {
			throw ::std::runtime_error("variant has no value");
		}
	}
	
	friend bool operator==(const VariantValue& v1, const VariantValue& v2);
};

inline bool operator==(const VariantValue& v1, const VariantValue& v2) {
	
	if ((v1.isValid() == false) && (v2.isValid() == false)) {
		return true;
	}
	if ((v1.isValid() == false) || (v2.isValid() == false)) {
		return false;
	}
	
	if (v1.isArithmetical() && v2.isArithmetical()) {
		if (v1.isIntegral() && v2.isIntegral()) {
			return v1.m_impl->convertToInteger() == v2.m_impl->convertToInteger();
		}
		return v1.m_impl->convertToInteger() == v2.m_impl->convertToFloatingPoint();
	}
	
	return v1.m_impl->equals(v2.m_impl.get());
}

inline bool operator!=(const VariantValue& v1, const VariantValue& v2) {
	return !(v1 == v2);
}


#endif /* VARIANT_H */