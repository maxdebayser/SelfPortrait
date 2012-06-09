#include "reflection.h"

#include "attribute.h"
#include "function.h"
#include "method.h"
#include "constructor.h"
#include "class.h"
#include "reflection_impl.h"

//--------attribute-----------------------------------------------

void Attribute::check_valid() const
{
	if (m_impl == nullptr) {
		throw std::runtime_error("Invalid use of uninitialized Attribute handle");
	}
}

Attribute::Attribute() : Attribute(nullptr) {}

Attribute::Attribute(AbstractAttributeImpl* impl)
	: AnnotatedFrontend(*impl)
	, m_impl(impl) {}

Attribute::Attribute(const Attribute& rhs) : Attribute(rhs.m_impl) {}
	
Attribute::Attribute(Attribute&& rhs) : Attribute(rhs.m_impl) {}
	
Attribute& Attribute::operator=(const Attribute& rhs) {
	m_impl = rhs.m_impl; return *this;
}

Attribute& Attribute::operator=(Attribute&& rhs) { 
	m_impl = rhs.m_impl; return *this;
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



//--------class---------------------------------------------------

void Class::check_valid() const
{
	if (m_impl == nullptr) {
		throw std::runtime_error("Invalid use of uninitialized Class handle");
	}
}

Class::Class()
	: Class(nullptr) {}
	
Class::Class(ClassImpl* impl)
	: AnnotatedFrontend(*impl)
	, m_impl(impl) {}

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
	const int pos = tmp.find_last_of(':');
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

const Class::ConstructorList& Class::constructors() const {
	check_valid();
	return m_impl->constructors();
}

const Class::AttributeList& Class::attributes() const {
	check_valid();
	return m_impl->attributes();
}

const Class::ClassList& Class::superclasses() const {
	check_valid();
	return m_impl->superclasses();
}

Class Class::lookup(const ::std::string& name)
{
	return ClassRegistry::instance().forName(name);
}

const Class ClassRegistry::forName(const ::std::string& name) const
{
	auto it = m_registry.find(name);
	if (it != m_registry.end()) {
		return it->second;
	} else {
		return Class();
	}
}

void ClassRegistry::registerClass(const ::std::string& name, const Class& c)
{
	m_registry[name] = c;
}

ClassRegistry& ClassRegistry::instance()
{
	static ClassRegistry instance;
	return instance;
}


//--------constructor---------------------------------------------


void Constructor::check_valid() const
{
	if (m_impl == nullptr) {
		throw std::runtime_error("Invalid use of uninitialized Constructor handle");
	}
}

Constructor::Constructor() : Constructor(nullptr) {}

Constructor::Constructor(const Constructor& rhs) : Constructor(rhs.m_impl) {}

Constructor& Constructor::operator=(const Constructor& rhs) { m_impl = rhs.m_impl; return *this; }

Constructor::Constructor(Constructor&& rhs) : Constructor(rhs.m_impl) {}

Constructor& Constructor::operator=(Constructor&& rhs) { m_impl = rhs.m_impl; return *this; }

Constructor::Constructor(ConstructorImpl* impl)
	: AnnotatedFrontend(*impl)
	, m_impl(impl) {}

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


//--------method---------------------------------------------


void Method::check_valid() const
{
	if (m_impl == nullptr) {
		throw std::runtime_error("Invalid use of uninitialized Method handle");
	}
}

Method::Method()
	: Method(nullptr) {}

Method::Method(MethodImpl* impl)
	: AnnotatedFrontend(*impl)
	, m_impl(impl) {}

Method::Method(const Method& rhs)
	: Method(rhs.m_impl) {}

Method::Method(Method&& rhs)
	: Method(rhs.m_impl) {}
	

Method& Method::operator=(const Method& rhs) { 
	m_impl = rhs.m_impl; 
	return *this;
}
	
Method& Method::operator=(Method&& rhs) {
	m_impl = rhs.m_impl;
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

//--------function---------------------------------------------


void Function::check_valid() const
{
	if (m_impl == nullptr) {
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
