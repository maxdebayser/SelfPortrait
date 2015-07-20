#include "conversion_cache.h"

conversion_cache& conversion_cache::instance() {
    static thread_local conversion_cache inst;
    return inst;
}

bool conversion_cache::conversionKnown(const std::type_info& to, const std::type_info& from, int& ptrOffset, bool &possible) const
{
    auto fit = m_cache.find(to);

    if (fit != m_cache.end()) {
        auto& fcache = fit->second;
        auto it = fcache.find(from);
        if (it != fcache.end()) {
            const entry& e = it->second;
            ptrOffset = e.ptrOffset;
            possible = e.possible;
            return true;
        }
    }
    return false;
}

void conversion_cache::registerConversion(const std::type_info& to, const std::type_info& from, int ptrOffset, bool possible)
{
    m_cache[to][from] = { possible, ptrOffset };
}
