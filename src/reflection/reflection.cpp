/*
** SelfPortrait API
** See Copyright Notice in reflection.h
*/
#include "reflection.h"

#include "attribute.h"
#include "function.h"
#include "method.h"
#include "constructor.h"
#include "class.h"
#include "reflection_impl.h"
#include "proxy.h"

#include <algorithm>
#include <sstream>

//--------attribute-----------------------------------------------

bool Attribute::isValid() const
{
	return (m_impl != nullptr);
}

void Attribute::check_valid() const
{
	if (!isValid()) {
		throw std::runtime_error("Invalid use of uninitialized Attribute handle");
	}
}

Attribute::Attribute() : Attribute(nullptr) {}

Attribute::Attribute(AbstractAttributeImpl* impl, ClassImpl *cimpl)
	: AnnotatedFrontend(*impl)
	, m_impl(impl)
	, m_class(cimpl) {}

Attribute::Attribute(AbstractAttributeImpl* impl) : Attribute(impl, nullptr) {}

Attribute::Attribute(const Attribute& rhs) : Attribute(rhs.m_impl, rhs.m_class) {}
	
Attribute::Attribute(Attribute&& rhs) : Attribute(rhs.m_impl, rhs.m_class) {}
	
Attribute& Attribute::operator=(const Attribute& rhs) {
	m_impl = rhs.m_impl; m_class = rhs.m_class; return *this;
}

Attribute& Attribute::operator=(Attribute&& rhs) { 
	m_impl = rhs.m_impl; m_class = rhs.m_class; return *this;
}

::std::string Attribute::name() const {
	check_valid();
	return m_impl->name();
}

::std::string Attribute::typeSpelling() const
{
	check_valid();
	return m_impl->typeSpelling();
}

#ifndef NO_RTTI
const ::std::type_info& Attribute::type() const {
	check_valid();
	return m_impl->type();
}
#endif

bool Attribute::isConst() const {
	check_valid();
	return m_impl->isConst();
}

bool Attribute::isStatic() const {
	check_valid();
	return m_impl->isStatic();
}

VariantValue Attribute::get() const {
	check_valid();
	return m_impl->get();
}

void Attribute::set(const VariantValue& value) const {
	check_valid();
	return m_impl->set(value);
}

VariantValue Attribute::get(const VariantValue& object) const {
	check_valid();
	return m_impl->get(object);
}

void Attribute::set(VariantValue& object, const VariantValue& value) const  {
	check_valid();
	return m_impl->set(object, value);
}

void Attribute::set(const VariantValue& object, const VariantValue& value) const {
	check_valid();
	return m_impl->set(object, value);
}

Class Attribute::getClass() const
{
	return Class(m_class);
}

namespace std {
	size_t hash<Attribute>::operator()(const Attribute& a) const {
		return reinterpret_cast<size_t>(a.m_impl);
	}
}


//--------class---------------------------------------------------

bool Class::isValid() const
{
	return m_impl != nullptr;
}

void Class::check_valid() const
{
	if (!isValid()) {
		throw std::runtime_error("Invalid use of uninitialized Class handle");
	}
}

Class::Class()
	: Class(nullptr) {}
	
Class::Class(ClassImpl* impl)
	: AnnotatedFrontend(*impl)
	, m_impl(impl)
{
	if (m_impl) {
		m_impl->resolveBases();
	}
}

Class::Class(const Class& rhs)
	: Class(rhs.m_impl) {}

Class& Class::operator=(const Class& rhs) {
	m_impl = rhs.m_impl;
	return *this;
}

Class::Class(Class&& rhs)
	: Class(rhs.m_impl) {}

Class& Class::operator=(Class&& rhs) {
	m_impl = rhs.m_impl;
	return *this;
}

std::string Class::simpleName() const {
	check_valid();
	::std::string tmp = m_impl->fullyQualifiedName();
    const size_t pos = tmp.find_last_of(':');
	return tmp.substr((pos == ::std::string::npos) ? 0 : pos+1);
}

const std::string& Class::fullyQualifiedName() const {
	check_valid();
	return m_impl->fullyQualifiedName();
}

#ifndef NO_RTTI
const std::type_info& Class::typeId() const {
	check_valid();
	return m_impl->typeId();
}
#endif
	
bool Class::isSubClassOf(const Class& rhs) const {
	check_valid();
	const ClassList& sups = m_impl->superclasses();
	return (::std::find(sups.begin(), sups.end(), rhs) != sups.end());
}
	
const Class::MethodList& Class::methods() const {
	check_valid();
	return m_impl->methods();
}

namespace {

	template<class T, class C>
	T findFirst(std::function<bool(const T& m)> criteria, const C& coll) {
		for(const T& c: coll) {
			if (criteria(c)) {
				return c;
			}
		}
		return T();
	}

	template<class T, class C>
	C findAll(std::function<bool(const T& m)> criteria, const C& coll) {
		C ret;
		for(const T& c: coll) {
			if (criteria(c)) {
				ret.push_back(c);
			}
		}
		return ret;
	}
}

Method Class::findMethod(std::function<bool(const Method& m)> criteria) const
{
	check_valid();
	return findFirst(criteria, m_impl->methods());
}

Class::MethodList Class::findAllMethods(std::function<bool(const Method& m)> criteria) const
{
	check_valid();
	return findAll(criteria, m_impl->methods());
}

const Class::ConstructorList& Class::constructors() const {
	check_valid();
	return m_impl->constructors();
}

Constructor Class::findConstructor(std::function<bool(const Constructor& m)> criteria) const
{
	check_valid();
	return findFirst(criteria, m_impl->constructors());
}

Class::ConstructorList Class::findAllConstructors(std::function<bool(const Constructor& m)> criteria) const
{
	check_valid();
	return findAll(criteria, m_impl->constructors());
}

const Class::AttributeList& Class::attributes() const {
	check_valid();
	return m_impl->attributes();
}

Attribute Class::getAttribute(const std::string& name) const
{
	return findAttribute([&](const Attribute& a){ return a.name() == name; });
}

Attribute Class::findAttribute(std::function<bool(const Attribute& m)> criteria) const
{
	check_valid();
	return findFirst(criteria, m_impl->attributes());
}

Class::AttributeList Class::findAllAttributes(std::function<bool(const Attribute& m)> criteria) const
{
	check_valid();
	return findAll(criteria, m_impl->attributes());
}

const Class::ClassList& Class::superclasses() const {
	check_valid();
	return m_impl->superclasses();
}


Class Class::findSuperClass(std::function<bool(const Class& m)> criteria) const
{
	check_valid();
	return findFirst(criteria, m_impl->superclasses());
}

Class::ClassList Class::findAllSuperClasses(std::function<bool(const Class& m)> criteria) const
{
	check_valid();
	return findAll(criteria, m_impl->superclasses());
}

Class Class::lookup(const ::std::string& name)
{
	return ClassRegistry::instance().forName(name);
}

#ifndef NO_RTTI
Class Class::lookup(const ::std::type_info& id)
{
	return ClassRegistry::instance().forTypeId(id);
}
#endif

bool Class::isInterface() const {
	return m_impl->isInterface();
}

bool Class::hasUnresolvedBases() const
{
	return m_impl->hasUnresolvedBases();
}

const Class ClassRegistry::forName(const ::std::string& name) const
{
	auto it = m_registryByName.find(name);
	if (it != m_registryByName.end()) {
		return it->second;
	} else {
		return Class();
	}
}

#ifndef NO_RTTI
const Class ClassRegistry::forTypeId(const ::std::type_info& id) const
{
	auto it = m_registryByTypeId.find(id);
	if (it != m_registryByTypeId.end()) {
		return it->second;
	} else {
		return Class();
	}
}
#endif

void ClassRegistry::registerClass(const Class& c)
{
	m_registryByName[c.fullyQualifiedName()] = c;
#ifndef NO_RTTI
	m_registryByTypeId[c.typeId()] = c;
#endif
}

ClassRegistry& ClassRegistry::instance()
{
	static ClassRegistry instance;
	return instance;
}

namespace {

	enum CRel {
		SAME,
		UNRELATED,
		MORE_ABSTRACT,
		LESS_ABSTRACT
	};

	CRel classifyRelationship(const Class& c1, const Class& c2)
	{
		// even if there is no difference in "abstractness" the order is deterministic
		if (c1 == c2) {
			return SAME;
		}

		if (find(c2.superclasses().begin(), c2.superclasses().end(), c1) != c2.superclasses().end()) {
			return MORE_ABSTRACT;
		}

		if (find(c1.superclasses().begin(), c1.superclasses().end(), c2) != c1.superclasses().end()) {
			return LESS_ABSTRACT;
		}

		return UNRELATED;
	}
}

bool inheritanceRelation(const Class& c1, const Class& c2)
{
	return classifyRelationship(c1, c2) != UNRELATED;
}

bool inherits(const Class& c1, const Class& c2)
{
	return classifyRelationship(c1, c2) == LESS_ABSTRACT;
}

bool inheritedBy(const Class& c1, const Class& c2)
{
	return classifyRelationship(c1, c2) == MORE_ABSTRACT;
}



//--------constructor---------------------------------------------

bool Constructor::isValid() const
{
	return m_impl != nullptr;
}

void Constructor::check_valid() const
{
	if (!isValid()) {
		throw std::runtime_error("Invalid use of uninitialized Constructor handle");
	}
}

Constructor::Constructor() : Constructor(nullptr) {}

Constructor::Constructor(const Constructor& rhs) : Constructor(rhs.m_impl, rhs.m_class) {}

Constructor& Constructor::operator=(const Constructor& rhs) { m_impl = rhs.m_impl; m_class = rhs.m_class; return *this; }

Constructor::Constructor(Constructor&& rhs) : Constructor(rhs.m_impl, rhs.m_class) {}

Constructor& Constructor::operator=(Constructor&& rhs) { m_impl = rhs.m_impl; m_class = rhs.m_class; return *this; }

Constructor::Constructor(ConstructorImpl* impl) : Constructor(impl, nullptr) {}

Constructor::Constructor(ConstructorImpl* impl, ClassImpl* cimpl)
	: AnnotatedFrontend(*impl)
	, m_impl(impl)
	, m_class(cimpl) {}

::std::size_t Constructor::numberOfArguments() const {
	check_valid();
	return m_impl->numberOfArguments();
}

::std::vector< ::std::string> Constructor::argumentSpellings() const
{
	check_valid();
	return m_impl->argumentSpellings();
}

bool Constructor::isDefaultConstructor() const {
	check_valid();
	return numberOfArguments() == 0;
}

#ifndef NO_RTTI
::std::vector<const ::std::type_info*> Constructor::argumentTypes() const {
	check_valid();
	return m_impl->argumentTypes();
}
#endif

VariantValue Constructor::callArgArray(const ::std::vector<VariantValue>& vargs) const {
	check_valid();
	return m_impl->call(vargs);
}

Class Constructor::getClass() const
{
	return Class(m_class);
}


namespace std {
	size_t hash<Constructor>::operator()(const Constructor& c) const {
		return reinterpret_cast<size_t>(c.m_impl);
	}
}

//--------method---------------------------------------------

bool Method::isValid() const
{
	return m_impl != nullptr;
}

std::string Method::fullName() const
{
	std::stringstream ss;
	if (isStatic()) {
		ss << "static ";
	}
	ss << returnSpelling() <<
		  " " <<
		  getClass().fullyQualifiedName() <<
		  "::" <<
		  name() <<
			"(";

	bool first = true;
	for(const std::string& s: argumentSpellings()) {
		if (!first) {
			ss << ", ";
		}
		first = false;
		ss << s;
	}
	ss << ")";

	if (isConst()) {
		ss << " const";
	}

	if (isVolatile()) {
		ss << " volatile";
	}

	return ss.str();
}

void Method::check_valid() const
{
	if (!isValid()) {
		throw std::runtime_error("Invalid use of uninitialized Method handle");
	}
}

Method::Method()
	: Method(nullptr) {}

Method::Method(MethodImpl* impl, ClassImpl* cimpl)
	: AnnotatedFrontend(*impl)
	, m_impl(impl)
	, m_class(cimpl) {}

Method::Method(MethodImpl* impl) : Method(impl, nullptr) {}

Method::Method(const Method& rhs)
	: Method(rhs.m_impl, rhs.m_class) {}

Method::Method(Method&& rhs)
	: Method(rhs.m_impl, rhs.m_class) {}
	

Method& Method::operator=(const Method& rhs) { 
	m_impl = rhs.m_impl; 
	m_class = rhs.m_class;
	return *this;
}
	
Method& Method::operator=(Method&& rhs) {
	m_impl = rhs.m_impl;
	m_class = rhs.m_class;
	return *this;
}

::std::string Method::name() const {
	check_valid();
	return m_impl->name();
}

::std::size_t Method::numberOfArguments() const {
	check_valid();
	return m_impl->numberOfArguments();
}


::std::string Method::returnSpelling() const {
	check_valid();
	return m_impl->returnTypeSpelling();
}

::std::vector< ::std::string> Method::argumentSpellings() const {
	check_valid();
	return m_impl->argumentSpellings();
}

#ifndef NO_RTTI
::std::vector<const ::std::type_info*> Method::argumentTypes() const {
	check_valid();
	return m_impl->argumentTypes();	
}

const ::std::type_info& Method::returnType() const {
	check_valid();
	return m_impl->returnType();
}
#endif

bool Method::isConst() const {
	check_valid();
	return m_impl->isConst();
}

bool Method::isVolatile() const {
	check_valid();
	return m_impl->isVolatile();	
}

bool Method::isStatic() const {
	check_valid();
	return m_impl->isStatic();
}

VariantValue Method::callArgArray(const ::std::vector<VariantValue>& vargs) const {
	check_valid();
	return m_impl->call(vargs );	
}

VariantValue Method::callArgArray(VariantValue& object, const ::std::vector<VariantValue>& vargs) const {
	check_valid();
	return m_impl->call(object, vargs );
}

VariantValue Method::callArgArray(const VariantValue& object, const ::std::vector<VariantValue>& vargs) const {
	check_valid();
	return m_impl->call(object, vargs );	
}

VariantValue Method::callArgArray(volatile VariantValue& object, const ::std::vector<VariantValue>& vargs) const {
	check_valid();
	return m_impl->call(object, vargs );	
}

VariantValue Method::callArgArray(const volatile VariantValue& object, const ::std::vector<VariantValue>& vargs) const {
	check_valid();
	return m_impl->call(object, vargs );	
}

Class Method::getClass() const {
	return Class(m_class);
}



namespace std {

	size_t hash<Method>::operator()(const Method& m) const {
		if (m.m_impl == nullptr) {
			return 0;
		} else {
			return reinterpret_cast<size_t>(m.m_impl->m_method);
		}
	}
}

bool overrides(const Method& m1, const Method& m2)
{
	// cheap tests first
	if (m1.getClass() == m2.getClass()) return false;

	if (m1.name() != m2.name()) return false;

	if (m1.isStatic() || m2.isStatic()) return false;

	if (m1.numberOfArguments() != m2.numberOfArguments()) return false;

	if (m1.isConst() != m2.isConst()) return false;

	if (m1.isVolatile() != m2.isVolatile()) return false;

	// costly tests
	if (!inheritanceRelation(m1.getClass(), m2.getClass())) return false;

	// The return value can be difference because of covariance
	// Fortunately (to us) the compiler doesn't allow overloads based only on return type

#ifndef NO_RTTI

	auto args1 = m1.argumentTypes();
	auto args2 = m2.argumentTypes();

	auto it1 = begin(args1);
	auto it2 = begin(args2);

	for (; it1 != end(args1); ++it1, ++it2)
		if (**it1 != **it2) return false;

#else

	auto args1 = m1.argumentSpellings();
	auto args2 = m2.argumentSpellings();

	auto it1 = begin(args1);
	auto it2 = begin(args2);

	for (; it1 != end(args1); ++it1, ++it2)
		if (*it1 != *it2) return false;

#endif

	// ok, one definitely overrides the other
	return inherits(m1.getClass(), m2.getClass());
}

bool overloads(const Method& m1, const Method& m2)
{
	// cheap tests first
	if (m1 == m2) return false;

	if (m1.name() != m2.name()) return false;

	// can a static method overload a non-static one?
	if (m1.isStatic() != m2.isStatic()) return false;

	// costly tests
	if (!inheritanceRelation(m1.getClass(), m2.getClass())) return false;

	bool differenceFound =
		(m1.isConst() != m2.isConst()) ||
		(m1.isVolatile() != m2.isVolatile()) ||
		(m1.numberOfArguments() != m2.numberOfArguments());

	if (!differenceFound) {

#ifndef NO_RTTI

		auto args1 = m1.argumentTypes();
		auto args2 = m2.argumentTypes();

		auto it1 = begin(args1);
		auto it2 = begin(args2);

		for (; it1 != end(args1); ++it1, ++it2) {
			if (**it1 != **it2) {
				differenceFound = true;
				break;
			}
		}

#else
		// less safe
		auto args1 = m1.argumentSpellings();
		auto args2 = m2.argumentSpellings();

		auto it1 = begin(args1);
		auto it2 = begin(args2);

		for (; it1 != end(args1); ++it1, ++it2) {
			if (*it1 != *it2) {
				differenceFound = true;
				break
			}
		}

#endif

	}


	// ok, they can only be overloads
	return true;
}


//--------function---------------------------------------------

bool Function::isValid() const
{
	return m_impl != nullptr;
}

void Function::check_valid() const
{
	if (!isValid()) {
		throw std::runtime_error("Invalid use of uninitialized Function handle");
	}
}

Function::Function()
	: Function(nullptr)
{}

Function::Function(const Function& rhs)
	: Function(rhs.m_impl)
{}

Function::Function(Function&& rhs)
	: Function(rhs.m_impl)
{}

Function& Function::operator=(const Function& rhs) {
	m_impl = rhs.m_impl; return *this;
}

Function& Function::operator=(Function&& rhs) {
	m_impl = rhs.m_impl; return *this;
}

::std::string Function::name() const {
	check_valid();
	return m_impl->name();
}

#ifndef NO_RTTI
const ::std::type_info& Function::returnType() const {
	check_valid();
	return m_impl->returnType();
}
#endif

::std::size_t Function::numberOfArguments() const {
	check_valid();
	return m_impl->numberOfArguments();
}

::std::string Function::returnSpelling() const {
	check_valid();
	return m_impl->returnTypeSpelling();
}

::std::vector< ::std::string> Function::argumentSpellings() const {
	check_valid();
	return m_impl->argumentSpellings();
}

#ifndef NO_RTTI
::std::vector<const ::std::type_info*> Function::argumentTypes() const {
	check_valid();
	return m_impl->argumentTypes();
}
#endif

VariantValue Function::callArgArray(const ::std::vector<VariantValue>& vargs) const
{
	check_valid();
	return m_impl->call(vargs);
}

Function::Function(FunctionImpl* impl)
	: AnnotatedFrontend(*impl)
	, m_impl(impl)
{}

const ::std::list<Function>& Function::findFunctions(const ::std::string& name)
{
	return FunctionRegistry::instance().findFunction(name);
}


const ::std::list<Function>& FunctionRegistry::findFunction(const ::std::string& name) const
{
	auto it = m_registry.find(name);
	if (it == m_registry.end()) {
		return emptyList;
	} else {
		return it->second;
	}
}

void FunctionRegistry::registerFunction(const ::std::string& name, const Function& func)
{
	auto it = m_registry.find(name);
	if (it == m_registry.end()) {
		it = m_registry.insert(make_pair(name, ::std::list<Function>())).first;
	}
	it->second.push_back(func);
}

FunctionRegistry& FunctionRegistry::instance() {
	static FunctionRegistry instance;
	return instance;
}

namespace std {
	size_t hash<Function>::operator()(const Function& f)const {
		if (f.m_impl == nullptr) {
			return 0;
		} else {
			return reinterpret_cast<size_t>(f.m_impl->m_f);
		}
	}
}

//--------proxy------------------------------------------------

Proxy::~Proxy() {
	//cout proxy morrendo
	m_impl->decHandleCount();
}

Proxy::Proxy(Proxy&& that)
	: m_impl(that.m_impl)
{
	m_impl->incHandleCount();
}

Proxy::Proxy(const Proxy& that)
	: m_impl(that.m_impl)
{
	m_impl->incHandleCount();
}

void Proxy::swap(Proxy& that)
{
	std::swap(m_impl, that.m_impl);
}

Proxy::Proxy(std::initializer_list<Class> ifaces)
	: Proxy(std::vector<Class>(ifaces))
{}

Proxy::Proxy(std::vector<Class> ifaces)
	: m_impl(new ProxyImpl())
{	
	m_impl->incHandleCount();
	m_impl->weakThis = m_impl;
	for(Class clazz: ifaces) {
		if (!clazz.m_impl->isInterface()) {
			throw std::logic_error(fmt_str("cannot create proxy for a class %1 which is not an interface", clazz.fullyQualifiedName()));
		}
		if (clazz.m_impl->hasUnresolvedBases()) {
			throw std::logic_error(fmt_str("cannot create proxy for a class %1 which has unresolved base classes", clazz.fullyQualifiedName()));
		}

		m_impl->registerInterface(clazz.m_impl);
	}
}

Proxy::Proxy(Class iface)
	: Proxy{iface}
{
}


Proxy::IFaceList Proxy::interfaces() const
{
	Proxy::IFaceList ret;
	auto ifaces = m_impl->interfaces();
	for (auto it = ifaces.begin(); it != ifaces.end(); ++it) {
		ret.push_back(Class(*it));
	}
	return std::move(ret);
}

void Proxy::addImplementation(const Method& m, MethodHandler handler)
{
	std::hash<Method> h;
    m_impl->registerHandler(h(m), std::move(handler));
}

bool Proxy::hasImplementation(const Method& m) const
{
	std::hash<Method> h;
	return m_impl->hasHandler(h(m));
}


VariantValue Proxy::reference(const Class& c) const
{
	return m_impl->ref(c.m_impl);
}
