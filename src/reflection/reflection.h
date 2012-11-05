#ifndef REFLECTION_H
#define REFLECTION_H

#include "collection_utils.h"
#include "variant.h"

#include <set>
#include <list>
#ifndef NO_RTTI
#include <typeinfo>
#endif

#include <map>
#include <functional>
#include <initializer_list>

// All user front-end classes


class ClassImpl;
class Class;

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

	bool isValid() const;

	Class getClass() const;
	
	Attribute(AbstractAttributeImpl* impl);
private:

	Attribute(AbstractAttributeImpl* impl, ClassImpl *cimpl);

	void check_valid() const;

	
	AbstractAttributeImpl* m_impl;
	ClassImpl* m_class;

	void setClass(ClassImpl* impl) { m_class = impl; }
	
	template<class A>
	friend Attribute make_attribute(const char* name, A ptr, const char* arg);
	template<class C, class A>
	friend Attribute make_static_attribute(const char* name, A ptr, const char* arg);

	friend struct std::hash<Attribute>;
	friend class ClassImpl;
};

namespace std {
	template<>
	struct hash<Attribute> {
		size_t operator()(const Attribute& a) const;
	};
}

inline bool operator==(const Attribute& a1, const Attribute& a2) {
	std::hash<Attribute> h;
	return h(a1) == h(a2);
}

inline bool operator!=(const Attribute& a1, const Attribute& a2) {
	return !(a1 == a2);
}

class ConstructorImpl;

class Constructor: public AnnotatedFrontend {
public:

	Constructor();
	Constructor(const Constructor& rhs);
	Constructor& operator=(const Constructor& rhs);
	Constructor(Constructor&& rhs);
	Constructor& operator=(Constructor&& rhs);

	bool isValid() const;

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

	Class getClass() const;
	
	Constructor(ConstructorImpl* impl);
private:
	Constructor(ConstructorImpl* impl, ClassImpl* cimpl);

	void check_valid() const;

	void setClass(ClassImpl* impl) { m_class = impl; }
	
	ConstructorImpl* m_impl;
	ClassImpl* m_class;
	
	friend class ClassImpl;
	friend struct std::hash<Constructor>;
};

namespace std {
	template<>
	struct hash<Constructor> {
		size_t operator()(const Constructor& c) const;
	};
}

inline bool operator==(const Constructor& c1, const Constructor& c2) {
	std::hash<Constructor> h;
	return h(c1) == h(c2);
}

inline bool operator!=(const Constructor& c1, const Constructor& c2) {
	return !(c1 == c2);
}


class MethodImpl;

typedef VariantValue (*boundmethod)(const volatile VariantValue&, const ::std::vector<VariantValue>& args);

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

	Class getClass() const;

	Method(MethodImpl* impl);

	bool isValid() const;

	std::string fullName() const;

private:

	Method(MethodImpl* impl, ClassImpl* cimpl);

	void check_valid() const;
		
	MethodImpl* m_impl;
	ClassImpl* m_class;

	void setClass(ClassImpl* impl) { m_class = impl; }
	
	template<class M> friend Method make_method(boundmethod m, const char* name, const char* rString, const char* argString);
	template<class C, class M> friend Method make_static_method(boundmethod m, const char* name, const char* rString, const char* argString);
	friend struct std::hash<Method>;
	friend class ClassImpl;
};


namespace std {

	template<>
	struct hash<Method>{
		size_t operator()(const Method& m) const;
	};
}

inline bool operator==(const Method& m1, const Method& m2)
{
	std::hash<Method> h;
	return h(m1) == h(m2);
}


inline bool operator!=(const Method& m1, const Method& m2)
{
	return !(m1 == m2);
}


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

	bool isValid() const;
		
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

	Method findMethod(std::function<bool(const Method& m)> criteria) const;

	MethodList findAllMethods(std::function<bool(const Method& m)> criteria) const;
	
	const ConstructorList& constructors() const;

	Constructor findConstructor(std::function<bool(const Constructor& m)> criteria) const;

	ConstructorList findAllConstructors(std::function<bool(const Constructor& m)> criteria) const;
	
	const AttributeList& attributes() const;

	Attribute getAttribute(const std::string& name) const;

	Attribute findAttribute(std::function<bool(const Attribute& m)> criteria) const;

	AttributeList findAllAttributes(std::function<bool(const Attribute& m)> criteria) const;
	
	const ClassList& superclasses() const;

	Class findSuperClass(std::function<bool(const Class& m)> criteria) const;

	ClassList findAllSuperClasses(std::function<bool(const Class& m)> criteria) const;

	static Class lookup(const ::std::string& name);

	bool isInterface() const;

	bool hasUnresolvedBases() const;
	
private:

	void check_valid() const;
	
	Class(ClassImpl* impl);
	
	ClassImpl* m_impl;
	
	template<class T> friend Class ClassOf();
	friend struct std::hash<Class>;
	friend class Constructor;
	friend class Attribute;
	friend class Method;
	friend class Proxy;
};


namespace std {
	template<>
	struct hash< ::Class> {
		size_t operator()(const Class& c) const {
			return reinterpret_cast<std::size_t>(c.m_impl);
		}
	};
}

inline bool operator==(const Class& c1, const Class& c2)
{
	std::hash<Class> h;
	return h(c1) == h(c2);
}

inline bool operator!=(const Class& c1, const Class& c2)
{
	return !(c1 == c2);
}


class FunctionImpl;

typedef VariantValue (*boundfunction)(const ::std::vector<VariantValue>& args);

class Function: public AnnotatedFrontend {
public:

	typedef ::std::list<Function> FunctionList;

	Function();
	Function(const Function& rhs);
	Function(Function&& rhs);
	Function& operator=(const Function& rhs);
	Function& operator=(Function&& rhs);

	bool isValid() const;

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

	Function(FunctionImpl* impl);
	FunctionImpl* m_impl;

	template<class FuncPtr>
	friend Function make_function(boundfunction bf, const char* name, const char* rString, const char* argString);
	friend struct std::hash<Function>;
};

namespace std {
	template<>
	struct hash< ::Function> {
		size_t operator()(const Function& f) const;
	};
}

inline bool operator==(const Function& f1, const Function& f2) {
	std::hash<Function> h;
	return h(f1) == h(f2);
}

inline bool operator!=(const Function& f1, const Function& f2) {
	return !(f1 == f2);
}

class ProxyImpl;

class Proxy {
public:

	// deep copy
	/*Proxy(const Proxy& that);

	void swap(Proxy& that);

	Proxy& operator=(Proxy that) {
		swap(that);
		return *this;
	}*/

	Proxy(Proxy&& that);

	~Proxy();

	typedef std::list<Class> IFaceList;
	typedef std::function<VariantValue(const std::vector<VariantValue>&)> MethodHandler;

	Proxy(Class iface);

	Proxy(std::initializer_list<Class> ifaces);

	Proxy(std::vector<Class> ifaces);

	IFaceList interfaces() const;

	void addImplementation(const Method& m, MethodHandler);

	bool hasImplementation(const Method& m) const;

	VariantValue reference(const Class& c) const;

private:

	std::unique_ptr<ProxyImpl> m_impl;
};


inline bool operator==(const Proxy& p1, const Proxy& p2) {
	// an instance is equal only to itself
	return &p1 == &p2;
}

inline bool operator!=(const Proxy& p1, const Proxy& p2) {
	return !(p1 == p2);
}

/*namespace std {
	inline void swap(Proxy& p1, Proxy& p2) {
		p1.swap(p2);
	}
}*/

#endif /* REFLECTION_H */
