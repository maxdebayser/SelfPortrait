#ifndef REFLECTION_H
#define REFLECTION_H

#include "variant.h"

#include <set>
#include <list>
#include <typeinfo>

// All user front-end classes

typedef ::std::string Annotation;
typedef ::std::set<Annotation> AnnotationSet;

class Annotated {
public:
	const AnnotationSet& annotations() const { return m_annotations; }
	void addAnnotation(const Annotation& a) { m_annotations.insert(m_annotations.end(), a); }
private:
	AnnotationSet m_annotations;
};

class AnnotatedFrontend {
public:
	const AnnotationSet& annotations() const { return m_instance.annotations(); }
	void addAnnotation(const Annotation& a) { m_instance.addAnnotation(a); }
	AnnotatedFrontend(Annotated& instance) : m_instance(instance) {}
private:
	Annotated& m_instance;
};

class AbstractAttributeImpl;

class Attribute: public AnnotatedFrontend {
public:
	
	Attribute();
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

class Constructor: public AnnotatedFrontend {
public:

	Constructor();
	Constructor(const Constructor& rhs);
	Constructor& operator=(const Constructor& rhs);
	Constructor(Constructor&& rhs);
	Constructor& operator=(Constructor&& rhs);

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

class Method: public AnnotatedFrontend {
public:
	
	Method();
	Method(const Method& rhs);
	Method& operator=(const Method& rhs);
	Method(Method&& rhs);
	Method& operator=(Method&& rhs);
	
	const ::std::string& name() const;
	::std::size_t numberOfArguments() const;
	::std::vector<const ::std::type_info*> argumentTypes() const;

	const ::std::type_info& returnType() const;

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

class Class: public AnnotatedFrontend {
public:
	
	typedef ::std::list<Method> MethodList;
	typedef ::std::list<Constructor> ConstructorList;
	typedef ::std::list<Class> ClassList;
	typedef ::std::list<Attribute> AttributeList;

	Class();
	
	Class(const Class& rhs);

	Class& operator=(const Class& rhs);

	Class(Class&& rhs);

	Class& operator=(Class&& rhs);
		
	std::string simpleName() const;
	
	const std::string& fullyQualifiedName() const;
	
	const ::std::type_info& typeId() const;
	
	template<class C>
	bool describes() const {
		return typeId() == typeid(C);
	}
	
	bool isSubClassOf(const Class& rhs) const;
		
	const MethodList& methods() const;
	
	const ConstructorList& constructors() const;
	
	const AttributeList& attributes() const;
	
	const ClassList& superclasses() const;

	static Class forName(const ::std::string& name);
	
private:
	
	Class(AbstractClassImpl* impl);
	
	AbstractClassImpl* m_impl;
	
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


class AbstractFunctionImpl;

class Function: public AnnotatedFrontend {
public:

	Function();
	Function(const Function& rhs);
	Function(Function&& rhs);
	Function& operator=(const Function& rhs);
	Function& operator=(Function&& rhs);

	const ::std::string& name() const;

	const ::std::type_info& returnType() const;
	::std::size_t numberOfArguments() const;
	::std::vector<const ::std::type_info*> argumentTypes() const;

	template<class... Args>
	VariantValue call(const Args&... args) const {
		::std::vector<VariantValue> vargs{ VariantValue(args)... };
		return callHelper(vargs);
	}

	static const ::std::list<Function> findFunctions(const ::std::string& name);

private:

	VariantValue callHelper(::std::vector<VariantValue>& vargs) const;

	Function(AbstractFunctionImpl* impl);
	AbstractFunctionImpl* m_impl;

	template<class FuncPtr>
	friend Function make_function(const ::std::string& name, FuncPtr ptr);
};


#endif /* REFLECTION_H */
