/**
 *  @file   fks_assert_ex.h
 *  @brief  ex assertion macro for fks-library.
 *  @author Masashi Kitamura (tenka@6809.net)
 *  @date   2000-2011,2020
 *  @license Boost Software License Version 1.0
 *  @note
 *      #define FKS_NO_ABORT                no exit()/_CrtDbgBreak()
 *      #define FKS_ABORT_PRINTF_TO_STDERR  force output stderr
 */
#ifndef FKS_ASSERT_EX_H_INCLUDED
#define FKS_ASSERT_EX_H_INCLUDED

#include <fks/fks_common.h>

#ifdef FKS_HAS_PRAGMA_ONCE
 #pragma once
#endif

#ifdef NDEBUG
//#define FKS_ABORT_PRINTF(...)
#define FKS_ABORT_PRINTF(x)
#ifndef FKS_ASSERT
#define FKS_ASSERT(x)
#endif
#define FKS_PTR_ASSERT(p)
#define FKS_PTR0_ASSERT(p)

#define FKS_ASSERT_EQ(l,r)
#define FKS_ASSERT_NE(l,r)

#define FKS_RANGE_ASSERT(x,a,b)
#define FKS_RANGE_PTR_ASSERT(x,a,b)
#define FKS_RANGE_SINT_ASSERT(x,a,b)
#define FKS_RANGE_UINT_ASSERT(x,a,b)
#define FKS_RANGE_FLT_ASSERT(x,a,b)

#define FKS_ARG_ASSERT(n,x)
#define FKS_ARG_PTR_ASSERT(n,x)
#define FKS_ARG_PTR0_ASSERT(n,x)
#define FKS_ARG_ASSERT_EQ(n,l,r)
#define FKS_ARG_ASSERT_NE(n,l,r)
#define FKS_ARG_RANGE_ASSERT(n,x,a,b)
#define FKS_ARG_RANGE_PTR_ASSERT(n,x,a,b)
#define FKS_ARG_RANGE_SINT_ASSERT(n,x,a,b)
#define FKS_ARG_RANGE_UINT_ASSERT(n,x,a,b)
#define FKS_ARG_RANGE_FLT_ASSERT(n,x,a,b)

#else   // NDEBUG

#ifndef FKS_ABORT_PRINTF
 #ifdef FKS_WIN32
  #ifdef __cplusplus
   extern "C"
  #endif
    int fks_abort_printf(char const* fmt, ...) FKS_NOEXCEPT;
  #define FKS_ABORT_PRINTF(...)     fks_abort_printf(__VA_ARGS__)
  //#define FKS_ABORT_PRINTF(x)     fks_abort_printf x
 #else
  #include <stdio.h>
  #define FKS_ABORT_PRINTF(...)     fprintf(stderr, __VA_ARGS__)
  //#define FKS_ABORT_PRINTF(x)     printf x
 #endif
#endif

#ifdef __cplusplus
#include <string>
#include <sstream>
#if __cplusplus >= 201703L || defined(FKS_CXX17)
#include <string_view>
#endif

namespace fks { namespace assert_ex_detail {

typedef char const* charp;

template<typename T=int>
struct ArgNum {
    ArgNum(T n);
    char str[64];
};
template<typename T>
ArgNum<T>::ArgNum(T n) {
    str[0] = 0;
    if (n > 0) {
        std::snprintf(str, sizeof str, "(arg %d)", n);
    }
}

template<class DMY>
bool bool_assert(bool b, charp bstr, charp fname, int line, charp fnc, int n) {
    if (!b) {
        FKS_ABORT_PRINTF("%s (%d): %s%s: assert(%s) is failed.\n", fname, line, fnc, ArgNum<>(n).str, bstr);
    }
    return b;
}

template<class DMY>
bool ptr_assert(void const* p, charp pstr, charp fname, int line, charp fnc, int n) {
    bool rc = FKS_IS_RAM_PTR(p);
    if (!rc)
        FKS_ABORT_PRINTF("%s (%d): %s%s:[ERROR] `%s'(0x%p) is bad pointer.\n", fname,line,fnc, ArgNum<>(n).str, pstr, p);
    return rc;
}
template<class DMY, typename T>
bool ptr_assert(T* p, charp pstr, charp fname, int line, charp fnc, int n) {
    return ptr_assert<DMY>((void const*)p, pstr, fname, line, fnc, n);
}

template<class DMY>
bool ptr0_assert(void const* p, charp pstr, charp fname, int line, charp fnc, int n) {
    bool rc = FKS_IS_RAM_PTR0(p);
    if (!rc)
        FKS_ABORT_PRINTF("%s (%d): %s%s:[ERROR] `%s'(0x%p) is bad pointer.\n", fname,line,fnc, ArgNum<>(n).str, pstr, p);
    return rc;
}

template<class DMY, typename T>
bool ptr_assert0(T* p, charp pstr, charp fname, int line, charp fnc, int n) {
    return ptr_assert0<DMY>((void const*)p, pstr, fname, line, fnc, n);
}

template<class DMY, typename T>
bool assert_eq(T const& l, T const& r, charp ls, charp rs, charp fname, int line, charp fnc, int n) {
    bool rc = l == r;
    if (!rc) {
        std::stringstream ss;
        ss << fname << " (" << line << "): " << fnc << ArgNum<>(n).str
           << ":[ERROR] `" << ls << "'(" << l << ") not equal `" << rs << "'(" << r << ")\n";
        FKS_ABORT_PRINTF("%s", ss.str().c_str());
    }
    return rc;
}

template<class DMY, typename T>
bool assert_ne(T const& l, T const& r, charp ls, charp rs, charp fname, int line, charp fnc, int n) {
    bool rc = l != r;
    if (!rc) {
        std::stringstream ss;
        ss << fname << " (" << line << "): " << fnc << ArgNum<>(n).str
           << ":[ERROR] `" << ls << "'(" << l << ") equal `" << rs << "'(" << r << ")\n";
        FKS_ABORT_PRINTF("%s", ss.str().c_str());
    }
    return rc;
}

template<class DMY, typename T>
bool range_assert(T const& t, T const& mi, T const& ma, charp tstr, charp mistr, charp mastr
        , charp fname, int line, charp fnc, int n)
{
    bool rc = (mi <= t && t <= ma);
    if (!rc) {
        FKS_ABORT_PRINTF("%s (%d): %s%s:[ERROR] `%s', out of range[%s, %s].\n"
            , fname,line,fnc,ArgNum<>(n).str, tstr,mistr,mastr);
    }
    return rc;
}

template<class DMY>
bool range_assert(FKS_LLONG t, FKS_LLONG mi, FKS_LLONG ma, charp ts, charp, charp
        , charp fname, int line, charp fnc, int n)
{
    bool rc = (mi <= t && t <= ma);
    if (!rc) {
        FKS_ABORT_PRINTF("%s (%d): %s%s:[ERROR] `%s'(%lld), out of range[%lld, %lld].\n"
            ,fname,line,fnc,ArgNum<>(n).str, ts,t,mi,ma);
    }
    return rc;
}

template<class DMY>
bool range_assert(FKS_ULLONG t, FKS_ULLONG mi, FKS_ULLONG ma, charp ts, charp, charp
        , charp fname, int line, charp fnc, int n)
{
    bool rc = (mi <= t && t <= ma);
    if (!rc) {
        FKS_ABORT_PRINTF("%s (%d): %s%s:[ERROR] `%s'(%llu), out of range[%llu, %llu].\n"
            ,fname,line,fnc,ArgNum<>(n).str, ts,t,mi,ma);
    }
    return rc;
}

template<class DMY>
bool range_assert(long double t, long double mi, long double ma, charp ts, charp, charp
        , charp fname, int line, charp fnc, int n)
{
    bool rc = (mi <= t && t <= ma);
    if (!rc) {
        FKS_ABORT_PRINTF("%s (%d): %s%s:[ERROR] `%s'(%Lg), out of range[%Lg, %Lg].\n"
            ,fname,line,fnc,ArgNum<>(n).str, ts,t,mi,ma);
    }
    return rc;
}

template<class DMY>
bool range_assert(void const* p, void const* mi, void const* ma, charp ps, charp mis, charp mas
        , charp fnm, int l, charp fn, int n)
{
    bool rc = ((char const*)mi <= (char const*)p && (char const*)p <= (char const*)ma);
    if (!rc) {
        FKS_ABORT_PRINTF("%s (%d): %s%s:[ERROR] `%s'(0x%p), out of range[%s(0x%p), %s(0x%p)].\n"
            , fnm,l,fn,ArgNum<>(n).str, ps,p,mis,mi, mas, ma);
    }
    return rc;
}

template<class DMY> inline
bool range_assert(char t, char mi, char ma, charp ts, charp, charp, charp fnm, int l, charp fn, int n)
{
    return range_assert<DMY>((FKS_LLONG)t, (FKS_LLONG)mi, (FKS_LLONG)ma, ts,nullptr,nullptr, fnm,l,fn, n);
}

template<class DMY> inline
bool range_assert(int t, int mi, int ma, charp ts, charp, charp, charp fnm, int l, charp fn, int n)
{
    return range_assert<DMY>((FKS_LLONG)t, (FKS_LLONG)mi, (FKS_LLONG)ma, ts,nullptr,nullptr, fnm,l,fn, n);
}

template<class DMY> inline
bool range_assert(unsigned t,unsigned mi,unsigned ma, charp ts,charp,charp, charp fnm,int l,charp fn,int n)
{
    return range_assert<DMY>((FKS_ULLONG)t, (FKS_ULLONG)mi, (FKS_ULLONG)ma, ts,nullptr,nullptr, fnm,l,fn,n);
}

template<class DMY> inline
bool range_assert(long t, long mi, long ma, charp ts, charp, charp, charp fnm, int l, charp fn, int n)
{
    return range_assert<DMY>((FKS_LLONG)t, (FKS_LLONG)mi, (FKS_LLONG)ma, ts,nullptr,nullptr, fnm,l,fn, n);
}

template<class DMY> inline
bool range_assert(unsigned long t, unsigned long mi, unsigned long ma, charp ts, charp, charp
            , charp fnm, int l, charp fn, int n)
{
    return range_assert<DMY>((FKS_ULLONG)t, (FKS_ULLONG)mi, (FKS_ULLONG)ma, ts,nullptr,nullptr, fnm,l,fn, n);
}

template<class DMY> inline
bool range_assert(double t, double mi, double ma, charp ts, charp, charp, charp fnm, int l, charp fn, int n)
{
    return range_assert<DMY>((long double)t,(long double)mi,(long double)ma, ts,nullptr,nullptr, fnm,l,fn,n);
}

template<class DMY> inline
bool range_assert(float t, float mi, float ma, charp ts, charp, charp, charp fnm, int l, charp fn, int n)
{
    return range_assert<DMY>((long double)t,(long double)mi,(long double)ma, ts,nullptr,nullptr, fnm,l,fn,n);
}

template<class DMY, typename T> inline
bool range_assert(T const* p, T const* mi, T const* ma, charp ps, charp mis, charp mas
        , charp fnm, int l, charp fn, int n)
{
    return range_assert<DMY>((void const*)p, (void const*)mi, (void const*)ma, ps,mis,mas, fnm,l,fn, n);
}

#if 0
template<class DMY>
bool range_assert(char const* p, char const* mi, char const* ma, charp ps, charp, charp
        , charp fnm, int l, charp fn, int n)
{
    if (std::strcmp(p, mi) < 0 || std::strcmp(ma,p) < 0) {
        FKS_ABORT_PRINTF("%s (%d): %s%s: `%s'(%s), out of range[%s, %s].\n", fnm,l,fn,ArgNum<>(n).str, ps,p,mi,ma);
        return false;
    }
    return true;
}

#if __cplusplus >= 201703L || defined(FKS_CXX17)
template<class DMY>
bool range_assert(std::string_view p, std::string_view mi, std::string_view ma, charp ps, charp, charp
        , charp fnm, int l, charp fn, int n)
{
    if (p < mi || ma < p) {
        FKS_ABORT_PRINTF("%s (%d): %s%s: `%s'(%*s), out of range[%*s, %*s].\n"
            , fnm,l,fn, ArgNum<>(n).str
            , ps, int(p.size()), p.data()
            , int(mi.size()), mi.data()
            , int(ma.size()), ma.data()
        );
        return false;
    }
    return true;
}
#endif
#endif

}}  // fks::assert_ex_detail

#undef FKS_ASSERT
#define FKS_ASSERT(b)               fks::assert_ex_detail::bool_assert<void>((b), #b, __FILE__,__LINE__,__func__,0)
#define FKS_PTR_ASSERT(p)           fks::assert_ex_detail::ptr_assert<void>((p), #p, __FILE__,__LINE__,__func__,0)
#define FKS_PTR0_ASSERT(p)          fks::assert_ex_detail::ptr0_assert<void>((p), #p, __FILE__,__LINE__,__func__,0)
#define FKS_ASSERT_EQ(l,r)          fks::assert_ex_detail::assert_eq<void>(l,r, #l, #r, __FILE__,__LINE__,__func__,0)
#define FKS_ASSERT_NE(l,r)          fks::assert_ex_detail::assert_ne<void>(l,r, #l, #r, __FILE__,__LINE__,__func__,0)
#define FKS_RANGE_ASSERT(t,mi,ma)   \
    fks::assert_ex_detail::range_assert<void>((t),(mi),(ma), #t, #mi, #ma, __FILE__,__LINE__,__func__,0)

#define FKS_ARG_ASSERT(n,b)         fks::assert_ex_detail::bool_assert<void>((b),#b, __FILE__,__LINE__,__func__,(n))
#define FKS_ARG_PTR_ASSERT(n,p)     fks::assert_ex_detail::ptr_assert<void>((p), #p, __FILE__,__LINE__,__func__,(n))
#define FKS_ARG_PTR0_ASSERT(n,p)    fks::assert_ex_detail::ptr0_assert<void>((p), #p,__FILE__,__LINE__,__func__,(n))
#define FKS_ARG_ASSERT_EQ(n,l,r)    fks::assert_ex_detail::assert_eq<void>(l,r, #l, #r, __FILE__,__LINE__,__func__,(n))
#define FKS_ARG_ASSERT_NE(n,l,r)    fks::assert_ex_detail::assert_ne<void>(l,r, #l, #r, __FILE__,__LINE__,__func__,(n))
#define FKS_ARG_RANGE_ASSERT(n,t,mi,ma) \
    fks::assert_ex_detail::range_assert<void>((t),(mi),(ma), #t, #mi, #ma, __FILE__,__LINE__,__func__,(n))

#if 1 // c ver.
#define FKS_RANGE_SINT_ASSERT(t,mi,ma)          FKS_RANGE_ASSERT(t, mi, ma)
#define FKS_RANGE_UINT_ASSERT(t,mi,ma)          FKS_RANGE_ASSERT(t, mi, ma)
#define FKS_RANGE_FLT_ASSERT(t,mi,ma)           FKS_RANGE_ASSERT(t, mi, ma)
#define FKS_RANGE_PTR_ASSERT(t,mi,ma)           FKS_RANGE_ASSERT(t, mi, ma)
#define FKS_ARG_RANGE_SINT_ASSERT(n,t,mi,ma)    FKS_ARG_RANGE_ASSERT(n, t, mi, ma)
#define FKS_ARG_RANGE_UINT_ASSERT(n,t,mi,ma)    FKS_ARG_RANGE_ASSERT(n, t, mi, ma)
#define FKS_ARG_RANGE_FLT_ASSERT(n,t,mi,ma)     FKS_ARG_RANGE_ASSERT(n, t, mi, ma)
#define FKS_ARG_RANGE_PTR_ASSERT(n,t,mi,ma)     FKS_ARG_RANGE_ASSERT(n, t, mi, ma)
#endif

#else   // __cplusplus

#ifndef FKS_ASSERT
#define FKS_ASSERT(x)                                                       \
        ((x) || FKS_ABORT_PRINTF("%s (%d): %s: assert(%s) is failed.\n"     \
                ,__FILE__,__LINE__,FKS_M_FUNC,#x) )
#endif

#define FKS_PTR_ASSERT(p)                                                   \
        (FKS_IS_RAM_PTR(p)                                                  \
         || FKS_ABORT_PRINTF("%s (%d): %s: %s(0x%p) is bad pointer.\n"      \
                , __FILE__,__LINE__,FKS_M_FUNC,#p, (p) ) )

#define FKS_PTR0_ASSERT(p)                                                  \
        (!p || FKS_IS_RAM_PTR(p)                                            \
         || FKS_ABORT_PRINTF( "%s (%d): %s: %s(0x%p) is bad pointer.\n"     \
                ,__FILE__,__LINE__,FKS_M_FUNC,#p, (p) )) )

#define FKS_RANGE_ASSERT(x,a,b)                                             \
        ( ((a) <= (x) && (x) <= (b))                                        \
         || FKS_ABORT_PRINTF( "%s (%d): %s: %s, out of range[%s, %s].\n"    \
                ,__FILE__,__LINE__,FKS_M_FUNC,#x, #a, #b)) )

#define FKS_RANGE_PTR_ASSERT(x,a,b)                                         \
        ( ((FKS_UINTPTR)(a) <= (FKS_UINTPTR)(x)                             \
            && (FKS_UINTPTR)(x) <= (FKS_UINTPTR)(b))                        \
         || FKS_ABORT_PRINTF(                                               \
                "%s (%d): %s: %s (%p), out of range[%p, %p].\n"             \
                ,__FILE__,__LINE__,FKS_M_FUNC,#x, (void*)(x)                \
                , (void*)(a), (void*)(b) )) )

#define FKS_RANGE_SINT_ASSERT(x,a,b)                                        \
        (((a) <= (x) && (x) <= (b))                                         \
         || FKS_ABORT_PRINTF(                                               \
                 "%s (%d): %s: %s (%lld), out of range[%lld, %lld].\n"      \
                ,__FILE__,__LINE__,FKS_M_FUNC,#x, (FKS_LLONG)(x)            \
                , (FKS_LLONG)(a), (FKS_LLONG)(b)) )

#define FKS_RANGE_UINT_ASSERT(x,a,b)                                        \
        ( ((a) <= (x) && (x) <= (b))                                        \
         || FKS_ABORT_PRINTF(                                               \
                 "%s (%d): %s: %s (%llx), out of range[%#llx, %#llx].\n"    \
                ,__FILE__,__LINE__,FKS_M_FUNC,#x, (FKS_ULLONG)(x)           \
                , (FKS_ULLONG)(a), (FKS_ULLONG)(b) ) )

#define FKS_RANGE_FLT_ASSERT(x,a,b)                                         \
        ( ((a) <= (x) && (x) <= (b))                                        \
         || FKS_ABORT_PRINTF(                                               \
                 "%s (%d): %s: %s (%lg), out of range[%lg, %lg].\n"         \
                ,__FILE__,__LINE__,FKS_M_FUNC,#x, (long double)(x)          \
                , (long double)(a), (long double)(b) ) )


#define FKS_ARG_ASSERT(n,x)                                                 \
        ((x) || FKS_ABORT_PRINTF(                                           \
                "%s (%d): %s(arg %d): assert(%s) is failed.\n"              \
                ,__FILE__,__LINE__,FKS_M_FUNC,(n),#x ) )

#define FKS_ARG_PTR_ASSERT(n,p)                                             \
        ( FKS_IS_RAM_PTR(p)                                                 \
            || FKS_ABORT_PRINTF(                                            \
                "%s (%d): %s(arg %d): %s(0x%p) is bad pointer.\n"           \
                ,__FILE__,__LINE__,FKS_M_FUNC,(n), #p, (p) ) )

#define FKS_ARG_PTR0_ASSERT(n,p)                                            \
        (!(p) || FKS_IS_RAM_PTR(p)                                          \
         || FKS_ABORT_PRINTF(                                               \
                "%s (%d): %s(arg %d): %s(0x%p) is bad pointer.\n"           \
                ,__FILE__,__LINE__,FKS_M_FUNC,(n), #p, (p) ) )

#define FKS_ARG_RANGE_ASSERT(n,x,a,b)                                       \
        ( ((a) <= (x) && (x) <= (b))                                        \
         || FKS_ABORT_PRINTF(                                               \
                "%s (%d): %s(arg %d): %s, out of range[%s, %s].\n"          \
                ,__FILE__,__LINE__,FKS_M_FUNC,(n), #x, #a, #b ) )

#define FKS_ARG_RANGE_PTR_ASSERT(n,x,a,b)                                   \
        ( ((FKS_UINTPTR)(a) <= (FKS_UINTPTR)(x)                             \
            && (FKS_UINTPTR)(x) <= (FKS_UINTPTR)(b))                        \
         || FKS_ABORT_PRINTF(                                               \
                "%s (%d): %s: %s (%p), out of range[%p, %p].\n"             \
                ,__FILE__,__LINE__,FKS_M_FUNC,(n), #x, (void*)(x)           \
                , (void*)(a), (void*)(b) ) )

#define FKS_ARG_RANGE_SINT_ASSERT(n,x,a,b)                                  \
        ( ((a) <= (x) && (x) <= (b))                                        \
         || FKS_ABORT_PRINTF(                                               \
                 "%s (%d): %s: %s (%lld), out of range[%lld, %lld].\n"      \
                ,__FILE__,__LINE__,FKS_M_FUNC,(n),#x, (FKS_LLONG)(x)        \
                , (FKS_LLONG)(a), (FKS_LLONG)(b)) )

#define FKS_ARG_RANGE_UINT_ASSERT(n,x,a,b)                                  \
        ( ((a) <= (x) && (x) <= (b))                                        \
         || FKS_ABORT_PRINTF(                                               \
                 "%s (%d): %s: %s (%llx), out of range[%#llx, %#llx].\n"    \
                ,__FILE__,__LINE__,FKS_M_FUNC,(n),#x, (FKS_ULLONG)(x)       \
                , (FKS_ULLONG)(a), (FKS_ULLONG)(b) ) )

#define FKS_ARG_RANGE_FLT_ASSERT(n,x,a,b)                                   \
        ( ((a) <= (x) && (x) <= (b))                                        \
         || FKS_ABORT_PRINTF(                                               \
                 "%s (%d): %s: %s (%lg), out of range[%Lg, %Lg].\n"         \
                ,__FILE__,__LINE__,FKS_M_FUNC,(n),#x, (long double)(x)      \
                , (long double)(a), (long double)(b) ) )

#endif  // __cplusplus

#endif  // NDEBUG

#endif  // FKS_ASSERT_EX_H_INCLUDED
