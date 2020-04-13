#include <fks/fks_mbc.h>
#include <fks/fks_assert_ex.h>
#include "fks_mbc_sub.h"


#ifdef __cplusplus
extern "C" {
#endif


char*    fks_dbc_setC(char*  d, char* e, unsigned c);
int 	fks_mbsCheckSJIS(char const* src, size_t len, int flags);


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
	int rc = fks_mbsCheckSJIS(src, len, canEndBroken != 0);
	return (uint8_t)rc;
}


static FKS_FORCE_INLINE unsigned sjis_chrLen(unsigned chr) {
    return 1 + (chr > 0xff);
}

MBC_IMPL(sjis)

#define sjis_setC			fks_dbc_setC

Fks_MbcEnc const fks_mbsEnc_sjisX213 = {
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
fks_mbcenc_t const fks_mbc_sjisX213 = &fks_mbsEnc_sjisX213;



static int cp932_checkEncoding(char const* src, size_t len, int canEndBroken)
{
	int rc = fks_mbsCheckSJIS(src, len, (unsigned)(canEndBroken != 0) | 2);
	if (rc & 0x7000)
		rc = 2;
	rc = (uint8_t)rc;
 #ifdef FKS_WIN32
	if (rc > 1) {
	    int l = MultiByteToWideChar(932, MB_ERR_INVALID_CHARS, src, len, NULL, 0);
	    if (l == 0)
	    	return 0;
	}
 #endif
	return rc;
}

Fks_MbcEnc const fks_mbsEnc_cp932 = {
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
	cp932_checkEncoding,			// 文字列の、文字エンコードがあっているかチェック.
};
fks_mbcenc_t const fks_mbc_cp932 = &fks_mbsEnc_cp932;



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
int  fks_mbsCheckEucJp(char const* src, size_t len, int canEndBroken)
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


static Fks_MbcEnc const fks_mbsEnc_eucjp = {
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
	fks_mbsCheckEucJp,
};
fks_mbcenc_t const fks_mbc_eucjp = &fks_mbsEnc_eucjp;



// --------------------------------------------------------------------------

static void fks_init_cp932kutenIdx2uni(void);
static void fks_init_utf32ToJisX213Tbl(void);
static void fks_init_utf32ToJisX213Tbl(void);
static void fks_init_utf32ToCp932JisTbl(void);
static void fks_init_utf32ToCp932JisTbl(void);

static unsigned fks_utf32ToJis(unsigned utf32);
static unsigned fks_utf32ToEucjp(unsigned utf32);
static unsigned fks_utf32ToSjisX213(unsigned utf32);
static unsigned fks_utf32ToCp932(unsigned utf32);
static unsigned fks_utf32ToCp932Jis(unsigned utf32);

static unsigned fks_jisToUtf32(unsigned jis);
static unsigned fks_eucjpToUtf32(unsigned eucjp);
static unsigned fks_sjisX213ToUtf32(unsigned sjis);
static unsigned fks_cp932ToUtf32(unsigned sjis);
static unsigned fks_cp932JisToUtf32(unsigned jis);

static unsigned fks_jisToEucjp(unsigned c);
static unsigned fks_jisToSjisX213(unsigned c);
static unsigned fks_jisToCp932(unsigned jisX213);
static unsigned fks_jisX213ToCp932Jis(unsigned jis);

static unsigned fks_eucjpToJis(unsigned c);
static unsigned fks_sjisX213ToJis(unsigned c);
static unsigned fks_sjisToJis(unsigned c);

static unsigned fks_cp932ToJis(unsigned sjis);
static unsigned fks_cp932ToEucjp(unsigned sjis);
static unsigned fks_cp932ToSjisX213(unsigned sjis);
static unsigned fks_cp932JisToJisX213(unsigned jis);

static unsigned fks_sjisX213ToCp932(unsigned sjis);
static unsigned fks_eucjpToCp932(unsigned eucjp);

#define fks_cp932JisToCp932 	fks_jisToSjisX213


// ---------------------------------------------------------------------------

static unsigned short fks_ubyte_to_utf32_tbl[] = {
    0x0000,0x0001,0x0002,0x0003,0x0004,0x0005,0x0006,0x0007,0x0008,0x0009,0x000a,0x000b,0x000c,0x000d,0x000e,0x000f,
    0x0010,0x0011,0x0012,0x0013,0x0014,0x0015,0x0016,0x0017,0x0018,0x0019,0x001a,0x001b,0x001c,0x001d,0x001e,0x001f,
    0x0020,0x0021,0x0022,0x0023,0x0024,0x0025,0x0026,0x0027,0x0028,0x0029,0x002a,0x002b,0x002c,0x002d,0x002e,0x002f,
    0x0030,0x0031,0x0032,0x0033,0x0034,0x0035,0x0036,0x0037,0x0038,0x0039,0x003a,0x003b,0x003c,0x003d,0x003e,0x003f,
    0x0040,0x0041,0x0042,0x0043,0x0044,0x0045,0x0046,0x0047,0x0048,0x0049,0x004a,0x004b,0x004c,0x004d,0x004e,0x004f,
    0x0050,0x0051,0x0052,0x0053,0x0054,0x0055,0x0056,0x0057,0x0058,0x0059,0x005a,0x005b,0x005c,0x005d,0x005e,0x005f,
    0x0060,0x0061,0x0062,0x0063,0x0064,0x0065,0x0066,0x0067,0x0068,0x0069,0x006a,0x006b,0x006c,0x006d,0x006e,0x006f,
    0x0070,0x0071,0x0072,0x0073,0x0074,0x0075,0x0076,0x0077,0x0078,0x0079,0x007a,0x007b,0x007c,0x007d,0x007e,0x007f,
    0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,
    0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,
    0xf8f0,0xff61,0xff62,0xff63,0xff64,0xff65,0xff66,0xff67,0xff68,0xff69,0xff6a,0xff6b,0xff6c,0xff6d,0xff6e,0xff6f,
    0xff70,0xff71,0xff72,0xff73,0xff74,0xff75,0xff76,0xff77,0xff78,0xff79,0xff7a,0xff7b,0xff7c,0xff7d,0xff7e,0xff7f,
    0xff80,0xff81,0xff82,0xff83,0xff84,0xff85,0xff86,0xff87,0xff88,0xff89,0xff8a,0xff8b,0xff8c,0xff8d,0xff8e,0xff8f,
    0xff90,0xff91,0xff92,0xff93,0xff94,0xff95,0xff96,0xff97,0xff98,0xff99,0xff9a,0xff9b,0xff9c,0xff9d,0xff9e,0xff9f,
    0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,
    0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,
};

#include "fks_mbc_kutenIdxToUtf.hh"

enum { fks_mbc_kutenIdxToUtf_size = sizeof(fks_mbc_kutenIdxToUtf) / sizeof(fks_mbc_kutenIdxToUtf[0]) };
//extern unsigned short const  fks_mbc_kutenIdxToUtf[];
//extern unsigned const		   fks_mbc_kutenIdxToUtfEx[];

static unsigned short const* fks_cp932kutenIdxToUtf32Tbl;

static void fks_init_cp932kutenIdx2uni(void)
{
	static unsigned short const s_repl_cp932[9][2] = {
		{ (1-1)*94+(17-1), 0xFFE3 },		// 203E →	￣	OVERLINE
		{ (1-1)*94+(29-1), 0x2015 },		// 2014 →	―	EM DASH
		{ (1-1)*94+(33-1), 0xFF5E },		// 301C →	～	WAVE DASH
		{ (1-1)*94+(34-1), 0x2225 },		// 2016 →	∥	DOUBLE VERTICAL LINE
		{ (1-1)*94+(61-1), 0xFF0D },		// 2212 →	2	MINUS SIGN
		{ (1-1)*94+(79-1), 0xFFE5 },		// 00A5 →	￥	YEN SIGN
		{ (1-1)*94+(81-1), 0xFFE0 },		// 00A2 →	￠	CENT SIGN
		{ (1-1)*94+(82-1), 0xFFE1 },		// 00A3 →	￡	POUND SIGN
		{ (2-1)*94+(44-1), 0xFFE2 },		// 00AC →	￢	NOT SIGN
	};
	int i, c;
	unsigned short const* s;
	unsigned short* dst = (unsigned short*)fks_calloc(1, 120 * 94 * sizeof(unsigned short));
	if (!dst) {
		FKS_ASSERT(dst != NULL);
		fks_cp932kutenIdxToUtf32Tbl = s_repl_cp932[0];
		return;
	}
	memcpy(dst, fks_mbc_kutenIdxToUtf, 120*94*sizeof(unsigned short));
	s = fks_mbc_kutenIdxToUtf + 2 * 94 * 94;
	// 89-92:NEC選定IBM拡張文字.
	for (i = (89-1)*94; i < (93-1) * 94; ++i) {
		c = s[i - (89-1)*94];
		if (c)
			dst[i] = c;
	}
	// ユーザー外字.
	for (i = (95-1) * 94; i < (115-1) * 94; ++i) {
		c = 0xE000 + i - (95-1) * 94;
		dst[i] = c;
	}
	// 115-119:IBM拡張文字.
	for (i = (115-1) * 94; i < (119-1) * 94 + 12; ++i) {
		c = s[i - (115-1)*94 + (93-89)*94];
		if (c)
			dst[i] = c;
	}

	for (i = 0; i < 9; ++i)
		dst[s_repl_cp932[i][0]] = s_repl_cp932[i][1];
	fks_cp932kutenIdxToUtf32Tbl = dst;
}


/*
unsigned fks_ms932ToUtf32winApi(int sjis) {
	char sjisStr[4] = { 0 };
	wchar_t wcs[4] = { 0 };
	int l;
	unsigned c;
	sjisStr[0] = sjis >> 8;
	sjisStr[1] = sjis;
    l = MultiByteToWideChar(932, MB_ERR_INVALID_CHARS, sjisStr, 2, wcs, 3);
	c = (l) ? wcs[0] : 0;
	if (0xD800 <= c && c <= 0xDBFF) {
		unsigned d = wcs[1];
		if (0xDC00 <= d && d <= 0xDFFF) {
			c = ((c & 0x3ff) << 10) | (d & 0x3ff);
			c += 0x10000;
		}
	}
	return c;
}
*/

static unsigned fks_cp932ToEucjp(unsigned sjis)
{
	unsigned jis = fks_cp932ToJis(sjis);
	return 	fks_jisToEucjp(jis);
}

static unsigned fks_cp932ToSjisX213(unsigned sjis)
{
	unsigned jis = fks_cp932ToJis(sjis);
	return fks_jisToSjisX213(jis);
}

static unsigned fks_cp932ToJis(unsigned sjis)
{
	unsigned jis = fks_sjisToJis(sjis);
	return fks_cp932JisToJisX213(jis);
}

static unsigned fks_cp932JisToJisX213(unsigned jis)
{
	unsigned utf32 = fks_cp932JisToUtf32(jis);
	return fks_utf32ToJis(utf32);
}

static unsigned fks_cp932ToUtf32(unsigned sjis)
{
	/*
	if (fks_mbc_useWinApi) {
		return fks_ms932ToUtf32winApi(sjis);
	} else
	*/
	{
		unsigned jis = fks_sjisToJis(sjis);
		return fks_cp932JisToUtf32(jis);
	}
}

static unsigned fks_cp932JisToUtf32(unsigned jis)
{
	if (jis <= 0xff) {
		return fks_ubyte_to_utf32_tbl[jis];
	} else {
		unsigned ku  = ((jis >> 8) - 0x21);
		unsigned ten = (jis & 0xff) - 0x21;
		unsigned idx = ku * 94 + ten;
		unsigned rc;
		if (!fks_cp932kutenIdxToUtf32Tbl)
			fks_init_cp932kutenIdx2uni();
		rc = fks_cp932kutenIdxToUtf32Tbl[idx];
		if (0xDC00 <= rc && rc <= 0xDFFF)
			rc = fks_mbc_kutenIdxToUtfEx[rc - 0xDC00];
		return rc;
	}
}


static unsigned fks_sjisX213ToUtf32(unsigned sjis)
{
	unsigned jis = fks_sjisX213ToJis(sjis);
	return fks_jisToUtf32(jis);
}

static unsigned fks_eucjpToUtf32(unsigned eucjp)
{
	unsigned jis = fks_eucjpToJis(eucjp);
	return fks_jisToUtf32(jis);
}

/** JIS X 213:2004(with JIS X 212) を UTF32 に変換
 * return unicode (bit31が立っていた場合は 2キャラ合成)
 */
static unsigned fks_jisToUtf32(unsigned jis)
{
	if (jis <= 0xff) {
		return fks_ubyte_to_utf32_tbl[jis];
	} else {
		unsigned men = jis >> 16;
		unsigned ku  = ((jis >> 8) & 0xff) - 0x21;
		unsigned ten = (jis & 0xff) - 0x21;
		unsigned idx = men * 94*94 + ku * 94 + ten;
		unsigned rc  = fks_mbc_kutenIdxToUtf[idx];
		if (0xDC00 <= rc && rc <= 0xDFFF)
			rc = fks_mbc_kutenIdxToUtfEx[rc - 0xDC00];
	    return rc;
	}
}

#if 0
static unsigned fks_kitenIdxToUtf32(unsigned kitenIdx)
{
	unsigned rc  = fks_mbc_kutenIdxToUtf[kitenIdx];
	if (0xDC00 <= rc && rc <= 0xDFFF)
		rc = fks_mbc_kutenIdxToUtfEx[rc - 0xDC00];
	return rc;
}
#endif

static unsigned fks_sjisX213ToCp932(unsigned sjis)
{
	unsigned jis = fks_sjisX213ToJis(sjis);
	return fks_jisToCp932(jis);
}

static unsigned fks_eucjpToCp932(unsigned eucjp)
{
	unsigned jis = fks_eucjpToJis(eucjp);
	return fks_jisToCp932(jis);
}

static unsigned fks_jisToCp932(unsigned jisX213)
{
	unsigned cp932jis = fks_jisX213ToCp932Jis(jisX213);
	return fks_cp932JisToCp932(cp932jis);
}

static unsigned fks_jisX213ToCp932Jis(unsigned jis)
{
	unsigned utf32 = fks_jisToUtf32(jis);
	return fks_utf32ToCp932Jis(utf32);
}

typedef struct FksMbcUtf32ConvPair {
	uint32_t	utf32;
	uint32_t	jis;
} FksMbcUtf32ConvPair;

static FksMbcUtf32ConvPair* s_fks_utf32_to_jisX213_tbl;
static FksMbcUtf32ConvPair* s_fks_utf32_to_cp932jis_tbl;
enum { fks_utf32_to_jisX213_tbl_size  = 2 * 94 * 94 + 0x40 };
enum { fks_utf32_to_cp932jis_tbl_size = 120 * 94 + 0x40 };

static int  fks_insertJisToUtf32Tbl(FksMbcUtf32ConvPair *tbl, unsigned* pNum, unsigned key, unsigned val);

static unsigned fks_utf32ToSjisX213(unsigned utf32)
{
	unsigned jis = fks_utf32ToJis( utf32 );
	return fks_jisToSjisX213( jis );
}

static unsigned fks_utf32ToEucjp(unsigned utf32)
{
	unsigned jis = fks_utf32ToJis( utf32 );
	return fks_jisToEucjp( jis );
}

static unsigned fks_utf32ToJis(unsigned utf32)
{
	if (utf32 < 0x80) {
		return utf32;
	} else {
		unsigned	low = 0;
		unsigned    hi  = fks_utf32_to_jisX213_tbl_size;
		FksMbcUtf32ConvPair const *tbl;
		if (!s_fks_utf32_to_jisX213_tbl)
			fks_init_utf32ToJisX213Tbl();
		tbl = s_fks_utf32_to_jisX213_tbl;
		while (low < hi) {
			unsigned	mid = (low + hi - 1) / 2;
			if (utf32 < tbl[mid].utf32) {
				hi = mid;
			} else if (tbl[mid].utf32 < utf32) {
				low = mid + 1;
			} else {
				return tbl[mid].jis;
			}
		}
		return 0;
	}
}

static void fks_init_utf32ToJisX213Tbl(void)
{
	unsigned num = 0;
	unsigned i, men, kuI, ten;
	uint8_t	 kuTbl[2*94];

	{
		uint8_t* p = kuTbl;
		for (i = 0; i < 94; ++i)
			*p++ = i;
		// jis x 213
		*p++ = 94 +  1 - 1;
		*p++ = 94 +  3 - 1;
		*p++ = 94 +  4 - 1;
		*p++ = 94 +  5 - 1;
		*p++ = 94 +  8 - 1;
		*p++ = 94 + 12 - 1;
		*p++ = 94 + 13 - 1;
		*p++ = 94 + 14 - 1;
		*p++ = 94 + 15 - 1;
		for (i = 78-1; i <= 94 - 1; ++i)
			*p++ = 94 + i;
		// jis x 212
		*p++ = 94 +  2 - 1;
		*p++ = 94 +  6 - 1;
		*p++ = 94 +  7 - 1;
		*p++ = 94 +  9 - 1;
		*p++ = 94 + 10 - 1;
		*p++ = 94 + 11 - 1;
		for (i = 16-1; i <= 77 - 1; ++i)
			*p++ = 94 + i;
	}

	s_fks_utf32_to_jisX213_tbl = (FksMbcUtf32ConvPair*)fks_calloc(1, fks_utf32_to_jisX213_tbl_size * sizeof(FksMbcUtf32ConvPair));
	FKS_PTR_ASSERT(s_fks_utf32_to_jisX213_tbl);
	for (men = 0; men < 2; ++men) {
		for (kuI = 0; kuI < 94; ++kuI) {
			unsigned ku = kuTbl[kuI];
			for (ten = 0; ten < 94; ++ten) {
				//uint32_t idx   = men * 94 * 94 + ku * 94 + ten;
				uint32_t jis   = ((men) << 16) | ((ku + 0x21) << 8) | (ten + 0x21);
				//uint32_t utf32 = fks_kitenIdxToUtf32(idx);
				uint32_t utf32 = fks_jisToUtf32(jis);
				fks_insertJisToUtf32Tbl(s_fks_utf32_to_jisX213_tbl, &num, utf32, jis);
			}
		}
	}

	// hankaku kana.
	for (i = 0xa0; i <= 0xDF; ++i) {
		uint32_t utf32 = fks_ubyte_to_utf32_tbl[i];
		fks_insertJisToUtf32Tbl(s_fks_utf32_to_jisX213_tbl, &num, utf32, i);
	}
}

static unsigned fks_utf32ToCp932(unsigned utf32)
{
	unsigned jis = fks_utf32ToCp932Jis(utf32);
	return fks_cp932JisToCp932(jis);
}

static unsigned fks_utf32ToCp932Jis(unsigned utf32)
{
	unsigned	low = 0;
	unsigned    hi  = fks_utf32_to_cp932jis_tbl_size;
	FksMbcUtf32ConvPair const *tbl;
	if (!s_fks_utf32_to_cp932jis_tbl)
		 fks_init_utf32ToCp932JisTbl();
	tbl = s_fks_utf32_to_cp932jis_tbl;
	while (low < hi) {
		unsigned	mid = (low + hi - 1) / 2;
		if (utf32 < tbl[mid].utf32) {
			hi = mid;
		} else if (tbl[mid].utf32 < utf32) {
			low = mid + 1;
		} else {
			return tbl[mid].jis;
		}
	}
	return 0;
}

static void fks_init_utf32ToCp932JisTbl(void)
{
	unsigned num = 0;
	unsigned i, ku, ten;

	s_fks_utf32_to_cp932jis_tbl = (FksMbcUtf32ConvPair*)fks_calloc(1, fks_utf32_to_cp932jis_tbl_size * sizeof(FksMbcUtf32ConvPair));
	FKS_PTR_ASSERT(s_fks_utf32_to_cp932jis_tbl);
	for (ku = 0; ku < 120; ++ku) {
		for (ten = 0; ten < 94; ++ten) {
			uint32_t jis   = ((ku + 0x21) << 8) | (ten + 0x21);
			uint32_t utf32 = fks_cp932JisToUtf32(jis);
			fks_insertJisToUtf32Tbl(s_fks_utf32_to_cp932jis_tbl, &num, utf32, jis);
		}
	}

	// hankaku kana.
	for (i = 0xa0; i <= 0xDF; ++i) {
		uint32_t utf32 = fks_ubyte_to_utf32_tbl[i];
		fks_insertJisToUtf32Tbl(s_fks_utf32_to_cp932jis_tbl, &num, utf32, i);
	}
}

static int fks_insertJisToUtf32Tbl(FksMbcUtf32ConvPair *tbl, unsigned* pNum, unsigned key, unsigned val)
{
	unsigned 	hi  = *pNum;
	unsigned 	low = 0;
	unsigned 	mid = 0;
	while (low < hi) {
		mid = (low + hi - 1) / 2;
		if (key < tbl[mid].utf32) {
			hi = mid;
		} else if (tbl[mid].utf32 < key) {
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
	tbl[mid].utf32 = key;
	tbl[mid].jis   = val;
	return mid;
}


// --------------------------------------------------------------------------
// JIS

// jis to eucJp
//#define FKS_JIS2EUCJP(jis)	((jis) | 0x8080)
//#define FKS_EUCJP2JIS(euc)	((euc) & ~0x8080)

static unsigned fks_sjisX213ToEucjp(unsigned sjis)
{
	unsigned jis = fks_sjisX213ToJis(sjis);
	return fks_jisToEucjp(jis);
}

static unsigned fks_jisToEucjp(unsigned c)
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

static unsigned fks_eucjpToSjisX213(unsigned eucjp)
{
	unsigned jis = fks_eucjpToJis(eucjp);
	return fks_jisToSjisX213(jis);
}

static unsigned fks_eucjpToJis(unsigned c)
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
static unsigned fks_jisToSjisX213(unsigned c)
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

#if 1
static unsigned fks_sjisToJis(unsigned c)
{
	if (c <= 0xff)
		return c;
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
#endif

static unsigned fks_sjisX213ToJis(unsigned c)
{
	if (c < 0xf000) {
		if (c <= 0xff)
			return c;
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


// --------------------------------------------------------------------------
#if 0
typedef struct FksMbcCodeCnvPair {
	uint16_t	key, val;
} FksMbcCodeCnvPair;

static int fks_mbc_jpFindTbl(FksMbcCodeCnvPair const *tbl, unsigned num, unsigned key);
static int fks_mbc_insertJpFindTbl(FksMbcCodeCnvPair *tbl, unsigned* pNum, unsigned key, unsigned val);
static void fks_mbc_init_jis2004_to_cp932sp(void);

static FksMbcCodeCnvPair fks_mbc_cp932sp_to_Jis2004_tbl[] = {
#include "fks_mbc_cp932xJis2004tbl.hh"
};
enum { fks_mbc_cp932sp_to_Jis2004_tbl_size = sizeof(fks_mbc_cp932sp_to_Jis2004_tbl) / sizeof(fks_mbc_cp932sp_to_Jis2004_tbl[0]) };
static FksMbcCodeCnvPair* fks_mbc_jis2004_to_cp932sp = NULL;


static unsigned fks_jisToCp932(unsigned c)
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


static unsigned fks_cp932ToJis(unsigned c)
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

static void fks_mbc_init_jis2004_to_cp932sp(void)
{
	unsigned num = 0;
	unsigned i;
	fks_mbc_jis2004_to_cp932sp = (FksMbcCodeCnvPair*)fks_calloc(1, sizeof(fks_mbc_jis2004_to_cp932sp[0]) * fks_mbc_cp932sp_to_Jis2004_tbl_size);
	FKS_PTR_ASSERT(fks_mbc_jis2004_to_cp932sp);
	for (i = 0; i < fks_mbc_cp932sp_to_Jis2004_tbl_size; ++i) {
		unsigned key = fks_mbc_cp932sp_to_Jis2004_tbl[i].val;
		unsigned val = fks_mbc_cp932sp_to_Jis2004_tbl[i].key;
		fks_mbc_insertJpFindTbl(fks_mbc_jis2004_to_cp932sp, &num, key, val);
	}
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
#endif


// --------------------------------------------------------------------------
#ifdef FKS_BIG_ENDIAN
#define fks_mbc_utf32		fks_mbc_utf32be
#else
#define fks_mbc_utf32		fks_mbc_utf32le
#endif

int fks_mbc_encToJisTypeNo(fks_mbcenc_t enc)
{
	if (enc == fks_mbc_utf8) 	return 0;
	if (enc == fks_mbc_utf16le)	return 0;
	if (enc == fks_mbc_utf16be)	return 0;
	if (enc == fks_mbc_utf32le)	return 0;
	if (enc == fks_mbc_utf32be)	return 0;
	if (enc == fks_mbc_eucjp) 	return 1;
	if (enc == fks_mbc_sjisX213) return 2;
	if (enc == fks_mbc_cp932)	return 3;
	//if (enc == fks_mbc_jis)  	return 4;
	return -1;
}

typedef unsigned (*charConvFnc_t)(unsigned c);

size_t  fks_mbsConvJisType(fks_mbcenc_t dstEnc, char dst[], size_t dstSz, fks_mbcenc_t srcEnc, char const* src, size_t srcSz)
{
	static charConvFnc_t const	s_conv[4][4] = {
		// utf32				eucjp				sjisX213			cp932				jis(X213)
		{ NULL,					fks_utf32ToEucjp,	fks_utf32ToSjisX213,fks_utf32ToCp932,	/*fks_utf32ToJis,*/		},	// utf32
		{ fks_eucjpToUtf32,		NULL,				fks_eucjpToSjisX213,fks_eucjpToCp932,	/*fks_eucjpToJis,*/		},	// eucjp
		{ fks_sjisX213ToUtf32,	fks_sjisX213ToEucjp,NULL,				fks_sjisX213ToCp932,/*fks_sjisX213ToJis,*/	},	// sjisX213
		{ fks_cp932ToUtf32,		fks_cp932ToEucjp,	fks_cp932ToSjisX213,NULL,				/*fks_cp932ToJis,*/		},	// cp932
		//{ fks_jisToUtf32,		fks_jisToEucjp,		fks_jisToSjisX213,	fks_jisToCp932,		/*NULL,*/				},	// jis(X213)
	};
	int 			srcNo;
	int				dstNo;
	charConvFnc_t	fnc;
	char const*		s;
	char const*		se;
	char*			d;
	char*			de;

	if (dstEnc == srcEnc)
		return fks_mbsLCpy(dstEnc, dst, dstSz, src, srcSz);

	dstNo = fks_mbc_encToJisTypeNo(dstEnc);
	srcNo = fks_mbc_encToJisTypeNo(srcEnc);

	FKS_ARG_PTR_ASSERT(1, dstEnc);
	FKS_ARG_PTR_ASSERT(2, dst);
	FKS_ARG_PTR_ASSERT(4, srcEnc);
	FKS_ARG_PTR_ASSERT(5, src);
	FKS_ASSERT(dstNo >= 0);
	FKS_ASSERT(srcNo >= 0);
	FKS_ASSERT(dstSz > 1);
	if (dstNo < 0 || srcNo < 0) {
		return 0; //(size_t)(ptrdiff_t)-1;
	}

	fnc   = s_conv[srcNo][dstNo];
	s  = src;
	se = src + srcSz;
	d  = dst;
	de = dst + dstSz;
	while (d < de && s < se) {
		unsigned c;
		//if (d + 1 >= de || s + 1 >= se)
		//	break;
		c = srcEnc->getC(&s);
		if (c >= 0x80 && fnc)
			c = fnc(c);
		d = dstEnc->setC(d, de, c);
	}
	if (d < de)
		*d = 0;
    return d - dst;
}


// --------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif
