/*
** SelfPortrait API
** See Copyright Notice in reflection.h
*/
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
#include "conversion_cache.h"
#include <cassert>
#include <cstddef>
#include <cstdint>

#include <iostream>
using namespace std;

namespace {

namespace number_conversion {
	
	typedef long long int dst_int_t; // largest integer type
    //typedef long double dst_float_t; // largest floating point type
    typedef double dst_float_t; // larget floating point type that doesn't trigger warnings
	
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
    struct convert<std::string, true> {
		static dst_int_t convertToInteger(const ::std::string& str, bool* success) {
            return ::strconv::fromString<dst_int_t>(str, success);
		}
		static dst_float_t convertToFloat(const ::std::string& str, bool* success) {
            return ::strconv::fromString<dst_float_t>(str, success);
		}
	};
	
	// If it were a char* instead of a const char*, chances would be higher that it's
	// not a 0-terminated string. Even with const char* this danger exists, but it's
	// usualy used for string literals
	template<>
	struct convert<const char*, true> {
		static dst_int_t convertToInteger(const ::std::string& str, bool* success) {
            return ::strconv::fromString<dst_int_t>(str, success);
		}
		static dst_float_t convertToFloat(const ::std::string& str, bool* success) {
            return ::strconv::fromString<dst_float_t>(str, success);
		}
	};
}


template<class T>
constexpr bool convertibleToInteger() {
        return ::std::is_arithmetic<T>::value || ::std::is_convertible<T, ::number_conversion::dst_int_t>::value || ::std::is_enum<T>::value || ::std::is_same<T, ::std::string>::value || ::std::is_same<T, const char *>::value;
}

template<class T>
constexpr bool convertibleToFloatingPoint() {
        return ::std::is_arithmetic<T>::value || ::std::is_convertible<T, ::number_conversion::dst_float_t>::value || ::std::is_enum<T>::value || ::std::is_same<T, ::std::string>::value || ::std::is_same<T, const char *>::value;;
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

namespace alignment_helper {
    constexpr uint64_t check(uint64_t x , uint64_t n, uint64_t shift) {
        return (shift == 1) ?
                (((x >> 1) != 0) ?  n - 2 : n - x) :
                (((x >> shift) != 0) ?
                        check(x >> shift, n-shift, shift >> 1)  :
                        check(x, n, shift >> 1));
    }

    constexpr unsigned int nlz(uint64_t x) {
        return check(x, 64, 32);
    }

    constexpr unsigned int alignement_bits(uint64_t x) {
        return (64 - nlz(x >> 1));
    }

    enum {
        max_alignment = alignof(max_align_t),
        max_alignment_bits = alignement_bits(max_alignment)
    };
}

template<typename T>
struct PointerPacker_x86_64 {

    enum {
        addr_bits = 48,
        lo_bits = alignment_helper::alignement_bits(alignof(T)),
        hi_mask = ~((1ull << addr_bits) - 1),
        lo_mask = ((1ull << lo_bits) - 1),
        usable_bits = addr_bits - lo_bits
    };

    static size_t ptr_pack(T const volatile* p) {
        const size_t v = reinterpret_cast<size_t>(p);
        assert((v & lo_mask) == 0);
        assert((v & hi_mask) == 0);
        return (v >> lo_bits);
    }

    static T* ptr_unpack (size_t v) {
        return reinterpret_cast<T*>((v << lo_bits) & ~hi_mask);
    }

};

template<>
struct PointerPacker_x86_64<void> {

    enum {
        addr_bits = 48,
        lo_bits = 0,
        hi_mask = ~((1ull << addr_bits) - 1),
        lo_mask = ((1ull << lo_bits) - 1),
        usable_bits = addr_bits - lo_bits
    };

    static size_t ptr_pack(void const volatile* p) {
        const size_t v = reinterpret_cast<size_t>(p);
        assert((v & lo_mask) == 0);
        assert((v & hi_mask) == 0);
        return (v >> lo_bits);
    }

    static void* ptr_unpack (size_t v) {
        return reinterpret_cast<void*>((v << lo_bits) & ~hi_mask);
    }

};

template<typename T>
struct NoopPointerPacker {

    enum {
        addr_bits = 8*sizeof(T*),
        lo_bits = 0,
        hi_mask = ~((1ull << addr_bits) - 1),
        lo_mask = ((1ull << lo_bits) - 1),
        usable_bits = addr_bits - lo_bits
    };

    static size_t ptr_pack(T const volatile* p) {
        return reinterpret_cast<size_t>(p);
    }

    static T* ptr_unpack (size_t v) {
        return reinterpret_cast<T*>(v);
    }

};

template<class T>
using PointerPacker = typename Select<sizeof(void*) == 8, PointerPacker_x86_64<T>, NoopPointerPacker<T>>::type;

class IValueHolder {
public:

    enum {
        max_sizeof = 0xffff,
        max_sizeof_bits = 16
    };

    enum class TypeCategories {
        POD = 0,
        INTEGRAL = 1,
        FLOATING = 2,
        POINTER = 3,
        STDSTRING = 4,
        NONE = 5
    };

    TypeCategories resolveCategory(bool isPod,
                               bool isIntegral,
                               bool isFloatingPoint,
                               bool isPointer,
                               bool isStdString) {

            if (isIntegral) {
                return TypeCategories::INTEGRAL;
            } else if (isFloatingPoint) {
                return TypeCategories::FLOATING;
            } else if (isPointer) {
                return TypeCategories::POINTER;
            } else if (isStdString) {
                return TypeCategories::STDSTRING;
            } else if (isPod) {
                return TypeCategories::POD;
            }
            return TypeCategories::NONE;
        }

    IValueHolder(
            const void* ptr,
            bool valInStruct,
#ifndef NO_RTTI
			const ::std::type_info& typeId,
#endif
			std::size_t sizeOf,
			std::size_t alignOf,
			bool isPod,
			bool isIntegral,
			bool isFloatingPoint,
			bool isPointer,
			bool isStdString,
			bool isConst)
        : m_offset(reinterpret_cast<ptrdiff_t>(ptr)-reinterpret_cast<ptrdiff_t>(this))
        , m_offsetToPtr(!valInStruct)
#ifndef NO_RTTI
        , m_typeId(PointerPacker<std::type_info>::ptr_pack(&typeId))
#endif
		, m_sizeOf(sizeOf)
		, m_alignOf(alignOf)
        , m_category(static_cast<unsigned int>(resolveCategory(isPod,isIntegral,isFloatingPoint,isPointer,isStdString)))
		, m_isConst(isConst)
    {
        assert(reinterpret_cast<ptrdiff_t>(ptr)-reinterpret_cast<ptrdiff_t>(this) > 0);
        assert(reinterpret_cast<ptrdiff_t>(ptr)-reinterpret_cast<ptrdiff_t>(this) < 64);
    }

	virtual IValueHolder* clone() const = 0;
	
	virtual bool equals(const IValueHolder* rhs) const = 0;

    void * ptrToValue() {
        if (m_offsetToPtr) {
            void** ptr = reinterpret_cast<void**>(reinterpret_cast<size_t>(this) + m_offset);
            return *ptr;
        } else {
            return reinterpret_cast<void*>(reinterpret_cast<size_t>(this) + m_offset);
        }
    }


    const void * ptrToValue() const {
        if (m_offsetToPtr) {
            void** ptr = reinterpret_cast<void**>(reinterpret_cast<size_t>(this) + m_offset);
            return *ptr;
        } else {
            return reinterpret_cast<void*>(reinterpret_cast<size_t>(this) + m_offset);
        }
    }
	
#ifndef NO_RTTI
    const ::std::type_info& typeId() const {
        return *PointerPacker<std::type_info>::ptr_unpack(m_typeId);
    }
#endif
	
	::std::size_t sizeOf() const { return m_sizeOf; }
	
	::std::size_t alignOf() const { return m_alignOf; }
	
    bool isPOD() const { return m_category == static_cast<unsigned int>(TypeCategories::POD) || isIntegral() || isFloatingPoint() || isPointer(); }

    bool isIntegral() const { return m_category == static_cast<unsigned int>(TypeCategories::INTEGRAL); }

    bool isFloatingPoint() const { return m_category == static_cast<unsigned int>(TypeCategories::FLOATING); }

    bool isPointer() const { return m_category == static_cast<unsigned int>(TypeCategories::POINTER); }

    bool isStdString() const { return m_category == static_cast<unsigned int>(TypeCategories::STDSTRING); }

	bool isConst() const { return m_isConst; }

	virtual void throwCast() const = 0;
	
	virtual ::std::string convertToString() const = 0;

	virtual number_return convertToNumber(NumberType t) const = 0;

	virtual ~IValueHolder() noexcept {}
	IValueHolder() = default;
	IValueHolder(const IValueHolder&) = delete;
	IValueHolder& operator=(const IValueHolder&) = delete;

private:
    const unsigned long m_offset : 6;
    const unsigned long m_offsetToPtr : 1;

#ifndef NO_RTTI
    const unsigned long m_typeId : PointerPacker<std::type_info>::usable_bits;
#endif

    const unsigned long m_sizeOf: max_sizeof_bits;

    const unsigned long m_alignOf: alignment_helper::max_alignment_bits;

    const unsigned int m_category: 3;
	const unsigned int m_isConst : 1;
};

//namespace {

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

    typedef CloneHelper<T, ::std::is_constructible<T, const T&>::value> Cloner;


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
        : IValueHolder(reinterpret_cast<const void*>(&m_value), true,
#ifndef NO_RTTI
					   typeid(ValueType),
#endif
					   sizeof(ValueType),
					   alignof(ValueType),
					   ::std::is_pod<ValueType>::value,
					   ::std::is_integral<ValueType>::value,
					   ::std::is_floating_point<ValueType>::value,
					   ::std::is_pointer<ValueType>::value,
                       ::std::is_same<std::string, ValueType>::value,
					   normalize_type<T>::is_const),
		m_value( ::std::forward<Args>(args)...)
    {
        static_assert(alignof(ValueType) < alignment_helper::max_alignment, "unsupported alignment size");
        static_assert(sizeof(ValueType) < IValueHolder::max_sizeof, "unsupported type size");
	}

	~ValueHolder() noexcept{
		//m_value.~ValueType();
	}

    ValueHolder(const ValueHolder& that)
        : IValueHolder(reinterpret_cast<const void*>(&m_value), true,
#ifndef NO_RTTI
                       typeid(ValueType),
#endif
                       sizeof(ValueType),
                       alignof(ValueType),
                       ::std::is_pod<ValueType>::value,
                       ::std::is_integral<ValueType>::value,
                       ::std::is_floating_point<ValueType>::value,
                       ::std::is_pointer<ValueType>::value,
                       ::std::is_same<std::string, ValueType>::value,
                       normalize_type<T>::is_const),
        m_value( that.m_value )
    {}

    ValueHolder(ValueHolder&& that)
        : IValueHolder(reinterpret_cast<const void*>(&m_value), true,
#ifndef NO_RTTI
                       typeid(ValueType),
#endif
                       sizeof(ValueType),
                       alignof(ValueType),
                       ::std::is_pod<ValueType>::value,
                       ::std::is_integral<ValueType>::value,
                       ::std::is_floating_point<ValueType>::value,
                       ::std::is_pointer<ValueType>::value,
                       ::std::is_same<std::string, ValueType>::value,
                       normalize_type<T>::is_const),
        m_value( that.m_value )
    {}

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
            return ::std::move(strconv::toString(m_value));
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
        : IValueHolder(reinterpret_cast<const void*>(&m_ptr), false,
#ifndef NO_RTTI
					   typeid(ValueType),
#endif
					   sizeof(ValueType),
					   alignof(ValueType),
					   ::std::is_pod<ValueType>::value,
					   ::std::is_integral<ValueType>::value,
					   ::std::is_floating_point<ValueType>::value,
					   ::std::is_pointer<ValueType>::value,
                       ::std::is_same<std::string, ValueType>::value,
                       normalize_type<T>::is_const),
          m_value(v), m_ptr(&v) {
        static_assert(alignof(ValueType) < alignment_helper::max_alignment, "unsupported alignment size");
        static_assert(sizeof(ValueType) < IValueHolder::max_sizeof, "unsupported type size");
    }

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
            return ::std::move(strconv::toString(m_value));
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
    const void* const m_ptr;
};


template <class T>
class ValueHolder<T&&>: public IValueHolder {
public:
    typedef T&& RefType;
    typedef typename normalize_type<T&&>::type ValueType;
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
        : IValueHolder(reinterpret_cast<const void*>(&m_ptr), false,
#ifndef NO_RTTI
                       typeid(ValueType),
#endif
                       sizeof(ValueType),
                       alignof(ValueType),
                       ::std::is_pod<ValueType>::value,
                       ::std::is_integral<ValueType>::value,
                       ::std::is_floating_point<ValueType>::value,
                       ::std::is_pointer<ValueType>::value,
                       ::std::is_same<std::string, ValueType>::value,
                       normalize_type<T>::is_const),
          m_value(v), m_ptr(&v) {
        static_assert(alignof(ValueType) < alignment_helper::max_alignment, "unsupported alignment size");
        static_assert(sizeof(ValueType) < IValueHolder::max_sizeof, "unsupported type size");
    }

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
            return ::std::move(strconv::toString(m_value));
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
    const void* const m_ptr;
};



//}

// A variant value contains a value type by value
class VariantValue {
public:
	//! Creates an empty variant
	explicit VariantValue();

    VariantValue(std::uint8_t t) : m_embedded(Types::UINT8) {
        new(&m_uint8) ValueHolder<std::uint8_t>(t);
    }
    VariantValue(std::int8_t t) : m_embedded(Types::INT8) {
        new(&m_int8) ValueHolder<std::int8_t>(t);
    }
    VariantValue(std::uint16_t t) : m_embedded(Types::UINT16) {
        new(&m_uint16) ValueHolder<std::uint16_t>(t);
    }
    VariantValue(std::int16_t t) : m_embedded(Types::INT16) {
        new(&m_int16) ValueHolder<std::int16_t>(t);
    }
    VariantValue(std::uint32_t t) : m_embedded(Types::UINT32) {
        new(&m_uint32) ValueHolder<std::uint32_t>(t);
    }
    VariantValue(std::int32_t t) : m_embedded(Types::INT32) {
        new(&m_int32) ValueHolder<std::int32_t>(t);
    }
    VariantValue(std::uint64_t t) : m_embedded(Types::UINT64) {
        new(&m_uint64) ValueHolder<std::uint64_t>(t);
    }
    VariantValue(std::int64_t t) : m_embedded(Types::INT64) {
        new(&m_int64) ValueHolder<std::int64_t>(t);
    }
    VariantValue(float t) : m_embedded(Types::FLOAT) {
        new(&m_float) ValueHolder<float>(t);
    }
    VariantValue(double t) : m_embedded(Types::DOUBLE) {
        new(&m_double) ValueHolder<double>(t);
    }
    VariantValue(std::string t) : m_embedded(Types::STRING) {
        new(&m_string) ValueHolder<std::string>(t);
    }

	template<class ValueType>
    VariantValue(const ValueType& t) : m_embedded(Types::DEFAULT) {
        new(&m_impl) shared_ptr<IValueHolder>(new ValueHolder<ValueType>(t));
    }

	template<class ValueType>
    VariantValue(ValueType* t) : m_embedded(Types::DEFAULT) {

        new(&m_impl) shared_ptr<IValueHolder>(new ValueHolder<ValueType*>(t));
    }


	template<class ValueType, class... Args>
	VariantValue& construct(Args&&... args) {
        destroyEmbedded();
        m_embedded = Types::DEFAULT;
        new(&m_impl) shared_ptr<IValueHolder>(new ValueHolder<ValueType>( ::std::forward<Args>(args)... ));
		return *this;
	}

    VariantValue& construct(std::uint8_t t) {
        *this = t; return *this;
    }
    VariantValue& construct(std::int8_t t) {
        *this = t; return *this;
    }
    VariantValue& construct(std::uint16_t t) {
        *this = t; return *this;
    }
    VariantValue& construct(std::int16_t t) {
        *this = t; return *this;
    }
    VariantValue& construct(std::uint32_t t) {
        *this = t; return *this;
    }
    VariantValue& construct(std::int32_t t) {
        *this = t; return *this;
    }
    VariantValue& construct(std::uint64_t t) {
        *this = t; return *this;
    }
    VariantValue& construct(std::int64_t t) {
        *this = t; return *this;
    }
    VariantValue& construct(double t) {
        *this = t; return *this;
    }
    VariantValue& construct(float t) {
        *this = t; return *this;
    }
    VariantValue& construct(std::string t) {
        *this = t; return *this;
    }


	VariantValue(const VariantValue& rhs);
	
	VariantValue(VariantValue&& rhs);
	
	VariantValue& operator=(const VariantValue& rhs);
	
	VariantValue& operator=(VariantValue&& rhs);

    ~VariantValue();
	
	template<class ValueType>
	VariantValue& operator=(ValueType value) {
        destroyEmbedded();
        m_embedded = Types::DEFAULT;
        new(&m_impl) shared_ptr<IValueHolder>(new ValueHolder<ValueType>( value ));
		return *this;
	}

    VariantValue& operator=(std::uint8_t t) {
        destroyEmbedded();
        m_embedded = Types::UINT8;
        new(&m_uint8) ValueHolder<std::uint8_t>(t);
        return *this;
    }
    VariantValue& operator=(std::int8_t t) {
        destroyEmbedded();
        m_embedded = Types::INT8;
        new(&m_int8) ValueHolder<std::int8_t>(t);
        return *this;
    }
    VariantValue& operator=(std::uint16_t t) {
        destroyEmbedded();
        m_embedded = Types::UINT16;
        new(&m_uint16) ValueHolder<std::uint16_t>(t);
        return *this;
    }
    VariantValue& operator=(std::int16_t t) {
        destroyEmbedded();
        m_embedded = Types::INT8;
        new(&m_int16) ValueHolder<std::int16_t>(t);
        return *this;
    }
    VariantValue& operator=(std::uint32_t t) {
        destroyEmbedded();
        m_embedded = Types::UINT32;
        new(&m_uint32) ValueHolder<std::uint32_t>(t);
        return *this;
    }
    VariantValue& operator=(std::int32_t t) {
        destroyEmbedded();
        m_embedded = Types::INT32;
        new(&m_int32) ValueHolder<std::int32_t>(t);
        return *this;
    }
    VariantValue& operator=(std::uint64_t t) {
        destroyEmbedded();
        m_embedded = Types::UINT64;
        new(&m_uint64) ValueHolder<std::uint64_t>(t);
        return *this;
    }
    VariantValue& operator=(std::int64_t t) {
        destroyEmbedded();
        m_embedded = Types::INT64;
        new(&m_int64) ValueHolder<std::int64_t>(t);
        return *this;
    }
    VariantValue& operator=(double t) {
        destroyEmbedded();
        m_embedded = Types::DOUBLE;
        new(&m_double) ValueHolder<double>(t);
        return *this;
    }
    VariantValue& operator=(float t) {
        destroyEmbedded();
        m_embedded = Types::FLOAT;
        new(&m_float) ValueHolder<float>(t);
        return *this;
    }
    VariantValue& operator=(std::string t) {
        destroyEmbedded();
        m_embedded = Types::STRING;
        new(&m_string) ValueHolder<std::string>(t);
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

    bool isEmbedded() const { return m_embedded != Types::DEFAULT; }
	
private:

#ifndef NO_RTTI
	template<class ValueType>
    typename normalize_type<ValueType>::ptr_type isAPriv() const {

        const std::type_info& from = impl()->typeId();
		const std::type_info& to   = typeid(ValueType);

		if (from == to) {
            if (!impl()->isConst() || (impl()->isConst() && normalize_type<ValueType>::is_const)) {
                return reinterpret_cast<typename normalize_type<ValueType>::ptr_type>(const_cast<void*>(impl()->ptrToValue()));
			}
		}

		int offset;
		bool possible;
		if (conversion_cache<ValueType>::instance().conversionKnown(from, offset, possible)) {
			if (possible) {
                const bool implConst = impl()->isConst();
				if (!implConst || (implConst && normalize_type<ValueType>::is_const)) {
                    return reinterpret_cast<typename normalize_type<ValueType>::ptr_type>(reinterpret_cast<char*>(const_cast<void*>(impl()->ptrToValue()))+offset);
				}
			} else {
				// conversion is known to fail, we won't even try
				return nullptr;
			}
		}

		try {
            impl()->throwCast();
		} catch(typename normalize_type<ValueType>::ptr_type ptr) {
            offset = reinterpret_cast<const char*>(ptr) - reinterpret_cast<const char*>(impl()->ptrToValue());
			conversion_cache<ValueType>::instance().registerConversion(from, offset, true);
			return ptr;
		} catch (...) {
			conversion_cache<ValueType>::instance().registerConversion(from, 0, false);
			return nullptr;
		}
		return nullptr;
	}
#else
	template<class ValueType>
	typename normalize_type<ValueType>::ptr_type isAPriv() const {

		try {
            impl()->throwCast();
		} catch(typename normalize_type<ValueType>::ptr_type ptr) {
			return ptr;
		} catch (...) {
			return nullptr;
		}
		return nullptr;
	}
#endif


	template<class ValueType>
    struct pointerConversion {
        typedef ValueType type;
        static type value(const IValueHolder* holder, bool * success) {

			try {
				holder->throwCast();
            } catch (type& ptr) {
				if (success != nullptr) *success = true;
				return ptr;
			} catch(...) { }
			if (success != nullptr) *success = false;
            return type();
		}
        static type value(const IValueHolder* holder) {
			try {
				holder->throwCast();
            } catch (type& ptr) {
				return ptr;
			} catch(...) { }
			throw std::runtime_error("failed to convert variant to pointer");
		}
	};

	template<class ValueType>
    struct integralConversion {
        typedef ValueType type;
        static type value(const IValueHolder* holder, bool * success) {
			if (success != nullptr) *success = true;
			number_return r = holder->convertToNumber(NumberType::INTEGER);
            return static_cast<type>(r.i);
		}
        static type value(const IValueHolder* holder) {
			return value(holder, nullptr);
		}
	};

	template<class ValueType>
    struct floatConversion {
        typedef ValueType type;
        static type value(const IValueHolder* holder, bool * success) {
			if (success != nullptr) *success = true;
			number_return r = holder->convertToNumber(NumberType::FLOATING);
            return static_cast<type>(r.f);
		}
        static type value(const IValueHolder* holder) {
			return value(holder, nullptr);
		}
	};

	template<class ValueType>
    struct stringConversion {
        typedef std::string type;
        static type value(const IValueHolder* holder, bool * success) {
			if (success != nullptr) *success = true;
			return holder->convertToString();
		}
        static type value(const IValueHolder* holder) {
			return value(holder, nullptr);
		}
	};

	template<class ValueType>
    struct impossibleConversion {
        typedef ValueType type;
        static type& value(const IValueHolder* holder, bool * success) {
			if (success != nullptr) *success = false;
            // something must be returned in order to compile, but this value should never be read
            return *reinterpret_cast<type*>(const_cast<void*>(holder->ptrToValue()));
        }
        static type value(const IValueHolder* holder) {
			throw std::runtime_error("failed conversion");
		}
	};

    template<class ValueType>
    struct impossibleConversion<ValueType&> {
        typedef ValueType& type;
        static type value(const IValueHolder* holder, bool * success) {
			if (success != nullptr) *success = false;
            // something must be returned in order to compile, but this value should never be read
            return *reinterpret_cast<ValueType*>(const_cast<void*>(holder->ptrToValue()));
		}
        static type value(const IValueHolder* holder) {
			throw std::runtime_error("failed conversion");
		}
	};

	template<class ValueType>
    struct impossibleConversion<ValueType&&> {
        typedef ValueType&& type;
        static ValueType& value(const IValueHolder* holder, bool * success) {
			if (success != nullptr) *success = false;
            // something must be returned in order to compile, but this value should never be read
            return *reinterpret_cast<ValueType*>(const_cast<void*>(holder->ptrToValue()));
		}
        static type value(const IValueHolder* holder) {
			throw std::runtime_error("failed conversion");
		}
	};


public:
	
	template<class ValueType>
	ValueType value() const {
		check_valid();
		auto ptr = isAPriv< ValueType >();

		if (ptr == nullptr) {
			throw ::std::runtime_error("variant value is not of the requested type");
		}
		return *ptr;
    }

    template<class ValueType> using converter = typename
                Select<std::is_integral<ValueType>::value || ::std::is_enum<ValueType>::value,
                    integralConversion<ValueType>,
                    typename Select<std::is_floating_point<ValueType>::value,
                        floatConversion<ValueType>,
                        typename Select<std::is_same<ValueType, ::std::string>::value,
                            stringConversion<ValueType>,
                                typename Select<std::is_same<ValueType, const ::std::string&>::value,
                                    stringConversion<ValueType>,
                                    typename Select<std::is_pointer<ValueType>::value,
                                            pointerConversion<ValueType>,
                                            impossibleConversion<ValueType>>::type>::type>::type>::type>::type;
	
    template<class ValueType>
    typename converter<ValueType>::type convertTo(bool * success = nullptr) const {
		check_valid();

        auto ptr = isAPriv<ValueType>();
        if (ptr != nullptr) {
			if (success != nullptr) * success = true;
			return *ptr;
		}

        auto ptrc = isAPriv<const ValueType>();
        if (ptrc != nullptr) {
            if (success != nullptr) * success = true;
            return *ptrc;
        }
        return converter<ValueType>::value(impl(), success);
	}

	template<class ValueType>
    typename converter<ValueType>::type convertToThrow() const {
		check_valid();

		auto ptr = isAPriv<ValueType>();

		if (ptr != nullptr) {
			return *ptr;
		}

        auto ptrc = isAPriv<const ValueType>();
        if (ptrc != nullptr) {
            return *ptrc;
        }
        return converter<ValueType>::value(impl());
	}


    template<class ValueType, class... T>
    typename converter<ValueType>::type convertToThrow(const char* fmt, const T&... t) const {
        try {
            return convertToThrow<ValueType>();
        } catch (const std::runtime_error& err) {
            if (fmt == nullptr) throw;
            throw std::runtime_error(strconv::fmt_str(fmt, t..., err.what()));
        }
    }

	template<class ValueType>
    typename converter<ValueType>::type moveValue(bool * success = nullptr) const {
		check_valid();

		auto ptr = isAPriv<ValueType>();
		if (ptr != nullptr) {
			if (success != nullptr) * success = true;
			return ::std::forward<ValueType>(*ptr);
		}

        auto ptrc = isAPriv<const ValueType>();
        if (ptrc != nullptr) {
            if (success != nullptr) * success = true;
            return ::std::forward<const ValueType>(*ptrc);
        }

        return ::std::forward<typename converter<ValueType>::type>(converter<ValueType>::value(impl(), success));
	}

    template<class ValueType>
    typename converter<ValueType>::type moveValueThrow() const {
		check_valid();

		auto ptr = isAPriv<ValueType>();
        if (ptr != nullptr) {
            return ::std::forward<ValueType>(*ptr);
        }
        auto ptrc = isAPriv<const ValueType>();
        if (ptrc != nullptr) {
            return ::std::forward<const ValueType>(*ptrc);
        }

        return ::std::forward<typename converter<ValueType>::type>(converter<ValueType>::value(impl()));
	}

    template<class ValueType, class... T>
    typename converter<ValueType>::type moveValueThrow(const char* fmt, const T&... t) const {
        try {
            return moveValueThrow<ValueType>();
        } catch (const std::runtime_error& err) {
            if (fmt == nullptr) throw;
            throw std::runtime_error(strconv::fmt_str(fmt, t..., err.what()));
        }
    }



	template<class ValueType>
    typename converter<ValueType>::type convertTo(bool * success = nullptr) const volatile {
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
	

    bool isValid() const { return impl() != nullptr; }

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

    /* These two cannot be global functions, otherwise implicit
     * type conversions and suddenly everything is comparable
     */
    bool operator==(const VariantValue& that) const;

    bool operator!=(const VariantValue& that) const;
	
private:

    enum class Types : char {
        DEFAULT,
        INT8,
        UINT8,
        INT16,
        UINT16,
        INT32,
        UINT32,
        INT64,
        UINT64,
        DOUBLE,
        FLOAT,
        STRING
    };

    Types m_embedded;

    union {

        std::shared_ptr<IValueHolder> m_impl;

        ValueHolder<std::uint8_t>   m_uint8;
        ValueHolder<std::int8_t>     m_int8;
        ValueHolder<std::uint16_t> m_uint16;
        ValueHolder<std::int16_t>   m_int16;
        ValueHolder<std::uint32_t> m_uint32;
        ValueHolder<std::int32_t>   m_int32;
        ValueHolder<std::uint64_t> m_uint64;
        ValueHolder<std::int64_t>   m_int64;
        ValueHolder<double>          m_double;
        ValueHolder<float>            m_float;
        ValueHolder<std::string>   m_string;
    };

    IValueHolder* impl() {
        switch (m_embedded) {
            case Types::DEFAULT:
                return m_impl.get();
            case Types::UINT8:
                return &m_uint8;
            case Types::INT8:
                return &m_int8;
            case Types::UINT16:
                return &m_uint16;
            case Types::INT16:
                return &m_int16;
            case Types::UINT32:
                return &m_uint32;
            case Types::INT32:
                return &m_int32;
            case Types::UINT64:
                return &m_uint64;
            case Types::INT64:
                return &m_int64;
            case Types::DOUBLE:
                return &m_double;
            case Types::FLOAT:
                return &m_double;
            case Types::STRING:
                return &m_string;
        }
        return m_impl.get();
    }
    const IValueHolder* impl() const {
        return const_cast<VariantValue*>(this)->impl();
    }

	void check_valid() const {
		if (!isValid()) {
			throw ::std::runtime_error("variant has no value");
		}
	}

    void destroyEmbedded();
};


#endif /* VARIANT_H */
