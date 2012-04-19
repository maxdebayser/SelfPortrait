#ifndef REFLECTION_H
#define REFLECTION_H

#include "variant.h"

#include <set>
#include <list>
#ifndef NO_RTTI
#include <typeinfo>
#endif

// All user front-end classes

inline void emplace(std::vector<VariantValue>& v )
{
	// nothing to do
}

template<class T, class... U>
inline void emplace(std::vector<VariantValue>& v, T&& t, U&&... u )
{
	v.emplace_back(t);
	emplace(v, u...);
}

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
	
	::std::string name() const;
	::std::string typeSpelling() const;
	
#ifndef NO_RTTI
	const ::std::type_info& type() const;
#endif
	bool isConst() const;
	bool isStatic() const;
	
	VariantValue get() const;
	
	void set(const VariantValue& value) const;
	
	VariantValue get(const VariantValue& object) const;
	void set(VariantValue& object, const VariantValue& value) const;
	void set(const VariantValue& object, const VariantValue& value) const;
	
private:

	void check_valid() const;

	Attribute(AbstractAttributeImpl* impl);
	
	AbstractAttributeImpl* m_impl;
	
	template<class A>
	friend Attribute make_attribute(const char* name, A ptr, const char* arg);
	template<class C, class A>
	friend Attribute make_static_attribute(const char* name, A ptr, const char* arg);
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
	::std::vector< ::std::string> argumentSpellings() const;
	
	bool isDefaultConstructor() const;
	
#ifndef NO_RTTI
	::std::vector<const ::std::type_info*> argumentTypes() const;
#endif
	
	template<class... Args>
	VariantValue call(const Args&... args) const {
		::std::vector<VariantValue> vargs;
		emplace(vargs, args...);
		return callArgArray(vargs);
	}

	VariantValue callArgArray(const ::std::vector<VariantValue>& vargs) const ;
	
private:

	void check_valid() const;

	Constructor(AbstractConstructorImpl* impl);
	
	AbstractConstructorImpl* m_impl;
	
	friend class Class;
	
	template<class C, class... A>
	friend Constructor make_constructor(const char* argString);
};

class AbstractMethodImpl;

class Method: public AnnotatedFrontend {
public:
	
	Method();
	Method(const Method& rhs);
	Method& operator=(const Method& rhs);
	Method(Method&& rhs);
	Method& operator=(Method&& rhs);
	
	::std::string name() const;
	::std::size_t numberOfArguments() const;
	::std::string returnSpelling() const;
	::std::vector< ::std::string> argumentSpellings() const;

#ifndef NO_RTTI
	::std::vector<const ::std::type_info*> argumentTypes() const;
	const ::std::type_info& returnType() const;
#endif

	bool isConst() const;
	bool isVolatile() const;
	bool isStatic() const;

	template<class... Args>
	VariantValue call(const Args&... args) const {
		::std::vector<VariantValue> vargs;
		emplace(vargs, args...);
		return callArgArray(vargs );
	}
	template<class... Args>
	VariantValue call(VariantValue& object, const Args&... args) const {
		::std::vector<VariantValue> vargs;
		emplace(vargs, args...);
		return callArgArray(object, vargs );
	}
	template<class... Args>
	VariantValue call(const VariantValue& object, const Args&... args) const {
		::std::vector<VariantValue> vargs;
		emplace(vargs, args...);
		return callArgArray(object, vargs );
	}
	template<class... Args>
	VariantValue call(volatile VariantValue& object, const Args&... args) const {
		::std::vector<VariantValue> vargs;
		emplace(vargs, args...);
		return callArgArray(object, vargs );
	}
	template<class... Args>
	VariantValue call(const volatile VariantValue& object, const Args&... args) const {
		::std::vector<VariantValue> vargs;
		emplace(vargs, args...);
		return callArgArray(object, vargs );
	}

	VariantValue callArgArray(const ::std::vector<VariantValue>& vargs) const;
	VariantValue callArgArray(VariantValue& object, const ::std::vector<VariantValue>& vargs) const;
	VariantValue callArgArray(const VariantValue& object, const ::std::vector<VariantValue>& vargs) const;
	VariantValue callArgArray(volatile VariantValue& object, const ::std::vector<VariantValue>& vargs) const;
	VariantValue callArgArray(const volatile VariantValue& object, const ::std::vector<VariantValue>& vargs) const;

private:

	void check_valid() const;
	
	Method(AbstractMethodImpl* impl);
		
	AbstractMethodImpl* m_impl;
	
	template<class M> friend Method make_method(const char* name, M ptr, const char* rString, const char* argString);
	template<class C, class M> friend Method make_static_method(const char* name, M ptr, const char* rString, const char* argString);
};



class ClassImpl;

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
	
#ifndef NO_RTTI
	const ::std::type_info& typeId() const;
	
	template<class C>
	bool describes() const {
		return typeId() == typeid(C);
	}
#endif
	
	bool isSubClassOf(const Class& rhs) const;
		
	const MethodList& methods() const;
	
	const ConstructorList& constructors() const;
	
	const AttributeList& attributes() const;
	
	const ClassList& superclasses() const;

	static Class forName(const ::std::string& name);
	
private:

	void check_valid() const;
	
	Class(ClassImpl* impl);
	
	ClassImpl* m_impl;
	
	template<class T> friend Class ClassOf();
	friend bool operator==(const Class& c1, const Class& c2);
};

inline bool operator==(const Class& c1, const Class& c2)
{
	// The address should be equal
	return c1.m_impl == c2.m_impl;
}

inline bool operator!=(const Class& c1, const Class& c2)
{
	return !(c1 == c2);
}


class AbstractFunctionImpl;

class Function: public AnnotatedFrontend {
public:

	typedef ::std::list<Function> FunctionList;

	Function();
	Function(const Function& rhs);
	Function(Function&& rhs);
	Function& operator=(const Function& rhs);
	Function& operator=(Function&& rhs);

	::std::string name() const;


	::std::size_t numberOfArguments() const;
	::std::string returnSpelling() const;
	::std::vector< ::std::string> argumentSpellings() const;

#ifndef NO_RTTI
	const ::std::type_info& returnType() const;
	::std::vector<const ::std::type_info*> argumentTypes() const;
#endif

	template<class... Args>
	VariantValue call(Args&&... args) const {
		::std::vector<VariantValue> vargs;
		emplace(vargs, args...);
		return callArgArray(vargs);
	}

	VariantValue callArgArray(const ::std::vector<VariantValue>& vargs) const;

	static const FunctionList& findFunctions(const ::std::string& name);

private:

	void check_valid() const;

	Function(AbstractFunctionImpl* impl);
	AbstractFunctionImpl* m_impl;

	template<class FuncPtr>
	friend Function make_function(const char* name, FuncPtr ptr, const char* rString, const char* argString);
};


#endif /* REFLECTION_H */
