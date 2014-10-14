/*
** SelfPortrait API
** See Copyright Notice in reflection.h
*/
#include "variant.h"

VariantValue::VariantValue() : m_impl() {}


VariantValue::VariantValue(const VariantValue& rhs) : m_impl(rhs.m_impl->clone()) {}

VariantValue::VariantValue(VariantValue&& rhs) : m_impl(::std::move(rhs.m_impl)) {}

VariantValue& VariantValue::operator=(const VariantValue& rhs)
{
	m_impl.reset(rhs.m_impl->clone());
	return *this;
}

VariantValue& VariantValue::operator=(VariantValue&& rhs)
{
	m_impl = ::std::move(rhs.m_impl);
	return *this;
}


VariantValue VariantValue::createReference() const {
	VariantValue ret;
	ret.m_impl = m_impl;
	return ::std::move(ret);
}


#ifndef NO_RTTI
const ::std::type_info& VariantValue::typeId() const
{
	check_valid();
	return m_impl->typeId();
}
#endif

// Is a Plain-Old-Datatype (can be memcpy'd)
bool VariantValue::isStdString() const
{
	check_valid();
	return m_impl->isStdString();
}

// Is a Plain-Old-Datatype (can be memcpy'd)
bool VariantValue::isIntegral() const
{
	check_valid();
	return m_impl->isIntegral();
}

bool VariantValue::isFloatingPoint() const
{
	check_valid();
	return m_impl->isFloatingPoint();
}

bool VariantValue::isArithmetical() const
{
	return isIntegral() || isFloatingPoint();
}

// Is a Plain-Old-Datatype (can be memcpy'd)
bool VariantValue::isPOD() const
{
	check_valid();
	return m_impl->isPOD();
}

::std::size_t VariantValue::sizeOf() const
{
	check_valid();
	return m_impl->sizeOf();
}

::std::size_t VariantValue::alignOf() const
{
	check_valid();
	return m_impl->alignOf();
}

// If the type is a POD and you know what you're doing, you can memcpy it, but beware of the alignment
const void * VariantValue::ptrToValue() const
{
	check_valid();
	return m_impl->ptrToValue();
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
            number_return r1 = this->m_impl->convertToNumber(NumberType::INTEGER);
            number_return r2 = that.m_impl->convertToNumber(NumberType::INTEGER);
			return r1.i == r2.i;
		} else {
            number_return r1 = this->m_impl->convertToNumber(NumberType::FLOATING);
            number_return r2 = that.m_impl->convertToNumber(NumberType::FLOATING);
			return r1.f == r2.f;
		}
	}

    return this->m_impl->equals(that.m_impl.get());
}

bool VariantValue::operator!=(const VariantValue& that) const
{
    return !(*this == that);
}

