/*
 *  @file   fks_mbc.c
 *  @brief  Multi Byte Character lib.
 *  @author Masashi Kitamura (tenka@6809.net)
 *  @license Boost Software Lisence Version 1.0
 */

#include <fks_common.h>
#include <fks_mbc.h>
#include <fks_alloca.h>
#include <fks_malloc.h>
#include <fks_assert_ex.h>
#include <fks_io_mbs.h>
#include "detail/fks_mbc_sub.h"

#ifdef _WIN32
#include <windows.h>
#ifdef _MSC_VER
#pragma warning (disable:4244)
#endif
#endif

#ifdef FKS_USE_MBC_JP
#include "detail/fks_mbc_jp.hh"
#endif

#ifdef __cplusplus
extern "C" {
#endif

// ---------------------------------------------------------------------------
// utf8

/** DBC high byte?
 */
static unsigned utf8_islead(unsigned c) {
    return c >= 0xC0;
}


/** Check character encode.
 */
static unsigned utf8_chkC(unsigned c) {
    return c < 0xD800 || (c >= 0xE000 && (uint16_t)c != 0xffff && c <= 0x10FFFD);
}

/** Get character.
 */
static unsigned utf8_getC(char const** pStr) {
    unsigned char const* s = (unsigned char*)*pStr;
    unsigned       c       = *s++;
	if (!c)
		return c;
    if (c < 0xC0/*0x80*/) {	// 0x80-0xbf bad code
        ;
    } else if (*s) {
        unsigned c2 = *s++;
        c2 &= 0x3F;
        if (c < 0xE0) {
            c = ((c & 0x1F) << 6) | c2;
        } else if (*s) {
            unsigned c3 = *s++;
            c3 &= 0x3F;
            if (c < 0xF0) {
                c = ((c & 0xF) << 12) | (c2 << 6) | c3;
            } else if (*s) {
                unsigned c4 = *s++;
                c4 &= 0x3F;
                if (c < 0xF8) {
                    c = ((c&7)<<18) | (c2<<12) | (c3<<6) | c4;
                } else if (*s) {
                    unsigned c5 = *s++;
                    c5 &= 0x3F;
                    if (c < 0xFC) {
                        c = ((c&3)<<24) | (c2<<18) | (c3<<12) | (c4<<6) | c5;
                    } else if (*s) {
                        unsigned c6 = *s++;
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

/** Peek character.
 */
static unsigned utf8_peekC(char const* s) {
    return utf8_getC(&s);
}

/** Set character.
 */
static char*    utf8_setC(char*  dst, char* e, unsigned c) {
    char* d = dst;
    if (c < 0xC0/*0x80*/) {	// 0x80-xBF bad code
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
        //if (c >= 0xff60 && c <= 0xff9f) {--(*adn); }	// hankaku-kana
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

/** Get display size of character.
 */
static unsigned utf8_chrWidth(unsigned chr) {
    //chr;
    return 1;
}

/** Get display size of character for JP font.
 */
static unsigned utf8_jp_chrWidth(unsigned c) {
    if (c < 0x370)
        return 1;
    if (c >= 0xff60 && c <= 0xff9f)
        return 1;
    return 2;
}

/** Check UTF8 Encode?
 * @return 0=not  1=ascii(<=7f) 2,3,4=ut8
 */
int  fks_mbcCheckUTF8(char const* src, size_t len, int canEndBroken)
{
	unsigned char const* s = (unsigned char*)src;
	unsigned char const* e = s + len;
	char	hasAsc  = 0;
	char	allAsc  = 1;
	char	badFlg  = 0;
	char	zenFlg  = 0;
	char	b5b6    = 0;
	char	salFlg  = 0;
	int		c, c2, c3, c4, c5, c6;
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

static Fks_MbcEnc const fks_mbcEnc_utf8 = {
	FKS_CP_UTF8,				// code page
    utf8_islead,                // DBC high byte?
    utf8_chkC,                  // Check character encode.
    utf8_getC,                  // Get character.
    utf8_peekC,                 // Peek character.
	utf8_charNext,				// Get pointer of next character.
    utf8_setC,                  // Set character.
    utf8_len1,                  // Size of character.
    utf8_chrLen,                // Size of character.
    utf8_chrWidth,              // Get display size of character.
    utf8_jp_chrWidth,           // Get display size of character for JP font.
	utf8_adjustSize,			// Adjust the end of the string.
	utf8_cmp,					// Compare string.
	fks_mbcCheckUTF8,			// Check string's character encode.
};
fks_mbcenc_t const fks_mbc_utf8 = &fks_mbcEnc_utf8;



// ---------------------------------------------------------------------------
#define REV_U16(c)		((uint8_t)((c) >> 8) | ((uint8_t)(c) << 8))

#ifdef FKS_BIG_ENDIAN
#define GET16LE(p)		REV_U16(*(uint16_t*)(p))
#define SET16LE(p, c)	(*(uint16_t*)(p) = REV_U16(c))
#else
#define GET16LE(p)		(*(uint16_t*)(p))
#define SET16LE(p, c)	(*(uint16_t*)(p) = (c))
#endif

static unsigned utf16le_getC(char const** ppSrc) {
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
	} else {	// error
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
		uint32_t	d = GET16LE(p);
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
	return fks_mbcCheckEncoding(fks_mbc_utf16le, src, (len & ~1), canEndBroken);
}

MBC_IMPL(utf16le)


static Fks_MbcEnc const fks_mbcEnc_utf16le = {
	FKS_CP_UTF16LE,
	utf8_islead,
    utf8_chkC,
    utf16le_getC,
	utf16le_peekC,
	utf16le_charNext,
    utf16le_setC,
    utf16le_len1,
    utf16le_chrLen,
    utf8_chrWidth,
    utf8_jp_chrWidth,
	utf16le_adjustSize,
	utf16le_cmp,
	utf16le_checkEncoding,
};
fks_mbcenc_t const fks_mbc_utf16le = &fks_mbcEnc_utf16le;



// ---------------------------------------------------------------------------

#ifdef FKS_BIG_ENDIAN
#define GET16BE(p)		REV_U16(*(uint16_t*)(p))
#define SET16BE(p, c)	(*(uint16_t*)(p) = REV_U16(c))
#else
#define GET16BE(p)		(*(uint16_t*)(p))
#define SET16BE(p, c)	(*(uint16_t*)(p) = (c))
#endif


static unsigned utf16be_getC(char const** ppSrc) {
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
	} else {	// error
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
		uint32_t	d = GET16BE(p);
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
	return fks_mbcCheckEncoding(fks_mbc_utf16be, src, (len & ~1), canEndBroken);
}

MBC_IMPL(utf16be)

static Fks_MbcEnc const fks_mbcEnc_utf16be = {
	FKS_CP_UTF16BE,
	utf8_islead,
    utf8_chkC,
    utf16be_getC,
	utf16be_peekC,
	utf16be_charNext,
    utf16be_setC,
    utf16be_len1,
    utf16be_chrLen,
    utf8_chrWidth,
    utf8_jp_chrWidth,
	utf16be_adjustSize,
	utf16be_cmp,
	utf16be_checkEncoding,
};
fks_mbcenc_t const fks_mbc_utf16be = &fks_mbcEnc_utf16be;



// ---------------------------------------------------------------------------
#define REV_U32(c)		((uint8_t)((c) >> 24)| ((uint8_t)((c) >> 16) << 8) | ((uint8_t)((c) >> 8)<<16) | ((uint8_t)(c) << 24))

#ifdef FKS_BIG_ENDIAN
#define GET32LE(p)		REV_U32(*(uint32_t*)(p))
#define SET32LE(p, c)	(*(uint32_t*)(p) = REV_U32(c))
#else
#define GET32LE(p)		(*(uint32_t*)(p))
#define SET32LE(p, c)	(*(uint32_t*)(p) = (c))
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
	return fks_mbcCheckEncoding(fks_mbc_utf32le, src, (len & ~3), canEndBroken);
}

MBC_IMPL(utf32le)

static Fks_MbcEnc const fks_mbcEnc_utf32le = {
	FKS_CP_UTF32LE,
	utf8_islead,
    utf8_chkC,
    utf32le_getC,
	utf32le_peekC,
	utf32le_charNext,
    utf32le_setC,
    utf32le_len1,
    utf32le_chrLen,
    utf8_chrWidth,
    utf8_jp_chrWidth,
	utf32le_adjustSize,
	utf32le_cmp,
	utf32le_checkEncoding,
};
fks_mbcenc_t const fks_mbc_utf32le = &fks_mbcEnc_utf32le;



// ---------------------------------------------------------------------------
#ifdef FKS_BIG_ENDIAN
#define GET32BE(p)		(*(uint32_t*)(p))
#define SET32BE(p, c)	(*(uint32_t*)(p) = (c))
#else
#define GET32BE(p)		REV_U32(*(uint32_t*)(p))
#define SET32BE(p, c)	(*(uint32_t*)(p) = REV_U32(c))
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
	return fks_mbcCheckEncoding(fks_mbc_utf32be, src, (len & ~3), canEndBroken);
}

MBC_IMPL(utf32be)

static Fks_MbcEnc const fks_mbcEnc_utf32be = {
	FKS_CP_UTF32BE,
	utf8_islead,
    utf8_chkC,
    utf32be_getC,
	utf32be_peekC,
	utf32be_charNext,
	utf32be_setC,
    utf32be_len1,
    utf32be_chrLen,
	utf8_chrWidth,
	utf8_jp_chrWidth,
	utf32be_adjustSize,
	utf32be_cmp,
	utf32be_checkEncoding,
};
fks_mbcenc_t const fks_mbc_utf32be = &fks_mbcEnc_utf32be;



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
static char*	asc_charNext(char const* s) { return (char*)s + (*s > 0); }

MBC_IMPL(asc)

static Fks_MbcEnc const fks_mbcEnc_asc = {
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
    asc_chrWidth,
	asc_adjustSize,
	asc_cmp,
};
fks_mbcenc_t const fks_mbc_asc = &fks_mbcEnc_asc;


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

#define dbc_setC	fks_dbc_setC

static unsigned dbc_chrLen(unsigned chr) {
    return 1 + (chr > 0xff);
}

static unsigned dbc_chrWidth(unsigned chr) {
    //TODO: とりあえず特殊なNEC半角は無視...
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

static int  dbc_checkEncoding(char const* s, size_t len, int canEndBroken) {
	return fks_mbcCheckEncoding(fks_mbc_dbc, s, len, canEndBroken);
}

MBC_IMPL(dbc)

static Fks_MbcEnc const fks_mbcEnc_dbc = {
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
    dbc_chrWidth,
	dbc_adjustSize,
	dbc_cmp,
	dbc_checkEncoding,
};
fks_mbcenc_t const fks_mbc_dbc = &fks_mbcEnc_dbc;

#endif



// ---------------------------------------------------------------------------

/** Check string (DBC type)
 *  @param 	s				source string
 *  @param 	len				Check range. ('\0' is not EOS but broken character)
 *	@param	canEndBroken	Can the end be broken
 *  @return	0:not 1:ascii 2,3,4:ok
 */
int  fks_mbcCheckEncoding(fks_mbcenc_t mbc, char const* s, size_t len, int canEndBroken) {
	char const* e = s + len;
	char asc = 1;
	while (s < e) {
		unsigned c;
		unsigned k = mbc->len1(s);
		if (s+k >= e)
			break;
		c = mbc->getC(&s);
		if (!c) {
			if (s < e || !canEndBroken)
				return 0;
			break;
		}
		if (c >= 0x80) {
			asc = 0;
		} else if (!mbc->chkC(c)) {
			if (!canEndBroken || *s != 0)
				return 0;
		}
	}
	if (asc)
		return 1;
	return 3;
}

/** 領域サイズから(意味的な)文字数を求める.
 */
size_t  fks_mbcSizeToChrs(fks_mbcenc_t mbc, char const* str, size_t size) {
    char const* s = str;
    char const* e = s + size;
    size_t      l = 0;
    if (e < s)
        e = (char const*)(~(size_t)0);
    FKS_ASSERT(str != 0 && size > 0);
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
size_t  fks_mbcChrsToSize(fks_mbcenc_t mbc, char const* str, size_t chrs) {
    char const* s  = str;
    size_t      sz = 0;
    FKS_ASSERT(str != 0);
    while (chrs) {
        unsigned c  = mbc->getC(&s);
        if (c == 0)
            break;
        sz += mbc->chrLen(c);
        --chrs;
    }
    return sz;
}


/** 領域サイズから半角文字単位の幅を求める.
 */
size_t  fks_mbcSizeToWidth(fks_mbcenc_t mbc, char const* str, size_t size) {
    char const* s = str;
    char const* e = s + size;
    size_t      b = 0;
    size_t      w = 0;
    if (e < s)
        e = (char const*)(~(size_t)0);
    FKS_ASSERT(str != 0 && size > 0);
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


/** 文字数から半角文字単位の幅を求める.
 */
size_t  fks_mbcChrsToWidth(fks_mbcenc_t mbc, char const* str, size_t chrs) {
    char const* s = str;
    size_t      w = 0;
    FKS_ASSERT(str != 0);
    while (chrs) {
        unsigned c  = mbc->getC(&s);
        if (c == 0)
            break;
        w += mbc->chrWidth(c);
        --chrs;
    }
    return w;
}


/** 半角文字単位の幅から領域サイズを求める.
 */
size_t  fks_mbcWidthToSize(fks_mbcenc_t mbc, char const* str, size_t width) {
    char const* s = str;
    char const* b = s;
    size_t      w = 0;
    FKS_ASSERT(str != 0);
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


/** 半角文字単位の幅から文字数を求める.
 */
size_t  fks_mbcWidthToChrs(fks_mbcenc_t mbc, char const* str, size_t width) {
    char const* s = str;
    char const* b;
    size_t      w = 0;
    size_t      n = 0;
    FKS_ASSERT(str != 0);
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
size_t  fks_mbcCpy(fks_mbcenc_t mbc, char dst[], size_t dstSz, char const* src)
{
    size_t    l;
    FKS_ASSERT(dst != NULL && dstSz > 0 && src != NULL);

    l = mbc->adjustSize(src, dstSz);
	return fks_mbcLCpy(mbc, dst, dstSz, src, l+1);
}


size_t fks_mbcLCpy(fks_mbcenc_t mbc, char dst[], size_t dstSz, char const* src, size_t l)
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
size_t fks_mbcCat(fks_mbcenc_t mbc, char dst[], size_t dstSz, char const* src)
{
    size_t l, l2;
    FKS_ASSERT(dst != NULL && dstSz > 0 && src != 0 && dst != src);
    l = mbc_raw_len(dst);
    if (l >= dstSz) {   // そもそも転送先が満杯ならサイズ調整のみ.
        return fks_mbcCpy(mbc, dst, dstSz, dst);
    }
    l2 = fks_mbcCpy(mbc, dst+l, dstSz - l, src);
    return l + l2;
}


/** コピー. mbcの時は文字が壊れない部分まで. dst == src もok.
 */
size_t	fks_mbcCpyNC(fks_mbcenc_t mbc, char dst[], size_t dstSz, char const* src, size_t nc)
{
    size_t    l;
    FKS_ASSERT(dst != NULL && dstSz > 0 && src != 0 && dst != src);
    l = fks_mbcChrsToSize(mbc, src, nc) + 1;
    l = dstSz < l ? dstSz : l;
    return fks_mbcCpy(mbc, dst, l, src);
}


/** 文字列の連結.
 */
size_t   fks_mbcCatNC(fks_mbcenc_t mbc, char dst[], size_t dstSz, char const* src, size_t nc)
{
    size_t l, l2;
    FKS_ASSERT(dst != NULL && dstSz > 0 && src != 0 && dst != src);
    l = mbc_raw_len(dst);
    if (l >= dstSz) {   // そもそも転送先が満杯ならサイズ調整のみ.
        return fks_mbcCpy(mbc, dst, dstSz, dst);
    }
    dstSz -= l;
    l2 = fks_mbcChrsToSize(mbc, src, nc) + 1;
    l2 = dstSz < l2 ? dstSz : l2;
    l2 = fks_mbcCpy(mbc, dst+l, l2, src);
    return l + l2;
}


/** コピー. mbcの時は文字が壊れない部分まで. dst == src もok.
 */
size_t fks_mbcCpyWidth(fks_mbcenc_t mbc, char dst[], size_t dstSz, char const* src, size_t width)
{
    size_t    l = fks_mbcWidthToSize(mbc, src, width) + 1;
    l = dstSz < l ? dstSz : l;
    return fks_mbcCpy(mbc, dst, l, src);
}


/** 文字列の連結.
 */
size_t  fks_mbcCatWidth(fks_mbcenc_t mbc, char dst[], size_t dstSz, char const* src, size_t width)
{
    size_t l, l2;
    FKS_ASSERT(dst != NULL && dstSz > 0 && src != 0 && dst != src);
    l = mbc_raw_len(dst);
    if (l >= dstSz) {   // そもそも転送先が満杯ならサイズ調整のみ.
        return fks_mbcCpy(mbc, dst, dstSz, dst);
    }
    dstSz -= l;
    l2  = fks_mbcWidthToSize(mbc, src, width) + 1;
    l2  = dstSz < l2 ? dstSz : l2;
    l2  = fks_mbcCpy(mbc, dst+l, l2, src);
    return l + l2;
}



// --------------------------------------------------------------------------
// UNICODE

int fks_mbcCheckUnicodeBOMi(char const* src, size_t len)
{
	unsigned char const* b = (unsigned char const*)src;
	if (len < 2)
		return 0;
	// Check Unicode BOM
	if (len >= 3 && b[0] == 0xEF && b[1] == 0xBB && b[2] == 0xBF) {
		return 1;		// UTF8:BOM
	} else if (len >= 2 && b[0] == 0xFF && b[1] == 0xFE) {
		if (len >= 4 && b[2] == 0x00 && b[3] == 0x00) {
			return 4; // UTF32LE:BOM
		} else {
			return 2;	// UTF16LE:BOM
		}
	} else if (len >= 2 && b[0] == 0xFE && b[1] == 0xFF) {
		return 3;		// UTF16BE:BOM
	} else if (len >= 4 && b[0] == 0x00 && b[1] == 0x00 && b[2] == 0xFE && b[3] == 0xFF) {
		return 5;		// UTF32BE:BOM
	}
	return 0;
}

fks_mbcenc_t fks_mbcCheckUnicodeBOM(char const* src, size_t len)
{

	static fks_mbcenc_t s_tbl[] = {
		NULL,
		&fks_mbcEnc_utf8,
		&fks_mbcEnc_utf16le,
		&fks_mbcEnc_utf16be,
		&fks_mbcEnc_utf32le,
		&fks_mbcEnc_utf32be,
	};
	return s_tbl[ fks_mbcCheckUnicodeBOMi(src, len) ];
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


size_t   fks_mbcUnicodeConv(fks_mbcenc_t dstMbc, char dst[], size_t dstSz, fks_mbcenc_t srcMbc, char const* src, size_t srcSz)
{
	FKS_ARG_ASSERT(1, Fks_MbcEnvToCheckUnicodeBomNumber(dstMbc) > 0);
	FKS_ARG_PTR_ASSERT(2, dst);
	FKS_ARG_ASSERT(3, dstSz > 0);
	FKS_ARG_ASSERT(4, Fks_MbcEnvToCheckUnicodeBomNumber(srcMbc) > 0);
	FKS_ARG_PTR_ASSERT(5, src);

	if (dstMbc == srcMbc) {
		return fks_mbcLCpy(dstMbc, dst, dstSz, src, srcSz);
	} else {
		char const* s  = src;
		char const* se = src + srcSz;
		char* d = dst;
		char* de = dst + dstSz;
		while (d < de && s < se) {
			unsigned c = srcMbc->getC(&s);
			if (!c)
				break;
			d = dstMbc->setC(d, de, c);
		}
		if (d < de)
			*d = 0;
		return d - dst;
	}
}


#ifdef FKS_WIN32
fks_mbcenc_t fks_mbc_makeDBC(Fks_MbcEnc* mbcEnv, fks_codepage_t cp) {
	*mbcEnv = fks_mbcEnc_dbc;
	mbcEnv->cp = cp;
	return mbcEnv;
}

size_t   fks_mbc_dbcFromUnicode(fks_codepage_t dstCP, char dst[], size_t dstSz, fks_mbcenc_t srcMbc, char const* src, size_t srcSz)
{
	wchar_t const* ws  = (wchar_t const*)src;
	size_t         wsl = srcSz / sizeof(*ws);
	FKS_ARG_PTR_ASSERT(2, dst);
	FKS_ARG_ASSERT(3, dstSz >= 4);
	FKS_ARG_PTR_ASSERT(4, srcMbc);
	FKS_ARG_PTR_ASSERT(5, src);
	if (srcMbc != fks_mbc_utf16le) {
		size_t tl = (srcMbc == fks_mbc_utf8) ? srcSz * 3 : (srcMbc == fks_mbc_utf16be) ? srcSz : srcSz*2;
		ws  = (wchar_t*)fks_alloca(tl);
		wsl = fks_mbcUnicodeConv(fks_mbc_utf16le, (char*)ws, tl, srcMbc, src, srcSz);
		wsl /= sizeof(*ws);
	}
	if (*ws == 0xFEFF) {
		++ws;
		--wsl;
	}
	return WideCharToMultiByte(dstCP,0,ws,wsl,dst,dstSz,0,0);
}

size_t   fks_mbc_unicodeFromDbc(fks_mbcenc_t dstMbc, char dst[], size_t dstSz, fks_codepage_t srcCP, char const* src, size_t srcSz)
{
	wchar_t * wd  = (wchar_t*)dst;
	size_t    wdl = dstSz;
	wchar_t*  m   = NULL;
	size_t	  l;
	if (dstMbc != fks_mbc_utf16le) {
		wdl = srcSz * 4;
		if (wdl <= 0x4000)
			wd = (wchar_t*)fks_alloca(wdl);
		else
			wd = m = (wchar_t*)fks_calloc(1, wdl);
	}
    l = MultiByteToWideChar(srcCP,0,src,srcSz,wd,wdl/sizeof(*wd));
	if (dstMbc == fks_mbc_utf16le)
    	return l * sizeof(wchar_t);
	l = fks_mbcUnicodeConv(dstMbc, dst, dstSz, fks_mbc_utf16le, (char*)wd, l * sizeof(wchar_t));
	if (m)
		fks_free(m);
	return l;
}

/*
size_t   fks_mbcUnicodeDbcConv(fks_mbcenc_t dstMbc, char dst[], size_t dstSz, fks_mbcenc_t srcMbc, char const* src, size_t srcSz)
{
	FKS_ARG_ASSERT(1, Fks_MbcEnvToCheckUnicodeBomNumber(dstMbc) > 0 || dstMbc == fks_mbc_dbc);
	FKS_ARG_PTR_ASSERT(2, dst);
	FKS_ARG_PTR_ASSERT(3, dstSz > 0);
	FKS_ARG_ASSERT(4, Fks_MbcEnvToCheckUnicodeBomNumber(srcMbc) > 0 || srcMbc == fks_mbc_dbc);
	FKS_ARG_PTR_ASSERT(5, src);

	if (dstMbc == srcMbc)
		return fks_mbcLCpy(dstMbc, dst, dstSz, src, srcSz);
	else if (Fks_MbcEnvToCheckUnicodeBomNumber(dstMbc) && Fks_MbcEnvToCheckUnicodeBomNumber(srcMbc))
		return fks_mbcUnicodeConv(dstMbc, dst, dstSz, srcMbc, src, srcSz);
	else if (dstMbc == fks_mbc_dbc)
		return fks_mbc_dbcFromUnicode(dstMbc->cp, dst, dstSz, srcMbc, src, srcSz);
	else
		return fks_mbc_unicodeFromDbc(dstMbc, dst, dstSz, srcMbc->cp, src, srcSz);
}
*/
#else

#endif



// --------------------------------------------------------------------------

/**
 */
size_t	fks_mbcCountCapa(fks_mbcenc_t dstMbc, fks_mbcenc_t srcMbc, char const* src, size_t srcSz)
{
	char const* s  = src;
	char const* se = src + srcSz;
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
			while (s < se) {
				unsigned c = srcMbc->getC(&s);
				if (!c)
					break;
				sz += dstMbc->chrLen(c);
			}
		} else {
			static uint8_t const s_tbl[] = {0,1,2,2,4,4,};
			size_t c1 = s_tbl[dstUni];
			while (s < se) {
				unsigned c = srcMbc->getC(&s);
				if (!c)
					break;
				sz += (c < 0x80) ? c1 : 4;
			}
			return sz;
		}
 #ifdef FKS_USE_MBC_JP
	} else if (dstMbc == fks_mbc_sjis) {
		if (srcMbc == fks_mbc_eucjp) {
			sz = srcSz;
		} else {
			while (s < se) {
				unsigned c = srcMbc->getC(&s);
				if (!c)
					break;
				sz += (c < 0x80) ? 1 : 2;
			}
		}
	} else if (dstMbc == fks_mbc_eucjp) {
		if (srcMbc == fks_mbc_sjis) {
			while (s < se) {
				unsigned c = sjis_getC(&s);
				if (!c)
					break;
				c = fks_sjis2jis(c);
				c = fks_jis2eucjp(c);
				sz += eucjp_chrLen(c);
			}
		} else {
			while (s < se) {
				unsigned c = srcMbc->getC(&s);
				if (!c)
					break;
				sz += (c < 0x80) ? 1 : 3;
			}
		}
 #endif
	} else {
		while (s < se) {
			unsigned c = srcMbc->getC(&s);
			sz += (c < 0x80) ? 1 : 4;
		}
	}
	return sz;
}


#ifdef FKS_USE_MBC_JP
size_t  fks_mbc_eucjpFromSjis(char dst[], size_t dstSz, char const* src, size_t srcSz);
size_t  fks_mbc_sjisFromEucjp(char dst[], size_t dstSz, char const* src, size_t srcSz);
#endif

size_t   fks_mbcConv(fks_mbcenc_t dstMbc, char dst[], size_t dstSz, fks_mbcenc_t srcMbc, char const* src, size_t srcSz)
{
 #ifdef FKS_WIN32
	Fks_MbcEnc dme;
 #endif
	FKS_ARG_PTR_ASSERT(1, dstMbc);
	FKS_ARG_PTR_ASSERT(2, dst);
	FKS_ARG_ASSERT(3, dstSz > 0);
	FKS_ARG_PTR_ASSERT(4, srcMbc);
	FKS_ARG_PTR_ASSERT(5, src);

	if (dstMbc == srcMbc) {
		return fks_mbcLCpy(dstMbc, dst, dstSz, src, srcSz);
	} else if (Fks_MbcEnvToCheckUnicodeBomNumber(dstMbc)) {
		if (Fks_MbcEnvToCheckUnicodeBomNumber(srcMbc))
			return fks_mbcUnicodeConv(dstMbc, dst, dstSz, srcMbc, src, srcSz);
	  #ifdef FKS_WIN32
	   #ifdef FKS_USE_MBC_JP
	  	if (srcMbc == fks_mbc_sjis || srcMbc == fks_mbc_eucjp)
			srcMbc = fks_mbc_makeDBC(&dme, srcMbc->cp);
	   #endif
		return fks_mbc_unicodeFromDbc(dstMbc, dst, dstSz, srcMbc->cp, src, srcSz);
	  #else
		FKS_ASSERT(0 && "It was a combination that could not be converted");
	  #endif
	} else if (Fks_MbcEnvToCheckUnicodeBomNumber(srcMbc)) {
	  #ifdef FKS_WIN32
	   #ifdef FKS_USE_MBC_JP
	  	if (dstMbc == fks_mbc_sjis || dstMbc == fks_mbc_eucjp)
			dstMbc = fks_mbc_makeDBC(&dme, dstMbc->cp);
	   #endif
		return fks_mbc_dbcFromUnicode(dstMbc->cp, dst, dstSz, srcMbc, src, srcSz);
	  #else
		FKS_ASSERT(0 && "It was a combination that could not be converted");
	  #endif
 #if defined FKS_WIN32 && defined FKS_USE_MBC_JP
	} else if (dstMbc == fks_mbc_dbc) {
		if (srcMbc == fks_mbc_sjis || srcMbc == fks_mbc_eucjp)
			srcMbc = fks_mbc_makeDBC(&dme, srcMbc->cp);
		if (dstMbc->cp == srcMbc->cp)
			return fks_mbcLCpy(dstMbc, dst, dstSz, src, srcSz);
		return fks_mbsConvCP(dstMbc->cp, dst, dstSz, srcMbc->cp, src, srcSz);
	} else if (srcMbc == fks_mbc_dbc) {
		if (dstMbc == fks_mbc_sjis || dstMbc == fks_mbc_eucjp)
			dstMbc = fks_mbc_makeDBC(&dme, dstMbc->cp);
		if (dstMbc->cp == srcMbc->cp)
			return fks_mbcLCpy(dstMbc, dst, dstSz, src, srcSz);
		return fks_mbsConvCP(dstMbc->cp, dst, dstSz, srcMbc->cp, src, srcSz);
 #endif
 #ifdef FKS_USE_MBC_JP
	} else if (dstMbc == fks_mbc_sjis && srcMbc == fks_mbc_eucjp) {
		return fks_mbc_sjisFromEucjp(dst, dstSz, src, srcSz);
	} else if (dstMbc == fks_mbc_eucjp && srcMbc == fks_mbc_sjis) {
		return fks_mbc_eucjpFromSjis(dst, dstSz, src, srcSz);
 #endif
	} else {
		FKS_ASSERT(0 && "It was a combination that could not be converted");
	}
	return 0;
}

/**
 *  @param 	src				Source string
 *  @param 	len				Check range. ('\0' is not EOS but broken character)
 *	@param	canEndBroken	Can the end be broken?
 */
fks_mbcenc_t fks_mbcAutoSelCharEncoding(char const* src, size_t len, int canEndBroken, fks_mbcenc_t encTbl[], size_t encNum)
{
	static fks_mbcenc_t const s_tbl[] = {
		&fks_mbcEnc_utf8,
	 #ifdef FKS_USE_MBC_JP
	  #ifdef FKS_WIN32
		&fks_mbcEnc_sjis,
		&fks_mbcEnc_eucjp,
	  #else
		&fks_mbcEnc_eucjp,
		&fks_mbcEnc_sjis,
	  #endif
	 #endif
	 #ifdef FKS_WIN32
		&fks_mbcEnc_dbc,
		&fks_mbcEnc_utf16le,
	 #endif
	 #if 0
		&fks_mbcEnc_utf32le,
		&fks_mbcEnc_utf16be,
		&fks_mbcEnc_utf32be,
	 #endif
	};
	enum { TBL_SZ = sizeof(s_tbl) / sizeof(s_tbl[0]) };
	int i, k, rnk, fndIdx;

	fks_mbcenc_t uck = fks_mbcCheckUnicodeBOM(src, len);
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

