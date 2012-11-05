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
	, m_stubCreator(nullptr)
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

void ClassImpl::registerSuperClass(const char* className)
{
	assert_open();
	m_unresolvedBases.push_back(className);
}

void ClassImpl::registerSuperClassInternal(Class c)
{
	m_superclasses.push_back(c);
	for (const Method& m : c.methods()) {
		m_methods.push_back(m);
	}
	for (const Attribute& a: c.attributes()) {
		m_attributes.push_back(a);
	}
	for(Class s: c.superclasses()) {
		registerSuperClassInternal(s);
	}
}

bool ClassImpl::hasUnresolvedBases() const
{
	return !m_unresolvedBases.empty();
}

void ClassImpl::resolveBases()
{
	for (auto it = m_unresolvedBases.begin(); it!= m_unresolvedBases.end(); ++it) {
		Class c = Class::lookup(*it);
		if (c.isValid()) {
			it = m_unresolvedBases.erase(it);
			registerSuperClassInternal(c);
		}
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
	return m_stubCreator != nullptr;
}

VariantValue ClassImpl::newInterface(std::shared_ptr<ProxyImpl>& proxyImpl) const
{
	if (m_stubCreator == nullptr) {
		return VariantValue();
	} else {
		return m_stubCreator(proxyImpl);
	}
}

void ClassImpl::registerInterface(StubCreator sc)
{
	m_stubCreator = sc;
}

#endif

