#ifndef REFLECTION_H
#define REFLECTION_H

#include "variant.h"
#include "object.h"

#include <list>
#include <typeinfo>

// All user front-end classes

class AbstractAttributeImpl;

class Attribute {
public:
	
	Attribute(const Attribute& rhs);
	Attribute(Attribute&& rhs);
	Attribute& operator=(const Attribute& rhs);
	Attribute& operator=(Attribute&& rhs);
	
	const ::std::string& name() const;
	
	const ::std::type_info& type() const;
	bool isConst() const;
	bool isStatic() const;
	
	VariantValue get() const;
	
	void set(const VariantValue& value) const;
	
	VariantValue get(const VariantValue& object) const;
	void set(VariantValue& object, const VariantValue& value) const;
	void set(const VariantValue& object, const VariantValue& value) const;
	
	
private:
	Attribute(AbstractAttributeImpl* impl);
	
	AbstractAttributeImpl* m_impl;
	
	template<class A>
	friend Attribute make_attribute(::std::string name, A ptr);
	template<class C, class A>
	friend Attribute make_static_attribute(::std::string name, A ptr);
};

class AbstractConstructorImpl;

class Constructor {
public:

	::std::size_t numberOfArguments() const;
	
	bool isDefaultConstructor() const;
	
	::std::vector<const ::std::type_info*> argumentTypes() const;
	
	template<class... Args>
	VariantValue call(const Args&... args) const {
		::std::vector<VariantValue> vargs{ VariantValue(args)... };
		return call_helper(vargs);
	}	
	
private:
	Constructor(AbstractConstructorImpl* impl);
	
	VariantValue call_helper(const ::std::vector<VariantValue>& vargs) const ;
	
	AbstractConstructorImpl* m_impl;
	
	friend class Class;
	
	template<class C, class... A>
	friend Constructor make_constructor();
};

class AbstractMethodImpl;

class Method {
public:
	
	Method(const Method& rhs);
	Method& operator=(const Method& rhs);
	Method(Method&& rhs);
	Method& operator=(Method&& rhs);
	
	const ::std::string& name() const;
	::std::size_t numberOfArguments() const;
	::std::vector<const ::std::type_info*> argumentTypes() const;
	bool isConst() const;
	bool isVolatile() const;
	bool isStatic() const;

	template<class... Args>
	VariantValue call(const Args&... args) const {
		::std::vector<VariantValue> vargs{ VariantValue(args)... };
		return call_helper(vargs );
	}
	template<class... Args>
	VariantValue call(VariantValue& object, const Args&... args) const {
		::std::vector<VariantValue> vargs{ VariantValue(args)... };
		return call_helper(object, vargs );
	}
	template<class... Args>
	VariantValue call(const VariantValue& object, const Args&... args) const {
		::std::vector<VariantValue> vargs{ VariantValue(args)... };
		return call_helper(object, vargs );
	}
	template<class... Args>
	VariantValue call(volatile VariantValue& object, const Args&... args) const {
		::std::vector<VariantValue> vargs{ VariantValue(args)... };
		return call_helper(object, vargs );
	}
	template<class... Args>
	VariantValue call(const volatile VariantValue& object, const Args&... args) const {
		::std::vector<VariantValue> vargs{ VariantValue(args)... };
		return call_helper(object, vargs );
	}
		
private:
	
	Method(AbstractMethodImpl* impl);
	
	VariantValue call_helper(const ::std::vector<VariantValue>& vargs) const;
	VariantValue call_helper(VariantValue& object, const ::std::vector<VariantValue>& vargs) const;
	VariantValue call_helper(const VariantValue& object, const ::std::vector<VariantValue>& vargs) const;
	VariantValue call_helper(volatile VariantValue& object, const ::std::vector<VariantValue>& vargs) const;
	VariantValue call_helper(const volatile VariantValue& object, const ::std::vector<VariantValue>& vargs) const;
	
	AbstractMethodImpl* m_impl;
	
	template<class M> friend Method make_method(const ::std::string& name, M ptr);
	template<class C, class M> friend Method make_static_method(const ::std::string& name, M ptr);
};



class AbstractClassImpl;

class Class {
public:
	
	typedef ::std::list<Method> MethodList;
	typedef ::std::list<Constructor> ConstructorList;
	typedef ::std::list<Class> ClassList;
	typedef ::std::list<Attribute> AttributeList;
	
	Class(const Class& rhs);
	
	Class& operator=(Class rhs);
		
	std::string simpleName() const;
	
	const std::string& fullyQualifiedName() const;
	
	const type_info& typeId() const;
	
	template<class C>
	bool describes() const {
		return typeId() == typeid(C);
	}
	
	bool isSubClassOf(const Class& rhs) const;
		
	const MethodList& methods() const;
	
	const ConstructorList& constructors() const;
	
	const AttributeList& attributes() const;
	
	const ClassList& superclasses() const;
	
private:
	
	constexpr Class(const AbstractClassImpl* impl);
	
	const AbstractClassImpl* m_impl;
	
	template<class T> friend Class ClassOf();
};

inline bool operator==(const Class& c1, const Class& c2)
{
	return c1.typeId() == c2.typeId();
}

inline bool operator!=(const Class& c1, const Class& c2)
{
	return !(c1 == c2);
}


#endif /* REFLECTION_H */
