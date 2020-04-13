/**
 *  @file   fks_misc.h
 *  @author Masashi Kitamura (tenka@6809.net)
 *  @license Boost Software Lisence Version 1.0
 */
#ifndef FKS_MISC_H_INCLUDED
#define FKS_MISC_H_INCLUDED

#include <fks/fks_common.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// ---------------------------
FKS_LIB_DECL (char const*)  fks_skipSpc(char const* s);
FKS_LIB_DECL (char const*)  fks_skipNotSpc(char const* s);

FKS_LIB_DECL (size_t)       fks_strToLF(char* buf, size_t len);

FKS_LIB_DECL (char const*)  fks_strGetLine(char line[], size_t lineSz
                                , char const* src, char const* srcEnd
                                , size_t flags FKS_ARG_INI(0x0f));

// ---------------------------
typedef int64_t fks_strExpr_val_t;
//typedef double  fks_strExpr_val_t;
FKS_LIB_DECL(int)  fks_strExpr(const char *s, const char **s_nxt, fks_strExpr_val_t *val);
FKS_LIB_DECL(void) fks_strExpr_setNameChkFunc(int (*name2valFnc)(char *name, fks_strExpr_val_t *valp));

// ---------------------------

#ifdef __cplusplus
}
#endif


#ifdef __cplusplus
namespace fks {

// ---------------------------
template<class T>
size_t binary_find_tbl_n(T& tbl, const T& key)
{
    return binary_find_tbl_n(&tbl[0], tbl.size(), key);
}

template<class T>
size_t binary_find_tbl_n(T* tbl, size_t tblSz, const T& key)
{
    size_t      low = 0;
    size_t      hi  = tblSz;
    while (low < hi) {
        size_t  mid = (low + hi - 1) / 2;
        if (key < tbl[mid]) {
            hi = mid;
        } else if (tbl[mid] < key) {
            low = mid + 1;
        } else {
            return mid;
        }
    }
    return tblSz;
}

template<class T>
size_t binary_insert_tbl_n(T* pTbl, size_t tblSz, size_t& rNum, const T& key) {
    size_t  hi  = rNum;
    size_t  low = 0;
    size_t  mid = 0;
    while (low < hi) {
        mid = (low + hi - 1) / 2;
        if (key < pTbl[mid]) {
            hi = mid;
        } else if (pTbl[mid] < key) {
            ++mid;
            low = mid;
        } else {
            return mid; // found
        }
    }
    if (rNum >= tblSz)
        return tblSz;
    // new
    ++rNum;
    for (hi = rNum; --hi > mid;)
        pTbl[hi] = pTbl[hi-1];
    pTbl[mid] = key;
    return mid;
}

template<class T>
size_t binary_insert_tbl_n(T& tbl, const T& key) {
    size_t  hi  = tbl.size();
    size_t  low = 0;
    size_t  mid = 0;
    while (low < hi) {
        mid = (low + hi - 1) / 2;
        if (key < tbl[mid]) {
            hi = mid;
        } else if (tbl[mid] < key) {
            ++mid;
            low = mid;
        } else {
            return mid; // found
        }
    }
    // new
    tbl.resize(tbl.size() + 1);
    for (hi = tbl.size(); --hi > mid;)
        tbl[hi] = tbl[hi-1];
    tbl[mid] = key;
    return mid;
}


// ---------------------------
/// line feed convertion (for vector<C> , basic_string(C))
/// @param flags : bit0:'\n' bit1:'\r' bit2='\r\n' bit3:skip '\0'
#ifndef FKS_OLD_CXX
template<class V, class C = typename V::value_type>
V& ConvLineFeed(V& v, C lf=C('\n'), size_t flags=0x0f)
#else
template<class V> V& ConvLineFeed(V& v, uint16_t lf=0x0d0a, size_t flags=0x0f)
#endif
{
 #ifdef FKS_OLD_CXX
    typedef char C;
 #endif
    size_t sz = v.size();
    if (sz == 0)
        return v;
    C*       d = &v[0];
    C const* s = d;
    C const* e = s + sz;
    unsigned c;
    do {
        c = *s++;
        if (c == C('\n') && (flags & 1)) {
            c = lf;
        } else if (c == C('\r')) {
            if (*s == C('\n') && (flags & 4)) {
                ++s;
                c = lf;
            } else if (flags & 2) {
                c = lf;
            }
        } else if (c == 0 && (flags & 8)) {
            continue;
        }
        *d++ = c;
    } while (s < e);
    sz = d - &v[0];
    if (sz < v.size())
        *d = 0;
    v.resize(sz);
    return v;
}

#ifndef FKS_OLD_CXX
template<class V, class C = typename V::value_type>
V& ConvLineFeed2(V& v, uint16_t lf=0x0d0a, size_t flags=0x0f)
#else
template<class V> V& ConvLineFeed2(V& v, uint16_t lf=0x0d0a, size_t flags=0x0f)
#endif
{
 #ifdef FKS_OLD_CXX
    typedef char C;
 #endif
    if (lf <= 0xff)
        return ConvLineFeed(v,lf,flags);
    size_t sz = v.size();
    if (sz == 0)
        return v;
    C const* s = &v[0];
    C const* e = s + sz;
    size_t n = 0;
    while (s < e) {
        C c = *s++;
        if (c == unsigned('\r') && *s == unsigned('\n'))
            ++s;
        n += (c == unsigned('\n') || c == unsigned('\r'));
    }
    sz += n;
    V dst(sz + 2);
    C lf1 = C(uint8_t(lf >> 8));
    C lf2 = C(uint8_t(lf));
    C* d = &dst[0];
    s = &v[0];
    e = s + sz;
    do {
        C c = *s++;
        if (c == C('\n') && (flags & 1)) {
            *d++ = lf1, *d++ = lf2;
        } else if (c == C('\r')) {
            if (*s == C('\n') && (flags & 4)) {
                ++s;
                *d++ = lf1, *d++ = lf2;
            } else if (flags & 2) {
                *d++ = lf1, *d++ = lf2;
            } else {
                *d++ = c;
            }
        } else if (!c && (flags & 8)) {
            continue;
        } else {
            *d++ = c;
        }
    } while (s < e);
    sz = d - &dst[0];
    dst.resize(sz);
    v.swap(dst);
    return v;
}


} // fks
#endif


#endif  // FKS_MISC_H_INCLUDED
