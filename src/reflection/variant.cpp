/*
** SelfPortrait API
** See Copyright Notice in reflection.h
*/
#include "variant.h"

VariantValue::VariantValue(const VariantValue& rhs)
    : m_deleter(rhs.m_deleter), m_getter(rhs.m_getter)
{
    if (rhs.m_deleter == &deleterINT32) {
        new(&m_int32) ValueHolder<std::int32_t>(rhs.m_int32);
    } else if (rhs.m_deleter == &deleterDOUBLE) {
        new(&m_double) ValueHolder<double>(rhs.m_double);
    } else if (rhs.m_deleter == &deleterDEFAULT) {
        m_deleter = deleterNULL;
        m_getter  = getterNULL;
        new(&m_impl) shared_ptr<IValueHolder>(rhs.m_impl->clone());
        if (m_impl.get() == nullptr) {
            deleterDEFAULT(this);
            throw std::runtime_error("type has no copy constructor");
        }
        m_deleter = deleterDEFAULT;
        m_getter  = getterDEFAULT;
    } else if (rhs.m_deleter == &deleterUINT8) {
        new(&m_uint8) ValueHolder<std::uint8_t>(rhs.m_uint8);
    } else if (rhs.m_deleter == &deleterINT8) {
        new(&m_int8) ValueHolder<std::int8_t>(rhs.m_int8);
    } else if (rhs.m_deleter == &deleterUINT16) {
        new(&m_uint16) ValueHolder<std::uint16_t>(rhs.m_uint16);
    } else if (rhs.m_deleter == &deleterINT16) {
        new(&m_int16) ValueHolder<std::int16_t>(rhs.m_int16);
    } else if (rhs.m_deleter == &deleterUINT32) {
        new(&m_uint32) ValueHolder<std::uint32_t>(rhs.m_uint32);
    } else if (rhs.m_deleter == &deleterUINT64) {
        new(&m_uint64) ValueHolder<std::uint64_t>(rhs.m_uint64);
    } else if (rhs.m_deleter == &deleterINT64) {
        new(&m_int64) ValueHolder<std::int64_t>(rhs.m_int64);
    } else if (rhs.m_deleter == &deleterFLOAT) {
        new(&m_float) ValueHolder<float>(rhs.m_float);
    } else if (rhs.m_deleter == &deleterSTRING) {
        new(&m_string) ValueHolder<std::string>(rhs.m_string);
    }
}

VariantValue::VariantValue(VariantValue&& rhs)
    : m_deleter(rhs.m_deleter), m_getter(rhs.m_getter)
{
    //cout << "move constructor called" << endl;
    if (rhs.m_deleter == &deleterINT32) {
        new(&m_int32) ValueHolder<std::int32_t>(std::move(rhs.m_int32));
    } else if (rhs.m_deleter == &deleterDOUBLE) {
        new(&m_double) ValueHolder<double>(std::move(rhs.m_double));
    } else if (rhs.m_deleter == &deleterDEFAULT) {
        new(&m_impl) shared_ptr<IValueHolder>(std::move(rhs.m_impl));
    } else if (rhs.m_deleter == &deleterUINT8) {
        new(&m_uint8) ValueHolder<std::uint8_t>(std::move(rhs.m_uint8));
    } else if (rhs.m_deleter == &deleterINT8) {
        new(&m_int8) ValueHolder<std::int8_t>(std::move(rhs.m_int8));
    } else if (rhs.m_deleter == &deleterUINT16) {
        new(&m_uint16) ValueHolder<std::uint16_t>(std::move(rhs.m_uint16));
    } else if (rhs.m_deleter == &deleterINT16) {
        new(&m_int16) ValueHolder<std::int16_t>(std::move(rhs.m_int16));
    } else if (rhs.m_deleter == &deleterUINT32) {
        new(&m_uint32) ValueHolder<std::uint32_t>(std::move(rhs.m_uint32));
    } else if (rhs.m_deleter == &deleterUINT64) {
        new(&m_uint64) ValueHolder<std::uint64_t>(std::move(rhs.m_uint64));
    } else if (rhs.m_deleter == &deleterINT64) {
        new(&m_int64) ValueHolder<std::int64_t>(std::move(rhs.m_int64));
    } else if (rhs.m_deleter == &deleterFLOAT) {
        new(&m_float) ValueHolder<float>(std::move(rhs.m_float));
    } else if (rhs.m_deleter == &deleterSTRING) {
        new(&m_string) ValueHolder<std::string>(std::move(rhs.m_string));
    }
}

VariantValue& VariantValue::operator=(const VariantValue& rhs)
{
    //std::cout << "copy asignement operator called" << std::endl;
    m_deleter(this);
    m_deleter = rhs.m_deleter;
    m_getter = rhs.m_getter;

    if (rhs.m_deleter == &deleterINT32) {
        new(&m_int32) ValueHolder<std::int32_t>(rhs.m_int32);
    } else if (rhs.m_deleter == &deleterDOUBLE) {
        new(&m_double) ValueHolder<double>(rhs.m_double);
    } else if (rhs.m_deleter == &deleterDEFAULT) {
        m_deleter = deleterNULL;
        m_getter  = getterNULL;
        new(&m_impl) shared_ptr<IValueHolder>(rhs.m_impl->clone());
        if (m_impl.get() == nullptr) {
            deleterDEFAULT(this);
            throw std::runtime_error("type has no copy constructor");
        }
        m_deleter = deleterDEFAULT;
        m_getter  = getterDEFAULT;
    } else if (rhs.m_deleter == &deleterUINT8) {
        new(&m_uint8) ValueHolder<std::uint8_t>(rhs.m_uint8);
    } else if (rhs.m_deleter == &deleterINT8) {
        new(&m_int8) ValueHolder<std::int8_t>(rhs.m_int8);
    } else if (rhs.m_deleter == &deleterUINT16) {
        new(&m_uint16) ValueHolder<std::uint16_t>(rhs.m_uint16);
    } else if (rhs.m_deleter == &deleterINT16) {
        new(&m_int16) ValueHolder<std::int16_t>(rhs.m_int16);
    } else if (rhs.m_deleter == &deleterUINT32) {
        new(&m_uint32) ValueHolder<std::uint32_t>(rhs.m_uint32);
    } else if (rhs.m_deleter == &deleterUINT64) {
        new(&m_uint64) ValueHolder<std::uint64_t>(rhs.m_uint64);
    } else if (rhs.m_deleter == &deleterINT64) {
        new(&m_int64) ValueHolder<std::int64_t>(rhs.m_int64);
    } else if (rhs.m_deleter == &deleterFLOAT) {
        new(&m_float) ValueHolder<float>(rhs.m_float);
    } else if (rhs.m_deleter == &deleterSTRING) {
        new(&m_string) ValueHolder<std::string>(rhs.m_string);
    }
    return *this;
}

VariantValue& VariantValue::operator=(VariantValue&& rhs)
{
    //std::cout << "move assignemnt operator called" << std::endl;
    m_deleter(this);
    m_deleter = rhs.m_deleter;
    m_getter = rhs.m_getter;

    if (rhs.m_deleter == &deleterINT32) {
        new(&m_int32) ValueHolder<std::int32_t>(std::move(rhs.m_int32));
    } else if (rhs.m_deleter == &deleterDOUBLE) {
        new(&m_double) ValueHolder<double>(std::move(rhs.m_double));
    } else if (rhs.m_deleter == &deleterDEFAULT) {
        new(&m_impl) shared_ptr<IValueHolder>(std::move(rhs.m_impl));
    } else if (rhs.m_deleter == &deleterUINT8) {
        new(&m_uint8) ValueHolder<std::uint8_t>(std::move(rhs.m_uint8));
    } else if (rhs.m_deleter == &deleterINT8) {
        new(&m_int8) ValueHolder<std::int8_t>(std::move(rhs.m_int8));
    } else if (rhs.m_deleter == &deleterUINT16) {
        new(&m_uint16) ValueHolder<std::uint16_t>(std::move(rhs.m_uint16));
    } else if (rhs.m_deleter == &deleterINT16) {
        new(&m_int16) ValueHolder<std::int16_t>(std::move(rhs.m_int16));
    } else if (rhs.m_deleter == &deleterUINT32) {
        new(&m_uint32) ValueHolder<std::uint32_t>(std::move(rhs.m_uint32));
    } else if (rhs.m_deleter == &deleterUINT64) {
        new(&m_uint64) ValueHolder<std::uint64_t>(std::move(rhs.m_uint64));
    } else if (rhs.m_deleter == &deleterINT64) {
        new(&m_int64) ValueHolder<std::int64_t>(std::move(rhs.m_int64));
    } else if (rhs.m_deleter == &deleterFLOAT) {
        new(&m_float) ValueHolder<float>(std::move(rhs.m_float));
    } else if (rhs.m_deleter == &deleterSTRING) {
        new(&m_string) ValueHolder<std::string>(std::move(rhs.m_string));
    }
    return *this;
}

bool VariantValue::assign(const VariantValue& v) noexcept
{
    return impl()->assign(v);
}

VariantValue VariantValue::createReference() const {
	VariantValue ret;

    check_valid();
    if (!isEmbedded()) {
        ret.m_deleter = m_deleter;
        ret.m_getter = m_getter;
        new(&ret.m_impl) shared_ptr<IValueHolder>(m_impl);
    } else {
        ret = *this;
    }
	return ::std::move(ret);
}


#ifndef NO_RTTI
const ::std::type_info& VariantValue::typeId() const
{
	check_valid();
    return impl()->typeId();
}
#endif

// Is a Plain-Old-Datatype (can be memcpy'd)
bool VariantValue::isStdString() const
{
	check_valid();
    return impl()->isStdString();
}

// Is a Plain-Old-Datatype (can be memcpy'd)
bool VariantValue::isIntegral() const
{
	check_valid();
    return impl()->isIntegral();
}

bool VariantValue::isFloatingPoint() const
{
	check_valid();
    return impl()->isFloatingPoint();
}

bool VariantValue::isArithmetical() const
{
	return isIntegral() || isFloatingPoint();
}

bool VariantValue::isConst() const
{
    check_valid();
    return impl()->isConst();
}

// Is a Plain-Old-Datatype (can be memcpy'd)
bool VariantValue::isPOD() const
{
	check_valid();
    return impl()->isPOD();
}

::std::size_t VariantValue::sizeOf() const
{
	check_valid();
    return impl()->sizeOf();
}

::std::size_t VariantValue::alignOf() const
{
	check_valid();
    return impl()->alignOf();
}

// If the type is a POD and you know what you're doing, you can memcpy it, but beware of the alignment
const void * VariantValue::ptrToValue() const
{
	check_valid();
    return impl()->ptrToValue();
}

bool VariantValue::operator==(const VariantValue& that) const
{

    if ((this->isValid() == false) && (that.isValid() == false)) {
		return true;
	}
    if ((this->isValid() == false) || (that.isValid() == false)) {
		return false;
	}

    if (this->isArithmetical() && that.isArithmetical()) {
        if (this->isIntegral() && that.isIntegral()) {
            number_return r1 = this->impl()->convertToNumber(NumberType::INTEGER);
            number_return r2 = that.impl()->convertToNumber(NumberType::INTEGER);
			return r1.i == r2.i;
		} else {
            number_return r1 = this->impl()->convertToNumber(NumberType::FLOATING);
            number_return r2 = that.impl()->convertToNumber(NumberType::FLOATING);
			return r1.f == r2.f;
		}
	}

    return this->impl()->equals(that.impl());
}

bool VariantValue::operator!=(const VariantValue& that) const
{
    return !(*this == that);
}


void VariantValue::deleterNULL(VariantValue*) noexcept {}
void VariantValue::deleterDEFAULT(VariantValue* self) noexcept { self->m_impl.~shared_ptr<IValueHolder>(); }
void VariantValue::deleterUINT8(VariantValue* self) noexcept { self->m_uint8.~ValueHolder<std::uint8_t>();}
void VariantValue::deleterINT8(VariantValue* self) noexcept { self->m_int8.~ValueHolder<std::int8_t>();}
void VariantValue::deleterUINT16(VariantValue* self) noexcept { self->m_uint16.~ValueHolder<std::uint16_t>();}
void VariantValue::deleterINT16(VariantValue* self) noexcept { self->m_int16.~ValueHolder<std::int16_t>();}
void VariantValue::deleterUINT32(VariantValue* self) noexcept { self->m_uint32.~ValueHolder<std::uint32_t>();}
void VariantValue::deleterINT32(VariantValue* self) noexcept { self->m_int32.~ValueHolder<std::int32_t>();}
void VariantValue::deleterUINT64(VariantValue* self) noexcept { self->m_uint64.~ValueHolder<std::uint64_t>();}
void VariantValue::deleterINT64(VariantValue* self) noexcept { self->m_int64.~ValueHolder<std::int64_t>();}
void VariantValue::deleterDOUBLE(VariantValue* self) noexcept { self->m_double.~ValueHolder<double>();}
void VariantValue::deleterFLOAT(VariantValue* self) noexcept { self->m_float.~ValueHolder<float>();}
void VariantValue::deleterSTRING(VariantValue* self) noexcept { self->m_string.~ValueHolder<std::string>(); }
