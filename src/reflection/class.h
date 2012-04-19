#ifndef CLASS_H
#define CLASS_H

#include <algorithm>
#include <string>
#include <list>
#ifndef NO_RTTI
#include <typeinfo>
#endif
#include <type_traits>
#include <stdexcept>

#include "variant.h"

#include "reflection.h"


// Keep as much type-independent information in the base class
class ClassImpl: public Annotated {
public:
	typedef Class::MethodList MethodList;
	typedef Class::ConstructorList ConstructorList;
	typedef Class::ClassList ClassList;
	typedef Class::AttributeList AttributeList;
		
	const std::string& fullyQualifiedName() const { return m_fqn; }
	
	const MethodList& methods() const { return m_methods; }
	
	const ConstructorList& constructors() const { return m_constructors; }
	
	const ClassList& superclasses() const { return m_superclasses; }
	
	const AttributeList& attributes() const { return m_attributes; }

	bool open() const {
		return m_open;
	}

	void close() {
		m_open = false;
	}

	void assert_open() const {
		if (!m_open) throw ::std::logic_error("meta-class is already closed for registration");
	}
	~ClassImpl() {}

	ClassImpl() : m_open(true) {}

	ClassImpl(const ClassImpl&) = delete;
	ClassImpl& operator=(const ClassImpl&) = delete;
	ClassImpl(ClassImpl&&) = delete;
	ClassImpl& operator=(ClassImpl&&) = delete;
	
	void setFullyQualifiedName(const ::std::string& fqn){
		assert_open();
		m_fqn = fqn;
	}
	
	void registerMethod(Method m){ // Method is just a lightweigth handle
		assert_open();
		m_methods.push_back(m);
	}
	
	void registerConstructor(Constructor c){ // Constructor is just a lightweigth handle
		assert_open();
		m_constructors.push_back(c);
	}
	
	void registerAttribute(Attribute attr){

		assert_open();
		m_attributes.push_back(attr);
	}
	
	void registerSuperClass(Class c) {
		assert_open();
		m_superclasses.push_back(c);
		for (const Method& m : c.methods()) {
			registerMethod(m);
		}
		for (const Attribute& a: c.attributes()) {
			registerAttribute(a);
		}
		for(Class s: c.superclasses()) {
			registerSuperClass(s);
		}
	}

#ifndef NO_RTTI
	const std::type_info& typeId() const { return *m_typeInfo; }
	void setTypeInfo(const std::type_info& info)  { m_typeInfo = &info; }
#endif
	

	template<class T>
	static ClassImpl* inst();

private:
	::std::string m_fqn = "error, meta-class uninitialized";
	MethodList m_methods;
	ConstructorList m_constructors;
	ClassList m_superclasses;
	AttributeList m_attributes;
	bool m_open;

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
