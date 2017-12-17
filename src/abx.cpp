/**
 *  @file   abx.cpp
 *  @brief  ファイル名を検索、該当ファイル名を文字列に埋込(バッチ生成)
 *  @author Masashi KITAMURA (tenka@6809.net)
 *  @date   1995-2017
 *  @note
 *      license
 *          Boost Software License Version 1.0
 *          see license.txt
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <process.h>
#include <algorithm>
#include <string>
#include <list>
#include <set>
//#define __STDC_LIMIT_MACROS
//#include <inttypes.h>
#ifdef _WIN32
 #include <windows.h>
#endif

#include "subr.hpp"
#include "StrzBuf.hpp"

#ifdef ENABLE_MT_X
#include "abxmt.h"
#endif



/*---------------------------------------------------------------------------*/

#define APP_HELP_TITLE      "abx v3.91(pre v4) ﾌｧｲﾙ名を検索,該当ﾌｧｲﾙ名を文字列に埋込(ﾊﾞｯﾁ生成).\n"	\
							"  https://github.com/tenk-a/abx.git      Boost Software License Version 1.0\n"
#define APP_HELP_CMDLINE    "usage : %s [ｵﾌﾟｼｮﾝ] ['変換文字列'] ﾌｧｲﾙ名 [=変換文字列]\n"
#define APP_HELP_OPTS       "ｵﾌﾟｼｮﾝ:                        ""変換文字:            変換例:\n"                       \
                            " -x[-]    ﾊﾞｯﾁ実行 -x-しない   "" $f ﾌﾙﾊﾟｽ(拡張子付)   d:\\dir\\dir2\\filename.ext\n"  \
                            " -xm[N]   Nスレッド実行.0自動  "" $g ﾌﾙﾊﾟｽ(拡張子無)   d:\\dir\\dir2\\filename\n"      \
                            " -r[-]    ﾃﾞｨﾚｸﾄﾘ再帰          "" $v ﾄﾞﾗｲﾌﾞ            d\n"                            \
                            " -a[nrhsd] 指定ﾌｧｲﾙ属性で検索  "" $p ﾃﾞｨﾚｸﾄﾘ(ﾄﾞﾗｲﾌﾞ付) d:\\dir\\dir2\n"                \
                            "          n:一般 s:ｼｽﾃﾑ h:隠し "" $d ﾃﾞｨﾚｸﾄﾘ(ﾄﾞﾗｲﾌﾞ無) \\dir\\dir2\n"                  \
                            "          r:ﾘｰﾄﾞｵﾝﾘｰ d:ﾃﾞｨﾚｸﾄﾘ "" $c ﾌｧｲﾙ(拡張子付)    filename.ext\n"                 \
                            " -z[N-M]  ｻｲｽﾞN～MのFILEを検索 "" $x ﾌｧｲﾙ(拡張子無)    filename\n"                     \
                            " -d[A-B]  日付A～BのFILEを検索 "" $e 拡張子            ext\n"                          \
                            " -s[neztam][r] ｿｰﾄ(整列)       "" $w ﾃﾝﾎﾟﾗﾘ･ﾃﾞｨﾚｸﾄﾘ    (環境変数TMPの内容)\n"          \
                            "          n:名 e:拡張子 z:ｻｲｽﾞ "" $z ｻｲｽﾞ(10進10桁)    1234567890 ※$Zなら16進8桁\n"   \
                            "          t:日付 a:属性 r:降順 "" $j 時間              1993-02-14\n"                   \
                            "          m:名(数)             "" $i 連番生成          ※$Iなら16進数\n"               \
                            " -n[-]    ﾌｧｲﾙ検索しない -n-有 "" $$ $  $[ <  $` '  $n 改行  $t ﾀﾌﾞ\n"                 \
                            " -u[-]    $c|$Cでﾌｧｲﾙ名大小文字"" $# #  $] >  $^ \"  $s 空白  $l 生入力のまま\n"       \
                            " -l[-]    @入力で名前は行単位  ""------------------------------------------------\n"   \
                            " -ci[N:M] N:$iの開始番号(M:終) ""-p<DIR>  $pの強制変更   ""-ct<FILE> FILEより新なら\n" \
                            " +CFGFILE .CFGﾌｧｲﾙ指定         ""-e<EXT>  ﾃﾞﾌｫﾙﾄ拡張子   ""-ck[-] 日本語名のみ検索\n"  \
                            " @RESFILE ﾚｽﾎﾟﾝｽﾌｧｲﾙ           ""-o<FILE> 出力ﾌｧｲﾙ指定   ""-cy[-] \\を含む全角名検索\n"\
                            " :変換名  CFGで定義した変換    ""-i<DIR>  検索ﾃﾞｨﾚｸﾄﾘ    ""-y     $cxfgdpwに\"付加\n"  \
                            " :        変換名一覧を表示     ""-w<DIR>  TMPﾃﾞｨﾚｸﾄﾘ     ""-t[N]  最初のN個のみ処理\n" \


enum { OBUFSIZ  = 0x80000 };    /* 定義ファイル等のサイズ               */
enum { FMTSIZ   = 0x80000 };    /* 定義ファイル等のサイズ               */


typedef std::list<std::string>  StrList;
typedef StrzBuf<FIL_NMSZ>       FnameBuf;



/*--------------------- エラー処理付きの標準関数 ---------------------------*/

/** exit終了する printf
 */
volatile void err_printfE(char const* fmt, ...) {
    va_list app;
    va_start(app, fmt);
    /*  fprintf(stdout, "%s %5d : ", src_name, src_line);*/
    vfprintf(stderr, fmt, app);
    va_end(app);
    exit(1);
}


/** エラーがあれば即exitの fopen()
 */
FILE *fopenE(char const* name, char const* mod) {
    FILE *fp = fopen(name,mod);
    if (fp == NULL) {
        err_printfE("ファイル %s をオープンできません\n", name);
    }
    setvbuf(fp, NULL, _IOFBF, 1024*1024);
    return fp;
}

#if 0
/** エラーがあれば即exitの fwrite()
 */
size_t  fwriteE(void const* buf, size_t sz, size_t num, FILE *fp) {
    size_t l = fwrite(buf, sz, num, fp);
    if (ferror(fp)) {
        err_printfE("ファイル書込みでエラー発生\n");
    }
    return l;
}
#endif

/** エラーがあれば即exitの fread()
 */
size_t  freadE(void* buf, size_t sz, size_t num, FILE *fp) {
    size_t l = fread(buf, sz, num, fp);
    if (ferror(fp)) {
        err_printfE("ファイル読込みでエラー発生\n");
    }
    return l;
}


/*---------------------------------------------------------------------------*/

class ConvFmt;

enum SortType {
    ST_NONE = 0x00,
    ST_NAME = 0x01,     // 名前でソート.
    ST_EXT  = 0x02,     // 拡張子.
    ST_SIZE = 0x04,     // サイズ.
    ST_DATE = 0x08,     // 日付/時間.
    ST_ATTR = 0x10,     // ファイル属性.
    ST_NUM  = 0x20,     // 数値比較の名前.
    //ST_MASK = 0x7F,   // ソート情報マスク
};

enum FileAttr {
    FA_Norm   = 0x100,
    FA_RdOnly = 0x001,
    FA_Hidden = 0x002,
    FA_Sys    = 0x004,
    FA_Volume = 0x008,
    FA_Dir    = 0x010,
    FA_Arcive = 0x020,
    FA_MASK   = 0x03f,
    FA_MASK_NOARC = 0xDF/*0x1f*/,
};

class FSrh {
public:
    FSrh()
        : recFlg_(false)
        , normalFlg_(true)
        , topFlg_(false)
        , nonFileFind_(false)
        , zenFlg_(false)
        , sortRevFlg_(false)
        , uplwFlg_(false)
        , sortType_(ST_NONE)
        , fattr_(FA_MASK)
        , knjChk_(0)
        , topN_(0)
        , topCnt_(0)
        , szMin_(0)
        , szMax_(0)
        , dateMin_(0)
        , dateMax_(0)
        , outFp_(NULL)
        , convFmt_(NULL)
        , membFunc_(NULL)
        , fpath_()
        , fname_()
    {
    }

    FSrh( unsigned atr, bool recFlg, bool zenFlg,
             size_t topN, SortType sortType, bool sortRevFlg, bool upLwFlg, int knjChk,
             size_t szmin, size_t szmax,
             unsigned short dtmin, unsigned short dtmax,
             FILE* outFp, ConvFmt* pConvFmt,
             int (ConvFmt::*fun)(char const* apath, FIL_FIND const* aff))
        : recFlg_(recFlg)
        , normalFlg_(false)
        , topFlg_(topN != 0)
        , nonFileFind_(false)
        , zenFlg_(zenFlg)
        , sortRevFlg_(sortRevFlg)
        , uplwFlg_(upLwFlg)
        , sortType_(sortType)
        //, fattr_(FA_MASK)
        , knjChk_(knjChk)
        , topN_(topN)
        , topCnt_(0)
        , szMin_(szmin)
        , szMax_(szmax)
        , dateMin_(dtmin)
        , dateMax_(dtmax)
        , outFp_(outFp)
        , convFmt_(pConvFmt)
        , membFunc_(fun)
        , fpath_()
        , fname_()
    {
        if (atr & FA_Norm) {
            normalFlg_ = true;
        }
        atr &= FA_MASK/*0xff*/;
        fattr_ = atr;
        //FIL_SetZenMode(zenFlg);
    }

    int findAndDo(char const* path, bool nonFileFind)
    {
        nonFileFind_ = nonFileFind;
        FIL_SetZenMode(zenFlg_);
        /*printf("%lu(%lx)-%lu(%lx)\n",szmin,szmin,szmax,szmax);*/
        /*printf("date %04x-%04x\n",dtmin,dtmax);*/
        FIL_FullPath(path, &fpath_[0]);
        char *p = STREND(&fpath_[0]);
        if (p[-1] == ':' || p[-1] == '\\' || p[-1] == '/')
            fpath_ += "*";
        p = FIL_BaseName(fpath_.c_str());
        fname_ = p;
        if (nonFileFind_) {   /* ファイル検索しない場合 */
            FIL_FIND ff;
            memset(&ff, 0, sizeof ff);
            if (   (knjChk_==0)
                || (knjChk_==1  && chkKnjs(fname_.c_str()))
                || (knjChk_==2  && strchr(fname_.c_str(),'\\'))
                || (knjChk_==-1 && !chkKnjs(fname_.c_str()))
                || (knjChk_==-2 && !strchr(fname_.c_str(),'\\')) )
            {
                (convFmt_->*membFunc_)(fpath_.c_str(), &ff);
            }
            return 0;
        }
        /* ファイル検索する場合 */
        *p = 0;
        if (sortType_ != ST_NONE) {  /* ソートする */
            return findAndDo_subSort();
        }
        return findAndDo_sub();
    }

private:
    static int chkKnjs(const char *p)
    {
        unsigned char c;
        while((c = *(unsigned char *)p++) != 0) {
            if (c & 0x80)
                return 1;
        }
        return 0;
    }


    struct FileFindNameCmp {
        bool operator()(FIL_FIND const& l, FIL_FIND const& r) const {
         #ifdef _WIN32
            return _stricmp(l.name, r.name) < 0;
         #else
            return strcmp(l.name, r.name) < 0;
         #endif
        }

    private:
        FSrh const*     pFSrh_;
    };
    typedef std::set<FIL_FIND, FileFindNameCmp> FileFindDirTree;


   #ifdef _WIN32
    #define FNAME_GET_C(c, p) do {                              \
            (c) = *(unsigned char*)((p)++);                     \
            if (IsDBCSLeadByte(BYTE(c)) && *(p))                \
                (c) = ((c) << 8) | *(unsigned char*)((p)++);    \
        } while (0)
   #else
    #define FNAME_GET_C(c, p)   ((c) = *((p)++))
   #endif

    static int fnameCmp(char const* l, char const* r) {
        #ifdef _WIN32
            int rc = _stricmp(l,r);
            if (rc != 0)
                return rc;
            return strcmp(l,r);
        #else
            return strcmp(l,r);
        #endif
    }

    /** ファイル名の大小比較. 数値があった場合、桁数違いの数値同士の大小を反映
    *   大小同一視. ディレクトリセパレータ \ / も同一視.
    *   以外は単純に文字列比較.
    */
    static int fnameNDigitCmp(const char* l, const char* r, size_t len) {
        const char* e = l + len;
        if (e < l)
            e = (const char*)-1;
        while (l < e) {
            unsigned    lc;
            unsigned    rc;

            FNAME_GET_C(lc, l);
            FNAME_GET_C(rc, r);

            if (lc <= 0x80 && isdigit(lc) && rc <= 0x80 && isdigit(rc)) {
                uint64_t   lv = uint64_t(strtoull(l - 1, (char**)&l, 10));
                uint64_t   rv = uint64_t(strtoull(r - 1, (char**)&r, 10));
                int64_t	   n  = int64_t(lv - rv);
                if (n == 0)
                    continue;
                return (n < 0) ? -1 : 1;
            }

            if (lc < 0x80)
                lc = tolower(lc);
            if (rc < 0x80)
                rc = tolower(rc);

            int64_t n  = int64_t(lc - rc);
            if (n == 0) {
                if (lc == 0)
                    return 0;
                continue;
            }

            if ((lc == '/' && rc == '\\') || (lc == '\\' && rc == '/')) {
                continue;
            }

            return (n < 0) ? -1 : 1;
        }
        return 0;
    }
    #undef FNAME_GET_C

    int  fileStatCmp(FIL_FIND const* f1, FIL_FIND const* f2) const {
        int n = 0;

        if (sortType_ == ST_NUM) {                      /* 数字部分は数値で比較する名前ソート */
            n = fnameNDigitCmp(f1->name, f2->name, (size_t)-1);
            if (sortRevFlg_)
                return -n;
            return n;
        }
        if (sortType_ <= ST_NAME) {                        /* 名前でソート */
            n = fnameCmp(f1->name, f2->name);
            if (sortRevFlg_)
                return -n;
            return n;
        }

        if (sortType_ == ST_EXT) {                     /* 拡張子 */
            char const* p = strrchr(f1->name, '.');
            p = (p == NULL) ? "" : p;
            char const* q = strrchr(f2->name, '.');
            q = (q == NULL) ? "" : q;
            n = fnameCmp(p,q);

        } else if (sortType_ == ST_SIZE) {              /* サイズ */
            long t = f1->size - f2->size;
            n = (t > 0) ? 1 : (t < 0) ? -1 : 0;

        } else if (sortType_ == ST_DATE) {              /* 時間 */
            int64_t t = f1->time_write - f2->time_write;
            n = (t > 0) ? 1 : (t < 0) ? -1 : 0;
        } else if (sortType_ == ST_ATTR) {              /* 属性 */
            /* アーカイブ属性は邪魔なのでオフする */
            n = ((int)f2->attrib & FA_MASK_NOARC) - ((int)f1->attrib & FA_MASK_NOARC);
        }

        if (n == 0) {
            n = fnameCmp(f1->name, f2->name);
            //if (sortRevFlg_)
            //    n = -n;
        }
        if (sortRevFlg_)
            return -n;
        return n;
    }

    struct FileFindCmp {
        FileFindCmp(FSrh const* pFSrh=NULL) : pFSrh_(pFSrh) {}

        bool operator()(FIL_FIND const& l, FIL_FIND const& r) const {
            return pFSrh_->fileStatCmp(&l, &r) < 0;
        }

    private:
        FSrh const*     pFSrh_;
    };
    typedef std::set<FIL_FIND, FileFindCmp>     FileFindTree;

    struct DoOne {
        DoOne(FSrh* pFSrh) : pFSrh_(pFSrh) {}

        void operator()(FIL_FIND const& ff) {
            if (pFSrh_->topFlg_) {
                if (pFSrh_->topCnt_ == 0)       /* 先頭 N個のみの処理のとき */
                    return;
                else
                    --pFSrh_->topCnt_;
            }
            char *t = STREND(&pFSrh_->fpath_[0]);
            strcpy(t, ff.name);
            (pFSrh_->convFmt_->*(pFSrh_->membFunc_))(pFSrh_->fpath_.c_str(), &ff);
            *t = 0;
        }

    private:
        FSrh*       pFSrh_;
    };

    struct DoOneDir {
        DoOneDir(FSrh* pFSrh) : pFSrh_(pFSrh) {}

        void operator()(FIL_FIND const& ff) {
            char *t = STREND(&pFSrh_->fpath_[0]);
            strcpy(t, ff.name);
            strcat(t, "\\");
            pFSrh_->findAndDo_subSort();
            *t = 0;
        }

    private:
        FSrh*     pFSrh_;
    };


    int findAndDo_subSort() {
        FIL_FIND_HANDLE hdl = 0;
        FIL_FIND        ff = {0};
        char*           t;

        if (topFlg_) {
            topCnt_ = topN_;
        }

        {
            FileFindTree    tree(FileFindCmp(this));
            t    = STREND(&fpath_[0]);
            strcpy(t, fname_.c_str());
            hdl = FIL_FINDFIRST(fpath_.c_str(), fattr_, &ff);
            if (FIL_FIND_HANDLE_OK(hdl)) {
                do {
                    *t = '\0';
                    if (normalFlg_ == 0 && (fattr_ & ff.attrib) == 0)
                        continue;
                    if ((fattr_ & FA_Dir) == 0 && (ff.attrib & FA_Dir))   /* ディレクトリ検索でないのにディレクトリがあったら飛ばす */
                        continue;
                    if(  (ff.name[0] != '.')
                      && (  (szMin_ > szMax_) || ((int)szMin_ <= ff.size && ff.size <= (int)szMax_) )
                      && (  (dateMin_ > dateMax_) || (dateMin_ <= ff.wr_date && ff.wr_date <= dateMax_) )
                      && (  (knjChk_==0) || (knjChk_==1 && chkKnjs(ff.name)) || (knjChk_==2 && strchr(ff.name,'\\'))
                                             || (knjChk_==-1&& !chkKnjs(ff.name))|| (knjChk_==-2&& !strchr(ff.name,'\\'))  )
                      )
                    {
                        tree.insert(ff);
                    }
                } while (FIL_FINDNEXT(hdl, &ff) == 0);
                FIL_FINDCLOSE(hdl);
            }
            std::for_each(tree.begin(), tree.end(), DoOne(this));
        }

        if (recFlg_ /*&& nonFileFind_ == 0*/) {
            FileFindDirTree     dirTree;
            strcpy(t,"*.*");
            hdl = FIL_FINDFIRST(fpath_.c_str(), FA_Dir, &ff);
            if (FIL_FIND_HANDLE_OK(hdl)) {
                do {
                    *t = '\0';
                    if ((ff.attrib & FA_Dir) && strcmp(ff.name, ".") && strcmp(ff.name, "..")) {
                        dirTree.insert(ff);
                    }
                } while (FIL_FINDNEXT(hdl, &ff) == 0);
                FIL_FINDCLOSE(hdl);
            }
            std::for_each(dirTree.begin(), dirTree.end(), DoOneDir(this));
        }
        return 0;
    }



    int findAndDo_sub() {
        FIL_FIND_HANDLE hdl;
        FIL_FIND        ff = {0};

        if (topFlg_) {
            topCnt_ = topN_;
        }
        char *t = STREND(&fpath_[0]);
        strcpy(t, fname_.c_str());
        hdl = FIL_FINDFIRST(fpath_.c_str(), fattr_, &ff);
        if (FIL_FIND_HANDLE_OK(hdl)) {
            do {
                *t = '\0';
                if (normalFlg_ == 0 && (fattr_ & ff.attrib) == 0)
                    continue;
                if ((fattr_ & FA_Dir) == 0 && (ff.attrib & FA_Dir))   /* ディレクトリ検索でないのにディレクトリがあったら飛ばす */
                    continue;
                if(  (ff.name[0] != '.')
                  && (  (szMin_ > szMax_) || ((int)szMin_ <= ff.size && ff.size <= (int)szMax_) )
                  && (  (dateMin_ > dateMax_) || (dateMin_ <= ff.wr_date && ff.wr_date <= dateMax_) )
                  && (  (knjChk_==0) || (knjChk_==1 && chkKnjs(ff.name)) || (knjChk_==2 && strchr(ff.name,'\\'))
                                         || (knjChk_==-1&& !chkKnjs(ff.name))|| (knjChk_==-2&& !strchr(ff.name,'\\'))  )
                  )
                {
                    strcpy(t, ff.name);
                    (convFmt_->*membFunc_)(fpath_.c_str(), &ff);
                    *t = 0;
                    if (topFlg_ && --topCnt_ == 0) {    /* 先頭 N個のみの処理のとき */
                        return 0;
                    }
                }
            } while (FIL_FINDNEXT(hdl, &ff) == 0);
            FIL_FINDCLOSE(hdl);
        }

        if (recFlg_) {
            strcpy(t,"*.*");
            hdl = FIL_FINDFIRST(fpath_.c_str(), 0x10, &ff);
            if (FIL_FIND_HANDLE_OK(hdl)) {
                do {
                    *t = '\0';
                    if ((ff.attrib & FA_Dir) && ff.name[0] != '.') {
                        strcpy(t, ff.name);
                        strcat(t, "\\");
                        findAndDo_sub();
                    }
                } while (FIL_FINDNEXT(hdl, &ff) == 0);
                FIL_FINDCLOSE(hdl);
            }
        }
        return 0;
    }


private:
    bool            recFlg_;        // 1:再帰する 0:しない
    bool            normalFlg_;     // ﾉｰﾏﾙ･ﾌｧｲﾙにﾏｯﾁ 1:する 0:しない
    bool            topFlg_;
    bool            nonFileFind_;   // 1:ファイル検索しない 0:する
    bool            zenFlg_;
    bool            sortRevFlg_;
    bool            uplwFlg_;
    SortType        sortType_;
    unsigned        fattr_;         // 検索ﾌｧｲﾙ属性
    int             knjChk_;
    size_t          topN_;
    size_t          topCnt_;
    size_t          szMin_;
    size_t          szMax_;
    unsigned short  dateMin_;
    unsigned short  dateMax_;
    FILE*           outFp_;
    ConvFmt*        convFmt_;
    int  (ConvFmt::*membFunc_)(char const* path, FIL_FIND const* ff);
    FnameBuf        fpath_;
    FnameBuf        fname_;
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
        FIL_GetTmpDir(&tmpDir_[0]); /* テンポラリディレクトリ名取得 */
    }

    void setChgPathDir(char const* dir) {
        FIL_FullPath(dir, &chgPathDir_[0]);
        char* p = STREND(&chgPathDir_[0]);
        if (p[-1] == '\\' || p[-1] == '/') {
            p[-1] = '\0';
        }
    }

    char const* tmpDir() const { return tmpDir_.c_str(); }

    void setTmpDir(char const* dir) {
        tmpDir_ = dir;
        FIL_GetTmpDir(&tmpDir_[0]); /* テンポラリディレクトリ名取得 */
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

    void setNum(size_t num) { num_ = num; }

    void setUpLwrFlag(bool sw) { upLwrFlg_ = sw; }

    void setAutoWq(bool sw) { autoWqFlg_ = sw; }

    void setLineBuf(char const* lineBuf) { lineBuf_ = lineBuf; }

    void setFmtStr(char const* fmtBuf) {
        fmtBuf_ = fmtBuf;
    }


    int write(char const* fpath, FIL_FIND const* ff) {
        splitPath(fpath);

        StrFmt(&tgtnm_[0], tgtnmFmt_.c_str(), tgtnm_.capacity(), ff);           // 今回のターゲット名を設定
        if (tgtnmFmt_.empty() || FIL_FdateCmp(tgtnm_.c_str(), fpath) < 0) { 	// 日付比較しないか、する場合はターゲットが古ければ
            StrFmt(&obuf_[0], &fmtBuf_[0], obuf_.capacity(), ff);
		 #if 1
			outBuf_.push_back(obuf_.c_str());
		 #else
            fprintf(fp, "%s", obuf_.c_str());
         #endif
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
                    fprintf(stderr,"Incorrect '$' format : '$%c'\n",c);
                    /*fprintfE(stderr,"レスポンス中の $指定がおかしい(%c)\n",c);*/
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

	StrList&		outBuf() { return outBuf_; }
	StrList const&	outBuf() const { return outBuf_; }
	void			clearOutBuf() { StrList().swap(outBuf_); }

private:
    char *stpCpy(char *d, char const* s, ptrdiff_t clm, int flg) {
        unsigned char c;
        size_t        n = 0;
        if (upLwrFlg_ == 0) {
            strcpy(d,s);
            n = strlen(s);
            d = d + n;
        } else if (flg == 0) {  /* 大文字化 */
            while ((c = *(unsigned char *)s++) != '\0') {
                if (islower(c))
                    c = (unsigned char)toupper(c);
                *d++ = (char)c;
                n++;
                if (ISKANJI(c) && *s && FIL_GetZenMode()) {
                    *d++ = *s++;
                    n++;
                }
            }
        } else {        /* 小文字化 */
            while ((c = *(unsigned char *)s++) != '\0') {
                if (isupper(c))
                    c = (unsigned char)tolower(c);
                *d++ = (char)c;
                n++;
                if (ISKANJI(c) && *s && FIL_GetZenMode()) {
                    *d++ = *s++;
                    n++;
                }
            }
        }
        clm -= (ptrdiff_t)n;
        while (clm > 0) {
            *d++ = ' ';
            --clm;
        }
        *d = '\0';
        return d;
    }

    void splitPath(char const* fpath) {
        FIL_SplitPath(&fpath[0], &drv_[0], &dir_[0], &name_[0], &ext_[0]);

        FIL_DelLastDirSep(&dir_[0]);  /* ディレクトリ名の後ろの'\'をはずす */
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


    void StrFmt(char *dst, char const* fmt, int sz, FIL_FIND const* ff) {
        char    buf[FIL_NMSZ*4] = {0};
        char    *b;
        int     f,n;
        char    drv[2];
        drv[0] = drv_[0];
        drv[1] = 0;

        char const* s = fmt;
        char*       p = dst;
        char*       pe = p + sz;
        char        c;
        while ((c = (*p++ = *s++)) != '\0' && p < pe) {
            if (c == '$') {
                --p;
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
                f = islower(c);
                switch (toupper(c)) {
                case 'S':   *p++ = ' ';     break;
                case 'T':   *p++ = '\t';    break;
                case 'N':   *p++ = '\n';    break;
                case '$':   *p++ = '$';     break;
                case '#':   *p++ = '#';     break;
                case '[':   *p++ = '<';     break;
                case ']':   *p++ = '>';     break;
                case '`':   *p++ = '\'';    break;
                case '^':   *p++ = '"';     break;

                case 'L':   p = stpCpy(p,lineBuf_,n,f);  break;
                case 'V':   p = stpCpy(p,drv,n,f);       break;
                case 'D':
                    if (autoWqFlg_) *p++ = '"';
                    p = stpCpy(p, dir_.c_str(), n, f);
                    if (autoWqFlg_) *p++ = '"';
                    *p = 0;
                    break;
                case 'X':
                    if (autoWqFlg_) *p++ = '"';
                    p = stpCpy(p,name_.c_str(),n,f);
                    if (autoWqFlg_) *p++ = '"';
                    *p = 0;
                    break;
                case 'E':
                    p = stpCpy(p,ext_.c_str(),n,f);
                    break;
                case 'W':
                    if (autoWqFlg_) *p++ = '"';
                    p = stpCpy(p, tmpDir_.c_str(), n, f);
                    if (autoWqFlg_) *p++ = '"';
                    *p = 0;
                    break;
                case 'P':
                    if (autoWqFlg_) *p++ = '"';
                    p = stpCpy(p,pathDir_.c_str(),n,f);
                    if (autoWqFlg_) *p++ = '"';
                    *p = 0;
                    break;

                case 'C':
                    b = buf;
                    if (autoWqFlg_) *b++ = '"';
                    b = stpCpy(b,name_.c_str(),0,f);
                    if (!ext_.empty()) {
                        b = STPCPY(b,".");
                        b = stpCpy(b,ext_.c_str(),0,f);
                    }
                    if (autoWqFlg_) *b++ = '"';
                    *b = 0;
                    if (n < 0) n = 1;
                    p += sprintf(p, "%-*s", n, buf);
                    break;
                case 'F':
                    b = buf;
                    if (autoWqFlg_) *b++ = '"';
                    b = stpCpy(b,drv_.c_str(),0,f);
                    b = stpCpy(b,dir_.c_str(),0,f);
                    b = STPCPY(b,"\\");
                    b = stpCpy(b,name_.c_str(),0,f);
                    if (!ext_.empty()) {
                        b = STPCPY(b,".");
                        b =stpCpy(b,ext_.c_str(),0,f);
                    }
                    if (autoWqFlg_) *b++ = '"';
                    *b = 0;
                    if (n < 0) n = 1;
                    p += sprintf(p, "%-*s", n, buf);
                    break;
                case 'G':
                    b = buf;
                    if (autoWqFlg_) *b++ = '"';
                    b = stpCpy(b,drv_.c_str(),0,f);
                    b = stpCpy(b,dir_.c_str(),0,f);
                    b = STPCPY(b,"\\");
                    b = stpCpy(b,name_.c_str(),0,f);
                    if (autoWqFlg_) *b++ = '"';
                    *b = '\0';
                    if (n < 0) n = 1;
                    p += sprintf(p, "%-*s", n, buf);
                    break;
                case 'O':
                    stpCpy(buf, tgtnm_.c_str(), 0, f);
                    if (n < 0) n = 1;
                    p += sprintf(p, "%-*s", n, buf);
                    break;
                case 'Z':
                    if (f) {
                        if (n < 0)
                            n = 10;
                        p += sprintf(p, "%*d", n, ff->size);
                    } else {
                        if (n < 0)
                            n = 8;
                        p += sprintf(p, "%*X", n, ff->size);
                    }
                    break;

                case 'I':
                    if (f) {
                        if (n < 0)
                            n = 1;
                        p += sprintf(p, "%0*d", n, unsigned(num_));
                    } else {
                        if (n < 0)
                            n = 1;
                        p += sprintf(p, "%0*X", n, unsigned(num_));
                    }
                    break;

                case 'J':
                    {   int y = 0, m = 0, d = 0;
                     #if defined _MSC_VER && _MSC_VER >= 1400
                        struct tm* ltm = _localtime64(&ff->time_write);
                     #else
                        struct tm* ltm = localtime(&ff->time_write);
                     #endif
                        if (ltm) {
                            y = ltm->tm_year + 1900;
                            m = ltm->tm_mon  + 1;
                            d = ltm->tm_mday;
                        }
                        if (n < 0)
                            n = 10;
                        if (n >= 10) {
                            sprintf(buf, "%04d-%02d-%02d", y, m, d);
                        } else if (n >= 8) {
                            sprintf(buf, "%02d-%02d-%02d", y %100, m, d);
                        } else {
                            sprintf(buf, "%02d-%02d", m, d);
                        }
                        p += sprintf(p, "%-*s", n, buf);
                    }
                    break;
                default:
                    if (c >= '1' && c <= '9') {
                        p = STPCPY(p, var_[c-'0'].c_str());
                    } else {
                        fprintf(stderr, "Incorrect '$' format : '$%c'\n",c);
                        /*fprintfE(stderr,".cfg 中 $指定がおかしい(%c)\n",c);*/
                        // exit(1);
                    }
                }
            }
        }
    }

private:
    bool                upLwrFlg_;
    bool                autoWqFlg_;     /* $f等で自動で両端に"を付加するモード. */
    size_t              num_;           /* $i で生成する番号 */
    size_t              numEnd_;        /* 連番をファイル名の文字列の代わりにする指定をした場合の終了アドレス */
    char const*         fmtBuf_;        /* 変換文字列を収める */
    char const*         lineBuf_;
    FnameBuf            var_[10];
    FnameBuf            drv_;
    FnameBuf            dir_;
    FnameBuf            name_;
    FnameBuf            ext_;
    FnameBuf            tmpDir_;
    FnameBuf            pathDir_;
    FnameBuf            chgPathDir_;
    FnameBuf            tgtnm_;
    FnameBuf            tgtnmFmt_;
    StrzBuf<OBUFSIZ>    obuf_;          /* .cfg(.res) 読み込みや、出力用のバッファ */

	StrList				outBuf_;
};



/*--------------------------------------------------------------------------*/
class Opts {
public:
    ConvFmt&        rConvFmt_;
    bool            recFlg_;                /* 再帰の有無 */
    bool            zenFlg_;                /* MS全角対応 */
    bool            batFlg_;                /* バッチ実行 */
    bool            batEx_;                 /* -bの有無 */
    bool            linInFlg_;              /* RES入力を行単位処理*/
    bool            autoWqFlg_;
    bool            upLwrFlg_;
    bool            sortRevFlg_;
    bool            sortUpLw_;
    int            noFindFile_;            /* ファイル検索しない */
    int             knjChk_;                /* MS全角存在チェック */
    unsigned        fattr_;                 /* ファイル属性 */
    SortType        sortType_;              /* ソート */
    size_t          topN_;                  /* 処理個数 */
    char *          iname_;                 /* 入力ファイル名 */
    char const*     dfltExtp_;              /* デフォルト拡張子 */
    size_t          szmin_;                 /* szmin > szmaxのとき比較を行わない*/
    size_t          szmax_;
    uint16_t		dtmin_;                 /* dtmin > dtmaxのとき比較を行わない*/
    uint16_t  		dtmax_;
 #ifdef ENABLE_MT_X
	unsigned		nthread_;
 #endif
    size_t          renbanStart_;           /* 連番の開始番号. 普通0 */
    size_t          renbanEnd_;             /* 連番の開始番号. 普通0 */
    FnameBuf        outname_;               /* 出力ファイル名 */
    FnameBuf        ipath_;                 /* 入力パス名 */
    FnameBuf        dfltExt_;               /* デフォルト拡張子 */
    FnameBuf        chgPathDir_;
    FnameBuf        exename_;

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
        , sortUpLw_(false)
        , noFindFile_(0)
        , knjChk_(0)
        , fattr_(0)
        , sortType_(ST_NONE)
        , topN_(0)
        //, iname_(&ipath_[0])
        , dfltExtp_(NULL)
        , szmin_(~size_t(0))
        , szmax_(0)
        , dtmin_(0xFFFFU)
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
        iname_ = &ipath_[0];
    }

    void setExename(char const* exename) {
        exename_ = exename;
     #ifdef _WIN32
        StrLwrN(&exename_[0], exename_.size());
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
            if (*p == 'd' || *p == 'D')	// -nd 現状機能しない
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
            FIL_FullPath(p, &ipath_[0]);
            p = STREND(&ipath_[0]);
            if (p[-1] != '\\' && p[-1] != '/') {
                *p++ = '\\';
                *p = '\0';
            }
            iname_ = p;
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
                case 'R': sortRevFlg_ = true;   break;
                }
                ++p;
            }
            break;

        case 'Z':
            szmin_ = (*p == '-') ? 0 : size_t( strtoull(p, &p, 0) );
            if (*p == 'K' || *p == 'k')         p++, szmin_ *= 1024UL;
            else if (*p == 'M' || *p == 'm')    p++, szmin_ *= 1024UL*1024UL;
            if (*p) { /* *p == '-' */
                szmax_ = 0xffffffffUL;
                p++;
                if (*p) {
                    szmax_ = size_t( strtoull(p,&p,0) );
                    if (*p == 'K' || *p == 'k')         p++, szmax_ *= 1024UL;
                    else if (*p == 'M' || *p == 'm')    p++, szmax_ *= 1024UL*1024UL;
                }
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
                unsigned long t;
                int y,m,d;
                if (*p == '-') {
                    dtmin_ = 0;
                } else {
                    t = strtoul(p,&p,10);
                    y = (int)((t/10000) % 100); y = (y >= 80) ? (y-80) : (y+100-80);
                    m = (int)((t / 100) % 100); if (m == 0 || 12 < m) goto ERR_OPTS;
                    d = (int)(t % 100);         if (d == 0 || 31 < d) goto ERR_OPTS;
                    dtmin_ = uint16_t((y<<9)|(m<<5)|d);
                }
                if (*p) {
                    p++;
                    dtmax_ = 0xFFFFU;
                    if (*p) {
                        t = strtoul(p,&p,10);
                        y = (int)(t/10000)%100; y = (y>=80) ? (y-80) : (y+100-80);
                        m = (int)(t/100) % 100; if (m==0 || 12 < m) goto ERR_OPTS;
                        d = (int)(t % 100);     if (d==0 || 31 < d) goto ERR_OPTS;
                        dtmax_ = uint16_t((y<<9)|(m<<5)|d);
                        if (dtmax_ < dtmin_)
                            goto ERR_OPTS;
                    }
                } else {
                    dtmax_ = dtmin_;
                }
            }
            break;

        case '?':
        case '\0':
            return usage();

        default:
      ERR_OPTS:
          #if 1
            err_printfE("コマンドラインでのオプション指定がおかしい : %s\n", s);
          #else
            err_printfE("Incorrect command line option : %s\n", s);
          #endif
        }
        return true;
    }


    bool usage() {
        printf(APP_HELP_TITLE APP_HELP_CMDLINE, exename_.c_str());
        printf("%s", APP_HELP_OPTS);
        return false;
    }
};


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
        size_t  l;

        if (name[0] == 0) {                     /* ファイル名がなければ標準入力 */
            l = fread(&resOBuf_[0], 1, resOBuf_.capacity(), stdin);
        } else {
            resName_ = name;
            FIL_AddExt(&resName_[0], "abx");
            FILE* fp = fopenE(resName_.c_str(), "rt");
            l = freadE(&resOBuf_[0], 1, resOBuf_.capacity(), fp);
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
        FIL_FullPath(name, &resName_[0]);
        FILE*   fp = fopenE(resName_.c_str(),"r");
        size_t  l  = freadE(&resOBuf_[0], 1, resOBuf_.capacity(), fp);
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
            } else {    /* 検索キーがなければ、一覧表示 */
                printf("\t%s\n",p);
            }
        }
        if (key[1])
            err_printfE("%s には %s は定義されていない\n", resName_.c_str(), key);
        exit(1);
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
            err_printfE(".cfg ファイルで $Ｎ 指定でおかしいものがある : $%s\n",p0);

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
            err_printfE(".cfg ファイルで $Ｎ=文字列指定 または $Ｎ:Ｍ{..}指定でおかしいものがある : $%s\n",p0);
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
        char    name[FIL_NMSZ];
        char*   p;
        char*   q;
        char*   d;
        enum Mode { MD_Body, MD_Bgn, MD_End, MD_TameBody };
        Mode    mode;

        d = fmtBuf_;
        mode = MD_Body;
        while ( (p = getLine()) != NULL ) {
            q = (char*)StrSkipSpc(p);
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
                            err_printfE("レスポンスファイル(定義ファイル中)の'変換文字列名'指定がおかしい\n");
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
                      #if 0
                        if (p[1]) {
                            d = STPCPY(d, p+1);
                            *d++ = '\n';
                            *d   = '\0';
                        }
                      #endif
                        mode = MD_TameBody;
                        goto NEXT_LINE;
                    case '-':               /* オプション文字列だ */
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
                    case '$':               /* $変数だ */
                        p = setDoll(p+1);
                        break;
                    default:
                        if (rOpts_.linInFlg_) { /* 行単位でファイル名を取得 */
                            rFileNameList_.push_back(p);
                            goto NEXT_LINE;
                        } else {            /* 空白区切りでファイル名を取得 */
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
        char    *k,*f;
        size_t  l;

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
                            err_printfE("%s のある検索行に{..}が10個以上ある %s\n", resName_.c_str(),lin);
                        }
                        rConvFmt_.setVar(varIdx_, f, l);
                        ++varIdx_;
                        k += l;
                        f = strchr(f,'}');
                        if (f == NULL) {
                  ERR1:
                            err_printfE("%s で{..}の指定がおかしい %s\n",resName_.c_str(), lin);
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
        return false;          /* マッチしなかった */
    }


private:
    Opts&               rOpts_;
    ConvFmt&            rConvFmt_;
    StrList&            rFileNameList_;
    StrList&            rBeforeStrList_;
    StrList&            rAfterStrList_;
    char*               fmtBuf_;
    char*               resP_;
    int                 varIdx_;
    int                 varNo_[10];
    FnameBuf            resName_;
    StrzBuf<OBUFSIZ>    resOBuf_;
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
    {
    }

    int main(int argc, char *argv[]) {
        opts_.setExename(FIL_BaseName(argv[0]));    /*アプリケーション名*/
        if (argc < 2) {
            opts_.usage();
            return 1;
        }

        abxName_ = argv[0];
        FIL_ChgExt(&abxName_[0], "cfg");

        if (scanOpts(argc, argv) == false)
            return 1;
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

    bool scanOpts(int argc, char *argv[]) {
        /* コマンドラインのオプション/ファイル名/変換文字列, 取得 */
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
                resCfgFile_.GetResFile(p+1);

            } else if (*p == '+') {
                ++p;
                if (*p == '\\' || *p == '/' || p[1] == ':') {
                    FIL_FullPath(p, &abxName_[0]);
                } else {
                    char fbuf[FIL_NMSZ];
                    strcpy(fbuf, argv[0]);
                    strcpy(FIL_BaseName(fbuf), p);
                    FIL_FullPath(fbuf, &abxName_[0]);
                }
                FIL_AddExt(&abxName_[0], "cfg");

            } else if (*p == ':') {
                if (p[1] == '#') {
                    err_printfE(":#で始まる文字列は指定できません（%s）\n",p);
                }
                if (resCfgFile_.GetCfgFile(&abxName_[0], p) == false)
                    return false;
            } else if (*p == '$') {
                if (p[1] >= '1' && p[1] <= '9' && p[2] == '=') {
                    unsigned    no  = p[1] - '0';
                    char const* s   = p + 3;
                    convFmt_.setVar(no, s, strlen(s));
                }
            } else {
                filenameList_.push_back(p);
            }
        }

	   #ifdef ENABLE_MT_X
		if (opts_.nthread_ && (!beforeStrList_.empty() || !afterStrList_.empty())) {
			err_printfE("-xm 指定と #begin,#end 指定は同時に指定できません\n");
		}
	   #endif

        /* バッチ実行のとき */
        if (opts_.batFlg_) {
		 #if 1
			TmpFile_make2(&tmpFName_[0], FIL_NMSZ, "abx_", ".bat");
printf("tmpfname=%s\n", &tmpFName_[0]);
			opts_.outname_  = tmpFName_;
		 #else
            opts_.outname_  = convFmt_.tmpDir();
            opts_.outname_ += "\\_abx_tmp.bat";
         #endif
        }

        if (opts_.fattr_ == 0) {     /* デフォルトのファイル検索属性 */
            opts_.fattr_ = FA_Norm|FA_RdOnly|FA_Hidden|FA_Sys|FA_Volume|FA_Arcive;  //0x127
        }
        convFmt_.setUpLwrFlag(opts_.upLwrFlg_);

        /* 変換文字列調整 */
        if (fmtBuf_.empty()) {
            if (opts_.recFlg_)
                fmtBuf_ = "$F\n";
            else
                fmtBuf_ = "$c\n";
        }
        if (strchr(fmtBuf_.c_str(), '\n') == NULL)
            fmtBuf_ += "\n";
        convFmt_.setFmtStr(fmtBuf_.c_str());

        convFmt_.setAutoWq(opts_.autoWqFlg_);       /* $f等で自動で両端に"を付加するモード. */

        return true;
    }


    bool genText() {
        if (opts_.batEx_) {                 /* バッチ実行用に先頭に echo off を置く */
            convFmt_.writeLine0("@echo off");
        }

        /* 直前出力テキスト */
        for (StrList::iterator ite = beforeStrList_.begin(); ite != beforeStrList_.end(); ++ite) {
            convFmt_.writeLine0(ite->c_str());
        }

        /* -u && -s ならば、指定ファイル名を小文字化 */
        if (opts_.upLwrFlg_ && opts_.sortType_) {
            for (StrList::iterator ite = filenameList_.begin(); ite != filenameList_.end(); ++ite) {
                StrLwrN(&(*ite)[0], ite->size());
            }
        }

        /* 実行 */
        FSrh    fsrh_(  opts_.fattr_, opts_.recFlg_, opts_.zenFlg_, opts_.topN_,
                        opts_.sortType_, opts_.sortRevFlg_, opts_.upLwrFlg_, opts_.knjChk_,
                        opts_.szmin_, opts_.szmax_, opts_.dtmin_, opts_.dtmax_, outFp_, &convFmt_, &ConvFmt::write
                    );
        if (opts_.renbanEnd_ == 0) {
            for (StrList::iterator ite = filenameList_.begin(); ite != filenameList_.end(); ++ite) {
                char const* p = ite->c_str();
                convFmt_.setLineBuf(p);
                if (*p != '\\' && *p != '/' && p[1] != ':') {   /* 相対パスのとき */
                    *opts_.iname_ = '\0';
                    opts_.ipath_ += p;
                    p = opts_.ipath_.c_str();
                } else {                                        /* フルパスのとき */
                }
                abxName_ = p;
                char const* s = STREND(p);
                if (*s == '/' || *s == '\\')
                    abxName_ += "*";
                FIL_AddExt(&abxName_[0], opts_.dfltExtp_);      /* デフォルト拡張子付加 */
                /* 実際のファイル名ごとの生成 */
                fsrh_.findAndDo(abxName_.c_str(), opts_.noFindFile_ != 0);
           }
        } else {
            opts_.noFindFile_ = 1;
            /* 連番生成での初期値設定 */
            for (size_t num = opts_.renbanStart_; num <= opts_.renbanEnd_; ++num) {
                convFmt_.setNum(num);
                sprintf(&abxName_[0], "%u", unsigned(num));
                convFmt_.setLineBuf(&abxName_[0]);
                /* 実際のファイル名ごとの生成 */
                fsrh_.findAndDo(abxName_.c_str(), opts_.noFindFile_ != 0);
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
            outFp_ = fopenE(opts_.outname_.c_str(), "wt");
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
				StrList& 					buf = convFmt_.outBuf();
				std::vector<std::string>	cmds(buf.size());
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
    FILE*           outFp_;
    StrList         filenameList_;
    StrList         beforeStrList_;
    StrList         afterStrList_;
    FnameBuf        abxName_;           /* 名前 work */
    ConvFmt         convFmt_;
    Opts            opts_;
    ResCfgFile      resCfgFile_;
    StrzBuf<FMTSIZ> fmtBuf_;            /* 変換文字列を収める */
	StrzBuf<FIL_NMSZ> tmpFName_;
};


/** start application
 */
int main(int argc, char *argv[]) {
    static App app;
    return app.main(argc, argv);
}
