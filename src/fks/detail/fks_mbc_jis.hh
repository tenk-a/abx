#include <fks_mbc.h>
#include "detail/fks_mbc_sub.h"


#ifdef __cplusplus
extern "C" {
#endif

int fks_mbcCheckSJIS(char const* src, size_t len, int flags);

typedef struct FksMbcCodeCnvPair { uint16_t	key, val; } FksMbcCodeCnvPair;
static FksMbcCodeCnvPair fks_mbc_cp932sp_to_Jis2004_tbl[] = {
#include "fks_mbc_cp932xJis2004tbl.hh"
};
enum { fks_mbc_cp932sp_to_Jis2004_tbl_size = sizeof(fks_mbc_cp932sp_to_Jis2004_tbl) / sizeof(fks_mbc_cp932sp_to_Jis2004_tbl[0]) };
static FksMbcCodeCnvPair* fks_mbc_jis2004_to_cp932sp = NULL;

extern char*    fks_dbc_setC(char*  d, char* e, unsigned c);

// ---------------------------------------------------------------------------

#if 0
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
#else
static unsigned dbc_chrLen(unsigned chr);
static unsigned dbc_chrWidth(unsigned chr);
#endif

/** 全角の1バイト目か?
 */
static unsigned sjis_islead(unsigned c) {
    //return (c >= 0x81) && ((c <= 0x9F) || ((c >= 0xE0) & (c <= 0xFC)));
    return (c >= 0x81) && (c <= 0xFC) && ((c <= 0x9F) || (c >= 0xE0));
}

/** 全角の2バイト目か?
 */
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
static unsigned sjis_getC(char const** pStr) {
    unsigned char const* s = (unsigned char*)*pStr;
    unsigned       c       = *s++;
    if (sjis_islead(c) && *s) {
        c = (c << 8) | *s++;
    }
    *pStr = (char const *)s;
    return c;
}

/** 一字取り出し.
 */
static unsigned sjis_peekC(char const* s) {
    unsigned       c       = *(unsigned char*)(s++);
    if (sjis_islead(c) && *s) {
        c = (c << 8) | *s;
    }
    return c;
}

static unsigned sjis_len1(char const* s);

/** 次の文字へポインタを進める.
 */
static char* sjis_charNext(char const* s) {
	return (char*)s + sjis_len1(s);
}

/** 1文字のchar数を返す.
 */
static unsigned sjis_len1(char const* s) {
    return (s[0] == 0) ? 0 : 1 + (sjis_islead(s[0]) && s[1]);
}

/** Check Shift-JIS Encode?
 * @return 0=not  (1:ascii)  2,3,4=sjis (2=use HANKAKU-KANA)
 */
static int sjis_checkEncoding(char const* src, size_t len, int canEndBroken)
{
	int rc = fks_mbcCheckSJIS(src, len, canEndBroken != 0);
	return (uint8_t)rc;
}

static FKS_FORCE_INLINE unsigned sjis_chrLen(unsigned chr) {
    return 1 + (chr > 0xff);
}

MBC_IMPL(sjis)

#define sjis_setC			fks_dbc_setC

Fks_MbcEnc const fks_mbcEnc_sjis = {
	FKS_CP_SJIS,					// コードページ.
    sjis_islead,                    // Cがマルチバイト文字の1バイト目か?
    sjis_chkC,                      // 文字コードが正しい範囲にあるかチェック.
    sjis_getC,                      // 1字取り出し＆ポインタ更新.
    sjis_peekC,                     // 一字取り出し.
	sjis_charNext,					// 次の文字へ.
    sjis_setC,                      // 1字書き込み.
    sjis_len1,                      // 1文字のchar数を返す.
    dbc_chrLen,                     // 1文字のchar数を返す.
    dbc_chrWidth,                   // 半角全角を考慮して文字の幅を返す.
	sjis_adjustSize,				// 最後の文字が壊れていたらそれを捨ててサイズを調整.
	sjis_cmp,						// 文字列の比較.
	sjis_checkEncoding,				// 文字列の、文字エンコードがあっているかチェック.
};
fks_mbcenc_t const fks_mbc_sjis = &fks_mbcEnc_sjis;



// ---------------------------------------------------------------------------

static unsigned euc_islead(unsigned c) {
    return (c >= 0xA1 && c <= 0xFE);
}

static unsigned eucjp_chkC(unsigned c)
{
    if (c > 0xff) {
        if (c > 0xffff) {
            if ((c >> 16) != 0x8f)
                return 0;
            c = (unsigned short)c;
        }
        return euc_islead(c >> 8) && euc_islead/*euc_istrail*/((unsigned char)c);
    }
    return 1;
}

static unsigned eucjp_getC(char const** pStr) {
    unsigned char const* s = (unsigned char*)*pStr;
    unsigned       c       = *s++;
    if (euc_islead(c) && *s) {
        unsigned   k  = c;
        c = (c << 8) | *s++;
        if (k == 0x8f && *s) {
            c = (c << 8) | *s++;
        }
    }
    *pStr = (char const*)s;
    return c;
}

static unsigned eucjp_peekC(char const* pStr) {
    unsigned char const* s = (unsigned char*)pStr;
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

static unsigned eucjp_len1(char const* s) {
    unsigned       c       = *(unsigned char const*)s;
    if (euc_islead(c) && s[1]) {
        if (c == 0x8f && s[2])
            return 3;
        return 2;
    }
    return (s[0] != 0);
}

static char* eucjp_charNext(char const* s) {
	return (char*)s + eucjp_len1(s);
}

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

static unsigned eucjp_chrWidth(unsigned chr) {
    unsigned h = chr >> 8;
    if (h == 0 || h == 0x8E) {
        return 1;
    }
    return 2;
}

/** Check EUC-JP Encode?
 * @return 0=not  (1=ascii) 2,3,4=euc-jp  (2=use HANKAKU-KANA)
 */
int  fks_mbcCheckEucJp(char const* src, size_t len, int canEndBroken)
{
	unsigned char const* s = (unsigned char*)src;
	unsigned char const* e = s + len;
	char	ascFlg  = 1;
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
			continue;
		}
		ascFlg = 0;
		if (s >= e) { badFlg = !canEndBroken; break; }
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
			if (s >= e) { badFlg = !canEndBroken; break; }
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
	if (ascFlg)
		return 1;
	return 0;
}

static FKS_FORCE_INLINE unsigned eucjp_chrLen(unsigned chr) {
	return 1 + (chr > 0xff) + (chr > 0xffff);
}

MBC_IMPL(eucjp)


static Fks_MbcEnc const fks_mbcEnc_eucjp = {
	FKS_CP_EUCJP,
    euc_islead,
    eucjp_chkC,
    eucjp_getC,
    eucjp_peekC,
	eucjp_charNext,
    eucjp_setC,
    eucjp_len1,
    dbc_chrLen,
    eucjp_chrWidth,
	eucjp_adjustSize,
	eucjp_cmp,
	fks_mbcCheckEucJp,
};
fks_mbcenc_t const fks_mbc_eucjp = &fks_mbcEnc_eucjp;



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
	} else {	// jis2004
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
#else // jis2004
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
	} else {	// jis2004
		unsigned a, b, f;
		b = (uint8_t)c;
		a = (uint8_t)(c >> 8);
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
		return 0x10000|(a << 8)|b;
	}
}
#endif


size_t  fks_mbc_sjisFromEucjp(char dst[], size_t dstSz, char const* src, size_t srcSz)
{
	char const* s  = src;
	char const* se = src + srcSz;
	char* 		d  = dst;
	char* 		de = dst + dstSz;
	while (d < de && s < se) {
		if (*s < 0x80) {
			if (*s == 0)
				break;
			*d++ = *s++;
		} else {
			unsigned c;
			if (d + 1 >= de || s + 1 >= se)
				break;
			c = eucjp_getC(&s);
			c = fks_eucjp2jis(c);
			c = fks_jis2sjis(c);
			d = sjis_setC(d, de, c); // d = dstMbc->setC(d, c);
		}
	}
	if (d < de)
		*d = 0;
    return d - dst;
}


size_t  fks_mbc_eucjpFromSjis(char dst[], size_t dstSz, char const* src, size_t srcSz)
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
			c = fks_sjis2jis(c);
			c = fks_jis2eucjp(c);
			d = eucjp_setC(d, de, c);
		}
	}
	if (d < de)
		*d = 0;
	return d - dst;
}


static int fks_mbc_insertJpFindTbl(FksMbcCodeCnvPair *tbl, unsigned* pNum, unsigned key, unsigned val) {
	unsigned 	hi  = *pNum;
	unsigned 	low = 0;
	unsigned 	mid = 0;
	while (low < hi) {
		mid = (low + hi - 1) / 2;
		if (key < tbl[mid].key) {
			hi = mid;
		} else if (tbl[mid].key < key) {
			++mid;
			low = mid;
		} else {
			return (int)mid;	/* found */
		}
	}

	// new
	++*pNum;
	for (hi = *pNum; --hi > mid;) {
		tbl[hi] = tbl[hi-1];
	}
	tbl[mid].key = key;
	tbl[mid].val = val;
	return mid;
}

static int fks_mbc_jpFindTbl(FksMbcCodeCnvPair const *tbl, unsigned num, unsigned key) {
	unsigned	low = 0;
	unsigned    hi  = num;
	while (low < hi) {
		unsigned	mid = (low + hi - 1) / 2;
		if (key < tbl[mid].key) {
			hi = mid;
		} else if (tbl[mid].key < key) {
			low = mid + 1;
		} else {
			return (int)mid;
		}
	}
	return -1;
}

static void fks_mbc_init_jis2004_to_cp932sp()
{
	unsigned num = 0;
	unsigned i;
	fks_mbc_jis2004_to_cp932sp = (FksMbcCodeCnvPair*)calloc(1, sizeof(fks_mbc_jis2004_to_cp932sp[0]) * fks_mbc_cp932sp_to_Jis2004_tbl_size);
	FKS_PTR_ASSERT(fks_mbc_jis2004_to_cp932sp);
	for (i = 0; i < fks_mbc_cp932sp_to_Jis2004_tbl_size; ++i) {
		unsigned key = fks_mbc_cp932sp_to_Jis2004_tbl[i].val;
		unsigned val = fks_mbc_cp932sp_to_Jis2004_tbl[i].key;
		fks_mbc_insertJpFindTbl(fks_mbc_jis2004_to_cp932sp, &num, key, val);
	}
}

static int fks_jis2cp932(unsigned c)
{
	if (c < 0x7521) {
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
	} else {	// from jis2004
		unsigned	ku  = (c >> 16) * 94 + ((uint8_t)(c >> 8) - 0x21);
		unsigned	ten = ((uint8_t)c - 0x21);
		unsigned 	key = ku * 94 + ten;
		if (!fks_mbc_jis2004_to_cp932sp)
			fks_mbc_init_jis2004_to_cp932sp();
		int	idx = fks_mbc_jpFindTbl(fks_mbc_jis2004_to_cp932sp, fks_mbc_cp932sp_to_Jis2004_tbl_size, key);
		if (idx >= 0) {
			unsigned d = fks_mbc_jis2004_to_cp932sp[idx].val;
			ku  = d / 94;
			ten = d % 94;
			return ((ku+0x21)<<8) | (ten + 0x21);
		}
		return 0x2121/*　*/;
	}
}


size_t  fks_mbc_cp932FromEucjp(char dst[], size_t dstSz, char const* src, size_t srcSz)
{
	char const* s  = src;
	char const* se = src + srcSz;
	char* 		d  = dst;
	char* 		de = dst + dstSz;
	while (d < de && s < se) {
		if (*s < 0x80) {
			if (*s == 0)
				break;
			*d++ = *s++;
		} else {
			unsigned c;
			if (d + 1 >= de || s + 1 >= se)
				break;
			c = eucjp_getC(&s);
			c = fks_eucjp2jis(c);
			c = fks_jis2cp932(c);
			d = sjis_setC(d, de, c); // d = dstMbc->setC(d, c);
		}
	}
	if (d < de)
		*d = 0;
    return d - dst;
}


static int fks_cp932ToJis(unsigned c)
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

	if (c >= 0x7521) {	// sjis=ED40  	cp932 vs jis2004
		unsigned 	key = ((c >> 8) - 0x21) * 94 + ((c & 0xff) - 0x21);
		int			idx = fks_mbc_jpFindTbl(fks_mbc_cp932sp_to_Jis2004_tbl, fks_mbc_cp932sp_to_Jis2004_tbl_size, key);
		if (idx >= 0) {
			unsigned d = fks_mbc_cp932sp_to_Jis2004_tbl[idx].val;
			unsigned ku  = d / 94;
			unsigned ten = d % 94;
			if (ku > 94) {
				return 0x10000 | (((ku-94)+0x21)<<8) | (ten + 0x21);
			}
			return ((ku+0x21)<<8) | (ten + 0x21);
		}
		//return 0x2121/*　*/;
	}
    return c;
}


size_t  fks_mbc_eucjpFromCp932(char dst[], size_t dstSz, char const* src, size_t srcSz)
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
			c = fks_cp932ToJis(c);
			c = fks_jis2eucjp(c);
			d = eucjp_setC(d, de, c);
		}
	}
	if (d < de)
		*d = 0;
	return d - dst;
}


#ifdef __cplusplus
}
#endif
