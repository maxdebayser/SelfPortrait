#include "class.h"
#include "proxy.h"

const std::string& ClassImpl::fullyQualifiedName() const
{
	return m_fqn;
}

const ClassImpl::MethodList& ClassImpl::methods() const
{
	return m_methods;
}

const ClassImpl::ConstructorList& ClassImpl::constructors() const
{
	return m_constructors;
}

const ClassImpl::ClassList& ClassImpl::superclasses() const
{
	return m_superclasses;
}

const ClassImpl::AttributeList& ClassImpl::attributes() const
{
	return m_attributes;
}

bool ClassImpl::open() const {
	return m_open;
}

void ClassImpl::close() {
	m_open = false;
}

void ClassImpl::assert_open() const
{
	if (!m_open) throw ::std::logic_error("meta-class is already closed for registration");
}


ClassImpl::ClassImpl()
	: m_open(true)
	, m_iface(nullptr)
{}

void ClassImpl::setFullyQualifiedName(const ::std::string& fqn)
{
	assert_open();
	m_fqn = fqn;
}

void ClassImpl::registerMethod(Method m)
{ // Method is just a lightweigth handle
	assert_open();
	m.setClass(this);
	m_methods.push_back(m);
}

void ClassImpl::registerConstructor(Constructor c)
{ // Constructor is just a lightweigth handle
	assert_open();
	c.setClass(this);
	m_constructors.push_back(c);
}

void ClassImpl::registerAttribute(Attribute attr)
{
	assert_open();
	attr.setClass(this);
	m_attributes.push_back(attr);
}

void ClassImpl::registerSuperClass(Class c)
{
	assert_open();
	m_superclasses.push_back(c);
	for (const Method& m : c.methods()) {
		m_methods.push_back(m);
	}
	for (const Attribute& a: c.attributes()) {
		m_attributes.push_back(a);
	}
	for(Class s: c.superclasses()) {
		registerSuperClass(s);
	}
}

#ifndef NO_RTTI
const std::type_info& ClassImpl::typeId() const
{
	return *m_typeInfo;
}

void ClassImpl::setTypeInfo(const std::type_info& info)
{
	m_typeInfo = &info;
}

bool ClassImpl::isInterface() const
{
	return m_iface != nullptr;
}

std::unique_ptr<Interface> ClassImpl::newInterface() const
{
	if (m_iface == nullptr) {
		return InterfacePtr();
	} else {
		return InterfacePtr(m_iface->clone());
	}
}

void ClassImpl::registerInterface(Interface* i)
{
	m_iface = i;
}

#endif

