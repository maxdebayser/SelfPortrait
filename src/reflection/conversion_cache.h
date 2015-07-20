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

class conversion_cache {
public:

    static conversion_cache& instance();

    bool conversionKnown(const std::type_info& to, const std::type_info& from, int& ptrOffset, bool &possible) const;

    void registerConversion(const std::type_info& to, const std::type_info& from, int ptrOffset, bool possible);

private:

    struct entry {
        bool possible;
        int ptrOffset;
    };

    typedef std::unordered_map<std::type_index, std::unordered_map<std::type_index, entry>> cache_t;

    cache_t m_cache;
};

#endif /* CONVERSION_CACHE */
