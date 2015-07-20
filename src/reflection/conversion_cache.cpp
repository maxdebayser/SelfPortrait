#include "conversion_cache.h"

bool conversion_cache_base::conversionKnown(const std::type_info& from, int& ptrOffset, bool &possible) const
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

void conversion_cache_base::registerConversion(const std::type_info& from, int ptrOffset, bool possible)
{
    m_cache[from] = { possible, ptrOffset };
}
