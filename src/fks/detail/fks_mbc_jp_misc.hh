

#if 0

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
static unsigned euc_chkC(unsigned c)
{
    if (c > 0xff) {
        return euc_islead(c >> 8) && euc_istrail((unsigned char)c);
    }
    return 1;
}


/** 1字取り出し＆ポインタ更新.
 */
static unsigned euc_getC(const char** pStr) {
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
static unsigned euc_peekC(const char* s) {
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

#define euc_setC			fks_dbc_setC
#define euc_chrLen			dbc_chrLen

static Fks_MbcEnv const fks_mbcEnv_euc = {
	FKS_CP_EUCJP,
    euc_islead,                 // Cがマルチバイト文字の1バイト目か?
    euc_chkC,                   // 文字コードが正しい範囲にあるかチェック.
    euc_getC,                   // 1字取り出し＆ポインタ更新.
    euc_peekC,                  // 一字取り出し.
	euc_charNext,
    euc_setC,                   // 1字書き込み.
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
#endif


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
static unsigned big5_chkC(unsigned c)
{
    if (c > 0xff) {
        return big5_islead(c >> 8) && big5_istrail((unsigned char)c);
    }
    return 1;
}



/** 1字取り出し＆ポインタ更新.
 */
static unsigned big5_getC(const char** pStr) {
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
static unsigned big5_peekC(const char* s) {
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
    big5_chkC,                      // 文字コードが正しい範囲にあるかチェック.
    big5_getC,                      // 1字取り出し＆ポインタ更新.
    big5_peekC,                     // 一字取り出し
	big5_charNext,
    fks_dbc_setC,                   // 1字書き込み.
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
static unsigned gbk_chkC(unsigned c)
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
static unsigned gbk_getC(const char** ppStr) {
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
static unsigned gbk_peekC(const char* pStr) {
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
static char*    gbk_setC(char* d, unsigned c) {
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
    gbk_chkC,                   // 文字コードが正しい範囲にあるかチェック.
    gbk_getC,                   // 1字取り出し＆ポインタ更新.
    gbk_peekC,                  // 一字取り出し
	gbk_charNext,
    gbk_setC,                   // 1字書き込み.
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
static unsigned uhc_chkC(unsigned c)
{
    if (c <= 0xff) {
        return 1;
    } else {
        return UHC_ISLEAD(c >> 8) && uhc_istrail((unsigned char)c);
    }
}


/** 1字取り出し＆ポインタ更新.
 */
static unsigned uhc_getC(const char** pStr) {
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
static unsigned uhc_peekC(const char* s) {
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
    uhc_chkC,                       // 文字コードが正しい範囲にあるかチェック.
    uhc_getC,                       // 1字取り出し＆ポインタ更新.
    uhc_peekC,                      // 一字取り出し
	uhc_charNext,
    fks_dbc_setC,                   // 1字書き込み.
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
