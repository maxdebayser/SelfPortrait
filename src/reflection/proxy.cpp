#include "proxy.h"
#include "class.h"

#include <stdexcept>


void ProxyImpl::registerInterface(ClassImpl* iface)
{
	std::shared_ptr<ProxyImpl> strongRef(weakThis);
	m_interfaces[iface] = std::move(iface->newInterface(strongRef));
}

void ProxyImpl::registerHandler(size_t method_hash, Proxy::MethodHandler mh)
{
	m_map[method_hash] = mh;
}

bool ProxyImpl::hasHandler(size_t method_hash) const {
	return (m_map.find(method_hash) != m_map.end());
}

VariantValue ProxyImpl::callArgArray(size_t method_hash, const ::std::vector<VariantValue>& vargs) const
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
		return it->second.createReference();
	}
	return VariantValue();
}

void ProxyImpl::incHandleCount()
{
	++m_handleCount;
}


void ProxyImpl::decHandleCount()
{
	// if there are no handles we can clear the interfaces
	// that form a circular reference to this object
	if (--m_handleCount <= 0) {
		m_interfaces.clear();
	}
}
