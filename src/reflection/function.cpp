#include "function.h"


FunctionImpl::FunctionImpl(
		boundfunction f
		, const char* name
		, const char* returnSpelling
		, int numArgs
		, const char* argSpellings
#ifndef NO_RTTI
		, const ::std::type_info& returnType
		, ::std::vector<const ::std::type_info*> argumentTypes
#endif
		)
	: m_name(name)
	, m_returnSpelling(returnSpelling)
	, m_numArgs(numArgs)
	, m_argSpellings(argSpellings)
#ifndef NO_RTTI
	, m_returnType(returnType)
	, m_argumentTypes(argumentTypes)
#endif
	, m_f(f)
{}

FunctionImpl::~FunctionImpl()
{}

::std::string FunctionImpl::name() const
{
	return m_name;
}

::std::size_t FunctionImpl::numberOfArguments() const
{
	return m_numArgs;
}

::std::string FunctionImpl::returnTypeSpelling() const
{
	return normalizedTypeName(m_returnSpelling);
}

::std::vector< ::std::string> FunctionImpl::argumentSpellings() const
{
	return splitArgs(m_argSpellings);
}

#ifndef NO_RTTI
const ::std::type_info& FunctionImpl::returnType() const
{
	return m_returnType;
}

::std::vector<const ::std::type_info*> FunctionImpl::argumentTypes() const
{
	return m_argumentTypes;
}
#endif

VariantValue FunctionImpl::call(const ::std::vector<VariantValue>& args) const
{
	return m_f(args);
}
