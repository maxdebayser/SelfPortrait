#include "proxy.h"
#include "class.h"

#include <stdexcept>

Interface::~Interface()
{
	// if another classe takes ownership of the stub
	// we detect this situation and avoid a double delete
	if (m_proxy) {
		m_proxy->releaseMe(this);
	}
}

void Interface::release() {
	m_proxy = nullptr;
}

VariantValue Interface::callArgArray(size_t method_hash, const ::std::vector<VariantValue>& vargs) const
{
	return m_proxy->call(method_hash, vargs);
}

ProxyImpl::ProxyImpl(const ProxyImpl& that)
	: m_map(that.m_map)
{
	for (auto it = that.m_interfaces.begin(); it != that.m_interfaces.end(); ++it) {
		m_interfaces[it->first] = InterfacePtr(it->second->clone());
	}
}


void ProxyImpl::registerInterface(ClassImpl* iface)
{
	InterfacePtr ptr = iface->newInterface();
	ptr->setProxy(this);

	m_interfaces[iface] = std::move(ptr);
}

void ProxyImpl::registerHandler(size_t method_hash, Proxy::MethodHandler mh)
{
	m_map[method_hash] = mh;
}

bool ProxyImpl::hasHandler(size_t method_hash) const {
	return (m_map.find(method_hash) != m_map.end());
}

VariantValue ProxyImpl::call(size_t method_hash, const ::std::vector<VariantValue>& vargs) const
{
	auto it = m_map.find(method_hash);
	if (it == m_map.end()) {
		throw std::logic_error("method not implemented");
	}

	return it->second(vargs);
}

std::list<ClassImpl*> ProxyImpl::interfaces() const
{
	std::list<ClassImpl*> ret;

	for (auto it = m_interfaces.begin(); it != m_interfaces.end(); ++it) {
		ret.push_back(it->first);
	}

	return std::move(ret);
}


VariantValue ProxyImpl::ref(ClassImpl* clazz) const
{
	auto it = m_interfaces.find(clazz);
	if (it != m_interfaces.end()) {
		return it->second->refToBase();
	}
	return VariantValue();
}

ProxyImpl::~ProxyImpl() {
	for (auto it = m_interfaces.begin(); it != m_interfaces.end(); ++it) {
		it->second->release();
	}
}

void ProxyImpl::releaseMe(Interface* i)
{
	for (auto it = m_interfaces.begin(); it != m_interfaces.end(); ++it) {
		if (it->second.get() == i) {
			m_interfaces.erase(it);
			break;
		}
	}
}
