/*
** SelfPortrait API
** See Copyright Notice in reflection.h
*/
#include "constructor.h"

ConstructorImpl::ConstructorImpl(
		boundcons c
		, int numArgs
		, const char* argSpellings
#ifndef NO_RTTI
		, ::std::vector<const ::std::type_info*> argumentTypes
#endif
		)
	: m_numArgs(numArgs)
	, m_argSpellings(argSpellings)
#ifndef NO_RTTI
	, m_argumentTypes(argumentTypes)
#endif
	, m_c(c)
{}

::std::size_t ConstructorImpl::numberOfArguments() const
{
	return m_numArgs;
}

::std::vector< ::std::string> ConstructorImpl::argumentSpellings() const
{
	return splitArgs(m_argSpellings);
}

VariantValue ConstructorImpl::call(const ArgArray& args) const
{
    if (args.size() < m_numArgs) {
        throw ::std::runtime_error("function or constructor called with insufficient number of arguments");
    }
	return m_c(args);
}


#ifndef NO_RTTI
::std::vector<const ::std::type_info*> ConstructorImpl::argumentTypes() const
{
	return m_argumentTypes;
}
#endif
