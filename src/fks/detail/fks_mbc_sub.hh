#ifndef FKS_MBC_SUB_H_INCLUDED
#define FKS_MBC_SUB_H_INCLUDED

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
        s += nm##_len1(s, e);									\
    }															\
    return b - str;												\
}																\
size_t  nm##_sizeToChrs(const char* str, size_t size) {			\
    const char* s = str;										\
    const char* e = s + size;									\
    size_t      l = 0;											\
    if (e < s)													\
        e = (const char*)(~(size_t)0);							\
    FKS_ASSERT(str != 0 && size > 0);							\
    while (s < e) {												\
        unsigned c;												\
        c  = nm##_getC(&s,e);									\
        if (c == 0)												\
            break;												\
        ++l;													\
    }															\
    if (s > e)													\
        --l;													\
    return l;													\
}																\
size_t  nm##_chrsToSize(const char* str, size_t chrs) {			\
    const char* s  = str;										\
    size_t      sz = 0;											\
    FKS_ASSERT(str != 0);										\
    while (chrs) {												\
        unsigned c  = nm##_getC(&s,(const char*)(~(size_t)0));	\
        if (c == 0)												\
            break;												\
        sz += nm##_chrLen(c);									\
        --chrs;													\
    }															\
    return sz;													\
}																\
int nm##_cmp(const char* lp, const char* rp) {					\
	int lc, rc;													\
    int d;														\
    FKS_ASSERT(lp != NULL);										\
    FKS_ASSERT(rp != NULL);										\
    do {														\
        lc = nm##_getC(&lp,(char const*)(~(size_t)0));			\
        rc = nm##_getC(&rp,(char const*)(~(size_t)0));			\
        d  = lc - rc;											\
    } while (d == 0 && lc);										\
    return d;													\
}																\
/**/

#ifdef __cplusplus
}
#endif

#endif
