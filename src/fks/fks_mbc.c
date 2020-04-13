/*
 *  @file   fks_mbc.c
 *  @brief  Multi Byte Character lib.
 *  @author Masashi Kitamura (tenka@6809.net)
 *  @license Boost Software Lisence Version 1.0
 */

#include <fks/fks_common.h>
#include <fks/fks_mbc.h>
#include <fks/fks_malloc.h>
#include <fks/fks_assert_ex.h>

#ifdef _WIN32
#include <windows.h>
#ifdef _MSC_VER
#pragma warning (disable:4244)
#endif
#endif

#include "detail/fks_mbc_sub.h"

#ifdef FKS_USE_MBC_JIS
#include "detail/fks_mbc_jis.hh"
#endif

#ifdef __cplusplus
extern "C" {
#endif

// ---------------------------------------------------------------------------
// utf8

/** Check character encode.
 */
static unsigned utf32_chkC(unsigned c) {
    return c < 0xD800 || (c >= 0xE000 && c <= 0x10FFFD && (uint16_t)c != 0xffff && (uint16_t)c != 0xfffe );
}


/** DBC high byte?
 */
static unsigned utf8_islead(unsigned c) {
    return c >= 0xC0;
}


/** Get character.
 */
static unsigned utf8_getC(char const** pStr) {
    unsigned char const* s = (unsigned char*)*pStr;
    unsigned char b = *s++;
    unsigned c = b, d = c;
    if (c < 0xC0) {
        if (!c)
            goto NIL;
        else if (c < 0x80)
            goto RET;
        goto ERET;
    }
    b = *s++;
    if (b < 0x80)
        goto ERET;
    d = (d << 6) | (b & 0x3f);        // 11=5+6 0x80 .. 0x7ff
    if (c < 0xE0) {
        d &= (1 << 11) - 1;
        goto RET;
    }
    b = *s++;
    if (b < 0x80)
        goto ERET;
    d = (d << 6) | (b & 0x3f);        // 16=4+6*2 0x8000 .. 0xffff
    if (c < 0xF0) {
        d &= (1 << 16) - 1;
        goto RET;
    }
    b = *s++;
    if (b < 0x80)
        goto ERET;
    d = (d << 6) | (b & 0x3f);        // 21=3+6*3
    if (c < 0xF8) {
        d &= (1 << 21) - 1;
        goto RET;
    }
    b = *s++;
    if (b < 0x80)
        goto ERET;
    d = (d << 6) | (b & 0x3f);        // 26=2+6*4
    if (c < 0xFC) {
        d &= (1 << 26) - 1;
        goto RET;
    }
    b = *s++;
    if (b < 0x80)
        goto ERET;
    d = (d << 6) | (b & 0x3f);        // 31=1+6*5
    d &= (1U << 31) - 1;
    goto RET;
 ERET:
    d = 0xffffffff;    // error char
    //if (!b) d = 0;
 NIL:
    --s;
 RET:
    *pStr = (char*)s;
    return d;
}

/** Peek character.
 */
static unsigned utf8_peekC(char const* s) {
    return utf8_getC(&s);
}

/** Set character.
 */
static char*    utf8_setC(char*  dst, char* e, unsigned c) {
    char* d = dst;
    if (c < 0xC0/*0x80*/) { // 0x80-xBF bad code
        if (d >= e) goto ERR;
        *d++ = c;
    } else if (c <= 0x7FF) {
        if (d+2 > e) goto ERR;
        *d++ = 0xC0|(c>>6);
        *d++ = 0x80|(c&0x3f);
    } else if (c <= 0xFFFF) {
        if (d+3 > e) goto ERR;
        *d++ = 0xE0|(c>>12);
        *d++ = 0x80|((c>>6)&0x3f);
        *d++ = 0x80|(c&0x3f);
        //if (c >= 0xff60 && c <= 0xff9f) {--(*adn); }  // hankaku-kana
    } else if (c <= 0x1fFFFF) {
        if (d+4 > e) goto ERR;
        *d++ = 0xF0|(c>>18);
        *d++ = 0x80|((c>>12)&0x3f);
        *d++ = 0x80|((c>>6)&0x3f);
        *d++ = 0x80|(c&0x3f);
    } else if (c <= 0x3fffFFFF) {
        if (d+5 > e) goto ERR;
        *d++ = 0xF8|(c>>24);
        *d++ = 0x80|((c>>18)&0x3f);
        *d++ = 0x80|((c>>12)&0x3f);
        *d++ = 0x80|((c>>6)&0x3f);
        *d++ = 0x80|(c&0x3f);
    } else {
        if (d+6 > e) goto ERR;
        *d++ = 0xFC|(c>>30);
        *d++ = 0x80|((c>>24)&0x3f);
        *d++ = 0x80|((c>>18)&0x3f);
        *d++ = 0x80|((c>>12)&0x3f);
        *d++ = 0x80|((c>>6)&0x3f);
        *d++ = 0x80|(c&0x3f);
    }
    return d;

ERR:
    while (d < e)
        *d++ = 0;
    return e;
}

/** Get pointer of next character.
 */
//static char* utf8_charNext(char const* s) { return s + utf8_len1(s); }
static char* utf8_charNext(char const* p) {
    unsigned c = *(unsigned char*)p;
    if (c == 0)
        return (char*)p;
    if (*p) ++p;
    if (c < 0xC0)
        return (char*)p;
    if (*p) ++p;
    if (c < 0xE0)
        return (char*)p;
    if (*p) ++p;
    if (c < 0xF0)
        return (char*)p;
    if (*p) ++p;
    if (c < 0xF8)
        return (char*)p;
    if (*p) ++p;
    if (c < 0xFC)
        return (char*)p;
    if (*p) ++p;
    return (char*)p;
}

/** Size of character.
 */
static unsigned utf8_len1(char const* s) {
    unsigned c = *(unsigned char*)s;
    if (c == 0)
        return 0;
    if (c < 0xC0)
        return 1;
    if (c < 0xE0)
        return 2;
    if (c < 0xF0)
        return 3;
    if (c < 0xF8)
        return 4;
    if (c < 0xFC)
        return 5;
    return 6;
}

/** Size of character.
 */
static unsigned utf8_chrLen(unsigned c) {
    if (c < 0x80)
        return 1;
    if (c <= 0x7FF)
        return 2;
    if (c <= 0xFFFF)
        return 3;
    if (c <= 0x1fFFFF)
        return 4;
    if (c <= 0x3fffFFFF)
        return 5;
    return 6;
}

/** Get display size of character(for JP font)
 */
static unsigned utf8_chrWidth(unsigned c) {
    if (c < 0x370)
        return 1;
    if (c >= 0xff60 && c <= 0xff9f)
        return 1;
    return 2;
}

/** Check UTF8 Encode?
 * @return 0=not  1=ascii(<=7f) 2,3,4=ut8
 */
int  fks_mbsCheckUTF8(char const* src, size_t len, int canEndBroken)
{
    unsigned char const* s = (unsigned char*)src;
    unsigned char const* e = s + len;
    char    hasAsc  = 0;
    char    allAsc  = 1;
    char    badFlg  = 0;
    char    zenFlg  = 0;
    char    b5b6    = 0;
    char    salFlg  = 0;
    int     c, c2, c3, c4, c5, c6;
    if (len == 0 )
        return 0;
    while (s < e) {
        c = *s++;
        if  (c <= 0x7f) {
            if (c == '\0') {
                badFlg = 1;
                break;
            }
            hasAsc = 1;
            continue;
        }

        allAsc = 0;
        if (c < 0xC0) {
            badFlg = 1;
            break;
        }

        if (s >= e) { badFlg = !canEndBroken; break; }
        c2 = *s++;
        if ((c2 & 0xC0) != 0x80) {
            badFlg = 1;
            break;
        }
        c2 &= 0x3f;
        if (c < 0xE0) {
            c = ((c & 0x1F) << 6) | c2;
            if (c <= 0x7F) {
                badFlg = 1;
                break;
            }
            zenFlg = 1;
            continue;
        }
        if (s >= e) {
            if (!canEndBroken)
                badFlg = 1;
            break;
        }

        if (s >= e) { badFlg = !canEndBroken; break; }
        c3 = *s++;
        if ((c3 & 0xC0) != 0x80) {
            badFlg = 1;
            break;
        }
        c3 &= 0x3f;
        if (c < 0xF0) {
            c = ((c & 0xF) << 12) | (c2 << 6) | c3;
            if (c <= 0x7FF) {
                badFlg = 1;
                break;
            }
            if (c >= 0xD800 && c <= 0xDFFF) {
                salFlg = 1;
            }
            zenFlg = 1;
            continue;
        }

        if (s >= e) { badFlg = !canEndBroken; break; }
        c4 = *s++;
        if ((c4 & 0xC0) != 0x80) {
            badFlg = 1;
            break;
        }
        c4 &= 0x3f;
        if (c < 0xF8) {
            c = ((c&7)<<18) | (c2<<12) | (c3<<6) | c4;
            if (c <= 0xFFFF) {
                badFlg = 1;
                break;
            }
            zenFlg = 1;
            continue;
        }

        if (s >= e) { badFlg = !canEndBroken; break; }
        c5 = *s++;
        if ((c5 & 0xC0) != 0x80) {
            badFlg = 1;
            break;
        }
        c5 &= 0x3f;
        if (c < 0xFC) {
            c = ((c&3)<<24) | (c2<<18) | (c3<<12) | (c4<<6) | c5;
            if (c <= 0x1fFFFF) {
                badFlg = 1;
                break;
            }
            zenFlg = 1;
            b5b6 = 1;
            continue;
        }

        if (s >= e) { badFlg = !canEndBroken; break; }
        c6 = *s++;
        if ((c6 & 0xC0) != 0x80) {
            badFlg = 1;
            break;
        }
        c6 &= 0x3f;
        c = ((c&1)<<30) |(c2<<24) | (c3<<18) | (c4<<12) | (c5<<6) | c6;
        if (c <= 0x3ffFFFF) {
            badFlg = 1;
            break;
        }
        zenFlg = 1;
        b5b6 = 1;
    }
    if (badFlg)
        return 0;
    if (zenFlg)
        return (salFlg) ? 2 : (b5b6) ? 3 : 4;
    if (allAsc)
        return 1;
    return 0;
}

MBC_IMPL(utf8)

static Fks_MbcEnc const fks_mbsEnc_utf8 = {
    FKS_CP_UTF8,                // code page
    utf8_islead,                // DBC high byte?
    utf32_chkC,                  // Check character encode.
    utf8_getC,                  // Get character.
    utf8_peekC,                 // Peek character.
    utf8_charNext,              // Get pointer of next character.
    utf8_setC,                  // Set character.
    utf8_len1,                  // Size of character.
    utf8_chrLen,                // Size of character.
    utf8_chrWidth,              // Get display size of character.
    utf8_adjustSize,            // Adjust the end of the string.
    utf8_cmp,                   // Compare string.
    fks_mbsCheckUTF8,           // Check string's character encode.
};
fks_mbcenc_t const fks_mbc_utf8 = &fks_mbsEnc_utf8;



// ---------------------------------------------------------------------------
#define REV_U16(c)      ((uint8_t)((c) >> 8) | ((uint8_t)(c) << 8))

#ifdef FKS_BIG_ENDIAN
#define GET16LE(p)      REV_U16(*(uint16_t*)(p))
#define SET16LE(p, c)   (*(uint16_t*)(p) = REV_U16(c))
#else
#define GET16LE(p)      (*(uint16_t*)(p))
#define SET16LE(p, c)   (*(uint16_t*)(p) = (c))
#endif

static unsigned utf16le_getC(char const** ppSrc) {
    uint16_t const* p = (uint16_t const*)*ppSrc;
    uint32_t        c = GET16LE(p);
    if (c)
        ++p;
    if (0xD800 <= c && c <= 0xDBFF) {
        uint32_t    d = GET16LE(p);
        if (0xDC00 <= d && d <= 0xDFFF) {
            ++p;
            c = ((c & 0x3ff) << 10) | (d & 0x3ff);
            c += 0x10000;
        }
    }
    *ppSrc = (char const*)p;
    return c;
}

static unsigned utf16le_peekC(char const* pSrc) {
    return utf16le_getC((char const**)&pSrc);
}

static char* utf16le_setC(char* d, char* e, unsigned c) {
    if (c < 0x10000) {
        if (d+2 > e) goto ERR;
        SET16LE(d, c);
        d += 2;
    } else if (c <= 0x10FFFF) {
        uint32_t x = c - 0x10000;
        if (d+4 > e) goto ERR;
        c = (x >> 10) | 0xD800;
        SET16LE(d, c);
        d += 2;
        c = (x & 0x3ff) | 0xDC00;
        SET16LE(d, c);
        d += 2;
    } else {    // error
        //SET16LE(d, c);
        //d += 2;
    }
    return d;
ERR:
    while (d < e)
        *d++ = 0;
    return e;
}

static unsigned utf16le_chrLen(unsigned c) {
    return c > 0xffff ? 4 : c ? 2 : 0;
}

static char* utf16le_charNext(char const* s) {
    uint16_t const* p = (uint16_t const*)s;
    uint32_t        c = GET16LE(p);
    if (c)
        ++p;
    if (0xD800 <= c && c <= 0xDBFF) {
        uint32_t    d = GET16LE(p);
        if (0xDC00 <= d && d <= 0xDFFF) {
            ++p;
        }
    }
    return (char*)p;
}

static unsigned utf16le_len1(char const* p) {
    unsigned c = GET16LE(p);
    return (c == 0) ? 0 : (0xD800 <= c && c <= 0xDBFF) ? 4 : 2;
}

static int  utf16le_checkEncoding(char const* src, size_t len, int canEndBroken) {
    return fks_mbsCheckEncoding(fks_mbc_utf16le, src, (len & ~1), canEndBroken);
}

MBC_IMPL(utf16le)


static Fks_MbcEnc const fks_mbsEnc_utf16le = {
    FKS_CP_UTF16LE,
    utf8_islead,
    utf32_chkC,
    utf16le_getC,
    utf16le_peekC,
    utf16le_charNext,
    utf16le_setC,
    utf16le_len1,
    utf16le_chrLen,
    utf8_chrWidth,
    utf16le_adjustSize,
    utf16le_cmp,
    utf16le_checkEncoding,
};
fks_mbcenc_t const fks_mbc_utf16le = &fks_mbsEnc_utf16le;



// ---------------------------------------------------------------------------

#ifdef FKS_BIG_ENDIAN
#define GET16BE(p)      REV_U16(*(uint16_t*)(p))
#define SET16BE(p, c)   (*(uint16_t*)(p) = REV_U16(c))
#else
#define GET16BE(p)      (*(uint16_t*)(p))
#define SET16BE(p, c)   (*(uint16_t*)(p) = (c))
#endif


static unsigned utf16be_getC(char const** ppSrc) {
    uint16_t const* p = (uint16_t const*)*ppSrc;
    uint32_t        c = GET16BE(p);
    if (c)
        ++p;
    if (0xD800 <= c && c <= 0xDBFF) {
        uint32_t    d = GET16BE(p);
        if (0xDC00 <= d && d <= 0xDFFF) {
            ++p;
            c = ((c & 0x3ff) << 10) | (d & 0x3ff);
            c += 0x10000;
        }
    }
    *ppSrc = (char const*)p;
    return c;
}

static unsigned utf16be_peekC(char const* pSrc) {
    return utf16be_getC((char const**)&pSrc);
}


static char* utf16be_setC(char* d, char* e, unsigned c) {
    if (c < 0x10000) {
        if (d+2 > e) goto ERR;
        SET16LE(d, c);
        d += 2;
    } else if (c <= 0x10FFFF) {
        uint32_t x = c - 0x10000;
        if (d+4 > e) goto ERR;
        c = (x >> 10) | 0xD800;
        SET16LE(d, c);
        d += 2;
        c = (x & 0x3ff) | 0xDC00;
        SET16LE(d, c);
        d += 2;
    } else {    // error
        //SET16LE(d, c);
        //d += 2;
    }
    return d;
ERR:
    while (d < e)
        *d++ = 0;
    return e;
}


static char* utf16be_charNext(char const* s) {
    uint16_t const* p = (uint16_t const*)s;
    uint32_t        c = GET16BE(p);
    if (c)
        ++p;
    if (0xD800 <= c && c <= 0xDBFF) {
        uint32_t    d = GET16BE(p);
        if (0xDC00 <= d && d <= 0xDFFF) {
            ++p;
        }
    }
    return (char*)p;
}

static unsigned utf16be_len1(char const* p) {
    unsigned c = GET16BE(p);
    return (c == 0) ? 0 : (0xD800 <= c && c <= 0xDBFF) ? 4 : 2;
}

static FKS_FORCE_INLINE unsigned utf16be_chrLen(unsigned c) {
    return c > 0xffff ? 4 : c ? 2 : 0;
}

static int  utf16be_checkEncoding(char const* src, size_t len, int canEndBroken) {
    return fks_mbsCheckEncoding(fks_mbc_utf16be, src, (len & ~1), canEndBroken);
}

MBC_IMPL(utf16be)

static Fks_MbcEnc const fks_mbsEnc_utf16be = {
    FKS_CP_UTF16BE,
    utf8_islead,
    utf32_chkC,
    utf16be_getC,
    utf16be_peekC,
    utf16be_charNext,
    utf16be_setC,
    utf16be_len1,
    utf16be_chrLen,
    utf8_chrWidth,
    utf16be_adjustSize,
    utf16be_cmp,
    utf16be_checkEncoding,
};
fks_mbcenc_t const fks_mbc_utf16be = &fks_mbsEnc_utf16be;



// ---------------------------------------------------------------------------
#define REV_U32(c)      ((uint8_t)((c) >> 24)| ((uint8_t)((c) >> 16) << 8) | ((uint8_t)((c) >> 8)<<16) | ((uint8_t)(c) << 24))

#ifdef FKS_BIG_ENDIAN
#define GET32LE(p)      REV_U32(*(uint32_t*)(p))
#define SET32LE(p, c)   (*(uint32_t*)(p) = REV_U32(c))
#else
#define GET32LE(p)      (*(uint32_t*)(p))
#define SET32LE(p, c)   (*(uint32_t*)(p) = (c))
#endif

static unsigned utf32le_getC(char const** ppSrc) {
    uint32_t const* p = (uint32_t const*)*ppSrc;
    uint32_t        c = GET32LE(p);
    if (c)
        ++p;
    *ppSrc = (char const*)p;
    return c;
}

static unsigned utf32le_peekC(char const* pSrc) {
    return GET32LE(pSrc);
}

static char* utf32le_setC(char* d, char* e, unsigned c) {
    if (d+4 <= e) {
        SET32LE(d, c);
        return d + 4;
    } else {
        return e;
    }
}

static unsigned utf32le_len1(char const* s) {
    return *(uint32_t const*)s ? 4 : 0;
}

static unsigned utf32le_chrLen(unsigned c) {
    return c > 0 ? 4 : 0;
}

static char* utf32le_charNext(char const* s) {
    return (char*)s + (*(uint32_t const*)s ? 4 : 0);
}

static int  utf32le_checkEncoding(char const* src, size_t len, int canEndBroken) {
    return fks_mbsCheckEncoding(fks_mbc_utf32le, src, (len & ~3), canEndBroken);
}

MBC_IMPL(utf32le)

static Fks_MbcEnc const fks_mbsEnc_utf32le = {
    FKS_CP_UTF32LE,
    utf8_islead,
    utf32_chkC,
    utf32le_getC,
    utf32le_peekC,
    utf32le_charNext,
    utf32le_setC,
    utf32le_len1,
    utf32le_chrLen,
    utf8_chrWidth,
    utf32le_adjustSize,
    utf32le_cmp,
    utf32le_checkEncoding,
};
fks_mbcenc_t const fks_mbc_utf32le = &fks_mbsEnc_utf32le;



// ---------------------------------------------------------------------------
#ifdef FKS_BIG_ENDIAN
#define GET32BE(p)      (*(uint32_t*)(p))
#define SET32BE(p, c)   (*(uint32_t*)(p) = (c))
#else
#define GET32BE(p)      REV_U32(*(uint32_t*)(p))
#define SET32BE(p, c)   (*(uint32_t*)(p) = REV_U32(c))
#endif

static unsigned utf32be_getC(char const** ppSrc) {
    uint32_t const* p = (uint32_t const*)*ppSrc;
    uint32_t        c = GET32BE(p);
    if (c)
        ++p;
    *ppSrc = (char const*)p;
    return c;
}

static unsigned utf32be_peekC(char const* pSrc) {
    return GET32BE(pSrc);
}

static char* utf32be_setC(char* d, char* e, unsigned c) {
    if (d+4 <= e) {
        SET32BE(d, c);
        return d + 4;
    } else {
        return e;
    }
}

static unsigned utf32be_len1(char const* s) {
    return *(uint32_t const*)s ? 4 : 0;
}

static unsigned utf32be_chrLen(unsigned c) {
    return c > 0 ? 4 : 0;
}

static char* utf32be_charNext(char const* s) {
    return (char*)s + (*(uint32_t const*)s ? 4 : 0);
}

static int  utf32be_checkEncoding(char const* src, size_t len, int canEndBroken) {
    return fks_mbsCheckEncoding(fks_mbc_utf32be, src, (len & ~3), canEndBroken);
}

MBC_IMPL(utf32be)

static Fks_MbcEnc const fks_mbsEnc_utf32be = {
    FKS_CP_UTF32BE,
    utf8_islead,
    utf32_chkC,
    utf32be_getC,
    utf32be_peekC,
    utf32be_charNext,
    utf32be_setC,
    utf32be_len1,
    utf32be_chrLen,
    utf8_chrWidth,
    utf32be_adjustSize,
    utf32be_cmp,
    utf32be_checkEncoding,
};
fks_mbcenc_t const fks_mbc_utf32be = &fks_mbsEnc_utf32be;



// ---------------------------------------------------------------------------
// ascii

static char*    asc_setC(char*  d, char* e, unsigned c) { return (d < e) ? (*d++ = c):0, d; }
static unsigned asc_chrLen(unsigned chr) { return 1; }
static unsigned asc_chrWidth(unsigned chr) { return 1; }
static unsigned asc_islead(unsigned c) { return 0; }
//static unsigned asc_istrail(unsigned c) { return 0; }
static unsigned asc_chkC(unsigned c) { return 1; }
static unsigned asc_getC(char const** pp) { return *((*pp)++); }
static unsigned asc_peekC(char const* s) { return *(unsigned char*)s; }
static unsigned asc_len1(char const* s) { return *s > 0; }
static char*    asc_charNext(char const* s) { return (char*)s + (*s > 0); }

MBC_IMPL(asc)

static Fks_MbcEnc const fks_mbsEnc_asc = {
    FKS_CP_1BYTE,
    asc_islead,
    asc_chkC,
    asc_getC,
    asc_peekC,
    asc_charNext,
    asc_setC,
    asc_len1,
    asc_chrLen,
    asc_chrWidth,
    asc_adjustSize,
    asc_cmp,
};
fks_mbcenc_t const fks_mbc_asc = &fks_mbsEnc_asc;


// ---------------------------------------------------------------------------
// DBC

char*    fks_dbc_setC(char*  d, char* e, unsigned c) {
    if (c > 0xff) {
        if (d+2 > e) goto ERR;
        *d++ = c >> 8;
        *d++ = c;
    } else {
        if (d >= e) goto ERR;
        *d++ = c;
    }
    return d;
ERR:
    if (d < e)
        *d = 0;
    return e;
}


#ifdef FKS_WIN32

#define dbc_setC    fks_dbc_setC

static unsigned dbc_chrLen(unsigned chr) {
    return 1 + (chr > 0xff);
}

static unsigned dbc_chrWidth(unsigned chr) {
    return 1 + (chr > 0xff);
}

static unsigned dbc_islead(unsigned c) {
    return IsDBCSLeadByte((unsigned char)c);
}


static unsigned dbc_istrail(unsigned c) {
    return ((c >= 0x30) & (c <= 0xFE)) && c != 0x7f;
}

static unsigned dbc_chkC(unsigned c)
{
    if (c > 0xff) {
        return IsDBCSLeadByte(c >> 8) && dbc_istrail((unsigned char)c);
    }
    return 1;
}

static unsigned dbc_getC(char const** pStr) {
    unsigned char const* s = (unsigned char*)*pStr;
    unsigned       c       = *s++;
    if (IsDBCSLeadByte(c) && *s) {
        c = (c << 8) | *s++;
    }
    *pStr = (char const*)s;
    return c;
}

static unsigned dbc_peekC(char const* s) {
    unsigned       c       = *(unsigned char*)(s++);
    if (IsDBCSLeadByte(c) && *s) {
        c = (c << 8) | *s;
    }
    return c;
}

static unsigned dbc_len1(char const* s) {
    return (s[0] == 0) ? 0 : 1 + (IsDBCSLeadByte(*(unsigned char const*)s) && s[1]);
}

static char* dbc_charNext(char const* s) {
    return (char*)s + dbc_len1(s);
}

static int  dbc_checkEncoding(char const* src, size_t len, int canEndBroken) {
    int rc = fks_mbsCheckEncoding(fks_mbc_dbc, src, len, canEndBroken);
    if (rc > 1) {
        int l = MultiByteToWideChar(0, MB_ERR_INVALID_CHARS, src, len, NULL, 0);
        if (l == 0)
            return 0;
    }
    return rc;
}

MBC_IMPL(dbc)

static Fks_MbcEnc const fks_mbsEnc_dbc = {
    0,
    dbc_islead,
    dbc_chkC,
    dbc_getC,
    dbc_peekC,
    dbc_charNext,
    dbc_setC,
    dbc_len1,
    dbc_chrLen,
    dbc_chrWidth,
    dbc_adjustSize,
    dbc_cmp,
    dbc_checkEncoding,
};
fks_mbcenc_t const fks_mbc_dbc = &fks_mbsEnc_dbc;

#endif


// --------------------------------------------------------------------------

/** Check Shift-JIS Encode?
 * @param flags             bit=0  canEndBroken  bit=1 check CP932
 * @return 0=not  1=ascii  2=HANKAKU-KANA (cpP932chk:not cp932)  3=sjis  4=sjis(low byte:use ascii)
 */
int fks_mbsCheckSJIS(char const* src, size_t len, int flags)
{
    unsigned char const* s = (unsigned char const*)src;
    unsigned char const* e = s + len;
    char    canEndBroken = (flags & 1);
    char    cp932   = (flags & 2);
    char    ascFlg  = 1;
    char    kataFlg = 0;
    char    badFlg  = 0;
    char    zenFlg  = 0;
    char    lowAsc  = 0;
    char    noFnt   = 0;
    char    usrFnt  = 0;
    char    cp932jisX213sp = 0;
    int     rc = 0;
    int     c;
    if (len == 0)
        return 0;
    while (s < e) {
        c = *s++;
        if (c == '\0') {
            badFlg = 1;
            break;
        }
        if  (c <= 0x7f) {
            continue;
        }
        ascFlg = 0;
        if (c >= 0xA0 && c <= 0xDF) {
            kataFlg = 1;
            continue;
        }
        if (s >= e) {
            if (!canEndBroken)
                badFlg = 1;
            break;
        }
        if (c >= 0x81 && c <= 0xfc) {
            unsigned d = c << 8;
            c = *s;
            d |= c;
            if (c) {
                ++s;
                if (c >= 0x40 && c <= 0xfc && c != 0x7f) {
                    if (c < 0x7f)
                        lowAsc = 1;
                    zenFlg = 1;
                    if (d >= 0xED40)
                        cp932jisX213sp = 1;
                    if (cp932 && d >= 0x8540) {
                        if ((0x8540 <= d && d <= 0x86FC) || (0x879F <= d && d <= 0x889E) || (0xEAA5 <= d && d <= 0xECFC)
                            || (0xEF40 <= d && d <= 0xEFFC) || (0xFC4C <= d && d <= 0xFCFC)
                        ) {
                            noFnt = 1;
                        }
                        if (0xF040 <= d && d <= 0xF9FC) {
                            usrFnt = 1;
                        }
                    }
                } else {
                    badFlg = 1;
                    break;
                }
            } else {
                badFlg = 1;
                break;
            }
        } else {
            badFlg = 1;
            break;
        }
    }

    if (cp932jisX213sp)
        rc |= 0x4000;
    if (usrFnt)
        rc |= 0x2000;
    if (noFnt)
        rc |= 0x1000;

    else if (badFlg)
        ;
    else if (zenFlg)
        rc |= (lowAsc) ? 4 : 3;
    else if (kataFlg)
        rc |= 2;
    else if (ascFlg)
        rc |= 1;
    return rc;
}


#if defined FKS_WIN32 && !defined FKS_USE_MBC_JIS

static int cp932_checkEncoding(char const* src, size_t len, int canEndBroken)
{
    int rc = fks_mbsCheckSJIS(src, len, (int)(canEndBroken != 0) | 2);
    if (rc & 0x7000)
        rc = 2;
    rc = (uint8_t)rc;

    if (rc > 1) {
        int l = MultiByteToWideChar(932, MB_ERR_INVALID_CHARS, src, len, NULL, 0);
        if (l == 0)
            return 0;
    }
    return rc;
}

static Fks_MbcEnc const fks_mbsEnc_cp932 = {
    FKS_CP_SJIS,
    dbc_islead,
    dbc_chkC,
    dbc_getC,
    dbc_peekC,
    dbc_charNext,
    dbc_setC,
    dbc_len1,
    dbc_chrLen,
    dbc_chrWidth,
    dbc_adjustSize,
    dbc_cmp,
    cp932_checkEncoding,
};
fks_mbcenc_t const fks_mbc_cp932 = &fks_mbsEnc_cp932;

#endif



// ---------------------------------------------------------------------------

/** Check string (DBC type)
 *  @param  s               source string
 *  @param  len             Check range. ('\0' is not EOS but broken character)
 *  @param  canEndBroken    Can the end be broken
 *  @return 0:not 1:ascii 2,3,4:ok
 */
int  fks_mbsCheckEncoding(fks_mbcenc_t mbc, char const* s, size_t len, int canEndBroken) {
    char const* e = s + len;
    char asc = 1;
    char hasAsc = 0;
    unsigned (*getC)(char const** s) = mbc->getC;
    unsigned (*chkC)(unsigned c)    = mbc->chkC;
    while (s < e) {
        unsigned c;
        unsigned k = mbc->len1(s);
        if (s+k >= e)
            break;
        c = getC(&s);
        if (!c) {
            if (s < e || !canEndBroken)
                return 0;
            break;
        }
        if (c < 0x80) {
            hasAsc = 1;
        } else {
            asc = 0;
        }
        if (!chkC(c)) {
            if (!canEndBroken || *s != 0)
                return 0;
        }
    }
    if (asc)
        return 1;
    return 2 + hasAsc;
}

/** 領域サイズから(意味的な)文字数を求める.
 */
size_t  fks_mbsSizeToChrs(fks_mbcenc_t mbc, char const* str, size_t size) {
    char const* s = str;
    char const* e = s + size;
    size_t      l = 0;
    unsigned (*getC)(char const** s) = mbc->getC;
    if (e < s)
        e = (char const*)(~(size_t)0);
    FKS_ASSERT(str != 0 && size > 0);
    while (s < e) {
        unsigned c = getC(&s);
        if (c == 0)
            break;
        ++l;
    }
    if (s > e)
        --l;
    return l;
}


/// 文字数から半角文字単位の領域サイズを求める.
size_t  fks_mbsChrsToSize(fks_mbcenc_t mbc, char const* str, size_t chrs) {
    char const* s  = str;
    size_t      sz = 0;
    unsigned (*getC)(char const** s) = mbc->getC;
    unsigned (*chrLen)(unsigned c)  = mbc->chrLen;
    FKS_ASSERT(str != 0);
    while (chrs) {
        unsigned c  = getC(&s);
        if (c == 0)
            break;
        sz += chrLen(c);
        --chrs;
    }
    return sz;
}


/** 領域サイズから半角文字単位の幅を求める.
 */
size_t  fks_mbsSizeToWidth(fks_mbcenc_t mbc, char const* str, size_t size) {
    char const* s = str;
    char const* e = s + size;
    size_t      b = 0;
    size_t      w = 0;
    unsigned (*getC)(char const** s) = mbc->getC;
    unsigned (*chrWidth)(unsigned chr) = mbc->chrWidth;
    if (e < s)
        e = (char const*)(~(size_t)0);
    FKS_ASSERT(str != 0 && size > 0);
    while (s < e) {
        unsigned c  = getC(&s);
        if (c == 0)
            break;
        b = w;
        w += chrWidth(c);
    }
    if (s == e)
        return w;
    return b;
}


/** 文字数から半角文字単位の幅を求める.
 */
size_t  fks_mbsChrsToWidth(fks_mbcenc_t mbc, char const* str, size_t chrs) {
    char const* s = str;
    size_t      w = 0;
    unsigned (*getC)(char const** s) = mbc->getC;
    unsigned (*chrWidth)(unsigned chr) = mbc->chrWidth;
    FKS_ASSERT(str != 0);
    while (chrs) {
        unsigned c  = getC(&s);
        if (c == 0)
            break;
        w += chrWidth(c);
        --chrs;
    }
    return w;
}


/** 半角文字単位の幅から領域サイズを求める.
 */
size_t  fks_mbsWidthToSize(fks_mbcenc_t mbc, char const* str, size_t width) {
    char const* s = str;
    char const* b = s;
    size_t      w = 0;
    unsigned (*getC)(char const** s) = mbc->getC;
    unsigned (*chrWidth)(unsigned chr) = mbc->chrWidth;
    FKS_ASSERT(str != 0);
    while (w < width) {
        unsigned c;
        b  = s;
        c  = getC(&s);
        if (c == 0)
            break;
        w += chrWidth(c);
    }
    if (w > width)
        s = b;
    return s - str;
}


/** 半角文字単位の幅から文字数を求める.
 */
size_t  fks_mbsWidthToChrs(fks_mbcenc_t mbc, char const* str, size_t width) {
    char const* s = str;
    char const* b;
    size_t      w = 0;
    size_t      n = 0;
    unsigned (*getC)(char const** s) = mbc->getC;
    unsigned (*chrWidth)(unsigned chr) = mbc->chrWidth;
    FKS_ASSERT(str != 0);
    while (w < width) {
        unsigned c;
        b  = s;
        c  = getC(&s);
        if (c == 0)
            break;
        ++n;
        w += chrWidth(c);
    }
    if (w > width)
        --n;
    return n;
}


/** コピー. mbcの時は文字が壊れない部分まで. dst == src もok.
 */
size_t  fks_mbsCpy(fks_mbcenc_t mbc, char dst[], size_t dstSz, char const* src)
{
    size_t    l;
    FKS_ASSERT(dst != NULL && dstSz > 0 && src != NULL);

    l = mbc->adjustSize(src, dstSz);
    return fks_mbsLCpy(mbc, dst, dstSz, src, l+1);
}


size_t fks_mbsLCpy(fks_mbcenc_t mbc, char dst[], size_t dstSz, char const* src, size_t l)
{
    FKS_ASSERT(dst != NULL && dstSz > 0 && src != NULL);

    if (l > dstSz)
        l = dstSz;
    // アドレスが同じなら、長さをあわせるのみ.
    if (dst == src) {
        dst[l-1] = 0;
        return l;
    }

    // コピー.
    {
        char*       de = dst + dstSz;
        char const* s  = src;
        char const* se = s + l;
        char*       d  = dst;
        while (s < se)
            *d++ = *s++;
        if (d < de)
            *d = 0;
        return d - dst;
    }
}


/** 文字列の連結.
 */
size_t fks_mbsCat(fks_mbcenc_t mbc, char dst[], size_t dstSz, char const* src)
{
    size_t l, l2;
    FKS_ASSERT(dst != NULL && dstSz > 0 && src != 0 && dst != src);
    l = mbc_raw_len(dst);
    if (l >= dstSz) {   // そもそも転送先が満杯ならサイズ調整のみ.
        return fks_mbsCpy(mbc, dst, dstSz, dst);
    }
    l2 = fks_mbsCpy(mbc, dst+l, dstSz - l, src);
    return l + l2;
}


/** コピー. mbcの時は文字が壊れない部分まで. dst == src もok.
 */
size_t  fks_mbsCpyNC(fks_mbcenc_t mbc, char dst[], size_t dstSz, char const* src, size_t nc)
{
    size_t    l;
    FKS_ASSERT(dst != NULL && dstSz > 0 && src != 0 && dst != src);
    l = fks_mbsChrsToSize(mbc, src, nc) + 1;
    l = dstSz < l ? dstSz : l;
    return fks_mbsCpy(mbc, dst, l, src);
}


/** 文字列の連結.
 */
size_t   fks_mbsCatNC(fks_mbcenc_t mbc, char dst[], size_t dstSz, char const* src, size_t nc)
{
    size_t l, l2;
    FKS_ASSERT(dst != NULL && dstSz > 0 && src != 0 && dst != src);
    l = mbc_raw_len(dst);
    if (l >= dstSz) {   // そもそも転送先が満杯ならサイズ調整のみ.
        return fks_mbsCpy(mbc, dst, dstSz, dst);
    }
    dstSz -= l;
    l2 = fks_mbsChrsToSize(mbc, src, nc) + 1;
    l2 = dstSz < l2 ? dstSz : l2;
    l2 = fks_mbsCpy(mbc, dst+l, l2, src);
    return l + l2;
}


/** コピー. mbcの時は文字が壊れない部分まで. dst == src もok.
 */
size_t fks_mbsCpyWidth(fks_mbcenc_t mbc, char dst[], size_t dstSz, char const* src, size_t width)
{
    size_t    l = fks_mbsWidthToSize(mbc, src, width) + 1;
    l = dstSz < l ? dstSz : l;
    return fks_mbsCpy(mbc, dst, l, src);
}


/** 文字列の連結.
 */
size_t  fks_mbsCatWidth(fks_mbcenc_t mbc, char dst[], size_t dstSz, char const* src, size_t width)
{
    size_t l, l2;
    FKS_ASSERT(dst != NULL && dstSz > 0 && src != 0 && dst != src);
    l = mbc_raw_len(dst);
    if (l >= dstSz) {   // そもそも転送先が満杯ならサイズ調整のみ.
        return fks_mbsCpy(mbc, dst, dstSz, dst);
    }
    dstSz -= l;
    l2  = fks_mbsWidthToSize(mbc, src, width) + 1;
    l2  = dstSz < l2 ? dstSz : l2;
    l2  = fks_mbsCpy(mbc, dst+l, l2, src);
    return l + l2;
}


// --------------------------------------------------------------------------
// UNICODE

int fks_mbsCheckUnicodeBOMi(char const* src, size_t len)
{
    unsigned char const* b = (unsigned char const*)src;
    if (len < 2)
        return 0;
    // Check Unicode BOM
    if (len >= 3 && b[0] == 0xEF && b[1] == 0xBB && b[2] == 0xBF) {
        return 1;       // UTF8:BOM
    } else if (len >= 2 && b[0] == 0xFF && b[1] == 0xFE) {
        if (len >= 4 && b[2] == 0x00 && b[3] == 0x00) {
            return 4; // UTF32LE:BOM
        } else {
            return 2;   // UTF16LE:BOM
        }
    } else if (len >= 2 && b[0] == 0xFE && b[1] == 0xFF) {
        return 3;       // UTF16BE:BOM
    } else if (len >= 4 && b[0] == 0x00 && b[1] == 0x00 && b[2] == 0xFE && b[3] == 0xFF) {
        return 5;       // UTF32BE:BOM
    }
    return 0;
}

fks_mbcenc_t fks_mbsCheckUnicodeBOM(char const* src, size_t len)
{
    static fks_mbcenc_t s_tbl[] = {
        NULL,
        &fks_mbsEnc_utf8,
        &fks_mbsEnc_utf16le,
        &fks_mbsEnc_utf16be,
        &fks_mbsEnc_utf32le,
        &fks_mbsEnc_utf32be,
    };
    return s_tbl[ fks_mbsCheckUnicodeBOMi(src, len) ];
}


int Fks_MbcEnvToCheckUnicodeBomNumber(fks_mbcenc_t env) {
    if (env == fks_mbc_utf8) {
        return 1;
    } else if (env == fks_mbc_utf16le) {
        return 2;
    } else if (env == fks_mbc_utf16be) {
        return 3;
    } else if (env == fks_mbc_utf32le) {
        return 4;
    } else if (env == fks_mbc_utf32be) {
        return 5;
    }
    return 0;
}


size_t   fks_mbsConvUnicode(fks_mbcenc_t dstMbc, char dst[], size_t dstSz, fks_mbcenc_t srcMbc, char const* src, size_t srcSz)
{
    FKS_ARG_ASSERT(1, Fks_MbcEnvToCheckUnicodeBomNumber(dstMbc) > 0);
    FKS_ARG_PTR_ASSERT(2, dst);
    FKS_ARG_ASSERT(3, dstSz > 0);
    FKS_ARG_ASSERT(4, Fks_MbcEnvToCheckUnicodeBomNumber(srcMbc) > 0);
    FKS_ARG_PTR_ASSERT(5, src);

    if (dstMbc == srcMbc) {
        return fks_mbsLCpy(dstMbc, dst, dstSz, src, srcSz);
    } else {
        char const* s  = src;
        char const* se = src + srcSz;
        char* d = dst;
        char* de = dst + dstSz;
        unsigned (*getC)(char const** s) = srcMbc->getC;
        char*    (*setC)(char* dst, char* dstEnd, unsigned c) = dstMbc->setC;
        while (d < de && s < se) {
            unsigned c = getC(&s);
            if (!c)
                break;
            d = setC(d, de, c);
        }
        if (d < de)
            *d = 0;
        return d - dst;
    }
}


#ifdef FKS_WIN32
fks_mbcenc_t fks_mbc_makeDBC(Fks_MbcEnc* mbcEnv, fks_codepage_t cp) {
    *mbcEnv = fks_mbsEnc_dbc;
    mbcEnv->cp = cp;
    return mbcEnv;
}

#if 0
static unsigned fks_dbcToUtf32(fks_mbcenc_t srcMbc, unsigned dbc)
{
    uint8_t     buf[8];
    uint8_t     p;
    wchar_t     wbuf[8] = {0};
    int         l;
    unsigned    wc;
    if (dbc < 0x80)
        return dbc;
    p = buf;
    if (dbc >> 24) *p++ = dbc >> 24;
    if (dbc >> 16) *p++ = dbc >> 16;
    if (dbc >>  8) *p++ = dbc >> 8;
    *p++ = dbc;
    *p = 0;
    l = MultiByteToWideChar(srcMbc->cp,0, buf, p - buf, wbuf, 16);
    wc = wbuf[0];
    if (wc >= 0xD800 && wc <= 0xDBFF && wbuf[1] >= 0xDC00 && wbuf[1] <= 0xDFFF)
        wc = (wc << 16) | wbuf[1];
    return wc;
}

static unsigned fks_utf32ToDbc(fks_mbcenc_t dstMbc, unsigned utf32)
{
    uint8_t     buf[8] = {0};
    wchar_t     wp;
    wchar_t     wbuf[4];
    int         l, i;
    unsigned    c;
    if (utf32 < 0x80)
        return utf32;
    wp = wbuf;
    if (utf32 > 0xffff)
        *wp++ = utf32 >> 16;
    *wp++ = utf32;
    *wp = 0;
    l = WideCharToMultiByte(dstMbc->cp,0,wbuf,wp - wbuf,buf,8,0,0);
    c = 0;
    for (i = 0; i < l; ++i) {
        c = (c << 8)|buf[i];
    }
    return c;
}
#endif

static size_t   fks_mbsDbcFromKnownEnc(fks_mbcenc_t dstMbc, char dst[], size_t dstSz, fks_mbcenc_t srcMbc, char const* src, size_t srcSz)
{
    size_t          tl;
    FKS_ARG_PTR_ASSERT(2, dst);
    FKS_ARG_ASSERT(3, dstSz >= 4);
    FKS_ARG_PTR_ASSERT(4, srcMbc);
    FKS_ARG_PTR_ASSERT(5, src);

    tl = (srcMbc == fks_mbc_utf8) ? srcSz * 3 : (srcMbc == fks_mbc_utf16be) ? srcSz : srcSz*2;
  #if 0
    if (tl < 0x7fffffff)
  #endif
    {
        wchar_t const*  ws  = (wchar_t const*)src;
        wchar_t*        mlc = NULL;
        size_t          wsl = srcSz / sizeof(*ws);
        int             l;
        if (srcMbc != fks_mbc_utf16le) {
            if (tl < 0x4000)
                ws  = (wchar_t*)fks_alloca(tl);
            else
                ws  = mlc = (wchar_t*)fks_malloc(tl);
         #ifdef FKS_USE_MBC_JIS
            wsl = fks_mbsConvJisType(fks_mbc_utf16le, (char*)ws, tl, srcMbc, src, srcSz);
         #else
            wsl = fks_mbsConvUnicode(fks_mbc_utf16le, (char*)ws, tl, srcMbc, src, srcSz);
         #endif
            wsl /= sizeof(*ws);
        }
        if (*ws == 0xFEFF) {
            ++ws;
            --wsl;
        }
        l = WideCharToMultiByte(dstMbc->cp,0,ws,wsl,dst,dstSz,0,0);
        if (mlc)
            fks_free(mlc);
        return l;
    }
 #if 0
    else {
        char const* s  = src;
        char const* se = src + srcSz;
        char* d = dst;
        char* de = dst + dstSz;
     #ifdef FKS_USE_MBC_JIS
        unsigned (*getC)(char const** s) = fks_mbc_getToUtf32Func(srcMbc);
     #else
        unsigned (*getC)(char const** s) = srcMbc->getC;
     #endif
        char     (**setC)(char* dst, char* dstEnd, unsigned c) = dstMbc->setC;
        unsigned c = (s < se) ? getC(&s) : 0;
        if (c == 0xFEFF)
            c = (s < se) ? getC(&s) : 0;
        if (c) {
            do {
                if (d >= de)
                    break;
                d = setC(d, de, c);
                if (s >= se)
                    break;
                c = getC(&s);
            } while (c);
        }
        if (d < de)
            *d = 0;
        return d - dst;
    }
 #endif
}

static size_t   fks_mbsKnownEncFromDbc(fks_mbcenc_t dstMbc, char dst[], size_t dstSz, fks_mbcenc_t srcMbc, char const* src, size_t srcSz)
{
    wchar_t * wd  = (wchar_t*)dst;
    size_t    wdl = dstSz;
    wchar_t*  m   = NULL;
    size_t    l;
    if (dstMbc != fks_mbc_utf16le) {
        wdl = srcSz * 4;
        if (wdl <= 0x4000)
            wd = (wchar_t*)fks_alloca(wdl);
        else
            wd = m = (wchar_t*)fks_calloc(1, wdl);
    }
    l = MultiByteToWideChar(srcMbc->cp,0,src,srcSz,wd,wdl/sizeof(*wd));
    if (dstMbc == fks_mbc_utf16le)
        return l * sizeof(wchar_t);
 #ifdef FKS_USE_MBC_JIS
    l = fks_mbsConvJisType(dstMbc, dst, dstSz, fks_mbc_utf16le, (char*)wd, l * sizeof(wchar_t));
 #else
    l = fks_mbsConvUnicode(dstMbc, dst, dstSz, fks_mbc_utf16le, (char*)wd, l * sizeof(wchar_t));
 #endif
    if (m)
        fks_free(m);
    return l;
}

size_t fks_mbsConvCP(fks_codepage_t dcp, char d[], size_t dl, fks_codepage_t scp, char const* s, size_t sl)
{
    FKS_ARG_PTR_ASSERT(1, d);
    FKS_ARG_ASSERT(2, dl > 1);
    FKS_ARG_PTR_ASSERT(3, s);
    if (dcp != scp) {
        wchar_t* m  = NULL;
        size_t   bl;
        size_t   wl = MultiByteToWideChar(scp,0,s,sl,NULL,0);
        size_t   tl = wl * 2 + 2;
        wchar_t* w;
        if (tl <= 0x4000)
            w = (wchar_t*)fks_alloca(tl);
        else
            w = m = (wchar_t*)fks_malloc(tl);
        if (!w)
            return 0;
        w[wl] = 0;
        MultiByteToWideChar(scp,0,s,sl,w,wl);
        bl = WideCharToMultiByte(dcp,0,w,wl,NULL,0,0,0);
        if (bl > dl)
            bl = dl;
        bl = WideCharToMultiByte(dcp,0,w,wl,d,bl,0,0);
        if (m)
            fks_free(m);
        if (bl < dl)
            d[bl] = 0;
        return bl;
    } else {
        sl = strlen(s) + 1;
        if (dl >= sl) {
            dl = sl;
        } else {
            // if (dl < 1) return NULL;
            // dl = fks_mbsAdjustSize(d, dl-1);
        }
        d[dl-1] = 0;
        if (d != s)
            memmove(d, s, dl);
        return dl-1;
    }
}

#endif



// --------------------------------------------------------------------------

/**
 */
size_t  fks_mbsCountCapa(fks_mbcenc_t dstMbc, fks_mbcenc_t srcMbc, char const* src, size_t srcSz)
{
    char const* s  = src;
    char const* se = src + srcSz;
    unsigned (*getC)(char const** s) = srcMbc->getC;
    size_t sz = 0;
    int  dstUni;
    int  srcUni;
    if (dstMbc == srcMbc) {
        return srcSz;
    }
    dstUni = Fks_MbcEnvToCheckUnicodeBomNumber(dstMbc);
    srcUni = Fks_MbcEnvToCheckUnicodeBomNumber(srcMbc);
    if (dstUni > 0) {
        if (srcUni > 0) {
            unsigned (*chrLen)(unsigned chr) = dstMbc->chrLen;
            while (s < se) {
                unsigned c = getC(&s);
                if (!c)
                    break;
                sz += chrLen(c);
            }
        } else {
            static uint8_t const s_tbl[] = {0,1,2,2,4,4,};
            size_t c1 = s_tbl[dstUni];
            while (s < se) {
                unsigned c = getC(&s);
                if (!c)
                    break;
                sz += (c < 0x80) ? c1 : 4;
            }
            return sz;
        }
 #ifdef FKS_USE_MBC_JIS
    } else if (dstMbc == fks_mbc_sjisX213) {
        if (srcMbc == fks_mbc_eucjp) {
            sz = srcSz;
        } else {
            while (s < se) {
                unsigned c = getC(&s);
                if (!c)
                    break;
                sz += (c < 0x80) ? 1 : 2;
            }
        }
    } else if (dstMbc == fks_mbc_eucjp) {
        while (s < se) {
            unsigned c = getC(&s);
            if (!c)
                break;
            sz += (c < 0x80) ? 1 : 3;
        }
 #endif
    } else {
        while (s < se) {
            unsigned c = getC(&s);
            sz += (c < 0x80) ? 1 : 4;
        }
    }
    return sz;
}

size_t   fks_mbsConv(fks_mbcenc_t dstMbc, char dst[], size_t dstSz, fks_mbcenc_t srcMbc, char const* src, size_t srcSz)
{
    FKS_ARG_PTR_ASSERT(1, dstMbc);
    FKS_ARG_PTR_ASSERT(2, dst);
    FKS_ARG_ASSERT(3, dstSz > 0);
    FKS_ARG_PTR_ASSERT(4, srcMbc);
    FKS_ARG_PTR_ASSERT(5, src);

 #ifdef FKS_WIN32
    if (dstMbc == srcMbc) {
        return fks_mbsLCpy(dstMbc, dst, dstSz, src, srcSz);
    } else {
      #ifdef FKS_USE_MBC_JIS
        int srcNo = fks_mbc_encToJisTypeNo(srcMbc)+1;   // 0:unicode 1:eucjp 2:jisX213 3:cp932
        int dstNo = fks_mbc_encToJisTypeNo(dstMbc)+1;
      #else
        int srcNo = Fks_MbcEnvToCheckUnicodeBomNumber(srcMbc);
        int dstNo = Fks_MbcEnvToCheckUnicodeBomNumber(dstMbc);
      #endif
        if (srcNo > 0 && dstNo > 0) {
          #ifdef FKS_USE_MBC_JIS
            return fks_mbsConvJisType(dstMbc, dst, dstSz, srcMbc, src, srcSz);
          #else
            return fks_mbsConvUnicode(dstMbc, dst, dstSz, srcMbc, src, srcSz);
          #endif
        } else if (dstNo > 0) {
            return fks_mbsKnownEncFromDbc(dstMbc, dst, dstSz, srcMbc, src, srcSz);
        } else if (srcNo > 0) {
            return fks_mbsDbcFromKnownEnc(dstMbc, dst, dstSz, srcMbc, src, srcSz);
        } else {
            return fks_mbsConvCP(dstMbc->cp, dst, dstSz, srcMbc->cp, src, srcSz);
        }
    }
 #else
  #ifdef FKS_USE_MBC_JIS
    return fks_mbsConvJisType(dstMbc, dst, dstSz, srcMbc, src, srcSz);
  #else
    return fks_mbsConvUnicode(dstMbc, dst, dstSz, srcMbc, src, srcSz);
  #endif
 #endif
}

char* fks_mbsConvMalloc(fks_mbcenc_t dstMbc, fks_mbcenc_t srcMbc, char const* src, size_t srcSz, size_t* pDstSz)
{
    char* d;
    size_t l;
    size_t bytes = fks_mbsCountCapa(dstMbc, srcMbc, src, srcSz);
    if (bytes == 0)
        return NULL;
    d = (char*)fks_calloc(1, bytes + 4);
    l = fks_mbsConv(dstMbc, d, bytes, srcMbc, src, srcSz);
    if (l > 0) {
        if (pDstSz)
            *pDstSz = l;
        return d;
    } else {
        fks_free(d);
        return NULL;
    }
}

/**
 *  @param  src             Source string
 *  @param  len             Check range. ('\0' is not EOS but broken character)
 *  @param  canEndBroken    Can the end be broken?
 */
fks_mbcenc_t fks_mbsAutoCharEncoding(char const* src, size_t len, int canEndBroken,fks_mbcenc_t encTbl[],size_t encNum)
{
    static fks_mbcenc_t const s_tbl[] = {
        &fks_mbsEnc_utf8,
     #if defined FKS_WIN32
        &fks_mbsEnc_cp932,
        &fks_mbsEnc_dbc,
        //&fks_mbsEnc_utf32le,
        //&fks_mbsEnc_utf16le,
     #else
      #ifdef FKS_USE_MBC_JIS
        &fks_mbsEnc_eucjp,
        //&fks_mbsEnc_cp932,
      #endif
     #endif
    };
    enum { TBL_SZ = sizeof(s_tbl) / sizeof(s_tbl[0]) };
    int i, k, rnk, fndIdx;

    fks_mbcenc_t uck = fks_mbsCheckUnicodeBOM(src, len);
    if (uck)
        return uck;

    if (!encTbl) {
        encTbl = (fks_mbcenc_t*)s_tbl;
        encNum = TBL_SZ;
    }

    rnk    = -1;
    fndIdx = -1;

    for (i = 0; i < encNum; ++i) {
        k = encTbl[i]->checkEncoding(src, len, canEndBroken);
        if (k == 1) {
            fndIdx = i;
            break;
        }
        if (k > rnk) {
            rnk = k;
            fndIdx = i;
        }
    }
    if (fndIdx >= 0) {
        return encTbl[fndIdx];
    }
    return encTbl[0];
}

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
size_t  fks_mbsCountCapa(fks_mbcenc_t dstMbc, fks_mbcenc_t srcMbc, unsigned char const* src, size_t srcSz) {
    return fks_mbsCountCapa(dstMbc, srcMbc, (char const*)src, srcSz);
}

size_t   fks_mbsConv(fks_mbcenc_t dstMbc, unsigned char dst[], size_t dstSz, fks_mbcenc_t srcMbc, unsigned  char const* src, size_t srcSz)
{
    return fks_mbsConv(dstMbc, (char*)dst, dstSz, srcMbc, (char const*)src, srcSz);
}

fks_mbcenc_t fks_mbsAutoCharEncoding(unsigned char const* src, size_t len, int canEndBroken, fks_mbcenc_t encTbl[], size_t encNum)
{
    return fks_mbsAutoCharEncoding((char*)src, len, canEndBroken, encTbl, encNum);
}
#endif
