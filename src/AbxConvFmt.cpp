/**
 *  @file   AbxConvFmt.cpp
 *  @brief  convert formated path string
 *  @author Masashi KITAMURA (tenka@6809.net)
 *  @license Boost Software License Version 1.0
 */

#include "AbxConvFmt.hpp"

#include <fks/fks_path.h>
#include <fks/fks_io.h>
#include <fks/fks_time.h>

#include "subr.hpp"

ConvFmt::ConvFmt()
	: upLwrFlg_(false)
	, autoWqFlg_(false)
	, num_(0)
	, numEnd_(0)
	, fmtBuf_(NULL)
	, lineBuf_(NULL)
	, drv_()
	, dir_()
	, name_()
	, ext_()
	, curDir_()
	, tmpDir_()
	, pathDir_()
	, chgPathDir_()
	, tgtnm_()
	, tgtnmFmt_()
	//, var_()
	, obuf_()
{
	setTmpDir(NULL);
}

void ConvFmt::setChgPathDir(char const* dir) {
	fks_fileFullpath(&chgPathDir_[0], chgPathDir_.capacity(), dir);
	char* p = STREND(&chgPathDir_[0]);
	if (fks_pathIsSep(p[-1])) {
	    p[-1] = '\0';
	}
}

void ConvFmt::setTmpDir(char const* dir) {
	FnameBuf	tmp;
	if (dir == NULL || dir[0] == 0) {
	    dir = &tmp[0];
	    fks_getTmpEnv(&tmp[0], tmp.capacity());
	    fks_pathDelLastSep(&tmp[0]);
	}
    fks_fileFullpath(&tmpDir_[0], tmpDir_.capacity(), dir);
    fks_pathDelLastSep(&tmpDir_[0]);
}

void ConvFmt::clearVar() {
	for (int l = 0; l < 10; l++)
	    var_[l].clear();
}

bool ConvFmt::setVar(unsigned m, char const* p, size_t l) {
	if (l < 1 || l >= (var_[0].capacity() - 1))
	    return false;
	var_[m].assign(p, p + l);
	return true;
}

/// 相対パスを作る時の基準ディレクトリ(通常はカレントディレクトリ)
void ConvFmt::setRelativeBaseDir(char const* dir) {
	fks_fileFullpath(&relativeBaseDir_[0], relativeBaseDir_.capacity(), dir);
}

/// fpath は fullpath 前提
int ConvFmt::write(char const* fpath, fks_stat_t const* st) {
	// 生テキスト.
	lineBuf_  = fpath;

	// カレントディレクトリがなければ取得(1回取得)
	if (curDir_.empty())
		fks_getcwd(&curDir_[0], curDir_.capacity());
	if (relativeBaseDir_.empty())
		relativeBaseDir_ = curDir_;

	// フルパス化して各種要素を準備.
	fpath = initPathStrings(fpath);

   	// ターゲットの指定があれば今回のターゲット名を設定し、日付チェック.
	bool ok = setTgtNameAndCheck(st);

	if (ok) { 	// 変換を行う場合.
	    StrFmt(&obuf_[0], &fmtBuf_[0], obuf_.capacity(), st);
	    outBuf_.push_back(obuf_.c_str());
	}
	++num_;
	return 0;
}

/// フルパス化して各種要素を準備.
char* ConvFmt::initPathStrings(char const* fpath0) {
	//char* fpath = fks_pathFullpath(&fullpath_[0], fullpath_.capacity(), fpath0, &curDir_[0]);
	char* fpath = fks_fileFullpath(&fullpath_[0], fullpath_.capacity(), fpath0);
	fks_pathGetDrive(&drv_[0], drv_.capacity(), fpath);
	fks_pathGetDir(&dir_[0], dir_.capacity(), fpath);
	fks_pathGetBaseNameNoExt(&name_[0], name_.capacity(), fks_pathBaseName(fpath));
	fks_pathCpy(&ext_[0], ext_.capacity(), fks_pathExt(fpath));

	fks_pathDelLastSep(&dir_[0]);  /* ディレクトリ名の後ろの'\'をはずす */
	pathDir_ = drv_;
	pathDir_ += dir_;
	if (!chgPathDir_.empty())
	    pathDir_ = chgPathDir_;
	if (ext_[0] == '.')		// 拡張子の '.' をはずす.
	    memmove(&ext_[0], &ext_[1], strlen(&ext_[1])+1);
	return fpath;
}

bool ConvFmt::setTgtNameAndCheck(fks_stat_t const* st) {
	if (tgtnmFmt_.empty())
		return true;
	fks_stat_t tgtSt = {0};
	StrFmt(&tgtnm_[0], tgtnmFmt_.c_str(), tgtnm_.capacity(), &tgtSt);
	if (fks_stat(&tgtnm_[0], &tgtSt) < 0)
		return true;	// error 時は日付比較できないので そのまま変換へ.
	fks_time_t	ltm  = tgtSt.st_mtime;
	fks_time_t  rtm  = st->st_mtime;
	if (ltm ==  0 || rtm == 0) // 0は日付無効状態とするので、そのまま変換へ.
		return true;
	// 双方に日付があるときのみ比較で、今回のpathがtargetより新しければ変換へ.
	return ltm < rtm;
}


int ConvFmt::writeLine0(char const* s) {
	char* p = &obuf_[0];
	char c;
	while ((c = (*p++ = *s++)) != '\0') {
	    if (c == '$') {
	    	--p;
	    	c = *s++;
	    	if (c == '$') {
	    	    *p++ = '$';
	    	} else if (c >= '1' && c <= '9') {
	    	    p = STPCPY(p, var_[c-'0'].c_str());
	    	} else {
	    	    //fprintfE(stderr,"レスポンス中の $指定がおかしい(%c)\n",c);
	    	    fprintf(stderr,"Incorrect '$' format : '$%c'\n",c);
	    	    exit(1);
	    	}
	    }
	}
 #if 1
	strcat(&obuf_[0], "\n");
	outBuf_.push_back(obuf_.c_str());
 #else
	fprintf(fp, "%s\n", obuf_.c_str());
 #endif
	return 0;
}


void ConvFmt::StrFmt(char *dst, char const* fmt, size_t sz, fks_stat_t const* st) {
	char	buf[FIL_NMSZ*4] = {0};
	char	*b;
	int 	n;
	char	drv[2];
	drv[0] = drv_[0];
	drv[1] = 0;

	char const* s = fmt;
	char*	    p = dst;
	char*	    q = NULL;
	char*	    pe = p + sz;
	char	    c;
	while ((c = (*p++ = *s++)) != '\0' && p < pe) {
	    if (c == '$') {
	    	--p;
	    	char* tp = p;
	    	bool relative = false;
	    	int  uplow    	= 0;
	    	int  sepMode	= 0;	// 1: / 化.  2: \ 化.
	    	n = -1;
	    	c = *s++;
	    	if (c == '+') { /* +NN は桁数指定だ */
	    	    n = strtoul(s,(char**)&s,10);
	    	    if (s == NULL || *s == 0)
	    	    	break;
	    	    if (n >= FIL_NMSZ)
	    	    	n = FIL_NMSZ;
	    	    c = *s++;
	    	}
			while (c) {
    	    	if (c == 'R') {
					relative = true;
					c = *s++;
    	    	//} else if (c == 'F') {
				//	relative = false;
				//	c = *s++;
				} else if (c == 'U') {
					uplow = 1;
					c = *s++;
				} else if (c == 'u') {
					uplow = -1;
					c = *s++;
				} else if (c == 'B') {
					sepMode = 2;
					c = *s++;
				} else if (c == 'b') {
					sepMode = 1;
					c = *s++;
				} else {
					break;
				}
			}
	    	switch (c) {
	    	case 's':   *p++ = ' ';     break;
	    	case 't':   *p++ = '\t';    break;
	    	case 'n':   *p++ = '\n';    break;
	    	case '$':   *p++ = '$';     break;
	    	case '#':   *p++ = '#';     break;
	    	case '[':   *p++ = '<';     break;
	    	case ']':   *p++ = '>';     break;
	    	case '`':   *p++ = '\'';    break;
	    	case '^':   *p++ = '"';     break;

	    	case 'l':   p = stpCpy(p,lineBuf_,n,uplow);  break;
	    	case 'v':   p = stpCpy(p,drv,n,uplow);  	 break;

	    	case 'd':
	    	    if (autoWqFlg_) *p++ = '"';
	    	    p = stpCpy(p, dir_.c_str(), n, uplow);
	    	    if (autoWqFlg_) *p++ = '"';
	    	    *p = 0;
	    	    break;

	    	case 'D':
	    	    if (autoWqFlg_) *p++ = '"';
	    	    q = fks_pathBaseName(&dir_[0]);
	    	    p = stpCpy(p, q, n, uplow);
	    	    if (autoWqFlg_) *p++ = '"';
	    	    *p = 0;
	    	    break;

	    	case 'x':
	    	    if (autoWqFlg_) *p++ = '"';
	    	    p = stpCpy(p,name_.c_str(),n,uplow);
	    	    if (autoWqFlg_) *p++ = '"';
	    	    *p = 0;
	    	    break;

	    	case 'e':
	    	    p = stpCpy(p,ext_.c_str(),n,uplow);
	    	    break;

	    	case 'w':
	    	    if (autoWqFlg_) *p++ = '"';
				tp = p;
	    	    p = stpCpy(p, tmpDir_.c_str(), n, uplow);
	    	    if (relative) p = changeRelative(tp);
	    	    if (sepMode)  changeSep(tp, sepMode);
	    	    if (autoWqFlg_) *p++ = '"';
	    	    *p = 0;
	    	    break;

	    	case 'p':
	    	    if (autoWqFlg_) *p++ = '"';
				tp = p;
	    	    p = stpCpy(p,pathDir_.c_str(),n,uplow);
	    	    if (relative) p = changeRelative(tp);
	    	    if (sepMode)  changeSep(tp, sepMode);
	    	    if (autoWqFlg_) *p++ = '"';
	    	    *p = 0;
	    	    break;

	    	case 'c':
	    	    b = buf;
	    	    if (autoWqFlg_) *b++ = '"';
	    	    b = stpCpy(b,name_.c_str(),0,uplow);
	    	    if (!ext_.empty()) {
	    	    	b = STPCPY(b,".");
	    	    	b = stpCpy(b,ext_.c_str(),0,uplow);
	    	    }
	    	    if (autoWqFlg_) *b++ = '"';
	    	    *b = 0;
	    	    if (n < 0) n = 1;
	    	    p += sprintf(p, "%-*s", n, buf);
	    	    break;

	    	case 'f':
	    	    b = buf;
	    	    if (autoWqFlg_) *b++ = '"';
				tp = b;
	    	    b = stpCpy(b,fullpath_.c_str(),0,uplow);
	    	    if (relative) b = changeRelative(tp);
	    	    if (sepMode)  changeSep(tp, sepMode);
	    	    if (autoWqFlg_) *b++ = '"';
	    	    *b = 0;
	    	    if (n < 0) n = 1;
	    	    p += sprintf(p, "%-*s", n, buf);
	    	    break;

	    	case 'g':
	    	    b = buf;
	    	    if (autoWqFlg_) *b++ = '"';
				tp = b;
	    	    q = b;
	    	    b = stpCpy(b,fullpath_.c_str(),0,uplow);
	    	    q = fks_pathExt(q);
	    	    if (q)
	    	    	*q = '\0';
	    	    if (relative) b = changeRelative(tp);
	    	    if (sepMode)  changeSep(tp, sepMode);
	    	    if (autoWqFlg_) *b++ = '"';
	    	    *b = '\0';
	    	    if (n < 0) n = 1;
	    	    p += sprintf(p, "%-*s", n, buf);
	    	    break;

	    	case 'o':
	    	    b = buf;
	    	    if (autoWqFlg_) *b++ = '"';
				tp = b;
	    	    b = stpCpy(b, tgtnm_.c_str(), 0, uplow);
	    	    if (relative && fks_pathIsAbs(tp)) b = changeRelative(tp);
	    	    if (sepMode)  changeSep(tp, sepMode);
	    	    if (autoWqFlg_) *b++ = '"';
	    	    *b = '\0';
	    	    if (n < 0) n = 1;
	    	    p += sprintf(p, "%-*s", n, buf);
	    	    break;

	    	case 'z':
    	    	if (n < 0)
    	    	    n = 10;
    	    	p += sprintf(p, "%*" PRIF_LL "d", n, (PRIF_LLONG)st->st_size);
    	    	break;

	    	case 'Z':
    	    	if (n < 0)
    	    	    n = 8;
    	    	p += sprintf(p, "%*" PRIF_LL "X", n, (PRIF_ULLONG)st->st_size);
	    	    break;

	    	case 'i':
    	    	if (n < 0)
    	    	    n = 1;
    	    	p += sprintf(p, "%0*" PRIF_LL "d", n, (PRIF_LLONG)(num_));
	    	    break;

	    	case 'I':
    	    	if (n < 0)
    	    	    n = 1;
    	    	p += sprintf(p, "%0*" PRIF_LL "X", n, (PRIF_ULLONG)(num_));
	    	    break;

	    	case 'j':
	    	case 'J':
	    	    {
					Fks_DateTime	dt = {0};
					fks_fileTimeToLocalDateTime(st->st_mtime, &dt);
	    	    	if (n < 0)
	    	    	    n = 10;
	    	    	if (n < 8) {			// 5
	    	    	    sprintf(buf, "%02d-%02d", dt.month, dt.day);
					} else if (n < 10) {	// 8
	    	    	    sprintf(buf, "%02d-%02d-%02d", dt.year%100, dt.month, dt.day);
					} else if (n < 13) {	// 10
	    	    	    sprintf(buf, "%04d-%02d-%02d", dt.year, dt.month, dt.day);
					} else if (n < 16) {	// 13
	    	    	    sprintf(buf, "%04d-%02d-%02d %02d", dt.year, dt.month, dt.day, dt.hour);
					} else if (n < 19) {	// 16
	    	    	    sprintf(buf, "%04d-%02d-%02d %02d:%02d", dt.year, dt.month, dt.day, dt.hour, dt.minute);
					} else if (n < 22) {	// 19
	    	    	    sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d", dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second);
					} else if (n < 23) {	// 22
	    	    	    sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d.%1d", dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second, ((dt.milliSeconds+49) / 100)%10);
					} else if (n < 24) {	// 23
	    	    	    sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d.%02d", dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second, ((dt.milliSeconds+5) / 10)%100);
					} else {				// 24
	    	    	    sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d.%03d", dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second,dt.milliSeconds%1000);
					}
					if (c == 'J') {	// 大文字指定だった場合は ファイル出力用.
						char* t = buf;
						while (*t) {
							if (*t == ' ') 		*t = '_';
							else if (*t == ':') *t = '_';
							++t;
						}
					}
	    	    	p += sprintf(p, "%-*s", n, buf);
	    	    }
	    	    break;

	    	default:
	    	    if (c >= '1' && c <= '9') {
					tp = p;
	    	    	p = STPCPY(p, var_[c-'0'].c_str());
		    	    if (relative && fks_pathIsAbs(tp)) p = changeRelative(tp);
    	    	    if (sepMode) changeSep(tp, sepMode);
	    	    } else {
	    	    	// fprintfE(stderr,".cfg 中 $指定がおかしい(%c)\n",c);
	    	    	fprintf(stderr, "Incorrect '$' format : '$%c'\n",c);
	    	    	// exit(1);
	    	    }
	    	}
	    }
	}
}

char *ConvFmt::stpCpy(char *d, char const* s, ptrdiff_t clm, int upLow) {
	size_t	      n = 0;
	if (upLow == 0) {
	    n = strlen(s);
		memmove(d, s, n);
		d += n;
	} else if (upLow > 0) {	/* 大文字化 */
		strcpy(d, s);
		fks_pathToUpper(d);
		d += strlen(d);
	} else {    	/* 小文字化 */
		strcpy(d, s);
		fks_pathToLower(d);
		d += strlen(d);
	}
	clm -= (ptrdiff_t)n;
	while (clm > 0) {
	    *d++ = ' ';
	    --clm;
	}
	*d = '\0';
	return d;
}

void ConvFmt::changeSep(char* d, int sepMode)
{
	if (sepMode == 1)
		fks_pathBackslashToSlash(d);
	else if (sepMode == 2)
		fks_pathSlashToBackslash(d);
}

char* ConvFmt::changeRelative(char* d)
{
	FnameBuf	buf;
	fks_pathRelativePath(&buf[0], buf.capacity(), d, &relativeBaseDir_[0]);
	return STPCPY(d, &buf[0]);
}

