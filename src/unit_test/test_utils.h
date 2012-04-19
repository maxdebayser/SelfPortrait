#ifndef TEST_UTILS
#define TEST_UTILS

#ifndef NO_RTTI
#define WITH_RTTI(EXPR) EXPR
#else
#define WITH_RTTI(EXPR)
#endif

#endif /* TEST_UTILS */
