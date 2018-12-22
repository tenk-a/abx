/*
 *  @file   fks_mbc.c
 *  @brief  Multi Byte Character lib.
 *  @author Masashi Kitamura (tenka@6809.net)
 *  @license Boost Software Lisence Version 1.0
 */

#include <fks_common.h>
#include <fks_mbc.h>
#include <fks_alloca.h>
#include <fks_assert_ex.h>
#include "detail/fks_mbc_sub.h"

#ifdef _WIN32
#include <windows.h>
#ifdef _MSC_VER
#pragma warning (disable:4244)
#endif
#endif


#ifdef __cplusplus
extern "C" {
#endif


// ---------------------------------------------------------------------------
// utf8

/** 全角の1バイト目か?
 */
static unsigned utf8_islead(unsigned c) {
    return c >= 0x80;
}


/** 文字コードが正しい範囲にあるかチェック. (\0もBOMもOKとする)
 */
static unsigned utf8_chkC(unsigned c)
{
    return 1;
}


static unsigned utf8_getC(char const** pStr, char const* pEnd) {
	const unsigned char* s = (unsigned char*)*pStr;
	const unsigned char* e = (unsigned char*)*pEnd;
	unsigned       c, c2, c3, c4, c5, c6;
	if (s >= e) goto ERR;
    c = *s++;
	if (!c)
		return c;
    if (c < 0xC0/*0x80*/) {	// 0x80-0xbf bad code
        ;
    } else if (*s) {
		if (s >= e) goto ERR;
        c2 = *s++;
        c2 &= 0x3F;
        if (c < 0xE0) {
            c = ((c & 0x1F) << 6) | c2;
        } else if (*s) {
			if (s >= e) goto ERR;
            c3 = *s++;
            c3 &= 0x3F;
            if (c < 0xF0) {
                c = ((c & 0xF) << 12) | (c2 << 6) | c3;
            } else if (*s) {
				if (s >= e) goto ERR;
                c4 = *s++;
                c4 &= 0x3F;
                if (c < 0xF8) {
                    c = ((c&7)<<18) | (c2<<12) | (c3<<6) | c4;
                } else if (*s) {
					if (s >= e) goto ERR;
                    c5 = *s++;
                    c5 &= 0x3F;
                    if (c < 0xFC) {
                        c = ((c&3)<<24) | (c2<<18) | (c3<<12) | (c4<<6) | c5;
                    } else if (*s) {
						if (s >= e) goto ERR;
                        c6 = *s++;
                        c6 &= 0x3F;
                        c = ((c&1)<<30) |(c2<<24) | (c3<<18) | (c4<<12) | (c5<<6) | c6;
                    }
                }
            }
        }
    }

    *pStr = (char*)s;
    return c;

ERR:
	*pStr = (char*)e;
	return 0;
}


static char* utf8_charNext(char const* pChr, char const* pEnd) {
	const unsigned char* s = (unsigned char*)pChr;
	const unsigned char* e = (unsigned char*)pEnd;
	unsigned       c = *s;
	if (s >= e) return (char*)e;
    if (!c)
        return (char*)s;
    ++s;
    if (c < 0x80) {
        return (char*)s;
    } else if (*s) {
		if (s >= e) return (char*)e;
		++s;
        if (c < 0xE0) {
            return (char*)s;
        } else if (*s) {
			if (s >= e) return (char*)e;
			++s;
            if (c < 0xF0) {
                return (char*)s;
            } else if (*s) {
				if (s >= e) return (char*)e;
				++s;
                if (c < 0xF8) {
                    return (char*)s;
                } else if (*s) {
					if (s >= e) return (char*)e;
					++s;
                    if (c < 0xFC) {
                        return (char*)s;
                    } else if (*s) {
						if (s >= e) return (char*)e;
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
static unsigned utf8_peekC(char const* s, char const* e) {
    return utf8_getC(&s, e);
}


/** 1文字のchar数を返す.
 */
static unsigned utf8_len1(char const* s, char const* e) {
    return utf8_charNext(s, e) - s;
}


/** 1字書き込み.
 */
static char*    utf8_setC(char*  dst, char* e, unsigned c) {
    char* d = dst;
    if (c < 0x80) {
		if (d < e)
	        *d++ = c;
    } else {
        if (c <= 0x7FF) {
			if (d+2 > e) goto ERR;
            *d++ = 0xC0|(c>>6);
            *d++ = 0x80|(c&0x3f);
        } else if (c <= 0xFFFF) {
			if (d+3 > e) goto ERR;
            *d++ = 0xE0|(c>>12);
            *d++ = 0x80|((c>>6)&0x3f);
            *d++ = 0x80|(c&0x3f);
            //if (c >= 0xff60 && c <= 0xff9f) {--(*adn); }  // 半角カナなら、半角文字扱い.
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
    }
    return d;

ERR:
	while (d < e)
		*d++ = 0;
	return e;
}


/** 1文字のchar数を返す.
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
	FKS_CP_UTF8,
    utf8_islead,                // Cがマルチバイト文字の1バイト目か?
    utf8_chkC,                  // 文字コードが正しい範囲にあるかチェック.
    utf8_getC,                  // 1字取り出し＆ポインタ更新.
    utf8_peekC,                 // 一字取り出し.
	utf8_charNext,
    utf8_setC,                  // 1字書き込み.
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

static unsigned utf16le_getC(char const** ppSrc, char const* e) {
	char const* s  = (char const*)*ppSrc;
	uint32_t	c, d;
	if (s + 2 > e) goto ERR;
	c = GET16LE(s);
	if (c)
		s += 2;
	if (0xD800 <= c && c <= 0xDBFF) {
		if (s + 2 > e) goto ERR;
		d = GET16LE(s);
		if (0xDC00 <= d && d <= 0xDFFF) {
			s += 2;
			c = ((c & 0x3ff) << 10) | (d & 0x3ff);
			c += 0x10000;
		}
	}
	*ppSrc = (char const*)s;
	return c;
 ERR:
	*ppSrc = e;
 	return 0;
}

static unsigned utf16le_peekC(char const* pSrc, char const* e) {
	return utf16le_getC((char const**)&pSrc, e);
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

static char* utf16le_charNext(char const* s, char const* e) {
	uint32_t c, d;
	if (s+2 > e) return (char*)e;
	c = GET16LE(s);
	if (c)
		s += 2;
	if (0xD800 <= c && c <= 0xDBFF) {
		if (s+2 > e) return (char*)e;
		d = GET16LE(s);
		if (0xDC00 <= d && d <= 0xDFFF) {
			s += 2;
		}
	}
	return (char*)s;
}

static unsigned utf16le_len1(char const* s, char const* e) {
	return utf16le_charNext(s, e) - s;
}

MBC_IMPL(utf16le)

static Fks_MbcEnv const fks_mbcEnv_utf16le = {
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

static unsigned utf16be_getC(char const** ppSrc, char const* e) {
	char const* s  = (char const*)*ppSrc;
	uint32_t	c, d;
	if (s + 2 > e) goto ERR;
	c = GET16BE(s);
	if (c)
		s += 2;
	if (0xD800 <= c && c <= 0xDBFF) {
		if (s + 2 > e) goto ERR;
		d = GET16BE(s);
		if (0xDC00 <= d && d <= 0xDFFF) {
			s += 2;
			c = ((c & 0x3ff) << 10) | (d & 0x3ff);
			c += 0x10000;
		}
	}
	*ppSrc = (char const*)s;
	return c;
 ERR:
	*ppSrc = e;
 	return 0;
}

static unsigned utf16be_peekC(char const* pSrc, char const* e) {
	return utf16be_getC((char const**)&pSrc, e);
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

static unsigned utf16be_chrLen(unsigned c) {
	return c > 0xffff ? 4 : c ? 2 : 0;
}

static char* utf16be_charNext(char const* s, char const* e) {
	uint32_t c, d;
	if (s >= e) return (char*)e;
	c = GET16BE(s);
	if (c)
		s += 2;
	if (0xD800 <= c && c <= 0xDBFF) {
		if (s >= e) return (char*)e;
		d = GET16BE(s);
		if (0xDC00 <= d && d <= 0xDFFF) {
			s += 2;
		}
	}
	return (char*)s;
}

static unsigned utf16be_len1(char const* s, char const* e) {
	return utf16be_charNext(s, e) - s;
}


MBC_IMPL(utf16be)

static Fks_MbcEnv const fks_mbcEnv_utf16be = {
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

static unsigned utf32le_getC(char const** ppSrc, char const* e) {
	char const* s = (char const*)*ppSrc;
	uint32_t	c;
	if (s + 4 <= e) {
		c = GET32LE(s);
		if (c)
			s += 4;
	} else {
		c = 0;
		s = e;
	}
	*ppSrc = e;
	return c;
}

static unsigned utf32le_peekC(char const* s, char const* e) {
	return (s+4 <= e) ? GET32LE(s) : 0;
}

static char* utf32le_setC(char* d, char* e, unsigned c) {
	if (d+4 <= e) {
		SET32LE(d, c);
		return d + 4;
	} else {
		return e;
	}
}

static unsigned utf32le_chrLen(unsigned c) {
	return c > 0 ? 4 : 0;
}

static char* utf32le_charNext(char const* s, char const* e) {
	return (char*)s + utf32le_len1(s, e);
}

static unsigned utf32le_len1(char const* s, char const* e) {
	return ((s+4 <= e) && *(uint32_t const*)s) ? 4 : 0;
}

MBC_IMPL(utf32le)

static Fks_MbcEnv const fks_mbcEnv_utf32le = {
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

static unsigned utf32be_getC(char const** ppSrc, char const* e) {
	char const* s = (uint32_t const*)*ppSrc;
	uint32_t	c;
	if (s + 4 <= e) {
		c = GET32BE(s);
		if (c)
			s += 4;
	} else {
		c = 0;
		s = e;
	}
	*ppSrc = e;
	return c;
}

static unsigned utf32be_peekC(char const* pSrc, char const* e) {
	return (s+4 <= e) ? GET32BE(pSrc) : 0;
}

static char* utf32be_setC(char* d, char* e, unsigned c) {
	if (d+4 <= e) {
		SET32BE(d, c);
		return d + 4;
	} else {
		return e;
	}
}

static unsigned utf32be_chrLen(unsigned c) {
	return c > 0 ? 4 : 0;
}

static char* utf32be_charNext(char const* s, char const* e) {
	return (char*)s + utf32be_len1(s,e);
}

static unsigned utf32be_len1(char const* s, char const* e) {
	return ((s+4 <= e) && *(uint32_t const*)s) ? 4 : 0;
}

MBC_IMPL(utf32be)

static Fks_MbcEnv const fks_mbcEnv_utf32be = {
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
	utf32be_chrsToSize,
	utf32be_sizeToChrs,
	utf32be_cmp,
};
Fks_MbcEnv const* const fks_mbc_utf32be = &fks_mbcEnv_utf32be;



// ---------------------------------------------------------------------------
// ascii

static char*    asc_setC(char*  d, char* e, unsigned c) { return (d < e) ? (*d++ = c):0, d; }
static unsigned asc_chrLen(unsigned chr) { return 1; }
static unsigned asc_chrWidth(unsigned chr) { return 1; }
static unsigned asc_islead(unsigned c) { return 0; }
static unsigned asc_istrail(unsigned c) { return 0; }
static unsigned asc_chkC(unsigned c) { return 1; }
static unsigned asc_getC(char const** pp, char const* e) { return (*pp < e) ? *((*pp)++) : 0; }
static unsigned asc_peekC(char const* s, char const* e) { return (s < e) ? *(unsigned char*)s : 0; }
static unsigned asc_len1(char const* s, char const* e) { return (s < e && *s); }
static char*	asc_charNext(char const* s, char const* e) { return (s < e) ? s + 1 : e; }

MBC_IMPL(asc)

static Fks_MbcEnv const fks_mbcEnv_asc = {
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
	asc_chrsToSize,
	asc_sizeToChrs,
	asc_cmp,
};
Fks_MbcEnv const* const fks_mbc_asc = &fks_mbcEnv_asc;


// ---------------------------------------------------------------------------
// DBC (win)

/** 1字書き込み.
 */
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


/** DBC 1バイト目か?
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
static unsigned dbc_chkC(unsigned c)
{
    if (c > 0xff) {
        return IsDBCSLeadByte(c >> 8) && dbc_istrail((unsigned char)c);
    }
    return 1;
}


/** 1字取り出し＆ポインタ更新.
 */
static unsigned dbc_getC(char const** pStr, char const* e) {
    const unsigned char* s = (unsigned char*)*pStr;
    unsigned       c;
    if ((char*)s >= e) goto ERR;
    c = *s++;
    if (IsDBCSLeadByte(c)) {
	    if ((char*)s >= e) goto ERR;
		if (*s) {
	        c = (c << 8) | *s;
    	    ++s;
    	}
    }
    *pStr = (char const*)s;
    return c;
 ERR:
 	*pStr = e;
	return 0;
}


/** 一字取り出し.
 */
static unsigned dbc_peekC(char const* s, char const* e) {
    unsigned       c;
    if (s >= e) return 0;
    c = *(unsigned char*)(s++);
    if (IsDBCSLeadByte(c)) {
	    if (s >= e) return 0;
    	if (*s)
        	c = (c << 8) | *s;
    }
    return c;
}


/** 1文字のchar数を返す.
 */
static unsigned dbc_len1(char const* s, char const* e) {
	if (s >= e || *s == 0)
		return 0;
	if (IsDBCSLeadByte(*s)) {
		if (s >= e) return 0;
		return 1 + (s[1] != 0);
	}
	return 1;
}

static char* dbc_charNext(char const* s, char const* e) {
	//return (char*)CharNext(s);
	if (s >= e || *s == 0)
		return e;
	if (IsDBCSLeadByte(*s)) {
		if (s >= e) return e;
		return s + 1 + (s[1] != 0);
	}
	return s;
}

MBC_IMPL(dbc)

static Fks_MbcEnv const fks_mbcEnv_dbc = {
	0,
    dbc_islead,                 // Cがマルチバイト文字の1バイト目か?
    dbc_chkC,                   // 文字コードが正しい範囲にあるかチェック.
    dbc_getC,                   // 1字取り出し＆ポインタ更新.
    dbc_peekC,                  // 一字取り出し.
	dbc_charNext,
    dbc_setC,                   // 1字書き込み.
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

#if 0
size_t    fks_mbcAdjustSize(Fks_MbcEnv const* mbc, char const* str, size_t size) {
    char const* s = str;
    char const* b = s;
    char const* e = s + size;
    FKS_ASSERT(str != 0 && size > 0);
    if (e < s)
        e = (char const*)(~(size_t)0);
    while (s < e) {
        if (*s == 0)
            return s - str;
        b = s;
        s += mbc->len1(s);
    }
    return b - str;
}


/// 領域サイズから(意味的な)文字数を求める
size_t  fks_mbcSizeToChrs(Fks_MbcEnv const* mbc, char const* str, size_t size) {
    char const* s = str;
    char const* e = s + size;
    //char const* b = s;
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
size_t  fks_mbcChrsToSize(Fks_MbcEnv const* mbc, char const* str, size_t chrs) {
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

/** '\0'終端文字列比較. 文字値が intの正数範囲に収まることに依存.
 */
int fks_mbcCmp(Fks_MbcEnv const* mbc, char const* lp, char const* rp) {
    int lc, rc;
    int d;
    FKS_ASSERT(lp != NULL);
    FKS_ASSERT(rp != NULL);
    do {
        lc = mbc->getC(&lp);
        rc = mbc->getC(&rp);
        d  = lc - rc;
    } while (d == 0 && lc);
    return d;
}
#endif


/// 領域サイズから半角文字単位の幅を求める.
size_t  fks_mbcSizeToWidth(Fks_MbcEnv const* mbc, char const* str, size_t size) {
    char const* s = str;
    char const* e = s + size;
    size_t      b = 0;
    size_t      w = 0;
    if (e < s)
        e = (char const*)(~(size_t)0);
    FKS_ASSERT(str != 0 && size > 0);
    while (s < e) {
        unsigned c  = mbc->getC(&s, e);
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
size_t  fks_mbcChrsToWidth(Fks_MbcEnv const* mbc, char const* str, size_t chrs) {
    char const* s = str;
    size_t      w = 0;
    FKS_ASSERT(str != 0);
    while (chrs) {
        unsigned c  = mbc->getC(&s, (char*)(~(size_t)0));
        if (c == 0)
            break;
        w += mbc->chrWidth(c);
        --chrs;
    }
    return w;
}


/// 半角文字単位の幅から領域サイズを求める.
size_t  fks_mbcWidthToSize(Fks_MbcEnv const* mbc, char const* str, size_t width) {
    char const* s = str;
    char const* b = s;
    size_t      w = 0;
    FKS_ASSERT(str != 0);
    while (w < width) {
        unsigned c;
        b  = s;
        c  = mbc->getC(&s, (char*)(~(size_t)0));
        if (c == 0)
            break;
        w += mbc->chrWidth(c);
    }
    if (w > width)
        s = b;
    return s - str;
}


/// 半角文字単位の幅から文字数を求める.
size_t  fks_mbcWidthToChrs(Fks_MbcEnv const* mbc, char const* str, size_t width) {
    char const* s = str;
    char const* b;
    size_t      w = 0;
    size_t      n = 0;
    FKS_ASSERT(str != 0);
    while (w < width) {
        unsigned c;
        b  = s;
        c  = mbc->getC(&s, (char*)(~(size_t)0));
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
size_t  fks_mbcCpy(Fks_MbcEnv const* mbc, char dst[], size_t dstSz, char const* src)
{
    size_t    l;
    FKS_ASSERT(dst != NULL && dstSz > 0 && src != NULL);

    l = mbc->adjustSize(src, dstSz);
	return fks_mbcLCpy(mbc, dst, dstSz, src, l+1);
}

size_t fks_mbcLCpy(Fks_MbcEnv const* mbc, char dst[], size_t dstSz, char const* src, size_t l)
{
    FKS_ASSERT(dst != NULL && dstSz > 0 && src != NULL);

	if (l > dstSz)
		l = dstSz;
    // アドレスが同じなら、長さをあわせるのみ.
    if (dst == src) {
        dst[l-1] = 0;
        return dst;
    }

    // コピー.
    {
        char const* s = src;
        char const* e = s + l;
        char*       d = dst;
        while (s < e)
            *d++ = *s++;
        *d = 0;
    }

    return d - dst;
}



/** 文字列の連結.
 */
char*   fks_mbcCat(Fks_MbcEnv const* mbc, char dst[], size_t dstSz, char const* src)
{
    size_t l;
    FKS_ASSERT(dst != NULL && dstSz > 0 && src != 0 && dst != src);
    l = mbc_raw_len(dst);
    if (l >= dstSz) {   // そもそも転送先が満杯ならサイズ調整のみ.
        return fks_mbcCpy(mbc, dst, dstSz, dst);
    }
    fks_mbcCpy(mbc, dst+l, dstSz - l, src);
    return dst;
}



/** コピー. mbcの時は文字が壊れない部分まで. dst == src もok.
 */
char*   fks_mbcCpyNC(Fks_MbcEnv const* mbc, char dst[], size_t dstSz, char const* src, size_t nc)
{
    size_t    l;
    FKS_ASSERT(dst != NULL && dstSz > 0 && src != 0 && dst != src);
    l = mbc->chrsToSize(src, nc) + 1;
    l = dstSz < l ? dstSz : l;
    return fks_mbcCpy(mbc, dst, l, src);
}



/** 文字列の連結.
 */
char*   fks_mbcCatNC(Fks_MbcEnv const* mbc, char dst[], size_t dstSz, char const* src, size_t nc)
{
    size_t l, l2;
    FKS_ASSERT(dst != NULL && dstSz > 0 && src != 0 && dst != src);
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
char*   fks_mbcCpyWidth(Fks_MbcEnv const* mbc, char dst[], size_t dstSz, char const* src, size_t width)
{
    size_t    l = fks_mbcWidthToSize(mbc, src, width) + 1;
    l = dstSz < l ? dstSz : l;
    return fks_mbcCpy(mbc, dst, l, src);
}



/** 文字列の連結.
 */
char*   fks_mbcCatWidth(Fks_MbcEnv const* mbc, char dst[], size_t dstSz, char const* src, size_t width)
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
    fks_mbcCpy(mbc, dst+l, l2, src);
    return dst;
}



// --------------------------------------------------------------------------
// UNICODE

int fks_mbc_checkUnicodeBOMi(char const* src, size_t len)
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

int fks_mbc_checkUnicodeBOM(char const* src, size_t len)
{
	static Fks_MbcEnv const* s_tbl[] = {
		NULL,
		fks_mbc_utf8,
		fks_mbc_utf16le,
		fks_mbc_utf16be,
		fks_mbc_utf32le,
		fks_mbc_utf32be,
	};
	return s_tbl[ fks_mbc_checkUnicodeBOMi(src, len) ];
}



int Fks_MbcEnvoCheckUnicodeBomNumber(Fks_MbcEnv const* env) {
	if (env == fks_mbc_utf8) {
		return 1;
	} else (env == fks_mbc_utf16le) {
		return 2;
	} else (env == fks_mbc_utf16be) {
		return 3;
	} else (env == fks_mbc_utf32le) {
		return 4;
	} else (env == fks_mbc_utf32be) {
		return 5;
	}
	return 0;
}


size_t   fks_mbc_unicodeConv(Fks_MbcEnv const* dstMbc, char dst[], size_t dstSz, Fks_MbcEnv const* srcMbc, char const* src, size_t srcSz)
{
	FKS_ARG_ASSERT(1, Fks_MbcEnvoCheckUnicodeBomNumber(dstMbc) > 0);
	FKS_ARG_PTR_ASSERT(2, dst);
	FKS_ARG_PTR_ASSERT(3, dstSz > 0);
	FKS_ARG_ASSERT(4, Fks_MbcEnvoCheckUnicodeBomNumber(srcMbc) > 0);
	FKS_ARG_PTR_ASSERT(5, src);

	if (dstMbc == srcMbc) {
		return fks_mbcLCpy(dstMbc, dst, dstSz, src, srcSz);
	} else {
		char const* s  = src;
		char const* se = src + srcSz;
		char* d = dst;
		char* de = dst + dstSz;
		while (d + 6 < de && s < se) {
			unsigned c = srcMbc->getC(&s);
			d = dstMbc->setC(d, c);
		}
		*d = 0;
		return d - dst;
	}
}


/** Check UTF8 Encode?
 * @return 0=not  1=ascii(<=7f) 2,3,4=ut8
 */
int  fks_mbc_checkUTF8(char const* src, size_t len, int lastBrokenOk)
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

		if (s >= e) { badFlg = !lastBrokenOk; break; }
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
			if (!lastBrokenOk)
				badFlg = 1;
			break;
		}

		if (s >= e) { badFlg = !lastBrokenOk; break; }
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

		if (s >= e) { badFlg = !lastBrokenOk; break; }
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

		if (s >= e) { badFlg = !lastBrokenOk; break; }
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

		if (s >= e) { badFlg = !lastBrokenOk; break; }
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


#ifdef FKS_WIN32
Fks_MbcEnv const* fks_mbcEnv_makeDBC(Fks_MbcEnv* mbcEnv, fks_codepage_t cp) {
	*mbcEnv = fks_mbcEnv_dbc;
	mbcEnv->cp = cp;
	return mbcEnv;
}

size_t   fks_mbc_dbcFromUnicode(fks_codepage_t dstCP, char dst[], size_t dstSz, Fks_MbcEnv const* srcMbc, char const* src, size_t srcSz)
{
	wchar_t const* ws  = (wchar_t const*)src;
	size_t         wsl = srcSz;
	if (srcMbc != fks_mbc_utf16le) {
		size_t tl = (srcMbc == fks_mbc_utf18) ? srcSz * 3 : (srcMbc == fks_mbc_utf16be) ? srcSz : srcSz*2;
		ws  = (wchar_t*)fks_alloca(tl);
		wsl = fks_mbc_unicodeConv(fks_mbc_utf16le, ws, tl, srcMbc, srcSz);
	}
	return WideCharToMultiByte(dstCP,0,ws,wl,dst,dstSz,0,0);
}

size_t   fks_mbc_unicodeFromDbc(Fks_MbcEnv const* dstMbc, char dst[], size_t dstSz, fks_codepage_t srcCP, char const* src, size_t srcSz)
{
	wchar_t const* wd  = (wchar_t const*)dst;
	size_t         wdl = dstSz;
	if (dstMbc != fks_mbc_utf16le) {
		wdl = srcSz * 4;
		wd  = (wchar_t*)fks_alloca(wdl);
	}
    l = MultiByteToWideChar(srcCP,0,src,srcSz,wd,wdl);
	if (dstMbc == fks_mbc_utf16le)
    	return l;
	return fks_mbc_unicodeConv(dstMbc, dst, dstMbc, fks_mbc_utf16le, l);
}

size_t   fks_mbc_unicodeDbcConv(Fks_MbcEnv const* dstMbc, char dst[], size_t dstSz, Fks_MbcEnv const* srcMbc, char const* src, size_t srcSz)
{
	FKS_ARG_ASSERT(1, Fks_MbcEnvoCheckUnicodeBomNumber(dstMbc) > 0 || dstMbc == fks_mbc_dbc);
	FKS_ARG_PTR_ASSERT(2, dst);
	FKS_ARG_PTR_ASSERT(3, dstSz > 0);
	FKS_ARG_ASSERT(4, Fks_MbcEnvoCheckUnicodeBomNumber(srcMbc) > 0 || srcMbc == fks_mbc_dbc);
	FKS_ARG_PTR_ASSERT(5, src);

	if (dstMbc == srcMbc)
		return fks_mbcLCpy(dstMbc, dst, dstSz, src, srcSz);
	else if (Fks_MbcEnvoCheckUnicodeBomNumber(dstMbc) && Fks_MbcEnvoCheckUnicodeBomNumber(srcMbc))
		return fks_mbc_unicodeConv(dstMbc, dst, dstSz, srcMbc, src, srcSz);
	else if (dstMbc == fks_mbc_dbc)
		return fks_mbc_dbcFromUnicode(dstMbc->cp, dst, dstSz, srcMbc, src, srcSz);
	else
		return fks_mbc_unicodeFromDbc(dstMbc, dst, dstSz, srcMbc->cp, src, srcSz);
}
#endif


#ifdef __cplusplus
}
#endif
