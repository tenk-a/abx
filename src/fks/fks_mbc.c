/*
 *  @file   fks_mbc.c
 *  @brief  Multi Byte Character lib.
 *  @author Masashi Kitamura (tenka@6809.net)
 *  @license Boost Software Lisence Version 1.0
 */

#include <fks_common.h>
#include <fks_mbc.h>
#include <assert.h>

#ifdef _WIN32
#include <windows.h>
#ifdef _MSC_VER
#define strncasecmp     _strnicmp
#else
#define strncasecmp     strnicmp
#endif
#endif

#ifdef _MSC_VER
#pragma warning (disable:4244)
#endif

#ifdef __cplusplus
extern "C" {
#endif

static inline size_t  mbc_raw_len(const char* s) {
	const char* p = s;
	assert(s);
	--p; do {} while (*++p);
	return p - s;
}


#define MBC_IMPL(nm)											\
size_t    nm##_adjustSize(const char* str, size_t size) {		\
    const char* s = str;										\
    const char* b = s;											\
    const char* e = s + size;									\
    assert(str != 0 && size > 0);								\
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
size_t  nm##_sizeToChrs(const char* str, size_t size) {			\
    const char* s = str;										\
    const char* e = s + size;									\
    size_t      l = 0;											\
    if (e < s)													\
        e = (const char*)(~(size_t)0);							\
    assert(str != 0 && size > 0);								\
    while (s < e) {												\
        unsigned c;												\
        c  = nm##_getc(&s);										\
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
    assert(str != 0);											\
    while (chrs) {												\
        unsigned c  = nm##_getc(&s);							\
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
    assert(lp != NULL);											\
    assert(rp != NULL);											\
    do {														\
        lc = nm##_getc(&lp);									\
        rc = nm##_getc(&rp);									\
        d  = lc - rc;											\
    } while (d == 0 && lc);										\
    return d;													\
}																\
/**/


// ---------------------------------------------------------------------------
// utf8

/** 全角の1バイト目か?
 */
static unsigned utf8_islead(unsigned c) {
    return c >= 0x80;
}


/** 文字コードが正しい範囲にあるかチェック. (\0もBOMもOKとする)
 */
static unsigned utf8_chkc(unsigned c)
{
    return 1;
}


static unsigned utf8_getc(char const** pStr) {
    const unsigned char* s = (unsigned char*)*pStr;
    unsigned       c       = *s++;
	if (!c)
		return c;
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


static char* utf8_charNext(char const* pChr) {
    const unsigned char* s = (unsigned char*)pChr;
    unsigned       c = *s;
    if (!c)
        return (char*)s;
    ++s;
    if (c < 0x80) {
        return (char*)s;
    } else if (*s) {
        ++s;
        if (c < 0xE0) {
            return (char*)s;
        } else if (*s) {
            ++s;
            if (c < 0xF0) {
                return (char*)s;
            } else if (*s) {
                ++s;
                if (c < 0xF8) {
                    return (char*)s;
                } else if (*s) {
                    ++s;
                    if (c < 0xFC) {
                        return (char*)s;
                    } else if (*s) {
                        ++s;
                        return (char*)s;
                    }
                }
            }
        }
    }
    return (char*)s;
}


/** 一字取り出し.
 */
static unsigned utf8_peekc(const char* s) {
    return utf8_getc(&s);
}


/** 1文字のchar数を返す.
 */
static unsigned utf8_len1(const char* pChr) {
    const unsigned char* s = (unsigned char*)utf8_charNext(pChr);
    return (const char*)s - pChr;
}


/** 1字書き込み.
 */
static char*    utf8_setc(char*  dst, unsigned c) {
    char* d = dst;
    if (c < 0x80) {
        *d++ = c;
    } else {
        if (c <= 0x7FF) {
            *d++ = 0xC0|(c>>6);
            *d++ = 0x80|(c&0x3f);
        } else if (c <= 0xFFFF) {
            *d++ = 0xE0|(c>>12);
            *d++ = 0x80|((c>>6)&0x3f);
            *d++ = 0x80|(c&0x3f);
            //if (c >= 0xff60 && c <= 0xff9f) {--(*adn); }  // 半角カナなら、半角文字扱い.
        } else if (c <= 0x1fFFFF) {
            *d++ = 0xF0|(c>>18);
            *d++ = 0x80|((c>>12)&0x3f);
            *d++ = 0x80|((c>>6)&0x3f);
            *d++ = 0x80|(c&0x3f);
        } else if (c <= 0x3fffFFFF) {
            *d++ = 0xF8|(c>>24);
            *d++ = 0x80|((c>>18)&0x3f);
            *d++ = 0x80|((c>>12)&0x3f);
            *d++ = 0x80|((c>>6)&0x3f);
            *d++ = 0x80|(c&0x3f);
        } else {
            *d++ = 0xFC|(c>>30);
            *d++ = 0x80|((c>>24)&0x3f);
            *d++ = 0x80|((c>>18)&0x3f);
            *d++ = 0x80|((c>>12)&0x3f);
            *d++ = 0x80|((c>>6)&0x3f);
            *d++ = 0x80|(c&0x3f);
        }
    }
    return d;
}


/** 1文字のchar数を返す.
 */
static unsigned utf8_chrLen(unsigned c) {
 #if 0  // あとで
    if (c <= 0x7FF) {
        if (c < 0x80)
            return 1;
        return 2;
    }
    if (c <= 0xFFFF)
        return 3;
    if (c <= 0x1fFFFF)
        return 4;
    if (c <= 0x3fffFFFF)
        return 5;
    return 6;
 #else
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
 #endif
}


/** 半角全角を考慮して文字の幅を返す.(面倒なんですべて同一サイズ扱い)
 */
static unsigned utf8_chrWidth(unsigned chr) {
    //chr;
    return 1;
}


/** 半角全角を考慮して文字の幅を返す. (日本語フォントを想定).
 */
static unsigned utf8_jp_chrWidth(unsigned c) {
    if (c < 0x370)
        return 1;
    if (c >= 0xff60 && c <= 0xff9f)
        return 1;
    return 2;
}

MBC_IMPL(utf8)

static Fks_MbcEnv const fks_mbcEnv_utf8 = {
    utf8_islead,                // Cがマルチバイト文字の1バイト目か?
    utf8_chkc,                  // 文字コードが正しい範囲にあるかチェック.
    utf8_getc,                  // 1字取り出し＆ポインタ更新.
    utf8_peekc,                 // 一字取り出し.
	utf8_charNext,
    utf8_setc,                  // 1字書き込み.
    utf8_len1,                  // 1文字のchar数を返す.
    utf8_chrLen,                // 1文字のchar数を返す.
    utf8_chrWidth,              // 半角全角を考慮して文字の幅を返す.
    utf8_jp_chrWidth,           // 半角全角を考慮して文字の幅を返す.
	utf8_adjustSize,
	utf8_chrsToSize,
	utf8_sizeToChrs,
	utf8_cmp,
};
Fks_MbcEnv const* const fks_mbc_utf8 = &fks_mbcEnv_utf8;



// ---------------------------------------------------------------------------
#define REV_U16(c)		((uint8_t)((c) >> 8) | ((uint8_t)(c) << 8))

#ifdef FKS_BIG_ENDIAN
#define GET16LE(p)		REV_U16(*(uint16_t*)(p))
#define SET16LE(p, c)	(*(uint16_t*)(p) = REV_U16(c))
#else
#define GET16LE(p)		(*(uint16_t*)(p))
#define SET16LE(p, c)	(*(uint16_t*)(p) = (c))
#endif

static unsigned utf16le_getc(char const** ppSrc) {
	uint16_t const* p = (uint16_t const*)*ppSrc;
	uint32_t        c = GET16LE(p);
	if (c)
		++p;
	if (0xD800 <= c && c <= 0xDBFF) {
		uint32_t	d = GET16LE(p);
		if (0xDC00 <= d && d <= 0xDFFF) {
			++p;
			c = ((c & 0x3ff) << 10) | (d & 0x3ff);
			c += 0x10000;
		}
	}
	*ppSrc = (char const*)p;
	return c;
}

static unsigned utf16le_peekc(char const* pSrc) {
	return utf16le_getc((char const**)&pSrc);
}

static char* utf16le_setc(char* pDst, unsigned c) {
	if (c < 0x10000) {
		SET16LE(pDst, c);
		if (c)
			pDst += 2;
	} else if (c <= 0x10FFFF) {
		uint32_t x = c - 0x10000;
		c = (x >> 10) | 0xD800;
		SET16LE(pDst, c);
		pDst += 2;
		c = (x & 0x3ff) | 0xDC00;
		SET16LE(pDst, c);
		pDst += 2;
	} else {	// error
		//SET16LE(pDst, c);
		//pDst += 2;
	}
	return pDst;
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
		uint32_t	d = GET16LE(p);
		if (0xDC00 <= d && d <= 0xDFFF) {
			++p;
		}
	}
	return (char*)p;
}

static unsigned utf16le_len1(char const* p) {
	return utf16le_charNext(p) - p;
}

MBC_IMPL(utf16le)

static Fks_MbcEnv const fks_mbcEnv_utf16le = {
    utf8_islead,
    utf8_chkc,
    utf16le_getc,
	utf16le_peekc,
	utf16le_charNext,
    utf16le_setc,
    utf16le_len1,
    utf16le_chrLen,
    utf8_chrWidth,
    utf8_jp_chrWidth,
	utf16le_adjustSize,
	utf16le_chrsToSize,
	utf16le_sizeToChrs,
	utf16le_cmp,
};
Fks_MbcEnv const* const fks_mbc_utf16le = &fks_mbcEnv_utf16le;



// ---------------------------------------------------------------------------

#ifdef FKS_BIG_ENDIAN
#define GET16BE(p)		REV_U16(*(uint16_t*)(p))
#define SET16BE(p, c)	(*(uint16_t*)(p) = REV_U16(c))
#else
#define GET16BE(p)		(*(uint16_t*)(p))
#define SET16BE(p, c)	(*(uint16_t*)(p) = (c))
#endif


static unsigned utf16be_getc(char const** ppSrc) {
	uint16_t const* p = (uint16_t const*)*ppSrc;
	uint32_t        c = GET16BE(p);
	if (c)
		++p;
	if (0xD800 <= c && c <= 0xDBFF) {
		uint32_t	d = GET16BE(p);
		if (0xDC00 <= d && d <= 0xDFFF) {
			++p;
			c = ((c & 0x3ff) << 10) | (d & 0x3ff);
			c += 0x10000;
		}
	}
	*ppSrc = (char const*)p;
	return c;
}

static unsigned utf16be_peekc(char const* pSrc) {
	return utf16be_getc((char const**)&pSrc);
}


static char* utf16be_setc(char* pDst, unsigned c) {
	if (c < 0x10000) {
		SET16BE(pDst, c);
		if (c)
			pDst += 2;
	} else if (c <= 0x10FFFF) {
		uint32_t x = c - 0x10000;
		c = (x >> 10) | 0xD800;
		SET16BE(pDst, c);
		pDst += 2;
		c = (x & 0x3ff) | 0xDC00;
		SET16BE(pDst, c);
		pDst += 2;
	} else {	// error
		//SET16BE(pDst, c);
		//pDst += 2;
	}
	return pDst;
}


static char* utf16be_charNext(char const* s) {
	uint16_t const* p = (uint16_t const*)s;
	uint32_t        c = GET16BE(p);
	if (c)
		++p;
	if (0xD800 <= c && c <= 0xDBFF) {
		uint32_t	d = GET16BE(p);
		if (0xDC00 <= d && d <= 0xDFFF) {
			++p;
		}
	}
	return (char*)p;
}

static FKS_FORCE_INLINE unsigned utf16be_len1(char const* p) {
	return utf16be_charNext(p) - p;
}

static FKS_FORCE_INLINE unsigned utf16be_chrLen(unsigned c) {
	return c > 0xffff ? 4 : c ? 2 : 0;
}


MBC_IMPL(utf16be)

static Fks_MbcEnv const fks_mbcEnv_utf16be = {
    utf8_islead,
    utf8_chkc,
    utf16be_getc,
	utf16be_peekc,
	utf16be_charNext,
    utf16be_setc,
    utf16be_len1,
    utf16be_chrLen,
    utf8_chrWidth,
    utf8_jp_chrWidth,
	utf16be_adjustSize,
	utf16be_chrsToSize,
	utf16be_sizeToChrs,
	utf16be_cmp,
};
Fks_MbcEnv const* const fks_mbc_utf16be = &fks_mbcEnv_utf16be;



// ---------------------------------------------------------------------------
#define REV_U32(c)		((uint8_t)((c) >> 24)| ((uint8_t)((c) >> 16) << 8) | ((uint8_t)((c) >> 8)<<16) | ((uint8_t)(c) << 24))

#ifdef FKS_BIG_ENDIAN
#define GET32LE(p)		REV_U32(*(uint32_t*)(p))
#define SET32LE(p, c)	(*(uint32_t*)(p) = REV_U32(c))
#else
#define GET32LE(p)		(*(uint32_t*)(p))
#define SET32LE(p, c)	(*(uint32_t*)(p) = (c))
#endif

static unsigned utf32le_getc(char const** ppSrc) {
	uint32_t const* p = (uint32_t const*)*ppSrc;
	uint32_t        c = GET32LE(p);
	if (c)
		++p;
	*ppSrc = (char const*)p;
	return c;
}

static unsigned utf32le_peekc(char const* pSrc) {
	return GET32LE(pSrc);
}

static char* utf32le_setc(char* pDst, unsigned c) {
	SET32LE(pDst, c);
	return pDst + 4;
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

MBC_IMPL(utf32le)

static Fks_MbcEnv const fks_mbcEnv_utf32le = {
    utf8_islead,
    utf8_chkc,
    utf32le_getc,
	utf32le_peekc,
	utf32le_charNext,
    utf32le_setc,
    utf32le_len1,
    utf32le_chrLen,
    utf8_chrWidth,
    utf8_jp_chrWidth,
	utf32le_adjustSize,
	utf32le_chrsToSize,
	utf32le_sizeToChrs,
	utf32le_cmp,
};
Fks_MbcEnv const* const fks_mbc_utf32le = &fks_mbcEnv_utf32le;



// ---------------------------------------------------------------------------
#ifdef FKS_BIG_ENDIAN
#define GET32BE(p)		(*(uint32_t*)(p))
#define SET32BE(p, c)	(*(uint32_t*)(p) = (c))
#else
#define GET32BE(p)		REV_U32(*(uint32_t*)(p))
#define SET32BE(p, c)	(*(uint32_t*)(p) = REV_U32(c))
#endif

static unsigned utf32be_getc(char const** ppSrc) {
	uint32_t const* p = (uint32_t const*)*ppSrc;
	uint32_t        c = GET32BE(p);
	if (c)
		++p;
	*ppSrc = (char const*)p;
	return c;
}

static unsigned utf32be_peekc(char const* pSrc) {
	return GET32BE(pSrc);
}

static char* utf32be_setc(char* pDst, unsigned c) {
	SET32BE(pDst, c);
	return pDst + 4;
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

MBC_IMPL(utf32be)

static Fks_MbcEnv const fks_mbcEnv_utf32be = {
    utf8_islead,
    utf8_chkc,
    utf32be_getc,
	utf32be_peekc,
	utf32be_charNext,
	utf32be_setc,
    utf32be_len1,
    utf32be_chrLen,
	utf8_chrWidth,
	utf8_jp_chrWidth,
	utf32be_adjustSize,
	utf32be_chrsToSize,
	utf32be_sizeToChrs,
	utf32be_cmp,
};
Fks_MbcEnv const* const fks_mbc_utf32be = &fks_mbcEnv_utf32be;



// ---------------------------------------------------------------------------
// DBC

/** 1字書き込み.
 */
static char*    dbc_setc(char*  d, unsigned c) {
    if (c > 0xff) {
        *d++ = c >> 8;
    }
    *d++ = c;
    return d;
}


/** 1文字のchar数を返す.
 */
static unsigned dbc_chrLen(unsigned chr) {
    return 1 + (chr > 0xff);
}


/** 半角全角を考慮して文字の幅を返す.
 */
static unsigned dbc_chrWidth(unsigned chr) {
    // とりあえず特殊なNEC半角は無視...
    return 1 + (chr > 0xff);
}


// ---------------------------------------------------------------------------

#ifdef FKS_WIN32
/** 全角の1バイト目か?
 */
static unsigned dbc_islead(unsigned c) {
    return IsDBCSLeadByte((unsigned char)c);
}


static unsigned dbc_istrail(unsigned c) {
    return ((c >= 0x30) & (c <= 0xFE)) && c != 0x7f;
}


#if 0
static unsigned dbc_istrailp(char const* p) {
    return dbc_istrail(*(unsigned char*)p);
}
#endif


/** 文字コードが正しい範囲にあるかチェック.
 */
static unsigned dbc_chkc(unsigned c)
{
    if (c > 0xff) {
        return IsDBCSLeadByte(c >> 8) && dbc_istrail((unsigned char)c);
    }
    return 1;
}


/** 1字取り出し＆ポインタ更新.
 */
static unsigned dbc_getc(const char** pStr) {
    const unsigned char* s = (unsigned char*)*pStr;
    unsigned       c       = *s++;
    if (IsDBCSLeadByte(c) && *s) {
        c = (c << 8) | *s++;
    }
    *pStr = (const char*)s;
    return c;
}


/** 一字取り出し.
 */
static unsigned dbc_peekc(const char* s) {
    unsigned       c       = *(unsigned char*)(s++);
    if (IsDBCSLeadByte(c) && *s) {
        c = (c << 8) | *s;
    }
    return c;
}


/** 1文字のchar数を返す.
 */
static unsigned dbc_len1(const char* pChr) {
    return (pChr[0] == 0) ? 0 : 1 + (IsDBCSLeadByte(pChr[0]) && pChr[1]);
}

static char* dbc_charNext(char const* pChr) {
	return (char*)pChr + dbc_len1(pChr);
}

MBC_IMPL(dbc)

static Fks_MbcEnv const fks_mbcEnv_dbc = {
    dbc_islead,                 // Cがマルチバイト文字の1バイト目か?
    dbc_chkc,                   // 文字コードが正しい範囲にあるかチェック.
    dbc_getc,                   // 1字取り出し＆ポインタ更新.
    dbc_peekc,                  // 一字取り出し.
	dbc_charNext,
    dbc_setc,                   // 1字書き込み.
    dbc_len1,                   // 1文字のchar数を返す.
    dbc_chrLen,                 // 1文字のchar数を返す.
    dbc_chrWidth,               // 半角全角を考慮して文字の幅を返す.
    dbc_chrWidth,               // 半角全角を考慮して文字の幅を返す.
	dbc_adjustSize,
	dbc_chrsToSize,
	dbc_sizeToChrs,
	dbc_cmp,
};
Fks_MbcEnv const* const fks_mbc_dbc = &fks_mbcEnv_dbc;

#endif



// ---------------------------------------------------------------------------

/** 全角の1バイト目か?
 */
static unsigned sjis_islead(unsigned c) {
    return (c >= 0x81) && ((c <= 0x9F) || ((c >= 0xE0) & (c <= 0xFC)));
}



static unsigned sjis_istrail(unsigned c) {
    return ((c >= 0x40 && c <= 0x7e) || (c >= 0x81 && c <= 0xFC));
}


#if 0
static unsigned sjis_istrailp(char const* p) {
    return sjis_istrail(*(unsigned char*)p);
}
#endif


/** 文字コードが正しい範囲にあるかチェック.
 */
static unsigned sjis_chkc(unsigned c)
{
    if (c > 0xff) {
        return sjis_islead(c >> 8) && sjis_istrail((unsigned char)c);
    }
    return 1;
}


/** 1字取り出し＆ポインタ更新.
 */
static unsigned sjis_getc(const char** pStr) {
    const unsigned char* s = (unsigned char*)*pStr;
    unsigned       c       = *s++;
    if (sjis_islead(c) && *s) {
        c = (c << 8) | *s++;
    }
    *pStr = (const char *)s;
    return c;
}


/** 一字取り出し.
 */
static unsigned sjis_peekc(const char* s) {
    unsigned       c       = *(unsigned char*)(s++);
    if (sjis_islead(c) && *s) {
        c = (c << 8) | *s;
    }
    return c;
}


/** 1文字のchar数を返す.
 */
static unsigned sjis_len1(const char* pChr) {
    return (pChr[0] == 0) ? 0 : 1 + (sjis_islead(pChr[0]) && pChr[1]);
}


static char* sjis_charNext(char const* pChr) {
	return (char*)pChr + sjis_len1(pChr);
}


static FKS_FORCE_INLINE unsigned sjis_chrLen(unsigned chr) {
    return 1 + (chr > 0xff);
}

MBC_IMPL(sjis)


Fks_MbcEnv const fks_mbcEnv_sjis = {
    sjis_islead,                    // Cがマルチバイト文字の1バイト目か?
    sjis_chkc,                      // 文字コードが正しい範囲にあるかチェック.
    sjis_getc,                      // 1字取り出し＆ポインタ更新.
    sjis_peekc,                     // 一字取り出し.
	sjis_charNext,
    dbc_setc,                       // 1字書き込み.
    sjis_len1,                      // 1文字のchar数を返す.
    dbc_chrLen,                     // 1文字のchar数を返す.
    dbc_chrWidth,                   // 半角全角を考慮して文字の幅を返す.
    dbc_chrWidth,                   // 半角全角を考慮して文字の幅を返す.
	sjis_adjustSize,
	sjis_chrsToSize,
	sjis_sizeToChrs,
	sjis_cmp,
};
Fks_MbcEnv const* const fks_mbc_sjis = &fks_mbcEnv_sjis;



// ---------------------------------------------------------------------------

/** 全角の1バイト目か?
 */
static unsigned euc_islead(unsigned c) {
    return (c >= 0xA1 && c <= 0xFE);
}


static unsigned euc_istrail(unsigned c) {
    return (c >= 0xA1 && c <= 0xFE);
}


#if 0
static unsigned euc_istrailp(char const* p) {
    return euc_istrail(*(unsigned char*)p);
}
#endif


/** 文字コードが正しい範囲にあるかチェック.
 */
static unsigned euc_chkc(unsigned c)
{
    if (c > 0xff) {
        return euc_islead(c >> 8) && euc_istrail((unsigned char)c);
    }
    return 1;
}


/** 1字取り出し＆ポインタ更新.
 */
static unsigned euc_getc(const char** pStr) {
    const unsigned char* s = (unsigned char*)*pStr;
    unsigned       c       = *s++;
    if (euc_islead(c) && *s) {
        c = (c << 8) | *s++;
    }
    *pStr = (const char*)s;
    return c;
}


/** 一字取り出し.
 */
static unsigned euc_peekc(const char* s) {
    unsigned       c       = *(unsigned char*)(s++);
    if (euc_islead(c) && *s) {
        c = (c << 8) | *s;
    }
    return c;
}


/** 1文字のchar数を返す.
 */
static unsigned euc_len1(const char* pChr) {
    return (pChr[0] == 0) ? 0 : 1 + (euc_islead(pChr[0]) && pChr[1]);
}


static char* euc_charNext(char const* pChr) {
	return (char*)pChr + euc_len1(pChr);
}

static FKS_FORCE_INLINE unsigned euc_chrLen(unsigned chr) {
    return 1 + (chr > 0xff);
}

MBC_IMPL(euc)


static Fks_MbcEnv const fks_mbcEnv_euc = {
    euc_islead,                 // Cがマルチバイト文字の1バイト目か?
    euc_chkc,                   // 文字コードが正しい範囲にあるかチェック.
    euc_getc,                   // 1字取り出し＆ポインタ更新.
    euc_peekc,                  // 一字取り出し.
	euc_charNext,
    dbc_setc,                   // 1字書き込み.
    euc_len1,                   // 1文字のchar数を返す.
    dbc_chrLen,                 // 1文字のchar数を返す.
    dbc_chrWidth,               // 半角全角を考慮して文字の幅を返す.
    dbc_chrWidth,               // 半角全角を考慮して文字の幅を返す.
	euc_adjustSize,
	euc_chrsToSize,
	euc_sizeToChrs,
	euc_cmp,
};
Fks_MbcEnv const* const fks_mbc_euc = &fks_mbcEnv_euc;



// ---------------------------------------------------------------------------

/** 文字コードが正しい範囲にあるかチェック.
 */
static unsigned eucjp_chkc(unsigned c)
{
    if (c > 0xff) {
        if (c > 0xffff) {
            if ((c >> 16) != 0x8f)
                return 0;
            c = (unsigned short)c;
        }
        return euc_islead(c >> 8) && euc_istrail((unsigned char)c);
    }
    return 1;
}



/** 1字取り出し＆ポインタ更新.
 */
static unsigned eucjp_getc(const char** pStr) {
    const unsigned char* s = (unsigned char*)*pStr;
    unsigned       c       = *s++;
    if (euc_islead(c) && *s) {
        unsigned   k  = c;
        c = (c << 8) | *s++;
        if (k == 0x8f && *s) {
            c = (c << 8) | *s++;
        }
    }
    *pStr = (const char*)s;
    return c;
}


/** 一字取り出し.
 */
static unsigned eucjp_peekc(const char* pStr) {
    const unsigned char* s = (unsigned char*)pStr;
    unsigned       		 c = *s++;
    if (euc_islead(c) && *s) {
        unsigned   k  = c;
        c = (c << 8) | *s++;
        if (k == 0x8f && *s) {
            c = (c << 8) | *s++;
        }
    }
    return c;
}


/** 1文字のchar数を返す.
 */
static unsigned eucjp_len1(const char* s) {
    unsigned       c       = *(const unsigned char*)s;
    if (euc_islead(c) && s[1]) {
        if (c == 0x8f && s[2])
            return 3;
        return 2;
    }
    return (s[0] != 0);
}


/** 1字書き込み.
 */
static char*    eucjp_setc(char*  d, unsigned c) {
    if (c > 0xff) {
        if (c > 0xffff)
            *d++ = c >> 16;
        *d++ = c >> 8;
    }
    *d++ = c;
    return d;
}


/** 半角全角を考慮して文字の幅を返す.
 */
static unsigned eucjp_chrWidth(unsigned chr) {
    unsigned h = chr >> 8;
    if (h == 0 || h == 0x8E) {
        return 1;
    }
    return 2;
}


static char* eucjp_charNext(char const* pChr) {
	return (char*)pChr + eucjp_len1(pChr);
}


static unsigned eucjp_chrLen(unsigned chr) {
	return 1 + (chr > 0xff);
}

MBC_IMPL(eucjp)


static Fks_MbcEnv const fks_mbcEnv_eucJp = {
    euc_islead,                     // Cがマルチバイト文字の1バイト目か?
    eucjp_chkc,                     // 文字コードが正しい範囲にあるかチェック.
    eucjp_getc,                     // 1字取り出し＆ポインタ更新.
    eucjp_peekc,                    // 一字取り出し.
	eucjp_charNext,
    eucjp_setc,                     // 1字書き込み.
    eucjp_len1,                     // 1文字のchar数を返す.
    eucjp_chrLen,                   // 1文字のchar数を返す.
    eucjp_chrWidth,                 // 半角全角を考慮して文字の幅を返す.
    eucjp_chrWidth,                 // 半角全角を考慮して文字の幅を返す.
	eucjp_adjustSize,
	eucjp_chrsToSize,
	eucjp_sizeToChrs,
	eucjp_cmp,
};
Fks_MbcEnv const* const fks_mbc_eucJp = &fks_mbcEnv_eucJp;



// ---------------------------------------------------------------------------
#if 0
/** 全角の1バイト目か?
 */
static unsigned big5_islead(unsigned c) {
    return (c >= 0xA1) && ((c <= 0xC6) || ((c >= 0xC9) & (c <= 0xF9)));
}


static unsigned big5_istrail(unsigned c) {
    return ((c >= 0x40 && c <= 0x7e) || (c >= 0xA1 && c <= 0xFE));
}


#if 0
static unsigned big5_istrailp(char const* p) {
    return big5_istrail(*(unsigned char*)p);
}
#endif


/** 文字コードが正しい範囲にあるかチェック.
 */
static unsigned big5_chkc(unsigned c)
{
    if (c > 0xff) {
        return big5_islead(c >> 8) && big5_istrail((unsigned char)c);
    }
    return 1;
}



/** 1字取り出し＆ポインタ更新.
 */
static unsigned big5_getc(const char** pStr) {
    const unsigned char* s = (unsigned char*)*pStr;
    unsigned             c = *s++;
    if (big5_islead(c) && *s) {
        c = (c << 8) | *s++;
    }
    *pStr = (const char *)s;
    return c;
}



/** 一字取り出し
 */
static unsigned big5_peekc(const char* s) {
    unsigned       c       = *(unsigned char*)(s++);
    if (big5_islead(c) && *s) {
        c = (c << 8) | *(unsigned char*)s;
    }
    return c;
}



/** 1文字のchar数を返す.
 */
static unsigned big5_len1(const char* pChr) {
    return (pChr[0] != 0) + (big5_islead(*(const unsigned char*)pChr) && pChr[1]);
}


static char* big5_charNext(char const* pChr) {
	return (char*)pChr + big5_len1(pChr);
}


MBC_IMPL(big5)


static Fks_MbcEnv const fks_mbcEnv_big5 = {
    big5_islead,                    // Cがマルチバイト文字の1バイト目か?
    big5_chkc,                      // 文字コードが正しい範囲にあるかチェック.
    big5_getc,                      // 1字取り出し＆ポインタ更新.
    big5_peekc,                     // 一字取り出し
	big5_charNext,
    dbc_setc,                       // 1字書き込み.
    big5_len1,                      // 1文字のchar数を返す.
    dbc_chrLen,                     // 1文字のchar数を返す.
    dbc_chrWidth,                   // 半角全角を考慮して文字の幅を返す.
    dbc_chrWidth,                   // 半角全角を考慮して文字の幅を返す.
	big5_adjustSize,
	big5_chrsToSize,
	big5_sizeToChrs,
	big5_cmp,
};
Fks_MbcEnv const* const fks_mbc_big5 = &fks_mbcEnv_big5;
#endif


// ---------------------------------------------------------------------------
// gbk, gb18030
#if 0
/** 全角の1バイト目か?
 */
static unsigned gbk_islead(unsigned c) {
    return ((c >= 0x81) & (c <= 0xFE));
}


static unsigned gbk_istrail(unsigned c) {
    return (c >= 0x40 && c <= 0xFE) && c != 0x7f;
}


#if 0
static unsigned gbk_istrailp(char const* p) {
    return gbk_istrail(*(unsigned char*)p);
}
#endif


/** 文字コードが正しい範囲にあるかチェック. 手抜きでかなり甘くしてる.
 */
static unsigned gbk_chkc(unsigned c)
{
    if (c <= 0xff) {
        return 1;
    } else if (c <= 0xffff) {
        return gbk_islead(c >> 8) && gbk_istrail((unsigned char)c);
    } else {
        unsigned a = c >> 24;
        unsigned b = c >> 16;
        unsigned x = c >>  8;
        unsigned y = (unsigned char)c;
        return (gbk_islead(a) && b >= 0x30 && b <= 0x39 && gbk_islead(x) && y >= 0x30 && y <= 0x39);
    }
}


/** 1字取り出し＆ポインタ更新.
 */
static unsigned gbk_getc(const char** ppStr) {
    const unsigned char* s = (unsigned char*)*ppStr;
    unsigned       c       = *s++;
    if (gbk_islead(c) && *s) {
        unsigned k = *s++;
        if (k >= 0x30 && k <= 0x39 && gbk_islead(*s) && s[1]) {
            c = (c << 24) | (k << 16) | (*s << 8) | s[1];
            s += 2;
        } else {
            c = (c << 8) | k;
        }
    }
    *ppStr = (const char*)s;
    return c;
}


/** 一字取り出し.
 */
static unsigned gbk_peekc(const char* pStr) {
    const unsigned char* s = (unsigned char*)pStr;
    unsigned             c = *s++;
    if (gbk_islead(c) && *s) {
        unsigned k = *s++;
        if (k >= 0x30 && k <= 0x39 && gbk_islead(*s) && s[1]) {
            c = (c << 24) | (k << 16) | (*s << 8) | s[1];
        } else {
            c = (c << 8) | k;
        }
    }
    return c;
}


/** 1文字のchar数を返す.
 */
static unsigned gbk_len1(const char* pStr) {
    const unsigned char* s = (unsigned char*)pStr;
    unsigned       c       = *s++;
    if (gbk_islead(c) && *s) {
        unsigned k = *s++;
        if (k >= 0x30 && k <= 0x39 && gbk_islead(*s) && s[1]) {
            return 4;
        } else {
            return 2;
        }
    }
    return c != 0;
}


/** 1字書き込み.
 */
static char*    gbk_setc(char* d, unsigned c) {
    if (c > 0xff) {
        if (c > 0xffff) {
            //if (c > 0xffffff)
                *d++ = c >> 24;
            *d++ = c >> 16;
        }
        *d++ = c >> 8;
    }
    *d++ = c;
    return d;
}


/** 1文字のchar数を返す.
 */
static unsigned gbk_chrLen(unsigned chr) {
    // return 1 + (c > 0xff) + (c > 0xffff) + (c > 0xffffff);
    return 1 + (chr > 0xff) + (chr > 0xffff) * 2;
}


/** 半角全角を考慮して文字の幅を返す... くわしいことわからないので1バイト文字のみ半角扱い.
 */
static unsigned gbk_chrWidth(unsigned chr) {
    return 1 + (chr > 0xff);
}


static char* gbk_charNext(char const* pChr) {
	return (char*)pChr + gbk_len1(pChr);
}


MBC_IMPL(bgk)


static Fks_MbcEnv const fks_mbcEnv_gbk = {
    gbk_islead,                 // Cがマルチバイト文字の1バイト目か?
    gbk_chkc,                   // 文字コードが正しい範囲にあるかチェック.
    gbk_getc,                   // 1字取り出し＆ポインタ更新.
    gbk_peekc,                  // 一字取り出し
	gbk_charNext,
    gbk_setc,                   // 1字書き込み.
    gbk_len1,                   // 1文字のchar数を返す.
    gbk_chrLen,                 // 1文字のchar数を返す.
    gbk_chrWidth,               // 半角全角を考慮して文字の幅を返す.
    gbk_chrWidth,               // 半角全角を考慮して文字の幅を返す.
	gbk_adjustSize,
	gbk_chrsToSize,
	gbk_sizeToChrs,
	gbk_cmp,
};
Fks_MbcEnv const* const fks_mbc_gbk = &fks_mbcEnv_gbk;
#endif


// ---------------------------------------------------------------------------
// uhc
#if 0

#define UHC_ISLEAD(c)   ((c >= 0x81) & (c <= 0xFE))

/** 全角の1バイト目か?
 */
static unsigned uhc_islead(unsigned c) {
    return UHC_ISLEAD(c);
}


static unsigned uhc_istrail(unsigned c) {
    if (c >= 0x40 && c <= 0xFE) {
        if (c >= 0x81 || c <= 0x5a)
            return 1;
        if (c >= 0x61 && c <= 0x7a)
            return 1;
    }
    return 0;
}


#if 0
static unsigned uhc_istrailp(char const* p) {
    return uhc_istrail(*(unsigned char*)p);
}
#endif

/** 文字コードが正しい範囲にあるかチェック. 手抜きでかなり甘くしてる.
 */
static unsigned uhc_chkc(unsigned c)
{
    if (c <= 0xff) {
        return 1;
    } else {
        return UHC_ISLEAD(c >> 8) && uhc_istrail((unsigned char)c);
    }
}


/** 1字取り出し＆ポインタ更新.
 */
static unsigned uhc_getc(const char** pStr) {
    const unsigned char* s = (unsigned char*)*pStr;
    unsigned             c = *s++;
    if (UHC_ISLEAD(c) && *s) {
        c = (c << 8) | *s++;
    }
    *pStr = (const char *)s;
    return c;
}


/** 一字取り出し
 */
static unsigned uhc_peekc(const char* s) {
    unsigned       c       = *(unsigned char*)(s++);
    if (UHC_ISLEAD(c) && *s) {
        c = (c << 8) | *(unsigned char*)s;
    }
    return c;
}


/** 1文字のchar数を返す.
 */
static unsigned uhc_len1(const char* pChr) {
    unsigned char c = *(unsigned char*)pChr;
    return (c != 0) + (UHC_ISLEAD(c) && pChr[1]);
}


static char* uhc_charNext(char const* pChr) {
	return (char*)pChr + uhc_len1(pChr);
}


MBC_IMPL(uhc)


static Fks_MbcEnv const fks_mbcEnv_uhc = {
    uhc_islead,                     // Cがマルチバイト文字の1バイト目か?
    uhc_chkc,                       // 文字コードが正しい範囲にあるかチェック.
    uhc_getc,                       // 1字取り出し＆ポインタ更新.
    uhc_peekc,                      // 一字取り出し
	uhc_charNext,
    dbc_setc,                       // 1字書き込み.
    uhc_len1,                       // 1文字のchar数を返す.
    dbc_chrLen,                     // 1文字のchar数を返す.
    dbc_chrWidth,                   // 半角全角を考慮して文字の幅を返す.
    dbc_chrWidth,                   // 半角全角を考慮して文字の幅を返す.
	uhc_adjustSize,
	uhc_chrsToSize,
	uhc_sizeToChrs,
	uhc_cmp,
};
Fks_MbcEnv const* const fks_mbc_uhc = &fks_mbcEnv_uhc;
#endif


// ---------------------------------------------------------------------------

#if 0
size_t    fks_mbcAdjustSize(Fks_MbcEnv const* mbc, const char* str, size_t size) {
    const char* s = str;
    const char* b = s;
    const char* e = s + size;
    assert(str != 0 && size > 0);
    if (e < s)
        e = (const char*)(~(size_t)0);
    while (s < e) {
        if (*s == 0)
            return s - str;
        b = s;
        s += mbc->len1(s);
    }
    return b - str;
}


/// 領域サイズから(意味的な)文字数を求める
size_t  fks_mbcSizeToChrs(Fks_MbcEnv const* mbc, const char* str, size_t size) {
    const char* s = str;
    const char* e = s + size;
    //const char* b = s;
    size_t      l = 0;
    if (e < s)
        e = (const char*)(~(size_t)0);
    assert(str != 0 && size > 0);
    while (s < e) {
        unsigned c;
        //b  = s;
        c  = mbc->getC(&s);
        if (c == 0)
            break;
        ++l;
    }
    if (s > e)
        --l;
    return l;
}

/// 文字数から半角文字単位の領域サイズを求める.
size_t  fks_mbcChrsToSize(Fks_MbcEnv const* mbc, const char* str, size_t chrs) {
    const char* s  = str;
    size_t      sz = 0;
    assert(str != 0);
    while (chrs) {
        unsigned c  = mbc->getC(&s);
        if (c == 0)
            break;
        sz += mbc->chrLen(c);
        --chrs;
    }
    return sz;
}

/** '\0'終端文字列比較. 文字値が intの正数範囲に収まることに依存.
 */
int fks_mbcCmp(Fks_MbcEnv const* mbc, const char* lp, const char* rp) {
    int lc, rc;
    int d;
    assert(lp != NULL);
    assert(rp != NULL);
    do {
        lc = mbc->getC(&lp);
        rc = mbc->getC(&rp);
        d  = lc - rc;
    } while (d == 0 && lc);
    return d;
}
#endif


/// 領域サイズから半角文字単位の幅を求める.
size_t  fks_mbcSizeToWidth(Fks_MbcEnv const* mbc, const char* str, size_t size) {
    const char* s = str;
    const char* e = s + size;
    size_t      b = 0;
    size_t      w = 0;
    if (e < s)
        e = (const char*)(~(size_t)0);
    assert(str != 0 && size > 0);
    while (s < e) {
        unsigned c  = mbc->getC(&s);
        if (c == 0)
            break;
        b = w;
        w += mbc->chrWidth(c);
    }
    if (s == e)
        return w;
    return b;
}


/// 文字数から半角文字単位の幅を求める.
size_t  fks_mbcChrsToWidth(Fks_MbcEnv const* mbc, const char* str, size_t chrs) {
    const char* s = str;
    size_t      w = 0;
    assert(str != 0);
    while (chrs) {
        unsigned c  = mbc->getC(&s);
        if (c == 0)
            break;
        w += mbc->chrWidth(c);
        --chrs;
    }
    return w;
}


/// 半角文字単位の幅から領域サイズを求める.
size_t  fks_mbcWidthToSize(Fks_MbcEnv const* mbc, const char* str, size_t width) {
    const char* s = str;
    const char* b = s;
    size_t      w = 0;
    assert(str != 0);
    while (w < width) {
        unsigned c;
        b  = s;
        c  = mbc->getC(&s);
        if (c == 0)
            break;
        w += mbc->chrWidth(c);
    }
    if (w > width)
        s = b;
    return s - str;
}


/// 半角文字単位の幅から文字数を求める.
size_t  fks_mbcWidthToChrs(Fks_MbcEnv const* mbc, const char* str, size_t width) {
    const char* s = str;
    const char* b;
    size_t      w = 0;
    size_t      n = 0;
    assert(str != 0);
    while (w < width) {
        unsigned c;
        b  = s;
        c  = mbc->getC(&s);
        if (c == 0)
            break;
        ++n;
        w += mbc->chrWidth(c);
    }
    if (w > width)
        --n;
    return n;
}



/** コピー. mbcの時は文字が壊れない部分まで. dst == src もok.
 */
char*   fks_mbcCpy(Fks_MbcEnv const* mbc, char dst[], size_t dstSz, const char* src)
{
    size_t    l;
    assert(dst != NULL && dstSz > 0 && src != NULL);

    l = mbc->adjustSize(src, dstSz);
	return fks_mbcLCpy(mbc, dst, dstSz, src, l+1);
}

char*   fks_mbcLCpy(Fks_MbcEnv const* mbc, char dst[], size_t dstSz, const char* src, size_t l)
{
    assert(dst != NULL && dstSz > 0 && src != NULL);

	if (l > dstSz)
		l = dstSz;
    // アドレスが同じなら、長さをあわせるのみ.
    if (dst == src) {
        dst[l-1] = 0;
        return dst;
    }

    // コピー.
    {
        const char* s = src;
        const char* e = s + l;
        char*       d = dst;
        while (s < e)
            *d++ = *s++;
        *d = 0;
    }

    return dst;
}



/** 文字列の連結.
 */
char*   fks_mbcCat(Fks_MbcEnv const* mbc, char dst[], size_t dstSz, const char* src)
{
    size_t l;
    assert(dst != NULL && dstSz > 0 && src != 0 && dst != src);
    l = mbc_raw_len(dst);
    if (l >= dstSz) {   // そもそも転送先が満杯ならサイズ調整のみ.
        return fks_mbcCpy(mbc, dst, dstSz, dst);
    }
    fks_mbcCpy(mbc, dst+l, dstSz - l, src);
    return dst;
}



/** コピー. mbcの時は文字が壊れない部分まで. dst == src もok.
 */
char*   fks_mbcCpyNC(Fks_MbcEnv const* mbc, char dst[], size_t dstSz, const char* src, size_t nc)
{
    size_t    l;
    assert(dst != NULL && dstSz > 0 && src != 0 && dst != src);
    l = mbc->chrsToSize(src, nc) + 1;
    l = dstSz < l ? dstSz : l;
    return fks_mbcCpy(mbc, dst, l, src);
}



/** 文字列の連結.
 */
char*   fks_mbcCatNC(Fks_MbcEnv const* mbc, char dst[], size_t dstSz, const char* src, size_t nc)
{
    size_t l, l2;
    assert(dst != NULL && dstSz > 0 && src != 0 && dst != src);
    l = mbc_raw_len(dst);
    if (l >= dstSz) {   // そもそも転送先が満杯ならサイズ調整のみ.
        return fks_mbcCpy(mbc, dst, dstSz, dst);
    }
    dstSz -= l;
    l2  = mbc->chrsToSize(src, nc) + 1;
    l2  = dstSz < l2 ? dstSz : l2;
    fks_mbcCpy(mbc, dst+l, l2, src);
    return dst;
}



/** コピー. mbcの時は文字が壊れない部分まで. dst == src もok.
 */
char*   fks_mbcCpyWidth(Fks_MbcEnv const* mbc, char dst[], size_t dstSz, const char* src, size_t width)
{
    size_t    l = fks_mbcWidthToSize(mbc, src, width) + 1;
    l = dstSz < l ? dstSz : l;
    return fks_mbcCpy(mbc, dst, l, src);
}



/** 文字列の連結.
 */
char*   fks_mbcCatWidth(Fks_MbcEnv const* mbc, char dst[], size_t dstSz, const char* src, size_t width)
{
    size_t l, l2;
    assert(dst != NULL && dstSz > 0 && src != 0 && dst != src);
    l = mbc_raw_len(dst);
    if (l >= dstSz) {   // そもそも転送先が満杯ならサイズ調整のみ.
        return fks_mbcCpy(mbc, dst, dstSz, dst);
    }
    dstSz -= l;
    l2  = fks_mbcWidthToSize(mbc, src, width) + 1;
    l2  = dstSz < l2 ? dstSz : l2;
    fks_mbcCpy(mbc, dst+l, l2, src);
    return dst;
}



static inline bool fks_mbc_isUnicode(Fks_MbcEnv const* e)
{
	if (e == fks_mbc_utf8
		|| e == fks_mbc_utf16le
		|| e == fks_mbc_utf16be
		|| e == fks_mbc_utf32le
		|| e == fks_mbc_utf32be)
	{
		return 1;
	}
	return 0;
}



/** コピー. mbcの時は文字が壊れない部分まで. dst == src もok.
 */
char*   fks_mbcConv(Fks_MbcEnv const* dstMbc, char dst[], size_t dstSz, Fks_MbcEnv const* srcMbc, const char* src, size_t srcSz)
{
	if (dstMbc == srcMbc) {
		fks_mbcLCpy(dstMbc, dst, dstSz, src, srcSz);
	} else if (fks_mbc_isUnicode(dstMbc) && fks_mbc_isUnicode(srcMbc)) {
		char const* s  = src;
		char const* se = src + srcSz;
		char* d = dst;
		char* de = dst + dstSz;
		while (d + 6 < de && s < se) {
			unsigned c = srcMbc->getC(&s);
			d = dstMbc->setC(d, c);
		}
	} else if (dstMbc == fks_mbc_sjis && srcMbc == fks_mbc_eucJp) {
		char const* s  = src;
		char const* se = src + srcSz;
		char* d = dst;
		char* de = dst + dstSz;
		while (d + 4 < de && s < se) {
			unsigned c = srcMbc->getC(&s);
			d = dstMbc->setC(d, c);
		}
	}

    size_t    l;
    assert(dst != NULL && dstSz > 0 && src != NULL);

    l = dstMbc->adjustSize(src, dstSz);

    // アドレスが同じなら、長さをあわせるのみ.
    if (dst == src) {
        dst[l] = 0;
        return dst;
    }

    // コピー.
    {
        const char* s = src;
        const char* e = s + l;
        char*       d = dst;
        while (s < e)
            *d++ = *s++;
        *d = 0;
    }

    return dst;
}

#ifdef __cplusplus
}
#endif
