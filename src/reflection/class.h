#ifndef CLASS_H
#define CLASS_H

#include <algorithm>
#include <string>
#include <list>
#include <typeinfo>
#include <type_traits>
#include <stdexcept>

#include "variant.h"

#include "reflection.h"


// Keep as much type-independent information in the base class
class AbstractClassImpl {
public:
	typedef Class::MethodList MethodList;
	typedef Class::ConstructorList ConstructorList;
	typedef Class::ClassList ClassList;
	typedef Class::AttributeList AttributeList;
		
	virtual const std::string& fullyQualifiedName() const { return m_fqn; }
	
	virtual const MethodList& methods() const { return m_methods; }
	
	virtual const ConstructorList& constructors() const { return m_constructors; }
	
	virtual const ClassList& superclasses() const { return m_superclasses; }
	
	virtual const AttributeList& attributes() const { return m_attributes; }
	
	virtual const ::std::type_info& typeId() const = 0;
	
protected:
	
	virtual ~AbstractClassImpl() {}
	
	AbstractClassImpl() {}
	
	void setFqn(const ::std::string& fqn){
		m_fqn = fqn;
	}
	
	void registerMethodPriv(Method m){ // Method is just a lightweigth handle
		m_methods.push_back(m);
	}
	
	void registerConstructorPriv(Constructor c){ // Constructor is just a lightweigth handle
		m_constructors.push_back(c);
	}
	
	void registerAttributePriv(Attribute attr){
		m_attributes.push_back(attr);
	}
	
	void registerSuperClassPriv(Class c){
		m_superclasses.push_back(c);
		for (const Method& m : c.methods()) {
			registerMethodPriv(m);
		}
		for (const Attribute& a: c.attributes()) {
			registerAttributePriv(a);
		}
		
		for(Class s: c.superclasses()) {
			registerSuperClassPriv(s);
		}
	}
	
	::std::string m_fqn = "error, meta-class uninitialized";
	MethodList m_methods;
	ConstructorList m_constructors;
	ClassList m_superclasses;
	AttributeList m_attributes;
};

template<class T>
class ClassImpl: public AbstractClassImpl {
public:
	static_assert(::std::is_class<T>::value, "Meta-classes can only be used to describe classes");
	
	typedef T DescribedClass;
		
	static ClassImpl* instance();
	
	virtual const ::std::type_info& typeId() const override {
		return typeid(DescribedClass);
	}
	
	bool open() const {
		return m_open;
	}

	void close() {
		m_open = false;

		// clean duplicated method declarations


	}
	
	void registerMethod(Method method) {
		assert_open();
		registerMethodPriv(method);
	}
	
	void registerConstructor(Constructor constructor) {
		assert_open();
		registerConstructorPriv(constructor);
	}
	
	void setFullyQualifiedName(const ::std::string& name) {
		assert_open();
		setFqn(name);
	}
	
	ClassImpl(const ClassImpl&) = delete;
	ClassImpl(ClassImpl&&) = delete;
	ClassImpl& operator=(const ClassImpl&) = delete;
	ClassImpl& operator=(ClassImpl&&) = delete;
	
private:
	constexpr ClassImpl() : AbstractClassImpl(), m_open(true) {}
	bool m_open;
	
	void assert_open() {
			if (!m_open) throw ::std::logic_error("meta-class is already closed for registration");
	}
};

template<class T>
Class ClassOf()
{
	return Class(ClassImpl<T>::instance());
}

#endif /* CLASS_H */
