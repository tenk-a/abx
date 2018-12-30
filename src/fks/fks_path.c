/**
 *  @file   fks_path.c
 *  @brief  ファイル名処理関係.
 *  @author Masashi Kitamura (tenka@6809.net)
 *  @license Boost Software Lisence Version 1.0
 *  @note
 *  -   c/c++ 用.
 *  -   win / linux(unix) 用.
 *      winでは\ か /が、以外は/がセパレータ.
 *  -   文字の0x80未満はascii系であること前提.
 *  -   sjis等の文字コード中の0x5c(\)や大文字小文字のことがあるため文字コード想定.
 *      win: char は dbc, wchar_t は utf16 想定.
 *          設定で utf8 対応. //FKS_PATH_UTF8 定義コンパイル時.
 *          ファイル名の大小文字同一視はascii範囲外にも及ぶので,
 *          文字コード判定や小文字化は極力 win api を用いる.
 *      他os: utf8.  FKS_PATH_DBC を定義すればsjis,big5,gbk考慮(環境変数LANG参照),
 *          FKS_PATH_ASCII を定義すれば 他バイト文字を考慮しない.
 *          wchar_t はたぶんutf32.
 *  - 比較関係は、ロケール対応不十分.
 */

#include <fks_common.h>
#include <fks_path.h>
#include <fks_assert_ex.h>
#include <fks_malloc.h>

//#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

// os の違い関係.
#if defined FKS_USE_WIN_API
 #include <windows.h>
 #ifndef FKS_NO_SHLWAPI_H
  #include <shlwapi.h>
 #endif
 #if defined _MSC_VER
  #pragma comment(lib, "User32.lib")            // CharNext()
  #pragma comment(lib, "Shlwapi.lib")           // StrCmpNI()
 #endif
#endif

#if (defined __cplusplus) && !(defined FKS_PATH_WCS_COMPILE)
extern "C" {
#endif

// fullpath 化でallocaを使う場合場合定義.
#define FKS_PATH_USE_ALLOCA

// c/c++ 対策.
#ifdef __cplusplus                                      // c++の場合、ポインタ操作のみの関数はconst版,非const版をつくる.
  #define FKS_PATH_const_CHAR       FKS_PATH_CHAR       // そのため、基本は、非const関数にする.
#else                                                   // cの場合は標準ライブラリにあわせ 引数constで戻り値 非const にする.
 #define FKS_PATH_const_CHAR        FKS_PATH_CHAR const
#endif



// ----------------------------------------------------------------------------
// macro

#define FKS_PATH_IS_DIGIT(c)        (('0') <= (c) && (c) <= ('9'))

// unicode対応. ※ mb系を使われたくないため tchar.h を使わず自前で対処.
#ifdef FKS_PATH_WCS_COMPILE // wchar_t 対応.
 #undef FKS_PATH_UTF8
 #undef FKS_PATH_DBC
 #undef FKS_PATH_ASCII
 #define FKS_PATH_C(x)              L##x
 #define FKS_PATH_CHAR              wchar_t
 #define FKS_PATH_R_STR(s,c)        wcsrchr((s),(c))        // '.'検索用.
 #define FKS_PATH_STRTOLL(s,t,r)    wcstoll((s),(t),(r))
 #define FKS_PATH_ADJUSTSIZE(p,l)   (l)
 #define FKS_STR_LEN(s)             wcslen(s)

 #ifdef FKS_USE_WIN_API
  #define FKS_PATH_CHARNEXT(p)      (FKS_PATH_CHAR*)CharNextW((FKS_PATH_CHAR*)(p))
  #define FKS_STRLWR_N(s, n)        (CharLowerBuffW((s), (n)), (s))
  #define FKS_STRUPR_N(s, n)        (CharUpperBuffW((s), (n)), (s))
  #define FKS_STRLWR(s)             CharLowerW(s)
  #define FKS_STRUPR(s)             CharUpperW(s)
  #define FKS_PATH_TO_LOWER(c)      (wchar_t)CharLowerW((wchar_t*)(c))
  #define FKS_PATH_TO_UPPER(c)      (wchar_t)CharUpperW((wchar_t*)(c))
  #define FKS_STR_N_CMP(l,r,n)      StrCmpNIW((l),(r),(n))
  //#define FKS_PATH_MATCHSPEC(a,b) PathMatchSpecW((a),(b))
 #else
  #define FKS_PATH_CHARNEXT(p)      ((p) + 1)
  #define FKS_PATH_TO_LOWER(c)      (((c) >= FKS_PATH_C('A') && (c) <= FKS_PATH_C('Z')) ? (c) - FKS_PATH_C('A') + FKS_PATH_C('a') : (c))
  #define FKS_PATH_TO_UPPER(c)      (((c) >= FKS_PATH_C('a') && (c) <= FKS_PATH_C('z')) ? (c) - FKS_PATH_C('a') + FKS_PATH_C('A') : (c))
  #if defined FKS_PATH_IGNORECASE   // 大小文字同一視.
   #define FKS_STR_N_CMP(l,r,n)     wcsncasecmp((l),(r),(n))
  #else                             // 大小区別.
   #define FKS_STR_N_CMP(l,r,n)     wcsncmp((l),(r),(n))
  #endif
 #endif
 #if defined FKS_PATH_IGNORECASE
  #define FKS_PATH_GET_C(c, p)      ((c) = *((p)++), (c) = FKS_PATH_TO_LOWER(c))
 #else
  #define FKS_PATH_GET_C(c, p)      ((c) = *((p)++))
 #endif
#else           // char ベース.
 #define FKS_PATH_C(x)              x
 #define FKS_PATH_CHAR              char
 #define FKS_PATH_R_STR(s,c)        strrchr((s),(c))        // '.'検索要 .
 #define FKS_PATH_STRTOLL(s,t,r)    strtoll((s),(t),(r))
 #define FKS_STR_LEN(s)             strlen(s)

 #if defined FKS_PATH_UTF8 && defined FKS_WIN32
  #define FKS_PATH_ISMBBLEAD(c)     (fks_pathIsUtf8() ? ((unsigned)(c) >= 0x80) : IsDBCSLeadByte(c))
  #define FKS_PATH_TO_LOWER(c)      ((c < 0x10000) ? (fks_pathIsUtf8() ? (wchar_t)CharLowerW((wchar_t*)(uint16_t)(c)) : (((c) >= FKS_PATH_C('A') && (c) <= FKS_PATH_C('Z')) ? (c) - FKS_PATH_C('A') + FKS_PATH_C('a') : (c))) : (c))
  #define FKS_PATH_TO_UPPER(c)      ((c < 0x10000) ? (fks_pathIsUtf8() ? (wchar_t)CharUpperW((wchar_t*)(uint16_t)(c)) : (((c) >= FKS_PATH_C('a') && (c) <= FKS_PATH_C('z')) ? (c) - FKS_PATH_C('a') + FKS_PATH_C('A') : (c))) : (c))
  #define FKS_PATH_CHARNEXT(p)      (fks_pathIsUtf8() ? (FKS_PATH_CHAR*)fks_pathUtf8CharNext(p) : (FKS_PATH_CHAR*)CharNextA((FKS_PATH_CHAR*)(p)))
  #define FKS_PATH_ADJUSTSIZE(p,l)  fks_pathAdjustSize(p,l)
 #elif defined FKS_PATH_UTF8
  #define FKS_PATH_ISMBBLEAD(c)     ((unsigned)(c) >= 0x80)
  #if defined FKS_WIN32
   #define FKS_PATH_TO_LOWER(c)     (((c) < 0x10000) ? (wchar_t)CharLowerW((wchar_t*)(uint16_t)(c)) : (c))
   #define FKS_PATH_TO_UPPER(c)     (((c) < 0x10000) ? (wchar_t)CharUpperW((wchar_t*)(uint16_t)(c)) : (c))
  #else
   #define FKS_PATH_TO_LOWER(c)     (((c) >= FKS_PATH_C('A') && (c) <= FKS_PATH_C('Z')) ? (c) - FKS_PATH_C('A') + FKS_PATH_C('a') : (c))
   #define FKS_PATH_TO_UPPER(c)     (((c) >= FKS_PATH_C('a') && (c) <= FKS_PATH_C('z')) ? (c) - FKS_PATH_C('a') + FKS_PATH_C('A') : (c))
  #endif
  #define FKS_PATH_CHARNEXT(p)      (char*)fks_pathUtf8CharNext((char*)(p))
  #define FKS_PATH_ADJUSTSIZE(p,l)  fks_pathAdjustSize(p,l)
  #if defined FKS_PATH_IGNORECASE
   #define FKS_PATH_GET_C(c, p)     (((c) = fks_pathUtf8GetC((char const**)&(p))), (c) = FKS_PATH_TO_LOWER(c))
  #else
   #define FKS_PATH_GET_C(c, p)     ((c) = fks_pathUtf8GetC((char const**)&(p)))
  #endif
 #elif defined FKS_WIN32
  #define FKS_PATH_ISMBBLEAD(c)     IsDBCSLeadByte(c)
  #define FKS_STR_N_CMP(l,r,n)      StrCmpNIA((l),(r),(n))
  #define FKS_STRLWR_N(s,n)         (CharLowerBuffA((s),(n)), (s))
  #define FKS_STRUPR_N(s,n)         (CharUpperBuffA((s),(n)), (s))
  #define FKS_STRLWR(s)             CharLowerA(s)
  #define FKS_STRUPR(s)             CharUpperA(s)
  #define FKS_PATH_CHARNEXT(p)      (char*)CharNextA((char*)(p))
  #define FKS_PATH_ADJUSTSIZE(p,l)  fks_pathAdjustSize(p,l)
  #define FKS_PATH_TO_LOWER(c)      (((c) >= FKS_PATH_C('A') && (c) <= FKS_PATH_C('Z')) ? (c) - FKS_PATH_C('A') + FKS_PATH_C('a') : (c))
  #define FKS_PATH_TO_UPPER(c)      (((c) >= FKS_PATH_C('a') && (c) <= FKS_PATH_C('z')) ? (c) - FKS_PATH_C('a') + FKS_PATH_C('A') : (c))
  //#define FKS_PATH_MATCHSPEC(a,b) PathMatchSpecA((a),(b))
 #else
  #if defined FKS_USE_FNAME_MBC
   #define FKS_PATH_ISMBBLEAD(c)    ((unsigned)(c) >= 0x80 && fks_pathIsZenkaku1(c) > 0)
   #define FKS_PATH_CHARNEXT(p)     ((p) + 1 + (FKS_PATH_ISMBBLEAD(*(unsigned char*)(p)) && (p)[1]))
   #define FKS_PATH_ADJUSTSIZE(p,l) fks_pathAdjustSize(p,l)
  #else
   #define FKS_PATH_ISMBBLEAD(c)    (0)
   #define FKS_PATH_CHARNEXT(p)     ((p) + 1)
  #define FKS_PATH_ADJUSTSIZE(p,l)  (l)
  #endif
  #if defined FKS_PATH_IGNORECASE   // 大小文字同一視.
   #define FKS_STR_N_CMP(l,r,n)     strncasecmp((l),(r),(n))
  #else                             // 大小区別.
   #define FKS_STR_N_CMP(l,r,n)     strncmp((l),(r),(n))
  #endif
  #define FKS_PATH_TO_LOWER(c)      (((c) >= FKS_PATH_C('A') && (c) <= FKS_PATH_C('Z')) ? (c) - FKS_PATH_C('A') + FKS_PATH_C('a') : (c))
  #define FKS_PATH_TO_UPPER(c)      (((c) >= FKS_PATH_C('a') && (c) <= FKS_PATH_C('z')) ? (c) - FKS_PATH_C('a') + FKS_PATH_C('A') : (c))
 #endif
 #if !defined FKS_PATH_GET_C
  #if defined FKS_PATH_UTF8 && defined FKS_WIN32
   #if defined FKS_PATH_IGNORECASE
    #define FKS_PATH_GET_C(c, p) do {                               \
        if (fks_pathIsUtf8()) {                                     \
            (c) = fks_pathUtf8GetC((char const**)&(p));             \
            if ((c) < 0x10000)                                      \
                (c) = (wchar_t)CharLowerW((wchar_t*)(uint16_t)(c)); \
        } else {                                                    \
            (c) = *(unsigned char*)((p)++);                         \
            if (IsDBCSLeadByte(c) && *(p))                          \
                (c) = ((c) << 8) | *(unsigned char*)((p)++);        \
            else                                                    \
                (c) = (((c) >= FKS_PATH_C('A') && (c) <= FKS_PATH_C('Z')) ? ((c) - FKS_PATH_C('A') + FKS_PATH_C('a')) : (c));   \
        }                                                           \
    } while (0)
   #else
    #define FKS_PATH_GET_C(c, p) do {                               \
        if (fks_pathIsUtf8()) {                                     \
            (c) = fks_pathUtf8GetC((char const**)&(p));             \
        } else {                                                    \
            (c) = *(unsigned char*)((p)++);                         \
            if (IsDBCSLeadByte(c) && *(p))                          \
                (c) = ((c) << 8) | *(unsigned char*)((p)++);        \
        }                                                           \
    } while (0)
   #endif
  #elif defined FKS_USE_FNAME_MBC
   #if !defined FKS_PATH_IGNORECASE
    #define FKS_PATH_GET_C(c, p) do {                       \
        (c) = *(unsigned char*)((p)++);                     \
        if (FKS_PATH_ISMBBLEAD(c) && *(p))                  \
            (c) = ((c) << 8) | *(unsigned char*)((p)++);    \
        else                                                \
            (c) = FKS_PATH_TO_LOWER(c);                     \
    } while (0)
   #else
    #define FKS_PATH_GET_C(c, p) do {                       \
        (c) = *(unsigned char*)((p)++);                     \
        if (FKS_PATH_ISMBBLEAD(c) && *(p))                  \
            (c) = ((c) << 8) | *(unsigned char*)((p)++);    \
    } while (0)
   #endif
  #else
   #if defined FKS_PATH_IGNORECASE
    #define FKS_PATH_GET_C(c, p)    ((c) = *((p)++), (c) = FKS_PATH_TO_LOWER(c))
   #else
    #define FKS_PATH_GET_C(c, p)    ((c) = *((p)++))
   #endif
  #endif
 #endif
#endif



// ----------------------------------------------------------------------------
// UTF-8

#if defined FKS_PATH_UTF8

#if defined FKS_WIN32
int _fks_priv_pathUtf8Flag = 0;
#endif

/** 1字取り出し＆ポインタ更新.
 */
static uint32_t fks_pathUtf8GetC(char const** pStr) FKS_NOEXCEPT {
    const unsigned char* s = (unsigned char*)*pStr;
    unsigned       c       = *s++;

    if (c < 0x80) {
        ;
    } else if (*s) {
        int c2 = *s++;
        c2 &= 0x3F;
        if (c < 0xE0) {
            c = ((c & 0x1F) << 6) | c2;
        } else if (*s) {
            int c3 = *s++;
            c3 &= 0x3F;
            if (c < 0xF0) {
                c = ((c & 0xF) << 12) | (c2 << 6) | c3;
            } else if (*s) {
                int c4 = *s++;
                c4 &= 0x3F;
                if (c < 0xF8) {
                    c = ((c&7)<<18) | (c2<<12) | (c3<<6) | c4;
                } else if (*s) {
                    int c5 = *s++;
                    c5 &= 0x3F;
                    if (c < 0xFC) {
                        c = ((c&3)<<24) | (c2<<18) | (c3<<12) | (c4<<6) | c5;
                    } else if (*s) {
                        int c6 = *s++;
                        c6 &= 0x3F;
                        c = ((c&1)<<30) |(c2<<24) | (c3<<18) | (c4<<12) | (c5<<6) | c6;
                    }
                }
            }
        }
    }

    *pStr = (char*)s;
    return c;
}


static char const* fks_pathUtf8CharNext(char const* pChr) {
    const unsigned char* s = (unsigned char*)pChr;
    unsigned       c = *s;
    if (!c)
        return (char const*)s;
    ++s;
    if (c < 0x80) {
        return (char const*)s;
    } else if (*s) {
        ++s;
        if (c < 0xE0) {
            return (char const*)s;
        } else if (*s) {
            ++s;
            if (c < 0xF0) {
                return (char const*)s;
            } else if (*s) {
                ++s;
                if (c < 0xF8) {
                    return (char const*)s;
                } else if (*s) {
                    ++s;
                    if (c < 0xFC) {
                        return (char const*)s;
                    } else if (*s) {
                        ++s;
                        return (char const*)s;
                    }
                }
            }
        }
    }
    return (char const*)s;
}

#endif


// ----------------------------------------------------------------------------
// win 以外で、Double Byte Charに対応する場合の処理.(簡易版)

#if defined FKS_PATH_DBC && !defined FKS_WIN32

static int    s_fks_path_shift_char_mode =  0;

/** とりあえず、0x5c関係の対処用.
 */
FKS_STATIC_DECL(int)    fks_pathMbcInit(void) FKS_NOEXCEPT
{
    const char*     lang = getenv("LANG");
    const char*     p;
    if (lang == 0)
        return -1;
    //s_fks_path_locale_ctype = strdup(lang);
    // ja_JP.SJIS のような形式であることを前提にSJIS,big5,gbkかをチェック.
    p = strrchr(lang, '.');
    if (p) {
        const char* enc = p + 1;
        // 0x5c対策が必要なencodingかをチェック.
        if (strncasecmp(enc, "sjis", 4) == 0) {
            return 1;
        } else if (strncasecmp(enc, "gbk", 3) == 0 || strncasecmp(enc, "gb2312", 6) == 0) {
            return 2;
        } else if (strncasecmp(enc, "big5", 4) == 0) {
            return 3;
        }
    }
    return -1;
}

FKS_STATIC_DECL(int)    fks_pathIsZenkaku1(unsigned c) FKS_NOEXCEPT
{
    if (s_fks_path_shift_char_mode  == 0)
        s_fks_path_shift_char_mode  = fks_pathMbcInit();
    switch (s_fks_path_shift_char_mode) {
    case 1 /* sjis */: return ((c >= 0x81 && c <= 0x9F) || (c >= 0xE0 && c <= 0xFC));
    case 2 /* GBK  */: return  (c >= 0x81 && c <= 0xFE);
    case 3 /* BIG5 */: return ((c >= 0xA1 && c <= 0xC6) || (c >= 0xC9 && c <= 0xF9));
    default:           return -1;
    }
}
#endif


// ----------------------------------------------------------------------------


FKS_LIB_DECL (FKS_PATH_SIZE)
fks_pathLen(FKS_PATH_CHAR const* path) FKS_NOEXCEPT
{
    return FKS_STR_LEN(path);
}


/** ファイルパス名中のディレクトリを除いたファイル名の位置を返す.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathBaseName(FKS_PATH_const_CHAR *adr) FKS_NOEXCEPT
{
    FKS_PATH_CHAR const *p = adr;
    FKS_ARG_PTR_ASSERT(1, adr);
    while (*p) {
        if (*p == FKS_PATH_C(':') || fks_pathIsSep(*p))
            adr = (FKS_PATH_CHAR*)p + 1;
        p = FKS_PATH_CHARNEXT(p);
    }
    return (FKS_PATH_CHAR*)adr;
}

/** ファイル名を削除(ディレクトリセパレータは残る)
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathDelBaseName(FKS_PATH_CHAR path[]) FKS_NOEXCEPT
{
    FKS_PATH_CHAR* p = fks_pathBaseName(path);
    *p = 0;
    return path;
}

/** sizeに収まる文字列の文字数を返す. \0を含まない.
 *  (win環境ではなるべくマルチバイト文字の途中で終わらないようにする.
 *   けど、用途的には切れる以上あまり意味ない...)
 */
FKS_LIB_DECL (FKS_PATH_SIZE)
fks_pathAdjustSize(FKS_PATH_CHAR const* str, FKS_PATH_SIZE size) FKS_NOEXCEPT
{
 #if defined FKS_PATH_UTF8 || defined FKS_PATH_DBC
    FKS_PATH_CHAR const* s = str;
    FKS_PATH_CHAR const* b = s;
    FKS_PATH_CHAR const* e = s + size;
    FKS_ARG_PTR_ASSERT(1, str);
    FKS_ARG_ASSERT(1, (size > 0));
    while (s < e) {
        if (*s == 0)
            return s - str;
        b = s;
        s = FKS_PATH_CHARNEXT((FKS_PATH_CHAR*)s);
    }
    if (s > e)
        s = b;
    return s - str;
 #else
    return size;
 #endif
}


/** ファイル名のコピー. mbcの時は文字が壊れない部分まで. dst == src もok.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathCpy(FKS_PATH_CHAR dst[], FKS_PATH_SIZE dstSz, FKS_PATH_CHAR const* src) FKS_NOEXCEPT
{
    FKS_PATH_SIZE   l;
    FKS_ARG_PTR_ASSERT(1, dst);
    FKS_ARG_ASSERT(2, (dstSz > 0));
    FKS_ARG_PTR0_ASSERT(3, src);

    if (src == NULL)
        return NULL;
    l = FKS_PATH_ADJUSTSIZE(src, dstSz);

    // アドレスが同じなら、長さをあわせるのみ.
    if (dst == src) {
        dst[l] = 0;
        return dst;
    }

    // コピー.
    {
        FKS_PATH_CHAR const*    s = src;
        FKS_PATH_CHAR const*    e = s + l;
        FKS_PATH_CHAR*      d = dst;
        while (s < e)
            *d++ = *s++;
        *d = 0;
    }

    return dst;
}


/** ファイル名文字列の連結.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathCat(FKS_PATH_CHAR dst[], FKS_PATH_SIZE dstSz, FKS_PATH_CHAR const* src) FKS_NOEXCEPT
{
    FKS_PATH_SIZE l;
    FKS_ARG_PTR_ASSERT(1, dst);
    FKS_ARG_ASSERT(2, (dstSz > 0));
    FKS_ARG_PTR0_ASSERT(3, src);
    if (src == 0)
        return NULL;
    FKS_ASSERT(src != 0 && dst != src);
    l = fks_pathLen(dst);
    if (l >= dstSz)     // そもそも転送先が満杯ならサイズ調整のみ.
        return fks_pathCpy(dst, dstSz, dst);
    fks_pathCpy(dst+l, dstSz - l,   src);
    return dst;
}


/** ディレクトリ名とファイル名をくっつける. fks_pathCat と違い、\   / を間に付加.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathCombine(FKS_PATH_CHAR buf[], FKS_PATH_SIZE bufSz, FKS_PATH_CHAR const *dir, FKS_PATH_CHAR const *name) FKS_NOEXCEPT
{
    fks_pathCpy(buf, bufSz, dir);
    if (buf[0])
        fks_pathAddSep(buf, bufSz);
    fks_pathCat(buf, bufSz, name);
    return buf;
}


/** ファイル名の大小比較.
 *  win/dos系は大小同一視. ディレクトリセパレータ \ / も同一視.
 *  以外は単純に文字列比較.
 */
FKS_LIB_DECL (int)
fks_pathCmp(FKS_PATH_CHAR const* l, FKS_PATH_CHAR const* r) FKS_NOEXCEPT
{
    return fks_pathNCmp(l, r, (FKS_PATH_SIZE)-1);
}


/** ファイル名の大小比較.
 *  win/dos系は大小同一視. ディレクトリセパレータ \ / も同一視.
 *   * UNICODE系では ascii 以外の大文字小文字の対応できるが,
 *     他のmbcはasciiのみで実際のファイルシステムとは異なる結果になるので注意.
 *  他環境は単純に文字列比較.(macは未考慮)
 */
FKS_LIB_DECL (int)
fks_pathNCmp(FKS_PATH_CHAR const* l, FKS_PATH_CHAR const* r, FKS_PATH_SIZE len) FKS_NOEXCEPT
{
 #ifdef FKS_STR_N_CMP
  #if 1
    FKS_ASSERT( l != 0 && r != 0 );
    return FKS_STR_N_CMP(l, r, len);
  #else
    int i;
    FKS_ASSERT( l != 0 && r != 0 );
    char*   orig = setlocale(LC_CTYPE, s_fks_path_locale_ctype);
    i = FKS_PATH_CMP(l, r);
    setlocale(orig);
    return i;
  #endif
 #else
    FKS_PATH_CHAR const* e = l + len;
    FKS_ASSERT( l != 0 && r != 0 );
    if (e < l)
        e = (FKS_PATH_CHAR const*)-1;
    while (l < e) {
        int      n;
        unsigned lc;
        unsigned rc;

        FKS_PATH_GET_C(lc, l);
        FKS_PATH_GET_C(rc, r);

        n  = (int)(lc - rc);
        if (n == 0) {
            if (lc == 0)
                return 0;
            continue;
        }
     #if defined FKS_PATH_WINDOS
        if ((lc == FKS_PATH_C('/') && rc == FKS_PATH_C('\\')) || (lc == FKS_PATH_C('\\') && rc == FKS_PATH_C('/')))
            continue;
     #endif
        return n;
    }
    return 0;
 #endif
}


FKS_STATIC_DECL (int) fks_pathNDigitCmp(FKS_PATH_CHAR const* l, FKS_PATH_CHAR const* r, FKS_PATH_SIZE len) FKS_NOEXCEPT;

/** ファイル名の大小比較. 数値があった場合、桁数違いの数値同士の大小を反映.
 *  win/dos系は大小同一視. ディレクトリセパレータ \ / も同一視.
 *  以外は単純に文字列比較.
 */
FKS_LIB_DECL(int)
fks_pathDigitCmp(FKS_PATH_CHAR const* l, FKS_PATH_CHAR const* r) FKS_NOEXCEPT
{
    return fks_pathNDigitCmp(l, r, (FKS_PATH_SIZE)-1);
}

/** ※ len より長い文字列で、len文字目が 数値列の途中だった場合、lenを超えてstrtolしてしまうため,
 *     意図した結果にならない場合がある。ので、fnameNDigitCmpは公開せずサブルーチンとする.
 */
FKS_STATIC_DECL (int)
fks_pathNDigitCmp(FKS_PATH_CHAR const* lhs, FKS_PATH_CHAR const* rhs, FKS_PATH_SIZE len) FKS_NOEXCEPT
{
    FKS_PATH_CHAR const* l = lhs;
    FKS_PATH_CHAR const* r = rhs;
    FKS_PATH_CHAR const* e = l + len;
    FKS_ASSERT( l != 0 && r != 0 );
    //if (strstr(lhs,"(3)") != 0) {
    //static int ss; ++ss;
    //}
    if (e < l)
        e = (FKS_PATH_CHAR const*)-1;
    while (l < e) {
        unsigned    lc;
        unsigned    rc;

        FKS_PATH_GET_C(lc, l);
        FKS_PATH_GET_C(rc, r);

        if (lc <= 0x80 && FKS_PATH_IS_DIGIT(lc) && rc <= 0x80 && FKS_PATH_IS_DIGIT(rc)) {
            FKS_LLONG   lv = FKS_PATH_STRTOLL(l - 1, (FKS_PATH_CHAR**)&l, 10);
            FKS_LLONG   rv = FKS_PATH_STRTOLL(r - 1, (FKS_PATH_CHAR**)&r, 10);
            FKS_LLONG   d  = lv - rv;
            if (d == 0)
                continue;
            return (d < 0) ? -1 : 1;
        }

     #ifdef FKS_PATH_IGNORECASE
        lc = FKS_PATH_TO_LOWER(lc);
        rc = FKS_PATH_TO_LOWER(rc);
     #endif

        if (lc == rc) {
            if (lc == 0)
                break; //return 0;
            continue;
        }

      #ifdef FKS_PATH_WINDOS
        if ((lc == FKS_PATH_C('/') && rc == FKS_PATH_C('\\')) || (lc == FKS_PATH_C('\\') && rc == FKS_PATH_C('/')))
            continue;
      #endif
        return (lc < rc) ? -1 : 1;
    }
    // 数値的に同一でも "0"の個数が違うこともあるので、文字列として比較.
    return fks_pathNCmp(lhs, rhs, len);
}



/** fnameがprefixで始まっていれば、fnameの余分の先頭のアドレスを返す.
 *  マッチしていなければNULLを返す.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathStartsWith(FKS_PATH_const_CHAR* fname, FKS_PATH_CHAR const* prefix) FKS_NOEXCEPT
{
    FKS_PATH_SIZE l;
    FKS_ASSERT(fname && prefix);
    l = fks_pathLen(prefix);
    if (l == 0)
        return (FKS_PATH_CHAR*)fname;
    return (fks_pathNCmp(fname, prefix, l) == 0) ? (FKS_PATH_CHAR*)fname+l : 0;
}


/** 拡張子の位置を返す. '.'は含む. なければ文字列の最後を返す.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathExt(FKS_PATH_const_CHAR* name) FKS_NOEXCEPT
{
    FKS_PATH_CHAR const *p;
    FKS_ARG_PTR_ASSERT(1, name);
    name = fks_pathBaseName(name);
    p    = FKS_PATH_R_STR(name, FKS_PATH_C('.'));
    if (/*p &&*/ p > name)  // 先頭のみ . の場合は拡張子ではない.
        return (FKS_PATH_CHAR*)(p);

    return (FKS_PATH_CHAR*)name + fks_pathLen(name);
}


/** ファイルパス名中の拡張子を削除する.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathDelExt(FKS_PATH_CHAR buf[]) FKS_NOEXCEPT
{
    FKS_PATH_CHAR *t;
    FKS_PATH_CHAR *p;
    FKS_ARG_PTR_ASSERT(1, buf);
    t = fks_pathBaseName(buf);
    p = FKS_PATH_R_STR(t, FKS_PATH_C('.'));
    if (p == 0)
        p = t + fks_pathLen(t);
    *p = 0;
    return buf;
}


/** ファイルパス名中の拡張子を除いた部分の取得.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathGetNoExt(FKS_PATH_CHAR dst[], FKS_PATH_SIZE size, FKS_PATH_CHAR const *src) FKS_NOEXCEPT
{
    FKS_PATH_CHAR const *s;
    FKS_PATH_CHAR const *e;
    FKS_PATH_SIZE         l = 0;
    FKS_ARG_PTR_ASSERT(1, dst);
    FKS_RANGE_UINTPTR_ASSERT(2, 2, (FKS_PATH_SIZE)-1);
    FKS_ARG_PTR_ASSERT(3, src);
    //if (dst == 0 || size == 0 || src == 0) return 0;
    s = fks_pathBaseName(src);
    e = FKS_PATH_R_STR(s, FKS_PATH_C('.'));
    if (e == 0)
        e = s + fks_pathLen(s);
    //l = e - src + 1;
    l = e - src;
    if (l > size)
        l = size;
    fks_pathCpy(dst, l, src);
    return dst;
}


/** 拡張子を、ext に変更する. dst == src でもよい.
 *  ext = NULL or "" のときは 拡張子削除.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathSetExt(FKS_PATH_CHAR dst[], FKS_PATH_SIZE size, FKS_PATH_CHAR const* src, FKS_PATH_CHAR const *ext) FKS_NOEXCEPT
{
    FKS_ASSERT(dst != 0 && size > 0 && src != 0);
    fks_pathGetNoExt(dst,   size, src);
    if (ext && ext[0]) {
        if (ext[0] != FKS_PATH_C('.'))
            fks_pathCat(dst, size, FKS_PATH_C("."));
        fks_pathCat(dst, size, ext);
    }
    return dst;
}


/** 拡張子がない場合、拡張子を追加する.(あれば何もしない). dst == src でもよい.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathSetDefaultExt(FKS_PATH_CHAR dst[], FKS_PATH_SIZE size, FKS_PATH_CHAR const* src, FKS_PATH_CHAR const *ext) FKS_NOEXCEPT
{
    FKS_PATH_CHAR* p;
    FKS_ASSERT(dst != 0 && size > 0 && src != 0);

    fks_pathCpy(dst, size, src);
    if (ext == 0)
        return dst;
    p = fks_pathBaseName(dst);
    p = FKS_PATH_R_STR(p, FKS_PATH_C('.'));
    if (p) {
        if (p[1])
            return dst;
        *p = 0;
    }
    if (ext[0]) {
        if (ext[0] != FKS_PATH_C('.'))
            fks_pathCat(dst, size, FKS_PATH_C("."));
        fks_pathCat(dst, size, ext);
    }
    return dst;
}

/** '.'を含まない拡張子を取得.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathGetNoDotExt(FKS_PATH_CHAR ext[], FKS_PATH_SIZE sz, FKS_PATH_CHAR const* src) FKS_NOEXCEPT
{
    FKS_PATH_CHAR const* e = fks_pathExt(src);
    if (e && *e == '.')
        ++e;
    return fks_pathCpy(ext, sz, e);
}


/** 文字列の最後に ディレクトリセパレータ文字がなければ追加.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathAddSep(FKS_PATH_CHAR dir[], FKS_PATH_SIZE size) FKS_NOEXCEPT
{
    FKS_PATH_CHAR* e = dir + size;
    FKS_PATH_CHAR* p = fks_pathCheckLastSep(dir);
    if (p == 0) {
        p = dir + fks_pathLen(dir);
        if (p+1 < e) {
            *p++ = FKS_PATH_SEP_CHR;
            *p = 0;
        }
    }
    return dir;
}


/** 文字列の最後に \ か / があれば削除.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathDelLastSep(FKS_PATH_CHAR dir[]) FKS_NOEXCEPT
{
    FKS_PATH_CHAR* p = fks_pathSkipRootCheckLastSep(dir);
    if (p)
        *p = 0;
    return dir;
}


/** 文字列の最後に \ か / があればその位置を返し、なければNULLを返す.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathSkipRootCheckLastSep(FKS_PATH_const_CHAR* dir) FKS_NOEXCEPT
{
    FKS_PATH_CHAR* p = fks_pathSkipDriveRoot(dir);
    FKS_PATH_SIZE  l = fks_pathLen(p);
    if (l == 0) return 0;
    return fks_pathCheckPosSep(p, (ptrdiff_t)l - 1);
}


/** 文字列の最後に \ か / があればその位置を返し、なければNULLを返す.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathCheckLastSep(FKS_PATH_const_CHAR* dir) FKS_NOEXCEPT
{
    FKS_PATH_SIZE l = fks_pathLen(dir);
    if (l == 0) return 0;
    return fks_pathCheckPosSep(dir, (ptrdiff_t)l - 1);
}


/** 文字列の指定位置に \ か / があればその位置を返し、なければNULLを返す.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathCheckPosSep(FKS_PATH_const_CHAR* dir, ptrdiff_t ofs) FKS_NOEXCEPT
{
    FKS_ASSERT(dir != 0);
    if (dir) {
        FKS_PATH_CHAR const*        s   = (FKS_PATH_CHAR*)dir;
        if (ofs >= 0) {
            FKS_PATH_CHAR const*    p   = s + ofs;
            if (*p == FKS_PATH_C('/'))
                return (FKS_PATH_CHAR *)p;
          #if (defined FKS_PATH_WINDOS)
            else if (*p == FKS_PATH_C('\\')) {
              #ifdef FKS_PATH_WCS_COMPILE
                return (FKS_PATH_CHAR *)p;
              #else     // adjust_sizeの結果がofs未満になってたら*pはマルチバイト文字の一部.
                if (FKS_PATH_ADJUSTSIZE(dir, ofs) == (FKS_PATH_SIZE)ofs)
                    return (FKS_PATH_CHAR *)p;
              #endif
            }
          #endif
        }
    }
    return NULL;
}


/** ドライブ名がついているか.
 */
FKS_LIB_DECL (int)
fks_pathHasDrive(FKS_PATH_CHAR const* path) FKS_NOEXCEPT
{
    // 先頭の"文字列:"をドライブ名とみなす.
    FKS_PATH_CHAR const* s = path;
    if (s == 0)
        return 0;
    if (*s && *s != FKS_PATH_C(':')) {
        while (*s && !fks_pathIsSep(*s)) {
            if (*s == FKS_PATH_C(':'))
                return 1;
            ++s;
        }
    }
    return 0;
}


/** ドライブ名があればそれをスキップしたポインタを返す.
 *   ※ c:等だけでなくhttp:もスキップするため "文字列:" をスキップ.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathSkipDrive(FKS_PATH_const_CHAR* path) FKS_NOEXCEPT
{
    FKS_PATH_CHAR const* s = path;
    if (*s && *s != FKS_PATH_C(':')) {
        while (*s && !fks_pathIsSep(*s)) {
            if (*s == FKS_PATH_C(':'))
                return (FKS_PATH_CHAR*)s+1;
            ++s;
        }
    }
    return (FKS_PATH_CHAR*)path;
}


/** 絶対パスか否か(ドライブ名の有無は関係なし)
 */
FKS_LIB_DECL (int)
fks_pathIsAbs(FKS_PATH_CHAR const* path) FKS_NOEXCEPT
{
    if (path == 0)
        return 0;
    path = fks_pathSkipDrive(path);
    return fks_pathIsSep(path[0]);
}


/** 文字コードを考慮した  strlwr.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathToLowerN(FKS_PATH_CHAR name[], size_t n) FKS_NOEXCEPT
{
 #ifdef FKS_STRLWR_N
    return FKS_STRLWR_N(name, n);
 #else
    FKS_PATH_CHAR *p = name;
    FKS_PATH_CHAR *e = p + n;
    FKS_ASSERT(name != NULL);

    while (p < e) {
        unsigned c = *p;
        *p = FKS_PATH_TO_LOWER(c);
        p  = FKS_PATH_CHARNEXT(p);
    }
    return name;
 #endif
}

FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathToLower(FKS_PATH_CHAR name[]) FKS_NOEXCEPT
{
 #ifdef FKS_STRLWR
    return FKS_STRLWR(name);
 #else
    return fks_pathToLowerN(name, fks_pathLen(name));
 #endif
}


/** 文字コードを考慮した  strupr.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathToUpperN(FKS_PATH_CHAR name[], size_t n) FKS_NOEXCEPT
{
 #ifdef FKS_STRUPR_N
    return FKS_STRUPR_N(name, n);
 #else
    FKS_PATH_CHAR *p = name;
    FKS_PATH_CHAR *e = p + n;
    FKS_ASSERT(name != NULL);

    while (p < e) {
        unsigned c = *p;
        *p = FKS_PATH_TO_UPPER(c);
        p  = FKS_PATH_CHARNEXT(p);
    }
    return name;
 #endif
}

FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathToUpper(FKS_PATH_CHAR name[]) FKS_NOEXCEPT
{
 #ifdef FKS_STRUPR
    return FKS_STRUPR(name);
 #else
    return fks_pathToUpperN(name, fks_pathLen(name));
 #endif
}


/** ファイルパス名中のディレクトリと拡張子を除いたファイル名の取得.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathGetBaseNameNoExt(FKS_PATH_CHAR dst[], FKS_PATH_SIZE size, FKS_PATH_CHAR const *src) FKS_NOEXCEPT
{
    FKS_PATH_CHAR const *s;
    FKS_PATH_CHAR const *e;
    FKS_PATH_SIZE         l = 0;
    FKS_ASSERT(dst != 0 && size > 1 && src != 0);
    //if (dst == 0 || size == 0 || src == 0) return 0;
    s = fks_pathBaseName(src);
    e = FKS_PATH_R_STR(s, FKS_PATH_C('.'));
    if (e == 0 || e == s)
        e = s + fks_pathLen(s);
    l = e - s; // +1;
    if (l > size)
        l = size;
    fks_pathCpy(dst, l, s);
    return dst;
}


/** ディレクトリ部分を返す. 最後のディレクトリセパレータは外す.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathGetDir(FKS_PATH_CHAR dir[], FKS_PATH_SIZE size, FKS_PATH_CHAR const *name) FKS_NOEXCEPT
{
    FKS_PATH_CHAR const*    p;
    size_t                  l;

    FKS_ASSERT(dir  != 0 && size > 0 && name != 0);

    p = fks_pathBaseName(name);
    l = p - name; // +1;
    if (l > size)
        l = size;
    if (l && dir != name)
        fks_pathCpy(dir, l, name);
    dir[l] = 0;
    if (l > 1)
        fks_pathDelLastSep(dir);
    return dir;
}


/** ドライブ名部分を取得. :つき. ※ file:等の対処のため"文字列:"をドライブ扱い.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathGetDrive(FKS_PATH_CHAR drive[], FKS_PATH_SIZE size, FKS_PATH_CHAR const *name) FKS_NOEXCEPT
{
    FKS_PATH_CHAR const*    s;
    FKS_PATH_SIZE           l;
    FKS_ASSERT(drive && size > 0 && name);
    drive[0] = 0;
    s = fks_pathSkipDrive(name);
    l = s - name;
    if (l > 0) {
        //++l;
        if (l > size)
            l = size;
        fks_pathCpy(drive, l,   name);
    }
    return drive;
}


/** ドライブ名とルート指定部分を取得.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathGetDriveRoot(FKS_PATH_CHAR dr[], FKS_PATH_SIZE size, FKS_PATH_CHAR const *name) FKS_NOEXCEPT
{
    FKS_PATH_CHAR const*    s;
    FKS_PATH_SIZE           l;
    FKS_ASSERT(dr && size > 0 && name);
    dr[0] = 0;
    s = fks_pathSkipDriveRoot(name);
    l = s - name;
    if (l > 0) {
        //++l;
        if (l > size)
            l = size;
        fks_pathCpy(dr, l, name);
    }
    return dr;
}


/** 文字列先頭のドライブ名,ルート指定をスキップしたポインタを返す.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathSkipDriveRoot(FKS_PATH_const_CHAR* path) FKS_NOEXCEPT
{
    FKS_PATH_CHAR* p = fks_pathSkipDrive(path);
    while (fks_pathIsSep(*p))
        ++p;
    return p;
}


/** filePath中の \ を / に置換.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathBackslashToSlash(FKS_PATH_CHAR filePath[]) FKS_NOEXCEPT
{
    FKS_PATH_CHAR *p = filePath;
    FKS_ASSERT(filePath != NULL);
    while (*p != FKS_PATH_C('\0')) {
        if (*p == FKS_PATH_C('\\')) {
            *p = FKS_PATH_C('/');
        }
        p = FKS_PATH_CHARNEXT(p);
    }
    return filePath;
}


/** filePath中の / を \ に置換.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathSlashToBackslash(FKS_PATH_CHAR filePath[]) FKS_NOEXCEPT
{
    FKS_PATH_CHAR *p;
    FKS_ASSERT(filePath != NULL);
    for (p = filePath; *p; ++p) {
        if (*p == FKS_PATH_C('/'))
            *p = FKS_PATH_C('\\');
    }
    return filePath;
}


FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathFullpath(FKS_PATH_CHAR dst[], FKS_PATH_SIZE size, FKS_PATH_CHAR const* path, FKS_PATH_CHAR const* currentDir)  FKS_NOEXCEPT
{
  #if defined FKS_PATH_WINDOS
    return fks_pathFullpathBS(dst, size, path, currentDir);
  #else
    return fks_pathFullpathSL(dst, size, path, currentDir);
  #endif
}


/** フルパス生成. ディレクトリセパレータを\\にして返すバージョン.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathFullpathBS(FKS_PATH_CHAR    dst[], FKS_PATH_SIZE size, FKS_PATH_CHAR const* path, FKS_PATH_CHAR const* currentDir) FKS_NOEXCEPT
{
    fks_pathFullpathSL(dst, size,   path, currentDir);
    fks_pathSlashToBackslash(dst);
    return dst;
}


/** フルパス生成. 文字列操作のみ. カレントパスは引数で渡す.
 *  currentDir は絶対パスであること. そうでない場合の挙動は不定.
 *  '\'文字対策で、セパレータは'/'に置き換ている.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathFullpathSL(FKS_PATH_CHAR dst[], FKS_PATH_SIZE size, FKS_PATH_CHAR const* path, FKS_PATH_CHAR const* currentDir) FKS_NOEXCEPT
{
    FKS_PATH_CHAR*      wk;
    FKS_PATH_SIZE       wkSz;

    FKS_ASSERT(dst != 0 && size > 2 && path != 0);
    if (dst == 0 || size <= 2 || path == 0)
        return 0;
    if (currentDir == 0)
        currentDir = FKS_PATH_C("/");   // DIRSEP_STR;
    FKS_ASSERT(fks_pathIsAbs(currentDir));

  #ifdef FKS_PATH_USE_ALLOCA
    // 書き込み先サイズが十分でなければ作業用メモリを確保.
    {
        FKS_PATH_SIZE pl = fks_pathLen(path);
        FKS_PATH_SIZE cl = fks_pathLen(currentDir);
        wkSz = pl + cl + 4;
        if (wkSz >= size) {     // dstサイズよりも、元が多いならワークを用意.
            FKS_ASSERT( wkSz <= FKS_PATH_CH_MAX_URL * sizeof(FKS_PATH_CHAR) );
            wk = (FKS_PATH_CHAR*)fks_alloca(wkSz*sizeof(FKS_PATH_CHAR));
            if (wk == 0) {
                wk   = dst;
                wkSz = size;
            }
        } else {
            wkSz = size;
            wk   = dst;
        }
    }
  #else // allocaを全く使わない場合は出力先を直接使うだけ.
    wkSz = size;
    wk   = dst;
  #endif

    // 作業用の絶対パスを作る.
    {
        unsigned hasDrive = fks_pathHasDrive(path);
        unsigned isAbs    = fks_pathIsAbs(path);
        wk[0] = 0;
        if (isAbs && hasDrive) {    // ドライブ付き絶対パスなら、そのまま.
            fks_pathCpy(wk, wkSz,   path);
        } else if (isAbs) {
            if (fks_pathHasDrive(currentDir))   // 絶対パスだけどドライブがない場合はcurrentDirからドライブだけいただく.
                fks_pathGetDrive(wk, wkSz, currentDir);
            fks_pathCat(wk, wkSz,   path);
        } else {
            if (hasDrive) {         // ドライブ付き相対パスで,
                if (!fks_pathHasDrive(currentDir))      // カレント側にドライブがなければ,
                    fks_pathGetDrive(wk, wkSz, path);   // pathのドライブ名を設定. ちがえばカレント側のドライブ名になる.
            }
            fks_pathCat(wk, wkSz,   currentDir);
            fks_pathAddSep(wk, wkSz);
            fks_pathCat(wk, wkSz,   fks_pathSkipDrive(path));
        }
    }

  #if defined FKS_PATH_WINDOS
    // 処理を簡単にするため、パスの区切りを一旦 / に変換.
    fks_pathBackslashToSlash(wk);
  #endif

    // "." や ".." を取り除く.
    {
        // この時点でwkは必ず絶対パスになっている.(currentDirが違反してた場合の挙動は不定扱い).
        FKS_PATH_CHAR*  s     = fks_pathSkipDrive(wk); // ドライブ名は弄らないのでスキップ.
        FKS_PATH_CHAR*  d     = s;
        FKS_PATH_CHAR*  top   = d;
        unsigned        first = 1;
        while (*s) {
            unsigned c = *s++;
            if (c == FKS_PATH_C('/')) {
                if (first) {    // 初回の / は "//" "///" を許す... あとで*d++=cするのでここでは2回まで.
                    unsigned i;
                    for (i = 0; i < 2 && *s == FKS_PATH_C('/'); ++i)
                        *d++ = *s++;
                }
                first = 0;
                // '/'の連続は一つの'/'扱い.
              RETRY:
                while (*s == FKS_PATH_C('/'))
                    ++s;
                if (*s == FKS_PATH_C('.')) {
                    if (s[1] == 0) {                    // "." のみは無視.
                        s += 1;
                        goto RETRY;
                    } else if (s[1] == FKS_PATH_C('/')) {       // "./" は無視.
                        s += 2;
                        goto RETRY;
                    } else if (s[1] == FKS_PATH_C('.') && (s[2] == FKS_PATH_C('/') || s[2] == 0)) { // "../" ".." のとき.
                        s += 2 + (s[2] != 0);
                        while (d > top && *--d != FKS_PATH_C('/'))  // 出力先のディレクトリ階層を１つ減らす.
                            ;
                        goto RETRY;
                    }
                }
            }
            *d++ = c;
        }
        *d = 0;
    }

  #ifdef FKS_PATH_USE_ALLOCA
    if (wk != dst)  // ワークをallocaしてたのなら、コピー.
        fks_pathCpy(dst, size, wk);
  #endif

    return dst;
}


FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathRelativePath(FKS_PATH_CHAR dst[], FKS_PATH_SIZE size, FKS_PATH_CHAR const* path, FKS_PATH_CHAR const* currentDir)  FKS_NOEXCEPT
{
  #if defined FKS_PATH_WINDOS
    return fks_pathRelativePathBS(dst, size, path, currentDir);
  #else
    return fks_pathRelativePathSL(dst, size, path, currentDir);
  #endif
}

/** 相対パス生成. ディレクトリセパレータを\\にして返すバージョン.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathRelativePathBS(FKS_PATH_CHAR    dst[], FKS_PATH_SIZE size, FKS_PATH_CHAR const* path, FKS_PATH_CHAR const* currentDir) FKS_NOEXCEPT
{
    fks_pathRelativePathSL(dst, size,   path, currentDir);
    fks_pathSlashToBackslash(dst);
    return dst;
}


/** currentDirからの相対パス生成.
 *  currentDir は絶対パスであること. そうでない場合の挙動は不定.
 *  '\'文字対策で、セパレータは'/'に置き換ている.
 *  カレントディレクトリから始まる場合、"./"はつけない.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathRelativePathSL(FKS_PATH_CHAR dst[], FKS_PATH_SIZE size, FKS_PATH_CHAR const* path, FKS_PATH_CHAR const* currentDir) FKS_NOEXCEPT
{
    FKS_STATIC_ASSERT(FKS_PATH_MAX >= 16);
    FKS_STATIC_ASSERT(FKS_PATH_MAX  >= 16);
    FKS_PATH_CHAR       curDir  [ FKS_PATH_MAX + 1 ] = {0};
    FKS_PATH_CHAR       fullName[ FKS_PATH_MAX + 1 ] = {0};
    FKS_PATH_CHAR*      cp;
    FKS_PATH_CHAR*      cpSav;
    FKS_PATH_CHAR*      fp;
    FKS_PATH_SIZE       cl;
    FKS_PATH_SIZE       fl;

    FKS_ASSERT(dst != 0 && size > 2 && path != 0);
    if (dst == 0 || size <= 2 || path == 0)
        return 0;

    // まず、カレントディレクトリをフルパス化(/) & 最後に/を付加.
    FKS_ASSERT(fks_pathIsAbs(currentDir));
    fks_pathFullpathSL(curDir, FKS_PATH_MAX, currentDir, FKS_PATH_C("/"));
    cp = fks_pathCheckLastSep(curDir);
    if (cp == 0) {
        cp = curDir + fks_pathLen(curDir);
        if (cp+1 < curDir+FKS_PATH_MAX) {
            *cp++ = FKS_PATH_C('/');
            *cp = 0;
        }
    }

    // 対象を path をフルパス化. \\は面倒なので/化した状態.
    fks_pathFullpathSL(fullName, FKS_PATH_MAX, path, curDir);

    // マッチしているディレクトリ部分をスキップ.
    cp    = fks_pathSkipDriveRoot(curDir);
    fp    = fks_pathSkipDriveRoot(fullName);

    // ドライブ名が違っていたら相対パス化しない.
    cl    = cp - curDir;
    fl    = fp - fullName;
    if (cl != fl || fks_pathNCmp(curDir,    fullName, fl) != 0) {
        fks_pathCpy(dst, size, fullName);
        return dst;
    }

    // 同じ部分をチェック.
    cpSav = cp;
    while (*cp && *fp) {
        unsigned cc;
        unsigned fc;
        FKS_PATH_GET_C(cc, cp);
        FKS_PATH_GET_C(fc, fp);
        if (cc != fc)
            break;
        if (*cp == FKS_PATH_C('/') && *fp == FKS_PATH_C('/'))
            cpSav = (FKS_PATH_CHAR*)cp + 1;
    }
    fp      = fp - (cp - cpSav);
    cp      = cpSav;

    // カレント位置から上への移動数分 ../ を生成.
    {
        FKS_PATH_CHAR* d = dst;
        FKS_PATH_CHAR* e = dst + size - 1;
        while (*cp) {
            if (*cp == FKS_PATH_C('/')) {
                if (d < e) *d++ = FKS_PATH_C('.');
                if (d < e) *d++ = FKS_PATH_C('.');
                if (d < e) *d++ = FKS_PATH_C('/');
            }
            ++cp;
        }
        *d = FKS_PATH_C('\0');
    }

    // カレント位置以下の部分をコピー.
    fks_pathCat(dst, size, fp);

    return dst;
}


/** ?,* のみの(dos/winな)ワイルドカード文字列比較.
 * *,? はセパレータにはマッチしない.
 * TODO: windows なら PathMatchSpecW に置き換えるが無難???
 *  @param tgt  ターゲット文字列.
 *  @param ptn  パターン(*?指定可能)
 */
FKS_LIB_DECL (int)
fks_pathMatchSpec(FKS_PATH_CHAR const* tgt, FKS_PATH_CHAR const* ptn) FKS_NOEXCEPT
{
 #ifdef FKS_PATH_MATCHSPEC
    return (int)FKS_PATH_MATCHSPEC(tgt, ptn);
 #else  // unDonutのソースより流用. その元はhttp://www.hidecnet.ne.jp/~sinzan/tips/main.htmらしいがリンク切.
    unsigned                tc;
    unsigned                pc;
    FKS_PATH_CHAR const*    tgt2 = tgt;
    FKS_PATH_GET_C(tc, tgt2);   // 1字取得& tgtポインタ更新.
    switch (*ptn) {
    case FKS_PATH_C('\0'):
        return tc == FKS_PATH_C('\0');

  #if defined FKS_PATH_WINDOWS
    case FKS_PATH_C('\\'):
  #endif
    case FKS_PATH_C('/'):
        return fks_pathIsSep(tc) && fks_pathMatchSpec(tgt2, ptn+1);

    case FKS_PATH_C('?'):
        return tc && !fks_pathIsSep(tc) && fks_pathMatchSpec(tgt2, ptn+1);

    case FKS_PATH_C('*'):
        // 拡張して ** でセパレータにもマッチさせる場合.
        //if (ptn[1] == FKS_PATH_C('*')) // ** ならセパレータにもマッチ.
        //  return fks_pathMatchSpec(tgt, ptn+2) || (tc && fks_pathMatchSpec(tgt2, ptn));
        return fks_pathMatchSpec(tgt, ptn+1) || (tc && !fks_pathIsSep(tc) && fks_pathMatchSpec(tgt2, ptn));

    default:
        FKS_PATH_GET_C(pc, ptn);    // 1字取得& ptnポインタ更新.
        return (pc == tc) && fks_pathMatchSpec(tgt2, ptn);
    }
 #endif
}


/** コマンドライン引数や、;区切りの複数パス指定を、分解するのに使う.
 *  ""はwinコマンドラインにあわせた扱い.
 *  sepChrで区切られた文字列(ファイル名)を取得. 0x20以外の空白は無視か0x20扱い.
 *  @return スキャン更新後のアドレスを返す。strがEOSだったらNULLを返す.
 */
FKS_LIB_DECL (FKS_PATH_CHAR*)
fks_pathScanArgStr(FKS_PATH_CHAR arg[], FKS_PATH_SIZE argSz, FKS_PATH_CHAR const *str, unsigned sepChr) FKS_NOEXCEPT
{
  #ifdef FKS_PATH_WCS_COMPILE
    FKS_PATH_CHAR const*    s = str;
  #else
    const unsigned char*    s = (const unsigned char*)str;
  #endif
    FKS_PATH_CHAR*          d = arg;
    FKS_PATH_CHAR*          e = d + argSz;
    unsigned                f = 0;
    unsigned                c;

    FKS_ASSERT( str != 0 && arg != 0 && argSz > 1 );

    // 0x20以外の空白とセパレータをスキップ.
    while ( *s == sepChr || (*s > 0U && *s < 0x20U) || *s == 0x7fU)
        ++s;
    if (*s == 0) {  // EOSだったら、見つからなかったとしてNULLを返す.
        arg[0] = 0;
        return NULL;
    }

    do {
        c = *s++;
        if (c == FKS_PATH_C('"')) {
            f ^= 1;                         // "の対の間は空白をファイル名に許す.ためのフラグ.

            // ちょっと気持ち悪いが、Win(XP)のcmd.exeの挙動に合わせてみる.
            if (*s == FKS_PATH_C('"') && f == 0)    // 閉じ"の直後にさらに"があれば、それはそのまま表示する.
                ++s;
            else
                continue;                   // 通常は " は省いてしまう.
        }
        if ((c > 0 && c < 0x20) || c == 0x7f)
            c = FKS_PATH_C(' ');
        if (d < e)
            *d++ = (FKS_PATH_CHAR)c;
    } while (c >= 0x20 && c != 0x7f && (f || (c != sepChr)));
    *--d  = FKS_PATH_C('\0');
    --s;
    return (FKS_PATH_CHAR *)s;
}


#if (defined __cplusplus) && !(defined FKS_PATH_WCS_COMPILE)
}
#endif
