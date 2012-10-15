#ifndef PROXY_H
#define PROXY_H

#include "collection_utils.h"
#include "reflection.h"


#include <map>
#include <vector>
#include <memory>

#include <functional>

class ProxyImpl;
class ClassImpl;

// Abstract base class for call-forwarding stubs
class Interface {
	// Life cycle is bound to ProxyImpl object
	ProxyImpl* m_proxy = nullptr;
public:

	virtual ~Interface();

	virtual Interface* clone() const = 0;

	virtual VariantValue refToBase() = 0;

	void release();

	template<class... Args>
	VariantValue call(size_t method_hash, const Args&... args) const {
		::std::vector<VariantValue> vargs;
		emplace(vargs, args...);
		return callArgArray(method_hash, vargs );
	}

	VariantValue callArgArray(size_t method_hash, const ::std::vector<VariantValue>& vargs) const;

	void setProxy(ProxyImpl* pi) { m_proxy = pi; }
};

typedef std::unique_ptr<Interface> InterfacePtr;


class ProxyImpl {
public:
	~ProxyImpl();

	ProxyImpl() = default;

	// deep copy
	ProxyImpl(const ProxyImpl& that);

	void registerInterface(ClassImpl* cimpl);

	void registerHandler(size_t method_hash, Proxy::MethodHandler);

	bool hasHandler(size_t method_hash) const;

	VariantValue call(size_t method_hash, const ::std::vector<VariantValue>& vargs) const;

	VariantValue ref(ClassImpl* clazz) const;

	std::list<ClassImpl*> interfaces() const;

	void releaseMe(Interface* i);

private:
	typedef size_t hash_t;
	typedef std::map<hash_t, Proxy::MethodHandler > methmap;
	typedef std::map<ClassImpl*, InterfacePtr > ifacemap;

	methmap m_map;
	ifacemap m_interfaces;
};


#endif /* PROXY_H */

