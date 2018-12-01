/**
 *  @file   abx.cpp
 *  @brief  ファイル名を検索、該当ファイル名を文字列に埋込(バッチ生成)
 *  @author Masashi KITAMURA (tenka@6809.net)
 *  @date   1995-2018
 *	@license Boost Software License Version 1.0
 *  @note
 *	add -xm multi thread version by misakichi (https://github.com/misakichi)
 */

#include <fks_common.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "subr.hpp"
#include "StrzBuf.hpp"
#include "AbxConvFmt.hpp"
#include "AbxFiles.hpp"
#include "AbxMsgStr.hpp"

#ifdef ENABLE_MT_X
#include "abxmt.hpp"
#endif

#include <fks_dirent.h>
#include <fks_path.h>
#include <fks_io.h>
#include <fks_time.h>
#include <fks_misc.h>

#ifdef _WIN32
#include <windows.h>
#endif


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


/*--------------------------------------------------------------------------*/
class Opts {
public:
    ConvFmt&	    rConvFmt_;

    AbxFiles_Opts	filesOpts_;

    bool    	    recFlg_;	    	    /* 再帰の有無 */
    bool    	    zenFlg_;	    	    /* MS全角対応 */
    bool    	    batFlg_;	    	    /* バッチ実行 */
    bool    	    batEx_; 	    	    /* -bの有無 */
    bool    	    linInFlg_;	    	    /* RES入力を行単位処理 */
    bool    	    autoWqFlg_;
    bool    	    upLwrFlg_;
    int     	    noFindFile_;     	    /* ファイル検索しない */
    size_t  	    topN_;  	    	    /* 処理個数 */
    size_t  	    renbanStart_;   	    /* 連番の開始番号. 普通0 */
    size_t  	    renbanEnd_;     	    /* 連番の開始番号. 普通0 */
    FnameBuf	    outname_;	    	    /* 出力ファイル名 */
    FnameBuf	    chgPathDir_;
    FnameBuf	    exename_;
 #ifdef ENABLE_MT_X
    unsigned	    nthread_;
 #endif

public:
    Opts(ConvFmt& rConvFmt);
    bool scan(char* s);
	bool usage();

	void setExename(char const* exename) {
		exename_ = exename;
	 #ifdef _WIN32
		fks_pathToLower(&exename_[0]);
	 #endif
	}

private:
	fks_time_t parseDateTime(char* &p);
	fks_isize_t	parseSize(char* &p);
};

Opts::Opts(ConvFmt& rConvFmt)
	: rConvFmt_(rConvFmt)
	, zenFlg_(true)
	, batFlg_(false)
	, batEx_(false)
	, linInFlg_(false)
	, autoWqFlg_(false)
	, upLwrFlg_(false)
	, noFindFile_(0)
	, topN_(0)
	, renbanStart_(0)
	, renbanEnd_(0)
	, outname_()
	, chgPathDir_()
	, exename_()
   #ifdef ENABLE_MT_X
	, nthread_(0)
   #endif
{
}

bool Opts::scan(char* s) {
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
	    filesOpts_.recFlg_ = (*p != '-');
	    break;

	case 'U':
		if (strcmp(p-1, "utf8") == 0) {
			fks_ioMbsOutputInit(1);
		} else {
		    upLwrFlg_ = (*p != '-');
		}
	    break;

	case 'N':
	    noFindFile_ = (*p != '-');
	    if (*p == 'd' || *p == 'D')
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
	    	filesOpts_.knjChk_ = 0;
	    } else if (c == 'D') {
			rConvFmt_.setRelativeBaseDir(p + 1);
	    } else if (c == 'K') {
	    	filesOpts_.knjChk_ = 1;
	    	if (p[1] == '-')
	    	    filesOpts_.knjChk_ = -1;
	    } else if (c == 'Y') {
	    	filesOpts_.knjChk_ = 2;
	    	if (p[1] == '-')
	    	    filesOpts_.knjChk_ = -2;
	    } else if (c == 'T' /*|| c == 'F'*/) {  // 'F'は旧互換.
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
	    filesOpts_.dfltExt_ = p;
		filesOpts_.dfltExtp_ = filesOpts_.dfltExt_.c_str();
	    if (*p == '$' && p[1] >= '1' && p[1] <= '9' && p[2] == 0) {
	    	filesOpts_.dfltExt_ = rConvFmt_.getVar(p[1]-'0');
	    }
	    /*filesOpts_.dfltExt_[3] = 0;*/
	    break;

	case 'O':
	    if (*p == 0)
	    	goto ERR_OPTS;
	    outname_ = p;
	    break;

	case 'I':
	    if (*p == 0)
	    	goto ERR_OPTS;
		fks_fileFullpath(&filesOpts_.ipath_[0], filesOpts_.ipath_.capacity(), p);
		fks_pathDelLastSep(&filesOpts_.ipath_[0]);
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
	    	case 'D': filesOpts_.fattr_ |= FA_Dir;    	break;
	    	case 'N': filesOpts_.fattr_ |= FA_Norm;		break;
	    	case 'R': filesOpts_.fattr_ |= FA_RdOnly; 	break;
	    	case 'H': filesOpts_.fattr_ |= FA_Hidden; 	break;
	    	case 'S': filesOpts_.fattr_ |= FA_Sys;    	break;
	    	case 'A': filesOpts_.fattr_ |= FA_Arcive; 	break;
	    	//case 'V': filesOpts_.fattr_ |= FA_Volume; break;
	    	case 'C': filesOpts_.fattr_ |= FA_Comp; 	break;
	    	case 'E': filesOpts_.fattr_ |= FA_Encrypt; 	break;
	    	case 'V': filesOpts_.fattr_ |= FA_Virtual;	break;
	    	}
	    	++p;
	    }
	    break;

	case 'S':
	    c = 0;
	    filesOpts_.sortType_ = ST_NAME;
	    strupr(p);
	    while (*p) {
	    	switch(*p) {
	    	case '-': filesOpts_.sortType_ = ST_NONE; break;
	    	case 'N': filesOpts_.sortType_ = ST_NAME; break;
	    	case 'E': filesOpts_.sortType_ = ST_EXT;  break;
	    	case 'Z': filesOpts_.sortType_ = ST_SIZE; break;
	    	case 'T': filesOpts_.sortType_ = ST_DATE; break;
	    	case 'A': filesOpts_.sortType_ = ST_ATTR; break;
	    	case 'M': filesOpts_.sortType_ = ST_NUM;  break;
	    	case 'R': filesOpts_.sortRevFlg_ = true;  break;
	    	}
	    	++p;
	    }
	    break;

	case 'Z':
	    filesOpts_.szmin_ = (*p == '-') ? 0 : parseSize(p);
	    if (*p) { /* *p == '-' */
			filesOpts_.szmax_ = FKS_ISIZE_MAX;
	    	if (*++p)
				filesOpts_.szmax_ = parseSize(p);
			if (filesOpts_.szmax_ < filesOpts_.szmin_)
	    	    goto ERR_OPTS;
	    } else {
			filesOpts_.szmax_ = filesOpts_.szmin_;
	    }
	    break;

	case 'D':
	    if (*p == 0) {
	    	filesOpts_.dtmax_ = filesOpts_.dtmin_ = 0;
	    } else {
			if (*p == '-') {
				filesOpts_.dtmin_ = 0;
				++p;
			} else {
				filesOpts_.dtmin_ = parseDateTime(p);
				if (filesOpts_.dtmin_ < 0)
					goto ERR_OPTS;
				filesOpts_.dtmax_ = FKS_TIME_MAX;
				if (*p == '-')
					++p;
			}
	    	if (*p) {
				filesOpts_.dtmax_ = parseDateTime(p);
				if (filesOpts_.dtmax_ < 0)
					goto ERR_OPTS;
				if (filesOpts_.dtmax_ < filesOpts_.dtmin_)
    	    	    goto ERR_OPTS;
	    	}
	    }
	    break;

	case '?':
	case 'H':
	case '\0':
		return usage();

	default:
  ERR_OPTS:
	    //fprintf(stderr, "Incorrect command line option : %s\n", s);
	    fprintf(stderr, ABXMSG(incorrect_command_line_option), s);
	    return false;
	}
	return true;
}

bool Opts::usage()
{
	printf("%s", ABXMSG(usage));
	printf("%s", ABXMSG(usage_options));
	return false;
}

static bool isDateSep(int c)
{
	return (c == '-' || c == '/' || c == '.' || c == ':' || c == '_');
}

static bool isTimeSep(int c)
{
	return (c == ':' || c == ' ' || c == '.' || c == '/' || c == '_');
}

fks_time_t Opts::parseDateTime(char* &p)
{
	unsigned y = 0, m = 0, d = 0, h = 0, min=0, sec=0, milli=0;
	y = strtoul(p, &p, 10);
	if (y < 10000 && isDateSep(*p)) {
		m = strtoul(p+1, &p, 10);
		if (isDateSep(*p) && isdigit(p[1])) {
			d = strtoul(p+1, &p, 10);
		}
	} else if (y >= 10000) {
		unsigned t = y;
	    y = (int)(t / 10000); y = (y < 70) ? 2000 + y : (y < 100) ? y + 1900 : y;
		m = (int)((t / 100) % 100); if (m == 0 || 12 < m) return -1;
		d = (int)(t % 100);     	if (d == 0 || 31 < d) return -1;
	}
	if (isTimeSep(*p) && isdigit(p[1])) {
		h = strtoul(p + 1, &p, 10);
		if (isTimeSep(*p) && isdigit(p[1])) {
			h = strtoul(p + 1, &p, 10);
			if (isTimeSep(*p) && isdigit(p[1])) {
				min = strtoul(p + 1, &p, 10);
				if (isTimeSep(*p) && isdigit(p[1])) {
					sec = strtoul(p + 1, &p, 10);
					if (isTimeSep(*p) && isdigit(p[1]))
						milli = strtoul(p + 1, &p, 10);
				}
			}
		}
	}

	Fks_DateTime dt = {y,m,0,d,h,min,sec,milli,0,0};
	return fks_localDateTimeToFileTime(&dt);
}

fks_isize_t	Opts::parseSize(char* &p)
{
	uint64_t	sz = uint64_t( strtoull(p, &p, 0) );
    if (*p == 'K' || *p == 'k')     	p++, sz *= 1024;
    else if (*p == 'M' || *p == 'm')	p++, sz *= 1024*1024;
    else if (*p == 'G' || *p == 'g')	p++, sz *= 1024*1024*1024;
    else if (*p == 'T' || *p == 't')	p++, sz *= FKS_LLONG_C(1024)*1024*1024*1024;
	return (fks_isize_t)sz;
}


// ----------------------------------------------------------------------------

class ResCfgFile {
public:
	ResCfgFile(Opts& rOpts, ConvFmt& rConvFmt, StrList& rFileNameList, StrList& rBeforeList, StrList& rAfterList, char* fmtBuf);
    bool GetResFile(char const* name);
    bool GetCfgFile(char *name, char *key);

private:
    char *getLine(void);
    char *setDoll(char *p0);
    char const* getFileNameStr(char *d, char const* s);
    bool getFmts();
    bool keyStrEqu(char *key, char *lin);

private:
    Opts&   	    	rOpts_;
    ConvFmt&	    	rConvFmt_;
    StrList&	    	rFileNameList_;
    StrList&	    	rBeforeStrList_;
    StrList&	    	rAfterStrList_;
    char*   	    	fmtBuf_;
    char*   	    	resP_;
    int     	    	varIdx_;
    int     	    	varNo_[10 + 1];
    FnameBuf	    	resName_;
    StrzBuf<OBUFSIZ>	resOBuf_;
};


ResCfgFile::ResCfgFile(Opts& rOpts, ConvFmt& rConvFmt, StrList& rFileNameList, StrList& rBeforeList, StrList& rAfterList, char* fmtBuf)
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
bool ResCfgFile::GetResFile(char const* name) {
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
	    	//fprintf(stderr, "File read error : %s\n", name);
			fprintf(stderr, ABXMSG(file_read_error), name);
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
bool ResCfgFile::GetCfgFile(char *name, char *key) {
	fks_fileFullpath(&resName_[0], resName_.capacity(), name);
	FILE*	fp = fopenX(resName_.c_str(),"r");
	if (!fp) {
		return false;
	}
	size_t	l  = fread(&resOBuf_[0], 1, resOBuf_.capacity(), fp);
    if (ferror(fp)) {
    	//fprintf(stderr, "Cfg-file read error : %s\n", name);
		fprintf(stderr, ABXMSG(cfg_read_error), name);
    	return false;
    }
	fclose(fp);

	resOBuf_[l] = 0;
	if (l == 0)
	    return false;

	if (key[1] == 0) /* ':'だけの指定のとき */
	    printf(ABXMSG(translation_names_list));

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
		fprintf(stderr, ABXMSG(key_is_not_defined), key, resName_.c_str());
	}
	return false;
}

//private:
/** resOBuf_に貯えたテキストより１行入力.
 * 行末の改行は削除. resOBuf_は破壊.
 */
char *ResCfgFile::getLine(void) {
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

/** $数字 変数の設定.
 */
char *ResCfgFile::setDoll(char *p0) {
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
	    //fprintf(stderr, ".cfg-file has an incorrect $N specification. : $%s\n", p0);
	    fprintf(stderr, ABXMSG(cfgfile_has_an_incorrect_dollN_specification), p0);

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
		//fprintf(stderr, "$N=STRING or $N:M{..} specification in .cfg-file is incorrect. : $%s\n", p0);
		fprintf(stderr, ABXMSG(dollN_specification_in_cfgfile_is_incorrect), p0);
	    exit(1);
	}
  RET:
	return p;
}

/** s より空白で区切られた単語(ファイル名)をname にコピーする.
 * ただし "file name" のように"があれば"を削除し替りに間の空白を残す.
 */
char const* ResCfgFile::getFileNameStr(char *d, char const* s) {
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
bool ResCfgFile::getFmts() {
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
	    	    	    fprintf(stderr, ABXMSG(single_quotation_string_is_broken));
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


bool ResCfgFile::keyStrEqu(char *key, char *lin) {
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
	    	    	    //fprintf(stderr, "There are ten or more {..} in a certain line in %s : %s\n", resName_.c_str(),lin);
	    	    	    fprintf(stderr, ABXMSG(there_are_more_par_pair_in_a_certain_line), resName_.c_str(),lin);
	    	    	    exit(1);
	    	    	}
	    	    	rConvFmt_.setVar(varIdx_, f, l);
	    	    	++varIdx_;
	    	    	k += l;
	    	    	f = strchr(f,'}');
	    	    	if (f == NULL) {
	    	  ERR1:
	    	    	    //fprintf(stderr, "%s で{..}の指定がおかしい %s\n",resName_.c_str(), lin);
	    	    	    //fprintf(stderr, "Invalid {..} designation in %s : %s\n",resName_.c_str(), lin);
	    	    	    fprintf(stderr, ABXMSG(invalid_par_pair),resName_.c_str(), lin);
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



// ----------------------------------------------------------------------------

class App {
public:
	App();

	~App() {
		// メモリー開放(free)等は os に任せれたほうが無難なので、あえてしない.
	}

    int main(int argc, char *argv[]);

private:
    bool scanOpts(int argc, char *argv[]);
    bool genText();
    bool outputText();
    bool execBat();

private:
    FILE*   	    	outFp_;
    StrList 	    	filenameList_;
    StrList 	    	beforeStrList_;
    StrList 	    	afterStrList_;
    FnameBuf	    	abxName_;	    	/* 名前 work */
    ConvFmt 	    	convFmt_;
    Opts    	    	opts_;
    ResCfgFile	    	resCfgFile_;
    StrzBuf<FMTSIZ> 	fmtBuf_;	    	/* 変換文字列を収める */
    StrzBuf<FIL_NMSZ>	tmpFName_;
	AbxFiles			files_;
};

App::App()
	: outFp_(NULL)
	, filenameList_()
	, beforeStrList_()
	, afterStrList_()
	, abxName_()
	, opts_(convFmt_)
	, resCfgFile_(opts_, convFmt_, filenameList_, beforeStrList_, afterStrList_, &fmtBuf_[0])
	, fmtBuf_()
	, files_()
{
}

int App::main(int argc, char *argv[]) {
	opts_.setExename(fks_pathBaseName(argv[0]));
	if (argc < 2) {
	    opts_.usage();
	    return 1;
	}

	fks_pathSetExt(&abxName_[0], abxName_.capacity(), argv[0], "cfg");
	if (scanOpts(argc, argv) == false)
	    return 1;

	if (!opts_.noFindFile_ && !opts_.renbanEnd_) {	// ファイル検索時.
		if (files_.getPathStats(filenameList_, opts_.filesOpts_) == false)
			return 1;
	} else {	// 生の文字列のみのとき.
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

/** コマンドラインのオプション/ファイル名/変換文字列, 取得.
 */
bool App::scanOpts(int argc, char *argv[]) {
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
	    	    //fprintf(stderr, ":\"#STRING\" can not be specified. : %s\n",p);
	    	    fprintf(stderr, ABXMSG(colon_string_can_not_be_specified), p);
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
	    fprintf(stderr, ABXMSG(xm_and_Hbegin_can_not_be_used_at_the_same_time));
	    return false;
	}
   #endif

	/* バッチ実行のとき */
	if (opts_.batFlg_) {
	    fks_tmpFile(&tmpFName_[0], FIL_NMSZ, "abx_", ".bat");
        //printf("tmpfname=%s\n", &tmpFName_[0]);
	    opts_.outname_  = tmpFName_;
	}

	if (opts_.filesOpts_.fattr_ == 0) {     /* デフォルトのファイル検索属性 */
		opts_.filesOpts_.fattr_ = FA_MASK_FILEYTPE;
	}
	convFmt_.setUpLwrFlag(opts_.upLwrFlg_);

	/* 変換文字列調整 */
	if (fmtBuf_.empty()) {
	    if (opts_.filesOpts_.recFlg_)
	    	fmtBuf_ = "$f\n";
	    else
	    	fmtBuf_ = "$c\n";
	}
	if (strchr(fmtBuf_.c_str(), '\n') == NULL)
	    fmtBuf_ += "\n";

	convFmt_.setFmtStr(fmtBuf_.c_str());

	convFmt_.setAutoWq(opts_.autoWqFlg_);	    /* $f等で自動で両端に"を付加するモード. */

	return true;
}


bool App::genText() {
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
    	    	sprintf(path, "%" PRIF_LL "u", (PRIF_ULLONG)(num));
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

bool App::outputText() {
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

bool App::execBat() {
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


/** start application
 */
#if defined(FKS_USE_LONGFNAME) && defined(FKS_HAS_WMAIN)
int wmain(int argc, wchar_t *wargv[]) {
	static App app;
	char** argv = fks_convArgWcsToMbs(argc, wargv);
	fks_ioMbsInit(1,0);
    return app.main(argc, argv);
}
#else
int main(int argc, char *argv[]) {
	static App app;
    return app.main(argc, argv);
}
#endif
