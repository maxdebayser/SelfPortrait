/*
** SelfPortrait API
** See Copyright Notice in reflection.h
*/
#include "method.h"

MethodImpl::MethodImpl(
		boundmethod m,
		const char* name,
		const char* returnSpelling,
		int numArguments,
		const char* argSpellings,
		bool isConst,
		bool isVolatile,
		bool isStatic
#ifndef NO_RTTI
		, const ::std::type_info& returnType
		, ::std::vector<const ::std::type_info*> argumentTypes
#endif
		)
	: m_method(m)
	, m_name(name)
	, m_returnSpelling(returnSpelling)
	, m_argSpellings(argSpellings)
	, m_numArgs(numArguments)
	, m_isConst(isConst)
	, m_isVolatile(isVolatile)
	, m_isStatic(isStatic)
#ifndef NO_RTTI
	, m_returnType(returnType)
	, m_argumentTypes(argumentTypes)
#endif
{}


const char* MethodImpl::name() const
{
	return m_name;
}

::std::size_t MethodImpl::numberOfArguments() const
{
	return m_numArgs;
}

::std::vector< ::std::string> MethodImpl::argumentSpellings() const
{
	return splitArgs(m_argSpellings);
}

::std::string MethodImpl::returnTypeSpelling() const
{
	return normalizedTypeName(m_returnSpelling);
}

bool MethodImpl::isConst() const
{
	return m_isConst;
}

bool MethodImpl::isVolatile() const
{
	return m_isVolatile;
}

bool MethodImpl::isStatic() const
{
	return m_isStatic;
}

#ifndef NO_RTTI
const ::std::type_info& MethodImpl::returnType() const
{
	return m_returnType;
}

::std::vector<const ::std::type_info*> MethodImpl::argumentTypes() const
{
	return m_argumentTypes;
}
#endif


VariantValue MethodImpl::call(const ::std::vector<VariantValue>& args) const
{
	if (args.size() < m_numArgs) {
		throw ::std::runtime_error("function or constructor called with insufficient number of arguments");
	}
	if (!m_isStatic) {
		throw ::std::runtime_error("cannnot call non-static method withtout object");
	}
	VariantValue v;
	return m_method(v, args);
}

VariantValue MethodImpl::call(VariantValue& object, const ::std::vector<VariantValue>& args) const
{
	if (args.size() < m_numArgs) {
		throw ::std::runtime_error("function or constructor called with insufficient number of arguments");
	}
	return m_method(object, args);
}

VariantValue MethodImpl::call(const VariantValue& object, const ::std::vector<VariantValue>& args) const
{
	if (args.size() < m_numArgs) {
		throw ::std::runtime_error("function or constructor called with insufficient number of arguments");
	}
	if (!m_isConst) {
		throw ::std::runtime_error("Called non-const method of const object");
	}
	return m_method(object, args);
}

VariantValue MethodImpl::call(volatile VariantValue& object, const ::std::vector<VariantValue>& args) const
{
	if (args.size() < m_numArgs) {
		throw ::std::runtime_error("function or constructor called with insufficient number of arguments");
	}
	if (!m_isVolatile) {
		throw ::std::runtime_error("Called non-volatile method of volatile object");
	}
	return m_method(object, args);
}

VariantValue MethodImpl::call(const volatile VariantValue& object, const ::std::vector<VariantValue>& args) const
{
	if (args.size() < m_numArgs) {
		throw ::std::runtime_error("function or constructor called with insufficient number of arguments");
	}
	if (!m_isConst) {
		throw ::std::runtime_error("Called non-const method of const object");
	}
	if (!m_isVolatile) {
		throw ::std::runtime_error("Called non-volatile method of volatile object");
	}
	return m_method(object, args);
}
