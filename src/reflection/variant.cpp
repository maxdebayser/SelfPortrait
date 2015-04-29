/*
** SelfPortrait API
** See Copyright Notice in reflection.h
*/
#include "variant.h"

VariantValue::VariantValue()
    : m_embedded(Types::DEFAULT)
{
    new(&m_impl) shared_ptr<IValueHolder>();
}

void VariantValue::destroyEmbedded()
{
    switch (m_embedded) {
        case Types::DEFAULT:
            m_impl.~shared_ptr<IValueHolder>();
            break;
        case Types::UINT8:
            m_uint8.~ValueHolder<std::uint8_t>();
            break;
        case Types::INT8:
            m_int8.~ValueHolder<std::int8_t>();
            break;
        case Types::UINT16:
            m_uint16.~ValueHolder<std::uint16_t>();
            break;
        case Types::INT16:
            m_int16.~ValueHolder<std::int16_t>();
            break;
        case Types::UINT32:
            m_uint32.~ValueHolder<std::uint32_t>();
            break;
        case Types::INT32:
            m_int32.~ValueHolder<std::int32_t>();
            break;
        case Types::UINT64:
            m_uint64.~ValueHolder<std::uint64_t>();
            break;
        case Types::INT64:
            m_int64.~ValueHolder<std::int64_t>();
            break;
        case Types::DOUBLE:
            m_double.~ValueHolder<double>();
            break;
        case Types::FLOAT:
            m_float.~ValueHolder<float>();
            break;
        case Types::STRING:
            m_string.~ValueHolder<std::string>();
            break;
    }
}

VariantValue::~VariantValue()
{
    destroyEmbedded();
}

VariantValue::VariantValue(const VariantValue& rhs)
    : m_embedded(rhs.m_embedded)
{
    switch (m_embedded) {
        case Types::DEFAULT:
            new(&m_impl) shared_ptr<IValueHolder>(rhs.m_impl->clone());
            break;
        case Types::UINT8:
            new(&m_uint8) ValueHolder<std::uint8_t>(rhs.m_uint8);
            break;
        case Types::INT8:
            new(&m_int8) ValueHolder<std::int8_t>(rhs.m_int8);
            break;
        case Types::UINT16:
            new(&m_uint16) ValueHolder<std::uint16_t>(rhs.m_uint16);
            break;
        case Types::INT16:
            new(&m_int16) ValueHolder<std::int16_t>(rhs.m_int16);
            break;
        case Types::UINT32:
            new(&m_uint32) ValueHolder<std::uint32_t>(rhs.m_uint32);
            break;
        case Types::INT32:
            new(&m_int32) ValueHolder<std::int32_t>(rhs.m_int32);
            break;
        case Types::UINT64:
            new(&m_uint64) ValueHolder<std::uint64_t>(rhs.m_uint64);
            break;
        case Types::INT64:
            new(&m_int64) ValueHolder<std::int64_t>(rhs.m_int64);
            break;
        case Types::DOUBLE:
            new(&m_double) ValueHolder<double>(rhs.m_double);
            break;
        case Types::FLOAT:
            new(&m_float) ValueHolder<float>(rhs.m_float);
            break;
        case Types::STRING:
            new(&m_string) ValueHolder<std::string>(rhs.m_string);
            break;
    }
}

VariantValue::VariantValue(VariantValue&& rhs)
    : m_embedded(rhs.m_embedded)
{
    switch (m_embedded) {
        case Types::DEFAULT:
            new(&m_impl) shared_ptr<IValueHolder>(std::move(rhs.m_impl));
            break;
        case Types::UINT8:
            new(&m_uint8) ValueHolder<std::uint8_t>(std::move(rhs.m_uint8));
            break;
        case Types::INT8:
            new(&m_int8) ValueHolder<std::int8_t>(std::move(rhs.m_int8));
            break;
        case Types::UINT16:
            new(&m_uint16) ValueHolder<std::uint16_t>(std::move(rhs.m_uint16));
            break;
        case Types::INT16:
            new(&m_int16) ValueHolder<std::int16_t>(std::move(rhs.m_int16));
            break;
        case Types::UINT32:
            new(&m_uint32) ValueHolder<std::uint32_t>(std::move(rhs.m_uint32));
            break;
        case Types::INT32:
            new(&m_int32) ValueHolder<std::int32_t>(std::move(rhs.m_int32));
            break;
        case Types::UINT64:
            new(&m_uint64) ValueHolder<std::uint64_t>(std::move(rhs.m_uint64));
            break;
        case Types::INT64:
            new(&m_int64) ValueHolder<std::int64_t>(std::move(rhs.m_int64));
            break;
        case Types::DOUBLE:
            new(&m_double) ValueHolder<double>(std::move(rhs.m_double));
            break;
        case Types::FLOAT:
            new(&m_float) ValueHolder<float>(std::move(rhs.m_float));
            break;
        case Types::STRING:
            new(&m_string) ValueHolder<std::string>(std::move(rhs.m_string));
            break;
    }
}

VariantValue& VariantValue::operator=(const VariantValue& rhs)
{
    destroyEmbedded();
    m_embedded = rhs.m_embedded;

    switch (m_embedded) {
        case Types::DEFAULT:
            new(&m_impl) shared_ptr<IValueHolder>(rhs.m_impl->clone());
            break;
        case Types::UINT8:
            new(&m_uint8) ValueHolder<std::uint8_t>(rhs.m_uint8);
            break;
        case Types::INT8:
            new(&m_int8) ValueHolder<std::int8_t>(rhs.m_int8);
            break;
        case Types::UINT16:
            new(&m_uint16) ValueHolder<std::uint16_t>(rhs.m_uint16);
            break;
        case Types::INT16:
            new(&m_int16) ValueHolder<std::int16_t>(rhs.m_int16);
            break;
        case Types::UINT32:
            new(&m_uint32) ValueHolder<std::uint32_t>(rhs.m_uint32);
            break;
        case Types::INT32:
            new(&m_int32) ValueHolder<std::int32_t>(rhs.m_int32);
            break;
        case Types::UINT64:
            new(&m_uint64) ValueHolder<std::uint64_t>(rhs.m_uint64);
            break;
        case Types::INT64:
            new(&m_int64) ValueHolder<std::int64_t>(rhs.m_int64);
            break;
        case Types::DOUBLE:
            new(&m_double) ValueHolder<double>(rhs.m_double);
            break;
        case Types::FLOAT:
            new(&m_float) ValueHolder<float>(rhs.m_float);
            break;
        case Types::STRING:
            new(&m_string) ValueHolder<std::string>(rhs.m_string);
            break;
    }

	return *this;
}

VariantValue& VariantValue::operator=(VariantValue&& rhs)
{
    destroyEmbedded();
    m_embedded = rhs.m_embedded;

    switch (m_embedded) {
        case Types::DEFAULT:
            new(&m_impl) shared_ptr<IValueHolder>(std::move(rhs.m_impl));
            break;
        case Types::UINT8:
            new(&m_uint8) ValueHolder<std::uint8_t>(std::move(rhs.m_uint8));
            break;
        case Types::INT8:
            new(&m_int8) ValueHolder<std::int8_t>(std::move(rhs.m_int8));
            break;
        case Types::UINT16:
            new(&m_uint16) ValueHolder<std::uint16_t>(std::move(rhs.m_uint16));
            break;
        case Types::INT16:
            new(&m_int16) ValueHolder<std::int16_t>(std::move(rhs.m_int16));
            break;
        case Types::UINT32:
            new(&m_uint32) ValueHolder<std::uint32_t>(std::move(rhs.m_uint32));
            break;
        case Types::INT32:
            new(&m_int32) ValueHolder<std::int32_t>(std::move(rhs.m_int32));
            break;
        case Types::UINT64:
            new(&m_uint64) ValueHolder<std::uint64_t>(std::move(rhs.m_uint64));
            break;
        case Types::INT64:
            new(&m_int64) ValueHolder<std::int64_t>(std::move(rhs.m_int64));
            break;
        case Types::DOUBLE:
            new(&m_double) ValueHolder<double>(std::move(rhs.m_double));
            break;
        case Types::FLOAT:
            new(&m_float) ValueHolder<float>(std::move(rhs.m_float));
            break;
        case Types::STRING:
            new(&m_string) ValueHolder<std::string>(std::move(rhs.m_string));
            break;
    }
	return *this;
}


VariantValue VariantValue::createReference() const {
	VariantValue ret;

    if (m_embedded == Types::DEFAULT) {
        ret.m_impl = m_impl;
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

