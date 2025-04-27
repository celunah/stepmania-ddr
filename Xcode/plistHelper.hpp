#if defined(DEBUG)
#define PRODUCT_NAME openddr-debug
#elif defined(MINSIZEREL)
#define PRODUCT_NAME openddr-min-size
#elif defined(RELWITHDEBINFO)
#define PRODUCT_NAME openddr-release-symbols
#else
#define PRODUCT_NAME openddr
#endif

