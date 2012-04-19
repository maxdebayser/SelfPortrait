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

class IValueHolder {
public:

	constexpr IValueHolder(std::size_t sizeOf,
						   std::size_t alignOf,
						   bool isPod,
						   bool isIntegral,
						   bool isFloatingPoint,
						   bool isPointer,
						   bool isStdString)
		: m_sizeOf(sizeOf)
		, m_alignOf(alignOf)
		, m_isPod(isPod)
		, m_isIntegral(isIntegral)
		, m_isFloatingPoint(isFloatingPoint)
		, m_isPointer(isPointer)
		, m_isStdString(isStdString)
	{}

	virtual IValueHolder* clone() const = 0;
	
	virtual bool equals(const IValueHolder* rhs) const = 0;
		
	const virtual void * ptrToValue() const = 0;
	
	void * ptrToValue() { return const_cast<void*>(static_cast<const IValueHolder*>(this)->ptrToValue()); }
	
#ifndef NO_RTTI
	virtual const ::std::type_info& typeId() const = 0;
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
	
	virtual number_conversion::dst_int_t convertToInteger() const = 0;
	
	virtual number_conversion::dst_float_t convertToFloatingPoint() const = 0;

	virtual ~IValueHolder() noexcept {}
	IValueHolder() = default;
	IValueHolder(const IValueHolder&) = delete;
	IValueHolder& operator=(const IValueHolder&) = delete;

private:
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
		: IValueHolder(sizeof(ValueType),
					 alignof(ValueType),
					 ::std::is_pod<ValueType>::value,
					 ::std::is_integral<ValueType>::value,
					 ::std::is_floating_point<ValueType>::value,
					 ::std::is_pointer<ValueType>::value,
					 ::std::is_same< ::std::string, ValueType>::value),
		m_value(*reinterpret_cast<ValueType*>(m_hack.c)) {
		new(m_hack.c) ValueType( ::std::forward<Args>(args)...);
	}

	~ValueHolder() noexcept{
		m_value.~ValueType();
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

	const void * ptrToValue() const override {
		return reinterpret_cast<const void*>(&m_value);
	}

#ifndef NO_RTTI
	virtual const ::std::type_info& typeId() const override {
		return typeid(ValueType);
	}
#endif

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

	virtual void throwCast() const {
		throw &m_value;
	}

private:
	/*alignas(ValueType) char m_buffer[sizeof(ValueType)]*/

	// Ugly hack until g++ supports alignas
	union align_hack {
		char c[sizeof(ValueType)];
		typename ::std::aligned_storage<sizeof(ValueType), alignof(ValueType)>::type placeholder;
	} m_hack;
	ValueType& m_value;
};


template <class T>
class ValueHolder<T&>: public IValueHolder {
public:
	typedef T& RefType;
	typedef typename strip_reference<T&>::type ValueType;
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
		: IValueHolder(sizeof(ValueType),
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

	const void * ptrToValue() const override {
		return reinterpret_cast<const void*>(&m_value);
	}

#ifndef NO_RTTI
	virtual const ::std::type_info& typeId() const override {
		return typeid(ValueType);
	}
#endif

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
	explicit VariantValue() : m_impl() {}
	
	template<class ValueType>
	VariantValue(const ValueType& t) : m_impl(new ValueHolder<ValueType>(t)) {}

	template<class ValueType>
	VariantValue(ValueType* t) : m_impl(new ValueHolder<ValueType*>(t)) {}


	template<class ValueType, class... Args>
	VariantValue& construct(Args&&... args) {
		m_impl = ::std::unique_ptr<ValueHolder<ValueType>>(new ValueHolder<ValueType>( ::std::forward<Args>(args)... ));
		return *this;
	}

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

	VariantValue createReference() {
		VariantValue ret;
		ret.m_impl = m_impl;
		return ::std::move(ret);
	}
	
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
	typename strip_reference<ValueType>::ptr_type isAPriv() const {
		if (isValid()) {
			try {
				m_impl->throwCast();
			} catch(typename strip_reference<ValueType>::ptr_type ptr) {
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
	};

	template<class ValueType>
	struct integralConversion {
		static ValueType value(const ::std::shared_ptr<IValueHolder>& holder, bool * success) {
			if (success != nullptr) *success = true;
			return static_cast<ValueType>(holder->convertToInteger());
		}
	};
		
	template<class ValueType>
	struct floatConversion {
		static ValueType value(const ::std::shared_ptr<IValueHolder>& holder, bool * success) {
			if (success != nullptr) *success = true;
			return static_cast<ValueType>(holder->convertToFloatingPoint());
		}
	};
		
	template<class ValueType>
	struct stringConversion {
		static ValueType value(const ::std::shared_ptr<IValueHolder>& holder, bool * success) {
			if (success != nullptr) *success = true;
			return holder->convertToString();
		}
	};
	
	template<class ValueType>
	struct impossibleConversion {
		static ValueType value(const ::std::shared_ptr<IValueHolder>& holder, bool * success) {
			if (success != nullptr) *success = false;
			return ValueType();
		}
	};
	
	template<class ValueType>
	struct impossibleConversion<ValueType&> {
		static ValueType& value(const ::std::shared_ptr<IValueHolder>& holder, bool * success) {
			if (success != nullptr) *success = false;
			ValueType* ptr = nullptr;
			return *ptr;
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
	ValueType&& moveValue(bool * success = nullptr) const {
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
						impossibleConversion<ValueType>>::type>::type>::type::value(m_impl, success));
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
	
	bool isValid() const { return m_impl.get() != nullptr; }

#ifndef NO_RTTI
	const ::std::type_info& typeId() const {
		check_valid();
		return m_impl->typeId();
	}
#endif
	
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
	std::shared_ptr<IValueHolder> m_impl;
	
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
