#include <fks_mbc.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Check UTF8 Encode?
 * @return 0=not  1=ascii(<=7f) 2,3,4=ut8
 */
int  fks_mbc_checkUTF8(char const* src, size_t len)
{
	unsigned char const* s = (unsigned char*)src;
	unsigned char const* e = s + len;
	char	hasAsc  = 0;
	char	allAsc  = 1;
	char	badFlg  = 0;
	char	zenFlg  = 0;
	char	b5b6    = 0;
	int		c;
	if (len == 0 )
		return 0;
	while (s < e) {
		c = *s++;
		if (c == '\0') {
			badFlg = 1;
			break;
		}
		if  (c <= 0x7f) {
			hasAsc = 1;
		} else {
			allAsc = 0;
			if (c < 0xC0) {
				badFlg = 1;
				break;
			}
			int c2 = *s++;
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
			} else {
				int c3 = *s++;
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
					zenFlg = 1;
				} else {
					int c4 = *s++;
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
					} else {
						int c5 = *s++;
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
						} else {
							int c6 = *s++;
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
					}
				}
			}
		}
	}
	if (badFlg)
		return 0;
	if (zenFlg)
		return (b5b6) ? 3 : 4;
	if (allAsc)
		return 1;
	return 0;
}


/** Check Shift-JIS Encode?
 * @return 0=not  (1:ascii)  2,3,4=sjis (2=use HANKAKU-KANA)
 */
int  fks_mbc_checkSJIS(char const* src, size_t len)
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
		} else if (c >= 0x81 && c <= 0xfe) {
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
		} else if (c >= 0xA0 && c <= 0xDF) {
			kataFlg = 1;
		} else {
			badFlg = 1;
			break;
		}
	}
	if (badFlg)
		return 0;
	if (zenFlg)
		return (lowAsc) ? 4 : 2;
	if (kataFlg)
		return 3;
	//if (hasAsc)
	//	return 1;
	return 0;
}


/** Check EUC-JP Encode?
 * @return 0=not  (1=ascii) 2,3,4=euc-jp  (2=use HANKAKU-KANA)
 */
int  fks_mbc_checkEucJp(char const* src, size_t len)
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
		if (c == '\0') {
			badFlg = 1;
			break;
		}
		if  (c <= 0x7f) {
			//hasAsc = 1;

		} else if (c >= 0xA0 && c <= 0xfe) {
			c = *s;
			if (c) {
				++s;
				if (c >= 0xA0 && c <= 0xfe) {
					zenFlg = 1;
				} else {
					badFlg = 1;
					break;
				}
			} else {
				badFlg = 1;
				break;
			}
		} else if (c == 0x8e) {	// hankaku-kana
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

		} else if (c == 0x8f) {	//TODO:
			c8f = 1;
		} else {
			badFlg = 1;
			break;
		}
	}
	if (badFlg)
		return 0;
	if (kataFlg)
		return 2;
	if (zenFlg)
		return (!c8f) ? 4 : 2;
	//if (hasAsc)
	//	return 1;
	return 0;
}


/** Japanese encode?
 *  @return  0=bad  1=ascii(<0x80) 2=SJIS 3=EUC-JP 4=UTF8
 */
int fks_mbc_tinyCheckJpEncode(char const* src, size_t len, int dfltCode)
{
	int  utf8 = fks_mbc_checkUTF8(src, len);
	int  sjis = fks_mbc_checkSJIS(src, len);
	int  euc  = fks_mbc_checkEucJp(src, len);

	if (utf8 == 1)
		return 1;

	if (utf8 > 0) {
		if (utf8 > euc && utf8 > sjis)
			return 3;
		if ((dfltCode == 3 || dfltCode == 0) && utf8 >= euc && utf8 >= sjis)
			return 3;
	}
	if (sjis > 0) {
		if (sjis > utf8 && sjis > euc)
			return 1;
		if ((dfltCode == 1 || dfltCode == 0) && sjis >= utf8 && sjis >= euc)
			return 1;
	}
	if (euc > 0) {
		if (euc > utf8 && euc > sjis)
			return 2;
		if ((dfltCode == 2 || dfltCode == 0) && euc >= utf8 && euc >= sjis)
			return 2;
	}

	if (utf8 > 0) {
		if (utf8 >= euc && utf8 >= sjis)
			return 3;
	}
	if (sjis > 0) {
		if (sjis >= utf8 && sjis >= euc)
			return 1;
	}
	if (euc > 0) {
		if (euc >= utf8 && euc >= sjis)
			return 2;
	}

	return dfltCode;
}

#ifdef __cplusplus
}
#endif
