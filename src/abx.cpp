/**
 *  @file   abx.cpp
 *  @brief  ファイル名を検索、該当ファイル名を文字列に埋込(バッチ生成)
 *  @author Masashi KITAMURA (tenka@6809.net)
 *  @date   1995-2018
 *	@license Boost Software License Version 1.0
 *  @note
 *  	    see license.txt
 *	-mt
 *	
 */

#include <fks/fks_config.h>
#include <fks/fks_path.h>
#include <fks/fks_io.h>
#include <fks/fks_dirent.h>
#include <fks/fks_time.h>
#include <fks/fks_misc.h>
#include <fks/fks_assert_ex.h>

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <process.h>
#include <algorithm>
#include <string>
#include <vector>
#include <list>

#include "subr.hpp"
#include "StrzBuf.hpp"

#include "abx_usage.h"


#ifdef ENABLE_MT_X
#include "abxmt.hpp"
#endif

#ifdef _WIN32
#include <windows.h>
#endif


/*---------------------------------------------------------------------------*/

enum { OBUFSIZ	= 0x80000 };	/* 定義ファイル等のサイズ   	    	*/
enum { FMTSIZ	= 0x80000 };	/* 定義ファイル等のサイズ   	    	*/


typedef std::list<std::string>	StrList;
typedef StrzBuf<FIL_NMSZ>   	FnameBuf;
typedef std::vector<Fks_DirEntPathStat const*>	PathStats;




/*--------------------- エラー処理付きの標準関数 ---------------------------*/

/** fopen +
 */
FILE *fopenX(char const* name, char const* mod) {
    FILE *fp = fopen(name,mod);
    if (fp == NULL) {
    	//fprintf(stderr, "ファイル %s をオープンできません\n", name);
    	fprintf(stderr, "File open error : %s\n", name);
		return NULL;
    }
    setvbuf(fp, NULL, _IOFBF, 1024*1024);
    return fp;
}



/*---------------------------------------------------------------------------*/

class ConvFmt;

enum SortType {
    ST_NONE = 0x00,
    ST_NAME = 0x01, 	// 名前でソート.
    ST_EXT  = 0x02, 	// 拡張子.
    ST_SIZE = 0x04, 	// サイズ.
    ST_DATE = 0x08, 	// 日付/時間.
    ST_ATTR = 0x10, 	// ファイル属性.
    ST_NUM  = 0x20, 	// 数値比較の名前.
    //ST_MASK = 0x7F,	// ソート情報マスク
};

enum FileAttr {
 #ifdef _WIN32
    FA_Norm   = FILE_ATTRIBUTE_NORMAL,
    FA_RdOnly = FILE_ATTRIBUTE_READONLY,
    FA_Hidden = FILE_ATTRIBUTE_HIDDEN,
    FA_Sys    = FILE_ATTRIBUTE_SYSTEM,
    FA_Volume = FILE_ATTRIBUTE_DEVICE,
    FA_Dir    = FILE_ATTRIBUTE_DIRECTORY,
    FA_Arcive = FILE_ATTRIBUTE_ARCHIVE,
    FA_MASK   = FILE_ATTRIBUTE_NORMAL|FILE_ATTRIBUTE_READONLY|FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM|FILE_ATTRIBUTE_DEVICE|FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_ARCHIVE,
    FA_MASK_NOARC = FILE_ATTRIBUTE_NORMAL|FILE_ATTRIBUTE_READONLY|FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM|FILE_ATTRIBUTE_DEVICE|FILE_ATTRIBUTE_DIRECTORY,
 #else
    FA_Norm   = 0x100,
    FA_RdOnly = 0x001,
    FA_Hidden = 0x002,
    FA_Sys    = 0x004,
    FA_Volume = 0x008,
    FA_Dir    = 0x010,
    FA_Arcive = 0x020,
    FA_MASK   = 0x03f,
    FA_MASK_NOARC = 0xDF/*0x1f*/,
 #endif
};


/*---------------------------------------------------------------------------*/

class ConvFmt {
public:
    enum { VAR_NMSZ = FIL_NMSZ };

    ConvFmt()
    	: upLwrFlg_(false)
    	, autoWqFlg_(false)
    	, num_(0)
    	, numEnd_(0)
    	, fmtBuf_(NULL)
    	, lineBuf_(NULL)
    	, curDir_(NULL)
    	, drv_()
    	, dir_()
    	, name_()
    	, ext_()
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

    void setChgPathDir(char const* dir) {
    	fks_fileFullpath(&chgPathDir_[0], chgPathDir_.capacity(), dir);
    	char* p = STREND(&chgPathDir_[0]);
    	if (fks_pathIsSep(p[-1])) {
    	    p[-1] = '\0';
    	}
    }

    char const* tmpDir() const { return tmpDir_.c_str(); }

    void setTmpDir(char const* dir) {
		FnameBuf	tmp;
		if (dir == NULL || dir[0] == 0) {
		    dir = &tmp[0];
		    fks_getTmpEnv(&tmp[0], tmp.size());
		    fks_pathDelLastSep(&tmp[0]);
		}
	    fks_fileFullpath(&tmpDir_[0], tmpDir_.capacity(), dir);
	    fks_pathDelLastSep(&tmpDir_[0]);
    }

    void setTgtnmFmt(char const* tgt) {
    	tgtnmFmt_ = tgt;
    }

    void clearVar() {
    	for (int l = 0; l < 10; l++)
    	    var_[l].clear();
    }

    char const* getVar(unsigned n) const {
    	return (n < 10) ? var_[n].c_str() : "";
    }

    bool setVar(unsigned m, char const* p, size_t l) {
    	if (l < 1 || l >= (var_[0].capacity() - 1))
    	    return false;
    	var_[m].assign(p, p + l);
    	return true;
    }

    void setNum(FKS_ULLONG num) { num_ = num; }

    void setUpLwrFlag(bool sw) { upLwrFlg_ = sw; }

    void setAutoWq(bool sw) { autoWqFlg_ = sw; }

    void setFmtStr(char const* fmtBuf) { fmtBuf_ = fmtBuf; }

	void setCurDir(char const* dir) { curDir_ = dir; }

	/// fpath は fullpath 前提
    int write(char const* fpath, fks_stat_t const* st) {
		lineBuf_ = fpath;
		//if (!noFindFile_) {
		//	fks_fileFullpath(&fullpath_[0], fullpath_.capacity(), fpath);
		//	fpath = &fullpath_[0];
		//} else
		{
			fullpath_ = fpath;
		}
    	splitPath(fpath);

		if (!tgtnmFmt_.empty())
    		StrFmt(&tgtnm_[0], tgtnmFmt_.c_str(), tgtnm_.capacity(), st);	   	// 今回のターゲット名を設定

    	if (tgtnmFmt_.empty() || fks_fileDateCmp(&tgtnm_[0], fpath) < 0) { 	// 日付比較しないか、する場合はターゲットが古ければ
    	    StrFmt(&obuf_[0], &fmtBuf_[0], obuf_.capacity(), st);
    	    outBuf_.push_back(obuf_.c_str());
    	}
    	++num_;
    	return 0;
    }

    int writeLine0(char const* s) {
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

    StrList&	    outBuf() { return outBuf_; }
    StrList const&  outBuf() const { return outBuf_; }
    void    	    clearOutBuf() { StrList().swap(outBuf_); }

private:
    void splitPath(char const* fullpath) {
		fks_pathGetDrive(&drv_[0], drv_.capacity(), fullpath);
		fks_pathGetDir(&dir_[0], dir_.capacity(), fullpath);
		fks_pathGetBaseNameNoExt(&name_[0], name_.capacity(), fks_pathBaseName(fullpath));
		fks_pathCpy(&ext_[0], ext_.capacity(), fks_pathExt(fullpath));

    	fks_pathDelLastSep(&dir_[0]);  /* ディレクトリ名の後ろの'\'をはずす */
    	pathDir_ = drv_;
    	pathDir_ += dir_;
    	if (!chgPathDir_.empty()) {
    	    pathDir_ = chgPathDir_;
    	}
    	/* 拡張子の '.' をはずす */
    	if (ext_[0] == '.') {
    	    memmove(&ext_[0], &ext_[1], strlen(&ext_[1])+1);
    	}
    }

    void StrFmt(char *dst, char const* fmt, size_t sz, fks_stat_t const* st) {
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
    	    	    if (autoWqFlg_) *p++ = '"';
    	    	    *p = 0;
    	    	    if (sepMode) changeSep(tp, sepMode);
    	    	    break;

    	    	case 'p':
    	    	    if (autoWqFlg_) *p++ = '"';
					tp = p;
    	    	    p = stpCpy(p,pathDir_.c_str(),n,uplow);
    	    	    if (relative) p = changeRelative(tp);
    	    	    if (autoWqFlg_) *p++ = '"';
    	    	    *p = 0;
    	    	    if (sepMode) changeSep(tp, sepMode);
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
    	    	    if (autoWqFlg_) *b++ = '"';
    	    	    *b = 0;
    	    	    if (n < 0) n = 1;
    	    	    p += sprintf(p, "%-*s", n, buf);
    	    	    if (sepMode) changeSep(tp, sepMode);
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
    	    	    if (autoWqFlg_) *b++ = '"';
    	    	    *b = '\0';
    	    	    if (n < 0) n = 1;
    	    	    p += sprintf(p, "%-*s", n, buf);
    	    	    if (sepMode) changeSep(tp, sepMode);
    	    	    break;

    	    	case 'o':
    	    	    b = buf;
    	    	    if (autoWqFlg_) *b++ = '"';
					tp = b;
    	    	    b = stpCpy(b, tgtnm_.c_str(), 0, uplow);
    	    	    if (relative && fks_pathIsAbs(tp)) b = changeRelative(tp);
    	    	    if (autoWqFlg_) *b++ = '"';
    	    	    if (n < 0) n = 1;
    	    	    p += sprintf(p, "%-*s", n, buf);
    	    	    if (sepMode) changeSep(tp, sepMode);
    	    	    break;

    	    	case 'z':
	    	    	if (n < 0)
	    	    	    n = 10;
	    	    	p += sprintf(p, "%*lld", n, (FKS_LLONG)st->st_size);
	    	    	break;

    	    	case 'Z':
	    	    	if (n < 0)
	    	    	    n = 8;
	    	    	p += sprintf(p, "%*llX", n, (FKS_ULLONG)st->st_size);
    	    	    break;

    	    	case 'i':
	    	    	if (n < 0)
	    	    	    n = 1;
	    	    	p += sprintf(p, "%0*lld", n, (FKS_ULLONG)(num_));
    	    	    break;

    	    	case 'I':
	    	    	if (n < 0)
	    	    	    n = 1;
	    	    	p += sprintf(p, "%0*llX", n, (FKS_ULLONG)(num_));
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
								else if (*t == ':') *t = '.';
							}
						}
    	    	    	p += sprintf(p, "%-*s", n, buf);
    	    	    }
    	    	    break;

    	    	default:
    	    	    if (c >= '1' && c <= '9') {
    	    	    	p = STPCPY(p, var_[c-'0'].c_str());
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

    char *stpCpy(char *d, char const* s, ptrdiff_t clm, int upLow) {
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

	void changeSep(char* d, int sepMode)
	{
		if (sepMode == 1)
			fks_pathBackslashToSlash(d);
		else if (sepMode == 2)
			fks_pathSlashToBackslash(d);
	}

	char* changeRelative(char* d)
	{
		FnameBuf	buf;
		fks_pathRelativePath(&buf[0], buf.capacity(), d, curDir_);
		return STPCPY(d, &buf[0]);
	}

private:
    bool    	    	upLwrFlg_;
    bool    	    	autoWqFlg_; 	/* $f等で自動で両端に"を付加するモード. */
    FKS_ULLONG 	    	num_;	    	/* $i で生成する番号 */
    FKS_ULLONG 	    	numEnd_;    	/* 連番をファイル名の文字列の代わりにする指定をした場合の終了アドレス */
    char const*     	fmtBuf_;    	/* 変換文字列を収める */
    char const*     	lineBuf_;
    char const*			curDir_;
    FnameBuf	    	var_[10];
	FnameBuf			fullpath_;
    FnameBuf	    	drv_;
    FnameBuf	    	dir_;
    FnameBuf	    	name_;
    FnameBuf	    	ext_;
    FnameBuf	    	tmpDir_;
    FnameBuf	    	pathDir_;
    FnameBuf	    	chgPathDir_;
    FnameBuf	    	tgtnm_;
    FnameBuf	    	tgtnmFmt_;
    StrzBuf<OBUFSIZ>	obuf_;	    	/* .cfg(.res) 読み込みや、出力用のバッファ */

    StrList 	    	outBuf_;
};



/*--------------------------------------------------------------------------*/
class Opts {
public:
    ConvFmt&	    rConvFmt_;
    bool    	    recFlg_;	    	    /* 再帰の有無 */
    bool    	    zenFlg_;	    	    /* MS全角対応 */
    bool    	    batFlg_;	    	    /* バッチ実行 */
    bool    	    batEx_; 	    	    /* -bの有無 */
    bool    	    linInFlg_;	    	    /* RES入力を行単位処理*/
    bool    	    autoWqFlg_;
    bool    	    upLwrFlg_;
    bool    	    sortRevFlg_;
    bool    	    sortLwrFlg_;
    int     	    noFindFile_;     	   /* ファイル検索しない */
    int     	    knjChk_;	    	    /* MS全角存在チェック */
    unsigned	    fattr_; 	    	    /* ファイル属性 */
    SortType	    sortType_;	    	    /* ソート */
    size_t  	    topN_;  	    	    /* 処理個数 */
    char const*     dfltExtp_;	    	    /* デフォルト拡張子 */
    fks_isize_t	    szmin_; 	    	    /* szmin > szmaxのとき比較を行わない*/
    fks_isize_t	    szmax_;
    fks_time_t	    dtmin_; 	    	    /* dtmin > dtmaxのとき比較を行わない*/
    fks_time_t	    dtmax_;
 #ifdef ENABLE_MT_X
    unsigned	    nthread_;
 #endif
    size_t  	    renbanStart_;   	    /* 連番の開始番号. 普通0 */
    size_t  	    renbanEnd_;     	    /* 連番の開始番号. 普通0 */
    FnameBuf	    outname_;	    	    /* 出力ファイル名 */
    FnameBuf	    ipath_; 	    	    /* 入力パス名 */
    FnameBuf	    dfltExt_;	    	    /* デフォルト拡張子 */
    FnameBuf	    chgPathDir_;
    FnameBuf	    exename_;
    FnameBuf	    curdir_;

public:
    Opts(ConvFmt& rConvFmt)
    	: rConvFmt_(rConvFmt)
    	, recFlg_(false)
    	, zenFlg_(true)
    	, batFlg_(false)
    	, batEx_(false)
    	, linInFlg_(false)
    	, autoWqFlg_(false)
    	, upLwrFlg_(false)
    	, sortRevFlg_(false)
    	, sortLwrFlg_(false)
    	, noFindFile_(0)
    	, knjChk_(0)
    	, fattr_(0)
    	, sortType_(ST_NONE)
    	, topN_(0)
    	, dfltExtp_(NULL)
    	, szmin_(FKS_ISIZE_MAX)
    	, szmax_(0)
    	, dtmin_(FKS_TIME_MAX)
    	, dtmax_(0)
       #ifdef ENABLE_MT_X
    	, nthread_(0)
       #endif
    	, renbanStart_(0)
    	, renbanEnd_(0)
    	, outname_()
    	, ipath_()
    	, dfltExt_()
    	, chgPathDir_()
    	, exename_()
    {
		fks_getcwd(&curdir_[0], curdir_.capacity());
    }

    void setExename(char const* exename) {
    	exename_ = exename;
     #ifdef _WIN32
    	fks_pathToLower(&exename_[0]);
     #endif
    }

    bool scan(char* s) {
    	char* p = s + 1;
    	int  c = *(unsigned char*)p++;
    	c = toupper(c);
    	switch (c) {
    	case 'X':
    	    batFlg_ = (*p != '-');
    	    //mt check
    	 #ifdef ENABLE_MT_X
    	    if (batFlg_) {
    	    	if (*p == 'm' || *p == 'M') {
    	    	    nthread_ = strtol(p+1, NULL, 0);
    	    	    ++nthread_;
    	    	}
    	    }
    	 #endif
    	    break;

    	case 'R':
    	    recFlg_ = (*p != '-');
    	    break;

    	case 'U':
    	    upLwrFlg_ = (*p != '-');
    	    break;

    	case 'N':
    	    noFindFile_ = (*p != '-');
    	    if (*p == 'd' || *p == 'D') // -nd 現状機能しない
    	    	noFindFile_ = 2;
    	    break;

    	case 'J':
    	    zenFlg_ = (*p != '-');
    	    break;

    	case 'B':
    	    batEx_ = (*p != '-');
    	    break;

    	case 'L':
    	    linInFlg_  = (*p != '-');
    	    break;

    	case 'T':
    	    if (*p == 0) {
    	    	topN_ = 1;
    	    } else {
    	    	topN_ = strtol(p,NULL,0);
    	    }
    	    break;

    	case 'C':
    	    c = toupper(*p);
    	    if (c == '-') {
    	    	knjChk_ = 0;
    	    } else if (c == 'D') {
				curdir_ = p + 1;
    	    } else if (c == 'K') {
    	    	knjChk_ = 1;
    	    	if (p[1] == '-')
    	    	    knjChk_ = -1;
    	    } else if (c == 'Y') {
    	    	knjChk_ = 2;
    	    	if (p[1] == '-')
    	    	    knjChk_ = -2;
    	    } else if (c == 'T' /*|| c == 'F'*/) {  // 'F'は旧互換
    	    	rConvFmt_.setTgtnmFmt(p + 1);
    	    } else if (c == 'I') {
    	    	renbanStart_ = strtol(p+1, (char**)&p, 0);
    	    	if (*p) {
    	    	    renbanEnd_ = strtol(p+1, (char**)&p, 0);
    	    	} else {
    	    	    renbanEnd_ = 0;
    	    	}
    	    } else {
    	    	goto ERR_OPTS;
    	    }
    	    break;

    	case 'Y':
    	    autoWqFlg_ = (*p != '-');
    	    break;

    	case 'E':
    	    dfltExt_ = p;
    	    dfltExtp_ = dfltExt_.c_str();
    	    if (*p == '$' && p[1] >= '1' && p[1] <= '9' && p[2] == 0) {
    	    	dfltExt_ = rConvFmt_.getVar(p[1]-'0');
    	    }
    	    /*dfltExt_[3] = 0;*/
    	    break;

    	case 'O':
    	    if (*p == 0)
    	    	goto ERR_OPTS;
    	    outname_ = p;
    	    break;

    	case 'I':
    	    if (*p == 0)
    	    	goto ERR_OPTS;
			fks_fileFullpath(&ipath_[0], ipath_.capacity(), p);
    	    p = STREND(&ipath_[0]);
    	    if (fks_pathIsSep(p[-1])) {
    	    	*p++ = FKS_PATH_SEP_CHR;
    	    	*p = '\0';
    	    }
    	    break;

    	case 'P':
    	    if (*p == 0)
    	    	goto ERR_OPTS;
    	    rConvFmt_.setChgPathDir(p);
    	    break;

    	case 'W':
    	    rConvFmt_.setTmpDir(p);
    	    break;

    	case 'A':
    	    strupr(p);
    	    while (*p) {
    	    	switch(*p) {
    	    	case 'N': fattr_ |= FA_Norm;   break;
    	    	case 'R': fattr_ |= FA_RdOnly; break;
    	    	case 'H': fattr_ |= FA_Hidden; break;
    	    	case 'S': fattr_ |= FA_Sys;    break;
    	    	case 'V': fattr_ |= FA_Volume; break;
    	    	case 'D': fattr_ |= FA_Dir;    break;
    	    	case 'A': fattr_ |= FA_Arcive; break;
    	    	}
    	    	++p;
    	    }
    	    break;

    	case 'S':
    	    c = 0;
    	    sortType_ = ST_NAME;
    	    strupr(p);
    	    while (*p) {
    	    	switch(*p) {
    	    	case '-': sortType_ = ST_NONE; break;
    	    	case 'N': sortType_ = ST_NAME; break;
    	    	case 'E': sortType_ = ST_EXT;  break;
    	    	case 'Z': sortType_ = ST_SIZE; break;
    	    	case 'T': sortType_ = ST_DATE; break;
    	    	case 'A': sortType_ = ST_ATTR; break;
    	    	case 'M': sortType_ = ST_NUM;  break;
    	    	case 'R': sortRevFlg_ = true;	break;
    	    	}
    	    	++p;
    	    }
    	    break;

    	case 'Z':
    	    szmin_ = (*p == '-') ? 0 : parseSize(p);
    	    if (*p) { /* *p == '-' */
    	    	szmax_ = FKS_ISIZE_MAX;
    	    	if (*++p)
    	    	    szmax_ = parseSize(p);
    	    	if (szmax_ < szmin_)
    	    	    goto ERR_OPTS;
    	    } else {
    	    	szmax_ = szmin_;
    	    }
    	    break;

    	case 'D':
    	    if (*p == 0) {
    	    	dtmax_ = dtmin_ = 0;
    	    } else {
				if (*p == '-') {
					dtmin_ = 0;
					++p;
				} else {
					dtmin_ = parseDateTime(p);
					if (dtmin_ < 0)
						goto ERR_OPTS;
					dtmax_ = FKS_TIME_MAX;
					if (*p == '-')
						++p;
				}
    	    	if (*p) {
					dtmax_ = parseDateTime(p);
					if (dtmax_ < 0)
						goto ERR_OPTS;
					if (dtmax_ < dtmin_)
	    	    	    goto ERR_OPTS;
    	    	}
    	    }
    	    break;

    	case '?':
    	case '\0':
			return usage();

    	default:
      ERR_OPTS:
    	    //fprintf(stderr, "コマンドラインでのオプション指定がおかしい : %s\n", s);
    	    fprintf(stderr, "Incorrect command line option : %s\n", s);
    	    return false;
    	}
    	return true;
    }

	bool usage()
	{
		abx_usage(exename_.c_str());
		return false;
	}

private:
	fks_time_t parseDateTime(char* &p)
	{
    	unsigned y = 0, m = 0, d = 0, h = 0, min=0, sec=0, milli=0;
		y = strtoul(p, &p, 10);
		if (y < 10000 && *p) {
			m = strtoul(p+1, &p, 10);
			if (*p) {
				d = strtoul(p+1, &p, 10);
				if (*p) {
					h = strtoul(p+1, &p, 10);
					if (*p) {
						h = strtoul(p+1, &p, 10);
						if (*p) {
							min = strtoul(p+1, &p, 10);
							if (*p) {
								sec = strtoul(p+1, &p, 10);
								if (*p)
									milli = strtoul(p+1, &p, 10);
							}
						}
					}
				}
			}
		} else if (y >= 10000) {
			unsigned t = y;
    	    y = (int)(t / 10000); y = (y < 70) ? 2000 + y : (y < 100) ? y + 1900 : y;
			m = (int)((t / 100) % 100); if (m == 0 || 12 < m) return -1;
			d = (int)(t % 100);     	if (d == 0 || 31 < d) return -1;
		}
	    Fks_DateTime dt = {y,m,0,d,h,min,sec,milli};
		return fks_localDateTimeToFileTime(&dt);
	}

	fks_isize_t	parseSize(char* &p)
	{
    	uint64_t	sz = uint64_t( strtoull(p, &p, 0) );
	    if (*p == 'K' || *p == 'k')     	p++, sz *= 1024;
	    else if (*p == 'M' || *p == 'm')	p++, sz *= 1024*1024;
	    else if (*p == 'G' || *p == 'g')	p++, sz *= 1024*1024*1024;
	    else if (*p == 'T' || *p == 't')	p++, sz *= 1024*1024*1024*1024ULL;
		return (fks_isize_t)sz;
	}
};


/*---------------------------------------------------------------------------*/
class Files {
	static Files* s_instance_;
public:
	Files(Opts& opts) : opts_(opts)
	{
		s_instance_ = this;
	}

	bool getPathStats(StrList& filenameList)
	{
		int flags = 0;
		if (opts_.recFlg_)
			flags |= FKS_DE_Recursive;
		if (!(opts_.fattr_ & FA_Dir))
			flags |= FKS_DE_FileOnly;
		else if (!(opts_.fattr_ & (FA_Norm|FA_RdOnly|FA_Hidden|FA_Sys|FA_Volume|FA_Arcive)))
			flags |= FKS_DE_DirOnly;

		Fks_DirEnt_IsMatchCB	isMatch = NULL;
		if ( (opts_.fattr_ & FA_Norm|FA_RdOnly|FA_Hidden|FA_Sys|FA_Volume|FA_Arcive)
			|| (opts_.szmin_ <= opts_.szmax_)
			|| (opts_.dtmin_ <= opts_.dtmax_)
		){
			isMatch = &matchCheck;
		}

		FnameBuf		ipath = opts_.ipath_;
		char*			iname = fks_pathBaseName(&ipath[0]);
	    FnameBuf	    fname;	    	/* 名前 work */
		pathStatBody_.reserve(filenameList.size());
		for (StrList::iterator ite = filenameList.begin(); ite != filenameList.end(); ++ite) {
   	    	char const* p = ite->c_str();
	    	if (!fks_pathIsAbs(p)) {	/* 相対パスのとき */
	    	    *iname  = '\0';
	    	    ipath  += p;
	    	    p       = &ipath[0];
	    	}
   	    	fname = p;
   	    	if (fks_pathCheckLastSep(p))
   	    	    fname += "*";

   	    	fks_pathSetDefaultExt(&fname[0], fname.capacity(), opts_.dfltExtp_);  	/* デフォルト拡張子付加 */

			Fks_DirEntPathStat*	pPathStats = NULL;
			fks_isize_t			n = fks_createDirEntPathStats(&pPathStats, NULL, fname.c_str(), flags, isMatch);
			if (n < 0)
				continue;
			if (n > 0 && pPathStats == NULL) {
				fprintf(stderr, "ERROR: read directories.\n");
				continue;
			}
			pathStatBody_.push_back(pPathStats);
			pathStats_.reserve( size_t(pathStats_.size() + n) );
			for (fks_isize_t i = 0; i < n; ++i) {
				Fks_DirEntPathStat* ps = &pPathStats[i];
				if (ps->path == NULL) {
					fprintf(stderr, "ERROR: pathname is null.\n");
					continue;
				}
				if (ps->stat == NULL) {
					fprintf(stderr, "ERROR: no file status (%s)\n", ps->path);
					continue;
				}
				pathStats_.push_back(ps);
			}
		}

		if (opts_.sortType_ || opts_.sortRevFlg_) {
			sortPartStats();
		}
		return true;
	}

	PathStats const&	pathStats() const {
		return pathStats_;
	}

private:
	static int matchCheck(Fks_DirEnt const* de)
	{
		Opts const& opts = s_instance_->opts_;
	 #if FKS_WIN32
		unsigned    fattr = opts.fattr_;
		if (fattr & FA_Norm|FA_RdOnly|FA_Hidden|FA_Sys|FA_Volume|FA_Arcive) {
			unsigned	w32attr = de->stat->st_native_attr;
			if ((fattr & w32attr) == 0)
				return 0;
		}
	 #endif
	 	if (opts.szmin_ < opts.szmax_) {
			if (de->stat->st_size < opts.szmin_ || opts.szmax_ < de->stat->st_size)
				return 0;
		}
		if (opts.dtmin_ < opts.dtmax_) {
			if (de->stat->st_size < opts.dtmin_ || opts.dtmax_ < de->stat->st_size)
				return 0;
		}

		int knjChk = opts.knjChk_;
		if (knjChk) {
			if (knjChk ==  1 && !chkKnjs(de->name))
				return 0;
			if (knjChk == -1 &&  chkKnjs(de->name))
				return 0;
			if (knjChk ==  2 && !strchr(de->name,'\\'))
				return 0;
			if (knjChk == -2 &&  strchr(de->name,'\\'))
				return 0;
		}

		return 1;
	}

    static int chkKnjs(const char *p)
    {
    	unsigned char c;
    	while((c = *(unsigned char *)p++) != 0) {
    	    if (c & 0x80)
    	    	return 1;
    	}
    	return 0;
    }

	/// 数字部分は数値で比較する名前比較.
	struct NumCmp {
		int 	dir_;
		NumCmp(int dir) : dir_(0) {}
    	bool operator()(Fks_DirEntPathStat const* l, Fks_DirEntPathStat const* r) const {
    	    return fks_pathDigitCmp(l->path, r->path) * dir_ < 0;
    	}
	};

	/// 名前比較.
	struct NameCmp {
		int 	dir_;
		NameCmp(int dir) : dir_(0) {}
    	bool operator()(Fks_DirEntPathStat const* l, Fks_DirEntPathStat const* r) const {
    	    return fks_pathCmp(l->path, r->path) * dir_ < 0;
    	}
	};

	/// 拡張子比較
	struct ExtCmp {
		int 	dir_;
		ExtCmp(int dir) : dir_(0) {}
    	bool operator()(Fks_DirEntPathStat const* l, Fks_DirEntPathStat const* r) const {
			int n = fks_pathCmp(fks_pathExt(l->path), fks_pathExt(r->path));
			if (n)
				return (dir_ * n) < 0;
    	    return fks_pathDigitCmp(l->path, r->path) * dir_ < 0;
    	}
	};

	/// サイズ比較
	struct SizeCmp {
		int 	dir_;
		SizeCmp(int dir) : dir_(0) {}
    	bool operator()(Fks_DirEntPathStat const* l, Fks_DirEntPathStat const* r) const {
			fks_isize_t  d = l->stat->st_size - r->stat->st_size;
			if (d)
				return (dir_ * d) < 0;
    	    return fks_pathDigitCmp(l->path, r->path) * dir_ < 0;
    	}
	};

	/// 日付比較
	struct DateCmp {
		int 	dir_;
		DateCmp(int dir) : dir_(0) {}
		bool operator()(Fks_DirEntPathStat const* l, Fks_DirEntPathStat const* r) const {
			fks_isize_t  d = l->stat->st_mtime - r->stat->st_mtime;
			if (d)
				return (dir_ * d) < 0;
			return fks_pathDigitCmp(l->path, r->path) * dir_ < 0;
		}
	};

	/// 属性比較. ほぼほぼ win 用
	struct AttrCmp {
		int 	dir_;
		AttrCmp(int dir) : dir_(0) {}
    	bool operator()(Fks_DirEntPathStat const* l, Fks_DirEntPathStat const* r) const {
		 #if FKS_WIN32
    	    /* アーカイブ属性は邪魔なのでオフする */
			int  d = (l->stat->st_native_attr & FA_MASK_NOARC) - (r->stat->st_native_attr & FA_MASK_NOARC);
		 #else
			int  d = l->stat->st_mode - r->stat->st_mode;
		 #endif
			if (d)
				return (dir_ * d) < 0;
    	    return fks_pathDigitCmp(l->path, r->path) * dir_ < 0;
    	}
	};

	//#define USE_LOWER_CMP	//TODO: 比較処理実装
 #ifdef	USE_LOWER_CMP	//TODO: linux,unix で 小文字化ソートしたい場合用
	/* 数字部分は数値で比較する名前比較 */
	/// 名前小文字列化比較
	struct LowerNumCmp {
		int 	dir_;
		LowerNumCmp(int dir) : dir_ {}
    	bool operator()(Fks_DirEntPathStat const* l, Fks_DirEntPathStat const* r) const {
			int n = fks_pathDigitLowerCmp(l->path, r->path);
			if (n == 0)
				n = fks_pathDigitCmp(l->path, r->path);
    	    return n * dir_ < 0;
    	}
	}

	/// 名前小文字列化比較
	struct LowerNameCmp {
		int 	dir_;
		LowerNameCmp(int dir) : dir_ {}
    	bool operator()(Fks_DirEntPathStat const* l, Fks_DirEntPathStat const* r) const {
			int n = fks_pathLowerCmp(l->path, r->path);
			if (n == 0)
				n = fks_pathCmp(l->path, r->path);
    	    return n * dir_ < 0;
    	}
	}
 #endif

	void sortPartStats()
	{
		int dir = opts_.sortRevFlg_ ? -1 : 1;
		switch (opts_.sortType_) {
		default:
		case ST_NUM :
		 #ifdef USE_LOWER_CMP
			if (opts_.sortLwrFlg_)
				std::sort(pathStats_.begin(), pathStats_.end(), LowerNumCmp(dir));
			else
		 #endif
				std::sort(pathStats_.begin(), pathStats_.end(), NumCmp(dir));
			break;
		case ST_NAME:
		 #ifdef USE_LOWER_CMP
			if (opts_.sortLwrFlg_)
				std::sort(pathStats_.begin(), pathStats_.end(), LowerNameCmp(dir));
			else
		 #endif
				std::sort(pathStats_.begin(), pathStats_.end(), NameCmp(dir));
			break;
		case ST_EXT :
			std::sort(pathStats_.begin(), pathStats_.end(), ExtCmp(dir));
			break;
		case ST_SIZE:
			std::sort(pathStats_.begin(), pathStats_.end(), SizeCmp(dir));
			break;
		case ST_DATE:
			std::sort(pathStats_.begin(), pathStats_.end(), DateCmp(dir));
			break;
		case ST_ATTR:
			std::sort(pathStats_.begin(), pathStats_.end(), AttrCmp(dir));
			break;
		}
	}

private:
	Opts const&		opts_;
	PathStats		pathStats_;
	PathStats		pathStatBody_;
};
Files*	Files::s_instance_;


/*---------------------------------------------------------------------------*/

class ResCfgFile {
public:
    ResCfgFile(Opts& rOpts, ConvFmt& rConvFmt, StrList& rFileNameList, StrList& rBeforeList, StrList& rAfterList, char* fmtBuf)
    	: rOpts_(rOpts)
    	, rConvFmt_(rConvFmt)
    	, rFileNameList_(rFileNameList)
    	, rBeforeStrList_(rBeforeList)
    	, rAfterStrList_(rAfterList)
    	, fmtBuf_(fmtBuf)
    	//, resP_(&resOBuf_[0])
    	, varIdx_(1)
    	//, varNo_[10]
    	, resName_()
    	, resOBuf_()
    {
    	memset(varNo_, 0, sizeof(varNo_));
    	resP_ = &resOBuf_[0];
    }

    /** レスポンスファイル入力
     */
    bool GetResFile(char const* name) {
    	size_t	l;

    	if (name[0] == 0) { 	    	    	/* ファイル名がなければ標準入力 */
    	    l = fread(&resOBuf_[0], 1, resOBuf_.capacity(), stdin);
    	} else {
			fks_pathSetDefaultExt(&resName_[0], resName_.capacity(), name, "abx");
    	    FILE* fp = fopenX(resName_.c_str(), "rt");
    	    if (!fp) {
				return false;
			}
		    l = fread(&resOBuf_[0], 1, resOBuf_.capacity(), fp);
		    if (ferror(fp)) {
		    	//fprintf(stderr, "ファイル読込みでエラー発生\n");
		    	fprintf(stderr, "File read error : %s\n", name);
		    	return false;
		    }
    	    fclose(fp);
    	}
    	resOBuf_[l] = 0;
    	if (l == 0)
    	    return true;

    	resP_ = &resOBuf_[0];
    	return getFmts();  /* 実際のファイル内容の処理 */
    }

    /** 定義ファイル入力
     */
    bool GetCfgFile(char *name, char *key) {
    	fks_fileFullpath(&resName_[0], resName_.capacity(), name);
    	FILE*	fp = fopenX(resName_.c_str(),"r");
    	if (!fp) {
			return false;
		}
    	size_t	l  = fread(&resOBuf_[0], 1, resOBuf_.capacity(), fp);
	    if (ferror(fp)) {
	    	//fprintf(stderr, "ファイル読込みでエラー発生\n");
	    	fprintf(stderr, "Cfg-file read error : %s\n", name);
	    	return false;
	    }
    	fclose(fp);

    	resOBuf_[l] = 0;
    	if (l == 0)
    	    return false;

    	if (key[1] == 0) /* ':'だけの指定のとき */
    	    printf("':変換名'一覧\n");
    	/*l = 1;*/
    	/*   */
    	strupr(key);
    	resP_ = &resOBuf_[0];
    	/* 改行+':'+変換名を探す */
    	while ((resP_ = strstr(resP_, "\n:")) != NULL) {
    	    resP_ ++;
    	    char* p = getLine();
    	    if (p)
    	    	p = strtok(p, " \t\r");
    	    /*printf("cmp %s vs %s\n",key,p);*/
    	    if (p == NULL || *p == 0)
    	    	continue;
    	    strupr(p);
    	    if (key[1]) {
    	    	/* 変換名が見つかればレスポンスと同じ処理をする */
    	    	if (keyStrEqu(key, p)) {
    	    	    if ((p = strstr(resP_, "\n:")) != NULL) {
    	    	    	*p = '\0';
    	    	    }
    	    	    return getFmts();
    	    	}
    	    } else {	/* 検索キーがなければ、一覧表示 */
    	    	printf("\t%s\n",p);
    	    }
    	}
    	if (key[1]) {
    		// fprintf(stderr, "%s には %s は定義されていない\n", resName_.c_str(), key);
    		fprintf(stderr, "%s is not defined in %s\n", key, resName_.c_str());
    	}
    	return false;
    }

private:
    /** resOBuf_に貯えたテキストより１行入力
     * 行末の改行は削除. resOBuf_は破壊
     */
    char *getLine(void) {
    	char *p;

    	p = resP_;
    	if (*p == 0)
    	    return NULL;
    	resP_ = strchr(p, '\n');
    	if (resP_ != NULL) {
    	    if (resP_[-1] == '\r')
    	    	resP_[-1] = 0;
    	    *resP_++ = 0;
    	} else {
    	    resP_ = STREND(p);
    	}
    	return p;
    }

    /** $数字 変数の設定
     */
    char *setDoll(char *p0) {
    	size_t l = 0;
    	char*  p = p0;
    	int m = *p++;
    	if (m < '1' || m > '9')
    	    goto ERR;
    	m -= '0';
    	if (*p == '=') {
    	    ++p;
    	    l = strcspn(p,"\t\n ");
    	    if (rConvFmt_.setVar(m, p, l) == false)
    	    	goto ERR;
    	    p += l+1;
    	    goto RET;

      ERR:
    	    //fprintf(stderr, ".cfg ファイルで $Ｎ 指定でおかしいものがある : $%s\n",p0);
    	    fprintf(stderr, ".cfg-file has an incorrect $N specification. : $%s\n", p0);
    	    
			exit(1);
    	} else if (*p++ == ':') {
    	    int i;
    	    int n = *p++;
    	    if (n < '1' || n > '9')
    	    	goto ERR2;
    	    n -= '0';
    	    if (*p++ != '{')
    	    	goto ERR2;
    	    i = 0;
    	    do {
    	    	l = strcspn(p,"|}");
    	    	if (i == varNo_[n]) {
    	    	    if (rConvFmt_.setVar(m, p, l) == false)
    	    	    	goto ERR;
    	    	    p = strchr(p,'}');
    	    	    if (p == NULL)
    	    	    	goto ERR2;
    	    	    p++;
    	    	    goto RET;
    	    	}
    	    	i++;
    	    	p += l + 1;
    	    } while (p[-1] == '|');
      ERR2:
    	    //fprintf(stderr, ".cfg ファイルで $Ｎ=文字列指定 または $Ｎ:Ｍ{..}指定でおかしいものがある : $%s\n",p0);
			fprintf(stderr, "$N=STRING or $N:M{..} specification in .cfg-file is incorrect. : $%s\n", p0);
    	    exit(1);
    	}
      RET:
    	return p;
    }

    /** s より空白で区切られた単語(ファイル名)をname にコピーする.
     * ただし "file name" のように"があれば"を削除し替りに間の空白を残す.
     */
    char const* getFileNameStr(char *d, char const* s) {
    	int f = 0;

    	s = StrSkipSpc(s);
    	while (*s) {
    	    if (*s == '"')
    	    	f ^= 1;
    	    else if (f == 0 && (*(unsigned char *)s <= ' '))
    	    	break;
    	    else
    	    	*d++ = *s;
    	    s++;
    	}
    	*d = 0;
    	return s;
    }

    /** 変換フォーマット文字列取得
     */
    bool getFmts() {
    	#define ISSPC(c)    ((unsigned char)c <= ' ')
    	char	name[FIL_NMSZ] = {0};
    	enum Mode { MD_Body, MD_Bgn, MD_End, MD_TameBody };
    	Mode	mode = MD_Body;
    	char* 	d 	 = fmtBuf_;
    	char*	p;
    	while ( (p = getLine()) != NULL ) {
    	    char* q = (char*)StrSkipSpc(p);
    	    if (strnicmp(q, "#begin", 6) == 0 && ISSPC(p[6])) {
    	    	mode = MD_Bgn;
    	    	continue;
    	    } else if (strnicmp(q, "#body", 5) == 0 && ISSPC(p[5])) {
    	    	mode = MD_Body;
    	    	continue;
    	    } else if (strnicmp(q, "#end", 4) == 0 && ISSPC(p[4])) {
    	    	mode = MD_End;
    	    	continue;
    	    }
    	    switch (mode) {
    	    case MD_Body: /* #body */
    	    	while (p && *p) {
    	    	    p = (char*)StrSkipSpc(p);  /* 空白スキップ */
    	    	    switch (*p) {
    	    	    case '\0':
    	    	    case '#':
    	    	    	goto NEXT_LINE;
    	    	    case '\'':
    	    	    	if (p[1] == 0) {
    	    	    	    //fprintf(stderr, "レスポンスファイル(定義ファイル中)の'変換文字列名'指定がおかしい\n");
    	    	    	    fprintf(stderr, "'CONVERSION-STRING-NAME' specification is incorrect.\n");
    	    	    	    return false;
    	    	    	}
    	    	    	p++;
    	    	    	d = strchr(p, '\'');
    	    	    	if (d) {
    	    	    	    *d = '\0';
    	    	    	    d = STPCPY(&fmtBuf_[0], p);
    	    	    	}
    	    	    	break;
    	    	    case '=':
    	    	    	d = &fmtBuf_[0];
    	    	    	mode = MD_TameBody;
    	    	    	goto NEXT_LINE;
    	    	    case '-':	    	    /* オプション文字列だ */
    	    	    	q = (char*)StrSkipNotSpc(p);
    	    	    	if (*q) {
    	    	    	    *q++ = 0;
    	    	    	} else {
    	    	    	    q = NULL;
    	    	    	}
    	    	    	if (rOpts_.scan(p) == false) {
    	    	    	    return false;
    	    	    	}
    	    	    	p = q;
    	    	    	break;
    	    	    case '$':	    	    /* $変数だ */
    	    	    	p = setDoll(p+1);
    	    	    	break;
    	    	    default:
    	    	    	if (rOpts_.linInFlg_) { /* 行単位でファイル名を取得 */
    	    	    	    rFileNameList_.push_back(p);
    	    	    	    goto NEXT_LINE;
    	    	    	} else {    	    /* 空白区切りでファイル名を取得 */
    	    	    	    p = (char*)getFileNameStr(name, p);
    	    	    	    rFileNameList_.push_back(name);
    	    	    	}
    	    	    }
    	    	}
    	      NEXT_LINE:;
    	    	break;
    	    case MD_Bgn: /* #begin */
    	    	rBeforeStrList_.push_back(p);
    	    	break;
    	    case MD_End: /* #end  */
    	    	rAfterStrList_.push_back(p);
    	    	break;
    	    case MD_TameBody: /* = バッファ溜め本体 */
    	    	d = STPCPY(d, p);
    	    	*d++ = '\n';
    	    	*d   = '\0';
    	    	break;
    	    }
    	}
    	return true;
    	#undef ISSPC
    }


    bool keyStrEqu(char *key, char *lin) {
    	char	*k,*f;
    	size_t	l;

    	rConvFmt_.clearVar();

    	k = key;
    	f = lin;
    	for (; ;) {
    	  NEXT:
    	    if (*k == *f) {
    	    	if (*k == '\0')
    	    	    return true;   /* マッチしたぞ */
    	    	k++;
    	    	f++;
    	    	continue;
    	    }
    	    if (*f == '{') {
    	    	f++;
    	    	varNo_[varIdx_] = 0;
    	    	do {
    	    	    l = strcspn(f,"|}");
    	    	    /*printf("l=%d\n",l);*/
    	    	    if (l < 1 || l >= ConvFmt::VAR_NMSZ-1) {
    	    	    	goto ERR1;
    	    	    }
    	    	    if (memcmp(k,f,l) == 0) {
    	    	    	if (varIdx_ >= 10) {
    	    	    	    //fprintf(stderr, "%s のある検索行に{..}が10個以上ある %s\n", resName_.c_str(),lin);
    	    	    	    fprintf(stderr, "There are ten or more {..} in a certain line in %s : %s\n", resName_.c_str(),lin);
    	    	    	    exit(1);
    	    	    	}
    	    	    	rConvFmt_.setVar(varIdx_, f, l);
    	    	    	++varIdx_;
    	    	    	k += l;
    	    	    	f = strchr(f,'}');
    	    	    	if (f == NULL) {
    	    	  ERR1:
    	    	    	    //fprintf(stderr, "%s で{..}の指定がおかしい %s\n",resName_.c_str(), lin);
    	    	    	    fprintf(stderr, "Invalid {..} designation in %s : %s\n",resName_.c_str(), lin);
    	    	    	    exit(1);
    	    	    	}
    	    	    	f++;
    	    	    	goto NEXT;
    	    	    }
    	    	    f += l + 1;
    	    	    varNo_[varIdx_]++;
    	    	} while (f[-1] == '|');
    	    	varNo_[varIdx_] = 0;
    	    }
    	    break;
    	}
    	return false;	       /* マッチしなかった */
    }


private:
    Opts&   	    	rOpts_;
    ConvFmt&	    	rConvFmt_;
    StrList&	    	rFileNameList_;
    StrList&	    	rBeforeStrList_;
    StrList&	    	rAfterStrList_;
    char*   	    	fmtBuf_;
    char*   	    	resP_;
    int     	    	varIdx_;
    int     	    	varNo_[10];
    FnameBuf	    	resName_;
    StrzBuf<OBUFSIZ>	resOBuf_;
};



class App {
public:
    App()
    	: outFp_(NULL)
    	, filenameList_()
    	, beforeStrList_()
    	, afterStrList_()
    	, abxName_()
    	, opts_(convFmt_)
    	, resCfgFile_(opts_, convFmt_, filenameList_, beforeStrList_, afterStrList_, &fmtBuf_[0])
    	, fmtBuf_()
    	, files_(opts_)
    {
    }

	~App() {
		// メモリー開放(free)等は os に任せれたほうが無難なので、あえてしない.
	}

    int main(int argc, char *argv[]) {
    	opts_.setExename(fks_pathBaseName(argv[0]));    /*アプリケーション名*/
    	if (argc < 2) {
    	    opts_.usage();
    	    return 1;
    	}

    	fks_pathSetExt(&abxName_[0], abxName_.capacity(), argv[0], "cfg");
    	if (scanOpts(argc, argv) == false)
    	    return 1;

		if (!opts_.noFindFile_ && !opts_.renbanEnd_) {	// ファイル検索時.
			if (files_.getPathStats(filenameList_) == false)
				return 1;
		} else {	// 生の文字列のみのとき
			//TODO: ソート対応.
		}

    	if (genText() == false)
    	    return 1;

    	if (outputText() == false)
    	    return 1;

    	if (execBat() == false)
    	    return 1;

    	if (tmpFName_[0])
    	    remove(&tmpFName_[0]);
    	return 0;
    }


private:

    /** コマンドラインのオプション/ファイル名/変換文字列, 取得
     */
    bool scanOpts(int argc, char *argv[]) {
    	int f = 0;
    	for (int i = 1; i < argc; ++i) {
    	    char* p = argv[i];
    	   LLL1:
    	    if (f) {
    	    	if (!fmtBuf_.empty())
    	    	    fmtBuf_ += " ";
    	    	fmtBuf_ += p;
    	    	if (f == 1) {
    	    	    p = strrchr(&fmtBuf_[0], '\'');
    	    	    if (p) {
    	    	    	f = 0;
    	    	    	*p = 0;
    	    	    }
    	    	}
    	    } else if (*p == '\'') {
    	    	f ^= 1;
    	    	p++;
    	    	if (*p)
    	    	    goto LLL1;

    	    } else if (*p == '=') {
    	    	f = 2;
    	    	p++;
    	    	if (*p)
    	    	    goto LLL1;

    	    } else if (*p == '-') {
    	    	if (opts_.scan(p) == false)
    	    	    return false;

    	    } else if (*p == '@') {
    	    	if (resCfgFile_.GetResFile(p+1) == false)
    	    		return false;

    	    } else if (*p == '+') {
    	    	++p;
    	    	if (fks_pathIsSep(*p) || fks_pathHasDrive(p)){
    	    	    fks_fileFullpath(&abxName_[0], abxName_.capacity(), p);
    	    	} else {
    	    	    char fbuf[FIL_NMSZ];
    	    	    fks_pathCpy(fbuf, FIL_NMSZ, argv[0]);
    	    	    *fks_pathBaseName(fbuf) = 0;
    	    	    fks_pathCat(fbuf, FIL_NMSZ, p);
    	    	    fks_fileFullpath(&abxName_[0], abxName_.capacity(), fbuf);
    	    	}
    	    	fks_pathSetDefaultExt(&abxName_[0], abxName_.capacity(), &abxName_[0], "cfg");

    	    } else if (*p == ':') {
    	    	if (p[1] == '#') {
    	    	    //fprintf(stderr, ":#で始まる文字列は指定できません（%s）\n",p);
    	    	    fprintf(stderr, ":\"#STRING\" can not be specified. : %s\n",p);
    	    	    return false;
    	    	}
    	    	if (resCfgFile_.GetCfgFile(&abxName_[0], p) == false)
    	    	    return false;
    	    } else if (*p == '$') {
    	    	if (p[1] >= '1' && p[1] <= '9' && p[2] == '=') {
    	    	    unsigned	no  = p[1] - '0';
    	    	    char const* s   = p + 3;
    	    	    convFmt_.setVar(no, s, strlen(s));
    	    	}
    	    } else {
    	    	filenameList_.push_back(p);
    	    }
    	}

       #ifdef ENABLE_MT_X
    	if (opts_.nthread_ && (!beforeStrList_.empty() || !afterStrList_.empty())) {
    	    //fprintf(stderr, "-xm 指定と #begin,#end 指定は同時に指定できません\n");
    	    fprintf(stderr, "You can not specify -xm and #begin(#end) at the same time.\n");
    	    return false;
    	}
       #endif

    	/* バッチ実行のとき */
    	if (opts_.batFlg_) {
    	    fks_tmpFile(&tmpFName_[0], FIL_NMSZ, "abx_", ".bat");
            //printf("tmpfname=%s\n", &tmpFName_[0]);
    	    opts_.outname_  = tmpFName_;
    	}

    	if (opts_.fattr_ == 0) {     /* デフォルトのファイル検索属性 */
    	    opts_.fattr_ = FA_Norm|FA_RdOnly|FA_Hidden|FA_Sys|FA_Volume|FA_Arcive;  //0x127
    	}
    	convFmt_.setUpLwrFlag(opts_.upLwrFlg_);

    	/* 変換文字列調整 */
    	if (fmtBuf_.empty()) {
    	    if (opts_.recFlg_)
    	    	fmtBuf_ = "$f\n";
    	    else
    	    	fmtBuf_ = "$c\n";
    	}
    	if (strchr(fmtBuf_.c_str(), '\n') == NULL)
    	    fmtBuf_ += "\n";
    	convFmt_.setFmtStr(fmtBuf_.c_str());

    	convFmt_.setAutoWq(opts_.autoWqFlg_);	    /* $f等で自動で両端に"を付加するモード. */

		convFmt_.setCurDir(&opts_.curdir_[0]);

    	return true;
    }


    bool genText() {
    	if (opts_.batEx_) { 	    	    /* バッチ実行用に先頭に echo off を置く */
    	    convFmt_.writeLine0("@echo off");
    	}

    	/* 直前出力テキスト */
    	for (StrList::iterator ite = beforeStrList_.begin(); ite != beforeStrList_.end(); ++ite) {
    	    convFmt_.writeLine0(ite->c_str());
    	}

    	/* 実行 */
		FKS_ULLONG topN = (opts_.topN_) ? opts_.topN_ : (FKS_ULLONG)(FKS_LLONG)-1;
		if (!opts_.noFindFile_ && !opts_.renbanEnd_) {	// 通常のファイル検索した結果を用いる場合.
   	    	convFmt_.setNum(opts_.renbanStart_);
    	    for (PathStats::const_iterator ite = files_.pathStats().begin(); ite != files_.pathStats().end() && topN > 0; ++ite, --topN) {
				Fks_DirEntPathStat const* ps = *ite;
    	    	convFmt_.write(ps->path, ps->stat);
    	   }
		} else {
			fks_stat_t	st   = { 0 };
    	    opts_.noFindFile_ = 1;
   	    	convFmt_.setNum(opts_.renbanStart_);
			if (opts_.renbanEnd_) {	/* 連番生成での初期値設定 */
				char*	path = &abxName_[0];
	    	    for (FKS_ULLONG num = opts_.renbanStart_; num <= opts_.renbanEnd_ && topN > 0; ++num, --topN) {
	    	    	convFmt_.setNum(num);
	    	    	sprintf(path, "%llu", (FKS_ULLONG)(num));
	    	    	convFmt_.write(path, &st);
	    	    }
			} else {	// 引数をそのまま 処理する場合.
				for (StrList::iterator ite = filenameList_.begin(); ite != filenameList_.end() && topN > 0; ++ite, --topN) {
					char const* path = ite->c_str();
	    	    	convFmt_.write(path, &st);
				}
			}
    	}

    	/* 直後出力テキスト */
    	for (StrList::iterator ite = afterStrList_.begin(); ite != afterStrList_.end(); ++ite) {
    	    convFmt_.writeLine0(ite->c_str());
    	}

    	if (opts_.batEx_)  /* バッチ実行用にファイル末に:ENDを付加する */
    	    convFmt_.writeLine0(":END");

    	return true;
    }

    bool outputText() {
    	/* 出力ファイル設定 */
    	if (!opts_.outname_.empty()) {
    	    outFp_ = fopenX(opts_.outname_.c_str(), "wt");
    	    if (!outFp_) {
    	    	return false;
    	    }
    	} else {
    	    outFp_ = stdout;
    	}

    	StrList const& outBuf = convFmt_.outBuf();
    	for (StrList::const_iterator ite = outBuf.begin(); ite != outBuf.end(); ++ite) {
    	    fprintf(outFp_, ite->c_str());
    	}

    	if (!opts_.outname_.empty()) {
    	    fclose(outFp_);
    	    outFp_ = NULL;
    	}
    	return true;
    }

    bool execBat() {
    	/* バッチ実行のとき */
    	if (opts_.batFlg_) {
    	 #ifdef ENABLE_MT_X
    	    if (opts_.nthread_) {
    	    	StrList&    	    	    buf = convFmt_.outBuf();
    	    	std::vector<std::string>    cmds(buf.size());
    	    	std::copy(buf.begin(), buf.end(), cmds.begin());
    	    	convFmt_.clearOutBuf();
    	    	mtCmd(cmds, opts_.nthread_-1);
    	    } else
    	 #endif
    	    {
    	     #if 1
    	    	system(opts_.outname_.c_str());
    	     #else
    	    	char* p = getenv("COMSPEC");
    	    	spawnl( _P_WAIT, p, p, "/c", opts_.outname_.c_str(), NULL);
    	     #endif
    	    }
    	}
    	return true;
    }


private:
    FILE*   	    outFp_;
    StrList 	    filenameList_;
    StrList 	    beforeStrList_;
    StrList 	    afterStrList_;
    FnameBuf	    abxName_;	    	/* 名前 work */
    ConvFmt 	    convFmt_;
    Opts    	    opts_;
    ResCfgFile	    resCfgFile_;
    StrzBuf<FMTSIZ> fmtBuf_;	    	/* 変換文字列を収める */
    StrzBuf<FIL_NMSZ> tmpFName_;
	Files			files_;
};

static App app;

/** start application
 */
#ifdef FKS_USE_LONGFNAME
int wmain(int argc, wchar_t *wargv[]) {
	char** argv = fks_convArgWcsToMbs(argc, wargv);
	fks_initMB();
    return app.main(argc, argv);
}
#else
int main(int argc, char *argv[]) {
    return app.main(argc, argv);
}
#endif
