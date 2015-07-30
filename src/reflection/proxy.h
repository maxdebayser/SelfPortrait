/*
** SelfPortrait API
** See Copyright Notice in reflection.h
*/
#ifndef PROXY_H
#define PROXY_H

#include "selfportrait_config.h"
#include "collection_utils.h"
#include "reflection.h"


#include <map>
#include <memory>

#include <functional>

class ProxyImpl;
class ClassImpl;


class ProxyImpl {
public:

	ProxyImpl() = default;

	ProxyImpl(const ProxyImpl& that) = delete;

	void registerInterface(ClassImpl* cimpl);

    void registerHandler(size_t method_hash, Proxy::MethodHandler);

	bool hasHandler(size_t method_hash) const;

    VariantValue callArgArray(size_t method_hash, const ArgArray& vargs) const;

	template<class... Args>
	VariantValue call(size_t method_hash, const Args&... args) const {
        ArgArray vargs;
		emplace(vargs, args...);
		return callArgArray(method_hash, vargs );
	}

	VariantValue ref(ClassImpl* clazz) const;

	std::list<ClassImpl*> interfaces() const;

	std::weak_ptr<ProxyImpl> weakThis;

	void incHandleCount();
	void decHandleCount();


private:
	typedef size_t hash_t;
	typedef std::map<hash_t, Proxy::MethodHandler > methmap;
	typedef std::map<ClassImpl*, VariantValue > ifacemap;

	methmap m_map;
	ifacemap m_interfaces;
	int m_handleCount = 0;
};


#endif /* PROXY_H */

