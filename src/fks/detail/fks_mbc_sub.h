#ifndef FKS_MBC_SUB_H_INCLUDED
#define FKS_MBC_SUB_H_INCLUDED

#include <fks/fks_assert_ex.h>

#ifdef __cplusplus
extern "C" {
#endif

static FKS_FORCE_INLINE size_t  mbc_raw_len(const char* s) {
	const char* p = s;
	--p; do {} while (*++p);
	return p - s;
}

#define MBC_IMPL(nm)											\
size_t    nm##_adjustSize(const char* str, size_t size) {		\
    const char* s = str;										\
    const char* b = s;											\
    const char* e = s + size;									\
    FKS_ASSERT(str != 0 && size > 0);							\
    if (e < s)													\
        e = (const char*)(~(size_t)0);							\
    while (s < e) {												\
        if (*s == 0)											\
            return s - str;										\
        b = s;													\
        s += nm##_len1(s);										\
    }															\
    return b - str;												\
}																\
int nm##_cmp(const char* lp, const char* rp) {					\
    int lc, rc;													\
    int d;														\
    FKS_ASSERT(lp != NULL);										\
    FKS_ASSERT(rp != NULL);										\
    do {														\
        lc = nm##_getC(&lp);									\
        rc = nm##_getC(&rp);									\
        d  = lc - rc;											\
    } while (d == 0 && lc);										\
    return d;													\
}																\
/**/

#ifdef __cplusplus
}
#endif

#endif
