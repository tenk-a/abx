#include <fks_mbc.h>
#include "detail/fks_mbc_sub.h"


#ifdef __cplusplus
extern "C" {
#endif

extern int fks_mbcEnv_toCheckUnicodeBomNumber(Fks_MbcEnv const* env);
extern char*    fks_dbc_setC(char*  d, char* e, unsigned c);

// ---------------------------------------------------------------------------

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


/** 全角の1バイト目か?
 */
static unsigned sjis_islead(unsigned c) {
    //return (c >= 0x81) && ((c <= 0x9F) || ((c >= 0xE0) & (c <= 0xFC)));
    return (c >= 0x81) && (c <= 0xFC) && ((c <= 0x9F) || (c >= 0xE0));
}

static unsigned sjis_istrail(unsigned c) {
    //return ((c >= 0x40 && c <= 0x7e) || (c >= 0x81 && c <= 0xFC));
    return (c >= 0x40) && (c <= 0xFC) && ((c <= 0x7e) || (c >= 0x81));
}

/** 文字コードが正しい範囲にあるかチェック.
 */
static unsigned sjis_chkC(unsigned c)
{
    if (c > 0xff) {
        return sjis_islead(c >> 8) && sjis_istrail((unsigned char)c);
    }
    return 1;
}


/** 1字取り出し＆ポインタ更新.
 */
static unsigned sjis_getC(char const** pStr, char const* e) {
    const unsigned char* s = (unsigned char*)*pStr;
    unsigned       c;
    if (s >= e) goto ERR;
    c = *s++;
    if (sjis_islead(c)) {
	    if (s >= e) goto ERR;
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
static unsigned sjis_peekC(char const* s, char const* e) {
    unsigned       c;
    if (s >= e) return 0;
    c = *(unsigned char*)(s++);
    if (sjis_islead(c)) {
	    if (s >= e) return 0;
    	if (*s)
        	c = (c << 8) | *s;
    }
    return c;
}


/** 1文字のchar数を返す.
 */
static unsigned sjis_len1(char const* s, char const* e) {
	if (s >= e || *s == 0)
		return 0;
	if (sjis_islead(*s)) {
		if (s >= e) return 0;
		return 1 + (s[1] != 0);
	}
	return 1;
}

static char* sjis_charNext(char const* s, char const* e) {
	//return (char*)CharNext(s);
	if (s >= e || *s == 0)
		return e;
	if (sjis_islead(*s)) {
		if (s >= e) return e;
		return s + 1 + (s[1] != 0);
	}
	return s;
}

static FKS_FORCE_INLINE unsigned sjis_chrLen(unsigned chr) {
    return 1 + (chr > 0xff);
}

MBC_IMPL(sjis)

#define sjis_setC			fks_dbc_setC

Fks_MbcEnv const fks_mbcEnv_sjis = {
	FKS_CP_SJIS,
    sjis_islead,                    // Cがマルチバイト文字の1バイト目か?
    sjis_chkC,                      // 文字コードが正しい範囲にあるかチェック.
    sjis_getC,                      // 1字取り出し＆ポインタ更新.
    sjis_peekC,                     // 一字取り出し.
	sjis_charNext,
    sjis_setC,                      // 1字書き込み.
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


/** 文字コードが正しい範囲にあるかチェック.
 */
static unsigned eucjp_chkC(unsigned c)
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
static unsigned eucjp_getC(char const** pStr, char const* e) {
    unsigned char const* s = (unsigned char const*)*pStr;
    unsigned	c, k;
    if (s >= e) goto ERR;
    c = *s++;
    if (euc_islead(c)) {
	    if (s >= e) goto ERR;
		if (*s) {
	        unsigned   k  = c;
	        c = (c << 8) | *s++;
	        if (k == 0x8f) {
			    if (s >= e) goto ERR;
				if (*s)
		            c = (c << 8) | *s++;
	        }
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
static unsigned eucjp_peekC(char const* pStr, char const* e) {
    unsigned char const* s = (unsigned char const*)*pStr;
    unsigned       		 c;
	if (s >= e) return 0;
    c = *s++;
    if (euc_islead(c)) {
		if (s >= e) return 0;
		if (*s) {
	        unsigned   k  = c;
	        c = (c << 8) | *s++;
	        if (k == 0x8f) {
				if (s >= e) return 0;
				if (*s)
		            c = (c << 8) | *s++;
	        }
	    }
    }
    return c;
}


static char* eucjp_charNext(char const* pChr, char const* e) {
	return (char*)pChr + eucjp_len1(pChr, e);
}

/** 1文字のchar数を返す.
 */
static unsigned eucjp_len1(char const* s, char const* e) {
    unsigned char const* s = (unsigned char const*)*pStr;
    unsigned       		 c;
	if (s >= e) return 0;
    c = *s++;
    if (euc_islead(c)) {
		if (s >= e) return 0;
		if (c == 0x8f) {
			if (s+1 >= e || *s == 0 || s[1] == 0) return 0;
			return 3;
		} else {
			if (s >= e || *s == 0) return 0;
			return 2;
		}
	} else {
		return *s ? 1 : 0;
	}
}


/** 1字書き込み.
 */
static char*    eucjp_setC(char*  d, char *e, unsigned c) {
	if (c <= 0xff) {
		if (d >= e) goto ERR;
		*d++ = c;
	} else if (c <= 0xFFFF) {
		if (d+2 > e) goto ERR;
        *d++ = c >> 8;
		*d++ = c;
	} else {
		if (d+3 > e) goto ERR;
        *d++ = c >> 16;
        *d++ = c >> 8;
		*d++ = c;
	}
	return d;

ERR:
	while (d < e)
		*d++ = 0;
	return e;
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


static FKS_FORCE_INLINE unsigned eucjp_chrLen(unsigned chr) {
	return 1 + (chr > 0xff);
}

MBC_IMPL(eucjp)


static Fks_MbcEnv const fks_mbcEnv_eucJp = {
	FKS_CP_EUCJP,
    euc_islead,                     // Cがマルチバイト文字の1バイト目か?
    eucjp_chkC,                     // 文字コードが正しい範囲にあるかチェック.
    eucjp_getC,                     // 1字取り出し＆ポインタ更新.
    eucjp_peekC,                    // 一字取り出し.
	eucjp_charNext,
    eucjp_setC,                     // 1字書き込み.
    eucjp_len1,                     // 1文字のchar数を返す.
    dbc_chrLen,                     // 1文字のchar数を返す.
    eucjp_chrWidth,                 // 半角全角を考慮して文字の幅を返す.
    eucjp_chrWidth,                 // 半角全角を考慮して文字の幅を返す.
	eucjp_adjustSize,
	eucjp_chrsToSize,
	eucjp_sizeToChrs,
	eucjp_cmp,
};
Fks_MbcEnv const* const fks_mbc_eucJp = &fks_mbcEnv_eucJp;



// --------------------------------------------------------------------------
// JIS

/// jis to eucJp
#define FKS_JIS2EUCJP(jis)	((jis) | 0x8080)
#define FKS_EUCJP2JIS(euc)	((euc) & ~0x8080)


static int fks_jis2eucjp(int c)
{
	if (c < 0x80) {
		return c;
	} else if ( c <= 0xff) {
		return 0x8e00|c;
	} else if (c <= 0xffff) {
		return 0x8080|c;
	} else {
		return 0xf88080 | (uint16_t)c;
	}
}

static int fks_eucjp2jis(int c)
{
	if (c <= 0xff) {
		return c;
	} else if ((c & 0xff00) == 0x8e00) {
		return (uint8_t)c;
	} else if (c <= 0xffff) {
		return c & ~0x8080;
	} else {
		return 0x10000|((uint16_t)c & ~0x8080);
	}
}


/// jis to sjis
static int fks_jis2sjis(unsigned c)
{
	if (c <= 0xffff) {
	    c -= 0x2121;
	    if (c & 0x100)
	    	c += 0x9e;
	    else
	    	c += 0x40;
	    if ((uint8_t)c >= 0x7f)
	    	++c;
	    c = (((c >> (8+1)) + 0x81)<<8) | ((uint8_t)c);
	    if (c >= 0xA000)
	    	c += 0x4000;
	    return c;
	} else {
		unsigned a, b;
		b = (uint16_t)c - 0x2121;
		a = b >> 8;
	    if (b & 0x100)
	    	b += 0x9e;
	    else
	    	b += 0x40;
	    b = (uint8_t)b;
		if (b >= 0x7f)
			++b;
		if (a < 78-1) {	// 1,3,4,5,8,12,15-ku (0,2,3,4,7,11,14)
			a = (a + 1 + 0x1df) / 2 - ((a+1)/8) * 3;
		} else { // 78..94
			a = (a + 1 + 0x19b) / 2;
		}
		return (a << 8) | b;
	}
}

/// sjis to jis
#if 0
static int fks_sjis2jis(unsigned c)
{
    if (c >= 0xE000)
    	c -= 0x4000;
    c = (((c>>8) - 0x81)<<9) | (uint8_t)c;
    if ((uint8_t)c >= 0x80)
    	c -= 1;
    if ((uint8_t)c >= 0x9e)
    	c += 0x62;
    else
    	c -= 0x40;
    c += 0x2121;
    return c;
}
#else // 2004
static int fks_sjis2jis(unsigned c)
{
	if (c < 0xf000) {
	    if (c >= 0xE000)
	    	c -= 0x4000;
	    c = (((c>>8) - 0x81)<<9) | (uint8_t)c;
	    if ((uint8_t)c >= 0x80)
	    	c -= 1;
	    if ((uint8_t)c >= 0x9e)
	    	c += 0x62;
	    else
	    	c -= 0x40;
	    c += 0x2121;
	    return c;
	} else {
		unsigned a, b, f;
		b = (uint8_t)c;
		f = (b >= 0x9f);
		if (c < 0xf29f) {
			if (c < 0xf100) {
				a = (f) ? 0x28 : 0x21;
			} else {
				a = (a - 0xf1) * 2 + 0x23 + f;
			}
		} else {
			if (c < 0xf49f) {
				a = (a - 0xf2) * 2 + 0x2c - 1 + f;
			} else {
				a = (a - 0xf4) * 2 + 0x6e - 1 + f;
			}
		}
		if (b <= 0x7e) {
			b  = b - 0x40 + 0x21;
		} else if (b <= 0x9e) {
			b  = b - 0x80 + 0x60;
		} else {
			b  = b - 0x9f + 0x21;
		}
		return (a << 8)|b;
	}
}
#endif

size_t  fks_sjisFromEucjp(char dst[], size_t dstSz, char const* src, size_t srcSz)
{
	char const* s  = src;
	char const* se = src + srcSz;
	char* 		d  = dst;
	char* 		de = dst + dstSz;
	while (d < de && s < se) {
		if (*s < 0x80) {
			if (c == 0)
				break;
			*d++ = *s++;
		} else {
			unsigned c;
			if (d + 1 >= de || s + 1 >= se)
				break;
			c  = eucjp_getC(&s);
			c  = fks_jis2sjis(fks_eucjp2jis(c));
			d = sjis_setC(d,c); // d = dstMbc->setC(d, c);
		}
	}
	*d = 0;
    return d - dst;
}


size_t  fks_eucjpFromSjis(char dst[], size_t dstSz, char const* src, size_t srcSz)
{
	char const* s  = src;
	char const* se = src + srcSz;
	char* 		d  = dst;
	char* 		de = dst + dstSz;

	while (d < de && s < se) {
		unsigned c = *s;
		if (c < 0x80) {
			if (c == 0)
				break;
			*d++ = *s++;
		} else {
			if (d + 1 >= de || (c == 0x8f && d + 2 >= de))
				break;
			c = sjis_getC(&s);
			c = fks_jis2eucjp(fks_sjis2jis(c));
			eucjp_setC(c);
		}
	}
	*d = 0;
	return d - dst;
}


// --------------------------------------------------------------------------

/**
 */
size_t	fks_mbcCountCapa(Fks_MbcEnv const* dstMbc, Fks_MbcEnv const* srcMbc, char const* src, size_t srcSz)
{
	char const* s  = src;
	char const* se = src + srcSz;
	size_t sz = 0;
	int dstUni = fks_mbcEnv_toCheckUnicodeBomNumber(dstMbc);
	int srcUni = fks_mbcEnv_toCheckUnicodeBomNumber(srcMbc);
	if (dstMbc == srcMbc) {
		sz = srcSz;
	} else if (dstUni > 0) {
		if (srcUni > 0) {
			while (s < se) {
				unsigned c = srcMbc->getC(&s);
				if (c)
					break;
				sz += dstMbc->chrLen(c);
			}
		} else {
			size_t c1 = (dstUni + 1) / 2;
			if (c1 == 3)
				++c1;
			while (s < se) {
				unsigned c = srcMbc->getC(&s);
				sz += (c < 0x80) ? c1 : 4;
			}
		}
	} else if (dstMbc == fks_mbc_sjis) {
		if (srcMbc == fks_mbc_eucJp) {
			sz = srcSz;
		} else {
			while (s < se) {
				unsigned c = srcMbc->getC(&s);
				sz += (c < 0x80) ? 1 : 2;
			}
		}
	} else if (dstMbc == fks_mbc_eucJp) {
		if (srcMbc == fks_mbc_sjis) {
			sz = srcSz;
			while (s < se) {
				if (0xA0 <= *s && *s <= 0xDF) {
					sz += 1;
				}
				++s;
			}
		} else {
			while (s < se) {
				unsigned c = srcMbc->getC(&s);
				sz += (c < 0x80) ? 1 : 3;
			}
		}
	} else {
		while (s < se) {
			unsigned c = srcMbc->getC(&s);
			sz += (c < 0x80) ? 1 : 4;
		}
	}
	return sz;
}



// --------------------------------------------------------------------------

/** Check Shift-JIS Encode?
 * @return 0=not  (1:ascii)  2,3,4=sjis (2=use HANKAKU-KANA)
 */
int  fks_mbc_checkSJIS(char const* src, size_t len, int lastBrokenOk)
{
	unsigned char const* s = (unsigned char const*)src;
	unsigned char const* e = s + len;
	//char	hasAsc  = 0;
	char	kataFlg = 0;
	char	badFlg  = 0;
	char	zenFlg  = 0;
	char	lowAsc  = 0;
	int		c;
	if (len == 0)
		return 0;
	while (s < e) {
		c = *s++;
		if (c == '\0') {
			badFlg = 1;
			break;
		}
		if  (c <= 0x7f) {
			//hasAsc = 1;
			continue;
		} else if (c >= 0xA0 && c <= 0xDF) {
			kataFlg = 1;
			continue;
		}
		if (s >= e) {
			if (!lastBrokenOk)
				badFlg = 1;
			break;
		}
		if (c >= 0x81 && c <= 0xfe) {
			c = *s;
			if (c) {
				++s;
				if (c >= 0x40 && c <= 0xfe && c != 0x7f) {
					if (c < 0x7f)
						lowAsc = 1;
					zenFlg = 1;
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
	if (badFlg)
		return 0;
	if (zenFlg)
		return 4; //(lowAsc) ? 4 : 3;
	if (kataFlg)
		return 3;
	//if (hasAsc)
	//	return 1;
	return 0;
}


/** Check EUC-JP Encode?
 * @return 0=not  (1=ascii) 2,3,4=euc-jp  (2=use HANKAKU-KANA)
 */
int  fks_mbc_checkEucJp(char const* src, size_t len, int lastBrokenOk)
{
	unsigned char const* s = (unsigned char*)src;
	unsigned char const* e = s + len;
	//char	hasAsc  = 0;
	char	kataFlg = 0;
	char	badFlg  = 0;
	char	zenFlg  = 0;
	char	c8f     = 0;
	int		c;
	while (s < e) {
		c = *s++;
		if  (c < 0x80) {
			if (c == '\0') {
				badFlg = 1;
				break;
			}
			//hasAsc = 1;
			continue;
		}
		if (s >= e) { badFlg = !lastBloenOk; break; }
		if (c == 0x8e) {	// hankaku-kana
			c = *s;
			if (c) {
				++s;
				if (c >= 0xA0 && c <= 0xDF) {
					kataFlg = 1;
				} else {
					badFlg  = 1;
					break;
				}
			} else {
				badFlg = 1;
				break;
			}
			continue;
		}
		if (c == 0x8f) {
			c8f = 1;
			if (s >= e) { badFlg = !lastBloenOk; break; }
			c = *s++;
		}
		if (c >= 0xa1 && c <= 0xfe) {
			c = *s;
			if (c) {
				++s;
				if (c >= 0xa1 && c <= 0xfe) {
					zenFlg = 1;
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
	if (badFlg)
		return 0;
	if (zenFlg)
		return (kataFlg || c8f) ? 3 : 4;
	if (kataFlg)
		return 2;
	//if (hasAsc)
	//	return 1;
	return 0;
}


/** Japanese encode?
 */
fks_mbcEnv_t const* fks_mbc_checkJpEncode(char const* src, size_t len, int lastBrokenOk, fks_mbcEnv_t const* dflt)
{
	int utf8, sjis, euc;

	fks_mbcEnv_t const* uck = fks_mbc_checkUnicodeBOM(src, len);
	if (uck)
		return uck;

	utf8 = fks_mbc_checkUTF8(src, len, lastBrokenOk);
	if (utf8 == 1)	// ascii
		return fks_mbcEnv_asc;
	sjis = fks_mbc_checkSJIS(src, len, lastBrokenOk);
	euc  = fks_mbc_checkEucJp(src, len, lastBrokenOk);

	if (utf8 > 0) {
		if (utf8 > euc && utf8 > sjis)
			return fks_mbcEnv_utf8;
		if ((dflt == fks_mbcEnv_utf8 || dflt == 0) && utf8 >= euc && utf8 >= sjis)
			return fks_mbcEnv_utf8;
	}
	if (sjis > 0) {
		if (sjis > utf8 && sjis > euc)
			return fks_mbcEnv_sjis;
		if ((dflt == fks_mbcEnv_sjis || dflt == 0) && sjis >= utf8 && sjis >= euc)
			return fks_mbcEnv_sjis;
	}
	if (euc > 0) {
		if (euc > utf8 && euc > sjis)
			return fks_mbcEnv_eucJp;
		if ((dflt == fks_mbcEnv_eucJp || dflt == 0) && euc >= utf8 && euc >= sjis)
			return fks_mbcEnv_eucJp;
	}

	if (utf8 > 0) {
		if (utf8 >= euc && utf8 >= sjis)
			return fks_mbcEnv_utf8;
	}
	if (sjis > 0) {
		if (sjis >= utf8 && sjis >= euc)
			return fks_mbcEnv_sjis;
	}
	if (euc > 0) {
		if (euc >= utf8 && euc >= sjis)
			return fks_mbcEnv_eucJp;
	}

	return 0;
}

#ifdef __cplusplus
}
#endif
