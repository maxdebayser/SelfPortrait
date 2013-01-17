#include "conversion_cache.h"


conversion_cache::conversion_cache() {}

conversion_cache& conversion_cache::instance()
{
	static conversion_cache inst;
	return inst;
}

bool conversion_cache::conversionKnown(const std::type_info& from, const std::type_info& to, int& ptrOffset, bool &possible) const
{
	auto it1 = m_cache.find(from);
	if (it1 != m_cache.end()) {
		auto it2 = it1->second.find(to);
		if (it2 != it1->second.end()) {
			const entry& e = it2->second;
			ptrOffset = e.ptrOffset;
			possible = e.possible;
			return true;
		}
	}
	return false;
}

void conversion_cache::registerConversion(const std::type_info& from, const std::type_info& to, int ptrOffset, bool possible)
{
	m_cache[from][to] = { possible, ptrOffset };
}
