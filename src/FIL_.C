/*
	ファイル関係
 */

#include <string.h>
#include "fil.h"

#define ISLOWER(c)	(((unsigned)(c)-'a') < 26U)

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
static int	FIL_zenFlg = 1;			/* 1:MS全角に対応 0:未対応 */
static int	FIL_wcFlg  = 0x08;


void	FIL_SetZenMode(int ff)
{
	FIL_zenFlg = ff;
}

void	FIL_GetZenMode(void)
{
	return FIL_zenFlg;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void	FIL_SetWcMode(int ff)
{
	FIL_wcFlg = ff;
}

void	FIL_GetWcMode(void)
{
	return FIL_wcFlg;
}


static int wcCmpFlgs;

static int WcGet_(char **fstr)
{
	int c;
	unsigned char *p;

	p = (unsigned char *)*fstr;
	c = *p++;
	if ((wcCmpFlgs & 0x10) == 0 && iskanji(c) && *p) {
		c = (c << 8) + *p;
		p++;
	}
	*fstr = p;
	return c;
}

static int WcCmp_(char *key, char *fstr)
{
	enum { _N_ = -2, _E_ = -1, _Y_ = 0};
	char		*sptr;
	unsigned	ax,cx;

	sptr = fstr;
	for (;;) {
		ax = *(unsigned char*)key;
		key++;
		if ((wcCmpFlgs & 0x10) == 0 && IsKanji(ax)) {
			if (*key == '\0')
				return _E_;
			ax = (ax << 8) + *(unsigned char *)key;
			key++;
			cx = WcGet_(&fstr);
			if (ax != cx)
				return _N_;
			continue;

		} else if (ax == '*' && (wcCmpFlgs & 0x02) == 0) {
			do {
				ax = WcCmp_(key, fstr);
				if (ax == _Y_)
					return _Y_;
				if (ax == _E_)
					return _E_;
				cx = WcGet_(&fstr);
			} while (cx);
			return _N_;

		} else if (ax == '?' && (wcCmpFlgs & 0x01) == 0) {
			cx = WcGet_(&fstr);
			if (cx == '\0' || cx == '.')
				return _N_;

		} else if (ax == '^' && (wcCmpFlgs & 0x08) == 0) {
			ax = WcGet_(&key);
			if (ax == 0)
				return _E_;
			cx = WcGet_(&fstr);
			if (ax == cx)
				return _N_;

		} else if (ax == '[' && (wcCmpFlgs & 0x04) == 0) {
			unsigned bx;
			int  dh;
			cx = WcGet_(&fstr);
			if (cx == 0)
				return _N_;
			ax = WcGet_(&key);
			dh = 0;
			if (ax == '^') {
				dh = 1;
				ax = WcGet_(&key);
			}
			bx = 0xFFFFU;
			goto J0;

			for (;;) {
				if (ax == '-') {
					ax = WcGet_(&key);
					if (ax == '\0')
						return _E_;
					if (ax == ']') {
						if (bx == cx)
							goto J1;
						if (dh)
							break;
						return _N_;
					}
					if (cx >= bx && cx <= ax)
						 goto J1;
				} else {
			 J0:
					if (ax == cx) {
			 J1:
						if (dh) return _N_;
						do {
							ax = WcGet_(&key);
							if (ax == 0)
								return _E_;
						} while (ax != ']');
						break;
					}
				fi
				bx = ax;
				ax = WcGet_(&key);
				if (ax == '\0')
					return _E_;
				if (ax == ']') {
					if (dh)
						break;
					return _N_;
				}
			}

		} else {
			cx = WcGet_(&fstr);
			if (ax != cx) {
				if (ax || fstr == sptr)
					return _N_;
				return (int)(fstr - sptr);
			}
			if (cx == 0)
				return _Y_;
		}
	}
}

int FIL_WcCmp(char *key, char *fstr)
{
	wcCmpFlgs = FIL_wcFlg & 0x0f;
	if (FIL_zenFlg == 0)
		wcCmpFlgs |= 0x10;
	return WcCmp_(key, fstr);
}



/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

char *FIL_BaseName(char *adr)
{
	char *p;

	p = adr;
	while (*p != '\0') {
		if (*p == ':' || *p == '/' || *p == '\\')
			adr = p + 1;
		if (FIL_zenFlg && iskanji((*(unsigned char *)p)) && *(p+1) )
			p++;
		p++;
	}
	return adr;
}

char *FIL_ChgExt(char filename[], char *ext)
{
	char *p;

	p = FIL_BaseName(filename);
	p = strrchr( p, '.');
	if (p == NULL) {
		if (ext) {
			strcat(filename,".");
			strcat( filename, ext);
		}
	} else {
		if (ext)
			strcpy(p+1, ext);
		else
			*p = 0;
	}
	return filename;
}

char *FIL_AddExt(char filename[], char *ext)
{
	if (strrchr(FIL_BaseName(filename), '.') == NULL) {
		if (ext) {
			strcat(filename,".");
			strcat(filename, ext);
		}
	}
	return filename;
}



/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
		a = _dos_getdrive(&drv);
			p = buf;
			buf[0] = '/';
			
			p = ss.cx
			b[p] = '/'; ++p
			ah = 0x47
			intr 0x21
			if (cf == 0)
				ax = 0
			fi
		}


void	FIL_FullPath(char *src, char *dst)
{
	char buf[1030];	/* はたして1030バイトでいいのか... */
	char *s,*d;

	s = src;
	d = dst;

	/*
	 * 絶対パスを得るためまずカレント・ドライブ＆ディレクトリ名を得る
	 */
	if (s[0] && s[1] == ':') {	//ドライブ名があるときはそれを用いる
		a = s[0];
		if (ISLOWER(a))
			a = a - 0x20;
		s += 2;
	} else {					//ドライブ名がなければDOSにカレントを聞く
		a = _getdrive() + 'A';	/* a = _dos_getdrive(&drv);*/
	}
	dv   = a - ('A'-1);			//dv=ドライブ番号
	*d++ = a;
	*d++ = ':';

	//ルートが指定されているか
	al = *s;
	if (al == '\\' || al == '/') {  //ルートなら名前をそのままbufにコピー
		strcpy(buf, s);
	} else {						//ﾙｰﾄでないならｶﾚﾝﾄﾃﾞｨﾚｸﾄﾘをDOSに聞いて残りをｺﾋﾟｰ
		buf[0] = '/';
		FIL_GetCurDir(dv, buf+1);
		dd = strend(buf);
		if (dd[-1] != '\\' && dd[-1] != '/') {
			*dd++ = '/';
		}
		strcpy(dd, s);
	}

	/*
	 * ｾﾊﾟﾚｰﾀ'\\' '/' を'/'にし、".." "."で不要ﾃﾞｨﾚｸﾄﾘ名文字列を削除
	 * （なぜ'/'にするかといえばMS全角の２バイト目にならないから）
	 */
	s = &buf;
	dx = d;		//dx:ルート位置(戻るときに戻り過ぎないように)
	bx = d;		//bx:前回のセパレータ'\\''/'の位置を保存
	for (;;) {
		al = *s++;
		if (al == 0)
			break;
		if (al == '\\' || al == '/') {
			bx = d;
			*d++ = '/';
			continue;
		}
		if (al == '.') {
			a = *s++;
			if (a == '.' && (*s == '\\'||*s == '/'||*s == '\0')) {
				// '..'なら一つﾃﾞｨﾚｸﾄﾘ名を削る
				if (*s == 0)
					--s
				d = bx;
				do {
					if (d <= dx) {
						d = bx
						goto PP_1;
					}
					--d;
					a = *d++;
				} while (a != '/' /*&& a != '\\'*/);
				bx = d
			  PP_1:
				++d
				continue;

			} else if (al == '\\' || al == '/' || al == '\0') {
				// '.'だけなら'.'を省く
				--s
				if (al == 0)
					--s
				fi
				d = bx
				++d
				continue;
			fi
			//'.'で始まるファイル名だ^^;
			s -= 2
		fi
		--s
		loop
			rep.load al,s
			--s
			break (al == '\0' || al == '\\' || al == '/')
			FIL_GetZenModeMACRO	//全角有効モードなら全角チェック
			if (!=)
				if (b[s] && IsKanji(al))
					rep.cpy d,s
				fi
			fi
			rep.cpy d,s
		endloop
	endloop
	rep.set d,0

	/*
	 * '/' を '\\' に置き換え、英小文字を大文字化
	 */
	d = dst;
	for (;;) {
		a = *d;
		if (a == 0)
			break;
		if (a == '/') {
			*d++ = '\\';
			continue;
		} else if (a >= 'a' && a <= 'z') {
			a -= 'a' - 'A';
			*d++ = a;
			continue;
		} else {
			++d;
			FIL_GetZenModeMACRO	//全角有効モードなら全角チェック
			go (==) L2
			if (*d && ISKANJI(a)) {
				++d;
			}
		}
	}
	return dst;
}


