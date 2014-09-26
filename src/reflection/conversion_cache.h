/*
** SelfPortrait API
** See Copyright Notice in reflection.h
*/
#ifndef CONVERSION_CACHE
#define CONVERSION_CACHE

#include <typeinfo>
#include <typeindex>
#include <unordered_map>

/** This cache keeps track of the dynamic_cast that can be done */

template<class To>
class conversion_cache {
public:

	static conversion_cache& instance() {
		static conversion_cache inst;
		return inst;
	}

	bool conversionKnown(const std::type_info& from, int& ptrOffset, bool &possible) const
	{
		auto it = m_cache.find(from);
		if (it != m_cache.end()) {
			const entry& e = it->second;
			ptrOffset = e.ptrOffset;
			possible = e.possible;
			return true;
		}
		return false;
	}

	void registerConversion(const std::type_info& from, int ptrOffset, bool possible)
	{
		m_cache[from] = { possible, ptrOffset };
	}

private:

	struct entry {
		bool possible;
		int ptrOffset;
	};

	typedef std::unordered_map<std::type_index, entry> cache_t;

	cache_t m_cache;
};

#endif /* CONVERSION_CACHE */
