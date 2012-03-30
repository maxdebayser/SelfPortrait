#include "reflection.h"

#include "attribute.h"
#include "method.h"
#include "constructor.h"
#include "class.h"

//--------attribute-----------------------------------------------

Attribute::Attribute(AbstractAttributeImpl* impl)
	: m_impl(impl) {}

Attribute::Attribute(const Attribute& rhs)
	: m_impl(rhs.m_impl) {}
	
Attribute::Attribute(Attribute&& rhs)
	: m_impl(rhs.m_impl) {}
	
Attribute& Attribute::operator=(const Attribute& rhs) {
	m_impl = rhs.m_impl; return *this;
}

Attribute& Attribute::operator=(Attribute&& rhs) { 
	m_impl = rhs.m_impl; return *this;
}

const ::std::string& Attribute::name() const { 
	return m_impl->name();
}

const ::std::type_info& Attribute::type() const {
	return m_impl->type();
}

bool Attribute::isConst() const {
	return m_impl->isConst();
}

bool Attribute::isStatic() const {
	return m_impl->isStatic();
}

VariantValue Attribute::get() const {
	return m_impl->get();
}

void Attribute::set(const VariantValue& value) const {
	return m_impl->set(value);
}

VariantValue Attribute::get(const VariantValue& object) const {
	return m_impl->get(object);
}

void Attribute::set(VariantValue& object, const VariantValue& value) const  {
	return m_impl->set(object, value);
}

void Attribute::set(const VariantValue& object, const VariantValue& value) const {
	return m_impl->set(object, value);
}



//--------class---------------------------------------------------

	
constexpr Class::Class(const AbstractClassImpl* impl)
	: m_impl(impl) {}

Class::Class(const Class& rhs)
	: m_impl(rhs.m_impl) {}

Class& Class::operator=(Class rhs) {
	m_impl = rhs.m_impl;
	return *this;
}

std::string Class::simpleName() const {
	::std::string tmp = m_impl->fullyQualifiedName();
	const int pos = tmp.find_last_of(':');
	return tmp.substr((pos == ::std::string::npos) ? 0 : pos);
}

const std::string& Class::fullyQualifiedName() const {
	return m_impl->fullyQualifiedName();
}

const type_info& Class::typeId() const {
	return m_impl->typeId();
}
	
bool Class::isSubClassOf(const Class& rhs) const {
	const ClassList& sups = m_impl->superclasses();
	return (::std::find(sups.begin(), sups.end(), rhs) != sups.end());
}
	
const Class::MethodList& Class::methods() const {
	return m_impl->methods();
}

const Class::ConstructorList& Class::constructors() const {
	return m_impl->constructors();
}

const Class::AttributeList& Class::attributes() const {
	return m_impl->attributes();
}

const Class::ClassList& Class::superclasses() const {
	return m_impl->superclasses();
}


//--------constructor---------------------------------------------

Constructor::Constructor(AbstractConstructorImpl* impl)
	: m_impl(impl) {}

::std::size_t Constructor::numberOfArguments() const {
	return m_impl->numberOfArguments();
}

bool Constructor::isDefaultConstructor() const {
	return numberOfArguments() == 0;
}

::std::vector<const ::std::type_info*> Constructor::argumentTypes() const {
	return m_impl->argumentTypes();
}

VariantValue Constructor::call_helper(const ::std::vector<VariantValue>& vargs) const {
	return m_impl->call(vargs);
}


//--------method---------------------------------------------

Method::Method(AbstractMethodImpl* impl)
	: m_impl(impl) {}

Method::Method(const Method& rhs)
	: m_impl(rhs.m_impl) {}

Method::Method(Method&& rhs)
	: m_impl(rhs.m_impl) {}
	

Method& Method::operator=(const Method& rhs) { 
	m_impl = rhs.m_impl; 
	return *this;
}
	
Method& Method::operator=(Method&& rhs) {
	m_impl = rhs.m_impl;
	return *this;
}

const ::std::string& Method::name() const {
	return m_impl->name();
}

::std::size_t Method::numberOfArguments() const {
	return m_impl->numberOfArguments();
}

::std::vector<const ::std::type_info*> Method::argumentTypes() const {
	return m_impl->argumentTypes();	
}

bool Method::isConst() const {
	return m_impl->isConst();
}

bool Method::isVolatile() const {
	return m_impl->isVolatile();	
}

bool Method::isStatic() const {
	return m_impl->isStatic();
}

VariantValue Method::call_helper(const ::std::vector<VariantValue>& vargs) const {
	return m_impl->call(vargs );	
}

VariantValue Method::call_helper(VariantValue& object, const ::std::vector<VariantValue>& vargs) const {
	return m_impl->call(object, vargs );
}

VariantValue Method::call_helper(const VariantValue& object, const ::std::vector<VariantValue>& vargs) const {
	return m_impl->call(object, vargs );	
}

VariantValue Method::call_helper(volatile VariantValue& object, const ::std::vector<VariantValue>& vargs) const {
	return m_impl->call(object, vargs );	
}

VariantValue Method::call_helper(const volatile VariantValue& object, const ::std::vector<VariantValue>& vargs) const {
	return m_impl->call(object, vargs );	
}
