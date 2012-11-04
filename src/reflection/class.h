#ifndef CLASS_H
#define CLASS_H

#include <algorithm>
#include <string>
#include <list>
#include <memory>
#ifndef NO_RTTI
#include <typeinfo>
#endif
#include <type_traits>
#include <stdexcept>

#include "variant.h"

#include "reflection.h"

class Interface;

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

	void registerSuperClass(const char* className);

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

	std::unique_ptr<Interface> newInterface() const;

	void registerInterface(Interface* i);

private:

	void registerSuperClassInternal(Class c);

	void assert_open() const;
	::std::string m_fqn = "error, meta-class uninitialized";
	MethodList m_methods;
	ConstructorList m_constructors;
	std::list<const char*> m_unresolvedBases;
	ClassList m_superclasses;
	AttributeList m_attributes;
	bool m_open;

	Interface* m_iface;

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
