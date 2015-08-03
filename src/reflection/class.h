/*
** SelfPortrait API
** See Copyright Notice in reflection.h
*/
#ifndef CLASS_H
#define CLASS_H

#include <algorithm>
#include <string>
#include <list>
#include <memory>
#include <unordered_map>
#ifndef NO_RTTI
#include <typeinfo>
#endif
#include <type_traits>
#include <stdexcept>

#include "variant.h"

#include "reflection.h"

class ProxyImpl;

typedef VariantValue (*StubCreator)(std::shared_ptr<ProxyImpl>&);

class ClassImpl: public Annotated {
public:
	typedef Class::MethodList MethodList;
	typedef Class::ConstructorList ConstructorList;
	typedef Class::ClassList ClassList;
	typedef Class::AttributeList AttributeList;
		
	const std::string& fullyQualifiedName() const;
	
	const MethodList& methods() const;
	
	const ConstructorList& constructors() const;
	
	const ClassList& superclasses() const;
	
	const AttributeList& attributes() const;

    void registerSuperClass(const char* className, std::function<VariantValue(const VariantValue&)> castFunction);

	void resolveBases();

	bool hasUnresolvedBases() const;

	bool open() const;

	void close();

	ClassImpl();

	ClassImpl(const ClassImpl&) = delete;
	ClassImpl& operator=(const ClassImpl&) = delete;
	ClassImpl(ClassImpl&&) = delete;
	ClassImpl& operator=(ClassImpl&&) = delete;
	
	void setFullyQualifiedName(const ::std::string& fqn);
	
	void registerMethod(Method m);
	
	void registerConstructor(Constructor c);
	
	void registerAttribute(Attribute attr);
	
#ifndef NO_RTTI
	const std::type_info& typeId() const;
	void setTypeInfo(const std::type_info& info);
#endif
	

	template<class T>
	static ClassImpl* inst();

	bool isInterface() const;

	VariantValue newInterface(std::shared_ptr<ProxyImpl>& proxyImpl) const;

	void registerInterface(StubCreator c);

    VariantValue castUp(const Class& base, const VariantValue& baseRef) const;

private:

	void registerSuperClassInternal(Class c);

	void assert_open() const;
	::std::string m_fqn = "error, meta-class uninitialized";
	MethodList m_methods;
	ConstructorList m_constructors;
    std::list<std::pair<const char*, std::function<VariantValue(const VariantValue&)>>> m_unresolvedBases;
	ClassList m_superclasses;
    std::unordered_map<Class, std::function<VariantValue(const VariantValue&)>> m_castFunctions;
	AttributeList m_attributes;
	bool m_open;

	StubCreator m_stubCreator;

#ifndef NO_RTTI
	const std::type_info* m_typeInfo;
#endif

};



template<class T>
Class ClassOf()
{
	return Class(ClassImpl::inst<T>());
}

#endif /* CLASS_H */
